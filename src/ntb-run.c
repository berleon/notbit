#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>

#include "ntb-main-context.h"
#include "ntb-log.h"
#include "ntb-network.h"
#include "ntb-store.h"
#include "ntb-proto.h"
#include "ntb-file-error.h"
#include "ntb-keyring.h"
#include "ntb-ipc.h"
#include "ntb-run.h"
#include "ntb-util.h"

struct ntb_run_context {
        struct ntb_main_context * mc;
        struct ntb_run_config * config;
        struct ntb_error * error;
        struct ntb_network * network;
        struct ntb_keyring * keyring;
        struct ntb_store * store;
};

struct ntb_run_config {
        struct address *option_listen_addresses;
        struct address *option_peer_addresses;
        char *option_log_file;
        bool option_daemonize;
        char *option_user;
        char *option_group;
        char *option_store_directory;
        char *option_mail_dir;
        bool option_only_explicit_addresses;
};

struct address {
        /* Only one of these will be set depending on whether the user
         * specified a full address or just a port */
        const char *address;
        const char *port;

        struct address *next;
};

static bool
set_log_file(struct ntb_run_config * rc,
             struct ntb_store *store,
             struct ntb_error **error)
{
        struct ntb_buffer buffer;
        bool res;

        if (rc->option_log_file) {
                return ntb_log_set_file(rc->option_log_file, error);
        } else if (rc->option_daemonize) {
                ntb_buffer_init(&buffer);
                ntb_buffer_append_string(&buffer,
                                         ntb_store_get_directory(store));
                if (buffer.length > 0 && buffer.data[buffer.length - 1] != '/')
                        ntb_buffer_append_c(&buffer, '/');
                ntb_buffer_append_string(&buffer, "notbit.log");

                res = ntb_log_set_file((const char *) buffer.data, error);

                ntb_buffer_destroy(&buffer);

                return res;
        } else {
                return ntb_log_set_file("/dev/stdout", error);
        }
}

struct ntb_run_context *
ntb_run_context_new(struct ntb_run_config * config)
{
        struct ntb_run_context * rc;
        rc = ntb_alloc(sizeof (struct ntb_run_context));

        rc->config = config;

        rc->network = ntb_network_new();
        rc->store = ntb_store_new(rc->config->option_store_directory,
                                      rc->config->option_mail_dir,
                                      &rc->error);

        if (rc->store == NULL) {
                fprintf(stderr, "%s\n", rc->error->message);
                return NULL;
        } else {
                ntb_store_set_default(rc->store);

                if (!set_log_file(rc->config, rc->store, &rc->error)) {
                    return NULL;
                }

                rc->keyring = ntb_keyring_new(rc->network);
        }
        return rc;
}
void
ntb_run_context_free(struct ntb_run_context * rc)
{
        ntb_keyring_free(rc->keyring);
        ntb_network_free(rc->network);
        /* We need to free the store after freeing the network so that
         * if the network queues anything in the store just before it
         * is freed then we will be sure to complete the task before
         * exiting */
        if (rc->store)
                ntb_store_free(rc->store);

        ntb_free(rc);
        ntb_log_close();
}

static void
add_address(struct address **list,
            const char *address)
{
        struct address *listen_address;

        listen_address = ntb_alloc(sizeof (struct address));
        listen_address->address = address;
        listen_address->port = NULL;
        listen_address->next = *list;
        *list = listen_address;
}

static void
add_port(struct address **list,
         const char *port_string)
{
        struct address *listen_address;

        listen_address = ntb_alloc(sizeof (struct address));
        listen_address->address = NULL;
        listen_address->port = port_string;
        listen_address->next = *list;
        *list = listen_address;
}

static void
free_addresses(struct address *list)
{
        struct address *address, *next;

        for (address = list;
             address;
             address = next) {
                next = address->next;
                ntb_free(address);
        }
}



static void
set_user(const char *user_name)
{
        struct passwd *user_info;

        user_info = getpwnam(user_name);

        if (user_info == NULL) {
                fprintf(stderr, "Unknown user \"%s\"\n", user_name);
                exit(EXIT_FAILURE);
        }

        if (setuid(user_info->pw_uid) == -1) {
                fprintf(stderr, "Error setting user privileges: %s\n",
                        strerror(errno));
                exit(EXIT_FAILURE);
        }
}

