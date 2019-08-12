#ifndef     MACUL_H
#define     MACUL_H
/*=========================================================================================

                        U P L I N K   M A C  H E A D E R   F I L E


Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

========================================================================================*/

/*========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macul.h_v   1.5   10 Jul 2002 10:04:24   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/macul.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------
12/15/14   as      In FACH state MAC should send CMAC_STATUS_IND only for TM CCCH UL data.
08/21/14   ts      FR22375: Fixed Medium compiler warnings in WCDMA on Bolt 2.0/2.1
03/19/14   kc      Changing FEATURE for heap corruption issue.
03/18/14   ts      Fix for heap memory corruption issue, to allocate and free heap memory fo rlog buffers in l2ultask itself
10/11/12   mrg     TVM optimization for MRAB
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/05/12   mrg     HS-RACH & MAC-I Feature code integrated
03/06/12   mrg     RACH staus info from L1 to MAC indicaiton with signal based approch enhanced with double buffer.
01/02/12   grk     Feature cleanup
12/06/11   ksr     Fix to handle back to back RACH ABORT and RACH NO ACK to MAC from L1
                   Moving to command based interface from signal based approach.
10/22/11   sk      NikeL UL CM workaround under feature FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
06/02/11   kc      Added support for logical channel to transport channel mapping info
11/11/10   mrg     MAC DL HS ZI memory optimization changes
09/16/10   mrg     Moved auido(MVS) dependency under the feature flag FEATURE_MVS
06/28/10   mrg     MAC-R99 ZI memory reduction changes
10/07/09   mrg     Fixed Klocwork warnings
05/07/09   ssg     Updated Copyright Information
04/29/09    av     Fixed compiler warnings in L2 
03/13/08    sk     Added MAC_CHECK_HSUPA_CONFIG_STATUS declaration
10/17/07    ps     Logic for deriving AMR rate based on sum of class A,
                   Class B and Class C bits.
03/20/07    sk     Added extern uint32 mac_get_cnt_c_val under FEATURE_WCDMA_CNT_C_SFN_DIFF
03/03/07    sk     Added support for WB-AMR under FEATURE_VOC_AMR_WB
10/16/06    sk     Fixed compiler warning
09/21/05   gsc     Added support for Rel-5 spec CR 2272.
07/20/05   gsc     Added support for new interface between L1/MAC for CFN reporting
                   to MAC by L1. This new mechanism in enabled when
                   FEATURE_WCDMA_HFN_UPDATE is not defined.
05/05/05    sk     Added protoype for mac_asc_selection()
03/09/05    sk     Removed ## from the macro definitions.
12/30/04    sk     Fixed lint errors
06/05/05    sk     Added MAC_UL_TFC_MAX to line it with L1_UL_TFC_MAX that L1 has for
                   number of TFC's for which the excess power state is computed
04/26/04    sk     Removed the number of ded logical channel constraints for the minimum
                   in the macro UL_DCCH_DTCH_NUM_IS_INVALID definition
04/14/04    sk     Added a macro GET_UL_TRCH_TF_INFO_PTR
12/26/03    sk     Added support for TFCS selection based on logical channel prioritites
                   under FEATURE_TFCS_LC_PRIORITIES
09/04/02    sk     Added extern declarations for some global variables under feature
                   definition
06/08/02    sk     Removed FEATURE_TRAFFIC_VOLUME_MEAS
05/13/02    sk     Defined macro GET_UL_TRCH_ID_TO_IDX_PTR. This macro would give the
                   transport channel parameters for a given transport channel id. This
                   addition is made under the FEATURE_TRAFFIC_VOLUME_MEAS
02/12/02    ps     Deleted UL U-RNTI related definitions and declarations
06/19/01    ps     Converted UL TX Status and Build Frame Indications from commands to signals
06/18/01    ps     declared ul_cctrch_info as external

=========================================================================================*/

#include "wcdma_variation.h"
#include "macinternal.h"
#include "mactraffic.h"
#include "stringl.h"

#ifdef FEATURE_MVS
#include "mvs.h"
#endif

#ifdef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION
#include "modem_mem.h"
#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */

