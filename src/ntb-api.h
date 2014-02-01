#ifndef NTB_API_H
#define NTB_API_H

#include "ntb-main-context.h"
#include "ntb-run.h"

struct ntb_run_context * ntb_init();

void ntb_connect(struct ntb_run_context * rc);

void ntb_get_messages(struct ntb_run_context * rc);

void ntb_on_message();


#endif /* NTB_API_H */
