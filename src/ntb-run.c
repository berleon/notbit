
#include <stdbool.h>

#include "ntb-keyring.h"
#include "ntb-store.h"

struct ntb_run_context {
        struct ntb_main_context * nm;
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

struct ntb_run_context *
ntb_run_context_new(struct ntb_run_config * config)
{
        struct ntb_run_context * rc;
        rc->config = config;
        rc = ntb_alloc(sizeof (struct ntb_run_context));

        rc->network = ntb_network_new();
        rc->store = ntb_store_new(rc->config->option_store_directory,
                                      rc->config->option_mail_dir,
                                      rc->error);

        rc->keyring = ntb_keyring_new(rc->network);
}
void
ntb_run_context_free(struct ntb_run_context * rc)
{
    ntb_store_free(rc->store);
    ntb_keyring_free(rc->keyring);
    ntb_network_free(rc->network);
    ntb_free(rc);
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
add_addresses(struct ntb_network *nw,
              struct ntb_error **error)
{
        struct address *address;

        for (address = option_listen_addresses;
             address;
             address = address->next) {
                if (!add_listen_address_to_network(nw,
                                                   address,
                                                   error))
                        return false;
        }

        for (address = option_peer_addresses;
             address;
             address = address->next) {
                if (!ntb_network_add_peer_address(nw,
                                                  address->address,
                                                  error))
                        return false;
        }

        if (option_only_explicit_addresses)
                ntb_network_set_only_use_explicit_addresses(nw, true);

        return true;
}

static bool
set_log_file(struct ntb_store *store,
             struct ntb_error **error)
{
        struct ntb_buffer buffer;
        bool res;

        if (option_log_file) {
                return ntb_log_set_file(option_log_file, error);
        } else if (option_daemonize) {
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


void
ntb_run_main_loop(struct ntb_run_context * rc)
{
        struct ntb_main_context_source *quit_source;
        bool quit = false;

        if (rc->config->option_group)
                set_group(rc->config->option_group);
        if (rc->config->option_user)
                set_user(rc->config->option_user);

        ntb_keyring_start(rc->keyring);
        ntb_store_start(rc->store);
        ntb_log_start(rc);

        ntb_network_load_store(rc->network);
        ntb_keyring_load_store(rc->keyring);

        quit_source = ntb_main_context_add_quit(NULL, quit_cb, &quit);

        do
                ntb_main_context_poll(NULL);
        while(!quit);

        ntb_log("Exiting...");

        ntb_main_context_remove_source(quit_source);
}

int
ntb_run_network(struct ntb_run_context * rc)
{
        struct ntb_store *store = NULL;
        struct ntb_network *nw;
        struct ntb_keyring *keyring;
        struct ntb_ipc *ipc;
        int ret = EXIT_SUCCESS;

        nw = ntb_network_new();

        if (!add_addresses(nw, rc->error)) {
                fprintf(stderr, "%s\n", rc->error->message);
                ntb_error_clear(&rc->error);
                ret = EXIT_FAILURE;
        } else {
                store = ntb_store_new(rc->config->option_store_directory,
                                      rc->config->option_mail_dir,
                                      rc->error);

                if (store == NULL) {
                        fprintf(stderr, "%s\n", rc->error->message);
                        ntb_error_clear(&rc->error);
                        ret = EXIT_FAILURE;
                } else {
                        ntb_store_set_default(store);

                        if (!set_log_file(store, &rc->error)) {
                                fprintf(stderr, "%s\n", rc->error->message);
                                ntb_error_clear(&rc->error);
                                ret = EXIT_FAILURE;
                        } else {
                                keyring = ntb_keyring_new(nw);
                                ipc = ntb_ipc_new(keyring, &rc->error);

                                if (ipc == NULL) {
                                        fprintf(stderr, "%s\n", rc->error->message);
                                        ntb_error_clear(&rc->error);
                                        ret = EXIT_FAILURE;
                                } else {
                                        run_main_loop(nw, keyring, store);
                                        ntb_ipc_free(ipc);
                                }

                                ntb_keyring_free(keyring);

                                ntb_log_close();
                        }
                }
        }

        ntb_network_free(nw);

        /* We need to free the store after freeing the network so that
         * if the network queues anything in the store just before it
         * is freed then we will be sure to complete the task before
         * exiting */
        if (store)
                ntb_store_free(store);

        return ret;
}