static void
set_group(const char *group_name)
{
        struct group *group_info;

        group_info = getgrnam(group_name);

        if (group_info == NULL) {
                fprintf(stderr, "Unknown group \"%s\"\n", group_name);
                exit(EXIT_FAILURE);
        }

        if (setgid(group_info->gr_gid) == -1) {
                fprintf(stderr, "Error setting group privileges: %s\n",
                        strerror(errno));
                exit(EXIT_FAILURE);
        }
}

static void
quit_cb(struct ntb_main_context_source *source,
        void *user_data)
{
        bool *quit = user_data;
        *quit = true;
}

static bool
add_listen_address_to_network(struct ntb_network *nw,
                              struct address *address,
                              struct ntb_error **error)
{
        struct ntb_error *local_error = NULL;
        char *full_address;
        bool res;

        if (address->address)
                return ntb_network_add_listen_address(nw,
                                                      address->address,
                                                      error);

        /* If just the port is specified then we'll first try
         * listening on an IPv6 address. Listening on IPv6 should
         * accept IPv4 connections as well. However some servers have
         * IPv6 disabled so if it doesn't work we'll fall back to
         * IPv4 */
        full_address = ntb_strconcat("[::]:", address->port, NULL);
        res = ntb_network_add_listen_address(nw, full_address, &local_error);
        ntb_free(full_address);

        if (res)
                return true;

        if (local_error->domain == &ntb_file_error &&
            (local_error->code == NTB_FILE_ERROR_PFNOSUPPORT ||
             local_error->code == NTB_FILE_ERROR_AFNOSUPPORT)) {
                ntb_error_free(local_error);
        } else {
                ntb_error_propagate(error, local_error);
                return false;
        }

        full_address = ntb_strconcat("0.0.0.0:", address->port, NULL);
        res = ntb_network_add_listen_address(nw, full_address, error);
        ntb_free(full_address);

        return res;
}

static bool
add_addresses(struct ntb_run_config * rc,struct ntb_network *nw,
              struct ntb_error **error)
{
        struct address *address;

        for (address = rc->option_listen_addresses;
             address;
             address = address->next) {
                if (!add_listen_address_to_network(nw,
                                                   address,
                                                   error))
                        return false;
        }

        for (address = rc->option_peer_addresses;
             address;
             address = address->next) {
                if (!ntb_network_add_peer_address(nw,
                                                  address->address,
                                                  error))
                        return false;
        }

        if (rc->option_only_explicit_addresses)
                ntb_network_set_only_use_explicit_addresses(nw, true);

        return true;
}

struct ntb_run_config *
ntb_run_config_default()
{
        struct ntb_run_config * rconf =
            ntb_alloc(sizeof (struct ntb_run_config));
        rconf->option_listen_addresses = NULL;
        rconf->option_peer_addresses = NULL;
        rconf->option_log_file = NULL;
        rconf->option_daemonize = false;
        rconf->option_user = NULL;
        rconf->option_group = NULL;
        rconf->option_store_directory = NULL;
        rconf->option_mail_dir = NULL;
        rconf->option_only_explicit_addresses = false;
        return rconf;
}


void
ntb_run_network(struct ntb_run_context * rc)
{
        struct ntb_main_context_source *quit_source;
        bool quit = false;

        if (rc->config->option_group)
                set_group(rc->config->option_group);
        if (rc->config->option_user)
                set_user(rc->config->option_user);

        ntb_keyring_start(rc->keyring);
        ntb_store_start(rc->store);
        ntb_log_start();

        ntb_network_load_store(rc->network);
        ntb_keyring_load_store(rc->keyring);

        quit_source = ntb_main_context_add_quit(rc->mc, quit_cb, &quit);

        do
                ntb_main_context_poll(rc->mc);
        while(!quit);

        ntb_log("Exiting...");

        ntb_main_context_remove_source(quit_source);
}

