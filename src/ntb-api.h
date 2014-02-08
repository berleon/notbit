#ifndef NTB_API_H
#define NTB_API_H

#include "ntb-main-context.h"
#include "ntb-run.h"


#ifdef __cplusplus
extern "C"{
#endif


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
                       const int leading_zeroes,
                       ntb_keyring_create_key_func func,
                       void *user_data);

bool
ntb_send_message(struct ntb_run_context * rc,
                 struct ntb_address *from_address,
                 struct ntb_address *to_addresses,
                 const size_t n_to_addresses,
                 int content_encoding,
                 const uint8_t * content,
                 size_t content_length);


#ifdef __cplusplus
}
#endif



#endif /* NTB_API_H */
