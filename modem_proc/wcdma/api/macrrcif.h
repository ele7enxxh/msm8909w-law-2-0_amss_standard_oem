#ifndef MACRRCIF_H
  #define MACRRCIF_H

/*========================================================================================

   MAC - RRC INTERFACE H E A D E R    F I L E

DESCRIPTION
  This file contains the declaration of the interface functions and
  data types used by MAC and RRC

  Copyright (c) 2001,2002 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2003-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macrrcif.h_v   1.20   10 Jul 2002 10:02:22   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/macrrcif.h#1 $  $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------------------------
01/20/16    kc      API to check if EUL is active in DCH state
07/21/14    kc      API to check if Secondary Carrier is configured at MAC
07/22/13    kc      New MRAB API - change to save CS call where PS RAB release is colliding with RLC RESET.
05/23/13    geg     WSW decoupling (except for component of QCHAT)
05/03/13    as      Fix to block data RB transimission during ongoing PS call 
                    release procedure at upper layers
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/26/12    grk     Added new API file exposing L2 command structures
04/12/12    grk     Feature cleanup.
04/03/12    grk     Feature cleanup.
03/20/12    grk     Dependency changes for modem API cleanup from FC.
07/29/11    kc      CMI Phase-4 de-featurization for Nikel 1.x
07/08/11    mk      Fixed compilation error.
05/30/11    kc      CMI Phase-4 de-featurization for Nikel 1.x
01/17/11    vp      Moved wcdma only declarations to macrrcif_v.h
11/11/10    mrg     MAC DL HS ZI memory optimization changes
10/18/10    ssg     Added changes for new WCDMA-L2 interface.
09/19/10    av      HS_FACH and Enhanced 0x4220 log changes
06/28/10    mrg     MAC-R99 ZI memory reduction changes
03/10/10    prk     featurizing opt_timer_enabled flag.
02/22/10    prk     removing featurization for opt_timer_enabled.
02/11/10    prk     MAC-hs timer optimization changes; Defined opt_timer_enabled variable inside 
                    RRC configuration structures. 
05/29/09    ssg     Added support for SNOW_3G algo.
05/07/09    ssg     Updated Copyright Information
03/12/09    rm      Moved MAC RRC "ehs" structures to public section of the file.
03/03/09    rm      Moved structures from private section to public under FEATURE_CMI
03/01/09    rm      Added new featurization for FEATURE_CMI.
10/29/08    mrg     Made changes to rrc_event_ul_amr_rate_type for event 
                    EVENT_WCDMA_UL_AMR_RATE to log the UL AMR rates.
10/27/08    ssg     Added new enums rrc_event_ul_amr_codec_type and rrc_event_ul_amr_rate_type.
                    wcdma_ul_amr_rate_event_type is defined - This is used to Post 
                    EVENT_WCDMA_UL_AMR_RATE event when AMR rate is changed.
10/17/08    grk     Merged taxis changes to mainline.
09/12/08    mrg     Added a new enum value SMC_INITIATED_NON_HO_CIPHER_CFG 
                    to mac_cipher_cfg_e_type.
05/02/08    sk      Added a new enum value MAC_LB_AND_UL_CFG_CHANGED
02/26/08    sk      Restore the next expected TSN values and the winddow up edge and low edge
                    if the restore flag is set to true. This is under
                    FEATURE_HSDPA_MAC_HS_DL_RESTORE_TSN
01/29/08    sk      Support for MBMS under FEATURE_MODEM_MBMS
09/26/07    mrg     Added support for ATF under FEATURE_CCCH_IE_ENH
11/14/07    rm      Added status field in mac_tfc_subset_cfg_cnf_type.
                    Also added new field new_tfcs_list_present in mac_ul_config_type
11/15/06    sk      Added HHO support
08/14/06    sk      Added HSUPA support
03/24/06    sk      Added function mac_rrc_get_cipher_pending_status() which will be called by
                    RRC before it sends the ciphering config.
07/18/05    sk      Added function rrc_query_mac_rab_status to get the RAB status of this call.
                    Added mac_rab_status_e_type enum type.
05/02/05    sk      Added the declaration for function rrc_get_gcf_flag_status(). This function
                    is defined in rrcdata.c.
02/15/05    gsc     Added HSDPA support under FEATURE_HSDPA feature flag.
12/22/04    sk      Added SMC_INITIATED_NON_HO_CIPHER_CFG to cipher_cfg_type. This would
                    indicate that ciphering should be started only after reaching the activation
                    time.
12/08/04    sk      Fixed compiler warnings.
12/04/04    sk      Added enum mac_cfg_changed_status_e_type. Changed the type of mac_config_changed
                    variable from a boolean to mac_cfg_changed_status_e_type.
06/17/04    sk      Added mac_tfc_subset_restriction_choice_e_type, mac_tfc_subset_cfg_cnf_type,
                    mac_ul_traffic_meas_ind_type under FEATURE_TFCS_CONTROL_ENABLE.
03/16/04    sk      Added meas_start_incl. The flag will indicate when to actually start measurements
                    Also added a #define MAX_NUM_EVENTS
08/28/03    sk      Added mac_config_changed boolean.
05/29/03    sk      Added declaration for mac_get_additional_traffic_vol_msmt_results().
05/01/03    sk      Added mac_cipher_cfg_e_type.
04/29/03    sk      Added enum to indicate if the loopback mode is LOOPBACK_MODE_1 or
                    LOOPBACK_MODE_2 or LOOPBACK_MODE_NA under FETURE_TC_CONSTANT_DELAY.
04/21/03    sk      Added a function declarartion rrc_get_start_value_to_transmit(). This function
                    is defined in rrcsmc.c and is a call  back function which is called by MAC when
                    it needs to get a new HFN.
11/21/02    scm     Remove stuff that's already defined in macl1rrcif.h.
10/03/02    sk      Chnaged the name of all_or_explicit to all_tfi_valid.
09/26/02    sk      Added MAC_TRAFFIC_MEAS_SAVE,MAC_TRAFFIC_MEAS_RESUME to
                    mac_ul_traffic_cmd_e_type;
08/02/02    sk      mac_ul_dch_trch_config_type: added boolean cs_or_ps_domain.
                    mac_dl_dch_trch_config_type: added boolean cs_or_ps_domain.
                    mac_cipher_config_type: added uint32 ciph_act_time.
                    mac_ul_config_type: added mac_cipher_config_type cipher_info[MAX_CN_DOMAIN].
                    mac_dl_config_type: added mac_cipher_config_type cipher_info[MAX_CN_DOMAIN].
                    mac_dl_ded_logch_config_type: added mac_ul_rlc_size_info_type rlc_size_info.
                    Added a new structure mac_ul_rlc_size_info_type, Added function
                    mac_rrc_get_hfn_info_type();
                    Added boolean wait_for_l1_sig in mac_ul_config_type and mac_dl_config_type.
06/08/02    sk      Removed the FEATURE_TRAFFIC_VOLUME_MEAS.
05/10/02    sk      Added structure definitions for the implementation of traffic volume
                    measurements. All the additional structure definitions, enum definitions
                    are done under FEATURE_TRAFFIC_VOLUME_MEAS.
05/08/02    yiz     Move mac_ul_tfcs_type to macl1rrcif.h and include macl1rrcif.h
                    for computed GF.
06/18/01    ps      Added RACH_TX_SUCCESS and RACH_TX_ABORTED fields to TX status Indication type
04/26/01    ps      Changed the following fields of "mac_dl_config_type":
                    - **fach_info to *fach_info[UE_MAX_TRCH]
                    - **dch_info to *dch_info[UE_MAX_TRCH]
                    - **dsch_info to *dsch_info[UE_MAX_TRCH]
04/25/01    ps      Added "tf_flag" to "mac_ul_tf_type" data structure
04/04/01    ps      Added "ctfc" field to "mac_ul_tfcs_type"
03/01/01    ps      Modified "mac_ul_asc_type" as follows:
                    - changed "double pval" to "uint32 pval"
                    - removed "pid"
02/13/01    ps      Replaced MAC_DL_MAX_DED_LOG_CHAN and MAC_UL_MAX_DED_LOG_CHAN
                    with MAX_DED_LOGCHAN_PER_TRCH
01/30/01    ps      included "uecomdef.h" and deleted "maccommdef.h"
                    modified to comply with definitions of "uecomdef.h"
01/12/01    ps      modified uplink TFCS structure
11/22/00    ps      deleted TZREX related code
11/17/00    ps      Initial release

========================================================================================*/

