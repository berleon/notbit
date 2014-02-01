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




struct ntb_run_context *
ntb_init(struct ntb_run_config * config)
{
        if (config == NULL) {
            config = ntb_run_config_default();
        }
        return ntb_run_context_new(config);
}

void
ntb_destroy(struct ntb_run_context * rc)
{
        ntb_run_context_free(rc);
}
void
ntb_connect(struct ntb_run_context * rc)
{
    ntb_run_network(rc);
}

void
ntb_get_messages(struct ntb_run_context * rc)
{

}

