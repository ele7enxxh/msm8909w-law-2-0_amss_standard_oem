#ifndef _DS_QMI_NAS_MSG_H_
#define _DS_QMI_NAS_MSG_H_

/*===========================================================================

                         QMI_NAS_MSG.H

DESCRIPTION

 The Qualcomm Network Access Services Message MSM Interface header file.

Copyright (c) 2010-2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_nas_msg.h#1 $ $Change: 11985193 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/15/12    aa     Centralized EONS support status message
05/21/10    hs     Initial version
===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/

#include "comdef.h"
#include "qmi_svc_defs.h"
#include "dsm_pool.h"
#include "nv.h"
#include "sys.h"
#include "network_access_service_v01.h"
#include <stringl/stringl.h>
#include "cmapi.h"

/*---------------------------------------------------------------------------
  Shared Constants
---------------------------------------------------------------------------*/
#define NASI_CURR_NAM_MASK         (0xFF)
#define NASI_MCC_LEN               (3)
#define NASI_11_12_LEN             (2)
#define NASI_MIN1_LEN              (7)
#define NASI_MIN2_LEN              (3)
#define MAX_NAM_NAME_LEN           (12)
#define MAX_ECIO_THRESHOLDS        (10)
#define MAX_SINR_THRESHOLDS        (5)
#define NAS_IS_856_MAX_LEN         (16)
#define NASI_MDN_MAX_LEN           (15)
#define NAS_CSG_NAME_MAX_LEN       (48)
#define NAS_LTE_MAX_NUM_LAYER      (CMAPI_LTE_MAX_NUM_LAYER)
#define NAS_SIB_NUM_MIN_VALID      (1)
#define NAS_SIB_NUM_MAX_VALID      (16)

enum qmi_nas_bc_e
{
  QMI_NAS_BC_BC_0                     = 0,
  QMI_NAS_BC_BC_1                     = 1,
  QMI_NAS_BC_BC_3                     = 3,
  QMI_NAS_BC_BC_4                     = 4,
  QMI_NAS_BC_BC_5                     = 5,
  QMI_NAS_BC_BC_6                     = 6,
  QMI_NAS_BC_BC_7                     = 7,
  QMI_NAS_BC_BC_8                     = 8,
  QMI_NAS_BC_BC_9                     = 9,
  QMI_NAS_BC_BC_10                    = 10,
  QMI_NAS_BC_BC_11                    = 11,
  QMI_NAS_BC_BC_12                    = 12,
  QMI_NAS_BC_BC_13                    = 13,
  QMI_NAS_BC_BC_14                    = 14,
  QMI_NAS_BC_BC_15                    = 15,
  QMI_NAS_BC_BC_16                    = 16,
  QMI_NAS_BC_BC_17                    = 17,
  QMI_NAS_BC_BC_18                    = 18,
  QMI_NAS_BC_BC_19                    = 19,
  QMI_NAS_BC_GSM_450                  = 40,
  QMI_NAS_BC_GSM_480                  = 41,
  QMI_NAS_BC_GSM_750                  = 42,
  QMI_NAS_BC_GSM_850                  = 43,
  QMI_NAS_BC_GSM_900_EXTENDED         = 44,
  QMI_NAS_BC_GSM_900_PRIMARY          = 45,
  QMI_NAS_BC_GSM_900_RAILWAYS         = 46,
  QMI_NAS_BC_GSM_1800                 = 47,
  QMI_NAS_BC_GSM_1900                 = 48,
  QMI_NAS_BC_WCDMA_2100               = 80,
  QMI_NAS_BC_WCDMA_PCS_1900           = 81,
  QMI_NAS_BC_WCDMA_DCS_1800           = 82,
  QMI_NAS_BC_WCDMA_1700_US            = 83,
  QMI_NAS_BC_WCDMA_850                = 84,
  QMI_NAS_BC_WCDMA_800                = 85,
  QMI_NAS_BC_WCDMA_2600               = 86,
  QMI_NAS_BC_WCDMA_900                = 87,
  QMI_NAS_BC_WCDMA_1700_JAPAN         = 88,
  QMI_NAS_BC_WCDMA_1500               = 90,
  QMI_NAS_BC_WCDMA_850_JAPAN          = 91,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_1  = 120,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_2  = 121,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_3  = 122,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_4  = 123,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_5  = 124,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_6  = 125,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_7  = 126,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_8  = 127,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_9  = 128,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_10 = 129,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_11 = 130,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_12 = 131,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_13 = 132,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_14 = 133,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_17 = 134,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_33 = 135,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_34 = 136,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_35 = 137,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_36 = 138,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_37 = 139,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_38 = 140,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_39 = 141,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_40 = 142,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_18 = 143,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_19 = 144,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_20 = 145,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_21 = 146,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_24 = 147,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_25 = 148,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_41 = 149,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_42 = 150,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_43 = 151,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_23 = 152,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_26 = 153,
  QMI_NAS_BC_E_UTRA_OPERATING_BAND_66 = 161,
  QMI_NAS_BC_TDSCDMA_BAND_A           = 200,
  QMI_NAS_BC_TDSCDMA_BAND_B           = 201,
  QMI_NAS_BC_TDSCDMA_BAND_C           = 202,
  QMI_NAS_BC_TDSCDMA_BAND_D           = 203,
  QMI_NAS_BC_TDSCDMA_BAND_E           = 204,
  QMI_NAS_BC_TDSCDMA_BAND_F           = 205,
  QMI_NAS_BC_MAX                      = 0xFFFF
};

typedef enum
{
  NASI_PS_UNKNOWN   = 0x00,   /* the mobile is not attached */
  NASI_PS_ATTACHED  = 0x01,   
  NASI_PS_DETACHED  = 0x02
} qmi_nasi_ps_attach_e_type;

enum qmi_nas_register_action_e
{
  QMI_NAS_REGISTER_ACTION_AUTOMATIC = 0x01,
  QMI_NAS_REGISTER_ACTION_MANUAL    = 0x02
};

enum qmi_nas_radio_if_e
{
  QMI_NAS_RADIO_IF_NONE           = 0,
  QMI_NAS_RADIO_IF_CDMA20001X     = 1,
  QMI_NAS_RADIO_IF_CDMA20001XEVDO = 2,
  QMI_NAS_RADIO_IF_GSM            = 4,
  QMI_NAS_RADIO_IF_UMTS           = 5,
  QMI_NAS_RADIO_IF_LTE            = 8,
  QMI_NAS_RADIO_IF_TDSCDMA        = 9,
  QMI_NAS_RADIO_IF_NO_CHANGE      = 0xFF
};

enum qmi_nas_change_duration_e
{
  QMI_NAS_CHANGE_DURATION_POWER_CYCLE = 0x00,
  QMI_NAS_CHANGE_DURATION_PERMANENT   = 0x01
};

#define QMI_NAS_TECH_PREF_AUTOMATIC   (0x0000)
#define QMI_NAS_TECH_PREF_BIT_3GPP2   (0x0001)
#define QMI_NAS_TECH_PREF_BIT_3GPP    (0x0002)
#define QMI_NAS_TECH_PREF_BIT_ANALOG  (0x0004)
#define QMI_NAS_TECH_PREF_BIT_DIGITAL (0x0008)
#define QMI_NAS_TECH_PREF_BIT_HDR     (0x0010)
#define QMI_NAS_TECH_PREF_BIT_LTE     (0x0020)
#define QMI_NAS_TECH_PREF_BIT_ALL     (QMI_NAS_TECH_PREF_BIT_3GPP2 | QMI_NAS_TECH_PREF_BIT_3GPP | QMI_NAS_TECH_PREF_BIT_ANALOG | QMI_NAS_TECH_PREF_BIT_DIGITAL | QMI_NAS_TECH_PREF_BIT_HDR | QMI_NAS_TECH_PREF_BIT_LTE)

enum qmi_nas_tech_pref_duration_e
{
  QMI_NAS_TECH_PREF_DURATION_PERMANENT      = 0x00,
  QMI_NAS_TECH_PREF_DURATION_POWER_CYCLE    = 0x01,
  QMI_NAS_TECH_PREF_DURATION_1_CALL         = 0x02,
  QMI_NAS_TECH_PREF_DURATION_1_CALL_OR_TIME = 0x03,
  QMI_NAS_TECH_PREF_DURATION_CM_1_CALL      = 0x04,
  QMI_NAS_TECH_PREF_DURATION_CM_1_CALL_PLUS = 0x05,
  QMI_NAS_TECH_PREF_DURATION_CM_MO_SMS      = 0x06
};

enum qmi_nas_sys_pref_e
{
  QMI_NAS_SYS_PREF_AUTOMATIC = 0x00,
  QMI_NAS_SYS_PREF_AUTO_A    = 0x01,
  QMI_NAS_SYS_PREF_AUTO_B    = 0x02
};

enum qmi_nas_hdr_personality_e
{
  QMI_NAS_HDR_PERSONALITY_UNKNOWN = 0x00,
  QMI_NAS_HDR_PERSONALITY_HRPD    = 0x01,
  QMI_NAS_HDR_PERSONALITY_EHRPD   = 0x02
};

// Limitations on SYS_INFO_IND reporting
#define QMI_NAS_LIMIT_BY_SRV_STATUS  (0x01ull)
#define QMI_NAS_LIMIT_BY_SRV_DOMAIN  (0x02ull)
#define QMI_NAS_LIMIT_BY_PLMN_ID     (0x04ull)
#define QMI_NAS_LIMIT_BY_SID_NID     (0x08ull)
#define QMI_NAS_LIMIT_BY_ROAM_STATUS (0x10ull)
#define QMI_NAS_LIMIT_BY_SRV_CAPABILITY  (0x20ull) 
#define QMI_NAS_LIMIT_BY_PACKET_ZONE  (0x40ull)
#define QMI_NAS_LIMIT_BY_IS856_SYS_ID  (0x80ull)
#define QMI_NAS_LIMIT_BY_CELL_ID       (0x100ull)
#define QMI_NAS_LIMIT_BY_LAC           (0x200ull)
#define QMI_NAS_LIMIT_BY_RAC           (0x400ull)
#define QMI_NAS_LIMIT_BY_TAC             (0x800ull)
#define QMI_NAS_LIMIT_BY_HS_CALL_STATUS  (0x1000ull)
#define QMI_NAS_LIMIT_BY_HS_IND          (0x2000ull)
#define QMI_NAS_LIMIT_BY_HDR_ACTIVE_PROTOCOL   (0x4000ull)
#define QMI_NAS_LIMIT_BY_EGPRS_SUPPORT_IND     (0x8000ull)
#define QMI_NAS_LIMIT_BY_HDR_PERSONALITY       (0x10000ull)

#define QMI_NAS_LIMIT_BY_CCS_SUPPORTED       (0x20000ull)
#define QMI_NAS_LIMIT_BY_DTM_SUPPORTED       (0x40000ull)
#define QMI_NAS_LIMIT_BY_CS_BAR_STATUS       (0x80000ull)
#define QMI_NAS_LIMIT_BY_PS_BAR_STATUS       (0x100000ull)
#define QMI_NAS_LIMIT_BY_VOICE_SUPPORT_ON_LTE       (0x200000ull)
#define QMI_NAS_LIMIT_BY_SYS_INFO_NO_CHANGE       (0x400000ull)
#define QMI_NAS_LIMIT_BY_IS_SYS_FORBIDDEN       (0x800000ull)
#define QMI_NAS_LIMIT_BY_LTE_EMBMS_COVERAGE       (0x1000000ull)
#define QMI_NAS_LIMIT_BY_LTE_VOICE_STATUS       (0x2000000ull)
#define QMI_NAS_LIMIT_BY_LTE_SMS_STATUS       (0x4000000ull)
#define QMI_NAS_LIMIT_BY_IS_SYS_PRL_MATCH       (0x8000000ull)
#define QMI_NAS_LIMIT_BY_P_REV_IN_USE       (0x10000000ull)
#define QMI_NAS_LIMIT_BY_CDMA_SYS_ID       (0x20000000ull)
#define QMI_NAS_LIMIT_BY_PSC       (0x40000000ull)
#define QMI_NAS_LIMIT_BY_SIM_REJ_INFO       (0x80000000ull)
#define QMI_NAS_LIMIT_BY_WCDMA_EUTRA_STATUS       (0x100000000ull)
#define QMI_NAS_LIMIT_BY_WCDMA_CSG_INFO       (0x200000000ull)
#define QMI_NAS_LIMIT_BY_LTE_CSG_INFO       (0x400000000ull)
#define QMI_NAS_LIMIT_BY_SRV_STATUS_TOGGLE       (0x800000000ull)
#define QMI_NAS_LIMIT_BY_SRV_RAT_TECH_CHANGE    (0x1000000000ull)