/*=======================================================================================

                     INCLUDE FILES

========================================================================================*/
#include        "customer.h"
#include        "uecomdef.h"
#include        "macl1rrcif.h"
#include        "wl1api.h"
#include        "wl2api.h"
/* --------------------------------- */
/* PUBLIC APIs of macrrcif.h         */
/* --------------------------------- */


/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/*========================================================================================

                       CONSTANT TYPE DATA DECLARATIONS

========================================================================================*/

  #define         MAX_NUM_EVENTS                          2


/*========================================================================================

                       ENUMERATED TYPE DATA DECLARATIONS

========================================================================================*/



/* Indicates what kind of a call it is. CS only, PS only or Multirab call */
typedef enum
{
  NO_RABS,
  CS_RABS,
  PS_RABS,
  CS_PLUS_PS_RABS
}mac_rab_status_e_type;




/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement results for a single radio bearer                         */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Radio bearer identity */
  uint16  rb_id;

  /* Raw rlc buffer payload value */
  uint32  raw_rlc_payload;

  /* average rlc buffer payload value */
  uint32  average_rlc_payload;

  /* variance of rlc buffer payload */
  uint32  variance_rlc_payload;

}mac_ul_traffic_rb_meas_ind_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement event results                                             */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Uplink transport channel type causing the event */
  mac_ul_traffic_trch_info_type           trch_info;

  /* Event identity whether it was underflow or overflow */
  mac_ul_traffic_event_id_e_type  event_id;

}mac_ul_traffic_event_result;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement results for all the radio bearers                         */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Measurement identity */
  uint16   meas_id;

  /* Number of Radio bearers */
  uint16   num_rbs;

  /* True indicates that MAC has sent to RRC the maximum number of reports that */
  /* MAC was configured to send for this measurement ID. This is in the case of */
  /* periodic mode reporting criteria                                           */
  boolean num_reports_ind;

  /* Indicates if it is a periodic or event trigger */
  mac_ul_traffic_report_mode_e_type report_mode;

  /* traffic volume measurements of all the radio bearers multiplexed onto the  */
  /* indicated transport channels */
  mac_ul_traffic_rb_meas_ind_type  rb_meas[MAX_RB];

  /* Event results */
  mac_ul_traffic_event_result  event_result;

  /* Indicates the report transfer mode (UM or AM ) */
  mac_ul_traffic_report_transfer_mode_e_type  report_transfer_mode;

}mac_ul_traffic_meas_ind_type;

