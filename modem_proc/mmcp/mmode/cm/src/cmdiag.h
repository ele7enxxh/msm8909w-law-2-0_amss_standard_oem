#ifndef CMDIAG_H
#define CMDIAG_H

#include "mmcp_variation.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*==========================================================================

                    Call Manager Diagnostic Packets

General Description
  This file contains packet definitions for CM diagnostic packets.

Copyright (c) 2002 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmdiag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/11   chl     Added support to answer call and select mode_pref with LTE
07/01/11   sg      Fixing compilation issue for e-calll build.
04/29/11   rm      Adding req/resp packet for CDMA info
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
12/23/10   sv      Integrate DSDS feature
08/10/10   aj      update diag IRAT meas req API
06/15/10   aj      support for IRAT measurement support
02/16/09   ks      Adding support for QDSP 6 based compiler
11/04/08   cl      Adding DIAG support for SUPS Command.
09/15/08   np      Added Assisted dialing test code
05/11/06   sk      Added CMDIAG_SO_VOICE_4GV_NB.
04/19/05   pk      Added CMDIAG_MODE_POWER_OFF_F mode
07/12/04   ic      Added support for WINCE
02/06/04   ws      Initial jaguar/mainline merge.
07/24/03   vt      removed FEATURE_SPECIAL_MDR
02/28/03   ws      Added #def of CMDIAG_H
11/20/01   as      Created file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "diagpkt.h"        /* for subsystem commands      */
#include "diagcmd.h"        /* for subsystem commands      */

/* This must be called during initializationfor the packet support to work. */
void cmdiag_init (void);

/* Mode change commands */
typedef enum
{
  CMDIAG_MODE_OFFLINE_A_F = 0,  /* Go to offline analog */
  CMDIAG_MODE_OFFLINE_D_F,      /* Go to offline digital */
  CMDIAG_MODE_RESET_F,          /* Reset. Only exit from offline */
  CMDIAG_MODE_FTM_F,            /* FTM mode - if supported */
  CMDIAG_MODE_ONLINE_F,         /* Online mode - if supported */
  CMDIAG_MODE_LPM_F,            /* LPM mode - if supported */
  CMDIAG_MODE_POWER_OFF_F,        /* Power off mode */
  CMDIAG_MODE_MAX_F             /* Last (and invalid) mode enum value */
}
cmdiag_mode_enum_type;


#ifdef T_WINCE
#pragma pack(1)
#endif
/*==========================================================================

PACKET   DIAG_CONTROL_F

PURPOSE  Request sent from the DM to the DMSS to change the operating mode

============================================================================*/
typedef PACKED struct PACKED_POST
{
  byte xx_cmd_code;
  word mode;            /* Mode to change to */
}
cmdiag_mode_control_req_type;

typedef cmdiag_mode_control_req_type cmdiag_mode_control_rsp_type;


/* Service options for call origination. */

#define CMDIAG_SO_NONE            0x00
#define CMDIAG_SO_LOOPBACK        0x02
#define CMDIAG_SO_MARKOV          0x03
#define CMDIAG_SO_DATA            0x04
#define CMDIAG_SO_VOICE_96A       0x05
#define CMDIAG_SO_VOICE13         0x06  /* 13K (MSM2 only) Voice call */
#define CMDIAG_SO_RS2_MARKOV      0x07  /* 13K (MSM2 only) Markov call (new) */
#define CMDIAG_SO_RS1_MARKOV      0x08
#define CMDIAG_SO_LOOPBACK_13K    0x09
#define CMDIAG_SO_MARKOV_13K      0x0A  /* 13K (MSM2 only) Markov call (old) */
#define CMDIAG_SO_EVRC            0x0B  /* EVRC voice option */
/* CAI SMS SO (6) value is overridden with a proprietary value from this
   list, so use unlikely SO value. */
#define CMDIAG_SO_SMS             0xFFFE
/* CAI_SO_RS2_SMS (14) is not in this proprietary list, so use the CAI SO. */
#define CMDIAG_SO_RS2_SMS         0x0E
#define CMDIAG_SO_VOICE_13K_IS733 0x11  /* IS-733 s.o. number */
#define CMDIAG_SO_MARKOV_SO54     0x36
#define CMDIAG_SO_LOOPBACK_SO55   0x37
#define CMDIAG_SO_VOICE_4GV_NB    0x44

#if defined(FEATURE_IS95B_MDR)
/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 1 forward and reverse.  Default Mux = 9 forward 1 reverse */
#define CMDIAG_SO_MDR_PKT_DATA_FRS1_RRS1   0x16 /* IS-707A s.o. number = 22 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 1 forward and rate set 2 reverse.
 Default Mux = 9 forward 2 reverse.  This SO is not support by MSM3000. */
#define CMDIAG_SO_MDR_PKT_DATA_FRS1_RRS2   0x17 /* IS-707A s.o. number = 23 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 2 forward and rate set 1 reverse.
 Default Mux = 10 forward 1 reverse.  This SO is not support by MSM3000. */
#define CMDIAG_SO_MDR_PKT_DATA_FRS2_RRS1   0x18 /* IS-707A s.o. number = 24 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 2 forward and reverse.  Default Mux = 10 forward 2 reverse */
#define CMDIAG_SO_MDR_PKT_DATA_FRS2_RRS2   0x19 /* IS-707A s.o. number = 25 */

#define CMDIAG_SO_MDR_DPT_FRS1_RRS1        0x24 /* MDR Data Pattern Test: Proprietary
                           Service Option rate set 1 fwd and rev */

