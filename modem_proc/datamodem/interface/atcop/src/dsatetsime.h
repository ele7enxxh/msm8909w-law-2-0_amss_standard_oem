#ifndef DSATETSIME_H
#define DSATETSIME_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( M o b i l e  E q u i p m e n t  C O M M A N D S )
                
                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


GENERAL DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  executing the mobile equipment commands.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsime.h_v   1.2   12 Jul 2002 10:08:26   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsime.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk/pg   Added support for Dynamic ATCoP.
07/02/13   pg      Migrated to MMGSDI API to find PLMN info in SE13 table
03/28/13   tk      Mainlined FEATURE_THIN_UI using new NV-EFS item.
11/30/12   tk      ATCoP changes for Triton DSDA.
09/13/12   sk      Fixed CSQ command.
07/31/12   sk      C-EONS Feature Support.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
01/19/12   sk      Feature cleanup.
10/20/11   nc      Added support for CUAD/CEAP/CERP commands.
01/11/11   ad      Removed pending variable .
07/27/10   ad      Fixed time zone parameter according new spec. 
06/04/10   ua      Removing support of CKPD and HS services for all 
                   THIN UI targets.
06/04/10   bs      MMGSDI SESSION Migration.
05/10/10   kk      Adding support for +ICCID.
02/08/10   ua      Removing support for CLVL and CMUT. 
01/20/10   kk      Moved +CFUN to CommonATCoP.
12/15/09   nc      Featurisation changes for LTE.
09/09/09   ua      Added support for EONS (spec 22.101).
04/29/09   ua      Fixed compiler warnings. 
12/12/08   ua      Fixed Off target lint errors.
12/02/08   ss      Off target Lint fix
10/23/08   bs      Added support for 1X AT Phonebook commands.
07/17/08   sa      Removed the support of +CKPD for thin-UI 8K builds.
06/18/08   ua      Moving cb_cmd_e_type enum to dsatme.h file.
01/22/08   sa      Fixed +CLCK and +CPIN issues related with SIM PIN2.
01/09/08   bs      Added new command $QCSIMSTAT for sim status.
10/25/07   sa      Removed multiple calls for mmgsdi_get_all_pin_status during 
                   initialization.
08/06/07   pp      Moved +CLVL, +CMUT commands to mainline.
07/05/07   ss      Full UCS2 character support for phone book commands.
06/21/07   ua      Enabling +CKPD command for single processor THIN UI builds.
05/17/07   ss      Moved to MMGSDI APIs for pin status opertaions 
                   Added prototype for dsatetsime_mmgsdi_req_cb
04/14/07   pkp     Added support for +CCLK (Clock).
02/15/07   ua      Added new phone book "FD" support in CLCK.
                   Phone book commands (CPBS,CPBR,CPBW,CPBF) now doesnt not 
                   rely on the staus of the ADN phone book at initialization. 
02/12/07   ua      Removing support for +CKPD in THIN_UI builds
10/09/06   ua      Moving function dsatetsime_exec_cnum_cmd() 
                   out of feature FEATURE_DATA_ETSI_PIN 
07/29/06   ua      Modifications supporting modified MMGSDI API for mmgsdi_callback_type
06/08/06   ua      Eliminating usage of CM internal function cmph_info_get.
06/08/06   sa      Add support for +CLAC and modified +CTZU command
05/12/06   ua      CPLS, CPOL commands wrapped under FEATURE_MMGSDI
05/08/06   ua      Eliminating the use of gsdi interface for CPOL command. 
                   dsatetsime_get_num_pplmn_rec is removed. 
03/13/06   ua      CPOL command updated to use +CPLS command for sim file selection.
02/27/06   snb     Add mnc_includes_pcs_digit field to 
                   dsat_me_cpol_write_param_s_type.
02/21/06   sa      Merged changes from Sirius Branch
                   -Added new limit macros for +CCLK processing.
12/17/05   snb     Correct call history entry.
02/11/06   ua      Removed feautre FEATURE_DSAT_CFUN_CARD_POWER_CTL for mmgsdi interfaces
                   to implement CPLS, CPOL using mmgsdi interfaces.
