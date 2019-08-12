#ifndef DSATETSICALL_H
#define DSATETSICALL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                U M T S

                C A L L   C O N T R O L
                                
                A T   C O M M A N D   P R O C E S S I N G

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( UMTS call control commands ) processor.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 - 2016 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.
  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsicall.h_v   1.5   12 Jul 2002 10:05:20   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsatetsicall.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/16   skc     Fixed USSD issue when both MO and MT USSD present.
06/09/16   skc     Fixed issue with at+chld=2 during MPTY calls
12/16/15   skc     Fixed issue where wrong call id is stored during one 
                   active call and one held call and at+chld=2 is issued.
04/15/15   sc      Fixed issue in event handling to use subs_id provided in 
                   event info.
08/01/14   sc      Added support for linked MO and MT USSD sessions.
07/08/14   sc      Fixed SGLTE+G issue on SUB2.
06/27/14   tk/sc   Added support for Dynamic ATCoP.
02/15/14   pg      Added support for SGLTE+G.
04/29/14   sc      Added csg_list_cat support to $QCCSGCOPS command.
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
07/02/13   pg      Migrated to MMGSDI API to find PLMN info in SE13 table
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
05/16/13   sk      $QCCOPS command for Extension of network search.
02/22/13   tk      ATCoP - CM interface changes for Triton DSDA.
01/23/13   sk      DS command buffer memory reduction.
11/30/12   tk      ATCoP changes for Triton DSDA.
10/12/12   sk      Support to add "No Change" when  value is not given in +COPS.
07/31/12   sk      C-EONS Feature Support.
03/07/12   sk      Made CLCC common for all modes.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
12/26/11   ad      Added Support the New Call Control Interface in Call Manager.
01/19/12   sk      Feature cleanup.
10/17/11   ad      Added +CECALL support.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
04/07/11   ad      Fix to ignore unexpected password indication from CM.
10/04/10   ua      Increasing LONG network name to be 24 characters. 
08/30/10   ad      Fixed cops act issue.
06/04/10   bs      MMGSDI SESSION Migration.
05/10/10   kk      Mainlining pending GOBI changes.
03/15/10   bs      Added +CQI support.
01/15/10   sa      Added support for +CPIN,^CPIN and +CLCK for RUIM cards.
12/15/09   nc      Featurisation changes for LTE.
11/30/09   bs      Added +COPS support for LTE.
06/11/09   nc      Modified the waiting call end using +CHUP to 
                   reflect the correct cause code.
12/29/08   nc      Added support for +VTS Command
12/12/08   ua      Fixed Off target lint errors.
12/10/08   ss      Off Target Lint fix.
11/04/07   pp      Lint low - fixes.
09/07/07   ua      Updated supserv_cm_op_s_type to support +CUSD abort
                   for NW initiated USSD operations.
08/24/07   ar      Fix feature wrapping for non-GCSD builds.
07/27/07   ar      Added dsatetsicall_update_crlp_param_set() prototype.
04/05/07   ua      Added support for COPS DEREGISTRATION;
03/21/07   sa      Add support for BS code not being sent in case of class 255 
                   to map to All tele and bearerservices of MMI code.
03/02/07   sa      Added outgoing voice call support when there is an active call  
                   for ATD command.
02/15/07   ua      Added new phone book "FD" support.
02/13/07   pkp     Added comments on the usage of DSAT_INVALID_OP_CODE.
07/11/06   ua      CACM/CAMM modifications according to the new CM API's
02/21/06   sa      Merged Changes from Sirius Branch
                   -Added +CLIR interrogation result enums.
05/12/06   ua      Function dsatetsicall_display_pref_networks is wrapped 
                   under FEATURE_MMGSDI 
01/19/06   ua      Added support for +CTFR command.
08/26/05   tkk     Added a new macro for CLI validity.
11/30/05   snb     Add support for +CGCMOD command.
09/07/05   ar      Added dsatetsicall_init_call().
07/25/05   snb     Add external RAB event handler function to support 
                   WCDMA-mode $QCDGEN.
05/05/05   iv      Added call id to sequence number mapping for +CLCC
04/29/05   ar      Add support for +CEER command.
04/27/05   ar      Add support for +CV120 command.
04/12/05   ar      Add support for +COPS access technology selection.
04/05/05   iv      Modified +CLCC to support CS data calls.
02/28/05   hap     Added support for +CSSN command
02/14/05   dvp     Added support for Personalization facility in +CLCK
02/03/05   iv      Added support for +CPOL command.
01/31/05   iv      Added support for +COPN command.
01/27/05   pdv     Merge from Sirius-ATCoP development.
01/27/05   pdv     Rebase from Mainline.
01/20/05   tkk     Added prototypes for +CLIP abort and event handlers.
01/05/05   iv      Added support for +CLCC command
12/19/04   tkk     Added prototypes for CLIP, COLP functions.
12/15/04   snb     Add ATH to command enum and make some variables external.
12/07/04   ar      Add support for +COPS command.
11/09/04   ar      Add support for +CPUC.
10/15/04   ar      Add +CAOC,+CACM,+CAMM support.
09/03/04   snb     Add +CHLD support.
05/12/04   snb     Fix +CGACT response when deactivating contexts activated via
                   embedded sockets.
04/06/04   ar      Added dsatetsicall_init_call_param_from_nv() prototype
03/19/04   snb     Added support for +CCWA command.
10/07/03   ar      Add +CREG and +CGREG support
09/05/03   ar      Added typedef dsat_sups_cmd_info_u_type.  Adjust
                   function prototypes for new parameter.
07/30/03   ar      Added prototypes for +CRLP command.
06/26/03   sb      Moved dsatetsicall_voice_cmd_orig_cb_handler() to common
                   files. Now it is dsatvoice_cmd_orig_cb_handler().
06/24/03   ar      Remove LOCAL on supserv_cm_op_s and supserv_cm_parm_u.
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
01/08/02   wx      Added support for +ES and +ESA command
12/09/02   ar      Added support for +CUSD command
12/02/02   ar      Added support for +CPWD command
11/15/02   ar      Added support for +CCUG and +CLCK commands
10/31/02   ar      Added support for +CCFC command
10/28/02   sb      Opened Voice call related handlers (moved to a common file)
10/17/01   sb      created file

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#ifdef FEATURE_ECALL_APP 
#include "ecall_app.h"
#include "dsatvend.h"
#endif /* FEATURE_ECALL_APP */
/* This file should be included only in GSM or WCDMA mode */
#ifdef FEATURE_DSAT_ETSI_MODE

#include "mn_cm_exp.h"
#include "dsati.h"
#include "dsatcmif.h"
#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) 
#include "dsucsdappif.h"
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

/*===========================================================================

            PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains public definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
    Typedefs for Supplementary Services support
---------------------------------------------------------------------------*/

#define DSAT_MAX_MMI_STRING_LEN  82 /*Maximum length of MMI string (+1 for NULL character)*/

/* Enum to track the current command executing. */
/* Mapping is a follows:                        */
/*   CALL_FWD --> +CCFC                         */
/*   FACILITY --> +CLCK or +CPWD                */
/*   USSD     --> +CUSD                         */
/*   CALL_WT  --> +CCWA                         */
typedef enum ss_operation_e
{
  DSAT_SSOP_CALL_FWD,
  DSAT_SSOP_FACILITY,
  DSAT_SSOP_USSD,
  DSAT_SSOP_CALL_WT,
  DSAT_SSOP_MAX
} ss_operation_e_type;