#define CMDIAG_SO_MDR_DPT_FRS2_RRS2        0x25 /* MDR Data Pattern Test: Proprietary
                           Service Option rate set 2 fwd and rev */
#endif /* defined(FEATURE_IS95B_MDR) */

/*===========================================================================

PACKET   DIAG_ORIG_F

PURPOSE  Sent by DM to originate a call.

RESPONSE DMSS originates a call using the specified number.

===========================================================================*/
#define CMDIAG_NV_MAX_DIAL_DIGITS 32

typedef PACKED struct PACKED_POST
{
  byte cmd_code;        /* Command code */
  byte cnt;         /* number of digits in dialed_digits array */
  char dialed_digits[CMDIAG_NV_MAX_DIAL_DIGITS];
  word so;
  /* desired service option - 0 = Reserved,  1 = Voice (IS96A), 2 = Loopback,
     8001 = Voice (IS96), 8002 = Markov, 8003 = Data (see cai.h) */
}
cmdiag_orig_req_type;

typedef PACKED struct PACKED_POST
{
  byte cmd_code;
}
cmdiag_orig_rsp_type;


/*===========================================================================

PACKET   DIAG_END_F

PURPOSE  Sent by DM to end a call.

RESPONSE DMSS ends the current call.

===========================================================================*/
typedef PACKED struct PACKED_POST
{
  byte cmd_code;
}
cmdiag_end_req_type;

typedef PACKED struct PACKED_POST
{
  byte cmd_code;
}
cmdiag_end_rsp_type;

/*===========================================================================

PACKET   DIAG_MEAS_MODE_F

PURPOSE  Sent by DM to change the measurement mode

RESPONSE DMSS changes the measurement mode

===========================================================================*/
typedef PACKED struct PACKED_POST
{
  byte cmd_code;
  sys_meas_mode_e_type mode;            /* Mode to change to */
}
cmdiag_meas_mode_req_type;

typedef PACKED struct PACKED_POST
{
  byte cmd_code;
}
cmdiag_meas_mode_rsp_type;

/*===========================================================================

PACKET   DIAG_MEAS_REQ_F

PURPOSE  Sent by DM to request measurement

RESPONSE DMSS returns the measurement result

===========================================================================*/

typedef PACKED struct PACKED_POST
{
  uint8                     cmd_code;
  uint8                     sys_mode;
  PACKED union PACKED_POST
  {
    PACKED struct PACKED_POST
    {
      boolean               srch_win_size_inc; /**< indicates if search
                                                **  window size is inc */
      byte                  srch_win_size;     /**< search window size */
      uint16                chan_num;          /** HDR channel number */
      uint8                 band;              /** HDR band class */
      byte                  neighbor_count;    /**< number of neighbours  */
      sys_pilot_pn_type     neighbor_pilot_pn [ CM_MAX_DO_NEIGHBOR_CNT ];
                                             /**< pilot PNs of neighbours */
    }hdr_req;
  }req;
}cmdiag_meas_req_type;

typedef PACKED struct PACKED_POST
{
  byte cmd_code;
}
cmdiag_meas_rsp_type;

/*===========================================================================

PACKET   DIAG_CNTRY_INFO_F

PURPOSE  Sent by DM to query country network information.

RESPONSE CM sends the current country information corresponding to MCC

===========================================================================*/
typedef PACKED struct
{
  byte cmd_code;
  byte pad; /* byte alighment*/
  word mcc;/* Mobile country code*/
}
cmdiag_cntry_info_req_type;


typedef PACKED struct
{
   word sid1; /* Starting of Sid range inclusive of sid1*/
   word sid2; /* end of Sid range inclusive of sid2 */
   word country_code;    /* cc carries the country code directly, Array to accommodate a leading 0 */
                    /* 0 can be neglected for now, used for some zones */
   char inter_dialed_digits[6]; /* idd digits start after last F */
   char special_prefix_code[6]; /* spc digits start after last F */
   char national_direct_dial[6]; /* national direct dial */
   boolean is_network_soln; /* network solution */
   word  mobile_country_code;   /* Mobile country code */
}
cmdiag_cntry_info_type;

typedef PACKED struct
{
   byte cmd_code;
   byte pad1; /* byte alignment*/
   word sid1; /* Starting of Sid range inclusive of sid1*/
   word sid2; /* end of Sid range inclusive of sid2 */
   word country_code;    /* cc carries the country code directly, Array to accommodate a leading 0 */
                    /* 0 can be neglected for now, used for some zones */
   char inter_dialed_digits[6]; /* idd digits start after last F */
   word pad2; /* Byte alignment*/
   char special_prefix_code[6]; /* spc digits start after last F */
   word pad3; /* Byte alignment*/
   char national_direct_dial[6]; /* national direct dial */
   word  mobile_country_code;   /* Mobile country code */
   byte is_network_soln; /* network solution */

}
cmdiag_cntry_info_rsp_type;


/*===========================================================================

PACKET   DIAG_SUPS_F

PURPOSE  Sent suplementary service request

RESPONSE DMSS process Suplementary Service Request

===========================================================================*/
typedef PACKED struct
{
  byte cmd_code;      /* Command Code */
  byte cnt;           /* number of digits in dialed_digits array */
  char dialed_digits[CMDIAG_NV_MAX_DIAL_DIGITS];
  word sups_type;     /* Suplementary Service Type */
}
cmdiag_sups_req_type;

typedef PACKED struct
{
  byte cmd_code;
}
cmdiag_sups_rsp_type;

/*===========================================================================
                        CM DIAG COMMAND DEFINITIONS
===========================================================================*/

