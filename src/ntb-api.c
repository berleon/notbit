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


#include "ntb-api.h"
#include "ntb-main-context.h"
#include "ntb-log.h"
#include "ntb-network.h"
#include "ntb-store.h"
#include "ntb-proto.h"
#include "ntb-file-error.h"
#include "ntb-keyring.h"
#include "ntb-run.h"
#include "ntb-blob.h"





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
ntb_create_key(struct ntb_run_context * rc,
               const char *label,
               const int leading_zeroes,
               ntb_keyring_create_key_func callback,
               void *user_data)
{
        ntb_keyring_create_key(rc->keyring,
                               label,
                               leading_zeroes,
                               callback, user_data);
}



bool
ntb_send_message(struct ntb_run_context *rc,
                 struct ntb_address *from_address,
                 struct ntb_address *to_addresses,
                 const size_t n_to_addresses,
                 int content_encoding,
                 const uint8_t * content,
                 size_t content_length)
{
    struct ntb_blob * content_blob = ntb_blob_new(NTB_PROTO_INV_TYPE_MSG,
                                                  content, content_length);

    return ntb_keyring_send_message(rc->keyring,
                             from_address,
                             to_addresses,
                             n_to_addresses,
                             content_encoding,
                             content_blob,
                             rc->error);
}

