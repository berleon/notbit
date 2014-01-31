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
#include "ntb-config.h"

struct ntb_api_context {
    struct ntb_main_context * nm;
    struct ntb_config * config;
    struct ntb_error * error;
};

struct ntb_main_context *
ntb_init()
{
        struct ntb_main_context *mc;
        struct ntb_error *error = NULL;

        mc = ntb_main_context_get_default(&error);

        if (mc == NULL) {
                fprintf(stderr, "%s\n", error->message);
                return NULL;
        }
        return mc;
}
void
ntb_connect(struct ntb_main_context * mc)
{
        int ret = EXIT_SUCCESS;
        ret = run_network();

        ntb_main_context_free(mc);

        free_addresses(option_peer_addresses);
        free_addresses(option_listen_addresses);

}

void
ntb_get_message(struct ntb_main_context * mc) {

}