#define MAC_UL_TFC_MAX 64

#ifdef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION
extern rlc_ul_frame_data_type    *rlc_ul_data_info_ptr;
#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */

/*------------------------------------------------------------------
At any given time MAC stores 2 ciphering configurations.
cur_ul_cipher_info stores the currently used ciphering configuration.
new_ul_cipher_info refers to the configuration that MAC would switch
to after ciphering activation time.
-------------------------------------------------------------------*/
extern mac_cipher_config_type cur_ul_cipher_info[MAX_CN_DOMAIN];

extern mac_cipher_config_type new_ul_cipher_info[MAX_CN_DOMAIN];

extern boolean ul_ciph_pending[MAX_CN_DOMAIN];

extern uint32 count_c[MAX_CN_DOMAIN];

extern boolean mac_rach_ccch_data_presence;

/*-----------------------------------------------------------------------
  This struct is added to implement Rel-5 Spec CR 2272. While in
  the middle of ciphering configuration and prior to reaching the activation
  time, if MAC receives a new ciphering config indicated to MAC by RRC
  as SMC_INITIATED_NON_HO_CIPHER_CFG, MAC will save the ciphering config
  and apply the config when the pending config's activation time has reached.
-----------------------------------------------------------------------*/
typedef struct {
  mac_cipher_config_type  cipher_info;
  boolean                 cipher_info_saved;
}mac_saved_cipher_info_type;


/*========================================================================================
 FUNCTION NAME  proc_ul_cipher_info

 DESCRIPTION
    This function processes the UL Ciphering information received from RRC

=========================================================================================*/
extern  void    proc_ul_cipher_info
(
    /* Pointer to UL MAC Ciphering information */
    mac_cipher_config_type   *cipherptr
);

/* PRACH transmit status */
extern      l1_prach_status_enum_type       rach_tx_status[2];

#ifdef FEATURE_WCDMA_HS_RACH
/* PRACH transmit status */
extern l1_hsrach_status_enum_type   hs_rach_tx_status[2]; //Made double buffer to store 2 status info
#endif

extern      boolean     ul_mac_crnti_valid;

extern      uint16      ul_mac_crnti;


/* Populate the index table as we fill in the int_ul_trch_info table */
extern      trch_id_to_idx_map_type ul_trch_id_to_idx_table[MAX_TRCH];

#define     GET_UL_TRCH_ID_TO_IDX_PTR(trch_id) &ul_trch_id_to_idx_table[trch_id-MAC_UL_MIN_TRCH_ID]

/* MACRO to check Trch id_to_idx is valid or not */
#define   TRCH_ID_TO_IDX_IS_INVALID(trch_id)\
            ((trch_id < MAC_UL_MIN_TRCH_ID) || (trch_id > MAX_TRCH))

#define     UL_MAC_CRNTI_IS_VALID   (ul_mac_crnti_valid)

#define     GET_UL_MAC_CRNTI        (ul_mac_crnti)

/* Selected TFCI */
extern      ul_cctrch_info_type         int_ul_cctrch_info;
#define     GET_SELECTED_TFCI           int_ul_cctrch_info.selected_tfci
#define     SET_SELECTED_TFCI(tfci)     int_ul_cctrch_info.selected_tfci = tfci;
#define     GET_UL_CCTRCH_TYPE          int_ul_cctrch_info.cctrch_type

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
/* FOR FREE FLOATING ... A mutex shared between L1 and MAC*/
/* Mutex to perform atomic state transistions */
extern rex_crit_sect_type wl1_tfc_excess_pwr_states_mutex;

#define MAC_WL1_TFC_EXCESS_PWR_INTLOCK()  REX_ISR_LOCK(&wl1_tfc_excess_pwr_states_mutex)
#define MAC_WL1_TFC_EXCESS_PWR_INTFREE()  REX_ISR_UNLOCK(&wl1_tfc_excess_pwr_states_mutex)

#else

#define MAC_WL1_TFC_EXCESS_PWR_INTLOCK()  {}
#define MAC_WL1_TFC_EXCESS_PWR_INTFREE()  {}

