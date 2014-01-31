

struct ntb_run_context;
struct ntb_run_config;

struct ntb_run_context * ntb_run_context_new(struct ntb_run_config * config);


void ntb_run_main_loop(struct ntb_network *nw,
              struct ntb_keyring *keyring,
              struct ntb_store *store);

void ntb_run_network(struct ntb_run_context * rc);

void ntb_run_context_free(struct ntb_run_context * rc);
