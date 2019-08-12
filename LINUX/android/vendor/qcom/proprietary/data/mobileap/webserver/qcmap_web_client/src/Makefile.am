AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat
AM_CXXFLAGS = \
        $(AM_CFLAGS) \
        -fpermissive

AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DAEEINTERFACE_CPLUSPLUS \
        -DFEATURE_SMD \
        -DFEATURE_DIAG_MP_MASTER_APPS \
        -DFEATURE_DS_LINUX_ANDROID \
        -DFEATURE_DSS_LINUX \
        -DFEATURE_LINUX \
        -DFEATURE_MULTIPROCESSOR \
        -DFEATURE_DSUTILS_OFFTARGET \
        -DFEATURE_NO_REX \
        -DFEATURE_XMLLIB \
        -I$(top_srcdir)/dsutils/inc \
        -I$(top_srcdir)/netmgr/inc \
        -I$(top_srcdir)/mobileap/client/inc \
        -I$(top_srcdir)/mobileap/ipc \
        -I../inc \
        -I$(WORKSPACE)/cxm-apps \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(XMLLIB_CFLAGS) \
        $(CFLAGS) \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat

requiredlibs = -lpthread -lrt $(QMI_LIBS) \
               ../../../client/src/libqcmap_client.la \
               ../../../ipc/libqcmapipc.la

#####################QCMAP_Web_CLIENT##################

QCMAP_Web_CLIENT_sources = QCMAP_Web_CLIENT.cpp QCMAP_Web_CLIENT_Utils.cpp

QCMAP_Web_CLIENT_h_sources = QCMAP_Web_CLIENT.h

bin_PROGRAMS = QCMAP_Web_CLIENT

# init_ddir = ${sysconfdir}/init.d
# init_d_SCRIPTS = start_QCMAP_Web_CLIENT_le

QCMAP_Web_CLIENT_CC = @CC@
QCMAP_Web_CLIENT_SOURCES = $(QCMAP_Web_CLIENT_sources) $(QCMAP_Web_CLIENT_h_sources)
if USE_GLIB
QCMAP_Web_CLIENT_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
QCMAP_Web_CLIENT_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(XMLLIB_LIBS) @GLIB_LIBS@
QCMAP_Web_CLIENT_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
QCMAP_Web_CLIENT_CFLAGS = $(AM_CFLAGS) -fPIC
QCMAP_Web_CLIENT_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(XMLLIB_LIBS)
QCMAP_Web_CLIENT_CPPFLAGS = $(AM_CPPFLAGS)
endif
QCMAP_Web_CLIENT_LDADD = $(requiredlibs)

