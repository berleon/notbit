

struct ntb_run_context;
struct ntb_run_config;

void ntb_run_main_loop(struct ntb_network *nw,
              struct ntb_keyring *keyring,
              struct ntb_store *store);

int ntb_run_network(struct ntb_api_context * ac);