typedef enum ccfc_reason_e
{
  DSAT_CCFC_REASON_UNCOND    = 0,
  DSAT_CCFC_REASON_BUSY      = 1,
  DSAT_CCFC_REASON_NOREPLY   = 2,
  DSAT_CCFC_REASON_NOTREACH  = 3,
  DSAT_CCFC_REASON_ALLCALL   = 4,
  DSAT_CCFC_REASON_ALLCOND   = 5,
  DSAT_CCFC_REASON_MAX
} ccfc_reason_e_type;

typedef enum ccfc_mode_e
{
  DSAT_CCFC_MODE_DISABLE    = 0,
  DSAT_CCFC_MODE_ENABLE     = 1,
  DSAT_CCFC_MODE_QUERY      = 2,
  DSAT_CCFC_MODE_REG        = 3,
  DSAT_CCFC_MODE_ERASURE    = 4,
  DSAT_CCFC_MODE_MAX
} ccfc_mode_e_type;

typedef enum ccwa_mode_e
{
  DSAT_CCWA_MODE_DISABLE    = 0,
  DSAT_CCWA_MODE_ENABLE     = 1,
  DSAT_CCWA_MODE_QUERY      = 2,
  DSAT_CCWA_MODE_MAX
} ccwa_mode_e_type;

/* This enum must match order of dsat_clck_fac_valstr[] */
/* and dsat_cpwd_fac_valstr[]                           */
typedef enum ss_fac_e
{
  DSAT_SS_FAC_ALLB,    /* All Barring Services */
  DSAT_SS_FAC_AINB,    /* All Incoming Barring Services */
  DSAT_SS_FAC_AOTB,    /* All Outgoing Barring Services */
  DSAT_SS_FAC_BAIC,    /* Barr All Incoming Calls */
  DSAT_SS_FAC_BAOC,    /* Barr All Outgoing Calls */
  DSAT_SS_FAC_BICR,    /* Barr Incoming Calls when Roaming */
  DSAT_SS_FAC_BOIC,    /* Barr Outgoing International Calls */
  DSAT_SS_FAC_BOICHC,  /* BOIC except to Home Country */
  DSAT_SS_FAC_PIN1,    /* SIM PIN1 Lock */
  DSAT_SS_FAC_PIN2,    /* SIM PIN2 Lock */
  DSAT_SS_FAC_MAX
} ss_fac_e_type;

/* States for USSD */
typedef enum cusd_state_e
{
  DSAT_CUSD_STATE_NULL,            /* No pending USSD transaction */
  DSAT_CUSD_STATE_MS_SEND,         /* MS initiated transaction    */
  DSAT_CUSD_STATE_MS_ABORT,        /* MS abort of transaction     */
  DSAT_CUSD_STATE_SS_REQUEST,      /* SS requested data           */
  DSAT_CUSD_STATE_SS_NOTIFY,       /* SS notification received    */
  DSAT_CUSD_STATE_CLEAR_MT_USSD,   /* Clear MT USSD Session */
  DSAT_CUSD_STATE_MAX
} cusd_state_e_type;

typedef enum cusd_result_e
{
  DSAT_CUSD_RESULT_DONE    = 0,    /* No further action required   */
  DSAT_CUSD_RESULT_MORE    = 1,    /* Further user action required */
  DSAT_CUSD_RESULT_ABORT   = 2,    /* USSD terminated by network   */
  DSAT_CUSD_RESULT_OTHER   = 3,    /* Other local client responded */
  DSAT_CUSD_RESULT_NOSUP   = 4,    /* Operation not supported      */
  DSAT_CUSD_RESULT_TIMEOUT = 5,    /* Network time out             */
  DSAT_CUSD_RESULT_MAX
} cusd_result_e_type;

typedef enum ss_class_e
{
  DSAT_SS_CLASS_MIN        = 0x0,
  DSAT_SS_CLASS_VOICE      = 0x1,
  DSAT_SS_CLASS_DATA       = 0x2,
  DSAT_SS_CLASS_FAX        = 0x4,
  DSAT_SS_CLASS_SMS        = 0x8,
  DSAT_SS_CLASS_DATA_SYNC  = 0x10,
  DSAT_SS_CLASS_DATA_ASYNC = 0x20,
  DSAT_SS_CLASS_DATA_PKT   = 0x40,
  DSAT_SS_CLASS_DATA_PAD   = 0x80,
  DSAT_SS_CLASS_ALL        = 0xFF,
  DSAT_SS_CLASS_MAX        = 0xFFFF
} ss_class_e_type;

typedef enum dsat_cssi_e
{
  DSAT_CSSI_ORIG_FWD_STAT         = -1,
  DSAT_CSSI_UNCOND_FWD_ACTIVE     = 0,
  DSAT_CSSI_COND_FWD_ACTIVE       = 1,
  DSAT_CSSI_CALL_FORWARDED        = 2,
  DSAT_CSSI_CALL_WAITING          = 3,
  DSAT_CSSI_CUG_CALL              = 4,
  DSAT_CSSI_OUTGOING_CALLS_BARRED = 5,
  DSAT_CSSI_INCOMING_CALLS_BARRED = 6,
  DSAT_CSSI_CLIR_SUPPRESSION_REJ  = 7,
  DSAT_CSSI_CALL_DEFLECTED        = 8,
  DSAT_CSSI_MAX 
}dsat_cssi_e_type;

typedef enum dsat_cssu_e
{
  DSAT_CSSU_FORWARDED_CALL                = 0,
  DSAT_CSSU_CUG_CALL                      = 1,
  DSAT_CSSU_CALL_HOLD                     = 2,
  DSAT_CSSU_CALL_RETRIEVED                = 3,
  DSAT_CSSU_MPTY_CALL                     = 4,
  DSAT_CSSU_CALL_HOLD_RELEASED            = 5,
  DSAT_CSSU_FWD_CHECK_SS_RECVD            = 6,
  DSAT_CSSU_ECT_CALL_REMOTE_PTY_ALERT     = 7,
  DSAT_CSSU_ECT_CALL_REMOTE_PTY_CONNECTED = 8,
  DSAT_CSSU_DEFLECTED_CALL                = 9,
  DSAT_CSSU_ADDITIONAL_INCOM_CALL_FWD     = 10,
  DSAT_CSSU_MAX 
}dsat_cssu_e_type;

/* To support asynchronous processing for +CACM/CAMM read commands */
typedef enum dsat_acm_e
{
  DSAT_ACM_NONE,
  DSAT_ACM_READ,    /* CACM Read */
  DSAT_ACM_MAX_READ /* CAMM Read */
}dsat_acm_e_type;

#define DSAT_CREG_STAT_CNG_UNLCK             0
#define DSAT_CREG_STAT_CNG_LCK               1
#define DSAT_CREG_STAT_CNG_CS_LCK            2
#define DSAT_CREG_STAT_CNG_PS_LCK            4
#define DSAT_CREG_STAT_CNG_CSPS_LCK          8

typedef struct bsg_conversion_s
{
  ss_class_e_type class; 
  bsg_code_type   bsg_type;/* BSG type - bearer services, teleservices */
  byte   net_bsg_code;     /* network BSG value */
} bsg_conversion_s_type;

/* Call forwarding user data */
typedef struct ss_cf_user_s
{
  ccfc_reason_e_type reason;                      /* Reason code      */
  ccfc_mode_e_type mode;                          /* Mode code        */
  uint8 type;                                     /* Address type     */
  uint8 satype;                                   /* Subaddress type  */
  uint8 time;                                     /* Time value       */
  byte  alength;                                  /* Address length   */
  byte  addr[CM_CALLED_PARTY_BCD_NO_LENGTH];      /* Address (number) */
  byte  subaddr[CM_CALLED_PARTY_SUBADDRESS_LENGTH+1];  /* Subaddress  */
} ss_cf_user_s_type;