01/09/06   ua      Add support for +CPLS command
09/23/05   snb     Corrections for +CSIM/+CRSM USIM support.
09/13/05   snb     Use LPM for +CFUN=4 mode.
08/29/05   snb     Add SIM card power control to +CFUN.
08/15/05   snb     Add support for reset via +CFUN command.
08/03/05   hap     Modified dsatetsime_ld_save to avoid making duplicate 
                   entries in call history
05/19/05   iv      Removed 'dsatetsime_crmp_handle_get_profile' fucntion.
04/19/05   tkk     Added prototypes for retrieving NV items.
04/19/05   hap     Changed design for +CNUM command &
                   corrections for ^SDBR
04/05/05   ar      Add support for +CIND command & generalize signal report.
04/05/05   tkk     Add prototypes for +CRSL, +CALM & +CTZU prototypes.
04/04/05   snb     Add cb_cmd_e_type for +CSIM / +CRSM.
03/11/05   snb     Add support for +CRSM command.
03/08/05   iv      Added suport for +CRMP and ^SSET command.
03/07/05   ar      Extend +CFUN capability for testing purposes.
02/22/05   iv      Added prototype for dsatetsime_cpbr_parse_arguments.
02/21/05   dvp     Added support for Personalization facilities in +CLCK
02/15/05   ar      Updates to supprot keypad emulation.
02/14/05   tkk     Added support for +CVIB command.
02/14/05   hap     Added support for ^SDBR command.
02/14/05   iv      Moved me_pb_entry_type from dsatetsime.c to dsatetsime.h
02/02/05   tkk     Added support for +CLIR command.
02/03/05   iv      Added support for +CPOL command.
01/31/05   iv      Added support for +COPN command.
01/31/05   iv      Added support for +CLCC command.
01/27/05   pdv     Merge from Sirius-ATCoP development.
01/27/05   pdv     Rebase from Mainline.
01/20/05   tkk     Added SND call back functions & CLIP_CM_CMD_ABORT in 
                   cb_cmd_e_type
01/05/05   hap     Added support for +CNUM command, 
                   added msisdn_fixed_sim_rec_s_type and msisdn_record_s_type
12/30/04   tkk     Added support for getting alphanumeric name.
12/21/04   tkk     Added support for +CCLK command.
12/07/04   ar      Add support for +COPS command.
11/19/04   tkk     Added support for +CLVL and +CMUT commands.
11/09/04   ar      Replace req_gsdi_sim_read with dsatetsime_sim_read.
                   Add support for +CPUC command.  
10/17/04   dvp     Changes to support ^SCKS, ^SCID and ^SCKA
10/15/04   ar      Add Advice of Charge support.
09/03/04   snb     Add +CSIM support.
07/06/04   snb     Add support for personalization PINs for ATCOP.
01/06/04   snb     Move registration for pin event with GSDI and handling 
                   callback for pin events to common file and add protocol
                   stack re-initialization under feature define.
10/29/03   snb     Added support for $QCPINSTAT command and for async IMSI 
                   read from either card slot by +CIMI command.
06/09/03   ar      Added dsatetsime_exec_cmer_cmd() prototype.
04/03/03   ar      Added support for +CKPD command
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
02/26/03   wx      Put dialed number into last dial ph book.
12/31/02   wx      Added direct dialing implementations
12/04/02   ar      Added prototype for dsatetsime_change_pin()
11/27/02   ar      Added prototypes for dsatetsime_get_pin_status() & 
                   dsatetsime_get_pin_status()
01/17/02   ar      Added support for +CPAS,+CBC,+CPIN,+CSQ commands
11/15/01   wx      Created module.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


/* this file should be included only in GSM or WCDMA mode */
#ifdef FEATURE_DSAT_ETSI_MODE

#include "dsati.h"
#include "dstask.h"
#include "sys.h"
#include "dsatclient.h"
#include "dsatme.h"
#include "dsatctab.h"

/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/
/* Limit for number of ME unsolicited notificaitons */
#define DSAT_MAX_ME_USNOTIFY_COUNT 40

typedef PACKED struct PACKED_POST {
  byte bcd_len;
  byte ton_npi;
  byte number[10];      /* BCD coding */
  byte cap_config_id;   /* 0xFF if not used */
  byte ext_id;          /* 0xFF if not used */
} msisdn_fixed_sim_rec_s_type;
/* Real time clock */
typedef struct 
{
  boolean            time_zone_available;  /* is time zone valid */
  sys_time_zone_type time_zone;            /* Time zone info */
} dsat_cclk_timezone_s_type;

