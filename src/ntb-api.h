#ifndef NTB_API_H
#define NTB_API_H

#include "ntb-main-context.h"
#include "ntb-run.h"


#ifdef __cplusplus
extern "C"{
#endif


struct ntb_run_context *
ntb_init(struct ntb_run_config * config);

void ntb_destroy(struct ntb_run_context * rc);

void ntb_connect(struct ntb_run_context * rc);

void ntb_get_messages(struct ntb_run_context * rc);

void ntb_on_message();

#ifdef __cplusplus
}
#endif



#endif /* NTB_API_H */
