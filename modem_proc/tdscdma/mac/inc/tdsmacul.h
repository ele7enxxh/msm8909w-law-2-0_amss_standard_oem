#ifndef     TDSMACUL_H
#define     TDSMACUL_H
/*=========================================================================================

                        U P L I N K   M A C  H E A D E R   F I L E

Copyright (c) 2002-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.


========================================================================================*/

/*========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macul.h_v   1.5   10 Jul 2002 10:04:24   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/mac/inc/tdsmacul.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------
02/02/11   ps      Changes for MAC HSUPA code
10/24/10   guo     Initial release Upmerged WCDMA to TDS

=========================================================================================*/

#include "tdscdma_variation.h"
#include "tdsmacinternal.h"
#include "tdsmactraffic.h"
#ifdef FEATURE_MVS
#include "mvs.h"
#endif

#include "modem_mem.h"

#define TDSMAC_UL_TFC_MAX 64

#ifdef FEATURE_TDSCDMA_TFC_ROUNDROBIN
#define TDSMAC_INVALID_PREV_TFCI 0xFFFF
#endif

extern tdsrlc_ul_frame_data_type    *tdsrlc_ul_data_info_ptr;




/*------------------------------------------------------------------
At any given time MAC stores 2 ciphering configurations.
tdscur_ul_cipher_info stores the currently used ciphering configuration.
tdsnew_ul_cipher_info refers to the configuration that MAC would switch
to after ciphering activation time.
-------------------------------------------------------------------*/
extern tdsmac_cipher_config_type tdscur_ul_cipher_info[TDSMAX_CN_DOMAIN];

extern tdsmac_cipher_config_type tdsnew_ul_cipher_info[TDSMAX_CN_DOMAIN];

extern boolean tdsul_ciph_pending[TDSMAX_CN_DOMAIN];

extern uint32 tdscount_c[TDSMAX_CN_DOMAIN];


/*-----------------------------------------------------------------------
  This struct is added to implement Rel-5 Spec CR 2272. While in
  the middle of ciphering configuration and prior to reaching the activation
  time, if MAC receives a new ciphering config indicated to MAC by RRC
  as TDSSMC_INITIATED_NON_HO_CIPHER_CFG, MAC will save the ciphering config
  and apply the config when the pending config's activation time has reached.
-----------------------------------------------------------------------*/
typedef struct {
  tdsmac_cipher_config_type  cipher_info;
  boolean                 cipher_info_saved;
}tdsmac_saved_cipher_info_type;


/*========================================================================================
 FUNCTION NAME  tdsproc_ul_cipher_info

 DESCRIPTION
    This function processes the UL Ciphering information received from RRC

=========================================================================================*/
extern  void    tdsproc_ul_cipher_info
(
    /* Pointer to UL MAC Ciphering information */
    tdsmac_cipher_config_type   *cipherptr
);



extern      boolean     tdsul_mac_crnti_valid;

extern      uint16      tdsul_mac_crnti;


/* Populate the index table as we fill in the tdsint_ul_trch_info table */
extern      tdstrch_id_to_idx_map_type tdsul_trch_id_to_idx_table[MAX_TRCH];

#define     TDSGET_UL_TRCH_ID_TO_IDX_PTR(tdstrch_id) &tdsul_trch_id_to_idx_table[tdstrch_id-TDSMAC_UL_MIN_TRCH_ID]

/* MACRO to check Trch id_to_idx is valid or not */
#define   TDSTRCH_ID_TO_IDX_IS_INVALID(tdstrch_id)\
            ((tdstrch_id < TDSMAC_UL_MIN_TRCH_ID) || (tdstrch_id > MAX_TRCH))

#define     TDSUL_MAC_CRNTI_IS_VALID   (tdsul_mac_crnti_valid)

#define     TDSGET_UL_MAC_CRNTI        (tdsul_mac_crnti)

/* Selected TFCI */
extern      tdsul_cctrch_info_type         tdsint_ul_cctrch_info;
#define     TDSGET_SELECTED_TFCI           tdsint_ul_cctrch_info.selected_tfci
#define     TDSSET_SELECTED_TFCI(tfci)     tdsint_ul_cctrch_info.selected_tfci = tfci;
#define     TDSGET_UL_CCTRCH_TYPE          tdsint_ul_cctrch_info.cctrch_type

#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
/* FOR FREE FLOATING ... A mutex shared between L1 and MAC*/
/* Mutex to perform atomic state transistions */
extern rex_crit_sect_type tdsl1_tfc_excess_pwr_states_mutex;

