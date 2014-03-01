#ifndef NTB_API_H
#define NTB_API_H

#include "ntb-main-context.h"
#include "ntb-run.h"
#include "ntb-keyring.h"


struct ntb_run_context *
ntb_init(struct ntb_run_config * config);

void
ntb_destroy(struct ntb_run_context * rc);

void
ntb_connect(struct ntb_run_context * rc);

void
ntb_disconnect(struct ntb_run_context *rc);

void
ntb_create_key(struct ntb_run_context * rc,
               const char *label,
               int leading_zeroes,
               uint64_t version,
               ntb_keyring_create_key_func callback,
               void *user_data);

bool
ntb_send_message(struct ntb_run_context * rc,
                 struct ntb_address *from_address,
                 struct ntb_address *to_addresses,
                 const size_t n_to_addresses,
                 int content_encoding,
                 const uint8_t * content,
                 size_t content_length);

#endif /* NTB_API_H */