/* This Table is from ICD (80-v1294-7)*/
#define CM_STATE_INFO                       0
#define CM_PLMN_SELECTION_MODE              1
#define CM_SYSTEM_SELECTION_PREFERENCE      2
#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#define CM_CDMA_CALL_ORIGINATION            3
#endif
#define CM_CALL_END                         4
#define CM_WCDMA_CALL_ORIGINATION           5
#define CM_GSM_CALL_ORIGINATION             6
#define CM_GET_CALL_INFORMATION_LIST        7
#define CM_SET_PASSWORD                     8
#define CM_SET_CLOSED_USER_GROUP_PREFERENCE 9
#ifdef CM_GW_SUPPORTED
#define CM_GET_CLOSED_USER_GROUP_PREFERENCE 10
#endif
#define CM_SEND_USSD_REPLY                  11
#ifdef CM_GW_SUPPORTED
#define CM_GET_CCM_INFO                     12
#endif
#define CM_SEND_SUPS_LCS_MOLR               13
#define CM_SEND_SUPS_LCS_MOLR_COMPLETED     14
#if defined(FEATURE_CALL_THROTTLE)
#define CM_ORIG_THR_TBL_RESET               15
#endif
#define CM_SYSTEM_SELECTION_PREFERENCE2     16
#define CM_GET_CELL_SERVICE_IND             17
#if (defined CM_GW_SUPPORTED && defined FEATURE_MULTIMEDIA_FALLBACK)
#error code not present
#endif

/* Dual Standby Preference DIAG command
*/
#define CM_STATE_INFO_PER_SUBS              24
#define CM_PLMN_SELECTION_MODE_PER_SUBS     25
#define CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS \
                                            26
#define CM_WCDMA_CALL_ORIGINATION_PER_SUBS  27
#define CM_GSM_CALL_ORIGINATION_PER_SUBS    28
#define CM_GET_CALL_INFORMATION_LIST_PER_SUBS \
                                            29
#define CM_SET_PASSWORD_PER_SUBS            30
#define CM_SEND_USSD_REPLY_PER_SUBS         31
#ifdef CM_GW_SUPPORTED
#define CM_GET_CCM_INFO_PER_SUBS            32
#endif
#define CM_SEND_SUPS_LCS_MOLR_PER_SUBS      33
#define CM_SEND_SUPS_LCS_MOLR_COMPLETED_PER_SUBS \
                                            34
#define CM_GET_CELL_SERVICE_IND_PER_SUBS    35
#ifdef FEATURE_MMODE_DUAL_SIM
#define CM_DUAL_STANDBY_PREF                36
#endif


#define CM_GET_CDMA_INFO                    39

#define CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE \
                                            40

#define CM_CALL_ANSWER                      41
#define CM_GW_CALL_SETUP_RESPONSE           42
#define CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS \
                                            43
#define CM_MULTISIM_STATE_INFO              44
#define CMLOG_MAX_F                         45 /* Must be one greater than highest cmd */


#define CM_MLC_NUM_DATA_MAX               9 /* MLC number used for MOLR */
#define CM_LCS_CLIENT_EXT_ADD_DATA_MAX    20 /* Ext add data used for MOLR */
#define CM_GPS_ASSIS_DATA_MAX             38 /* GPS assisted data for MOLR */
#define USS_DEF_ALPHABET_LANG_UNSPEC 0x0F


/*---------------------------------------------------------------------------
    CM STATE INFO -- SUBCOMMAND 0
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_STATE_INFO)
 /* No additional fields */
DIAGPKT_RSP_END
/*lint -esym(751, DIAG_SUBSYS_CM_CM_STATE_INFO_req_type)*/

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_STATE_INFO)
  int  call_state;                 /* ENUM for EVENT_ID 416 */
  int  oprt_mode;                  /* ENUM for EVENT_ID 417 */
  int  system_mode;                /* ENUM for EVENT_ID 418 */
  int  mode_pref;                  /* ENUM for EVENT_ID 520 */
  int  band_pref;                  /* ENUM for EVENT_ID 521 */
  int  roam_pref;                  /* ENUM for EVENT_ID 522 */
  int  srv_domain_pref;            /* ENUM for EVENT_ID 523 */
  int  acq_order_pref;             /* ENUM for EVENT_ID 524 */
  int  hybr_pref;                  /* ENUM for EVENT_ID 525 */
  int  network_sel_mode_pref;      /* ENUM for EVENT_ID 526 */
  int  srv_status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM PLMN LIST -- SUBCOMMAND 1
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_PLMN_SELECTION_MODE)
 /* No additional fields */
DIAGPKT_RSP_END
/*lint -esym(751, DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_req_type)*/

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_PLMN_SELECTION_MODE)
  int  status;                     /* SUCCESS/FAILURE_REASON */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM SYSTEM SELECTION REQUEST -- SUBCOMMAND 2
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE)
  byte         network_sel_mode_pref;     /* AUTOMATIC/MANUAL      */
  byte         PLMN[3];                   /* PLMN (if manual)      */
  unsigned int mode_pref;                 /* ENUM for EVENT_ID 520 */
  unsigned int acq_order_pref;            /* ENUM for EVENT_ID 524 */
  unsigned int band_pref;                 /* ENUM for EVENT_ID 521 */
  unsigned int roam_pref;                 /* ENUM for EVENT_ID 522 */
  unsigned int hybr_pref;                 /* ENUM for EVENT_ID 525 */
  unsigned int srv_domain_pref;           /* ENUM for EVENT_ID 523 */
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE)
  int  status;                            /* SUCCESS/FAILURE_REASON */
DIAGPKT_RSP_END