#define TDSMAC_L1_TFC_EXCESS_PWR_INTLOCK()  REX_ISR_LOCK(&tdsl1_tfc_excess_pwr_states_mutex)
#define TDSMAC_L1_TFC_EXCESS_PWR_INTFREE()  REX_ISR_UNLOCK(&tdsl1_tfc_excess_pwr_states_mutex)

#else

#define TDSMAC_L1_TFC_EXCESS_PWR_INTLOCK()  {}
#define TDSMAC_L1_TFC_EXCESS_PWR_INTFREE()  {}

#endif /*FEATURE_TDSCDMA_FREE_FLOATING_TASK*/

/*===========================================================================================
FUNCTION NAME   tdsproc_ul_rnti_info

PURPOSE         Process the UL RNTI information received from RRC

PARAMETERS      rntiptr : Pointer to the RNTI information

DESCRIPTION     If the RNTI info is valid, the received RNTI information
                is saved in MAC's local memory
===========================================================================================*/
extern  void    tdsproc_ul_rnti_info
(
    /* Pointer to the RNTI information */
    tdsmac_rnti_config_type   *rntiptr
);

/* MACRO for UL Logical channel information pointer */
extern      tdsul_lc_info_type                 tdsint_ul_lc_info[];
#define     TDSGET_UL_LC_INFO_PTR(rlc_id)      &tdsint_ul_lc_info[rlc_id - TDSMAC_UL_MIN_RLC_ID]

/* MACRO for UL Transport Channel information pointer */
extern      tdsul_trch_info_type               tdsint_ul_trch_info[];
#define     TDSGET_UL_TRCH_INFO_PTR(trch_idx)  &tdsint_ul_trch_info[trch_idx]


/* MACRO for getting the logical channel id's of all the logical channels
which are mapped onto a particular transport channel having a particular
priority */
extern      tdsmaci_ul_trch_priority_info_type     **tdsint_ul_trch_priority_info;

#define     TDSGET_UL_TRCH_PRIORITY_INFO_PTR(trch_idx, priority) (&tdsint_ul_trch_priority_info[trch_idx][priority - TDSMAC_UL_MIN_MLP_ID])

extern      tdsmaci_ul_trch_tf_type tdsint_ul_trch_tf_info[TDSUE_MAX_TRCH][TDSUE_MAX_TF];
#define   TDSGET_UL_TRCH_TF_INFO_PTR(trch_idx, tf) (&tdsint_ul_trch_tf_info[trch_idx][tf])


/* MACRO to check the UL Logical Channel Priority */
#define     TDSUL_LC_PRIORITY_IS_INVALID(priority)\
            ((priority < TDSMAC_UL_MIN_MLP_ID ) || (priority > TDSMAC_UL_MAX_MLP_ID ))

/* MACRO to check the UL ASC validity */
#define     TDSUL_ASC_IS_INVALID(asc)          (asc > TDSMAC_UL_MAX_ASC_ID)

/* MACRO to check UL RLC ID */
#define     TDSUL_RLC_ID_IS_INVALID(rlc_id)    (rlc_id > TDSMAC_UL_MAX_RLC_ID)

/* MACRO to check UL RB ID */
#define     TDSUL_RB_ID_IS_INVALID(rb_id)      (rb_id > TDSMAC_UL_MAX_RB_ID)

/* MACRO to check UL Transport Channel ID */
#define     TDSUL_TRCH_ID_IS_INVALID(tdstrch_id)\
            ((tdstrch_id < TDSMAC_UL_MIN_TRCH_ID) || (tdstrch_id > TDSMAC_UL_MAX_TRCH_ID))

/* MACRO to check RLC Mode */
#define     TDSUL_RLC_MODE_IS_INVALID(rlc_mode)\
            ((rlc_mode >= UE_MODE_MAX_NUM))

/* MACRO to check MAC ID */
#define     TDSUL_MAC_ID_IS_INVALID(mac_id)\
            ((mac_id < TDSMAC_MIN_MAC_ID ) || (mac_id > TDSMAC_MAX_MAC_ID))

/* Condition to check invalid number of DCCH/DTCHs */
#define     TDSUL_DCCH_DTCH_NUM_IS_INVALID(ndlchan)\
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
#define TDSAMR_MODE_475_CLASS_A_BIT_SIZE  42
#define TDSAMR_MODE_475_CLASS_B_BIT_SIZE  53

/* AMR 5.15 kbps speech */
#define TDSAMR_MODE_515_CLASS_A_BIT_SIZE  49
#define TDSAMR_MODE_515_CLASS_B_BIT_SIZE  54

/* AMR 5.90 kbps speech */
#define TDSAMR_MODE_590_CLASS_A_BIT_SIZE  55
#define TDSAMR_MODE_590_CLASS_B_BIT_SIZE  63