#define MAX_CCLK_STRTOK_LENGTH 2  /* Each of the token yy or MM or dd cannot
                                     go beyond 2 characters */

typedef enum 
{
  FINDING_CCLK_YEAR = 0,
  FINDING_CCLK_MONTH = 1,
  FINDING_CCLK_DAY = 2, 
  FINDING_CCLK_HOUR = 3,
  FINDING_CCLK_MINUTE = 4
} dsat_cclk_tok_e_type;

#define DSAT_TIMEZONE_HIGH   (sys_time_zone_type)96
#define DSAT_TIMEZONE_LOW    (sys_time_zone_type)96

#define DSAT_TIMEZONE_MIN_YEAR     2000
#define DSAT_TIMEZONE_MAX_YEAR     2100
#define DSAT_TIMEZONE_MAX_MONTH      12
#define DSAT_TIMEZONE_MAX_DAY        31
#define DSAT_TIMEZONE_MAX_HOUR       23
#define DSAT_TIMEZONE_MAX_MINUTES    59
#define DSAT_TIMEZONE_MAX_SECONDS    59

#define DSAT_YEAR_BASE_1900 1900
#define DSAT_YEAR_BASE_2000 2000
#define DSAT_YEAR_BASE_2100 2100

#define DSAT_CLK_MIN_YEAR     1980
#define DSAT_CLK_MAX_YEAR     2115

/* TDS Signal level conversion result codes */
#define RSSI_TDS_TOOLO_CODE   100
#define RSSI_TDS_TOOHI_CODE   191
#define RSSI_TDS_UNKNOWN_CODE 199

/* TDS RSSI range conversion */
#define RSSI_TDS_MIN        25   
#define RSSI_TDS_MAX        116  
#define RSSI_TDS_NO_SIGNAL  125  /* from CM */
#define RSSI_TDS_OFFSET     113 
#define RSSI_TDS_SLOPE      (-100.0/191.0)

/* Signal level conversion result codes */
#define RSSI_TOOLO_CODE   0
#define RSSI_TOOHI_CODE   31
#define RSSI_UNKNOWN_CODE 99

/* RSSI range conversion */
#define RSSI_MIN        51   /* per 3GPP 27.007  (negative value) */
#define RSSI_MAX        113  /* per 3GPP 27.007  (negative value) */
#define RSSI_NO_SIGNAL  125  /* from CM */
#define RSSI_OFFSET       182.26
#define RSSI_SLOPE        (-100.0/62.0)



#define DSAT_CSQ_MAX_SIGNAL       RSSI_TOOHI_CODE  /* = Maximum signal      */

typedef uint8 dsat_ind_val_type;

#define DSAT_CIND_BINARY_SET      1
#define DSAT_CIND_BINARY_UNSET    0

#define DSAT_CIND_INDEX_BATTCHG 0
#define DSAT_CIND_INDEX_SIGNAL    1
#define DSAT_CIND_INDEX_SERVICE   2
#define DSAT_CIND_INDEX_CALL      3
#define DSAT_CIND_INDEX_ROAM      4
#define DSAT_CIND_INDEX_SMSFULL   5
#define DSAT_CIND_INDEX_PACKET    6
#define DSAT_CIND_INDEX_CALLSETUP 7

#define DSAT_CIND_MAX_BATTCHG   5  /* = 100% battery charge */
#define DSAT_CIND_MAX_SIGNAL      5  /* = Maximum signal      */

#define DSAT_CIND_CSETUP_NONE     0  /* No active call setup  */
#define DSAT_CIND_CSETUP_MTRING   1  /* MT call is waiting of ringing */
#define DSAT_CIND_CSETUP_MOINIT   2  /* MO call was initiated  */
#define DSAT_CIND_CSETUP_MORING   3  /* MO call ringing at B-party  */

/* Different vibrator values */
#define DSAT_CVIB_INVALID     0
#define DSAT_CVIB_OFF         1 
#define DSAT_CVIB_ON          2 
#define DSAT_CRSL_SETTING_INVALID 0xFF
#define DSAT_CALM_SETTING_INVALID 0xFF
#define DSAT_CVIB_SETTING_INVALID 0xFF