/* Facility lock user data */
typedef struct ss_fl_user_s
{
  ss_fac_e_type facility;                         /* Facility code    */
  clck_mode_e_type mode;                          /* Mode code        */
/*  Password length should be MAX(GSDI_MAX_PIN_LEN, MAX_PWD_CHAR) + 1 currently,
    GSDI_MAX_PIN_LEN is 8 so using that */
  byte  passwd[MMGSDI_PIN_MAX_LEN + 1]; /* Passwords */
  byte  new_passwd[MMGSDI_PIN_MAX_LEN + 1];
} ss_fl_user_s_type;

/* USSD user data */
typedef struct ss_us_user_s
{
  uint8 dcs;                                  /* Data coding scheme   */
  byte  string[MAX_USS_CHAR_ASCII+1];               /* USSD string          */
} ss_us_user_s_type;

/* Call waiting user data */
typedef struct ss_cw_user_s
{
  ccwa_mode_e_type mode;                      /* Mode code            */
} ss_cw_user_s_type;

/* User data operation union */
typedef union ss_user_data_u
{
  ss_cf_user_s_type call_fwd;                 /* Call forwarding data */
  ss_fl_user_s_type fac_lock;                 /* Facility lock data   */
  ss_us_user_s_type ussd;                     /* USSD data            */
  ss_cw_user_s_type call_wt;                  /* Call Waiting data    */
} ss_op_data_u_type;

/* User entered data */
typedef struct ss_user_data_s
{
  uint8 class;                                 /* Class user entered  */
  ss_operation_e_type operation;               /* Operation underway  */
  ss_op_data_u_type opdata;                    /* Operation data      */
} ss_user_data_s_type;

/* Call Manager sups types */
typedef union supserv_cm_parm_u
{
  cm_reg_ss_params_s_type      reg;            /* Registration        */
  cm_erase_ss_params_s_type    erase;          /* Erasure             */
  cm_act_ss_params_s_type      act;            /* Activation          */
  cm_deact_ss_params_s_type    deact;          /* Deactivation        */
  cm_interr_ss_params_s_type   interr;         /* Interrogation       */ 
} supserv_cm_parm_u_type;

/* CUSD Session ID */
typedef enum cusd_session_id_e
{
  DSAT_CUSD_SESSION_ID_MO  = 0, /* MO USSD Session */
  DSAT_CUSD_SESSION_ID_MT  = 1, /* MT USSD Session */
  DSAT_CUSD_SESSION_ID_MAX = 2  /* Maximum number of USSD sessions */
} cusd_session_id_e_type;

/* CUSD Session Info */
typedef struct cusd_session_info_s
{
  boolean is_active;      /* Session active flag */
  uint8   invoke_id;      /* Network invocation ID */
  uint8   ussd_oper_type; /* USSD operation type */
} cusd_session_info_s_type;

typedef enum cops_fmt_e
{
  DSAT_COPS_FMT_LONG = 0,  /* Long format alphanumeric */
  DSAT_COPS_FMT_SHRT = 1,  /* Short format alphanumeric */
  DSAT_COPS_FMT_PLMN = 2,  /* Numeric PLMN */
  DSAT_COPS_FMT_MAX      /* Internal use only */
} dsat_cops_fmt_e_type;

/* To keep track of +CHLD Sups/+CHUP command in progress and know events that
   must occur before result code can be given. If 0x80 bit set the 
   CM_CALL_EVENT_MNG_CALLS_CONF event hasn't been received yet for +CHLD 
   commands. */
typedef enum
{
  CMD_CHLD0  = CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB,
  CMD_CHLD1  = CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL,
  CMD_CHLD1x = CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL,
  CMD_CHLD2  = CM_CALL_SUPS_TYPE_HOLD_CALL,
  CMD_CHLD2x = CM_CALL_SUPS_TYPE_SELECT_CALL_X,
  CMD_CHLD3  = CM_CALL_SUPS_TYPE_MULTIPARTY_CALL,
  CMD_CHLD4  = CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER,
  CMD_CHUP,
  CMD_ATH,
  CMD_CONF   = 0x40,
  CMD_NONE   = 0x80
} etsicall_cmd_type;

/* Call command types. */
typedef enum
{
  DSAT_CALL_CMD_ORIG,
  DSAT_CALL_CMD_ANSWER,
  DSAT_CALL_CMD_END,
  DSAT_CALL_CMD_SUPS_CHLD_0,
  DSAT_CALL_CMD_SUPS_CHLD_1,
  DSAT_CALL_CMD_SUPS_CHLD_1X,
  DSAT_CALL_CMD_SUPS_CHLD_2,
  DSAT_CALL_CMD_SUPS_CHLD_2X,
  DSAT_CALL_CMD_SUPS_CHLD_3,
  DSAT_CALL_CMD_SUPS_CHLD_4
} dsat_call_cmd_e_type;

typedef enum 
{
  DSAT_CSG_LIST_CAT_UNKNOWN = 0x00,
  DSAT_CSG_LIST_CAT_ALLOWED = 0x01,
  DSAT_CSG_LIST_CAT_OPERATOR = 0x02
} dsat_csg_list_cat_e_type;

extern boolean            dsat_cdip_pending[DSAT_SYS_MODEM_AS_ID_MAX];
extern etsicall_cmd_type etsicall_cmd_pending[DSAT_SYS_MODEM_AS_ID_MAX];

#define DSATETSICALL_CAOC_REPORT_TIMER 10000  /* Ten second interval*/
extern rex_timer_type dsatetsicall_aoc_timer;
extern rex_timer_type dsatetsicall_inband_dtmf_timer;

#define DSAT_CLIP_ENABLED  (dsat_num_item_type)1
#define DSAT_COLP_ENABLED  (dsat_num_item_type)1
#define DSAT_CDIP_ENABLED  (dsat_num_item_type)1

#define DSAT_MAX_LINEID_BUF        100
#define DSAT_MAX_CALLED_LINEID_BUF 100

#define DSAT_NO_NW_SERVICE (uint32)2

#define DSAT_CLIP_NUMBER_TYPE_UNKNOWN 128
#define DSAT_CDIP_NUMBER_TYPE_UNKNOWN 128
/* CLIP, COLP and CLIR lie in the range 11 through 13 */

/* This opcode also mapped to all_ss in ss_operation_code_T enum */
#define DSAT_INVALID_OP_CODE            0

#define DSAT_CLIP_NUMBER_CLI_VALIDITY (byte)2

/* This enum represents the CLIR provisioning status at the network */
typedef enum dsat_clir_status_e {
  DSAT_CLIR_SRV_NOT_PROVISIONED         = 0,
  DSAT_CLIR_SRV_PROVISIONED_PERMANENT   = 1,
  DSAT_CLIR_SRV_NO_NETWORK              = 2,
  DSAT_CLIR_SRV_PRESENTATION_RESTRICTED = 3,
  DSAT_CLIR_SRV_PRESENTATION_ALLOWED    = 4 
} dsat_clir_status_e_type;

/* CLIR enable/disable settings */
typedef enum clir_type_e{
  DSAT_CLIR_PRESENTATION_INDICATOR = 0,
  DSAT_CLIR_INVOCATION_OPTION  = 1,
  DSAT_CLIR_SUPPRESSION_OPTION = 2
}dsat_clir_e_type;

#define DSAT_CLI_ALLOWED        (byte)0
#define DSAT_CLI_RESTRICTED     (byte)1
#define DSAT_CLI_NOT_AVAILABLE  (byte)2
#define DSAT_CLI_INVALID        (byte)2