/* AMR 6.70 kbps speech */
#define TDSAMR_MODE_670_CLASS_A_BIT_SIZE  58
#define TDSAMR_MODE_670_CLASS_B_BIT_SIZE  76

/* AMR 7.40 kbps speech */
#define TDSAMR_MODE_740_CLASS_A_BIT_SIZE  61
#define TDSAMR_MODE_740_CLASS_B_BIT_SIZE  87

/* AMR 7.95 kbps speech */
#define TDSAMR_MODE_795_CLASS_A_BIT_SIZE  75
#define TDSAMR_MODE_795_CLASS_B_BIT_SIZE  84

/* AMR 10.2 kbps speech */
#define TDSAMR_MODE_102_CLASS_A_BIT_SIZE  65
#define TDSAMR_MODE_102_CLASS_B_BIT_SIZE  99
#define TDSAMR_MODE_102_CLASS_C_BIT_SIZE  40

/* AMR 12.2 kbps speech */
#define TDSAMR_MODE_122_CLASS_A_BIT_SIZE  81
#define TDSAMR_MODE_122_CLASS_B_BIT_SIZE 103
#define TDSAMR_MODE_122_CLASS_C_BIT_SIZE  60

/* AMR Mode 8 (SID) */
#define TDSAMR_MODE_8_CLASS_A_BIT_SIZE   39

#define TDSAMR_CLASS_NO_DATA_FRAME_SIZE 0

#define  TDSAMR_MODE_475_TOTAL_BITS  (TDSAMR_MODE_475_CLASS_A_BIT_SIZE + \
                                                                                 TDSAMR_MODE_475_CLASS_B_BIT_SIZE)
#define  TDSAMR_MODE_515_TOTAL_BITS (TDSAMR_MODE_515_CLASS_A_BIT_SIZE + \
                                                                          TDSAMR_MODE_515_CLASS_B_BIT_SIZE)
#define TDSAMR_MODE_590_TOTAL_BITS (TDSAMR_MODE_590_CLASS_A_BIT_SIZE + \
                                                                          TDSAMR_MODE_590_CLASS_B_BIT_SIZE)
#define TDSAMR_MODE_670_TOTAL_BITS (TDSAMR_MODE_670_CLASS_A_BIT_SIZE + \
                                                                          TDSAMR_MODE_670_CLASS_B_BIT_SIZE)
#define TDSAMR_MODE_740_TOTAL_BITS (TDSAMR_MODE_740_CLASS_A_BIT_SIZE + \
                                                                         TDSAMR_MODE_740_CLASS_B_BIT_SIZE)
#define TDSAMR_MODE_795_TOTAL_BITS (TDSAMR_MODE_795_CLASS_A_BIT_SIZE + \
                                                                         TDSAMR_MODE_795_CLASS_B_BIT_SIZE)
#define TDSAMR_MODE_102_TOTAL_BITS (TDSAMR_MODE_102_CLASS_A_BIT_SIZE + \
             TDSAMR_MODE_102_CLASS_B_BIT_SIZE + TDSAMR_MODE_102_CLASS_C_BIT_SIZE)
#define TDSAMR_MODE_122_TOTAL_BITS (TDSAMR_MODE_122_CLASS_A_BIT_SIZE + \
             TDSAMR_MODE_122_CLASS_B_BIT_SIZE + TDSAMR_MODE_122_CLASS_C_BIT_SIZE)

#ifdef FEATURE_VOC_AMR_WB

#define TDSAMR_WB_CLASS_A_BIT_SIZE  40

#define TDSAMR_WB_MODE_660_TOTAL_BITS (TDSMVS_AMR_WB_MODE_660_CLASS_A_BIT_SIZE + \
                                                                            TDSMVS_AMR_WB_MODE_660_CLASS_B_BIT_SIZE)
#define TDSAMR_WB_MODE_885_TOTAL_BITS (TDSMVS_AMR_WB_MODE_2385_CLASS_A_BIT_SIZE + \
                                                                            TDSMVS_AMR_WB_MODE_885_CLASS_B_BIT_SIZE)
#define TDSAMR_WB_MODE_1265_TOTAL_BITS (TDSMVS_AMR_WB_MODE_1265_CLASS_A_BIT_SIZE + \
                                                                           TDSMVS_AMR_WB_MODE_1265_CLASS_B_BIT_SIZE)
#define TDSAMR_WB_MODE_1425_TOTAL_BITS (TDSMVS_AMR_WB_MODE_1425_CLASS_A_BIT_SIZE+  \
                                                                           TDSMVS_AMR_WB_MODE_1425_CLASS_B_BIT_SIZE)
