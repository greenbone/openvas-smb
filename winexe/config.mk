#################################
# Start BINARY winexe
[BINARY::winexe]
INSTALLDIR = BINDIR
OBJ_FILES = \
                winexe.o \
		service.o \
		async.o \
		winexesvc/winexesvc_exe.o
PRIVATE_DEPENDENCIES = \
                POPT_SAMBA \
                POPT_CREDENTIALS \
                LIBPOPT \
		RPC_NDR_SVCCTL
# End BINARY winexe
#################################
#################################
# Start Library libwincmd
[LIBRARY::wincmd]
VERSION=0.80
SO_VERSION=1
LIBRARY_REALNAME = libwincmd.$(SHLIBEXT)
OBJ_FILES = \
                wincmd.o \
                service.o \
                async.o \
                winexesvc/winexesvc_exe.o
PRIVATE_DEPENDENCIES = \
                POPT_SAMBA \
                POPT_CREDENTIALS \
                LIBPOPT \
                RPC_NDR_SVCCTL
# End BINARY wincmd
#################################

winexe/winexesvc/winexesvc_exe.c: winexe/winexesvc/winexesvc.c
	@$(MAKE) -C winexe/winexesvc