#endif /* FEATURE_WCDMA_FREE_FLOATING_TASK */
/*===========================================================================================
FUNCTION NAME   proc_ul_rnti_info

PURPOSE         Process the UL RNTI information received from RRC

PARAMETERS      rntiptr : Pointer to the RNTI information

DESCRIPTION     If the RNTI info is valid, the received RNTI information
                is saved in MAC's local memory
===========================================================================================*/
extern  void    proc_ul_rnti_info
(
    /* Pointer to the RNTI information */
    mac_rnti_config_type   *rntiptr
);

/* MACRO for UL Logical channel information pointer */
extern      ul_lc_info_type                 int_ul_lc_info[];
#define     GET_UL_LC_INFO_PTR(rlc_id)      &int_ul_lc_info[rlc_id - MAC_UL_MIN_RLC_ID]

/* MACRO for UL Transport Channel information pointer */
extern      ul_trch_info_type               int_ul_trch_info[];
#define     GET_UL_TRCH_INFO_PTR(trch_idx)  &int_ul_trch_info[trch_idx]


/* MACRO for getting the logical channel id's of all the logical channels
which are mapped onto a particular transport channel having a particular
priority */
#ifndef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION
extern      maci_ul_trch_priority_info_type     int_ul_trch_priority_info[][MAC_UL_MAX_MLPS];
#else
extern      maci_ul_trch_priority_info_type     **int_ul_trch_priority_info;
#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */

#define     GET_UL_TRCH_PRIORITY_INFO_PTR(trch_idx, priority) (&int_ul_trch_priority_info[trch_idx][priority - MAC_UL_MIN_MLP_ID])

extern      maci_ul_trch_tf_type int_ul_trch_tf_info[UE_MAX_TRCH][UE_MAX_TF];
#define   GET_UL_TRCH_TF_INFO_PTR(trch_idx, tf) (&int_ul_trch_tf_info[trch_idx][tf])

/* MACRO to check the UL Logical Channel Priority */
#define     UL_LC_PRIORITY_IS_INVALID(priority)\
            ((priority < MAC_UL_MIN_MLP_ID ) || (priority > MAC_UL_MAX_MLP_ID ))

/* MACRO to check the UL ASC validity */
#define     UL_ASC_IS_INVALID(asc)          (asc > MAC_UL_MAX_ASC_ID)

/* MACRO to check UL RLC ID */
#define     UL_RLC_ID_IS_INVALID(rlc_id)    (rlc_id > MAC_UL_MAX_RLC_ID)

/* MACRO to check UL RB ID */
#define     UL_RB_ID_IS_INVALID(rb_id)      (rb_id > MAC_UL_MAX_RB_ID)

/* MACRO to check UL Transport Channel ID */
#define     UL_TRCH_ID_IS_INVALID(trch_id)\
            ((trch_id < MAC_UL_MIN_TRCH_ID) || (trch_id > MAC_UL_MAX_TRCH_ID))

/* MACRO to check RLC Mode */
#define     UL_RLC_MODE_IS_INVALID(rlc_mode)\
            ((rlc_mode >= UE_MODE_MAX_NUM))

/* MACRO to check MAC ID */
#define     UL_MAC_ID_IS_INVALID(mac_id)\
            ((mac_id < MAC_MIN_MAC_ID ) || (mac_id > MAC_MAX_MAC_ID))

/* Condition to check invalid number of DCCH/DTCHs */
#define     UL_DCCH_DTCH_NUM_IS_INVALID(ndlchan)\
            (ndlchan > MAX_DED_LOGCHAN_PER_TRCH)

/*
* The following constants are used when determining the Logical
* channel mapping information for the vocoder AMR Modes and Classes.
* Some of these are not actually used in the code anywhere, but it's
* nice to have them here for completeness.  The unused ones are
* commented out to prevent lint complaints.
*/
/* #define WVS_AMR_MODE_CLASS_C_0_BIT_SIZE     0 */

/* AMR 4.75 kbps speech */
#define AMR_MODE_475_CLASS_A_BIT_SIZE  42
#define AMR_MODE_475_CLASS_B_BIT_SIZE  53