/* +CLIR subscription info variable */
#ifdef FEATURE_ECALL_APP 
/* Type of CEcall */
typedef enum {
  DSAT_CECALL_TEST    = 0,
  DSAT_CECALL_RECONF  = 1,
  DSAT_CECALL_MANUAL  = 2,
  DSAT_CECALL_AUTO    = 3,
  DSAT_CECALL_MAX     = 4
}dsat_cecall_e_type;
#endif /* FEATURE_ECALL_APP */

/* Capture the call end status codes for +CEER command */
typedef enum
{
  DSAT_CES_DOMAIN_NULL,    /* Internal use only */
  DSAT_CES_DOMAIN_CS,
  DSAT_CES_DOMAIN_PS,
  DSAT_CES_DOMAIN_MAX      /* Internal use only */
} dsatetsicmif_ces_domain_e_type;

typedef  struct
{
  cm_call_end_e_type         end_status;
  ie_cm_cc_cause_s_type      cc_cause;
  ie_cm_cc_reject_s_type     cc_reject;
} ceer_cs_domain_s_type;

typedef struct
{
  sys_sys_mode_e_type        sys_mode;
  cause_enum_type_T          pdp_cause_type;
  pdp_cause_T                cause;
  /*for lte cause codes*/
  esm_cause_T                esm_cause;
  lte_nas_local_cause_T      esm_local_cause;
} ceer_ps_domain_s_type;

typedef struct dsatetsicmif_ces_info_s
{
  dsatetsicmif_ces_domain_e_type domain;
  union
  {
    ceer_cs_domain_s_type  cs_domain;
    ceer_ps_domain_s_type  ps_domain;
  } error_info;
} dsatetsicmif_ces_info_s_type;

typedef struct
{
  cm_call_id_type call_id;
  cm_call_substate_type sub_state;
}dsat_clcc_call_info;

typedef struct {
  boolean                 is_profile_id_valid;
  uint16                  profile_id;
  dsat_num_item_type      subs_id;
  dsat_call_abort_cb_type data_s7_abort_handler;
}dsat_s7_info_type;

#define MAX_COPN_LEN                  6
#define DSAT_PUCT_LEN                 5
#define DSAT_MAX_DATA_CALLS           2
/*SS*/
typedef struct dsat_call_state_ss_info
{
  byte                         dsatetsicall_cpuc_mmgsdi_data[DSAT_PUCT_LEN];
/* To store the index value of next PLMN stored on the ME */
  int dsatetsicall_copn_plmn_index;
  char etsicall_copn_display_name[MAX_COPN_LEN];
  dsat_s7_info_type dsatetsicall_s7_info[DSAT_MAX_DATA_CALLS];
  
}dsatetsicall_call_state_ss_info;

/*DSDA*/
typedef struct  dsat_call_state_da_info
{
/* CLIP and COLP bufs are integrated in one as dsat_lineid_buf */
  byte                     dsat_lineid_buf[DSAT_MAX_LINEID_BUF];
  uint32                   dsat_lineid_buf_length;
  byte                     dsat_called_lineid_buf[DSAT_MAX_CALLED_LINEID_BUF];
  uint32                   dsat_called_lineid_buf_length;
/* Theses variables needed because CM_CALL_EVENT_MNG_CALLS_CONF may not precede
   call END/CONNECT events as described in CM documentation. If a waiting 
   call is connected and made active and then MNG_CALLS_CONF occurs, state
   info has been lost and correct course of action may not be clear.
   These variables keeps track of any waiting call at time command
   was issued. */
  boolean etsicall_was_waiting;
  cm_call_id_type was_waiting_call_id;
/* These two flags are introduced to handle scenario where one active call 
   and one held call are there and at+chld=2 is issued */
  boolean was_active_call_hold;
  boolean was_hold_call_retrieved;
/* This variable is needed to filter CM_CALL_EVENT_MNG_CALLS_CONF in cases 
   where a call is originated by ATD when another call is already up */ 
  boolean etsicall_was_call_active;
/* To keep track of calls in effect */
  uint8 etsicall_num_active_calls;
  cm_call_id_type etsicall_active_call_ids[CM_CALL_ID_MAX];
  uint8 etsicall_num_held_calls;
  cm_call_id_type etsicall_held_call_ids[CM_CALL_ID_MAX];
  boolean etsicall_wait_call_present;
/* Flag to handled scenario in MNG_CALLS event for MPTY calls during
   at+chld=2 */
  boolean etsicall_was_mpty_call;
  cm_call_id_type etsicall_wait_call_id;
#ifdef FEATURE_DUAL_ACTIVE
  uint8 etsicall_num_local_held_calls;
  cm_call_id_type etsicall_local_held_call_ids[CM_CALL_ID_MAX];
#endif /* FEATURE_DUAL_ACTIVE */
/* To keep track of the sequence of setting up or receiving the calls 
   (active, held or waiting) as seen by the served subscriber, described
   in 3GPP TS 22.030 section 6.5.5.1 and required by +CHLD command. */
  dsat_seqnum_s_type dsat_seqnum_callid[CM_CALL_ID_MAX];
/* Capture the call end status information for +CEER command */
  dsatetsicmif_ces_info_s_type dsatetsicmif_call_end_status;
#ifdef FEATURE_DSAT_ETSI_DATA
  cm_call_end_e_type dsat_pdp_deactivation_cause;
#endif /* FEATURE_DSAT_ETSI_DATA */
#ifdef FEATURE_DSAT_GOBI_MAINLINE
/*call manager current incoming call info*/
  cm_num_s_type      dsat_cm_curr_incoming_call_num;
#endif /* FEATURE_DSAT_GOBI_MAINLINE */
  uint8   dsatact_num_clcc_calls;
/* Counter which says number of active/in conversation voice calls
   based on which we can find out whether conferance call is there or not */
  uint8   dsatact_clcc_mpty_calls;                       
  boolean dsat_abort_call_info_listing;
 /* To store CM call ids of all CS calls */
  dsat_clcc_call_info dsatcmif_clcc_no_call_ids[CM_CALL_ID_MAX];
}dsatetsicall_call_state_da_info;

/*MSMS*/
typedef struct  dsat_call_state_ms_info
{
  dsat_acm_e_type          dsat_acm_res_type;
  /* Advice of Charge state variables */
  uint32                   etsicall_prev_ccm_val ;
  dsat_num_item_type       etsicall_acmmax;
}dsatetsicall_call_state_ms_info;

typedef struct dsat_call_state_info
{
  dsatetsicall_call_state_ss_info ss;
  dsatetsicall_call_state_da_info da[DSAT_SYS_MODEM_AS_ID_MAX];
  dsatetsicall_call_state_ms_info ms[DSAT_SYS_MODEM_AS_ID_MAX];
}dsatetsicall_call_state_info;
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION  DSATETSICALL_INIT_CALL

DESCRIPTION
  This function intializes ETSI call handling.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatetsicall_init_call ( void );