#define TDSAMR_WB_MODE_1585_TOTAL_BITS (TDSMVS_AMR_WB_MODE_1585_CLASS_A_BIT_SIZE + \
                                                                           TDSMVS_AMR_WB_MODE_1585_CLASS_B_BIT_SIZE)
#define TDSAMR_WB_MODE_1825_TOTAL_BITS (TDSMVS_AMR_WB_MODE_1825_CLASS_A_BIT_SIZE +  \
                                                                           TDSMVS_AMR_WB_MODE_1825_CLASS_B_BIT_SIZE)
#define TDSAMR_WB_MODE_1985_TOTAL_BITS (TDSMVS_AMR_WB_MODE_1985_CLASS_A_BIT_SIZE +  \
                                                                           TDSMVS_AMR_WB_MODE_1985_CLASS_B_BIT_SIZE)
#define TDSAMR_WB_MODE_2305_TOTAL_BITS (TDSMVS_AMR_WB_MODE_2305_CLASS_A_BIT_SIZE +  \
                                                                         TDSMVS_AMR_WB_MODE_2305_CLASS_B_BIT_SIZE)
#define TDSAMR_WB_MODE_2385_TOTAL_BITS (TDSMVS_AMR_WB_MODE_2385_CLASS_A_BIT_SIZE +  \
                                                                         TDSMVS_AMR_WB_MODE_2385_CLASS_B_BIT_SIZE)

/* WB-AMR 6.60 kbps bit size */
#define TDSMVS_AMR_WB_MODE_660_CLASS_A_BIT_SIZE    54
#define TDSMVS_AMR_WB_MODE_660_CLASS_B_BIT_SIZE    78

/* WB-AMR 8.85 kbps bit size */
#define TDSMVS_AMR_WB_MODE_885_CLASS_A_BIT_SIZE    64
#define TDSMVS_AMR_WB_MODE_885_CLASS_B_BIT_SIZE    113

/* WB-AMR 12.65 kbps bit size */
#define TDSMVS_AMR_WB_MODE_1265_CLASS_A_BIT_SIZE   72
#define TDSMVS_AMR_WB_MODE_1265_CLASS_B_BIT_SIZE   181

/* WB-AMR 14.25 kbps bit size */
#define TDSMVS_AMR_WB_MODE_1425_CLASS_A_BIT_SIZE   72
#define TDSMVS_AMR_WB_MODE_1425_CLASS_B_BIT_SIZE   213

/* WB-AMR 15.85 kbps bit size */
#define TDSMVS_AMR_WB_MODE_1585_CLASS_A_BIT_SIZE   72
#define TDSMVS_AMR_WB_MODE_1585_CLASS_B_BIT_SIZE   245

/* WB-AMR 18.25 kbps bit size */
#define TDSMVS_AMR_WB_MODE_1825_CLASS_A_BIT_SIZE   72
#define TDSMVS_AMR_WB_MODE_1825_CLASS_B_BIT_SIZE   293

/* WB-AMR 19.85 kbps bit size */
#define TDSMVS_AMR_WB_MODE_1985_CLASS_A_BIT_SIZE   72
#define TDSMVS_AMR_WB_MODE_1985_CLASS_B_BIT_SIZE   325

/* WB-AMR 23.05 kbps bit size */
#define TDSMVS_AMR_WB_MODE_2305_CLASS_A_BIT_SIZE   72
#define TDSMVS_AMR_WB_MODE_2305_CLASS_B_BIT_SIZE   389

/* WB-AMR 23.85 kbps bit size */
#define TDSMVS_AMR_WB_MODE_2385_CLASS_A_BIT_SIZE   72
#define TDSMVS_AMR_WB_MODE_2385_CLASS_B_BIT_SIZE   405

#endif /* FEATURE_VOC_AMR_WB */


/*=====================================================================

 FUNCTION NAME  tdsconfig_ul_mac

 PURPOSE        Processes UL MAC Configuration Data

 PARAMETERS     cfgptr - Pointer to the UL MAC Configuration data

 DESCRIPTION    This function performs the following functions:
                - saves configuration related information in local memory
                - if configured for RACH, indicates RLC to notify
                  when there is an SDU in its uplink queue
                - updates uplink logical channel information table
=====================================================================*/
extern  void    tdsconfig_ul_mac
(
    /* Pointer to UL MAC Configuration Data */
    tdsmac_ul_config_type *cfgptr
);
#ifdef FEATURE_MVS
/*====================================================================
FUNCTION: tdsmac_ul_compute_amr_rate()

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
tdsmac_ul_compute_amr_rate
(
  tdsmac_amr_mode_info_type *mac_amr_mode_info_ptr
);

/*====================================================================
FUNCTION: tdsmac_ul_compute_amr_rate()

DESCRIPTION:
  This function calls the mvs function to set the rate

DEPENDENCIES:


RETURN VALUE:



SIDE EFFECTS:


====================================================================*/
void
tdsmac_ul_set_amr_rate
(
  mvs_amr_mode_type amr_mode
);
#endif
/*===============================================================================================
FUNCTION NAME tdsmac_asc_selection

DESCRIPTION   Selects an Access Service Class

RETURNS     0 - 7 Depending on the selected ASC
===============================================================================================*/
uint8 tdsmac_asc_selection (tdsrlc_ul_frame_format_type *chan_fmt);