/* AMR 5.15 kbps speech */
#define AMR_MODE_515_CLASS_A_BIT_SIZE  49
#define AMR_MODE_515_CLASS_B_BIT_SIZE  54

/* AMR 5.90 kbps speech */
#define AMR_MODE_590_CLASS_A_BIT_SIZE  55
#define AMR_MODE_590_CLASS_B_BIT_SIZE  63

/* AMR 6.70 kbps speech */
#define AMR_MODE_670_CLASS_A_BIT_SIZE  58
#define AMR_MODE_670_CLASS_B_BIT_SIZE  76

/* AMR 7.40 kbps speech */
#define AMR_MODE_740_CLASS_A_BIT_SIZE  61
#define AMR_MODE_740_CLASS_B_BIT_SIZE  87

/* AMR 7.95 kbps speech */
#define AMR_MODE_795_CLASS_A_BIT_SIZE  75
#define AMR_MODE_795_CLASS_B_BIT_SIZE  84

/* AMR 10.2 kbps speech */
#define AMR_MODE_102_CLASS_A_BIT_SIZE  65
#define AMR_MODE_102_CLASS_B_BIT_SIZE  99
#define AMR_MODE_102_CLASS_C_BIT_SIZE  40

/* AMR 12.2 kbps speech */
#define AMR_MODE_122_CLASS_A_BIT_SIZE  81
#define AMR_MODE_122_CLASS_B_BIT_SIZE 103
#define AMR_MODE_122_CLASS_C_BIT_SIZE  60

/* AMR Mode 8 (SID) */
#define AMR_MODE_8_CLASS_A_BIT_SIZE   39

#define AMR_CLASS_NO_DATA_FRAME_SIZE 0

#define  AMR_MODE_475_TOTAL_BITS  (AMR_MODE_475_CLASS_A_BIT_SIZE + \
                                                                                 AMR_MODE_475_CLASS_B_BIT_SIZE)
#define  AMR_MODE_515_TOTAL_BITS (AMR_MODE_515_CLASS_A_BIT_SIZE + \
                                                                          AMR_MODE_515_CLASS_B_BIT_SIZE)
#define AMR_MODE_590_TOTAL_BITS (AMR_MODE_590_CLASS_A_BIT_SIZE + \
                                                                          AMR_MODE_590_CLASS_B_BIT_SIZE)
#define AMR_MODE_670_TOTAL_BITS (AMR_MODE_670_CLASS_A_BIT_SIZE + \
                                                                          AMR_MODE_670_CLASS_B_BIT_SIZE)
#define AMR_MODE_740_TOTAL_BITS (AMR_MODE_740_CLASS_A_BIT_SIZE + \
                                                                         AMR_MODE_740_CLASS_B_BIT_SIZE)
#define AMR_MODE_795_TOTAL_BITS (AMR_MODE_795_CLASS_A_BIT_SIZE + \
                                                                         AMR_MODE_795_CLASS_B_BIT_SIZE)
#define AMR_MODE_102_TOTAL_BITS (AMR_MODE_102_CLASS_A_BIT_SIZE + \
             AMR_MODE_102_CLASS_B_BIT_SIZE + AMR_MODE_102_CLASS_C_BIT_SIZE)
#define AMR_MODE_122_TOTAL_BITS (AMR_MODE_122_CLASS_A_BIT_SIZE + \
             AMR_MODE_122_CLASS_B_BIT_SIZE + AMR_MODE_122_CLASS_C_BIT_SIZE)

#ifdef FEATURE_VOC_AMR_WB

#define AMR_WB_CLASS_A_BIT_SIZE  40

#define AMR_WB_MODE_660_TOTAL_BITS (MVS_AMR_WB_MODE_660_CLASS_A_BIT_SIZE + \
                                                                            MVS_AMR_WB_MODE_660_CLASS_B_BIT_SIZE)
#define AMR_WB_MODE_885_TOTAL_BITS (MVS_AMR_WB_MODE_885_CLASS_A_BIT_SIZE + \
                                                                            MVS_AMR_WB_MODE_885_CLASS_B_BIT_SIZE)
