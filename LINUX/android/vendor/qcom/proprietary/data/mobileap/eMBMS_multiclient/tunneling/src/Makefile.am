AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat

AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DAEEINTERFACE_CPLUSPLUS \
        -DFEATURE_SMD \
        -DFEATURE_DIAG_MP_MASTER_APPS \
        -DFEATURE_DS_LINUX_ANDROID \
        -DFEATURE_DATA_LINUX_LE \
        -DFEATURE_DSS_LINUX \
        -DFEATURE_LINUX \
        -DFEATURE_MULTIPROCESSOR \
        -DFEATURE_DSUTILS_OFFTARGET \
        -DFEATURE_NO_REX \
        -DFEATURE_XMLLIB \
        -I$(top_srcdir)/dsutils/inc \
        -I../inc \
        -I$(top_srcdir)/netmgr/inc \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
        -I$(top_srcdir)/dsi_netctrl/inc \
        -I$(top_srcdir)/dsi_netctrl/src \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(XMLLIB_CFLAGS) \
        $(CFLAGS) \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat

requiredlibs = -lpthread -lrt \
        ../../../../netmgr/src/libnetmgr.la \
        ../../../../dsi_netctrl/src/libdsi_netctrl.la \
        ../../../../qdi/src/libqdi.la

library_includedir = $(pkgincludedir)

eMBMs_TunnelingModule_sources = eMBMs_TunnelingModule.cpp

eMBMs_TunnelingModule_h_sources = eMBMs_TunnelingModule.h embms_tm_api.h

bin_PROGRAMS = eMBMs_TunnelingModule

init_ddir = ${sysconfdir}/init.d
init_d_SCRIPTS = start_eMBMs_TunnelingModule_le

eMBMs_TunnelingModule_CC = @CC@
eMBMs_TunnelingModule_SOURCES = $(eMBMs_TunnelingModule_sources) $(eMBMs_TunnelingModule_h_sources)
if USE_GLIB
eMBMs_TunnelingModule_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
eMBMs_TunnelingModule_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(XMLLIB_LIBS) @GLIB_LIBS@
eMBMs_TunnelingModule_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
eMBMs_TunnelingModule_CFLAGS = $(AM_CFLAGS) -fPIC
eMBMs_TunnelingModule_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(XMLLIB_LIBS)
eMBMs_TunnelingModule_CPPFLAGS = $(AM_CPPFLAGS)
endif
eMBMs_TunnelingModule_LDADD = $(requiredlibs)
