
void
ntb_run_main_loop(struct ntb_network *nw,
              struct ntb_keyring *keyring,
              struct ntb_store *store)
{
        struct ntb_main_context_source *quit_source;
        bool quit = false;

        if (option_group)
                set_group(option_group);
        if (option_user)
                set_user(option_user);

        if (option_daemonize)
                daemonize();

        ntb_keyring_start(keyring);
        ntb_store_start(store);
        ntb_log_start();

        ntb_network_load_store(nw);
        ntb_keyring_load_store(keyring);

        quit_source = ntb_main_context_add_quit(NULL, quit_cb, &quit);

        do
                ntb_main_context_poll(NULL);
        while(!quit);

        ntb_log("Exiting...");

        ntb_main_context_remove_source(quit_source);
}

int
ntb_run_network(struct ntb_api_context * ac)
{
        struct ntb_store *store = NULL;
        struct ntb_network *nw;
        struct ntb_keyring *keyring;
        struct ntb_ipc *ipc;
        int ret = EXIT_SUCCESS;

        nw = ntb_network_new();

        if (!add_addresses(nw, &error)) {
                fprintf(stderr, "%s\n", error->message);
                ntb_error_clear(&error);
                ret = EXIT_FAILURE;
        } else {
                store = ntb_store_new(ac->config->option_store_directory,
                                      ac->config->option_mail_dir,
                                      ac->error);

                if (store == NULL) {
                        fprintf(stderr, "%s\n", error->message);
                        ntb_error_clear(&error);
                        ret = EXIT_FAILURE;
                } else {
                        ntb_store_set_default(store);

                        if (!set_log_file(store, &error)) {
                                fprintf(stderr, "%s\n", error->message);
                                ntb_error_clear(&error);
                                ret = EXIT_FAILURE;
                        } else {
                                keyring = ntb_keyring_new(nw);
                                ipc = ntb_ipc_new(keyring, &error);

                                if (ipc == NULL) {
                                        fprintf(stderr, "%s\n", error->message);
                                        ntb_error_clear(&error);
                                        ret = EXIT_FAILURE;
                                } else {
                                        run_main_loop(nw, keyring, store);
                                        ntb_ipc_free(ipc);
                                }

                                ntb_keyring_free(keyring);

                                ntb_log_close();
                        }
                }
        }

        ntb_network_free(nw);

        /* We need to free the store after freeing the network so that
         * if the network queues anything in the store just before it
         * is freed then we will be sure to complete the task before
         * exiting */
        if (store)
                ntb_store_free(store);

        return ret;
}