/*===========================================================================

FUNCTION  DSATETSICALL_SEND_CR_RESULT

DESCRIPTION
  Sends intermediate service reporting result code to TE or PS protocol
  stack based on response routing provided by response router.

  Should be called from mode specific protocol stack at time during
  connection negotiation that speed and quality of service is determined 
  and before compression and connect result code are sent.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatetsicall_send_cr_result
(
  dsat_cr_service_e_type service
);

/*===========================================================================

FUNCTION  DSATETSICALL_SEND_CR_RESULT_EX

DESCRIPTION
  Sends intermediate service reporting result code to TE or PS protocol
  stack based on response routing provided by response router.

  Should be called from mode specific protocol stack at time during
  connection negotiation that speed and quality of service is determined 
  and before compression and connect result code are sent.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatetsicall_send_cr_result_ex
(
  dsat_cr_service_e_type service,
  ds3g_siolib_port_e_type port 
);

/*===========================================================================

FUNCTION  DSATETSICALL_EXEC_CEER_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +CEER command, which report the reason for
  the last call failure to setup or release.  Both CS and PS domain
  call types are reported on.  Cause data is captured from Call
  Manager events and cached locally to later use by this command.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_OK : if it is a success for local execution
  
SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_ceer_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION  DSATETSICALL_SET_CRC_RESULT

DESCRIPTION
  Produces unformatted incoming call indication or GPRS network request
  for PDP context activation result and returns in DSM item.

  If AT+CRC is 0, only RING is returned.
  If AT+CRC is 1, the extendted format RING <type> is returned.
                  <type> is defined in Section 6.11 of 3GPP TS 27.007

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to DSM item containing unformatted CRC result.
  
SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *dsatetsicall_set_crc_result
(
  dsat_result_enum_type crc_code
);

/*===========================================================================

FUNCTION DSATETSICALL_VOICE_DIAL_CMD

DESCRIPTION
  Handle atd voice dial commands.  This function is called by 
  dsatact_exec_atd_cmd when dial string has ;.

  This function initiates a voice call and goes to DIALING state.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ASYNC_CMD : If the command succeeds.  CM will call the cmd cb 
                   function later to report status.
  DSAT_ERROR:      If the command fails.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_voice_dial_cmd
(
  const dsati_dial_str_type * dial_str        /* containing dial string 
                                                 and its attribs*/
);


/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CHUP_CMD

DESCRIPTION
  This is the command handler function for +CHUP.

  This function sends request to CM to end current ETSI voice calls for
  action command.

  Return OK for test command.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : if the action command succeeds.
  DSAT_OK        : if there is no active voice call or if this
                   is a test command.
  DSAT_ERROR     : if the action command fails.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_chup_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CREG_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CREG and +CGREG commands, which manage the parameters
  for network registration reporting.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_creg_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_REPORT_CREG_RESULT

DESCRIPTION
  This function generates the unsolicited result code for registration
  commands +CREG and +CGREG.  The command parameter <n> controls whether
  results are presented or suppressed.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_report_creg_result
(
  dsat_num_item_type      stack_id,
  sys_modem_as_id_e_type  subs_id,
  dsat_creg_report_e_type report_creg
);

/*===========================================================================

FUNCTION DSATETSICALL_CREG_STATE_MACHINE

DESCRIPTION
  This function generates the unsolicited result code for registration
  commands +CREG and +CGREG.  The command parameter <n> controls whether
  results are presented or suppressed.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_creg_state_machine
(
  const ds_at_cm_ss_event_type * event_ptr,        /* DS event pointer */
  dsat_stack_id_e_type           stack_id,
  sys_modem_as_id_e_type         subs_id
);

/*===========================================================================

FUNCTION  DSATETSICALL_REPORT_QCRCIND_RESULT

DESCRIPTION
  This functions displays the MO call unsolicited result codes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
 void dsatetsicall_report_qcrcind_result
(
sys_modem_as_id_e_type        subs_id,
cm_call_event_e_type         *call_event_ptr
);

/*===========================================================================

FUNCTION ETSICALL_MOVE_CALLS

DESCRIPTION
  This function moves calls from one call list to another, excluding call
  excl_call_id if it is something other than CM_CALL_ID_INVALID. It is used by
  event handlers that maintain call state info.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of validation.
  possible values:
    TRUE:   if call id to be excluded was found
    FALSE : if not found

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsicall_move_calls
(
  uint8 *fm_qty,
  cm_call_id_type *fm_list,
  uint8 *to_qty,
  cm_call_id_type *to_list,
  const cm_call_id_type excl_call_id
);

/*===========================================================================

FUNCTION DSATETSICALL_UPDATE_CREG_LAC_CELL_ID

DESCRIPTION
  This function update CELL ID/LAC ID when COPS command is under process.
  It also set CREG pending flag.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_update_creg_lac_cell_id
(
  const ds_at_cm_ss_event_type  *event_ptr,        /* DS event pointer */
  dsat_stack_id_e_type           stack_id,
  sys_modem_as_id_e_type         subs_id,
  dsat_creg_report_e_type       *report_creg
);

/*===========================================================================

FUNCTION DSATETSICALL_RESET_LAST_REPORTED_CREG

DESCRIPTION
  This function reset last reported creg parameter

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_reset_last_reported_creg(dsatcmif_servs_state_ms_info  *ph_ss_ms_val);
/*===========================================================================

FUNCTION DSATETSICALL_SET_NET_REG

DESCRIPTION
  This function set last reported domain.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void dsatetsicall_set_net_reg
(
  net_reg_domain_e_type   domain ,
  net_reg_status_e_type   value,
  sys_modem_as_id_e_type subs_id
);
/*===========================================================================

FUNCTION DSATETSICALL_GET_NET_REG

DESCRIPTION
  This function return last reported domain.

DEPENDENCIES
  None
  
RETURN VALUE
  Domain Net registation status

SIDE EFFECTS
  None
  
===========================================================================*/

net_reg_status_e_type  dsatetsicall_get_net_reg
( 
  net_reg_domain_e_type  domain,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================

FUNCTION DSATETSICALL_CREG_GET_NET_REG_STATUS

DESCRIPTION
  This function return CREG network registration status based on COPS 
  and CFUN value.

DEPENDENCIES
  None
  
RETURN VALUE
  NET REG STATUS

SIDE EFFECTS
  None
  
===========================================================================*/
net_reg_status_e_type dsatetsicall_creg_get_net_reg_status
(
  const ds_at_cm_ss_event_type * event_ptr,        /* DS event pointer */
  dsat_stack_id_e_type           stack_id,
  sys_modem_as_id_e_type         subs_id
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CCUG_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CCUG command, which manages the parameters for Closed User
  Group supplementary service acording to 3GPP TS 22.085

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_ccug_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_WCDMA_L1_HS_CQI_STAT
/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CQI_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CQI command, which reports the Channel Quality Indicator.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cqi_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_REPORT_CQI_RESULT

DESCRIPTION
  This function generates the unsolicited result code for +CQI command.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_report_cqi_result ( void );
#endif /* FEATURE_WCDMA_L1_HS_CQI_STAT  */
/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CCFC_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CCFC command, which controls the call forwarding
  supplementary service acording to 3GPP TS 22.082

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_ccfc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CTFR_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CTFR command, which controls the call deflection (call transfer)
  supplementary service acording to 3GPP TS 22.072 [30]

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_ctfr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_COPS_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +COPS command, which list available networks and sets
  automatic/manual registration.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cops_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_COPS_ABORT_CMD_HANDLER

DESCRIPTION
  This function handles an abort command to terminate asynchronous
  processing for +COPS registration operation.  The network
  registration preferences before the last +COPS comamnd handler
  execution are restored.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not command should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  Allocates and submits command buffer to DS task

===========================================================================*/
boolean dsatetsicall_cops_abort_cmd_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
);

/*===========================================================================

FUNCTION DSATETSICALL_COPS_ABORT_EVENT_HANDLER

DESCRIPTION
  This function handles teh +COPS abort event handling. Most of the
  processing is done in the abort command handler.  This routine just
  ensures the AT command processor returns to normal processing.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not command should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_cops_abort_event_handler
(
   ds_at_cmd_status_type *cmd_info_ptr
);

/*===========================================================================

FUNCTION DSATETSICALL_COPS_ASYNCH_HANDLER

DESCRIPTION
  This function manages the asnychronous processing of the +COPS
  command. Numerous acynch queries are required depnding on the +COPS
  syntax.  For test syntax, its is necessary to get available and
  poreferred networks from CM.  The home PLMN is determined from the
  GSDI IMSI query if necessary.  For read syntax, current state of
  phone is requried from CM.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_CMD_ERR_RSP: if there was a problem in lower layers
    DSAT_ASYNC_CMD : if aynchronous command continues successfully

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_cops_async_handler
(
  dsat_cops_async_state_e_type  cmd_state,    /* Processing state    */
  sys_modem_as_id_e_type        subs_id,
  ds_at_cm_ph_event_type       *ph_event      /* PH event pointer */
);