#define AMR_WB_MODE_1265_TOTAL_BITS (MVS_AMR_WB_MODE_1265_CLASS_A_BIT_SIZE + \
                                                                           MVS_AMR_WB_MODE_1265_CLASS_B_BIT_SIZE)
#define AMR_WB_MODE_1425_TOTAL_BITS (MVS_AMR_WB_MODE_1425_CLASS_A_BIT_SIZE+  \
                                                                           MVS_AMR_WB_MODE_1425_CLASS_B_BIT_SIZE)
#define AMR_WB_MODE_1585_TOTAL_BITS (MVS_AMR_WB_MODE_1585_CLASS_A_BIT_SIZE + \
                                                                           MVS_AMR_WB_MODE_1585_CLASS_B_BIT_SIZE)
#define AMR_WB_MODE_1825_TOTAL_BITS (MVS_AMR_WB_MODE_1825_CLASS_A_BIT_SIZE +  \
                                                                           MVS_AMR_WB_MODE_1825_CLASS_B_BIT_SIZE)
#define AMR_WB_MODE_1985_TOTAL_BITS (MVS_AMR_WB_MODE_1985_CLASS_A_BIT_SIZE +  \
                                                                           MVS_AMR_WB_MODE_1985_CLASS_B_BIT_SIZE)
#define AMR_WB_MODE_2305_TOTAL_BITS (MVS_AMR_WB_MODE_2305_CLASS_A_BIT_SIZE +  \
                                                                         MVS_AMR_WB_MODE_2305_CLASS_B_BIT_SIZE)
#define AMR_WB_MODE_2385_TOTAL_BITS (MVS_AMR_WB_MODE_2385_CLASS_A_BIT_SIZE +  \
                                                                         MVS_AMR_WB_MODE_2385_CLASS_B_BIT_SIZE)

/* WB-AMR 6.60 kbps bit size */
#define MVS_AMR_WB_MODE_660_CLASS_A_BIT_SIZE    54
#define MVS_AMR_WB_MODE_660_CLASS_B_BIT_SIZE    78

/* WB-AMR 8.85 kbps bit size */
#define MVS_AMR_WB_MODE_885_CLASS_A_BIT_SIZE    64
#define MVS_AMR_WB_MODE_885_CLASS_B_BIT_SIZE    113

/* WB-AMR 12.65 kbps bit size */
#define MVS_AMR_WB_MODE_1265_CLASS_A_BIT_SIZE   72
#define MVS_AMR_WB_MODE_1265_CLASS_B_BIT_SIZE   181

/* WB-AMR 14.25 kbps bit size */
#define MVS_AMR_WB_MODE_1425_CLASS_A_BIT_SIZE   72
#define MVS_AMR_WB_MODE_1425_CLASS_B_BIT_SIZE   213

/* WB-AMR 15.85 kbps bit size */
#define MVS_AMR_WB_MODE_1585_CLASS_A_BIT_SIZE   72
#define MVS_AMR_WB_MODE_1585_CLASS_B_BIT_SIZE   245

/* WB-AMR 18.25 kbps bit size */
#define MVS_AMR_WB_MODE_1825_CLASS_A_BIT_SIZE   72
#define MVS_AMR_WB_MODE_1825_CLASS_B_BIT_SIZE   293

/* WB-AMR 19.85 kbps bit size */
#define MVS_AMR_WB_MODE_1985_CLASS_A_BIT_SIZE   72
#define MVS_AMR_WB_MODE_1985_CLASS_B_BIT_SIZE   325

/* WB-AMR 23.05 kbps bit size */
#define MVS_AMR_WB_MODE_2305_CLASS_A_BIT_SIZE   72
#define MVS_AMR_WB_MODE_2305_CLASS_B_BIT_SIZE   389

/* WB-AMR 23.85 kbps bit size */
#define MVS_AMR_WB_MODE_2385_CLASS_A_BIT_SIZE   72
#define MVS_AMR_WB_MODE_2385_CLASS_B_BIT_SIZE   405

#endif /* FEATURE_VOC_AMR_WB */

#ifdef FEATURE_WCDMA_MRAB_TVM_MAC_OPT
#error code not present
#endif /* FEATURE_WCDMA_MRAB_TVM_MAC_OPT */


