
AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic

AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_DATA_LINUX_LE \
        -DFEATURE_XMLLIB \
        -DFEATURE_DSS_LINUX \
        -DFEATURE_LINUX \
        -DFEATURE_DIAG_MP_MASTER_APPS \
        -DFEATURE_MULTIPROCESSOR \
        -DFEATURE_DSUTILS_OFFTARGET \
        -I$(top_srcdir)/dsutils/inc \
        -I$(top_srcdir)/mobileap/ipc \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
        $(DIAG_CFLAGS) \
        -I$(top_srcdir)/netmgr/inc \
        $(DSUTILS_CFLAGS) \
        $(XMLLIB_CFLAGS) \
        $(CFLAGS) \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat

requiredlibs = $(top_srcdir)/netmgr/src/libnetmgr.la \
               $(top_srcdir)/mobileap/ipc/libqmi_ip.la

#####################QMI_IP##################

qmi_ip_c_sources = qmi_ip_main.c \
                   qmi_ip_conn_mgmt.c \
                   qmi_ip_msg_hdlr.c \
                   qmi_ip_tun_mgmt.c \
                   qmi_ip_xml.c \
                   qmi_ip_fpop_hdlr.c

qmi_ip_h_sources = qmi_ip.h

bin_PROGRAMS = qmi_ip_multiclient
qmi_ip_multiclient_CC = @CC@
qmi_ip_multiclient_SOURCES = $(qmi_ip_c_sources) $(qmi_ip_h_sources)
if USE_GLIB
qmi_ip_multiclient_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(DIAG_CFLAGS) @GLIB_CFLAGS@
qmi_ip_multiclient_LDFLAGS = $(XMLLIB_LIBS) $(DSUTILS_LIBS) -lpthread -lqmi_encdec -lqmiservices -lqmi_csi -lqmi_cci @GLIB_LIBS@ -shared -version-info 1:0:0
else
qmi_ip_multiclient_CFLAGS = $(AM_CFLAGS) $(DIAG_CFLAGS) -fPIC
qmi_ip_multiclient_LDFLAGS = $(XMLLIB_LIBS) $(DSUTILS_LIBS) -lpthread -lqmi_encdec -lqmiservices -lqmi_csi -lqmi_cci -shared -version-info 1:0:0
endif
qmi_ip_multiclient_CPPFLAGS = $(AM_CPPFLAGS)
qmi_ip_multiclient_LDADD = $(requiredlibs)

etcdir = ${sysconfdir}
etc_SCRIPTS = qmi_ip_cfg.xml

init_ddir = ${sysconfdir}/init.d
init_d_SCRIPTS = start_stop_qmi_ip_multiclient


