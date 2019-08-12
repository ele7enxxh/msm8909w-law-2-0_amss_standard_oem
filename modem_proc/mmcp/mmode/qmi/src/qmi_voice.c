/*===========================================================================

                         D S _ Q M I _ VOICE . C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

EXTERNALIZED FUNCTIONS

  qmi_voice_init()
    Register the VOICE service with QMUX for all applicable QMI links
    

Copyright (c) 2009-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_voice.c#1 $ $DateTime: 2010/04/22 22:49:27 $ $voiceor: nlocanin $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/25/09    vs     Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_QMI_SERVICE_SPLIT

#include "qmi_voice.h"

#include "dsm.h"
#include "nv.h"
#include "err.h"
#include "amssassert.h"
#include "qmi_voice_cm_if.h"
#include "qmi_voice_cmd_list.h"
#include "modem_mem.h"
#include "qmi_mmode_svc.h"
#include "qmi_mmode_task_cmd.h"

#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "voice_service_v02.h"
#include "voice_service_impl_v02.h"
#include "qm_timer.h"
#include "qm_util.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

#ifndef FEATURE_NO_QMI_VOICE
/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define VOICEI_MAX_CLIDS    (10)

/*---------------------------------------------------------------------------
  Major and Minor Version Nos for VOICE
---------------------------------------------------------------------------*/
#define VOICEI_BASE_VER_MAJOR    (2)
#define VOICEI_BASE_VER_MINOR    (1)

#define VOICEI_ADDENDUM_VER_MAJOR  (0)
#define VOICEI_ADDENDUM_VER_MINOR  (0)

/*---------------------------------------------------------------------------
  Optional TLVs
---------------------------------------------------------------------------*/
#define VOICEI_IND_REG_DTMF             (0x10)
#define VOICEI_IND_REG_VOICE_PRIV       (0x11)
#define VOICEI_IND_REG_SUPS_NOTIFY      (0x12)
#define VOICEI_IND_REG_CALL_EVENTS_NOTIFY      (0x13)
#define VOICEI_IND_REG_HANDOVER_NOTIFY  (0x14)
#define VOICEI_IND_REG_SPEECH_NOTIFY        (0x15)
#define VOICEI_IND_REG_USSD_NOTIFY          (0x16)
#define VOICEI_IND_REG_SUPS_OTHER_NOTIFY    (0x17)
#define VOICEI_IND_REG_MODIFICATION_NOTIFY  (0x18)
#define VOICEI_IND_REG_UUS_NOTIFY           (0x19)
#define VOICEI_IND_REG_AOC_NOTIFY                         (0x1A)
#define VOICEI_IND_REG_CONF_NOTIFY           (0x1B)
#define VOICEI_IND_REG_EXT_BRST_INTL_NOTIFY     (0x1C)
#define VOICEI_IND_REG_MT_PAGE_MISS_NOTIFY  (0x1D)
#define VOICEI_IND_REG_CC_RESULT_NOTIFY     (0x1E)
#define VOICEI_IND_REG_CONF_PARTS_NOTIFY     (0x1F)
#define VOICEI_IND_REG_TTY_INFO_NOTIFY     (0x20)
#define VOICEI_IND_REG_E911_ORIG_FAIL_NOTIFY   (0x21)
#define VOICEI_IND_REG_AUDIO_RAT_CHANGE_NOTIFY (0x23)
#define VOICEI_IND_REG_ADD_CALL_INFO_NOTIFY     (0x24)
#define VOICEI_IND_REG_VICE_DIALOG_INFO_NOTIFY (0x27)

#define VOICEI_DIAL_CALL_TYPE   (0x10)
#define VOICEI_DIAL_CLIR_TYPE       (0x11)
#define VOICEI_DIAL_UUS_INFO        (0x12)
#define VOICEI_DIAL_CUG             (0x13)
#define VOICEI_DIAL_CALL_ID         (0x10)
#define VOICEI_DIAL_ALPHA_ID        (0x11)
#define VOICEI_DIAL_EMER_CAT        (0x14)
#define VOICEI_DIAL_CALLED_PARTY_SUB_ADDRRESS      (0x15)
#define VOICEI_DIAL_SRV_TYPE        (0x16)
#define VOICEI_DIAL_SIP_URI_OVERFLOW    (0x17)
#define VOICEI_DIAL_AUDIO_ATTRIB        (0x18)
#define VOICEI_DIAL_VIDEO_ATTRIB        (0x19)
#define VOICEI_DIAL_IP_CALL_PI          (0x1A)
#define VOICEI_DIAL_VS_CALL_ATTRIB      (0x1B)
#define VOICEI_DIAL_ECALL_VARIANT       (0x1C)
#define VOICEI_DIAL_CONFERENCE_INIT_URI      (0x1D)
#define VOICEI_DIAL_DISPLAY_TEXT        (0x1E)
#define VOICEI_DIAL_ECALL_MSD           (0x1F)
#define VOICEI_DIAL_CALL_PULL           (0x20)



#define VOICEI_DIAL_CC_RESULT_TYPE   (0x12)
#define VOICEI_DIAL_CC_RESULT_SUPS   (0x13)
#define VOICEI_DIAL_RESP_END_REASON  (0x14)
#define VOICEI_DIAL_RESP_MEDIA_ID    (0x15)

#define VOICEI_BURST_DTMF_LEN       (0x10)
#define VOICEI_BURST_DTMF_CALL_ID   (0x10)

#define VOICEI_END_CALL_ID          (0x10)

#define VOICEI_ANSWER_CALL_ID       (0x10)

#define VOICEI_ANSWER_CALL_TYPE        (0x10)
#define VOICEI_ANSWER_AUDIO_ATTRIB     (0x11)
#define VOICEI_ANSWER_VIDEO_ATTRIB     (0x12)
#define VOICEI_ANSWER_IP_CALL_PI       (0x13)
#define VOICEI_ANSWER_VS_FILE_ATTRIB   (0x14)
#define VOICEI_ANSWER_REJECT_CALL      (0x15)
#define VOICEI_ANSWER_REJECT_CAUSE     (0x16)
#define VOICEI_ANSWER_REJECT_SIP_CODE  (0x17)

#define VOICEI_SETUP_ANSWER_RESP_CALL_ID     (0x10)

#define VOICEI_SETUP_ANSWER_REJECT_CALL      (0x10)
#define VOICEI_SETUP_ANSWER_REJECT_CAUSE     (0x11)

#define VOICEI_GET_CALL_INFO        (0x10)
#define VOICEI_GET_CALL_RP_NUM      (0x11)
#define VOICEI_GET_CALL_SO          (0x12)
#define VOICEI_GET_CALL_VP          (0x13)
#define VOICEI_GET_CALL_OTSP        (0x14)
#define VOICEI_GET_CALL_RP_NAME     (0x15)
#define VOICEI_GET_CALL_UUS_INFO    (0x16)
#define VOICEI_GET_CALL_ALERTING    (0x17)
#define VOICEI_GET_CALL_ALPHA       (0x18)
#define VOICEI_GET_CALL_CONN_NUM_INFO       (0x19)
#define VOICEI_GET_CALL_DIAGNOSTICS       (0x1A)
#define VOICEI_GET_CALL_ALERTING_PATTERN  (0x1B)
#define VOICEI_GET_CALL_AUDIO_ATTRIB       (0x1C)
#define VOICEI_GET_CALL_VIDEO_ATTRIB       (0x1D)
#define VOICEI_GET_CALL_VS_VARIANT         (0x1E)
#define VOICEI_GET_CALL_SIP_URI            (0x1F)
#define VOICEI_GET_CALL_IS_SRVCC_CALL      (0x20)
#define VOICEI_GET_CALL_RP_NUM_EXT         (0x21)
#define VOICEI_GET_CALL_SECOND_ALPHA       (0x22)
#define VOICEI_GET_CALL_END_REASON_TEXT    (0x24)

#define VOICEI_SEND_FLASH_CALL_ID   (0x10)

#define VOICEI_START_CONT_DTMF_CALL_ID   (0x10)

#define VOICEI_STOP_CONT_DTMF_CALL_ID    (0x10)

#define VOICEI_DTMF_IND_PW              (0x10)
#define VOICEI_DTMF_IND_DIG_INT         (0x11)
#define VOICEI_DTMF_IND_IP_VOLUME       (0x12)

#define VOICEI_ALL_CALL_IND_RP_NUM                          (0x10)
#define VOICEI_ALL_CALL_IND_RP_NAME                         (0x11)
#define VOICEI_ALL_CALL_IND_ALERTING                        (0x12)
#define VOICEI_ALL_CALL_IND_SO                              (0x13)
#define VOICEI_ALL_CALL_IND_END_REASON                      (0x14)
#define VOICEI_ALL_CALL_IND_ALPHA                           (0x15)
#define VOICEI_ALL_CALL_IND_CONNECTED_NUM                   (0x16)
#define VOICEI_ALL_CALL_IND_DIAGNOSTICS                     (0x17)
#define VOICEI_ALL_CALL_IND_CALLED_PARTY_NUM                (0x18)
#define VOICEI_ALL_CALL_IND_REDIRECTING_PARTY_NUM           (0x19)
#define VOICEI_ALL_CALL_IND_ALERTING_PATTERN                (0x1A)
#define VOICEI_ALL_CALL_IND_AUDIO_ATTRIB                    (0x1B)
#define VOICEI_ALL_CALL_IND_VIDEO_ATTRIB                    (0x1C)
#define VOICEI_ALL_CALL_IND_VS_VARIANT                      (0x1D)
#define VOICEI_ALL_CALL_IND_SIP_URI                         (0x1E)
#define VOICEI_ALL_CALL_IND_IS_SRVCC_CALL                   (0x1F)
#define VOICEI_ALL_CALL_IND_PARENT_CALL_INFO                (0x20)
#define VOICEI_ALL_CALL_IND_LOCAL_CAP                       (0x21)
#define VOICEI_ALL_CALL_IND_PEER_CAP                        (0x22)
#define VOICEI_ALL_CALL_IND_CHILD_NUM                       (0x23)
#define VOICEI_ALL_CALL_IND_DISPLAY_TEXT                    (0x24)
#define VOICEI_ALL_CALL_IND_RP_NUM_EXT                      (0x25)
#define VOICEI_ALL_CALL_IND_CONNECTED_NUM_EXT               (0x26)
#define VOICEI_ALL_CALL_IND_MEDIA_ID                        (0x27)
#define VOICEI_ALL_CALL_IND_IS_ADDITIONAL_CALL_INFO_PRESENT (0x28)
#define VOICEI_ALL_CALL_IND_CALL_ATTRIB_STATUS              (0x29)
#define VOICEI_ALL_CALL_IND_ORIG_FAIL_REASON                (0x2A)
#define VOICEI_ALL_CALL_IND_RP_NUM_EXT2                     (0x2B)
#define VOICEI_ALL_CALL_IND_SECOND_ALPHA                    (0x2C)
#define VOICEI_ALL_CALL_IND_END_REASON_TEXT                 (0x2E)
#define VOICEI_ALL_CALL_IND_IP_CALLER_NAME                  (0x2D)
#define VOICEI_ALL_CALL_IND_CALLER_NAME_PI                  (0x2F)
#define VOICEI_ALL_CALL_IND_IS_CONNECTED_NUMBER_ECT         (0x31)
#define VOICEI_ALL_CALL_IND_IS_SECURE_CALL                  (0x32)
#define VOICEI_ALL_CALL_IND_IS_CALLED_PARTY_RINGING         (0x34)
#define VOICEI_ALL_CALL_IND_RAW_CALL_END_CAUSE_CODE         (0x35)

#define VOICEI_GET_ALL_CALL_INFO                  (0x10)
#define VOICEI_GET_ALL_CALL_RP_NUM                (0x11)
#define VOICEI_GET_ALL_CALL_RP_NAME               (0x12)
#define VOICEI_GET_ALL_CALL_ALERTING              (0x13)
#define VOICEI_GET_ALL_CALL_UUS_INFO              (0x14)
#define VOICEI_GET_ALL_CALL_SO                    (0x15)
#define VOICEI_GET_ALL_CALL_OTASP                 (0x16)
#define VOICEI_GET_ALL_CALL_VP                    (0x17)
#define VOICEI_GET_ALL_CALL_END_REASON            (0x18)
#define VOICEI_GET_ALL_CALL_ALPHA                 (0x19)
#define VOICEI_GET_ALL_CALL_CONNECTED_NUM         (0x1A)
#define VOICEI_GET_ALL_CALL_DIAGNOSTICS           (0x1B)
#define VOICEI_GET_ALL_CALL_CALLED_PARTY_NUM      (0x1C)
#define VOICEI_GET_ALL_CALL_REDIRECTING_PARTY_NUM (0x1D)
#define VOICEI_GET_ALL_CALL_ALERTING_PATTERN      (0x1E)
#define VOICEI_GET_ALL_CALL_AUDIO_ATTRIBUTE       (0x1F)
#define VOICEI_GET_ALL_CALL_VIDEO_ATTRIBUTE       (0x20)
#define VOICEI_GET_ALL_CALL_VS_VARIANT            (0x21)
#define VOICEI_GET_ALL_CALL_SIP_URI               (0x22)
#define VOICEI_GET_ALL_CALL_IS_SRVCC_CALL         (0x23)
#define VOICEI_GET_ALL_CALL_ATTRIB_STATUS         (0x24)
#define VOICEI_GET_ALL_CALL_RP_NUM_EXT            (0x25)
#define VOICEI_GET_ALL_CALL_SECOND_ALPHA          (0x26)
#define VOICEI_GET_ALL_CALL_END_REASON_TEXT       (0x28)
#define VOICEI_GET_ALL_CALL_RAW_CALL_END_CAUSE_CODE (0x2B)

#define VOICEI_MNG_CALLS_REQ_CALL_ID        (0x10)
#define VOICEI_MNG_CALLS_REQ_REJ_CAUSE        (0x11)

#define VOICEI_MNG_CALLS_FAILURE_CAUSE  (0x10)

#define VOICEI_SUPS_NOTIFY_IND_CUG                   (0x10)
#define VOICEI_SUPS_NOTIFY_IND_ECT                   (0x11)
#define VOICEI_SUPS_NOTIFY_IND_REASON                (0x12)
#define VOICEI_SUPS_NOTIFY_IND_IP_FORWARD_HIST_INFO  (0x13)

#define VOICEI_SET_SUPS_SC                   (0x10)
#define VOICEI_SET_SUPS_BARR_PWD             (0x11)
#define VOICEI_SET_SUPS_FWD_NUM              (0x12)
#define VOICEI_SET_SUPS_NR_TIMER             (0x13)
#define VOICEI_SET_SUPS_NUM_TYPE_PLAN        (0x14)
#define VOICEI_SET_SUPS_SC_EXT               (0x15)
#define VOICEI_SET_SUPS_BARR_NUM_LIST        (0x16)
#define VOICEI_SET_SUPS_COLR_PI              (0x17)
#define VOICEI_SET_SUPS_CALL_FWD_START_TIME  (0x18)
#define VOICEI_SET_SUPS_CALL_FWD_END_TIME    (0x19)

#define VOICEI_SET_SUPS_FAILURE_CAUSE     (0X10)
#define VOICEI_SET_SUPS_ALPHA_ID          (0X11)
#define VOICEI_SET_SUPS_CC_RESULT_TYPE    (0x12)
#define VOICEI_SET_SUPS_CC_RESULT_CALL_ID (0X13)
#define VOICEI_SET_SUPS_CC_RESULT_SUPS    (0X14)
#define VOICEI_SET_SUPS_SERVICE_STATUS    (0X15)
#define VOICEI_SET_SUPS_FAILURE_DESC      (0X16)
#define VOICEI_SET_SUPS_RETRY_DURATION    (0x17)

#define VOICEI_GET_CW_SC                (0x10)
#define VOICEI_GET_CW_REQ_SC_EXT        (0x11)

#define VOICEI_GET_CW_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_CW_ALPHA_ID          (0X12)
#define VOICEI_GET_CW_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_CW_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_CW_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_CW_RESP_SC_EXT       (0x16)
#define VOICEI_GET_CW_RETRY_DURATION    (0x17)

#define VOICEI_GET_CB_SC                (0x10)
#define VOICEI_GET_CB_REQ_SC_EXT             (0X11)

#define VOICEI_GET_CB_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_CB_ALPHA_ID          (0X12)
#define VOICEI_GET_CB_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_CB_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_CB_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_CB_RESP_SC_EXT       (0X16)
#define VOICEI_GET_CB_RESP_BARR_LIST    (0X17)
#define VOICEI_GET_CB_RETRY_DURATION    (0x18)

#define VOICEI_GET_CLIP_RESP              (0x10)
#define VOICEI_GET_CLIP_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_CLIP_ALPHA_ID          (0X12)
#define VOICEI_GET_CLIP_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_CLIP_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_CLIP_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_CLIP_RETRY_DURATION    (0x16)

#define VOICEI_GET_COLP_RESP              (0x10)
#define VOICEI_GET_COLP_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_COLP_ALPHA_ID          (0X12)
#define VOICEI_GET_COLP_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_COLP_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_COLP_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_COLP_RETRY_DURATION    (0X16)

#define VOICEI_GET_CLIR_RESP              (0x10)
#define VOICEI_GET_CLIR_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_CLIR_ALPHA_ID          (0X12)
#define VOICEI_GET_CLIR_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_CLIR_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_CLIR_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_CLIR_RETRY_DURATION    (0x16)

#define VOICEI_GET_CF_SC                   (0x10) //Optional Request TLV
#define VOICEI_GET_CF_REQ_SC_EXT               (0x11) 

#define VOICEI_GET_CF_INFO                (0x10)
#define VOICEI_GET_CF_FAILURE_CAUSE       (0X11)
#define VOICEI_GET_CF_ALPHA_ID            (0X12)
#define VOICEI_GET_CF_CC_RESULT_TYPE      (0x13)
#define VOICEI_GET_CF_CC_RESULT_CALL_ID   (0X14)
#define VOICEI_GET_CF_CC_RESULT_SUPS      (0X15)
#define VOICEI_GET_CF_EXTEN_INFO          (0X16)
#define VOICEI_GET_CF_EXTEN2_INFO         (0X17)
#define VOICEI_GET_CF_RETRY_DURATION      (0X18)
#define VOICEI_GET_CF_PROVISION_STATUS    (0X19)
#define VOICEI_GET_CF_CALL_FWD_START_TIME (0x1A)
#define VOICEI_GET_CF_CALL_FWD_END_TIME   (0x1B)

#define VOICEI_GET_ALS_LINE_SWITCH_STATUS   (0x10)

#define VOICEI_GET_ALS_SELECTED_LINE        (0x10)

#define VOICEI_SET_CB_PWD_FAILURE_CAUSE     (0X10)
#define VOICEI_SET_CB_PWD_ALPHA_ID          (0X11)
#define VOICEI_SET_CB_PWD_CC_RESULT_TYPE    (0x12)
#define VOICEI_SET_CB_PWD_CC_RESULT_CALL_ID (0X13)
#define VOICEI_SET_CB_PWD_CC_RESULT_SUPS    (0X14)
#define VOICEI_SET_CB_PWD_RETRY_DURATION    (0x15)

#define VOICEI_ORIG_USSD_FAILURE_CAUSE     (0X10)
#define VOICEI_ORIG_USSD_ALPHA_ID          (0X11)
#define VOICEI_ORIG_USSD_DATA              (0X12)
#define VOICEI_ORIG_USSD_CC_RESULT_TYPE    (0x13)
#define VOICEI_ORIG_USSD_CC_RESULT_CALL_ID  (0X14)
#define VOICEI_ORIG_USSD_CC_RESULT_SUPS     (0X15)
#define VOICEI_ORIG_USSD_DATA_UTF16         (0x16) 

#define VOICEI_USSD_IND_DATA               (0x10)
#define VOICEI_USSD_IND_DATA_UTF16         (0x11)

#define VOICEI_FLASH_PAYLOAD    (0x10)
#define VOICEI_FLASH_TYPE       (0x11)

#define VOICEI_ORIG_USSD_NO_WAIT_ERROR          (0x10)
#define VOICEI_ORIG_USSD_NO_WAIT_FAILURE_CAUSE  (0x11)
#define VOICEI_ORIG_USSD_NO_WAIT_DATA           (0x12)
#define VOICEI_ORIG_USSD_NO_WAIT_ALPHA_ID       (0x13)
#define VOICEI_ORIG_USSD_NO_WAIT_DATA_UTF16     (0x14)

#define VOICEI_MODEM_SET_CONFIG_AUTO_ANSWER          (0x10)
#define VOICEI_MODEM_SET_CONFIG_AIR_TIMER            (0x11)
#define VOICEI_MODEM_SET_CONFIG_ROAM_TIMER           (0x12)
#define VOICEI_MODEM_SET_CONFIG_TTY_MODE             (0x13)
#define VOICEI_MODEM_SET_CONFIG_PREF_VOICE_SO        (0x14)
#define VOICEI_MODEM_SET_CONFIG_PREF_VOICE_DOMAIN    (0x15)
#define VOICEI_MODEM_SET_CONFIG_UI_TTY_SETTING       (0x16)
#define VOICEI_MODEM_SET_CONFIG_ECALL_MSD            (0x17)


#define VOICEI_MODEM_GET_CONFIG_AUTO_ANSWER          (0x10)
#define VOICEI_MODEM_GET_CONFIG_AIR_TIMER            (0x11)
#define VOICEI_MODEM_GET_CONFIG_ROAM_TIMER           (0x12)
#define VOICEI_MODEM_GET_CONFIG_TTY_MODE             (0x13)
#define VOICEI_MODEM_GET_CONFIG_PREF_VOICE_SO        (0x14)
#define VOICEI_MODEM_GET_CONFIG_AMR_STATUS           (0x15)
#define VOICEI_MODEM_GET_CONFIG_VOICE_PRIVACY        (0x16)
#define VOICEI_MODEM_GET_CONFIG_NAM_ID               (0x17)
#define VOICEI_MODEM_GET_CONFIG_PREF_VOICE_DOMAIN    (0x18)
#define VOICEI_MODEM_GET_CONFIG_UI_TTY_SETTING       (0x19)

#define VOICEI_MODEM_GET_CONFIG_RESP_PREF_VOICE_DOMAIN    (0x17)
#define VOICEI_MODEM_GET_CONFIG_RESP_UI_TTY_SETTING       (0x18)

#define VOICEI_BIND_SUBSCRIPTION             (0x10)

#define VOICEI_MNG_IP_CALLS_CALL_ID        (0x10)
#define VOICEI_MNG_IP_CALLS_CALL_TYPE      (0x11)
#define VOICEI_MNG_IP_CALLS_AUDIO_ATTRIB   (0x12)
#define VOICEI_MNG_IP_CALLS_VIDEO_ATTRIB   (0x13)
#define VOICEI_MNG_IP_CALLS_SIP_URI        (0x14)
#define VOICEI_MNG_IP_CALLS_SIP_REJECT_CAUSE       (0x15)
#define VOICEI_MNG_IP_CALLS_SIP_REJECT_CODE    (0x16)
#define VOICEI_MNG_IP_CALLS_SPEECH_CODEC      (0x17)
#define VOICEI_MNG_IP_CALLS_ECT_TYPE      (0x18)
#define VOICEI_MNG_IP_CALLS_TRANSFER_TARGET_CALL_ID      (0x19)

#define VOICEI_MNG_IP_CALLS_RESP_FAILURE_CAUSE        (0x11)


#define VOICEI_GET_COLR_RESP              (0x10)
#define VOICEI_GET_COLR_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_COLR_ALPHA_ID          (0X12)
#define VOICEI_GET_COLR_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_COLR_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_COLR_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_COLR_PI                (0x16)
#define VOICEI_GET_COLR_RETRY_DURATION    (0x17)

#define VOICEI_MODIFIED_CALL_ID            (0x01)
#define VOICEI_MODIFIED_CALL_TYPE          (0x10)
#define VOICEI_MODIFIED_CALL_AUDIO_ATTRIB  (0x11)
#define VOICEI_MODIFIED_CALL_VIDEO_ATTRIB  (0x12)
#define VOICEI_MODIFIED_CALL_FAILURE_CAUSE (0x13)
#define VOICEI_MODIFIED_CALL_MEDIA_ID      (0x14)
#define VOICEI_MODIFIED_CALL_ATTRIB_STATUS (0x15)

#define VOICEI_MODIFY_ACCEPT_CALL_ID   (0x01)
#define VOICEI_MODIFY_ACCEPT_CALL_TYPE   (0x10)
#define VOICEI_MODIFY_ACCEPT_AUDIO_ATTRIB   (0x11)
#define VOICEI_MODIFY_ACCEPT_VIDEO_ATTRIB   (0x12)


#define VOICEI_GET_CNAP_RESP              (0x10)
#define VOICEI_GET_CNAP_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_CNAP_ALPHA_ID          (0X12)
#define VOICEI_GET_CNAP_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_CNAP_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_CNAP_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_CNAP_RETRY_DURATION    (0x16)

#define VOICEI_AOC_GET_CALL_METER_INFO_ACM                (0x10)
#define VOICEI_AOC_GET_CALL_METER_INFO_ACMMAX             (0x11)
#define VOICEI_AOC_GET_CALL_METER_INFO_CCM                (0x12)

#define VOICEI_SRVCC_CALL_CONFIG_REQ_ALERTING_TYPE    (0x10)
#define VOICEI_SRVCC_CALL_CONFIG_REQ_CALL_NUM_PI      (0x11)
#define VOICEI_SRVCC_CALL_CONFIG_REQ_CALL_NAME        (0x12)




/*---------------------------------------------------------------------------
  Response TLVs
---------------------------------------------------------------------------*/

#define VOICEI_INFO_REC_CALL_ID  (0x01)
#define VOICEI_CALL_STATUS       (0x01)
#define VOICEI_DTMF_INFORMATION  (0x01)
#define VOICEI_SIGNAL_INFO       (0x10)
#define VOICEI_CALLER_ID         (0x11)
#define VOICEI_DISPLAY_INFO      (0x12)
#define VOICEI_EXT_DISPLAY_INFO  (0x13)
#define VOICEI_CALLER_NAME_INFO  (0x14)
#define VOICEI_CALL_WAITING_IND  (0x15)
#define VOICEI_CONN_NUM_INFO     (0x16)
#define VOICEI_CALLING_PARTY_NUM_INFO (0x17)
#define VOICEI_CALLED_PARTY_NUM_INFO  (0x18)
#define VOICEI_REDIRECT_NUM_INFO  (0x19)
#define VOICEI_NSS_CLIR_INFO  (0x1A)
#define VOICEI_NSS_AUD_CTRL_INFO  (0x1B)
#define VOICEI_NSS_RELEASE_INFO  (0x1C)
#define VOICEI_LINE_CTRL_INFO  (0x1D)
#define VOICEI_EXT_DISPLAY_INFO_RECORD  (0x1E)


#define VOICEI_OTASP_STATUS      (0x01)

#define VOICEI_PRIVACY_INFO                 (0x01)
#define VOICEI_CALL_END_REASON              (0x10)
#define VOICEI_CALL_TYPE                    (0x11)
#define VOICEI_ALL_CALL_STATUS              (0x01)
#define VOICEI_SUPS_NOTIFY_INFO             (0x01)
#define VOICEI_USSD_NOTIFY_TYPE          (0x01)
#define VOICEI_UUS_IND_DATA               (0x01)

