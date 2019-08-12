#ifndef DSAT707VEND_H
#define DSAT707VEND_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( IS-707 mode vendor specific commands )
  processor.

EXTERNALIZED FUNCTIONS

dsat707_exec_qcotc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCOTC command.

dsat707_exec_qcprev_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPREV command.
  Returns protocol revision in use.

dsat707_exec_qcmdr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMDR command.
  Set Medium Data Rate.

dsat707_exec_qcscrm_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSCRM command.
  Enable/Disable mobile from SCRMing.

dsat707_exec_qctrtl_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTRTL command.
  Enable/Dsiable R_SCH throttling.

dsat707_exec_qcchs_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCHS command.
  Enable/Dsiable mobile originated control/hold.
  
dsat707_exec_qcqnc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQNC command.
  Enable/Dsiable Quick Net Connect.

dsat707_exec_qcso_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSO command.
  Set Data Service Option number.

dsat707_exec_qcvad_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCVAD command.
  Prearrangement setting. Respond to page message that has a voice service
  option with a page response that has a data service option.

dsat707_exec_qccav_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCAV command.
  Answer incoming voice call.

dsat707_exec_qcqospri_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQOSPRI command.
  Set desired value of QOS non-assured priority adjustment.

dsat707_exec_hdrcsq_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT^HDRCSQ command. It provides
  the intensity level of current hdr_rssi.

dsat707_exec_reset_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT^RESET command. This command
  reboots the device.

  Copyright (c) 1995-2013, 2009-2011 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707vend.h_v   1.6   08 Jan 2003 12:27:58   sramacha  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsat707vend.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/13/12    dvk    Merged enabling mdr val update always during powerup.
13/11/11    ms     Fix compiler errors when MDR feature is turned off. 
07/05/11    nc     Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
05/10/10    kk     Added support for $$GPS* and $SP* commands.
05/10/10    kk     Added support for ^RESET .
05/10/10    kk     Added support for $QCGSN and $SPMSID.
05/10/10    kk     Added ^SYSINFO,^HDRCSQ commands. 
11/16/09    ca     Added support for MMGSDI AUTH commands.  
11/21/02    rsl    Fixed merge issues with at$qcchv.
10/15/02    ak     Added exec_qcchs, which is for control/hold.
10/03/02    atp    Fixed minor compilation problem by including .h.
09/24/02    atp    Added support for QOS. (1x Release-A feature).
07/12/02    ak     Extern for dat707_set_mdr_val.
04/03/02    rsl    Initial version of file.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_profile_3gpp2.h"
#include "ds_profile.h"

#ifdef FEATURE_DATA_IS707
#include "dsati.h"

/*===========================================================================

                           Global Data 

===========================================================================*/

#ifdef  FEATURE_DSAT_MMGSDI_AUTH
#define DSAT_PROCESS_CONTROL_CAVE   16
#define DSAT_PROCESS_CONTROL_SSD    0
#define DSAT_VPM_FIRST_OCTET        0
#define DSAT_VPM_LAST_OCTET         64
#define DSAT_MAX_SMEKEY_LEN         8
#define DSAT_MAX_RANDBS_LEN         4
#define DSAT_MAX_RANDSSD_LEN        7
#define DSAT_MAX_VPM_LEN            65
#define DSAT_MAX_RANDU_LEN          3
#define DSAT_MAX_AUTHU_LEN          3
#define DSAT_CAVE_DIG_LEN           0
#define DSAT_MMGSDI_RANDU_TYPE      1


#define DS3G_MMGSDI_INVALID_CLIENT_ID     0x00000000FFFFFFFF

#define DS3G_MMGSDI_INVALID_SESSION_ID     0x00000000FFFFFFFF

typedef struct
{
  uint64  session_id;
  uint32  chap_length;
  uint8   *chap_challenge;
  uint8  chap_id;
}dsat707_md5_s_type;