#define QMI_NAS_LIMIT_ALL            (0x1FFFFFFFFFull)
#define QMI_NAS_LIMIT_NONE           (0x00ull)

/*---------------------------------------------------------------------------
  Begin message-specific definitions
---------------------------------------------------------------------------*/
#ifdef _WIN32
  #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif


/*---------------------------------------------------------------------------
  QMI_NAS_GET_OPERATOR_NAME_DATA
---------------------------------------------------------------------------*/

#define NAS_0039_RSP_T11_DATA_MAX            64

/*---------------------------------------------------------------------------
  QMI_NAS_SET_EVENT_REPORT
---------------------------------------------------------------------------*/

#define NAS_0002_REQ_T10                                      0x10

#define NAS_0002_REQ_DO_NOT_REPORT 0
#define NAS_0002_REQ_REPORT 1

/*---------------------------------------------------------------------------
  QMI_NAS_PERFORM_NETWORK_SCAN
---------------------------------------------------------------------------*/
#define NAS_PERFORM_NET_SCAN_NW_DESC_MAX_LEN       255 
#define NAS_PERFORM_NET_SCAN_3GPP_NW_INFO_LIST_MAX 40

enum nas_perform_nw_scan_rat_type_e {
 QMI_NAS_PERFORM_NW_SCAN_RAT_TYPE_GERAN = 0x04,
 QMI_NAS_PERFORM_NW_SCAN_RAT_TYPE_UMTS  = 0x05,
 QMI_NAS_PERFORM_NW_SCAN_RAT_TYPE_LTE   = 0x08
};

enum nas_perform_nw_scan_result_type_e {
  QMI_NAS_SCAN_SUCCESS = 0x00, 
  QMI_NAS_SCAN_AS_ABORT = 0x01, 
  QMI_NAS_SCAN_REJ_IN_RLF = 0x02
};

enum nas_perform_nw_scan_csg_list_cat_e {
  QMI_NAS_PERFORM_NW_SCAN_CSG_LIST_CAT_UNKNOWN = 0x00,
  QMI_NAS_PERFORM_NW_SCAN_CSG_LIST_CAT_ALLOWED = 0x01,
  QMI_NAS_PERFORM_NW_SCAN_CSG_LIST_CAT_OPERATOR = 0x02
};


/*---------------------------------------------------------------------------
  QMI_NAS_INITIATE_NETWORK_REGISTER
---------------------------------------------------------------------------*/
#define NAS_0022_REQ_T01        (0x01)
#define NAS_0022_REQ_T10        (0x10)
#define NAS_0022_REQ_T11        (0x11)
#define NAS_0022_REQ_T12        (0x12)

struct nas_0022_req_p_s
{
  PACK(struct) nas_0022_req_t01_p_s
  {
    uint8 register_action; // enum nas_register_action_e
  } t01; // register action
  PACK(struct) nas_0022_req_t10_p_s
  {
    uint16 mobile_country_code;
    uint16 mobile_network_code;
    uint8  radio_access_technology; // enum nas_radio_if_e
  } t10; // Manual Network Register Information
  PACK(struct) nas_0022_req_t11_p_s
  {
    uint8 change_duration; // enum nas_change_duration_e
  } t11; // Change Duration
  PACK(struct) nas_0022_req_t12_p_s
  {
    uint8 mnc_includes_pcs_digit;
  } t12;
};

struct nas_0022_req_s
{
  struct nas_0022_req_t01_s
  {
    uint8 register_action; // enum nas_register_action_e
  } t01; // register action
  struct nas_0022_req_t10_s
  {
    uint16 mobile_country_code;
    uint16 mobile_network_code;
    uint8  radio_access_technology; // enum nas_radio_if_e
  } t10; // Manual Network Register Information
  struct nas_0022_req_t11_s
  {
    uint8 change_duration; // enum nas_change_duration_e
  } t11; // Change Duration
  struct nas_0022_req_t12_s
  {
    uint8 mnc_includes_pcs_digit;
  } t12;

  boolean t01_valid;
  boolean t10_valid;
  boolean t11_valid;
  boolean t12_valid;  
};

#define NAS_0022_RSP_T02        (0x02)

struct nas_0022_rsp_p_s
{
  PACK(struct) nas_0022_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
};

struct nas_0022_rsp_s
{
  struct nas_0022_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  boolean t02_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_INITIATE_ATTACH
---------------------------------------------------------------------------*/
#define NAS_0023_REQ_T10        (0x10)

struct nas_0023_req_p_s {
  PACK(struct) nas_0023_req_t10_p_s {
    uint8 ps_attach_action;
  } t10;
};

struct nas_0023_req_s {
  struct nas_0023_req_t10_s {
    uint8 ps_attach_action;
  } t10;

  boolean t10_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_SET_TECHNOLOGY_PREFERENCE
---------------------------------------------------------------------------*/
#define NAS_002A_REQ_T01        (0x01)

struct nas_002A_req_p_s
{
  PACK(struct) nas_002A_req_t01_p_s
  {
    uint16 technology_pref;
    uint8  duration; // enum nas_tech_pref_duration_e
  } t01; // Technology preference
};

struct nas_002A_req_s
{
  PACK(struct) nas_002A_req_t01_s
  {
    uint16 technology_pref;
    uint8  duration; // enum nas_tech_pref_duration_e
  } t01; // Technology preference

  boolean t01_valid;
};

#define NAS_002A_RSP_T02        (0x02)

struct nas_002A_rsp_p_s
{
  PACK(struct) nas_002A_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
};

struct nas_002A_rsp_s
{
  struct nas_002A_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  boolean t02_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_GET_TECHNOLOGY_PREFERENCE
---------------------------------------------------------------------------*/
#define NAS_002B_RSP_T01        (0x01)
#define NAS_002B_RSP_T02        (0x02)
#define NAS_002B_RSP_T10        (0x10)

struct nas_002B_rsp_p_s
{
  PACK(struct) nas_002B_rsp_t01_p_s
  {
    uint16 active_technology_pref;
    uint8  duration; // enum nas_tech_pref_duration_e
  } t01; // Active technology preference

  PACK(struct) nas_002B_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  PACK(struct) nas_002B_rsp_t10_p_s
  {
    uint16 persistent_technology_pref;
  } t10; // Persistent technology preference
};

struct nas_002B_rsp_s
{
  struct nas_002B_rsp_t01_s
  {
    uint16 active_technology_pref;
    uint8  duration; // enum nas_tech_pref_duration_e
  } t01; // Active technology preference

  struct nas_002B_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  struct nas_002B_rsp_t10_s
  {
    uint16 persistent_technology_pref;
  } t10; // Persistent technology preference

  boolean t01_valid;
  boolean t02_valid;
  boolean t10_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_GET_ACCOLC
---------------------------------------------------------------------------*/
#define NAS_002C_RSP_ACCOLC (0x01)

struct nas_002C_rsp_p_s {
 PACK(struct) nas_002C_rsp_t01_p_s {
   uint8 accolc;
 } t01;
};

struct nas_002C_rsp_s {
  struct nas_002C_rsp_t01_s {
    uint8 accolc;
  } t01;

  boolean t01_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_SET_ACCOLC
---------------------------------------------------------------------------*/
#define NAS_002D_REQ_ACCOLC     (0x01)
#define NAS_002D_REQ_ACCOLC_MAX (15)

struct nas_002D_req_p_s {
  PACK(struct) nas_002D_req_t01_p_s {
      uint8 spc[6];
      uint8 accolc;
  } t01;
};

struct nas_002D_req_s {
  struct nas_002D_req_t01_s {
    uint8 spc[6];
    uint8 accolc;
  } t01;

  boolean t01_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_GET_NETWORK_SYSTEM_PREFERENCE
---------------------------------------------------------------------------*/
#define NAS_002E_RSP_T01        (0x01)
#define NAS_002E_RSP_T02        (0x02)

struct nas_002E_rsp_p_s
{
  PACK(struct) nas_002E_rsp_t01_p_s
  {
    uint8 sys_pref; // enum nas_sys_pref_e
  } t01; // System Preference

  PACK(struct) nas_002E_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
};

struct nas_002E_rsp_s
{
  struct nas_002E_rsp_t01_s
  {
    uint8 sys_pref; // enum nas_sys_pref_e
  } t01; // System Preference

  struct nas_002E_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  boolean t01_valid;
  boolean t02_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_GET_DEVICE_CONFIG
---------------------------------------------------------------------------*/
#define NAS_002F_RSP_SPC                    (0x10)
#define NAS_002F_RSP_SCI                    (0x11)
#define NAS_002F_RSP_SCM                    (0x12)
#define NAS_002F_RSP_REG_PARAMS             (0x13)
#define NAS_002F_RSP_FORCE_REV0             (0x14)
#define NAS_002F_RSP_HDR_SCP_CFG            (0x15)
#define NAS_002F_RSP_ROAM_PREF              (0x16)
#define NAS_002F_RSP_FORCE_HDRSCP_AT_CONFIG (0x17)

//roam preference
typedef enum
{
  NAS_002F_ROAM_PREF_AUTO      = 0x00,
  NAS_002F_ROAM_PREF_HOME_ONLY = 0x01,
  NAS_002F_ROAM_PREF_ROAM_ONLY = 0x02,
  NAS_002F_ROAM_PREF_AFFIL     = 0x03,
  NAS_002F_ROAM_PREF_MAX,
  NAS_002F_ROAM_PREF_MAX32     = 0x10000000
} nas_002F_roam_pref_e_type;

struct nas_002F_rsp_p_s {
  PACK(struct) nas_002F_rsp_t11_p_s {
    uint8 sci;
  } t11;
  PACK(struct) nas_002F_rsp_t12_p_s {
    uint8 scm;
  } t12;
  PACK(struct) nas_002F_rsp_t13_p_s {
    uint8 reg_home_sid;
    uint8 reg_foreign_sid;
    uint8 reg_foreign_nid;
  } t13;
  PACK(struct) nas_002F_rsp_t14_p_s {
    uint8 force_rev0;
  } t14;
  PACK(struct) nas_002F_rsp_t15_p_s {
    uint8   state;
    uint32  protocol_mask;
    uint32  broadcast_mask;
    uint32  application_mask;
  } t15;
  PACK(struct) nas_002F_rsp_t16_p_s {
    uint8 roam_pref;
  } t16;
  PACK(struct) nas_002F_req_t17_p_s {
    uint8 force_hdrscp_at_config;
  } t17;
};

struct nas_002F_rsp_s {
  struct nas_002F_rsp_t11_s {
    uint8 sci;
  } t11;
  struct nas_002F_rsp_t12_s {
    uint8 scm;
  } t12;
  struct nas_002F_rsp_t13_s {
    uint8 reg_home_sid;
    uint8 reg_foreign_sid;
    uint8 reg_foreign_nid;
  } t13;
  struct nas_002F_rsp_t14_s {
    uint8 force_rev0;
  } t14;
  struct nas_002F_rsp_t15_s {
    uint8   state;
    uint32  protocol_mask;
    uint32  broadcast_mask;
    uint32  application_mask;
  } t15;
  struct nas_002F_rsp_t16_s {
    uint8 roam_pref;
  } t16;
  struct nas_002F_req_t17_s {
    uint8 force_hdrscp_at_config;
  }t17;