#define VOICEI_SUPS_IND_SERVICE_INFO                 (0x01)
#define VOICEI_SUPS_IND_SERVICE_CLASS               (0x10)
#define VOICEI_SUPS_IND_REASON                            (0x11)
#define VOICEI_SUPS_IND_CFW_NUM                        (0x12)
#define VOICEI_SUPS_IND_CFW_NR_TIMER               (0x13)
#define VOICEI_SUPS_IND_USS_INFO                        (0x14)
#define VOICEI_SUPS_IND_CALL_ID                           (0x15)
#define VOICEI_SUPS_IND_ALPHA                              (0x16)
#define VOICEI_SUPS_IND_PWD                                 (0x17)
#define VOICEI_SUPS_IND_NEW_PWD                        (0x18)
#define VOICEI_SUPS_IND_SUPS_DATA_SOURCE                (0x19)
#define VOICEI_SUPS_IND_FAILURE_CAUSE                   (0x1A)
#define VOICEI_SUPS_IND_CF_NTWK_DATA                    (0x1B)
#define VOICEI_SUPS_IND_CLIR_STATUS                     (0x1C)
#define VOICEI_SUPS_IND_CLIP_STATUS                     (0x1D)
#define VOICEI_SUPS_IND_COLP_STATUS                     (0x1E)
#define VOICEI_SUPS_IND_COLR_STATUS                     (0x1F)
#define VOICEI_SUPS_IND_CNAP_STATUS                     (0x20)
#define VOICEI_SUPS_IND_USS_INFO_UTF16                  (0x21)
#define VOICEI_SUPS_IND_SERVICE_CLASS_EXT               (0x22)
#define VOICEI_SUPS_IND_BARR_LIST                       (0x23)

#define VOICEI_SPEECH_CODEC_INFO_NW_MODE               (0x010)
#define VOICEI_SPEECH_CODEC_INFO_CODEC_TYPE            (0x011)
#define VOICEI_SPEECH_CODEC_INFO_SAMPLE_RATE           (0x012)
#define VOICEI_SPEECH_CODEC_INFO_CALL_ID               (0x013)

#define VOICEI_HANDOVER_STATE                         (0x01)
#define VOICEI_HANDOVER_TYPE                          (0x10)

#define VOICEI_EXT_BRST_INTL_DATA                         (0x01)

#define VOICEI_CONFERENCE_CONF_INFO_XML_DATA          (0x01)
#define VOICEI_CONFERENCE_CONF_INFO_SEQUENCE_NUM          (0x02)
#define VOICEI_CONFERENCE_CONF_INFO_TOTAL_XML_FILE_LENGTH          (0x10)
#define VOICEI_CONFERENCE_CONF_INFO_CALL_ID          (0x11)

#define VOICEI_CONFERENCE_JOIN_CALL_ID                    (0x01)
#define VOICEI_CONFERENCE_JOIN_PARTICIPANT_INFO           (0x02)

#define VOICEI_CONFERENCE_PARTICIPANT_UPDATE_INFO       (0x01)

#define VOICEI_CONFERENCE_PARTICIPANTS_INFO             (0x01)

#define VOICEI_PAGE_MISS_REASON                         (0x01)

#define VOICEI_CC_RESULT_INFO_RESULT                    (0x01)
#define VOICEI_CC_RESULT_INFO_ALPHA_PRESENCE            (0x02)
#define VOICEI_CC_RESULT_INFO_ALPHA_GSM8                (0x10)
#define VOICEI_CC_RESULT_INFO_ALPHA_UTF16               (0x11)

#define VOICEI_TTY_MODE             (0x01)

#define VOICEI_AUDIO_RAT_CHANGE_AUDIO_SESSION_INFO   (0x10)
#define VOICEI_AUDIO_RAT_CHANGE_RAT_INFO             (0x11)

#define VOICEI_CONF_PART_STATUS_INFO_CALL_ID             (0x01)
#define VOICEI_CONF_PART_STATUS_INFO_PART_ID             (0x02)
#define VOICEI_CONF_PART_STATUS_INFO_PART_OP_INFO             (0x10)


#define VOICEI_E911_ORIG_FAIL       (0x01)

#define VOICEI_ADDITIONAL_CALL_INFO_CALL_ID       (0x01)
#define VOICEI_ADDITIONAL_CALL_INFO_EXT_INFO_DATA          (0x10)


/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/
#define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }

#define VOICEI_TLV_TAG_SIZE     1  /* in bytes */
#define VOICEI_TLV_LENGTH_SIZE  2  /* in bytes */

#define QMI_VOICEI_PULL(sdu_in, value, siz) (siz == dsm_pullup( sdu_in, value, siz ))

/*===========================================================================

                                DATA TYPES

===========================================================================*/


/*---------------------------------------------------------------------------
  VOICE Command enum type - not equal to the actual command values!
  mapping is in qmi_voice_cmd_callbacks table

  DO NOT REORDER THIS ENUM!
---------------------------------------------------------------------------*/
typedef enum
{
  VOICEI_CMD_MIN                      = 0,
  VOICEI_CMD_INDICATION_REGISTER      = VOICEI_CMD_MIN,
  VOICEI_CMD_DIAL_CALL                ,
  VOICEI_CMD_END_CALL                 ,
  VOICEI_CMD_ANSWER_CALL              ,
  VOICEI_CMD_SETUP_ANSWER             ,
  VOICEI_CMD_GET_CALL_INFO            ,
  VOICEI_CMD_SEND_FLASH               ,
  VOICEI_CMD_BURST_DTMF               ,
  VOICEI_CMD_START_CONT_DTMF          ,
  VOICEI_CMD_STOP_CONT_DTMF           ,
  VOICEI_CMD_SET_PREFERRED_PRIVACY    ,
  VOICEI_CMD_SUPS_MNG_CALLS,
  VOICEI_CMD_SUPS_GET_CLIP,
  VOICEI_CMD_SUPS_GET_COLP,
  VOICEI_CMD_SUPS_GET_CALL_WAITING,
  VOICEI_CMD_SUPS_GET_CALL_BARRING,
  VOICEI_CMD_SUPS_SET_CALL_BARRING,
  VOICEI_CMD_GET_CLIR,
  VOICEI_CMD_GET_CALL_FORWARDING,
  VOICEI_CMD_GET_ALL_CALL_INFO ,
  VOICEI_CMD_SUPS_SET_SUPS_SERVICE,
  VOICEI_CMD_SUPS_ORIG_USSD,
  VOICEI_CMD_SUPS_ANSWER_USSD,
  VOICEI_CMD_SUPS_CANCEL_USSD,
  VOICEI_CMD_SET_CONFIG_ITEMS,
  VOICEI_CMD_GET_CONFIG_ITEMS,
  VOICEI_CMD_SUPS_GET_COLR,
  VOICEI_CMD_BIND_SUBSCRIPTION,
  VOICEI_CMD_SUPS_ORIG_USSD_NO_WAIT,
  VOICEI_CMD_ALS_SET_LINE_SWITCHING,
  VOICEI_CMD_ALS_SELECT_LINE,
  VOICEI_CMD_AOC_RESET_ACM                   ,
  VOICEI_CMD_AOC_SET_ACMMAX                  ,
  VOICEI_CMD_AOC_GET_CALL_METER_INFO         ,
  VOICEI_CMD_SUPS_MANAGE_IP_CALLS,
  VOICEI_CMD_ALS_GET_LINE_SWITCHING_STATUS,
  VOICEI_CMD_ALS_GET_SELECTED_LINE,
  VOICEI_CMD_SUPS_GET_CNAP,
  VOICEI_CMD_SRVCC_CALL_CONFIG_REQ,
  VOICEI_CMD_CALL_ENCRYPTION_REQ,
  VOICEI_CMD_MAX, 
  VOICEI_CMD_WIDTH                    = 0xFFFF          
} qmi_voicei_cmd_e_type;

typedef enum
{
  VOICEI_CMD_VAL_INDICATION_REGISTER    = 0x0003,
  VOICEI_CMD_VAL_DIAL_CALL              = 0x0020,
  VOICEI_CMD_VAL_END_CALL               = 0x0021,
  VOICEI_CMD_VAL_ANSWER_CALL            = 0x0022,
  VOICEI_CMD_VAL_GET_CALL_INFO          = 0x0024,
  VOICEI_CMD_VAL_OTASP_STATUS           = 0x0025,
  VOICEI_CMD_VAL_INFO_REC_IND           = 0x0026,
  VOICEI_CMD_VAL_SEND_FLASH             = 0x0027,
  VOICEI_CMD_VAL_BURST_DTMF             = 0x0028,
  VOICEI_CMD_VAL_START_CONT_DTMF        = 0x0029,
  VOICEI_CMD_VAL_STOP_CONT_DTMF         = 0x002A,
  VOICEI_CMD_VAL_DTMF_IND               = 0x002B,
  VOICEI_CMD_VAL_SET_PREFERRED_PRIVACY  = 0x002C,
  VOICEI_CMD_VAL_PRIVACY_IND            = 0x002D,
  VOICEI_CMD_VAL_ALL_CALL_STATUS_IND    = 0x002E,
  VOICEI_CMD_VAL_GET_ALL_CALL_INFO      = 0x002F,
  /*Supplementary services API's*/
  VOICEI_CMD_VAL_SUPS_MANAGE_CALLS      = 0x0031,
  VOICEI_CMD_VAL_SUPS_NOTIFICATION_IND  = 0x0032,
  VOICEI_CMD_VAL_SUPS_SET_SUPS_SERVICE  = 0x0033,
  VOICEI_CMD_VAL_SUPS_GET_CALL_WAITING  = 0x0034,
  VOICEI_CMD_VAL_SUPS_GET_CALL_BARRING  = 0x0035,
  VOICEI_CMD_VAL_SUPS_GET_CLIP          = 0x0036,
  VOICEI_CMD_VAL_SUPS_GET_CLIR             = 0x0037,
  VOICEI_CMD_VAL_SUPS_GET_CALL_FORWARDING  = 0x0038,
  VOICEI_CMD_VAL_SUPS_SET_CALL_BARRING  = 0x0039,
  VOICEI_CMD_VAL_SUPS_ORIG_USSD            = 0x003A,
  VOICEI_CMD_VAL_SUPS_ANSWER_USSD          = 0x003B,
  VOICEI_CMD_VAL_SUPS_CANCEL_USSD          = 0x003C,
  VOICEI_CMD_VAL_SUPS_USSD_RELEASE_IND     = 0x003D,
  VOICEI_CMD_VAL_SUPS_USSD_IND             = 0x003E,
  VOICEI_CMD_VAL_SUPS_UUS_IND             = 0x003F,
  /*VOICE NV services API's*/
  VOICEI_CMD_VAL_SET_CONFIG_ITEMS             = 0x0040,
  VOICEI_CMD_VAL_GET_CONFIG_ITEMS             = 0x0041,
  VOICEI_CMD_VAL_SUPS_IND             = 0x0042,
  VOICEI_CMD_VAL_SUPS_ORIG_USSD_NO_WAIT       = 0x0043,
  VOICEI_CMD_VAL_SUPS_ORIG_USSD_NO_WAIT_IND   = 0x0043,
  /*Voice DSDS API's*/
  VOICEI_CMD_VAL_BIND_SUBSCRIPTION             = 0x0044,
  VOICEI_CMD_VAL_ALS_SET_LINE_SWITCHING    = 0x0045,
  VOICEI_CMD_VAL_ALS_SELECT_LINE           = 0x0046,
  VOICEI_CMD_VAL_AOC_RESET_ACM                        = 0x0047,
  VOICEI_CMD_VAL_AOC_SET_ACMMAX                       = 0x0048,
  VOICEI_CMD_VAL_AOC_GET_CALL_METER_INFO              = 0x0049,
  VOICEI_CMD_VAL_AOC_LOW_FUNDS_IND                    = 0x004A,
  VOICEI_CMD_VAL_SUPS_GET_COLP          = 0x004B,
  VOICEI_CMD_VAL_SUPS_GET_COLR             = 0x004C,
  VOICEI_CMD_VAL_SUPS_MANAGE_IP_CALLS      = 0x004E,
  VOICEI_CMD_VAL_ALS_GET_LINE_SWITCHING_STATUS    = 0x004F,
  VOICEI_CMD_VAL_ALS_GET_SELECTED_LINE           = 0x0050,
  VOICEI_CMD_VAL_CALL_MODIFIED_IND             = 0x0051,
  VOICEI_CMD_VAL_CALL_MODIFY_ACCEPT_IND             = 0x0052,
  VOICEI_CMD_VAL_SPEECH_CODEC_INFO_IND      = 0x0053,
  VOICEI_CMD_VAL_SUPS_GET_CNAP           = 0x004D,
  VOICEI_CMD_VAL_HANDOVER_IND           = 0x0054,
  VOICEI_CMD_VAL_CONF_INFO_IND          = 0x0055,
  VOICEI_CMD_VAL_CONF_JOIN_IND          = 0x0056,
  VOICEI_CMD_VAL_CONF_PARTICIPANT_UPDATE_IND          = 0x0057,
   VOICEI_CMD_VAL_EXT_BRST_INTL_IND           = 0x0058,
  VOICEI_CMD_VAL_MT_PAGE_MISS_IND       = 0x0059,
  VOICEI_CMD_VAL_CC_RESULT_INFO_IND      = 0x005A,
  VOICEI_CMD_VAL_CONFERENCE_PARTICIPANTS_INFO_IND  = 0x005B,
  VOICEI_CMD_VAL_SETUP_ANSWER           = 0x005C,
  VOICEI_CMD_VAL_TTY_IND  = 0x005D,
  VOICEI_CMD_VAL_ADDITIONAL_CALL_INFO_IND  = 0x0062,
    VOICEI_CMD_VAL_AUDIO_RAT_CHANGE_INFO_IND  = 0x0063,
  VOICEI_CMD_VAL_SET_SRVCC_CALL_CONFIG      = 0x0064,
  VOICEI_CMD_VAL_E911_ORIG_FAIL_IND  = 0x0065,
  VOICEI_CMD_VAL_CONF_PART_STATUS_IND  = 0x0066, 
  VOICEI_CMD_VAL_CALL_ENCRYPTION_REQ = 0x006D, 
/* QC EXTERNAL QMI COMMAND RANGE IS 0x0000 - 0x5555. 
   Add the next external QMI Command here */

/* VENDOR SPECIFIC QMI COMMAND RANGE IS 0x5556 - 0xAAAA.
   IMPORTANT!
   Add the vendor specific QMI Commands within this range only to avoid 
   conflicts with QC QMI commands that would get released in future */

/* RESERVED QC QMI COMMAND RANGE IS 0xAAAB - 0xFFFE 
   Internal QMI Commands must be added in DECREASING ORDER from Below */
  VOICEI_CMD_VAL_WIDTH                  = 0xFFFF
} qmi_voicei_cmd_val_e_type;


/*---------------------------------------------------------------------------
  QMI VOICE client state definition
---------------------------------------------------------------------------*/
typedef struct qmi_voicei_client_state_s
{
  qmi_common_client_state_type common; // must be first since we alias
  int16 instance;
  int16 service_id;
  struct
  {
    boolean                      reg_dtmf_events;
    boolean                      reg_voice_privacy_events;
    boolean                      reg_sups_events;
    boolean                      reg_call_events;
    boolean                      reg_handover_events;
    boolean                      reg_speech_events;
    boolean                      reg_ussd_notification_events;    
    boolean                      reg_modification_events;
    boolean                      reg_uus_events;   
    boolean                      reg_aoc_events;
    boolean                      reg_conference_events;
    boolean                      reg_brst_intl_events;
    boolean                      reg_mt_page_miss_events;
    boolean                      reg_cc_result_events;
    boolean                      reg_conf_participants_events;
    boolean                      reg_tty_info_events;
    boolean                      reg_audio_rat_change_events;
    boolean                      reg_e911_orig_fail_events;
    boolean                      reg_add_call_info_events;
    boolean                      reg_vice_dialog_info_events;
  }voice_info;
  struct
  {
    boolean is_bind_set_by_client;
    uint8 as_id;
  }asubs_info;
} qmi_voicei_client_state_type;


/*---------------------------------------------------------------------------
  QMI VOICE State state definition
  Watermark queue to hold deferred responses
---------------------------------------------------------------------------*/
typedef struct
{
  uint16                        num_qmi_instances;
  qmi_voicei_client_state_type*  client[VOICEI_MAX_CLIDS];
} qmi_voicei_state_type;

static qmi_voicei_state_type  qmi_voice_state;

typedef struct
{
  qmi_mmode_svc_config_type         cmn_svc_cfg;
  /* Other svc config */
} qmi_voice_svc_config_type;

static qmi_voice_svc_config_type  qmi_voicei_cfg;

struct qmi_voice_e911_orig_fail_ind_s_type
{
  PACK(struct) qmi_voice_e911_orig_fail_ind_s 
  {
    uint8     call_id;
    uint16              end_reason;
  }e911_orig_fail_ind;
};

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

qmi_idl_service_object_type voice_svc_obj;

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

#ifdef _WIN32
#define VOICE_HDLR_PROTOTYPE(x)   dsm_item_type* x( void*, void*, void*, dsm_item_type** )
#else
#define VOICE_HDLR_PROTOTYPE(x)   dsm_item_type* x( void*, void*, void*, dsm_item_type** )__attribute__((unused))
#endif

static VOICE_HDLR_PROTOTYPE(qmi_voicei_dial_call);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_end_call);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_answer_call);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_setup_answer);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_send_flash);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_get_call_info);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_set_preferred_privacy);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_get_all_call_info);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_indication_register);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_burst_dtmf);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_start_cont_dtmf);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_stop_cont_dtmf);
/*Supplementary services*/
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_clir);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_call_forwarding);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_clip);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_colp);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_call_waiting);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_call_barring);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_set_call_barring);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_manage_calls);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_set_sups_service);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_orig_ussd);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_answer_ussd);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_cancel_ussd);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_colr);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_cnap);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_srvcc_call_config_req);


/*Voice NV services*/
static VOICE_HDLR_PROTOTYPE(qmi_voicei_set_modem_config);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_get_modem_config);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_orig_ussd_no_wait);
/*Voice DSDS**/
static VOICE_HDLR_PROTOTYPE(qmi_voicei_bind_subscription);

/* ALS services*/
static VOICE_HDLR_PROTOTYPE(qmi_voicei_als_set_line_switching);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_als_select_line);

/*AoC services*/
static VOICE_HDLR_PROTOTYPE(qmi_voicei_aoc_reset_acm);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_aoc_set_acmmax);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_aoc_get_call_meter_info);

static VOICE_HDLR_PROTOTYPE(qmi_voicei_als_get_line_switching_status);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_als_get_selected_line);

/* VoIP calls */
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_manage_ip_calls);

static dsm_item_type* qmi_voicei_call_encryption_req(void*, void*, void*, dsm_item_type **);

#define VOICE_HDLR(a,b)  QMI_SVC_HDLR( a, (qmi_svc_hdlr_ftype)b )

