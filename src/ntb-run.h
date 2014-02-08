

struct ntb_run_context;
struct ntb_run_config;

struct ntb_run_context * ntb_run_context_new(struct ntb_run_config * config);

struct ntb_run_config * ntb_run_config_default();


void ntb_run_main_loop(struct ntb_run_context);

void ntb_run_network(struct ntb_run_context * rc);

void
ntb_run_config_free(struct ntb_run_config *conf);

void ntb_run_context_free(struct ntb_run_context * rc);