#ifdef FEATURE_DSAT_UMTS_UNDP
/* Variable that stores number of retires of SIM PIN or
 * SIM PIN2. */
typedef struct {
  boolean valid_flag;          /* whether the data is valid */
  uint8  pin_retries; /* number of retries remaining */
} dsat_pin_retry_status_type;

extern dsat_pin_retry_status_type dsat_pin_retry_status;
#endif /* FEATURE_DSAT_UMTS_UNDP */

#ifdef FEATURE_DATA_PS_EAP 
extern rex_timer_type dsatetsime_ceap_report_timer;
extern rex_timer_type dsatetsime_ceap_eap_process_timer;
#endif /* FEATURE_DATA_PS_EAP */
/*-------------------------------------------------------------------------
            PUBLIC FUNCTION DECLARATIONS
-------------------------------------------------------------------------*/

#endif /* FEATURE_DSAT_ETSI_MODE */

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CPAS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPAS command.
  The result is in res_buff_ptr, reporting phone activity status

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
dsat_result_enum_type dsatetsime_exec_cpas_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================

FUNCTION  DSATETSIME_REPORT_CPAS

DESCRIPTION
  This function generates the query response for the +CPAS read command.
  Data is extracted from the data returned from Call Manager and formatted.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_report_cpas
(
  const dsat_mode_enum_type  mode,          /*  AT command mode:            */
  ds_at_cm_call_event_type  *call_event_ptr, /* Call event pointer */
  dsm_item_type             *res_buff_ptr,  /*  Place to put response       */
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CSQ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CSQ command.
  The result is in res_buff_ptr, reporting received signal strength 
  indication and channel bit error rate.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.
    DSAT_ASYNC_CMD: if this is function invokes an async command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_csq_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================

FUNCTION  DSATETSIME_CONVERT_RSSI

DESCRIPTION
  This function converts the RSSI values from Call Manager into a
  value used by AT commands.  The RSSI value and scale factor are
  input parameters.  The output value will represent the signal level
  as a fraction of the scale value.

  Both +CSQ and +CIND utilize this function.

DEPENDENCIES
  None

RETURN VALUE
  Signal level as integer, scaled based on parameter.

SIDE EFFECTS
  None

===========================================================================*/
uint8 dsatetsime_convert_rssi
(
  uint16    rssi,    /* Call Manager RSSI as positive integer */
  uint8     scale    /* ATCOP scaling factor */
);


/*===========================================================================

FUNCTION DSATETSIME_EXEC_CBC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CBC command.
  The result is in res_buff_ptr, reporting battery condition status
  and charge level.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.
    DSAT_ASYNC_CMD: if this is function invokes an async command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_cbc_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

#ifdef FEATURE_DATA_ETSI_PIN
/*===========================================================================

FUNCTION  DSAT_PIN_MMGSDI_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the GSDI events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat_pin_mmgsdi_event_handler_ex
(
  ds_at_mmgsdi_event_type *mmgsdi_event_info /* MMGSDI event info pointer */
);

/*===========================================================================

FUNCTION  DSAT_PIN_MMGSDI_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the GSDI events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat_pin_mmgsdi_event_handler
(
  ds_at_mmgsdi_event_type *mmgsdi_event_info /* MMGSDI event info pointer */
);
/*===========================================================================

FUNCTION DSATETSIME_EXEC_CPIN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPIN command.
  This set commands sends to the ME a password which is necessary before 
  operation.

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
dsat_result_enum_type dsatetsime_exec_cpin_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIME_EXEC_QCPINSTAT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCPINSTAT command.
  This command sends to the ME the status of all PINs for all cards.

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
dsat_result_enum_type dsatetsime_exec_qcpinstat_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION DSATETSIME_VERIFY_PIN

DESCRIPTION
  This function provides an interface to verify a specific SIM PIN.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_verify_pin
(
  mmgsdi_pin_enum_type pin_id,          /* PIN identifier */
  byte *         pin,                   /* PIN password */
  cb_cmd_e_type  req_submitter          /* Callback return identifier */
);

/*===========================================================================

FUNCTION DSATETSIME_GET_PIN_STATUS

DESCRIPTION
  This function provides an interface to query SIM PIN.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_get_pin_status
(
  mmgsdi_pin_enum_type pin_id                /* PIN identifier */
);