static qmi_svc_cmd_hdlr_type  qmi_voicei_cmd_callbacks[VOICEI_CMD_MAX] =
{   
  #ifndef REMOVE_QMI_VOICE_INDICATION_REGISTER_V02
  VOICE_HDLR( VOICEI_CMD_VAL_INDICATION_REGISTER, qmi_voicei_indication_register ),
  #endif
  #ifndef REMOVE_QMI_VOICE_DIAL_CALL_V02
  VOICE_HDLR( VOICEI_CMD_VAL_DIAL_CALL, qmi_voicei_dial_call ),
  #endif
  #ifndef REMOVE_QMI_VOICE_END_CALL_V02
  VOICE_HDLR( VOICEI_CMD_VAL_END_CALL, qmi_voicei_end_call ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ANSWER_CALL_V02
  VOICE_HDLR( VOICEI_CMD_VAL_ANSWER_CALL, qmi_voicei_answer_call ),
  #endif
  #ifndef REMOVE_QMI_VOICE_SETUP_ANSWER_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SETUP_ANSWER, qmi_voicei_setup_answer ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CALL_INFO_V02
  VOICE_HDLR( VOICEI_CMD_VAL_GET_CALL_INFO, qmi_voicei_get_call_info ),
  #endif
  #ifndef REMOVE_QMI_VOICE_SEND_FLASH_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SEND_FLASH, qmi_voicei_send_flash ),
  #endif
  #ifndef REMOVE_QMI_VOICE_BURST_DTMF_V02
  VOICE_HDLR( VOICEI_CMD_VAL_BURST_DTMF, qmi_voicei_burst_dtmf ),
  #endif
  #ifndef REMOVE_QMI_VOICE_START_CONT_DTMF_V02
  VOICE_HDLR( VOICEI_CMD_VAL_START_CONT_DTMF, qmi_voicei_start_cont_dtmf ),
  #endif
  #ifndef REMOVE_QMI_VOICE_STOP_CONT_DTMF_V02
  VOICE_HDLR( VOICEI_CMD_VAL_STOP_CONT_DTMF, qmi_voicei_stop_cont_dtmf ),
  #endif
  #ifndef REMOVE_QMI_VOICE_SET_PREFERRED_PRIVACY_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SET_PREFERRED_PRIVACY, qmi_voicei_set_preferred_privacy ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_ALL_CALL_INFO_V02
  VOICE_HDLR( VOICEI_CMD_VAL_GET_ALL_CALL_INFO, qmi_voicei_get_all_call_info ),
  #endif

  /* Sups command handlers */
  #ifndef REMOVE_QMI_VOICE_MANAGE_CALLS_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_MANAGE_CALLS, qmi_voicei_sups_manage_calls ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CLIP_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CLIP, qmi_voicei_sups_get_clip ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CALL_WAITING_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CALL_WAITING, qmi_voicei_sups_get_call_waiting ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CALL_BARRING_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CALL_BARRING, qmi_voicei_sups_get_call_barring ),
  #endif
  #ifndef REMOVE_QMI_VOICE_SET_CALL_BARRING_PASSWORD_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_SET_CALL_BARRING, qmi_voicei_sups_set_call_barring ),
  #endif
  #ifndef REMOVE_QMI_VOICE_SET_SUPS_SERVICE_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_SET_SUPS_SERVICE, qmi_voicei_sups_set_sups_service ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CLIR_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CLIR, qmi_voicei_sups_get_clir ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CALL_FORWARDING_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CALL_FORWARDING, qmi_voicei_sups_get_call_forwarding ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ORIG_USSD_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_ORIG_USSD, qmi_voicei_sups_orig_ussd ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ANSWER_USSD_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_ANSWER_USSD, qmi_voicei_sups_answer_ussd ),
  #endif
  #ifndef REMOVE_QMI_VOICE_CANCEL_USSD_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_CANCEL_USSD, qmi_voicei_sups_cancel_ussd ),
  #endif
  /* NV command handlers */
  #ifndef REMOVE_QMI_VOICE_SET_CONFIG_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SET_CONFIG_ITEMS, qmi_voicei_set_modem_config ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CONFIG_V02
  VOICE_HDLR( VOICEI_CMD_VAL_GET_CONFIG_ITEMS, qmi_voicei_get_modem_config ),
  #endif
  #ifndef REMOVE_QMI_VOICE_BIND_SUBSCRIPTION_V02
  VOICE_HDLR( VOICEI_CMD_VAL_BIND_SUBSCRIPTION, qmi_voicei_bind_subscription ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ORIG_USSD_NO_WAIT_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_ORIG_USSD_NO_WAIT, qmi_voicei_sups_orig_ussd_no_wait ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ALS_SET_LINE_SWITCHING_V02
  VOICE_HDLR( VOICEI_CMD_VAL_ALS_SET_LINE_SWITCHING, qmi_voicei_als_set_line_switching ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ALS_SELECT_LINE_V02
  VOICE_HDLR( VOICEI_CMD_VAL_ALS_SELECT_LINE, qmi_voicei_als_select_line ),
  #endif
  #ifndef REMOVE_QMI_VOICE_AOC_RESET_ACM_V02
  VOICE_HDLR( VOICEI_CMD_VAL_AOC_RESET_ACM, qmi_voicei_aoc_reset_acm ),
  #endif
  #ifndef REMOVE_QMI_VOICE_AOC_SET_ACMMAX_V02
  VOICE_HDLR( VOICEI_CMD_VAL_AOC_SET_ACMMAX, qmi_voicei_aoc_set_acmmax ),
  #endif
  #ifndef REMOVE_QMI_VOICE_AOC_GET_CALL_METER_INFO_V02
  VOICE_HDLR( VOICEI_CMD_VAL_AOC_GET_CALL_METER_INFO, qmi_voicei_aoc_get_call_meter_info ),
  #endif
  #ifndef REMOVE_QMI_VOICE_MANAGE_IP_CALLS_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_MANAGE_IP_CALLS, qmi_voicei_sups_manage_ip_calls ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_COLP_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_COLP, qmi_voicei_sups_get_colp ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_COLR_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_COLR, qmi_voicei_sups_get_colr ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ALS_GET_LINE_SWITCHING_STATUS_V02
  VOICE_HDLR( VOICEI_CMD_VAL_ALS_GET_LINE_SWITCHING_STATUS, qmi_voicei_als_get_line_switching_status ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ALS_GET_SELECTED_LINE_V02
  VOICE_HDLR( VOICEI_CMD_VAL_ALS_GET_SELECTED_LINE, qmi_voicei_als_get_selected_line ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CNAP_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CNAP, qmi_voicei_sups_get_cnap ),
  #endif
  #ifndef REMOVE_QMI_VOICE_SRVCC_CALL_CONFIG_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SET_SRVCC_CALL_CONFIG, qmi_voicei_srvcc_call_config_req ),
  #endif
  #ifndef REMOVE_QMI_VOICE_CALL_ENCRYPTION_REQ_V02
  VOICE_HDLR( VOICEI_CMD_VAL_CALL_ENCRYPTION_REQ, qmi_voicei_call_encryption_req),
  #endif

};



/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/

static void qmi_voicei_dial_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_end_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_answer_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_setup_answer_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_send_flash_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_get_call_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_set_pref_privacy_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_burst_dtmf_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_start_cont_dtmf_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_stop_cont_dtmf_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_get_all_call_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_dtmf_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_info_rec_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_otasp_status_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_privacy_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_notification_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_all_call_status_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_ussd_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_ussd_release_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_uus_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_orig_ussd_no_wait_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_speech_codec_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_conf_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_additional_call_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_conf_join_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_conf_participant_update_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_handover_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_aoc_low_funds_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_ext_brst_intl_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_conference_participants_info_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_mt_page_miss_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_call_control_result_info_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_tty_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_audio_rat_change_info_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_conference_participant_status_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);


static void qmi_voicei_e911_orig_fail_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

/* Sups command responses */
static void qmi_voicei_sups_get_clir_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
/*Supplementary services resp*/

static void qmi_voicei_sups_get_call_forwarding_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
static void qmi_voicei_sups_get_clip_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_get_colp_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);


static void qmi_voicei_sups_get_call_waiting_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_get_call_barring_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_set_call_barring_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_manage_calls_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_set_sups_service_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_orig_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_answer_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_cancel_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_set_modem_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_get_modem_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_als_set_line_switching_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_als_select_line_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_aoc_reset_acm_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_aoc_set_acmmax_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_aoc_get_call_meter_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_manage_ip_calls_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_get_colr_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_als_get_line_switching_status_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_als_get_selected_line_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_call_modified_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_call_modify_accept_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_get_cnap_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_srvcc_call_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_vice_dialog_info_ind
( const qmi_voice_cm_if_info_item_s *const data_ptr
);

void qmi_voicei_call_encryption_resp
( const qmi_voice_cm_if_info_item_s *const data_ptr
);

/* FW callbacks */
static void qmi_voice_fw_init_cback
(
  uint16 num_instances
);

static boolean qmi_voice_fw_alloc_clid_cback
( 
  qmi_framework_common_msg_hdr_type *msg_hdr
);

static void qmi_voice_fw_dealloc_clid_cback
(
  qmi_framework_common_msg_hdr_type *msg_hdr
);

static void qmi_voice_fw_req_cback 
( 
  qmi_framework_msg_hdr_type *msg_hdr,
  dsm_item_type ** sdu 
);

void qmi_voice_handle_fw_cmd
(
  qmi_mmode_cmd_enum_type cmd,
  void *            user_data_ptr
);

void qmi_voice_process_cmd
(
  qmi_mmode_cmd_enum_type        cmd,
  void                   * user_data_ptr
);

/*===========================================================================
  FUNCTION QMI_VOICE_INIT()

  DESCRIPTION
    Register the VOICE service with QMI FW

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voice_init
(
  void
)
{
  qmi_framework_err_e_type      errval;
  qmi_idl_service_object_type     svc_obj;
  uint32_t                          idl_minor_ver = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //QMI VOICE qmi_voice_init

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the cmd handlers in QMI MMODE task */
  qmi_mmode_set_cmd_handler(QMI_MMODE_CMD_VOICE_FW_CB, qmi_voice_handle_fw_cmd);
    
  /* memset the service configuration */
    memset(&qmi_voicei_cfg.cmn_svc_cfg.fw_cfg, 0, sizeof(qmi_framework_svc_config_type) );
    
  /*-----------------------------------------------------------------------
  Step1: Fill configuration 
  -----------------------------------------------------------------------*/
  qmi_voicei_cfg.cmn_svc_cfg.fw_cfg.base_version.major     = VOICEI_BASE_VER_MAJOR;
  qmi_voicei_cfg.cmn_svc_cfg.fw_cfg.base_version.minor     = VOICEI_BASE_VER_MINOR;
  qmi_voicei_cfg.cmn_svc_cfg.fw_cfg.addendum_version.major = VOICEI_ADDENDUM_VER_MAJOR;
  qmi_voicei_cfg.cmn_svc_cfg.fw_cfg.addendum_version.minor = VOICEI_ADDENDUM_VER_MINOR;
    
  /* mandatory callbacks should be filled */
  qmi_voicei_cfg.cmn_svc_cfg.fw_cfg.cbs.alloc_clid         = qmi_voice_fw_alloc_clid_cback;
  qmi_voicei_cfg.cmn_svc_cfg.fw_cfg.cbs.dealloc_clid       = qmi_voice_fw_dealloc_clid_cback;
  qmi_voicei_cfg.cmn_svc_cfg.fw_cfg.cbs.init_cback         = qmi_voice_fw_init_cback;
  qmi_voicei_cfg.cmn_svc_cfg.fw_cfg.cbs.cmd_hdlr           = qmi_voice_fw_req_cback;
  
  qmi_voicei_cfg.cmn_svc_cfg.cmd_hdlr_array    = qmi_voicei_cmd_callbacks;
  qmi_voicei_cfg.cmn_svc_cfg.cmd_num_entries   = VOICEI_CMD_MAX;
  qmi_voicei_cfg.cmn_svc_cfg.service_id = QMUX_SERVICE_VOICE;
  
  /*-----------------------------------------------------------------------
  step 2:  calling QMI Framework API to register the service.
  ----------------------------------------------------------------------*/
  errval = qmi_framework_reg_service(QMUX_SERVICE_VOICE, &qmi_voicei_cfg.cmn_svc_cfg.fw_cfg);
  if (QMI_FRAMEWORK_ERR_NONE != errval)
  {
    QM_MSG_ERROR_2("Service registration failed Service:%d error:%d", QMUX_SERVICE_VOICE, errval);
    if (QMI_FRAMEWORK_ERR_FW_NOT_READY == errval)
    {
     /* Start timer and set signal to mmode task and try to do the registration again ?
       Have a max retry count and exit ? TBD */
    }
      /*-----------------------------------------------------------------------
    step 3:  Service registration fails. See error code. 
      -----------------------------------------------------------------------*/
    // Registration may have failied due to any one of the following reasons
    // 1. Service already registered  
    // 2. Mandatory callback is missing
    // 3. Framework is NOT ready
    // If Framework is not ready, then service needs to retry registration later.
    // Handling different failures is left to the discretion of the QMI service owner 
  }

  svc_obj =  voice_get_service_object_v02();
  (void) qmi_idl_get_idl_version( svc_obj, &idl_minor_ver );
  (void) qmi_si_register_id( QMUX_SERVICE_VOICE, 
                             0, /* service_instance */
                             2, /* IDL major version */
                             idl_minor_ver, 
                             voice_get_service_impl_v02() );

} /* qmi_voice_init() */


void qmi_voice_process_cmd
(
  qmi_mmode_cmd_enum_type        cmd,
  void                   * user_data_ptr
)
{
  #ifndef FEATURE_NO_QMI_VOICE
  qmi_mmode_cmd_data_buf_type *  data_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT (user_data_ptr);

  data_ptr = (qmi_mmode_cmd_data_buf_type *) user_data_ptr;
  ASSERT (data_ptr->cmd.cm_if_data_ptr->header.cmd < QMI_VOICE_CM_IF_CMD_MAX);

  QM_MSG_MED_1("QMI VOICE qmi_voice_process_cmd %d",
               data_ptr->cmd.cm_if_data_ptr->header.cmd);

  switch (data_ptr->cmd.cm_if_data_ptr->header.cmd)
  {
    case QMI_VOICE_CM_IF_CMD_DIAL_CALL_RESP:
      qmi_voicei_dial_call_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_END_CALL_RESP:
      qmi_voicei_end_call_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_ANSWER_CALL_RESP:
      qmi_voicei_answer_call_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SETUP_ANSWER_RESP:
      qmi_voicei_setup_answer_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_GET_ALL_CALL_INFO_RESP:
      qmi_voicei_get_all_call_info_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SEND_FLASH_RESP:
      qmi_voicei_send_flash_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_INFO_REC_IND:
      qmi_voicei_info_rec_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_OTASP_STATUS_IND:
      qmi_voicei_otasp_status_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_GET_CALL_INFO_RESP:
      qmi_voicei_get_call_info_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SET_PREFERRED_PRIVACY_RESP:
      qmi_voicei_set_pref_privacy_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_BURST_DTMF_RESP:
      qmi_voicei_burst_dtmf_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_START_CONT_DTMF_RESP:
      qmi_voicei_start_cont_dtmf_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_STOP_CONT_DTMF_RESP:
      qmi_voicei_stop_cont_dtmf_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_PRIVACY_IND:
      qmi_voicei_privacy_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_DTMF_IND:
      qmi_voicei_dtmf_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_NOTIFICATION_IND:
      qmi_voicei_sups_notification_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_ALL_CALL_STATUS_IND:
      qmi_voicei_all_call_status_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
/* Sups responses */
    case QMI_VOICE_CM_IF_CMD_SUPS_MANAGE_CALLS_RESP:
      qmi_voicei_sups_manage_calls_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CLIR_RESP:
      qmi_voicei_sups_get_clir_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_FORWARDING_RESP:
      qmi_voicei_sups_get_call_forwarding_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CLIP_RESP:
       qmi_voicei_sups_get_clip_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_WAITING_RESP:
       qmi_voicei_sups_get_call_waiting_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_BARRING_RESP:
       qmi_voicei_sups_get_call_barring_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
    case QMI_VOICE_CM_IF_CMD_SUPS_SET_CALL_BARRING_RESP:
       qmi_voicei_sups_set_call_barring_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
    case QMI_VOICE_CM_IF_CMD_SUPS_SET_SUPS_SERVICE_RESP:
       qmi_voicei_sups_set_sups_service_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
    case QMI_VOICE_CM_IF_CMD_SUPS_ORIG_USSD_RESP:
      qmi_voicei_sups_orig_ussd_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_ANSWER_USSD_RESP:
      qmi_voicei_sups_answer_ussd_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_CANCEL_USSD_RESP:
      qmi_voicei_sups_cancel_ussd_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_USSD_IND:
      qmi_voicei_sups_ussd_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_USSD_RELEASE_IND:
      qmi_voicei_sups_ussd_release_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_UUS_IND:
      qmi_voicei_uus_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_IND:
      qmi_voicei_sups_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_HANDOVER_IND:
      qmi_voicei_handover_ind( data_ptr->cmd.cm_if_data_ptr );
      break;	  
    case QMI_VOICE_CM_IF_CMD_AOC_LOW_FUNDS_IND:
      qmi_voicei_aoc_low_funds_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_EXT_BRST_INTL_IND:
      qmi_voicei_ext_brst_intl_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SPEECH_CODEC_INFO_IND:
      qmi_voicei_speech_codec_info_ind( data_ptr->cmd.cm_if_data_ptr );
      break;      
    /*Modem Configuration items response*/
    case QMI_VOICE_CM_IF_CMD_SET_MODEM_CONFIG_RESP:
      qmi_voicei_set_modem_config_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_GET_MODEM_CONFIG_RESP:
      qmi_voicei_get_modem_config_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_ORIG_USSD_NO_WAIT_IND:
      qmi_voicei_sups_orig_ussd_no_wait_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_ALS_SET_LINE_SWITCHING_RESP:
      qmi_voicei_als_set_line_switching_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_ALS_SELECT_LINE_RESP:
      qmi_voicei_als_select_line_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_AOC_RESET_ACM_RESP:
      qmi_voicei_aoc_reset_acm_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_AOC_SET_ACMMAX_RESP:
      qmi_voicei_aoc_set_acmmax_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_AOC_GET_CALL_METER_INFO_RESP:
      qmi_voicei_aoc_get_call_meter_info_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_MANAGE_IP_CALLS_RESP:
      qmi_voicei_sups_manage_ip_calls_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_COLP_RESP:
       qmi_voicei_sups_get_colp_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
   case QMI_VOICE_CM_IF_CMD_SUPS_GET_COLR_RESP:
      qmi_voicei_sups_get_colr_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_ALS_GET_LINE_SWITCHING_STATUS_RESP:
      qmi_voicei_als_get_line_switching_status_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_ALS_GET_SELECTED_LINE_RESP:
      qmi_voicei_als_get_selected_line_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_CALL_MODIFIED_IND:
      qmi_voicei_call_modified_ind( data_ptr->cmd.cm_if_data_ptr );
      break;      
    case QMI_VOICE_CM_IF_CMD_CALL_MODIFY_ACCEPT_IND:
      qmi_voicei_call_modify_accept_ind( data_ptr->cmd.cm_if_data_ptr );
      break;      
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CNAP_RESP:
      qmi_voicei_sups_get_cnap_resp( data_ptr->cmd.cm_if_data_ptr );
      break;      
    case QMI_VOICE_CM_IF_CMD_CONF_INFO_IND:
      qmi_voicei_conf_info_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_ADDITIONAL_CALL_INFO_IND:
      qmi_voicei_additional_call_info_ind( data_ptr->cmd.cm_if_data_ptr );
      break;      
    case QMI_VOICE_CM_IF_CMD_CONF_PARTICIPANT_STATUS_IND:
      qmi_voicei_conference_participant_status_ind( data_ptr->cmd.cm_if_data_ptr );
      break; 	  
    case QMI_VOICE_CM_IF_CMD_CONF_JOIN_IND:
      qmi_voicei_conf_join_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_CONF_PARTICIPANT_UPDATE_IND:
      qmi_voicei_conf_participant_update_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_CONF_PARTICIPANTS_INFO_IND:
      qmi_voicei_conference_participants_info_ind(data_ptr->cmd.cm_if_data_ptr);
      break;
    case QMI_VOICE_CM_IF_CMD_MT_PAGE_MISS_IND:
      qmi_voicei_mt_page_miss_ind(data_ptr->cmd.cm_if_data_ptr);
      break;
    case QMI_VOICE_CM_IF_CMD_CC_RESULT_INFO_IND:
      qmi_voicei_call_control_result_info_ind(data_ptr->cmd.cm_if_data_ptr);
      break;
    case QMI_VOICE_CM_IF_CMD_SRVCC_CALL_CONFIG_RESP:
      qmi_voicei_srvcc_call_config_resp(data_ptr->cmd.cm_if_data_ptr);
      break;
    case QMI_VOICE_CM_IF_CMD_TTY_INFO_IND:
      qmi_voicei_tty_ind(data_ptr->cmd.cm_if_data_ptr);
      break;
    case QMI_VOICE_CM_IF_CMD_AUDIO_RAT_CHANGE_INFO_IND:
      qmi_voicei_audio_rat_change_info_ind(data_ptr->cmd.cm_if_data_ptr);
      break;
    case QMI_VOICE_CM_IF_CMD_E911_ORIG_FAIL_IND:
      qmi_voicei_e911_orig_fail_ind(data_ptr->cmd.cm_if_data_ptr);
      break;

    case QMI_VOICE_CM_IF_CMD_VICE_DIALOG_IND:
      qmi_voicei_vice_dialog_info_ind(data_ptr->cmd.cm_if_data_ptr);
      break;

    case QMI_VOICE_CM_IF_CMD_CALL_ENCRYPTION_RESP:
      qmi_voicei_call_encryption_resp(data_ptr->cmd.cm_if_data_ptr);
      break;  

    default:
      QM_MSG_MED_1("Ignoring cmd %d", data_ptr->cmd.cm_if_data_ptr->header.cmd);
      break;
  }
  QMI_VOICE_CM_IF_MEM_FREE(data_ptr->cmd.cm_if_data_ptr);
  #else
  (void) cmd;
  (void) user_data_ptr;
  #endif /* !FEATURE_NO_QMI_VOICE */
  return;
}

/*===========================================================================
  FUNCTION QMI_VOICE_HAS_VALID_CLIENT()

  DESCRIPTION
    Checks if there is a client using this service
    
  PARAMETERS
    None
    
  RETURN VALUE
    True if QMI VOICE has a valid client.
    False otherwise.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_voice_has_valid_client
( 
  void
)
{
  qmi_voicei_state_type *        voice_sp;
  qmi_voicei_client_state_type * cl_sp;
  int                            j;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if((cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED))
    {
      return TRUE;
    }
  }

  return FALSE;
} /* qmi_voice_has_valid_client */


/*===========================================================================
  FUNCTION qmi_voicei_validate_cmd_buf_p()

  DESCRIPTION
    Function to validate returned cmd_buf_p is still allocated in client.

  PARAMETERS
    cmd_buf_p: pointer to queue element for comparison

RETURN VALUE
  Pointer to the client state if cmdbuf valid. otherwise NULL
===========================================================================*/
qmi_voicei_client_state_type* qmi_voicei_validate_cmd_buf_p( qmi_cmd_buf_type *cmd_buf_p )
{
  int i, j;
  qmi_voicei_state_type *svc_state_ptr = &qmi_voice_state;

  if(cmd_buf_p == NULL)
  {
    return NULL;
  }

  for( i = 0; i < VOICEI_MAX_CLIDS; i++ )
  {
    if( svc_state_ptr->client[i] != NULL )
    {
      for( j = 0; j < MAX_PENDING_COMMANDS; j++ )
      {
        if( cmd_buf_p == &(svc_state_ptr->client[i]->common.cmd_buf[j]) )
        {
          if( svc_state_ptr->client[i]->common.cmd_buf[j].x_p != NULL )
          {
            return svc_state_ptr->client[i];
          }
          else
          {
            QM_MSG_ERROR_2("Client %d cmd_buf %d has null transaction", i, j);
            return NULL;
          }
        }
      }
    }
  }

  QM_MSG_HIGH("Buffer does not match existing client transaction");
  return NULL;
}


/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

/*===========================HELPER FUNCTIONS==============================*/

/*===========================================================================
  FUNCTION QMI_VOICEI_GET_CLIENT_SP_BY_CLID()

  DESCRIPTION
    Retrieve client sp based on clid

  PARAMETERS
    clid  :  uint8

  RETURN VALUE
    cl_sp   :  client sp

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void * qmi_voicei_get_client_sp_by_clid
(
  uint8 clid
)
{
  qmi_voicei_state_type *voice_sp;
  int i=0;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((clid == QMI_SVC_CLID_UNUSED ) ||
      (clid > VOICEI_MAX_CLIDS ))
  {
    QM_MSG_MED_1("Invalid client ID clid :%d", clid);
    return NULL;
  }

  voice_sp = &qmi_voice_state;
  for(i=0; i < VOICEI_MAX_CLIDS; i++ ) 
  {
    if(voice_sp->client[i] != NULL)
    {
      if( voice_sp->client[i]->common.clid == clid)
      {
        return voice_sp->client[i];
      }
    }
  }
  return NULL;
} /* qmi_voicei_get_client_sp_by_clid() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SEND_RESPONSE()

  DESCRIPTION
    Send the response to framework

  PARAMETERS
    clid: uint8

  RETURN VALUE
    TRUE if sent successfully, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_voicei_send_response
(
  qmi_error_e_type        errval,
  qmi_cmd_buf_type *      cmd_buf_p,
  dsm_item_type *         msg_ptr
)
{
  qmi_framework_common_msg_hdr_type  common_hdr;
  qmi_voicei_client_state_type *    cl_sp;
  qmi_result_e_type       result;
  boolean status = TRUE;


  /* Check if the client is still present */
  if( (cl_sp = qmi_voicei_validate_cmd_buf_p(cmd_buf_p)) == NULL )
  {
    QM_MSG_ERROR("Client, transaction no longer valid. Cannot send response");
    dsm_free_packet(&msg_ptr);
    return FALSE;
  }

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  if(!qmi_svc_put_result_tlv(&msg_ptr, result, errval))
  {
    QM_MSG_ERROR("qmi_svc_put_result_tlv failed. Cannot send response");
    qmi_mmode_svc_free_transaction_cmd_buf( &cmd_buf_p );
    dsm_free_packet(&msg_ptr);
    return FALSE;
  }

  /* Fill common header and send response */
  memset(&common_hdr, 0, sizeof(common_hdr));
  common_hdr.service = (qmux_service_e_type)cl_sp->service_id;
  common_hdr.client_id = cl_sp->common.clid;
  /* Transaction id will be filled by svc layer */
  common_hdr.qmi_instance = cl_sp->instance;

  /* Send the response */
  status = qmi_mmode_svc_send_response( &common_hdr,cmd_buf_p, msg_ptr);
  if(!status && ( msg_ptr != NULL ))
  {
    dsm_free_packet(&msg_ptr);
  }
  return status;
}/* qmi_voicei_send_response */


/*===========================================================================
  FUNCTION QMI_VOICEI_SEND_RESPONSE_ENC()

  DESCRIPTION
    Send the encoded response to framework

  PARAMETERS
    clid: uint8

  RETURN VALUE
    TRUE if sent successfully, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_voicei_send_response_enc
(
  qmi_cmd_buf_type *      cmd_buf_p,
  dsm_item_type *         msg_ptr
)
{
  qmi_framework_common_msg_hdr_type  common_hdr;
  qmi_voicei_client_state_type *    cl_sp;
  boolean status = TRUE;
  uint16  message_id;


  /* Check if the client is still present */
  if( (cl_sp = qmi_voicei_validate_cmd_buf_p(cmd_buf_p)) == NULL )
  {
    QM_MSG_ERROR("Client, transaction no longer valid. Cannot send response");
    dsm_free_packet(&msg_ptr);
    return FALSE;
  }
  message_id = cmd_buf_p->cmd_type;
  /* Fill common header and send response */
  memset(&common_hdr, 0, sizeof(common_hdr));
  common_hdr.service = (qmux_service_e_type)cl_sp->service_id;
  common_hdr.client_id = cl_sp->common.clid;
  /* Transaction id will be filled by svc layer */
  common_hdr.qmi_instance = cl_sp->instance;

  /* Send the response */
  status = qmi_mmode_svc_send_response( &common_hdr,cmd_buf_p, msg_ptr);
  if(!status && ( msg_ptr != NULL ))
  {
    dsm_free_packet(&msg_ptr);
  }
  QM_MSG_HIGH_2("Status of QMI VOICE Response(%d) encoded is: %d", message_id, status);
  return status;
}/* qmi_voicei_send_response_enc */


/*===========================================================================
  FUNCTION QMI_VOICEI_SEND_INDICATION()

  DESCRIPTION
    This function is calls the QMI Framework API to send out the VOICE service
    indication to client.

  PARAMETERS
    msg_hdr_p : QMI Framework message header
    cmd_type : type of indication
    ind : dsm item
 
  RETURN VALUE
    TRUE if ind sent to Framework sucessfully
    FALSE otherwise
 
  DEPENDENCIES
    QMI Voice service must be initialized and registered with Framework
 
  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_voicei_send_indication
(
  qmi_voicei_client_state_type * cl_sp,
  uint16                         cmd_type,
  dsm_item_type *                ind
)
{
  qmi_framework_common_msg_hdr_type  common_hdr;
  boolean status = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(cl_sp);  

  /*-----------------------------------------------------------------------
    Fill the QMI Framework message common header 
  -----------------------------------------------------------------------*/
  common_hdr.client_id =  cl_sp->common.clid;
  common_hdr.qmi_instance = cl_sp->instance;
  common_hdr.service = (qmux_service_e_type)cl_sp->service_id;

  status = qmi_mmode_svc_send_indication(&common_hdr, cmd_type,ind );
  if(!status && (ind != NULL))
  {
    dsm_free_packet(&ind);
  }
  QM_MSG_MED_2("Status of QMI Voice indication(%d) sent: %d",cmd_type, status);
  return status;
} /* qmi_voicei_send_indication */


/*===========================================================================
  FUNCTION QMI_VOICEI_ALLOC_CL_SP()

  DESCRIPTION
    Allocate the client data buffer

  PARAMETERS
    clid: uint8

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_voicei_client_state_type * qmi_voicei_alloc_cl_sp
(
  uint8 clid
)
{
  qmi_voicei_client_state_type *  cl_sp = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QM_MSG_MED_1("qmi_voicei_alloc_cl_sp clid :%d", clid);

  if ((clid == QMI_SVC_CLID_UNUSED ) ||
      (clid > VOICEI_MAX_CLIDS ))
  {
    QM_MSG_MED_1("Invalid client ID clid :%d", clid);
    return cl_sp;
  }
  cl_sp = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(qmi_voicei_client_state_type));
  if ( cl_sp == NULL ) 
  {
	  qmi_voice_mem_error();
    ASSERT(cl_sp);
  }
  qmi_voice_state.client[clid - 1] = cl_sp;
  return cl_sp;
} /* qmi_voicei_alloc_cl_sp */


/*===========================================================================
  FUNCTION QMI_VOICE_FW_DEALLOC_CL_SP()

  DESCRIPTION
    Free the client data buffer for the given clid.

  PARAMETERS
    clid: uint8 type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_voicei_dealloc_cl_sp
(
  uint8 clid
)
{
  qmi_voicei_client_state_type *  cl_sp = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QM_MSG_MED_1("qmi_voicei_dealloc_cl_sp clid :%d", clid);

  if ((clid == QMI_SVC_CLID_UNUSED ) ||
      (clid > VOICEI_MAX_CLIDS ))
  {
    QM_MSG_MED_1("Invalid client ID clid :%d", clid);
    return FALSE;
  }

  cl_sp = (qmi_voicei_client_state_type *) qmi_voice_state.client[clid - 1];
  if(cl_sp)
  {
    q_destroy( &cl_sp->common.x_free_q );
    q_destroy( &cl_sp->common.cmd_free_q );

    QMI_VOICE_CM_IF_MEM_FREE(cl_sp);
    qmi_voice_state.client[clid - 1] = NULL;
  }
  else
  {
    QM_MSG_ERROR("Client state pointer is already NULL");
    return FALSE;
  }
  return TRUE;
} /* qmi_voicei_dealloc_cl_sp */


/*===========================================================================
  FUNCTION QMI_VOICEI_RESET_CLIENT()

  DESCRIPTION
    Reset the client data

  PARAMETERS
    cl_sp_ptr: qmi_voicei_client_state_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_reset_client
(
  qmi_voicei_client_state_type * cl_sp_ptr
)
{

  ASSERT(cl_sp_ptr);

  qmi_mmode_svc_reset_common_client(&cl_sp_ptr->common);
  
  cl_sp_ptr->voice_info.reg_dtmf_events = FALSE;
  cl_sp_ptr->voice_info.reg_voice_privacy_events = FALSE;
  cl_sp_ptr->voice_info.reg_sups_events = TRUE;
  cl_sp_ptr->voice_info.reg_call_events = TRUE;
  cl_sp_ptr->voice_info.reg_handover_events = FALSE;
  cl_sp_ptr->voice_info.reg_speech_events   = FALSE;
  cl_sp_ptr->voice_info.reg_ussd_notification_events = TRUE;
  cl_sp_ptr->voice_info.reg_modification_events = TRUE;
  cl_sp_ptr->voice_info.reg_uus_events = TRUE;
  cl_sp_ptr->voice_info.reg_aoc_events               = FALSE;
  cl_sp_ptr->voice_info.reg_conference_events        = FALSE;
  cl_sp_ptr->voice_info.reg_brst_intl_events = FALSE;
  cl_sp_ptr->voice_info.reg_conf_participants_events = FALSE;
  cl_sp_ptr->voice_info.reg_mt_page_miss_events = FALSE;
  cl_sp_ptr->voice_info.reg_cc_result_events = FALSE;
  cl_sp_ptr->voice_info.reg_tty_info_events = FALSE;
  cl_sp_ptr->voice_info.reg_audio_rat_change_events = FALSE;
  cl_sp_ptr->voice_info.reg_e911_orig_fail_events = FALSE;
  cl_sp_ptr->voice_info.reg_add_call_info_events = FALSE;
  cl_sp_ptr->voice_info.reg_vice_dialog_info_events = FALSE;

  QM_MSG_MED_1("qmi_voicei_reset_client clid =%d", cl_sp_ptr->common.clid);

} /* qmi_voicei_reset_client() */


/*===========================FW CALLBACK HANDLING==========================*/

/*===========================================================================
  FUNCTION QMI_VOICE_INIT_CBACK()

  DESCRIPTION
    Callback for Service initialization from Framework

  PARAMETERS
    num_instances: uint16

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voice_fw_init_cback
(
  uint16 num_instances
)
{
    qmi_mmode_cmd_data_buf_type *cmd_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    ;
    if( (cmd_ptr = qmi_mmode_get_cmd_data_buf()) == NULL)
    {
      //Unable to allocate QMI VOICE cmd buf
      return;
    }
    memset(cmd_ptr, 0, sizeof(qmi_mmode_cmd_data_buf_type));
    cmd_ptr->cmd.qmi_ptr = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(qmi_mmode_common_cmd_type));
    if( NULL == cmd_ptr->cmd.qmi_ptr )
    {
		qmi_voice_mem_fatal();
      qmi_mmode_free_cmd_data_buf(cmd_ptr);
      return;
    }

    
    cmd_ptr->cmd.qmi_ptr->id = QMI_MMODE_FW_INIT_CB;
    cmd_ptr->cmd.qmi_ptr->data.qmi_fw_info.init_cb.num_instances = num_instances;

    qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_FW_CB, cmd_ptr);
}


/*===========================================================================
  FUNCTION QMI_VOICEI_FW_INIT_CBACK_HDLR()

  DESCRIPTION
    Process the Service initialization request from Framework

  PARAMETERS
    num_instances: uint16

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voicei_fw_init_cback_hdlr
(
  uint16 num_instances
)
{
  qmi_voicei_state_type *voice_sp;

  voice_sp = &qmi_voice_state;
  memset(voice_sp, 0, sizeof(qmi_voicei_state_type));

  voice_sp->num_qmi_instances = num_instances;

  qmi_voicei_cfg.cmn_svc_cfg.svc_sp = voice_sp;

  qmi_mmode_set_cmd_handler(QMI_MMODE_CMD_VOICE_CB, qmi_voice_process_cmd);

  /* Initialize the Voice -> CM  interface */
  qmi_voice_cm_if_init();
  
  voice_svc_obj = voice_get_service_object_v02();
}/* voicei_init_cback */


