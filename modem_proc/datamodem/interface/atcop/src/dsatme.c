/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( M O B I L E  E Q U I P M E N T  C O M M A N D S )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the common, 
  non-ETSI Mobile Equipment commands.

EXTERNALIZED FUNCTIONS

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsat_init_me
    ME initialization function:
  - registers the client call back function for GSDI events.
  - registers as a client of MMGSDI.

  dsatme_set_cme_error
    Write Mobile Equipment error to res_buff_ptr.  The Mobile Equipment
    error code and error text are defined in Section 9.2 of 3GPP TS 27.007

  dsatme_exec_cpbs_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CPBS/$QCCPBS command.
    The result is in res_buff_ptr.

  dsatme_exec_cpbr_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CPBR/$QCCPBR command.
    +CPBR command is used to read phonebook entries.

  dsatme_exec_cpbf_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CPBF/$QCCPBF command.
    +CPBF command is used to search phonebook entries.

  dsatme_exec_cpbw_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CPBW/$QCCPBW command.
    +CPBW command is used to write phonebook entry.

  dsatme_exec_cfun_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CFUN command.
    This command selects the level of functionality in the phone to
    control power consumption.

  dsatme_cmd_status_handler
    This function is the async event handler for async functions such as
    pbm_file_info, pbm_read, pbm_write, and pbm_find_name.

  dsatme_direct_dial
    Handle atd direct dialing from phonebooks command.  

  dsatme_ld_init_record
    Prepare the ld_rec for saving dialing number into last dial ph book.

  dsatme_md5_abort_cmd
    Called by AT command processor to abort MD5 command.


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004 - 2016 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatme.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/16   skc     Fixed issue related to duplicate call of csp, spn
                   and imsi read handler.
08/03/16   skc     Adding support for ESC SEQUENCE for PS Data call.
12/18/15   skc     Fixed KW issues
09/16/15   skc     Fixed compilation issue with WTGL Flavour
08/27/14   pg      Fixed CLCK query issue.
06/27/14   tk/pg   Added support for Dynamic ATCoP.
06/12/14   pg      Depricated mmgsdi_session_perso APIs.
06/06/14   sc      Fixed AT+CPBR=? issue.
04/23/14   sc      Fixed +CFUN issue.
04/17/14   sc      Fixed ATCoP KW errors.
04/14/14   tk      Fixed issues in MMGSDI events handling.
03/17/14   pg      Fixed number of SW resets in Ni/Di targets.
03/14/14   tk      Optimized debug macros usage in ATCoP.
12/25/13   tk      Enhanced $QCSIMAPP command for better user experience.
11/18/13   sc      Fixed ATCoP KW errors.
11/15/13   sc      $QCSIMAPP AT command support for TDSCDMA/LTE DSDS.
11/13/13   sc      Added support for $QCCSGCOPS command.
10/28/13   pg      Fixed CPOL issue on DSDS.
10/21/13   sc      Fixed +CFUN and +COPS issue.
10/18/13   tk      Added support for OnChip SIM in +CFUN command.
10/17/13   tk      Added support for OnChip SIM in +CIMI command.
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
08/28/13   pg      Fixed CFUN=6 issue.
08/01/13   sc      Fixed KW errors.
07/24/13   sc      Migrated Extended MMGSDI api to disaply RPLMN name always. 
07/16/13   pg      Fixed ^SYSINFO o/p for "SIM is not inserted" 
07/16/13   sc      Fixed compilation error when FEATURE_MMGSDI_PERSONALIZATION
                   is disabled.
07/02/13   tk      Fixed issue with type casting in dsatme_extract_operator_name().
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
05/28/13   tk      Fixed +CPBS issue with PIN2 for FD phone book.
05/16/13   sk      $QCCOPS command for Extension of network search.
04/19/13   tk      Fixed issue with both DSDS and thin UI enabled.
04/08/13   tk      Fixed issue with restricted mode in few scenarios.
03/28/13   tk      Mainlined FEATURE_THIN_UI using new NV-EFS item.
03/27/13   tk      Fixed issues in dual stack commands.
02/28/13   tk      Fixed stack overflow issue.
02/20/13   tk      Fixed CPWD issue with wrong PIN entries.
01/09/13   tk      Fixed issues found in DTF CI runs.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
12/03/12   tk      Mainlined the PMIC RTC features.
11/30/12   tk      ATCoP changes for Triton DSDA.
11/02/12   tk      Migrated to mmgsdi_session_get_file_attr_ext().
10/12/12   sk      Support to add "No Change" when  value is not given in +COPS.
08/22/12   sk      Mainlining Nikel Changes.
07/31/12   sk      C-EONS Feature Support.
07/31/12   sb      Fixed KW Errors.
07/26/12   sb      Fixed KW Errors.
07/02/12   sk      Reset support for  CFUN = 7 followed by CFUN = 6
06/20/12   tk      Fixed issues related to ^HDRCSQ and ^SYSINFO commands.
07/29/11   ua      Fixed to update OPL/PNN caches at the index based on the 
                   record number returned by MMGSDI read confirmation.
05/18/12   tk      Migrated to MSG 2.0 macros
04/11/12   sk      Adding support for forwarding CFUN.
03/29/12   sk      Fixed errors & warnings when FEATURE_DSAT_EXTENDED_CMD is enabled.
03/21/12   sk      EONS not triggering for Non-Reg PLMN's.
03/20/12   nc      Graceful handling of FCN for unregistered files in ATCoP.  
05/03/12   sk      Dun call failure fix on LTE.
02/20/12   sb      Fixed KW Warnings.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/06/12   nc      Added support for ^SPN and URC Queue.
01/19/12   sk      Feature cleanup.
01/04/12   sb      Fixed KW Warnings.
11/03/11   sb      Fixed KW warnings.
10/20/11   nc      Added support for CUAD/CEAP/CERP commands.
10/25/11   ua      Enhanced EONS to read EFHPLMN to check if a PLMN is registered
                   in HPLMN. 
09/20/11   ad      Fixed qcsimapp issue.
08/19/11   ua      EONS name from ONS file is to picked only for HPLMN.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
06/24/11   bs      Added support for EF CFIS/CFF.
06/16/11   ad      added delimiters if option fields are null in +CPBR cmd.
11/18/10   ad      Fixed +CPBW issue(TLB miss crash). 
04/13/11   ua      Extneding EONS for DSDS.
04/04/11   bs      Fixed COPS issue.
03/17/11   bs      Fixed DSM corruption issue.
03/11/11   bs      Fixed SUBSCRIPTION issue.
03/03/11   bs      Added check for refresh files.
03/03/11   bs      Fixed refresh registration. 
02/24/11   bs      Fixed Klocwork errors.
01/11/11   ad      Removed pending variable .
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/15/10   ad      Fixed +CPBW issue. 
10/25/10   ad      Init Changes for DSDS. 
10/18/10   bs      Fixed +CLCK issue.
10/06/10   ad      Added Error msg before Assert(0).
10/04/10   ua      Corrected EONS service table indexes. 
09/24/10   ua      Fixed reading of all OPL/PNN files correctly. 
09/09/10   ua      Fixed to use PNN PLMN name only for registered PLMN's. 
10/01/10   nc      Fixed CPBS potential crash for SM memory after SIM lock 
                   enable and disable.
09/16/10   ad      Fixed MMGSDI receives status with Success. 
09/09/10   ua      Fixed to use PNN name only for registered PLMN.  
08/30/10   ad      Fixed CPBF issue.
08/05/10   ad      Fixed +CPBF issue. 
07/27/10   ad      Fixed CPBR issue. 
07/27/10   bs      Fixed MMGSDI commands queuing in RPC context.
07/24/10   ua      Correcting the usage of command buffers in 
                   dsat_mmgsdi_event_cb.
07/14/10   ua      Fixed race condition in processing REFRESH for 
                   SIM files against regular SIM file operations. 
07/13/10   bs      Fixed linker issue.
06/21/10   kk      Fix to send coherent response for PNN read request.
06/04/10   bs      MMGSDI SESSION Migration.
05/11/10   ad      Adding new fields in +CPBW Commands.
04/10/10   kk      CDMA PIN and PBM fixes.
04/10/10   kk      Adding support for +ICCID.
04/10/10   kk      Added support for ^HC SMS commands.
03/23/10   ua      Fixed to send REFRESH complete even when 
                   MMGSDI GET_FILE_ATTR failed.  
01/20/10   kk      Extended +CFUN to CDMA/MM targets.
01/15/10   ua      PBM AT Commands support for DSDS and CSIM.
01/12/10   ua      Added support for ^CPIN commands. 
01/11/10   ms      Replaced UIM_3GPD Constants with NVRUIM_3GPD.
11/16/09   ca      Added to support for MD5 message entry mode for MMGSDI AUTH
                   commands. 
12/15/09   nc      Featurisation changes for LTE.
12/10/09   nc      Fixed feature wrap issues.
12/10/09   ps      Mainlining OMH feature based on new profile management
                   design.
12/07/09   nc      Added support for <E-UTRAN_AcT> parameter for +CPOL in LTE.
10/08/09   bs      Fixed wrongly originating of a duplicated voice call.
09/09/09   bs      Fixed Klocwork criticals.
09/22/09   vrk	   Merged LTE changes.
09/09/09   ua      Added support for EONS (spec 22.101).
08/21/09   sa      Fixed Lint Errors.
07/20/09   bs      Fixed MAX record support in phonebook.
05/18/09   bs      Support special character for phonebook read & write.
07/01/09   vg      Replacing deprecated macro MSG_XXX with MSG_SPRINTF_X.
04/29/09   ua      Fixed Lint High Errors.
04/29/09   ua      Fixed compiler warnings. 
02/17/09   bs      Fixed +CPBW to write directly to storage.
01/30/09   ua      Added support of CGEV for DUN calls.
02/10/09   bs      Fixed +CPBR crash on empty record read.
01/20/09   nc      Fixed ATD Response issue for a CSD call.
01/07/09   bs      Fixed compiler/lint warning.
12/17/08   bs      Fixed RUIM card reading issue. 
12/02/08   ss      Off target Lint fix
11/13/08   bs      Corrected Featurization issue for 1X Phonebook commands.
10/23/08   bs      Added support for 1X AT Phonebook commands.
09/25/08   ua      Fixed issues related with CPIN for 1x+GSM targets.
08/26/08   sa/psng Fixed NV RUIM read failure for OMH targets.
08/05/08   nc      Fixing compilation issues.
08/01/08   sa      Added support for CDMA DS NAArefresh.
07/21/08   bs      Added support for CDMA NAARefresh.
04/28/08   psng    Added NV Read Support for multiple SIP profiles.
06/13/08   ua      Correcting reading of data from RUIM/NV items
                   for OMH 2.
03/24/08   bs      Fixed Klocwork criticals.
01/31/08   bs      Fixing $QCSIMSTAT featurization issue.
01/21/08   bs      Added new command $QCSIMSTAT for sim status.
01/17/08   mga     Merged and made changes to support FEATURE_UIM_SUPPORT_3GPD
11/24/07   ua      Fixed Lint-lows. 
10/09/07   ss      Correcting the behaviour for GSM+1x targets.
06/27/07   ua      Changing RUIM read/write interactions from UIM to MMGSDI API's.
                   Retreiving rtre_control data through CM API. 
06/08/07   ua      Included more GSDI SIM related errors.
05/02/07   ua      Fixed lint medium errors. 
03/07/07   sa      Fixed feature wrap issues.
02/15/07   ss      Fixed lint high errors
10/20/06   sa      Modfication of +CRSM/+CSIM using MMGSDI for AID retrieval.
07/29/06   ua      Modifications supporting modified MMGSDI API for mmgsdi_callback_type
05/09/06   ua      Added function dsatme_mmgsdi_get_file_attr_cb
02/09/06   ua      The FEATURE_DSAT_CFUN_CARD_POWER_CTL is removed from this file
                   to extend for MMGSDI support for +CPLS and +CPOL commands.
08/29/05   snb     Register as MMGSDI client on initialization.
11/23/04   jk      Changes to Support 3GPD on R-UIM
09/13/04   snb     Fix 6500 build warning.
07/06/04   snb     Register for personalization PIN events.
05/18/04   jk      Added initialization code for 3GPD support on R-UIM card.
01/06/04   snb     Move registration for pin event with GSDI and handling 
                   callback for pin events to common file and add protocol
                   stack re-initialization under feature define.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DS_OMH
#include "mobile.h"
#endif /* FEATURE_DS_OMH */

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */
#include "dsatme.h"
#include "dsatctab.h"

#ifdef FEATURE_MMGSDI
#include "err.h"
#include "amssassert.h"
#include "dsatcmif.h"
#include "dsatparm.h"

/* Needed for MMGSDI support for SIM card power control functionality,
  +CPLS and +CPOL commands */
#include "mmgsdilib.h"
#include "mmgsdilib_v.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#include "sm_common.h"
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#ifdef FEATURE_DATA_ETSI_PIN
#include "dsatetsime.h"
#include "dsatetsictab.h"
#endif /* FEATURE_DATA_ETSI_PIN */

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicmif.h"
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DATA_IS707

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip.h"
#include "dsat707mip.h"
#endif /* FEATURE_DS_MOBILE_IP */

#include "ds707_data_session_profile.h"
#include "dsat707util.h"
#include "dsat707ext.h"
#include "dsat707vend.h"
#endif /* FEATURE_DATA_IS707 */

#include "nvruimi.h"
#include "dstaski.h"
#include "ps_svc.h"
#include "uim_v.h"
#endif /* FEATURE_MMGSDI */
#include "sys_v.h"
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
#include "err.h"
#include "time_svc.h"
#include "time_tod.h"
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#include "sys_m_reset.h"
#include "dsati.h"
#include "mcfg_fs.h"

/*-------------------------------------------------------------------------
  import definitions:
-------------------------------------------------------------------------*/
/* ATD Dial Table */
extern dsat_dial_type dsati_dial_table[DSAT_MODE_MAX][DSAT_MAX_DIAL_TYPE];
/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/
#define DEBUG( x )
uint8 is_card_action = 1;

#ifdef FEATURE_MMGSDI
clck_perso_fac_e_type clck_perso_fac;
#ifdef FEATURE_DSAT_ETSI_MODE
static const char dsat_qcsim_stat [][DSAT_MAX_QCSIMSTAT_CHARS] =
    {"SIM INIT COMPLETED","SIM ERROR","UNKNOWN"};
#endif /* FEATURE_DSAT_ETSI_MODE */

/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

extern uint8 cops_no_mode_change;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
    Local definitions:
-------------------------------------------------------------------------*/
#endif /* FEATURE_MMGSDI */

/*-------------------------------------------------------------------------
  Local variables
-------------------------------------------------------------------------*/
extern cb_cmd_e_type cb_expected ;
#ifdef  FEATURE_DSAT_MMGSDI_AUTH
extern dsat707_md5_s_type dsat707_md5_val;
extern dsat_mmgsdi_auth_e_type dsat_mmgsdi_auth_cmd_pending;
#endif  /* FEATURE_DSAT_MMGSDI_AUTH */

#ifdef FEATURE_DATA_ETSI_PIN
#ifdef FEATURE_MMGSDI_PERSONALIZATION
LOCAL dsat_result_enum_type cpin_process_perso_resp (ds_at_cmd_status_type *);
LOCAL dsat_cme_error_e_type me_choose_cme_error(dsat_cpin_required_e_type mask);
#endif /* FEATURE_MMGSDI_PERSONALIZATION */

LOCAL dsat_result_enum_type cpin_process_pin_status (ds_at_cmd_status_type *);

LOCAL dsat_result_enum_type cpin_verify_pin_status (ds_at_cmd_status_type *);
#ifdef FEATURE_DSAT_CFUN_CARD_POWER_CTL

LOCAL dsat_result_enum_type cfun_process_card_pwr_ctl_status
(
  ds_at_cmd_status_type *cmd_info_ptr
);
#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */

#endif /* FEATURE_DATA_ETSI_PIN */

/*===========================================================================

FUNCTION  DSATME_REPORT_CLCK_FDN_STATUS

DESCRIPTION
  This function reports the CLCK FDN status received from GSDI.

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
LOCAL dsat_result_enum_type dsatme_report_clck_fdn_status
(
  ds_at_cmd_status_type *cmd_info_ptr
);

#ifdef FEATURE_DATA_ETSI_PIN
const status_handler_table_entry_type status_handler_table[] =
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  {CPIN_MMGSDI_PERSO_REG,    cpin_process_perso_resp},
  {CPIN_MMGSDI_PERSO_DEACT,  cpin_process_perso_resp},
  {CPIN_MMGSDI_PERSO_ACT,    cpin_process_perso_resp},
  {CPIN_MMGSDI_PERSO_PERM_DIS,    cpin_process_perso_resp},
  {CPIN_MMGSDI_PERSO_GET_IND_RSP, cpin_process_perso_resp},
#endif /* FEATURE_MMGSDI_PERSONALIZATION */

  {CPIN_MMGSDI_PIN1_STATUS,  cpin_process_pin_status},
  {CPIN_MMGSDI_PIN2_STATUS,  cpin_process_pin_status},
  {CPIN_MMGSDI_PIN1_VERIFY,  cpin_verify_pin_status},
  {CPIN_MMGSDI_PIN2_VERIFY,  cpin_verify_pin_status},

  {CLCK_MMGSDI_PIN1_STATUS,   cpin_process_pin_status},
  {CLCK_MMGSDI_PIN1_VERIFY,   cpin_verify_pin_status},
  {CLCK_MMGSDI_FDN_LOCK,      cpin_verify_pin_status},
  {CLCK_MMGSDI_FDN_UNLOCK,    cpin_verify_pin_status},

  {CPWD_MMGSDI_PIN1_CHANGE,   cpin_verify_pin_status},
  {CPWD_MMGSDI_PIN2_CHANGE,   cpin_verify_pin_status},
  {CACM_MMGSDI_PIN2_VERIFY,   cpin_verify_pin_status},
  {CAMM_MMGSDI_PIN2_VERIFY,   cpin_verify_pin_status},
  {CPUC_MMGSDI_PIN2_VERIFY,   cpin_verify_pin_status},

  {CLCK_MMGSDI_FDN_ENABLE,   dsatme_report_clck_fdn_status},  
  {CLCK_MMGSDI_FDN_DISABLE,  dsatme_report_clck_fdn_status},
  {CLCK_MMGSDI_FDN_QUERY,    dsatme_report_clck_fdn_status},

  {COPS_CM_CMD_ABORT,      dsatetsicall_cops_abort_event_handler},
  {SS_CM_CMD_ABORT,        dsatetsicall_ss_abort_event_handler},
  {CLCC_CM_CMD_ABORT,      dsatetsicmif_clcc_abort_event_handler},
  {COPN_CM_CMD_ABORT,      dsatetsicall_copn_abort_event_handler},
#ifdef FEATURE_DSAT_CFUN_CARD_POWER_CTL

  /* Handle the response to MMGSDI card power control request caused by 
     +CFUN and return final result code */
  {CFUN_MMGSDI_CARD_PWR_CTL, cfun_process_card_pwr_ctl_status},
#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */

  {CB_NONE,                NULL}
};
#endif /* FEATURE_DATA_ETSI_PIN */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/* Type of address according to 3GPP TS27.007 Section 6.1 */
#define TA_INTERNATIONAL 145
#define TA_OTHER         129

/* The min and max ph book entry. 
   They are used to check if the current storage's min and max
   index are reasonable */
#define PB_INDEX_MIN       1
#define PB_INDEX_MAX     1000
#define PIN_MIN_LEN 4

/* max allowed number length */
#define CPB_MAX_NUM_LENGTH PBM_MAX_NUM_LENGTH

#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)

#define CPB_FIELD_GROUP 3
#define CPB_FIELD_ADNUMBER 4
#define CPB_FIELD_ADN_TYPE 5
#define CPB_FIELD_SECOND_TEXT 6
#define CPB_FIELD_EMAIL 7
#endif /*  defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */

#define PB_MAX_REC_LEN 500

/* The table to match pbm_device_type to at storage type 
   of 3GPP TS27.007 Section 8.11 */
typedef struct {
  /* pbm enum corresponding to the storage type */
  pbm_device_type device;

 /* storage type of 3GPP TS27.007 Section 8.11 */
  byte storage[CPBS_VAL_LEN + 1];
  /* category type */
  uint16 cat_type;
} pbm_table_type;

/* hold infos of current ph book storage 
   Initialize to be the default (SM) in the dsat_cpbs_list*/
extern me_pb_entry_type me_cpbs_entry;

extern pbm_phonebook_type dsat_pb_id;

#ifdef FEATURE_PBM_USE_EFS_PB
#define CPBS_SUPPORTED_PHBKS_NUM           9   /* Total number of phone books 
      supported by CPBS as mentioned in the array dsat_cpbs_storage_valstr */
#else
#define CPBS_SUPPORTED_PHBKS_NUM           8
#endif /* FEATURE_PBM_USE_EFS_PB */
#define SPBS_SUPPORTED_PHBKS_NUM          13   /* Total number of phone books 
      supported by CPBS as mentioned in the array dsat_spbs_storage_valstr */

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/
LOCAL dsat_result_enum_type cpbs_process_pin2_status(ds_at_cmd_status_type *);

LOCAL dsat_result_enum_type cpbs_process_verify_pin_status(ds_at_cmd_status_type *);


/*--------              pbm cmd handlers                         --------*/

LOCAL dsat_result_enum_type dsatme_find_grp_id
(
  char                *group_name ,
  dsat_num_item_type   group_length,  
  dsat_num_item_type  *grp_id
);

LOCAL dsat_result_enum_type dsatme_find_grp_name
(
  char                *group_name ,
  dsat_num_item_type   buf_length,
  dsat_num_item_type  *group_length,
  dsat_num_item_type   grp_id
);
LOCAL dsat_result_enum_type cpb_pbm_read_handler
(
  void
);

LOCAL dsat_result_enum_type cpb_pbm_reenter_read_handler
(
  ds_cmd_type* cmd_ptr
);

LOCAL dsat_result_enum_type cpb_pbm_read_handler_normal_status
(
  dsat_pbm_record_s_type     *record_ptr,
  dsat_rsp_enum_type * rsp_ptr
);

LOCAL dsat_result_enum_type cpb_pbm_read_handler_error_status
(
  pbm_return_type  pbm_return
);

LOCAL dsat_result_enum_type cpb_pbm_find_handler
(
  void
);

LOCAL dsat_result_enum_type cpb_pbm_reenter_find_handler
(
  ds_cmd_type* cmd_ptr
);

LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_success
(
  dsat_pbm_record_s_type     * record_ptr,
  uint16                       loc_id,
  dsat_rsp_enum_type         * rsp_ptr
);

LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_eof
(
  dsat_rsp_enum_type async_rsp
);

LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_fail
(
  pbm_return_type  pbm_return
);

/*--------    pbm cb cmd handlers & other misc utilities        -------- */
LOCAL dsat_result_enum_type cpb_pbm_cb_file_info_handler
(
  ds_at_pbm_info_type *pbm_info /* PBM info pointer */
);

LOCAL void cpb_pbm_cb_file_info_handler_output_info
(
  int  used
);

LOCAL dsat_result_enum_type cpb_pbm_cb_file_info_handler_error_status
(
  pbm_return_type   pbm_return
);

LOCAL dsat_result_enum_type cpb_pbm_cb_write_handler
(
  ds_at_pbm_info_type *pbm_info /* PBM info pointer */
);

LOCAL dsat_result_enum_type dd_pbm_file_info_handler
(
  ds_at_pbm_info_type *pbm_info /* PBM info pointer */
);

LOCAL dsat_result_enum_type dd_pbm_find_handler
(
  const byte *          str, 
  etsime_dd_type  *dd_val_ptr,
  pbm_device_type  pbm_device
);

/*--------                pbm call back functions                --------*/

/* pbm ready event */
LOCAL void me_ld_pbm_write_cb
(
  pbm_writecb_data_s_type * cb_data  
);

/*-------- pbm function call and cb cmd status  processing function --------*/

LOCAL dsat_result_enum_type process_pbm_record_write_return
(
  pbm_return_type   pbm_result,
  dsm_item_type *   err_buff_ptr
);

/*--------      Other ph book local function           --------*/
/* utility functions */

LOCAL boolean get_pbm_device
(
  const byte            * pb,
  pbm_device_type       * dev_ptr
);

LOCAL boolean get_pb_storage
(
  pbm_device_type       device,
  byte                  * storage_ptr
);

/* for +cpbs command */

LOCAL dsat_result_enum_type cpbs_parse_arguments
(
  const tokens_struct_type * token_ptr,
  cpbs_argu_type           * cpbs_arg_ptr
);

LOCAL dsat_result_enum_type cpbs_process_test_cmd
(
  const dsati_cmd_type *parse_table,    
  dsm_item_type *res_buff_ptr           
);

/* for +cpbr command */
LOCAL dsat_result_enum_type cpbr_parse_arguments
(
  const tokens_struct_type * token_ptr, 
  uint16 * index1_ptr,
  uint16 * index2_ptr,  
  dsm_item_type * res_buff_ptr
);

/* for +cpbw command */

LOCAL dsat_result_enum_type cpbw_parse_arguments
(
  const tokens_struct_type * token_ptr,
  dsat_pbm_record_s_type   * record_ptr,
  dsm_item_type            * res_buff_ptr
);

LOCAL boolean cpbw_check_num_string
(
  const byte * in_str,
  byte * out_str
);

/* local function prototypes for direct dialing */
LOCAL boolean me_dd_parse_dial_str
(
  const dsati_dial_str_type     * dial_ptr,
  etsime_dd_type                * dd_val_ptr
);

LOCAL dsat_result_enum_type me_dd_process_n
(
  const byte     *mem, 
  etsime_dd_type *dd_val_ptr,
  dsm_item_type  *err_buff_ptr
);

LOCAL dsat_result_enum_type me_dd_process_mem_n
(
  etsime_dd_type *dd_val_ptr,
  dsm_item_type  *err_buff_ptr
);

LOCAL dsat_result_enum_type me_dd_process_str
(
  etsime_dd_type *dd_val_ptr,
  dsm_item_type  *err_buff_ptr
);

#ifdef FEATURE_DSAT_ETSI_MODE
LOCAL dsat_result_enum_type me_dd_dial
(
  dsati_dial_str_type   * dial_ptr
);
#endif /* FEATURE_DSAT_ETSI_MODE */

/* pb book command helper functions */
LOCAL void cpb_process_pbm_error
(
  pbm_return_type   pbm_return, 
  dsm_item_type     *err_buf_ptr
);

#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#ifdef FEATURE_DS_OMH
  LOCAL void init_uim_device_model( void );
#endif /* FEATURE_DS_OMH */

/*==============================================================================
FUNCTION  DSATME_MMGSDI_CPOL_GET_FILE_ATTR_HANDLER

DESCRIPTION
  Handles the asynchronous read response for UIM_USIM_ONS or UIM_GSM_ONS.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_cpol_get_file_attr_handler
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
);

/*==============================================================================
FUNCTION  DSATME_MMGSDI_CUAD_GET_FILE_ATTR_HANDLER

DESCRIPTION
  Handles the asynchronous read response for EF DIR.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_cuad_get_file_attr_handler
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
);

#ifdef FEATURE_DSAT_GOBI_MAINLINE
#ifdef FEATURE_DATA_IS707
LOCAL void me_report_simst_result(void);
#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
#define LAST_DIALED_STR "LD"

/* The table to match pbm_device_type to AT storage type */
LOCAL const pbm_table_type
pbm_table[] = {
  { PBM_DS_SM,              "SM"             ,(uint16)PBM_CAT_SIM_ADN},
  { PBM_DIALED,             "DC"             ,(uint16)PBM_CAT_OUTGOING},
  { PBM_FDN,                "FD"             ,(uint16)PBM_CAT_SIM_FDN},
  { PBM_LND,                LAST_DIALED_STR  ,(uint16)PBM_CAT_OUTGOING},
  { PBM_MISS,               "MC"             ,(uint16)PBM_CAT_MISSED},
#ifdef FEATURE_PBM_USE_EFS_PB
  { PBM_EFS,                "ME"             ,(uint16)PBM_CAT_NONE},
#endif /* FEATURE_PBM_USE_EFS_PB */
  { PBM_RCVD,               "RC"             ,(uint16)PBM_CAT_INCOMING},
  { PBM_ECC,                "EN"             ,(uint16)PBM_CAT_ECC},
  { PBM_MSISDN,             "ON"             ,(uint16)PBM_CAT_SIM_MSISDN}
};

LOCAL const int pbm_table_size = ARR_SIZE(pbm_table);
LOCAL const status_handler_table_entry_type pbm_status_handler_table[] =
{
#ifdef  FEATURE_MMGSDI   
  {CPBS_MMGSDI_PIN2_STATUS,   cpbs_process_pin2_status},
  {CPBS_MMGSDI_PIN2_VERIFY,   cpbs_process_verify_pin_status},
#endif  /* FEATURE_ETSI_PBM */
  {CB_NONE,                NULL}
};

const pbm_cb_handler_table_type pbm_cb_handler_table[] =
{
  { CPB_PBM_CB_READY,           cpb_pbm_cb_ready_handler                },
  { CPB_PBM_CB_READY_FILE_INFO, cpb_pbm_cb_ready_file_info_handler      },
  { CPB_PBM_CB_FILE_INFO,       cpb_pbm_cb_file_info_handler            },
  { CPB_PBM_CB_WRITE,           cpb_pbm_cb_write_handler                },
  { DD_PBM_FILE_INFO,           dd_pbm_file_info_handler                },
  { CB_NONE,                    NULL}
};
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

/*-------------------------------------------------------------------------
            Function Definitions: Public functions
-------------------------------------------------------------------------*/

/*--------          Functions exported outside of ATCOP          --------*/


/*--------               INITIALIZATION FUNCTIONS                --------*/
#ifdef FEATURE_MMGSDI

/*--------     AT mobile equipment cmd handler functions         --------*/
#ifdef FEATURE_DSAT_MMGSDI_AUTH

/*===========================================================================

FUNCTION  DSATME_MMGSDI_AUTH_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the MMGSDI AUTH events

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
LOCAL dsat_result_enum_type dsatme_mmgsdi_auth_event_handler
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
)
{
  dsat_result_enum_type result     = DSAT_ASYNC_EVENT;
#ifdef FEATURE_DSAT_ETSI_MODE
  cb_cmd_e_type client_ref         = CB_NONE;

  client_ref = (cb_cmd_e_type)sim_info->client_ref;

  if ( GSN_MMGSDI_MEID_ESN == client_ref )
  {
    dsm_item_type *rb_ptr = NULL;    /*  Place to put response       */
    nv_stat_enum_type nv_status;
  
    if (dsat_mmgsdi_auth_cmd_pending == DSAT_MMGSDI_AUTH_GSN )
    {
      nv_status = dsatutil_get_nv_item( NV_MEID_I , &ds_nv_item );
  
      rb_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
      /* If MEID is not set, then report ESN   */
      rb_ptr->used = (word)snprintf((char*)rb_ptr->data_ptr,
                                               rb_ptr->size,
                                               "^GSN: ");
    
      if (( nv_status != NV_DONE_S ) || 
       ((0 == qw_hi(ds_nv_item.meid)) && (0 == qw_lo(ds_nv_item.meid)))) 
      {
        /* ESN Reporting */
        nv_status = dsatutil_get_nv_item (NV_ESN_I, &ds_nv_item);
        if ((nv_status == NV_DONE_S) && (0 != ds_nv_item.esn.esn)) 
        {
          rb_ptr->used += (word)
                       snprintf((char*)&rb_ptr->data_ptr[rb_ptr->used],
                                              rb_ptr->size - rb_ptr->used,
                                              "0x%08X ,", ds_nv_item.esn.esn);
        }
        else 
        {
           rb_ptr->used += (word)
                       snprintf((char*)&rb_ptr->data_ptr[rb_ptr->used],
                                              rb_ptr->size - rb_ptr->used,
                                               "0x0 ,");
        }
      }
      else /* Else report MEID       */
      {
        rb_ptr->used += (word)
                        snprintf((char*)&rb_ptr->data_ptr[rb_ptr->used],
                                            rb_ptr->size - rb_ptr->used,
                                            "0x%08X%08X ,", 
                                            qw_hi(ds_nv_item.meid),
                                            qw_lo(ds_nv_item.meid));
      }
    
      if ( (MMGSDI_SUCCESS == sim_info->cmd_status ) &&
           ( sim_info->data_len != 0 ))
      {
        uint8 i=0;
  
        /*report UIMID*/
        for ( i=1; i< 5;++i)
        {
          rb_ptr->used += (word)
                 snprintf((char*)&rb_ptr->data_ptr[rb_ptr->used],
                                rb_ptr->size - rb_ptr->used,
                                "%02X",
                     (uint8)(sim_info->data[i]));
        }
      }
      else
      {
        rb_ptr->used += (word)
              snprintf((char*)&rb_ptr->data_ptr[rb_ptr->used],
                       rb_ptr->size - rb_ptr->used,
                       "00000000");
      }
      dsatcmdp_handle_async_cmd_rsp (rb_ptr, DSAT_COMPLETE_RSP);
      result = DSAT_OK;
    }
    else
    {
      DS_AT_MSG0_ERROR("dsat_mmgsdi_auth_cmd_pending != DSAT_MMGSDI_AUTH_GSN");
    }
    
  }
#endif /* FEATURE_DSAT_ETSI_MODE */

  return result;
}/* dsatme_mmgsdi_auth_event_handler*/
#endif /* FEATURE_DSAT_MMGSDI_AUTH */

/*===========================================================================

FUNCTION  DSATME_MMGSDI_SIM_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the MMGSDI SIM events

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
dsat_result_enum_type dsatme_mmgsdi_sim_event_handler
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
)
{
  cb_cmd_e_type client_ref; 
  dsat_result_enum_type result   = DSAT_ASYNC_EVENT;
 
  sim_info->apps_id = dsatme_update_appsid(sim_info->session_id);
  if (!IS_VALID_GW_APPS_ID(sim_info->apps_id))
  {
    DS_AT_MSG1_ERROR("Invalid APPS id = %d",sim_info->apps_id);
    return DSAT_ASYNC_EVENT;
  }

  client_ref = (cb_cmd_e_type)sim_info->client_ref;
  DS_AT_MSG1_HIGH("ATCoP MMGSDI SIM event handler with client_ref = %d",
                                                      client_ref);

  if ( ( client_ref & DSAT_MMGSDI_REFRESH_MASK_CB ) == 
                                           DSAT_MMGSDI_REFRESH_MASK_CB )
  {
    DS_AT_MSG0_HIGH("dsatme_mmgsdi_sim_event_handler for Refresh file");
    client_ref = (cb_cmd_e_type)((int32)client_ref & (int32)(~DSAT_MMGSDI_REFRESH_MASK_CB));
  }
  switch( client_ref )
  {
#ifdef FEATURE_MMGSDI
#ifdef FEATURE_DSAT_ETSI_MODE
    case CUAD_MMGSDI_SIM_ERROR:
      SET_PENDING(DSATETSI_EXT_ACT_CUAD_ETSI_IDX ,0, DSAT_PENDING_CUAD_NONE)
      result = dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      break;	
    case CUAD_MMGSDI_SIM_FILE_ATTR:
      result = dsatme_mmgsdi_cuad_get_file_attr_handler(sim_info);
      break;
    case CUAD_MMGSDI_SIM_READ:
      result = dsatme_mmgsdi_cuad_read_handler(sim_info);
      break;

    case CPOL_MMGSDI_SIM_READ:
      result = dsatme_mmgsdi_cpol_read_handler(sim_info);
      break;
    case CPOL_MMGSDI_SIM_WRITE:
      SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
      result = DSAT_OK;
      break;
    case CPOL_MMGSDI_SIM_FILE_ATTR:
      result = dsatme_mmgsdi_cpol_get_file_attr_handler(sim_info);
      break;
    case CPOL_MMGSDI_SIM_ACCESS_DENIED:
    case CPOL_MMGSDI_SIM_ERROR:
      SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
      result = dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      break;
#endif /* FEATURE_DSAT_ETSI_MODE*/
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    case ICCID_MMGSDI_SIM_READ:
      result = dsatetsime_report_iccid(sim_info);
      break;
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
#endif /* FEATURE MMGSDI */	 
    case CPUC_MMGSDI_SIM_READ:
#ifdef FEATURE_DATA_ETSI_PIN
      result = dsatetsicall_report_cpuc(sim_info);
#else 
      result = DSAT_OK;
#endif /* FEATURE_DATA_ETSI_PIN */
      break;
    case CPUC_MMGSDI_SIM_WRITE:
      result = DSAT_OK;  /* GSDI status already verified */
      break;
    case COPN_UI_NETWK_BY_INDEX:
      result = dsatetsicall_display_plmn_list();
      break;

    default:

        DS_AT_MSG1_ERROR("Unsupported MMGSDI SIM read event: %d",
                           sim_info->client_ref);

  }
  return result;
} /* dsatme_mmgsdi_sim_event_handler */

#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION  DSAT_SEND_CME_ERROR 

DESCRIPTION
  Sends Mobile Equipment error to TE or PS protocol
  stack based on response routing.  The Mobile Equipment error 
  code and error text are defined in Section 9.2 of 3GPP TS 27.007

  If AT+CMEE is 0, +CME ERROR : <err> result code is disabled
                   ERROR is used
  If AT+CMEE is 1, +CME ERROR : <err> result code is enabled
                   to used numeric err value
  If AT+CMEE is 2, +CME ERROR : <err> result code is enabled
                   to used verbose text err value

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_CMD_ERR_RSP: for convenience in setting return value this is always 
                    returned.
  
SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat_send_cme_error
(
  dsat_cme_error_e_type error_code
)
{
  /* cmee enables the ME error result code */
  dsm_item_type *raw_rsp_ptr;

  raw_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
  dsatme_set_cme_error( error_code, raw_rsp_ptr );

  if( raw_rsp_ptr->used == 0 )
  {
    (void) dsm_free_buffer( raw_rsp_ptr );
  }
  else
  {
    /* Indicate abort command processing has completed if not disabled.
       This function performs no action if abort command processing is
       not currently in progress. */
    if ( dsatcmdp_end_abort_in_send_rsp )
    {
      dsatcmdp_command_abort_complete( );
    }

    /* Format response, send the response to the user as information text, */
    /* clear the regional block indications flag, & free raw_rsp_ptr */
    dsatcmdp_handle_async_cmd_rsp(raw_rsp_ptr, DSAT_COMPLETE_RSP);  

    raw_rsp_ptr = NULL;
  }

  return DSAT_CMD_ERR_RSP;
} /* dsat_send_cme_result( ) */

/*===========================================================================

FUNCTION DSATME_CMD_STATUS_HANDLER

DESCRIPTION
  This function is the async event handler for async functions such as
  pbm_file_info, pbm_read, pbm_write, pbm_find_name and gsdi pin requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_cmd_status_handler
(
  ds_at_cmd_status_type *status_cmd_ptr /* Status command pointer */
)
{
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) || defined FEATURE_DSAT_ETSI_MODE
  int index;
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) || FEATURE_DSAT_ETSI_MODE */
  dsat_result_enum_type result;

  /* Default action is to return the incoming cmd status */
  result = (dsat_result_enum_type)status_cmd_ptr->cmd_status;

  if (!IS_VALID_GW_APPS_ID(status_cmd_ptr->apps_id) 
#ifdef FEATURE_DSAT_CDMA_PIN
      ||((status_cmd_ptr->cmd_id >= DSAT_CDMA_CPIN_MMGSDI_ALL_PIN_STATUS) &&
         (status_cmd_ptr->cmd_id <= DSAT_CDMA_CPWD_MMGSDI_PIN2_CHANGE))
#endif /* FEATURE_DSAT_CDMA_PIN */
     )
  {
#ifdef FEATURE_DSAT_CDMA_PIN
    result = dsat707_status_event_handler(status_cmd_ptr);
#endif /* FEATURE_DSAT_CDMA_PIN */
  }
  else
  {
#ifdef FEATURE_DSAT_ETSI_MODE
    /* Check for cmd status handler */
    for(index = 0; status_handler_table[index].cmd_id != CB_NONE; index++)
    {
      if( status_handler_table[index].cmd_id ==
          (cb_cmd_e_type)status_cmd_ptr->cmd_id )
      {
        result = 
          status_handler_table[index].status_handler(status_cmd_ptr);
      }
    }
#endif /* FEATURE_DSAT_ETSI_MODE */
  }

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  /* Check for phonebook cmd status handler */
  for(index = 0; pbm_status_handler_table[index].cmd_id != CB_NONE; index++)
  {
    if( pbm_status_handler_table[index].cmd_id ==
        (cb_cmd_e_type)status_cmd_ptr->cmd_id )
    {
      result = 
        pbm_status_handler_table[index].status_handler (status_cmd_ptr);
    }
  }
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */
  return result;
} /* dsatme_cmd_status_handler */


#ifdef FEATURE_DSAT_GOBI_MAINLINE
#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION ME_REPORT_SIMST_RESULT

DESCRIPTION
  This function will report the ^SIMST unsolicited result

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void me_report_simst_result (void)
{
  dsm_item_type           *res_buff_ptr = NULL ;
  dsm_item_type * formatted_res_buff = NULL;
  uint8 sim_state = DSAT_SYSINFO_ROMSIM;
  boolean no_report = FALSE;
  
  res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
  if (NULL != res_buff_ptr)
  {
    if ( DSAT_RTRE_CONTROL_NV ==  dsat_sysinfo_cmd.dsat_rtre_config)
    {
      sim_state = DSAT_SYSINFO_ROMSIM;
    }
    else if ( DSAT_RTRE_CONTROL_RUIM ==  dsat_sysinfo_cmd.dsat_rtre_config)
    {
      if (DSAT_SIM_VALID_PRESENT == dsat_sysinfo_cmd.sim_valid[DSAT_APPS_ID_1X_PRIMARY])
      {
        sim_state = DSAT_SYSINFO_UIM;
      }
      else
      {
        sim_state = DSAT_SYSINFO_NOUIM;
      }
    }
    else
    {
      /* do not report any unsolicited response */
      no_report = TRUE;
      DS_AT_MSG0_ERROR("Shouln't report ^SIMST");
    }

    if( FALSE == no_report)
    {
      res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                 res_buff_ptr->size ,
                                                 "^SIMST: %d",
                                                 sim_state);
      DSAT_SEND_RESPONSE(res_buff_ptr,DSAT_COMPLETE_RSP);
    }/* no report == FAlSE */
  }
}/* me_report_simst_result */

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)


/*===========================================================================

FUNCTION DSATME_DIRECT_DIAL

DESCRIPTION
  Handle atd direct dialing from phonebooks command.  
  See 3GPP TS 27007 Section 6.3

  This function get the number from ph book first, and then dial 
  them number.

  This function handle all the 3 cases.
  1. D><str>[I][G][;]
  2. D>mem<n>[I][G][;]
  3. D><n>[I][G][;]

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : Async calls were make to pbm to get the number
  DSAT_CMD_ERR_RSP : A cme error relating to ph book is returned to TE.
  DSAT_ERROR       : If the command fails.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatme_direct_dial
(
  /* containing dial string and its attribs                     */
  const dsati_dial_str_type     * dial_ptr,

  /*  Place to put response                                     */
  dsm_item_type                 * res_buff_ptr
)
{
  dsat_result_enum_type  result;
  dsat_num_item_type     enum_val;
  etsime_dd_type        *dd_val_ptr;

  enum_val = (dsat_num_item_type) DSATETSI_EXT_ACT_CPBS_ETSI_IDX;
  

  dd_val_ptr = dsat_get_cmd_mem_ptr();

  /*-------- Init dd_val_ptr from dial_ptr                  --------*/
  if(FALSE == me_dd_parse_dial_str(dial_ptr, dd_val_ptr))
  {
    return DSAT_ERROR;
  }

  /* index should start from 1                                  */
  if(dd_val_ptr->mem[0] != '\0' && dd_val_ptr->index > 0)
  {
    /* D> string starts with mem and a valid number             */
    
    if( !strncmp((char *) dd_val_ptr->mem, 
                (char *) dsat_cpbs_storage_valstr[
                (dsat_num_item_type)dsatutil_get_val(enum_val,0,0,NUM_TYPE)], CPBS_VAL_LEN + 1) )

    {
      /* <mem> is current pb mem, check if index is valid       */
      if(me_cpbs_entry.total == -1 || 
         dd_val_ptr->index <= me_cpbs_entry.total )
      {
        result = me_dd_process_n(dd_val_ptr->mem, dd_val_ptr, res_buff_ptr);
      }
      else
      {
        /* the index is invalid, use D> string as search string */
        result = me_dd_process_str(dd_val_ptr, res_buff_ptr);
      }
    }
    else
    {
      /* <mem> is not current pb mem, query the mem's info.
         Check if index is in the right range.                  */
      result = me_dd_process_mem_n(dd_val_ptr, res_buff_ptr);
    }
  }
  else if (dd_val_ptr->index > 0)
  {
    /* D> string is a valid number, use current pb mem to get
       the ph number.                                           */
    if(me_cpbs_entry.total == -1 || 
       dd_val_ptr->index <= me_cpbs_entry.total )
    {
      /* Either the direct dial index is in the right range or 
         we don't know what the right range is, in which case
         we tried to get the number from the ph book          */
      result = me_dd_process_n(me_cpbs_entry.storage, dd_val_ptr, 
                                   res_buff_ptr);
    }
    else
    {
      /* the index is invalid, use D> string as search string */
      result = me_dd_process_str(dd_val_ptr, res_buff_ptr);
    }
  }
  else
  {
    /* D><str> case                                             */
    result = me_dd_process_str(dd_val_ptr, res_buff_ptr);
  }

  return result;
} /* dsatme_direct_dial */


/*===========================================================================

FUNCTION DSATME_LD_INIT_RECORD

DESCRIPTION
  Prepare the ld_rec for saving dialing number into last dial ph book.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_ld_init_record
(
  char * number, /* dialed number                         */
  int    type,   /* number type, -1 means "not provided"        */
  char * text    /* text associate with the dialed number */
)
{
  time_julian_type          julian_time;
  dsatme_pbm_state_ms_info *pbm_ms_val = NULL;

  pbm_ms_val = dsat_get_base_addr_current_subs(DSAT_PBM_MS_VALS);

  if (number != NULL)
  {
    if (NULL == pbm_ms_val->ld_rec_ptr)
    {
      pbm_ms_val->ld_rec_ptr = dsat_alloc_memory(sizeof(ld_rec_type), FALSE);
    }

    pbm_ms_val->ld_rec_ptr->state = LD_DIALED;

    /* prepare the ld_rec for updating last dial ph book */
    pbm_ms_val->ld_rec_ptr->rec.index = -1;
	/* need to verify with CPB_MAX_NUM_LENGTH */
    (void) strlcpy(pbm_ms_val->ld_rec_ptr->rec.number, number, PBM_NUM_SIZE);

    if(type != -1)
    {
      pbm_ms_val->ld_rec_ptr->rec.type= (uint8) type;
    }
    else
    {
      /* type of number is not given, figure it out by the number */
      if(number[0] == '+')
      {
        pbm_ms_val->ld_rec_ptr->rec.type = TA_INTERNATIONAL;
      }
      else
      {
        pbm_ms_val->ld_rec_ptr->rec.type = TA_OTHER;
      }
    }

    if(text != NULL)
    {
		/* need to verify with PBM_MAX_TEXT_LENGTH */
    (void) dsatutil_memscpy((void*)pbm_ms_val->ld_rec_ptr->rec.text,
            PBM_TEXT_SIZE_BYTES,(void*)text,PBM_TEXT_SIZE_CHARS);  
    }
    else
    {
      pbm_ms_val->ld_rec_ptr->rec.text[0] = '\0';
    }

    /* fill out time fields */

    memset(&julian_time, 0, sizeof(time_julian_type));

    time_get_julian(&julian_time);

    pbm_ms_val->ld_rec_ptr->rec.time = time_jul_to_secs(&julian_time);
    pbm_ms_val->ld_rec_ptr->rec.duration = 0;

    pbm_ms_val->ld_rec_ptr->rec.is_answered = FALSE;
  }
  else
  {
    DS_AT_MSG0_LOW("dsatme_ld_init_record called with no dial number");
  }  
} /* dsatme_ld_init_record */
/*--------            gsdi function return status handlers       --------*/

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION CPBS_PROCESS_PIN2_STATUS

DESCRIPTION
  Process the pin2 status for cpbs command.

  This function is called after the mmgsdi fullfiled the request to
  get the pin status.  The gsdi reply is in pin_status.

  If status show the ME is pin2 locked, continue to verify the password.
  Otherwise, set the ph book storage.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD if the request is sent to gsdi to verify pin
                 or sent to pbm to set the storage type.
  DSAT_CMD_ERR_RSP if a me error happens when pbm executes the 
                   set storage request.  A cme error is returned
                   to TE.
  DSAT_ERROR if the set storage request is refused by PBM.
             It should not happen.  A system ERR is submitted.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpbs_process_pin2_status
(
   ds_at_cmd_status_type *cmd_info_ptr
)
{
  dsat_result_enum_type result = DSAT_OK;
  boolean               pin2_locked;
  pbm_phonebook_type pb_id_local;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);
  me_ms_val = dsat_get_base_addr_per_session(cmd_info_ptr->apps_id, FALSE);

  memset(&pb_id_local,0,sizeof (pbm_phonebook_type));

  /*-------- Ensure GSDI call was successful --------*/
  if (MMGSDI_SUCCESS != cmd_info_ptr->cmd_status )
  {
    DS_AT_MSG0_HIGH("MMGSDI call not successful");
    return DSAT_ERROR;
  }

  pin2_locked = (cmd_info_ptr->cmd_data != (int16)MMGSDI_PIN_DISABLED &&
                 cmd_info_ptr->cmd_data != (int16)MMGSDI_PIN_STATUS_NOT_INITIALIZED);

  /* if the ME is pin2 locked, verify the password */
  if(pin2_locked)
  {
    mmgsdi_return_enum_type mmgsdi_status;
    mmgsdi_data_type pin_data;

    pin_data.data_ptr = (uint8 *)pbm_ss_val->me_cpbs_params.password;

#ifdef FEATURE_DSAT_CUST
    #error code not present
#else
    pin_data.data_len = (mmgsdi_len_type)strlen((char*) pbm_ss_val->me_cpbs_params.password);
#endif /* FEATURE_DSAT_CUST */

    cb_expected = CPBS_MMGSDI_PIN2_VERIFY;
    /* verify the pin, this will unlock the pin2 lock if the password
       is correct */
    VERIFY_PIN( dsat_qcsimapp_info[cmd_info_ptr->apps_id].session_id,
                MMGSDI_PIN2, 
                pin_data, 
                CPBS_MMGSDI_PIN2_VERIFY );

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      result = DSAT_ASYNC_CMD;
    }
    else if (mmgsdi_status == MMGSDI_INCORRECT_PARAMS)
    {
      /* If attempted verification of pin2 failed due to incorrect pin
         show this result in next +CPIN command response */
      if (IS_VALID_GW_APPS_ID(cmd_info_ptr->apps_id))
      {
        me_ms_val->dsat_mask_pin2_state = FALSE;
      }
      result = dsat_send_cme_error(DSAT_CME_SIM_PIN2_REQUIRED);
    }
    else 
    {
      result = DSAT_ERROR;
    }
  }
  else
  {
    /* call pbm_get_file_info to get the info about the storage 
       to be set */
    cb_expected = CPB_PBM_CB_FILE_INFO;
    /* error cases are also handled in the call-back */
    dsat_get_phonebook_id(&pb_id_local);
    pb_id_local.device_type = pbm_ss_val->me_cpbs_params.pbm_device;
    (void) pbm_session_extended_file_info_async(pb_id_local,
                                      me_session_pbm_file_info_cb);
    result = DSAT_ASYNC_CMD;
  }
  
  /* clear the password, we are not allowed to maintain a copy
     of the pin2 code */
  dsatutil_reset_password(&pbm_ss_val->me_cpbs_params.password[0],
                            sizeof(pbm_ss_val->me_cpbs_params.password));
  
  return result;
} /* cpbs_process_pin2_status */


/*===========================================================================

FUNCTION CPBS_PROCESS_VERIFY_PIN_STATUS

DESCRIPTION
  Process the pin2 verification status for cpbs command.
  If status is fine, continue to set the storage type.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_PARAM : if the parameter is not correct
    DSAT_CMD_ERR_RSP : if there was any problem in executing the command 
                 gsdi_verify_pin
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpbs_process_verify_pin_status
(
   ds_at_cmd_status_type *cmd_info_ptr
)
{
  dsat_result_enum_type result = DSAT_OK;
  mmgsdi_return_enum_type mmgsdi_result = cmd_info_ptr->cmd_status;
  pbm_phonebook_type pb_id_local;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  dsatme_pbm_state_ss_info     *pbm_ss_val = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);
  me_ms_val = dsat_get_base_addr_per_session(cmd_info_ptr->apps_id, FALSE);

  memset(&pb_id_local,0,sizeof (pbm_phonebook_type));
  
  switch (mmgsdi_result)
  {
  case MMGSDI_SUCCESS:
    /* call pbm_get_file_info to get the info about the storage 
       to be set */
    cb_expected = CPB_PBM_CB_FILE_INFO;
    /* error cases are also handled in the call-back */
     dsat_get_phonebook_id(&pb_id_local);
    pb_id_local.device_type = pbm_ss_val->me_cpbs_params.pbm_device;
    (void) pbm_session_extended_file_info_async(pb_id_local,
                                   me_session_pbm_file_info_cb);
    result = DSAT_ASYNC_CMD;
    break;

  case MMGSDI_INCORRECT_CODE:
#ifdef FEATURE_DSAT_UMTS_UNDP
    dsat_pin_retry_status.valid_flag = TRUE;
    dsat_pin_retry_status.pin_retries = cmd_info_ptr->cmd_data;
#endif  /* FEATURE_DSAT_UMTS_UNDP */
    result = dsat_send_cme_error(DSAT_CME_INCORRECT_PASSWORD);
    break;

  case MMGSDI_CODE_BLOCKED:
    /* If attempted verification of pin2 failed due to incorrect pin
       show this result in next +CPIN command response */
    if (IS_VALID_GW_APPS_ID(cmd_info_ptr->apps_id))
    {
      me_ms_val->dsat_mask_pin2_state = FALSE;
    }
    result = dsat_send_cme_error(DSAT_CME_SIM_PUK2_REQUIRED);
    break;

  default:
    DS_AT_MSG1_ERROR("Unsupported MMGSDI status : %d",mmgsdi_result);
    result = dsat_send_cme_error (DSAT_CME_SIM_FAILURE);
    break;
  }

  return result;
} /* cpbs_process_verify_pin_status */
#endif /* FEATURE_MMGSDI */


/*===========================================================================

FUNCTION DSATME_EXEC_CPBS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBS/$QCCPBS command.
  The result is in res_buff_ptr.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.
    DSAT_ASYNC_CMD: if this function invokes an async command
    DSAT_CMD_ERR_RSP : if PBM not ready

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_exec_cpbs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type     result = DSAT_OK;
  dsat_apps_id_e_type       apps_id;
  dsatme_pbm_state_ss_info *pbm_ss_val = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);
  /* Check the number of arguments.                                        */
  if (tok_ptr->args_found > 2)
  {
    DS_AT_MSG0_ERROR("Too many arguements");
    return(DSAT_ERROR);
  }

  /* process set and read command */
  if( (tok_ptr->op == NA) || 
      (tok_ptr->op == (NA|EQ|AR)) || 
      (tok_ptr->op == (NA | QU)) )
  {
    memset( (void*)pbm_ss_val->me_cpbs_params.display_name, 0, 
            sizeof(pbm_ss_val->me_cpbs_params.display_name) );
    (void)strlcpy( pbm_ss_val->me_cpbs_params.display_name, 
                       (char *)parse_table->name,
                       sizeof(pbm_ss_val->me_cpbs_params.display_name));

    if( cpbs_parse_arguments(tok_ptr, &pbm_ss_val->me_cpbs_params) != DSAT_OK)
    {
      result = DSAT_ERROR;
    }
    else /* the arguments are correct */
    {
      /* check if the argument contains password
       the password is used to unlock the pin2 lock */
      if(tok_ptr->args_found >=2)
      {
#ifdef FEATURE_MMGSDI 
        mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
        cb_cmd_e_type  client_ref = CPBS_MMGSDI_PIN2_STATUS;
        /* process the pin2 code */
        cb_expected = CPBS_MMGSDI_PIN2_STATUS;

        apps_id = dsat_get_current_apps_id(TRUE);
        if (!IS_VALID_APPS_ID(apps_id))
        {
          return DSAT_ERROR;
        }

        GET_PIN_STATUS(dsat_qcsimapp_info[apps_id].session_id, client_ref);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          result = DSAT_ASYNC_CMD;
        }
        else
        {
          /* MMGSDI command queue was full */
          dsatme_set_cme_error(DSAT_CME_MEMORY_FULL, res_buff_ptr);
          result = DSAT_CMD_ERR_RSP;
        }
#endif /* FEATURE_MMGSDI */
      }
      else
      {
        /* call pbm_get_file_info to get the info about the storage 
           to be set */
        cb_expected = CPB_PBM_CB_FILE_INFO;
        /* error cases are also handled in the call-back */
        dsat_pb_id.device_type = pbm_ss_val->me_cpbs_params.pbm_device;
        (void) pbm_session_extended_file_info_async(dsat_pb_id,
                                      me_session_pbm_file_info_cb);
        result = DSAT_ASYNC_CMD;
      }
    }
  }

  /* process the TEST command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    result = cpbs_process_test_cmd(
                                   parse_table,
                                   res_buff_ptr
                                   );
  }

  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatme_exec_cpbs_cmd */

/*===========================================================================

FUNCTION DSATME_EXEC_CPBR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBR/$QCCPBR command.
  +CPBR command is used to read phonebook entries.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK        : success
    DSAT_ERROR     : if there was any problem in executing the command
    DSAT_ASYNC_CMD : Async call is successfully made to pbm to read
                     ph book number
    DSAT_CMD_ERR_RSP : if PBM not ready

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_exec_cpbr_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  pbm_phonebook_type    pb_id_local;
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);

  memset(&pb_id_local,0,sizeof (pbm_phonebook_type));

  /* the function will write to the beginning of the buffer,
     make sure it is empty */
  ASSERT(res_buff_ptr->used == 0);

  /* Check the number of arguments.                                        */
  if (tok_ptr->args_found > parse_table->compound)
  {
    DS_AT_MSG0_ERROR("Too many arguements");
    return(DSAT_ERROR);
  }

  /* Process execution command */
  if ( tok_ptr->op == (NA|EQ|AR) )
  {

    /* Update cpbr_index.current, cpbr_index.last from command arguments.
	   We have to maintain cpbr_index.last through out because even if 
	   we re-enter reading phone book records, it knows when to stop.*/
    if( (result = cpbr_parse_arguments(tok_ptr, &pbm_ss_val->cpbr_index.current, 
                                       &pbm_ss_val->cpbr_index.last, res_buff_ptr)
		                              ) != DSAT_OK )
    {
      return result;
    }

    memset( (void*)pbm_ss_val->cpbr_index.display_name, 0, 
            sizeof(pbm_ss_val->cpbr_index.display_name) );
    
    (void)strlcpy( pbm_ss_val->cpbr_index.display_name, 
                       (char *) parse_table->name,
                       sizeof(pbm_ss_val->cpbr_index.display_name));

    /* Start reading from Phone Book */
    result = cpb_pbm_read_handler();
  }
    /* Process test command */
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    char * buff_ptr = (char *) res_buff_ptr->data_ptr;

    if (me_cpbs_entry.total < 1) 
    {
      DS_AT_MSG2_ERROR("invalid me_cpbs_entry.total in CPBR test command: %d on pbm dev %d",
                me_cpbs_entry.total, me_cpbs_entry.device);
      if (PBM_SUCCESS == me_cpbs_entry.pbm_status)
      {
      dsatme_set_cme_error(DSAT_CME_OP_NOT_ALLOWED, res_buff_ptr);
      }
      else
      {
        cpb_process_pbm_error(me_cpbs_entry.pbm_status, res_buff_ptr);
      }
      return DSAT_CMD_ERR_RSP;
    }
    else
    {
#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)

      res_buff_ptr->used = (word) snprintf(buff_ptr, 
                              res_buff_ptr->size, 
                              "%s: (1-%d),%d,%d,%d,%d,%d",
                              (char *) parse_table->name,
                              me_cpbs_entry.total,
                              CPB_MAX_NUM_LENGTH, 
                              me_cpbs_entry.text_len,
                              PBM_WORD_SIZE,
                              me_cpbs_entry.text_len,
                              me_cpbs_entry.text_len);
#else
      res_buff_ptr->used = (word) snprintf(buff_ptr, 
                              res_buff_ptr->size, 
                              "%s: (1-%d),%d,%d",
                              (char *) parse_table->name,
                              me_cpbs_entry.total,
                              CPB_MAX_NUM_LENGTH, 
                              me_cpbs_entry.text_len);

 #endif /* defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */
    } 
  }
  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatme_exec_cpbr_cmd () */

/*===========================================================================

FUNCTION DSATME_EXEC_CPBF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBF/$QCCPBF command.
  +CPBF command is used to search phonebook entries.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK          : success
    DSAT_ERROR       : if there was any problem in executing the command
    DSAT_ASYNC_CMD   : Async call is successfully made to pbm to find a
                       number in the ph book
    DSAT_CMD_ERR_RSP : if there is cme error.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_exec_cpbf_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type cscs_val ;
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;

  /* Check the number of arguments.                                        */
  if (tok_ptr->args_found > parse_table->compound)
  {
    DS_AT_MSG0_ERROR("Too many arguements");
    return(DSAT_ERROR);
  }

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);

  /* we don't support find to last dial list because pbm does 
       not support it */
  if(me_cpbs_entry.device == PBM_DS_LD)
  {
    dsatme_set_cme_error(DSAT_CME_OP_NOT_SUPPORTED, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  /* Process execution command */
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    pbm_return_type pbm_result;
    char findtext[PBM_MAX_TEXT_LENGTH];
    uint16  text_len;

    memset( (void*)findtext, 0, sizeof(findtext) );

    /* attempt to strip out the "s from the tok_ptr->arg[0] and
       convert from TE alphabet if needed: on failure return ERROR */

    if ( !VALID_TOKEN(0) )
    {
      /* Parameter value error */
      return DSAT_ERROR;
    }
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    if ( !dsatutil_strip_quotes_out(tok_ptr->arg[0],
                                    tok_ptr->arg[0],
                                    (PBM_MAX_TEXT_LENGTH * UCS2_CHAR_LEN) + 1) ||
         dsatutil_convert_chset((const char *)tok_ptr->arg[0], 
                                (dsati_chset_type)cscs_val,
                                &findtext[0],
                                ALPHA_PBM_UCS2,
                                PBM_MAX_TEXT_LENGTH, 
                                FALSE) == NULL )
    {
      dsatme_set_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT, res_buff_ptr);
      return DSAT_ERROR;
    }
    text_len =(uint16)
               ((pbmutils_wstrlen((uint16 *)findtext) + 1) * sizeof(uint16));
    /* Initialize search of the address book */
    dsat_pb_id.device_type = me_cpbs_entry.device;
    pbm_result =  pbm_session_enum_rec_init_ext( 
                           dsat_pb_id,
                           (uint16)PBM_CAT_NONE,
                            PBM_FIELD_NAME,
                           (uint8 *)findtext,
                           text_len,
                           PBM_SEARCHTYPE_STARTSWITH|PBM_STRTYPE_UCS2,
                           &pbm_ss_val->dsat_pbm_handler);

    /* Search initialization successful */
    if (pbm_result == PBM_SUCCESS )
    {
       memset( (void*)pbm_ss_val->cpbf_index.display_name, 0, 
            sizeof(pbm_ss_val->cpbf_index.display_name) );
    
      (void)strlcpy( pbm_ss_val->cpbf_index.display_name, 
                       (char *) parse_table->name,
                       sizeof(pbm_ss_val->cpbf_index.display_name));
      /* start the search */
      result = cpb_pbm_find_handler();
      pbm_session_enum_rec_init_ext_free_handle(pbm_ss_val->dsat_pbm_handler);
    }
    else if ( pbm_result == PBM_ERROR_NOT_FOUND )
    {
      result = dsat_send_cme_error(DSAT_CME_NOT_FOUND);
    }
    else
    { /* check for all other possible errors */
      result = cpb_pbm_find_handler_status_fail(pbm_result);
    }
  }
  /* Process test command */
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    char * buff_ptr = (char *) res_buff_ptr->data_ptr;

    /* if tlength info is not available */
    if(me_cpbs_entry.text_len == -1)
    {
      res_buff_ptr->used = (word) snprintf(buff_ptr,
                                                 res_buff_ptr->size, 
                                                 "%s",(char *) parse_table->name);
    }
    else
    {
      
#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)

      res_buff_ptr->used = (word) snprintf(buff_ptr,
                                                 res_buff_ptr->size, 
                                                 "%s: %d,%d,%d,%d,%d",
                                                 (char *) parse_table->name, 
                                                 CPB_MAX_NUM_LENGTH, 
                                                 me_cpbs_entry.text_len,
                                                 PBM_WORD_SIZE,
                                                 me_cpbs_entry.text_len,
                                                 me_cpbs_entry.text_len);
#else
    res_buff_ptr->used = (word) snprintf(buff_ptr,
                                               res_buff_ptr->size, 
                                               "%s: %d,%d",
                                               (char *) parse_table->name, 
                                               CPB_MAX_NUM_LENGTH, 
                                               me_cpbs_entry.text_len);
#endif /* defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */
    }
  }
  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatme_exec_cpbf_cmd () */


/*===========================================================================

FUNCTION DSATME_EXEC_CPBW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBW/$QCCPBW command.
  +CPBW command is used to write phonebook entry.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK          : success
    DSAT_ASYNC_CMD   : Async call is successfully made to pbm to write a
                       number into the ph book
    DSAT_ERROR       : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if there is cme error.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_exec_cpbw_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  uint8 index = 0;
  uint16 dsat_pbm_cat = (uint16)PBM_CAT_NONE;
  pbm_phonebook_type pb_id_local;

  memset(&pb_id_local,0,sizeof (pbm_phonebook_type));
  /* Check the number of arguments.                                        */
  if (tok_ptr->args_found > parse_table->compound)
  {
    DS_AT_MSG0_ERROR("Too many arguements");
    return(DSAT_ERROR);
  }

  /* Phone book types for which write is allowed are  
     "SM", "FD", "ON" & "ME" only */
  if(!((me_cpbs_entry.device == PBM_ADN) ||
      (me_cpbs_entry.device == PBM_FDN) ||
      (me_cpbs_entry.device == PBM_MSISDN) ||
      (me_cpbs_entry.device == PBM_EFS)))
  {
    dsatme_set_cme_error(DSAT_CME_OP_NOT_ALLOWED, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  /* process action command */
  if( tok_ptr->op == (NA|EQ|AR) )
  {
    dsat_pbm_record_s_type *record_ptr;
    pbm_return_type pbm_result;
    uint32 data_sizes[DSAT_CPBW_MAX_FIELDS_TOTAL] = {0};
    uint8 *fields_ptr = NULL;
    uint32 fields_ptr_size = 0;
    pbm_field_s_type *ptyped_fields_ptr = NULL;
    uint32 num_fields = 0;
    uint16 *temp_buff_ptr;
    dsat_pbm_rec_id_type rec_id;
    dsat_num_item_type       grp_id;
    record_ptr = (dsat_pbm_record_s_type *)dsat_alloc_memory(sizeof(dsat_pbm_record_s_type),FALSE);
    temp_buff_ptr = (uint16 *)dsat_alloc_memory(PBM_TEXT_SIZE_BYTES * sizeof(uint16),FALSE);
    if( (result = cpbw_parse_arguments(tok_ptr, record_ptr, res_buff_ptr)) == DSAT_OK )
    {
      /* If number is not provided, then it is for deletion*/
      if (!((1 == tok_ptr->args_found )&&(record_ptr->index != -1)))
      {
        /* Update the data_sizes sizeof(uint16) factor is to support FULL UCS2
           For string types provide space for NULL character */ 
        if(record_ptr->text[0] != '\0')
        {
          (void) dsatutil_memscpy((void*)temp_buff_ptr,
                  PBM_TEXT_SIZE_BYTES,(void*)record_ptr->text,PBM_TEXT_SIZE_BYTES);
          data_sizes[CPB_FIELD_TEXT] = (pbmutils_wstrlen(temp_buff_ptr)+ 1) * sizeof(uint16);
        }       
        else
        {
          data_sizes[CPB_FIELD_TEXT] = sizeof(uint16);
          /*set default size */
        }
        data_sizes[CPB_FIELD_NUMBER] =  strlen(record_ptr->number)+ 1;
        data_sizes[CPB_FIELD_NUMBER_TYPE] =  sizeof(uint8);
        num_fields = DSAT_CPBW_MAX_FIELDS;
#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)

        if( record_ptr->group[0] != '\0')
        {
          (void) dsatutil_memscpy((void*)temp_buff_ptr,
                            PBM_TEXT_SIZE_BYTES,(void*)record_ptr->group,PBM_TEXT_SIZE_BYTES);
          data_sizes[num_fields]= (pbmutils_wstrlen(temp_buff_ptr)+ 1) * sizeof(uint16);
          num_fields ++;
        }
        if(record_ptr->adnumber[0] != '\0')
        {
          data_sizes[num_fields]= strlen(record_ptr->adnumber)+1;
          num_fields ++;
          data_sizes[num_fields] =  sizeof(uint8);
          num_fields ++;
        }
        if(record_ptr->secondtext[0] != '\0')
        {
          (void) dsatutil_memscpy((void*)temp_buff_ptr,
                            PBM_TEXT_SIZE_BYTES,(void*)record_ptr->secondtext,PBM_TEXT_SIZE_BYTES);
          data_sizes[num_fields] = 
               (pbmutils_wstrlen(temp_buff_ptr)+ 1) * sizeof(uint16);
          num_fields ++;
        }
        if(record_ptr->email[0] != '\0')
        {
          (void) dsatutil_memscpy((void*)temp_buff_ptr,
                            PBM_TEXT_SIZE_BYTES,(void*)record_ptr->email,PBM_TEXT_SIZE_BYTES);
          data_sizes[num_fields] = 
             (pbmutils_wstrlen(temp_buff_ptr)+ 1) * sizeof(uint16);
          num_fields ++;
        }
#endif /*defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */
        if (FALSE == pbm_allocate_fields
               (&fields_ptr, &fields_ptr_size, num_fields, data_sizes))
        {
          DS_AT_MSG0_ERROR("Error: Could not allocate size");
          dsatutil_free_memory((void *) record_ptr);
          dsatutil_free_memory((void *) temp_buff_ptr);
          return DSAT_ERROR;
        }
        
        ptyped_fields_ptr = (pbm_field_s_type *) fields_ptr;

        pbm_fill_field(&ptyped_fields_ptr[CPB_FIELD_TEXT], PBM_FIELD_NAME,PBM_FT_UCS2,
                       (byte *)(&record_ptr->text[0]), (uint16) data_sizes[CPB_FIELD_TEXT]);

        pbm_fill_field(&ptyped_fields_ptr[CPB_FIELD_NUMBER], PBM_FIELD_PHONE_GENERIC,
                       PBM_FT_PHONE,(byte *)record_ptr->number, 
                       (uint16) data_sizes[CPB_FIELD_NUMBER]);

        pbm_fill_field(&ptyped_fields_ptr[CPB_FIELD_NUMBER_TYPE], PBM_FIELD_GSM_TONNPI,
                       PBM_FT_BYTE, (byte *)&record_ptr->type, 
                       (uint16) data_sizes[CPB_FIELD_NUMBER_TYPE]);

       num_fields = DSAT_CPBW_MAX_FIELDS;
           /*Set num filed to baisc value (3) and then increment for non null value */
#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)

        if(record_ptr->group[0] != '\0')
        {
          if ( DSAT_ERROR == dsatme_find_grp_id(
                              &record_ptr->group[0] ,
                              data_sizes[num_fields],
                              &grp_id))
          {
            /* Free allocated memory and throw error */
            pbm_free_fields(&fields_ptr,num_fields);
            dsatutil_free_memory((void *) record_ptr);
            dsatutil_free_memory((void *) temp_buff_ptr);
            return DSAT_ERROR;
          }
          pbm_fill_field(&ptyped_fields_ptr[num_fields],
                         PBM_FIELD_GRPNAME_ID,
                         PBM_FT_WORD,
                        (byte *)(&grp_id), 
                        (uint16) sizeof(uint16));
          num_fields ++;
        }
        if(record_ptr->adnumber[0] != '\0')
        {
          pbm_fill_field(&ptyped_fields_ptr[num_fields],
                         PBM_FIELD_ADDITIONAL_NUMBER,
                         PBM_FT_PHONE,
                         (byte *)record_ptr->adnumber, 
                         (uint16) data_sizes[num_fields]);
          num_fields ++;
          
          pbm_fill_field(&ptyped_fields_ptr[num_fields],
                         PBM_FIELD_NUMBER_TYPE,
                         PBM_FT_BYTE, 
                         (byte *)&record_ptr->adtype, 
                         (uint16) data_sizes[num_fields]);
          num_fields ++;
        }
        if(record_ptr->secondtext[0] != '\0')
        {
          pbm_fill_field(&ptyped_fields_ptr[num_fields],
                         PBM_FIELD_OTHERNAME,
                         PBM_FT_UCS2,
                        (byte *)(&record_ptr->secondtext[0]),
                        (uint16) data_sizes[num_fields]);
          num_fields ++;
        }
        
        if(record_ptr->email[0] != '\0')
        {
          pbm_fill_field(&ptyped_fields_ptr[num_fields], 
                         PBM_FIELD_EMAIL,
                         PBM_FT_UCS2,
                         (byte *)(&record_ptr->email[0]),
                         (uint16) data_sizes[num_fields]);
          num_fields ++;
        }
#endif /*  defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */
        /* If valid index is provided, update in that location else get the first 
           avalible index from PBM */
        dsat_pb_id.device_type = me_cpbs_entry.device;
        rec_id = (record_ptr->index>0)? 
                  pbm_session_location_to_record_id(dsat_pb_id,record_ptr->index):
                  PBM_SESSION_GET_FIRST_AVAILABLE;
      }
      else
      {
        /* For deletion fields_ptr should be NULL and num_fields must be zero */
        fields_ptr = NULL;
        num_fields = 0;
        dsat_pb_id.device_type = me_cpbs_entry.device;
        rec_id = pbm_session_location_to_record_id(dsat_pb_id,record_ptr->index);
      }
      
       /*------ search pbm cat type for pb in static pbm_table ------*/
      for (index = 0; index < pbm_table_size; index++)
      {
        if(me_cpbs_entry.device == pbm_table[index].device)
        {
          DS_AT_MSG1_HIGH("PBM device type = %d",me_cpbs_entry.device);
          dsat_pbm_cat = pbm_table[index].cat_type;
        }
      }

      cb_expected = CPB_PBM_CB_WRITE;
      dsat_pb_id.device_type = me_cpbs_entry.device;
      pbm_result = pbm_session_record_write( dsat_pb_id, 
                                             &rec_id,
                                             dsat_pbm_cat,
                                             (int)num_fields, 
                                             fields_ptr, 
                                             fields_ptr_size,
                                             me_pbm_write_cb, 
                                             NULL);

      result = process_pbm_record_write_return(pbm_result, res_buff_ptr);

      if(result != DSAT_ASYNC_CMD)
      {
        cb_expected = CB_NONE;
      }
      /* Now its safe to free the fields_ptr */
      pbm_free_fields(&fields_ptr,num_fields);
    }
    dsatutil_free_memory((void *) record_ptr);
    dsatutil_free_memory((void *) temp_buff_ptr);
  }
  
  /* Process test command */
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    char * buff_ptr = (char *) res_buff_ptr->data_ptr;
    int total = me_cpbs_entry.total > 0 ? me_cpbs_entry.total : 1;
    char text_len_str[4];

    if(me_cpbs_entry.total < 1)
    {
      DS_AT_MSG2_ERROR("invalid me_cpbs_entry.total in CPBW test command: %d on pbm dev %d",
                me_cpbs_entry.total, me_cpbs_entry.device);
      if (PBM_SUCCESS == me_cpbs_entry.pbm_status)
      {
      dsatme_set_cme_error(DSAT_CME_OP_NOT_ALLOWED, res_buff_ptr);
      }
      else
      {
        cpb_process_pbm_error(me_cpbs_entry.pbm_status, res_buff_ptr);
      }
      return DSAT_CMD_ERR_RSP;
    }

    if(me_cpbs_entry.text_len < 0)
    {
      text_len_str[0] = '\0';
    }
    else
    {
      (void) dsatutil_itoa((word) me_cpbs_entry.text_len, 
                           (byte *)text_len_str, 10);
    }

#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)

    res_buff_ptr->used = 
        (word) snprintf(buff_ptr,res_buff_ptr->size, 
                              "%s: (1-%d),%d,(128-255),%s,%d,%s,%s",
                              (char*)parse_table->name, 
                              total,
                              CPB_MAX_NUM_LENGTH, 
                              text_len_str,
                              PBM_WORD_SIZE,
                              text_len_str,
                              text_len_str
                              );
#else
    res_buff_ptr->used = 
        (word) snprintf(buff_ptr,res_buff_ptr->size, 
                              "%s: (1-%d),%d,(128-255),%s",
                              (char*)parse_table->name,
                              total,
                              CPB_MAX_NUM_LENGTH, text_len_str);
#endif /* defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */

  }
  
  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatme_exec_cpbw_cmd () */
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

/*===========================================================================

FUNCTION  DSATETSIME_SET_CME_ERROR

DESCRIPTION
  Write Mobile Equipment error to res_buff_ptr.  The Mobile Equipment error
  code and error text are defined in Section 9.2 of 3GPP TS 27.007

  If AT+CMEE is 0, +CME ERROR : <err> result code is disabled
                   ERROR is used
  If AT+CMEE is 1, +CME ERROR : <err> result code is enabled
                   to used numeric err value
  If AT+CMEE is 2, +CME ERROR : <err> result code is enabled
                   to used verbose text err value

DEPENDENCIES
  res_buff_ptr point to a good dsm buffer with enough space left 
  (at lease 60 bytes) to write error code.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatme_set_cme_error
(
  dsat_cme_error_e_type error_code,
  dsm_item_type         *res_buff_ptr
)
{
  /* err code table according to Section 9.2 of 3GPP TS 27.007 */
  static const struct {
    dsat_cme_error_e_type cme_code;
    char * cme_text;
  } cme_table[] = {
    { DSAT_CME_PHONE_FAILURE,            "phone failure" },
    { DSAT_CME_NO_CONNECTION,            "no connection to phone"},
    { DSAT_CME_ADAPTOR_LINK_RESERVED,    "phone adaptor link reserved"},
    { DSAT_CME_OP_NOT_ALLOWED,           "operation not allowed"},
    { DSAT_CME_OP_NOT_SUPPORTED,         "operation not supported"},
    { DSAT_CME_PH_SIM_PIN_REQUIRED,      "PH-SIM PIN required"},
    { DSAT_CME_PH_FSIM_PIN_REQUIRED,     "PH-FSIM PIN required"},
    { DSAT_CME_PH_FSIM_PUK_REQUIRED,     "PH-FSIM PUK required"},
    { DSAT_CME_SIM_NOT_INSERTED,         "SIM not inserted"},
    { DSAT_CME_SIM_PIN_REQUIRED,         "SIM PIN required"},
    { DSAT_CME_SIM_PUK_REQUIRED,         "SIM PUK required"},
    { DSAT_CME_SIM_PIN2_REQUIRED,        "SIM PIN2 required"},
    { DSAT_CME_SIM_PUK2_REQUIRED,        "SIM PUK2 required"},
    { DSAT_CME_SIM_FAILURE,              "SIM failure"},
    { DSAT_CME_SIM_BUSY,                 "SIM busy"},
    { DSAT_CME_SIM_WRONG,                "SIM wrong"},
    { DSAT_CME_INCORRECT_PASSWORD,       "incorrect password"},
    { DSAT_CME_MEMORY_FULL,              "memory full"},
    { DSAT_CME_INVALID_INDEX,            "invalid index"},
    { DSAT_CME_NOT_FOUND,                "not found"},
    { DSAT_CME_MEMORY_FAILURE,           "memory failure"},
    { DSAT_CME_TEXT_STRING_TOO_LONG,     "text string too long"},
    { DSAT_CME_INVALID_CHAR_IN_TEXT,     "invalid characters in text string"},
    { DSAT_CME_DIAL_STRING_TOO_LONG,     "dial string too long"},
    { DSAT_CME_INVALID_CHAR_IN_DIAL_STRING,
                                        "invalid characters in dial string"},
    { DSAT_CME_NO_NETWORK_SERVICE,       "no network service"},
    { DSAT_CME_NETWORK_TIMEOUT,          "network timeout"},
    { DSAT_CME_NETWORK_NOT_ALLOWED, "network not allowed - emergency calls only"},
    { DSAT_CME_NET_PERSON_PIN_REQUIRED,  "network personalization PIN required"},
    { DSAT_CME_NET_PERSON_PUK_REQUIRED,  "network personalization PUK required"},

    { DSAT_CME_NET_SUBSET_PERSON_PIN_REQUIRED,
                               "network subset personalization PIN required"},

    { DSAT_CME_NET_SUBSET_PERSON_PUK_REQUIRED, 
                               "network subset personalization PUK required"},

    { DSAT_CME_SP_PERSON_PIN_REQUIRED,   
                               "service provider personalization PIN required"},

    { DSAT_CME_SP_PERSON_PUK_REQUIRED,   
                               "service provider personalization PUK required"},

    { DSAT_CME_CORP_PERSON_PIN_REQUIRED, 
                               "corporate personalization PIN required"},

    { DSAT_CME_CORP_PERSON_PUK_REQUIRED, 
                               "corporate personalization PUK required"},
    { DSAT_CME_EAP_NOT_SUPPORTED, 
                               "EAP method not supported"},
    { DSAT_CME_EAP_INVALID_PARAM, 
                               "Invalid EAP parameter"},
    { DSAT_CME_AUTH_PARAM_LENGTH_ERROR, "parameter length error"},
    { DSAT_CME_MMGSDI_AUTH_ERROR,       "Invalid Auth data"},
    { DSAT_CME_UNKNOWN,                  "unknown"},
    { DSAT_CME_ILLEGAL_MS,               "Illegal MESSAGE"},
    { DSAT_CME_ILLEGAL_ME,               "Illegal ME"},
    { DSAT_CME_GPRS_serv_not_allowed,    "GPRS services not allowed"},
    { DSAT_CME_PLMN_NOT_ALLOWED,         "PLMN not allowed"},
    { DSAT_CME_LOCATION_NOT_ALLOWED,     "Location area not allowed"},
    { DSAT_CME_ROAMING_NOT_ALLOWED,  "Roaming not allowed in this location area"},
    { DSAT_CME_SERV_OP_NOT_SUPPORTED,    "service option not supported"},

    { DSAT_CME_REQ_SERV_OP_NOT_SUBSCRIBED, 
                                    "requested service option not subscribed"},

    { DSAT_CME_SERV_OP_TEMP_OUT_OF_ORDER,  
                                    "service option temporarily out of order"},

    { DSAT_CME_unspecified_GPRS_error,   "unspecified GPRS error"},
    { DSAT_CME_PDP_AUTH_FAILURE,         "PDP authentication failure"},
    { DSAT_CME_INVALID_MOBILE_CLASS,     "invalid mobile class"},

    { DSAT_CME_SUPS_NETWORK_REJECTED,    "network rejected request"},
    { DSAT_CME_SUPS_RETRY_OPERATION,     "retry operation"},
{ DSAT_CME_SUPS_DFL_INVALID_NUMBER,  "invalid deflected to number"},

    { DSAT_CME_SUPS_DFL_SELF_NUMBER,     "deflected to own number"},
    { DSAT_CME_SUPS_UNKNOWN_NUMBER,      "unknown subscriber"},
    { DSAT_CME_SUPS_SERVICE_UNAVAIL,     "service not available"},
    { DSAT_CME_SUPS_UNKNOWN_CLASS,       "unknown class specified"},
    { DSAT_CME_SUPS_UNKNOWN_NETMSG,      "unknown network message" },
#ifdef FEATURE_SECONDARY_PDP
    { DSAT_CME_INVALID_PARAM_COMB,       "invalid parameter combination" },
    { DSAT_CME_MIN_TFT_PER_ADDR,         "minimum TFTs per PDP address violated"} ,
    { DSAT_CME_TFT_EPI_DUPLICATE,        "TFT precedence index not unique" },  
#endif /* FEATURE_SECONDARY_PDP */
    { DSAT_CME_CALL_INDEX_ERROR,          "Call index error" },
    { DSAT_CME_CALL_STATE_ERROR,          "Call state error" },
    { DSAT_CME_SYS_STATE_ERROR,           "Sys state error" },
    { DSAT_CME_PARAMETER_ERROR,           "Parameters error" },
    { DSAT_CME_MAX,                      NULL }
  };
  
  char * buff_ptr = (char *) (res_buff_ptr->data_ptr + res_buff_ptr->used);
  /* cmee enables the ME error result code */
  if( (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_CMEE_IDX,0,0,NUM_TYPE) == 1 || 
    (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_CMEE_IDX,0,0,NUM_TYPE) == 2 )
  {
    int index = 0;
    char * cme_string = NULL;

    /* search for the cme table for the error code/text */
    do
    {
      cme_string = cme_table[index].cme_text;
    } while (cme_table[index++].cme_code != error_code &&
             cme_string != NULL);
  
    if(cme_string == NULL)
    {
      DS_AT_MSG1_ERROR("Incorrect cme code %d", error_code);
      return;
    }
  
    if ((dsat_num_item_type)dsatutil_get_val(DSAT_EXT_CMEE_IDX,0,0,NUM_TYPE) == 1)
    {
      res_buff_ptr->used += (word)snprintf(buff_ptr,
                                           res_buff_ptr->size - res_buff_ptr->used,
                                           "+CME ERROR: %d",
                                           error_code);
    }
    else
    {
      res_buff_ptr->used += (word)snprintf(buff_ptr, 
                                          res_buff_ptr->size - res_buff_ptr->used,
                                          "+CME ERROR: %s",
                                          cme_string);
#ifdef FEATURE_DSAT_UMTS_UNDP
      if( ( DSAT_CME_INCORRECT_PASSWORD == error_code ) &&
          ( TRUE == dsat_pin_retry_status.valid_flag ) )
      {
        res_buff_ptr->used += (word)snprintf(buff_ptr, 
                                          res_buff_ptr->size - res_buff_ptr->used,
                                          "%s %d %s",
                                          buff_ptr,
                                          dsat_pin_retry_status.pin_retries,
                                          "attempts remaining");
        dsat_pin_retry_status.valid_flag = FALSE;
      }
#endif /* FEATURE_DSAT_UMTS_UNDP */
    }
  }
  else /* AT+CMEE = 0 */
  {
    res_buff_ptr->used += (word)snprintf(buff_ptr, 
                                          res_buff_ptr->size - res_buff_ptr->used,
                                          "ERROR");
  }

} /* dsatme_set_cme_error( ) */

/*===========================================================================

FUNCTION DSATME_EXEC_CARDMODE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ^CARDMODE command, which returns the 
  mode of the card amongst SIM or USIM

^CARDMODE	<CR><LF>^CARDMODE:<sim_type><CR><LF><CR><LF>OK <CR><LF>

<sim_type>:Types of SIM card. Its values are as below:
0:  Unknown mode
1:  SIM card
2:  USIM card
3:  CSIM card
4:  RUIM card

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
/*ARGSUSED */
dsat_result_enum_type dsatme_exec_cardmode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define DSAT_GSM_SIM 1
#define DSAT_USIM 2
#define DSAT_CSIM 3
#define DSAT_RUIM 4
#define DSAT_UNKONWN_SIM 0
#define DSAT_NO_SIM 255

  dsat_num_item_type sim_type = DSAT_UNKONWN_SIM;
  /*..................................................................................................*/

  if( tok_ptr->op == (NA) )
  {
    switch (dsat_get_current_app_type(TRUE))
    {
      case MMGSDI_APP_SIM:
        sim_type = DSAT_GSM_SIM;
        break;

      case MMGSDI_APP_USIM:
        sim_type = DSAT_USIM;
        break;

      case MMGSDI_APP_RUIM:
        sim_type = DSAT_RUIM;
        break;

      case MMGSDI_APP_CSIM:
        sim_type = DSAT_CSIM;
        break;

      case MMGSDI_APP_NONE:
        sim_type = DSAT_NO_SIM;
        break;

      default:
        sim_type = DSAT_UNKONWN_SIM;
        break;
    }

    res_buff_ptr->used =
      (word) snprintf((char*)res_buff_ptr->data_ptr,res_buff_ptr->size,
                             "^CARDMODE: %d",sim_type);
    return DSAT_OK;
  }
  else
  {
    return DSAT_ERROR;
  }
}/* dsatme_exec_cardmode_cmd */

/*===========================================================================
FUNCTION DSATME_EXEC_SPN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to display the brand information from EF-SPN

Command | Response
--------|---------------------
^SPN	|^SPN:<disp_rplmn>,<coding>,<spn_name>
        | OK

<disp_rplmn>:
 0: RPLMN will not be displayed
 1: RPLMN is required to be displayed
 99:this field is noneffective, and there’s no need for reading spn_name field again.

<coding>:coding scheme, representing the character coding of spn_name field and indicating the language:
 0:GSM 7 bit Default Alphabet
 1:USC2 mode

<spn_name>:character string, the length will not exceed 16 bytes.

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
dsat_result_enum_type dsatme_exec_spn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{

  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  me_ms_val = dsat_get_base_addr_current_session();

  if(tok_ptr->op == NA)
  {
    if(me_ms_val->dsat_spn_info.spn_info_valid)
    {
      res_buff_ptr->used =
      (word) snprintf((char*)res_buff_ptr->data_ptr,res_buff_ptr->size,
                               "^SPN: %d,%d,\"",
                               ((me_ms_val->dsat_spn_info.display_cond & 0x02)? 0:1),
                                 me_ms_val->dsat_spn_info.encoding_type);

      (void) dsatutil_memscpy((void*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
              DSAT_BUF_SIZE(res_buff_ptr->size,res_buff_ptr->used),
             (void *)&me_ms_val->dsat_spn_info.operator_name[0],
              MIN(DSAT_MAX_SPN_NAME_LEN,me_ms_val->dsat_spn_info.operator_name_len));

      res_buff_ptr->used += me_ms_val->dsat_spn_info.operator_name_len;

      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\"';
      res_buff_ptr->used++;

      return DSAT_OK;
    }
    else
    {
      DS_AT_MSG0_ERROR("SPN info not available");
    }
  }

  return DSAT_ERROR;
}/* dsatme_exec_spn_cmd */
/*===========================================================================

FUNCTION DSATME_DISPLAY_QCSIMAPP_VAL

DESCRIPTION
  This function displays result for the qcismapp_val passed.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED */
LOCAL void dsatme_display_qcsimapp_val
(
  dsat_num_item_type  qcsimapp_val,
  dsm_item_type      *res_buff_ptr
)
{
  dsatcmif_servs_state_ms_info  *ph_ss_ms_val = NULL;
  char                           subs_str[5] = "";
  char                           sys_mode_str[15];
  sys_modem_as_id_e_type         subs_id;

  if (qcsimapp_val >= DSAT_SLOT_IDX_MAX)
  {
    return;
  }

  subs_id = dsat_qcsimapp_table[qcsimapp_val].subs_id;

  memset(sys_mode_str, 0, sizeof(sys_mode_str));

  if (!IS_VALID_SUBS_ID(subs_id))
  {
    snprintf(sys_mode_str, sizeof(sys_mode_str), "No Service");
  }
  else
  {
    snprintf(subs_str, sizeof(subs_str), "SUB%u", subs_id + 1);

    ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS,subs_id, FALSE);

    switch (ph_ss_ms_val->dsat_net_reg_state.sys_mode)
    {
      case SYS_SYS_MODE_CDMA:
      case SYS_SYS_MODE_HDR:
        snprintf(sys_mode_str, sizeof(sys_mode_str), "C");
        break;
  
      case SYS_SYS_MODE_GSM:
        snprintf(sys_mode_str, sizeof(sys_mode_str), "G");
        break;
  
      case SYS_SYS_MODE_WCDMA:
        snprintf(sys_mode_str, sizeof(sys_mode_str), "W");
        break;
  
  #ifdef FEATURE_DSAT_LTE
      case SYS_SYS_MODE_LTE:
        snprintf(sys_mode_str, sizeof(sys_mode_str), "L");
        break;
  #endif /* FEATURE_DSAT_LTE */
  
  #ifdef FEATURE_TDSCDMA
      case SYS_SYS_MODE_TDS:
        snprintf(sys_mode_str, sizeof(sys_mode_str), "T");
        break;
  #endif /* FEATURE_TDSCDMA */
      case SYS_SYS_MODE_NO_SRV:
      default:
        snprintf(sys_mode_str, sizeof(sys_mode_str), "No Service");
        break;
    }
  }

  res_buff_ptr->used += (word)snprintf(
                           (char *)(res_buff_ptr->data_ptr + res_buff_ptr->used),
                           (res_buff_ptr->size - res_buff_ptr->used),
                           "$QCSIMAPP: %d,\"%s\",\"%s\"",
                           qcsimapp_val,
                           subs_str,
                           sys_mode_str);

  return;
}/* dsatme_display_qcsimapp_val */

/*===========================================================================

FUNCTION DSATME_EXEC_QCSIMAPP_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes $QCSIMAPP command which selects the SIM Applications
  in the card.

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
/*ARGSUSED */
dsat_result_enum_type dsatme_exec_qcsimapp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_ERROR;
  dsat_sio_info_s_type  *sio_info_ptr = NULL;
  dsat_num_item_type     qcsimapp_val = 0;
  uint8                  max_subs = dsat_get_max_subs();

  if (tok_ptr->op == (NA|EQ|AR))
  {
    if ( !VALID_TOKEN(0) || (dsatutil_atoi( &qcsimapp_val, 
                      tok_ptr->arg[0], 10) != ATOI_OK ) || ( qcsimapp_val >= max_subs ))
    {
      /* if index1 can not be converted into a legal number,
        this is invalid parameter */
      DS_AT_MSG1_ERROR("Arg 1 must be a number between 0-%d", (max_subs - 1));
      return DSAT_ERROR;
    }

    DSATUTIL_SET_VAL(DSAT_VENDOR_QCSIMAPP_IDX,0,0,0,qcsimapp_val,NUM_TYPE);

    sio_info_ptr = dsatcmdp_get_sio_info_ptr(ds3g_siolib_get_active_port());
    sio_info_ptr->qcsimapp_val = qcsimapp_val;
    dsat_set_qcsimapp_val_user_flag();
    result = DSAT_OK;
  }
  else if(tok_ptr->op == (NA|QU))
  {
    dsatme_display_qcsimapp_val(dsat_get_qcsimapp_val(), res_buff_ptr);

    result = DSAT_OK;
  }
  else if(tok_ptr->op == (NA|EQ|QU))
  {
    for (qcsimapp_val = 0; qcsimapp_val < max_subs; qcsimapp_val++)
    {
      dsatme_display_qcsimapp_val(qcsimapp_val, res_buff_ptr);

      if (qcsimapp_val != (max_subs - 1))
      {
        res_buff_ptr->used += (word)snprintf(
                                 (char *)(res_buff_ptr->data_ptr + res_buff_ptr->used),
                                 (res_buff_ptr->size - res_buff_ptr->used),
                                 "\n");
      }
    }

    result = DSAT_OK;
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}
#ifdef FEATURE_MMGSDI
#ifdef FEATURE_DATA_ETSI_PIN
#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================

FUNCTION CPIN_PROCESS_PERSO_RESP

DESCRIPTION
  This function is the handler for async response following request of 
  registration for personalization PIN events or deactivation of 
  personalization PIN request made to GSDI.  
  The GSDI reply is in cmd_status.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : if GSDI personalization PIN deactivation request was
                     successfully made.
  DSAT_ASYNC_EVENT : if GSDI personalization PIN event registration was not
                     successful or unrecognized event.
  DSAT_CMD_ERR_RSP : if personalization PIN password supplied was incorrect.
  DSAT_ERROR       : if GSDI personalization PIN deactivation failed for some 
                     other reason.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpin_process_perso_resp
(
   ds_at_cmd_status_type *cmd_info_ptr
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  mmgsdi_return_enum_type mmgsdi_result = cmd_info_ptr->cmd_status;
  dsat_cme_error_e_type error_code = DSAT_CME_MAX;
  dsat_apps_id_e_type apps_id = cmd_info_ptr->apps_id;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  mmgsdi_session_get_info_type  get_info;
  mmgsdi_session_info_query_type  query_info;
  
  memset((void*)&get_info , 0 , sizeof(get_info) );
  me_ms_val = dsat_get_base_addr_per_session(apps_id, FALSE);
  switch (cmd_info_ptr->cmd_id)
  {
    case CPIN_MMGSDI_PERSO_DEACT:
      switch ( mmgsdi_result )
      {
        case MMGSDI_SUCCESS:
          /* Event is sent when the app is in MMGSDI_APP_STATE_READY_FOR_PERSO */
          query_info.query_type = MMGSDI_SESSION_INFO_QUERY_BY_ID;
          query_info.query_key.session_id = dsat_qcsimapp_info[apps_id].session_id;
          (void)mmgsdi_session_get_info_sync(query_info, &get_info);
          if(get_info.app_state == MMGSDI_APP_STATE_READY)
          {
            result = DSAT_OK;
          }
          else if(get_info.app_state == MMGSDI_APP_STATE_READY_FOR_PERSO)
          {
            result = DSAT_ASYNC_CMD;    /* Deact event should follow */
          }
          else
          {
            result = DSAT_ERROR;
          }
          break;

        case MMGSDI_INCORRECT_CODE:
          /* GSDI returns GSDI_PERSO_INVALID_CK even after PIN has been
             blocked, so judge from pin blocked mask if possible which cme
             error should be returned, either one of the personalization pins
             was blocked or simply an incorrect password */
          if ( DSAT_CME_NO_ERROR == 
               (error_code =
                me_choose_cme_error((dsat_cpin_required_e_type)me_ms_val->dsat_pin_blocked)) )
          {
            error_code = DSAT_CME_INCORRECT_PASSWORD;
          }
          result = dsat_send_cme_error(error_code);
          break;

        case MMGSDI_CODE_BLOCKED:
          /* In this case assume that the command response has occurred before
             the corresponding event and look to the pin required mask */
          if ( DSAT_CME_NO_ERROR ==
               (error_code =
                me_choose_cme_error((dsat_cpin_required_e_type)me_ms_val->dsat_pin_required)))
          {
            error_code = DSAT_CME_PHONE_FAILURE;
          }
          result = dsat_send_cme_error(error_code);
          break;

        default:
          DS_AT_MSG1_HIGH("Unexpected MMGSDI result code",
                     mmgsdi_result);
          result = dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
          break;
      } /* switch (gsdi_result) */
      break;

    case CPIN_MMGSDI_PERSO_ACT:
    case CPIN_MMGSDI_PERSO_PERM_DIS:
      result = (cmd_info_ptr->cmd_data == 1) ? DSAT_OK : DSAT_ERROR;
      break;
    case CPIN_MMGSDI_PERSO_GET_IND_RSP:
      {
        dsm_item_type *res_buff_ptr;

        res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
        if(cmd_info_ptr->cmd_data) {
          res_buff_ptr->used = (word)
          snprintf((char*)res_buff_ptr->data_ptr,
                         res_buff_ptr->size,
                         "%s: 1",
                         dsat_curr_cmd_var.tok_ptr->name);
        } else {
          res_buff_ptr->used = (word)
          snprintf((char*)res_buff_ptr->data_ptr,
                         res_buff_ptr->size,
                         "%s: 0",
                         dsat_curr_cmd_var.tok_ptr->name);
        }
        dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
      }
      result = DSAT_OK;
      break;
    default:
      DS_AT_MSG1_HIGH("Unexpected MMGSDI personalization cmd id: %d",
                 cmd_info_ptr->cmd_id);
      result = dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
      break;
  } /* switch (cmd_info_ptr->cmd_id) */

  /* Clear pending command flag if no further event expected */
  if ( result != DSAT_ASYNC_CMD )
  {
    dsatme_pending_mmgsdi_command[cmd_info_ptr->apps_id] = FALSE;
  }
  
  return result;
} /* cpin_process_perso_resp */

#endif /* FEATURE_MMGSDI_PERSONALIZATION */
#endif /* FEATURE_DATA_ETSI_PIN */
#ifdef FEATURE_DSAT_ETSI_MODE

/*===========================================================================

FUNCTION DSATME_EXEC_QCSIMSTAT_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes $QCSIMSTAT command, which enables/disables the URC response
  of the SIM status based on GSDI events.

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
/*ARGSUSED */
dsat_result_enum_type dsatme_exec_qcsimstat_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsatme_mmgsdi_state_ms_slot_info  *me_ms_slot_val = NULL;
  mmgsdi_slot_id_enum_type slot_id = dsat_get_current_slot_id();

  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
   Processing for TEST command 
  -----------------------------------------------------------*/
  if ((tok_ptr->op == (NA|EQ|AR)) ||
      (tok_ptr->op == (NA|EQ|QU)) ||
      (tok_ptr->op == (NA)))
  {
    result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
    
  }
 /*--------------------------------------
  Processing for a READ command
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|QU) )
  {      
    me_ms_slot_val = dsat_get_base_addr_per_slot(slot_id, FALSE);

    res_buff_ptr->used =
      (word)snprintf((char*)res_buff_ptr->data_ptr,res_buff_ptr->size,
                            "%s: %d,%s",
                            "$QCSIMSTAT",
                            (dsat_num_item_type)dsatutil_get_val(
                            DSAT_VENDOR_QCSIMSTAT_IDX,0,0,NUM_TYPE),
                            dsat_qcsim_stat[me_ms_slot_val->dsat_qcsimstat_curr]);
  }
  else
  {
    /* wrong operation */
    result = DSAT_ERROR;
  }
  return result;
}/* dsatme_exec_qcsimstat_cmd */

/*===========================================================================

FUNCTION DSATME_REPORT_QCSIMSTAT_RESULT

DESCRIPTION
  This function generates the unsolicited result code for SIM status
  command $QCSIMSTAT.  The command parameter <n> controls whether
  results are presented or suppressed.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_report_qcsimstat_result 
( 
  mmgsdi_slot_id_enum_type slot_id
)
{
  dsm_item_type * res_buff_ptr = NULL;
  dsatme_mmgsdi_state_ms_slot_info  *me_ms_slot_val = NULL;

  me_ms_slot_val = dsat_get_base_addr_per_slot(slot_id, FALSE);

  res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
  if (NULL != res_buff_ptr)
  {
    res_buff_ptr->used =
      (word) snprintf((char*)res_buff_ptr->data_ptr,res_buff_ptr->size,
                             "$QCSIMSTAT: %d %s",
                             (dsat_num_item_type)dsatutil_get_val(
                             DSAT_VENDOR_QCSIMSTAT_IDX,0,0,NUM_TYPE),
                             dsat_qcsim_stat[me_ms_slot_val->dsat_qcsimstat_curr]);

    /* Send response to TE */
    dsatcmdp_send_urc_per_slot(slot_id, res_buff_ptr, DSAT_COMPLETE_RSP);
  }
} /* dsatme_report_qcsimstat_result */

/*===========================================================================

FUNCTION  DSATME_INIT_SIM_FILENAME

DESCRIPTION
  This function is an initialization function for the sim file which is 
  used in +CPLS and +CPOL commands.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : If it is a success.
    DSAT_ERROR: ERROR.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_init_sim_filename( void )
{
  dsat_result_enum_type result = DSAT_OK;
  unsigned int          supp_plmn_selectors;
  dsat_num_item_type    cpls_val = 0;
  dsat_apps_id_e_type   apps_id = dsat_get_current_gw_apps_id(FALSE);

  cpls_val = (dsat_num_item_type)dsatutil_get_val(
                    DSATETSI_EXT_ACT_CPLS_ETSI_IDX,apps_id,0,NUM_TYPE);

  result = dsatetsime_get_supp_plmn_selectors( &supp_plmn_selectors );
  if( result == DSAT_OK )
  {
    if( (1 << cpls_val ) & supp_plmn_selectors )
    {
      result = dsatetsime_set_plmn_selector( cpls_val);
    }
    else
    {
      DS_AT_MSG0_ERROR("CPLS value not supported");
      result = DSAT_ERROR;
    }

  }
  return result;
}/* dsatme_init_sim_filename */
#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* FEATURE_MMGSDI */
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/*--------              pbm cb cmd handlers                      --------*/
/*===========================================================================

FUNCTION DSATME_CPB_PBM_READ_RECORD

DESCRIPTION
  This function is used to read record from pbm.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsatme_cpb_pbm_read_record
(
  dsat_pbm_rec_id_type        record_id,
  dsat_pbm_record_s_type     *record_ptr,
  pbm_return_type            *pbm_return_ptr
)
{
  uint32 fields_ptr_size;
  uint16 category;
  pbm_field_s_type *fields_ptr;
  int num_fields = 0;
  uint16 i;

  fields_ptr_size =(uint32)pbm_session_calculate_fields_size_from_id(record_id);
  fields_ptr = (pbm_field_s_type *)dsat_alloc_memory(PB_MAX_REC_LEN, TRUE);
  if (NULL == fields_ptr)
  {
    *pbm_return_ptr = PBM_ERROR;
    return;
  }

  memset(fields_ptr, 0, PB_MAX_REC_LEN);
  memset(record_ptr, 0, sizeof(dsat_pbm_record_s_type));
  if(fields_ptr_size == 0)
  {
    *pbm_return_ptr = pbm_session_record_read(record_id,
                                         &category,
                                         &num_fields,
                                         NULL,
                                         fields_ptr_size );

  }
  else
  {
    *pbm_return_ptr = pbm_session_record_read(record_id,
                                     &category,
                                     &num_fields,
                                     (uint8 *)fields_ptr,
                                     fields_ptr_size );
  }

  if(*pbm_return_ptr == PBM_SUCCESS)
  {
    /* Fill in the record with default values. */
    record_ptr->index = pbm_session_record_id_to_location(record_id);

    memset(record_ptr, 0, sizeof(dsat_pbm_record_s_type));
    DS_AT_MSG1_HIGH("Number of Fields in the Record Read - %d",num_fields);
    record_ptr->type = TA_OTHER;
    /* Process the information from the buffer */
    for ( i = 0; (i < num_fields) &&
                 ( (i+1) < (PB_MAX_REC_LEN/sizeof(pbm_field_s_type) - 1) ) ; i++)
    {
      switch (fields_ptr[i].field_id)
      {
        case PBM_FIELD_HARDCODED_ECC:
        case PBM_FIELD_PHONE_GENERIC:
          (void) dsatutil_memscpy((void*)record_ptr->number,PBM_NUM_SIZE+1,
                 (void*)pbm_get_field_data_ptr(fields_ptr,i),
                 fields_ptr[i].data_len );
          if((i+1 < num_fields )&&
             ((fields_ptr[i+1].field_id ==  PBM_FIELD_GSM_TONNPI)||
             (fields_ptr[i+1].field_id ==  PBM_FIELD_NUMBER_TYPE))
            )
          {
            i++;
            (void) dsatutil_memscpy((void*)&(record_ptr->type),sizeof(uint8),
                   (void*)pbm_get_field_data_ptr(fields_ptr,i),
                   fields_ptr[i].data_len );
          }
          break;
        case PBM_FIELD_NAME:
          (void) dsatutil_memscpy((void*)record_ptr->text,PBM_TEXT_SIZE_BYTES,
                 (void*)pbm_get_field_data_ptr(fields_ptr,i),
                 fields_ptr[i].data_len );
          break;

#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)
        case PBM_FIELD_ADDITIONAL_NUMBER:
          (void) dsatutil_memscpy((void*)record_ptr->adnumber,PBM_NUM_SIZE,
                 (void*)pbm_get_field_data_ptr(fields_ptr,i),
                 fields_ptr[i].data_len );
          if((i+1 < num_fields )&&
             ((fields_ptr[i+1].field_id ==  PBM_FIELD_GSM_TONNPI)||
             (fields_ptr[i+1].field_id ==  PBM_FIELD_NUMBER_TYPE))
            )
          {
            i++;
            (void) dsatutil_memscpy((void*)&(record_ptr->adtype),sizeof(uint8),
                   (void*)pbm_get_field_data_ptr(fields_ptr,i),
                   fields_ptr[i].data_len );
          }
          break;
       case PBM_FIELD_GRPNAME_ID:
         (void) dsatutil_memscpy((void*)record_ptr->group,PBM_TEXT_SIZE_BYTES,
                (void*)pbm_get_field_data_ptr(fields_ptr,i),
                 fields_ptr[i].data_len );
          break;
        case PBM_FIELD_OTHERNAME:
          (void) dsatutil_memscpy((void*)record_ptr->secondtext,PBM_TEXT_SIZE_BYTES,
                 (void*)pbm_get_field_data_ptr(fields_ptr,i),
                 fields_ptr[i].data_len );
          break;
        case PBM_FIELD_EMAIL:
          (void) dsatutil_memscpy((void*)record_ptr->email,PBM_TEXT_SIZE_BYTES,
                 (void*)pbm_get_field_data_ptr(fields_ptr,i),
                fields_ptr[i].data_len );
          break;
#endif /* defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */

        default: 
          /* Rest of the fields are ignored, since not required for CPBR */
          break;
      }
    }
    if ( record_ptr->type == 0 )
    {
      if (record_ptr->number[0] == '+')
      {
        record_ptr->type = TA_INTERNATIONAL;
      }
      else
      {
        record_ptr->type = TA_OTHER;
      }
    }
#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)
    if( (record_ptr->adnumber[0] != '\0') && ( record_ptr->adtype ==0) )
    {
        if (record_ptr->adnumber[0] == '+')
        {
          record_ptr->adtype = TA_INTERNATIONAL;
        }
        else
        {
          record_ptr->adtype = TA_OTHER;
        }
    }
#endif /*  defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */
  }

  dsatutil_free_memory(fields_ptr);
} /* dsatme_cpb_pbm_read_record */

/*===========================================================================

FUNCTION CPB_PBM_READ_HANDLER

DESCRIPTION
  This function is called during CPBR execution for reading PB records
  and sends the response to terminal.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : If reading of all records complete.
  DSAT_ASYNC_CMD   : Additional entries remain to be read.
  DSAT_CMD_ERR_RSP : If there is a CME ERROR.
  DSAT_ERROR       : Read action fails due to non-CME errors.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type cpb_pbm_read_handler
(
  void
)
{
  /* Remember if the the record is at the beginning, middle, or end
     of the data */
  static dsat_rsp_enum_type async_resp = DSAT_START_OF_RSP;
  dsat_result_enum_type result = DSAT_OK;
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);

  /* For each index from current to last perform pbm_record_read.*/
  for ( ;pbm_ss_val->cpbr_index.current <= pbm_ss_val->cpbr_index.last; ++pbm_ss_val->cpbr_index.current )
  {
    dsat_pbm_rec_id_type   rec_id;
    pbm_return_T pbm_return;
    dsat_pbm_record_s_type *record_ptr;
    record_ptr = (dsat_pbm_record_s_type *)dsat_alloc_memory(sizeof(dsat_pbm_record_s_type),FALSE);
    dsat_pb_id.device_type = me_cpbs_entry.device;
    rec_id = pbm_session_location_to_record_id(dsat_pb_id,pbm_ss_val->cpbr_index.current);
    dsatme_cpb_pbm_read_record(rec_id, record_ptr, &pbm_return);

    if ( pbm_return == PBM_SUCCESS )
    {
      result = cpb_pbm_read_handler_normal_status(record_ptr, &async_resp);
    }
    /* If no record found until last index, send error response 
       else return DSAT_OK */
    else if ( pbm_return == PBM_ERROR_NOT_FOUND )
    {
      if ( pbm_ss_val->cpbr_index.current == pbm_ss_val->cpbr_index.last )
      {
        result = (async_resp == DSAT_START_OF_RSP)?
          dsat_send_cme_error(DSAT_CME_NOT_FOUND):DSAT_OK;
      }
    }
    else
    {
      result = cpb_pbm_read_handler_error_status(pbm_return);
      break;
    }

    /* Check for SIO TX Water mark high case : This is needed because +CPBR 
	   command can pump huge data to SIO for transmitting out */
    if( result == DSAT_ASYNC_CMD )
    {
      /* Additional entries remain to be read */
      if ( dsati_tx_wm_high )
      {
        /* High watermark has been hit on dsat to sio queue: 
           set up handler to re-enter PB entry reading once queue hits low
           watermark */
        dsati_to_sio_low_handler = cpb_pbm_reenter_read_handler;
        dsatutil_free_memory((void *)record_ptr);
        return result;
      }

      /* If DSM large item free count has gone below a threshold quantity... */
      if ( DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL) < DSAT_FREE_ITEM_TRIGGER )
      {
        dsat_timer_s_type *t_ptr = &dsatutil_timer_table[DSAT_TIMER_FREE_ITEM];

        /* Set timer expiry handler function */
        dsati_handle_free_item_timer_expiry = cpb_pbm_reenter_read_handler;

        /* Set timer and await its expiration */
        (void) rex_set_timer( t_ptr->timer_ptr, t_ptr->duration );
        t_ptr->is_running = TRUE;
        dsatutil_free_memory((void *)record_ptr);
        return DSAT_ASYNC_CMD;
      }
    }/* (result == DSAT_ASYNC_CMD) */
    dsatutil_free_memory((void *)record_ptr);
  /* continue reading next index */
  }/* for (cpbr_index.current .. )*/

  /* +cpbr finishes. reset static variable async_rsp */
  async_resp = DSAT_START_OF_RSP;

  return result;
} /* cpb_pbm_read_handler */

/*===========================================================================

FUNCTION CPB_PBM_REENTER_READ_HANDLER

DESCRIPTION
  Local function that is used to reenter reading PB entries once DSAT to SIO
  queue has hit low watermark (or) DSM large item threshold timer expiry.

DEPENDENCIES
  none

RETURN VALUE
  DSAT_OK          : If reading of all records complete.
  DSAT_ASYNC_CMD   : Additional entries remain to be read.
  DSAT_CMD_ERR_RSP : If there is a CME ERROR.
  DSAT_ERROR       : Read action fails due to non-CME errors.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_reenter_read_handler
(
  ds_cmd_type* cmd_ptr
)
{
  dsat_result_enum_type result;
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);
  /* Reset handler function pointers */
  /* Handler function for low event, now not neccesary */
  dsati_to_sio_low_handler = NULL;
  dsati_handle_free_item_timer_expiry = NULL;

  /* continue reading next index */
  ++pbm_ss_val->cpbr_index.current;

  DS_AT_MSG1_HIGH("cpb_pbm_cb_reenter_read_handler: reading record from %d",
          pbm_ss_val->cpbr_index.current);

  result = cpb_pbm_read_handler ();

  return result;
}/* cpb_pbm_reenter_read_handler */

/*===========================================================================

FUNCTION CPB_PBM_READ_HANDLER_NORMAL_STATUS

DESCRIPTION
  This function is used to process the normal status for pbm_record_read

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : If reached end of reading records.
  DSAT_ASYNC_CMD   : Additional entries remain to be read.
  DSAT_CMD_ERR_RSP : If there is a CME ERROR.

SIDE EFFECTS
  None 
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_read_handler_normal_status
(
  dsat_pbm_record_s_type  *record_ptr,
  dsat_rsp_enum_type * rsp_ptr      /* if this record is at the beginning, 
                                       middle, or end of all the records
                                       to be read                         */
)
{
  dsm_item_type * raw_rsp_ptr = NULL;
  char *name = NULL;
#ifdef FEATURE_DSAT_EXTENDED_CMD
  uint16 name_size = PBM_TEXT_SIZE_CHARS * UCS2_CHAR_LEN;
#else
  uint16 name_size = PBM_TEXT_SIZE_CHARS;
#endif /* FEATURE_DSAT_EXTENDED_CMD */
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  dsat_num_item_type cscs_val ;
#ifndef FEATURE_DSAT_EXTENDED_CMD
  uint8 i = 0;
#endif /* FEATURE_DSAT_EXTENDED_CMD */
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;
  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);

  if (record_ptr != NULL)
  {
    name = (char *)dsat_alloc_memory(name_size, FALSE);

#ifdef FEATURE_DSAT_EXTENDED_CMD
    /* Determine if the contents can be represented as ASCII, or if we need
       to display in RAW mode */
    uint8 coding_mode = DSATME_ENCODING_ASCII;
    uint16 *ucs2_char = (uint16 *) record_ptr->text;
    uint16 i;
    char *conv_ptr = name;

    memset(name, '\0', name_size);
    for (i = 0; i < (PBM_MAX_TEXT_LENGTH / sizeof(uint16)) && ucs2_char[i]; i++)
    {
      /* Basic ASCII range is 0 to 0x7F - if any character is not in that range
         then we display using the RAW mode */
      if ( ucs2_char[i] & 0xFF80 )
      {
        coding_mode = DSATME_ENCODING_RAW;
        /* Always display w/coding mode 1 (full UCS2 chars) because that's what 
           we get from PBM */
        name[0] = '8';
        name[1] = '0';
        conv_ptr = &name[2];
        break;
      }
    }
#else
    memset(name, '\0', name_size);
#endif /* FEATURE_DSAT_EXTENDED_CMD */
#ifdef FEATURE_DSAT_EXTENDED_CMD
    if( NULL == dsatutil_convert_chset(
                  record_ptr->text, 
                  ALPHA_PBM_UCS2,
                  conv_ptr,
                  ( (coding_mode == DSATME_ENCODING_ASCII)?
                     ALPHA_IRA: ALPHA_UCS2 ),
                  name_size,
                  TRUE) )
#else
    memset(name, '\0', name_size);
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    if( NULL == dsatutil_convert_chset( record_ptr->text, 
                                        ALPHA_PBM_UCS2,
                                        name,
                                        (dsati_chset_type)cscs_val,
                                        name_size,
                                        TRUE)
                                      )
#endif /* FEATURE_DSAT_EXTENDED_CMD */
    {
      dsatutil_free_memory(name);
      return dsat_send_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT);
    }
    /* Convert HARDPAUSE/TIMEPAUSE character */
    while((i < PBM_NUM_SIZE) && (record_ptr->number[i] != '\0'))
    {
      if(record_ptr->number[i] == 'T')
      {
        record_ptr->number[i] = 'p';
      }
      i++;
    }

    raw_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
#ifdef FEATURE_DSAT_EXTENDED_CMD
    raw_rsp_ptr->used = (word) snprintf((char *)raw_rsp_ptr->data_ptr,
                                     raw_rsp_ptr->size,
                                     "%s: %d,\"%s\",%d,\"%s\",%d",
                                     pbm_ss_val->cpbr_index.display_name,
                                     pbm_ss_val->cpbr_index.current,
                                     record_ptr->number,
                                     record_ptr->type,
                                     name,
                                     coding_mode
                                     );
#else
    raw_rsp_ptr->used = (word) snprintf((char *)raw_rsp_ptr->data_ptr,
                                     raw_rsp_ptr->size,
                                     "%s: %d,\"%s\",%d,\"%s\"",
                                     pbm_ss_val->cpbr_index.display_name,
                                     pbm_ss_val->cpbr_index.current,
                                     record_ptr->number,
                                     record_ptr->type,
                                     name);
#endif /* FEATURE_DSAT_EXTENDED_CMD */
#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)
    if(record_ptr->group[0] != '\0')
    {
      /* ADN phonebook have group ID.and Group Phonebook
       * have group name corresponding to group id .here   
       * we are mapping group id received from ADN to group 
       * name . 
      */
      char *temp_name = NULL;
      uint16 temp_name_size = PBM_TEXT_SIZE_CHARS * UCS2_CHAR_LEN;
      dsat_num_item_type count   = 0;
      dsat_num_item_type grp_id  = 0;
      dsat_num_item_type grp_len = 0;

      while(( count < PBM_TEXT_SIZE_BYTES) && (record_ptr->group[count] != 0))
      {
        grp_id = grp_id * 10 + (dsat_num_item_type)record_ptr->group[count];
        count++;
      }
      dsatme_find_grp_name(name,sizeof(name) ,&grp_len,grp_id);
      if(grp_len != 0 && grp_id != 0)
      {
      
        DS_AT_MSG2_HIGH("Group id is %d length %d",grp_id,grp_len);
        temp_name = (char *)dsat_alloc_memory(temp_name_size, FALSE);
        memset(temp_name, '\0', temp_name_size);
        cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
        if( NULL == dsatutil_convert_chset( name, 
                                          ALPHA_PBM_UCS2,
                                            temp_name,
                                          (dsati_chset_type)cscs_val,
                                          name_size,
                                          TRUE)
                                        )
        {
          dsatutil_free_memory(temp_name);
          dsatutil_free_memory(name);
          return dsat_send_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT);
        }
        if((grp_len + raw_rsp_ptr->used) > raw_rsp_ptr->size)
        {
          DS_AT_MSG0_ERROR("Record length Exceeded \n");
        }
        else
        {
          raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                             raw_rsp_ptr->size,
                                             "%s,\"%s\"",
                                             raw_rsp_ptr->data_ptr,
                                             temp_name);
        }
        dsatutil_free_memory(temp_name);
      }
      else
      {
        DS_AT_MSG2_ERROR("Invalid Group id is %d length %d",grp_id,grp_len);
      }
    }
    else
    {
        raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                           raw_rsp_ptr->size,
                                           "%s,",
                                           raw_rsp_ptr->data_ptr);

    }
    i = 0;
    /* Convert HARDPAUSE/TIMEPAUSE character */
    while((i < PBM_NUM_SIZE) && (record_ptr->adnumber[i] != '\0'))
    {
      if(record_ptr->adnumber[i] == 'T')
      {
        record_ptr->adnumber[i] = 'p';
      }
      i++;
    }
    raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                       raw_rsp_ptr->size,
                                       "%s,\"%s\",%d",
                                       raw_rsp_ptr->data_ptr,
                                       record_ptr->adnumber,
                                       record_ptr->adtype);  
    if(record_ptr->secondtext[0] != '\0')
    {
      memset(name, '\0', name_size);
      cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
      if( NULL == dsatutil_convert_chset( record_ptr->secondtext, 
                                          ALPHA_PBM_UCS2,
                                          name,
                                          (dsati_chset_type)cscs_val,
                                          name_size,
                                          TRUE)
                                        )
      {
        dsatutil_free_memory(name);
        return dsat_send_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT);
      }
      if((strlen(name) + raw_rsp_ptr->used) > raw_rsp_ptr->size)
      {
        DS_AT_MSG0_ERROR("Record length Exceeded \n");
      }
      else
      {
        raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                         raw_rsp_ptr->size,
                                         "%s,\"%s\"",
                                         raw_rsp_ptr->data_ptr,
                                         name);
      }
    }
    else
    {
        raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                           raw_rsp_ptr->size,
                                           "%s,",
                                           raw_rsp_ptr->data_ptr);

    }

    if(record_ptr->email[0] != '\0')
    {
      memset(name, '\0', name_size);
      cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
      if( NULL == dsatutil_convert_chset( record_ptr->email, 
                                          ALPHA_PBM_UCS2,
                                          name,
                                          (dsati_chset_type)cscs_val,
                                          name_size,
                                          TRUE)
                                        )
      {
        dsatutil_free_memory(name);
        return dsat_send_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT);
      }
      if((strlen(name) + raw_rsp_ptr->used) > raw_rsp_ptr->size)
      {
        DS_AT_MSG0_ERROR("Record length Exceeded \n");
      }
      else
      {
        raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                         raw_rsp_ptr->size,
                                         "%s,\"%s\"",
                                         raw_rsp_ptr->data_ptr,
                                         name);
      }
    }
    else
    {
        raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                           raw_rsp_ptr->size,
                                           "%s,",
                                           raw_rsp_ptr->data_ptr);
    }
#endif /* defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */
    /* send the async response to terminal */
    raw_rsp_ptr->used +=(word)snprintf((char *)(&raw_rsp_ptr->data_ptr[raw_rsp_ptr->used]),
                                             (raw_rsp_ptr->size - raw_rsp_ptr->used),
                                             "\n");
    dsatcmdp_handle_async_cmd_rsp(raw_rsp_ptr, *rsp_ptr);

    /* note that we have sent out record */
    if(*rsp_ptr == DSAT_START_OF_RSP)
    {
      *rsp_ptr = DSAT_INTERMEDIATE_RSP;
    }

    dsatutil_free_memory(name);
  }
  else 
  {
    /* the record is empty */
    /* This block only executed if read of a single entry results in
       NULL record. If multi-entry read returns a NULL record it is
       simply ignored */
    if(pbm_ss_val->cpbr_index.last == pbm_ss_val->cpbr_index.current &&
       *rsp_ptr == DSAT_START_OF_RSP)
    {
      /* no record is read */
      return dsat_send_cme_error(DSAT_CME_NOT_FOUND);
    }
  }

  /* Check for end of reading records */
  if(pbm_ss_val->cpbr_index.last == pbm_ss_val->cpbr_index.current)
  {
    result = DSAT_OK;
  }

  return result;

} /* cpb_pbm_read_handler_normal_status */

/*===========================================================================

FUNCTION CPB_PBM_FIND_HANDLER

DESCRIPTION
  This function is called by +CPBF to return the results of pbm_find
  function call.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : find action succeeds
  DSAT_ASYNC_CMD   : continue searching for next number
  DSAT_CMD_ERR_RSP : if there is a CME ERROR
  DSAT_ERROR       : find action fails due to non-cme errors

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type cpb_pbm_find_handler
(
  void
)
{
  /* Remember if the the record is the beginning, middle, or end
     of the data */
  static dsat_rsp_enum_type async_rsp = DSAT_START_OF_RSP;

  dsat_result_enum_type  result = DSAT_OK;
  pbm_return_type        pbm_result;
  dsat_pbm_record_s_type *record_ptr;
  dsat_pbm_rec_id_type   rec_id; /* record id */
  uint16                loc_id; /* location id */
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;
  record_ptr = (dsat_pbm_record_s_type *)dsat_alloc_memory(sizeof(dsat_pbm_record_s_type),FALSE);
  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);

  /* Get the next record from PBM which matches the search criteria */
  while ( (pbm_result = pbm_session_enum_next_rec_id_ext(&rec_id, pbm_ss_val->dsat_pbm_handler))== PBM_SUCCESS )
  {
    loc_id = pbm_session_record_id_to_location(rec_id);
    dsatme_cpb_pbm_read_record(rec_id, record_ptr, &pbm_result);

    /* success in reading the record */
    if (pbm_result == PBM_SUCCESS )
    {
      result = cpb_pbm_find_handler_status_success(record_ptr,
                                                   loc_id,
                                                   &async_rsp
                                                  );

      /* Check for SIO TX Water mark high case : This is added as a
         precautionary measure.

         Some insights:
         If search takes some definite amount of time, it is very unlikely
         that we exceed AT->SIO high watermark count.
         In real use-case it is unlikely that every entry in phone 
         book matches with the serach criteria and tirgger huge data to SIO 
         for transmitting out. */
	     
      /* If additional entries remain to be read during fine, we re-enter 
         through cpb_pbm_reenter_find_handler, which inturn calls this 
         function */
      if ( dsati_tx_wm_high )
      {
        /* High watermark has been hit on dsat to sio queue: 
           set up handler to re-enter PB entry reading once queue hits low
           watermark */
        dsati_to_sio_low_handler = cpb_pbm_reenter_find_handler;
        dsatutil_free_memory((void *)record_ptr);
        return result;
      }

      /* If DSM large item free count has gone below a threshold quantity... */
      if ( DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL) < DSAT_FREE_ITEM_TRIGGER )
      {
        dsat_timer_s_type *t_ptr = &dsatutil_timer_table[DSAT_TIMER_FREE_ITEM];

        /* Set timer expiry handler function */
        dsati_handle_free_item_timer_expiry = cpb_pbm_reenter_find_handler;

        /* Set timer and await its expiration */
        (void) rex_set_timer( t_ptr->timer_ptr, t_ptr->duration );
        t_ptr->is_running = TRUE;
        dsatutil_free_memory((void *)record_ptr);
        return DSAT_ASYNC_CMD;
      }
    } /* if (pbm_result == PBM_SUCCESS ) */
  } /* while (pbm_result == PBM_SUCCESS) */

  /* Reached end of search */
  if (pbm_result == PBM_EOF)
  {
    result = cpb_pbm_find_handler_status_eof(async_rsp);
  }
  else /* All remaining error cases */
  {
    result = cpb_pbm_find_handler_status_fail(pbm_result);
  }

  if (result != DSAT_ASYNC_CMD)
  {
    /* Reset the static variable as we are done with finding entries and 
	   reporting. */
    async_rsp = DSAT_START_OF_RSP;
  }
  dsatutil_free_memory((void *)record_ptr);
  return result;
} /* cpb_pbm_find_handler */

/*===========================================================================

FUNCTION CPB_PBM_REENTER_FIND_HANDLER

DESCRIPTION
  Local function that is used to reenter finding PB entries once DSAT to SIO
  queue has hit low watermark.

DEPENDENCIES
  none

RETURN VALUE
  DSAT_OK          : Reached end of finding entries.
  DSAT_ASYNC_CMD   : Continue searching for next number.
  DSAT_CMD_ERR_RSP : If there is a CME ERROR.
  DSAT_ERROR       : Find action failed due to non-cme errors.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_reenter_find_handler
(
  ds_cmd_type* cmd_ptr
)
{
  dsat_result_enum_type result;

  /* Reset handler function pointers */
  /* Handler function for low event, now not neccesary */
  dsati_to_sio_low_handler = NULL;
  dsati_handle_free_item_timer_expiry = NULL;

  DS_AT_MSG0_HIGH("cpb_pbm_cb_reenter_find_handler: start finding again!");

  /* Continue finding next record */
  result = cpb_pbm_find_handler();

  return result;
}/* cpb_pbm_reenter_find_handler */

/*===========================================================================

FUNCTION CPB_PBM_FIND_HANDLER_STATUS_SUCCESS

DESCRIPTION
  This function is called by cpb_pbm_find_handler when finding a record
  succeeds. This function is used to report the output to terminal.
  
DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : Continue searching for next number.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_success
(
  dsat_pbm_record_s_type  * record_ptr,/* a record found */
  uint16               loc_id,         /* location id */
  dsat_rsp_enum_type * rsp_ptr         /* if the record is the beginning, 
                                       middle, or end of all the record found */
)
{
  dsm_item_type * raw_rsp_ptr = NULL;
  char name[PBM_TEXT_SIZE_CHARS];
  dsat_num_item_type cscs_val ;
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);
 
  if (record_ptr != NULL && record_ptr->number[0] != '\0')
  {
    memset(name, '\0', PBM_TEXT_SIZE_CHARS);
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    if( NULL == dsatutil_convert_chset( record_ptr->text, 
                                       ALPHA_PBM_UCS2,
                                       name,
                                       (dsati_chset_type)cscs_val,
                                       sizeof(name),
                                       TRUE))
    {
      return dsat_send_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT);
    }

    raw_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

      raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                       raw_rsp_ptr->size,
                                       "%s: %d,\"%s\",%d,\"%s\"",
                                       pbm_ss_val->cpbf_index.display_name,
                                       loc_id,
                                       record_ptr->number,
                                       record_ptr->type,
                                       name);

#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)
    if(record_ptr->group[0] != '\0')
    {
      memset(name, '\0', sizeof(name));
      cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
      if( NULL == dsatutil_convert_chset( record_ptr->group, 
                                          ALPHA_PBM_UCS2,
                                          name,
                                          (dsati_chset_type)cscs_val,
                                          sizeof(name),
                                          TRUE))
      {
        return dsat_send_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT);
      }
      if((strlen(name) + raw_rsp_ptr->used) > raw_rsp_ptr->size)
      {
        DS_AT_MSG0_ERROR("Record length Exceeded \n");
      }
      else
      {
        raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                                raw_rsp_ptr->size,
                                                "%s,\"%s\"",
                                                raw_rsp_ptr->data_ptr,
                                                name);
      }
    }
    if((strlen(record_ptr->adnumber) + raw_rsp_ptr->used) > raw_rsp_ptr->size)
    {
      DS_AT_MSG0_ERROR("Record length Exceeded \n");
    }
    else
    {
      raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                             raw_rsp_ptr->size,
                                             "%s,\"%s\",%d",
                                             raw_rsp_ptr->data_ptr,
                                             record_ptr->adnumber,
                                             record_ptr->adtype);
    }
    if(record_ptr->secondtext[0] != '\0')
    {
      memset(name, '\0', sizeof(name));
      cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
      if( NULL == dsatutil_convert_chset( record_ptr->secondtext, 
                                          ALPHA_PBM_UCS2,
                                          name,
                                          (dsati_chset_type)cscs_val,
                                          sizeof(name),
                                          TRUE))
      {
        return dsat_send_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT);
      }
      if((strlen(name) + raw_rsp_ptr->used) > raw_rsp_ptr->size)
      {
        DS_AT_MSG0_ERROR("Record length Exceeded \n");
      }
      else
      {
        raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                                raw_rsp_ptr->size,
                                                "%s,\"%s\"",
                                                raw_rsp_ptr->data_ptr,
                                                name);
      }
    }
    if(record_ptr->email[0] != '\0')
    {
      memset(name, '\0', sizeof(name));
      cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
      if( NULL == dsatutil_convert_chset( record_ptr->email, 
                                          ALPHA_PBM_UCS2,
                                          name,
                                          (dsati_chset_type)cscs_val,
                                           sizeof(name),
                                           TRUE))
      {
        return dsat_send_cme_error(DSAT_CME_INVALID_CHAR_IN_TEXT);
      }
      if((strlen(name) + raw_rsp_ptr->used) > raw_rsp_ptr->size)
      {
        DS_AT_MSG0_ERROR("Record length Exceeded \n");
      }
      else
      {
        raw_rsp_ptr->used=(word)snprintf((char *)raw_rsp_ptr->data_ptr,
                                                raw_rsp_ptr->size,
                                                "%s,\"%s\"",
                                                raw_rsp_ptr->data_ptr,
                                                name);
      }
    }
#endif /* defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */
    /* Send the async response to terminal */
    raw_rsp_ptr->used +=(word)snprintf(
                             (char *)(&raw_rsp_ptr->data_ptr[raw_rsp_ptr->used]),
                             (raw_rsp_ptr->size - raw_rsp_ptr->used),
                              "\n");
    dsatcmdp_handle_async_cmd_rsp(raw_rsp_ptr, *rsp_ptr);
    *rsp_ptr = DSAT_INTERMEDIATE_RSP;
  }

  return DSAT_ASYNC_CMD;
} /* cpb_pbm_find_handler_status_success */
/*===========================================================================

FUNCTION DSATME_FIND_GRP_ID

DESCRIPTION
   this function take group name as argument and find it into group phonebook.if 
   group doen't exist then  create new entry into group phonebook and calculate group id .
  
DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK    - valid group id  
  DSAT_ERROR - else

SIDE EFFECTS
  None
===========================================================================*/

LOCAL dsat_result_enum_type dsatme_find_grp_id
(
  char                *group_name ,
  dsat_num_item_type   group_length,
  dsat_num_item_type  *grp_id
)
{
  pbm_record_id_type   rec_id            = 0;
  pbm_return_type      pbm_result        = PBM_SUCCESS;
  uint8               *fields_ptr        = NULL; 
  uint32               fields_ptr_size   = 0;
  pbm_field_s_type    *ptyped_fields_ptr = NULL;
  pbm_phonebook_type   pb_id;
  uint8 dsat_handler;
  if(NULL == group_name || 0 == group_length )
  {
    DS_AT_MSG1_HIGH("Group Name is NULL Length %d",group_length);
    return DSAT_ERROR;
  }
  (void) dsatutil_memscpy((void*)&pb_id,
          sizeof(dsat_pb_id),(void*)&dsat_pb_id,sizeof(dsat_pb_id));
  pb_id.device_type = PBM_GRPNAME;

  pbm_result =  pbm_session_enum_rec_init_ext( 
                            pb_id,
                           (uint16)PBM_CAT_GRPNAME,
                            PBM_FIELD_NAME,
                           (uint8 *)group_name,
                            group_length,
                            PBM_SEARCHTYPE_EXACT|PBM_STRTYPE_UCS2,
                            &dsat_handler);


  if (pbm_result == PBM_SUCCESS )
  {
    
    pbm_result = pbm_session_enum_next_rec_id_ext(&rec_id, dsat_handler);
    if(pbm_result == PBM_SUCCESS)
    {
       *grp_id = pbm_session_record_id_to_location(rec_id);
    }
    DS_AT_MSG3_HIGH("Group Found Result rec id %d grp id %d %d",pbm_result,rec_id,*grp_id);
    pbm_session_enum_rec_init_ext_free_handle(dsat_handler);
  }
  if( pbm_result != PBM_SUCCESS)
  {
   /* Group Doesn't present in group phonebook so create new one */
   
    pbm_allocate_fields(&fields_ptr, &fields_ptr_size, 1,(const uint32 *)&group_length);
    ptyped_fields_ptr = (pbm_field_s_type *) fields_ptr;
    pbm_fill_field(&ptyped_fields_ptr[0],
                         PBM_FIELD_NAME,
                         PBM_FT_UCS2,
                        (byte *)group_name, 
                        (uint16) group_length);
    
  rec_id = PBM_SESSION_GET_FIRST_AVAILABLE;
  pbm_result = pbm_session_record_write(pb_id, 
                                       &rec_id,
                                       (uint16)PBM_CAT_GRPNAME,
                                        1, 
                                        fields_ptr, 
                                        fields_ptr_size,
                                        me_pbm_write_cb, 
                                        NULL);
  *grp_id = pbm_session_record_id_to_location(rec_id);
  
  DS_AT_MSG3_HIGH("Group Created Result rec id %d grp id %d %d",pbm_result,rec_id,*grp_id);
  }
  if( PBM_SUCCESS == pbm_result)
  {
    return DSAT_OK;
  }
  else
  {
    return DSAT_ERROR;
  }
}/* dsatme_find_grp_id */
/*===========================================================================

FUNCTION DSATME_FIND_GRP_NAME

DESCRIPTION
   this function take group id and find corresponding group name in group 
   phonebook  .
  
DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK    - Group exist 
  DSAT_ERROR - else

SIDE EFFECTS
  None
===========================================================================*/

LOCAL dsat_result_enum_type dsatme_find_grp_name
(
char               *group_name ,
dsat_num_item_type  buf_length,
dsat_num_item_type *group_length,
dsat_num_item_type  grp_id
)
{
  uint32               record_id        = 0;
  pbm_return_type      pbm_result       = PBM_SUCCESS;
  pbm_field_s_type     *fields_ptr      = NULL; 
  uint32               fields_ptr_size  = 0;
  uint16               category         = PBM_CAT_MAX;
  dsat_num_item_type   num_fields = 0,i = 0;
  pbm_phonebook_type   pb_id ;

  (void) dsatutil_memscpy((void*)&pb_id,
          sizeof(dsat_pb_id),(void*)&dsat_pb_id,sizeof(dsat_pb_id));
  pb_id.device_type = PBM_GRPNAME;
  record_id = pbm_session_location_to_record_id(pb_id,grp_id);
  fields_ptr_size =(uint32)pbm_session_calculate_fields_size_from_id(record_id);
  

  DS_AT_MSG3_HIGH("Rec id %d Grp id %d Fields ptr size %d",record_id,grp_id,fields_ptr_size);
  fields_ptr = (pbm_field_s_type *)dsat_alloc_memory(PB_MAX_REC_LEN, FALSE);
  memset(fields_ptr, 0, PB_MAX_REC_LEN);
  
   pbm_result = pbm_session_record_read(record_id,
                                     &category,
                              (int *)&num_fields,
                                     (uint8 *)fields_ptr,
                                     fields_ptr_size );

  if( PBM_SUCCESS == pbm_result)
  {
    for ( i = 0; i < num_fields ; i++)
    {
      if(fields_ptr[i].field_id == PBM_FIELD_NAME)
      {

        (void) dsatutil_memscpy((void*)group_name,buf_length,
            (void*)pbm_get_field_data_ptr(fields_ptr,i),fields_ptr[i].data_len);
        *group_length =  fields_ptr[i].data_len; 
      }
    }
  }
  DS_AT_MSG2_HIGH("*group_length %d PBM result %d",*group_length,pbm_result);

  dsatutil_free_memory(fields_ptr);

  return DSAT_OK;
}/* dsatme_find_grp_name */


/*===========================================================================

FUNCTION CPB_PBM_FIND_HANDLER_STATUS_EOF

DESCRIPTION
  This function is called by cpb_pbm_find_handler when the
  pbm_find_name_cb function's cmd status is PBM_EOF.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : Search completes.
  DSAT_CMD_ERR_RSP : Found no entries.
  
SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_eof
(
  dsat_rsp_enum_type async_rsp
)
{
  dsat_result_enum_type result;

  /* If there is nothing in the result buffer. */
  /* Instead of output nothing, we do CME error */
  if (async_rsp == DSAT_START_OF_RSP)
  {
    result = dsat_send_cme_error(DSAT_CME_NOT_FOUND);
  }
  else
  {
    result = DSAT_OK;
  }

  return result;
} /* cpb_pbm_find_handler_status_eof */

/*===========================================================================

FUNCTION CPB_PBM_FIND_HANDLER_STATUS_FAIL

DESCRIPTION
  This function is called by cpb_pbm_find_handler when the
  pbm_find_name_cb function's cmd status indicate PBM_ERRORs.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_CMD_ERR_RSP : If there is a CME ERROR
  DSAT_ERROR       : For all unexpected PBM errors.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_find_handler_status_fail
(
  pbm_return_T status
)
{
  dsat_result_enum_type result;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  me_ms_val = dsat_get_base_addr_current_session();

  switch (status)
  {
    case PBM_ERROR:
    case PBM_ERROR_NO_SIM:
    case PBM_ERROR_PH_SIM_REQ:
    case PBM_ERROR_SIM_PIN_REQ:
    case PBM_ERROR_SIM_PUC_REQ:
    case PBM_ERROR_SIM_FAIL:
    case PBM_ERROR_NOT_FOUND:
    case PBM_ERROR_TEXT_TOO_LONG:
    case PBM_ERROR_INVALID_CHAR:
      /* mobile equipment error encountered */
      cpb_process_pbm_error(status, NULL);
      result = DSAT_CMD_ERR_RSP;
      break;

    case PBM_ERROR_PIN2_REQ:
    case PBM_ERROR_PUC2_REQ:
        /* mobile equipment error encountered */
      cpb_process_pbm_error(status, NULL);
      result = DSAT_CMD_ERR_RSP;
      /* If attempted verification of pin2 failed due to incorrect pin
       show this result in next +CPIN command response */

      me_ms_val->dsat_mask_pin2_state = FALSE;
      break;

    case PBM_ERROR_INDEX_INVALID:
    case PBM_ERROR_MEM_FULL:
    case PBM_ERROR_NUMBER_TOO_LONG:
    case PBM_ERROR_NOT_READY:
    case PBM_ERROR_NOT_AVAILABLE:
      /* pbm should not return this result at this time */
      DS_AT_MSG1_ERROR("Wrong pbm_result: %d", status);
      result = DSAT_ERROR;
      break;
        
    default:
      /* appease lint */
      result = DSAT_ERROR;
      DS_AT_MSG1_ERROR("Wrong pbm_result: %d", status);
  }

  return result;
} /* cpb_pbm_find_handler_status_fail */


/*===========================================================================

FUNCTION CPB_PBM_CB_FILE_INFO_HANDLER

DESCRIPTION
  This function processes the file_info returned from pbm.

  If the pbm_file_info command succeeds, update the content of 
  val_ptr (dsat_cpbs_val) of cpbs command and save the storage info in 
  current cpbs entry, me_cpbs_entry.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_cb_file_info_handler
(
  ds_at_pbm_info_type *pbm_info /* PBM info pointer */
)

{
  dsat_result_enum_type        result;
  ds_at_pbm_file_info_type    *info_ptr   = NULL;
  dsatme_pbm_state_ss_info    *pbm_ss_val = NULL;
  dsat_num_item_type           enum_val;
  pbm_return_type              status;

  /* command pbm_file_info status */
  status = pbm_info->status;
    /* file info */
  info_ptr = &pbm_info->cmd_info.file_info; 
  
  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);
  enum_val = (dsat_num_item_type) DSATETSI_EXT_ACT_CPBS_ETSI_IDX;
  me_cpbs_entry.pbm_status = status;

  /*-------- the command succeeds --------*/
  if(status == PBM_SUCCESS)
  {
    int index;
    byte storage[CPBS_VAL_LEN + 1];
    boolean find_index = FALSE;
    
    /* get the AT storage by pbm_device type */
    if(get_pb_storage(info_ptr->pbm_device, storage))
    {
      /* update the cpbs parameter */
      for (index = 0; dsat_cpbs_storage_valstr[index][0] != '\0'; index++)
      {
        if(!strcmp( (char *) dsat_cpbs_storage_valstr[index], 
                    (char *) storage))
        {
          find_index = TRUE;
          break;
        }
      }
      if (find_index)
      {
        DSATUTIL_SET_VAL(enum_val,0,0,0,index,NUM_TYPE)
        /* update the current storage info for +cpbs, +cpbr, +cpbf, and +cpbw */
        me_cpbs_entry.device = info_ptr->pbm_device;
        (void) strlcpy((char *)me_cpbs_entry.storage,
                           (char *)storage,
                           sizeof(me_cpbs_entry.storage));
        
        me_cpbs_entry.total = (int16) info_ptr->total;
        me_cpbs_entry.text_len = (int16) MIN(info_ptr->text_len,PBM_MAX_TEXT_LENGTH);

        /* if the +cpbs command is read command, output the storage info */
        if (pbm_ss_val->me_cpbs_params.is_read)
        {
          cpb_pbm_cb_file_info_handler_output_info(info_ptr->used);
        }
        result = DSAT_OK;
      }
      else
      {
        /* could not find a storage in supported at storage list for
           the returned pbm_device                                      */
        DS_AT_MSG1_ERROR("pbm_device %d not supported",info_ptr->pbm_device);
        result = DSAT_ERROR;
      }
    }
    else
    {
      /* could not map the pbm_device to a AT storage */
      DS_AT_MSG1_ERROR("No storage for pbm_device %d",info_ptr->pbm_device);
      result = DSAT_ERROR;
    }

  } /* if(status == PBM_SUCCESS) */

  /* the command fails. the storage did not get set correctly */
  else 
  {
#ifdef FEATURE_DSAT_GOBI_MAINLINE
    /* if the +cpbs command is read command, output the storage info */
    if (pbm_ss_val->me_cpbs_params.is_read)
    {
      me_cpbs_entry.total = -1;
      cpb_pbm_cb_file_info_handler_output_info(-1);
    }
#endif /* FEATURE_DSAT_GOBI_MAINLINE */
    /* return error */
    result = cpb_pbm_cb_file_info_handler_error_status(status);
  }
  cb_expected = CB_NONE;
  return result;
} /* cpb_pbm_cb_file_info_handler */


/*===========================================================================

FUNCTION CPB_PBM_CB_FILE_INFO_HANDLER_OUTPUT_INFO

DESCRIPTION
  This function is called by me_pbm_file_info_cb to output the 
  storage info if the +cpbs command is read command.

  This function makes me_pbm_file_info_cb shorter.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL void cpb_pbm_cb_file_info_handler_output_info(int used)
{
  dsm_item_type * raw_rsp_ptr;
  char * buff_ptr;
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);

  raw_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
  buff_ptr = (char *) raw_rsp_ptr->data_ptr;
        
  /* if current storage's used or total value does not make sense,
     don't output them */
  if (used < 0 || me_cpbs_entry.total < 1) 
  {
    /* When the total and used is not supported, they should be -1 */
    ASSERT(me_cpbs_entry.total == -1);

    raw_rsp_ptr->used = (word) snprintf(buff_ptr,
                                              raw_rsp_ptr->size,
                                              "%s: \"%s\"",
                                              pbm_ss_val->me_cpbs_params.display_name,
                                              me_cpbs_entry.storage);
  }
  else
  {
    raw_rsp_ptr->used = (word) snprintf(buff_ptr,
                                       raw_rsp_ptr->size,
                                       "%s: \"%s\",%d,%d", 
                                       pbm_ss_val->me_cpbs_params.display_name,
                                       me_cpbs_entry.storage,
                                       used,
                                       me_cpbs_entry.total);
  }
        
  dsatcmdp_handle_async_cmd_rsp(raw_rsp_ptr, DSAT_COMPLETE_RSP);
} /* cpb_pbm_cb_file_info_handler_output_info */


/*===========================================================================

FUNCTION CPB_PBM_FILE_INFO_HANDLER_ERROR_STATUS

DESCRIPTION
  This function is called by me_pbm_file_info_cb to process error status.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR:           non ME pbm error encountered
  DSAT_CMD_ERR_RSP:     a ME pbm error is reported to TE

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type cpb_pbm_cb_file_info_handler_error_status
(
  pbm_return_type  status
)
{
  dsat_result_enum_type result;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  me_ms_val = dsat_get_base_addr_current_session();

  /* handles the error code */
  switch (status)
  {
    /* mobile equipment error encountered */

    case PBM_ERROR_PIN2_REQ:
    case PBM_ERROR_PUC2_REQ:
      /* If attempted verification of pin2 failed due to incorrect pin
         show this result in next +CPIN command response */
        me_ms_val->dsat_mask_pin2_state = FALSE;

      /* -fallthrough */
    case PBM_ERROR_PH_SIM_REQ:
    case PBM_ERROR_SIM_PIN_REQ:
    case PBM_ERROR_SIM_PUC_REQ:
    case PBM_ERROR:
    case PBM_ERROR_NO_SIM:
    case PBM_ERROR_SIM_FAIL:
    case PBM_ERROR_SYNC:
      /* translate pbm error into cme error and 
         output it to TE */
      cpb_process_pbm_error(status, NULL);
      result = DSAT_CMD_ERR_RSP;
      break;
          
    case PBM_EOF:
    case PBM_ERROR_INDEX_INVALID:
    case PBM_ERROR_MEM_FULL:
    case PBM_ERROR_NOT_FOUND:
    case PBM_ERROR_TEXT_TOO_LONG:
    case PBM_ERROR_NUMBER_TOO_LONG:
    case PBM_ERROR_INVALID_CHAR:
    case PBM_ERROR_NOT_READY:
    case PBM_ERROR_NOT_AVAILABLE:
      /* pbm should not return this result at this time */
      DS_AT_MSG1_ERROR("Wrong pbm_result: %d", status);
      result = DSAT_ERROR;
      break;
        
    default:
      /* appease lint */
      result = DSAT_ERROR;
      DS_AT_MSG1_ERROR("Wrong pbm_result: %d", status);
  }
  
  return result;
} /* cpb_pbm_cb_file_info_handler_error_status */


/*===========================================================================

FUNCTION DSATETSIME_CPB_PBM_CB_READ_HANDLER_ERROR_STATUS

DESCRIPTION
  This function is to process error status for pbm_record_read.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR:           non ME pbm error encountered
  DSAT_CMD_ERR_RSP:     a ME pbm error is reported to TE

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type cpb_pbm_read_handler_error_status
(
  pbm_return_type status          /*  command pbm_read status  */
)
{
  dsat_result_enum_type result;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  
  me_ms_val = dsat_get_base_addr_current_session();

  /* handles the error code */
  switch (status)
  {
    /* mobile equipment error encountered */
    case PBM_ERROR:
    case PBM_ERROR_NO_SIM:
    case PBM_ERROR_PH_SIM_REQ:
    case PBM_ERROR_SIM_PIN_REQ:
    case PBM_ERROR_SIM_PUC_REQ:
    case PBM_ERROR_SIM_FAIL:
    case PBM_ERROR_SYNC:
    case PBM_ERROR_INDEX_INVALID:
      cpb_process_pbm_error(status, NULL);
      result = DSAT_CMD_ERR_RSP;
      break;

    case PBM_ERROR_PIN2_REQ:
    case PBM_ERROR_PUC2_REQ:
      /* mobile equipment error encountered */
      cpb_process_pbm_error(status, NULL);
      result = DSAT_CMD_ERR_RSP;
      /* If attempted verification of pin2 failed due to incorrect pin
       show this result in next +CPIN command response */
      me_ms_val->dsat_mask_pin2_state = FALSE;
      break;

    case PBM_EOF:
    case PBM_ERROR_MEM_FULL:
    case PBM_ERROR_TEXT_TOO_LONG:
    case PBM_ERROR_NUMBER_TOO_LONG:
    case PBM_ERROR_INVALID_CHAR:
    case PBM_ERROR_NOT_READY:
    case PBM_ERROR_NOT_AVAILABLE:
      /* pbm should not return this result at this time */
      DS_AT_MSG1_ERROR("Wrong pbm_result: %d", status);
      result = DSAT_ERROR;
      break;
        
    default:
      /* appease lint */
      result = DSAT_ERROR;
      DS_AT_MSG1_ERROR("Wrong pbm_result: %d", status);
  }

  return result;
  
} /* cpb_pbm_read_handler_error_status */


/*===========================================================================

FUNCTION CPB_PBM_CB_WRITE_HANDLER

DESCRIPTION
  This function is called by pbm to return the results of pbm_write
  function call.

  This function is passed to pbm with pbm_write function call.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : if write action succeeds
  DSAT_CMD_ERR_RSP : if there is a CME ERROR
  DSAT_ERROR       : if write action fails

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cpb_pbm_cb_write_handler
(
  ds_at_pbm_info_type *pbm_info /* PBM info pointer */
)
{
  dsat_result_enum_type result;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  pbm_return_type               status ;

  /* command pbm_write status */
  status = pbm_info->status;

  me_ms_val = dsat_get_base_addr_current_session();

  /*-------- process the pbm_write cmd status --------*/
  if(status == PBM_SUCCESS)
  {
    result = DSAT_OK;
  }
  else
  {
    /* handles the error code */
    switch (status)
    {
      case PBM_ERROR:
      case PBM_ERROR_NO_SIM:
      case PBM_ERROR_PH_SIM_REQ:
      case PBM_ERROR_SIM_PIN_REQ:
      case PBM_ERROR_SIM_PUC_REQ:
      case PBM_ERROR_SIM_FAIL:
      case PBM_ERROR_TEXT_TOO_LONG:
      case PBM_ERROR_INVALID_CHAR:
      case PBM_ERROR_NUMBER_TOO_LONG:
      case PBM_ERROR_MEM_FULL:
      case PBM_ERROR_INDEX_INVALID:
      case PBM_ERROR_SYNC:
        /* mobile equipment error encountered */
        cpb_process_pbm_error(status, NULL);
        result = DSAT_CMD_ERR_RSP;
        break;
          
      case PBM_ERROR_NOT_FOUND:
      case PBM_ERROR_NOT_READY:
      case PBM_ERROR_NOT_AVAILABLE:
        /* pbm should not return this result at this time */
        DS_AT_MSG1_ERROR("Wrong pbm_result: %d", status);
        result = DSAT_ERROR;
        break;

      case PBM_ERROR_PIN2_REQ:
      case PBM_ERROR_PUC2_REQ:
        /* If attempted verification of pin2 failed due to incorrect pin
         show this result in next +CPIN command response */
        me_ms_val->dsat_mask_pin2_state = FALSE;

        cpb_process_pbm_error(status, NULL);
        result = DSAT_CMD_ERR_RSP;
        break;

      default:
        /* appease lint */
       result = DSAT_ERROR;
	   cb_expected = CB_NONE;
       DS_AT_MSG1_ERROR("Wrong pbm_result: %d", status);
    }
  }

  return result;
} /* cpb_pbm_cb_write_handler */


/*===========================================================================

FUNCTION DD_PBM_FILE_INFO_HANDLER

DESCRIPTION
  The handler function for DD_PBM_FILE_INFO.  The pbm_file_info call 
  back function returned the info of the querried ph book type.

  This function process the info of the ph book type.

  If dd_val_ptr->index is in the correct range of the index, get the 
  number from the ph book.
  If not, we treat the D> string as D>STR case.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : seaching a number in ph book asynchrously
  DSAT_ERROR       : fail to get a number in ph book
  DSAT_CMD_ERR_RSP : fail to get a number in ph book due to a CME ERROR

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type dd_pbm_file_info_handler
(
  ds_at_pbm_info_type *pbm_info /* PBM info pointer */
)
{
  /* pbm_file_info call status */
  pbm_return_type        status;
  /* number of total entries of dd.mem */
  int                    total;
  dsat_result_enum_type  result;
  etsime_dd_type        *dd_val_ptr;

  /*-------- get pbm_file_info call status --------*/
  status = pbm_info->status;

  /* the command succeeds */
  if(status == PBM_SUCCESS)
  {
    dd_val_ptr = dsat_get_cmd_mem_ptr();

    /* we gets the info about dd_val_ptr->mem, it can't be NULL */
    ASSERT(dd_val_ptr->mem[0] != '\0');

    /* See what the number of total entries is.  The valid index is
     from 1 to it.                                                      */
    total = pbm_info->cmd_info.file_info.total;

    if(dd_val_ptr->index > 0 && dd_val_ptr->index <= total)
    {
      /* the index is valid */
      result = me_dd_process_n(dd_val_ptr->mem, dd_val_ptr, NULL);
    }
    else
    {
      /* the index is not valid, treat the D> string as search string */
      result = me_dd_process_str(dd_val_ptr, NULL);
    }
  }
  else
  {
    result = cpb_pbm_cb_file_info_handler_error_status(status);
  }

  return result;
} /* dd_pbm_file_info_handler */


/*===========================================================================

FUNCTION DD_PBM_FIND_HANDLER

DESCRIPTION
  If we find an exact match, stop searching and dial the number out.
  Otherwise, continue the search.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_NO_RSP           We find the number, and dial it out.
  DSAT_ERROR            There is an error.
  DSAT_CMD_ERR_RSP      There is cme error.
  DSAT_OK               The search went fine.
  DSAT_ASYNC_CMD        The number is dialed out with an async cmd.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type dd_pbm_find_handler
(
  const byte      *srh_str,
  etsime_dd_type  *dd_val_ptr,
  pbm_device_type  pbm_device
)
{

  dsat_result_enum_type result = DSAT_OK;

  /* pbm_find_name_next call return     */
  pbm_return_type      pbm_result;

  /* the found entry                    */
  dsat_pbm_record_s_type      *record_ptr;
  dsat_pbm_rec_id_type    rec_id;
  uint16 converted_len;
  uint8 dsat_handler;

  record_ptr = (dsat_pbm_record_s_type *)dsat_alloc_memory(sizeof(dsat_pbm_record_s_type),FALSE);
  converted_len = (uint16)((pbmutils_wstrlen((uint16*)srh_str) + 1) * sizeof(uint16));
  if(converted_len > PBM_TEXT_SIZE_BYTES)
  {
    DS_AT_MSG0_ERROR("Invalid length");
    dsatutil_free_memory(record_ptr);
    return DSAT_ERROR;
  }
  dsat_pb_id.device_type = me_cpbs_entry.device;
  pbm_result =  pbm_session_enum_rec_init_ext( 
                         dsat_pb_id,
                         (uint16)PBM_CAT_NONE,
                          PBM_FIELD_NAME,
                         (uint8 *)srh_str,
                         converted_len,
                         PBM_SEARCHTYPE_STARTSWITH|PBM_STRTYPE_UCS2,
                         &dsat_handler);
  if (pbm_result == PBM_SUCCESS )
  {
    while ( (pbm_result = pbm_session_enum_next_rec_id_ext(&rec_id, dsat_handler))== PBM_SUCCESS )
    {
      dsatme_cpb_pbm_read_record(rec_id, record_ptr, &pbm_result);
      if (pbm_result == PBM_SUCCESS )
      {
        if(dd_val_ptr->record.number[0] == '\0')
        {
          (void) dsatutil_memscpy((void*)(&dd_val_ptr->record),sizeof(dsat_pbm_record_s_type),
            (void*)record_ptr,sizeof(dsat_pbm_record_s_type));
        }
        if(!memcmp((void *)srh_str, (void *)record_ptr->text,converted_len))
        {
          /* we found a exact match, dial the number out */
          
          (void) dsatutil_memscpy((void*)(&dd_val_ptr->record),sizeof(dsat_pbm_record_s_type),
            (void*)record_ptr,sizeof(dsat_pbm_record_s_type));
          dd_val_ptr->dial_str.num = (byte *) dd_val_ptr->record.number;
          dd_val_ptr->dial_str.len = (uint16) strlen((char *) dd_val_ptr->dial_str.num);
#ifdef FEATURE_DSAT_ETSI_MODE
          result = me_dd_dial(&dd_val_ptr->dial_str);
#endif /* FEATURE_DSAT_ETSI_MODE */
          break;
        }
      }/* if */
    }/* while */
    pbm_session_enum_rec_init_ext_free_handle(dsat_handler);
  }/* if */
  dsatutil_free_memory(record_ptr);
  return result;
} /* dd_pbm_find_handler */


/*===========================================================================

FUNCTION ME_LD_PBM_WRITE_CB

DESCRIPTION
  This function is called by pbm to return the results of pbm_session_record_write
  function call for last dial.

  This function is passed to pbm with pbm_record_write function call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void me_ld_pbm_write_cb
(
  pbm_writecb_data_s_type *cb_data   /* pbm_session_record_write callback structure */
)
{
  DS_AT_MSG1_HIGH("me_ld_pbm_write_cb status %d",cb_data->ret);
} /* me_ld_pbm_write_cb */

/*-------- pbm function call and cb cmd status  processing function --------*/

/*===========================================================================

FUNCTION PROCESS_PBM_RECORD_WRITE_RETURN

DESCRIPTION
  Process the function return status of pbm_write call.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : async call pbm_write succeeds
  DSAT_CMD_ERR_RSP : if there is a CME ERROR
  DSAT_ERROR       : if pbm_write call fails

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsat_result_enum_type process_pbm_record_write_return
(
  /* return from pbm_find_location */
  pbm_return_type pbm_result,

  /* reporting error if there is any */
  dsm_item_type * err_buff_ptr
)
{
  dsat_result_enum_type result;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;


  me_ms_val = dsat_get_base_addr_current_session();

  switch (pbm_result) 
  {
    case PBM_SUCCESS:
      /* pbm_write is an async command */
      result = DSAT_ASYNC_CMD;
      break;
    case PBM_ERROR_NOT_FOUND:
      /* the only time "not found" returned on write attempt is when */ 
      /* trying to delete an already empty index. */ 
      result = DSAT_OK;
      break;
    case PBM_ERROR:
    case PBM_ERROR_NO_SIM:
    case PBM_ERROR_PH_SIM_REQ:
    case PBM_ERROR_INDEX_INVALID:
    case PBM_ERROR_SIM_FAIL:
    case PBM_ERROR_SYNC:
    case PBM_ERROR_TEXT_TOO_LONG:
    case PBM_ERROR_NUMBER_TOO_LONG:
    case PBM_ERROR_INVALID_CHAR:
    case PBM_ERROR_MEM_FULL:
      /* mobile equipment error encountered */
      cpb_process_pbm_error(pbm_result, err_buff_ptr);
      result = DSAT_CMD_ERR_RSP;
      cb_expected = CB_NONE;
      break;

    case PBM_EOF:
    case PBM_ERROR_SIM_PIN_REQ:
    case PBM_ERROR_SIM_PUC_REQ:
    case PBM_ERROR_NOT_READY:
    case PBM_ERROR_NOT_AVAILABLE:
    case PBM_ERROR_RESTRICTED:
    case PBM_ERROR_INVALID_REC:
      /* pbm should not return this result at this time */
      DS_AT_MSG1_ERROR("Wrong pbm_result: %d", pbm_result);
      result = DSAT_ERROR;
      cb_expected = CB_NONE;
      break;

    case PBM_ERROR_PIN2_REQ:
    case PBM_ERROR_PUC2_REQ:
      /* mobile equipment error encountered */
      cpb_process_pbm_error(pbm_result, err_buff_ptr);
      result = DSAT_CMD_ERR_RSP;
      cb_expected = CB_NONE;
      /* If attempted verification of pin2 failed due to incorrect pin
       show this result in next +CPIN command response */
      me_ms_val->dsat_mask_pin2_state = FALSE;
      break;

    default:
      /* appease lint */
      result = DSAT_ERROR;
      cb_expected = CB_NONE;
      DS_AT_MSG1_ERROR("Wrong pbm_result %d", pbm_result);
  }
  return result;
} /* process_pbm_record_write_return */
/*--------      Other ph book local function           --------*/
/* utility functions */

/*===========================================================================

FUNCTION GET_PBM_DEVICE

DESCRIPTION
  Map an ATCOP ph book storage type (2 char string) to pbm device type
  (an enum).

DEPENDENCIES
  None

RETURN VALUE
  TRUE : Successful.
  FALSE: Fail, did not find a pbm device type for the storage type.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean get_pbm_device
(
  /* Input, an ATCOP ph book storage type (2 char string, such as "SM" )*/
  const byte            * pb,

  /* Output, pbm device type (an enum, such as PBM_DS_SM) */
  pbm_device_type       * dev_ptr
)
{
  int                  index;
  dsati_mode_e_type    current_mode = dsatcmdp_get_current_mode();
  mmgsdi_app_enum_type app_type = dsat_get_current_app_type(TRUE);

  if(!dev_ptr)
  {
    DS_AT_MSG0_ERROR("NULL dev_ptr pointer");
    return FALSE;
  }

#ifdef FEATURE_MMGSDI
  /* If looking for last dialed PB device we must determine current card
     SIM/USIM, which could change depending on $QCSLOT setting */
  if( !dsatutil_strcmp_ig_sp_case(pb, (const byte *)LAST_DIALED_STR) )
  {

    if ((app_type == MMGSDI_APP_SIM) || (app_type == MMGSDI_APP_RUIM))
    {
      *dev_ptr = PBM_LND;
      return TRUE;
    }
#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)
    else if (app_type == MMGSDI_APP_USIM) 
    {
      *dev_ptr = PBM_OCI;
      return TRUE;
    }
#endif /*defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */
    /* If not SIM or USIM fail in ETSI mode */
    else
    {
      if( IS_ETSI_MODE(current_mode) )
      {
        return FALSE;
      }
      else
      {
        *dev_ptr = PBM_LND;
        return TRUE;
      }
    }
  }
  else
#endif /* FEATURE_MMGSDI */
  {
    /*-------- search pbm device for pb in static pbm_table --------*/
    for (index = 0; index < pbm_table_size; index++)
    {
      if(!dsatutil_strcmp_ig_sp_case(pb, pbm_table[index].storage))
      {
        *dev_ptr = pbm_table[index].device;
        return TRUE;
      }
    }
  }
  DS_AT_MSG0_HIGH("PBM device type not found");
  return FALSE;
} /* get_pbm_device */


/*===========================================================================

FUNCTION GET_PB_STORAGE

DESCRIPTION
  Map a pbm device type (an enum) to an ATCOP ph book storage 
  type (2 char string).

DEPENDENCIES
  None

RETURN VALUE
  TRUE : Successful.
  FALSE: Fail, did not find a storage for the pbm device type.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean get_pb_storage
(
  /* Input, pbm device type (an enum, such as PBM_DS_SM) */
  pbm_device_type       device,

  /* Output, an ATCOP ph book storage type (2 char string, such as "SM" ) */
  byte                  * storage_ptr
)
{
  int                  index;
  mmgsdi_app_enum_type app_type = dsat_get_current_app_type(TRUE);

  if(!storage_ptr)
  {
    DS_AT_MSG0_ERROR("NULL pb pointer");
    return FALSE;
  }
     DS_AT_MSG1_HIGH("PBM device recived %d",device);
  /* If looking for a device that would map to "LD" storage we must 
     determine current card SIM/USIM, which could change depending on 
     $QCSLOT setting */
#ifdef FEATURE_MMGSDI
  if( device == PBM_LND ||
      device == PBM_OCI )
  {
    if ( ( (app_type == MMGSDI_APP_SIM) && (device == PBM_LND) ) ||
         ( (app_type == MMGSDI_APP_RUIM) && (device == PBM_LND) ) ||
         ( (app_type == MMGSDI_APP_RUIM) && (device == PBM_OCI) ) )
    {
      (void)strlcpy( (char *)storage_ptr, LAST_DIALED_STR,
                         sizeof(storage_ptr));
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  /*-------- else search pbm device for pb in pbm_table --------*/ 
  else
#endif /* FEATURE_MMGSDI */
  {
    for (index = 0; index < pbm_table_size; index++)
    {
      if(pbm_table[index].device == device)
      {
        (void) strlcpy((char *) storage_ptr,
                           (char *) pbm_table[index].storage,
                           sizeof(storage_ptr));
        return TRUE;
      }
    }
  }
  
  return FALSE;
} /* get_pb_storage */

/* cpbs local functions */

/*===========================================================================

FUNCTION CPBS_PARSE_ARGUMENTS

DESCRIPTION
  Parse the +cpbs argument into cpbs_argu_type data structure so that
  we can continue on the command after we receive the reply for pin_status,
  and pin_verification from GSDI.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_PARAM : if the parameter is not correct
    DSAT_ERROR : if the storage type is not supported
    DSAT_OK    : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpbs_parse_arguments
(
  const tokens_struct_type * tok_ptr,
  cpbs_argu_type           * cpbs_argu
)
{
  byte first_argu_no_quote[CPBS_VAL_LEN + 1];
  int index;
  dsat_result_enum_type result = DSAT_OK;
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);
  /* this is a read command */
  if( tok_ptr->op == (NA | QU) )
  {
    /* dsat_cpbs_val might be reset by atz at&f command.
       If that is the case, we need reset me_cpbs_entry 
       here */

    /* use current device type */
    cpbs_argu->pbm_device = me_cpbs_entry.device;
    
    cpbs_argu->password[0] = '\0';
    cpbs_argu->is_read = TRUE;
  }
  else /* set command */
  {
    cpbs_argu->is_read = FALSE;
    if(tok_ptr->args_found == 0)
    {
      /* the AT cmd has no argument, use the default entry in the 
         dsat_cpbs_list */
      const dsat_string_item_type * default_storage;
      default_storage = dsat_cpbs_storage_valstr[dsat_cpbs_list.default_v];
      if(pbm_ss_val->dsatme_nv_only == TRUE)
      {
        default_storage = dsat_cpbs_storage_valstr[5];
      }
      if (!get_pbm_device(default_storage, &cpbs_argu->pbm_device))
      {
        /* can't find a pbm device for the default storage type */
        DS_AT_MSG2_ERROR("No pbm_device for %c%c",default_storage[0],default_storage[1]);
        result = DSAT_ERROR;
      }
    }
    else
    {
      /* strip out of the quotes from the first argument */
      if (!dsatutil_strip_quotes_out(tok_ptr->arg[0], first_argu_no_quote, 
                                     CPBS_VAL_LEN + 1) )
      {
        result = DSAT_PARAM;
      }
      else
      {
        /* determine the pbm device type from the first argument */
        if(!( get_pbm_device(first_argu_no_quote, &cpbs_argu->pbm_device)))
        {
          /* the AT storage (first argument) can't be mapped to a pbm_device
             the first argument is not correct */
          result = DSAT_ERROR;
        }
      }
    }

    /* check if the tokens contains password
     the password is used to unlock the pin2 lock */
    if(result == DSAT_OK && tok_ptr->args_found >= 2)
    {
      int pin2_len = (int) strlen((char *) tok_ptr->arg[1]);

      /* the pin2 is 4 to 8 long +2 \"s 
         the first char is \", last char is \" */
      if( pin2_len < PIN_MIN_LEN + 2 || pin2_len > PIN_MAX_LEN + 2 ||
          tok_ptr->arg[1][0] != '"' ||
          tok_ptr->arg[1][pin2_len - 1] != '"')
      {
        DS_AT_MSG_SPRINTF_1_MED(
          "The cpbs password parameter (%s) is wrong",tok_ptr->arg[1]);
        result = DSAT_PARAM;
      }
      else /* The code passes the argument check */
      {
        dsatutil_reset_password(cpbs_argu->password, sizeof(cpbs_argu->password));
      
        /* get the string between \" and \" of the second argument*/
        for (index = 0; index < pin2_len - 2; index++)
        {
          cpbs_argu->password[index] = tok_ptr->arg[1][index+1];
        }
        cpbs_argu->password[index] = 0;
      }
    } /* if(tok_ptr->args_found >= 2) */
  }

  return result;
} /* cpbs_parse_arguments */


/*===========================================================================

FUNCTION cpbs_process_test_cmd

DESCRIPTION
  This function process the test command for +CPBS. This function displays 
  the phone books list supported by the SIM and ME.

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
LOCAL dsat_result_enum_type cpbs_process_test_cmd
(
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  pbm_return_type pbm_return;
  pbm_device_type pbm_dev;
  unsigned short index=0;
  boolean first;
  dsatme_pbm_state_ss_info  *pbm_ss_val = NULL;
  pbm_extended_fileinfo_s_type *info = NULL;

  pbm_ss_val = dsat_get_base_addr(DSAT_PBM_SS_VALS, FALSE);
 
  (void)strlcpy(pbm_ss_val->me_cpbs_params.display_name,
                    ((char *)(parse_table->name)),
                    MAX_PBM_AT_NAME_LEN);

  info = dsat_alloc_memory(sizeof(pbm_extended_fileinfo_s_type), FALSE);
  
  res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "%s: (",
                                             pbm_ss_val->me_cpbs_params.display_name);

  first = TRUE;
 /* update the cpbs parameter */
  for (index = 0; index < CPBS_SUPPORTED_PHBKS_NUM; index++)
  {
      memset( info, 0x0, sizeof(pbm_extended_fileinfo_s_type));
      if (!get_pbm_device(dsat_cpbs_storage_valstr[index], &pbm_dev))
      {
        /* can't find a pbm device for the default storage type */
        DS_AT_MSG0_ERROR("No pbm_device mapped");
        return DSAT_ERROR;
      }
      dsat_pb_id.device_type = pbm_dev;
      pbm_return = pbm_session_extended_file_info( dsat_pb_id,
   	                                            info );
      DS_AT_MSG4_HIGH(" index:%d, pbm_dev:%d, pbm_return:%d, num_of_records:%d ",
                                                         index,
                                                         pbm_dev,
                                                         pbm_return,
                                                         info->number_of_records);

      if( pbm_return == PBM_SUCCESS
#ifdef FEATURE_DSAT_EXTENDED_CMD
          || pbm_dev == PBM_DS_SM
#endif /* FEATURE_DSAT_EXTENDED_CMD */
        )
      {
        if( info->number_of_records > 0 )
        {
          if(first)
          {
          res_buff_ptr->used += (word) snprintf(
                           (char *)res_buff_ptr->data_ptr + res_buff_ptr->used,
                           res_buff_ptr->size - res_buff_ptr->used,
                                  "\"%s\"",
                                  dsat_cpbs_storage_valstr[index]);
          }
          else
          {
            res_buff_ptr->used += (word) snprintf(
                                  (char *)res_buff_ptr->data_ptr + res_buff_ptr->used,
                                  res_buff_ptr->size - res_buff_ptr->used,
                                  ",\"%s\"",
                           dsat_cpbs_storage_valstr[index]);
          }
          first = FALSE;
        }
      }
      else if ((pbm_return == PBM_ERROR_NOT_AVAILABLE) ||
               (pbm_return == PBM_ERROR_NOT_READY) ||
               (pbm_return == PBM_ERROR_NO_SIM))
      {
        /* skip the corresponding phone book from the display of 
           supported phone books list */
      }
      else
      {
	dsatutil_free_memory( info );
        return DSAT_ERROR;
      }
    }

  res_buff_ptr->used += (word) snprintf((char *)
                        res_buff_ptr->data_ptr + res_buff_ptr->used,
                        res_buff_ptr->size - res_buff_ptr->used,
                        ")");
  dsatutil_free_memory( info );																						
  return DSAT_OK;

} /* cpbs_process_test_cmd */

/*===========================================================================

FUNCTION CPBR_PARSE_ARGUMENTS

DESCRIPTION
  Parse the +cpbr argument into index1_ptr and index2_ptr.
  Report error to res_buff_ptr if there is any.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_ERROR       : if the parameter is not correct
    DSAT_CMD_ERR_RSP : if there is a cme error
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type cpbr_parse_arguments
(
  const tokens_struct_type * tok_ptr,
  uint16                      * index1_ptr,
  uint16                      * index2_ptr,
  dsm_item_type            * res_buff_ptr  /* for reporting error */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type arg1, arg2;

    /* get the indexes to read the ph book from index1 to index2 */
  if ( dsatutil_atoi((dsat_num_item_type *) &arg1, 
                     tok_ptr->arg[0], 10) != ATOI_OK )
  {
    /* if index1 can not be converted into a legal number,
       this is invalid parameter */
     DS_AT_MSG_SPRINTF_1_MED(
       "CPBR index1 %s must be a positive number",tok_ptr->arg[0]);
    dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  if( ( (me_cpbs_entry.total != -1) && 
        ((int16) arg1 > me_cpbs_entry.total))
      || (arg1 < PB_INDEX_MIN) || (arg1 > PB_INDEX_MAX) )
  {
    dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  if (tok_ptr->args_found > 1)
  {
    if ( dsatutil_atoi((dsat_num_item_type *) &arg2, 
                       tok_ptr->arg[1], 10) != ATOI_OK )
    {
      /* if index2 can not be converted into a legal number,
         this is invalid parameter */
       DS_AT_MSG_SPRINTF_1_MED(
         "CPBR 2nd arg %s must be a positive number",tok_ptr->arg[1]);
       return DSAT_ERROR;
    }
      
    /* if the index is out of PBM allowed range, output a cme error */
    if( ( (me_cpbs_entry.total != -1) && 
          ((int16) arg2 > me_cpbs_entry.total) )
        || (arg2 < PB_INDEX_MIN) || (arg2 > PB_INDEX_MAX) )
    {
      dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
  }
  else
  {
    /* only give index 1 */
    arg2 = arg1;
  }

  *index1_ptr =(uint16)((uint32)( MIN(arg1, arg2)));
  *index2_ptr = (uint16)((uint32)(MAX(arg1, arg2)));

  return result;
  
} /* cpbr_parse_arguments */

/*===========================================================================

FUNCTION CPBW_PARSE_ARGUMENTS

DESCRIPTION
  Parse the +cpbw argument into pbm_record_s_type.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_ERROR       : if the parameter is not correct
    DSAT_CMD_ERR_RSP : if there is a cme error
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpbw_parse_arguments
(
  const tokens_struct_type * tok_ptr,
  dsat_pbm_record_s_type        * record_ptr,
  dsm_item_type            * res_buff_ptr  /* for reporting error */
)
{
  int index_to_write;
  dsat_num_item_type cscs_val ;
#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)
  if ( tok_ptr->args_found > 9 )
  {
    DS_AT_MSG0_HIGH("Total number of arguments is >9 ");
    return DSAT_ERROR;
  }
#else
  if ( tok_ptr->args_found > 5 )
  {
    DS_AT_MSG0_HIGH("Total number of arguments is >5 ");
    return DSAT_ERROR;
  }
#endif /* defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */   
  /* Get the index to write */
  if (tok_ptr->arg[0][0] == '\0')
  {
    index_to_write = -1;
  }
  else
  {
    if ( dsatutil_atoi((dsat_num_item_type *)&index_to_write, 
                       tok_ptr->arg[0], 10) != ATOI_OK)
    {
      DS_AT_MSG_SPRINTF_1_MED(
        "CPBW's first argument (%s) should be a positive number",
         tok_ptr->arg[0]);
      dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
    if( ( (me_cpbs_entry.total != -1) && 
          ((int16) index_to_write > me_cpbs_entry.total))
        || (index_to_write < PB_INDEX_MIN) 
        || (index_to_write > PB_INDEX_MAX) )
    {
      dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
  }
    
  record_ptr->index = (int16) index_to_write;
    
  /* if the number field is ignored, the record_ptr->index
     entry is erased */
  if( (tok_ptr->args_found < 2) || (tok_ptr->arg[1][0] == '\0') )
  {
    record_ptr->number[0] = '\0';
  }
  else /* user input a number */
  {
    /* strip out the "s from the tok_ptr->arg[1] */
    if ( !dsatutil_strip_quotes_out( tok_ptr->arg[1], 
                                     tok_ptr->arg[1], MAX_LINE_SIZE))
    {
      return DSAT_ERROR;
    }

    /* strip out format dividers and err on illegal chars in the number 
       string.  It is done here, not in pbm because pbm_record_s_type
       can't hold a number more than PBM_MAX_NUM_LENGTH (CPB_MAX_NUM_LENGTH)
       long.  Thus, a legal number of PBM_MAX_NUM_LENGTH long with ()- in will 
       exceed PBM_MAX_NUM_LENGTH and fail on length check.
     */
    if( !cpbw_check_num_string(tok_ptr->arg[1], tok_ptr->arg[1]) )
    {
      dsatme_set_cme_error(DSAT_CME_INVALID_CHAR_IN_DIAL_STRING, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    /* check length */
    if(strlen((char *) tok_ptr->arg[1]) > CPB_MAX_NUM_LENGTH)
    {
      dsatme_set_cme_error(DSAT_CME_DIAL_STRING_TOO_LONG, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    /* copy it into record_ptr->number */
    (void) strlcpy(record_ptr->number,
                       (char *)tok_ptr->arg[1],
                       PBM_NUM_SIZE);
  }

  /* get the address type */
  if( (tok_ptr->args_found < 3) || (tok_ptr->arg[2][0] == '\0') )
  {
    /* user does not give a number type */
    if(record_ptr->number[0] == '+')
    {
      record_ptr->type = TA_INTERNATIONAL;
    }
    else
    {
      record_ptr->type = TA_OTHER;
    }
  }
  else /* user input a type */
  {
    int addr_type;
        
    if ( dsatutil_atoi((dsat_num_item_type *) &addr_type, 
                       tok_ptr->arg[2], 10) != ATOI_OK )
    {
      DS_AT_MSG_SPRINTF_1_LOW(
        "cpbw incorrect address type %s",tok_ptr->arg[2]);
      return DSAT_ERROR;
    }

    /* ui can't handle a CPB_MAX_NUM_LENGTH international number since UI 
       always add + sign before the number.  This makes it 
       CPB_MAX_NUM_LENGTH + 1 long */
    if ( (addr_type == TA_INTERNATIONAL) && 
         (record_ptr->number[0] != '+') &&
         (strlen(record_ptr->number) > (CPB_MAX_NUM_LENGTH - 1)) )
    {
      dsatme_set_cme_error(DSAT_CME_DIAL_STRING_TOO_LONG, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    /* type is allowed from 128-255 */
    if(addr_type < 128 || addr_type > 255)
    {
      /* wrong parameter */
      dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
    
    record_ptr->type = (uint8) addr_type;
  }

  /* get the fourth argument, ph text */
  if( (tok_ptr->args_found < 4) || (tok_ptr->arg[3][0] == '\0'))
  {
    /* Need to set 2 bytes to NULL because PBM expects UCS2 input */
    record_ptr->text[0] = '\0';
    record_ptr->text[1] = '\0';
  }
  else /* user give a text */
  {
#ifdef FEATURE_DSAT_EXTENDED_CMD
    dsat_num_item_type  encoding;
    if ( tok_ptr->args_found == 4 )
    {
      DS_AT_MSG0_ERROR("extended CMD requires coding field when text field specified");
      return DSAT_ERROR;
    }
    else
    {
      dsatutil_atoi( &encoding, tok_ptr->arg[4], 10 );
      if ( (encoding != DSATME_ENCODING_RAW) &&
           (encoding != DSATME_ENCODING_ASCII) )
      {
        DS_AT_MSG1_ERROR("invalid coding mode specified (%d)",encoding);
        return DSAT_ERROR;
      }
    }

    /* Strip out quotes */
    if ( !dsatutil_strip_quotes_out(tok_ptr->arg[3],
                                    tok_ptr->arg[3], 
                                    (uint16)strlen((char *)tok_ptr->arg[3])) )
    {
      return DSAT_ERROR;
    }

    if ( encoding == DSATME_ENCODING_ASCII )
    {
      if( dsatutil_convert_chset((const char *)tok_ptr->arg[3], 
                                 ALPHA_IRA,
                                 record_ptr->text,
                                 ALPHA_PBM_UCS2,
                                 PBM_MAX_TEXT_LENGTH, 
                                 FALSE) == NULL )
      {
        return DSAT_ERROR;
      }
    }
    else /* RAW encoding */
    {
      dsat_cme_error_e_type conv_err;
      uint16 text_len = (me_cpbs_entry.text_len < 0 || me_cpbs_entry.text_len > PBM_TEXT_SIZE_CHARS) 
                          ? PBM_TEXT_SIZE_CHARS : me_cpbs_entry.text_len;
      conv_err = dsatutil_convert_gsm_ucs2_to_pbm_ucs2((char *) tok_ptr->arg[3],
                                                  (uint16 *) record_ptr->text,
                                                  text_len);
      if ( DSAT_CME_UNKNOWN == conv_err )
      {
        return DSAT_ERROR;
      }
      else if ( DSAT_CME_NO_ERROR != conv_err )
      {
        dsatme_set_cme_error(conv_err, res_buff_ptr);
        return DSAT_CMD_ERR_RSP;
      }
    }
#else /* FEATURE_DSAT_EXTENDED_CMD */
    /* If me_cpbs_entry.text_len is -1 it means maximum supported length
     * could not be determined for this device in this case use default */
    int max_text_len = me_cpbs_entry.text_len < 0 ? 
      PBM_MAX_TEXT_LENGTH : me_cpbs_entry.text_len;

    /* What length of <text> parm would be once quotes are stripped and
     * it is converted to ME alphabet */
    uint16 converted_len = (uint16)(strlen((char *) tok_ptr->arg[3]) - 2);
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    switch ( cscs_val )
    {
      case ALPHA_UCS2:
        converted_len >>= 2;
        break;
      case ALPHA_HEX:
        converted_len >>= 1;
        break;
      default:
        /* Length will not be changed by conversion from TE to ME alphabet */
        break;
    }
#ifdef FEATURE_DSAT_EXTENDED_CMD
    if ( (tok_ptr->args_found >= 5) && (tok_ptr->arg[4][0] != '2') )
    {
      DS_AT_MSG1_ERROR("tried cpbw with encoding %c but currently only support 2 = ASCII",
                tok_ptr->arg[4][0]);
      dsatme_set_cme_error(DSAT_CME_OP_NOT_SUPPORTED, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
#endif /* FEATURE_DSAT_EXTENDED_CMD */
    /* Verify converted <text> will be of correct length */
    if ( converted_len > max_text_len )
    {
      dsatme_set_cme_error(DSAT_CME_TEXT_STRING_TOO_LONG,
                               res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    /* Attempt to strip out the "s from the tok_ptr->arg[3] and
       convert from TE to ME alphabet return ERROR on failure */
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    if( !dsatutil_strip_quotes_out(tok_ptr->arg[3],
                                   tok_ptr->arg[3], 
                                   (uint16)strlen((char *)tok_ptr->arg[3])) ||
         dsatutil_convert_chset((const char *)tok_ptr->arg[3], 
                                (dsati_chset_type)cscs_val,
                                record_ptr->text,
                                ALPHA_PBM_UCS2,
                                PBM_MAX_TEXT_LENGTH, 
                                FALSE) == NULL )
    {
      return DSAT_ERROR;
    }
#endif /* FEATURE_DSAT_EXTENDED_CMD */
  }
#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)

  /* if the group number field is ignored, the record_ptr->group
     entry is set to NULL  */
  if( (tok_ptr->args_found < 5) || (tok_ptr->arg[4][0] == '\0') )
  {
    record_ptr->group[0] = '\0';
  }
  else /* user input a number */
  {
     int max_text_len = me_cpbs_entry.text_len < 0 ? PBM_MAX_TEXT_LENGTH : me_cpbs_entry.text_len;
      /* What length of <group> parm would be once quotes are stripped and
             it is converted to ME alphabet */
    uint16 converted_len = (uint16)(strlen((char *) tok_ptr->arg[4]) - 2);
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    switch ( cscs_val )
    {
      case ALPHA_UCS2:
        converted_len >>= 2;
      break;
      case ALPHA_HEX:
        converted_len >>= 1;
      break;
      default:
      /* Length will not be changed by conversion from TE to ME alphabet */
      break;
    }
    /* Verify converted <group> will be of correct length */
    if ( converted_len > max_text_len )
    {
      dsatme_set_cme_error(DSAT_CME_TEXT_STRING_TOO_LONG,
                                    res_buff_ptr);
            
      return DSAT_CMD_ERR_RSP;
    }
    /* strip out the "s from the tok_ptr->arg[4] */
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                    DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    if( !dsatutil_strip_quotes_out(tok_ptr->arg[4],
                                    tok_ptr->arg[4], 
                                   (uint16)strlen((char *)tok_ptr->arg[4])) ||
         dsatutil_convert_chset((const char *)tok_ptr->arg[4], 
                                (dsati_chset_type)cscs_val,
                                (const char *)&record_ptr->group[0],
                                 ALPHA_PBM_UCS2,
                                 PBM_MAX_TEXT_LENGTH, 
                                 FALSE) == NULL )
    {
      return DSAT_ERROR;
    }
  }
  
  /* get the sixth  argument, ph adn number */
  if( (tok_ptr->args_found < 6) || (tok_ptr->arg[5][0] == '\0'))
  {
    record_ptr->adnumber[0] = '\0';
  }
  else /* user give a adn number */
  {
    /* strip out the "s from the tok_ptr->arg[5] */
    if ( !dsatutil_strip_quotes_out( tok_ptr->arg[5], 
                                     tok_ptr->arg[5], MAX_LINE_SIZE))
    {
      return DSAT_ERROR;
    }
    
    /* strip out format dividers and err on illegal chars in the number 
       string.  It is done here, not in pbm because pbm_record_s_type
       can't hold a number more than PBM_MAX_NUM_LENGTH (CPB_MAX_NUM_LENGTH)
       long.  Thus, a legal number of PBM_MAX_NUM_LENGTH long with ()- in will 
       exceed PBM_MAX_NUM_LENGTH and fail on length check.
     */
    if( !cpbw_check_num_string(tok_ptr->arg[5], tok_ptr->arg[5]) )
    {
      dsatme_set_cme_error(DSAT_CME_INVALID_CHAR_IN_DIAL_STRING, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
    
    /* check length */
    if(strlen((char *) tok_ptr->arg[5]) > CPB_MAX_NUM_LENGTH)
    {
      dsatme_set_cme_error(DSAT_CME_DIAL_STRING_TOO_LONG, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
    
    /* copy it into record_ptr->adnumber */
    (void) strlcpy(record_ptr->adnumber,
                       (char *)tok_ptr->arg[5],
                       PBM_NUM_SIZE);
  }
  /* get the address type */
  if( (tok_ptr->args_found < 7) || (tok_ptr->arg[6][0] == '\0') )
  {
    /* user does not give a adnumber type */
    if(record_ptr->adnumber[0] == '+')
    {
      record_ptr->adtype = TA_INTERNATIONAL;
    }
    else
    {
      record_ptr->adtype = TA_OTHER;
    }
  }
  else
  {
      dsat_num_item_type adn_addr_type;
      
  if ( dsatutil_atoi(&adn_addr_type, 
                     tok_ptr->arg[6], 10) != ATOI_OK )
  {
    DS_AT_MSG1_ERROR("cpbw incorrect additional address type %s",tok_ptr->arg[6]);
    return DSAT_ERROR;
  }
   /* ui can't handle a CPB_MAX_NUM_LENGTH international number since UI 
     always add + sign before the number.  This makes it 
     CPB_MAX_NUM_LENGTH + 1 long */
  if ( (adn_addr_type == TA_INTERNATIONAL) && 
       (record_ptr->adnumber[0] != '+') &&
       (strlen(record_ptr->adnumber) > (CPB_MAX_NUM_LENGTH - 1)) )
  {
    dsatme_set_cme_error(DSAT_CME_DIAL_STRING_TOO_LONG, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }
   /* type is allowed from 128-255 */
  if(adn_addr_type < 128 || adn_addr_type > 255)
  {
    /* wrong parameter */
    dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }
  
  record_ptr->adtype = (uint8) adn_addr_type;
  }
   /* get the Eight argument, second text */
  if( (tok_ptr->args_found < 8) || (tok_ptr->arg[7][0] == '\0'))
  {
    record_ptr->secondtext[0] = '\0';
  }
  else /* user give a text */
  {
    /* If me_cpbs_entry.text_len is -1 it means maximum supported length
       could not be determined for this device in this case use default */
    /* This should change according new API from PB */
    int max_text_len = me_cpbs_entry.text_len < 0 ? 
    PBM_MAX_TEXT_LENGTH : me_cpbs_entry.text_len;
    /* What length of <text> parm would be once quotes are stripped and
       it is converted to ME alphabet */
    uint16 converted_len = (uint16)(strlen((char *) tok_ptr->arg[7]) - 2);
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    switch ( cscs_val )
    {
      case ALPHA_UCS2:
        converted_len >>= 2;
        break;
      case ALPHA_HEX:
        converted_len >>= 1;
        break;
      default:
        /* Length will not be changed by conversion from TE to ME alphabet */
        break;
    }
  /* Verify converted <secondtext> will be of correct length */
    if ( converted_len > max_text_len )
    {
      dsatme_set_cme_error(DSAT_CME_TEXT_STRING_TOO_LONG,
                               res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
    /* Attempt to strip out the "s from the tok_ptr->arg[7] and
       convert from TE to ME alphabet return ERROR on failure */
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    if( !dsatutil_strip_quotes_out(tok_ptr->arg[7],
                                   tok_ptr->arg[7], 
                                   (uint16)strlen((char *)tok_ptr->arg[7])) ||
         dsatutil_convert_chset((const char *)tok_ptr->arg[7], 
                                (dsati_chset_type)cscs_val,
                                record_ptr->secondtext,
                                ALPHA_PBM_UCS2,
                                PBM_MAX_TEXT_LENGTH, 
                                FALSE) == NULL )
    {
      return DSAT_ERROR;
    }
  }
  /* get the 9 argument, email */
  if( (tok_ptr->args_found < 9) || (tok_ptr->arg[8][0] == '\0'))
  {
    record_ptr->email[0] = '\0';
  }
  else /* user give a text */
  {
    /* If me_cpbs_entry.text_len is -1 it means maximum supported length
       could not be determined for this device in this case use default */
    /* This should change according new API from PB */
    int max_text_len = me_cpbs_entry.text_len < 0 ? 
      PBM_MAX_TEXT_LENGTH : me_cpbs_entry.text_len;
    /* What length of <email> parm would be once quotes are stripped and
       it is converted to ME alphabet */
    uint16 converted_len = (uint16)(strlen((char *) tok_ptr->arg[8]) - 2);
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    switch ( cscs_val )
    {
      case ALPHA_UCS2:
        converted_len >>= 2;
        break;
      case ALPHA_HEX:
        converted_len >>= 1;
        break;
      default:
        /* Length will not be changed by conversion from TE to ME alphabet */
        break;
    }

    /* Verify converted <email> will be of correct length */
    if ( converted_len > max_text_len )
    {
      dsatme_set_cme_error(DSAT_CME_TEXT_STRING_TOO_LONG,
                               res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }
    /* Attempt to strip out the "s from the tok_ptr->arg[8] and
       convert from TE to ME alphabet return ERROR on failure */
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    if( !dsatutil_strip_quotes_out(tok_ptr->arg[8],
                                   tok_ptr->arg[8], 
                                   (uint16)strlen((char *)tok_ptr->arg[8])) ||
         dsatutil_convert_chset((const char *)tok_ptr->arg[8], 
                                (dsati_chset_type)cscs_val,
                                record_ptr->email,
                                ALPHA_PBM_UCS2,
                                PBM_MAX_TEXT_LENGTH, 
                                FALSE) == NULL )
    {
      return DSAT_ERROR;
    }
  }
#endif /* defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */
  return DSAT_OK;

} /* cpbw_parse_arguments */


/*===========================================================================

FUNCTION CPBW_CHECK_NUM_STRING

DESCRIPTION
  The input is supposed to be a null-terminated byte string.
  This function strip out fomat dividers, ie ()-.
  The function will return FALSE if there are illegal chars in
  the in_str.  The illegal chars are anything not 0-9, #, *, +.
  
  + sign is legal if it is at the beginning of the num.

DEPENDENCIES
  Caller makes sure there are enough space allocated for output string.
  in_str is null-terminated.

RETURN VALUE
  TRUE check passed
  FALSE if there is illegal chars here

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean cpbw_check_num_string
(
  const byte * in_str,
  byte * out_str
)
{
  const byte *in_ptr = in_str;
  byte *out_ptr = out_str;

  /* Handle special case of + */
  if(*in_ptr == '+')
  {
    *out_ptr = *in_ptr;
    in_ptr++;
    out_ptr++;
  }

  while(*in_ptr)
  {
    /* strip out format divider */
    if ( (*in_ptr == '(') || (*in_ptr == ')') || (*in_ptr == '-') )
    {
      in_ptr++;
      /* ignore (,),- */
      continue;
    }

    /* Convert TIMEPAUSE/HARDPAUSE character */
    /* check illegal chars */
    if ( (*in_ptr != '#') && (*in_ptr != '*') && (*in_ptr != 'p') &&
         (*in_ptr != '?') &&(*in_ptr != 'e') && 
         ((*in_ptr < '0') || (*in_ptr > '9') ) )
    {
      return FALSE;
    }
    
    if(*in_ptr == 'p')
    {
      *out_ptr = 'T';
    }
    else
    {
    *out_ptr = *in_ptr;
    }
    in_ptr++;
    out_ptr++;
  }
  
  *out_ptr = 0;
  return TRUE;
} /* cpbw_check_num_string */


/*===========================================================================

FUNCTION DSATME_CPB_CME_ERROR_LOOKUP

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
/* ARGSUSED */
dsat_cme_error_e_type
dsatme_cpb_cme_error_lookup(pbm_return_type result)
{
  int index;

  static struct {
    pbm_return_type pbm_result;
    dsat_cme_error_e_type me_error_code;
  } error_code_table[] =
    {
      { PBM_ERROR_PH_SIM_REQ, DSAT_CME_PH_SIM_PIN_REQUIRED },
      { PBM_ERROR_NO_SIM,     DSAT_CME_SIM_NOT_INSERTED    },
      { PBM_ERROR_SIM_PIN_REQ, DSAT_CME_SIM_PIN_REQUIRED   },
      { PBM_ERROR_SIM_PUC_REQ, DSAT_CME_SIM_PUK_REQUIRED   },
      { PBM_ERROR_SIM_FAIL,    DSAT_CME_SIM_FAILURE        },
      { PBM_ERROR_PIN2_REQ,    DSAT_CME_SIM_PIN2_REQUIRED  },
      { PBM_ERROR_PUC2_REQ,    DSAT_CME_SIM_PUK2_REQUIRED  },
      { PBM_ERROR_MEM_FULL,    DSAT_CME_MEMORY_FULL        },
      { PBM_ERROR_INDEX_INVALID, DSAT_CME_INVALID_INDEX    },

      /* when entry not found for pbm_search */
      { PBM_ERROR_NOT_FOUND,     DSAT_CME_NOT_FOUND        },

      /* when the text too long for pbm_write or pbm_search */
      { PBM_ERROR_TEXT_TOO_LONG, DSAT_CME_TEXT_STRING_TOO_LONG },

      /* when the number too long for pbm_write */
      { PBM_ERROR_NUMBER_TOO_LONG, DSAT_CME_DIAL_STRING_TOO_LONG},

      /* when there is invalid char in number for pbm_write */
      { PBM_ERROR_INVALID_CHAR, DSAT_CME_INVALID_CHAR_IN_DIAL_STRING},

      /* SIM still synchronizing/initializing */
      { PBM_ERROR_SYNC, DSAT_CME_SIM_BUSY },

      { PBM_ERROR,              DSAT_CME_UNKNOWN}
    };

  for (index = 0;; index++) {
    if(error_code_table[index].pbm_result == result)
    {
      return error_code_table[index].me_error_code;
    }
    
    /* result not found and run to the end of error code table */
    else if(error_code_table[index].pbm_result == PBM_ERROR)
    {
        return DSAT_CME_MAX;
    }
  }

} /* dsatme_cpb_cme_error_lookup */


/*===========================================================================

FUNCTION CPB_PROCESS_PBM_ERROR

DESCRIPTION
  Process pbm error, translate them into cme error and output it.

  err_buff_ptr: write error to this buffer.
                If err_buff_ptr is NULL.  The CME error is sent 
                to dsatcmdp to process.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void cpb_process_pbm_error(pbm_return_type pbm_err, 
                                 dsm_item_type * err_buff_ptr)
{
  dsm_item_type * res_buff_ptr;
  dsat_cme_error_e_type error_code = dsatme_cpb_cme_error_lookup(pbm_err);

  if(error_code == DSAT_CME_MAX)
  {
    /* error code not found, use DSAT_CME_UNKNOWN */
    DS_AT_MSG1_ERROR("unrecognized pbm_return_type error %d",pbm_err);
    error_code = DSAT_CME_UNKNOWN;
  }
  
  if(err_buff_ptr != NULL)
  {
    dsatme_set_cme_error(error_code, err_buff_ptr);
  }
  else
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    dsatme_set_cme_error(error_code, res_buff_ptr);
    dsatcmdp_handle_async_cmd_rsp( res_buff_ptr, DSAT_COMPLETE_RSP );
  }
} /* cpb_process_pbm_error */
/* local function for direct dialing */

/*===========================================================================

FUNCTION ME_DD_PARSE_DIAL_STR

DESCRIPTION
  Parse dial_ptr of type dsati_dial_str_type.  Get <mem> and <n> for direct
  dial.  The result is put into dd_val_ptr.

DEPENDENCIES
  None

RETURN VALUE
  Return FALSE if dd_val_ptr doesnot get initialized
  Return TRUE  if dd_val_ptr gets initialized

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean me_dd_parse_dial_str
(
  /* input containing the string after D>       */
  const dsati_dial_str_type     * dial_ptr,

  /* output containing mem and n, the original
     dial string is save                        */
  etsime_dd_type                * dd_val_ptr
)
{
  /* loop index                                 */
  int   index;

  /* point to the char <n> starts in d> string  */
  byte  * num_ptr;

  atoi_enum_type atoi_ret;

  /*-------- Init dd_val_ptr from dial_ptr      --------*/
  if( !((dial_ptr->attrib & DIAL_ATTRIB_GREAT) &&
        (strlen((char *) dial_ptr->num) == dial_ptr->len)))
  {
    ASSERT(0);
    return FALSE;
  }

  /* init srch_pb */
  dd_val_ptr->srch_pb = -1;

  /* copy dial string from data_ptr to dd_val_ptr       */
  dd_val_ptr->dial_str = *dial_ptr;
  (void) strlcpy((char *) dd_val_ptr->str,
                     (char *) dial_ptr->num, 
                     MAX_LINE_SIZE);
 
  /* set dd_val dial_str.num point to dd_val's str
     byte array.                                        */
  dd_val_ptr->dial_str.num = dd_val_ptr->str;

  /* parse mem and <n>                                  */
  num_ptr = dd_val_ptr->str;

  memset((char *) dd_val_ptr->mem, '\0', CPBS_VAL_LEN + 1);
  /* search for dsat_cpbs_storage_valstr to march the first 2 char 
     of D> string                                                       */
  if ( dial_ptr->len > CPBS_VAL_LEN )
  {
    for (index =0; dsat_cpbs_storage_valstr[index][0] != '\0'; index++)
    {
      if ( ( (UPCASE (dd_val_ptr->str[0])) == 
             dsat_cpbs_storage_valstr[index][0]) &&
           ( (UPCASE (dd_val_ptr->str[1])) == 
             dsat_cpbs_storage_valstr[index][1])
           )
      {
        /* The first two char in atd> string case-insenstive-match's one of 
           the supported phone book storage type.                         */
        (void) strlcpy((char *) dd_val_ptr->mem, 
                       (char *) dsat_cpbs_storage_valstr[index],
                       CPBS_VAL_LEN+1);
        num_ptr = &dd_val_ptr->str[CPBS_VAL_LEN];
        break;
      }
    }

  }

  atoi_ret = dsatutil_atoi((dsat_num_item_type *) &dd_val_ptr->index, 
                           num_ptr, 10);

  if(atoi_ret != ATOI_OK)
  {
    /* not a valid index */
    dd_val_ptr->index = -1;
  }

  return TRUE;
} /* me_dd_parse_dial_str */


/*===========================================================================

FUNCTION ME_DD_PROCESS_N

DESCRIPTION
  Process direct dialing when D> string contains a number.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : async call make to pbm to get a ph number
  DSAT_CMD_ERR_RSP : if there is a cme error
  DSAT_ERROR       : if there is an error
  DSAT_NO_RSP      : We find the number, and dial it out.
  DSAT_OK
SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsat_result_enum_type me_dd_process_n
(
  /* the ph book memory to get the number from */
  const byte     *mem,

  /* data for direct dialing processing */
  etsime_dd_type *dd_val_ptr,

  /* reporting error if there is any */
  dsm_item_type  *err_buff_ptr
)
{
  pbm_return_type     pbm_result;
  dsat_result_enum_type result = DSAT_OK;
  pbm_device_type       device;
  dsat_pbm_rec_id_type  record_id;
  pbm_phonebook_type    pb_id_local;

  /*-------- get pbm device type of the ph book memory ---------*/
  if (!get_pbm_device(mem, &device))
  {
    DS_AT_MSG3_ERROR("Wrong mem, %c%c, %d", mem[0], mem[1], mem[2]);
    /* can't map mem to a valid pbm device */
    return DSAT_ERROR;
  }

  memset(&pb_id_local,0x0,sizeof (pbm_phonebook_type));
  dsat_get_phonebook_id(&pb_id_local);
  pb_id_local.device_type = device;
 
  record_id = pbm_session_location_to_record_id(pb_id_local, dd_val_ptr->index);

  dsatme_cpb_pbm_read_record( record_id, &dd_val_ptr->record, &pbm_result);

  if (pbm_result == PBM_SUCCESS )
  {
    /* dial it out */
    dd_val_ptr->dial_str.num = (byte *) dd_val_ptr->record.number;
    dd_val_ptr->dial_str.len = (uint16) strlen((char *) dd_val_ptr->dial_str.num);
#ifdef FEATURE_DSAT_ETSI_MODE
    result = me_dd_dial(&dd_val_ptr->dial_str);
    /* prepare the last dial number for saving to the last dial ph book */
    if( (result == DSAT_NO_RSP) || (result == DSAT_ASYNC_CMD) )
    {
      /* the number was dialed, init the ld_rec */
      dsatme_ld_init_record(dd_val_ptr->record.number, dd_val_ptr->record.type, 
                                dd_val_ptr->record.text);
      dsatme_ld_save(dsat_get_current_subs_id(FALSE));
    }
#endif /* FEATURE_DSAT_ETSI_MODE */
  }
  else if (pbm_result == PBM_ERROR_NOT_FOUND)
  {
    cpb_process_pbm_error(pbm_result, NULL);
    result = DSAT_CMD_ERR_RSP;
  }
  else
  {
    result = cpb_pbm_read_handler_error_status(pbm_result);
  }
  return result;
  
} /* me_dd_process_n */


/*===========================================================================

FUNCTION ME_DD_PROCESS_MEM_N

DESCRIPTION
  Process direct dialing when D> string contains mem<n>.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : async call make to pbm to get a ph number
  DSAT_CMD_ERR_RSP : if there is a cme error
  DSAT_ERROR       : if there is an error

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type me_dd_process_mem_n
(
  /* data for direct dialing processing */
  etsime_dd_type *dd_val_ptr,

  /* reporting error if there is any */
  dsm_item_type  *err_buff_ptr
)
{
  pbm_device_type    pbm_device;
  pbm_phonebook_type pb_id_local;

  memset(&pb_id_local,0,sizeof (pbm_phonebook_type));
  /*-------- get file info of the memory mem            --------*/

  if (!get_pbm_device(dd_val_ptr->mem, &pbm_device))
  {
    DS_AT_MSG3_ERROR("Wrong mem, %c%c, %d", dd_val_ptr->mem[0],
                     dd_val_ptr->mem[1], dd_val_ptr->mem[2]);
    /* can't map mem to a valid pbm device */
    return DSAT_ERROR;
  }
  cb_expected = DD_PBM_FILE_INFO;
  /* error cases are also handled in the call-back */
  dsat_get_phonebook_id(&pb_id_local);
  pb_id_local.device_type = pbm_device;
  (void)pbm_session_extended_file_info_async(pb_id_local,
                               me_session_pbm_file_info_cb);
  return DSAT_ASYNC_CMD;
} /* me_dd_process_mem_n */


/*===========================================================================

FUNCTION ME_DD_PROCESS_STR

DESCRIPTION
  Process direct dialing when D> string contains str, pattern to 
  search names in the phone book.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : async call make to pbm to get a ph number
  DSAT_CMD_ERR_RSP : if there is a cme error
  DSAT_ERROR       : if there is an error

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsat_result_enum_type me_dd_process_str
(
  /* data for direct dialing processing */
  etsime_dd_type *dd_val_ptr,

  /* reporting error if there is any                                       */
  dsm_item_type  *err_buff_ptr
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  dsat_num_item_type cscs_val ;
  pbm_device_type       pbm_device;
  dsat_num_item_type    enum_val;
  dsat_num_item_type    temp_val;
   /* copy the content of str here since str is const                    */
  char  srh_str[MAX_LINE_SIZE+1];

  enum_val = (dsat_num_item_type) DSATETSI_EXT_ACT_CPBS_ETSI_IDX;

    temp_val = (dsat_num_item_type)dsatutil_get_val(enum_val,0,0,NUM_TYPE);
    dd_val_ptr->srch_pb = (int8) temp_val;
    dd_val_ptr->record.number[0] = '\0';
    memset(srh_str, '\0', MAX_LINE_SIZE + 1);
    /* Convert from TE character set, per +CSCA setting, to PBM's internal 
       chset, fail on inconvertible character */
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    (void)dsatutil_convert_chset( (const char *)dd_val_ptr->str,
                                  (dsati_chset_type)cscs_val,
                                  srh_str,
                                  ALPHA_PBM_UCS2, 
                                  MAX_LINE_SIZE + 1,
                                  FALSE );



  /* see if we are done with the last ph book */
  while(dsat_cpbs_storage_valstr[dd_val_ptr->srch_pb][0] != '\0')
  {

    /* search the ph book */
    if (!get_pbm_device(dsat_cpbs_storage_valstr[dd_val_ptr->srch_pb], &pbm_device))
    {
      DS_AT_MSG0_ERROR("Wrong mem");
      /* can't map mem to a valid pbm device */
      return DSAT_ERROR;
    }

    /* Perform search in the PBM */
    result = dd_pbm_find_handler((byte *)srh_str, dd_val_ptr, pbm_device);
    /* prepare the last dial number for saving to the last dial ph book */
    if( (result == DSAT_NO_RSP) || (result == DSAT_ASYNC_CMD) )
    {
      /* the number was dialed, init the ld_rec */
      dsatme_ld_init_record(dd_val_ptr->record.number, dd_val_ptr->record.type, 
                                dd_val_ptr->record.text);
      /* Save last dial record to phone book */
      dsatme_ld_save(dsat_get_current_subs_id(FALSE));
      return result;
    }

    /* Next Phone book */
    temp_val = (dsat_num_item_type) dsatutil_get_val(enum_val,0,0,NUM_TYPE);
    if (dd_val_ptr->srch_pb == (int8) temp_val)
    {
      /* we are done with current ph book, continue searching the 
         first non-current ph book */
      dd_val_ptr->srch_pb = 0;
      temp_val = (dsat_num_item_type) dsatutil_get_val(enum_val,0,0,NUM_TYPE);
      if(dd_val_ptr->srch_pb == (int8) temp_val)
      {
        dd_val_ptr->srch_pb++;
      }
    }
    else
    {
      /* continue searching the next non-current ph book */
      dd_val_ptr->srch_pb++;
      temp_val = (dsat_num_item_type) dsatutil_get_val(enum_val,0,0,NUM_TYPE);
      if(dd_val_ptr->srch_pb == (int8) temp_val)
      {
        dd_val_ptr->srch_pb++;
      }
    }
  }
  /* see if we are done with the last ph book */
  if(dsat_cpbs_storage_valstr[dd_val_ptr->srch_pb][0] == '\0')
  {
    /* reset dd_val.srch_pb */
    dd_val_ptr->srch_pb = -1;

    if(dd_val_ptr->record.number[0] == '\0')
    {
      /* we are done with all the phone book but not found a 
         match*/
      cpb_process_pbm_error(PBM_ERROR_NOT_FOUND, err_buff_ptr);
      result = DSAT_CMD_ERR_RSP;
    }
    else
    {
      /* we found a match, dial it out                    */
      dd_val_ptr->dial_str.num = (byte *) dd_val_ptr->record.number;
      dd_val_ptr->dial_str.len = (uint16) strlen((char *) dd_val_ptr->dial_str.num);
#ifdef FEATURE_DSAT_ETSI_MODE
      result = me_dd_dial(&dd_val_ptr->dial_str);
      
      /* prepare the last dial number for saving to the last dial ph book */
      if( (result == DSAT_NO_RSP) || (result == DSAT_ASYNC_CMD) )
      {
        /* the number was dialed, init the ld_rec */
        dsatme_ld_init_record(dd_val_ptr->record.number, dd_val_ptr->record.type, 
                                  dd_val_ptr->record.text);
        /* Save last dial record to phone book */
        dsatme_ld_save(dsat_get_current_subs_id(FALSE));
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
    }
  }

  return result;
} /* me_dd_process_str */
#ifdef FEATURE_DSAT_ETSI_MODE
/*===========================================================================

FUNCTION ME_DD_DIAL

DESCRIPTION
  Dial a number in etsi mode.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_NO_RSP           We find the number, and dial it out.
  DSAT_CMD_ERR_RSP      There is cme error.
  DSAT_ASYNC_CMD        The number is dialed out with an async cmd.

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsat_result_enum_type me_dd_dial
(
 /* number to dial */
  dsati_dial_str_type   * dial_ptr
)
{
  dsat_result_enum_type         result;
  dsati_mode_e_type             cur_mode;

  dsat_dial_str_enum_type       call_type;

  dsat_dial_modifier_info_type  modifier_info;

  /*-------- check if user has directed to abort the call --------*/
  if(dsatcmdp_abortable_state == ABORTING)
  {
    /* no dial, we are done */
    result = DSAT_OK;
  }
  else
  {

    /* error for empty dial number */
    if (dial_ptr->len == 0)
    {
      DS_AT_MSG0_HIGH("Dial number empty.");
      result = DSAT_ERROR;
    }
    else
    {
      cur_mode = dsatcmdp_get_current_mode();

      DS_AT_MSG2_LOW("dd num len: %d, attrib: %d",
                    dial_ptr->len,dial_ptr->attrib);
            DS_AT_MSG3_LOW("dd num is %c%c%c",
                    dial_ptr->num[0],dial_ptr->num[1],dial_ptr->num[2]);
            DS_AT_MSG3_LOW("dd num is %c%c%c",
                    dial_ptr->num[3],dial_ptr->num[4],dial_ptr->num[5]);
            DS_AT_MSG3_LOW("dd num is %c%c%c",
                    dial_ptr->num[6],dial_ptr->num[7],dial_ptr->num[8]);
            DS_AT_MSG3_LOW("dd num is %c%c%c",
                    dial_ptr->num[9],dial_ptr->num[10],dial_ptr->num[11]);

      /* Check if the call is a voice call. */
      if (dial_ptr->attrib & DIAL_ATTRIB_SEMICOLON)
      {
        /* if DIAL_ATTRIB_SEMICOLON is set in ETSI mode,
           this is a voice call.                            */

        DEBUG(DS_AT_MSG0_LOW("dd voice dial"););

        result =  dsatetsicall_voice_dial_cmd(dial_ptr);
      }
      else /* non-voice calls */
      {

        if ( dsatutil_strcmp_pkt_dial_str( dial_ptr->num, &cur_mode) )
        {
          dial_ptr->attrib |= DIAL_ATTRIB_PKT;
        }

        /* dial through the dial table, depending on mode, CS or PKT call */
        if ( dial_ptr->attrib & DIAL_ATTRIB_PKT )
        {
          DS_AT_MSG0_LOW("dd packet dial");
          call_type = DSAT_PKT_DIAL_TYPE;
        }
        else
        {
          DS_AT_MSG0_LOW("dd circuit dial");
          call_type = DSAT_DEFAULT_DIAL_TYPE;
        }

        memset(&modifier_info, 0, sizeof(dsat_dial_modifier_info_type));

        /* Fill out dial string modifiers */
        if (DSAT_OK != dsatutil_fill_modifier_info(dial_ptr, &modifier_info))
        {
          /* Problem with modifier setup */
          result = DSAT_ERROR;
        }
        else
        {

          if ( dsati_dial_table[cur_mode][call_type] == NULL )
          {
            DS_AT_MSG0_HIGH("unregistered call handler");
            result = DSAT_ERROR;
          }
          else
          {
#ifdef FEATURE_DSAT_ETSI_DATA
             if ( (IS_ETSI_MODE(cur_mode)) && 
                  (call_type == DSAT_PKT_DIAL_TYPE ) )
             {
               byte temp_cid = 1; /* default profile id*/
           
               /* Update CID if provided  */
               if ( ( modifier_info.modifier_present ) &&
                    ( modifier_info.modifier_info.cid.cid_val > 0) )
               {
                 temp_cid  =  modifier_info.modifier_info.cid.cid_val;
                 if ( !DSAT_IS_PROFILE_ID_WITHIN_RANGE(temp_cid))
                 {
                   DS_AT_MSG1_ERROR("Invalid CID %d",temp_cid);
                   return DSAT_ERROR;
                 }
               }
             }
#endif /* FEATURE_DSAT_ETSI_DATA */
            dsat_curr_abort_handler =
              dsati_dial_table[cur_mode][call_type](
                                         dial_ptr->num,
                                         dial_ptr->attrib & DIAL_ATTRIB_ASCII,
                                         &modifier_info,
                                         dsat_get_current_subs_id(FALSE));
            result = DSAT_NO_RSP;
          }
        }
      } /* (dial_ptr->attrib & DIAL_ATTRIB_SEMICOLON) */
    } /* if (dial_ptr->len == 0) */
  } /* if(dsatcmdp_abortable_state == ABORTING) */

  return result;
} /* me_dd_dial */
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DATA_ETSI_PIN
#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION CPIN_PROCESS_PIN_STATUS

DESCRIPTION
  This function is called after the MMGSDI fulfilled the request to
  get the pin status.  The MMGSDI reply is in cmd_status.

  If status shows the ME is pin locked, continue to verify the password.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD if the request is sent to mmgsdi to verify pin
                 or sent to pbm to set the storage type.
  DSAT_CMD_ERR_RSP if a me error happens when pbm executes the 
                   set storage request.  A cme error is returned
                   to TE.
  DSAT_ERROR if the set storage request is refused by PBM.
             It should not happen.  A system ERR is submitted.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpin_process_pin_status
(
   ds_at_cmd_status_type *cmd_info_ptr
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  mmgsdi_pin_status_enum_type pin_status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;
  boolean pin_locked = FALSE;
  mmgsdi_pin_enum_type  pin_id = MMGSDI_PIN1;
  mmgsdi_events_enum_type event = MMGSDI_MAX_EVT_ENUM;
  byte *pin = NULL;

  cb_cmd_e_type handler = CB_NONE;
  DS_AT_MSG1_HIGH("Command id = %d",cmd_info_ptr->cmd_id);

  /* Ensure MMGSDI call was successful */
  if ( MMGSDI_SUCCESS != cmd_info_ptr->cmd_status )
  {
    DS_AT_MSG0_HIGH("MMGSDI call not successful");
    result = DSAT_ERROR;
  }
  else
  {
    pin_status = (mmgsdi_pin_status_enum_type)cmd_info_ptr->cmd_data;
    DS_AT_MSG2_HIGH("command id = %d apps_id = %d",cmd_info_ptr->cmd_id,cmd_info_ptr->apps_id);
    /* Process the PIN state */
    switch (cmd_info_ptr->cmd_id)
    {
    case CPIN_MMGSDI_PIN1_STATUS:
      pin_id = MMGSDI_PIN1;
      handler = CPIN_MMGSDI_PIN1_VERIFY;
      event = MMGSDI_PIN1_EVT;
      pin_locked = (cmd_info_ptr->cmd_data != (int16)MMGSDI_PIN_DISABLED);
      break;
      
    case CPIN_MMGSDI_PIN2_STATUS:
      pin_id = MMGSDI_PIN2;
      event = MMGSDI_PIN2_EVT;
      handler = CPIN_MMGSDI_PIN2_VERIFY;
      pin_locked = (cmd_info_ptr->cmd_data != (int16)MMGSDI_PIN_DISABLED);
      break;

    case CLCK_MMGSDI_PIN1_STATUS:
      result = dsatetsicmif_report_clck (NULL, cmd_info_ptr, (sys_modem_as_id_e_type)(cmd_info_ptr->apps_id));
      break;
      
    default:
      DS_AT_MSG1_MED("Invalid PIN Status cmd_id: %d",cmd_info_ptr->cmd_id);
      result = DSAT_ERROR;
      break;
    }
  }
  
  if (DSAT_ASYNC_CMD == result)
  {
    /* Update PIN state tracking */
    if (DSAT_OK != cpin_track_pin_status (event, pin_status,cmd_info_ptr->apps_id))
    {
      DS_AT_MSG0_ERROR("Problem updating PIN status");
      result = DSAT_ERROR;
    }
    else
    {
      mmgsdi_data_type pin_data;

      pin_data.data_ptr = (uint8 *)dsatutil_get_val(
               DSATETSI_EXT_ACT_CPIN_ETSI_IDX,cmd_info_ptr->apps_id,0,MIX_STR_TYPE);

#ifdef FEATURE_DSAT_CUST
     #error code not present
#else
     pin_data.data_len = (mmgsdi_len_type)strlen((char*) dsatutil_get_val(
               DSATETSI_EXT_ACT_CPIN_ETSI_IDX,cmd_info_ptr->apps_id,0,MIX_STR_TYPE));
#endif /* FEATURE_DSAT_CUST */

      /* if PIN locked, verify the password (if provided) */
      if( pin_locked && (0 != pin_data.data_ptr[0]) && (0xFF != pin_data.data_ptr[0]) )
      {
        mmgsdi_return_enum_type mmgsdi_status;

        /* verify the pin, this will unlock the pin lock if the password
           is correct */
        VERIFY_PIN( dsat_qcsimapp_info[cmd_info_ptr->apps_id].session_id,
                    pin_id, pin_data, handler );

        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          result = DSAT_ASYNC_CMD;
        }
        else
        {
          result = DSAT_ERROR;
        }

        /* clear the password, we are not allowed to maintain a copy
           of the pin code */
        dsatutil_reset_password(pin, PIN_MAX_LEN+1);
      }
    }
  }
  else
  {
    /* If not in user-initiated command, suppress error return code. */
    /* This avoids spuriour ERRORs appearing on TE during init.      */
    if (!dsatme_pending_mmgsdi_command[cmd_info_ptr->apps_id])
    {
      result = DSAT_ASYNC_CMD;
    }
  }
  
  /* Flag completion of user-initiated GSDI action */
  dsatme_pending_mmgsdi_command[cmd_info_ptr->apps_id] = FALSE;
  
  return result;
} /* cpin_process_pin_status */


/*===========================================================================

FUNCTION CPIN_VERIFY_PIN_STATUS

DESCRIPTION
  Process the pin verification status.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_CMD_ERR_RSP : if the data parameter was not correct
    DSAT_ERROR :       if there was any problem in executing the command 
    DSAT_OK :          if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpin_verify_pin_status
(
   ds_at_cmd_status_type *cmd_info_ptr
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_apps_id_e_type     apps_id = cmd_info_ptr->apps_id;
  mmgsdi_return_enum_type mmgsdi_status = cmd_info_ptr->cmd_status;
  mmgsdi_service_enum_type service = MMGSDI_NONE;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  DS_AT_MSG3_HIGH("Status = %d, Command = %d Apps id %d",mmgsdi_status,cmd_info_ptr->cmd_id,apps_id);

  if (!IS_VALID_GW_APPS_ID(apps_id))
  {
    DS_AT_MSG1_ERROR("Invalid APPS ID %d",apps_id);
    return DSAT_ASYNC_EVENT;
  }

  me_ms_val = dsat_get_base_addr_per_session(apps_id, FALSE);

  if (dsat_qcsimapp_info[apps_id].app_type == MMGSDI_APP_SIM)
  {
    service = MMGSDI_GSM_SRV_FDN;
  }
  else
  {
    service = MMGSDI_USIM_SRV_FDN;
  }
  /* Flag completion of user-initiated GSDI action */
  dsatme_pending_mmgsdi_command[apps_id] = FALSE;
  /* Clear flag for manual state change and return */
  if (0 != (me_ms_val->dsat_pin_required & (uint32)DSAT_CPIN_STATE_CHANGE_PENDING))
  {
    me_ms_val->dsat_pin_required &= (uint32)~((uint32)DSAT_CPIN_STATE_CHANGE_PENDING);
  }
  /* See if GSDI call was unsuccessful */
  switch (mmgsdi_status)
  {
  case MMGSDI_SUCCESS:
    /* Supplied PIN was correct so update PIN state tracking */
    switch (cmd_info_ptr->cmd_id)
    {
    case CPIN_MMGSDI_PIN1_VERIFY:
      if (DSAT_OK !=
          cpin_track_pin_status ( MMGSDI_PIN1_EVT, MMGSDI_PIN_ENABLED_VERIFIED , apps_id))
      {
        DS_AT_MSG0_ERROR("Problem verifying PIN1 status");
        return DSAT_ERROR;
      }
      break;
      
    case CPIN_MMGSDI_PIN2_VERIFY:
      if (DSAT_OK !=
          cpin_track_pin_status ( MMGSDI_PIN2_EVT, MMGSDI_PIN_ENABLED_VERIFIED, apps_id ))
      {
        DS_AT_MSG0_ERROR("Problem verifying PIN2 status");
        return DSAT_ERROR;
      }
      break;

    case CLCK_MMGSDI_FDN_LOCK:
      if (DSAT_OK !=
          cpin_track_pin_status ( MMGSDI_PIN2_EVT, MMGSDI_PIN_ENABLED_VERIFIED, apps_id ))
      {
        DS_AT_MSG0_ERROR("Problem verifying PIN2 status");
        return DSAT_ERROR;
      }

      if(MMGSDI_SUCCESS == mmgsdi_session_enable_service(
                                cmd_info_ptr->session_id,
                                service,
                                dsatme_mmgsdi_perso_req_cb,
                                (uint32)CLCK_MMGSDI_FDN_ENABLE)) 
      {
        result = DSAT_ASYNC_EVENT; 
      }
      else 
      { 
        result = DSAT_ERROR; 
      }
      break;

    case CLCK_MMGSDI_FDN_UNLOCK:
      if (DSAT_OK !=
          cpin_track_pin_status ( MMGSDI_PIN2_EVT, MMGSDI_PIN_ENABLED_VERIFIED, apps_id))
      {
        DS_AT_MSG0_ERROR("Problem verifying PIN2 status");
        return DSAT_ERROR;
      }

      if(MMGSDI_SUCCESS == mmgsdi_session_disable_service(
                                cmd_info_ptr->session_id,
                                service,
                                dsatme_mmgsdi_perso_req_cb,
                                (uint32)CLCK_MMGSDI_FDN_DISABLE)) 
      {
        result = DSAT_ASYNC_EVENT; 
      }
      else 
      { 
        result = DSAT_ERROR; 
      }
      break;

    case CLCK_MMGSDI_PIN1_VERIFY:
    case CPWD_MMGSDI_PIN1_CHANGE:
    case CPWD_MMGSDI_PIN2_CHANGE:
      /* Do nothing */      
      break;

    case CACM_MMGSDI_PIN2_VERIFY:
    case CAMM_MMGSDI_PIN2_VERIFY:
      result = dsatetsicall_aoc_verify_pin_handler (cmd_info_ptr);
      break;

    case CPUC_MMGSDI_PIN2_VERIFY:
      result = dsatetsicall_cpuc_mmgsdi_write (cmd_info_ptr);
      break;
      
    default:
      DS_AT_MSG1_HIGH("Invalid PIN Verify cmd_id: %d",
               cmd_info_ptr->cmd_id);
      return DSAT_ERROR;
    }
    break; /* End case MMGSDI_SUCCESS */

  case MMGSDI_INCORRECT_CODE:
    switch (cmd_info_ptr->cmd_id)
    {
    case CPIN_MMGSDI_PIN2_VERIFY:
    case CPBS_MMGSDI_PIN2_VERIFY:
    case CACM_MMGSDI_PIN2_VERIFY:
    case CAMM_MMGSDI_PIN2_VERIFY:
    case CPUC_MMGSDI_PIN2_VERIFY:
    case CLCK_MMGSDI_FDN_LOCK:
    case CLCK_MMGSDI_FDN_UNLOCK:
    case CPWD_MMGSDI_PIN2_CHANGE:
       /* If attempted verification of pin2 failed due to incorrect pin
          show this result in next +CPIN command response */
      me_ms_val->dsat_mask_pin2_state = FALSE;
      break;
    default:
      break;
    }
    result = dsat_send_cme_error (DSAT_CME_INCORRECT_PASSWORD);
    break;

  case MMGSDI_CODE_BLOCKED:
    switch (cmd_info_ptr->cmd_id)
    {
    case CPIN_MMGSDI_PIN1_VERIFY:
    case CLCK_MMGSDI_PIN1_VERIFY:
    case CPWD_MMGSDI_PIN1_CHANGE:
      result = 
        dsat_send_cme_error ( DSAT_CME_SIM_PUK_REQUIRED );
      break;
          
    case CPIN_MMGSDI_PIN2_VERIFY:
    case CPBS_MMGSDI_PIN2_VERIFY:
    case CACM_MMGSDI_PIN2_VERIFY:
    case CAMM_MMGSDI_PIN2_VERIFY:
    case CPUC_MMGSDI_PIN2_VERIFY:
    case CLCK_MMGSDI_FDN_LOCK:
    case CLCK_MMGSDI_FDN_UNLOCK:
    case CPWD_MMGSDI_PIN2_CHANGE:
      /* If attempted verification of pin2 failed due to incorrect pin
         show this result in next +CPIN command response */
      me_ms_val->dsat_mask_pin2_state = FALSE;
      result = 
        dsat_send_cme_error ( DSAT_CME_SIM_PUK2_REQUIRED );
      break;

    default:
      DS_AT_MSG1_HIGH("Invalid PIN Verify cmd_id: %d",
              cmd_info_ptr->cmd_id);
      return DSAT_ERROR;
    }
    break;

  case MMGSDI_WARNING_NO_INFO_GIVEN:
    /* Assume this corresponds to state requested same as current */
    break;

  case MMGSDI_INCOMPAT_PIN_STATUS:
    /* +CPWD ME facility pin password change attempted when pin disabled */ 
    if ((cmd_info_ptr->cmd_id == (uint16)CPWD_MMGSDI_PIN1_CHANGE)||
        (cmd_info_ptr->cmd_id == (uint16)CPWD_MMGSDI_PIN2_CHANGE))
    {
      DS_AT_MSG0_HIGH("Changing password when pin disabled prohibited");
      result = dsat_send_cme_error (DSAT_CME_OP_NOT_ALLOWED);
    }
    break;
      
  case MMGSDI_ERROR:
  case MMGSDI_ERROR_NO_EVENT_NEEDED:
    result = dsat_send_cme_error (DSAT_CME_SIM_FAILURE);
    break;

  default:
    DS_AT_MSG1_HIGH("Unsupported MMGSDI event received: %d",
              mmgsdi_status);
    result = dsat_send_cme_error (DSAT_CME_SIM_FAILURE);
    break;
  }

  return result;
} /* cpin_verify_pin_status */
#endif /* FEATURE_MMGSDI */

#endif /* FEATURE_DATA_ETSI_PIN */

#ifdef FEATURE_DATA_ETSI_PIN
/*===========================================================================

FUNCTION  dsatetsicmif_report_clck_FDN_STATUS

DESCRIPTION
  This function reports the CLCK FDN status received from GSDI.

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
LOCAL dsat_result_enum_type dsatme_report_clck_fdn_status(ds_at_cmd_status_type *cmd_info_ptr)
{
  dsat_result_enum_type result = DSAT_OK;
  mmgsdi_return_enum_type  mmgsdi_status = cmd_info_ptr->cmd_status;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  if (!IS_VALID_GW_APPS_ID(cmd_info_ptr->apps_id))
  {
    DS_AT_MSG1_ERROR("Invalid APPS ID %d",cmd_info_ptr->apps_id);
    return DSAT_ASYNC_EVENT;
  }

  me_ms_val = dsat_get_base_addr_per_session(cmd_info_ptr->apps_id, FALSE);

  /* Flag completion of user-initiated GSDI action */
  dsatme_pending_mmgsdi_command[cmd_info_ptr->apps_id] = FALSE;

  /* See if GSDI call was unsuccessful */
  switch (mmgsdi_status)
  {
    case MMGSDI_SUCCESS:
      /* Supplied PIN was correct so update PIN state tracking */
    switch (cmd_info_ptr->cmd_id)
    {
      case CLCK_MMGSDI_FDN_ENABLE:
      case CLCK_MMGSDI_FDN_DISABLE:
        break;
      case CLCK_MMGSDI_FDN_QUERY:
        {
          dsm_item_type *res_buff_ptr;
          res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
          res_buff_ptr->used = (word)
            snprintf((char*)res_buff_ptr->data_ptr,
                     res_buff_ptr->size,
                     "+CLCK: %d", cmd_info_ptr->cmd_data);
          dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
        }
        break;
      default:
        result = DSAT_ERROR;
        break;
    }
    break;

    case MMGSDI_INCORRECT_CODE:
      result = dsat_send_cme_error (DSAT_CME_INCORRECT_PASSWORD);
      break;

    case MMGSDI_CODE_BLOCKED:
      /* If attempted verification of pin2 failed due to incorrect pin
         show this result in next +CPIN command response */
      me_ms_val->dsat_mask_pin2_state = FALSE;
      result = 
        dsat_send_cme_error ( DSAT_CME_SIM_PUK2_REQUIRED );
      break;
    default:
      DS_AT_MSG1_ERROR(" CLCK query status is %d (NOT SUCCESS)",mmgsdi_status);
      result = DSAT_ERROR;
      break;
  }

  return result;
} /* dsatetsicmif_report_clck_fdn_status */

#endif /* FEATURE_DATA_ETSI_PIN */
#ifdef FEATURE_DSAT_CFUN_CARD_POWER_CTL

/*===========================================================================

FUNCTION CFUN_PROCESS_CARD_PWR_CTL_STATUS

DESCRIPTION
  This function is the handler for a failure response from MMGSDI to an
  attempt to set the SIM card power control. The callback will only post the 
  command to DS task on failure to set the card power so that an ERROR result
  might be delivered, else the event that follows (MMGSDI_CARD_INSERTED_EVT 
  for power-up, MMGSDI_CARD_ERROR_EVT for power-down) will trigger the OK 
  final result code.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   if .
  DSAT_CMD_ERR_RSP if .
    
SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type cfun_process_card_pwr_ctl_status
(
  ds_at_cmd_status_type *cmd_info_ptr
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;

  /* Ensure GSDI call was successful */
  if ( ( MMGSDI_SUCCESS != cmd_info_ptr->cmd_status ) &&
       ( MMGSDI_REFRESH_SUCCESS != cmd_info_ptr->cmd_status ))
  {
    SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE)
    DS_AT_MSG1_HIGH("MMGSDI SIM card power control call not successful: %d",
              cmd_info_ptr->cmd_status);
    if ( MMGSDI_RESTRICTED_IN_ONCHIP_MODE != cmd_info_ptr->cmd_status )
    {
      /* Report +CME error */
      result = dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
    }
    else
    {
      /* Report OK when OnChip SIM is activated */
      result = DSAT_OK;
    }
  }
  else
  {
    DS_AT_MSG0_HIGH("Unexpected result in MMGSDI SIM card power control hdlr");
  }

  return result;
} /* cfun_process_card_pwr_ctl_status */
#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */

/*===========================================================================
FUNCTION DSATME_EXEC_QCPBMPREF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to choose the application and
  Local or Global phone book type. A change in the Phone book type 
  comes into effect only for the subsequent Phone book AT commands.
  Allowed range of input is (0 - 3).

     Syntax of the command:
        $QCPBMPREF=<Preference>

<Preference>:
0 -- Slot 1 Global Phone Book
1 -- App 1 Local Phone Book
2 -- Slot 2 Gloobal Phone Book
3 -- App 2 Local Phone Book

Default: 0 (Slot 1 Global Phone Book)

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK          : success
    DSAT_ERROR       : if there was any problem in executing the command

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED */
dsat_result_enum_type dsatme_exec_qcpbmpref_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define DSAT_SLOT1_GPB 0
#define DSAT_APP1_LPB  1
#define DSAT_SLOT2_GPB 2
#define DSAT_APP2_LPB  3
#define DSAT_SLOT3_GPB 4
#define DSAT_APP3_LPB  5



  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type temp1;
  dsat_num_item_type    prev_pbmpref_val =
    (dsat_num_item_type)dsatutil_get_val(DSAT_VENDOR_QCPBMPREF_IDX,0,0,NUM_TYPE); 
  dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();
  /* Save previous in case of error restore it back */
#ifndef FEATURE_TRIPLE_SIM
  char *fmt_str[]={"0 -- Slot 1 Global Phone Book \n",
                   "1 -- Apps 1 Local  Phone Book \n",
                   "2 -- Slot 2 Global Phone Book \n",
                   "3 -- Apps 2 Local  Phone Book \n"};
#else
  char *fmt_str[]={"0 -- Slot 1 Global Phone Book \n",
                   "1 -- Apps 1 Local  Phone Book \n",
                   "2 -- Slot 2 Global Phone Book \n",
                   "3 -- Apps 2 Local  Phone Book \n",
                   "4 -- Slot 3 Global  Phone Book \n" ,
                   "5 -- Apps 3 Local  Phone Book \n" };
#endif /* FEATURE_TRIPLE_SIM */

  /* Process Write Command */
  if( tok_ptr->op == (NA|EQ|AR) )
  {
    result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );


    if ( DSAT_OK == result )
    {
      /* TODO: Verification of SLOT details must be checked 
         against QCSLOT.  */
      temp1 = (dsat_num_item_type)dsatutil_get_val(
                              DSAT_VENDOR_QCPBMPREF_IDX,0,0,NUM_TYPE);
      if ( temp1 == DSAT_SLOT1_GPB || temp1 == DSAT_SLOT2_GPB || temp1 == DSAT_SLOT3_GPB )
      {
        dsat_pb_id.prov_type   = PBM_PROVISION_NONE;
        dsat_pb_id.pb_category = PBM_GPB;
        switch(temp1)
        {
          case DSAT_SLOT1_GPB:
          {
            dsat_pb_id.slot_id     = (uint16)PBM_SLOT_1;
            break;
          }
  
          case DSAT_SLOT2_GPB:
          {
            dsat_pb_id.slot_id     = (uint16)PBM_SLOT_2;
            break;
          }
  
#ifdef FEATURE_TRIPLE_SIM
          case DSAT_SLOT3_GPB:
          {
            dsat_pb_id.slot_id     = (uint16)PBM_SLOT_3;
            break;
          }
#endif /* FEATURE_TRIPLE_SIM */
        }
      }
      else if ( temp1 == DSAT_APP1_LPB || temp1 == DSAT_APP2_LPB || temp1 == DSAT_APP3_LPB )
      {
        dsat_pb_id.pb_category = PBM_LPB;
        if ( IS_ETSI_MODE(current_mode) )
        {
          switch ( temp1 )
          {
            case DSAT_APP1_LPB:
            {
              if ( TRUE == dsat_qcsimapp_info[DSAT_APPS_ID_GW_PRIMARY].active )
              {
                dsat_pb_id.prov_type = PBM_PROVISION_GW_PRIMARY;
                dsat_pb_id.slot_id   = (uint16)dsat_qcsimapp_info[DSAT_APPS_ID_GW_PRIMARY].slot_id;
              }
              else
              {
                DS_AT_MSG1_ERROR(" Provision apps %d not ready ",DSAT_APPS_ID_GW_PRIMARY);
                result = DSAT_ERROR;
              }
              break;
            }

            case DSAT_APP2_LPB:
            {
              if ( TRUE == dsat_qcsimapp_info[DSAT_APPS_ID_GW_SECONDARY].active )
              {
                dsat_pb_id.prov_type = PBM_PROVISION_GW_SECONDARY;
                dsat_pb_id.slot_id   = (uint16)dsat_qcsimapp_info[PBM_PROVISION_GW_SECONDARY].slot_id;
              }
              else
              {
                DS_AT_MSG1_ERROR(" Provision apps %d not ready ",PBM_PROVISION_GW_SECONDARY);
                result = DSAT_ERROR;
              }
              break;
            }

#ifdef FEATURE_TRIPLE_SIM
            case DSAT_APP3_LPB:
            {
              if ( TRUE == dsat_qcsimapp_info[DSAT_APPS_ID_GW_TERTIARY].active )
              {
                dsat_pb_id.prov_type = PBM_PROVISION_GW_SECONDARY;
                dsat_pb_id.slot_id   = (uint16)dsat_qcsimapp_info[DSAT_APPS_ID_GW_TERTIARY].slot_id;
              }
              else
              {
                DS_AT_MSG1_ERROR(" Provision apps %d not ready ",DSAT_APPS_ID_GW_TERTIARY);
                result = DSAT_ERROR;
              }
              break;
            }
#endif /* FEATURE_TRIPLE_SIM */
          }
        }
        else                 /* CDMA Mode */
        {
          if ( temp1 == DSAT_APP1_LPB )
          {
            dsat_pb_id.prov_type = PBM_PROVISION_1X_PRIMARY;
            dsat_pb_id.slot_id   = (uint16)dsat_qcsimapp_info[DSAT_APPS_ID_1X_PRIMARY].slot_id;
          }
        }
      }
      else
      {
        DS_AT_MSG1_ERROR("Invalid Preference %d not supported",temp1);
        result = DSAT_ERROR;
      }
    }
  }  /* Process Test Command */
  else if ( tok_ptr->op == (NA|EQ|QU))
  {
    uint16 index = 0;
#ifdef FEATURE_TRIPLE_SIM
    uint8 max_pbk = 6;
#else
    uint8 max_pbk = 4;
#endif /* FEATURE_TRIPLE_SIM */

    res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
#ifdef FEATURE_TRIPLE_SIM
                                              "$QCPBMPREF:(0-5)\n"
#else
                                              "$QCPBMPREF:(0-3)\n"
#endif /* FEATURE_TRIPLE_SIM */
                                        );

    for (index = 0; index < max_pbk; ++index)
    {
      res_buff_ptr->used += (word)snprintf(
                            (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                            res_buff_ptr->size - res_buff_ptr->used,
                            "%s",fmt_str[index]);
      
    }
  }  /* Process Read Command */
  else if ( tok_ptr->op == (NA|QU))
  {
    dsat_num_item_type index;
    index = (dsat_num_item_type)dsatutil_get_val(
                               DSAT_VENDOR_QCPBMPREF_IDX,0,0,NUM_TYPE);
    res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "$QCPBMPREF: %d \n%s",
                                               (dsat_num_item_type)dsatutil_get_val(
                                               DSAT_VENDOR_QCPBMPREF_IDX,0,0,NUM_TYPE),
                                               fmt_str[index]);
  }
  else
  {
    DS_AT_MSG0_ERROR("Syntax Error in $QCPBMPREF command");
    return DSAT_ERROR;
  }

  if ( DSAT_ERROR == result )
  {
    /* Restore PBM pref */
    DSATUTIL_SET_VAL(DSAT_VENDOR_QCPBMPREF_IDX,0,0,0,prev_pbmpref_val,NUM_TYPE)

  }
  return result;
}/* dsatme_exec_qcpbmpref_cmd */

#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_DATA_ETSI_PIN
#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================

FUNCTION  ME_CHOOSE_CME_ERROR

DESCRIPTION
  Determines from mask which CME ERROR should be returned to indicate PUK set.

DEPENDENCIES
  None

RETURN VALUE
  dsat_cme_error_e_type per mask given

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_cme_error_e_type me_choose_cme_error
(
  dsat_cpin_required_e_type mask
)
{
  if ( (uint32)mask & ((uint32)DSAT_CPIN_NET_PIN_REQUIRED 
     ) )
  {
    return DSAT_CME_NET_PERSON_PUK_REQUIRED;
  }
  else if ( (uint32)mask & ((uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED 
          ) )
  {
    return DSAT_CME_NET_SUBSET_PERSON_PUK_REQUIRED;
  }
  else if ( (uint32)mask & ((uint32)DSAT_CPIN_SP_PIN_REQUIRED 
          ) )
  {
    return DSAT_CME_SP_PERSON_PUK_REQUIRED;
  }
  else if ( (uint32)mask & ((uint32)DSAT_CPIN_CORP_PIN_REQUIRED 
          ) )
  {
    return DSAT_CME_CORP_PERSON_PUK_REQUIRED;
  }
  else if ( (uint32)mask & ((uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED 
          ) )
  {
    return DSAT_CME_PH_FSIM_PUK_REQUIRED;
  }
  else
  {
    return DSAT_CME_NO_ERROR;
  }
} /* me_choose_cme_error */

#endif /* FEATURE_MMGSDI_PERSONALIZATION */
#endif /* FEATURE_DATA_ETSI_PIN */

/* ----------------------------------------------------------------------------
   FUNCTION:      DSATME_MMGSDI_WRITE_TRANSPARENT_CB

   DESCRIPTION:
     This function provides internal callback function pointer to internal
     processing that MMGSDI does not care for the callback information returned

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
/* ARGSUSED */
void dsatme_mmgsdi_write_transparent_cb (mmgsdi_return_enum_type status, 
                                         mmgsdi_cnf_enum_type    cnf, 
                                         const mmgsdi_cnf_type        *cnf_ptr)
{
#ifdef FEATURE_DSAT_ETSI_MODE
  ds_cmd_type * cmd_buf;
#endif /* FEATURE_DSAT_ETSI_MODE */
  uint32 temp_client_ref; 
  ds_at_mmgsdi_sim_info_type *sim_info = NULL;

  ASSERT( cnf_ptr != NULL );

  DS_AT_MSG3_HIGH("Status=%d Session id = %d Client Data = %d",
     status,cnf_ptr->response_header.session_id,
     cnf_ptr->write_cnf.response_header.client_data);

  temp_client_ref = cnf_ptr->write_cnf.response_header.client_data;

  switch ( temp_client_ref )
  {
#ifdef FEATURE_DSAT_ETSI_MODE
    case CPOL_MMGSDI_SIM_WRITE:
    {
      cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_mmgsdi_sim_info_type), FALSE);
      sim_info = cmd_buf->cmd_payload_ptr;

      if ( status == MMGSDI_SUCCESS )
      {
        sim_info->client_ref = 
          cnf_ptr->write_cnf.response_header.client_data;
      }
      else if (status == MMGSDI_ACCESS_DENIED )
      {
        sim_info->client_ref = (uint32)CPOL_MMGSDI_SIM_ACCESS_DENIED;
      }    
      else
      {
        sim_info->client_ref = (uint32)CPOL_MMGSDI_SIM_ERROR;
      }
      /* There is no info that needs to be populated */
      cmd_buf->hdr.cmd_id  = DS_CMD_ATCOP_MMGSDI_SIM_INFO;
      sim_info->session_id = cnf_ptr->response_header.session_id;

      ds_put_cmd(cmd_buf);
    }
      break;
#endif /* FEATURE_DSAT_ETSI_MODE */
    default:
      DS_AT_MSG1_ERROR("Invalid client ref %d",temp_client_ref);
  }
} /* dsatme_mmgsdi_write_transparent_cb */

/* ----------------------------------------------------------------------------
   FUNCTION:      DSATME_MMGSDI_GET_FILE_ATTR_CB

   DESCRIPTION:
     This function is a call back function for MMGSDI GET FILE ATTR request. 

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
/* ARGSUSED */
void dsatme_mmgsdi_get_file_attr_cb 
( 
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf, 
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  ds_cmd_type            *cmd_buf  = NULL;
  ds_at_mmgsdi_sim_info_type *sim_info = NULL;
  uint32        temp_client_ref;

  ASSERT( cnf_ptr != NULL );

  temp_client_ref = cnf_ptr->get_file_attr_cnf.response_header.client_data;

  DS_AT_MSG2_HIGH(" ATCoP MMGSDI get file attr response for client ref %d Status %d",
                         temp_client_ref, status);
 
  cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_mmgsdi_sim_info_type), FALSE);
  sim_info = cmd_buf->cmd_payload_ptr;

  switch ( temp_client_ref )
  {
#ifdef FEATURE_DSAT_ETSI_MODE
    case CPOL_MMGSDI_SIM_FILE_ATTR:
    {
      if( status == MMGSDI_SUCCESS )
      {
        sim_info->client_ref = 
          cnf_ptr->get_file_attr_cnf.response_header.client_data;
      }
      else if (status == MMGSDI_ACCESS_DENIED )
      {
        sim_info->client_ref = (uint32)CPOL_MMGSDI_SIM_ACCESS_DENIED;
      }    
      else
      {
        sim_info->client_ref = (uint32)CPOL_MMGSDI_SIM_ERROR;
      }
    }
    break;
    case CUAD_MMGSDI_SIM_FILE_ATTR:
    {
      if( status == MMGSDI_SUCCESS )
      {
        sim_info->client_ref = 
          cnf_ptr->get_file_attr_cnf.response_header.client_data;
      }
      else
      {
        sim_info->client_ref = (uint32)CUAD_MMGSDI_SIM_ERROR;
      }
    }
    break;
#endif /* FEATURE_DSAT_ETSI_MODE */
    default:
      ds_release_cmd_buf(&cmd_buf);
      return;
  }/* switch( temp_client_ref) */

    /* Copy the file attribute cnf data and send the message to ATCOP */
  cmd_buf->hdr.cmd_id                     = DS_CMD_ATCOP_MMGSDI_SIM_INFO;
  sim_info->session_id = cnf_ptr->response_header.session_id;
  
  sim_info->cmd_status = status;
  (void) dsatutil_memscpy((void*)&sim_info->file_attr,
          sizeof(sim_info->file_attr),
          (void*)&cnf_ptr->get_file_attr_cnf.file_attrib,
         sizeof(sim_info->file_attr));

  ds_put_cmd(cmd_buf);
}/* dsatme_mmgsdi_get_file_attr_cb */

#endif /* FEATURE_MMGSDI */

/*-------------------------------------------------------------------------
            Function Definitions: Internal functions
-------------------------------------------------------------------------*/

#ifdef FEATURE_DS_OMH
/*===========================================================================

FUNCTION init_uim_device_model

DESCRIPTION
  This function initializes the Device Model information elementary file

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void init_uim_device_model
(
  void
)
{
#define UIM_CDMA_MODEL_CHAR_LEN     1
#define UIM_CDMA_MODEL_LANG_LEN     1
#define UIM_CDMA_MODEL_PROD_LEN     32
#define UIM_CDMA_MODEL_MFGR_LEN     32
#define UIM_CDMA_MODEL_VER_LEN      60

 mmgsdi_write_data_type write_data;  
 nv_stat_enum_type nv_status;
 static char model_info[UIM_CDMA_MODEL_PROD_LEN] = "Gobi 2000";
 static char mfg_info[UIM_CDMA_MODEL_MFGR_LEN] = "QUALCOMM INCORPORATED";
 byte device_model[126];
 byte offset = 0;
 byte len;
 
  memset(device_model, 0xFF, 126);

  /*-----------------------------------------------------------------------
  Set Character Encoding and Language Indicator.
  -----------------------------------------------------------------------*/
  device_model[offset] = 0x00; 
  offset += UIM_CDMA_MODEL_CHAR_LEN;

  device_model[offset] = 0x01;
  offset += UIM_CDMA_MODEL_LANG_LEN;

  /*-----------------------------------------------------------------------
  Read the stored model info from the NV.
  -----------------------------------------------------------------------*/
  nv_status = dsatutil_get_nv_item(NV_UNDP_HSU_PRODSTR_I, &ds_nv_item);
  if( nv_status == NV_DONE_S )
  {
    (void) strlcpy((char *) device_model + offset,
                       (const char*)ds_nv_item.undp_hsu_prodstr,
                       UIM_CDMA_MODEL_PROD_LEN);
  }
  if (( nv_status != NV_DONE_S ) || 
      ( 0 == strlen((char*)ds_nv_item.undp_hsu_prodstr)))
  {
    /* Query NV for model identification */
    nv_status = dsatutil_get_nv_item(NV_MOB_MODEL_I, &ds_nv_item);
    if( nv_status == NV_DONE_S )
    {
      (void) dsatutil_itoa(ds_nv_item.mob_model, (unsigned char*)model_info, 10);
      (void) strlcpy((char *) device_model + offset,
                         (const char*)model_info,
                         UIM_CDMA_MODEL_PROD_LEN);
    } else {
      /* Use global model variable instead */
      (void) strlcpy((char *) device_model + offset,
                         (const char*)model_info,
                         UIM_CDMA_MODEL_PROD_LEN);
    }
  }
  if((len = strlen((char *) device_model + offset)) < UIM_CDMA_MODEL_PROD_LEN)
      device_model[offset + len] = 0xFF;
  offset += UIM_CDMA_MODEL_PROD_LEN;


  /*-----------------------------------------------------------------------
  Read the stored MFR. info from the NV.
  -----------------------------------------------------------------------*/
  if (NV_DONE_S == dsatutil_get_nv_item(NV_UNDP_HSU_MFGSTR_I, &ds_nv_item)) {
    strlcpy((char *) device_model + offset,
                (const char*)ds_nv_item.undp_hsu_mfgstr,
                UIM_CDMA_MODEL_MFGR_LEN);
  }
  else
  {
    strlcpy((char *) device_model + offset,
                (const char*)mfg_info,
                UIM_CDMA_MODEL_MFGR_LEN);
  }
  if((len = strlen((char *) device_model + offset)) < UIM_CDMA_MODEL_MFGR_LEN)
      device_model[offset + len] = 0xFF;
  offset += UIM_CDMA_MODEL_MFGR_LEN;

  /*-----------------------------------------------------------------------
  Read the version information.
  -----------------------------------------------------------------------*/
  snprintf((char *) device_model + offset, 
                 UIM_CDMA_MODEL_VER_LEN, "%s", 
                 (char *) ver_dir);
  if((len = strlen((char *) device_model + offset)) < UIM_CDMA_MODEL_VER_LEN)
      device_model[offset + len] = 0xFF;

  /*-----------------------------------------------------------------------
  Send the write request of Device Model information using MMGSDI API  
  -----------------------------------------------------------------------*/
  write_data.data_len = 126;
  write_data.data_ptr = device_model;
  if(MMGSDI_SUCCESS != mmgsdi_write_transparent(dsatme_mmgsdi_client_id, 
                         MMGSDI_SLOT_AUTOMATIC, 
                         MMGSDI_CDMA_MODEL, 
                         0, write_data, 
                         (mmgsdi_callback_type)dsatme_mmgsdi_write_cb,
                         MMGSDI_CDMA_RUIM_OP_WRITE))
  {
    DS_AT_MSG0_ERROR(" MMGSDI rejected setting of Device Model information");
  }
  return;
} /* init_uim_device_model */
#endif /* FEATURE_DS_OMH */

/*===========================================================================
FUNCTION     DSATME_REQ_MMGSDI_GET_FILE_ATTR

DESCRIPTION:
  This function posts a command to MMGSDI to get the file attributes 
  asynchronously.

DEPENDENCIES:
  None

LIMITATIONS:
  NONE

RETURN VALUE:
  TRUE:  If successfully posted to MMGSDI.
  FALSE: If failed to post to MMGSDI.

SIDE EFFECTS:
  None
=========================================================================== */
boolean dsatme_req_mmgsdi_get_file_attr 
( 
  cb_cmd_e_type       cb_cmd,
  dsat_apps_id_e_type apps_id 
)
{
  mmgsdi_file_enum_type      file_name = MMGSDI_NO_FILE_ENUM;
  mmgsdi_client_data_type    client_ref = 0;
  mmgsdi_access_type         file_access;

  if ( cb_cmd == CB_NONE )
  {
    DS_AT_MSG0_ERROR("Get File attributes request on an invalid file ");
    return FALSE;
  }
  /* convert command call back to MMGSDI file name */
#ifdef FEATURE_DSAT_ETSI_MODE
  etsime_map_cmd_to_file(&file_name, cb_cmd, apps_id );
#endif /* FEATURE_DSAT_ETSI_MODE */
  /* Update the client reference and the file access details */
  client_ref = (mmgsdi_client_data_type)cb_cmd;
  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_name;

  /*Asynchronous call to MMGSDI to get file attributes */
  if (MMGSDI_SUCCESS == mmgsdi_session_get_file_attr_ext(
               dsat_qcsimapp_info[apps_id].session_id,
          file_access, 
          (mmgsdi_callback_type) dsatme_mmgsdi_get_file_attr_cb,
          client_ref,
          TRUE ))
  {
    DS_AT_MSG1_HIGH("Succesfully posted to get attributes for file %d",file_name);
    return TRUE;
  }
  else
  {
    DS_AT_MSG1_ERROR("Failed to post get attributes for file %d",file_name);
    return FALSE;
  }

}/* dsatme_req_mmgsdi_get_file_attr */

#ifdef FEATURE_DSAT_ETSI_MODE
/*==============================================================================
FUNCTION  DSATME_MMGSDI_CPOL_GET_FILE_ATTR_HANDLER

DESCRIPTION
  Handles the asynchronous read response for UIM_USIM_ONS or UIM_GSM_ONS.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_cpol_get_file_attr_handler
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
)
{
  dsm_item_type *res_buff_ptr;
  int num_rec;
  dsat_result_enum_type result = DSAT_NO_RSP;
  mmgsdi_access_type  file_access;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  
  me_ms_val = dsat_get_base_addr_current_session();

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = me_ms_val->dsat_sim_filename_plmn_selector;


  if (CHECK_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX,0,DSAT_PENDING_CPOL_WRITE))
  {
    if (me_ms_val->dsat_plmn_wact_supported )
    {
      num_rec = 
        sim_info->file_attr.file_size/ PLMN_W_ACT_LEN;
    }
    else 
    {
      num_rec = 
        sim_info->file_attr.file_size / PLMN_LEN;
    }
    /* Validate the Index */
    if (CHECK_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX,0,DSAT_PENDING_CPOL_WRITE) && 
        (me_ms_val->dsat_cpol_write_params.index != CPOL_INVALID_INDEX &&
          (me_ms_val->dsat_cpol_write_params.index > num_rec || 
            me_ms_val->dsat_cpol_write_params.index < 1 )))
    {
      SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
      result = dsat_send_cme_error(DSAT_CME_INVALID_INDEX);
      return result;
    }
    /* Using MMGSDI Interface */
    if ( MMGSDI_SUCCESS == mmgsdi_session_read_transparent(
         dsat_qcsimapp_info[sim_info->apps_id].session_id,
         file_access, 
         0, 0, 
         (mmgsdi_callback_type) dsatme_mmgsdi_read_transparent_cb,
         (uint32)CPOL_MMGSDI_SIM_READ ))
    {
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
      result = dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
      return result;
    }
  }
  else if (CHECK_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX,0,DSAT_PENDING_CPOL_TEST))
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
    if (me_ms_val->dsat_plmn_wact_supported)
    {
      num_rec = sim_info->file_attr.file_size/ PLMN_W_ACT_LEN;
    }
    else
    {
      num_rec = sim_info->file_attr.file_size/ PLMN_LEN;
    }

    res_buff_ptr->used = (word)snprintf(
                     (char*)res_buff_ptr->data_ptr,
                     res_buff_ptr->size,
                     "+CPOL: (1-%d),(0-2)",
                     MIN(num_rec, SYS_USER_PPLMN_LIST_MAX_LENGTH));
    (void)dsatcmdp_send_respose_to_te(res_buff_ptr, DSAT_COMPLETE_RSP);
    result = DSAT_OK;
    SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
  }

  return result;
}/* dsatme_mmgsdi_cpol_get_file_attr_handler */

/*==============================================================================
FUNCTION  DSATME_MMGSDI_CUAD_GET_FILE_ATTR_HANDLER

DESCRIPTION
  Handles the asynchronous read response for File attributes for EF DIR.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_cuad_get_file_attr_handler
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
)
{

  dsat_result_enum_type result = DSAT_NO_RSP;
  mmgsdi_access_type  file_read_access;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  dsat_apps_id_e_type           apps_id;

  apps_id  = sim_info->apps_id;
  me_ms_val = dsat_get_base_addr_per_session(apps_id, FALSE);
  file_read_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_read_access.file.file_enum = MMGSDI_DIR;

  if (CHECK_PENDING(DSATETSI_EXT_ACT_CUAD_ETSI_IDX,0,DSAT_PENDING_CUAD_READ))
  {
    if( sim_info->client_ref != CUAD_MMGSDI_SIM_FILE_ATTR )
    {
      SET_PENDING(DSATETSI_EXT_ACT_CUAD_ETSI_IDX ,0, DSAT_PENDING_CUAD_NONE)
      result = dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
      return result;
    }
    if ( sim_info->file_attr.file_type == MMGSDI_LINEAR_FIXED_FILE )
    {
      me_ms_val->dsat_cuad_recs.num_of_rec = 
        sim_info->file_attr.file_info.linear_fixed_file.num_of_rec;
      me_ms_val->dsat_cuad_recs.rec_len = 
        sim_info->file_attr.file_info.linear_fixed_file.rec_len;
      me_ms_val->dsat_cuad_recs.curr_rec = 0;
    }
    else
    {
      me_ms_val->dsat_cuad_recs.num_of_rec = 0;
      me_ms_val->dsat_cuad_recs.curr_rec = 0;
      me_ms_val->dsat_cuad_recs.rec_len = 0;
      SET_PENDING(DSATETSI_EXT_ACT_CUAD_ETSI_IDX ,0, DSAT_PENDING_CUAD_NONE)
      result = dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
      return result;
    }
    DS_AT_MSG2_HIGH("In _cuad_get_file_attr_handler - num_of_recs %d - rec_len %d",
                  me_ms_val->dsat_cuad_recs.num_of_rec,me_ms_val->dsat_cuad_recs.rec_len);

    if(me_ms_val->dsat_cuad_recs.num_of_rec > me_ms_val->dsat_cuad_recs.curr_rec )
    {
      me_ms_val->dsat_cuad_recs.curr_rec++;
      if (MMGSDI_SUCCESS == mmgsdi_session_read_record(
                                    dsat_qcsimapp_info[apps_id].session_id,
                                    file_read_access,
                                    me_ms_val->dsat_cuad_recs.curr_rec, 
                                    me_ms_val->dsat_cuad_recs.rec_len,
                                    (mmgsdi_callback_type)dsatme_mmgsdi_read_record_cb,
                                    (mmgsdi_client_data_type)CUAD_MMGSDI_SIM_READ))
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        SET_PENDING(DSATETSI_EXT_ACT_CUAD_ETSI_IDX ,0, DSAT_PENDING_CUAD_NONE)
        result = dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
        return result;
      }
    }
    else
    {
      SET_PENDING(DSATETSI_EXT_ACT_CUAD_ETSI_IDX ,0, DSAT_PENDING_CUAD_NONE)
      result = dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
      return result;
    }

  }

  return result;
}/* dsatme_mmgsdi_cuad_get_file_attr_handler */

/*===========================================================================

FUNCTION  DSATME_FORMAT_CMD_RSP

DESCRIPTION
  Utilty function for formatting the input cmd_ptr input from integer to Hex and appending it to the output 
  res_buff_ptr[res_buff_ptr->used].
  
DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.
  
SIDE EFFECTS
  None
==============================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_format_cmd_rsp
(
  byte   *cmd_ptr,
  uint32 num_bytes,
  dsm_item_type *res_buff_ptr
)
{

  byte *curr_byte = NULL;
  byte *end_byte = NULL;
  byte *curr_char = NULL;
  uint16 num_chars = 0;
  dsm_item_type *curr_buff_ptr = NULL;

  boolean check_append = FALSE;


  ASSERT( (res_buff_ptr != NULL) && (cmd_ptr != NULL) );

  curr_buff_ptr = res_buff_ptr;
  curr_byte = cmd_ptr;

  num_chars = (uint16)(num_bytes << 1);

  if ( (curr_buff_ptr->used + (num_chars + 2)) > curr_buff_ptr->size )
  {
    check_append = TRUE;
  }

  for ( end_byte = curr_byte + num_bytes,
              curr_char = &curr_buff_ptr->data_ptr[curr_buff_ptr->used];
            curr_byte < end_byte;
            curr_byte++ )
  {
    /* Check to see if a new DSM item is required */
    if ( check_append )
    {
      /* Check that room for 4 more characters exists, chose 4 to save 
          having to check when '"' and terminating NULL are added later */
      curr_buff_ptr = dsatutil_append_dsm_item(
                        curr_buff_ptr,
                        curr_buff_ptr->size - 4);
      /* If buffer appended... */
      if ( curr_buff_ptr != res_buff_ptr )
      {
        /* Point curr_char to beginning of its data array */
        curr_buff_ptr->used = 0;
        curr_char = &curr_buff_ptr->data_ptr[curr_buff_ptr->used];
      }

     /* Appending 2 DSM items will never be required */
       check_append = FALSE;
     }
  
     if( *curr_byte < 16 )
     {
      *curr_char++ = '0';
     }
    curr_char = dsatutil_itoa( (uint32)*curr_byte, curr_char, 16 );
    curr_buff_ptr->used += 2;
  }
  
      *curr_char++ = '\"';
      res_buff_ptr->used++;
  
      *curr_char++ = '\0';
  
  return DSAT_OK;
}/* dsatme_format_cmd_rsp */


/*==============================================================================
FUNCTION  DSATME_MMGSDI_CUAD_READ_HANDLER

DESCRIPTION
  Handles the asynchronous read response for a record read on EF-DIR.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_cuad_read_handler
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
)
{
#define FIRST_RECORD 1

  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  dsm_item_type *res_buff_ptr = NULL;
  mmgsdi_access_type  file_read_access;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  dsat_apps_id_e_type           apps_id;

  apps_id     = sim_info->apps_id;
  
  me_ms_val = dsat_get_base_addr_per_session(apps_id, FALSE);

  file_read_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_read_access.file.file_enum = MMGSDI_DIR;
  
  ASSERT(me_ms_val->dsat_cuad_recs.curr_rec == sim_info->accessed_rec_num)
  	
  if (!(CHECK_PENDING(DSATETSI_EXT_ACT_CUAD_ETSI_IDX,0,DSAT_PENDING_CUAD_READ) ))
  {
    return result;
  }
  DS_AT_MSG2_HIGH("In _cuad_read_handler - curr_rec %d - status %d ",
                  me_ms_val->dsat_cuad_recs.curr_rec,sim_info->cmd_status);


  if(sim_info->cmd_status == MMGSDI_SUCCESS)
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    if(me_ms_val->dsat_cuad_recs.curr_rec == FIRST_RECORD)
    {
      res_buff_ptr->used = (word)snprintf( (char*)res_buff_ptr->data_ptr, 
                                              res_buff_ptr->size, 
                                              "+CUAD: " );
    }

    res_buff_ptr->data_ptr[res_buff_ptr->used] = '\"';
    res_buff_ptr->used++;

    if(dsatme_format_cmd_rsp(&sim_info->data[0],
                              sim_info->data_len,
                              res_buff_ptr) == DSAT_ERROR)
    {
      dsm_free_packet(&res_buff_ptr);
      return DSAT_ERROR;
    }

    if(me_ms_val->dsat_cuad_recs.num_of_rec == me_ms_val->dsat_cuad_recs.curr_rec)
    {
      DSAT_SEND_RESPONSE(res_buff_ptr, DSAT_COMPLETE_RSP);  
      result = DSAT_OK;
      return result;
    }
    else
    {
      if(me_ms_val->dsat_cuad_recs.curr_rec == FIRST_RECORD)
      {
        DSAT_SEND_RESPONSE(res_buff_ptr, DSAT_START_OF_RSP);
      }
      else
      {
        DSAT_SEND_RESPONSE(res_buff_ptr, DSAT_INTERMEDIATE_RSP);
      }
    }

    if(me_ms_val->dsat_cuad_recs.num_of_rec > me_ms_val->dsat_cuad_recs.curr_rec)
    {
      me_ms_val->dsat_cuad_recs.curr_rec++;
      if (MMGSDI_SUCCESS == mmgsdi_session_read_record(
                                    dsat_qcsimapp_info[sim_info->apps_id].session_id,
                                    file_read_access,
                                    me_ms_val->dsat_cuad_recs.curr_rec,
                                    me_ms_val->dsat_cuad_recs.rec_len,
                                    (mmgsdi_callback_type)dsatme_mmgsdi_read_record_cb,
                                    (mmgsdi_client_data_type)CUAD_MMGSDI_SIM_READ))
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CUAD_NONE)
        result = dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
        return result;
      }
    }
  }
  else
  {
    return DSAT_ERROR;
  }

  return result;
}
/*==============================================================================
FUNCTION  DSATME_MMGSDI_CPOL_READ_HANDLER

DESCRIPTION
  Handles the asynchronous read response for UIM_USIM_ONS or UIM_GSM_ONS.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_cpol_read_handler
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
)
{
#ifdef FEATURE_DSAT_LTE
#define ACT_EUTRAN          0x40
#endif /* FEATURE_DSAT_LTE */
  int index;
  mmgsdi_plmn_id_list_type plmn_id_list;
  mmgsdi_plmn_id_type *plmn_id = NULL;
  dsat_me_cpol_read_param_s_type *read_params_ptr         = NULL;
  sys_user_pref_plmn_list_s_type *user_pref_networks = NULL;
  int i;
  int num_rec; 
  dsat_result_enum_type result = DSAT_OK;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  
  me_ms_val = dsat_get_base_addr_current_session();

  read_params_ptr    = dsat_get_cmd_mem_ptr();
  user_pref_networks = &read_params_ptr->user_pref_networks;
  user_pref_networks->length = 0;
  
  if (me_ms_val->dsat_plmn_wact_supported )
  {
    num_rec = 
      sim_info->data_len / PLMN_W_ACT_LEN;
  }
  else 
  {
    num_rec = 
      sim_info->data_len / PLMN_LEN;
  }
  if (CHECK_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX,0,DSAT_PENDING_CPOL_READ) )
  {
    read_params_ptr->end_index = 
   (int16)MIN(num_rec, SYS_USER_PPLMN_LIST_MAX_LENGTH);
  }
  
  /*
  ** Loop over the Preferred PLMN List and append each PLMN to the output 
  ** Preferred PLMN List. The output Preferred PLMN List size is minimum of
  ** input Preferred PLMN List size and SYS_USER_PPLMN_LIST_MAX_LENGTH.
  */
  for(i = 0; i < sim_info->data_len && 
             user_pref_networks->length < SYS_USER_PPLMN_LIST_MAX_LENGTH;)
  {
    user_pref_networks->info[user_pref_networks->length].plmn.identity[0]=
      sim_info->data[i++];
    user_pref_networks->info[user_pref_networks->length].plmn.identity[1]=
      sim_info->data[i++];
    user_pref_networks->info[user_pref_networks->length].plmn.identity[2]=
      sim_info->data[i++];
     
    user_pref_networks->info[user_pref_networks->length].access_tech = 0;
    
    if ( me_ms_val->dsat_plmn_wact_supported )  /* If Access Technology is supported */
    {
      /*
      ** Loop over the Preferred PLMN With Access Technology List and
      ** append each PLMN to the output Preferred PLMN List.  A PLMN
      ** that supports both GSM and UMTS access technologies may be
      ** added to the list twice depending upon the mode preference.
      */
      if ( !sys_plmn_id_is_undefined( 
              user_pref_networks->info[user_pref_networks->length].plmn) )
      {
#ifdef FEATURE_DSAT_LTE
        if (sim_info->data[i] & ACT_EUTRAN)
        {
          user_pref_networks->info[user_pref_networks->length].access_tech =
            user_pref_networks->info[user_pref_networks->length].access_tech |
            SYS_ACT_LTE_MASK;
        }
#endif /* FEATURE_DSAT_LTE */
        if (sim_info->data[i++] & ACT_UMTS)
        {
          user_pref_networks->info[user_pref_networks->length].access_tech =
            user_pref_networks->info[user_pref_networks->length].access_tech |
            SYS_ACT_UMTS_MASK;
        }
        if (sim_info->data[i] & ACT_GSM)
        {
          user_pref_networks->info[user_pref_networks->length].access_tech =
            user_pref_networks->info[user_pref_networks->length].access_tech |
            SYS_ACT_GSM_MASK;
        }
  
        if (sim_info->data[i++] & ACT_GSM_COMPACT)
        {
          user_pref_networks->info[user_pref_networks->length].access_tech =
            user_pref_networks->info[user_pref_networks->length].access_tech |
            SYS_ACT_GSM_COMPACT_MASK;
        }
  
      }
      else 
      {
        i += 2;
      }
    }  
    user_pref_networks->length++;
  }
  if (CHECK_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX,0,DSAT_PENDING_CPOL_READ) )
  {

    plmn_id_list.num_of_plmn_ids = user_pref_networks->length;
    plmn_id = (mmgsdi_plmn_id_type *)dsat_alloc_memory((plmn_id_list.num_of_plmn_ids)*
                                                   sizeof(mmgsdi_plmn_id_type ), FALSE);
    plmn_id_list.plmn_list_ptr = &plmn_id[0];
    i = 0;
    for (index = read_params_ptr->start_index -1; 
             (index < read_params_ptr->end_index && 
              index <= ( user_pref_networks->length - 1)) ; index++)
      {
      if (!sys_plmn_id_is_undefined(user_pref_networks->info[index].plmn))
      {
        (void) dsatutil_memscpy((void*)plmn_id->plmn_id_val,
                MMGSDI_PLMN_ID_SIZE,(void*)&user_pref_networks->info[index].plmn.identity[0],MMGSDI_PLMN_ID_SIZE);
        i++;
      }
      else
      {
        memset(plmn_id,0xFF,MMGSDI_PLMN_ID_SIZE); 
      }
      plmn_id->csg_id = 0xFFFFFFFF;
      plmn_id->rat = MMGSDI_RAT_NONE;

      plmn_id++; 
  }

  if( i == 0 )
  {
    if(plmn_id_list.plmn_list_ptr != NULL)
    {
        dsatutil_free_memory(plmn_id_list.plmn_list_ptr);
        plmn_id_list.plmn_list_ptr = NULL;
    }
      SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
      return DSAT_OK;
  }

    if( MMGSDI_SUCCESS == mmgsdi_session_get_operator_name_ext (
              dsat_get_current_gw_session_id(FALSE),
              plmn_id_list,
              MMGSDI_EONS_IGNORE_RPLMN_DISPLAY_COND,
              dsatme_mmgsdi_get_operator_name_cb,
              DSAT_MMGSDI_GET_PREF_NETS_NAME) )
    {
      DS_AT_MSG0_MED ( "Query EONS for Long and short names");
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
      DS_AT_MSG0_ERROR("mmgsdi_session_get_operator_name API call failed");
      dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
      result = DSAT_CMD_ERR_RSP;
        }
    if(plmn_id_list.plmn_list_ptr != NULL)
        {
      dsatutil_free_memory(plmn_id_list.plmn_list_ptr);
      plmn_id_list.plmn_list_ptr = NULL;
    }
  }
  else if (CHECK_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX,0,DSAT_PENDING_CPOL_WRITE))
  {
    result = dsatetsime_cpol_write_plmn_handler(user_pref_networks);
    SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
  }
  else
  {
    SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
    result = DSAT_ERROR;
  }

  return result;
}/* dsatme_mmgsdi_cpol_read_handler */
#endif /* FEATURE_DSAT_ETSI_MODE */


/*===========================================================================

FUNCTION DSATME_REPORT_PACSP_RESULT

DESCRIPTION
  This function generates the unsolicited/normal
  response for PACSP command.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsatme_report_pacsp_result 
( 
  dsat_apps_id_e_type  apps_id
)
{
  dsm_item_type * res_buff_ptr = NULL;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  
  me_ms_val = dsat_get_base_addr_per_session(apps_id, FALSE);

  if( CSP_PLMN_MODE_BIT_UNSET != me_ms_val->dsatme_plmn_mode_sel_bit )
  {
    /* Report PACSP to the terminal */
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
    if (NULL != res_buff_ptr)
    {
      DS_AT_MSG1_HIGH("Reporting PACSP result %d",me_ms_val->dsatme_plmn_mode_sel_bit);
      res_buff_ptr->used =
        (word) snprintf((char*)res_buff_ptr->data_ptr,
                              res_buff_ptr->size,
                              "+PACSP%d",
                              me_ms_val->dsatme_plmn_mode_sel_bit);
      /* Send the unsolicited response */
      dsatcmdp_send_urc_per_session(apps_id, res_buff_ptr, DSAT_COMPLETE_RSP);
    }
  }
}/* dsatme_report_pacsp_result */

/*===========================================================================
FUNCTION DSATME_GET_IMSI_INFO

DESCRIPTION
   This is an internal function, which return pointer to imsi info based on 
   qcsimappval . 
  
DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

dsat_mmgsdi_imsi_info_s_type *  dsatme_get_imsi_info(void)
{
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  me_ms_val = dsat_get_base_addr_current_session();

  return &me_ms_val->dsat_imsi_info;
}


/*===========================================================================

FUNCTION DSATME_MMGSDI_REQ_CB

DESCRIPTION
  This function is the callback function for the GSDI to return pin status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void dsatme_mmgsdi_perso_req_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
)
{
#if defined(FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS) 
  byte data = 0;
  mmgsdi_session_id_type application_id = 0;

  DS_AT_MSG3_HIGH("Session id = %d, Response type = %d, Status = %d",
    cnf_ptr->response_header.session_id,cnf_ptr->response_header.response_type,
    status);
  
  application_id = cnf_ptr->response_header.session_id;

  switch (cnf_ptr->response_header.response_type)
  {
    case MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF:
      data = cnf_ptr->session_app_capabilities_cnf.fdn_enabled;
      break;
    case MMGSDI_SESSION_PERSO_CNF:
      switch(cnf_ptr->session_perso_cnf.perso_action)
      {
        case MMGSDI_PERSO_DEACTIVATE:
          data = (byte)cnf_ptr->session_perso_cnf.perso_cnf_data.feature_data_cnf.num_retries;
          break;
        case MMGSDI_PERSO_GET_PERM_DISABLE_IND:
          data = (MMGSDI_SUCCESS ==
                  cnf_ptr->session_perso_cnf.response_header.mmgsdi_status) ? 1 : 0;
          break;
        case MMGSDI_PERSO_ACTIVATE:
          data = (MMGSDI_SUCCESS ==
                  cnf_ptr->session_perso_cnf.response_header.mmgsdi_status) ? 1 : 0;
          break;
        case MMGSDI_PERSO_GET_FEATURE_IND:
          if(MMGSDI_SUCCESS !=
                  cnf_ptr->session_perso_cnf.response_header.mmgsdi_status)
          {
            data = 0;
          }
          else
          {
            switch(clck_perso_fac)
            {
              case   DSAT_MMGSDI_PERSO_FAC_PN:  /* Network personalization */
                data = cnf_ptr->session_perso_cnf.perso_cnf_data.get_feature_ind_cnf.nw_ind_status;
                break;
              case   DSAT_MMGSDI_PERSO_FAC_PU:  /* Network subnet personalization */
                data = cnf_ptr->session_perso_cnf.perso_cnf_data.get_feature_ind_cnf.ns_ind_status;
                break;
              case   DSAT_MMGSDI_PERSO_FAC_PP:  /* Service provider personalization*/
                data = cnf_ptr->session_perso_cnf.perso_cnf_data.get_feature_ind_cnf.sp_ind_status;
                break;
              case   DSAT_MMGSDI_PERSO_FAC_PC:  /* Corporate Personalization */
                data = cnf_ptr->session_perso_cnf.perso_cnf_data.get_feature_ind_cnf.cp_ind_status;
                break;
              case   DSAT_MMGSDI_PERSO_FAC_PF:  /* Phone locked to SIM */
                data = cnf_ptr->session_perso_cnf.perso_cnf_data.get_feature_ind_cnf.sim_ind_status;
                break;
              default:
                break;
            }
          }
          break;
        default:
          break;
      }
      break;

    default:
      DS_AT_MSG1_ERROR("Not a valid mmgsdi cb rsp: %d", 
           cnf_ptr->response_header.response_type);
      break;
  }

  submit_cmd_status( (cb_cmd_e_type)cnf_ptr->response_header.client_data, 
                     status,
                     application_id,
                     data );

  cb_expected = CB_NONE;
#endif/*  defined(FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS) */
} /* dsatetsime_mmgsdi_req_cb */

/*===========================================================================

FUNCTION DSATME_SIM_MMGSDI_PERSO_REQ_CB

DESCRIPTION
  This function is the callback function for the MMGSDI to return pin status for SIMLOCK feature.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void dsatme_sim_mmgsdi_perso_req_cb
(
  simlock_result_enum_type status,
  const simlock_message_response_data_type  * resp_ptr,
  const void                                * user_data_ptr
)
{
#if defined(FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS) 
  dsat_perso_user_type *data_ptr;
  byte data                             = 0;
   
  data_ptr=(dsat_perso_user_type *)user_data_ptr;

  ASSERT( (NULL != resp_ptr) && (NULL != user_data_ptr) );
  
  DS_AT_MSG4_HIGH("msg type = %d,session_id = %d,cmd = %d,slot %d",resp_ptr->msg_type,
          (mmgsdi_session_id_type)data_ptr->session_id,
          (cb_cmd_e_type)data_ptr->perso_cmd,
           data_ptr->slot);

  switch (resp_ptr->msg_type)
  {
    case SIMLOCK_SET_LOCK_CK_MSG:
      data = (SIMLOCK_SUCCESS == status) ? TRUE : FALSE;
      break;
    case SIMLOCK_UNLOCK_DEVICE_MSG:
      data = (byte)resp_ptr->message.unlock_resp.curr_retries;
      break;
    case SIMLOCK_GET_STATUS_MSG:
      switch(clck_perso_fac)
      {
        case   DSAT_MMGSDI_PERSO_FAC_PN:  /* Network personalization */
         if(data_ptr->slot < SIMLOCK_SLOT_COUNT_MAX)
         {
           data = resp_ptr->message.get_status_resp.lock_info[data_ptr->slot].category_info[SIMLOCK_CATEGORY_3GPP_NW].category_status;
         }
         break;
        case   DSAT_MMGSDI_PERSO_FAC_PU:  /* Network subnet personalization */
         if(data_ptr->slot < SIMLOCK_SLOT_COUNT_MAX)
         {
           data = resp_ptr->message.get_status_resp.lock_info[data_ptr->slot].category_info[SIMLOCK_CATEGORY_3GPP_NS].category_status;
         }
         break;
        case   DSAT_MMGSDI_PERSO_FAC_PP:  /* Service provider personalization*/
         if(data_ptr->slot < SIMLOCK_SLOT_COUNT_MAX)
         {
           data = resp_ptr->message.get_status_resp.lock_info[data_ptr->slot].category_info[SIMLOCK_CATEGORY_3GPP_SP].category_status;
         }
         break;
        case   DSAT_MMGSDI_PERSO_FAC_PC:  /* Corporate Personalization */
         if(data_ptr->slot < SIMLOCK_SLOT_COUNT_MAX)
         {
           data = resp_ptr->message.get_status_resp.lock_info[data_ptr->slot].category_info[SIMLOCK_CATEGORY_3GPP_CP].category_status;
         }
         break;
        case   DSAT_MMGSDI_PERSO_FAC_PF:  /* Phone locked to SIM */
         if(data_ptr->slot < SIMLOCK_SLOT_COUNT_MAX)
         {
           data = resp_ptr->message.get_status_resp.lock_info[data_ptr->slot].category_info[SIMLOCK_CATEGORY_3GPP_SIM].category_status;
         }
         break;
        default:
         break;
      }
      break;
    default:
      DS_AT_MSG0_ERROR("Not a valid mmgsdi cb rsp"); 
      break;
  }
  
  submit_cmd_status( (data_ptr->perso_cmd), 
                     (mmgsdi_return_enum_type)status,
                     (data_ptr->session_id),
                     data );
  
  cb_expected = CB_NONE;
#endif/*  defined(FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS) */
}/*dsatetsime_mmgsdi_req_cb */

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION DSATME_MMGSDI_GET_OPERATOR_NAME_CB

DESCRIPTION
  This function is the callback function for the MMGSDI to return
  PLMN names.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_mmgsdi_get_operator_name_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
)
{
  
  ds_cmd_type *cmd_ptr;
  uint16 i;
  mmgsdi_plmn_info_type * src_plmn_info_ptr = NULL;
  ds_at_plmn_info * dst_plmn_info_ptr       = NULL;
  mmgsdi_eons_name_type * spn               = NULL;
  ds_at_mmgsdi_oper_name_type *oper_name    = NULL;

  DS_AT_MSG3_HIGH( "Session id = %d, Response type = %d, Status = %d",
    cnf_ptr->response_header.session_id,cnf_ptr->response_header.response_type,
    status);

  /* tell atcop that the async return is done */
  cmd_ptr  = dsat_get_cmd_buf(sizeof(ds_at_mmgsdi_oper_name_type), FALSE);
  oper_name = cmd_ptr->cmd_payload_ptr;

  cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_MMGSDI_OPER_NAME_INFO;
  oper_name->session_id = cnf_ptr->response_header.session_id;
  oper_name->mmgsdi_status = status;
  oper_name->client_ref = cnf_ptr->response_header.client_data;
  
  if(MMGSDI_SUCCESS == status)
  {
    oper_name->num_of_plmn_ids = 
                   cnf_ptr->session_get_operator_name_cnf.plmn_list.num_of_plmn_ids;

    /* Copy SPN info if the encodig is valid */
    spn = &oper_name->spn;

    spn->eons_encoding = 
        cnf_ptr->session_get_operator_name_cnf.spn.eons_encoding;
    
    spn->eons_data.data_len = 0;

    if((cnf_ptr->session_get_operator_name_cnf.spn.eons_encoding != MMGSDI_EONS_ENC_INVALID) &&
       (cnf_ptr->session_get_operator_name_cnf.spn.eons_data.data_ptr != NULL) &&
       (cnf_ptr->session_get_operator_name_cnf.spn.eons_data.data_len > 0))
    {
      spn->eons_data.data_len = 
                  cnf_ptr->session_get_operator_name_cnf.spn.eons_data.data_len;

      spn->eons_data.data_ptr = 
        (byte *)dsat_alloc_memory(sizeof(byte)*(spn->eons_data.data_len), FALSE);

      (void) dsatutil_memscpy((void*)spn->eons_data.data_ptr,
              spn->eons_data.data_len,
                  (void *)cnf_ptr->session_get_operator_name_cnf.spn.eons_data.data_ptr,
                   spn->eons_data.data_len);
      }

    oper_name->plmn_info_ptr =
      ( ds_at_plmn_info **)dsat_alloc_memory(sizeof(ds_at_plmn_info *)
                                                     * (oper_name->num_of_plmn_ids), FALSE);

    src_plmn_info_ptr = cnf_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr;

    /* Loop pver the list of PLMN Id's and copy the EONS data */
    for( i =0 ; i < oper_name->num_of_plmn_ids ; i++)
    {
      dst_plmn_info_ptr = ( ds_at_plmn_info *)dsat_alloc_memory(sizeof(ds_at_plmn_info), FALSE);

      DS_AT_MSG3_LOW("Allocated %x memory of %d size for %d index",dst_plmn_info_ptr,sizeof(ds_at_plmn_info),i);
      if((dst_plmn_info_ptr != NULL) && (src_plmn_info_ptr != NULL))
      {
        dst_plmn_info_ptr->lac = src_plmn_info_ptr->lac;
        dst_plmn_info_ptr->plmn_id = src_plmn_info_ptr->plmn_id;

        dst_plmn_info_ptr->long_name_encoding = src_plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding;
        dst_plmn_info_ptr->short_name_encoding = src_plmn_info_ptr->plmn_short_name.plmn_name.eons_encoding;

        dst_plmn_info_ptr->long_name_len = src_plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len;
        dst_plmn_info_ptr->short_name_len = src_plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len;

        dst_plmn_info_ptr->long_plmn_name_ci = src_plmn_info_ptr->plmn_long_name.plmn_name_ci;
        dst_plmn_info_ptr->short_plmn_name_ci = src_plmn_info_ptr->plmn_short_name.plmn_name_ci;

        dst_plmn_info_ptr->long_plmn_name_spare_bits = src_plmn_info_ptr->plmn_long_name.plmn_name_spare_bits;
        dst_plmn_info_ptr->short_plmn_name_spare_bits = src_plmn_info_ptr->plmn_short_name.plmn_name_spare_bits;

        if(src_plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr != NULL)
        {
        (void) dsatutil_memscpy((void*)dst_plmn_info_ptr->long_name,
                 2*DSAT_MAX_FULLNAME_STR_LEN,
                (void*)src_plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
                 dst_plmn_info_ptr->long_name_len);
          
        }
        if(src_plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr != NULL)
        {
          (void) dsatutil_memscpy((void*)dst_plmn_info_ptr->short_name,
                   2*DSAT_MAX_SHORTNAME_STR_LEN,
                  (void*)src_plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
                   dst_plmn_info_ptr->short_name_len);
        }
      }

      oper_name->plmn_info_ptr[i] = dst_plmn_info_ptr;

      src_plmn_info_ptr++;

    }
  }
  else
  {
    oper_name->num_of_plmn_ids = 0;
    oper_name->plmn_info_ptr = NULL;
  }
  ds_put_cmd(cmd_ptr);
}/* dsatme_mmgsdi_get_operator_name_cb */

/*===========================================================================
FUNCTION DSAT_MMGSDI_GET_OPERATOR_NAME_HANDLER

DESCRIPTION
  This function is handler function for C-EONS get_operator API

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem .
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/

dsat_result_enum_type dsat_mmgsdi_get_operator_name_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  uint32                        j;
  dsat_result_enum_type         result = DSAT_ASYNC_EVENT;
  dsat_apps_id_e_type           apps_id;
  sys_modem_as_id_e_type        subs_id;
  dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;
  ds_at_mmgsdi_oper_name_type  *oper_name = dsat_get_cmd_payload_ptr(cmd_ptr);

  apps_id = dsatme_update_appsid(oper_name->session_id);
  if (!IS_VALID_GW_APPS_ID(apps_id))
  {
    return DSAT_ERROR;
  }

  subs_id = dsat_qcsimapp_info[apps_id].subs_id;

  if( oper_name->mmgsdi_status != MMGSDI_SUCCESS )
  {
    DS_AT_MSG1_ERROR("Unexpected MMGSDI status - %d",oper_name->mmgsdi_status);
    result = DSAT_ERROR;
  }
  else
  {
    if((oper_name->client_ref == DSAT_MMGSDI_GET_CURR_OPER_NAME) ||
       (oper_name->client_ref == DSAT_MMGSDI_GET_OPER_NAME_LIST))
    {
      /* +COPS? and +COPS=? commands handling */
      result = dsatetsicall_report_cops_result(oper_name,subs_id);
    }
#ifdef FEATURE_SGLTE
    else if (oper_name->client_ref == DSAT_MMGSDI_GET_CURR_HYBR_OPER_NAME)
    {
      result = dsatetsicall_report_qchcops_result(oper_name);
    }
#endif /* FEATURE_SGLTE */

    if(oper_name->client_ref == DSAT_MMGSDI_GET_PREF_NETS_NAME)
    {
      if (CHECK_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX,0,DSAT_PENDING_CPOL_READ) )
      {
        result = dsatetsicall_display_pref_networks(oper_name);
      }
    }
    
    if(oper_name->spn.eons_encoding != MMGSDI_EONS_ENC_INVALID)
    {
      if(oper_name->spn.eons_data.data_ptr != NULL)
      {
       dsatutil_free_memory(oper_name->spn.eons_data.data_ptr);
      }
    }
    for( j=0; j < oper_name->num_of_plmn_ids; j++ )
    { 
      if( oper_name->plmn_info_ptr[j] != NULL)
      {
        dsatutil_free_memory(oper_name->plmn_info_ptr[j]);
      }
    }
    if( oper_name->plmn_info_ptr != NULL)
    {
      dsatutil_free_memory(oper_name->plmn_info_ptr);
    }
  }
  /* Clear pending variables */
  if((oper_name->client_ref == DSAT_MMGSDI_GET_CURR_OPER_NAME) ||
       (oper_name->client_ref == DSAT_MMGSDI_GET_OPER_NAME_LIST))
  {
    ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS,subs_id, FALSE);

    ph_ss_ms_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
    ph_ss_ms_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
  }

#ifdef FEATURE_SGLTE
  if (oper_name->client_ref == DSAT_MMGSDI_GET_CURR_HYBR_OPER_NAME)
  {
    ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, SYS_MODEM_AS_ID_2, FALSE);

    ph_ss_ms_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
    ph_ss_ms_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
  }
#endif /* FEATURE_SGLTE */

  if(oper_name->client_ref == DSAT_MMGSDI_GET_PREF_NETS_NAME)
  {
    if (CHECK_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX,0,DSAT_PENDING_CPOL_READ) )
    {
      SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
    }
  }
  return result;
}/* dsat_mmgsdi_get_operator_name_handler */
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_DSAT_MMGSDI_AUTH
/*===========================================================================

FUNCTION DSATME_MD5_ABORT_CMD

DESCRIPTION
  Called by At command processor to abort an MD5 command. A command can be
  aborted if user enters ESC key or if the ATCOP runs out of buffers. This 
  function will send an DS CMD to the AT Command processor indicating that
  the commaand has been aborted.

  If status is 0, the function is called when user enters ESC key.
  If status is 1, the function is called when we run out of buffer
                  or other cmd error.
  If status is other number, there is a cme error.  This function can be
                             called when there is error sending MD5 
                             messages.

  Event will contain the following result code:
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message
  A CME error number: if there is a cms error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  DS CMD is generated.

===========================================================================*/
void dsatme_md5_abort_cmd
(
 dsat_cme_error_e_type error_code /* CME Error */
)
{
  dsat_result_enum_type result_code;
  ds_cmd_type            *cmd_ptr;
  ds_at_md5_abt_type    *md5_abt = NULL;

  /* tell atcop that there is a command abort */
  cmd_ptr = dsat_get_cmd_buf(sizeof(ds_at_md5_abt_type), FALSE);
  md5_abt = cmd_ptr->cmd_payload_ptr;

  DS_AT_MSG0_HIGH("Aborting MD5 command");
  /* check to see what type of abort it is */
  /* CME_ERROR: Command ERROR              */
  /* CME_NONE: ESC from user               */
  if((int)error_code >= 0 )
  {
    result_code = DSAT_CMD_ERR_RSP;
    md5_abt->cme_error = (int)error_code;
  }
  else
  {
    result_code = DSAT_OK;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_MD5_ABT_CMD;
  md5_abt->cmd_status = (int16) result_code;
  ds_put_cmd(cmd_ptr);
}/* dsatsms_abort_cmd */

/*===========================================================================

FUNCTION  DSATME_MD5_ABT_HANDLER

DESCRIPTION
  Handler function for aborting a MD5 command

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_md5_abt_handler
(
 ds_cmd_type * cmd_ptr     /* DS Command pointer */
)
{
  dsm_item_type          *res_buff;
  ds_at_md5_abt_type *md5_abt = dsat_get_cmd_payload_ptr(cmd_ptr);

  DS_AT_MSG1_HIGH("dsatme_md5_abt_handler, error:%d", 
                md5_abt->cme_error);

  dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;

  /* Check to see if we need to send +CME ERROR */
  if ( (dsat_result_enum_type) md5_abt->cmd_status == 
                                                     DSAT_CMD_ERR_RSP )
  {
    res_buff = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
     dsatme_set_cme_error( 
           (dsat_cme_error_e_type)md5_abt->cme_error, res_buff );
    /* send the response to the user */
    dsatcmdp_handle_async_cmd_rsp(res_buff, DSAT_COMPLETE_RSP);
    return DSAT_CMD_ERR_RSP;
  }
  else if(DSAT_ERROR ==
            (dsat_result_enum_type)md5_abt->cmd_status) 
  {
    DS_AT_MSG0_ERROR(" MD5 Abort handler called");
    return DSAT_ERROR;
  } 
  else
  {
    /* just return the result code */
    return (dsat_result_enum_type) md5_abt->cmd_status;
  }

}/* dsatme_md5_abt_handler */

/*===========================================================================

FUNCTION DSATME_SEND_MD5_MSG

DESCRIPTION
  Central place/function call for sending MD5 message.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that message has to be typed. Then the 
  message is transmited. This function is used to transmit the actual message.
  It calls the mode-specific functions to send MD5 message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_send_md5_msg
(
 byte  * msg, /* Ptr to the text message that has to be sent */
 int   len    /* Len of the text message that has to be sent */
)
{
  mmgsdi_compute_ip_data_type compute_ip_data;
  byte challenge_data[255]={0};

  ASSERT( NULL != msg );

  DS_AT_MSG3_HIGH("dsatme_send_md5_msg, len: %d, msg: %c%c...",
                len, msg[0], msg[1]);

  /*Hex data in ASCII so double the size */
  if ( len != dsat707_md5_val.chap_length * 2 ) 
  {
    dsatme_md5_abort_cmd(DSAT_CME_MMGSDI_AUTH_ERROR);
    return;
  }

  if ( NULL == dsatutil_convert_chset( (const char *)msg, 
                                      ALPHA_HEX,
                                      (const char *)challenge_data,
                                      ALPHA_8BIT,
                                      sizeof(challenge_data),
                                      FALSE ) )
  {
    dsatme_md5_abort_cmd(DSAT_CME_AUTH_PARAM_LENGTH_ERROR);
    return;
  }
  compute_ip_data.operation_type = MMGSDI_CMPT_IP_HRPD_CHAP;
  compute_ip_data.data.hrpd_chap_data.chap_challenge_length = 
                                      (uint16)dsat707_md5_val.chap_length;  
  compute_ip_data.data.hrpd_chap_data.chap_challenge_ptr = challenge_data;
  compute_ip_data.data.hrpd_chap_data.chap_id = dsat707_md5_val.chap_id;


/* call mmgsdi API */  
  dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_MD5;
  if ( MMGSDI_SUCCESS != 
              mmgsdi_session_compute_ip_auth (  dsat707_md5_val.session_id,
                                                            compute_ip_data,
                               (mmgsdi_callback_type)dsat707_mmgsdi_auth_cb,
                            (mmgsdi_client_data_type)MD5_MMGSDI_IP_AUTH ))
  {
    dsat_mmgsdi_auth_cmd_pending = DSAT_MMGSDI_AUTH_NONE;
    DS_AT_MSG0_ERROR("Failed to send MD5 command for compute ip");
    dsatme_md5_abort_cmd(DSAT_CME_MMGSDI_AUTH_ERROR );
  }

}/* dsatme_send_md5_msg */
#endif /* FEATURE_DSAT_MMGSDI_AUTH */

/*===========================================================================

FUNCTION DSATME_EXEC_CFUN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CFUN command.
  This command selects the level of functionality in the phone to
  control power consumption.  Additionally, certain UE specific
  modes (e.ge FTM) can be selected.

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
dsat_result_enum_type dsatme_exec_cfun_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  if( mode != DSAT_CMD )
  {
    return DSAT_ERROR;
  }
  
  /* Process the WRITE command */
  if (tok_ptr->op == (NA|EQ|AR))
  {
    /* Save old opmode */
    dsat_num_item_type temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSAT_EXT_CFUN_IDX,0,0,MIX_NUM_TYPE);
    dsat_me_func_e_type   old_opmode = (dsat_me_func_e_type)temp_val;

    /* Default <rst> to 0 */
    
    DSATUTIL_SET_VAL(DSAT_EXT_CFUN_IDX,0,1,0,DSAT_ME_DONT_RESET,MIX_NUM_TYPE)
    /* If reset parameter was provided... */
    if ( VALID_TOKEN(1) && !VALID_TOKEN(0) )
    {
      /* Parameter value error */
      return DSAT_ERROR;
    }
    /* Parse command line input to dsat_cfun_val[0/1] */
    if (DSAT_OK == 
        dsatparm_exec_param_cmd (mode, parse_table, tok_ptr, res_buff_ptr))
    {
      
      if ((dsat_num_item_type) dsatutil_get_val(
             DSAT_EXT_CFUN_IDX,0,1,MIX_NUM_TYPE)== (dsat_num_item_type)DSAT_ME_DO_RESET &&
           (dsat_num_item_type)dsatutil_get_val(
             DSAT_EXT_CFUN_IDX,0,0,MIX_NUM_TYPE) != (dsat_num_item_type)DSAT_ME_FUNC_FULL )
      {

        DSATUTIL_SET_VAL(DSAT_EXT_CFUN_IDX,0,0,0,old_opmode,MIX_NUM_TYPE)
        DS_AT_MSG0_HIGH("+CFUN Only supports reset to full functionality");
        dsatme_set_cme_error(DSAT_CME_OP_NOT_SUPPORTED, res_buff_ptr);
        return  DSAT_CMD_ERR_RSP;
      }
    }
    else
    {
      return DSAT_ERROR;
    }
    /* Don't allow +CFUN write command until current mode is known */

    if ( old_opmode == DSAT_ME_FUNC_MAX )
    {
      /* Make an asynch call to cm to retreive the oprt_mode */
      /* The success as well as failure will be handled by 
         the call back function */
      SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_WRITE_CB)
      result = DSAT_ASYNC_CMD;
      (void) cm_ph_cmd_get_ph_info (dsatcmif_ph_cmd_cb_func,
                                    NULL,
                                    dsatcm_client_id);
    }
    else 
    {
      SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_WRITE)
      /* For DSAT_CFUN_WRITE, the argument to dsatme_process_cfun_cmd is not required
         just to support the prototype of the function */
      result = dsatme_process_cfun_cmd ( (sys_oprt_mode_e_type) NULL, old_opmode);
    }
  }

  /* Process the READ command */
  else if (tok_ptr->op == (NA|QU))
  {
    /* If functionality unknown query CM */
    if ( (dsat_num_item_type)DSAT_ME_FUNC_MAX == 
         (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_CFUN_IDX,0,0,MIX_NUM_TYPE))
    {
      /* The success as well as failure will be handled by 
         the call back function */
      SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_READ);
      result = DSAT_ASYNC_CMD;
      (void)cm_ph_cmd_get_ph_info (dsatcmif_ph_cmd_cb_func,
                                   NULL,
                                   dsatcm_client_id);
    }
    else
    {
      /* Only the functionality value is output */
      res_buff_ptr->used = 0;
      res_buff_ptr->used = (word)snprintf ( (char*)res_buff_ptr->data_ptr,
                                                  res_buff_ptr->size,
                                                  "+CFUN: %d",
                                                  (dsat_num_item_type)dsatutil_get_val(
                                                  DSAT_EXT_CFUN_IDX,0,0,MIX_NUM_TYPE));
    }
  }

  /* Process the TEST command */
  else if (tok_ptr->op == (NA|EQ|QU)) 
  {
    result = dsatparm_exec_param_cmd (mode, parse_table, tok_ptr, res_buff_ptr);
  }
  /* Command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatme_exec_cfun_cmd */


/*===========================================================================

FUNCTION DSATME_PROCESS_CFUN_CMD

DESCRIPTION
  This function process the response for the +CFUN command on return from
  asynchronous processing / synchronous function call. 
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :       if there was any problem in executing the command
    DSAT_OK :          if it is a success.
    DSAT_CMD_ERR_RSP : If there is a CME ERROR.
    DSAT_ASYNC_CMD:    if this is function invokes an async command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_process_cfun_cmd
(
  sys_oprt_mode_e_type  oprt_mode,
  dsat_me_func_e_type   old_opmode
)
{
  dsat_result_enum_type result = DSAT_OK;
  sys_oprt_mode_e_type  new_opmode = SYS_OPRT_MODE_ONLINE;
  dsat_mixed_param_val_type *temp_mix_type;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  if ( CHECK_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_WRITE_CB) )
  {
    /* oprt_mode is received from CM */
    if ( TRUE != dsatme_convert_phone_state(TRUE, &oprt_mode, &old_opmode) )
    {
      (void)dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE)
      DSATUTIL_SET_VAL(DSAT_EXT_CFUN_IDX,0,0,0,DSAT_ME_FUNC_MAX,MIX_NUM_TYPE)
      return DSAT_CMD_ERR_RSP;
    }
    SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_WRITE)
  }
  if (CHECK_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_WRITE))
  {
    SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE)
   /* If not resetting and current and requested opmode 
    are the same just return OK */
    if ( (dsat_num_item_type)dsatutil_get_val(
          DSAT_EXT_CFUN_IDX,0,1,MIX_NUM_TYPE) == (dsat_num_item_type)DSAT_ME_DONT_RESET &&
         (dsat_num_item_type)old_opmode == (dsat_num_item_type)dsatutil_get_val(
          DSAT_EXT_CFUN_IDX,0,0,MIX_NUM_TYPE))
    {
#if defined(FEATURE_DSAT_CFUN_CARD_POWER_CTL) && defined (FEATURE_DSAT_ETSI_MODE)
      if ((DSAT_ME_FUNC_FULL == old_opmode) && (is_card_action == TRUE))
      {
        return dsatme_process_cfun_card_pup_cmd(FALSE);
      }
#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */
      return DSAT_OK;
    }
    else
    {
      /* If resetting, change +CFUN <fun> value to put phone in offline mode 
         in preparation for reset */
     /* Reset support for CFUN = 6 */
      if ( (dsat_num_item_type)DSAT_ME_DO_RESET == 
           (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_CFUN_IDX,0,1,MIX_NUM_TYPE) ||
          (((dsat_num_item_type)DSAT_ME_FUNC_RESET == (dsat_num_item_type)dsatutil_get_val(
            DSAT_EXT_CFUN_IDX,0,0,MIX_NUM_TYPE))) )
      {
         /*Do not change cfun value to OFFLINE for command CFUN=6 */
         if(FALSE == ((DSAT_ME_FUNC_OFFLINE == old_opmode )&&((dsat_num_item_type)DSAT_ME_FUNC_RESET == (dsat_num_item_type)dsatutil_get_val(
            DSAT_EXT_CFUN_IDX,0,0,MIX_NUM_TYPE))))
         {
           DSATUTIL_SET_VAL(DSAT_EXT_CFUN_IDX,0,0,0,DSAT_ME_FUNC_OFFLINE,MIX_NUM_TYPE)
         }
         if(TRUE == cm_rpm_check_reset_allowed())
         {
           sys_m_initiate_shutdown();
           DS_AT_MSG0_HIGH("Called sys_m_initiate_shutdown");
           return DSAT_OK;
         }
         else
         {
           DS_AT_MSG0_HIGH("Reset is blocked");
           DSATUTIL_SET_VAL(DSAT_EXT_CFUN_IDX,0,0,0,old_opmode,MIX_NUM_TYPE);
           return DSAT_ERROR;
         }
      }
      temp_mix_type = (dsat_mixed_param_val_type *)dsatutil_get_val(
                                    DSAT_EXT_CFUN_IDX,0,0,MIX_INDEX_TYPE);
      /* Convert +CFUN <fun> parm to desired new opmode */
      if ( TRUE == dsatme_convert_phone_state( 
                   FALSE, 
                   &new_opmode,
                   (dsat_me_func_e_type*)&(temp_mix_type->num_item) ) )
      {
        /* Invoke Call Manager API requesting functionality change */
        result = dsatcmif_change_operating_mode (new_opmode);
        if (DSAT_ASYNC_CMD != result)
        {
          SET_PENDING(DSAT_EXT_CFUN_IDX, 0, DSAT_PENDING_CFUN_NONE);
          if (result == DSAT_OK)
          {
            return result;
          } 
        }
      }
      else
      {
        SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE)
        result = DSAT_ERROR;
      }
    }
     /* Restore original operating mode; new mode set via asynch event */
    DSATUTIL_SET_VAL(DSAT_EXT_CFUN_IDX,0,0,0,old_opmode,MIX_NUM_TYPE)
  }

  /* Process the READ command */
  if ( CHECK_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_READ))
  {
    dsm_item_type * res_buff_ptr;

    SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE)
    temp_mix_type = (dsat_mixed_param_val_type *)dsatutil_get_val(
                                 DSAT_EXT_CFUN_IDX,0,0,MIX_INDEX_TYPE);
    if (TRUE != dsatme_convert_phone_state
               (TRUE, &oprt_mode,
               (dsat_me_func_e_type*)&(temp_mix_type->num_item)))
    {
      return DSAT_ERROR;
    }     
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
    /* Only the functionality value is output */
    
    res_buff_ptr->used = (word)snprintf ( (char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "+CFUN: %d",
                                                (dsat_num_item_type)dsatutil_get_val(
                                                DSAT_EXT_CFUN_IDX,0,0,MIX_NUM_TYPE));
    dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
  }

  return result;
} /* dsatme_process_cfun_cmd */

/*===========================================================================

FUNCTION DSATME_CONVERT_PHONE_STATE

DESCRIPTION
  This function converts the phone state enum values between Call
  Manager and ATCOP.  Bidirectional conversio is supported.  The ATCOP
  values are driven by 3GPP 27.007 +CFUN command description.  The
  Call Manager values are strictly internal.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on successful conversion; FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatme_convert_phone_state
(
  const boolean          to_at_state,  /* Conversion direction */
  sys_oprt_mode_e_type  *cm_mode_ptr,  /* Call Manager mode    */
  dsat_me_func_e_type   *at_mode_ptr   /* ATCOP mode           */
)
{
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;
  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  if (TRUE == to_at_state)
  {
    switch ( *cm_mode_ptr )
    {
    case SYS_OPRT_MODE_PWROFF:
      *at_mode_ptr = DSAT_ME_FUNC_MIN;
      break;

    case SYS_OPRT_MODE_LPM:
#if defined(FEATURE_DSAT_EXTENDED_CMD)
      *at_mode_ptr = DSAT_ME_FUNC_MIN;
#else
      /* LPM is duplicated in mapping between system modes and +CFUN parms:
         used for +CFUN=0 (w/ SIM card power down) and +CFUN=4 (Tx & Rx off) */
      if ( CHECK_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_WRITE_RFOFF) )
      {
        *at_mode_ptr = DSAT_ME_FUNC_RFOFF;
      }
      else
      {
        *at_mode_ptr = DSAT_ME_FUNC_MIN;
      }
#endif /* defined(FEATURE_DSAT_EXTENDED_CMD) */
      break;

    case SYS_OPRT_MODE_OFFLINE:
    case SYS_OPRT_MODE_OFFLINE_AMPS:
    case SYS_OPRT_MODE_OFFLINE_CDMA:
#if defined(FEATURE_DSAT_EXTENDED_CMD)
      *at_mode_ptr = DSAT_ME_FUNC_RFOFF;
#else
      *at_mode_ptr = DSAT_ME_FUNC_OFFLINE;
#endif /* defined(FEATURE_DSAT_EXTENDED_CMD) */
      break;    

    case SYS_OPRT_MODE_FTM:
      *at_mode_ptr = DSAT_ME_FUNC_FTM;
      break;    

    case SYS_OPRT_MODE_RESET:
      *at_mode_ptr = DSAT_ME_FUNC_RESET;
      break;    

    case SYS_OPRT_MODE_ONLINE:
      *at_mode_ptr = DSAT_ME_FUNC_FULL;
      break;
      
    default:
      DS_AT_MSG1_ERROR("Unsupported CM operating mode: %d",*cm_mode_ptr);
      return FALSE;    
    }
  }
  else
  {
    /* Convert demand state to CM operating mode */
    switch ( *at_mode_ptr )
    {
    case DSAT_ME_FUNC_MIN:
      *cm_mode_ptr = SYS_OPRT_MODE_LPM;
      SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_WRITE_MIN)
      break;
    
    case DSAT_ME_FUNC_FULL:
      *cm_mode_ptr = SYS_OPRT_MODE_ONLINE;
       SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_WRITE_FULL)
      break;    

    case DSAT_ME_FUNC_FTM:
      *cm_mode_ptr = SYS_OPRT_MODE_FTM;
      SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_WRITE_FTM)
      break;
      
    case DSAT_ME_FUNC_OFFLINE:
#if defined(FEATURE_DSAT_EXTENDED_CMD)
      *cm_mode_ptr = SYS_OPRT_MODE_OFFLINE_CDMA;
#else
      *cm_mode_ptr = SYS_OPRT_MODE_OFFLINE;
#endif /* defined(FEATURE_DSAT_EXTENDED_CMD) */
      SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_WRITE_OFFLINE)
      break;
      
    case DSAT_ME_FUNC_RESET:
      *cm_mode_ptr = SYS_OPRT_MODE_RESET;
       SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_WRITE_RESET)
      break;
      
    case DSAT_ME_FUNC_RFOFF:
#if defined(FEATURE_DSAT_EXTENDED_CMD)
      *cm_mode_ptr = SYS_OPRT_MODE_OFFLINE;
#else
      *cm_mode_ptr = SYS_OPRT_MODE_LPM;
#endif /* defined(FEATURE_DSAT_EXTENDED_CMD) */
      SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_WRITE_RFOFF)
      break;
      
    default:
      DS_AT_MSG1_ERROR("Unsupported ATCOP operating mode: %d",*at_mode_ptr);
      return FALSE;    
    }
  }
  return TRUE;
} /* dsatme_convert_phone_state() */

#ifdef FEATURE_DSAT_CFUN_CARD_POWER_CTL
/*===========================================================================

FUNCTION DSATME_PROCESS_CFUN_CARD_PDOWN_CMD

DESCRIPTION
  This function process the +CFUN card power down command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR :    If there was any problem in executing the command.
    DSAT_OK :       If it is a success.
    DSAT_ASYNC_CMD: If this is function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_process_cfun_card_pdown_cmd(void)
{
  dsat_result_enum_type             result = DSAT_OK;
  mmgsdi_slot_id_enum_type          slot_id;
  mmgsdi_return_enum_type           ret_val;
  dsatme_mmgsdi_state_ss_info      *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);
  me_ss_val->dsat_is_card_power_down = TRUE;

  for (slot_id = MMGSDI_SLOT_1; slot_id <= dsat_get_max_subs(); slot_id++)
    {
      ret_val = mmgsdi_card_pdown( me_ss_val->dsat_mmgsdi_client_id,
                                   slot_id,
                                   dsatetsime_mmgsdi_card_power_ctl_cb,
                                   MMGSDI_CARD_POWER_DOWN_NOTIFY_GSDI,
                                   0 );

      if (MMGSDI_RESTRICTED_IN_ONCHIP_MODE != ret_val)
      {
        result = DSAT_ASYNC_CMD;
        SET_PENDING(DSAT_EXT_CFUN_IDX, 0, DSAT_PENDING_CFUN_WRITE_CARD_DOWN);

#if defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)
        dsat_set_provision_state(DSAT_MMGSDI_DOWN);
#endif /* defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION) */
      }
    }
  return result;
} /* dsatme_process_cfun_card_pdown_cmd */

/*===========================================================================

FUNCTION DSATME_PROCESS_CFUN_CARD_PUP_CMD

DESCRIPTION
  This function process the +CFUN card power up command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR :    If there was any problem in executing the command.
    DSAT_OK :       If it is a success.
    DSAT_ASYNC_CMD: If this is function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_process_cfun_card_pup_cmd
(
  boolean opmode_change
)
{
  dsat_result_enum_type             result = DSAT_OK;
  mmgsdi_slot_id_enum_type          slot_id;
  mmgsdi_return_enum_type           ret_val;
  dsatme_mmgsdi_state_ss_info      *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);


  if (TRUE == me_ss_val->dsat_is_card_power_down)
  {
    me_ss_val->dsat_is_card_power_down = FALSE;
    me_ss_val->dsat_card_error_event_count = 0;
    me_ss_val->dsat_num_of_card_operations = 0;

    for (slot_id = MMGSDI_SLOT_1; slot_id <= dsat_get_max_subs(); slot_id++)
    {
      ret_val = mmgsdi_card_pup( me_ss_val->dsat_mmgsdi_client_id,
                                 slot_id,
                                 dsatetsime_mmgsdi_card_power_ctl_cb,
                                 MMGSDI_CARD_POWER_UP_INITIAL_PUP,
                                 0 );

      if (MMGSDI_RESTRICTED_IN_ONCHIP_MODE != ret_val)
      {
        me_ss_val->dsat_num_of_card_operations++;
        result = DSAT_ASYNC_CMD;
        SET_PENDING(DSAT_EXT_CFUN_IDX, 0, DSAT_PENDING_CFUN_WRITE_CARD_PUP);

#if defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)
        dsat_set_provision_state(DSAT_MMGSDI_UP);
#endif /* defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION) */
      }
    }
  }

  return result;
} /* dsatme_process_cfun_card_pup_cmd */
#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */

/*===========================================================================

FUNCTION  DSATME_INIT_CONFIG_FROM_EFS

DESCRIPTION
This Fucntion Creates EFS file  atcop_cops_auto_mode.txt if file is not present 
if file is already present it reads cops_no_mode_change value from file and
assigns it to variable cops_no_mode_change.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatme_init_config_from_efs(void)
{
/* File Path EFS */

#define DSAT_COPS_MODE_F_NAME \
                        (char*)"/ds/atcop/atcop_cops_auto_mode.txt"
#define DSAT_CFUN_CARD_ACT_F_NAME \
                        (char*)"/ds/atcop/atcop_cfun_card_action.txt"

/* Number of configuration file */
#define DSAT_CONFIG_FILE   2
#define DSAT_MAX_PATH_LEN  64

  boolean                           result[DSAT_CONFIG_FILE] = {FALSE,TRUE};
  boolean                           is_dir_created =FALSE;
  int fd;
  dsat_num_item_type                file_count = 0;
  struct fs_stat temp_buf;
  char buf[2] = {'\0'};
  char                              file_path[DSAT_CONFIG_FILE][DSAT_MAX_PATH_LEN];
  dsat_debug_result_code            result_code ;
  mcfg_fs_status_e_type             fs_status = MCFG_FS_STATUS_OK;
  
  dsatutil_memscpy(file_path[0],DSAT_MAX_PATH_LEN,DSAT_COPS_MODE_F_NAME,strlen(DSAT_COPS_MODE_F_NAME) + 1);
  dsatutil_memscpy(file_path[1],DSAT_MAX_PATH_LEN,DSAT_CFUN_CARD_ACT_F_NAME,strlen(DSAT_CFUN_CARD_ACT_F_NAME) + 1);
  
  for(file_count = 0 ;file_count < DSAT_CONFIG_FILE ; file_count++)
  {
    memset ((void*)buf, 0x0, sizeof(buf));
    memset(&result_code,0x0,sizeof(dsat_debug_result_code));

    fs_status = mcfg_fs_stat(file_path[file_count],
                             &temp_buf,
                             MCFG_FS_TYPE_EFS,
                             MCFG_FS_SUBID_NONE);

    /*EFS SUCCESS == 0*/
    if(fs_status != MCFG_FS_STATUS_OK)
    {
      result_code.result_code =  result_code.result_code | DSAT_FILE_NOT_FOUND;
      if(is_dir_created == FALSE)
  {
        (void)mcfg_mkdir("/ds", MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS);
        (void)mcfg_mkdir("/ds/atcop", MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS);
        is_dir_created = TRUE;
      }

        buf[0] =( (result[file_count] == TRUE )? '1':'0');
        
      /* MCFG will call into efs_put, which will use the o_CREAT flag. 
        No Need to close the file explicitly */

      fs_status = mcfg_fs_write(file_path[file_count],
                                (byte*)buf,
                                sizeof(char),
                                MCFG_FS_O_CREAT,
                                MCFG_FS_ALLPERMS,
                                MCFG_FS_TYPE_EFS,
                                MCFG_FS_SUBID_NONE);

      if (fs_status != MCFG_FS_STATUS_OK) 
      {
          result_code.result_code =  result_code.result_code | DSAT_FILE_WRITE_FAILED;
      }
  }
  else  
  {
      fd = mcfg_fopen(file_path[file_count], 
                      MCFG_FS_O_RDONLY,
                      MCFG_FS_ALLPERMS,
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE);
    if (fd < 0)
    {
        result_code.result_code =  result_code.result_code | DSAT_FILE_OPEN_FAILED;
    }
    else
    {
        if(-1 == mcfg_fread(fd, buf, sizeof(char), MCFG_FS_TYPE_EFS))
      {
          result_code.result_code =  result_code.result_code | DSAT_FILE_READ_FAILED;
      }
      else
      {
      buf[1] = '\0';
          result[file_count] =( (atoi(buf) == 1) ? 1 : 0);
          result_code.result_code =  result_code.result_code | DSAT_SUCCESS;
      }
        (void)mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
    }
  }
     DS_AT_MSG1_HIGH("File open result code: %d",result_code.result_code);
}

  cops_no_mode_change   = result[0];
  is_card_action = result[1];
}/*dsatme_init_config_from_efs*/

/*===========================================================================

FUNCTION  DSAT_GET_MAX_SUBS

DESCRIPTION
  This function is used to get the maximum number of subscriptions based on
  device mode.

DEPENDENCIES
  None

RETURN VALUE
  Returns the maximum number of subscriptions.

SIDE EFFECTS
  None

===========================================================================*/

uint8 dsat_get_max_subs(void)
{
  uint8 max_subs;

  switch (dsatcmif_get_device_mode())
  {
    case SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY:
      max_subs = 3;
      break;

    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY:
    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE:
      max_subs = 2;
      break;

    case SYS_MODEM_DEVICE_MODE_SINGLE_SIM:
    default:
      max_subs = 1;
      break;
  }

  return max_subs;
} /* dsat_get_max_subs */