/*===========================================================================

FUNCTION DSATETSIME_SET_PIN_STATUS

DESCRIPTION
  This function provides an interface to enable/disable a specific SIM PIN.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_set_pin_status
(
  mmgsdi_pin_enum_type pin_id,          /* PIN identifier */
  boolean              pin_enabled,           /* Enabled flag   */
  byte *               pin,                    /* PIN password */
  dsat_apps_id_e_type  apps_id
);


/*===========================================================================

FUNCTION DSATETSIME_CHANGE_PIN

DESCRIPTION
  This function provides an interface to change a specific SIM PIN.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_change_pin
(
  mmgsdi_pin_enum_type pin_id,          /* PIN identifier */
  dsat_string_item_type * pin,          /* PIN password */
  dsat_string_item_type * new_pin       /* PIN new password */
);

/*===========================================================================

FUNCTION DSATETSIME_MMGSDI_FILE_ATTR_RSP_CB

DESCRIPTION
  This function is the callback function for the GSDI to return
  pin status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsime_mmgsdi_file_attr_rsp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
);
#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION DSATETSIME_SIM_READ

DESCRIPTION
  This function provides an interface to read SIM EF's.  The input
  parameter dictates the caller which maps to a specific EF.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.
    DSAT_ERROR :  an error occurred.
                      
SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_sim_read
(
  cb_cmd_e_type  req_submitter          /* Callback return identifier */
);


/*===========================================================================

FUNCTION DSATETSIME_SIM_WRITE

DESCRIPTION
  This function provides an interface to write SIM EF's.  The input
  parameter req_submitter dictates the caller which maps to a
  specific EF.  The buffer and buffer lenght are also specified.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.
    DSAT_ERROR :  an error occurred.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_sim_write
(
  cb_cmd_e_type  req_submitter,         /* Callback return identifier */
  byte * buffer,                        /* EF buffer */
  uint8  buflen                         /* Buffer length */
);

#endif /* FEATURE_DATA_ETSI_PIN */

#ifdef FEATURE_MMGSDI
#if defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)

/*===========================================================================
FUNCTION DSAT_GET_PROVISIONED_VAL

DESCRIPTION
  This function will return state of apps provisioning.

DEPENDENCIES
  None

RETURN VALUE
value of dsat_app_provisioned

SIDE EFFECTS
  NONE

===========================================================================*/

dsat_mmgsdi_ready_state dsat_get_provision_state(void);

/*===========================================================================
FUNCTION DSAT_SET_PROVISIONED_VAL

DESCRIPTION
  This function will set state of apps provisioning.

DEPENDENCIES
  None

RETURN VALUE
value of dsat_app_provisioned

SIDE EFFECTS
  NONE

===========================================================================*/


void dsat_set_provision_state(dsat_mmgsdi_ready_state provisioned_val);

#endif /*defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)*/
/*===========================================================================
FUNCTION ETSIME_MAP_CMD_TO_FILE

DESCRIPTION
  This function maps a command call back used as a client refernce to the 
  exact MMGSDI file name.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/
void etsime_map_cmd_to_file 
(
  mmgsdi_file_enum_type *filename,   /* Output file name to be filled in. */
  cb_cmd_e_type          cb_cmd,       /* Input command reference. */
  dsat_apps_id_e_type    apps_id
);
#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CNUM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CNUM command.
  This command returns the MSISDN from PBM for the SIM card in 
  the operational slot.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP : if there was any problem in executing the command
    DSAT_ERROR : if the command is not supported
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cnum_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CIND_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CIND command.
  This command reports the status of ME indicators.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK :         if it is a success.
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cind_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIME_GENERATE_CIND_RESPONSE