/*===========================================================================
  FUNCTION QMI_VOICE_ALLOC_CLID_CBACK()

  DESCRIPTION
    Callback to allocate the client from Framework

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_voice_fw_alloc_clid_cback
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_mmode_cmd_data_buf_type *cmd_ptr;

  ASSERT(common_msg_hdr);

  if( (cmd_ptr = qmi_mmode_get_cmd_data_buf()) == NULL)
  {
    //Unable to allocate QMI VOICE cmd buf"
    return FALSE;
  }
  memset(cmd_ptr, 0, sizeof(qmi_mmode_cmd_data_buf_type));
  cmd_ptr->cmd.qmi_ptr = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(qmi_mmode_common_cmd_type));
  if( NULL == cmd_ptr->cmd.qmi_ptr )
  {
    qmi_voice_mem_fatal();
    qmi_mmode_free_cmd_data_buf(cmd_ptr);
    return FALSE;
  }

  cmd_ptr->cmd.qmi_ptr->id = QMI_MMODE_FW_ALLOC_CLID_CB;
  cmd_ptr->cmd.qmi_ptr->data.qmi_fw_info.alloc_clid.msg_hdr = *common_msg_hdr;

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_FW_CB, cmd_ptr);
  return TRUE;

}/* qmi_voice_fw_alloc_clid */


/*===========================================================================
  FUNCTION QMI_VOICE_ALLOC_CLID_HDLR()

  DESCRIPTION
    Process the client allocation request from Framework

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voicei_fw_alloc_clid_hdlr
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_voicei_client_state_type   *  cl_sp;  
  qmi_result_e_type                result = QMI_RESULT_FAILURE;
  qmi_voicei_state_type *  voice_sp;
  qmi_error_e_type                errval = QMI_ERR_NONE;
/*-------------------------------------------------------------------------*/

  ASSERT(common_msg_hdr);
  cl_sp = NULL;
  voice_sp = &qmi_voice_state;
  cl_sp = qmi_voicei_alloc_cl_sp(common_msg_hdr->client_id);

  if(cl_sp)
  {
    qmi_mmode_svc_initialize_common_client(&cl_sp->common, voice_sp);

    cl_sp->voice_info.reg_dtmf_events = FALSE;
    cl_sp->voice_info.reg_voice_privacy_events = FALSE;
    cl_sp->voice_info.reg_sups_events = TRUE;
    cl_sp->voice_info.reg_call_events = TRUE;
    cl_sp->voice_info.reg_handover_events = FALSE;
    cl_sp->voice_info.reg_speech_events = FALSE;
    cl_sp->voice_info.reg_ussd_notification_events = TRUE;
    cl_sp->voice_info.reg_modification_events = TRUE;
    cl_sp->voice_info.reg_uus_events = TRUE;
    cl_sp->voice_info.reg_aoc_events               = FALSE;
    cl_sp->voice_info.reg_conference_events = FALSE;
    cl_sp->voice_info.reg_brst_intl_events = FALSE;
    cl_sp->voice_info.reg_conf_participants_events = FALSE;
    cl_sp->voice_info.reg_mt_page_miss_events = FALSE;
    cl_sp->voice_info.reg_cc_result_events = FALSE;
    cl_sp->voice_info.reg_tty_info_events = FALSE;
    cl_sp->voice_info.reg_audio_rat_change_events = FALSE;
    cl_sp->voice_info.reg_e911_orig_fail_events = FALSE;
    cl_sp->voice_info.reg_add_call_info_events = FALSE;
    cl_sp->voice_info.reg_vice_dialog_info_events = FALSE;

    cl_sp->asubs_info.is_bind_set_by_client = FALSE;
    cl_sp->asubs_info.as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

    cl_sp->common.clid = common_msg_hdr->client_id;
    cl_sp->instance    = (uint16)common_msg_hdr->qmi_instance;
    cl_sp->service_id  = common_msg_hdr->service ;

    QM_MSG_MED_1("qmi_voicei clid set to local client state ptr:  clid%d", cl_sp->common.clid);
    result = QMI_RESULT_SUCCESS;
  }
  else
  {
    QM_MSG_MED("No available service clids!");
    errval = QMI_ERR_CLIENT_IDS_EXHAUSTED;
  }

  /* Notifying the clid allocation back to client via QMI Framework*/
  qmi_framework_svc_send_alloc_clid_result_ex(result, common_msg_hdr,errval);

}/* qmi_voicei_fw_alloc_clid_hdlr */


/*===========================================================================
  FUNCTION QMI_VOICE_FW_DEALLOC_CLID_CBACK()

  DESCRIPTION
    Deallocate the client data.

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_voice_fw_dealloc_clid_cback
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_mmode_cmd_data_buf_type *cmd_ptr;

  ASSERT(common_msg_hdr);

  if( (cmd_ptr = qmi_mmode_get_cmd_data_buf()) == NULL)
  {
    //Unable to allocate QMI VOICE cmd buf
    return;
  }
  memset(cmd_ptr, 0, sizeof(qmi_mmode_cmd_data_buf_type));
  cmd_ptr->cmd.qmi_ptr = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(qmi_mmode_common_cmd_type));
  if( NULL == cmd_ptr->cmd.qmi_ptr )
  {
	  qmi_voice_mem_fatal();
    qmi_mmode_free_cmd_data_buf(cmd_ptr);
    return;
  }

  cmd_ptr->cmd.qmi_ptr->id = QMI_MMODE_FW_DEALLOC_CLID_CB;
  cmd_ptr->cmd.qmi_ptr->data.qmi_fw_info.dealloc_clid.msg_hdr = *common_msg_hdr;

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_FW_CB, cmd_ptr);

}/* qmi_voice_fw_dealloc_clid_cback */


/*===========================================================================
  FUNCTION QMI_VOICE_FW_DEALLOC_CLID_HDLR()

  DESCRIPTION
    Process the dealloc clid request from framework.

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voicei_fw_dealloc_clid_hdlr
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_voicei_client_state_type   *  cl_sp;   
  qmi_result_e_type                result = QMI_RESULT_FAILURE; 
  qmi_error_e_type                errval = QMI_ERR_NONE;
 /*-------------------------------------------------------------------------*/
 
   ASSERT(common_msg_hdr);
 

  if ((common_msg_hdr->client_id == QMI_SVC_CLID_UNUSED ) ||
      (common_msg_hdr->client_id > VOICEI_MAX_CLIDS ))
  {
    QM_MSG_ERROR_1("Invalid client ID clid :%d", common_msg_hdr->client_id);
    return;
  }

   cl_sp = NULL;
   //qmi_voicei_fw_dealloc_clid_hdlr
 
   cl_sp = (qmi_voicei_client_state_type *) 
           qmi_voice_state.client[common_msg_hdr->client_id  - 1];
 
   if (cl_sp != NULL && cl_sp->common.clid == common_msg_hdr->client_id)
   {
     qmi_voicei_reset_client(cl_sp);
     qmi_voicei_dealloc_cl_sp(common_msg_hdr->client_id);
     QM_MSG_MED_1("qmi voice clid free to local client state ptr:  clid%d", common_msg_hdr->client_id);
     result = QMI_RESULT_SUCCESS;
   }
   else
   {
     QM_MSG_MED("No available service clids!");
     errval = QMI_ERR_INVALID_CLIENT_ID;
   }
 
   /* Notifying the clid deallocation back to client via QMI Framework*/
   qmi_framework_svc_send_dealloc_clid_result_ex(result, common_msg_hdr, errval);

}/* qmi_voicei_fw_dealloc_clid_hdlr */


/*===========================================================================
  FUNCTION QMI_VOICE_FW_REQ_CB()

  DESCRIPTION
    Callback for qmi request from Framework

  PARAMETERS
    msg_hdr: qmi_framework_msg_hdr_type ptr
    sdu: dsm_item_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voice_fw_req_cback
( 
  qmi_framework_msg_hdr_type *msg_hdr, 
  dsm_item_type **sdu
)
{
  qmi_mmode_cmd_data_buf_type *cmd_ptr;

  ASSERT(msg_hdr);

  if( (cmd_ptr = qmi_mmode_get_cmd_data_buf()) == NULL)
  {
    //Unable to allocate QMI VOICE cmd buf
    return;
  }
  memset(cmd_ptr, 0, sizeof(qmi_mmode_cmd_data_buf_type));
  cmd_ptr->cmd.qmi_ptr = QMI_VOICE_CM_IF_MEM_ALLOC(
                                 sizeof(qmi_mmode_common_cmd_type));
  if( NULL == cmd_ptr->cmd.qmi_ptr )
  {
	  qmi_voice_mem_fatal();
    qmi_mmode_free_cmd_data_buf(cmd_ptr);
    return;
  }

  cmd_ptr->cmd.qmi_ptr->id = QMI_MMODE_FW_REQ_CB;
  cmd_ptr->cmd.qmi_ptr->data.qmi_fw_info.req_cb.msg_hdr = *msg_hdr;
  cmd_ptr->cmd.qmi_ptr->data.qmi_fw_info.req_cb.sdu_in  = *sdu;

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_FW_CB, cmd_ptr);

}/* qmi_voice_fw_req_cb */


/*===========================================================================
  FUNCTION QMI_VOICE_FW_REQ_HDLR()

  DESCRIPTION
    Process qmi request from Framework

  PARAMETERS
    msg_hdr: qmi_framework_msg_hdr_type ptr
    sdu: dsm_item_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voicei_fw_req_hdlr
(
  qmi_framework_msg_hdr_type* msg_hdr,
  dsm_item_type * sdu_in 
)
{
  qmi_voicei_client_state_type * cl_sp;
  
/*-------------------------------------------------------------------------*/

  ASSERT(msg_hdr);
  ASSERT(sdu_in);

  QM_MSG_MED_3("Process QMI VOICE svc Request handlr clid %d tx_id %d ctl_flag %d", msg_hdr->common_hdr.client_id, msg_hdr->common_hdr.transaction_id, msg_hdr->msg_ctl_flag);

  if(msg_hdr->common_hdr.client_id > VOICEI_MAX_CLIDS)
  {
    QM_MSG_ERROR_1("Cannot process more than %d clients in qmi-Voice", VOICEI_MAX_CLIDS);
    return;
  }

  /*-------------------------------------------------------------------------
    Get a service state and client state pointer
  -------------------------------------------------------------------------*/
  if( msg_hdr->common_hdr.client_id > 0 )
  {
    cl_sp = (qmi_voicei_client_state_type *) 
               qmi_voice_state.client[msg_hdr->common_hdr.client_id - 1];
 
    ASSERT(cl_sp);

   /*-------------------------------------------------------------------------
     Invoke the common svc request handler
    -------------------------------------------------------------------------*/
   qmi_mmode_svc_req_hdlr(&qmi_voicei_cfg.cmn_svc_cfg, msg_hdr, &cl_sp->common, 
                          sdu_in);
  }
  else
  {
    QM_MSG_ERROR("Received Client id is non positive"); 
    return;
  }
}/* qmi_voicei_fw_req_hdlr */


/*===========================================================================
  FUNCTION QMI_VOICE_HANDLE_FW_CMD()

  DESCRIPTION
    Process QMI-Voice cmds posted from Framework callbacks

  PARAMETERS
    cmd: qmi_mmode_cmd_enum_type
    user_data_ptr: void ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voice_handle_fw_cmd
(
  qmi_mmode_cmd_enum_type cmd,
  void *            user_data_ptr
)
{
  qmi_mmode_cmd_data_buf_type* data_ptr;
  qmi_mmode_common_cmd_type *qmi_info;

  ASSERT (user_data_ptr);

  data_ptr = (qmi_mmode_cmd_data_buf_type *) user_data_ptr;
  ASSERT(data_ptr->cmd.qmi_ptr);
  ASSERT (data_ptr->cmd.qmi_ptr->id < QMI_VOICE_CM_IF_CMD_MAX);
  qmi_info = data_ptr->cmd.qmi_ptr;

  switch(qmi_info->id)
  {
    case QMI_MMODE_FW_INIT_CB:
      qmi_voicei_fw_init_cback_hdlr(qmi_info->data.qmi_fw_info.init_cb.num_instances);
      break;
    case QMI_MMODE_FW_ALLOC_CLID_CB:
      qmi_voicei_fw_alloc_clid_hdlr(&qmi_info->data.qmi_fw_info.alloc_clid.msg_hdr);
      break;
    case QMI_MMODE_FW_DEALLOC_CLID_CB:
      qmi_voicei_fw_dealloc_clid_hdlr(&qmi_info->data.qmi_fw_info.dealloc_clid.msg_hdr);
      break;
    case QMI_MMODE_FW_REQ_CB:
      qmi_voicei_fw_req_hdlr(&qmi_info->data.qmi_fw_info.req_cb.msg_hdr,
                               qmi_info->data.qmi_fw_info.req_cb.sdu_in);
      break;
    default:
      QM_MSG_ERROR("Unsupported qmi-voice fw cmd");
      break;
  }

  QMI_VOICE_CM_IF_MEM_FREE(qmi_info);

}/* qmi_voice_handle_fw_cmd */


/*===========================================================================
  FUNCTION QMI_VOICEI_CMD_VALIDATE()

  DESCRIPTION
    
    
  PARAMETERS
    cl_sp     : client state pointer

  RETURN VALUE
    qmi_error_e_type
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_voicei_cmd_validate
(
  qmi_mmode_cmd_data_buf_type     *cmd_ptr,
  void *            cl_sp,
  qmi_voicei_client_state_type   **client_sp_p
)
{
  qmi_common_client_state_type *cmn_cl_sp;

  //qmi_voicei_cmd_validate()

  if(cmd_ptr == NULL)
  {
    QM_MSG_ERROR("Out of QMI_MMODE cmd buf");
    return QMI_ERR_INTERNAL;
  }

  cmn_cl_sp = (qmi_common_client_state_type *)cl_sp;
  *client_sp_p = (qmi_voicei_client_state_type *) qmi_voicei_get_client_sp_by_clid(cmn_cl_sp->clid);
  if(*client_sp_p == NULL)
  {
    QM_MSG_ERROR_1("Invalid client with id %d", cmn_cl_sp->clid);
    return QMI_ERR_INVALID_CLIENT_ID;
  } 

  cmd_ptr->cmd.cm_if_data_ptr = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));
  if(cmd_ptr->cmd.cm_if_data_ptr == NULL)
  {
	  qmi_voice_mem_fatal();
    qmi_mmode_free_cmd_data_buf(cmd_ptr);
    return QMI_ERR_INTERNAL;
  }
 
  return QMI_ERR_NONE;
} /* qmi_voicei_cmd_validate */


/*===========================================================================
  FUNCTION QMI_VOICEI_SEND_RESULT()

  DESCRIPTION
    

  PARAMETERS
    cl_sp: client state pointer

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_send_result
(
  qmi_error_e_type               errval,
  qmi_mmode_cmd_data_buf_type   *cmd_ptr
)
{
  boolean             retval;
  qmi_result_e_type   result   = (errval? QMI_RESULT_FAILURE : QMI_RESULT_SUCCESS);
  dsm_item_type      *response = NULL;

  if(cmd_ptr != NULL)
  {
    if(cmd_ptr->cmd.cm_if_data_ptr != NULL)
    {
      QMI_VOICE_CM_IF_MEM_FREE(cmd_ptr->cmd.cm_if_data_ptr);
    }
    qmi_mmode_free_cmd_data_buf(cmd_ptr);
  }
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_voicei_send_result */



/*=====================QMI-VOICE CMD DECODE & ENCODE APIS==================*/

/*===========================================================================
  FUNCTION QMI_VOICEI_DIAL_CALL()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_dial_call
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_DIAL_CALL_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.dial_call),
                                         (uint32_t) sizeof(voice_dial_call_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_DIAL_CALL;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_dial_call() */


/*===========================================================================
  FUNCTION QMI_VOICEI_DIAL_CALL_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_dial_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.dial_call_resp),
                                    (uint32_t) sizeof(voice_dial_call_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_dial_call_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_END_CALL()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_end_call
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  /* Deactivate the retransmission timer if started */
  qm_timer_stop(QM_TIMER_ID_INCOM_EVT_RESEND);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_END_CALL_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.end_call),
                                         (uint32_t) sizeof(voice_end_call_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_END_CALL;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
}/* qmi_voicei_end_call() */