/*=====================================================================

 FUNCTION NAME  config_ul_mac

 PURPOSE        Processes UL MAC Configuration Data

 PARAMETERS     cfgptr - Pointer to the UL MAC Configuration data

 DESCRIPTION    This function performs the following functions:
                - saves configuration related information in local memory
                - if configured for RACH, indicates RLC to notify
                  when there is an SDU in its uplink queue
                - updates uplink logical channel information table
=====================================================================*/
extern  void    config_ul_mac
(
    /* Pointer to UL MAC Configuration Data */
    mac_ul_config_type *cfgptr
);
#ifdef FEATURE_MAC_HS_UL_QXDM_LOGGING
/*===========================================================================
FUNCTION: mac_eul_deallocate_mem_log_records

DESCRIPTION:
This function frees up dynamic memory allocated for EUL 0x430E log record pointer
===========================================================================*/
void mac_eul_deallocate_mem_log_records(void);
#endif

#ifdef FEATURE_WCDMA_MULTIRATE_AMR
/*====================================================================
FUNCTION: mac_ul_compute_amr_rate()

DESCRIPTION:
  This function looks at the uplink tfcs table and computes the new
  rate based on what TFCI's are allowed at this point. Based on the
  allowed TFCI's the rate is computed and vocoder is configured with
  this new rate.

  This function is used to change rates for only the AMR. No effect
  on the PS.

DEPENDENCIES:
  VOCODER has to support multiple rates. RRC will have to support
  multiple rates for AMR.

RETURN VALUE:

  The function determines the rate with which MVS should be configured.
  It determines the lowest rate defined based on the tfcs table.
  It also determines if its a single_rate_amr or multi_rate_amr.

SIDE EFFECTS:
  There is a worst case delay of 40ms and we can lose upto one frame
  in the process of siwtching the rates

====================================================================*/
void
mac_ul_compute_amr_rate
(
  mac_amr_mode_info_type *mac_amr_mode_info_ptr
);

/*====================================================================
FUNCTION: mac_ul_compute_amr_rate()

DESCRIPTION:
  This function calls the mvs function to set the rate

DEPENDENCIES:


RETURN VALUE:



SIDE EFFECTS:


====================================================================*/
void
mac_ul_set_amr_rate
(
  mvs_amr_mode_type amr_mode
);

#endif

/*===============================================================================================
FUNCTION NAME mac_asc_selection

DESCRIPTION   Selects an Access Service Class

RETURNS     0 - 7 Depending on the selected ASC
===============================================================================================*/
uint8 mac_asc_selection (rlc_ul_frame_format_type *chan_fmt);

/*==============================================================================================
 FUNCTION NAME  void mac_update_ciphering_params

 DESCRIPTION : 1. MAC maintains 2 configurations for ciphering. Current Ciphering Parameters (cur_ul_cipher_info)
 and New Ciphering Parameters (new_ul_cipher_info).

 2. If there is an activation time pending, then new_ul_cipher_info is used. The parameters
 are copied into cur_ul_cipher_info at activation time.

 3. cfn_hfn_table is a map of the CFN and the corresponding HFN. CFN serves as the
 index of this array and the value at an index is the HFN. DL MAC reads from this
 table to get the HFN associated with a CFN. UL MAC maintains this table.

 4. When MAC starts ciphering for the first time and an activation time is pending,
   the sequence to increment the HFN is Reach Activation Time, ROLL OVER.
   To illustrate lets consider 2 cases

   Case 1: ciphering_activation_time = 12 and CFN = 30. Now the CFN has to roll over and then reach the
   activation time of 12. HFN would be incremented only after CFN rolls over after the activation time.

   Case 2: ciphering_activation_time = 220 and CFN = 5. Ciphering Activation time is reached before the
   CFN roll over and hence not treated as a special case.

   The flag first_time_rolled_over serves to address the case 1 above for us not to increment HFN
   if there is a CFN roll over and ciphering_activation_time is not reached.

 5. Subsequently every time there is a roll over MAC would increment the HFN.
 roll over is detected by the following piece of code :
 if ( cfn < prev_cfn). MAC maintains the last CFN value. This code is generic and would
 be able to handle even couple of build frame indications get missed for some reason.

 6. In HO_STEP1_CIPHER_CFG : the ciphering_activation_pending flag would be set to FALSE.
 The HFN would not be incremented even if we reach activation time and there are roll overs.
 This is a temporary state when WCDMA has received a request for Handover from GSM and WCDMA
 side has not yet sent the confirm. The same is applicable for hard handovers.

 7. In HO_STEP2_CIPHER_CFG : This configuration is received when WCDMA has sent a confirm.
 On receiving this configuration, MAC would increment the HFN by 1 when it hits activation
 time without waiting for the roll over. After this, the behaviour is as in the case of
 NON_HO_CIPHER_CFG.

===============================================================================================*/