DESCRIPTION
  This function generates the response for the +CIND command on return from
  asynchronous processing.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK :         if it is a success.
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_generate_cind_response
(
  ds_at_cm_call_event_type *call_event_ptr, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================

FUNCTION DSATETSIME_CHANGE_INDICATOR_STATE

DESCRIPTION
  This function updates the specified indicator to the specified
  state.  It provides a wrapper for indicator state tracking.  The
  state of the indicators is reported by +CIND.  Unsolicited response
  code generation is not suported but could be added to this function
  in future release.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :  if there was any problem in executing the command
    DSAT_OK :     if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_change_indicator_state
(
  uint8                    indicator,
  dsat_ind_val_type        value,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================

FUNCTION DSATETSIME_REPORT_CIEV_RESULT

DESCRIPTION
  This function reports +CIEV URC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsime_report_ciev_result
(
  uint8                   indicator,
  dsat_ind_val_type       value,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================

FUNCTION DSATETSIME_GET_INDICATOR_STATE

DESCRIPTION
  This function returns the current value of the specified indicator.
  This is the value displayed for +CIND read syntax.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns indicator value as integer type.

SIDE EFFECTS
  None

===========================================================================*/
dsat_ind_val_type  dsatetsime_get_indicator_state
(
  uint8                  indicator,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CMEC_CMD

DESCRIPTION

  This function takes the result from the command line parser and
  executes it. It executes the +CMEC command.  This command selects
  the equipment which operates the keypad, writes to the display, and
  sets indicators.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK :         if it is a success.
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cmec_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIME_EXEC_CMER_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CMER command.
  This command control sending of unsolicited result codes from TA to TE
  in the case of key pressings.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK :         if it is a success.
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cmer_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);



/*===========================================================================

FUNCTION  DSATETSIME_INIT_ME

DESCRIPTION
  ME initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to CM.
  - Registers the CM client call back functions.
  - Initializes the unsolicited response event queue.
  - Sets initial phone functionality level.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsime_init_me ( void );

#ifdef FEATURE_DSAT_ETSI_MODE
/*===========================================================================

FUNCTION DSATETSIME_EXEC_CIMI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CIMI command.
  This command returns the international mobile subscriber identy (IMSI)
  from the SIM card in sthe slot specified by $QCSLOT command.

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
dsat_result_enum_type dsatetsime_exec_cimi_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIME_REPORT_ICCID

DESCRIPTION
  Handles the asynchronous response from the ^SCID initiated SIM read of the 
  ICCID.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_ERROR :       if there was any problem in executing the command 
    DSAT_OK :          if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_report_iccid
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
);
#ifdef FEATURE_DSAT_CFUN_CARD_POWER_CTL

/*===========================================================================

FUNCTION DSATETSIME_MMGSDI_CARD_POWER_CTL_CB

DESCRIPTION
  This function is the callback function for the GSDI to return SIM card
  power control request status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsime_mmgsdi_card_power_ctl_cb
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type        *cnf_ptr
);
#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */


#ifdef FEATURE_MMGSDI_ATCSIM
#ifdef FEATURE_MMGSDI

/*===========================================================================
FUNCTION ETSIME_MMGSDI_CRSM_CB

DESCRIPTION
  This function handles the Callback for +CRSM response callback.  

DEPENDENCIES
  None


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void etsime_mmgsdi_crsm_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
);

/*===========================================================================

FUNCTION  DSATETSIME_MMGSDI_CRSM_HANDLER

DESCRIPTION
  This function is the handler function for the CRSM response handler

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
    DSAT_OK :          if it is a success.
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_mmgsdi_crsm_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
/*===========================================================================

FUNCTION  DSATETSIME_MMGSDI_SEND_APDU_HANDLER

DESCRIPTION
  This function is the handler function for the CSIM/CRSM  events.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
    DSAT_OK :          if it is a success.
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/

dsat_result_enum_type dsatetsime_mmgsdi_send_apdu_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
/*===========================================================================
FUNCTION ETSIME_MMGSDI_SEND_APDU_CB

DESCRIPTION
  This function handles the Send APDU response callback.  


DEPENDENCIES
  None


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

 void etsime_mmgsdi_send_apdu_cb
(
  mmgsdi_return_enum_type    status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
);
#endif /*  FEATURE_MMGSDI */

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CSIM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CSIM command.
  This command allows generic SIM access.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_OK          : if it is a success.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_csim_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CRSM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CRSM command.
  This command allows restricted SIM access.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_CMD_ERR_RSP:  if SELECT command implied by presence of <file_id>
                       parameter failed.
    DSAT_OK          : if it is a success.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_crsm_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);
#endif /* FEATURE_MMGSDI_ATCSIM */


/*===========================================================================
FUNCTION: dsatetsime_exec_cclk_cmd

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at+cclk command.
  at+cclk is used to set the real time clock.

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
dsat_result_enum_type dsatetsime_exec_cclk_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);
/*===========================================================================
FUNCTION: DSATETSIME_EXEC_CSTF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at+cstf command.
  at+cstf is used to set/get time format.

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
dsat_result_enum_type dsatetsime_exec_cstf_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);
/*===========================================================================
FUNCTION: DSATETSIME_EXEC_CSDF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at+csdf command.
  at+csdf is used to set/get date format.

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
dsat_result_enum_type dsatetsime_exec_csdf_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);


/*===========================================================================

FUNCTION  dsatetsime_get_alphanumeric_name

DESCRIPTION
  This function gets the alphanumeric name, given the telephone number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
pbm_return_type
dsatetsime_get_alphanumeric_name(byte *num_buf, byte num_len);


/*===========================================================================

FUNCTION DSATETSIME_GET_NEXT_NETWK_PLMN

DESCRIPTION
  This function is used to get the next PLMN stored on the ME

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR : If unable to get the next PLMN details
  DSAT_ASYNC_CMD: If PLMN details are posted to the DS task.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_get_next_netwk_plmn(void);



#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION DSATETSIME_EXEC_CPOL_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +CPOL command, which read and write entries into
  preferred Network operators list.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ASYNC_CMD : If list of preferred networks need to be retrieved 
                     from Call Manager
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_CMD_ERR_RSP : If SIM initialization is failed

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cpol_cmd
(
  dsat_mode_enum_type       mode,          /*  AT command mode:            */
  const dsati_cmd_type     *parse_table,   /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type            *res_buff_ptr   /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIME_CPOL_WRITE_PLMN_HANDLER

DESCRIPTION
  This function is used to write entries into preferred Network operators list.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ASYNC_CMD : If preffered plmn list update status need to be retrieved 
                     from Call Manager
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsime_cpol_write_plmn_handler
(
  sys_user_pref_plmn_list_s_type  *pref_networks
);

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CPLS_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +CPLS command, which selects a  PLMN selector with 
  access technology based on the card mode. 

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_CMD_ERR_RSP : If SIM initialization is failed

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cpls_cmd
(
  dsat_mode_enum_type       mode,          /*  AT command mode:            */
  const dsati_cmd_type     *parse_table,   /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type            *res_buff_ptr   /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIME_GET_SUPP_PLMN_SELECTORS

DESCRIPTION
  This function is used to get the number of supported PLMN selctors with access technology.
  A read request to different EF's is made and if they return success, then 
  the EF is added into the supported PLMN selector list.

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
dsat_result_enum_type dsatetsime_get_supp_plmn_selectors
(
  unsigned int *num_selectors
);

/*===========================================================================

FUNCTION DSATETSIME_GET_PLMN_SELECTOR

DESCRIPTION
  This function is used to get the selected PLMN selctor.

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
dsat_result_enum_type dsatetsime_get_plmn_selector
(
dsat_cpls_plmn_selector *plmn_selector
);

/*===========================================================================

FUNCTION DSATETSIME_SET_PLMN_SELECTOR

DESCRIPTION
  This function is used to set the user provided PLMN selctor in the sim.

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
dsat_result_enum_type dsatetsime_set_plmn_selector
(
dsat_num_item_type cpls_val
);

#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION DSATETSIME_CPB_CME_ERROR_LOOKUP

DESCRIPTION
  Map pbm return error type to a cme error code.

DEPENDENCIES
  None

RETURN VALUE
  Returns the cme error code if found.
  Returns DSAT_CME_MAX if not found.

SIDE EFFECTS
  None

===========================================================================*/
dsat_cme_error_e_type
dsatme_cpb_cme_error_lookup(pbm_return_type result);

/*===========================================================================
FUNCTION: DSATETSIME_EXEC_CTZU_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at+ctzu command.
  at+ctzu is used to NITZ update.

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
dsat_result_enum_type dsatetsime_exec_ctzu_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================
FUNCTION: DSATETSIME_INIT_AUTO_NITZ_SETTING_FROM_NV

DESCRIPTION
  This function initializes the auto NITZ update from NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsime_init_auto_nitz_setting_from_nv(void);

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CLAC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CLAC command.
  The result is in res_buff_ptr, reporting all supported AT commands

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
dsat_result_enum_type dsatetsime_exec_clac_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION DSATETSIME_EXEC_ICCID_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +ICCID command.
  This command returns the ICCID of the SIM card.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_CMD_ERR_RSP:  if attempt was made to read IMSI from a slot that is
                       not the GSM operationa slot.
    DSAT_OK          : if it is a success.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_iccid_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);
#endif /* FEATURE_MMGSDI */
#ifdef FEATURE_DATA_PS_EAP 
/*===========================================================================

FUNCTION  DSATETSIME_CEAP_TIMER_EXPIRED_HANDLER 

DESCRIPTION
  This function is the handler function for the +CEAP command
  reporting timer expired event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_ceap_timer_expired_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
/*===========================================================================

FUNCTION  DSATETSIME_CEAP_PROCESS_TIMER_EXPIRED_HANDLER 

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_ceap_process_timer_expired_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
/*===========================================================================

FUNCTION  DSATETSIME_EAP_TASK_SW_HANDLER

DESCRIPTION
  Handler for switching the task context from EAP module context to DS task.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_eap_task_sw_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION  DSATETSIME_EAP_RESULT_IND_HANDLER

DESCRIPTION
  Function handler to store the PMK key from EAP for a successfull EAP session 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
    DSAT_OK :          if it is a success.
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_eap_result_ind_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);


/*===========================================================================

FUNCTION  DSATETSIME_EAP_SUPP_RESULT_IND_HANDLER

DESCRIPTION
  Function handler to store the EMSK key from EAP for a successfull EAP session 

  This function call completes a successful EAP session subjective to correct data passed through. 

  This also clears the timers - DSAT_TIMER_CEAP_REPORT_TIMEOUT, DSAT_TIMER_CEAP_EAP_TIMEOUT if found 
  running.Once the Keys(EMSK here and PMK in dsatetsime_eap_result_ind_handler) are copied, the EAP instance
  is deleted.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK :       If it is a success.
    DSAT_ASYNC_EVENT : If there is an unexpected input is received. 

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_eap_supp_result_ind_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION  DSATETSIME_EAP_TRP_TX_HANDLER

DESCRIPTION
  Function handler to forward the EAP packet response towards the terminal

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:  If incorrect packet data is recieved 
    DSAT_OK :       If it is a success.
    DSAT_ASYNC_EVENT : If there is an unexpected input is received. 

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_eap_trp_tx_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
/*===========================================================================

FUNCTION DSATETSIME_EXEC_CUAD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CUAD command.
  This command asks the MT to discover what applications are available for selection on the UICC. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_CMD_ERR_RSP:  if attempt was made to read IMSI from a slot that is
                       not the GSM operationa slot.
    DSAT_OK          : if it is a success.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cuad_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIME_EXEC_CEAP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CEAP command. This command allows a TE 
  to exchange EAP packets with the UICC or the ME.


DEPENDENCIES
  Only EAP AKA and EAP SIM are supported for now.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_CMD_ERR_RSP:  if attempt was made to read IMSI from a slot that is
                       not the GSM operationa slot.
    DSAT_OK          : if it is a success.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_ceap_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);


/*===========================================================================
FUNCTION DSATETSIME_EXEC_CERP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CERP command.
  This command allows a TE to retrieve EAP session parameters / derived keys after a run of the +CEAP command. 


DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_CMD_ERR_RSP:  if attempt was made to read IMSI from a slot that is
                       not the GSM operationa slot.
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cerp_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);
#endif /* FEATURE_DATA_PS_EAP */
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_MMGSDI

/*===========================================================================
FUNCTION: DSATETSIME_CPOL_MMGSDI_CMD_HANDLER

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
dsat_result_enum_type dsatetsime_cpol_mmgsdi_handler
(
  ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION: DSATETSIME_CPOL_CMD_CB_FUNC

DESCRIPTION
  This function handles at+cops command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsime_cpol_cmd_cb_func
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);
#endif /*FEATURE_MMGSDI*/

/*===========================================================================
FUNCTION: DSATETSIME_PROCESS_PLMN_INFO

DESCRIPTION
  This function handles at+cops command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_process_plmn_info(void);


#endif /* DSATETSIME_H */