#define DIAG_MAX_WCDMA_DIGITS 64
#define CM_DIAG_MAX_GSM_DIGITS 64
#ifdef CM_GW_SUPPORTED
#define USS_NOTIFY_RESPONSE  1
#define USS_RESPONSE  0
#endif /* #ifdef CM_GW_SUPPORTED */

/*
* cmlog_check_request_length () checks for the correctness of
* input packet structure depending on the declaration of that structure.
* cmlog_check_request_length () has to be modified for any change
* to the structures of WCDMA/GSM call origination, call end,
* set closed user group, set password and send USSD reply.
*/

/*---------------------------------------------------------------------------
    CM_WCDMA_CALL_ORIGINATION -- SUBCOMMAND 5
---------------------------------------------------------------------------*/
/* WCDMA call origination request */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_WCDMA_CALL_ORIGINATION)

 /* DM shall set it to the number of valid digits in DIGITS field */
 uint8  num_digits;

 /* Rate of the vocoder: 0 ~ 7 for
    4.75, 5.15, 5.90, 6.70, 7.40, 7.95, 10.2, 12.2 kbps
 */
 uint8  amr_rate;

 /* Dialed digits; ASCII chars; 0-9, #, * only */
 uint8  digits[DIAG_MAX_WCDMA_DIGITS];

DIAGPKT_REQ_END

#ifdef CM_GW_SUPPORTED
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_WCDMA_CALL_ORIGINATION)
  boolean status;
  byte    call_id;
DIAGPKT_RSP_END
#endif /* #ifdef CM_GW_SUPPORTED */


/*---------------------------------------------------------------------------
    CM_GSM_CALL_ORIGINATION -- SUBCOMMAND 6
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GSM_CALL_ORIGINATION)

 /* DM shall set it to the number of valid digits in DIGITS field */
 uint8  num_digits;

 /* Dialed digits; ASCII chars; 0-9, #, * only */
 uint8  digits[CM_DIAG_MAX_GSM_DIGITS];

DIAGPKT_REQ_END

#ifdef CM_GW_SUPPORTED
DIAGPKT_SUBSYS_RSP_DEFINE (CM, CM_GSM_CALL_ORIGINATION)
  boolean status;
  byte    call_id;
DIAGPKT_RSP_END
#endif /* #ifdef CM_GW_SUPPORTED */


#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))


#define CMLOG_CM_MAX_NUMBER_CHARS       64
#define CMLOG_CM_MAX_SUBADDRESS_DIGITS  21
#define CMLOG_CM_NUMBER_MAX             2
#define CMLOG_CM_MAX_ALPHA_DIGITS       12
#define CMLOG_CM_ORIG_QOS_LEN_MAX       31


/*---------------------------------------------------------------------------
    CM_CDMA_CALL_ORIGINATION -- SUBCOMMAND 3
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_CDMA_CALL_ORIGINATION)
  uint32    call_type;                              /* cm_call_type_e_type */
  uint32    srv_type;                                /* cm_srv_type_e_type */
  uint8     called_calling_num_count;
  uint8     alpha_digits_count;
  uint16    service_option;
  PACKED struct PACKED_POST {
    uint8   digits[CMLOG_CM_MAX_NUMBER_CHARS];
    uint8   subaddr[CMLOG_CM_MAX_SUBADDRESS_DIGITS];
    uint8   digit_mode;
    uint8   number_type;
    uint8   number_plan;
    uint8   presentation_ind;
    uint8   screening_ind;
    uint8   subaddr_extension;
    uint8   subaddr_type;
    uint8   subaddr_odd_even_ind;
    uint8   subaddr_count;
    uint8   digits_count;
    uint8   reserved;
  } number[CMLOG_CM_NUMBER_MAX];
  uint8     alpha_digits[CMLOG_CM_MAX_ALPHA_DIGITS];
  uint8     otasp_activate_code;
  boolean   drs_bit;
  boolean   sr_id_included;
  uint8     sr_id;
  uint8     qos_params[CMLOG_CM_ORIG_QOS_LEN_MAX];
  boolean   qos_params_included;
  uint32    hdr_last_act_data_net;
  uint8     qos_params_count;
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_CDMA_CALL_ORIGINATION)
  boolean  status;
  byte     call_id;
DIAGPKT_RSP_END
#endif /* if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */


#define CM_DIAG_CALL_ID_MAX  8
#define MAX_DIAG_PWD_CHAR    4
#define MAX_DIAG_USS_CHAR    160 /* Max num of char is USS data*/

/*---------------------------------------------------------------------------
    CM_CDMA_CALL_ORIGINATION -- SUBCOMMAND 4
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_CALL_END)
  byte         call_id_count;
  PACKED struct PACKED_POST {
    uint8        call_id;
    uint8        call_mode_type;             /* CDMA - 0, GW_CS=1, GW_PS=2 */
    boolean      cdma_end_reason_included;
    uint8        cdma_end_reason;
    boolean      gw_tear_down_ind_valid;
    boolean      gw_tear_down_ind_ti_flag;
    uint8        gw_cause;
  } end_params[CM_DIAG_CALL_ID_MAX];
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_CALL_END)
  boolean  status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_GET_CALL_INFORMATION_LIST -- SUBCOMMAND 7
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GET_CALL_INFORMATION_LIST)
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_GET_CALL_INFORMATION_LIST)
  boolean  status;
  uint8    num_calls;

  PACKED struct PACKED_POST {
    uint8    call_id;
    uint8    call_type;
    uint8    call_state;
    boolean  is_ps_call_dormant;
  } call_info [CM_CALL_ID_MAX];

DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_SET_CLOSED_USER_GROUP_PREFERENCE -- SUBCOMMAND 9
---------------------------------------------------------------------------*/
/* Set CUG preference */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SET_CLOSED_USER_GROUP_PREFERENCE)
uint8 use_cug;
uint8 cug_suppress_oa;
uint16 cug_group_id;

DIAGPKT_REQ_END

#ifdef CM_GW_SUPPORTED
/* Response packet for setting CUG preference*/
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SET_CLOSED_USER_GROUP_PREFERENCE)
uint8 status;
uint8 use_cug;
uint16 cug_group_id;
uint8 cug_suppress_oa;

DIAGPKT_RSP_END

/* Get CUG preference */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GET_CLOSED_USER_GROUP_PREFERENCE)

DIAGPKT_REQ_END

/* Response packet for setting CUG preference*/
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_GET_CLOSED_USER_GROUP_PREFERENCE)
uint8 status;
uint8 use_cug;
uint16 cug_group_id;
uint8 cug_suppress_oa;

DIAGPKT_RSP_END

#endif /* #ifdef CM_GW_SUPPORTED */

/*---------------------------------------------------------------------------
    CM_SET_PASSWORD -- SUBCOMMAND 8
---------------------------------------------------------------------------*/
/* Set Password request message */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SET_PASSWORD)
char  ss_password[MAX_DIAG_PWD_CHAR];
uint8  invoke_id;
DIAGPKT_REQ_END

#ifdef CM_GW_SUPPORTED

/* Response packet for Password set request*/
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SET_PASSWORD)
uint8 invoke_id;
uint8 status;
DIAGPKT_RSP_END
#endif /* #ifdef CM_GW_SUPPORTED */

/*---------------------------------------------------------------------------
    CM_SEND_USSD_REPLY -- SUBCOMMAND 11
---------------------------------------------------------------------------*/
/* Request message for sending USSD reply  */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SEND_USSD_REPLY)
uint8  invoke_id;
uint8  ussd_type;
byte  reply_data_size;
byte  reply_data[MAX_DIAG_USS_CHAR];
DIAGPKT_REQ_END

#ifdef CM_GW_SUPPORTED
/* Response message for request (send USSD reply)*/
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SEND_USSD_REPLY)
uint8  status;
uint8  invoke_id;
uint8  ussd_type;
DIAGPKT_RSP_END
#endif /* #ifdef CM_GW_SUPPORTED */

#ifdef CM_GW_SUPPORTED
/*---------------------------------------------------------------------------
    CM GET CCM INFO -- SUBCOMMAND 12
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GET_CCM_INFO)
 /* No additional fields */
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_GET_CCM_INFO)
 unsigned long ccm;
DIAGPKT_RSP_END
#endif /* #ifdef CM_GW_SUPPORTED */




/*---------------------------------------------------------------------------
    CM_SEND_SUPS_LCS_MOLR -- SUBCOMMAND 13
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR)
uint8   ss_ref;
uint8   molr_type;
boolean location_method_present;
uint8   location_method_data;
boolean lcs_qos_present;
boolean loc_qos_hor_accur_present;
uint8   lcs_qos_hor_accur_data;
boolean lcs_qos_ver_cord_req_present;
boolean lcs_qos_ver_accur_present;
uint8   lcs_qos_ver_accur_data;
boolean lcs_qos_rsp_time_present;
uint8   lcs_qos_rsp_time_categ;
boolean lcs_client_external_id_present;
boolean lcs_client_external_add_present;
uint8   lcs_client_external_add_size;
uint8   lcs_client_external_add_data[CM_LCS_CLIENT_EXT_ADD_DATA_MAX];
boolean mlc_number_present;
uint8   mlc_number_size;
uint8   mlc_number_data[CM_MLC_NUM_DATA_MAX];
boolean gps_assis_data_present;
uint8   gps_assis_data_size;
uint8   gps_assis_data[CM_GPS_ASSIS_DATA_MAX];
boolean supp_gad_shapes_present;
uint8   supp_gad_shapes_data;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR)
uint8 status;
uint8 ss_ref;
DIAGPKT_RSP_END


/*---------------------------------------------------------------------------
    CM_SEND_SUPS_LCS_MOLR_COMPLETED -- SUBCOMMAND 14
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR_COMPLETED)
uint8   ss_ref;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR_COMPLETED)
uint8 status;
uint8 ss_ref;
DIAGPKT_RSP_END




#if (defined (FEATURE_CALL_THROTTLE))
/*---------------------------------------------------------------------------
    CM_ORIG_THR_TBL_RESET -- SUBCOMMAND 15
---------------------------------------------------------------------------*/

/* Request Packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_ORIG_THR_TBL_RESET)
/* No more fields required
*/
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE (CM, CM_ORIG_THR_TBL_RESET)
  boolean status;
DIAGPKT_RSP_END
#endif /* #if (defined (FEATURE_CALL_THROTTLE))*/


/*---------------------------------------------------------------------------
    CM_SYSTEM_SELECTION_PREFERENCE2 -- SUBCOMMAND 16
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE2)
  byte         network_sel_mode_pref;     /* AUTOMATIC/MANUAL      */
  byte         PLMN[3];                   /* PLMN (if manual)      */
  unsigned int mode_pref;                 /* cm_mode_pref_e_type   */
  unsigned int acq_order_pref;            /* cm_gw_acq_order_pref_e*/
  uint64       band_pref;                 /* cm_band_pref_e_type   */
  unsigned int roam_pref;                 /* cm_roam_pref_e_type   */
  unsigned int hybr_pref;                 /* cm_hybr_pref_e_type   */
  unsigned int srv_domain_pref;           /* cm_srv_domain_pref_e_type */
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE2)
  int  status;                            /* SUCCESS/FAILURE_REASON */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
     CM_GET_CELL_SERVICE_IND  -- SUBCOMMAND 17
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GET_CELL_SERVICE_IND)
/* No more fields required
*/
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_GET_CELL_SERVICE_IND)
  uint8 hsdpa_hsupa_support; /* sys_hs_ind_e_type       */
  uint8 dtm_support;         /* sys_dtm_support_e_type  */
  uint8 reserved1;           /* Reserved for future use */
  uint8 reserved2;           /* Reserved for future use */
  uint8 reserved3;           /* Reserved for future use */