/*===========================================================================

FUNCTION DSATETSICALL_REPORT_COPS_RESULT

DESCRIPTION
  This function generates the response for network query command +COPS
  read syntax.  The order of networks shown in the respose is dictated
  in TS 27.007 section 7.3: home, SIM (preferred), other.  Only those
  networks "present" are to be reported.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure
  DSAT_OK : if it is a success for local execution
  DSAT_ERROR : if there was any problem in executing the command
  
SIDE EFFECTS
  May append further DSM items to original result buffer.
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_report_cops_result
(
  ds_at_mmgsdi_oper_name_type *oper_name, /* Operator name pointer */
  sys_modem_as_id_e_type       subs_id
);

#ifdef FEATURE_SGLTE
/*===========================================================================

FUNCTION DSATETSICALL_REPORT_QCHCOPS_RESULT

DESCRIPTION
  This function generates the response for network query command $QCHCOPS
  read syntax.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_OK : if it is a success for local execution
  DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  May append further DSM items to original result buffer.
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatetsicall_report_qchcops_result
(
  ds_at_mmgsdi_oper_name_type *oper_name /* Operator name pointer */
);
#endif /* FEATURE_SGLTE */

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CLCK_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CLCK command, which controls the facility lock
  supplementary service acording to 3GPP TS 22.088

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was an problem in executing the command
    DSAT_CMD_ERR_RSP : if there was a problem during aynchronous execution
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_clck_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CPWD_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CPWD command, which sets new passwords for facility lock
  supplementary service.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cpwd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CUSD_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CUSD command, which allows control of Unstructured
  Supplementary Service Data (USSD).

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cusd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSICALL_CUSD_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CUSD read & test
  commands.  The &V command respose is handled as well.  Data is
  extracted from data structure and formatted according to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsicall_cusd_response_fmt