  boolean t11_valid;
  boolean t12_valid;
  boolean t13_valid;
  boolean t14_valid;
  boolean t15_valid;
  boolean t16_valid;
  boolean t17_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_GET_RF_BAND_INFO
---------------------------------------------------------------------------*/
#define NAS_0031_RSP_T01             (0x01)
#define NAS_0031_RSP_T11             (0x11)
#define NAS_0031_RSP_MAX_RADIO_IFS   (2)

struct nas_0031_rsp_p_s {
  PACK(struct) nas_0031_rsp_t01_p_s {
    uint8 num_instances;
    PACK(struct)
    {
      uint8   radio_if;
      uint16  active_band;
      uint16  active_channel;
    } instances[NAS_0031_RSP_MAX_RADIO_IFS];
  } t01;
  PACK(struct) nas_0031_rsp_t11_p_s {
    uint8 num_instances;
    PACK(struct)
    {
      uint8   radio_if;
      uint16  active_band;
      uint32  active_channel;
    } instances[NAS_0031_RSP_MAX_RADIO_IFS];
  } t11;

  boolean t01_valid;
  boolean t11_valid;
};

struct nas_0031_rsp_s {
  struct nas_0031_rsp_t01_s {
    uint8 num_instances;
    struct
    {
      uint8   radio_if;
      uint16  active_band;
      uint16  active_channel;
    } instances[NAS_0031_RSP_MAX_RADIO_IFS];
  } t01;
  struct nas_0031_rsp_t11_s {
    uint8 num_instances;
    struct
    {
      uint8   radio_if;
      uint16  active_band;
      uint32  active_channel;
    } instances[NAS_0031_RSP_MAX_RADIO_IFS];
  } t11;

  boolean t01_valid;
  boolean t11_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_GET_AN_AAA_STATUS
---------------------------------------------------------------------------*/
#define NAS_0032_RSP_T01      (0x01)

//AN AAA (Authentication, Authorization and Accounting) status definition
typedef enum
{
  NAS_0032_AN_AAA_STATUS_FAILED   = 0, /* Failed authentication */
  NAS_0032_AN_AAA_STATUS_SUCCESS,      /* Authenticated successfully */
  NAS_0032_AN_AAA_STATUS_NOT_AUTH      /* Not Authenticated */
} nas_0032_an_aaa_status_e_type;

struct nas_0032_rsp_p_s {
  PACK(struct) nas_0032_rsp_t01_p_s {
    uint8 an_aaa_status;
  } t01;
};

struct nas_0032_rsp_s {
  struct nas_0032_rsp_t01_s {
    uint8 an_aaa_status;
  } t01;

  boolean t01_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE (0x0033)
---------------------------------------------------------------------------*/

typedef enum
{
  NAS_0033_EMERGENCY_MODE_OFF     = 0x00,
  NAS_0033_EMERGENCY_MODE_ON      = 0x01
} nas_0033_emergency_mode_e_type;

// replaces #define NAS_0033_MODE_PREF_BIT_ALL 0x003F
#define QMI_NAS_RAT_MODE_PREF_ALL ( QMI_NAS_RAT_MODE_PREF_CDMA2000_1X_V01 | QMI_NAS_RAT_MODE_PREF_CDMA2000_HRPD_V01 | \
                                    QMI_NAS_RAT_MODE_PREF_GSM_V01         | QMI_NAS_RAT_MODE_PREF_UMTS_V01          | \
                                    QMI_NAS_RAT_MODE_PREF_LTE_V01         | QMI_NAS_RAT_MODE_PREF_TDSCDMA_V01       )


/*---------------------------------------------------------------------------
  QMI_NAS_GET_CSP_PLMN_MODE_BIT
---------------------------------------------------------------------------*/

#define NAS_003B_RSP_T10                           (0x10)
#define NAS_003B_RSP_T10_PLMN_MODE_DO_NOT_RESTRICT (0)
#define NAS_003B_RSP_T10_PLMN_MODE_RESTRICT        (1)

struct nas_003B_rsp_s {
  PACK (struct) nas_003B_rsp_t10_s {                    // 18
    uint8 plmn_mode;
  } t10;

  boolean t10_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_UPDATE_AKEY
---------------------------------------------------------------------------*/
#define NAS_003D_REQ_AKEY                 (0x01)

#define NAS_003D_REQ_AKEY_ASCII_MAX_SIZE  (26)

struct nas_003D_req_p_s
{
  PACK(struct) nas_003D_req_t01_p_s {
    uint8 akey[NAS_003D_REQ_AKEY_ASCII_MAX_SIZE];
  } t01;
};

struct nas_003D_req_s
{
  struct nas_003D_req_t01_s {
    uint8 akey[NAS_003D_REQ_AKEY_ASCII_MAX_SIZE];
  } t01;

  boolean t01_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_GET_3GPP2_SUBSCRIPTION_INFO
---------------------------------------------------------------------------*/

#define NAS_003E_REQ_NAM            (0x01)
#define NAS_003E_REQ_MASK           (0x10)
#define NAS_003E_REQ_CURR_NAM_MASK  (NASI_CURR_NAM_MASK)

#define NAS_003E_RSP_MCC_LEN        (NASI_MCC_LEN)
#define NAS_003E_RSP_11_12_LEN      (NASI_11_12_LEN)
#define NAS_003E_RSP_MIN1_LEN       (NASI_MIN1_LEN)
#define NAS_003E_RSP_MIN2_LEN       (NASI_MIN2_LEN)
#define NAS_003E_RSP_MDN_LEN        (NASI_MDN_MAX_LEN)



/*---------------------------------------------------------------------------
  QMI_NAS_SET_3GPP2_SUBSCRIPTION_INFO
---------------------------------------------------------------------------*/

#define NAS_003F_REQ_PRL_IS_IN_CHAN_CELLULAR_SYS_A( chan ) \
  ( ((chan) >= 1      &&  (chan) <= 333)  ||            \
    ((chan) >= 667    &&  (chan) <= 716)  ||            \
        ((chan) >= 991    &&  (chan) <= 1323) )

#define NAS_003F_REQ_PRL_IS_IN_CHAN_CELLULAR_SYS_B( chan ) \
  ( ((chan) >= 334    && (chan) <= 666) ||              \
        ((chan) >= 717    && (chan) <= 799) )
        
#define NAS_003F_REQ_CM_BAND_PREF_JPN \
  ( CM_BAND_PREF_WCDMA_VI_800 |   \
    CM_BAND_PREF_WCDMA_I_IMT_2000 )

/*---------------------------------------------------------------------------
  QMI_NAS_GET_MOB_CAI_REV
---------------------------------------------------------------------------*/

#define NAS_0040_REQ_MOB_CAI_REV (0x10)

struct nas_0040_rsp_p_s
{
  PACK(struct) nas_0040_rsp_t10_p_s
  {
    uint8 cai_rev;
  } t10;
};
  
struct nas_0040_rsp_s
{
  struct nas_0040_rsp_t10_s
  {
    uint8 cai_rev;
  } t10;

  boolean t10_valid;
};
  
/*---------------------------------------------------------------------------
  QMI_NAS_GET_RTRE_CONFIG
---------------------------------------------------------------------------*/

#define NAS_0041_RSP_RTRE_CONFIG       (0x10)
#define NAS_0041_RSP_RTRE_CONFIG_PREF  (0x11)

typedef enum {
  NAS_RTRE_CFG_RUIM_ONLY              = 0x01,
  NAS_RTRE_CFG_INTERNAL_SETTINGS_ONLY = 0x02,
  NAS_RTRE_CFG_RUIM_IF_AVAIL          = 0x03,
  NAS_RTRE_CFG_GSM_ON_1X              = 0x04
} nas_rtre_cfg_e_type;

struct nas_0041_rsp_p_s
{
  PACK(struct) nas_0041_rsp_t10_p_s
  {
    uint8 rtre_cfg;
  } t10;
  
  PACK(struct) nas_0041_rsp_t11_p_s
  {
    uint8 rtre_cfg_pref;
  } t11;
};
  
struct nas_0041_rsp_s
{
  struct nas_0041_rsp_t10_s
  {
    uint8 rtre_cfg;  // rtre_control  value return control
  } t10;
  struct nas_0041_rsp_t11_s
  {
    uint8 rtre_cfg_pref;  // rtre_control  value return preference
  } t11;

  boolean t10_valid;
  boolean t11_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_SET_RTRE_CONFIG
---------------------------------------------------------------------------*/
#define NASI_RTRE_MAX                       (0x05)

/*---------------------------------------------------------------------------
  QMI_NAS_GET_CELL_LOCATION_INFO
---------------------------------------------------------------------------*/

// THE STANDARD:  Timing adjustment is done in multiples of 0.52 microseconds.
// When the TA command is transmitted as part of the random-access response,
// an 11-bit TA command with possible values of TA=0, 1, 2, ...,1282 is used.
// This provides timing adjustment from 0 to 667 usecs in steps of 0.52μs,
// equivalent to a maximum supportable cell radius of 100km.

// *NOTE:  Implementations may extend the LTE TA, as desired.

// BUT: 0x0043 TLV 0x1E is in microseconds (us), giving the range 0..667.

#define NAS_0043_RSP_T1E_LTE_TIMING_ADVANCE_MAX  INT32_MAX  // 667, see NOTE above.

/*---------------------------------------------------------------------------
  QMI_NAS_GET_PLMN_NAME 
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  QMI_NAS_BIND_SUBSCRIPTION
---------------------------------------------------------------------------*/
#define NAS_0045_REQ_T01                        (0x01)
#define NAS_0045_REQ_T01_PRIMARY_SUBSCRIPTION   (0x00)
#define NAS_0045_REQ_T01_SECONDARY_SUBSCRIPTION (0x01)
#define NAS_0045_REQ_T01_TERTIARY_SUBSCRIPTION  (0x02)



struct nas_0045_req_p_s
{
  PACK(struct) nas_0045_req_t01_p_s
  {
    uint8 subscription_type;
  } t01;
};

struct nas_0045_req_s
{
  struct nas_0045_req_t01_s
  {
    uint8 subscription_type;
  } t01;
  
  boolean t01_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_MANAGED_ROAMING_IND
---------------------------------------------------------------------------*/
#define NAS_0046_IND_T10      (0x10)

struct nas_0046_ind_p_s
{
  PACK(struct) nas_0046_ind_t10_p_s
  {
    uint8 radio_if;
  } t10;
};

struct nas_0046_ind_s
{
  struct nas_0046_ind_t10_s
  {
    uint8 radio_if;
  } t10;

  boolean t10_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_DUAL_STANDBY_PREF_IND
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  QMI_NAS_SUBSCRIPTION_INFO_IND
---------------------------------------------------------------------------*/

enum nas_is_priority_subs_enum_e
{
  NAS_PRIORITY_SUBSCRIPTION_FALSE = 0x00,
  NAS_PRIORITY_SUBSCRIPTION_TRUE  = 0x01
};

enum nas_active_subs_info_enum_e
{
  NAS_SUBSCRIPTION_ACTIVE_FALSE = 0x00,
  NAS_SUBSCRIPTION_ACTIVE_TRUE  = 0x01
};

/*---------------------------------------------------------------------------
  QMI_NAS_GET_MODE_PREF
---------------------------------------------------------------------------*/
#define NAS_0049_RSP_T02        (0x02)
#define NAS_0049_RSP_T10        (0x10)
#define NAS_0049_RSP_T11        (0x11)

struct nas_0049_rsp_p_s
{
  PACK(struct) nas_0049_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
  PACK(struct) nas_0049_rsp_t10_p_s
  {
    uint16 mode_pref_0;
  } t10; // Mode Pref 0
  PACK(struct) nas_0049_rsp_t11_p_s
  {
    uint16 mode_pref_1;
  } t11; // Mode Pref 1
};

struct nas_0049_rsp_s
{
  struct nas_0049_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
  struct nas_0049_rsp_t10_s
  {
    uint16 mode_pref_0;
  } t10; // Mode Pref 0
  struct nas_0049_rsp_t11_s
  {
    uint16 mode_pref_1;
  } t11; // Mode Pref 1

  boolean t02_valid;
  boolean t10_valid;
  boolean t11_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_THERMAL_EMERGENCY_STATE_IND
---------------------------------------------------------------------------*/
#define NAS_004A_IND_T01      (0x01)

enum nas_thermal_emergency_mode_e
{
  NAS_THERMAL_EMERGENCY_MODE_NORMAL    = 0,
  NAS_THERMAL_EMERGENCY_MODE_EMERGENCY = 1
};

struct nas_004A_ind_p_s
{
  PACK(struct) nas_004A_ind_t01_p_s
  {
    uint8 thermal_emergency_mode; // enum nas_thermal_emergency_mode_e
  } t01;
};

struct nas_004A_ind_s
{
  struct nas_004A_ind_t01_s
  {
    uint8 thermal_emergency_mode; // enum nas_thermal_emergency_mode_e
  } t01;

