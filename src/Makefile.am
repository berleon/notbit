bin_PROGRAMS = \
	notbit \
	$(NULL)

lib_LTLIBRARIES = libnotbit.la

noinst_PROGRAMS = dump-store

AM_CPPFLAGS = \
	$(LIBCRYPTO_CFLAGS) \
	$(NOTBIT_EXTRA_CFLAGS) \
	$(NULL)

libnotbit_la_SOURCES = \
	ntb-address.c \
	ntb-address.h \
	ntb-api.c \
	ntb-api.h \
	ntb-base58.c \
	ntb-base58.h \
	ntb-base64.c \
	ntb-base64.h \
	ntb-blob.c \
	ntb-blob.h \
	ntb-buffer.c \
	ntb-buffer.h \
	ntb-connection.c \
	ntb-connection.h \
	ntb-cpus.c \
	ntb-cpus.h \
	ntb-crypto.c \
	ntb-crypto.h \
	ntb-daemon.c \
	ntb-daemon.h \
	ntb-dns-bootstrap.c \
	ntb-dns-bootstrap.h \
	ntb-ecc.c \
	ntb-ecc.h \
	ntb-encoded-words.c \
	ntb-encoded-words.h \
	ntb-error.c \
	ntb-error.h \
	ntb-file-error.c \
	ntb-file-error.h \
	ntb-file.c \
	ntb-file.h \
	ntb-hash-table.c \
	ntb-hash-table.h \
	ntb-ipc.c \
	ntb-ipc.h \
	ntb-ipc-client.c \
	ntb-ipc-client.h \
	ntb-ipc-proto.c \
	ntb-ipc-proto.h \
	ntb-ipc-sockaddr.c \
	ntb-ipc-sockaddr.h \
	ntb-key.c \
	ntb-key.h \
	ntb-key-value.c \
	ntb-key-value.h \
	ntb-keygen.c \
	ntb-keygen.h \
	ntb-keyring.c \
	ntb-keyring.h \
	ntb-list.c \
	ntb-list.h \
	ntb-load-keys.c \
	ntb-load-keys.h \
	ntb-load-outgoings.c \
	ntb-load-outgoings.h \
	ntb-log.c \
	ntb-log.h \
	ntb-mail-parser.c \
	ntb-mail-parser.h \
	ntb-main-context.c \
	ntb-main-context.h \
	ntb-mkdir.c \
	ntb-mkdir.h \
	ntb-netaddress.c \
	ntb-netaddress.h \
	ntb-network.c \
	ntb-network.h \
	ntb-parse-addresses.c \
	ntb-parse-addresses.h \
	ntb-parse-content-type.c \
	ntb-parse-content-type.h \
	ntb-pointer-array.h \
	ntb-pow.c \
	ntb-pow.h \
	ntb-proto.c \
	ntb-proto.h \
	ntb-quoted-printable.c \
	ntb-quoted-printable.h \
	ntb-ref-count.h \
	ntb-save-message.c \
	ntb-save-message.h \
	ntb-run.c \
	ntb-run.h \
	ntb-sendmail.c \
	ntb-sendmail.h \
	ntb-signal.h \
	ntb-signal.c \
	ntb-slab.c \
	ntb-slab.h \
	ntb-slice.c \
	ntb-slice.h \
	ntb-socket.c \
	ntb-socket.h \
	ntb-store.c \
	ntb-store.h \
	ntb-util.c \
	ntb-util.h \
	$(NULL)

notbit_SOURCES = main.c

notbit_LDADD = libnotbit.la

libnotbit_la_LDFLAGS = \
	$(LIBCRYPTO_LIBS) \
	$(NOTBIT_EXTRA_LIBS) \
	-pthread \
	$(NULL)

dump_store_SOURCES = \
	dump-store.c \
	$(NULL)

dump_store_LDFLAGS = \
	-pthread \
	libnotbit.la \
	$(NULL)

NOTBIT_LINKS = \
	notbit-sendmail$(EXEEXT) \
	notbit-keygen$(EXEEXT) \
	$(NULL)

notbit-sendmail$(EXEEXT): notbit
	$(AM_V_GEN)$(RM) $@ && $(LN_S) $< $@

notbit-keygen$(EXEEXT): notbit
	$(AM_V_GEN)$(RM) $@ && $(LN_S) $< $@

install-exec-hook:
	for x in $(NOTBIT_LINKS); do \
	  $(RM) $(DESTDIR)$(bindir)/$$x; \
	  $(LN_S) notbit $(DESTDIR)$(bindir)/$$x || exit 1; \
	done

all-local: $(NOTBIT_LINKS)

CLEANFILES = \
	$(NOTBIT_LINKS) \
	$(NULL)