/*===========================================================================
  FUNCTION QMI_VOICEI_END_CALL_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_end_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.end_call_resp),
                                    (uint32_t) sizeof(voice_end_call_resp_msg_v02),
                                    &response )
    )
  {
   qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_end_call_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_ANSWER_CALL()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_answer_call
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  /* Deactivate the retransmission timer if started */
  qm_timer_stop(QM_TIMER_ID_INCOM_EVT_RESEND);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ANSWER_CALL_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.answer_call),
                                         (uint32_t) sizeof(voice_answer_call_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_ANSWER_CALL;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_answer_call */

/*===========================================================================
  FUNCTION QMI_VOICEI_SETUP_ANSWER()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_setup_answer
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_SETUP_ANSWER_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.setup_answer),
                                         (uint32_t) sizeof(voice_setup_answer_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SETUP_ANSWER;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_setup_answer() */

/*===========================================================================
  FUNCTION QMI_VOICEI_ANSWER_CALL_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_answer_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
  {
    qmi_cmd_buf_type *      cmd_buf_p;
    dsm_item_type *         response;
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
    cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;
  
    if(cmd_buf_p == NULL)
    {
      QM_MSG_HIGH("cmd_buf_p is NULL");
      return;
    }
  
    response = NULL;
    if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                      QMI_IDL_RESPONSE,
                                      (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                      (void *) &(data_ptr->data.answer_call_resp),
                                      (uint32_t) sizeof(voice_answer_call_resp_msg_v02),
                                      &response )
      )
    {
      qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
    }
  }


/*===========================================================================
  FUNCTION QMI_VOICEI_SETUP_ANSWER_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_setup_answer_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
  {
    qmi_cmd_buf_type *      cmd_buf_p;
    dsm_item_type *         response;
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  
    cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;
  
    if(cmd_buf_p == NULL)
    {
      QM_MSG_HIGH("cmd_buf_p is NULL");
      return;
    }
  
    response = NULL;
    if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                      QMI_IDL_RESPONSE,
                                      (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                      (void *) &(data_ptr->data.setup_answer_resp),
                                      (uint32_t) sizeof(voice_setup_answer_resp_msg_v02),
                                      &response )
      )
    {
      qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
    }
  }




/*===========================================================================
  FUNCTION QMI_VOICEI_SEND_FLASH()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_send_flash
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_SEND_FLASH_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.send_flash),
                                         (uint32_t) sizeof(voice_send_flash_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SEND_FLASH;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_send_flash() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SEND_FLASH_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_send_flash_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.send_flash_resp),
                                    (uint32_t) sizeof(voice_send_flash_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
} /* qmi_voicei_send_flash_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_GET_CALL_INFO()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_get_call_info
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CALL_INFO_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_call_info),
                                         (uint32_t) sizeof(voice_get_call_info_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_GET_CALL_INFO;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_get_call_info() */

/*===========================================================================
  FUNCTION QMI_VOICEI_GET_CALL_INFO_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_get_call_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;

  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_call_info_resp),
                                    (uint32_t) sizeof(voice_get_call_info_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
} /* qmi_voicei_get_call_info_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SET_PREFERRED_PRIVACY()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_set_preferred_privacy
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_SET_PREFERRED_PRIVACY_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.set_pref_privacy),
                                         (uint32_t) sizeof(voice_set_preferred_privacy_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SET_PREFERRED_PRIVACY;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_set_preferred_privacy() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SET_PREF_PRIVACY_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_set_pref_privacy_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.set_pref_privacy_resp),
                                    (uint32_t) sizeof(voice_set_preferred_privacy_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response) ;
  }
} /* qmi_voicei_set_pref_privacy_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_INDICATION_REGISTER()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_indication_register
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_voicei_client_state_type * client_sp;
  qmi_common_client_state_type *cmn_cl_sp;

  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  uint8              type;
  uint16             len;
  uint16             expected_len;
  void *             value;
  boolean            got_priv_event=FALSE, got_dtmf_event=FALSE;
  boolean            got_sups_event=FALSE,got_call_event=FALSE, got_handover_event=FALSE;  
  boolean            got_speech_event=FALSE,got_ussd_notification_event=FALSE, got_modification_event=FALSE;  
  boolean            got_uus_event = FALSE, got_aoc_event=FALSE, got_conf_event=FALSE,got_ext_brst_intl_event = FALSE, got_page_miss_event_reg = FALSE;
  boolean            got_conf_parts_event_reg = FALSE;
  boolean            got_cc_result_event_reg = FALSE;
  boolean            got_tty_info_event = FALSE;
  boolean            got_audio_rat_change_event = FALSE;
  boolean            got_e911_orig_fail_event = FALSE;
  boolean            got_add_call_info_event = FALSE;
  boolean	     got_vice_dialog_info_event = FALSE;
  uint8              priv_event, dtmf_event, sups_event, call_event, handover_event;
  uint8              speech_event, ussd_notification_event, modification_event, uus_event, aoc_event, conf_event,page_miss_event;
  uint8              ext_brst_intl_event,conf_parts_event,cc_result_event,tty_info_event, audio_rat_change_event, e911_orig_fail_event,add_call_info_event;
  uint8		     vice_dialog_info_event;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //QMI VOICE qmi_voicei_indication_register

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  response          = NULL;
  errval            = QMI_ERR_NONE;
  value             = NULL;
  got_priv_event    = FALSE;
  got_dtmf_event    = FALSE;
  got_sups_event    = FALSE;
  got_call_event    = FALSE;
  got_conf_event    = FALSE;

  cmn_cl_sp = (qmi_common_client_state_type *)cl_sp;
  client_sp = (qmi_voicei_client_state_type *) qmi_voicei_get_client_sp_by_clid(cmn_cl_sp->clid);

  if(client_sp != NULL)
  {
    priv_event = client_sp->voice_info.reg_voice_privacy_events;
    dtmf_event = client_sp->voice_info.reg_dtmf_events;
    sups_event = client_sp->voice_info.reg_sups_events;
    call_event = client_sp->voice_info.reg_call_events;
    handover_event = client_sp->voice_info.reg_handover_events;
    speech_event = client_sp->voice_info.reg_speech_events;
    ussd_notification_event = client_sp->voice_info.reg_ussd_notification_events;
    modification_event = client_sp->voice_info.reg_modification_events;
    uus_event = client_sp->voice_info.reg_uus_events;
    aoc_event = client_sp->voice_info.reg_aoc_events;
    conf_event = client_sp->voice_info.reg_conference_events;
    ext_brst_intl_event = client_sp->voice_info.reg_brst_intl_events;
    conf_parts_event = client_sp->voice_info.reg_conf_participants_events;
    page_miss_event = client_sp->voice_info.reg_mt_page_miss_events;
    cc_result_event = client_sp->voice_info.reg_cc_result_events;
    tty_info_event = client_sp->voice_info.reg_tty_info_events;
    audio_rat_change_event = client_sp->voice_info.reg_audio_rat_change_events;
    e911_orig_fail_event = client_sp->voice_info.reg_e911_orig_fail_events;
    add_call_info_event = client_sp->voice_info.reg_add_call_info_events;
    vice_dialog_info_event  = client_sp->voice_info.reg_vice_dialog_info_events;
  }
  else
  {
    QM_MSG_ERROR_1("Invalid client with id %d", cmn_cl_sp->clid);
    errval = QMI_ERR_INVALID_CLIENT_ID;
    goto send_result;
  }

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
	QM_MSG_MED_1("type received: %d", type);
    switch (type)
    {
      case VOICEI_IND_REG_VOICE_PRIV:
        expected_len = sizeof(priv_event);
        value = &priv_event;
        got_priv_event = TRUE;
        break;
      case VOICEI_IND_REG_DTMF:
        expected_len = sizeof(dtmf_event);
        value = &dtmf_event;
        got_dtmf_event = TRUE;
        break;
      case VOICEI_IND_REG_SUPS_NOTIFY:
        expected_len = sizeof(sups_event);
        value = &sups_event;
        got_sups_event = TRUE;
        break;
      case VOICEI_IND_REG_CALL_EVENTS_NOTIFY:
        expected_len = sizeof(call_event);
        value = &call_event;
        got_call_event = TRUE;
        break;

      case VOICEI_IND_REG_HANDOVER_NOTIFY:
        expected_len = sizeof(handover_event);
        value = &handover_event;
        got_handover_event = TRUE;
        break;

      case VOICEI_IND_REG_SPEECH_NOTIFY:
        expected_len = sizeof(speech_event);
        value = &speech_event;
        got_speech_event = TRUE;
        break;

      case VOICEI_IND_REG_USSD_NOTIFY:
        expected_len = sizeof(ussd_notification_event);
        value = &ussd_notification_event;
        got_ussd_notification_event = TRUE;
        break;

      case VOICEI_IND_REG_MODIFICATION_NOTIFY:
        expected_len = sizeof(modification_event);
        value = &modification_event;
        got_modification_event = TRUE;
        break;

      case VOICEI_IND_REG_UUS_NOTIFY:
        expected_len = sizeof(uus_event);
        value = &uus_event;
        got_uus_event = TRUE;
        break;

      case VOICEI_IND_REG_AOC_NOTIFY:
        expected_len = sizeof(aoc_event);
        value = &aoc_event;
        got_aoc_event = TRUE;
        break;

      case VOICEI_IND_REG_CONF_NOTIFY:
        expected_len = sizeof(conf_event);
        value = &conf_event;
        got_conf_event = TRUE;
        break;

      case VOICEI_IND_REG_EXT_BRST_INTL_NOTIFY:
        expected_len = sizeof(ext_brst_intl_event);
        value = &ext_brst_intl_event;
        got_ext_brst_intl_event = TRUE;
        break;
     case VOICEI_IND_REG_CONF_PARTS_NOTIFY:
       expected_len = sizeof(conf_parts_event);
       value = &conf_parts_event;
       got_conf_parts_event_reg = TRUE;
       break;
     case VOICEI_IND_REG_MT_PAGE_MISS_NOTIFY:
       expected_len = sizeof(page_miss_event);
       value = &page_miss_event;
       got_page_miss_event_reg = TRUE;
       break;

     case VOICEI_IND_REG_CC_RESULT_NOTIFY:
       expected_len = sizeof(cc_result_event);
       value = &cc_result_event;
       got_cc_result_event_reg = TRUE;
       break;
     case VOICEI_IND_REG_TTY_INFO_NOTIFY:
       expected_len = sizeof(tty_info_event);
       value = &tty_info_event;
       got_tty_info_event = TRUE;
       break;
     case VOICEI_IND_REG_AUDIO_RAT_CHANGE_NOTIFY:
       expected_len = sizeof(audio_rat_change_event);
       value = &audio_rat_change_event;
       got_audio_rat_change_event = TRUE;
       break;
     case VOICEI_IND_REG_E911_ORIG_FAIL_NOTIFY:
       expected_len = sizeof(e911_orig_fail_event);
       value = &e911_orig_fail_event;
       got_e911_orig_fail_event = TRUE;
       break;
     case VOICEI_IND_REG_ADD_CALL_INFO_NOTIFY:
       expected_len = sizeof(add_call_info_event);
       value = &add_call_info_event;
       got_add_call_info_event = TRUE;
       break;  

     case VOICEI_IND_REG_VICE_DIALOG_INFO_NOTIFY:
       expected_len = sizeof(vice_dialog_info_event);
       value = &vice_dialog_info_event;
       got_vice_dialog_info_event = TRUE;
       break;


      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ( (expected_len != 0) && (expected_len != len) )
    {
      QM_MSG_MED_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if ( len != dsm_pullup( sdu_in, value, len ) )
    {                                                                    
      QM_MSG_MED("Invalid length in TLV");
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }                     

  if (got_priv_event||got_dtmf_event||got_sups_event ||got_call_event ||
      got_handover_event||got_speech_event||got_ussd_notification_event||
      got_modification_event||got_uus_event ||got_aoc_event||got_conf_event||got_ext_brst_intl_event || got_conf_parts_event_reg || 
      got_page_miss_event_reg || got_cc_result_event_reg || got_tty_info_event || got_audio_rat_change_event || got_e911_orig_fail_event|| 
      got_add_call_info_event||got_vice_dialog_info_event)
  {
    QM_MSG_MED("Received registration for some events");
  }
  else
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  client_sp->voice_info.reg_voice_privacy_events = priv_event;
  client_sp->voice_info.reg_dtmf_events          = dtmf_event;
  client_sp->voice_info.reg_sups_events          = sups_event;
  client_sp->voice_info.reg_call_events          = call_event;
  client_sp->voice_info.reg_handover_events      = handover_event;
  client_sp->voice_info.reg_speech_events      = speech_event;
  client_sp->voice_info.reg_ussd_notification_events      = ussd_notification_event;
  client_sp->voice_info.reg_modification_events      = modification_event;
  client_sp->voice_info.reg_uus_events      = uus_event;
  client_sp->voice_info.reg_aoc_events                    = aoc_event;
  client_sp->voice_info.reg_conference_events      = conf_event;
  client_sp->voice_info.reg_brst_intl_events   = ext_brst_intl_event;
  client_sp->voice_info.reg_mt_page_miss_events      = page_miss_event;
  client_sp->voice_info.reg_cc_result_events      = cc_result_event;
  client_sp->voice_info.reg_conf_participants_events      = conf_parts_event;
  client_sp->voice_info.reg_tty_info_events      = tty_info_event;
  client_sp->voice_info.reg_audio_rat_change_events      = audio_rat_change_event;
  client_sp->voice_info.reg_e911_orig_fail_events = e911_orig_fail_event;
  client_sp->voice_info.reg_add_call_info_events      = add_call_info_event;
  client_sp->voice_info.reg_vice_dialog_info_events = vice_dialog_info_event;

  QM_MSG_MED_6("voice ind_reg with sups event = %d, call event = %d, handover event = %d,speech event = %d, ussd not event = %d, modifn event = %d", 
                sups_event, call_event, handover_event,speech_event, ussd_notification_event, modification_event);
  QM_MSG_MED_6("voice ind_reg with uus event = %d, aoc event = %d, conf event = %d,ext brst intl event = %d, page miss event = %d, cc result event = %d",
                uus_event, aoc_event, conf_event, ext_brst_intl_event, page_miss_event, cc_result_event);
  QM_MSG_MED_6("voice ind_reg with vice_dialog_info_event =%d,conf parts = %d, tty info event = %d,e911_orig_fail=%d,add call info %d, audio rat change event = %d",
              vice_dialog_info_event, conf_parts_event, tty_info_event,e911_orig_fail_event,
              add_call_info_event, audio_rat_change_event);

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
}

/*===========================================================================
  FUNCTION QMI_VOICEI_BURST_DTMF_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_burst_dtmf_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.burst_dtmf_resp),
                                    (uint32_t) sizeof(voice_burst_dtmf_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_burst_dtmf_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_START_CONT_DTMF_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_start_cont_dtmf_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.start_cont_dtmf_resp),
                                    (uint32_t) sizeof(voice_start_cont_dtmf_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_start_cont_dtmf_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_STOP_CONT_DTMF_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_stop_cont_dtmf_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.stop_cont_dtmf_resp),
                                    (uint32_t) sizeof(voice_stop_cont_dtmf_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_stop_cont_dtmf_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_GET_ALL_CALL_INFO()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_get_all_call_info
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_ALL_CALL_INFO_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_all_call_info),
                                         (uint32_t) sizeof(voice_get_all_call_info_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_GET_ALL_CALL_INFO;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_get_all_call_info() */



/*===========================================================================
  FUNCTION QMI_VOICEI_GET_ALL_CALL_INFO_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_get_all_call_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  qmi_voice_cm_if_all_call_info_s       *all_call_info;
  uint8     i, num_of_instances=0,alpha_len=0;
  int       tot_len=0, uus_len, cnap_len, rp_num_len, tag, diags_len=0, con_len=0;
  int       redirect_len=0,called_len=0;
  uint8     len_SIP_URI=0;
  struct
  {
    uint8      call_id;
    uint8      call_state;
    uint8      call_type;
    uint8      direction;
    uint8      mode;
    uint8      mpty;
    uint8      als;
  }basic_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("Null cmd_buf ptr in qmi_voicei_get_all_call_info_resp");
    return;
  }

  all_call_info = (qmi_voice_cm_if_all_call_info_s*)&data_ptr->data.get_all_call_info_resp.info;

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  response = NULL;
  errval = data_ptr->data.get_all_call_info_resp.error;


  if( QMI_ERR_NONE == errval )
  {

    /* Raw Call End Cause Code Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->raw_call_end_cause_code_valid)
      {
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->raw_call_end_cause_code, sizeof(info->raw_call_end_cause_code)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += sizeof(info->raw_call_end_cause_code) + sizeof(info->call_id);
        QM_MSG_MED_2("Raw End reason = %d for call id %d", info->raw_call_end_cause_code, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_RAW_CALL_END_CAUSE_CODE;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }

    /* Second Alpha Optional TLV */
    num_of_instances = 0;
    tot_len = 0;
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if( (info->is_second_alpha_available == TRUE) && (info->call_state == QMI_VOICE_CM_IF_CALL_STATE_ORIG) )
      {
        alpha_len = sizeof(info->second_alpha_info.alpha_dcs) + \
                          sizeof(info->second_alpha_info.alpha_len) + \
                          info->second_alpha_info.alpha_len;
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->second_alpha_info, alpha_len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += alpha_len + sizeof(info->call_id);
        QM_MSG_MED_3("Second Alpha coding scheme = %d name length =%d for call id %d", info->second_alpha_info.alpha_dcs, info->second_alpha_info.alpha_len, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_SECOND_ALPHA;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }
    
    /* Remote Party Number Ext Optional TLV */
    num_of_instances = 0;
    tot_len = 0;
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
      
      if( info->is_num_available )
      {
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)info->num.buf, info->num.len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->num.len, sizeof(info->num.len)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->num.num_plan, sizeof(info->num.num_plan)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->num.num_type, sizeof(info->num.num_type)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->num.si, sizeof(info->num.si)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->num.pi, sizeof(info->num.pi)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += info->num.len + sizeof(info->num.len) + sizeof(info->num.num_plan) +\
                   sizeof(info->num.num_type) + sizeof(info->num.si) + sizeof(info->num.pi) + sizeof(info->call_id);
        QM_MSG_MED_2("RP Number Ext = %s for call id %d",(const char *)info->num.buf, info->call_id );
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_RP_NUM_EXT;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }

    num_of_instances = 0;
    tot_len = 0;

    /* End Reason Text Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];

      if( (info->is_end_reason_text_available) && 
          (info->end_reason_text.end_reason_text_length < QMI_VOICE_CM_IF_MAX_END_REASON_TEXT_LEN) && 
          (info->end_reason_text.end_reason_text_length != 0) )
      {
        /* end_reason_text_length is the number of UTF16 characters */
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->end_reason_text.end_reason_text_buf, 2*info->end_reason_text.end_reason_text_length) &&
              QMI_SVC_PKT_PUSH(&response, (void*)&info->end_reason_text.end_reason_text_length, sizeof(info->end_reason_text.end_reason_text_length)) &&
              QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))))
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += sizeof(info->end_reason_text.end_reason_text_length) + 2*info->end_reason_text.end_reason_text_length + sizeof(info->call_id);
        QM_MSG_MED_1("end_reason_text added for call id %d", info->call_id);
      }
    }

    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_END_REASON_TEXT;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }

    num_of_instances = 0;
    tot_len = 0;

    /* Call Attribute Status Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];

      if(info->is_call_attrib_status_available)
      {
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->call_attrib_status, sizeof(info->call_attrib_status)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
      
        num_of_instances++;
        tot_len += sizeof(info->call_attrib_status) + sizeof(info->call_id);
        QM_MSG_MED_2("call_attrib_status = %d for call id %d", info->call_attrib_status, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_ATTRIB_STATUS;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }

    num_of_instances = 0;
    tot_len = 0;

    /* Is SRVCC  Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];

      if(info->is_srvcc_call_available)
      {
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->is_srvcc_call, sizeof(info->is_srvcc_call)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += sizeof(info->is_srvcc_call) + sizeof(info->call_id);
      }
      QM_MSG_MED_2("Is SRVCC CALL = %d for call id %d", info->is_srvcc_call, info->call_id);
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_IS_SRVCC_CALL;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }

    num_of_instances = 0;
    tot_len = 0;

    /* Alerting Pattern Optional TLV */
    for( i=0; i<all_call_info->num_of_calls; i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_alerting_pattern_available)
      {
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->alerting_pattern, sizeof(info->alerting_pattern)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += sizeof(info->alerting_pattern) + sizeof(info->call_id);
        QM_MSG_MED_2("Alerting Pattern = %d for call id %d", info->alerting_pattern, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_ALERTING_PATTERN;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }
    
    num_of_instances = 0;
    tot_len = 0;
      

      
    /* Connected number Info Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_conn_num_available)
      {
        con_len = sizeof(info->conn_num_data.pi) + \
                  sizeof(info->conn_num_data.si) + \
                  sizeof(info->conn_num_data.num_type) + \
                  sizeof(info->conn_num_data.num_plan) + \
                  sizeof(info->conn_num_data.len) + \
                  info->conn_num_data.len;
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->conn_num_data, con_len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += con_len + sizeof(info->call_id);
        QM_MSG_MED_4("Connected num info pi= %d,si=%d,num_len=%d, call id = %d",info->conn_num_data.pi,info->conn_num_data.si,
                      info->conn_num_data.len,info->call_id );
      }
    }
        
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_CONNECTED_NUM;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }
        
    num_of_instances = 0;
    tot_len = 0;
      
    /* Called party number Info Optional TLV */
    for( i=0; i<all_call_info->num_of_calls; i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_called_party_num_available)
      {
        called_len = sizeof(info->called_party_num_data.pi) + \
                     sizeof(info->called_party_num_data.si) + \
                     sizeof(info->called_party_num_data.num_type) + \
                     sizeof(info->called_party_num_data.num_plan) + \
                     sizeof(info->called_party_num_data.len) + \
                     info->called_party_num_data.len;
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->called_party_num_data, called_len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += called_len + sizeof(info->call_id);
        QM_MSG_MED_4("Called party num info pi= %d,si=%d,num_len=%d, call id = %d",info->called_party_num_data.pi,
                     info->called_party_num_data.si,info->called_party_num_data.len, info->call_id);
      }
    }
        
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_CALLED_PARTY_NUM;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }

    num_of_instances = 0;
    tot_len = 0;
      
    /* Redirecting party number Info Optional TLV */
    for( i=0; i<all_call_info->num_of_calls; i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_redirecting_party_num_available)
      {
        redirect_len = sizeof(info->redirecting_party_num_data.pi) + \
                       sizeof(info->redirecting_party_num_data.si) + \
                       sizeof(info->redirecting_party_num_data.num_type) + \
                       sizeof(info->redirecting_party_num_data.num_plan) + \
                       sizeof(info->redirecting_party_num_data.len) + \
                       info->redirecting_party_num_data.len;
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->redirecting_party_num_data, redirect_len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += redirect_len + sizeof(info->call_id);
        QM_MSG_MED_4("Redirect party num info pi= %d,si=%d,num_len=%d call id = %d",info->redirecting_party_num_data.pi,
                     info->redirecting_party_num_data.si,info->redirecting_party_num_data.len,info->call_id);
      }
    }
        
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_REDIRECTING_PARTY_NUM;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }
        
    num_of_instances = 0;
    tot_len = 0;
      
    /* Call End Diagnostics Info Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
      
      if(info->is_diagnostics_info_available)
      {
        diags_len = sizeof(info->diagnostic_data.diagnostic_length) + \
                    info->diagnostic_data.diagnostic_length ;
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->diagnostic_data, diags_len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += diags_len + sizeof(info->call_id);
        QM_MSG_MED_2("Diagnostic info len=%d for call id %d", info->diagnostic_data.diagnostic_length, info->call_id);
      }
    }
        
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_DIAGNOSTICS;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }

    tot_len = 0;
    num_of_instances = 0;

    /* VS Variant Attributes Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_vs_call_variant_valid)
      {
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->vs_call_variant, sizeof(info->vs_call_variant)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += sizeof(info->vs_call_variant) + sizeof(info->call_id);
        QM_MSG_MED_2("VS variant Attributes = %d for call id %d", info->vs_call_variant, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_VS_VARIANT;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }

    tot_len = 0;
    num_of_instances = 0;

    /* Audio Attributes Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_audio_attrib_available)
      {
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->audio_attrib, sizeof(info->audio_attrib)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += sizeof(info->audio_attrib) + sizeof(info->call_id);
        QM_MSG_MED_2("Audio Attributes = %d for call id %d", info->audio_attrib, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_AUDIO_ATTRIBUTE;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }

    tot_len = 0;
    num_of_instances = 0;

    /* Video Attributes Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_video_attrib_available)
      {
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->video_attrib, sizeof(info->video_attrib)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += sizeof(info->video_attrib) + sizeof(info->call_id);
        QM_MSG_MED_2("Video Attributes = %d for call id %d", info->video_attrib, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_VIDEO_ATTRIBUTE;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }    

    num_of_instances = 0;
    tot_len = 0;

    /* Alpha Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_alpha_available)
      {
        alpha_len = sizeof(info->alpha_info.alpha_dcs) + \
                          sizeof(info->alpha_info.alpha_len) + \
                          info->alpha_info.alpha_len;
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->alpha_info, alpha_len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += alpha_len + sizeof(info->call_id);
        QM_MSG_MED_3("Alpha coding scheme = %d name length =%d for call id %d", info->alpha_info.alpha_dcs, info->alpha_info.alpha_len, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_ALPHA;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }

    tot_len = 0;
    num_of_instances = 0;

    /* Call End Reason Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_end_reason_available)
      {
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->end_reason, sizeof(info->end_reason)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += sizeof(info->end_reason) + sizeof(info->call_id);
        QM_MSG_MED_2("End reason = %d for call id %d", info->end_reason, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_END_REASON;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }
    
    num_of_instances = 0;
    tot_len = 0;
    i = 0;
    
    /* Voice Privacy Optional TLV */
    /* For CDMA only one call at a time */
    if(all_call_info->num_of_calls == 1)
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_voice_privacy_available)
      {
        tag = VOICEI_GET_ALL_CALL_VP;
        tot_len = sizeof(info->voice_privacy);
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->voice_privacy, sizeof(info->voice_privacy))  &&
                QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ))
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
        }
        QM_MSG_MED_2("Voice Privacy = %d for call id %d", info->voice_privacy, info->call_id);
      }
    }
    
    num_of_instances = 0;
    tot_len = 0;
    i = 0;
    
    /* Otasp Status Optional TLV */
    /* For CDMA only one call at a time */
    if(all_call_info->num_of_calls == 1)
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
      if(info->is_otasp_status_available)
      {
        tag = VOICEI_GET_ALL_CALL_OTASP;
        tot_len = sizeof(info->otasp_status);
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->otasp_status, sizeof(info->otasp_status)) &&
                QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ))
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
        }
        QM_MSG_MED_2("Otasp Status = %d for call id %d", info->otasp_status, info->call_id);
      }
    }
    
    num_of_instances = 0;
    tot_len = 0;
    
    /* Service Opt Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS) ; i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_srv_opt_available)
      {
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->srv_opt, sizeof(info->srv_opt)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += sizeof(info->srv_opt) + sizeof(info->call_id);
        QM_MSG_MED_2("Service option = %d for call id %d", info->srv_opt, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_SO;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }
    
    num_of_instances = 0;
    tot_len = 0;
    
    /* UUS Info Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_uus_info_available)
      {
        uus_len = sizeof(info->uus_info.uus_type) + \
                  sizeof(info->uus_info.uus_dcs) + \
                  sizeof(info->uus_info.uus_data_len) + \
                  info->uus_info.uus_data_len;
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->uus_info, uus_len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += uus_len + sizeof(info->call_id);
        QM_MSG_MED_3("UUS Type = %d Dcs =%d for call id %d", info->uus_info.uus_type, info->uus_info.uus_dcs, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_UUS_INFO;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }
    
    num_of_instances = 0;
    tot_len = 0;
    
    /* Alerting Type Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_alerting_type_available)
      {
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->alerting_type, sizeof(info->alerting_type)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += sizeof(info->alerting_type) + sizeof(info->call_id);
        QM_MSG_MED_2("Alerting type = %d for call id %d", info->alerting_type, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_ALERTING;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }
    
    num_of_instances = 0;
    tot_len = 0;
    
    /* Cnap Info Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_cnap_info_available)
      {
        cnap_len = sizeof(info->cnap_info.name_presentation) + \
                   sizeof(info->cnap_info.coding_scheme) + \
                   sizeof(info->cnap_info.name_len) + \
                   info->cnap_info.name_len;
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->cnap_info, cnap_len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += cnap_len + sizeof(info->call_id);
        QM_MSG_MED_3("Cnap coding scheme = %d name length =%d for call id %d", info->cnap_info.coding_scheme, info->cnap_info.name_len, info->call_id);
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_RP_NAME;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }
    
    num_of_instances = 0;
    tot_len = 0;
    
    /* Remote Party Number Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
      if(info->is_num_available && (info->num.len<=CM_MAX_CALLED_INFO_CHARS))
      {
        rp_num_len = sizeof(info->num.pi)+ sizeof(info->num.len) + info->num.len;
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)&info->num.buf, info->num.len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->num.len, sizeof(info->num.len)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->num.pi, sizeof(info->num.pi)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += rp_num_len + sizeof(info->call_id);
        QM_MSG_MED_2("Rp Number = %s for call id %d",(const char *)info->num.buf, info->call_id );
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_RP_NUM;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }
    
    num_of_instances = 0;
    tot_len = 0;
    
    /* SIP URI Optional TLV */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
  
      if(info->is_num_available && (info->num.len>CM_MAX_CALLED_INFO_CHARS))
      {
        rp_num_len = sizeof(info->num.len) + info->num.len;
        len_SIP_URI= info->num.len;
        if( !(QMI_SVC_PKT_PUSH(&response, (void*)info->num.buf, info->num.len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->num.len, sizeof(len_SIP_URI)) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&info->call_id, sizeof(info->call_id))) )
        {
          num_of_instances = 0;
          dsm_free_packet(&response);
          break;
        }
        num_of_instances++;
        tot_len += rp_num_len + sizeof(info->call_id);
        QM_MSG_MED_2("SIP URI = %s for call id %d",(const char *)info->num.buf, info->call_id );
      }
    }
    
    if( num_of_instances > 0 )
    {
      tot_len += sizeof(num_of_instances);
      tag = VOICEI_GET_ALL_CALL_SIP_URI;
      /* Put the num_of_instances, total length, Tag id */
      if(!(QMI_SVC_PKT_PUSH(&response, (void *)&num_of_instances, sizeof(num_of_instances)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
           ) )
      {
        dsm_free_packet(&response);
      }
    }    

    tot_len = 0;
    
    /* Fill Mandatory basic call information */
    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
      /* Fill the basic call info */
      basic_info.call_id = info->call_id;
      basic_info.call_state = info->call_state;
      basic_info.call_type = info->call_type;
      basic_info.direction = info->direction;
      basic_info.mode = info->mode;
      basic_info.mpty = info->is_mpty;
      basic_info.als = info->als;
      if( !QMI_SVC_PKT_PUSH(&response, &basic_info, sizeof(basic_info)) )
      {
        dsm_free_packet(&response);
        errval = QMI_ERR_NO_MEMORY;
        goto send_result;
      }
      tot_len += sizeof(basic_info);
      QM_MSG_MED_7("call id %d, Call State =%d, call type =%d, direction %d, Call Mode =%d, is mpty =%d, als =%d", info->call_id,info->call_state,
      info->call_type, info->direction, info->mode, info->is_mpty, info->als);
    }
    
    tot_len += sizeof(all_call_info->num_of_calls);
    /* For get all call info response */
    tag = VOICEI_GET_ALL_CALL_INFO;

    /* Put the num_of_calls, total length, Tag id */
    if(!(QMI_SVC_PKT_PUSH(&response, (void *)&all_call_info->num_of_calls, 
                                                sizeof(all_call_info->num_of_calls)) &&
         QMI_SVC_PKT_PUSH(&response, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
         QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)
         ) )
    {
      dsm_free_packet(&response);
      errval = QMI_ERR_NO_MEMORY;
    }
  }

send_result:
  if (!qmi_voicei_send_response(errval, (void*)cmd_buf_p, response))
  {
    QM_MSG_MED("Unable to send response for QMI VOICE GET ALL CALL INFO operation");
  }
} /* qmi_voicei_get_all_call_info_resp() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_MANAGE_CALLS_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    None 
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_manage_calls_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.manage_calls_resp),
                                    (uint32_t) sizeof(voice_manage_calls_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_sups_manage_calls_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CLIP_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_clip_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_clip_resp),
                                    (uint32_t) sizeof(voice_get_clip_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_sups_get_clip_resp*/


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_COLP_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_colp_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_colp_resp),
                                    (uint32_t) sizeof(voice_get_colp_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_sups_get_colp_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_WAITING_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_call_waiting_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_call_waiting_resp),
                                    (uint32_t) sizeof(voice_get_call_waiting_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_sups_get_call_waiting_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_BARRING_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    service class for the specified reason/facility for which call barring is active 
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_call_barring_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  uint16                   total_alpha_len = 0;
  uint16                  total_barr_list_tlv_size = 0;
  uint8                   barr_sc_count=0, barr_num_count=0;
  uint8 tag=0;
  uint8 i=0, j=0;
  struct
  {
    uint8 service_class;
  }call_barring_resp;

  struct
  {
    uint32 service_class_ext;
  }call_barring_ext_resp;

  struct
  {
    uint16 failure_cause;
  }get_call_barring_err_resp;
  struct
  {
    uint8 srv_type;
    uint8 reason;
  }sups_cc_data;
  qmi_voice_cm_if_sups_cc_result_s cc_res;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("Null cmd_buf ptr in qmi_voicei_sups_get_call_barring_resp");
    return;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  response = NULL;
  errval = data_ptr->data.get_call_barring_resp.error;

  /* Fill Retry Duration info if no error is present */
  if( data_ptr->data.get_call_barring_resp.is_retry_duration_available == TRUE )
  {
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_RETRY_DURATION,
                                      sizeof(data_ptr->data.get_call_barring_resp.retry_duration),
                                      (void *) &data_ptr->data.get_call_barring_resp.retry_duration))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

  if( QMI_ERR_NONE == errval )
  {
    call_barring_resp.service_class = data_ptr->data.get_call_barring_resp.service_class;
    QM_MSG_MED_1("Sending QMI GET Call barring with service_class = %d", call_barring_resp.service_class);
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_SC,
                                      sizeof(call_barring_resp),
                                      (void *) &call_barring_resp))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

  if( QMI_ERR_NONE == errval )
  {
    call_barring_ext_resp.service_class_ext = data_ptr->data.get_call_barring_resp.service_class_ext;
    QM_MSG_MED_1("Sending QMI GET Call barring with service_class_ext = %d", call_barring_ext_resp.service_class_ext);
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_RESP_SC_EXT,
                                      sizeof(call_barring_ext_resp),
                                      (void *) &call_barring_ext_resp))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

  if( QMI_ERR_SUPS_FAILURE_CAUSE == errval )
  {
    get_call_barring_err_resp.failure_cause = data_ptr->data.get_call_barring_resp.sups_failure_cause;
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_FAILURE_CAUSE,
                                      sizeof(get_call_barring_err_resp),
                                      (void *) &get_call_barring_err_resp))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }
  else if( data_ptr->data.get_call_barring_resp.is_cc_modified )
  {
    cc_res = data_ptr->data.get_call_barring_resp.cc_result;
    if( cc_res.is_mod_to_sups )
    {
      sups_cc_data.srv_type = cc_res.service;
      sups_cc_data.reason = cc_res.reason;
      if(FALSE == qmi_svc_put_param_tlv(&response,
                                        VOICEI_GET_CB_CC_RESULT_SUPS,
                                        sizeof(sups_cc_data),
                                        (void *) &sups_cc_data))
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
      }
    }
    else if( cc_res.is_mod_to_voice )
    {
      if(FALSE == qmi_svc_put_param_tlv(&response,
                                        VOICEI_GET_CB_CC_RESULT_CALL_ID,
                                        sizeof(cc_res.call_id),
                                        (void *) &cc_res.call_id))
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
      }
    }
    else
    {
      QM_MSG_ERROR("Invalid call control data in response");
    }

    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_CC_RESULT_TYPE,
                                      sizeof(data_ptr->data.get_call_barring_resp.cc_mod_type),
                                      (void *) &(data_ptr->data.get_call_barring_resp.cc_mod_type)))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }

  }

  /* Fill alpha which resulted from SIM call control */
  if(data_ptr->data.get_call_barring_resp.is_alpha_available)
  {
    total_alpha_len = sizeof(data_ptr->data.get_call_barring_resp.alpha_id.alpha_dcs) + \
                      sizeof(data_ptr->data.get_call_barring_resp.alpha_id.alpha_len) + \
                      data_ptr->data.get_call_barring_resp.alpha_id.alpha_len;
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_ALPHA_ID,
                                      total_alpha_len,
                                      (void *) &(data_ptr->data.get_call_barring_resp.alpha_id)))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }
  
  //Fill TLV 0x17: Barred Number List
  if(data_ptr->data.get_call_barring_resp.sc_barr_num_list_valid)
  {
    tag = VOICEI_GET_CB_RESP_BARR_LIST;

    barr_sc_count = data_ptr->data.get_call_barring_resp.sc_barr_num_list_len;

    for(i=0;i<barr_sc_count;i++)
    {
      const qmi_voice_cm_if_icb_resp_s *barred_num = &data_ptr->data.get_call_barring_resp.sc_barr_num_list_info[i];
      barr_num_count = barred_num->barr_num_list_len;

      for(j=0;j<barr_num_count && j<QMI_VOICE_CM_IF_MAX_BARR_LIST_LEN;j++) 
      {
        const qmi_voice_cm_if_icb_sc_s *barred_num_sc = &barred_num->barr_num_list_info[j];
        if( !(QMI_SVC_PKT_PUSH(&response, (void *)barred_num_sc->barr_num, barred_num_sc->barr_num_len) &&
              QMI_SVC_PKT_PUSH(&response, (void *)&barred_num_sc->barr_num_len, sizeof(barred_num_sc->barr_num_len)) &&
              QMI_SVC_PKT_PUSH(&response, (void *)&barred_num_sc->barr_num_status, sizeof(barred_num_sc->barr_num_status))) )
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
        }  
        total_barr_list_tlv_size = total_barr_list_tlv_size+\
                                   sizeof(barred_num_sc->barr_num_status)+\
                                   sizeof(barred_num_sc->barr_num_len)+\
                                   barred_num_sc->barr_num_len;
      }
      if(barred_num->barr_num_list_info)
      {
        QMI_VOICE_CM_IF_MEM_FREE((void *)barred_num->barr_num_list_info);
      }
    
      if( !(QMI_SVC_PKT_PUSH(&response, (void*)&barr_num_count, sizeof(barr_num_count)) &&
            QMI_SVC_PKT_PUSH(&response, (void *)&barred_num->barr_sc_ext, sizeof(barred_num->barr_sc_ext))) )
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
      }
      total_barr_list_tlv_size = total_barr_list_tlv_size+\
                                 sizeof(barred_num->barr_sc_ext)+\
                                 sizeof(barr_num_count);
    }

    total_barr_list_tlv_size += sizeof(barr_sc_count);

    if( !(QMI_SVC_PKT_PUSH(&response, (void *)&barr_sc_count, sizeof(barr_sc_count)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&total_barr_list_tlv_size, VOICEI_TLV_LENGTH_SIZE) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }    
  }
  
  if (!qmi_voicei_send_response(errval, (void*)cmd_buf_p, response))
  {
    QM_MSG_MED("Unable to send response for QMI VOICE GET CALL BARRING operation");
  }
}/* qmi_voicei_sups_get_call_barring_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_SET_CALL_BARRING_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    None 
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_set_call_barring_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.set_call_barring_resp),
                                    (uint32_t) sizeof(voice_set_call_barring_password_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_sups_set_call_barring_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_SET_SUPS_SERVICE_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    None 
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_set_sups_service_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.set_sups_resp),
                                    (uint32_t) sizeof(voice_set_sups_service_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_sups_set_sups_service_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_DTMF_IND()

  DESCRIPTION
    
    
  PARAMETERS
    sp      : service provided state pointer (user data)
    clid    : client ID
    sdu_in  : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_dtmf_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_dtmf_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {

      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_DTMF_IND_V02,
                                        (void *) &(data_ptr->data.dtmf_ind),
                                        (uint32_t) sizeof(voice_dtmf_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp, QMI_VOICE_DTMF_IND_V02, ind );
      }
    }
  }
} /* qmi_voicei_dtmf_ind() */