  boolean t01_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_SET_DUAL_STANDBY_PREF_NEW
---------------------------------------------------------------------------*/
#define NAS_004B_REQ_PRIMARY_SUBSCRIPTION            (0x00)
#define NAS_004B_REQ_SECONDARY_SUBSCRIPTION          (0x01)
#define NAS_004B_REQ_TERTIARY_SUBSCRIPTION           (0x02)
#define NAS_004B_REQ_T10_AUTOMATIC_WITH_TUNE_AWAY    (0x05)
#define NAS_004B_REQ_T10_AUTOMATIC_WITHOUT_TUNE_AWAY (0x06)

/*---------------------------------------------------------------------------
  QMI_NAS_NETWORK_TIME_IND
---------------------------------------------------------------------------*/
#define NAS_004C_IND_T01      (0x01)
#define NAS_004C_IND_T10      (0x10)
#define NAS_004C_IND_T11      (0x11)
#define NAS_004C_IND_T12      (0x12)

enum nas_day_of_week_e
{
  NAS_DAY_OF_WEEK_MON = 0,
  NAS_DAY_OF_WEEK_TUE = 1,
  NAS_DAY_OF_WEEK_WED = 2,
  NAS_DAY_OF_WEEK_THU = 3,
  NAS_DAY_OF_WEEK_FRI = 4,
  NAS_DAY_OF_WEEK_SAT = 5,
  NAS_DAY_OF_WEEK_SUN = 6
};

enum nas_daylt_sav_adj_e
{
  NAS_DAYLT_SAV_ADJ_NONE = 0,
  NAS_DAYLT_SAV_ADJ_1HR  = 1,
  NAS_DAYLT_SAV_ADJ_2HR  = 2
};

struct nas_004C_ind_p_s
{
  // Universal Time
  PACK(struct) nas_004C_ind_t01_p_s
  {
    uint16 year;
    uint8  month;
    uint8  day;
    uint8  hour;
    uint8  minute;
    uint8  second;
    uint8  day_of_week; // enum nas_day_of_week_e
  } t01;
  // time_zone: Offset from Universal time, i.e., difference between local time
  // and Universal time, in increments of 15 min. (signed value)
  PACK(struct) nas_004C_ind_t10_p_s
  {
    int8 time_zone;
  } t10;
  // daylt_sav_adj: Daylight Saving Adjustment
  PACK(struct) nas_004C_ind_t11_p_s
  {
    uint8 daylt_sav_adj; // enum nas_daylt_sav_adj_e
  } t11;

  PACK(struct) nas_004C_ind_t12_p_s
  {
    uint8 radio_if;
  } t12;
};

struct nas_004C_ind_s
{
  // Universal Time
  struct nas_004C_ind_t01_s
  {
    uint16 year;
    uint8  month;
    uint8  day;
    uint8  hour;
    uint8  minute;
    uint8  second;
    uint8  day_of_week; // enum nas_day_of_week_e
  } t01;
  // time_zone: Offset from Universal time, i.e., difference between local time
  // and Universal time, in increments of 15 min. (signed value)
  struct nas_004C_ind_t10_s
  {
    int8 time_zone;
  } t10;
  // daylt_sav_adj: Daylight Saving Adjustment
  struct nas_004C_ind_t11_s
  {
    uint8 daylt_sav_adj; // enum nas_daylt_sav_adj_e
  } t11;

  struct nas_004C_ind_t12_s
  {
    uint8 radio_if;
  } t12;

  boolean t01_valid;
  boolean t10_valid;
  boolean t11_valid;
  boolean t12_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_GET_SYS_INFO (004D)
---------------------------------------------------------------------------*/

// network_access_service_v01.h and common_v01.h equivalent NOT available, but this is still in use.

#define NAS_004D_CDMA_REG_ZONE_MASK          0x0FFF

typedef enum
{
  NAS_VOICE_SMS_STATUS_NONE = 0, 
  NAS_VOICE_SMS_STATUS_IMS  = 1,
  NAS_VOICE_SMS_STATUS_1X   = 2,
  NAS_VOICE_SMS_STATUS_3GPP = 3 
}nas_voice_sms_status_e_type;

/*---------------------------------------------------------------------------
  QMI_NAS_HDR_SESSION_CLOSE_IND
---------------------------------------------------------------------------*/
enum nas_hdr_session_close_reason_type {
   NAS_HDR_CLOSE_REASON_NEW_NETWORK = 0, ///< AMPfail: reacquired on a new network
   NAS_HDR_CLOSE_REASON_UATI_FAIL,   ///< AMPfail: timed out 5x waiting for UATI rsp
   NAS_HDR_CLOSE_REASON_KA_EXP,      ///< KeepAliveTimer not reset for TsmpClose min
   NAS_HDR_CLOSE_REASON_DEACTIVATE,  ///< Internal Deactivation
   NAS_HDR_CLOSE_REASON_REPLY,       ///< Rxed a session close msg from the AN
   NAS_HDR_CLOSE_REASON_CONN_OPEN_FAIL,
      ///< Failed to establish a connection 5x to send a session config message
   NAS_HDR_CLOSE_REASON_CFG_MSG_FAIL, ///< in ATInit: Couldn't send Config msg
   NAS_HDR_CLOSE_REASON_CFG_RSP_EXP,
      ///< in ATInit: Timed out waiting for config response
   NAS_HDR_CLOSE_REASON_PROT_NEG_FAIL, ///< in ATInit: Bad Config Response from AN
   NAS_HDR_CLOSE_REASON_AN_INIT_EXP,   ///< in ANInit: AN init setup timer expired
   NAS_HDR_CLOSE_REASON_QUICK_FAILURE, ///< in ANInit: Connection closed in ANInit
   NAS_HDR_CLOSE_REASON_CONN_OPEN_DENY,
      ///< Failed to establish a connection 5x for sending config message.
      ///< Rxed a connection deny at least once from the network.
   NAS_HDR_CLOSE_REASON_SILENT_DEACTIVATE, ///< Internal Silent Deactivation
   NAS_HDR_CLOSE_REASON_NEW_ESN,
      ///< AMPfail: phone ESN different from ESN associated with our
      ///< current session
   NAS_HDR_CLOSE_REASON_AN_GAUP_FAIL, ///< AT Rejected AN GAUP message
   NAS_HDR_CLOSE_REASON_PERSONALITY_INDEX_INVALID,
      ///< AN included invalid personality index in SoftCC message
   NAS_HDR_CLOSE_REASON_NOT_MAINT_UATI,
      ///< AMP: close the session due to not maintaining the UATI.
   NAS_HDR_CLOSE_REASON_NEW_NAI,
      ///< phone NAI different from NAI associated with our current session
   NAS_HDR_CLOSE_REASON_EHRPD_CREDENTIALS_CHANGED,
      ///< eHRPD credentials (IMSI, EAP AKA, OP) has changed
   NAS_HDR_CLOSE_REASON_MAX
};

struct nas_0054_ind_p_s
{
  // Session Close Reason
  PACK(struct) nas_0054_ind_t01_p_s
  {
    uint32  close_reason;
  } t01;
};

struct nas_0054_ind_s
{
  // Session Close Reason
  struct nas_0054_ind_t01_s
  {
    uint32  close_reason;
  } t01;

  boolean t01_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_HDR_UATI_IND
---------------------------------------------------------------------------*/
#define NAS_UATI_LENGTH (16)

struct nas_0055_ind_p_s
{
  // UATI
  PACK(struct) nas_0055_ind_t01_p_s
  {
    uint8  uati[NAS_UATI_LENGTH];
  } t01;
};

struct nas_0055_ind_s
{
  // UATI
  struct nas_0055_ind_t01_s
  {
    uint8  uati[NAS_UATI_LENGTH];
  } t01;

  boolean t01_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_GET_HDR_SUBTYPE
---------------------------------------------------------------------------*/

struct nas_0056_req_p_s
{
  PACK(struct) nas_0056_req_t01_p_s
  {
    uint32 protocol;
  } t01;
};

struct nas_0056_req_s
{
  struct nas_0056_req_t01_s
  {
    uint32 protocol;
  } t01;
  
  boolean t01_valid;
};

struct nas_0056_rsp_p_s {
  PACK(struct) nas_0056_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
  PACK(struct) nas_0056_rsp_t01_p_s {
    uint16 subtype;
  } t10;
};

struct nas_0056_rsp_s {
  struct nas_0056_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
  struct nas_0056_rsp_t01_s {
    uint16 subtype;
  } t10;

  boolean t02_valid;
  boolean t10_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_GET_HDR_COLOR_CODE
---------------------------------------------------------------------------*/
struct nas_0057_rsp_p_s {
  PACK(struct) nas_0057_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
  PACK(struct) nas_0057_rsp_t01_p_s {
    uint8 color_code;
  } t10;
};

struct nas_0057_rsp_s {
  struct nas_0057_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
  struct nas_0057_rsp_t01_s {
    uint8 color_code;
  } t10;

  boolean t02_valid;
  boolean t10_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_SET_RX_DIVERSITY
---------------------------------------------------------------------------*/
struct nas_0059_req_s
{
  struct nas_0059_req_t01_s
  {
    uint8 radio_if;
    uint8 rx_chain_bitmask;
  } t01;

  boolean t01_valid;
};

struct nas_0059_req_p_s
{
  PACK(struct) nas_0059_req_t01_p_s
  {
    uint8 radio_if;
    uint8 rx_chain_bitmask;
  } t01;
};

struct nas_0059_rsp_s
{
  struct nas_0059_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  boolean t02_valid;
};

struct nas_0059_rsp_p_s
{
  PACK(struct) nas_0059_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
};

/*---------------------------------------------------------------------------
  QMI_NAS_SET_TX_RX_INFO
---------------------------------------------------------------------------*/
struct nas_005A_req_s
{
  struct nas_005A_req_t01_s
  {
    uint8 radio_if;
  } t01;

  boolean t01_valid;
};

struct nas_005A_req_p_s
{
  PACK(struct) nas_005A_req_t01_p_s
  {
    uint8 radio_if;
  } t01;
};

struct nas_005A_rsp_s
{
  struct nas_005A_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  struct nas_005A_rsp_t10_s
  {
    uint8  is_radio_tuned;
    int32  rx_pwr; /**< RX power value in 1/10 dbm resolution */
    int32  ecio; /**< Ec/Io in 1/10 dbm. Valid for CDMA/HDR/GSM/WCDMA/LTE */
    int32  rscp; /**< Received Signal Code Power in 1/10 dbm. Valid for WCDMA. */
    int32  rsrp; /**< Current Reference Signal Received Power in 1/10 dbm. Valid for LTE. */
    uint32 phase; /**< Phase in 1/100 degrees. 0xFFFFFFFF when phase is unknown. Valid for LTE. */
  } t10; // rx chain 0 info

  struct nas_005A_rsp_t11_s
  {
    uint8  is_radio_tuned;
    int32  rx_pwr; /**< RX power value in 1/10 dbm resolution */
    int32  ecio; /**< Ec/Io in 1/10 dbm. Valid for CDMA/HDR/GSM/WCDMA/LTE */
    int32  rscp; /**< Received Signal Code Power in 1/10 dbm. Valid for WCDMA. */
    int32  rsrp; /**< Current Reference Signal Received Power in 1/10 dbm. Valid for LTE. */
    uint32 phase; /**< Phase in 1/100 degrees. 0xFFFFFFFF when phase is unknown. Valid for LTE. */
  } t11; // rx chain 1 info

  struct nas_005A_rsp_t12_s
  {
    uint8  is_in_traffic;
    int32  tx_pwr; /**< TX power value in 1/10 dbm */
  } t12; // tx info

  struct nas_005A_rsp_t13_s
  {
    uint8  downlink_mod_len;
    uint32 downlink_mod[NAS_LTE_MAX_NUM_LAYER]; /**< Enum representing the LTE downlink modulation. */
  } t13; // LTE Downlink Modulation

