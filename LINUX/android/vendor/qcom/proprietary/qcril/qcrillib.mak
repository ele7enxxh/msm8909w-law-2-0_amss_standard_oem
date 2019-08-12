# -*- Mode: text -*-
#==============================================================================
# FILE: qcrillib.mak
#
# SERVICE: RIL
#
# DESCRIPTION:
#
#==============================================================================

default: all

RELPATHTOROOT = ../../../..

TARGET = libril-qc-qmi-1
MAKEFILETYPE = shared
MAKEFILENAME = qcrillib.mak

C_SRC =\
   qcril_qmi/qcril.c\
   qcril_qmi/qcril_arb.c\
   qcril_qmi/qcril_cm_ss.c\
   qcril_qmi/qcril_cm_util.c\
   qcril_qmi/qcril_event.c\
   qcril_qmi/qcril_log.c\
   qcril_qmi/qcril_other.c\
   qcril_qmi/qcril_pbm.c\
   qcril_qmi/qcril_qmi_client.c\
   qcril_qmi/qcril_qmi_ims.c\
   qcril_qmi/qcril_qmi_imss.c\
   qcril_qmi/qcril_qmi_nas.c\
   qcril_qmi/qcril_qmi_nas2.c\
   qcril_qmi/qcril_qmi_sms.c\
   qcril_qmi/qcril_qmi_sms_errors.c\
   qcril_qmi/qcril_qmi_voice.c\
   qcril_qmi/qcril_reqlist.c\
   qcril_qmi/qcril_qmi_imsa.c\
   qcril_qmi/services/qmi_embms_v01.c\
   qcril_qmi/services/qtuner_v01.c\
   qcril_qmi/qcril_qmi_coex.c\
   qcril_qmi/ims_socket/imsIF.pb-c.c\
   qcril_qmi/ims_socket/qcril_qmi_ims_if_pb.c\
   qcril_qmi/ims_socket/qcril_qmi_ims_misc.c\
   qcril_qmi/ims_socket/qcril_qmi_ims_packing.c\
   common/uim/qcril_gstk_qmi.c\
   common/uim/qcril_uim.c\
   common/uim/qcril_uim_card.c\
   common/uim/qcril_uim_file.c\
   common/uim/qcril_uim_security.c\
   common/uim/qcril_uim_util.c\
   common/uim/qcril_uim_qcci.c\
   common/uim/qcril_uim_queue.c\
   common/uim/qcril_uim_refresh.c\
   common/uim/qcril_uim_restart.c\
   common/uim/qcril_uim_slot_mapping.c\
   common/uim/qcril_uim_lpa.c\   
   common/uim/qcril_uim_http.c\
   common/uim/qcril_scws.c\
   common/uim/qcril_scws_opt.c\
   common/data/qcril_data_netctrl.c\
   common/data/qcril_data_req_resp.c\
   common/data/qcril_data_qos.c\
   common/data/qcril_data_qmi_wds.c\
   common/data/qcril_data_utils.c\
   common/data/qcril_data_embms.c\
   common/data/qcril_data_lqe.c\
   common/data/qcril_data_client.c

C++_SRC=\
   qcril_qmi/ims_socket/qcril_qmi_ims_socket.cc\
   qcril_qmi/oem_socket/qcril_qmi_oem_socket.cc\
   qcril_qmi/qcril_qmi_generic_socket.cc\
   qcril_qmi/qcril_qmi_pil_monitor.cc\

