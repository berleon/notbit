
#ifndef NTB_CONFIG_H
#define NTB_CONFIG_H


struct ntb_config {
        struct address *option_listen_addresses;
        struct address *option_peer_addresses;
        char *option_log_file;
        bool option_daemonize;
        char *option_user;
        char *option_group;
        char *option_store_directory;
        char *option_mail_dir;
        bool option_only_explicit_addresses;
};

#endif /* NTB_CONFIG_H */