typedef enum
{
  DSAT_MMGSDI_AUTH_NONE = 0,
  DSAT_MMGSDI_AUTH_SSD_UPDATE ,
  DSAT_MMGSDI_AUTH_SSD_BS_CHAL,
  DSAT_MMGSDI_AUTH_SSD_CNF,
  DSAT_MMGSDI_AUTH_KEYGEN_VPM,
  DSAT_MMGSDI_AUTH_CAVE,
  DSAT_MMGSDI_AUTH_MD5,
  DSAT_MMGSDI_AUTH_GSN,
  DSAT_MMGSDI_AUTH_UIMAUTH,
  DSAT_MMGSDI_AUTH_MAX = 0xFF
} dsat_mmgsdi_auth_e_type;
#endif /* FEATURE_DSAT_MMGSDI_AUTH */

/*===========================================================================
                                 DATA STRUCTURES 
===========================================================================*/

#if defined(FEATURE_DSAT_SP_CMDS)
typedef enum
{
  DSAT_SRV_NONE,         /* No Service */
  DSAT_SRV_CDMA,         /* Registered to CDMA network */
  DSAT_SRV_EVDO,         /* Registerd to EVDO */
  DSAT_SRV_EVDO_REV_A,   /* Registered to EVDO REV A*/
  DSAT_SRV_MAX = 0xFF  /* ATCOP Internal use */
}dsat_spservice_e_type;
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

/*===========================================================================
                                 EXTERN VARIABLES 
===========================================================================*/
#if defined(FEATURE_DSAT_SP_CMDS)
extern dsat_spservice_e_type dsat_spservice;
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

/*===========================================================================
                       EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

#ifdef FEATURE_DATA_IS707_ASYNC
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCOTC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCOTC command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcotc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcotc_cmd */
#endif /* FEATURE_DATA_IS707_ASYNC */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCPREV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPREV command.
  Returns protocol revision in use.
  1 - JSTD008
  3 - IS_95A
  4 - IS_95B
  6 - IS2000

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcprev_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcprev_cmd */


#ifdef FEATURE_DS_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMDR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMDR command.
  Set Medium Data Rate.
  0: MDR service only
  1: MDR service if available
  2: LSPD only
  3: SO 33, if available

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmdr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcmdr_cmd */

#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCSCRM_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSCRM command.
  Enable/Disable mobile from SCRMing.
  0 - Mobile never SCRMs
  1 - Mobile can SCRM as needed.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcscrm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcscrm_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCTRTL_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTRTL command.
  Enable/Dsiable R_SCH throttling.
  0 - Mobile never throttles R-SCH
  1 - Mobile can throttle R-SCH as needed.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qctrtl_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qctrtl_cmd */

#endif /* FEATURE_IS2000_R_SCH */
#endif /* FEATURE_DS_AT_TEST_ONLY */

#ifdef FEATURE_IS2000_CHS
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCQNC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCHS command.
  Enable/Dsiable mobile originated control/hold.
  0 - Disable mobile-originated control/hold.
  1-255 - Enable mobile-originated control-hold.  Mobile waits for this many
          idle (both tx and rx) 20-msec frames before asking BS for
          control/hold.  

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcchs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
);
#endif /* FEATURE_IS2000_CHS */

#ifdef FEATURE_DS_QNC
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCQNC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQNC command.
  Enable/Dsiable Quick Net Connect.
  0 - Disable QNC capability
  1 - Enable QNC capability

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcqnc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcqnc_cmd */
#endif /* FEATURE_DS_QNC */

#ifdef FEATURE_DS_CHOOSE_SO
#ifdef FEATURE_DS_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCSO_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSO command.
  Set Data Service Option number.
  0 - pre-707 numbers
  1 - proprietary 707 numbers
  2 - IS-707 SO numbers

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcso_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcso_cmd */
#endif /* FEATURE_DS_AT_TEST_ONLY */
#endif /* FEATURE_DS_CHOOSE_SO */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCVAD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCVAD command.
  Prearrangement setting. Respond to page message that has a voice service
  option with a page response that has a data service option.
  0 - Off
  1 - Fax for next call
  2 - Fax for all calls
  3 - Async. for next call
  4 - Async. for all calls

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcvad_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcvad_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCCAV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCAV command.
  Answer incoming voice call.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qccav_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qccav_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCCHV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCHV command.
  Hangs up incoming voice call.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcchv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcchv_cmd */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCQOSPRI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQOSPRI command.
  Sets desired value of QOS non-assured priority adjustment.
  (Can range from 0 to DS707_QOS_DESIRED_PRI_MAX (0xF).)

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcqospri_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                    /*  Place to put response       */
); /* dsat707_exec_qcqospri_cmd */
#endif /* FEATURE_IS2000_REL_A    */