void mac_update_ciphering_params
(
  uint8 cfn
);

#ifdef FEATURE_WCDMA_CNT_C_SFN_DIFF

/*-------------------------------------------------------------------
FUNCTION uint32 mac_get_cnt_c_val()

DESCRIPTION
This function will be called by L1 to get teh count_c value

DEPENDENCIES
  None

RETURN VALUE
  The count_c value correspondign the current cfn will be returned to L1

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
extern uint32 mac_get_cnt_c_val(uint8 cfn);

#endif /* FEATURE_WCDMA_CNT_C_SFN_DIFF */

#ifdef FEATURE_WCDMA_HSUPA
/*-------------------------------------------------------------------
FUNCTION boolean MAC_CHECK_HSUPA_CONFIG_STATUS()

DESCRIPTION
This function will look to see if ul configuration is pending and
if yes then whether mac config has changed or not and whether
mac_reset_indicator is set to true.

DEPENDENCIES
  None

RETURN VALUE
  Boolean indicates that mac configuration has chnaged and mac_reset_indicator
  is set to TRUE

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
mac_ul_config_type* mac_check_hsupa_config_status(void);

#endif

/*================================================================================================
 FUNCTION NAME  init_ul_mac_ciphering_params

 DESCRIPTION  This function initialises UL MAC ciphering parameters
================================================================================================*/
void  init_ul_mac_ciphering_params
(
  void
);


/*=================================================================================================
 FUNCTION NAME  proc_ul_mac_tfcc_command

 PURPOSE    This function process UL TFCC config Commands
================================================================================================*/

void proc_ul_mac_tfcc_command
(
  mac_tfc_subset_restriction_info_type *tfcc_config_ptr
);

/*=================================================================================================
 FUNCTION NAME    proc_ul_cctrch_info

 DESCRIPTION    This function processes uplink CCTrCh information
=================================================================================================*/
void    proc_ul_cctrch_info
(
  /* Pointer to ordered config data of UL CCTrCh */
  mac_ul_cctrch_config_type *cctrch_ptr
);


#ifdef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION

/*-------------------------------------------------------------------
FUNCTION mac_ul_free_mem_ZI_opt

DESCRIPTION
 This function will be called by RRC in the call back , so that MAC can de-allocate/free
 the dynamic memory allocated data strucutre varaibles.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
--------------------------------------------------------------------*/

void  mac_ul_free_mem_ZI_opt(void);

#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */

/*===============================================================================================
FUNCTION NAME mac_ul_get_trch_type

DESCRIPTION   This function returns the up-link transport channel type corresponding to given RLC & RB IDs
===============================================================================================*/

log_wcdma_trch_type mac_ul_get_trch_type
  (
   /* RLC logical channel buffer identifier (0-20) */
   rlc_lc_id_type rlc_id, 

   /* Radio Bearer Identifier (0-31) */
   rb_id_type rb_id
  );


/*===============================================================================================
FUNCTION NAME mac_ul_check_edch_trch

DESCRIPTION   This function checks if given rlc-id and rb-id pair map to a UL-EDCH transport channel
===============================================================================================*/

log_wcdma_trch_type mac_ul_check_edch_trch
(
   /* RLC logical channel buffer identifier (0-20) */
   rlc_lc_id_type rlc_id, 

   /* Radio Bearer Identifier (0-31) */
   rb_id_type rb_id
);
#endif