/*===========================================================================
  FUNCTION QMI_VOICEI_HANDOVER_IND()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE

    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_handover_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_handover_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_HANDOVER_IND_V02,
                                        (void *) &(data_ptr->data.handover_ind),
                                        (uint32_t) sizeof(voice_handover_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp, QMI_VOICE_HANDOVER_IND_V02, ind );
      }
    }
  }
} /* qmi_voicei_handover_ind() */

/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_LOW_FUNDS_IND()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE

    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_aoc_low_funds_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_aoc_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_AOC_LOW_FUNDS_IND_V02,
                                        (void *) &(data_ptr->data.low_funds_ind),
                                        (uint32_t) sizeof(voice_aoc_low_funds_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp, QMI_VOICE_AOC_LOW_FUNDS_IND_V02, ind );
      }
    }
  }
} /* qmi_voicei_aoc_low_funds_ind() */


/*===========================================================================
  FUNCTION QMI_VOICEI_EXT_BRST_INTL_IND()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE

    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_ext_brst_intl_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_brst_intl_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_EXT_BRST_INTL_IND_V02,
                                        (void *) &(data_ptr->data.ext_brst_intl_ind),
                                        (uint32_t) sizeof(voice_ext_brst_intl_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp, QMI_VOICE_EXT_BRST_INTL_IND_V02, ind);
      }
    }
  }
} /* qmi_voicei_ext_brst_intl_ind() */

/*===========================================================================
  FUNCTION QMI_VOICEI_CONFERENCE_PARTICIPANTS_INFO_IND()

  DESCRIPTION

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
None

  SIDE EFFECTS
None
===========================================================================*/
static void qmi_voicei_conference_participants_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_conf_participants_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CONFERENCE_PARTICIPANTS_INFO_IND_V02,
                                        (void *) &(data_ptr->data.parsed_conf_info),
                                        (uint32_t) sizeof(voice_conf_participants_info_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_CONFERENCE_PARTICIPANTS_INFO_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_conference_participants_info_ind() */


/*===========================================================================
  FUNCTION QMI_VOICEI_MT_PAGE_MISS_IND()

  DESCRIPTION
	
	
  PARAMETERS

  RETURN VALUE

	
  DEPENDENCIES
	None

  SIDE EFFECTS
	None
===========================================================================*/
static void qmi_voicei_mt_page_miss_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_mt_page_miss_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_MT_PAGE_MISS_IND_V02,
                                        (void *) &(data_ptr->data.mt_page_miss_ind),
                                        (uint32_t) sizeof(voice_mt_page_miss_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp,QMI_VOICE_MT_PAGE_MISS_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_mt_page_miss_ind() */

/*===========================================================================
  FUNCTION QMI_VOICEI_CALL_CONTROL_RESULT_INFO_IND()

  DESCRIPTION

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
None

  SIDE EFFECTS
None
===========================================================================*/
static void qmi_voicei_call_control_result_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_cc_result_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CALL_CONTROL_RESULT_INFO_IND_V02,
                                        (void *) &(data_ptr->data.cc_result_info_ind),
                                        (uint32_t) sizeof(voice_call_control_result_info_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp,QMI_VOICE_CALL_CONTROL_RESULT_INFO_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_call_control_result_info_ind() */



/*===========================================================================
  FUNCTION QMI_VOICEI_ALL_CALL_STATUS_IND()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_all_call_status_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type *               voice_sp;
  qmi_voicei_client_state_type          * cl_sp;
  int                                   j;
  dsm_item_type *                       ind;
  qmi_voice_cm_if_all_call_info_s       *all_call_info;
  uint8     i, num_of_instances=0;
  int       tot_len=0, cnap_len, alpha_len=0,rp_num_len, tag,con_len=0,diags_len=0;
  int       called_len=0,redirect_len=0;
  boolean   is_ind_sent = FALSE;
  enum qm_subs_e subs = QM_SUBS_PRI;
  uint8     is_incoming_call_present = FALSE;
  uint8     conn_num_pi=0;
  uint8     is_adnl_info_present= FALSE;
  struct
  {
    uint8      call_id;
    uint8      call_state;
    uint8      call_type;
    uint8      direction;
    uint8      mode;
    uint8      mpty;
    uint8      als;
  }basic_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  all_call_info = (qmi_voice_cm_if_all_call_info_s*)&data_ptr->data.all_call_status_ind.info;
  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
    {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if((cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
         (cl_sp->voice_info.reg_call_events) &&
         (!cl_sp->asubs_info.is_bind_set_by_client ||
          (cl_sp->asubs_info.as_id == data_ptr->header.as_id)))
      {

        ind = NULL;

        num_of_instances = 0;
        tot_len = 0;
    
        /* Raw Call End Cause code Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->raw_call_end_cause_code_valid)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->raw_call_end_cause_code, sizeof(info->raw_call_end_cause_code)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->raw_call_end_cause_code) + sizeof(info->call_id);
            QM_MSG_MED_2("Raw End reason = %d for call id %d", info->raw_call_end_cause_code, info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_RAW_CALL_END_CAUSE_CODE;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }

        /* End Reason Text Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if( (info->is_end_reason_text_available) && 
              (info->end_reason_text.end_reason_text_length < QMI_VOICE_CM_IF_MAX_END_REASON_TEXT_LEN) && 
              (info->end_reason_text.end_reason_text_length != 0) )
          {
            /* end_reason_text_length is the number of UTF16 characters */
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->end_reason_text.end_reason_text_buf, 2*info->end_reason_text.end_reason_text_length) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&info->end_reason_text.end_reason_text_length, sizeof(info->end_reason_text.end_reason_text_length)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))))
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->end_reason_text.end_reason_text_length) + 2*info->end_reason_text.end_reason_text_length + sizeof(info->call_id);
            QM_MSG_MED_1("end_reason_text added for call id %d", info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_END_REASON_TEXT;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }

        /* Second Alpha Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if( (info->is_second_alpha_available == TRUE) && (info->call_state == QMI_VOICE_CM_IF_CALL_STATE_ORIG) )
          {
            alpha_len = sizeof(info->second_alpha_info.alpha_dcs) + \
                             sizeof(info->second_alpha_info.alpha_len) + \
                             info->second_alpha_info.alpha_len;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->second_alpha_info, alpha_len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += alpha_len + sizeof(info->call_id);
            QM_MSG_MED_3("Second Alpha coding scheme = %d name length =%d for call id %d", info->second_alpha_info.alpha_dcs, info->second_alpha_info.alpha_len, info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_SECOND_ALPHA;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
        
        /* Remote Party number Ext2 Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
          
          if(info->is_num_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)info->num.buf, info->num.len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->num.len, sizeof(info->num.len)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->num.num_plan, sizeof(info->num.num_plan)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->num.num_type, sizeof(info->num.num_type)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->num.si, sizeof(info->num.si)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->num.pi, sizeof(info->num.pi)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += info->num.len + sizeof(info->num.len) + sizeof(info->num.num_plan) +\
                       sizeof(info->num.num_type) + sizeof(info->num.si) + sizeof(info->num.pi) + sizeof(info->call_id);
            QM_MSG_MED_2("Remote Pty Number Ext2 = %s for call id %d",(const char *)info->num.buf, info->call_id );
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_RP_NUM_EXT2;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }


        /* Call attribute status info Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_call_attrib_status_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_attrib_status, sizeof(info->call_attrib_status)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))))
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->call_attrib_status) + sizeof(info->call_id);
            QM_MSG_MED_1("Call attribute status added for call id %d", info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_CALL_ATTRIB_STATUS;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }  


        /* Is Additional Call info Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_additional_call_info_available)
          {
            is_adnl_info_present = (uint8) info->is_additional_call_info_available;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void *)&info->num_indications, sizeof(info->num_indications)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&is_adnl_info_present, sizeof(is_adnl_info_present)) && 
                  QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(is_adnl_info_present) + sizeof(info->num_indications) + sizeof(info->call_id);
            QM_MSG_MED_1("Additional Info Flag added for call id %d", info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_IS_ADDITIONAL_CALL_INFO_PRESENT;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }  


        /* Media ID Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];

          if(info->is_media_id_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->media_id, sizeof(info->media_id)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->media_id) + sizeof(info->call_id);
            QM_MSG_MED_2("Media ID = %d for call id %d", info->media_id, info->call_id);
          }
        }

        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_MEDIA_ID;
          /* Put the num_of_instances, total length, Tag id */
          if( !(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
                QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
          }
        }

                /* Orig fail reason Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_orig_fail_reason_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->orig_fail_reason, sizeof(info->orig_fail_reason)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id)) ))
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->orig_fail_reason) + sizeof(info->call_id);
            QM_MSG_MED_1("Orig fail reason populated for call id %d", info->call_id);
          }
        }
        
        if(num_of_instances > 0)
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_ORIG_FAIL_REASON;
          /* Put the num_of_instances, total length, Tag id */
          if( !(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
              QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
              QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
          }
        }


        /* Local call capabilities info Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_local_cap_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->local_cap.video_restrict_cause, sizeof(info->local_cap.video_restrict_cause)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->local_cap.video_attrib, sizeof(info->local_cap.video_attrib)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->local_cap.audio_restrict_cause, sizeof(info->local_cap.audio_restrict_cause)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->local_cap.audio_attrib, sizeof(info->local_cap.audio_attrib)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))))
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->local_cap.video_restrict_cause)  + sizeof(info->local_cap.video_attrib) +\
                       sizeof(info->local_cap.audio_restrict_cause)  + sizeof(info->local_cap.audio_attrib) + sizeof(info->call_id);
            QM_MSG_MED_1("Local call capabilities added for call id %d", info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_LOCAL_CAP;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }  


        /* Peer call capabilities info Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_peer_cap_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->peer_cap.video_restrict_cause, sizeof(info->peer_cap.video_restrict_cause)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->peer_cap.video_attrib, sizeof(info->peer_cap.video_attrib)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->peer_cap.audio_restrict_cause, sizeof(info->peer_cap.audio_restrict_cause)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->peer_cap.audio_attrib, sizeof(info->peer_cap.audio_attrib)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))))
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->peer_cap.video_restrict_cause)  + sizeof(info->peer_cap.video_attrib) +\
                       sizeof(info->peer_cap.audio_restrict_cause)  + sizeof(info->peer_cap.audio_attrib) + sizeof(info->call_id);
            QM_MSG_MED_1("Peer call capabilities added for call id %d", info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_PEER_CAP;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }  


/* Child number info Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_child_num_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)info->child_num.buf, info->child_num.len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->child_num.len, sizeof(info->child_num.len)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))))
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += info->child_num.len  + sizeof(info->child_num.len) + sizeof(info->call_id);
            QM_MSG_MED_1("Child number added for call id %d", info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_CHILD_NUM;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }  

        /* IP caller name Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_ip_caller_name_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)info->ip_caller_name.name, 2*(info->ip_caller_name.name_len)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->ip_caller_name.name_len, sizeof(info->ip_caller_name.name_len)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))))
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += 2*(info->ip_caller_name.name_len)  + sizeof(info->ip_caller_name.name_len) + sizeof(info->call_id);
            QM_MSG_MED_1("IP caller name added for call id %d", info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_IP_CALLER_NAME;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }  

        /* Caller name PI info Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_srvcc_caller_name_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->name_pi, sizeof(info->name_pi)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))))
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->name_pi) + sizeof(info->call_id);
            QM_MSG_MED_1("Caller name PI added for call id %d", info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_CALLER_NAME_PI;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }

        /* Display Text info Optional TLV */
        num_of_instances = 0;
        tot_len = 0;
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_display_text_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)info->display_text.text, 2*(info->display_text.text_len)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->display_text.text_len, sizeof(info->display_text.text_len)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))))
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += 2*(info->display_text.text_len)  + sizeof(info->display_text.text_len) + sizeof(info->call_id);
            QM_MSG_MED_1("Display text added for call id %d", info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_DISPLAY_TEXT;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }  


        num_of_instances = 0;
        tot_len = 0;

        /* Parent ID info  Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_parent_id_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->is_parent_call_id_cleared, sizeof(info->is_parent_call_id_cleared)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&info->parent_call_id, sizeof(info->parent_call_id)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->is_parent_call_id_cleared) + sizeof(info->parent_call_id) + sizeof(info->call_id);
            QM_MSG_MED_3("Parent_ID = %d, Cleared=%d for call id %d", info->parent_call_id, info->is_parent_call_id_cleared, info->call_id);
          }
        }
          
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_PARENT_CALL_INFO;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }

        num_of_instances = 0;
        tot_len = 0;

        /* Is SRVCC  Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_srvcc_call_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->is_srvcc_call, sizeof(info->is_srvcc_call)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->is_srvcc_call) + sizeof(info->call_id);
          }
          QM_MSG_MED_2("Is SRVCC Call = %d for call id %d", info->is_srvcc_call, info->call_id);
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_IS_SRVCC_CALL;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
    
        num_of_instances = 0;
        tot_len = 0;
/* Alerting Pattern Optional TLV */
        for( i=0; i<all_call_info->num_of_calls; i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
          if(info->is_alerting_pattern_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->alerting_pattern, sizeof(info->alerting_pattern)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->alerting_pattern) + sizeof(info->call_id);
            QM_MSG_MED_2("Alerting pattern = %d for call id %d", info->alerting_pattern, info->call_id);
          }
         }
    
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_ALERTING_PATTERN;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
    
        num_of_instances = 0;
        tot_len = 0;
          

          
        /* Connected number Info Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_conn_num_available)
          {
            con_len = sizeof(info->conn_num_data.pi) + \
                      sizeof(info->conn_num_data.si) + \
                      sizeof(info->conn_num_data.num_type) + \
                      sizeof(info->conn_num_data.num_plan) + \
                      sizeof(info->conn_num_data.len) + \
                      info->conn_num_data.len;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->conn_num_data, con_len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += con_len + sizeof(info->call_id);
	    QM_MSG_MED_4("Connected num info pi= %d,si=%d,num_len=%d,call id = %d",info->conn_num_data.pi,info->conn_num_data.si,info->conn_num_data.len,info->call_id );
          }
        }
            
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_CONNECTED_NUM;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
            


	  num_of_instances = 0;
	  tot_len = 0;
		  
	  /* Is remote ring alert received TLV */
	  for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
	  {
		const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
	  
		if(info->is_called_party_ringing_status_avail)
		{
		  con_len = sizeof(info->is_called_party_ringing);
		  
		  if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->is_called_party_ringing, con_len) &&
			  QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
		  {
			num_of_instances = 0;
			dsm_free_packet(&ind);
			break;
		  }
		  num_of_instances++;
		  tot_len += con_len + sizeof(info->call_id);
		  QM_MSG_MED_2("Is Called party ringing value = %d call id = %d",info->is_called_party_ringing, info->call_id);
       
		}
	  }
		
	  if( num_of_instances > 0 )
	  {
		tot_len += sizeof(num_of_instances);
		tag = VOICEI_ALL_CALL_IND_IS_CALLED_PARTY_RINGING;
		/* Put the num_of_instances, total length, Tag id */
		if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
			 QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
			 QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
			 ) )
		{
		  dsm_free_packet(&ind);
		}
	  }
	  


            
        num_of_instances = 0;
        tot_len = 0;
        
        /* Is Connected Number ECT TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_conn_num_available && info->is_connected_number_ECT)
          {
            con_len = sizeof(info->is_connected_number_ECT);
            
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->is_connected_number_ECT, con_len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += con_len + sizeof(info->call_id);
			QM_MSG_MED_2("Connected num ECT value = %d call id = %d",info->is_connected_number_ECT, info->call_id);
          }
        }
            
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_IS_CONNECTED_NUMBER_ECT;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
            
        num_of_instances = 0;
        tot_len = 0;

		/* Is Secure Call TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_secure_call_available)
          {
            con_len = sizeof(info->is_secure_call);
            
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->is_secure_call, con_len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += con_len + sizeof(info->call_id);
			QM_MSG_MED_2("Is Secure Call value = %d call id = %d",info->is_secure_call, info->call_id);
          }
        }
          
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_IS_SECURE_CALL;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
          
        num_of_instances = 0;
        tot_len = 0;
        /* Called party number Info Optional TLV */
        for( i=0; i<all_call_info->num_of_calls; i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_called_party_num_available)
          {
            called_len = sizeof(info->called_party_num_data.pi) + \
                         sizeof(info->called_party_num_data.si) + \
                         sizeof(info->called_party_num_data.num_type) + \
                         sizeof(info->called_party_num_data.num_plan) + \
                         sizeof(info->called_party_num_data.len) + \
                         info->called_party_num_data.len;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->called_party_num_data, called_len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += called_len + sizeof(info->call_id);
	    QM_MSG_MED_4("Called party num info pi= %d,si=%d,num_len=%d,call id = %d",info->called_party_num_data.pi,info->called_party_num_data.si,info->called_party_num_data.len,info->call_id );

          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_CALLED_PARTY_NUM;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
    
        num_of_instances = 0;
        tot_len = 0;
          
        /* Redirecting party number Info Optional TLV */
        for( i=0; i<all_call_info->num_of_calls; i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
    
          if(info->is_redirecting_party_num_available)
          {
            redirect_len = sizeof(info->redirecting_party_num_data.pi) + \
                           sizeof(info->redirecting_party_num_data.si) + \
                           sizeof(info->redirecting_party_num_data.num_type) + \
                           sizeof(info->redirecting_party_num_data.num_plan) + \
                           sizeof(info->redirecting_party_num_data.len) + \
                           info->redirecting_party_num_data.len;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->redirecting_party_num_data, redirect_len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += redirect_len + sizeof(info->call_id);
	    QM_MSG_MED_4("Redirect party num info pi= %d,si=%d,num_len=%d,call id = %d",info->redirecting_party_num_data.pi,
                         info->redirecting_party_num_data.si,info->redirecting_party_num_data.len,info->call_id  );

          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_REDIRECTING_PARTY_NUM;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }    
    
        num_of_instances = 0;
        tot_len = 0;
      
        /* Call End Diagnostics Info Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
          
          if(info->is_diagnostics_info_available)
          {
            diags_len = sizeof(info->diagnostic_data.diagnostic_length) + \
                        info->diagnostic_data.diagnostic_length ;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->diagnostic_data, diags_len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += diags_len + sizeof(info->call_id);
            QM_MSG_MED_2("Diagnostic info len=%d for call id %d", info->diagnostic_data.diagnostic_length, info->call_id);
          }
        }
            
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_DIAGNOSTICS;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
    
        num_of_instances = 0;
        tot_len = 0;
    
        /* VS Variant info Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_vs_call_variant_valid)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->vs_call_variant, sizeof(info->vs_call_variant)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->vs_call_variant) + sizeof(info->call_id);
            QM_MSG_MED_2("Vs variant = %d for call id %d", info->vs_call_variant, info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_VS_VARIANT;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }


        num_of_instances = 0;
        tot_len = 0;
    
        /* Video Call Attibutes Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_video_attrib_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->video_attrib, sizeof(info->video_attrib)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->video_attrib) + sizeof(info->call_id);
            QM_MSG_MED_2("Video Attrib = %d for call id %d", info->video_attrib, info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_VIDEO_ATTRIB;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }         

        num_of_instances = 0;
        tot_len = 0;
    
        /* Audio Call Attibutes Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_audio_attrib_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->audio_attrib, sizeof(info->audio_attrib)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->audio_attrib) + sizeof(info->call_id);
            QM_MSG_MED_2("Audio Attrib = %d for call id %d", info->audio_attrib, info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_AUDIO_ATTRIB;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }  

        num_of_instances = 0;
        tot_len = 0;
            /* Alpha Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_alpha_available)
          {
            alpha_len = sizeof(info->alpha_info.alpha_dcs) + \
                             sizeof(info->alpha_info.alpha_len) + \
                             info->alpha_info.alpha_len;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->alpha_info, alpha_len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += alpha_len + sizeof(info->call_id);
            QM_MSG_MED_3("Alpha coding scheme = %d name length =%d for call id %d", info->alpha_info.alpha_dcs, info->alpha_info.alpha_len, info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_ALPHA;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
        
        
        num_of_instances = 0;
        tot_len = 0;
    
        /* Call End Reason Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_end_reason_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->end_reason, sizeof(info->end_reason)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->end_reason) + sizeof(info->call_id);
            QM_MSG_MED_2("End reason = %d for call id %d", info->end_reason, info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_END_REASON;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
        
        
        num_of_instances = 0;
        tot_len = 0;
        
        /* Service Opt Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_srv_opt_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->srv_opt, sizeof(info->srv_opt)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->srv_opt) + sizeof(info->call_id);
            QM_MSG_MED_2("Service option = %d for call id %d", info->srv_opt, info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_SO;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
        
        num_of_instances = 0;
        tot_len = 0;
        
        /* Alerting Type Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_alerting_type_available)
          {
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->alerting_type, sizeof(info->alerting_type)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += sizeof(info->alerting_type) + sizeof(info->call_id);
            QM_MSG_MED_2("Alerting type = %d for call id %d", info->alerting_type, info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_ALERTING;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
        
        num_of_instances = 0;
        tot_len = 0;
        
        /* Cnap Info Optional TLV */
        for( i=0;(i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_cnap_info_available)
          {
            cnap_len = sizeof(info->cnap_info.name_presentation) + \
                       sizeof(info->cnap_info.coding_scheme) + \
                       sizeof(info->cnap_info.name_len) + \
                       info->cnap_info.name_len;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->cnap_info, cnap_len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += cnap_len + sizeof(info->call_id);
            QM_MSG_MED_3("Cnap coding scheme = %d name length =%d for call id %d", info->cnap_info.coding_scheme, info->cnap_info.name_len, info->call_id);
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_RP_NAME;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
        
        num_of_instances = 0;
        tot_len = 0;
        
        /* Remote Party Number Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_num_available && (info->num.len<=CM_MAX_CALLED_INFO_CHARS))
          {
            rp_num_len = sizeof(info->num.pi)+ sizeof(info->num.len) + info->num.len;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&info->num.buf, info->num.len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->num.len, sizeof(info->num.len)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->num.pi, sizeof(info->num.pi)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += rp_num_len + sizeof(info->call_id);
            QM_MSG_MED_2("Rp Number = %s for call id %d",(const char *)info->num.buf, info->call_id );
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_RP_NUM;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
        
        num_of_instances = 0;
        tot_len = 0;
        
        /* SIP URI Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_num_available && (info->num.len>CM_MAX_CALLED_INFO_CHARS))
          {
            rp_num_len = sizeof(info->num.len) + info->num.len;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)info->num.buf, info->num.len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->num.len, sizeof(uint8)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += rp_num_len + sizeof(info->call_id);
            QM_MSG_MED_2("SIP URI = %s for call id %d",(const char *)info->num.buf, info->call_id );
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_SIP_URI;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }
        
        num_of_instances = 0;
        tot_len = 0;      
        
        /* Ext Remote Party number  info (IP) Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_num_available && (info->num.len>CM_MAX_CALLED_INFO_CHARS))
          {            
            rp_num_len = sizeof(info->num.len) + info->num.len;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)info->num.buf, info->num.len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->num.len, sizeof(info->num.len)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->num.pi, sizeof(info->num.pi)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += rp_num_len + sizeof(info->num.pi) + sizeof(info->call_id) ;
            QM_MSG_MED_2("Ext Remote Pty number = %s for call id %d",(const char *)info->num.buf, info->call_id );
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_RP_NUM_EXT;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }


        num_of_instances = 0;
        tot_len = 0;
        conn_num_pi = 0;
        
        /* Ext Connected Party number  info (IP) Optional TLV */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
        
          if(info->is_conn_num_available && (info->conn_num_data.len>CM_MAX_CALLED_INFO_CHARS))
          {
            conn_num_pi = info->conn_num_data.pi;
            rp_num_len = sizeof(info->conn_num_data.len) + info->conn_num_data.len;
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)info->conn_num_data.num, info->conn_num_data.len) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->conn_num_data.len, sizeof(info->conn_num_data.len)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->conn_num_data.pi, sizeof(conn_num_pi)) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&info->call_id, sizeof(info->call_id))) )
            {
              num_of_instances = 0;
              dsm_free_packet(&ind);
              break;
            }
            num_of_instances++;
            tot_len += rp_num_len + sizeof(conn_num_pi) + sizeof(info->call_id) ;
            QM_MSG_MED_2("Ext connected number = %s for call id %d",(const char *)info->conn_num_data.num, info->call_id );
          }
        }
        
        if( num_of_instances > 0 )
        {
          tot_len += sizeof(num_of_instances);
          tag = VOICEI_ALL_CALL_IND_CONNECTED_NUM_EXT;
          /* Put the num_of_instances, total length, Tag id */
          if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&num_of_instances, sizeof(num_of_instances)) &&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
               QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
               ) )
          {
            dsm_free_packet(&ind);
          }
        }

        num_of_instances = 0;
        tot_len = 0;
        
        /* Fill Mandatory basic call information */
        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];
          /* Fill the basic call info */
          basic_info.call_id = info->call_id;
          basic_info.call_state = info->call_state;
          basic_info.call_type = info->call_type;
          basic_info.direction = info->direction;
          basic_info.mode = info->mode;
          basic_info.mpty = info->is_mpty;
          basic_info.als = info->als;
          if( !QMI_SVC_PKT_PUSH(&ind, &basic_info, sizeof(basic_info)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
          tot_len += sizeof(basic_info);
	  QM_MSG_MED_7("Call Info for Call id %d :Call State =%d, call type =%d, dir %d, Call Mode =%d, is mpty =%d, als =%d",info->call_id, info->call_state, 
          info->call_type, info->direction,info->mode, info->is_mpty, info->als);

          if(basic_info.call_state == QMI_VOICE_CM_IF_CALL_STATE_INCOMING)
          {
            is_incoming_call_present = TRUE;
          }
        }
        
        tot_len += sizeof(all_call_info->num_of_calls);
        tag = QMI_TYPE_REQUIRED_PARAMETERS;
    
        /* Put the num_of_calls, total length, Tag id */
        if(!(QMI_SVC_PKT_PUSH(&ind, (void *)&all_call_info->num_of_calls, 
                                                    sizeof(all_call_info->num_of_calls)) &&
             QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE)&&
             QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)
             ) )
        {
          dsm_free_packet(&ind);
          continue;
        }

      /* This is a broadcast indication. Send this to all the clients one at
       a time just to avoid dealing with instances.*/
      if( !qmi_voicei_send_indication(cl_sp,
                            VOICEI_CMD_VAL_ALL_CALL_STATUS_IND,
                               ind))
      {
          QM_MSG_MED("VOICE ALL CALL STATUS Indication could not be sent");
      }
      else
      {
          is_ind_sent = TRUE;
          if(cl_sp->asubs_info.is_bind_set_by_client)
          {
            subs = (enum qm_subs_e)cl_sp->asubs_info.as_id;
          }
      }
    }    
  }

  if(is_ind_sent && is_incoming_call_present)
  {
    /* Check if indication retransmission is required and start the timer for retransmit */
    qm_timer_start(QM_TIMER_ID_INCOM_EVT_RESEND, subs);
  }

  return;
} /* qmi_voicei_all_call_status_ind() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_USSD_IND()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_ussd_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_ussd_notification_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_USSD_IND_V02,
                                        (void *) &(data_ptr->data.ussd_ind),
                                        (uint32_t) sizeof(voice_ussd_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_USSD_IND_V02, ind );
      }
    }
  }
} /* qmi_voicei_sups_ussd_ind() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_USSD_RELEASE_IND()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_ussd_release_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  int                             j;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_ussd_notification_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
        if( !qmi_voicei_send_indication( cl_sp,
                              VOICEI_CMD_VAL_SUPS_USSD_RELEASE_IND,
                                     NULL))
        {
          QM_MSG_MED("VOICE SUPS USSD RELEASE Indication could not be sent");
      }
    }
  }
      return;
} /* qmi_voicei_sups_ussd_release_ind() */



