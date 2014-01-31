
#include <stdbool.h>

struct ntb_config {
        struct address *option_listen_addresses;
        struct address *option_peer_addresses;
        char *option_log_file;
        bool option_daemonize;
        char *option_user;
        char *option_group;
        char *option_store_directory;
        bool option_only_explicit_addresses;
};