DIAGPKT_RSP_END


/*---------------------------------------------------------------------------
     CM_CALL_CMD_MODIFY_REQ -- SUBCOMMAND 20
----------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_CALL_CMD_MODIFY_REQ)
  uint8        call_id;
DIAGPKT_RSP_END

#if (defined CM_GW_SUPPORTED && defined FEATURE_MULTIMEDIA_FALLBACK)
#error code not present
#endif  /* (defined CM_GW_SUPPORTED) && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

/*---------------------------------------------------------------------------
     CM_CALL_CMD_MODIFY_RSP -- SUBCOMMAND 21
----------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_CALL_CMD_MODIFY_RSP)
  uint8                   call_id;
  boolean                 gw_cs_modify_res_para;
  ie_cm_cc_cause_s_type   *cc_cause_ptr_para;
DIAGPKT_RSP_END

#if ((defined CM_GW_SUPPORTED) && defined (FEATURE_MULTIMEDIA_FALLBACK))
#error code not present
#endif  /* (defined CM_GW_SUPPORTED) && defined (FEATURE_MULTIMEDIA_FALLBACK)) */


#ifdef FEATURE_MMODE_DUAL_SIM

/*---------------------------------------------------------------------------
    CM STATE INFO -- SUBCOMMAND 22
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_STATE_INFO_PER_SUBS)
  uint8  asubs_id;
DIAGPKT_RSP_END
/*lint -esym(751, DIAG_SUBSYS_CM_CM_STATE_INFO_req_type)*/

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_STATE_INFO_PER_SUBS)
  int  call_state;                 /* ENUM for EVENT_ID 416 */
  int  oprt_mode;                  /* ENUM for EVENT_ID 417 */
  int  system_mode;                /* ENUM for EVENT_ID 418 */
  int  mode_pref;                  /* ENUM for EVENT_ID 520 */
  int  band_pref;                  /* ENUM for EVENT_ID 521 */
  int  roam_pref;                  /* ENUM for EVENT_ID 522 */
  int  srv_domain_pref;            /* ENUM for EVENT_ID 523 */
  int  acq_order_pref;             /* ENUM for EVENT_ID 524 */
  int  hybr_pref;                  /* ENUM for EVENT_ID 525 */
  int  network_sel_mode_pref;      /* ENUM for EVENT_ID 526 */
  int  srv_status;
  int  asubs_id;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM PLMN LIST -- SUBCOMMAND 23
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_PLMN_SELECTION_MODE_PER_SUBS)
  uint8  asubs_id;
DIAGPKT_RSP_END
/*lint -esym(751, DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_req_type)*/

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_PLMN_SELECTION_MODE_PER_SUBS)
  int  status;                     /* SUCCESS/FAILURE_REASON */
DIAGPKT_RSP_END

/*
* cmlog_check_request_length () checks for the correctness of
* input packet structure depending on the declaration of that structure.
* cmlog_check_request_length () has to be modified for any change
* to the structures of WCDMA/GSM call origination, call end,
* set closed user group, set password and send USSD reply.
*/

/*---------------------------------------------------------------------------
    CM_WCDMA_CALL_ORIGINATION -- SUBCOMMAND 24
---------------------------------------------------------------------------*/
/* WCDMA call origination request */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_WCDMA_CALL_ORIGINATION_PER_SUBS)

 uint8  asubs_id;

 /* DM shall set it to the number of valid digits in DIGITS field */
 uint8  num_digits;

 /* Rate of the vocoder: 0 ~ 7 for
    4.75, 5.15, 5.90, 6.70, 7.40, 7.95, 10.2, 12.2 kbps
 */
 uint8  amr_rate;

 /* Dialed digits; ASCII chars; 0-9, #, * only */
 uint8  digits[DIAG_MAX_WCDMA_DIGITS];

DIAGPKT_REQ_END

#ifdef CM_GW_SUPPORTED
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_WCDMA_CALL_ORIGINATION_PER_SUBS)
  boolean status;
  byte    call_id;
DIAGPKT_RSP_END
#endif /* #ifdef CM_GW_SUPPORTED */


/*---------------------------------------------------------------------------
    CM_GSM_CALL_ORIGINATION -- SUBCOMMAND 25
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GSM_CALL_ORIGINATION_PER_SUBS)

 uint8  asubs_id;

 /* DM shall set it to the number of valid digits in DIGITS field */
 uint8  num_digits;

 /* Dialed digits; ASCII chars; 0-9, #, * only */
 uint8  digits[CM_DIAG_MAX_GSM_DIGITS];

DIAGPKT_REQ_END

#ifdef CM_GW_SUPPORTED
DIAGPKT_SUBSYS_RSP_DEFINE (CM, CM_GSM_CALL_ORIGINATION_PER_SUBS)
  boolean status;
  byte    call_id;
DIAGPKT_RSP_END
#endif /* #ifdef CM_GW_SUPPORTED */