#ifdef  FEATURE_DSAT_MMGSDI_AUTH
/*===========================================================================
  FUNCTION DSAT707_EXEC_CAVE_CMD

  DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes AT^CAVE command. Client sends CAVE authentication
  related parameter (RANDU) to datacard, datacard returns the response. 
  
  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD : if the command processing is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cave_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cave_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SSDUPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^SSDUPD command. Client sends RANDSSD
  to datacard, datacard calculates and reports result.
 
  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD : if the command processing is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_ssdupd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_ssdupd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_GSN_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^GSN command. Client checks
  datacard/module’s ESN and UIMID. If terminal uses MEID, 
  then it returns MEID, and if terminal uses ESN, then it
  returns ESN.
  
  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD : if the command processing is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_gsn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_gsn_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_UIMAUTH_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^UIMAUTH command. Client queries 
  UIM to check the authentication algorithm it supports.
 
  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD : if the command processing is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_uimauth_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_uimauth_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_VPM_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^VPM command. Client asks datacard 
  to generate key.
  
  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD : if the command processing is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_vpm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_vpm_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_SSDUPDCFM_CMD

  DESCRIPTION
  This function takes the result from the command line parser 
  and executes it. It executes AT^SSDUPDCFM command. Client sends 
  updated SSD confirmation (AUTHBS) to datacard.
  
  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_CME_UNKNOWN : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD : if the command processing is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_ssdupdcfm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_ssdupdcfm_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MD5_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^MD5 command. Client queries 
  UIM to check the authentication algorithm it supports.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  DSAT_ASYNC_CMD : if the command processing is in progress.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_md5_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_MMGSDI_AUTH_CB

  DESCRIPTION
  This is the callback function provided while calling MMGSDI authentication 
  interface commands.
   
  DEPENDENCIES
  None.
  
  RETURN VALUE
  None.
  
  SIDE EFFECTS
  None.
===========================================================================*/
void dsat707_mmgsdi_auth_cb 
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf, 
  const mmgsdi_cnf_type   *cnf_ptr
);

/*===========================================================================
  FUNCTION DSAT707_MMGSDI_AUTH_EVENT_HANDLER

  DESCRIPTION
  This function is the event handler for DS_AT_MMGSDI_AUTH_CMD. This takes care of the AUTH commands 
  responses to be sent back to the terminal. 

  DEPENDENCIES
  None

  RETURN VALUE
  
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_mmgsdi_auth_event_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
#endif /* FEATURE_DSAT_MMGSDI_AUTH */

/*===========================================================================
  FUNCTION DSAT707_EXEC_HDRCSQ_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^HDRCSQ command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.

===========================================================================*/
dsat_result_enum_type dsat707_exec_hdrcsq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
); /* dsat707_exec_hdrcsq_cmd */