/*===========================================================================
  FUNCTION QMI_VOICEI_UUS_IND()
  
  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_uus_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_uus_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_UUS_IND_V02,
                                        (void *) &(data_ptr->data.uus_ind),
                                        (uint32_t) sizeof(voice_uus_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp, QMI_VOICE_UUS_IND_V02, ind );
      }
    }
  }
} /* qmi_voicei_uus_ind() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_IND()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type *        voice_sp;
  qmi_voicei_client_state_type * cl_sp;
  int                            j;
  dsm_item_type *                ind;
  uint16 tot_len=0;
  uint8 tag=VOICEI_UUS_IND_DATA;
  uint16 total_len = 0;
  uint16                  total_barr_list_tlv_size = 0;
  uint8                   barr_num_count = 0;
  uint8  i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
    {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL)&& (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
         (cl_sp->voice_info.reg_sups_events) &&
         (!cl_sp->asubs_info.is_bind_set_by_client ||
         (cl_sp->asubs_info.as_id == data_ptr->header.as_id)))
      {

        ind = NULL;

        if(data_ptr->data.sups_ind.is_ntwk_resp)
        {
          if(FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_SUPS_DATA_SOURCE,
                                            sizeof(data_ptr->data.sups_ind.is_ntwk_resp),
                                            (void *) &data_ptr->data.sups_ind.is_ntwk_resp))
          {
            dsm_free_packet(&ind);
            continue;
          }

          if(data_ptr->data.sups_ind.is_failure_cause_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_FAILURE_CAUSE,
                                            sizeof(data_ptr->data.sups_ind.sups_failure_cause),
                                            (void *) &data_ptr->data.sups_ind.sups_failure_cause)))
          {
            dsm_free_packet(&ind);
            continue;
          }

          if(data_ptr->data.sups_ind.is_clir_status_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_CLIR_STATUS,
                                            sizeof(data_ptr->data.sups_ind.cli_status),
                                            (void *) &data_ptr->data.sups_ind.cli_status)))
          {
            dsm_free_packet(&ind);
            continue;
          }

          if(data_ptr->data.sups_ind.is_clip_status_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_CLIP_STATUS,
                                            sizeof(data_ptr->data.sups_ind.cli_status),
                                            (void *) &data_ptr->data.sups_ind.cli_status)))
          {
            dsm_free_packet(&ind);
            continue;
          }

          if(data_ptr->data.sups_ind.is_colp_status_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_COLP_STATUS,
                                            sizeof(data_ptr->data.sups_ind.cli_status),
                                            (void *) &data_ptr->data.sups_ind.cli_status)))
          {
            dsm_free_packet(&ind);
            continue;
          }

	  if(data_ptr->data.sups_ind.is_colr_status_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_COLR_STATUS,
                                            sizeof(data_ptr->data.sups_ind.cli_status),
                                            (void *) &data_ptr->data.sups_ind.cli_status)))
          {
            dsm_free_packet(&ind);
            continue;
          }

	  if(data_ptr->data.sups_ind.is_cnap_status_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_CNAP_STATUS,
                                            sizeof(data_ptr->data.sups_ind.cli_status),
                                            (void *) &data_ptr->data.sups_ind.cli_status)))
          {
            dsm_free_packet(&ind);
            continue;
          }

		  
          if(data_ptr->data.sups_ind.is_call_fwd_info_from_ntwk_valid)
          {
            const qmi_voice_cm_if_call_fwd_data_ntwk_s *fwd_rsp = &data_ptr->data.sups_ind.cf_data;
            int idx = 0;
            uint8 call_fwd_rsp_tag = VOICEI_SUPS_IND_CF_NTWK_DATA;
            boolean success = TRUE;

            /* Get the highest index */
            idx = fwd_rsp->num_cfw_instances - 1;
            total_len = 0;
            QM_MSG_HIGH_2("CFW: Instances :%d,idx=%d  ", fwd_rsp->num_cfw_instances,idx);
            /* Loop through the service class instances and fill the data */
            while( (idx >= 0) && (idx<QMI_VOICE_CM_IF_MAX_SERVICE_CLASS_INSTANCES))
            {
              /* Put No reply timer */
              if(!QMI_SVC_PKT_PUSH(&ind, (void*)&fwd_rsp->sc_instances[idx].no_reply_timer, 
                                                               sizeof(fwd_rsp->sc_instances[idx].no_reply_timer)))
              {
                dsm_free_packet(&ind);
                success = FALSE;
                break;
              }
              total_len += sizeof(uint8);
              /* Put fwd number if available */
              if(fwd_rsp->sc_instances[idx].num_len > 0)
              {
                if(!QMI_SVC_PKT_PUSH(&ind, (void*)fwd_rsp->sc_instances[idx].num_buf, fwd_rsp->sc_instances[idx].num_len))
                {
                  dsm_free_packet(&ind);\
                  success = FALSE;
                  break;
                }
                total_len += fwd_rsp->sc_instances[idx].num_len;
              }
              /* Put num_len, service_class, service_status */
              if(!(QMI_SVC_PKT_PUSH(&ind, (void*)&fwd_rsp->sc_instances[idx].num_len, 
                                                   sizeof(fwd_rsp->sc_instances[idx].num_len)) &&
                   QMI_SVC_PKT_PUSH(&ind, (void*)&fwd_rsp->sc_instances[idx].service_class, 
                                                   sizeof(fwd_rsp->sc_instances[idx].service_class))&&
                   QMI_SVC_PKT_PUSH(&ind, (void*)&fwd_rsp->sc_instances[idx].service_status,
                                                   sizeof(fwd_rsp->sc_instances[idx].service_status))
                   ) )
              {
                dsm_free_packet(&ind);
                success = FALSE;
                break;
              }
              total_len += 3 * sizeof(uint8);
              /* Decrement the index */
              idx--;
            } /* while */
            
            if( success )
            {
              total_len += sizeof(fwd_rsp->num_cfw_instances);
              QM_MSG_HIGH_2("CFW: Instances :%d,total_len after=%d  ", fwd_rsp->num_cfw_instances,total_len);
              /* Put the num_instances, total length, Tag id */
              if(!(QMI_SVC_PKT_PUSH(&ind, (void*)&fwd_rsp->num_cfw_instances, sizeof(fwd_rsp->num_cfw_instances)) &&
                   QMI_SVC_PKT_PUSH(&ind, (void*)&total_len, VOICEI_TLV_LENGTH_SIZE)&&
                   QMI_SVC_PKT_PUSH(&ind, (void*)&call_fwd_rsp_tag, VOICEI_TLV_TAG_SIZE)
                   ) )
              {
                dsm_free_packet(&ind);
                continue;
              }
            }
            else
            {
              continue;
            }
          }/* Call fwd data filled */
        }

        if(data_ptr->data.sups_ind.is_new_pwd_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.new_pwd_again) + \
                        sizeof(data_ptr->data.sups_ind.new_pwd);
          tag = VOICEI_SUPS_IND_NEW_PWD;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)data_ptr->data.sups_ind.new_pwd_again, sizeof(data_ptr->data.sups_ind.new_pwd_again)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)data_ptr->data.sups_ind.new_pwd, sizeof(data_ptr->data.sups_ind.new_pwd)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_cb_pwd_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.pwd) ;
          tag = VOICEI_SUPS_IND_PWD;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)data_ptr->data.sups_ind.pwd, sizeof(data_ptr->data.sups_ind.pwd)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_alpha_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.alpha_info.alpha_dcs) + \
                        sizeof(data_ptr->data.sups_ind.alpha_info.alpha_len) + \
                        data_ptr->data.sups_ind.alpha_info.alpha_len;
          tag = VOICEI_SUPS_IND_ALPHA;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.alpha_info, tot_len) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_call_id_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.call_id) ;
          tag = VOICEI_SUPS_IND_CALL_ID;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.call_id, sizeof(data_ptr->data.sups_ind.call_id)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_uss_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.uss_info.uss_dcs) + \
                        sizeof(data_ptr->data.sups_ind.uss_info.uss_len) + \
                        data_ptr->data.sups_ind.uss_info.uss_len;
          tag = VOICEI_SUPS_IND_USS_INFO;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.uss_info, tot_len) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_ussd_info_utf16_available)
        {
          QM_MSG_MED("sending UCS2 data");
          tot_len = sizeof(data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16_len) + \
                        (data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16_len)*2;

          tag = VOICEI_SUPS_IND_USS_INFO_UTF16;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16, (data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16_len)*2) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16_len, sizeof(data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16_len)) &&
                QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
        if(data_ptr->data.sups_ind.is_cfw_nrtimer_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.no_reply_timer) ;
          tag = VOICEI_SUPS_IND_CFW_NR_TIMER;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.no_reply_timer, sizeof(data_ptr->data.sups_ind.no_reply_timer)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_cfw_number_available)
        {
          tot_len = data_ptr->data.sups_ind.num_len;
          tag = VOICEI_SUPS_IND_CFW_NUM;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)data_ptr->data.sups_ind.num_buf, tot_len) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_reason_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.reason) ;
          tag = VOICEI_SUPS_IND_REASON;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.reason, sizeof(data_ptr->data.sups_ind.reason)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_service_class_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.service_class) ;
          tag = VOICEI_SUPS_IND_SERVICE_CLASS;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.service_class, sizeof(data_ptr->data.sups_ind.service_class)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
	if(data_ptr->data.sups_ind.is_service_class_ext_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.service_class_ext) ;
          tag = VOICEI_SUPS_IND_SERVICE_CLASS_EXT;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.service_class_ext, sizeof(data_ptr->data.sups_ind.service_class_ext)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
        if(data_ptr->data.sups_ind.barr_num_list_valid)
        {
          tag = VOICEI_SUPS_IND_BARR_LIST;
          barr_num_count = data_ptr->data.sups_ind.barr_num_list_len;
          total_barr_list_tlv_size = 0;
          for(i=0;i<barr_num_count;i++) 
          {
            const qmi_voice_cm_if_icb_ind_s *barred_num = &data_ptr->data.sups_ind.barr_num_list[i];
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)barred_num->barr_num, barred_num->barr_num_len) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&barred_num->barr_num_len, sizeof(barred_num->barr_num_len))) )
            {
              dsm_free_packet(&ind);
              continue;
            }  
            total_barr_list_tlv_size = total_barr_list_tlv_size+ \
                                       sizeof(barred_num->barr_num_len)+ \
                                       barred_num->barr_num_len;
          }
          
          total_barr_list_tlv_size += sizeof(barr_num_count);
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&barr_num_count, sizeof(barr_num_count)) &&
                 QMI_SVC_PKT_PUSH(&ind, (void *)&total_barr_list_tlv_size, VOICEI_TLV_LENGTH_SIZE) &&
                 QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }    
        }
    
    
    /*Mandatory TLV */
        tot_len = sizeof(data_ptr->data.sups_ind.service_type) + \
                      sizeof(data_ptr->data.sups_ind.is_modified_by_call_control);
        tag = VOICEI_SUPS_IND_SERVICE_INFO;
        if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.is_modified_by_call_control, sizeof(data_ptr->data.sups_ind.is_modified_by_call_control)) &&
                 QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.service_type, sizeof(data_ptr->data.sups_ind.service_type)) &&
                 QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                 QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
        {
          dsm_free_packet(&ind);
          continue;
        }

      if( !qmi_voicei_send_indication(cl_sp,
                          VOICEI_CMD_VAL_SUPS_IND,
                                      ind))
        {
        QM_MSG_MED("VOICE SUPS Indication could not be sent");
      }
    }
  }

  return;
} /* qmi_voicei_sups_ussd_ind() */

/*===========================================================================
  FUNCTION qmi_voicei_speech_codec_info_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_speech_codec_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_speech_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_SPEECH_CODEC_INFO_IND_V02,
                                        (void *) &(data_ptr->data.speech_codec_info_ind),
                                        (uint32_t) sizeof(voice_speech_codec_info_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp, QMI_VOICE_SPEECH_CODEC_INFO_IND_V02, ind );
      }
    }
  }
} /* qmi_voicei_speech_codec_info_ind() */

/*===========================================================================
  FUNCTION qmi_voicei_tty_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_tty_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_tty_info_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_TTY_IND_V02,
                                        (void *) &(data_ptr->data.tty_info),
                                        (uint32_t) sizeof(voice_tty_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_TTY_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_tty_ind() */


/*===========================================================================
  FUNCTION qmi_voicei_audio_rat_change_info_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_audio_rat_change_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_audio_rat_change_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_AUDIO_RAT_CHANGE_INFO_IND_V02,
                                        (void *) &(data_ptr->data.audio_rat_change_info),
                                        (uint32_t) sizeof(voice_audio_rat_change_info_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_AUDIO_RAT_CHANGE_INFO_IND_V02,ind);
      }
    }
  }
} /* qmi_voicei_audio_rat_change_info_ind() */

/*===========================================================================
  FUNCTION qmi_voicei_conference_participant_status_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_conference_participant_status_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type *        voice_sp;
  qmi_voicei_client_state_type * cl_sp;
  int                            j;
  dsm_item_type *                ind;
  voice_conf_participant_status_info_ind_msg_v02 *conf_part_status_info_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  conf_part_status_info_ptr = (voice_conf_participant_status_info_ind_msg_v02*)&data_ptr->data;

  QM_MSG_MED_4("Participant status IND: call_id=%d, op=%d,uri=%s, status_sip_code =%d", 
               conf_part_status_info_ptr->call_id, conf_part_status_info_ptr->op_status.operation,
  	conf_part_status_info_ptr->participant_uri, conf_part_status_info_ptr->op_status.sip_status);

  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if((cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
       (cl_sp->voice_info.reg_conf_participants_events) &&
       (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)))
    {  
      ind = NULL;

      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CONF_PARTICIPANT_STATUS_INFO_IND_V02,
                                        (void *) &(data_ptr->data.participant_status_info),
                                        (uint32_t) sizeof(voice_conf_participant_status_info_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_CONF_PARTICIPANT_STATUS_INFO_IND_V02, ind );
      }

    }
  }
  return;
} /* qmi_voicei_conference_participant_status_ind() */


/*===========================================================================
  FUNCTION qmi_voicei_conf_info_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_conf_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_conference_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CONFERENCE_INFO_IND_V02,
                                        (void *) &(data_ptr->data.conference_info_ind),
                                        (uint32_t) sizeof(voice_conference_info_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_CONFERENCE_INFO_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_conf_info_ind() */

/*===========================================================================
  FUNCTION qmi_voicei_additional_call_info_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_additional_call_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_add_call_info_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_ADDITIONAL_CALL_INFO_IND_V02,
                                        (void *) &(data_ptr->data.additional_call_info),
                                        (uint32_t) sizeof(voice_additional_call_info_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp,QMI_VOICE_ADDITIONAL_CALL_INFO_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_additional_call_info_ind() */


/*===========================================================================
  FUNCTION qmi_voicei_conf_join_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_conf_join_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_conference_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CONFERENCE_JOIN_IND_V02,
                                        (void *) &(data_ptr->data.conference_join_ind),
                                        (uint32_t) sizeof(voice_conference_join_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_CONFERENCE_JOIN_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_conf_join_ind() */


/*===========================================================================
  FUNCTION qmi_voicei_conf_participant_update_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_conf_participant_update_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_conference_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CONFERENCE_PARTICIPANT_UPDATE_IND_V02,
                                        (void *) &(data_ptr->data.conf_participant_info_ind),
                                        (uint32_t) sizeof(voice_conference_participant_update_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_CONFERENCE_PARTICIPANT_UPDATE_IND_V02,ind );
      }
    }
  }
}/* qmi_voicei_conf_participant_update_ind() */

/*===========================================================================
  FUNCTION qmi_voicei_e911_orig_fail_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_e911_orig_fail_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type *        voice_sp;
  qmi_voicei_client_state_type * cl_sp;
  int                            j;
  dsm_item_type *                ind;
  qmi_voice_cm_if_e911_orig_fail_ind_s *e911_orig_info_ptr;
  struct qmi_voice_e911_orig_fail_ind_s_type orig_ind;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  e911_orig_info_ptr = (qmi_voice_cm_if_e911_orig_fail_ind_s*)&data_ptr->data;

  QM_MSG_MED_2 ("E911 orig info IND:call_id=%d,end_reason=%d",e911_orig_info_ptr->call_id,e911_orig_info_ptr->end_reason);

  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if((cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
       (cl_sp->voice_info.reg_e911_orig_fail_events) &&     
       (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)))
    {
  
      ind = NULL;
  
      orig_ind.e911_orig_fail_ind.call_id = e911_orig_info_ptr->call_id;
      orig_ind.e911_orig_fail_ind.end_reason = e911_orig_info_ptr->end_reason;

      if(FALSE == qmi_svc_put_param_tlv(&ind,
                                     VOICEI_E911_ORIG_FAIL,
                                     sizeof(orig_ind.e911_orig_fail_ind),
                                     (void*)&orig_ind.e911_orig_fail_ind))
      {
        dsm_free_packet(&ind);
        continue;
      }
  
      if( !qmi_voicei_send_indication(cl_sp, VOICEI_CMD_VAL_E911_ORIG_FAIL_IND, ind))
      {
        QM_MSG_MED("VOICE TTY Indication could not be sent" );
      }
    }
  }
  return;
} /* qmi_voicei_e911_orig_fail_ind() */


/*===========================================================================
  FUNCTION qmi_voicei_vice_dialog_info_ind()

  DESCRIPTION 
   VICE indication from IMS 
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_vice_dialog_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type *        voice_sp = NULL;
  qmi_voicei_client_state_type * cl_sp = NULL;
  int                            j;
  dsm_item_type *                ind = NULL;
  vice_dialog_info_ind_msg_v02  *vice_dialog_info_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  vice_dialog_info_ptr = (vice_dialog_info_ind_msg_v02*)&data_ptr->data;

  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if((cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
       (cl_sp->voice_info.reg_vice_dialog_info_events) &&
       (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)))
    {  
      ind = NULL;

      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_VICE_DIALOG_INFO_IND_V02,
                                        (void *) &(data_ptr->data.vice_dialog_info_ind),
                                        (uint32_t) sizeof(vice_dialog_info_ind_msg_v02),
                                        &ind )
        )
      {
        if( !qmi_voicei_send_indication( cl_sp,
                                         QMI_VOICE_VICE_DIALOG_INFO_IND_V02,
                                         ind )
          )
        {
          QM_MSG_MED_1("QMI_VOICE_VICE_DIALOG_INFO_IND_V02 could not be sent to client with clid=%d.", cl_sp->common.clid);
        }
      }

    }
  }
  return;
} /* qmi_voicei_vice_dialog_info_ind() */

/*===========================================================================
  FUNCTION QMI_VOICEI_BURST_DTMF()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_burst_dtmf
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_BURST_DTMF_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.burst_dtmf),
                                         (uint32_t) sizeof(voice_burst_dtmf_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_BURST_DTMF;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_burst_dtmf() */

/*===========================================================================
  FUNCTION QMI_VOICEI_START_CONT_DTMF()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_start_cont_dtmf
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_START_CONT_DTMF_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.start_cont_dtmf),
                                         (uint32_t) sizeof(voice_start_cont_dtmf_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_START_CONT_DTMF;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_start_cont_dtmf() */

/*===========================================================================
  FUNCTION QMI_VOICEI_STOP_CONT_DTMF()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_stop_cont_dtmf
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_STOP_CONT_DTMF_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.stop_cont_dtmf),
                                         (uint32_t) sizeof(voice_stop_cont_dtmf_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_STOP_CONT_DTMF;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_stop_cont_dtmf() */


/*===========================================================================
  FUNCTION QMI_VOICEI_INFO_REC_IND()

  DESCRIPTION
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_info_rec_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_call_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_INFO_REC_IND_V02,
                                        (void *) &(data_ptr->data.info_rec_ind),
                                        (uint32_t) sizeof(voice_info_rec_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_INFO_REC_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_info_rec_ind() */


/*===========================================================================
  FUNCTION QMI_VOICEI_OTASP_STATUS_IND()

  DESCRIPTION
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_otasp_status_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_call_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_OTASP_STATUS_IND_V02,
                                        (void *) &(data_ptr->data.otasp_status_ind),
                                        (uint32_t) sizeof(voice_otasp_status_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp, QMI_VOICE_OTASP_STATUS_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_otasp_status_ind() */


/*===========================================================================
  FUNCTION QMI_VOICEI_PRIVACY_IND()

  DESCRIPTION
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_privacy_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_voice_privacy_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_PRIVACY_IND_V02,
                                        (void *) &(data_ptr->data.privacy_ind),
                                        (uint32_t) sizeof(voice_privacy_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_PRIVACY_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_privacy_ind() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_NOTIFICATION_IND()

  DESCRIPTION
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_notification_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_sups_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_SUPS_NOTIFICATION_IND_V02,
                                        (void *) &(data_ptr->data.sups_notify_ind),
                                        (uint32_t) sizeof(voice_sups_notification_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_SUPS_NOTIFICATION_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_sups_notification_ind() */