/*---------------------------------------------------------------------------
    CM_GET_CALL_INFORMATION_LIST -- SUBCOMMAND 26
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GET_CALL_INFORMATION_LIST_PER_SUBS)
  uint8  asubs_id;
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_GET_CALL_INFORMATION_LIST_PER_SUBS)
  boolean  status;
  uint8    num_calls;

  PACKED struct PACKED_POST {
    uint8    call_id;
    uint8    call_type;
    uint8    call_state;
    boolean  is_ps_call_dormant;
  } call_info [CM_CALL_ID_MAX];

DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_SET_PASSWORD -- SUBCOMMAND 27
---------------------------------------------------------------------------*/
/* Set Password request message */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SET_PASSWORD_PER_SUBS)
  uint8  asubs_id;
  char   ss_password[MAX_DIAG_PWD_CHAR];
  uint8  invoke_id;
DIAGPKT_REQ_END

#ifdef CM_GW_SUPPORTED

/* Response packet for Password set request*/
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SET_PASSWORD_PER_SUBS)
uint8 invoke_id;
uint8 status;
DIAGPKT_RSP_END
#endif /* #ifdef CM_GW_SUPPORTED */

/*---------------------------------------------------------------------------
    CM_SEND_USSD_REPLY -- SUBCOMMAND 28
---------------------------------------------------------------------------*/
/* Request message for sending USSD reply  */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SEND_USSD_REPLY_PER_SUBS)
  uint8  asubs_id;
  uint8  invoke_id;
  uint8  ussd_type;
  byte  reply_data_size;
  byte  reply_data[MAX_DIAG_USS_CHAR];
DIAGPKT_REQ_END

#ifdef CM_GW_SUPPORTED
/* Response message for request (send USSD reply)*/
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SEND_USSD_REPLY_PER_SUBS)
  uint8  status;
  uint8  invoke_id;
  uint8  ussd_type;
DIAGPKT_RSP_END
#endif /* #ifdef CM_GW_SUPPORTED */

#ifdef CM_GW_SUPPORTED
/*---------------------------------------------------------------------------
    CM GET CCM INFO -- SUBCOMMAND 29
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GET_CCM_INFO_PER_SUBS)
 uint8  asubs_id;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_GET_CCM_INFO_PER_SUBS)
 unsigned long ccm;
DIAGPKT_RSP_END
#endif /* #ifdef CM_GW_SUPPORTED */


/*---------------------------------------------------------------------------
    CM_SEND_SUPS_LCS_MOLR -- SUBCOMMAND 30
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR_PER_SUBS)
  uint8  asubs_id;
  uint8   ss_ref;
  uint8   molr_type;
  boolean location_method_present;
  uint8   location_method_data;
  boolean lcs_qos_present;
  boolean loc_qos_hor_accur_present;
  uint8   lcs_qos_hor_accur_data;
  boolean lcs_qos_ver_cord_req_present;
  boolean lcs_qos_ver_accur_present;
  uint8   lcs_qos_ver_accur_data;
  boolean lcs_qos_rsp_time_present;
  uint8   lcs_qos_rsp_time_categ;
  boolean lcs_client_external_id_present;
  boolean lcs_client_external_add_present;
  uint8   lcs_client_external_add_size;
  uint8   lcs_client_external_add_data[CM_LCS_CLIENT_EXT_ADD_DATA_MAX];
  boolean mlc_number_present;
  uint8   mlc_number_size;
  uint8   mlc_number_data[CM_MLC_NUM_DATA_MAX];
  boolean gps_assis_data_present;
  uint8   gps_assis_data_size;
  uint8   gps_assis_data[CM_GPS_ASSIS_DATA_MAX];
  boolean supp_gad_shapes_present;
  uint8   supp_gad_shapes_data;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR_PER_SUBS)
  uint8 status;
  uint8 ss_ref;
DIAGPKT_RSP_END


/*---------------------------------------------------------------------------
    CM_SEND_SUPS_LCS_MOLR_COMPLETED -- SUBCOMMAND 31
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR_COMPLETED_PER_SUBS)
  uint8  asubs_id;
  uint8   ss_ref;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SEND_SUPS_LCS_MOLR_COMPLETED_PER_SUBS)
  uint8 status;
  uint8 ss_ref;
DIAGPKT_RSP_END


/*---------------------------------------------------------------------------
    CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS -- SUBCOMMAND 32
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS)
  uint8        asubs_id;
  byte         network_sel_mode_pref;     /* AUTOMATIC/MANUAL      */
  byte         PLMN[3];                   /* PLMN (if manual)      */
  unsigned int mode_pref;                 /* cm_mode_pref_e_type   */
  unsigned int acq_order_pref;            /* cm_gw_acq_order_pref_e*/
  unsigned int band_pref;                 /* cm_band_pref_e_type   */
  unsigned int roam_pref;                 /* cm_roam_pref_e_type   */
  unsigned int hybr_pref;                 /* cm_hybr_pref_e_type   */
  unsigned int srv_domain_pref;           /* cm_srv_domain_pref_e_type */
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS)
  int  status;                            /* SUCCESS/FAILURE_REASON */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
     CM_GET_CELL_SERVICE_IND  -- SUBCOMMAND 33
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GET_CELL_SERVICE_IND_PER_SUBS)
  uint8  asubs_id;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_GET_CELL_SERVICE_IND_PER_SUBS)
  uint8 hsdpa_hsupa_support; /* sys_hs_ind_e_type       */
  uint8 dtm_support;         /* sys_dtm_support_e_type  */
  uint8 reserved1;           /* Reserved for future use */
  uint8 reserved2;           /* Reserved for future use */
  uint8 reserved3;           /* Reserved for future use */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
     CM_DUAL_STANDBY_PREF  -- SUBCOMMAND 36
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_DUAL_STANDBY_PREF)
  uint8  default_voice_subs;  /* Subscription to be used for default voice */
  uint8  default_data_subs;   /* Subscription to be used for default data */
  uint8  priority_subs;       /* Priority subscription */
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_DUAL_STANDBY_PREF)
  uint8 status;       /* status indicating if the cmd is queued to CM  */