/*==============================================================================================
 FUNCTION NAME  void tdsmac_update_ciphering_params

 DESCRIPTION : 1. MAC maintains 2 configurations for ciphering. Current Ciphering Parameters (tdscur_ul_cipher_info)
 and New Ciphering Parameters (tdsnew_ul_cipher_info).

 2. If there is an activation time pending, then tdsnew_ul_cipher_info is used. The parameters
 are copied into tdscur_ul_cipher_info at activation time.

 3. tdscfn_hfn_table is a map of the CFN and the corresponding HFN. CFN serves as the
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
 if ( cfn < tdsprev_cfn). MAC maintains the last CFN value. This code is generic and would
 be able to handle even couple of build frame indications get missed for some reason.

 6. In TDSHO_STEP1_CIPHER_CFG : the ciphering_activation_pending flag would be set to FALSE.
 The HFN would not be incremented even if we reach activation time and there are roll overs.
 This is a temporary state when WCDMA has received a request for Handover from GSM and WCDMA
 side has not yet sent the confirm. The same is applicable for hard handovers.

 7. In TDSHO_STEP2_CIPHER_CFG : This configuration is received when WCDMA has sent a confirm.
 On receiving this configuration, MAC would increment the HFN by 1 when it hits activation
 time without waiting for the roll over. After this, the behaviour is as in the case of
 TDSNON_HO_CIPHER_CFG.

===============================================================================================*/

void tdsmac_update_ciphering_params
(
  uint8 cfn
);



/*-------------------------------------------------------------------
FUNCTION uint32 tdsmac_get_cnt_c_val()

DESCRIPTION
This function will be called by L1 to get teh tdscount_c value

DEPENDENCIES
  None

RETURN VALUE
  The tdscount_c value correspondign the current cfn will be returned to L1

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
extern uint32 tdsmac_get_cnt_c_val(uint8 cfn);



/*================================================================================================
 FUNCTION NAME  tdsinit_ul_mac_ciphering_params

 DESCRIPTION  This function initialises UL MAC ciphering parameters
================================================================================================*/
void  tdsinit_ul_mac_ciphering_params
(
  void
);


/*=================================================================================================
 FUNCTION NAME  tdsproc_ul_mac_tfcc_command

 PURPOSE    This function process UL TFCC config Commands
================================================================================================*/

void tdsproc_ul_mac_tfcc_command
(
  tdsmac_tfc_subset_restriction_info_type *tfcc_config_ptr
);

/*=================================================================================================
 FUNCTION NAME    tdsproc_ul_cctrch_info

 DESCRIPTION    This function processes uplink CCTrCh information
=================================================================================================*/
void    tdsproc_ul_cctrch_info
(
  /* Pointer to ordered config data of UL CCTrCh */
  tdsmac_ul_cctrch_config_type *cctrch_ptr
);

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

void  tdsmac_ul_free_mem_ZI_opt(void);

/*===============================================================================================
FUNCTION NAME tdsmac_ul_get_trch_type

DESCRIPTION   This function returns the up-link transport channel type corresponding to given RLC & RB IDs
===============================================================================================*/

log_tdscdma_trch_type tdsmac_ul_get_trch_type
  (
   /* RLC logical channel buffer identifier (0-20) */
   rlc_lc_id_type rlc_id, 

   /* Radio Bearer Identifier (0-31) */
   rb_id_type rb_id
  );


/*===============================================================================================
FUNCTION NAME tdsmac_ul_check_edch_trch

DESCRIPTION   This function checks if given rlc-id and rb-id pair map to a UL-EDCH transport channel
===============================================================================================*/

log_tdscdma_trch_type tdsmac_ul_check_edch_trch
(
   /* RLC logical channel buffer identifier (0-20) */
   rlc_lc_id_type rlc_id, 

   /* Radio Bearer Identifier (0-31) */
   rb_id_type rb_id
);
#endif