  struct nas_005A_rsp_t14_s
  {
    uint8  uplink_mod_len;
    uint32 uplink_mod[NAS_LTE_MAX_NUM_LAYER]; /**< Enum representing the LTE uplink modulation. */
  } t14; // LTE Uplink Modulation

  struct nas_005A_rsp_t15_s
  {
    uint8  is_radio_tuned;
    int32  rx_pwr; /**< RX power value in 1/10 dbm resolution */
    int32  ecio; /**< Ec/Io in 1/10 dbm. Valid for CDMA/HDR/GSM/WCDMA/LTE */
    int32  rscp; /**< Received Signal Code Power in 1/10 dbm. Valid for WCDMA. */
    int32  rsrp; /**< Current Reference Signal Received Power in 1/10 dbm. Valid for LTE. */
    uint32 phase; /**< Phase in 1/100 degrees. 0xFFFFFFFF when phase is unknown. Valid for LTE. */
  } t15; // rx chain 1 info

  struct nas_005A_rsp_t16_s
  {
    uint8  is_radio_tuned;
    int32  rx_pwr; /**< RX power value in 1/10 dbm resolution */
    int32  ecio; /**< Ec/Io in 1/10 dbm. Valid for CDMA/HDR/GSM/WCDMA/LTE */
    int32  rscp; /**< Received Signal Code Power in 1/10 dbm. Valid for WCDMA. */
    int32  rsrp; /**< Current Reference Signal Received Power in 1/10 dbm. Valid for LTE. */
    uint32 phase; /**< Phase in 1/100 degrees. 0xFFFFFFFF when phase is unknown. Valid for LTE. */
  } t16; // rx chain 1 info

  boolean t02_valid;
  boolean t10_valid;
  boolean t11_valid;
  boolean t12_valid;
  boolean t13_valid;
  boolean t14_valid;
  boolean t15_valid;
  boolean t16_valid;
};

struct nas_005A_rsp_p_s
{
  PACK(struct) nas_005A_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  PACK(struct) nas_005A_rsp_t10_p_s
  {
    uint8  is_radio_tuned;
    int32  rx_pwr; /**< RX power value in 1/10 dbm resolution */
    int32  ecio; /**< Ec/Io in 1/10 dbm. Valid for CDMA/HDR/GSM/WCDMA/LTE */
    int32  rscp; /**< Received Signal Code Power in 1/10 dbm. Valid for WCDMA. */
    int32  rsrp; /**< Current Reference Signal Received Power in 1/10 dbm. Valid for LTE. */
    uint32 phase; /**< Phase in 1/100 degrees. 0xFFFFFFFF when phase is unknown. Valid for LTE. */
  } t10; // rx chain 0 info

  PACK(struct) nas_005A_rsp_t11_p_s
  {
    uint8  is_radio_tuned;
    int32  rx_pwr; /**< RX power value in 1/10 dbm resolution */
    int32  ecio; /**< Ec/Io in 1/10 dbm. Valid for CDMA/HDR/GSM/WCDMA/LTE */
    int32  rscp; /**< Received Signal Code Power in 1/10 dbm. Valid for WCDMA. */
    int32  rsrp; /**< Current Reference Signal Received Power in 1/10 dbm. Valid for LTE. */
    uint32 phase; /**< Phase in 1/100 degrees. 0xFFFFFFFF when phase is unknown. Valid for LTE. */
  } t11; // rx chain 1 info

  PACK(struct) nas_005A_rsp_t12_p_s
  {
    uint8  is_in_traffic;
    int32  tx_pwr; /**< TX power value in 1/10 dbm */
  } t12; // tx info

  PACK(struct) nas_005A_rsp_t13_p_s
  {
    uint8  downlink_mod_len;
    uint32 downlink_mod[NAS_LTE_MAX_NUM_LAYER]; /**< Enum representing the LTE downlink modulation. */
  } t13; // LTE Downlink Modulation

  PACK(struct) nas_005A_rsp_t14_p_s
  {
    uint8  uplink_mod_len;
    uint32 uplink_mod[NAS_LTE_MAX_NUM_LAYER]; /**< Enum representing the LTE uplink modulation. */
  } t14; // LTE Uplink Modulation

  PACK(struct) nas_005A_rsp_t15_p_s
  {
    uint8  is_radio_tuned;
    int32  rx_pwr; /**< RX power value in 1/10 dbm resolution */
    int32  ecio; /**< Ec/Io in 1/10 dbm. Valid for CDMA/HDR/GSM/WCDMA/LTE */
    int32  rscp; /**< Received Signal Code Power in 1/10 dbm. Valid for WCDMA. */
    int32  rsrp; /**< Current Reference Signal Received Power in 1/10 dbm. Valid for LTE. */
    uint32 phase; /**< Phase in 1/100 degrees. 0xFFFFFFFF when phase is unknown. Valid for LTE. */
  } t15; // rx chain 1 info

  PACK(struct) nas_005A_rsp_t16_p_s
  {
    uint8  is_radio_tuned;
    int32  rx_pwr; /**< RX power value in 1/10 dbm resolution */
    int32  ecio; /**< Ec/Io in 1/10 dbm. Valid for CDMA/HDR/GSM/WCDMA/LTE */
    int32  rscp; /**< Received Signal Code Power in 1/10 dbm. Valid for WCDMA. */
    int32  rsrp; /**< Current Reference Signal Received Power in 1/10 dbm. Valid for LTE. */
    uint32 phase; /**< Phase in 1/100 degrees. 0xFFFFFFFF when phase is unknown. Valid for LTE. */
  } t16; // rx chain 1 info
};


/*---------------------------------------------------------------------------
  QMI_NAS_UPDATE_AKEY_EXT
---------------------------------------------------------------------------*/
#define NAS_005B_REQ_AKEY_EXT                 (0x01)

#define NAS_005B_REQ_AKEY_EXT_ASCII_MAX_SIZE  (26)
#define NAS_005B_REQ_AKEY_EXT_SPC_SIZE        (6)

struct nas_005B_req_p_s
{
  PACK(struct) nas_005B_req_t01_p_s {
    uint8 spc[NAS_005B_REQ_AKEY_EXT_SPC_SIZE];
    uint8 akey[NAS_005B_REQ_AKEY_EXT_ASCII_MAX_SIZE];
  } t01;
};

struct nas_005B_req_s
{
  struct nas_005B_req_t01_s {
    uint8 spc[NAS_005B_REQ_AKEY_EXT_SPC_SIZE];
    uint8 akey[NAS_005B_REQ_AKEY_EXT_ASCII_MAX_SIZE];
  } t01;

  boolean t01_valid;
};

/*---------------------------------------------------------------------------
  QMI_NAS_DETACH_LTE
---------------------------------------------------------------------------*/
struct nas_005D_rsp_s
{
  struct nas_005D_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  boolean t02_valid;
};

struct nas_005D_rsp_p_s
{
  PACK(struct) nas_005D_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
};

/*---------------------------------------------------------------------------
  QMI_NAS_BLOCK_LTE_PLMN
---------------------------------------------------------------------------*/
struct nas_005E_req_s
{
  struct nas_005E_req_t01_s
  {
    uint16 mcc;
    uint16 mnc;
    uint8  mnc_includes_pcs_digit;
  } t01; // PLMN

  struct nas_005E_req_t10_s
  {
    uint32 blocking_interval_abs;
  } t10; // Blocking interval absolute time

  struct nas_005E_req_t11_s
  {
    float blocking_interval_mult;
  } t11; // Blocking interval T3204 multiplier

  boolean t01_valid;
  boolean t10_valid;
  boolean t11_valid;
};

struct nas_005E_req_p_s
{
  PACK(struct) nas_005E_req_t01_p_s
  {
    uint16 mcc;
    uint16 mnc;
    uint8  mnc_includes_pcs_digit;
  } t01; // PLMN

  PACK(struct) nas_005E_req_t10_p_s
  {
    uint32 blocking_interval_abs;
  } t10; // Blocking interval absolute time

  PACK(struct) nas_005E_req_t11_p_s
  {
    float blocking_interval_mult;
  } t11; // Blocking interval T3204 multiplier
};

struct nas_005E_rsp_s
{
  struct nas_005E_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  boolean t02_valid;
};

struct nas_005E_rsp_p_s
{
  PACK(struct) nas_005E_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
};

/*---------------------------------------------------------------------------
  QMI_NAS_UNBLOCK_LTE_PLMN
---------------------------------------------------------------------------*/
struct nas_005F_req_s
{
  struct nas_005F_req_t01_s
  {
    uint16 mcc;
    uint16 mnc;
    uint8  mnc_includes_pcs_digit;
  } t01; // PLMN

  boolean t01_valid;
};

struct nas_005F_req_p_s
{
  PACK(struct) nas_005F_req_t01_p_s
  {
    uint16 mcc;
    uint16 mnc;
    uint8  mnc_includes_pcs_digit;
  } t01; // PLMN
};

struct nas_005F_rsp_s
{
  struct nas_005F_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  boolean t02_valid;
};

struct nas_005F_rsp_p_s
{
  PACK(struct) nas_005F_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
};

/*---------------------------------------------------------------------------
  QMI_NAS_RESET_LTE_PLMN_BLOCKING
---------------------------------------------------------------------------*/
struct nas_0060_rsp_s
{
  struct nas_0060_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  boolean t02_valid;
};

struct nas_0060_rsp_p_s
{
  PACK(struct) nas_0060_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code
};

/*---------------------------------------------------------------------------
 QMI_NAS_CURRENT_PLMN_NAME_IND
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 QMI_NAS_CONFIG_EMBMS
---------------------------------------------------------------------------*/
#define NAS_0062_REQ_T01                     (0x01)

struct nas_0062_req_s
{
  struct nas_0062_req_t01_s
  {
    uint8 enable;
  } t01;

  struct nas_0062_req_t10_s
  {
    int16 trace_id;
  } t10;

  boolean t01_valid;
  boolean t10_valid;
};

struct nas_0062_req_p_s
{
  PACK(struct) nas_0062_req_t01_p_s
  {
    uint8 enable;
  } t01;

  PACK(struct) nas_0062_req_t10_p_s
  {
    int16 trace_id;
  } t10;
};

#define NAS_0062_RSP_T02                 (0x02)

struct nas_0062_rsp_p_s
{
  PACK(struct) nas_0062_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  PACK(struct) nas_0062_rsp_t10_p_s
  {
    int16 trace_id;
  } t10;
};
  
struct nas_0062_rsp_s
{
  struct nas_0062_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code

  struct nas_0062_rsp_t10_s
  {
    int16 trace_id;
  } t10;

  boolean t02_valid;
  boolean t10_valid;
};

/*---------------------------------------------------------------------------
 QMI_NAS_EMBMS_STATUS_IND
---------------------------------------------------------------------------*/

struct nas_0064_ind_s
{
  struct nas_0064_ind_t01_s
  {
    uint8 enabled;
  } t01; 

  struct nas_0064_ind_t10_s
  {
    int16 trace_id;
  } t10; 

  boolean t01_valid;
  boolean t10_valid;
};

struct nas_0064_ind_p_s
{
  PACK(struct) nas_0064_ind_t01_p_s
  {
    uint8 enabled;
  } t01; 

  PACK(struct) nas_0064_ind_t10_p_s
  {
    int16 trace_id;
  } t10; 
};

/*---------------------------------------------------------------------------
 QMI_NAS_GET_CDMA_POSITION_INFO
---------------------------------------------------------------------------*/
#define QMI_NAS_CDMA_POSITION_INFO_MAX 10

enum qmi_nas_cdma_pilot_e
{
  QMI_NAS_CDMA_PILOT_CURR_ACT_PLT,
  QMI_NAS_CDMA_PILOT_NEIGHBOR_PLT
};

struct nas_0065_rsp_s
{
  struct nas_0065_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 

  struct nas_0065_rsp_t10_s
  {
    boolean  ue_in_idle;
    uint8    bs_len;

    struct nas_0065_cdma_bs_info_s
    {
      uint32 pilot_type; // enum qmi_nas_cdma_pilot_e
      uint16 sid;
      uint16 nid;
      uint16 base_id;
      uint16 pilot_pn;
      uint16 pilot_strength;
      uint32 base_lat;
      uint32 base_long;
      uint64 time_stamp;

    } bs[QMI_NAS_CDMA_POSITION_INFO_MAX];
  } t10;