NEEDINC=\
   frameworks/native/include\
   hardware/ril/include\
   hardware/ril/include/telephony \
   hardware/libhardware_legacy/include\
   $(QCPATH)/qmi/src\
   $(QCPATH)/common/inc/\
   $(QCPATH)/diag/include\
   $(QCPATH)\
   $(QCPATH)/qmi/inc/\
   $(QCPATH)/qmi/platform/\
   $(QCPATH)/qmi/services\
   $(QCPATH)/qmi/qmi_client_helper\
   $(QCPATH)/qmi/core/lib/inc\
   $(QCPATH)/data/dsi_netctrl/inc\
   $(QCPATH)/data/dsutils/inc\
   $(QCPATH)/data/qdp/inc\
   $(QCPATH)/qcril/qcril_qmi\
   $(QCPATH)/qcril/qcril_qmi/ims_socket\
   $(QCPATH)/qcril/qcril_qmi/oem_socket\
   $(QCPATH)/qcril/common/uim\
   $(QCPATH)/qcril/common/data\
   $(QCPATH)/qcril/qcrilhook_oem\
   $(QCPATH)/qcril\
   system/core/include\
   system/core/include/cutils\
   external/protobuf-c/src\

## Need this to ensure we pickup qmi/services dir instead of qcril_qmi/services dir 
NEEDINC +=\
   $(QCPATH)/qcril/qcril_qmi/services\
   $(QCPATH)/qmi/\


NEEDLIB =\
   $(QCPATH)/data/dsutils/src/libdsutils\
   $(QCPATH)/qmi/libqmi\
   $(QCPATH)/qmi/services/libqmiservices\
   $(QCPATH)/qmi/core/lib/src/libqmiidl\
   $(QCPATH)/diag/src/libdiag\
   $(QCPATH)/data/qdp/src/libqdp\
   $(QCPATH)/data/lqe/src/liblqe\
   $(QCPATH)/data/dsi_netctrl/src/libdsi_netctrl\
   $(QCPATH)/qcril/qcrilhook_oem/libril-qcril-hook-oem\
   external/protobuf-c/src/libprotobuf\

## Note: need strl functions for off-target, they are not present in GNU libs
NEEDLIB +=\
   $(QCPATH)/test/ril/strllib/libstrl\

LIBS +=\
   -lpthread\
   -lrt

CFLAGS += -m32
CFLAGS += -DRIL_SHLIB
CFLAGS += -DQCRIL_DATA_OFFTARGET
CFLAGS += -DFEATURE_TARGET_GLIBC_x86

## Define for UIM files using QMI
CFLAGS += -DFEATURE_QCRIL_UIM_QMI
CFLAGS += -DFEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
CFLAGS += -DFEATURE_QCRIL_QMI_CAT
CFLAGS += -DFEATURE_DATA_EMBMS
CFLAGS += -DFEATURE_DATA_LQE
CFLAGS += -DFEATURE_QCRIL_USE_NETCTRL
CFLAGS += -DFEATURE_QCRIL_8960## use sys_info msgs instead of serving system

## Turn on UIM ISIM REFRESH FEATURE
CFLAGS += -DFEATURE_QCRIL_UIM_ISIM_REFRESH

## Obsolete defines?
#
#CFLAGS += -DANDROID
## for asprinf
#CFLAGS += -D_GNU_SOURCE
## defines necessary for QCRIL code
#CFLAGS += -DFEATURE_MMGSDI_GSM
#CFLAGS += -DFEATURE_AUTH
#CFLAGS += -DPACKED=
#CFLAGS += -DFEATURE_QCRIL_UUS
#CFLAGS += -DFEATURE_QCRIL_HDR_RELB
#CFLAGS += -DFEATURE_QCRIL_NCELL
#
## defines common to all shared libraries
#CFLAGS += \
#  -DLOG_NDDEBUG=0 \
#  -D__packed__= \
#  -DIMAGE_APPS_PROC \
#  -DFEATURE_Q_SINGLE_LINK \
#  -DFEATURE_Q_NO_SELF_QPTR \
#  -DFEATURE_NATIVELINUX \
#  -DFEATURE_DSM_DUP_ITEMS \
#
## compiler options
#CFLAGS += -fno-inline
#CFLAGS += -fno-short-enums

LDFLAGS += -m32

BTBUILD=$(QCPATH)/test/ril/btbuild
include $(RELPATHTOROOT)/$(BTBUILD)/bt.driver.mki

#======================================================================
# Insert manual overrides here:
#======================================================================