/*--------------------------------------------------------------------------------------*/
/* MAC to RRC acknowledgement command. RRC_CMAC_MEAS_CTRL_CNF                           */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Given in Meas Req command to MAC as config_ack_num;*/
  uint16 config_ack_num;

  /* measurement identity */
  uint16 meas_id;

  /* True indicates that the command was accepted */
  boolean status;

}mac_meas_ctrl_cnf_type;

/*--------------------------------------------------------------------------------------*/
/* RRC to MAC config command for TFCC info. CMAC_UL_TFCC_SUBSET_CFG_REQ                 */
/*--------------------------------------------------------------------------------------*/


typedef struct
{
  uint8             transaction_id;

  rb_id_type        rb_id;
/* Indicates if the validation at MAC was succesful or not*/
  boolean status;
}mac_tfc_subset_cfg_cnf_type;


typedef enum
{
    WCDMA_TRCH_INVALID,
	WCDMA_TRCH_BCH, 
	WCDMA_TRCH_PCH,
	WCDMA_TRCH_FACH,
	WCDMA_TRCH_DCH, 
	WCDMA_TRCH_HSDSCH, 
	WCDMA_TRCH_RACH, 
	WCDMA_TRCH_EDCH
}log_wcdma_trch_type;
/*===========================================================================
FUNCTION rrcrcr_is_proc_active

DESCRIPTION
  Returns rcr sub-state
===========================================================================*/
extern  boolean rrcrcr_is_proc_active( void );

/*===========================================================================
FUNCTION   rrcrb_query_mac_rab_status

DESCRIPTION
This function will be called by MAC at configuration time to determine
if it is a CS only or PS only call. Based on this MAC will determine how
the AMR rate changes need to be applied based on the power.

DEPENDENCIES
  None.

RETURN VALUE
Function returns if the call is of type CS only, PS only or CS+PS call.

SIDE EFFECTS
  None.
===========================================================================*/
extern mac_rab_status_e_type rrc_query_mac_rab_status( void );

/*===========================================================================
FUNCTION          rrc_query_mrab_status

DESCRIPTION
  This function checks if CS voice and PS RAB is present 

DEPENDENCIES
  None.

RETURN VALUE
 Boolean

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean rrc_query_mrab_status( void );

#ifdef FEATURE_WCDMA_DC_HSUPA
boolean mac_hs_ul_is_sec_carrier_active();
#endif

/*API to check if EUL is active at MAC in DCH state */
boolean mac_is_eul_active();

#endif