#if defined(FEATURE_DSAT_SP_CMDS)
/*===========================================================================
  FUNCTION DSAT707_EXEC_SPMSID_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMSID command.
  returns the device msid.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_spmsid_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_SPC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $SPSPC command. It is used to set/get 
  Secure Program Code.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK    :         if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_spc_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_RESET_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPRESET command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_reset_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_FIRMWARE_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPFWREV? command.
  returns the device firmware version.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_firmware_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_PRL_ID_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPPRL command.
  returns the device prl number.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_prl_id_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_ROAM_PREF_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPROAM? command.
  returns/set the current roam setting:
    0 - Sprint only
    1 - Automatic
    2 - Roam Only

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_roam_pref_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_ROAMING_IND_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPERI command.
  It returns the device's extended roaming indicator.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_roaming_ind_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_SIG_DBM_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPSIGDBM command.
  It returns the current Receive Signal Strength Indicator.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_sig_dbm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_LOCKED_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPLOCKED command.
  It returns phone lock state [Locked / Unlocked].

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_locked_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSATETSIME_EXEC_SP_UNLOCK_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $SPUNLOCK command.
  Syntax format:
    AT$SPUNLOCK=####(##)
    #### is PIN, should also accept SPC (######).
  Response should be:
    ERROR - Unable to unlock phone, still locked.
    OK - Phone already unlocked or was unlocked."

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK    :      if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_unlock_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_LOCK_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $SPLOCK command.
  Syntax format:
    AT$SPLOCK=####
    #### is PIN which must be correct before phone is locked.
  Response should be:
    ERROR - Unable to lock phone, still unlocked.
    OK - Phone already locked or was locked."

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK    :      if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_lock_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSATETSIME_EXEC_SP_LOCK_CHG_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $SPLOCKCHG command.
  Syntax format:
  AT$SPLOCKCHG=####(##),####
    First  #### is current PIN, should also accept SPC (######).
    Second #### should be new PIN.

  Response should be:
    ERROR - Unable to change lock PIN.
    OK -Lock PIN changed."

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK    :      if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_lock_chg_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSATETSIME_EXEC_SP_CURRENT_LOCK_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $SPCURRENTLOCK command.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK    :      if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_current_lock_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_NAI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPNAI command. It prints "0" as we don't
  support more than 1 MIP session.


  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK    : if it is a success.
    DSAT_ERROR : if there was any problem in executing the command

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_nai_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SPSERVICE_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPSERVICE command.
  This should show the current service capabilites. 

  Response:
    0 -- No Service
    1 -- 1xRTT (CDMA)
    2 -- EVDO
    3 -- EVDO REV A
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
 dsat_result_enum_type dsat707_exec_spservice_cmd
(
  dsat_mode_enum_type      mode,         /* AT command mode:            */
  const dsati_cmd_type     *tab_entry,   /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,     /* Command tokens from parser  */
  dsm_item_type            *res_buff_ptr /* Place to put response       */
);
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

#if defined(FEATURE_DSAT_GPS_CMDS)
/*===========================================================================
  FUNCTION DSAT707_EXEC_A_TRACKING_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$A_TRACKING command.
  Initiates NMEA output stream.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_a_tracking_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_NMEA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPNMEA command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_nmea_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_SP_LOCATION_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$SPLOCATION command.
  It returns GPS lock state [ON / OFF].

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_sp_location_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_GPS_MODE_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$GPS_MODE command.
    0 - speed
    1 - accuracy
    2 - data
    3 - ms assisted
    4 - ms based
    5 - standalone

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_gps_mode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_PDE_TRS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$PDE_TRS command.
  Sets the GPS transport layer.
    0 - TCP
    1 - DBM
    2 - UI (SIO)
    3 - [Reserved for GCS]
    4 - [Reserved for CP]

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_pde_trs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_GPS_PDEADDR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$GPS_PDEADDR command.
  Sets the IP address and port of the PDE.
  [Data1].[Data2].[Data3].[Data4]:[Data5]

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_gps_pdeaddr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_GPS_PORT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$GPS_PORT command.
  Sets the port of the PDE.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_gps_port_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_INIT_MASK_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$INIT_MASK command.
  Sets the PE init mask to clear data structures.
    0  - Synonymous with 7
    1  - Delete ephemeris data in EFS
    2  - Delete almanac data in EFS
    4  - Delete MS position data in EFS
    7  - Delete all data in EFS
    8  - Set max MS speed to DATA2
    16 - System Init

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_gps_init_mask_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_GET_POS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$get_pos command.
  Initiates a GPS position fix.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_get_pos_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_GPS_INFO_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$GPS_INFO command.
  Gets results of last position fix requested.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_gps_info_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_GPS_STOP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$$GPS_STOP command.
  Stops position fix [seris] in progress.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_gps_stop_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* defined(FEATURE_DSAT_GPS_CMDS) */

/*===========================================================================
  FUNCTION DSAT707_EXEC_RESET_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^RESET command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.

===========================================================================*/
dsat_result_enum_type dsat707_exec_reset_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
); /* dsat707_exec_reset_cmd */

#endif /* FEATURE_DATA_IS707 */
#endif /* DSAT707VEND_H      */