  boolean t02_valid;
  boolean t10_valid;
};

struct nas_0065_rsp_p_s
{
  PACK(struct) nas_0065_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 

  PACK(struct) nas_0065_rsp_t10_p_s
  {
    boolean  ue_in_idle;
    uint8    bs_len;

    PACK(struct) nas_0065_cdma_bs_info_p_s
    {
      uint32 pilot_type; // enum qmi_nas_cdma_pilot_e
      uint16  sid;
      uint16  nid;
      uint16  base_id;
      uint16  pilot_pn;
      uint16  pilot_strength;
      uint32  base_lat;
      uint32  base_long;
      uint64  time_stamp;

    } bs[QMI_NAS_CDMA_POSITION_INFO_MAX];
  } t10;
};

/*---------------------------------------------------------------------------
 QMI_NAS_RF_BAND_INFO_IND
---------------------------------------------------------------------------*/
#define NAS_0066_IND_T01 (0x01)
#define NAS_0066_IND_T11 (0x11)

struct nas_0066_ind_p_s {
  PACK(struct) nas_0066_ind_t01_p_s {
    uint8   radio_if;
    uint16  active_band;
    uint16  active_channel;
  } t01;

  PACK(struct) nas_0066_ind_t11_p_s {
    uint8   radio_if;
    uint16  active_band;
    uint32  active_channel;
  } t11;
};

struct nas_0066_ind_s {
  struct nas_0066_ind_t01_s {
    uint8   radio_if;
    uint16  active_band;
    uint16  active_channel;
  } t01;
  struct nas_0066_ind_t11_s {
    uint8   radio_if;
    uint16  active_band;
    uint32  active_channel;
  } t11;

  boolean t01_valid;
  boolean t11_valid;
};

/*---------------------------------------------------------------------------
 QMI_NAS_NETWORK_REJECT_IND
---------------------------------------------------------------------------*/
#define NAS_0068_IND_T01 (0x01)
#define NAS_0068_IND_T02 (0x02)
#define NAS_0068_IND_T03 (0x03)
#define NAS_0068_IND_T10 (0x10)
#define NAS_0068_IND_T11 (0x11)

struct nas_0068_ind_p_s {
  PACK(struct) nas_0068_ind_t01_p_s {
    uint8   radio_if;
  } t01;

  PACK(struct) nas_0068_ind_t02_p_s {
    uint8   reject_srv_domain;
  } t02;

  PACK(struct) nas_0068_ind_t03_p_s {
    uint8   rej_cause;
  } t03;

  PACK(struct) nas_0068_ind_t10_p_s {
    uint16 mcc;
    uint16 mnc;
    uint8  mnc_includes_pcs_digit;
  } t10;

  PACK(struct) nas_0068_ind_t11_p_s {
    uint32 csg_id;
  } t11;
};

struct nas_0068_ind_s {
  struct nas_0068_ind_t01_s {
    uint8   radio_if;
  } t01;

  struct nas_0068_ind_t02_s {
    uint8   reject_srv_domain;
  } t02;

  struct nas_0068_ind_t03_s {
    uint8   rej_cause;
  } t03;

  struct nas_0068_ind_t10_s {
    uint16 mcc;
    uint16 mnc;
    uint8  mnc_includes_pcs_digit;
  } t10;

  struct nas_0068_ind_t11_s {
    uint32 csg_id;
  } t11;

  boolean t01_valid;
  boolean t02_valid;
  boolean t03_valid;
  boolean t10_valid;
  boolean t11_valid;
};

/*---------------------------------------------------------------------------
 QMI_NAS_FORCE_NETWORK_SEARCH
---------------------------------------------------------------------------*/
struct nas_0067_rsp_s
{
  struct nas_0067_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 

  boolean t02_valid;
};

struct nas_0067_rsp_p_s
{
  PACK(struct) nas_0067_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
};

/*---------------------------------------------------------------------------
  QMI_NAS_RTRE_CONFIG_IND
---------------------------------------------------------------------------*/
#define NAS_006A_IND_RTRE_CONFIG_T10       (0x10)
#define NAS_006A_IND_RTRE_CONFIG_PREF_T11  (0x11)

struct nas_006A_ind_p_s
{
  PACK(struct) nas_006A_ind_t10_p_s
  {
    uint8 rtre_cfg; // rtre_control 
  } t10;
 
  PACK(struct) nas_006A_ind_t11_p_s
  {
    uint8 rtre_cfg_pref;  // rtre_config
  } t11;
  
};

struct nas_006A_ind_s
{
  struct nas_006A_ind_t10_s
  {
    uint8 rtre_cfg; // rtre_control 
  } t10;
 
  struct nas_006A_ind_t11_s
  {
    uint8 rtre_cfg_pref;  // rtre_config
  } t11;

  boolean t10_valid;
  boolean t11_valid;  
};

/*---------------------------------------------------------------------------
 QMI_NAS_GET_MANAGED_ROAMING_CONFIG
---------------------------------------------------------------------------*/
#define NAS_0069_RSP_T10        (0x10)
#define NAS_0069_RSP_T02        (0x02)

struct nas_0069_rsp_s
{
  struct nas_0069_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 

  struct nas_0069_rsp_t10_s
  {
    uint8 managed_roaming_supported;
  } t10;

  boolean t02_valid;  
  boolean t10_valid;
};

struct nas_0069_rsp_p_s
{
  PACK(struct) nas_0069_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;    
  } t02; // Result Code 

  PACK(struct) nas_0069_rsp_t10_p_s
  {
    uint8 managed_roaming_supported;    
  } t10;
};

/*---------------------------------------------------------------------------
 QMI_NASI_GET_CENTRALIZED_EONS_SUPPORT_STATUS
---------------------------------------------------------------------------*/
#define NAS_006B_RSP_T10        (0x10)
#define NAS_006B_RSP_T02        (0x02)

struct nas_006B_rsp_s
{
  struct nas_006B_rsp_t02_s
  {
	uint16 result_code; // qmi_result_e_type;
	uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
	
  struct nas_006B_rsp_t10_s
  {
	uint8 centralized_eons_supported;
  } t10;
	
  boolean t02_valid;	
  boolean t10_valid;

};

struct nas_006B_rsp_p_s
{
  PACK(struct) nas_006B_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
	   
  PACK(struct) nas_006B_rsp_t10_p_s
  {
    uint8 centralized_eons_supported;
  } t10;	
};

/*---------------------------------------------------------------------------
  QMI_NAS_CONFIG_SIG_INFO2 (006C)
---------------------------------------------------------------------------*/
#define NAS_006C_MIN_DELTA                   (1)

/*---------------------------------------------------------------------------
  QMI_NAS_SET_HPLMN_IRAT_SEARCH_TIMER 
---------------------------------------------------------------------------*/
#define NAS_006E_REQ_T01                     (0x01)

struct nas_006E_req_s
{
  struct nas_006E_req_t01_s
  {
    uint16 timer_value;
  } t01;

  boolean t01_valid;
};

struct nas_006E_req_p_s
{
  PACK(struct) nas_006E_req_t01_p_s
  {
    uint16 timer_value;
  } t01;
};

#define NAS_006E_REQ_T02                     (0x02)

struct nas_006E_rsp_s
{
  struct nas_006E_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02;

  boolean t02_valid;
};

struct nas_006E_rsp_p_s
{
  PACK(struct) nas_006E_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02;
};


/*---------------------------------------------------------------------------
 * QMI_NAS_LIMIT_SYS_INFO_IND_REPORTING
---------------------------------------------------------------------------*/
#define NAS_0070_REQ_T01        (0x01)

struct nas_0070_req_s
{
  struct nas_0070_req_t01_s
  {
    uint64 limit_sys_info_rpt_chg;
  } t01;

  boolean t01_valid;
};

struct nas_0070_req_p_s
{
  PACK(struct) nas_0070_req_t01_p_s
  {
    uint64 limit_sys_info_rpt_chg;
  } t01;
};

struct nas_0070_rsp_s
{
  struct nas_0070_rsp_t02_s
  {
	uint16 result_code; // qmi_result_e_type;
	uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 

  boolean t02_valid;
};

struct nas_0070_rsp_p_s
{
  PACK(struct) nas_0070_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
};

/*---------------------------------------------------------------------------
 * QMI_NAS_GET_SYS_INFO_IND_REPORTING_LIMIT
---------------------------------------------------------------------------*/
#define NAS_0071_RSP_T01       (0x01)

struct nas_0071_rsp_s
{
  struct nas_0071_rsp_t01_s
  {
    uint64 limit_sys_info_rpt_chg;
  } t01;

  struct nas_0071_rsp_t02_s
  {
	uint16 result_code; // qmi_result_e_type;
	uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 

  boolean t01_valid;
  boolean t02_valid;
};

struct nas_0071_rsp_p_s
{
  PACK(struct) nas_0071_rsp_t01_p_s
  {
    uint64 limit_sys_info_rpt_chg;
  } t01;

  PACK(struct) nas_0071_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
};

/*---------------------------------------------------------------------------
 * QMI_NAS_SET_HPLMN_SEARCH_TIMER
---------------------------------------------------------------------------*/
#define NAS_0078_REQ_T01        (0x01)

struct nas_0078_req_s
{
  struct nas_0078_req_t01_s
  {
    uint32 timer_value;
  } t01;

  boolean t01_valid;
};

struct nas_0078_req_p_s
{
  PACK(struct) nas_0078_req_t01_p_s
  {
    uint32 timer_value;
  } t01;
};

struct nas_0078_rsp_s
{
  struct nas_0078_rsp_t02_s
  {
	uint16 result_code; // qmi_result_e_type;
	uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 

  boolean t02_valid;
};

struct nas_0078_rsp_p_s
{
  PACK(struct) nas_0078_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
};

/*---------------------------------------------------------------------------
 * QMI_NAS_GET_HPLMN_SEARCH_TIMER
---------------------------------------------------------------------------*/
#define NAS_0079_RSP_T10       (0x10)

struct nas_0079_rsp_s
{
  struct nas_0079_rsp_t02_s
  {
	uint16 result_code; // qmi_result_e_type;
	uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 

  struct nas_0079_rsp_t10_s
  {
    uint32 timer_value;
  } t10;

  boolean t02_valid;
  boolean t10_valid;
};

struct nas_0079_rsp_p_s
{
  PACK(struct) nas_0079_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 

  PACK(struct) nas_0079_rsp_t10_p_s
  {
    uint32 timer_value;
  } t10;
};

/*---------------------------------------------------------------------------
 * QMI_NAS_SET_E911_STATE
---------------------------------------------------------------------------*/
#define NAS_007A_REQ_T01        (0x01)
#define NAS_007A_REQ_T10        (0x10)
#define NAS_007A_RSP_T02        (0x02)

typedef enum
{
  NAS_007A_E911_ENTER              = 0x00,
  NAS_007A_E911_EXIT_WITH_ECB      = 0x01,
  NAS_007A_E911_EXIT               = 0x02,
  NAS_007A_E911_ENTER_ECB          = 0x03,
} nas_007A_set_e911_state_e_type;

 
struct nas_007A_req_s
{
  struct nas_007A_req_t01_s
  {
    uint32 action;
  } t01;
 
  struct nas_007A_req_t10_s
  {
    boolean redial;
  } t10;
 
  boolean t01_valid;
  boolean t10_valid;
};
 
struct nas_007A_req_p_s
{
  PACK(struct) nas_007A_req_t01_p_s
  {
    uint32 action;
  } t01;
  
  PACK(struct) nas_007A_req_t10_p_s
  {
    boolean redial;
  } t10;
};
 
struct nas_007A_rsp_s
{
  struct nas_007A_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
 