DIAGPKT_RSP_END


#endif /* FEATURE_MMODE_DUAL_SIM */

/*---------------------------------------------------------------------------
    CM STATE INFO -- SUBCOMMAND 37
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_STATE_INFO2)
 /* No additional fields */
DIAGPKT_RSP_END
/*lint -esym(751, DIAG_SUBSYS_CM_CM_STATE_INFO2_req_type)*/

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_STATE_INFO2)
  int     call_state;                 /* ENUM for EVENT_ID 416 */
  int     oprt_mode;                  /* ENUM for EVENT_ID 417 */
  int     system_mode;                /* ENUM for EVENT_ID 418 */
  int     mode_pref;                  /* ENUM for EVENT_ID 520 */
  uint64  band_pref;                  /* ENUM for EVENT_ID 521 */
  int     roam_pref;                  /* ENUM for EVENT_ID 522 */
  int     srv_domain_pref;            /* ENUM for EVENT_ID 523 */
  int     acq_order_pref;             /* ENUM for EVENT_ID 524 */
  int     hybr_pref;                  /* ENUM for EVENT_ID 525 */
  int     network_sel_mode_pref;      /* ENUM for EVENT_ID 526 */
  int     srv_status;
  uint64  lte_band_pref;              /* ENUM for EVENT_ID 1921 */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_SYSTEM_SELECTION_PREFERENCE3 -- SUBCOMMAND 38
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE3)
  byte         network_sel_mode_pref;     /* AUTOMATIC/MANUAL      */
  byte         PLMN[3];                   /* PLMN (if manual)      */
  unsigned int mode_pref;                 /* cm_mode_pref_e_type   */
  unsigned int acq_order_pref;            /* cm_gw_acq_order_pref_e*/
  uint64       band_pref;                 /* cm_band_pref_e_type   */
  unsigned int roam_pref;                 /* cm_roam_pref_e_type   */
  unsigned int hybr_pref;                 /* cm_hybr_pref_e_type   */
  unsigned int srv_domain_pref;           /* cm_srv_domain_pref_e_type */
  uint64       lte_band_pref;             /* LTE cm_band_pref_e_type */
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE3)
  int  status;                            /* SUCCESS/FAILURE_REASON */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_GET_CDMA_INFO -- SUBCOMMAND 39
---------------------------------------------------------------------------*/
/* Get CDMA info request */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GET_CDMA_INFO)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_GET_CDMA_INFO)
  uint16   active_channel;
  int16    band_class;
  uint16   packet_zone;
  int16    roam_status;
  int32    latitude;
  int32    longitude;
  uint16   mcc;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE -- SUBCOMMAND 40
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE)
  byte         network_sel_mode_pref;     /* AUTOMATIC/MANUAL      */
  byte         PLMN[3];                   /* PLMN (if manual)      */
  unsigned int mode_pref;                 /* ENUM for EVENT_ID 520 */
  unsigned int acq_order_pref;            /* ENUM for EVENT_ID 524 */
  unsigned int band_pref;                 /* ENUM for EVENT_ID 521 */
  unsigned int lte_band_pref;             /* ENUM for EVENT_ID 521 */
  unsigned int roam_pref;                 /* ENUM for EVENT_ID 522 */
  unsigned int hybr_pref;                 /* ENUM for EVENT_ID 525 */
  unsigned int srv_domain_pref;           /* ENUM for EVENT_ID 523 */
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE)
  boolean status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_CALL_ANSWER -- SUBCOMMAND 41
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_CALL_ANSWER)
  uint8   call_id;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_CALL_ANSWER)
  boolean status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_GW_CALL_SETUP_RESPONSE -- SUBCOMMAND 42
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_GW_CALL_SETUP_RESPONSE)
  uint8   call_id;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_GW_CALL_SETUP_RESPONSE)
  boolean status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS -- SUBCOMMAND 43
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS)
  uint8        asubs_id;
  byte         network_sel_mode_pref;     /* AUTOMATIC/MANUAL      */
  byte         PLMN[3];                   /* PLMN (if manual)      */
  unsigned int mode_pref;                 /* cm_mode_pref_e_type   */
  unsigned int acq_order_pref;            /* cm_gw_acq_order_pref_e*/
  uint64       band_pref;                 /* cm_band_pref_e_type   */
  unsigned int roam_pref;                 /* cm_roam_pref_e_type   */
  unsigned int hybr_pref;                 /* cm_hybr_pref_e_type   */
  unsigned int srv_domain_pref;           /* cm_srv_domain_pref_e_type */
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS)
  int  status;                            /* SUCCESS/FAILURE_REASON */
DIAGPKT_RSP_END

#ifdef FEATURE_MMODE_DUAL_SIM

/*---------------------------------------------------------------------------
    CM_MULTISIM_STATE_INFO -- SUBCOMMAND 44
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(CM, CM_MULTISIM_STATE_INFO)
/* No additional fields */
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(CM, CM_MULTISIM_STATE_INFO)
  uint8 active_subs;
  uint8 standby_pref;
  uint8 default_data_subs;
  uint8 priority_subs;
DIAGPKT_RSP_END

#endif /* FEATURE_MMODE_DUAL_SIM */


#ifdef T_WINCE
#pragma pack()
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CMDIAG_H */