(
  const dsati_cmd_type   *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const  *out_buff_ptr,   /*  Place to put response       */
  multi_index_fmt_e_type  format          /*  Flag for format type        */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CCWA_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CCWA command, which controls the call waiting
  supplementary service according to 3GPP TS 22.083

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_ccwa_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CHLD_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CHLD command, which controls the call hold
  supplementary service according to 3GPP TS 27.007 and 22.083

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_chld_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_SUPS_CMD_CB_HANDLER

DESCRIPTION
  This function is the handler function for the CM SupS call commands

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR :      if there was any problem in execution.
    DSAT_ASYNC_CMD :  if it is a success and asynch cmd continues.
    DSAT_OK :         if it is a success and asynch cmd done. 

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_sups_cmd_cb_handler
(
  sys_modem_as_id_e_type  subs_id,
  ds_at_cm_call_cmd_type *call_cmd /* Call command pointer */
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_MNG_CALLS_CONF_EX

DESCRIPTION
  This function is call by dsatcmif_cm_call_event_handler when CM report 
  the CM_CALL_EVENT_MNG_CALLS_CONF event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_NO_CARRIER : if the current state is not one of IDLE_STATE, 
                    CONNECTED_OTHER_CLIENT, WAITING_FOR_ANSWER.
  DSAT_ASYNC_EVENT: otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_mng_calls_conf_ex
(
  ds_at_cm_call_event_type *call_event,  /* Call event pointer */
  sys_modem_as_id_e_type    subs_id      /* Subsription ID */
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_MNG_CALLS_CONF

DESCRIPTION
  This function is call by dsatcmif_cm_call_event_handler when CM report 
  the CM_CALL_EVENT_MNG_CALLS_CONF event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_NO_CARRIER : if the current state is not one of IDLE_STATE, 
                    CONNECTED_OTHER_CLIENT, WAITING_FOR_ANSWER.
  DSAT_ASYNC_EVENT: otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_mng_calls_conf
(
  ds_at_cm_call_event_type *call_event, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id     /* Subsription ID */
);

/*===========================================================================

FUNCTION ETSICALL_SEARCH_CALL_LIST

DESCRIPTION
  This function searches for the call id in the call list provided, 
  deleting it if remove is TRUE and returning TRUE if call id was found. 
  It is used by event handlers that maintain call state info.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of validation.
  possible values:
    TRUE :  if call id was found
    FALSE : not found.

SIDE EFFECTS
  Call id is removed from list per boolean remove 
  
===========================================================================*/
boolean etsicall_search_call_list
(
  uint8 *call_qty,
  cm_call_id_type *call_list,
  const cm_call_id_type call_id,
  boolean rmv_call_id
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_CONNECT_EX

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM reports 
  the CM_CALL_EVENT_CONNECT event and adds the call id connected to the active
  calls list of our state info.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure.
  DSAT_ASYNC_EVENT:  otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_connect_ex
(
  ds_at_cm_call_event_type*  call_event,     /* call event pointer         */
  sys_modem_as_id_e_type     subs_id              /* Subsription ID */
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_CONNECT

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM reports 
  the CM_CALL_EVENT_CONNECT event and adds the call id connected to the active
  calls list of our state info.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure.
  DSAT_ASYNC_EVENT:  otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_connect
(
  ds_at_cm_call_event_type*  call_event,     /* call event pointer         */
  sys_modem_as_id_e_type     subs_id,        /* Subsription ID */
  dsat_result_enum_type      result
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_END_EX

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM reports 
  the CM_CALL_EVENT_MNG_CALLS_CONF event and removes the call id ended from 
  our call state info.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure.
  DSAT_ASYNC_EVENT:  otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_end_ex
(
  ds_at_cm_call_event_type *call_event, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id     /* Subsription ID */
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_END

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM reports 
  the CM_CALL_EVENT_MNG_CALLS_CONF event and removes the call id ended from 
  our call state info.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure.
  DSAT_ASYNC_EVENT:  otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_end
(
  ds_at_cm_call_event_type *call_event, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id     /* Subsription ID */
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_INCOM

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM reports 
  the CM_CALL_EVENT_INCOM event and adds the waiting call id to our call state
  info.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : if CM reports failure.
  DSAT_ASYNC_EVENT:  otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_call_event_incom
(
  ds_at_cm_call_event_type*   call_event,    /* Call Event pointer         */
  sys_modem_as_id_e_type      subs_id        /* Subsription ID */
);

#ifdef FEATURE_DUAL_ACTIVE
/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_LOCAL_HOLD

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM
  reports the CM_CALL_EVENT_LOCAL_HOLD event and moves the calls
  between active call list and local held call list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_call_event_local_hold
(
  ds_at_cm_call_event_type* call_event,    /* DS Command pointer */
  sys_modem_as_id_e_type    subs_id        /* Subsription ID */
);
#endif /* FEATURE_DUAL_ACTIVE */

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CBST_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CBST command.It is used to select bearser services to be 
  used when data calls are originated. 

DEPENDENCIES
  None
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsicall_exec_cbst_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CRLP_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CRLP command, which processes parameters for
  non-transparnet/asynchronous circuit-switched data calls.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_crlp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_CRLP_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CRLP read & test
  commands.  The &V command respose is handled as well.  Data is
  extracted from data structure and formatted according to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsicall_crlp_response_fmt
(
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type  format         /*  Flag for format type        */
);


/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CV120_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +CV120 command. It is used to set the V.120
  rate adaption paramaters for the CS Data stack.

DEPENDENCIES
  The values exported may or may not be appropriate for the mode-specific
  handler.  Validation in mode-specific handler is assumed. 
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cv120_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION  DSATETSICALL_INIT_RLP_EXPORT

DESCRIPTION
  This function intializes the CS Data RLP control variables.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatetsicall_init_rlp_export ( void );


/*===========================================================================

FUNCTION DSATETSICALL_UPDATE_CRLP_PARAM_SET

DESCRIPTION
  This function performs parameter set validation for +CRLP.  If
  validation successful, the dsat_crlp_val array is updated for the
  specified RLP version.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of validation.
  possible values:
    DSAT_ERROR : if there was any problem
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_update_crlp_param_set
(
  const ds_ucsd_rlp_type  *rlp_params_ptr    /* +CRLP parameter values */
);


/*===========================================================================

FUNCTION DSATETSICALL_UPDATE_DS_PARAM_SET

DESCRIPTION
  This function performs parameter set validation for +DS.  If
  validation successful, the dsat_ds_val array is updated.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of validation.
  possible values:
    DSAT_ERROR : if there was any problem
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_update_ds_param_set
(
  const ds_ucsd_v42bis_type  *ds_params_ptr    /* +DS parameter values */
);

#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CAOC_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +CAOC command, which enables/disables
  unsolitited result codes for advice of charge supplementary service.
  In addition, the current value of accumulated call meter may be
  queried from the SIM/USIM.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  Unsolicited result codes may be generated at a future time if enabled.
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_caoc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION  DSATETSICALL_CAOC_TIMER_EXPIRED_HANDLER 

DESCRIPTION
  This function is the handler function for the +CAOC command
  reporting timer expired event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_caoc_timer_expired_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CACM_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +CACM command, which reports the
  accumulated call meter from the SIM/USIM.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cacm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CAMM_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +CAMM command, which reports or resets the 
  accumulated call meter maximum from the SIM/USIM.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_camm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION  DSATETSICALL_AOC_VERIFY_PIN_HANDLER

DESCRIPTION
  This function is the handler function for the +CACM/+CAMM commands
  PIN2 verification.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK      - if subcommand executed successfully
  DSAT_ERROR   - if error encountered

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_aoc_verify_pin_handler
(
  const ds_at_cmd_status_type *cmd_ptr
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CPUC_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +CPUC command, which reports or sets the 
  Price per Unit and Currency table from the SIM/USIM.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cpuc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSICALL_CPUC_MMGSDI_WRITE

DESCRIPTION
  This function write the Price per Unit and Currency table
  to the SIM/USIM.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
  DSAT_ASYNC_CMD :   if it is a success.

SIDE EFFECTS
  None.

===========================================================================*/
dsat_result_enum_type  dsatetsicall_cpuc_mmgsdi_write
(
  ds_at_cmd_status_type *cmd_ptr
);

/*===========================================================================

FUNCTION DSATETSICALL_REPORT_CPUC

DESCRIPTION
  This function reports the Price per Unit and Currency table
  stored in the SIM/USIM.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
  DSAT_OK :          if it is a success.

SIDE EFFECTS
  None.

===========================================================================*/
dsat_result_enum_type dsatetsicall_report_cpuc
(
  ds_at_mmgsdi_sim_info_type *sim_info_ptr /* MMGSDI SIM info pointer */
);

/*===========================================================================
FUNCTION DSATETSICALL_INIT_CALL_PARAM_FROM_NV

DESCRIPTION
  This function reads call setup defules from NV.  If paramaters are
  unintialized, intial values are written basd on current build
  configuration.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  NV parameters are updated if not previously initialized.
  
===========================================================================*/
void dsatetsicall_init_call_param_from_nv ( void );


#ifdef FEATURE_WCDMA
/*===========================================================================

FUNCTION DSATETSICALL_EXEC_ES_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +ES command, which set parameters for entering or exiting
  synchronous access mode.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_es_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_ESA_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +ESA command, which set parameters for synchronous access 
  mode configuration.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_esa_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_ES_EXPORT

DESCRIPTION
  This function export the +ES's value to other subsystem.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_es_export
(
  void
);

/*===========================================================================

FUNCTION DSATETSICALL_ESA_EXPORT

DESCRIPTION
  This function export the +ESA's value to other subsystem.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_esa_export
(
  void
);

#endif /* FEATURE_WCDMA */


/*===========================================================================

FUNCTION  DSATETSICALL_REPORT_SS_LINEID_STATUS

DESCRIPTION
  This function reports the COLP/CLIP supplementary service status
  received from Call Manager.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :    if there was any problem in execution.
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_report_ss_lineid_status
(
  ss_operation_code_T       * op_code,    /* Interrogation operation code */
  const ie_ss_status_T      * ss_info_ptr, /* SS info pointer */
  boolean                     in_service
);

/*===========================================================================

FUNCTION ETSICALL_REPORT_AOC_CCM

DESCRIPTION
  This function displays the current call meter reported by Call Manager

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void etsicall_report_aoc_ccm
(
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_COLP_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +COLP command, which enables/disables the display of 
  connected line identification of the remote party

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_colp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CLIP_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CLIP command, which enables/disables the display of 
  calling line identification of the remote party

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_clip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION  DSATETSICALL_REPORT_CLIP_RESULT

DESCRIPTION
  This function accumulates all the CLIP data and calls the PBM to retrieve 
  name.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsicall_report_clip_result
(
  ds_at_cm_call_event_type *call_event_ptr, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id
);


/*==========================================================================
FUNCTION  DSATETSICALL_REPORT_CLIP_RESULT_RING

DESCRIPTION
  This function report Caller ID to the TE if CLIP reporting is enabled.

DEPENDENCIES
  None

RETURN VALUE
  void.

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsicall_report_clip_result_ring
(
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CDIP_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CDIP command, which enables/disables the display of 
  called line identification.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.
    DSAT_ASYNC_CMD : For interrogating with the network and getting the response
                     from the network.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cdip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION  DSATETSICALL_REPORT_CDIP_RESULT

DESCRIPTION
  This function accumulates all the CDIP data and calls the PBM to retrieve 
  name.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsicall_report_cdip_result
(
  ds_at_cm_call_event_type *call_event_ptr, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id
);


/*==========================================================================
FUNCTION  DSATETSICALL_REPORT_CDIP_RESULT_RING

DESCRIPTION
  This function report Caller ID to the TE if CDIP reporting is enabled.

DEPENDENCIES
  None

RETURN VALUE
  void.

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsicall_report_cdip_result_ring
(
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================

FUNCTION  DSATETSICALL_REPORT_COLP_RESULT

DESCRIPTION
  This function report connected number to the TE if CLIP reporting is enabled.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsicall_report_colp_result
(
  ds_at_cm_call_event_type *call_event_ptr, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================

FUNCTION  DSATETSICALL_EXEC_SS_ABORT_HANDLER

DESCRIPTION
  This function is the abort handler function for the SS functionality. 
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE - meaning to abort the delay execution.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatetsicall_exec_ss_abort_handler
(
  const struct dsati_cmd_struct *ptr
);


/*===========================================================================

FUNCTION DSATETSICALL_SS_ABORT_EVENT_HANDLER

DESCRIPTION
  This function handles +CLIP/+COLP/+CLIR abort event handling.We set the SS 
  service to invalid state and return. This routine just ensures the AT 
  command processor returns to normal processing.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR : as the function is aborted

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_ss_abort_event_handler
(
  ds_at_cmd_status_type *cmd_info_ptr
);

#ifndef FEATURE_DSAT_CUST
/*===========================================================================

FUNCTION DSATETSICALL_EXEC_COPN_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +COPN command, which displays list of 
  operator names from the ME. This command is implemented as 
  asynchronous command to make it abortable

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_copn_cmd
(
  dsat_mode_enum_type       mode,         /*  AT command mode:            */
  const dsati_cmd_type     *parse_table,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type            *res_buff_ptr  /*  Place to put response       */
);
#endif /* FEATURE_DSAT_CUST */

/*===========================================================================

FUNCTION DSATETSICALL_DISPLAY_PLMN_LIST

DESCRIPTION
  This function displays list of operator names from the ME.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_display_plmn_list
(
  void
);

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION  DSATETSICALL_DISPLAY_PREF_NETWORKS

DESCRIPTION
  This function displays valid preffered PLMNs in the given index range.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_display_pref_networks
(
  ds_at_mmgsdi_oper_name_type *oper_name_ptr /* Operator name pointer */
);
#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION DSATETSICALL_PLMN_ABORT_CMD_HANDLER

DESCRIPTION
  This function handles an abort command to terminate listing of both 
  preffered PLMN and PLMN list stored on the ME.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not command should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

===========================================================================*/
boolean dsatetsicall_plmn_abort_cmd_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
);

/*===========================================================================

FUNCTION DSATETSICALL_COPN_ABORT_EVENT_HANDLER

DESCRIPTION
  This function handles teh +COPN abort event handling. Most of the
  processing is done in the abort command handler.  This routine just
  ensures the AT command processor returns to normal processing.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR : As command is aborted

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_copn_abort_event_handler
(
  ds_at_cmd_status_type *cmd_info_ptr
);


/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CLIR_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +COLP command, which enables/disables the restriction of the 
  display of calling line identification.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_clir_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================
FUNCTION DSATETSICALL_EXEC_CSSN_CMD
DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CSSN command, supplementary service according to 
  3GPP TS 27.007 and 22.083
DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR :     if AT command syntax error
    DSAT_ASYNC_CMD : if CM call successful
    DSAT_CMD_ERR_RSP : if CM couldn't alloc cmd buffer
SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cssn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_CSSI_REPORT

DESCRIPTION
  This function passes the intermediate result codes to the TE

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if AT command syntax error
    DSAT_OK    : if successful

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_call_event_cssi_report
(
  ds_at_cm_call_event_type *call_event, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id,    /* Subsription ID */
  dsat_cssi_e_type          cssi_res
);

/*===========================================================================

FUNCTION DSATETSICALL_CALL_EVENT_CSSU_REPORT

DESCRIPTION
  This function passes the unsolicited result codes to the TE

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if AT command syntax error
    DSAT_OK    : if successful

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsicall_call_event_cssu_report
(
  ds_at_cm_call_event_type *call_event, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id,    /* Subsription ID */
  dsat_cssu_e_type          cssu_res
);

/*===========================================================================

FUNCTION DSATETSICALL_SEQNUM_TO_CALLID

DESCRIPTION
  Given a call sequence number as described in 3GPP TS 22.030 section 6.5.5.1
  returns the associated call ID.

DEPENDENCIES
  None
  
RETURN VALUE
  Call ID on success or
  CALL_CALL_ID_INVALID on out-of-range sequence number or failure to find.

SIDE EFFECTS
  None
  
===========================================================================*/
cm_call_id_type dsatetsicall_seqnum_to_callid
(
  sys_modem_as_id_e_type     subs_id,
  dsat_num_item_type         sequence_number
);

/*========================================================================

FUNCTION  DSATETSICALL_REPORT_CLIR_STATUS

DESCRIPTION
  This function reports the CLIR supplementary service interrogation status
  received from Call Manager.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :    if there was any problem in execution.
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_report_clir_status
(
  ds_at_cm_sups_event_type *sups_event_ptr, /* SUPS event pointer */
  sys_modem_as_id_e_type    subs_id
);

