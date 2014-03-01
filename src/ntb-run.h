#ifndef NTB_RUN_H
#define NTB_RUN_H

struct ntb_run_context {
        struct ntb_main_context * mc;
        struct ntb_run_config * config;
        struct ntb_error * error;
        struct ntb_network * network;
        struct ntb_keyring * keyring;
        struct ntb_store * store;
};

struct ntb_run_config;

struct ntb_run_context *
ntb_run_context_new(struct ntb_run_config * config);

struct ntb_run_config *
ntb_run_config_default();


void ntb_run_main_loop(struct ntb_run_context);

void ntb_run_network(struct ntb_run_context * rc);

void
ntb_run_config_free(struct ntb_run_config *conf);

void ntb_run_context_free(struct ntb_run_context * rc);

#endif