  boolean t02_valid;
};
 
struct nas_007A_rsp_p_s
{
  PACK(struct) nas_007A_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
};

/*---------------------------------------------------------------------------
 * QMI_NAS_GET_EMBMS_SIG_EXT
---------------------------------------------------------------------------*/
#define NAS_EMBMS_TMGI_SVC_ID_SIZE (3)
#define NAS_EMBMS_TMGI_PLMN_OFFSET (3)
#define NAS_EMBMS_TMGI_PLMN_ID_SIZE (3)

/*---------------------------------------------------------------------------
 * QMI_NAS_SUBSCRIPTION_CHANGE_IND
---------------------------------------------------------------------------*/
#define NAS_0086_IND_T10        (0x10)

typedef enum
{
  NAS_0086_IND_PRIMARY_SUBSCRIPTION   = 0x00,
  NAS_0086_IND_SECONDARY_SUBSCRIPTION = 0x01,
  NAS_0086_IND_TERTIARY_SUBSCRIPTION  = 0x02
} nas_0086_subs_e_type;

typedef enum
{
  NAS_0086_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE = 0x00
} nas_0086_ph_cmd_e_type;

struct nas_0086_ind_s
{
  struct nas_0086_ind_t10_s
  {
    uint8 subs_type;
    uint32 ph_cmd;
  } t10;
 
  boolean t10_valid;
};
 
struct nas_0086_ind_p_s
{
  PACK(struct) nas_0086_ind_t10_p_s
  {
    uint8 subs_type;
    uint32 ph_cmd;
  } t10;
};
 
/*---------------------------------------------------------------------------
 * QMI_NAS_IMS_DEREGISTRATION
---------------------------------------------------------------------------*/
struct nas_0087_rsp_s
{
  struct nas_0087_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
 
  boolean t02_valid;
};
 
struct nas_0087_rsp_p_s
{
  PACK(struct) nas_0087_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
};

/*---------------------------------------------------------------------------
 * QMI_NAS_T3346_TIMER_STATUS_CHANGE_IND
---------------------------------------------------------------------------*/
#define NAS_00A0_IND_T1        (0x01)
#define NAS_00A0_IND_T10        (0x10)

typedef enum
{
  NAS_00A0_IND_T3346_TIMER_START   = 0x01,
  NAS_00A0_IND_T3346_TIMER_STOP    = 0x02,
  NAS_00A0_IND_T3346_TIMER_EXPIRE  = 0x03,
  NAS_00A0_IND_T3346_TIMER_MAX     = 0xFFFFFFFF
} nas_00A0_timer_status_e_type;

typedef enum
{
  NAS_00A0_RAT_GSM   = 0x04,
  NAS_00A0_RAT_UMTS  = 0x05,
  NAS_00A0_RAT_LTE   = 0x08
} nas_00A0_rat_e_type;

struct nas_00A0_ind_s
{
  struct nas_00A0_ind_t1_s
  {
    nas_00A0_timer_status_e_type timer_status;
  } t1;
  
  struct nas_00A0_ind_t10_s
  {
    nas_00A0_rat_e_type radio_access_technology;
  } t10;
 
  boolean t10_valid;
};
 
struct nas_00A0_ind_p_s
{
  PACK(struct) nas_00A0_ind_t1_p_s
  {
    nas_00A0_timer_status_e_type timer_status;
  } t1;
  
  PACK(struct) nas_00A0_ind_t10_p_s
  {
    nas_00A0_rat_e_type radio_access_technology;
  } t10;
};

/*---------------------------------------------------------------------------
 * QMI_NAS_VOLTE_ONLY_MODE_IND
---------------------------------------------------------------------------*/
#define NAS_00A2_IND_T01        (0x01)

typedef enum
{
  NAS_00A2_CALL_MODE_STATE_NORMAL          = 0x01,
  NAS_00A2_CALL_MODE_STATE_VOLTE_ONLY      = 0x02,
  NAS_00A2_CALL_MODE_STATE_MAX             = 0xFFFFFFFF
} nas_00A2_call_mode_state_e_type;

struct nas_00A2_ind_s
{
  struct nas_00A2_ind_t01_s
  {
    nas_00A2_call_mode_state_e_type call_mode;
  } t1; 
};

struct nas_00A2_ind_p_s
{
  PACK(struct) nas_00A2_ind_t01_p_s
  {
    nas_00A2_call_mode_state_e_type call_mode;
  } t1; 
};

/*---------------------------------------------------------------------------
 * QMI_NAS_GET_VOLTE_ONLY_MODE_STATUS
---------------------------------------------------------------------------*/
#define NAS_00A3_RSP_T02        (0x02)
#define NAS_00A3_RSP_T10        (0x10)

typedef enum
{
  NAS_00A3_CALL_MODE_STATE_NORMAL          = 0x01,
  NAS_00A3_CALL_MODE_STATE_VOLTE_ONLY      = 0x02,
  NAS_00A3_CALL_MODE_STATE_MAX             = 0xFFFFFFFF
} nas_00A3_call_mode_state_e_type;

struct nas_00A3_rsp_s
{
  struct nas_00A3_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
 
  struct nas_00A3_rsp_t10_s
  {
    nas_00A3_call_mode_state_e_type call_mode;
  } t10; 
  
  boolean t10_valid;
};
 
struct nas_00A3_rsp_p_s
{
  PACK(struct) nas_00A3_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
  
  PACK(struct) nas_00A3_rsp_t10_p_s
  {
    nas_00A3_call_mode_state_e_type call_mode;
  } t10; 
  
  boolean t10_valid;
};

/*---------------------------------------------------------------------------
 * QMI_NAS_VOLTE_ONLY_MODE_SWITCH
---------------------------------------------------------------------------*/
#define NAS_00A4_REQ_T01        (0x01)
#define NAS_00A4_RSP_T02        (0x02)

struct nas_00A4_req_s
{
  struct nas_00A4_req_t01_s
  {
    boolean is_volte_mode_enabled;
  } t01;
  
  boolean t10_valid;
};
 
struct nas_00A4_req_p_s
{
  PACK(struct) nas_00A4_req_t01_p_s
  {
    boolean is_volte_mode_enabled;
  } t01;
};
 
struct nas_00A4_rsp_s
{
  struct nas_00A4_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
};
 
struct nas_00A4_rsp_p_s
{
  PACK(struct) nas_00A4_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
};

/*---------------------------------------------------------------------------
 * QMI_NAS_IMS_CALL_STATE_NOTIFICATION
---------------------------------------------------------------------------*/
#define NAS_00A1_REQ_T01        (0x01)
#define NAS_00A1_REQ_T10        (0x10)
#define NAS_00A1_RSP_T02        (0x02)

typedef enum
{
  NAS_00A1_IMS_CALL_STATUS_START   = 0x00,
  NAS_00A1_IMS_CALL_STATUS_STOP    = 0x01,
  NAS_00A1_IMS_CALL_STATUS_MAX     = 0xFFFFFFFF,
} nas_00A1_ims_call_status_e_type;

typedef enum
{
  NAS_00A1_IMS_CALL_TYPE_VOICE   = 0x01,
  NAS_00A1_IMS_CALL_TYPE_VIDEO   = 0x02,
  NAS_00A1_IMS_CALL_TYPE_SMS     = 0x04,
  NAS_00A1_IMS_CALL_TYPE_MAX     = 0xFFFFFFFFFFFFFFFFULL,
} nas_00A1_ims_call_type_mask_e_type;

struct nas_00A1_req_s
{
  struct nas_00A1_req_t01_s
  {
    nas_00A1_ims_call_status_e_type call_status;
  } t01;
  
  struct nas_00A1_req_t10_s
  {
    nas_00A1_ims_call_type_mask_e_type call_type;
  } t10;
  
  boolean t10_valid;
};
 
struct nas_00A1_req_p_s
{
  PACK(struct) nas_00A1_req_t01_p_s
  {
    nas_00A1_ims_call_status_e_type call_status;
  } t01;
  
