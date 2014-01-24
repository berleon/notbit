
TESTS = check_notbit
check_PROGRAMS = check_notbit
check_notbit_SOURCES = check_notbit.c  \
	$(top_builddir)/src/ntb-list.h  \
	$(top_builddir)/src/ntb-file-error.h \
	$(top_builddir)/src/ntb-pointer-array.h \
	$(top_builddir)/src/ntb-keyring.h \
	$(top_builddir)/src/ntb-load-keys.h \
	$(top_builddir)/src/ntb-base58.h \
	$(top_builddir)/src/ntb-log.h \
	$(top_builddir)/src/ntb-netaddress.h \
	$(top_builddir)/src/ntb-crypto.h \
	$(top_builddir)/src/ntb-hash-table.h \
	$(top_builddir)/src/ntb-pow.h \
	$(top_builddir)/src/ntb-slice.h \
	$(top_builddir)/src/ntb-connection.h \
	$(top_builddir)/src/ntb-error.h \
	$(top_builddir)/src/ntb-cpus.h \
	$(top_builddir)/src/ntb-address.h \
	$(top_builddir)/src/ntb-network.h \
	$(top_builddir)/src/ntb-socket.h \
	$(top_builddir)/src/ntb-blob.h \
	$(top_builddir)/src/ntb-key.h \
	$(top_builddir)/src/ntb-proto.h \
	$(top_builddir)/src/ntb-store.h \
	$(top_builddir)/src/ntb-ecc.h \
	$(top_builddir)/src/ntb-util.h \
	$(top_builddir)/src/ntb-slab.h \
	$(top_builddir)/src/ntb-signal.h \
	$(top_builddir)/src/ntb-ref-count.h \
	$(top_builddir)/src/ntb-main-context.h \
	$(top_builddir)/src/ntb-buffer.h \
	$(NULL)

check_notbit_CFLAGS = @CHECK_CFLAGS@
check_notbit_LDADD = $(top_builddir)/src/libnotbit.la @CHECK_LIBS@