/* Supplementary Services API's*/


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CLIR()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_clir
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CLIR_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_clir),
                                         (uint32_t) sizeof(voice_get_clir_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CLIR;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_clir() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CLIR_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_clir_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_clir_resp),
                                    (uint32_t) sizeof(voice_get_clir_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_sups_get_clir_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_FORWARDING()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_call_forwarding
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CALL_FORWARDING_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_call_fwd),
                                         (uint32_t) sizeof(voice_get_call_forwarding_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_FORWARDING;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_call_forwarding() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_FORWARDING_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_call_forwarding_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_call_fwd_resp),
                                    (uint32_t) sizeof(voice_get_call_forwarding_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_sups_get_call_forwarding_resp() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CLIP()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_clip
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CLIP_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_clip),
                                         (uint32_t) sizeof(voice_get_clip_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CLIP;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_clip() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_COLP()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_colp
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_COLP_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_colp),
                                         (uint32_t) sizeof(voice_get_colp_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_COLP;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_colp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_WAITING()

  DESCRIPTION
    Get the CallWaiting status of a service class if not specified 
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_get_call_waiting
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CALL_WAITING_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_call_waiting),
                                         (uint32_t) sizeof(voice_get_call_waiting_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_WAITING;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
}


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_BARRING()

  DESCRIPTION
    Get the Callbarring status for a specified reason with service class.
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_get_call_barring
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CALL_BARRING_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_call_barring),
                                         (uint32_t) sizeof(voice_get_call_barring_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_BARRING;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_call_barring() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_SET_CALL_BARRING()

  DESCRIPTION
    Set the Callbarring Password for a specified reason with service class.
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_set_call_barring
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_SET_CALL_BARRING_PASSWORD_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.set_call_barring),
                                         (uint32_t) sizeof(voice_set_call_barring_password_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_SET_CALL_BARRING;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_set_call_barring() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_MANAGE_CALLS()

  DESCRIPTION
    Set the Callbarring Password for a specified reason with service class.
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_manage_calls
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  uint8              type;
  uint16             len;
  uint16             expected_len;
  void *             value;
  boolean            got_v_in_required;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  value             = NULL;
  got_v_in_required = FALSE;

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sups_type);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sups_type;
        got_v_in_required = TRUE;
        break;

      case VOICEI_MNG_CALLS_REQ_CALL_ID:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id_valid = TRUE;
        break;

      case VOICEI_MNG_CALLS_REQ_REJ_CAUSE:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause_valid = TRUE;
        break;

      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ( (expected_len != 0) && (expected_len != len) )
    {
      QM_MSG_MED_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if ( len != dsm_pullup( sdu_in, value, len ) )
    {                                                                    
      QM_MSG_MED("Invalid length in TLV");
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd = QMI_VOICE_CM_IF_CMD_SUPS_MANAGE_CALLS;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_call_barring() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_SET_SUPS_SERVICE()

  DESCRIPTION
    Set the Call Independant supplementary services with specified reason and service.
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_set_sups_service
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  uint8              type;
  uint16             len;
  uint16             expected_len;
  void *             value;
  boolean            got_v_in_required;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  uint8 i=0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  value             = NULL;
  got_v_in_required = FALSE;

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));


  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.sups_service_info);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.sups_service_info;
        got_v_in_required = TRUE;
        break;
      case VOICEI_SET_SUPS_SC:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_SC_EXT:
	expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_ext);
	value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_ext;
	cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_ext_valid = TRUE;
	break;
      case VOICEI_SET_SUPS_BARR_PWD:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.password);
        value = cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.password;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.password_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_FWD_NUM:
        value = cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_number;
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_number))
        {
          QM_MSG_MED("Invalid calling number length");
          errval = QMI_ERR_ARG_TOO_LONG;
          
          goto send_result;
        }
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_number_len = (uint8)len;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_number_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_NR_TIMER:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.no_reply_timer);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.no_reply_timer;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.no_reply_timer_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_NUM_TYPE_PLAN:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.num_type_plan);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.num_type_plan;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.num_type_plan_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_BARR_NUM_LIST:
        if ( !(QMI_VOICEI_PULL(sdu_in, &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list_len, sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list_len)) ) )
        {
          QM_MSG_MED("Invalid length in barr list TLV");
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        else
        {
          for(i=0;i<cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list_len;i++)
          {
            if(!(QMI_VOICEI_PULL(sdu_in, &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list[i].barr_num_len, sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list[i].barr_num_len)) &&
                 QMI_VOICEI_PULL(sdu_in, cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list[i].barr_num, cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list[i].barr_num_len))
                 )
             {
               QM_MSG_MED("Invalid data in barr list TLV");
               errval = QMI_ERR_MALFORMED_MSG;
               goto send_result;
             }
          }
        }
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list_valid = TRUE;
        continue; //By design; do not change.
      case VOICEI_SET_SUPS_COLR_PI:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.colr_pi);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.colr_pi;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.colr_pi_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_CALL_FWD_START_TIME:
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_start_time))
        {
          QM_MSG_MED("Invalid Start time length");
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_start_time;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_start_time_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_CALL_FWD_END_TIME:
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_end_time))
        {
          QM_MSG_MED("Invalid End time length");
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_end_time;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_end_time_valid = TRUE;
        break;
      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ( (expected_len != 0) && (expected_len != len) )
    {
      QM_MSG_MED_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if ( len != dsm_pullup( sdu_in, value, len ) )
    {                                                                    
      QM_MSG_MED("Invalid length in TLV");
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if( (cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_valid &&
  	   cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_ext_valid)
  	   &&
  	  (cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class != 
  	   cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_ext)
    )
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd = QMI_VOICE_CM_IF_CMD_SUPS_SET_SUPS_SERVICE;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_set_sups_service() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_ANSWER_USSD()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_answer_ussd
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ANSWER_USSD_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.answer_ussd),
                                         (uint32_t) sizeof(voice_answer_ussd_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_ANSWER_USSD;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_answer_ussd */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_ANSWER_USSD_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_answer_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.answer_ussd_resp),
                                    (uint32_t) sizeof(voice_answer_ussd_resp_msg_v02),
                                    &response )
    )
  {
   qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_sups_answer_ussd_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_ORIG_USSD()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_orig_ussd
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ORIG_USSD_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.ussd_orig_info),
                                         (uint32_t) sizeof(voice_orig_ussd_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_ORIG_USSD;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_orig_ussd() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_ORIG_USSD_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_orig_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.ussd_orig_resp),
                                    (uint32_t) sizeof(voice_orig_ussd_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_sups_orig_ussd_resp*/


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_CANCEL_USSD()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_cancel_ussd
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_CANCEL_USSD_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.cancel_ussd),
                                         (uint32_t) sizeof(voice_cancel_ussd_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_CANCEL_USSD;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_cancel_ussd() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_ORIG_USSD_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_cancel_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.cancel_ussd_resp),
                                    (uint32_t) sizeof(voice_cancel_ussd_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_sups_cancel_ussd_resp*/

/*===========================================================================
  FUNCTION QMI_VOICEI_SET_CONFIG_ITEMS()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_set_modem_config
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_SET_CONFIG_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.set_config),
                                         (uint32_t) sizeof(voice_set_config_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SET_MODEM_CONFIG;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_set_modem_config() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SET_MODEM_CONFIG_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_set_modem_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.set_config_resp),
                                    (uint32_t) sizeof(voice_set_config_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_set_modem_config_resp*/

/*===========================================================================
  FUNCTION QMI_VOICEI_SRVCC_CALL_CONFIG_REQ()

  DESCRIPTION
    This Command will fill the call information in CM for enabling SRVCC of calls
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_srvcc_call_config_req
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  uint8              type,i=0,j=0;
  uint16             len;
  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voice_cm_if_set_srvcc_call_context_s  *srvcc_calls_info;
  qmi_voicei_client_state_type * client_sp;

  uint8              instance_id=0;
  boolean            instance_found = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  srvcc_calls_info = &cmd_ptr->cmd.cm_if_data_ptr->data.srvcc_calls_info;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        if ( !(QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->num_calls, sizeof(srvcc_calls_info->num_calls)) ) )
        {
          QM_MSG_MED("Invalid length in call info TLV");
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        else
        {
          for(i=0;i<srvcc_calls_info->num_calls;i++)
          {
            if(!(QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].instance_id, sizeof(srvcc_calls_info->srvcc_calls[i].instance_id)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].call_type, sizeof(srvcc_calls_info->srvcc_calls[i].call_type)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].call_state, sizeof(srvcc_calls_info->srvcc_calls[i].call_state)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].call_substate, sizeof(srvcc_calls_info->srvcc_calls[i].call_substate)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].is_mpty_call, sizeof(srvcc_calls_info->srvcc_calls[i].is_mpty_call)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].direction, sizeof(srvcc_calls_info->srvcc_calls[i].direction)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].num_len, sizeof(srvcc_calls_info->srvcc_calls[i].num_len)) &&
                 QMI_VOICEI_PULL(sdu_in, srvcc_calls_info->srvcc_calls[i].num, srvcc_calls_info->srvcc_calls[i].num_len))
                 )
             {
               QM_MSG_MED("Invalid data in call info TLV");
               errval = QMI_ERR_MALFORMED_MSG;
               goto send_result;
             }
            QM_MSG_MED_3("Instance ID %d Call Type %d Call State %d", srvcc_calls_info->srvcc_calls[i].instance_id, srvcc_calls_info->srvcc_calls[i].call_type, srvcc_calls_info->srvcc_calls[i].call_state);
            QM_MSG_MED_3("Call Substate %d Is_Mpty %d Direction %d", srvcc_calls_info->srvcc_calls[i].call_substate, srvcc_calls_info->srvcc_calls[i].is_mpty_call, srvcc_calls_info->srvcc_calls[i].direction);
            QM_MSG_MED_2("Num Len %d Num %d",srvcc_calls_info->srvcc_calls[i].num_len, srvcc_calls_info->srvcc_calls[i].num_len);
          }
        }
        break;
      case VOICEI_SRVCC_CALL_CONFIG_REQ_ALERTING_TYPE:
        if ( !(QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->alerting_type_list_len, sizeof(srvcc_calls_info->alerting_type_list_len)) ) )
        {
          QM_MSG_MED("Invalid length in call info TLV");
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        else
        {
          for(i=0;i<srvcc_calls_info->alerting_type_list_len;i++)
          {
            if(QMI_VOICEI_PULL(sdu_in, &instance_id, sizeof(instance_id)))
            {
              instance_found = FALSE;
              for(j=0;j<srvcc_calls_info->num_calls;j++)
              {
                if(srvcc_calls_info->srvcc_calls[j].instance_id == instance_id)
                {
                  instance_found = TRUE;
                  srvcc_calls_info->srvcc_calls[j].is_alerting_type_valid = TRUE;
                  if(!QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].alerting_type, sizeof(srvcc_calls_info->srvcc_calls[i].alerting_type)))
                  {
                    QM_MSG_MED("Invalid data in alerting_type TLV");
                    errval = QMI_ERR_MALFORMED_MSG;
                    goto send_result;
                  }
                QM_MSG_MED_1("Alerting Type %d",srvcc_calls_info->srvcc_calls[i].alerting_type);
                }
              }
              if(instance_found == FALSE)
              {
                QM_MSG_MED_1("instance ID %d given in the alerting_type not found in call list", instance_id);
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
            }
            else
            {
               QM_MSG_MED("Invalid instance id in alerting_type TLV");
               errval = QMI_ERR_MALFORMED_MSG;
               goto send_result;
            }
          }
        }
        break;

      case VOICEI_SRVCC_CALL_CONFIG_REQ_CALL_NUM_PI:
        if ( !(QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->num_pi_list_len, sizeof(srvcc_calls_info->num_pi_list_len)) ) )
        {
          QM_MSG_MED("Invalid length in call info TLV");
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        else
        {
          for(i=0;i<srvcc_calls_info->num_pi_list_len;i++)
          {
            if(QMI_VOICEI_PULL(sdu_in, &instance_id, sizeof(instance_id)))
            {
              instance_found = FALSE;
              for(j=0;j<srvcc_calls_info->num_calls;j++)
              {
                if(srvcc_calls_info->srvcc_calls[j].instance_id == instance_id)
                {
                  instance_found = TRUE;
                  srvcc_calls_info->srvcc_calls[j].is_num_pi_valid = TRUE;
                  if(!QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].num_pi, sizeof(srvcc_calls_info->srvcc_calls[i].num_pi)))
                  {
                    QM_MSG_MED("Invalid data in num_pi TLV");
                    errval = QMI_ERR_MALFORMED_MSG;
                    goto send_result;
                  }
                QM_MSG_MED_1("Number Pi %d", srvcc_calls_info->srvcc_calls[i].num_pi);
                }
              }
              if(instance_found == FALSE)
              {
                QM_MSG_MED_1("instance ID %d given in the num_pi not found in call list", instance_id);
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
            }
            else
            {
               QM_MSG_MED("Invalid instance id in num_pi TLV");
               errval = QMI_ERR_MALFORMED_MSG;
               goto send_result;
            }
          }
        }
        break;
      case VOICEI_SRVCC_CALL_CONFIG_REQ_CALL_NAME:
        if ( !(QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->caller_name_list_len, sizeof(srvcc_calls_info->caller_name_list_len)) ) )
        {
          QM_MSG_MED("Invalid length in call info TLV");
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        else
        {
          for(i=0;i<srvcc_calls_info->caller_name_list_len;i++)
          {
            if(QMI_VOICEI_PULL(sdu_in, &instance_id, sizeof(instance_id)))
            {
              instance_found = FALSE;
              for(j=0;j<srvcc_calls_info->num_calls;j++)
              {
                if(srvcc_calls_info->srvcc_calls[j].instance_id == instance_id)
                {
                  instance_found = TRUE;
                  srvcc_calls_info->srvcc_calls[j].is_caller_name_type_valid = TRUE;
                  if(!(QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].name_pi, sizeof(srvcc_calls_info->srvcc_calls[i].name_pi)) &&
                       QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].name_len, sizeof(srvcc_calls_info->srvcc_calls[i].name_len)) &&
                       QMI_VOICEI_PULL(sdu_in, srvcc_calls_info->srvcc_calls[i].caller_name, 2*(srvcc_calls_info->srvcc_calls[i].name_len))
                       )
                    )
                  {
                    QM_MSG_MED_2("Invalid data in caller name TLV (NAME_PI:%d, NAME_LEN%d)",
                                  srvcc_calls_info->srvcc_calls[i].name_pi,
                                  srvcc_calls_info->srvcc_calls[i].name_len);
                    errval = QMI_ERR_MALFORMED_MSG;
                    goto send_result;
                  }
                 QM_MSG_MED_2("name Pi %d Name Len %d", srvcc_calls_info->srvcc_calls[i].name_pi, srvcc_calls_info->srvcc_calls[i].name_len);
                }
              }
              if(instance_found == FALSE)
              {
                QM_MSG_MED_1("instance ID %d given in the caller name not found in call list", instance_id);
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
            }
            else
            {
               QM_MSG_MED("Invalid instance id in caller name TLV");
               errval = QMI_ERR_MALFORMED_MSG;
               goto send_result;
            }
          }
        }
        break;
      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }
  }

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd = QMI_VOICE_CM_IF_CMD_SRVCC_CALL_CONFIG;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }


  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_srvcc_call_config_req() */


/*===========================================================================
  FUNCTION qmi_voicei_srvcc_call_config_resp()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_srvcc_call_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  //QMI VOICE qmi_voicei_srvcc_call_config_resp

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("Null cmd_buf ptr in qmi_voicei_srvcc_call_config_resp");
    return;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  errval = data_ptr->data.set_srvcc_resp.error;
  response = NULL;

  if (!qmi_voicei_send_response(errval, (void*)cmd_buf_p, response))
  {
    QM_MSG_MED("Unable to send response for qmi_voicei_srvcc_call_config operation");
  }
}

/*===========================================================================
  FUNCTION QMI_VOICEI_GET_CONFIG_ITEMS()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_get_modem_config
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CONFIG_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_config),
                                         (uint32_t) sizeof(voice_get_config_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_GET_MODEM_CONFIG;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_get_modem_config() */

/*===========================================================================
  FUNCTION QMI_VOICEI_GET_MODEM_CONFIG_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_get_modem_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_config_resp),
                                    (uint32_t) sizeof(voice_get_config_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
}/*qmi_voicei_get_modem_config_resp*/


/*===========================================================================
  FUNCTION QMI_VOICEI_BIND_SUBSCRIPTION()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_bind_subscription
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type     *response = NULL;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_BIND_SUBSCRIPTION_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.bind_subscription),
                                         (uint32_t) sizeof(voice_bind_subscription_req_msg_v02));
  if(errval) goto send_result;

  if((uint8)cmd_ptr->cmd.cm_if_data_ptr->data.bind_subscription.subs_type >= QMI_VOICE_CM_IF_AS_ID_MAX)
  {
    QM_MSG_MED_1("Unsupported TLV value (%d) for subs_type.", cmd_ptr->cmd.cm_if_data_ptr->data.bind_subscription.subs_type);
    errval = QMI_ERR_NO_SUBSCRIPTION;
    goto send_result;
  }

  if(client_sp != NULL)
  {
    client_sp->asubs_info.as_id = (uint8)cmd_ptr->cmd.cm_if_data_ptr->data.bind_subscription.subs_type;
    client_sp->asubs_info.is_bind_set_by_client = TRUE;
    QM_MSG_MED_2("BIND REQUESTED BY: clid=%d Bounded to as_id=%d ",client_sp->common.clid,client_sp->asubs_info.as_id);
  }
  else
  {
    /*Freeing cm_if_data_ptr and cmd_ptr*/
    QMI_VOICE_CM_IF_MEM_FREE(cmd_ptr->cmd.cm_if_data_ptr);
    qmi_mmode_free_cmd_data_buf(cmd_ptr);
    return QMI_SVC_RESPONSE_NONE;
  }

send_result:
  /*Freeing cm_if_data_ptr and cmd_ptr*/
  if(cmd_ptr != NULL)
  {
    if(cmd_ptr->cmd.cm_if_data_ptr != NULL)
    {
      QMI_VOICE_CM_IF_MEM_FREE(cmd_ptr->cmd.cm_if_data_ptr);
    }
    qmi_mmode_free_cmd_data_buf(cmd_ptr);
  }

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
}/*qmi_voicei_bind_subscription*/

/*===========================================================================
  FUNCTION QMI_VOICEI_ORIG_USSD_NO_WAIT()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_orig_ussd_no_wait
(
  void *                          sp,
  void *                          cmd_buf_p,
  void *                          cl_sp,
  dsm_item_type **                sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  voice_uss_info_type_v02         *uusd_data;
  qmi_voice_cmd_list_public_type   info_ptr;
  qmi_result_e_type   result;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  memset(&info_ptr, 0, sizeof(info_ptr));

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ORIG_USSD_NO_WAIT_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.ussd_orig_no_wait_info),
                                         (uint32_t) sizeof(voice_orig_ussd_no_wait_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_ORIG_USSD_NO_WAIT;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)client_sp;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  uusd_data = &(cmd_ptr->cmd.cm_if_data_ptr->data.ussd_orig_no_wait_info.uss_info);

  /* Check the incoming UUSD parameters. */
  if ((uusd_data->uss_data_len == 0) ||
      (uusd_data->uss_data_len >= QMI_VOICE_USS_DATA_MAX_V02) ||
      (uusd_data->uss_dcs < USS_DCS_ASCII_V02) ||
      (uusd_data->uss_dcs > USS_DCS_UCS2_V02))
  {
    QM_MSG_HIGH("Received ORIG_USSD_NO_WAIT with Invalid Data");
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /* Check to see if a USSD request is in progress. */
  if (qmi_voice_cmd_list_query_by_event(QMI_VOICE_CM_IF_EVT_SUPS_USS_IND,
                                        &info_ptr, cmd_ptr->cmd.cm_if_data_ptr->header.as_id) == E_SUCCESS)
  {
    errval = QMI_ERR_INCOMPATIBLE_STATE;
    goto send_result;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

send_result:
  if (errval == QMI_ERR_NONE)
  {
    result = QMI_RESULT_SUCCESS;
  }
  else
  {
    result = QMI_RESULT_FAILURE;
    if(cmd_ptr != NULL)
    {
      if(cmd_ptr->cmd.cm_if_data_ptr != NULL)
      {
        QMI_VOICE_CM_IF_MEM_FREE(cmd_ptr->cmd.cm_if_data_ptr);
      }
      qmi_mmode_free_cmd_data_buf(cmd_ptr);
    }
  }
  
  return qmi_voicei_send_result(errval, NULL);
} /* qmi_voicei_sups_orig_ussd_no_wait() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_ORIG_USSD_NO_WAIT_IND()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_orig_ussd_no_wait_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Pull the client state pointer from the handle. For no_wait_ind, the
     handle is the client state and not the cmd_buf. */
  cl_sp = (qmi_voicei_client_state_type *)data_ptr->header.handle;

  /* Verify that the client state has not been freed. */
  if ((NULL == cl_sp) || (QMI_SVC_CLID_UNUSED == cl_sp->common.clid))
  {
    QM_MSG_MED("QMI VOICE invalid client state pointer");
    return;
  }
  else if(!cl_sp->voice_info.reg_ussd_notification_events)
  {
    QM_MSG_MED("QMI VOICE, client not registed for orig_ussd_no_wait_ind");
    return;
  }

  voice_sp = &qmi_voice_state;

  ind = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_INDICATION,
                                    (uint16_t) QMI_VOICE_ORIG_USSD_NO_WAIT_IND_V02,
                                    (void *) &(data_ptr->data.ussd_orig_no_wait_ind),
                                    (uint32_t) sizeof(voice_orig_ussd_no_wait_ind_msg_v02),
                                    &ind )
    )
  {
   qmi_voicei_send_indication( cl_sp,QMI_VOICE_ORIG_USSD_NO_WAIT_IND_V02,ind );
  }
}/*qmi_voicei_sups_orig_ussd_no_wait_ind*/

/*===========================================================================
  FUNCTION qmi_voicei_call_modified_ind()
  
  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_call_modified_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_modification_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_MODIFIED_IND_V02,
                                        (void *) &(data_ptr->data.call_modified_ind),
                                        (uint32_t) sizeof(voice_modified_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_MODIFIED_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_call_modified_ind() */

/*===========================================================================
  FUNCTION qmi_voicei_call_modify_accept_ind()
  
  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_call_modify_accept_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_state_type          *voice_sp;
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;
  int                             j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  voice_sp = &qmi_voice_state;

  for( j = 0; j < VOICEI_MAX_CLIDS; j++ )
  {
    cl_sp = (qmi_voicei_client_state_type *)voice_sp->client[j];
    if( (cl_sp != NULL) && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        (cl_sp->voice_info.reg_modification_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_MODIFY_ACCEPT_IND_V02,
                                        (void *) &(data_ptr->data.call_modify_accept_ind),
                                        (uint32_t) sizeof(voice_modify_accept_ind_msg_v02),
                                        &ind )
        )
      {
      qmi_voicei_send_indication( cl_sp,QMI_VOICE_MODIFY_ACCEPT_IND_V02,ind );
      }
    }
  }
} /* qmi_voicei_call_modify_accept_ind() */

/*===========================================================================
  FUNCTION QMI_VOICEI_ALS_SET_LINE_SWITCHING()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_als_set_line_switching
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ALS_SET_LINE_SWITCHING_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.set_line_switch),
                                         (uint32_t) sizeof(voice_als_set_line_switching_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_ALS_SET_LINE_SWITCHING;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_als_set_line_switching */

/*===========================================================================
  FUNCTION QMI_VOICEI_ALS_SET_LINE_SWITCHING_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_als_set_line_switching_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.set_line_switch_resp),
                                    (uint32_t) sizeof(voice_als_set_line_switching_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_als_set_line_switching_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_ALS_SELECT_LINE()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_als_select_line
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ALS_SELECT_LINE_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.select_line),
                                         (uint32_t) sizeof(voice_als_select_line_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_ALS_SELECT_LINE;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_als_select_line */

/*===========================================================================
  FUNCTION QMI_VOICEI_ALS_SELECT_LINE_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_als_select_line_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.select_line_resp),
                                    (uint32_t) sizeof(voice_als_select_line_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_als_select_line_resp */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_COLR()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_colr
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_COLR_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_colr),
                                         (uint32_t) sizeof(voice_get_colr_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_COLR;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_colr() */



/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_COLR_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_colr_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_colr_resp),
                                    (uint32_t) sizeof(voice_get_colr_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_sups_get_colr_resp*/

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CNAP()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_cnap
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CNAP_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_cnap),
                                         (uint32_t) sizeof(voice_get_cnap_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CNAP;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_cnap() */



/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CNAP_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_cnap_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_cnap_resp),
                                    (uint32_t) sizeof(voice_get_cnap_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_sups_get_cnap_resp*/




/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_MANAGE_IP_CALLS()

  DESCRIPTION
    Manage the call dependant sups for VoIP Calls.
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_manage_ip_calls
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  uint8              type;
  uint16             len;
  uint16             expected_len;
  void *             value;
  boolean            got_v_in_required;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  value             = NULL;
  got_v_in_required = FALSE;

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    QM_MSG_MED_1("sups_manage_ip_calls(): TLV type (%d)", type);
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sups_type);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sups_type;
        got_v_in_required = TRUE;
        break;

      case VOICEI_MNG_IP_CALLS_CALL_ID:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id_valid = TRUE;
        break;

      case VOICEI_MNG_IP_CALLS_CALL_TYPE:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_type);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_type;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_type_valid = TRUE;
        break;

      case VOICEI_MNG_IP_CALLS_AUDIO_ATTRIB:
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.audio_attrib))
        {
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.audio_attrib;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.is_audio_attrib_valid= TRUE;
        break;

      case VOICEI_MNG_IP_CALLS_VIDEO_ATTRIB:
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.video_attrib))
        {
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.video_attrib;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.is_video_attrib_valid= TRUE;
        break;         

      case VOICEI_MNG_IP_CALLS_SIP_URI:        
        value = cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_uri;
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_uri))
        {
          QM_MSG_MED("Invalid SIP URI length");
          errval = QMI_ERR_ARG_TOO_LONG;
          
          goto send_result;
        }
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_uri_len =(uint8)len;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.is_sip_uri_valid = TRUE;
        break;
      case VOICEI_MNG_IP_CALLS_SIP_REJECT_CAUSE:                
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause))
        {
          QM_MSG_MED("Invalid Reject cause length");
          errval = QMI_ERR_ARG_TOO_LONG;
          
          goto send_result;
        }        
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause_valid = TRUE;
        break;
      case VOICEI_MNG_IP_CALLS_SIP_REJECT_CODE:                
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_reject_code))
        {
          QM_MSG_MED("Invalid SIP Reject Code length");
          errval = QMI_ERR_ARG_TOO_LONG;
          
          goto send_result;
        }        
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_reject_code;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_reject_code_valid = TRUE;
        break;
      case VOICEI_MNG_IP_CALLS_SPEECH_CODEC:
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.speech_codec))
        {
          QM_MSG_MED("Invalid Speech Codec length");
          errval = QMI_ERR_ARG_TOO_LONG;

          goto send_result;
        }
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.speech_codec;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.is_speech_codec_valid = TRUE;
        break;

      case VOICEI_MNG_IP_CALLS_ECT_TYPE:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.ect_type);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.ect_type;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.is_ect_type_valid = TRUE;
        break;

      case VOICEI_MNG_IP_CALLS_TRANSFER_TARGET_CALL_ID:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.transfer_target_call_id);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.transfer_target_call_id;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.is_transfer_target_call_id = TRUE;
        break;

      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ( (expected_len != 0) && (expected_len != len) )
    {
      QM_MSG_MED_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if ( len != dsm_pullup( sdu_in, value, len ) )
    {                                                                    
      QM_MSG_MED("Invalid length in TLV");
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd = QMI_VOICE_CM_IF_CMD_SUPS_MANAGE_IP_CALLS;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  cmd_ptr->cmd.cm_if_data_ptr->header.as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_manage_ip_calls() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_MANAGE_IP_CALLS_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_manage_ip_calls_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.mng_ip_calls_resp),
                                    (uint32_t) sizeof(voice_manage_ip_calls_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
} /* qmi_voicei_sups_manage_ip_calls_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_GET_LINE_SWITCHING_STATUS()

  DESCRIPTION
  
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_als_get_line_switching_status
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ALS_GET_LINE_SWITCHING_STATUS_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_line_switch_status),
                                         (uint32_t) sizeof(voice_als_get_line_switching_status_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_ALS_GET_LINE_SWITCHING_STATUS;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_get_line_switching_status() */


/*===========================================================================
  FUNCTION QMI_VOICEI_GET_LINE_SWITCHING_STATUS_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_als_get_line_switching_status_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_line_switch_status_resp),
                                    (uint32_t) sizeof(voice_als_get_line_switching_status_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
} /* qmi_voicei_get_line_switching_status_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_GET_SELECTED_LINE()

  DESCRIPTION
  
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_als_get_selected_line
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ALS_GET_SELECTED_LINE_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_selected_line),
                                         (uint32_t) sizeof(voice_als_get_selected_line_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_ALS_GET_SELECTED_LINE;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_get_line_switching_status() */


/*===========================================================================
  FUNCTION QMI_VOICEI_GET_SELECTED_LINE_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_als_get_selected_line_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_selected_line_resp),
                                    (uint32_t) sizeof(voice_als_get_selected_line_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
} /* qmi_voicei_get_selected_line() */


/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_RESET_ACM()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_aoc_reset_acm
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_AOC_RESET_ACM_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.reset_acm),
                                         (uint32_t) sizeof(voice_aoc_reset_acm_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_AOC_RESET_ACM;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
}/* qmi_voicei_aoc_reset_acm() */

/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_RESET_ACM_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_aoc_reset_acm_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.reset_acm_resp),
                                    (uint32_t) sizeof(voice_aoc_reset_acm_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
}/* qmi_voicei_aoc_reset_acm_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_SET_ACMMAX()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_aoc_set_acmmax
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type    *cmd_ptr;
  qmi_voicei_client_state_type   *client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_AOC_SET_ACMMAX_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.set_acmmax),
                                         (uint32_t) sizeof(voice_aoc_set_acmmax_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_AOC_SET_ACMMAX;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
}/* qmi_voicei_aoc_set_acmmax() */

/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_SET_ACMMAX_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_aoc_set_acmmax_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.set_acmmax_resp),
                                    (uint32_t) sizeof(voice_aoc_set_acmmax_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
}/* qmi_voicei_aoc_set_acmmax_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_GET_CALL_MATER_INFO()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_aoc_get_call_meter_info
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_AOC_GET_CALL_METER_INFO_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_call_meter_info),
                                         (uint32_t) sizeof(voice_aoc_get_call_meter_info_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_AOC_GET_CALL_METER_INFO;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
}/* qmi_voicei_aoc_get_call_meter_info() */

/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_GET_CALL_MATER_INFO_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_aoc_get_call_meter_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_call_meter_info_resp),
                                    (uint32_t) sizeof(voice_aoc_get_call_meter_info_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_aoc_get_call_meter_info_resp() */

/*===========================================================================
  FUNCTION QMI_VOICEI_CALL_ENCRYPTION_REQ

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_call_encryption_req
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_CALL_ENCRYPTION_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.call_encryption_req),
                                         (uint32_t) sizeof(voice_call_encryption_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_CALL_ENCRYPTION;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);

}
/*===========================================================================
  FUNCTION QMI_VOICEI_CALL_ENCRYPTION_REQ

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voicei_call_encryption_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_cmd_buf_type *      cmd_buf_p;
  dsm_item_type *         response;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  cmd_buf_p = (qmi_cmd_buf_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }
  /* Remove the entry from cmd list here */
 qmi_voice_cmd_list_free( data_ptr->header.handle );
  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.call_encryption_resp),
                                    (uint32_t) sizeof(voice_call_encryption_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
  
}

#endif/* !FEATURE_NO_QMI_VOICE*/

#endif // FEATURE_QMI_SERVICE_SPLIT