  PACK(struct) nas_00A1_req_t10_p_s
  {
    nas_00A1_ims_call_type_mask_e_type call_type;
  } t10;
};
 
struct nas_00A1_rsp_s
{
  struct nas_00A1_rsp_t02_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
};
 
struct nas_00A1_rsp_p_s
{
  PACK(struct) nas_00A1_rsp_t02_p_s
  {
    uint16 result_code; // qmi_result_e_type;
    uint16 error_code;  // qmi_error_e_type;
  } t02; // Result Code 
};

/*---------------------------------------------------------------------------
 End message-specific definitions 
---------------------------------------------------------------------------*/
#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif

/*---------------------------------------------------------------------------
  Function Prototypes
---------------------------------------------------------------------------*/

//QMI_NAS_ABORT
extern qmi_error_type_v01 qmi_nas_0001_req_check ( nas_abort_req_msg_v01 *p_msg );

//QMI_NAS_SET_EVENT_REPORT
extern qmi_error_e_type qmi_nas_0002_req_check ( nas_set_event_report_req_msg_v01 *p_msg); 

//QMI_NAS_INDICATION_REGISTER
extern qmi_error_type_v01 qmi_nas_0003_req_check ( nas_indication_register_req_msg_v01 *p_msg );

//QMI_NAS_GET_SIGNAL_STRENGTH
extern qmi_error_e_type qmi_nas_0020_req_check ( nas_get_signal_strength_req_msg_v01 *p_msg);
\
//QMI_NAS_PERFORM_NETWORK_SCAN
extern qmi_error_type_v01 qmi_nas_0021_req_check ( nas_perform_network_scan_req_msg_v01 *p_msg);

//QMI_NAS_INITIATE_NETWORK_REGISTER
extern qmi_error_e_type qmi_nas_0022_req_read  ( struct nas_0022_req_s *p_msg, dsm_item_type  **p_sdu_in);
extern qmi_error_e_type qmi_nas_0022_req_check ( struct nas_0022_req_s *p_msg);
extern qmi_error_e_type qmi_nas_0022_rsp_write ( struct nas_0022_rsp_s *p_msg, dsm_item_type  **p_response);

//QMI_NAS_INITIATE_ATTACH
extern qmi_error_e_type qmi_nas_0023_req_read  ( struct nas_0023_req_s *p_msg, dsm_item_type  **p_sdu_in);
extern qmi_error_e_type qmi_nas_0023_req_check ( struct nas_0023_req_s *p_msg);

//QMI_NAS_SET_TECHNOLOGY_PREFERENCE
extern qmi_error_e_type qmi_nas_002A_req_read  ( struct nas_002A_req_s *p_msg, dsm_item_type **sdu_in);
extern qmi_error_e_type qmi_nas_002A_req_check ( struct nas_002A_req_s *p_msg);
extern qmi_error_e_type qmi_nas_002A_rsp_write ( struct nas_002A_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_GET_TECHNOLOGY_PREFERENCE
extern qmi_error_e_type qmi_nas_002B_rsp_write ( struct nas_002B_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_GET_ACCOLC
extern qmi_error_e_type qmi_nas_002C_rsp_write ( struct nas_002C_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_SET_ACCOLC
extern qmi_error_e_type qmi_nas_002D_req_read  ( struct nas_002D_req_s *p_msg, dsm_item_type **sdu_in);
extern qmi_error_e_type qmi_nas_002D_req_check ( struct nas_002D_req_s *p_msg); 

//QMI_NAS_GET_NETWORK_SYSTEM_PREFERENCE
extern qmi_error_e_type qmi_nas_002E_rsp_write ( struct nas_002E_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_SET_DEVICE_CONFIG
extern qmi_error_e_type qmi_nas_0030_req_check ( nas_set_device_config_req_msg_v01 *p_msg); 

//QMI_NAS_GET_RF_BAND_INFO
extern qmi_error_e_type qmi_nas_0031_rsp_write ( struct nas_0031_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_GET_AN_AAA_STATUS
extern qmi_error_e_type qmi_nas_0032_rsp_write ( struct nas_0032_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE
extern qmi_error_type_v01 qmi_nas_0033_req_check ( nas_set_system_selection_preference_req_msg_v01 *p_msg );

//QMI_NAS_UPDATE_AKEY
extern qmi_error_e_type qmi_nas_003D_req_read  ( struct nas_003D_req_s *p_msg, dsm_item_type **sdu_in);
extern qmi_error_e_type qmi_nas_003D_req_check ( struct nas_003D_req_s *p_msg);

//QMI_NAS_GET_3GPP2_INFO
extern qmi_error_e_type qmi_nas_003E_req_check ( nas_get_3gpp2_subscription_info_req_msg_v01 *p_msg);

//QMI_NAS_SET_3GPP2_INFO
extern qmi_error_type_v01 qmi_nas_003F_req_check ( nas_set_3gpp2_subscription_info_req_msg_v01 *p_msg );

//QMI_NAS_GET_MOB_CAI_REV
extern qmi_error_e_type qmi_nas_0040_rsp_write ( struct nas_0040_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_GET_RTRE_CONFIG
extern qmi_error_e_type qmi_nas_0041_rsp_write ( struct nas_0041_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_SET_RTRE_CONFIG
extern qmi_error_type_v01 qmi_nas_0042_req_check ( nas_set_rtre_config_req_v01 *p_msg );

// QMI_NAS_GET_PLMN_NAME
extern qmi_error_type_v01 qmi_nas_0044_req_check ( nas_get_plmn_name_req_msg_v01 *p_msg);

// QMI_NAS_BIND_SUBSCRIPTION
extern qmi_error_e_type qmi_nas_0045_req_read ( struct nas_0045_req_s  *p_msg, dsm_item_type **sdu_in ); 
extern qmi_error_e_type qmi_nas_0045_req_check (struct nas_0045_req_s *p_msg );

// QMI_NAS_MANAGED_ROAMING_IND
extern qmi_error_e_type qmi_nas_0046_ind_write (struct nas_0046_ind_s *p_msg, dsm_item_type **response);

// QMI_NAS_GET_MODE_PREF
extern qmi_error_e_type qmi_nas_0049_rsp_write (struct nas_0049_rsp_s *p_msg, dsm_item_type **response);

// QMI_NAS_THERMAL_EMERGENCY_STATE_IND
extern qmi_error_e_type qmi_nas_004A_ind_write( struct nas_004A_ind_s *p_msg, dsm_item_type **response );

// QMI_NAS_SET_DUAL_STANDBY_PREF_NEW
extern qmi_error_type_v01 qmi_nas_004B_req_check ( nas_set_dual_standby_pref_req_msg_v01 *p_msg );

// QMI_NAS_NETWORK_TIME_IND
extern qmi_error_e_type qmi_nas_004C_ind_write( struct nas_004C_ind_s *p_msg, dsm_item_type **response );

// QMI_NAS_CONFIG_SIG_INFO 0x0050
extern qmi_error_type_v01 qmi_nas_0050_req_check( nas_config_sig_info_req_msg_v01 * p_msg );

// QMI_NAS_HDR_SESSION_CLOSE_IND
extern qmi_error_e_type qmi_nas_0054_ind_write ( struct nas_0054_ind_s *p_msg, dsm_item_type  **p_response);

// QMI_NAS_HDR_UATI_IND
extern qmi_error_e_type qmi_nas_0055_ind_write ( struct nas_0055_ind_s *p_msg, dsm_item_type  **p_response);

// QMI_NAS_GET_HDR_SUBTYPE
extern qmi_error_e_type qmi_nas_0056_req_read(struct nas_0056_req_s *p_msg, dsm_item_type **sdu_in);
extern qmi_error_e_type qmi_nas_0056_req_check(struct nas_0056_req_s *p_msg);
extern qmi_error_e_type qmi_nas_0056_rsp_write(struct nas_0056_rsp_s *p_msg, dsm_item_type **response);

// QMI_NAS_GET_HDR_COLOR_CODE
extern qmi_error_e_type qmi_nas_0057_rsp_write (struct nas_0057_rsp_s *p_msg, dsm_item_type **response);

// QMI_NAS_SET_RX_DIVERSITY
extern qmi_error_e_type qmi_nas_0059_req_read(struct nas_0059_req_s *p_msg, dsm_item_type **sdu_in);
extern qmi_error_e_type qmi_nas_0059_req_check(struct nas_0059_req_s *p_msg);
extern qmi_error_e_type qmi_nas_0059_rsp_write(struct nas_0059_rsp_s *p_msg, dsm_item_type **response);

// QMI_NAS_SET_TX_RX_INFO
extern qmi_error_e_type qmi_nas_005A_req_check(nas_get_tx_rx_info_req_msg_v01 *p_msg);

//QMI_NAS_UPDATE_AKEY_EXT
extern qmi_error_e_type qmi_nas_005B_req_read  ( struct nas_005B_req_s *p_msg, dsm_item_type **sdu_in);
extern qmi_error_e_type qmi_nas_005B_req_check ( struct nas_005B_req_s *p_msg);

//QMI_NAS_DETACH_LTE
extern qmi_error_e_type qmi_nas_005D_rsp_write(struct nas_005D_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_BLOCK_LTE_PLMN
extern qmi_error_e_type qmi_nas_005E_req_read(struct nas_005E_req_s *p_msg, dsm_item_type **sdu_in);
extern qmi_error_e_type qmi_nas_005E_req_check(struct nas_005E_req_s *p_msg);
extern qmi_error_e_type qmi_nas_005E_rsp_write(struct nas_005E_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_UNBLOCK_LTE_PLMN
extern qmi_error_e_type qmi_nas_005F_req_read(struct nas_005F_req_s *p_msg, dsm_item_type **sdu_in);
extern qmi_error_e_type qmi_nas_005F_req_check(struct nas_005F_req_s *p_msg);
extern qmi_error_e_type qmi_nas_005F_rsp_write(struct nas_005F_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_RESET_LTE_PLMN_BLOCKING
extern qmi_error_e_type qmi_nas_0060_rsp_write(struct nas_0060_rsp_s *p_msg, dsm_item_type **response);

// QMI_NAS_CONFIG_EMBMS
extern qmi_error_e_type qmi_nas_0062_req_read(struct nas_0062_req_s *p_msg, dsm_item_type **sdu_in);
extern qmi_error_e_type qmi_nas_0062_req_check(struct nas_0062_req_s *p_msg);
extern qmi_error_e_type qmi_nas_0062_rsp_write(struct nas_0062_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_EMBMS_STATUS_IND
extern qmi_error_e_type qmi_nas_0064_ind_write(struct nas_0064_ind_s *p_msg, dsm_item_type **response);

//QMI_NAS_GET_CDMA_POSITION_INFO
extern qmi_error_e_type qmi_nas_0065_rsp_write(struct nas_0065_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_RF_BAND_INFO_IND
extern qmi_error_e_type qmi_nas_0066_ind_write(struct nas_0066_ind_s *p_msg, dsm_item_type **response);

//QMI_NAS_NETWORK_REJECT_IND
extern qmi_error_e_type qmi_nas_0068_ind_write(struct nas_0068_ind_s *p_msg, dsm_item_type **response);

//QMI_NAS_FORCE_NETWORK_SEARCH
extern qmi_error_e_type qmi_nas_0067_rsp_write(struct nas_0067_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_RTRE_CONFIG_IND
extern qmi_error_e_type qmi_nas_006A_ind_write(struct nas_006A_ind_s *p_msg, dsm_item_type **p_response);

//QMI_NAS_GET_MANAGED_ROAMING_CONFIG
extern qmi_error_e_type qmi_nas_0069_rsp_write(struct nas_0069_rsp_s *p_msg, dsm_item_type **response);

// QMI_NAS_GET_CENTRALIZED_EONS_SUPPORT_STATUS
extern qmi_error_e_type qmi_nas_006B_rsp_write(struct nas_006B_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_CONFIG_SIG_INFO2
extern qmi_error_type_v01 qmi_nas_006C_req_check( nas_config_sig_info2_req_msg_v01 *p_msg );

// QMI_NAS_SET_HPLMN_IRAT_SEARCH_TIMER 
extern qmi_error_e_type qmi_nas_006E_req_read ( struct nas_006E_req_s  *p_msg, dsm_item_type **sdu_in );
extern qmi_error_e_type qmi_nas_006E_req_check (struct nas_006E_req_s *p_msg);
extern qmi_error_e_type qmi_nas_006E_rsp_write ( struct nas_006E_rsp_s *p_msg, dsm_item_type **p_response );

//QMI_NAS_GET_EMBMS_SIG
extern qmi_error_e_type qmi_nas_006F_req_check(nas_get_embms_sig_req_msg_v01 *p_msg);

// QMI_NAS_LIMIT_SYS_INFO_IND_REPORTING
extern qmi_error_e_type qmi_nas_0070_req_read(struct nas_0070_req_s *p_msg, dsm_item_type **sdu_in);
extern qmi_error_e_type qmi_nas_0070_req_check(struct nas_0070_req_s *p_msg);
extern qmi_error_e_type qmi_nas_0070_rsp_write(struct nas_0070_rsp_s *p_msg, dsm_item_type **response);

// QMI_NAS_GET_SYS_INFO_IND_REPORTING_LIMIT
extern qmi_error_e_type qmi_nas_0071_rsp_write(struct nas_0071_rsp_s *p_msg, dsm_item_type **response);

// QMI_NAS_UPDATE_IMS_STATUS
extern qmi_error_type_v01 qmi_nas_0072_req_check(nas_update_ims_status_req_msg_v01 *p_msg);

// QMI_NAS_CONFIG_PLMN_NAME_IND_REPORT_ING
extern qmi_error_type_v01 qmi_nas_0075_req_check ( nas_config_plmn_name_ind_reporting_req_msg_v01 *p_msg );

//QMI_NAS_CDMA_AVOID_SYSTEM
extern qmi_error_type_v01 qmi_nas_0076_req_check ( nas_cdma_avoid_system_req_msg_v01 *p_msg );

// QMI_NAS_SET_HPLMN_SEARCH_TIMER
extern qmi_error_e_type qmi_nas_0078_req_read(struct nas_0078_req_s *p_msg, dsm_item_type **sdu_in);
extern qmi_error_e_type qmi_nas_0078_req_check(struct nas_0078_req_s *p_msg);
extern qmi_error_e_type qmi_nas_0078_rsp_write(struct nas_0078_rsp_s *p_msg, dsm_item_type **response);

// QMI_NAS_GET_HPLMN_SEARCH_TIMER
extern qmi_error_e_type qmi_nas_0079_rsp_write(struct nas_0079_rsp_s *p_msg, dsm_item_type **response);

//QMI_NAS_SET_E911_STATE
extern qmi_error_e_type qmi_nas_007A_req_read  (struct nas_007A_req_s *p_msg, dsm_item_type  **sdu_in);
extern qmi_error_e_type qmi_nas_007A_req_check (struct nas_007A_req_s *p_msg); 
extern qmi_error_e_type qmi_nas_007A_rsp_write(struct nas_007A_rsp_s *p_msg, dsm_item_type **response);

// QMI_NAS_SET_LTE_BAND_PRIORITY
extern qmi_error_type_v01 qmi_nas_0080_req_check(nas_set_lte_band_priority_req_msg_v01 *p_msg);

//QMI_NAS_GET_EMBMS_SIG_EXT
extern qmi_error_type_v01 qmi_nas_0081_req_check( nas_get_embms_sig_ext_req_msg_v01 *p_msg );

//QMI_NAS_SET_BUILT_IN_PLMN_LIST
extern qmi_error_e_type qmi_nas_0084_req_check ( nas_set_builtin_plmn_list_req_msg_v01 *p_msg );

//QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN
extern qmi_error_type_v01 qmi_nas_0085_req_check ( nas_perform_incremental_network_scan_req_msg_v01 *p_msg);

//QMI_NAS_SET_DRX
extern qmi_error_type_v01 qmi_nas_0088_req_check( nas_set_drx_req_msg_v01 *p_msg );

//QMI_NAS_CSG_SEARCH_SELECT_CONFIG
extern qmi_error_type_v01 qmi_nas_008A_req_check ( nas_csg_search_selection_config_req_msg_v01 *p_msg);

//QMI_NAS_GET_SERV_CELL_SIB
extern qmi_error_type_v01 qmi_nas_008F_req_check ( nas_get_serv_cell_sib_req_msg_v01 *p_msg);

//QMI_NAS_SET_PERIODIC_SEARCH_ALLOWED
qmi_error_type_v01 qmi_nas_0092_req_check( nas_set_periodic_search_allowed_req_msg_v01 *p_msg );

//QMI_NAS_SET_DATA_ROAMING_REQ_MSG
qmi_error_type_v01 qmi_nas_009A_req_check ( nas_set_data_roaming_req_msg_v01 *p_msg);

#endif // !_DS_QMI_NAS_MSG_H_