#ifdef FEATURE_DATA_ETSI_PIN
/*===========================================================================

FUNCTION  DSATETSICALL_EXEC_VTS_CMD

DESCRIPTION
  This function generates a dtmf during an ongoing call.

DEPENDENCIES
  A active call

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :    if there was any problem in execution.
    DSAT_ASYNC_CMD : Intermediate Response
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/

dsat_result_enum_type dsatetsicall_exec_vts_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION  DSATETSICALL_START_DTMF

DESCRIPTION
  This function process the 'write' command of +vts.

DEPENDENCIES
  A active call

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :    if there was any problem in execution.
    DSAT_ASYNC_CMD : Intermediate Response
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_start_dtmf 
(
  const tokens_struct_type *tok_ptr 
);


/*===========================================================================

FUNCTION  dsatetsicall_inband_dtmf_timer_expired_handler

DESCRIPTION
  Handler Function for DTMF Timer.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :    if there was any problem in execution.
    DSAT_ASYNC_EVENT : Intermediate Response
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_inband_dtmf_timer_expired_handler 
(
  ds_cmd_type   * cmd_ptr         /* DS Command pointer		   */

);

#endif /* FEATURE_DATA_ETSI_PIN */

#ifdef FEATURE_ECALL_APP 
/*===========================================================================

FUNCTION DSATETSICALL_CECALL_CB_FUNC

DESCRIPTION
  CECALL query command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

  
===========================================================================*/
 void dsatetsicall_cecall_cb_func
(
  ecall_type_of_call     type_of_call,
  ecall_activation_type  activation_type
);

/*===========================================================================

FUNCTION DSATETSICALL_EXEC_CECALL_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CECALL command, to trigger an eCall to the network. Based 
  on the configuration selected, it can be used to either trigger a test call,
  a reconfiguration call, a manually initiated eCall or an automatically initiated eCall"


DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cecall_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
); 
#endif /*FEATURE_ECALL_APP*/

#ifdef FEATURE_DSAT_LTE
/*===========================================================================
FUNCTION: DSATETSICALL_EXEC_CVMOD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at+cvmod command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cvmod_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);
#endif /* FEATURE_DSAT_LTE*/

/*===========================================================================
FUNCTION: DSATETSICALL_EXEC_CEN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at+cstf command.
  at+cen is used to enable/disable new emergency numbers received from the 
  network with unsolicited result codes .

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_exec_cen_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

#ifdef FEATURE_MMGSDI

/*===========================================================================
FUNCTION: DSATETSICALL_COPS_MMGSDI_CMD_HANDLER

DESCRIPTION
  This function handles at+cops command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_cops_mmgsdi_handler
(
  ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION: DSATETSICALL_COPS_CMD_CB_FUNC

DESCRIPTION
  This function handles at+cops command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsicall_cops_cmd_cb_func
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

#endif /*FEATURE_MMGSDI*/

/*===========================================================================
FUNCTION: DSATETSICALL_PROCESS_PLMN_INFO

DESCRIPTION
  This function handles at+cops command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicall_process_plmn_info ( void );

/*===========================================================================

FUNCTION  DSATETSICALL_UPDATE_SUBS_STATUS

DESCRIPTION
  This function updates the subscription status of the subs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatetsicall_update_subs_status
(
  cm_subscription_status_e_type *status_one,
  cm_subscription_status_e_type *status_two,
  cm_subscription_status_e_type *status_three,
  sys_modem_as_id_e_type         subs_id
);

#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* DSATETSICALL_H */
