/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC NULL MODE MODULE
***
***
*** DESCRIPTION
***
***  This module administrates the signal reception and the associated actions
***  for NULL mode states.
***
*** EXTERNALIZED FUNCTIONS
***
***  xxx
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  xxx
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacnull.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 09/11/10   sjw     Updated for Dual SIM support
*** 20/10/04   zyu     Added EGPRS logging
*** 20/11/01   sb      Created
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#include "grlcx.h"    /* required for test output usage */
#include "gmac.h"
#include "gmacdef.h"
#include "gmacnull.h"
#include "gmacutil.h"
#include "gmacmac.h"
#include "gmacrb.h"
#include "rr_public_store.h" /* required for tlli checking */
#include "gmaclog.h"
#include "gprs_odm_info.h"
#include "gmacproc.h"
#include "gmacds.h"
#include "stringl/stringl.h"

#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/
#define NULL_UL_SB_TYPE_PMR  0
#define NULL_UL_SB_TYPE_PCCF 1
#define NULL_UL_SB_TYPE_PRR  2

/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/
typedef struct
{
  uint32  starting_time;
  boolean polling_flag;
  uint8   tn;
} gmac_store_51_access_polling_t;

typedef struct
{
  gprs_mac_null_substates_t       mac_null_substate;
  mac_l1_sig_t                    gmac_null_ul_sb_store;
  mac_l1_sig_t                    gmac_null_dl_sb_store;
  uint8                           gmac_null_ul_sb_type;
  mac_l1_sig_t                    gmac_null_store_l1_dyn_realloc; /* uplink allocation confirm */

  /* used by NULL to indicate to IDLE that a PCA needs to be sent */
  gmac_store_51_access_polling_t  gmac_store_51access_polling;

  /*stores access type requested by rlc, needed when performing  */
  /*access retries during contention resolution failure          */
  uint8                           rlc_access_type;
  gmac_null_tbf_rel_t             gmac_null_tbf_rel_cause;

  /*used to save dl config rvcd while ul tbf is being est in state NULL_UPLINK_TBF_ACCESS*/
  gm_dl_tbf_est_t                 temp_gm_dl_tbf_est;

  /*stores RLC config data when GRR_MAC_UL_TBF_EST rcvd*/
  mr_ul_alloc_cnf_t               store_rlc_ul_alloc_data;

  null_flags_t                    n_flags;

#ifdef FEATURE_GSM_DTM
  dtm_tbf_type_t                  gmac_null_dtm_tbf_est_type;
  dtm_ass_status_t                gmac_null_dtm_ass_status;
  mr_ul_alloc_cnf_t               gmac_null_mr_ul_alloc_cnf_sig;
  mac_l1_dtm_channel_assign_req_t gmac_null_mac_l1_dtm_chan_ass_req;
#endif /*FEATURE_GSM_DTM*/

  mac_l1_freq_config_t            gmac_null_mac_l1_freq_config;
  mac_l1_dl_tbf_config_t          gmac_null_mac_l1_dl_config;
  mac_l1_dl_single_block_t        gmac_null_mac_l1_dl_sb_config;
  gm_dl_sb_est_t                  temp_gm_dl_sb_est;
  boolean                         gmac_null_releasing_l1;  /*Only used by NULL when L1 has been sent MAC_L1_RELEASE*/
} gmac_null_data_t;

/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/



/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/
static gmac_null_data_t gmac_null_data[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/
static void gmac_null_tbf_rel_confirm(gas_id_t gas_id);
static void gmac_util_send_grr_mac_rach_cnf(gas_id_t gas_id);

static void gmac_null_send_release_to_l1( gmac_null_tbf_rel_t null_rel_reason, release_tbf_t rel_type, gas_id_t gas_id);

static void gmac_null_send_ul_tbf_req(uint8 access_type, uint8 radio_priority, boolean ps_page_resp, gas_id_t gas_id);

static boolean gmac_null_handle_ul_sb_est_pmr(gas_id_t gas_id);
static boolean gmac_null_handle_ul_sb_est_pccf(gas_id_t gas_id);

static void gmac_null_send_sb_config(gas_id_t gas_id);

static boolean gmac_null_handle_ul_sb( frequency_information_T  *fc_msg_ptr,
                                       mac_sb_tbf_information_T *mac_l1_sb_ptr,
                                       gas_id_t gas_id );

static boolean gmac_null_ul_dyn_tbf(mr_ul_alloc_cnf_t *store_rlc_ul_alloc_data_ptr, gas_id_t gas_id);

static boolean gmac_null_ul_tbf_req(gas_id_t gas_id);

static void  gmac_util_send_mr_ul_alloc_cnf(mr_ul_alloc_cnf_t *store_rlc_ul_alloc_data_ptr, gas_id_t gas_id);

static boolean gmac_check_egprs_dl_ass(gm_dl_tbf_est_t *dl_assign_ptr, gas_id_t gas_id);

#ifdef FEATURE_GSM_DTM
static void  gmac_null_dtm_send_ul_tbf_req(
             uint8 access_type,
             rm_ul_est_tbf_req_t *ul_req_data_ptr,
             gas_id_t gas_id);

static void  gmac_null_rej_dtm_ass(dtm_ass_status_t dtm_ass_rej, gas_id_t gas_id);

#endif /* FEATURE_GSM_DTM */

static boolean gmac_null_dl_sb_est_handler(  gm_dl_sb_est_t             *dl_sb_assign_ptr,
                                             mac_l1_dl_single_block_t   *mac_l1_dl_sb_config_ptr,
                                             mac_l1_freq_config_t       *mac_l1_freq_config_ptr,
                                             gas_id_t                    gas_id);

static boolean gmac_null_dl_tbf_est_handler( gm_dl_tbf_est_t            *dl_assign_ptr,
                                             mac_l1_dl_tbf_config_t     *mac_l1_dl_config_ptr,
                                             mac_l1_freq_config_t       *mac_l1_freq_config_ptr,
                                             gas_id_t                    gas_id
                                           );

static void gmac_send_mac_l1_dl_sb_config(mac_l1_dl_single_block_t *mac_l1_dl_sb_config_ptr,
                                          gas_id_t                  gas_id);

static void gmac_null_check_for_polled_dl_ass(gas_id_t gas_id);

static uint8 gmac_null_determine_grr_access_type( grr_data_req_cause_t data_req_cause,
                                                  uint8                rlc_mode,
                                                  uint8                num_of_blocks,
                                                  uint8                access_phase,
                                                  gas_id_t             gas_id );

static boolean gmac_null_handle_ul_sb_est_prr(gas_id_t gas_id);

static gmac_null_data_t* gmac_get_null_data_ptr(gas_id_t gas_id);

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION    gmac_get_null_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to null data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_null_data_t*
=== 
===  SIDE EFFECTS
===
===
===========================================================================*/
static gmac_null_data_t* gmac_get_null_data_ptr(gas_id_t gas_id)
{
  gmac_null_data_t *null_data_ptr = &gmac_null_data[0];

  /* validate the GAS ID */
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    null_data_ptr = &gmac_null_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)];
  }
  else
  {    
    MSG_GERAN_ERROR_1_G("Null bad gas_id:%d",gas_id);
  }

  return null_data_ptr;
}

/*===========================================================================
===
===  FUNCTION    gmac_init_null_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) null data.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_null_data_non_zero( gas_id_t gas_id )
{
  gmac_null_data_t *null_data_ptr = gmac_get_null_data_ptr(gas_id);

  null_data_ptr->mac_null_substate = NULL_INACTIVE;
  null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_NO_RELEASE_IN_PROGRESS;

#ifdef FEATURE_GSM_DTM
  null_data_ptr->gmac_null_dtm_tbf_est_type = NO_TBF_PRESENT;
  null_data_ptr->gmac_null_dtm_ass_status = NO_DTM_ASS_ERROR;
#endif /*FEATURE_GSM_DTM*/
}

/*===========================================================================
===
===  FUNCTION    gmac_init_null_data()
===
===  DESCRIPTION
===  Initialises Null data.  Used for Lower layer resets.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_null_data( gas_id_t gas_id )
{
  gmac_null_data_t *null_data_ptr = gmac_get_null_data_ptr(gas_id);

  gmac_init_null_data_non_zero(gas_id);

  memset(&null_data_ptr->gmac_null_ul_sb_store,0,sizeof(mac_l1_sig_t));
  memset(&null_data_ptr->gmac_null_dl_sb_store,0,sizeof(mac_l1_sig_t));
  null_data_ptr->gmac_null_ul_sb_type = 0;
  memset(&null_data_ptr->gmac_null_store_l1_dyn_realloc,0,sizeof(mac_l1_sig_t));
  memset(&null_data_ptr->gmac_store_51access_polling,0,sizeof(gmac_store_51_access_polling_t));
  null_data_ptr->rlc_access_type = 0;
  memset(&null_data_ptr->temp_gm_dl_tbf_est,0,sizeof(gm_dl_tbf_est_t));
  memset(&null_data_ptr->store_rlc_ul_alloc_data,0,sizeof(mr_ul_alloc_cnf_t));
  memset(&null_data_ptr->n_flags,0,sizeof(null_flags_t));

#ifdef FEATURE_GSM_DTM
  memset(&null_data_ptr->gmac_null_mr_ul_alloc_cnf_sig,0,sizeof(mr_ul_alloc_cnf_t));
  memset(&null_data_ptr->gmac_null_mac_l1_dtm_chan_ass_req,0,sizeof(mac_l1_dtm_channel_assign_req_t));
#endif /*FEATURE_GSM_DTM*/

  memset(&null_data_ptr->gmac_null_mac_l1_freq_config,0,sizeof(mac_l1_freq_config_t));
  memset(&null_data_ptr->gmac_null_mac_l1_dl_config,0,sizeof(mac_l1_dl_tbf_config_t));
  memset(&null_data_ptr->gmac_null_mac_l1_dl_sb_config,0,sizeof(mac_l1_dl_single_block_t));
  memset(&null_data_ptr->temp_gm_dl_sb_est,0,sizeof(gm_dl_sb_est_t));
  null_data_ptr->gmac_null_releasing_l1 = FALSE;
}

/*===========================================================================
===
===  FUNCTION    gmac_set_mac_state_for_reset()
===
===  DESCRIPTION
===  When the lower layers panic reset then this function sets the MAC state.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_set_mac_state_for_reset( gas_id_t gas_id )
{
  gmac_null_data_t *null_data_ptr = gmac_get_null_data_ptr(gas_id);
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("GMAC State null NULL_RESET");
 
  null_data_ptr->mac_null_substate = NULL_RESET;
  global_data_ptr->mac_mode = NULL_MODE;
  null_data_ptr->n_flags.rlc_ul_reset_cnf_pending = TRUE;
  null_data_ptr->n_flags.rlc_dl_reset_cnf_pending = TRUE; 
}

/*===========================================================================
===
===  FUNCTION      gmac_null_tbf_rel_confirm
===
===  DESCRIPTION
===  When releasing TBF, wait for TBF release confirmation from L1 before
===  returning to NULL.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_null_tbf_rel_confirm(gas_id_t gas_id)
{
  gmac_null_data_t *null_data_ptr = gmac_get_null_data_ptr(gas_id);

  switch (null_data_ptr->gmac_null_tbf_rel_cause)
  {
    case GMAC_NULL_TBF_REL_NORMAL:
      /*With MAC initiated NO PS ACCESS RLC remains blocked till  */
      /*L1_MAC_TBF_REL_CONFIRM is rcvd then RLC will be unblocked */
      gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_NULL,__LINE__,gas_id);

      /*If MAC terminates PCCO must send mac_status before MAC_GRR_CELL_RESEL_STATUS*/
      gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

      /*complete any PCCO activity and PCCF activities*/
      gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,MAC_INITIATED,GMAC_NULL,__LINE__,gas_id);
      gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_NULL,__LINE__,gas_id);

      MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
      null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
      (void)mac_log_state(gas_id);

      break;

    case GMAC_NULL_TBF_REL_UL_EST_INT_BY_DL_SB_EST:

      /*Note IA for DL SB ALWAYS results in RLC being blocked until the *
      * DL SB has either been rcvd or the DL SB connection is terminated*/

      /*load stored IA for dl sb, config L1*/
      MSG_GERAN_HIGH_0_G("MAC info action IA for DL SB now L1 has release UL connection");
      if(gmac_null_dl_sb_est_handler(&null_data_ptr->temp_gm_dl_sb_est,
                                      &null_data_ptr->gmac_null_mac_l1_dl_sb_config,
                                      &null_data_ptr->gmac_null_mac_l1_freq_config,
                                       gas_id)  == TRUE)
      {
       /* wait for starting time to occur before configuring RLC dl tbf */
       gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
       gmac_send_mac_l1_dl_sb_config(&null_data_ptr->gmac_null_mac_l1_dl_sb_config,gas_id);

       /*complete any PCCF activities. The theory is that PCCF sending can not be interrupted*/
       /*by DL est as N/W does not know the current location of the MS. Added here becasue   */
       /*before IPA this was what was done                                                   */
       gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_NULL,__LINE__,gas_id);

       MSG_GERAN_MED_0_G("GMAC State null NULL_DOWNLINK_SB_ESTAB");
       null_data_ptr->mac_null_substate = NULL_DOWNLINK_SB_ESTAB;
      }
      else
      {
        /*Before proceeding with the DL SB config DL SB assignment was checked and     *
        *found to be OK. If there had been any error in the assignment then the UL tbf *
        *est would not have been aborted and hence we would not have got here be here  */
        if(geran_get_nv_recovery_restart_enabled(gas_id))
        {
          MSG_GERAN_ERROR_0_G("GMAC info stored DL SB config has been corrupted");
          MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
          geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GMAC info stored DL SB config has been corrupted");
        }
      }

      gmac_util_send_grr_mac_status(MAC_IDLE_BLOCKED,gas_id);
      break;

    case GMAC_NULL_TBF_REL_UL_EST_INT_BY_DL_EST:

      /*Note IA for DL ALWAYS results in RLC being blocked until the *
      * DL has either been rcvd or the DL SB connection is terminated*/

      /*load stored IA for dl, config L1*/
      MSG_GERAN_HIGH_0_G("MAC info action IA for DL now L1 has release UL connection");
      if(gmac_null_dl_tbf_est_handler(&null_data_ptr->temp_gm_dl_tbf_est,
                                      &null_data_ptr->gmac_null_mac_l1_dl_config,
                                      &null_data_ptr->gmac_null_mac_l1_freq_config,
                                       gas_id)  == TRUE)
      {
       /* wait for starting time to occur before configuring RLC dl tbf */
       gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
       gmac_send_mac_l1_dl_config(&null_data_ptr->gmac_null_mac_l1_dl_config,gas_id);
       gmac_null_check_for_polled_dl_ass(gas_id);

       /*complete any PCCF activities. The theory is that PCCF sending can not be interrupted*/
       /*by DL est as N/W does not know the current location of the MS. Added here becasue   */
       /*before IPA this was what was done                                                   */
       gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_NULL,__LINE__,gas_id);

       MSG_GERAN_MED_0_G("GMAC State null NULL_DL_TBF_EST");
       null_data_ptr->mac_null_substate = NULL_DL_TBF_EST;
      }
      else
      {
        /*Before proceeding with the DL config DL assignment was checked and found   *
        *to be OK. If there had been any error in the assignment then the UL tbf est *
        *would not have been aborted and hence we would not have got here be here    */
        if(geran_get_nv_recovery_restart_enabled(gas_id))
        {
          MSG_GERAN_ERROR_0_G("GMAC info stored DL SB config has been corrupted");
          MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
          geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GMAC info stored DL SB config has been corrupted");
        }
      }

      gmac_util_send_grr_mac_status(MAC_IDLE_BLOCKED,gas_id);
      break;

    case GMAC_NULL_GRR_INITIATED_NO_PS_ACCESS:

      /*With GRR initiated NO PS ACCESS RLC remains blocked till GRR send PS ACCESS */

      /*send mac status before attempting to complete PCCO process*/
      gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

      /*complete any PCCO activity and PCCF activities*/
      gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,GRR_INITIATED,GMAC_NULL,__LINE__,gas_id);
      gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,GRR_INITIATED,GMAC_NULL,__LINE__,gas_id);

      MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
      null_data_ptr->mac_null_substate = NULL_INACTIVE;
      (void)mac_log_state(gas_id);

      break;

    default:
      /* Note There is a possibility, if MAC sent to back to back  release  request  */
      /* from Transfer OR idle to L1, that L1 will get the 2nd release after it has  */
      /* transitioned to it's L1 idle state. NO action absorbed these releases.      */
      MSG_GERAN_ERROR_2_G("GMAC info No L1 release requested but L1_MAC_TBF_REL_CONFIRM rcvd. rel_cause %d, state %d",
                          null_data_ptr->gmac_null_tbf_rel_cause,null_data_ptr->gmac_null_tbf_rel_cause);

      break;
  }


  MSG_GERAN_MED_2_G("GMAC info Current values gmac_null_releasing_l1 %d gmac_null_tbf_rel_cause %d ",
                    null_data_ptr->gmac_null_releasing_l1,null_data_ptr->gmac_null_tbf_rel_cause);

  null_data_ptr->gmac_null_releasing_l1 = FALSE;
  null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_NO_RELEASE_IN_PROGRESS;

  gmac_reset_rac_params(gas_id);
}

/*===========================================================================
===
===  FUNCTION  gmac_util_send_grr_mac_rach_cnf
===
===  DESCRIPTION
===  Send MAC_GRR_RACH_CNF to GRR.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_util_send_grr_mac_rach_cnf(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Sent MAC_GRR_RACH_CNF");
  global_data_ptr->grr_sig.mac_grr_mac_status.message_header.message_set = MS_MAC_RR;
  global_data_ptr->grr_sig.mac_grr_mac_status.message_header.message_id = (uint8)MAC_GRR_RACH_CNF;
  gprs_grr_mac_acc_func(&global_data_ptr->grr_sig,gas_id);
}

/*===========================================================================
===
===  FUNCTION      gmac_null_send_release_to_l1
===
===  DESCRIPTION
===  Inform L1 of TBF release.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===  sets  gmac_null_tbf_rel_cause
===  sets  gmac_null_releasing_l1
===========================================================================*/
static void gmac_null_send_release_to_l1( gmac_null_tbf_rel_t null_rel_reason, release_tbf_t rel_type, gas_id_t gas_id)
{
  mac_l1_sig_t      mac_l1_sig;
  gmac_null_data_t *null_data_ptr = gmac_get_null_data_ptr(gas_id);


  /*event logging changes*/
  mac_log_packet_tbf_rel_event(rel_type,gas_id);

  MSG_GERAN_MED_3_G("GMAC info current gmac_null_tbf_rel_cause %d requested rel reason %d gmac_null_releasing_l1 %d ",
                    null_data_ptr->gmac_null_tbf_rel_cause,null_rel_reason,null_data_ptr->gmac_null_releasing_l1);

  null_data_ptr->gmac_null_tbf_rel_cause = null_rel_reason;

  null_data_ptr->gmac_null_releasing_l1 = TRUE;

  /* Send MAC_L1_RELEASE */
  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_RELEASE");
  mac_l1_sig.mac_l1_release.release_tbf = rel_type;
  mac_l1_sig.mac_l1_release.message_header.message_set = MS_MAC_L1;
  mac_l1_sig.mac_l1_release.message_header.message_id = (uint8)MAC_L1_RELEASE;
  gprs_l1_mac_acc_func(&mac_l1_sig,gas_id);

}

/*===========================================================================
===
===  FUNCTION    gmac_null_handle_ul_sb_est_prr()
===
===  DESCRIPTION
===  Store uplink TBF configuration parameters during uplink two-phase
===  packet access. This data will be sent to L1 after the frequency
===  parameters have been confirmed.
===
===  DEPENDENCIES
===  gmac_51_sb_params - storage for UL config data
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static boolean gmac_null_handle_ul_sb_est_prr(gas_id_t gas_id)
{
  boolean egprs_capable_cell = FALSE;
  boolean egprs_pkt_chan_req = FALSE;

  gmac_null_data_t          *null_data_ptr = gmac_get_null_data_ptr(gas_id);
  frequency_information_T   *fc_msg_ptr = &(null_data_ptr->gmac_null_mac_l1_freq_config.frequency_information);
  gmac_global_data_t        *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  mac_sb_tbf_information_T  *mac_l1_sb_ptr = &(global_data_ptr->gmac_51_sb_params.sb_tbf);

  /* set up generic sb msg fields and check frequency */
  if ( gmac_null_handle_ul_sb( fc_msg_ptr,
                               mac_l1_sb_ptr,
                               gas_id ) == FALSE )
  {
    return FALSE;
  }

  /* setup sb msgs fields specific to PRR */
  mac_l1_sb_ptr->two_phase_access = TRUE;

  rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL, &egprs_capable_cell, gas_id );
  rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_PKT_CHAN_REQ, &egprs_pkt_chan_req, gas_id );

  if(egprs_capable_cell == TRUE)
  {
    if(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_alloc == TRUE)
    {
      mac_l1_sb_ptr->bep_period2.present = FALSE;
      mac_l1_sb_ptr->bep_period2.bep_period2 = 0;

      /*only if a EGPRS rach is sent can a MB alloc be rcvd in the IA and hence */
      /*only under these conditions can there be any EGPRS params in the IA     */

      MSG_GERAN_MED_1_G("MAC egprs 2p access IA with MB alloc, blocks allocated %d",
                        (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_num_radio_blocks + 1));
      if(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_num_radio_blocks == 0)
      {
        global_data_ptr->egprs_2p_sb_mb_alloc = MB_ALLOC_OF_1_BLOCK;
        mac_l1_sb_ptr->num_rb_alloc = 1;
      }
      else if(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_num_radio_blocks == 1)
      {
        global_data_ptr->egprs_2p_sb_mb_alloc = MB_ALLOC_OF_2_BLOCKS;
        mac_l1_sb_ptr->num_rb_alloc = 2;
      }
      else
      {
        MSG_GERAN_HIGH_1_G("MAC Error in MB alloc for,IA ignored. Reserved blocks allocated %d",
                           global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_num_radio_blocks + 1);
        return FALSE;
      }

      /***************************   check access technology in IA      **********************/

      if ( global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.access_tech_type_count  > MAX_ACCESS_TECH_TYPE )
      {
        MSG_GERAN_ERROR_1_G("MAC ERR acc_tech_req out of range, count:%d",
                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.access_tech_type_count);
        return FALSE;
      }
      else
      {
        if ( 0 == global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.access_tech_type_count )
        {
          global_data_ptr->access_tech_req_present = FALSE;
        }
        else
        {
          memscpy(global_data_ptr->access_tech_req, sizeof(global_data_ptr->access_tech_req),
                 global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.access_tech_type,
                 (sizeof(global_data_ptr->access_tech_req[0])) * global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.access_tech_type_count );

          global_data_ptr->access_tech_req_count = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.access_tech_type_count;
          global_data_ptr->access_tech_req_present = TRUE;
        }
      }
    }
    else if ( (egprs_pkt_chan_req == FALSE) && (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_alloc == FALSE))
    {
      /*When a GPRS rach (with access type = 2p) is sent to establish an EGPRS ul. then only a SB alloc */
      /*can be rcvd in the IA and hence under these conditions there can be no EGPRS params in the IA.  */
      /*However MS must respond as if "access technology request" has been rcvd and the band of the     */
      /*BCCH bands access technology is reported                                                        */

      MSG_GERAN_MED_0_G("MAC egprs 2p access IA with SB alloc");
      if(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_alloc == FALSE)
      {
        mac_l1_sb_ptr->num_rb_alloc = 1;
        global_data_ptr->egprs_2p_sb_mb_alloc = EGPRS_SB_ALLOC;

        /*as there are no EGPRS params*/
        global_data_ptr->c_flags.egprs_rac_retran_req = FALSE;
        global_data_ptr->access_tech_req_present = FALSE;

      }
      else
      {
        /*error if did not send egprs rach then can only get SB alloc*/
        MSG_GERAN_HIGH_0_G("MAC Error egprs alloc indicated but SB alloc assigned in IA,ignored IA ???");
          return FALSE;
      }
    }
    else
    {
      MSG_GERAN_MED_2_G("MAC info ERROR in IA for SB/MB assigment egprs_pkt_chan_req %d egprs_alloc %d",
                        egprs_pkt_chan_req,global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_alloc);
      global_data_ptr->egprs_2p_sb_mb_alloc = GPRS_SB_ALLOC;
    }
  }
  else
  {
   /* GPRS 2p ass*/
   MSG_GERAN_MED_0_G("MAC gprs 2p access IA with SB alloc");
   global_data_ptr->egprs_2p_sb_mb_alloc = GPRS_SB_ALLOC;
  }

  /*Indicate to L1 what tbf mode to apply to the SB config*/
  if (global_data_ptr->egprs_2p_sb_mb_alloc == GPRS_SB_ALLOC)
  {
    mac_l1_sb_ptr->tbf_mode = TBF_MODE_GPRS;
  }
  else
  {
    mac_l1_sb_ptr->tbf_mode = TBF_MODE_EGPRS;
  }

  mac_l1_sb_ptr->starting_time = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.starting_time;

  /* NCO and NC_NON_DRX_PERIOD not applicable for two phase single block */

  return TRUE;
}

/*===========================================================================
===
===  FUNCTION    gmac_null_send_ul_tbf_req()
===
===  DESCRIPTION
===  Send MAC_GRR_UL_TBF_REQ to GRR to initiate an UL packet transfer.
===  If SHORT ACCESS is requested change request to ONE PHASE ACCESS
===  because CCCH procedures do not support SHORT ACCESS.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_null_send_ul_tbf_req ( uint8 access_type,
                                        uint8 radio_priority,
                                        boolean ps_page_resp,
                                        gas_id_t gas_id
                                       )

{
  mac_grr_sig_t grr_sig;

  MSG_GERAN_MED_0_G("MAC Sent MAC_GRR_UL_TBF_REQ");

  if ((access_type == GMAC_SHORT_ACCESS) || (access_type == GMAC_NO_PHASE_ACCESS))
  {
    access_type = GMAC_ONE_PHASE_ACCESS;
  }

  if ( ps_page_resp )
  {
    grr_sig.mac_grr_ul_tbf_req.channel_req_info.pkt_est_cause = RR_PKT_EST_CAUSE_PAGE_RESP;
  }
  else
  {
    grr_sig.mac_grr_ul_tbf_req.channel_req_info.pkt_est_cause = RR_PKT_EST_CAUSE_UNKNOWN;
  }

  grr_sig.mac_grr_ul_tbf_req.channel_req_info.access_type = access_type;
  grr_sig.mac_grr_ul_tbf_req.channel_req_info.radio_priority = radio_priority;
  grr_sig.mac_grr_ul_tbf_req.message_header.message_set = MS_MAC_RR;
  grr_sig.mac_grr_ul_tbf_req.message_header.message_id = (uint8)MAC_GRR_UL_TBF_REQ;
  gprs_grr_mac_acc_func(&grr_sig, gas_id);
}

/*===========================================================================
===
===  FUNCTION    gmac_null_handle_ul_sb_est_pmr()
===
===  DESCRIPTION
===  Handle request from GRR to send PMR. Initiate uplink single block
===  procedure by storing single block parameters and sending frequency
===  parameters to L1.
===
===  DEPENDENCIES
===  gmac_local_pmr_params - stored PMR
===  gmac_null_ul_sb_store - single block configuration for L1
===
===  RETURN VALUE
===
===    boolean - PMR pass/fail checks
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_null_handle_ul_sb_est_pmr(gas_id_t gas_id)
{
  gmac_null_data_t          *null_data_ptr = gmac_get_null_data_ptr(gas_id);
  frequency_information_T   *fc_msg_ptr = &(null_data_ptr->gmac_null_mac_l1_freq_config.frequency_information);
  mac_sb_tbf_information_T  *mac_l1_sb_ptr = &(null_data_ptr->gmac_null_ul_sb_store.mac_l1_single_block_config.sb_tbf);
  gmac_global_data_t        *global_data_ptr = gmac_get_global_data_ptr(gas_id);


  if(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_alloc == TRUE)
  {
     /*According to 44.018 Table 3.5.2.1.2.1: when MS request channel request(for sending PMR/PCCF)*/
     /*with establishment cause single block then network should not give immediate assignment with EGPRS allocation */

     MSG_GERAN_ERROR_0_G("GMAC Received 2-phase alloc for EGPRS, Invalid NW response to Chan Req (Single Block)");
     return FALSE;
  }

  /* set up generic sb msg fields and check frequency */
  if ( gmac_null_handle_ul_sb( fc_msg_ptr,
                               mac_l1_sb_ptr,
                               gas_id ) == FALSE )
  {
    return FALSE;
  }

  /* set up SB msg fields specific to PMR */
  mac_l1_sb_ptr->nco = global_data_ptr->gmac_local_pmr_params.nco;
  mac_l1_sb_ptr->nc_non_drx_period = global_data_ptr->gmac_local_pmr_params.nc_non_drx_period;
  mac_l1_sb_ptr->meas_report = TRUE;
  mac_l1_sb_ptr->two_phase_access = FALSE;
  mac_l1_sb_ptr->starting_time = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.starting_time;

  return TRUE;
}

/*===========================================================================
===
===  FUNCTION    gmac_null_handle_ul_sb_est_pccf()
===
===  DESCRIPTION
===  Handle request from GRR to send PCCF. Initiate uplink single block
===  procedure by storing single block parameters and sending frequency
===  parameters to L1.
===
===  DEPENDENCIES
===  gmac_local_pmr_params - stored PCCF
===  gmac_null_ul_sb_store - single block configuration for L1
===
===  RETURN VALUE
===    boolean - PCCF request pass/fail check.
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_null_handle_ul_sb_est_pccf(gas_id_t gas_id)
{
  gmac_null_data_t              *null_data_ptr = gmac_get_null_data_ptr(gas_id);
  frequency_information_T       *fc_msg_ptr = &(null_data_ptr->gmac_null_mac_l1_freq_config.frequency_information);
  mac_sb_tbf_information_T      *mac_l1_sb_ptr = &(null_data_ptr->gmac_null_ul_sb_store.mac_l1_single_block_config.sb_tbf);
  gmac_global_data_t            *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  if(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_alloc == TRUE)
  {
     /*According to 44.018 Table 3.5.2.1.2.1: when MS request channel request(for sending PMR/PCCF)*/
     /*with establishment cause single block then network should not give immediate assignment with EGPRS allocation */

     MSG_GERAN_ERROR_0_G("GMAC Received 2-phase alloc for EGPRS, Invalid NW response to Chan Req (Single Block)");
     return FALSE;
  }

  /* set up generic sb msg fields and check frequency */
  if ( gmac_null_handle_ul_sb( fc_msg_ptr,
                               mac_l1_sb_ptr,
                               gas_id ) == FALSE )
  {
    return FALSE;
  }

  /* set up PCCF specific SB msg fields */
  mac_l1_sb_ptr->starting_time = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.starting_time;
  mac_l1_sb_ptr->two_phase_access = FALSE;

  return TRUE;
}

/*===========================================================================
===
===  FUNCTION    gmac_null_handle_ul_sb()
===
===  DESCRIPTION
===  Generic handler for SB estbalishment of PRR,PMR and PCCF.
===  Populates the MAC L1 Freq Config and MAC L1 SB Config msgs.
===  It also checks that the frequency is within a valid band range.
===
===  DEPENDENCIES
===  writes to gmac_null_mac_l1_freq_config.reuse_sb_frq_alloc
===  reads msg_ptr->grr_mac_sig.msg.gm_ul_sb_est
===
===  RETURN VALUE
===    boolean - FALSE/TRUE - freq band range result
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_null_handle_ul_sb( frequency_information_T  *fc_msg_ptr,
                                       mac_sb_tbf_information_T *mac_l1_sb_ptr,
                                       gas_id_t gas_id )
{
  gmac_null_data_t *null_data_ptr = gmac_get_null_data_ptr(gas_id);
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  mac_l1_sb_ptr->meas_report = FALSE;
  mac_l1_sb_ptr->ts_number = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.tn;
  if (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.alpha_flag == valid)
  {
    mac_l1_sb_ptr->alpha_present = TRUE;
    mac_l1_sb_ptr->alpha = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.alpha;
  }
  else
  {
    mac_l1_sb_ptr->alpha_present = FALSE;
  }

  mac_l1_sb_ptr->gamma_tn = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.gamma;

  if (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.dl_pwr_ctrl_flag == valid)
  {
    mac_l1_sb_ptr->downlink_pwr.params_valid = TRUE;
    mac_l1_sb_ptr->downlink_pwr.po = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.dl_pwr_ctrl.po;
    mac_l1_sb_ptr->downlink_pwr.bts_pwr_ctrl_mode = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.dl_pwr_ctrl.bts_pwr_ctrl_mode;
    mac_l1_sb_ptr->downlink_pwr.pr_mode = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.dl_pwr_ctrl.pr_mode;
  }
  else
  {
    mac_l1_sb_ptr->downlink_pwr.params_valid = FALSE;
  }


  mac_l1_sb_ptr->timing_advance_params.ta_value_ie.present = TRUE;
  mac_l1_sb_ptr->timing_advance_params.ta_value_ie.ta_value = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.timing_advance;
  mac_l1_sb_ptr->timing_advance_params.cont_ta_ie.present = FALSE;

  if ( gmac_gprs_check_frequency_params(
              &(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.frequency_list),
                global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.hopping_flag,
                gas_id ) == FALSE )
  {
    MSG_GERAN_MED_0_G("GMAC INFO Freq Band Check Passed");
  }
  else
  {
    MSG_GERAN_MED_0_G("GMAC INFO Freq Band Check FAILED");
    return FALSE;
  }

  /*Populate global parameters*/
  global_data_ptr->gmac_store_freq_params_in_use.tsc = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.tsc;
  global_data_ptr->gmac_store_freq_params_in_use.hopping_flag = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.hopping_flag;
  global_data_ptr->gmac_store_freq_params_in_use.maio = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.maio;
  global_data_ptr->gmac_store_freq_params_in_use.hsn = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.hsn;
  global_data_ptr->gmac_store_freq_params_in_use.frequency_list = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.frequency_list;

  /* populate MAC_L1_FREQ_CONFIG with parameters */
  fc_msg_ptr->tsc = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.tsc;
  fc_msg_ptr->hopping_flag = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.hopping_flag;
  fc_msg_ptr->maio = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.maio;
  fc_msg_ptr->hsn = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.hsn;
  fc_msg_ptr->frequency_list = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.frequency_list;

  null_data_ptr->gmac_null_mac_l1_freq_config.reuse_sb_frq_alloc = FALSE;
#ifdef FEATURE_GSM_DTM
  null_data_ptr->gmac_null_mac_l1_freq_config.dtm_freq_param_present = FALSE;
#endif /*FEATURE_GSM_DTM*/
  null_data_ptr->gmac_null_mac_l1_freq_config.tbf_freq_cnf = UL_FREQ_CONFIG;

  return TRUE;
}

/*===========================================================================
===
===  FUNCTION    gmac_null_send_sb_config()
===
===  DESCRIPTION
===  MAC provide uplink single block data and parameters. First send
===  MAC_L1_DATA_REQ containing RLC/MAC block to be transmitted and
===  then send MAC_L1_SINGLE_BLOCK_CONFIG with single block configuration
===  parameters.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_null_send_sb_config(gas_id_t gas_id)
{
  gmac_null_data_t *null_data_ptr = gmac_get_null_data_ptr(gas_id);

  /* send MAC_L1_SINGLE_BLOCK_CONFIG with parameters */
  MSG_GERAN_MED_0_G("MAC Send MAC_L1_SINGLE_BLOCK_CONFIG");
  null_data_ptr->gmac_null_ul_sb_store.mac_l1_single_block_config.message_header.message_set = MS_MAC_L1;
  null_data_ptr->gmac_null_ul_sb_store.mac_l1_single_block_config.message_header.message_id = (uint8)MAC_L1_SINGLE_BLOCK_CONFIG;
  gprs_l1_mac_acc_func(&null_data_ptr->gmac_null_ul_sb_store,gas_id);
}

/*===========================================================================
===
===  FUNCTION    gmac_null_ul_dyn_tbf()
===
===  DESCRIPTION
===  Handle uplink dynamic assignment for one phase access from GRR.
===
===  DEPENDENCIES
===  store_rlc_ul_alloc_data_ptr
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static boolean gmac_null_ul_dyn_tbf(mr_ul_alloc_cnf_t *store_rlc_ul_alloc_data_ptr,
                                    gas_id_t gas_id)
{
  gmac_null_data_t              *null_data_ptr = gmac_get_null_data_ptr(gas_id);
  mac_dynamic_tbf_information_T *ul_tbf_msg_ptr = &(null_data_ptr->gmac_null_store_l1_dyn_realloc.mac_l1_dynamic_tbf_config.dynamic_tbf);
  gmac_global_data_t            *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gm_ul_tbf_est_t               *grr_msg_ptr = &(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est);

  uint8                         loop;

  #define  BEP_PERIOD2_NOT_SPECIFIED 0xFF
  uint16   log_BEP_prd2;
  boolean  egprs_tbf_valid = TRUE;

  /* starting time */
  null_data_ptr->gmac_store_51access_polling.polling_flag = FALSE;
  if(grr_msg_ptr->tbf_starting_time_flag == valid)
  {
    ul_tbf_msg_ptr->tbf_start_time_valid = TRUE;
    ul_tbf_msg_ptr->starting_time = grr_msg_ptr->tbf_starting_time;

    /* If polling bit set send PCA on RRBP using starting time */
    /* Store ST and TN for RRBP and wait until freq params are confirmed before sending */
    if (grr_msg_ptr->polling_bit)
    {
      gmac_rrbp_send_pca(grr_msg_ptr->tbf_starting_time, grr_msg_ptr->tn, FALSE, 0, PCA_51_MSG, gas_id);
    }
  }
  else
  {
    ul_tbf_msg_ptr->tbf_start_time_valid = FALSE;
  }

  /* continuous timing advance */
  if (grr_msg_ptr->timing_advance_index_flag == valid)
  {
    ul_tbf_msg_ptr->timing_advance_params.cont_ta_ie.present = TRUE;
    ul_tbf_msg_ptr->timing_advance_params.cont_ta_ie.tai = grr_msg_ptr->timing_advance_index;
    ul_tbf_msg_ptr->timing_advance_params.cont_ta_ie.ta_timeslot = grr_msg_ptr->tn;
  }
  else
  {
    ul_tbf_msg_ptr->timing_advance_params.cont_ta_ie.present = FALSE;
  }

  ul_tbf_msg_ptr->timing_advance_params.ta_value_ie.present = TRUE;
  ul_tbf_msg_ptr->timing_advance_params.ta_value_ie.ta_value = grr_msg_ptr->timing_advance;


  /* downlink control power */
  if (grr_msg_ptr->alloc.dynamic_alloc.po_pr_mode_flag == valid)
  {
    ul_tbf_msg_ptr->dl_ctrl_param.params_valid = TRUE;
    ul_tbf_msg_ptr->dl_ctrl_param.po = grr_msg_ptr->alloc.dynamic_alloc.po;
    ul_tbf_msg_ptr->dl_ctrl_param.pr_mode = grr_msg_ptr->alloc.dynamic_alloc.pr_mode;
  }
  else
  {
    ul_tbf_msg_ptr->dl_ctrl_param.params_valid = FALSE;
  }

  /* Dynamic allocation USF */
  ul_tbf_msg_ptr->usf_granularity = grr_msg_ptr->alloc.dynamic_alloc.usf_granularity;
  for (loop=0; loop<8; loop++)
  {
    ul_tbf_msg_ptr->usf[loop].present = FALSE;
  }
  ul_tbf_msg_ptr->usf[grr_msg_ptr->tn].present = TRUE;
  ul_tbf_msg_ptr->usf[grr_msg_ptr->tn].usf_value = grr_msg_ptr->alloc.dynamic_alloc.usf;

  /* set globals to be used during transfer */
  global_data_ptr->gmac_local_ul_ts_alloc = 0x80 >> (grr_msg_ptr->tn);
  global_data_ptr->gmac_local_ul_dl_ctrl_ts = global_data_ptr->gmac_local_ul_ts_alloc;

  ul_tbf_msg_ptr->power_control_params.present = TRUE;

  /* power control - alpha */
  if (grr_msg_ptr->alpha > PWR_CTRL_ALPHA_MAX_VALUE)
  {
    ul_tbf_msg_ptr->power_control_params.alpha = PWR_CTRL_ALPHA_MAX_VALUE;
  }
  else
  {
    ul_tbf_msg_ptr->power_control_params.alpha = grr_msg_ptr->alpha;
  }

  /* power control - gamma */
  ul_tbf_msg_ptr->power_control_params.gamma_TN0.present = FALSE;
  ul_tbf_msg_ptr->power_control_params.gamma_TN1.present = FALSE;
  ul_tbf_msg_ptr->power_control_params.gamma_TN2.present = FALSE;
  ul_tbf_msg_ptr->power_control_params.gamma_TN3.present = FALSE;
  ul_tbf_msg_ptr->power_control_params.gamma_TN4.present = FALSE;
  ul_tbf_msg_ptr->power_control_params.gamma_TN5.present = FALSE;
  ul_tbf_msg_ptr->power_control_params.gamma_TN6.present = FALSE;
  ul_tbf_msg_ptr->power_control_params.gamma_TN7.present = FALSE;

  if (grr_msg_ptr->tn == 0)
  {
    ul_tbf_msg_ptr->power_control_params.gamma_TN0.present = TRUE;
    ul_tbf_msg_ptr->power_control_params.gamma_TN0.gamma = grr_msg_ptr->gamma;
  }
  else if (grr_msg_ptr->tn == 1)
  {
    ul_tbf_msg_ptr->power_control_params.gamma_TN1.present = TRUE;
    ul_tbf_msg_ptr->power_control_params.gamma_TN1.gamma = grr_msg_ptr->gamma;
  }
  else if (grr_msg_ptr->tn == 2)
  {
    ul_tbf_msg_ptr->power_control_params.gamma_TN2.present = TRUE;
    ul_tbf_msg_ptr->power_control_params.gamma_TN2.gamma = grr_msg_ptr->gamma;
  }
  else if (grr_msg_ptr->tn == 3)
  {
    ul_tbf_msg_ptr->power_control_params.gamma_TN3.present = TRUE;
    ul_tbf_msg_ptr->power_control_params.gamma_TN3.gamma = grr_msg_ptr->gamma;
  }
  else if (grr_msg_ptr->tn == 4)
  {
    ul_tbf_msg_ptr->power_control_params.gamma_TN4.present = TRUE;
    ul_tbf_msg_ptr->power_control_params.gamma_TN4.gamma = grr_msg_ptr->gamma;
  }
  else if (grr_msg_ptr->tn == 5)
  {
    ul_tbf_msg_ptr->power_control_params.gamma_TN5.present = TRUE;
    ul_tbf_msg_ptr->power_control_params.gamma_TN5.gamma = grr_msg_ptr->gamma;
  }
  else if (grr_msg_ptr->tn == 6)
  {
    ul_tbf_msg_ptr->power_control_params.gamma_TN6.present = TRUE;
    ul_tbf_msg_ptr->power_control_params.gamma_TN6.gamma = grr_msg_ptr->gamma;
  }
  else if (grr_msg_ptr->tn == 7)
  {
    ul_tbf_msg_ptr->power_control_params.gamma_TN7.present = TRUE;
    ul_tbf_msg_ptr->power_control_params.gamma_TN7.gamma = grr_msg_ptr->gamma;
  }

  /*set up EGPRS fields for the MAC_L1_DYNAMIC_TBF_CONFIG msg*/
  if ( grr_msg_ptr->egprs_ul_tbf.egprs_alloc )
  {
    ul_tbf_msg_ptr->tbf_mode = TBF_MODE_EGPRS;

    if ( grr_msg_ptr->egprs_ul_tbf.bep_period2_present )
    {
      if ( grr_msg_ptr->egprs_ul_tbf.bep_period2 > 15 )
      {
        egprs_tbf_valid = FALSE;
        MSG_GERAN_ERROR_1_G("MAC ERR EGPRS bep_period2 out of range, BP2:0x%x",grr_msg_ptr->egprs_ul_tbf.bep_period2);
      }
      else
      {
        ul_tbf_msg_ptr->bep_period2.present = TRUE;
        ul_tbf_msg_ptr->bep_period2.bep_period2 = grr_msg_ptr->egprs_ul_tbf.bep_period2;
      }
    }
    else
    {
      ul_tbf_msg_ptr->bep_period2.present = FALSE;
      ul_tbf_msg_ptr->bep_period2.bep_period2 = 0;
    }

    if ( global_data_ptr->gmac_local_stored_ul_chan_req.rlc_mode == GRLC_MODE_ACK )
    {
      ul_tbf_msg_ptr->rlc_mode =  RLC_ACK;
    }
    else
    {
      ul_tbf_msg_ptr->rlc_mode =  RLC_UNACK;
    }

    ul_tbf_msg_ptr->test_mode = TEST_MODE_OFF; /* hard coded for rel 0.1 */
  }
  else
  {
    ul_tbf_msg_ptr->tbf_mode = TBF_MODE_GPRS;

    /* set arbitrary values when not doing EGPRS */
    ul_tbf_msg_ptr->bep_period2.present = FALSE;
    ul_tbf_msg_ptr->bep_period2.bep_period2 = 0;
    ul_tbf_msg_ptr->rlc_mode =  RLC_UNACK;
    ul_tbf_msg_ptr->test_mode = TEST_MODE_OFF;
  }

  /* Store global version of TFI */
  global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = TRUE;
  global_data_ptr->gmac_local_tfi_store.ul_tfi = grr_msg_ptr->tfi;

  /*save RLC data rcvd in GRR_MAC_UL_TBF_EST*/
  memset(store_rlc_ul_alloc_data_ptr, 0, sizeof(mr_ul_alloc_cnf_t));

  store_rlc_ul_alloc_data_ptr->access_mode = RM_DYNAMIC_ALLOC;
  store_rlc_ul_alloc_data_ptr->cs = grr_msg_ptr->ch_block_coding_cmd;
  store_rlc_ul_alloc_data_ptr->ul_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
  store_rlc_ul_alloc_data_ptr->num_timeslots = 1;
  store_rlc_ul_alloc_data_ptr->usf_granularity = grr_msg_ptr->alloc.dynamic_alloc.usf_granularity;

  /* NOTE : blocks_granted is not present for CCCH so treat as not present */
  store_rlc_ul_alloc_data_ptr->tbf_type = RM_OPEN_ENDED_TBF;
  store_rlc_ul_alloc_data_ptr->blocks_granted = 0;

  store_rlc_ul_alloc_data_ptr->access_phase = GMAC_ONE_PHASE_ACCESS;
  store_rlc_ul_alloc_data_ptr->tlli_block_cs = grr_msg_ptr->tlli_block_ch_coding;

  store_rlc_ul_alloc_data_ptr->ti = RM_TLLI_PRESENT;
  store_rlc_ul_alloc_data_ptr->retry = grr_msg_ptr->retry_bit;
  store_rlc_ul_alloc_data_ptr->tlli = grr_msg_ptr->tlli;

  /* NOTE : MR_UL_ALLOC_CNF final_alloc is for fixed only */
  store_rlc_ul_alloc_data_ptr->final_alloc = 0;

  if ( grr_msg_ptr->egprs_ul_tbf.egprs_alloc )
  {
    store_rlc_ul_alloc_data_ptr->tbf_mode = TBF_MODE_EGPRS;

    if ( gmac_util_egprs_mcs_valid(grr_msg_ptr->egprs_ul_tbf.egprs_channel_coding_command, gas_id) == FALSE )
    {
      egprs_tbf_valid = FALSE;
      MSG_GERAN_ERROR_1_G("MAC ERR EGPRS chan code invalid ignore assignment, mcs:0x%x",
                          grr_msg_ptr->egprs_ul_tbf.egprs_channel_coding_command);
    }
    else
    {
      store_rlc_ul_alloc_data_ptr->mcs = grr_msg_ptr->egprs_ul_tbf.egprs_channel_coding_command;
    }

    store_rlc_ul_alloc_data_ptr->resegment = grr_msg_ptr->egprs_ul_tbf.resegment;

    /* convert 5-bit encoded WS (04.60 12.5.2) into an absolute value */
    store_rlc_ul_alloc_data_ptr->ws = ((grr_msg_ptr->egprs_ul_tbf.egprs_window_size * 32) + 64);

    /*cr134062*/
    /*check EGPRS UL WS assigned does not exceed MAX allowed for the MULTISLOCT class of UE*/
    if(gmac_check_assigned_egprs_ws(global_data_ptr->gmac_egprs_ms_class_max_ts_tx,store_rlc_ul_alloc_data_ptr->ws, gas_id) == FAIL_RESULT)
    {
      egprs_tbf_valid = FALSE;
      MSG_GERAN_ERROR_1_G("MAC ERR EGPRS WS out of range, WS:%d, TS:1",store_rlc_ul_alloc_data_ptr->ws);
    }
  }
  else
  {
    store_rlc_ul_alloc_data_ptr->tbf_mode = TBF_MODE_GPRS;
  }

#ifdef FEATURE_GSM_DTM
  global_data_ptr->gmac_dtm_cs_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
#endif /*FEATURE_GSM_DTM*/

  /*Featured ul_tbf_est logging. Consider both feature and both option in EDGE*/
  if ( grr_msg_ptr->egprs_ul_tbf.bep_period2_present )
    log_BEP_prd2=ul_tbf_msg_ptr->bep_period2.bep_period2;
  else
    log_BEP_prd2=BEP_PERIOD2_NOT_SPECIFIED;

  if (egprs_tbf_valid)
  {
    mac_log_ul_etbf_establish(
      grr_msg_ptr->tfi,
      global_data_ptr->gmac_local_ul_ts_alloc,
      (byte)egprs_tbf_valid,
      store_rlc_ul_alloc_data_ptr->ws,
      store_rlc_ul_alloc_data_ptr->resegment ,
      (uint8)log_BEP_prd2,
       gas_id);

  }
  else
  {
    mac_log_ul_tbf_establish(
      grr_msg_ptr->tfi,
      global_data_ptr->gmac_local_ul_ts_alloc,
      gas_id);
  }

  return egprs_tbf_valid;
}

/*===========================================================================
===
===  FUNCTION    gmac_null_ul_tbf_req
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  global var store_rlc_ul_alloc_data, used to store rcvd UL assignment data
===  destined for RLC.
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/

static boolean gmac_null_ul_tbf_req(gas_id_t gas_id)
{
  gmac_null_data_t              *null_data_ptr = gmac_get_null_data_ptr(gas_id);
  frequency_information_T       *freq_config_msg_ptr = &(null_data_ptr->gmac_null_mac_l1_freq_config.frequency_information);
  gmac_global_data_t            *global_data_ptr = gmac_get_global_data_ptr(gas_id);


    if (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.alloc_flag == dynamic)
    {

      if ( !gmac_null_ul_dyn_tbf(&null_data_ptr->store_rlc_ul_alloc_data, gas_id) )
      {
        return FALSE;
      }
      global_data_ptr->tbf_type = (uint8)dynamic_alloc_e;

      /* store current mac mode for multislot checking */
      global_data_ptr->gmac_current_mac_mode = MAC_L1_MAC_MODE_DYNAMIC;
    }
    else
    {
      /* FIXED ALLOCATION no supported in this build. */
      MSG_GERAN_HIGH_0_G("MAC error UL TBF type not supported");

      return FALSE;
    }

    /* Store global freq params for use in transfer */
    global_data_ptr->gmac_store_freq_params_in_use.tsc = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.tsc;
    global_data_ptr->gmac_store_freq_params_in_use.hopping_flag = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.hopping_flag;
    global_data_ptr->gmac_store_freq_params_in_use.maio = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.maio;
    global_data_ptr->gmac_store_freq_params_in_use.hsn = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.hsn;
    global_data_ptr->gmac_store_freq_params_in_use.frequency_list = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.frequency_list;

    /* store EGPRS global parameters */
    if ( global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.egprs_ul_tbf.egprs_alloc )
    {
      global_data_ptr->current_tbf_mode = TBF_MODE_EGPRS;

      global_data_ptr->current_egprs_ul_ws = ((global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.egprs_ul_tbf.egprs_window_size * 32) + 64);

      if ( global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.egprs_ul_tbf.access_tech_type_count  > MAX_ACCESS_TECH_TYPE )
      {
        MSG_GERAN_ERROR_1_G("MAC ERR acc_tech_req out of range, count:%d",
                            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.egprs_ul_tbf.access_tech_type_count);
        return FALSE;
      }
      else
      {
        if ( 0 == global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.egprs_ul_tbf.access_tech_type_count )
        {
          global_data_ptr->access_tech_req_present = FALSE;
        }
        else
        {
          memscpy(global_data_ptr->access_tech_req, sizeof(global_data_ptr->access_tech_req), 
                  global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.egprs_ul_tbf.access_tech_type,
                  (sizeof(global_data_ptr->access_tech_req[0])) * global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.egprs_ul_tbf.access_tech_type_count );

          global_data_ptr->access_tech_req_count = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.egprs_ul_tbf.access_tech_type_count;
          global_data_ptr->access_tech_req_present = TRUE;
        }
      }
    }
    else
    {
      global_data_ptr->current_tbf_mode = TBF_MODE_GPRS;
    }

    if ( gmac_gprs_check_frequency_params(
              &(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.frequency_list),
                global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.hopping_flag,
                gas_id ) == FALSE )
    {
      MSG_GERAN_MED_0_G("GMAC INFO Freq Band Check Passed");
    }
    else
    {
      MSG_GERAN_MED_0_G("GMAC INFO Freq Band Check FAILED");
      return FALSE;
    }

    /* Populate MAC_L1_FREQ_CONFIG with parameters */
    freq_config_msg_ptr->tsc = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.tsc;
    freq_config_msg_ptr->hopping_flag = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.hopping_flag;
    freq_config_msg_ptr->maio = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.maio;
    freq_config_msg_ptr->hsn = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.hsn;
    freq_config_msg_ptr->frequency_list = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.frequency_list;

    null_data_ptr->gmac_null_mac_l1_freq_config.reuse_sb_frq_alloc = FALSE;

    return TRUE;
}

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION    gmac_init_null_flags()
===
===  DESCRIPTION
===  Initialises Null flags, should be called once at start up.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_null_flags( gas_id_t gas_id )
{
 gmac_null_data_t *null_data_ptr = gmac_get_null_data_ptr(gas_id);

 null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 = FALSE;
 null_data_ptr->n_flags.gmac_pcco_T3174_exp_flag = FALSE;
 null_data_ptr->n_flags.ps_page_response = FALSE;

 null_data_ptr->n_flags.rlc_ul_reset_cnf_pending = FALSE;
 null_data_ptr->n_flags.rlc_dl_reset_cnf_pending = FALSE;
}



/*===========================================================================
===
===  FUNCTION  gmac_util_send_mr_ul_alloc_cnf
===
===  DESCRIPTION
===  Send MR_UL_ALLOC_CNF.
===
===  DEPENDENCIES
===  store_rlc_ul_alloc_data, RLC UL assigment rcvd in GRR_MAC_UL_TBF_EST
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===========================================================================*/
static void gmac_util_send_mr_ul_alloc_cnf(mr_ul_alloc_cnf_t *store_rlc_ul_alloc_data_ptr,
                                           gas_id_t gas_id)
{
  mac_rlc_msg_t                 *mr_alloc_cnf_ptr;

  mr_alloc_cnf_ptr = grlc_get_cmd_buf_mac();
  if (mr_alloc_cnf_ptr != NULL)
  {
    memset(mr_alloc_cnf_ptr, 0, sizeof(mac_rlc_msg_t));

    mr_alloc_cnf_ptr->msg.ul_alloc_cnf = *store_rlc_ul_alloc_data_ptr;

    MSG_GERAN_MED_0_G("MAC Send MR_UL_ALLOC_CNF");
    mr_alloc_cnf_ptr->msg.ul_alloc_cnf.sig = MR_UL_ALLOC_CNF;
    gmac_rlc_mac_insert_gas_id(mr_alloc_cnf_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mr_alloc_cnf_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }

}
/*===========================================================================
===
===  FUNCTION  check_egprs_dl_ass
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===========================================================================*/
static boolean gmac_check_egprs_dl_ass(gm_dl_tbf_est_t *dl_assign_ptr,
                                       gas_id_t gas_id)
{
  uint16  temp_ws = 0;
  boolean egprs_tbf_valid = TRUE;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  if ( TRUE == dl_assign_ptr->egprs_alloc )
  {
    temp_ws = ((dl_assign_ptr->egprs_dl_ws * 32) + 64);

    /*cr134062*/
    /*check EGPRS DL WS assigned does not exceed MAX allowed for the MULTISLOCT class of UE*/
    if(gmac_check_assigned_egprs_ws(global_data_ptr->gmac_egprs_ms_class_max_ts_rx,temp_ws,
                                    gas_id) == FAIL_RESULT)
    {
      egprs_tbf_valid = FALSE;
      MSG_GERAN_ERROR_1_G("MAC ERR EGPRS WS out of range, WS:%d, TS:1",temp_ws);
    }

    /* check the link quality measurement mode field is in range 0-3*/
    if ( dl_assign_ptr->egprs_lq_meas_mode > 3 )
    {
      egprs_tbf_valid = FALSE;
      MSG_GERAN_ERROR_1_G("MAC ERR EGPRS Link Qual Meas Mode out of range :%d",dl_assign_ptr->egprs_lq_meas_mode);
    }

    if ( dl_assign_ptr->egprs_bep_period2_present == TRUE )
    {
      if ( dl_assign_ptr->egprs_bep_period2 > 15 )
      {
         egprs_tbf_valid = FALSE;
         MSG_GERAN_ERROR_1_G("MAC ERR EGPRS BEP Period2 out of range :%d",dl_assign_ptr->egprs_bep_period2);
      }
    }
  }

  return egprs_tbf_valid;
}


#ifdef FEATURE_GSM_DTM

/*===========================================================================
===
===  FUNCTION    gmac_null_dtm_send_ul_tbf_req()
===
===  DESCRIPTION
===  Send MAC_GRR_UL_TBF_REQ to GRR to initiate an UL packet transfer.
===
===  NOTE When GRR request an UL est via  via the DCCH, the DTM request
===       contains channel request description. This is not the case
===       for normal GPRS operation
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/

static void gmac_null_dtm_send_ul_tbf_req
(
 uint8 access_type,
 rm_ul_est_tbf_req_t *ul_req_data_ptr,
 gas_id_t gas_id
)

{
  mac_grr_sig_t grr_sig;

  channel_req_desc_t    *rlc_crd_ptr;
  rr_channel_req_info_t *grr_chan_req_info_ptr;

  grr_chan_req_info_ptr = &(grr_sig.mac_grr_ul_tbf_req.channel_req_info);
  rlc_crd_ptr = &(ul_req_data_ptr->chan_req_desc);

  MSG_GERAN_MED_0_G("MAC Sent MAC_GRR_UL_TBF_REQ");

  if ((access_type == GMAC_SHORT_ACCESS) || (access_type == GMAC_NO_PHASE_ACCESS))
  {
    access_type = GMAC_ONE_PHASE_ACCESS;
  }

  /********************************************************************************
  * "Access_type" is only relevent for GPRS/EGPRS access via RAChing. For DTM the *
  * DTM request msg sent on DCCH uses PACKET_ESTABLISHMENT_CAUSE                  *
  ********************************************************************************/
  grr_chan_req_info_ptr->access_type = access_type;
  grr_chan_req_info_ptr->radio_priority = rlc_crd_ptr->radio_priority;


  /*PACKET_ESTABLISHMENT_CAUSE is mapped from GRR_DATA_REQ Cause indicators ref LLC*/
  /*10.5.2.8b.2: 44.018 Channel Request Description 2                              */

  switch(ul_req_data_ptr->data_req_cause)
  {
   case GRR_CAUSE_GMM_PAGE_RESPONSE:
     grr_chan_req_info_ptr->pkt_est_cause = RR_PKT_EST_CAUSE_PAGE_RESP;
     break;

   case GRR_CAUSE_GMM_CELL_UPDATE:
   case GRR_CAUSE_GMM_CELL_NOTIFY:
     grr_chan_req_info_ptr->pkt_est_cause = RR_PKT_EST_CAUSE_CELL_UPD;
     break;

   case GRR_CAUSE_GMM_SIGNALLING :
     grr_chan_req_info_ptr->pkt_est_cause = RR_PKT_EST_CAUSE_MM;
     break;

   case GRR_CAUSE_LAY3_DATA :
   case GRR_CAUSE_LLC_SIGNALLING :
     grr_chan_req_info_ptr->pkt_est_cause = RR_PKT_EST_CAUSE_USER_DATA;
     break;

   default:
     grr_chan_req_info_ptr->pkt_est_cause = RR_PKT_EST_CAUSE_UNKNOWN;
     break;
  }

  grr_chan_req_info_ptr->peak_thr_class = rlc_crd_ptr->peak_thr_class;
  grr_chan_req_info_ptr->rlc_mode = rlc_crd_ptr->rlc_mode;
  grr_chan_req_info_ptr->llc_pdu_type = rlc_crd_ptr->llc_pdu_type;
  grr_chan_req_info_ptr->rlc_octet_cnt = rlc_crd_ptr->rlc_octet_cnt;

  grr_chan_req_info_ptr->pfi_present = ul_req_data_ptr->pfi_present;
  grr_chan_req_info_ptr->pfi = ul_req_data_ptr->pfi;

  if((ul_req_data_ptr->data_req_cause == GRR_CAUSE_GMM_SIGNALLING) ||
     (ul_req_data_ptr->data_req_cause == GRR_CAUSE_GMM_CELL_NOTIFY) ||
     (ul_req_data_ptr->data_req_cause == GRR_CAUSE_GMM_CELL_UPDATE) ||
     (ul_req_data_ptr->data_req_cause == GRR_CAUSE_GMM_PAGE_RESPONSE))
  {
    grr_sig.mac_grr_ul_tbf_req.gttp_gmm_sig_octets = ul_req_data_ptr->pdu_octets;
  }
  else
  {
    grr_sig.mac_grr_ul_tbf_req.gttp_gmm_sig_octets = 0;
  }

  grr_sig.mac_grr_ul_tbf_req.message_header.message_set = MS_MAC_RR;
  grr_sig.mac_grr_ul_tbf_req.message_header.message_id = (uint8)MAC_GRR_UL_TBF_REQ;

  gprs_grr_mac_acc_func(&grr_sig, gas_id);

}


/*===========================================================================
===
===  FUNCTION    gmac_null_rej_dtm_ass()
===
===  DESCRIPTION
===
===    When MAC detects an error in the DTM ass rcvd form GRR, MAC will
===      1) inform GRR of the error  indicating the reason for failure
===      2) Always block RLC
===      3) will switch to NULL_INACTIVE state and wait for GRR to send
===         GRR_MAC_PS_ACCESS_REQ
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/

static void  gmac_null_rej_dtm_ass (dtm_ass_status_t dtm_ass_rej,
                                    gas_id_t gas_id)
{
    /*block RLC ul irrespective of whether UL was being est*/
    /*we now wait for PS_ACCESS before continuing          */
    gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

    MSG_GERAN_MED_1_G("MAC GRR_MAC_DTM_CHANNEL_ASSIGN_REQ contains invalid ass reason%d",dtm_ass_rej);
    gmac_send_mac_grr_dtm_channel_assign_rej(dtm_ass_rej,gas_id);
    gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);
}

#endif /* FEATURE_GSM_DTM */
/*===========================================================================
===
===  FUNCTION    gmac_null_dl_sb_est_handler()
===
===  DESCRIPTION
===  Handle downlink single block request from GRR.
===  loads MAC_L1_FREQ_CONFIG data
===  loads MAC_L1_DL_SINGLE_BLOCK data
===
===  DEPENDENCIES
===
===  RETURN VALUE
===  none
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static boolean gmac_null_dl_sb_est_handler
(
  gm_dl_sb_est_t             *dl_sb_assign_ptr,
  mac_l1_dl_single_block_t   *mac_l1_dl_sb_config_ptr,
  mac_l1_freq_config_t       *mac_l1_freq_config_ptr,
  gas_id_t                    gas_id
)
{
  boolean tbf_ass_ok = TRUE;

  frequency_information_T       *fc_msg_ptr = &(mac_l1_freq_config_ptr->frequency_information);
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  mac_l1_dl_sb_config_ptr->tn = dl_sb_assign_ptr->tn;

  /* note ! starting time decoded by RR before sending to MAC */
  mac_l1_dl_sb_config_ptr->tbf_starting_time = dl_sb_assign_ptr->tbf_starting_time;

#ifdef FEATURE_GSM_DTM
  global_data_ptr->gmac_dtm_cs_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
#endif /*FEATURE_GSM_DTM*/

  if ( gmac_gprs_check_frequency_params(
              &(dl_sb_assign_ptr->frequency_list),
                dl_sb_assign_ptr->hopping_flag,
                gas_id) == FALSE )
  {
    /* load MAC_L1_FREQ_CONFIG with parameters */
    MSG_GERAN_MED_0_G("GMAC INFO Freq Band Check Passed");

    mac_l1_freq_config_ptr->reuse_sb_frq_alloc = FALSE;

    fc_msg_ptr->tsc = dl_sb_assign_ptr->tsc;
    fc_msg_ptr->hopping_flag = dl_sb_assign_ptr->hopping_flag;
    fc_msg_ptr->maio = dl_sb_assign_ptr->maio;
    fc_msg_ptr->hsn = dl_sb_assign_ptr->hsn;
    fc_msg_ptr->frequency_list = dl_sb_assign_ptr->frequency_list;

  }
  else
  {
    tbf_ass_ok = FALSE;
  }

  return(tbf_ass_ok);
}


/*===========================================================================

FUNCTION gmac_send_mac_l1_dl_sb_config

DESCRIPTION
  Sends mac_l1_dl_config

PARAMS
  mac_l1_dl_sb_config_ptr - DL SB Assignment parameters for L1.

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/

static void gmac_send_mac_l1_dl_sb_config(mac_l1_dl_single_block_t *mac_l1_dl_sb_config_ptr,
                                          gas_id_t                  gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  IMH_T  *l1_sig_message_header_ptr;

  MSG_GERAN_MED_0_G( "MAC sent MAC_L1_DL_SINGLE_BLOCK");

  l1_sig_message_header_ptr = &(global_data_ptr->l1_sig.mac_l1_dl_single_block.message_header);
  global_data_ptr->l1_sig.mac_l1_dl_single_block = *mac_l1_dl_sb_config_ptr;

  l1_sig_message_header_ptr->message_set = MS_MAC_L1;
  l1_sig_message_header_ptr->message_id = (uint8)MAC_L1_DL_SINGLE_BLOCK;
  gprs_l1_mac_acc_func(&global_data_ptr->l1_sig, gas_id);

}

/*===========================================================================
===
===  FUNCTION    gmac_null_dl_tbf_est_handler()
===
===  DESCRIPTION
===  Handle downlink TBF block request from GRR. Store L1 parameters
===  to be sent to L1 when frequency parameteres have been confirmed.
===  Build and send frequency parameters to L1 using MAC_L1_FREQ_CONFIG.
===
===  DEPENDENCIES
===  gmac_null_store_l1_dl_config - used to store L1 parameters until required
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static boolean gmac_null_dl_tbf_est_handler
(
  gm_dl_tbf_est_t                    *dl_assign_ptr,
  mac_l1_dl_tbf_config_t             *mac_l1_dl_config_ptr,
  mac_l1_freq_config_t               *mac_l1_freq_config_ptr,
  gas_id_t                            gas_id
)
{
  boolean                  tbf_ass_ok = TRUE;
  control_ack_type_t       ab_type;
  frequency_information_T *fc_msg_ptr = &(mac_l1_freq_config_ptr->frequency_information);
  gmac_null_data_t        *null_data_ptr = gmac_get_null_data_ptr(gas_id);
  gmac_global_data_t      *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  null_data_ptr->gmac_store_51access_polling.polling_flag = FALSE;

  if(gmac_check_egprs_dl_ass(dl_assign_ptr,gas_id) == TRUE)
  {
    if (dl_assign_ptr->tbf_starting_time_flag == valid)
    {
      mac_l1_dl_config_ptr->tbf_start_time_valid = TRUE;
      mac_l1_dl_config_ptr->tbf_start_time = dl_assign_ptr->tbf_starting_time;

      /*
      ** If the Polling bit is set to 1, MS shall send a PACKET CONTROL ACKNOWLEDGEMENT message (see 04.60)
      ** on the assigned PDCH, in the uplink block specified by the TBF Starting Time. In this case the
      ** TBF Starting Time is used both to indicate when the assigned PDCH becomes valid and to specify
      ** the uplink block. If the TBF Starting Time is not present or has expired, the MS shall ignore
      ** the polling request.
      */
      if (dl_assign_ptr->polling_bit == 0x01)
      {
        null_data_ptr->gmac_store_51access_polling.polling_flag = TRUE;
        null_data_ptr->gmac_store_51access_polling.tn = dl_assign_ptr->tn;
        null_data_ptr->gmac_store_51access_polling.starting_time = dl_assign_ptr->tbf_starting_time;
      }
      else if (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est.ta_valid == FALSE)
      {
        MSG_GERAN_MED_1_G("GMAC info IA DL ass with no TA value send PCA",
                          global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est.ta_valid);
        rr_read_public_store(RRPS_CELL_OPTIONS_ACCESS_BURST_TYPE,&ab_type,gas_id);

        if (ab_type == ACCESS_BURST)
        {
          null_data_ptr->gmac_store_51access_polling.polling_flag = TRUE;
          null_data_ptr->gmac_store_51access_polling.tn = dl_assign_ptr->tn;
          null_data_ptr->gmac_store_51access_polling.starting_time = dl_assign_ptr->tbf_starting_time;
        }
      }
    }
    else
    {
      mac_l1_dl_config_ptr->tbf_start_time_valid = FALSE;
    }

    global_data_ptr->gmac_store_dl_rlc_mode = dl_assign_ptr->rlc_mode;

    /* MAC_MODE not provided by GRR, set default of DYNAMIC allocation */
    mac_l1_dl_config_ptr->mac_mode = MAC_L1_MAC_MODE_DYNAMIC;

    /* store current mac mode for multislot checking */
    global_data_ptr->gmac_current_mac_mode = MAC_L1_MAC_MODE_DYNAMIC;

    /* convert TN into ts_allocation bitmap */
    mac_l1_dl_config_ptr->ts_allocation = 0x80 >> (dl_assign_ptr->tn);

    /* set global to be used during transfer */
    global_data_ptr->gmac_local_dl_ts_alloc = mac_l1_dl_config_ptr->ts_allocation;


    if (dl_assign_ptr->dl_pwr_ctrl_flag == valid)
    {
      mac_l1_dl_config_ptr->downlink_pwr.params_valid = TRUE;
      mac_l1_dl_config_ptr->downlink_pwr.po = dl_assign_ptr->dl_pwr_ctrl.po;
      mac_l1_dl_config_ptr->downlink_pwr.bts_pwr_ctrl_mode = dl_assign_ptr->dl_pwr_ctrl.bts_pwr_ctrl_mode;
      mac_l1_dl_config_ptr->downlink_pwr.pr_mode = dl_assign_ptr->dl_pwr_ctrl.pr_mode;
    }
    else
    {
      mac_l1_dl_config_ptr->downlink_pwr.params_valid = FALSE;
    }

    if (dl_assign_ptr->timing_advance_idx_flag == valid)
    {
      mac_l1_dl_config_ptr->ta_params.cont_ta_ie.present = TRUE;
      mac_l1_dl_config_ptr->ta_params.cont_ta_ie.tai = dl_assign_ptr->timing_advance_idx;
      mac_l1_dl_config_ptr->ta_params.cont_ta_ie.ta_timeslot = dl_assign_ptr->tn;
    }else
    {
      mac_l1_dl_config_ptr->ta_params.cont_ta_ie.present = FALSE;
    }

    if (dl_assign_ptr->ta_valid == (uint8)valid)
    {
      mac_l1_dl_config_ptr->ta_params.ta_value_ie.present = TRUE;
      mac_l1_dl_config_ptr->ta_params.ta_value_ie.ta_value = dl_assign_ptr->timing_advance;
    }else
    {
      mac_l1_dl_config_ptr->ta_params.ta_value_ie.present = FALSE;
    }

    mac_l1_dl_config_ptr->power_control.present = TRUE;

    /* power control - alpha */
    if (dl_assign_ptr->alpha > PWR_CTRL_ALPHA_MAX_VALUE)
    {
      mac_l1_dl_config_ptr->power_control.alpha = PWR_CTRL_ALPHA_MAX_VALUE;
    }
    else
    {
      mac_l1_dl_config_ptr->power_control.alpha = dl_assign_ptr->alpha;
    }

    /* power control - gamma */
    mac_l1_dl_config_ptr->power_control.gamma_TN0.present = FALSE;
    mac_l1_dl_config_ptr->power_control.gamma_TN1.present = FALSE;
    mac_l1_dl_config_ptr->power_control.gamma_TN2.present = FALSE;
    mac_l1_dl_config_ptr->power_control.gamma_TN3.present = FALSE;
    mac_l1_dl_config_ptr->power_control.gamma_TN4.present = FALSE;
    mac_l1_dl_config_ptr->power_control.gamma_TN5.present = FALSE;
    mac_l1_dl_config_ptr->power_control.gamma_TN6.present = FALSE;
    mac_l1_dl_config_ptr->power_control.gamma_TN7.present = FALSE;

    if (dl_assign_ptr->tn == 0)
    {
      mac_l1_dl_config_ptr->power_control.gamma_TN0.present = TRUE;
      mac_l1_dl_config_ptr->power_control.gamma_TN0.gamma = dl_assign_ptr->gamma;
    }
    else if (dl_assign_ptr->tn == 1)
    {
      mac_l1_dl_config_ptr->power_control.gamma_TN1.present = TRUE;
      mac_l1_dl_config_ptr->power_control.gamma_TN1.gamma = dl_assign_ptr->gamma;
    }
    else if (dl_assign_ptr->tn == 2)
    {
      mac_l1_dl_config_ptr->power_control.gamma_TN2.present = TRUE;
      mac_l1_dl_config_ptr->power_control.gamma_TN2.gamma = dl_assign_ptr->gamma;
    }
    else if (dl_assign_ptr->tn == 3)
    {
      mac_l1_dl_config_ptr->power_control.gamma_TN3.present = TRUE;
      mac_l1_dl_config_ptr->power_control.gamma_TN3.gamma = dl_assign_ptr->gamma;
    }
    else if (dl_assign_ptr->tn == 4)
    {
      mac_l1_dl_config_ptr->power_control.gamma_TN4.present = TRUE;
      mac_l1_dl_config_ptr->power_control.gamma_TN4.gamma = dl_assign_ptr->gamma;
    }
    else if (dl_assign_ptr->tn == 5)
    {
      mac_l1_dl_config_ptr->power_control.gamma_TN5.present = TRUE;
      mac_l1_dl_config_ptr->power_control.gamma_TN5.gamma = dl_assign_ptr->gamma;
    }
    else if (dl_assign_ptr->tn == 6)
    {
      mac_l1_dl_config_ptr->power_control.gamma_TN6.present = TRUE;
      mac_l1_dl_config_ptr->power_control.gamma_TN6.gamma = dl_assign_ptr->gamma;
    }
    else if (dl_assign_ptr->tn == 7)
    {
      mac_l1_dl_config_ptr->power_control.gamma_TN7.present = TRUE;
      mac_l1_dl_config_ptr->power_control.gamma_TN7.gamma = dl_assign_ptr->gamma;
    }

    mac_l1_dl_config_ptr->dl_tfi_present = TRUE;
    mac_l1_dl_config_ptr->dl_tfi = dl_assign_ptr->tfi_assignment;

    /* set up MAC_L1_DL_TBF_CONFIG parameters */
    if ( TRUE == dl_assign_ptr->egprs_alloc )
    {
      if ( TRUE == dl_assign_ptr->egprs_bep_period2_present )
      {
        mac_l1_dl_config_ptr->bep_period2.present = TRUE;
        mac_l1_dl_config_ptr->bep_period2.bep_period2 = dl_assign_ptr->egprs_bep_period2;
      }
      else
      {
        mac_l1_dl_config_ptr->bep_period2.present = FALSE;
      }

      mac_l1_dl_config_ptr->tbf_mode = TBF_MODE_EGPRS;
      mac_l1_dl_config_ptr->link_qual_meas_mode = dl_assign_ptr->egprs_lq_meas_mode;
      mac_l1_dl_config_ptr->rlc_mode = (mac_rlc_mode_T)dl_assign_ptr->rlc_mode;
    }
    else
    {
      mac_l1_dl_config_ptr->tbf_mode = TBF_MODE_GPRS;
      mac_l1_dl_config_ptr->rlc_mode = (mac_rlc_mode_T)dl_assign_ptr->rlc_mode;
    }

    /* Store global version of TFI */
    global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = TRUE;
    global_data_ptr->gmac_local_tfi_store.dl_tfi = dl_assign_ptr->tfi_assignment;

    /* Store TLLI on which the DL Assignment from GRR was addressed. */
    global_data_ptr->gmac_store_dl_ass_tlli = dl_assign_ptr->tlli;

    /* Store global freq params for use in transfer */
    global_data_ptr->gmac_store_freq_params_in_use.tsc = dl_assign_ptr->tsc;
    global_data_ptr->gmac_store_freq_params_in_use.hopping_flag = dl_assign_ptr->hopping_flag;
    global_data_ptr->gmac_store_freq_params_in_use.maio = dl_assign_ptr->maio;
    global_data_ptr->gmac_store_freq_params_in_use.hsn = dl_assign_ptr->hsn;
    global_data_ptr->gmac_store_freq_params_in_use.frequency_list = dl_assign_ptr->frequency_list;

    /* store EGPRS global parameters */

    if ( TRUE == dl_assign_ptr->egprs_alloc )
    {
      global_data_ptr->current_tbf_mode = TBF_MODE_EGPRS;

      /* convert 5-bit encoded WS (04.60 12.5.2) into an absolute value */
      global_data_ptr->current_egprs_dl_ws = ((dl_assign_ptr->egprs_dl_ws * 32) + 64);
    }
    else
    {
      global_data_ptr->current_tbf_mode = TBF_MODE_GPRS;
    }

    /*There is no ctrl ack in DL assignment rcvd via IA. Set to default*/   
    global_data_ptr->current_rcvd_dl_ctrl_ack = FALSE;

#ifdef FEATURE_GSM_DTM
    global_data_ptr->gmac_dtm_cs_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
#endif /*FEATURE_GSM_DTM*/

    /*Featured dl_tbf_est logging. Consider both feature and both option in EDGE*/
    if (dl_assign_ptr->egprs_alloc)
    {
      mac_log_dl_etbf_establish(
        global_data_ptr->gmac_local_tfi_store.dl_tfi,
        global_data_ptr->gmac_local_dl_ts_alloc,
        global_data_ptr->gmac_store_dl_rlc_mode,
        (byte) dl_assign_ptr->egprs_alloc,
        global_data_ptr->current_egprs_dl_ws,
        mac_l1_dl_config_ptr->link_qual_meas_mode,
        mac_l1_dl_config_ptr->bep_period2.bep_period2,
        gas_id);

    }
    else
    {
         mac_log_dl_tbf_establish(
         global_data_ptr->gmac_local_tfi_store.dl_tfi,
         global_data_ptr->gmac_local_dl_ts_alloc,
         global_data_ptr->gmac_store_dl_rlc_mode,
         gas_id);
    }

    /*note DL RLC config is sent on ph_connect_ind and uses function    */
    /*gmac_util_send_dl_ass_to_rlc() to build and send the signal to RLC*/

    if ( gmac_gprs_check_frequency_params(
                &(dl_assign_ptr->frequency_list),
                  dl_assign_ptr->hopping_flag,
                  gas_id) == FALSE )
    {
      MSG_GERAN_MED_0_G("GMAC INFO Freq Band Check Passed");

      /* load  MAC_L1_FREQ_CONFIG with parameters */
      mac_l1_freq_config_ptr->reuse_sb_frq_alloc = FALSE;
      fc_msg_ptr->tsc = dl_assign_ptr->tsc;
      fc_msg_ptr->hopping_flag = dl_assign_ptr->hopping_flag;
      fc_msg_ptr->maio = dl_assign_ptr->maio;
      fc_msg_ptr->hsn = dl_assign_ptr->hsn;
      fc_msg_ptr->frequency_list = dl_assign_ptr->frequency_list;

    }
    else
    {
      tbf_ass_ok = FALSE;
    }
  }
  else
  {
    tbf_ass_ok = FALSE;
  }

  return(tbf_ass_ok);
}


/*===========================================================================
===
===  FUNCTION    gmac_null_check_for_polled_dl_ass()
===
===  DESCRIPTION
===
===      If IA for DL is polled then send PCA in to L1 an RRBP buffer.
===      Note IA for DL SB alloc the polling bit should not be include
===      in the assignment, and is not relevant for IA for DL SB. Hence
===      if it appears in the assignment it is ignored.
===
===  DEPENDENCIES
===
===      gmac_store_51access_polling
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===========================================================================*/


static void gmac_null_check_for_polled_dl_ass(gas_id_t gas_id)
{
  gmac_null_data_t *null_data_ptr = gmac_get_null_data_ptr(gas_id);

  if (null_data_ptr->gmac_store_51access_polling.polling_flag == TRUE)
  {
    MSG_GERAN_MED_1_G("MAC info IA DL ass was polled p_flg =%d",null_data_ptr->gmac_store_51access_polling.polling_flag);
    gmac_rrbp_send_pca(null_data_ptr->gmac_store_51access_polling.starting_time, null_data_ptr->gmac_store_51access_polling.tn, FALSE, 0, PCA_51_MSG,gas_id);
    null_data_ptr->gmac_store_51access_polling.polling_flag = FALSE;
  }

  return;
}

/*===========================================================================
===
===  FUNCTION      null_mode_handler
===
===  DESCRIPTION
===  Handles the signal reception and the associated actions
===  for null mode states.
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void null_mode_handler(uint8 task_id, uint8 signal_id, gas_id_t gas_id)
{
  gmac_null_data_t   *null_data_ptr = gmac_get_null_data_ptr(gas_id);
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  switch(null_data_ptr->mac_null_substate)
  {
    /***************************
    * null state NULL_INACTIVE *
    ****************************/
    case NULL_INACTIVE:
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {

          case GRR_MAC_PS_ACCESS_REQ:
            MSG_GERAN_MED_2_G("GMAC Rcvd GRR_MAC_PS_ACCESS_REQ action %d pcco_in_progress %d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ps_access_req.action,
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ps_access_req.pcco_in_progress);

            /*Stop t3172 if camping on to a new cell*/
            if(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ps_access_req.action == CELL_CHANGE_PURGE_GMM_SIG)
            {
              gmac_stop_t3172(gas_id);
            }

            /*check if PCCO */
            gmac_util_start_pcco_process(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ps_access_req.pcco_in_progress,
                                         GMAC_NULL,__LINE__,(uint8)null_data_ptr->mac_null_substate,
                                         gas_id);

            /*Always make sure gmac_null_releasing_l1 has not been left */
            /*inadvertently set when moving to null  inactive state     */
            null_data_ptr->gmac_null_releasing_l1 = FALSE;

#ifdef FEATURE_GSM_EDTM
            /* Initialise the edtm enabled flag from the public store*/
            gmac_edtm_enabled_init(gas_id);
#endif /*FEATURE_GSM_EDTM*/

            if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
            {
              /*Unblock RLC and wait for RM_PS_ACCESS_CNF*/
              MSG_GERAN_HIGH_0_G("GMAC resuming PS access");
              gmac_util_unblock_rlc(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ps_access_req.action
                                    ,GMAC_NULL,__LINE__,
                                    gas_id);
            }
            else
            {
              /*emulate what rr does for IA rej with wait indicator .ie sends no access     */
              /*with  del one and on access send NO_CELL_CHANGE_PURGE_GMM_SIG               */                     
              /*if t3172 is running than UL access is prohibited. However DL establishment  */
              /*is allowed so switch to null inactive state so IA for DL can be rcvd. If    */
              /*during DL est or release,t3172 is running than RLC will not be given access.*/

              MSG_GERAN_HIGH_0_G("GMAC T3172 Running. Wait for T3172 to expire before resuming PS access");

              gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);

               /*  52 page mode handling is under review              */
              /*reflects better what was in the original code Current*/
              gmac_util_initialise_page_mode(gas_id);

              /* Initialise multislot class data - this is based on the NV multislot class */
              /* value in the Public Store                                                 */
              gmac_multislot_class_init(gas_id);

              MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
              null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
              (void)mac_log_state(gas_id);

              MSG_GERAN_HIGH_1_G("MAC set MAC_MODE : %d",global_data_ptr->mac_mode);
              mac_log_camped_on_cell(gas_id);

  #ifdef FEATURE_GSM_EDTM
              /* Initialise the edtm enabled flag from the public store*/
              gmac_edtm_enabled_init(gas_id);
  #endif /*FEATURE_GSM_EDTM*/

              /*send cnf to RR*/
              gmac_send_mac_grr_ps_access_cnf(gas_id);

            }
            break;


          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            reset_tbf_parameters(gas_id);
            gmac_reset_rac_params(gas_id);

            gmac_util_block_rlc(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                GMAC_NULL,__LINE__,
                                gas_id);

            /*send mac status before attempting to complete PCCO process*/
            gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

            /*complete any PCCO/ PCCF activities*/
            gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,GRR_INITIATED,GMAC_NULL,__LINE__,gas_id);
            gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,GRR_INITIATED,GMAC_NULL,__LINE__,gas_id);
            break;

          case GRR_MAC_RACH_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_RACH_REQ");
            gmac_util_send_grr_mac_rach_cnf(gas_id);
            break;

#ifdef FEATURE_GPRS_PS_HANDOVER
          case GRR_MAC_PSHO_REQ:
            MSG_GERAN_MED_1_G("GMAC rxd GRR_MAC_PSHO_REQ psho_type: 0x%x",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_psho_req.psho_type);

          break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/
          default:
            /* unknown signal from GRR */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from GRR %d",signal_id);
            break;
        }
      } /* end of GRR_MAC signals */
      else if (task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_PS_ACCESS_CNF:

            MSG_GERAN_MED_0_G("GMAC Info Rcvd RM_PS_ACCESS_CNF");

            if(gmac_util_is_rlc_blocked(GMAC_IDLE,__LINE__,gas_id) == BLOCKED)
            {
              /*Possibility exist when MAC is releasing the DL TFB and has sent        */
              /*MR_PS_ACCESS_REQ to RLC,Leaves transfer and returns to NULL mode       */
              /*state NULL_GPRS_51_ACTIVE. Before RLC can respond withRM_PS_ACCESS_CNF */
              /*MAC receives GRR_MAC_NO_PS_ACCESS_IND From RR. MAC transitions         */
              /*to NULL_INACTIVE. Then rlc access cnf is rcvd. This cause mac          */
              /*incorrectly to move back to NULL_GPRS_51_ACTIVE. RM_PS_ACCESS_CNF      */
              /* should be ignored.                                                    */
              gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
            }
            else
            {
              /*  52 page mode handling is under review              */
              /*reflects better what was in the original code Current*/
              gmac_util_initialise_page_mode(gas_id);
  
              /* Initialise multislot class data - this is based on the NV multislot class */
              /* value in the Public Store                                                 */
              gmac_multislot_class_init(gas_id);
  
              MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
              null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
              (void)mac_log_state(gas_id);
  
              MSG_GERAN_HIGH_1_G("MAC set MAC_MODE : %d",global_data_ptr->mac_mode);
              mac_log_camped_on_cell(gas_id);

#ifdef FEATURE_GSM_EDTM
              /* Initialise the edtm enabled flag from the public store*/
              gmac_edtm_enabled_init(gas_id);
#endif /*FEATURE_GSM_EDTM*/
  
              /*send cnf to RR*/
              gmac_send_mac_grr_ps_access_cnf(gas_id);
            }
            break;


          case RM_UL_EST_TBF_REQ:
            /*Case where GRR_MAC_NO_PS_ACCESS_IND was rcvd in NULL_GPRS_51_ACTIVE and MAC    */
            /*sends RLC PS NO ACCESS however the next signal in MAC's Q is RM_UL_EST_TBF_REQ */
            /*as MAC has already sent RLC NO PS ACCESS blocking RLC, MAC will ignore this    */
            /*request and unblock when GRR_MAC_PS_ACCESS_REQ is rcvd                         */
            MSG_GERAN_MED_0_G("GMAC Info Rcvd RM_UL_EST_TBF_REQ race condition sig Ignored");
            (void) gmac_util_is_rlc_blocked(GMAC_NULL,__LINE__,gas_id);
            break;

          default:
            /* unknown internal signal from RLC */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from RLC %d",signal_id);
            break;
        }
      } /* end of RLC_MAC signals */
      break; /* end of NULL_INACTIVE */

    /*********************************
    * null state NULL_GPRS_51_ACTIVE *
    **********************************/
    case NULL_GPRS_51_ACTIVE:
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {

          case GRR_MAC_PS_ACCESS_REQ:
            MSG_GERAN_ERROR_0_G("GMAC Unexpected GRR_MAC_PS_ACCESS_REQ");
            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            reset_tbf_parameters(gas_id);
            gmac_reset_rac_params(gas_id);

            gmac_util_block_rlc(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                GMAC_NULL,__LINE__,
                                gas_id);

            /*send mac status before attempting to complete PCCO process*/
            gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

            /*complete any PCCO activity and PCCF activities*/
            gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,GRR_INITIATED,GMAC_NULL,__LINE__,gas_id);
            gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,GRR_INITIATED,GMAC_NULL,__LINE__,gas_id);

            MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
            null_data_ptr->mac_null_substate = NULL_INACTIVE;
            break;

          case GRR_MAC_SEND_MEAS_RPT:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_MEAS_RPT");

            if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
            {
              if (gmac_util_encode_ul_control_msg_handler(PMR,FALSE,gas_id) == PASS)
              {
                /*establishing UL for GRR block RLC until PMR is sent*/
                gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

                null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 = FALSE;
                null_data_ptr->gmac_null_ul_sb_type = NULL_UL_SB_TYPE_PMR;
                gmac_util_send_grr_mac_status(MAC_IDLE_BLOCKED,gas_id);
                gmac_null_send_ul_tbf_req(GMAC_SINGLE_BLOCK_ACCESS, RR_PRIORITY_CLASS_1,FALSE,gas_id);
                MSG_GERAN_MED_0_G("GMAC State null NULL_UPLINK_SB_ESTAB");
                null_data_ptr->mac_null_substate = NULL_UPLINK_SB_ESTAB;
                (void)mac_log_state(gas_id);
              }
            }
            else
            {
             MSG_GERAN_MED_0_G( "GMAC T3172 running. Can't send PMR");
             gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
            }
            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");

            if (gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
            {
                /*establishing UL for GRR block RLC until PCCF is sent*/
                gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

                gmac_util_start_pccf_process(SENDING_VIA_SB,GMAC_NULL,__LINE__,(uint8)NULL_GPRS_51_ACTIVE,gas_id);

                null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 = FALSE;
                null_data_ptr->gmac_null_ul_sb_type = NULL_UL_SB_TYPE_PCCF;
                gmac_util_send_grr_mac_status(MAC_IDLE_BLOCKED,gas_id);
                gmac_null_send_ul_tbf_req(GMAC_SINGLE_BLOCK_ACCESS, RR_PRIORITY_CLASS_1,FALSE,gas_id);
                MSG_GERAN_MED_0_G("MAC null state NULL_UPLINK_SB_ESTAB");
                null_data_ptr->mac_null_substate = NULL_UPLINK_SB_ESTAB;
                (void)mac_log_state(gas_id);
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GMAC info Failed to CSN encode PCCF");
              /*This is an exception case in the formatting of PCCF which can't be recovered   *
              * from by trying to resent another PCCF. Don't inform GRR of the fialure to send *
              * PCCF wait for 73176 to expire at which point GRR will perform a reselection    */
            }
            break;

          case GRR_MAC_DL_SB_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DL_SB_EST");

            if (gmac_null_dl_sb_est_handler(&global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_sb_est,
                                            &null_data_ptr->gmac_null_mac_l1_dl_sb_config,
                                            &null_data_ptr->gmac_null_mac_l1_freq_config,
                                             gas_id)  == TRUE)
            {
              /*establishing DL SB for GRR block RLC until DL SB has been rcvd*/
              gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

              gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
              gmac_send_mac_l1_dl_sb_config(&null_data_ptr->gmac_null_mac_l1_dl_sb_config,gas_id);
              gmac_util_send_grr_mac_status(MAC_IDLE_BLOCKED,gas_id);
              MSG_GERAN_MED_0_G("GMAC State null NULL_DOWNLINK_SB_ESTAB");
              null_data_ptr->mac_null_substate = NULL_DOWNLINK_SB_ESTAB;
              (void)mac_log_state(gas_id);
            }
            else
            {
             /*assignment error*/
             MSG_GERAN_HIGH_0_G("MAC info error in DL SB ass, Ignore IA"); 
             gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);
            }

            break;

          case GRR_MAC_DL_TBF_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DL_TBF_EST");
            if(gmac_null_dl_tbf_est_handler(&global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est,
                                            &null_data_ptr->gmac_null_mac_l1_dl_config,
                                            &null_data_ptr->gmac_null_mac_l1_freq_config,
                                            gas_id)  == TRUE)
            {

              /*establishing DL block RLC until DL has been connected*/
              gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

              /* wait for starting time to occur before configuring RLC dl tbf */
              gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
              gmac_send_mac_l1_dl_config(&null_data_ptr->gmac_null_mac_l1_dl_config,gas_id);
              gmac_util_send_grr_mac_status(MAC_IDLE_BLOCKED,gas_id);
              gmac_null_check_for_polled_dl_ass(gas_id);
              MSG_GERAN_MED_0_G("GMAC State null NULL_DL_TBF_EST");
              null_data_ptr->mac_null_substate = NULL_DL_TBF_EST;
            }
            else
            {
              /*assignment error*/
              MSG_GERAN_HIGH_0_G("MAC info error in DL ass, Ignore IA"); 
              gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);
            }
            break;

          case GRR_MAC_RACH_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_RACH_REQ");
            gmac_util_send_grr_mac_rach_cnf(gas_id);
            break;


#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:

            /* AS no UL has been requested the only acceptable DTM ass here */
            /* is one to establish a DL TBF                                 */
            if (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type == DL_TBF_PRESENT)
            {
              MSG_GERAN_MED_0_G("MAC rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ :- DL_TBF_PRESENT ");

              null_data_ptr->gmac_null_dtm_ass_status = gmac_dtm_chan_ass_handler(
                                                         &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                                                         &null_data_ptr->gmac_null_mr_ul_alloc_cnf_sig,
                                                         &null_data_ptr->gmac_null_mac_l1_dtm_chan_ass_req,
                                                         &null_data_ptr->gmac_null_mac_l1_freq_config,
                                                         gas_id);

              if (null_data_ptr->gmac_null_dtm_ass_status == NO_DTM_ASS_ERROR)
              {
                /*establishing DL block RLC until DL has been connected*/
                gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

                gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
                gmac_send_mac_l1_dtm_channel_assign_req(&null_data_ptr->gmac_null_mac_l1_dtm_chan_ass_req,gas_id);
                null_data_ptr->gmac_null_dtm_tbf_est_type = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type;
                null_data_ptr->mac_null_substate = NULL_DTM_CONNECT_PENDING;
                MSG_GERAN_MED_1_G("GMAC State null NULL_DTM_CONNECT_PENDING gmac_null_dtm_tbf_est_type %d",
                                  null_data_ptr->gmac_null_dtm_tbf_est_type);
              }
              else
              {
                gmac_null_rej_dtm_ass(null_data_ptr->gmac_null_dtm_ass_status,gas_id);
                MSG_GERAN_MED_0_G("GMAC State null  NULL_INACTIVE");
                null_data_ptr->mac_null_substate = NULL_INACTIVE;
              }
            }
            else
            {
              gmac_null_rej_dtm_ass(DTM_ASS_REJ_PROTOCOL_ERR,gas_id);
              MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
              null_data_ptr->mac_null_substate = NULL_INACTIVE;
            }
            break;

          case GRR_MAC_DTM_UL_DCCH_IND:
            MSG_GERAN_MED_0_G("MAC unexpected recpeiton of GRR_MAC_DTM_UL_DCCH_IND");
            break;
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_0_G("GMAC Rejected GRR_MAC_EDTM_PKT_CS_EST_REQ, Not in Contention Resolution");
            gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_CANNOT_SERVICE_REQUEST,gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
          case GRR_MAC_PSHO_REQ:
            MSG_GERAN_MED_0_G("GMAC Rejected GRR_MAC_PSHO_REQ in null mode");
            gmac_util_send_grr_psho_cnf(PSHO_FAILURE,gas_id);
          break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/
          default:
            /* unknown signal from GRR */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from GRR %d",signal_id);
            break;
        }
      } /* end of GRR_MAC signals */
      else if (task_id == MS_MAC_MAC)
      {
        switch (signal_id)
        {

          case MAC_TN_NO_SERVICE:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TN_NO_SERVICE");

            /*GRR initiated NO access was rcvd in Transfer. No action. */
            /*Wait for GRR_MAC_PS_ACCESS_IND then unblock RLC          */
            MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
            null_data_ptr->mac_null_substate = NULL_INACTIVE;
            break;

          case MAC_IN_NO_SERVICE:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_IN_NO_SERVICE");
            /*GRR initiated NO access was rcvd in idle. No action. */
            /*Wait for GRR_MAC_PS_ACCESS_IND then unblock RLC      */
            MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
            null_data_ptr->mac_null_substate = NULL_INACTIVE;
            break;

          case MAC_TIMER_T3172:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3172");
            /* If t3172 is runninig RLC will have been blocked. Unblock RLC*/
            gmac_util_unblock_rlc(NO_CELL_CHANGE_PURGE_GMM_SIG,GMAC_NULL,__LINE__,gas_id);
            break;

          default:
            /* unknown internal signal from MAC */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from MAC %d",signal_id);
            break;
        }
      } /* end of MAC_MAC signals */
      else if (task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {

          case RM_UL_EST_TBF_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_EST_TBF_REQ");

            if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.test_mode != TEST_MODE_OFF )
            {
              MSG_GERAN_ERROR_1_G("GMAC TEST Mode: %d, ordered when in NULL", 
                                  global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.test_mode);
            }

            global_data_ptr->current_test_mode = TEST_MODE_OFF;

            if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.pfi_present )
            {
              global_data_ptr->pfi_present = TRUE;
              global_data_ptr->pfi = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.pfi;
            }
            else
            {
              global_data_ptr->pfi_present = FALSE;
            }

            global_data_ptr->access_requested = determine_access_requested(gas_id);

            gmac_util_send_grr_mac_status(MAC_IDLE_BLOCKED,gas_id);

            null_data_ptr->rlc_access_type = gmac_null_determine_grr_access_type(
                                              global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.data_req_cause,
                                              global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.chan_req_desc.rlc_mode,
                                              global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.num_of_blocks,
                                              global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.access_phase,
                                              gas_id );

            /*reset ps_page_response before handling each new request*/
            null_data_ptr->n_flags.ps_page_response = FALSE;

            if(global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.data_req_cause == GRR_CAUSE_GMM_PAGE_RESPONSE)
            {
              null_data_ptr->n_flags.ps_page_response = TRUE;
            }

            /*TLLI in UL CNF to RLC*/
            global_data_ptr->gmac_store_ul_est_tlli =  global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.tlli;
            MSG_GERAN_HIGH_2_G("MAC info UL est tlli %x, gmac_store_ul_est_tlli %x",
                               global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.tlli, 
                               global_data_ptr->gmac_store_ul_est_tlli);

#ifdef FEATURE_GSM_DTM
            gmac_null_dtm_send_ul_tbf_req(null_data_ptr->rlc_access_type,
                                          &(global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req),gas_id);
#else
            gmac_null_send_ul_tbf_req(null_data_ptr->rlc_access_type,
                                      global_data_ptr->gmac_local_stored_ul_chan_req.radio_priority,
                                      null_data_ptr->n_flags.ps_page_response,
                                      gas_id);
#endif /*FEATURE_GSM_DTM*/

            MSG_GERAN_MED_0_G("GMAC State null NULL_UPLINK_TBF_ESTAB");
            null_data_ptr->mac_null_substate = NULL_UPLINK_TBF_ESTAB;
            (void)mac_log_state(gas_id);

            break;

          default:
            /* unknown signal from RLC */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from RLC %d",signal_id);
            break;
        }
      } /* end of RLC_MAC signals */
      else if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {

          case L1_MAC_TBF_REL_CONFIRM:
            MSG_GERAN_ERROR_1_G("GMAC info rcvd L1_MAC_TBF_REL_CONFIRM should not get this signal in this state gmac_null_tbf_rel_cause %d",
                                null_data_ptr->gmac_null_tbf_rel_cause);

            break;

          default:
            /* unknown signal from L1 */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from L1 %d",signal_id);
            break;
        }
      } /* end of L1_MAC signals */
      break;

    /***********************************
    * null state NULL_UPLINK_TBF_ESTAB *
    ************************************/
    case NULL_UPLINK_TBF_ESTAB:
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {

          case GRR_MAC_PS_ACCESS_REQ:
            MSG_GERAN_ERROR_0_G("GMAC Unexpected GRR_MAC_PS_ACCESS_REQ");
            break;

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            reset_tbf_parameters(gas_id);
            gmac_reset_rac_params(gas_id);

            gmac_util_block_rlc(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                GMAC_NULL,__LINE__,gas_id);

            if(null_data_ptr->gmac_null_releasing_l1 == TRUE )
            {
              /* MAC has rcvd an assignment and is already releasing. ie assignment was erroneous */
              /* erroneous. under these conditions RR does not release L1 from its raching state  */

              /*stay in state till rel cnf rcvd*/
              gmac_null_send_release_to_l1(GMAC_NULL_GRR_INITIATED_NO_PS_ACCESS,UL_TBF_ABNORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_CELL_RESELECTION,gas_id);
            }
            else
            {
              /*MAC has yet to have rcvd an assignment, RR terminated UL est before and assignment*/
              /*was rcvd. under these conditions RR will release L1 from its raching state        */

              /*send mac status before attempting to complete PCCO process*/
              gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

              /*complete any PCCO activity and PCCF activities*/
              gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,GRR_INITIATED,GMAC_NULL,__LINE__,gas_id);
              gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,GRR_INITIATED,GMAC_NULL,__LINE__,gas_id);

              MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
              null_data_ptr->mac_null_substate = NULL_INACTIVE;
            }
            break;

          case GRR_MAC_UL_TBF_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_UL_TBF_EST");

            if ( gmac_null_ul_tbf_req(gas_id) == TRUE )
            {
              gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);

              /***** simulate Freq Confirm *******************************/
              if((global_data_ptr->access_tech_req_present == TRUE) && (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS))
              {
               MSG_GERAN_MED_1_G("MAC info NW request for RAC info access_tech_req_present %d",global_data_ptr->access_tech_req_present);

               gmac_idle_egprs_rac_procedure_2(FALSE,FALSE,FALSE,FALSE,gas_id);
              }

              /* send MAC_L1_DUMMY_CTRL_BLOCK to L1 */
              gmac_ul_dummy_blk_to_l1(gas_id);

              /*if pccf has to be sent*/
              if(gmac_util_get_pccf_state(gas_id) == SENDING_VIA_UL_PACCH)
              {
                MSG_GERAN_MED_0_G("MAC info PCCF SENT TO L1");
                gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NOT_NEEDED,gas_id);
              }

              /* send MAC_L1_DYNAMIC_TBF_CONFIG with parameters              */

              /* NOTE can only ever receive DYNAMIC alloc as any fixed alloc */
              /* is filtered out in gmactask via gmac_assigned_fix_alloc()   */
              MSG_GERAN_MED_0_G("MAC Sent MAC_L1_DYNAMIC_TBF_CONFIG");

              gmac_restart_ul_await_ass_guard
              ( null_data_ptr->gmac_null_store_l1_dyn_realloc.mac_l1_dynamic_tbf_config.dynamic_tbf.tbf_start_time_valid,
                null_data_ptr->gmac_null_store_l1_dyn_realloc.mac_l1_dynamic_tbf_config.dynamic_tbf.starting_time,
                gas_id);

              gmac_send_mac_l1_dyn_config(&null_data_ptr->gmac_null_store_l1_dyn_realloc.mac_l1_dynamic_tbf_config,gas_id);

              MSG_GERAN_MED_0_G("GMAC State null NULL_UPLINK_TBF_ACCESS");
              null_data_ptr->mac_null_substate = NULL_UPLINK_TBF_ACCESS;
              (void)mac_log_state(gas_id);
            }
            else
            {
              MSG_GERAN_HIGH_0_G("MAC info error in GRR_MAC_UL_TBF_EST ass, abort estab");
              gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);
              null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_TBF_REL_NORMAL;
              gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,UL_TBF_ABNORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_DATA_ERROR,gas_id);
            }
            break;

          case GRR_MAC_UL_SB_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_UL_SB_EST");

#ifdef FEATURE_GSM_DTM
            global_data_ptr->gmac_dtm_cs_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
#endif /*FEATURE_GSM_DTM*/

            if ( gmac_null_handle_ul_sb_est_prr(gas_id) == TRUE )
            {
              gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);

              /******* Simulate Frequency Confirm *******************/
              MSG_GERAN_MED_0_G("MAC Sent MAC_L1_SINGLE_BLOCK_CONFIG");

              gmac_restart_ul_await_ass_guard(TRUE,global_data_ptr->gmac_51_sb_params.sb_tbf.starting_time,gas_id);

              gmac_send_mac_l1_sb_config(&global_data_ptr->gmac_51_sb_params,gas_id);

              if(global_data_ptr->egprs_2p_sb_mb_alloc == MB_ALLOC_OF_2_BLOCKS)
              {
                gmac_ul_dummy_blk_to_l1(gas_id);
              }
              /******************************************************/

              null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 = TRUE;
              null_data_ptr->gmac_null_ul_sb_type = NULL_UL_SB_TYPE_PRR;

              MSG_GERAN_MED_0_G("GMAC State null NULL_UPLINK_SB_ESTAB");
              null_data_ptr->mac_null_substate = NULL_UPLINK_SB_ESTAB;
              (void)mac_log_state(gas_id);

              /* Note that the next msg in the sequence is a quality report.  L1 will send
                 this to MAC (in response to the SB config) just before the PH_CONNECT_IND */
            }
            else
            {
              MSG_GERAN_HIGH_0_G("MAC info error in GRR_MAC_UL_SB_EST ass, abort estab");

              gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);
              null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_TBF_REL_NORMAL;
              gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,UL_TBF_ABNORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_DATA_ERROR,gas_id);
            }
            break;

          case GRR_MAC_DL_TBF_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DL_TBF_EST");

            if(gmac_null_dl_tbf_est_handler(&global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est,
                                            &null_data_ptr->gmac_null_mac_l1_dl_config,
                                            &null_data_ptr->gmac_null_mac_l1_freq_config,
                                             gas_id)  == TRUE)
            {
              /**********************************************************************
              * GSM 04.18 section 3.5.2.1.2: If downlink assignment received during *
              * uplink establishment then abort uplink and respond to downlink      *
              * assignment. If DL est is successful, then the interrupted UL est    *
              * will be re-initiated in DL transfer                                 *
              **********************************************************************/

              /*establishing DL SB for GRR block RLC until DL SB has been rcvd*/
              gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

              /* wait for starting time to occur before configuring RLC dl tbf */
              gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
              gmac_send_mac_l1_dl_config(&null_data_ptr->gmac_null_mac_l1_dl_config,gas_id);

              /* abort UL TBF and continue with DL TBF */
              MSG_GERAN_MED_0_G("MAC info UL int by DL TBF");

              gmac_null_check_for_polled_dl_ass(gas_id);
              MSG_GERAN_MED_0_G("GMAC State null NULL_DL_TBF_EST");
              null_data_ptr->mac_null_substate = NULL_DL_TBF_EST;
            }
            else
            {
             /*assignment error*/

             /*Note In this state we are waiting for an IA for UL in response to our RACH *
             * However if an IA for DL is rcvd before IA for UL then GRR will have abort  *
             * the UL access process and hence in the case where the DL assignment is      *
             * erroneous we need to abort any UL est attempt so that RLC can request to   *
             * establishes  the interrupted UL                                             */

             MSG_GERAN_HIGH_0_G("MAC info error in GRR_MAC_DL_TBF_EST ass, abort estab");

             gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);
             null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_TBF_REL_NORMAL;
             gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,UL_TBF_ABNORMAL,gas_id);
             mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_DATA_ERROR,gas_id);
            }
            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if (gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
            {
              gmac_util_start_pccf_process(SENDING_VIA_UL_PACCH,GMAC_NULL,__LINE__,(uint8)NULL_UPLINK_TBF_ESTAB,gas_id);
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GMAC info Failed to CSN encode PCCF");
              /*This is an exception case in the formatting of PCCF which can't be recovered   *
              * from by trying to resent another PCCF. Don't inform GRR of the fialure to send *
              * PCCF wait for 73176 to expire at which point GRR will perform a reselection    */
            }
            break;

          case GRR_MAC_DL_SB_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DL_SB_EST");
            if (gmac_null_dl_sb_est_handler(&global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_sb_est,
                                            &null_data_ptr->gmac_null_mac_l1_dl_sb_config,
                                            &null_data_ptr->gmac_null_mac_l1_freq_config,
                                            gas_id)  == TRUE)
            {
              /* GSM 04.18 section 3.5.2.1.2:  If DL assignment rcvd during UL est */
              /* then abort UL TBF and continue with DL TBF                        */

              /*establishing DL SB for GRR block RLC until DL SB has been rcvd*/
              gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

              gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
              gmac_send_mac_l1_dl_sb_config(&null_data_ptr->gmac_null_mac_l1_dl_sb_config,gas_id);

              MSG_GERAN_MED_0_G("GMAC State null NULL_DOWNLINK_SB_ESTAB");
              null_data_ptr->mac_null_substate = NULL_DOWNLINK_SB_ESTAB;
              (void)mac_log_state(gas_id);
            }
            else
            {
              /*Note In this state we are waiting for an IA for UL in response to our RACH *
              * However if an IA for DL is rcvd before IA for UL then GRR will have abort  *
              * the UL access process and hence in the case where the DL assignment is      *
              * erroneous we need to abort any UL est attempt so that RLC can request to   *
              * establishes  the interrupted UL                                             */

              MSG_GERAN_HIGH_0_G("MAC info error in GRR_MAC_DL_SB_EST ass, abort estab");
              gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);
              null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_TBF_REL_NORMAL;
              gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,UL_TBF_ABNORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_DATA_ERROR,gas_id);
            }

            break;

          case GRR_MAC_RACH_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_RACH_REQ");
            gmac_util_send_grr_mac_rach_cnf(gas_id);
            break;

#ifdef FEATURE_GSM_DTM
          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:

            /* When in NON TRANSFER mode, TBF assigment can either be for UL or DL but */
            /* not both simualtiously.If while est UL a DL est is rcvd, the DL takes   */
            /* priority over UL est. The UL will be est in transfer mode via the DL    */

            if ( ( global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type == UL_TBF_PRESENT ) ||
                 ( global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type == DL_TBF_PRESENT ) ||
                 ( global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type == UL_DL_TBF_PRESENT ) )
            {
              MSG_GERAN_MED_1_G("GMAC rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ gmac_null_dtm_tbf_est_type %d",
                                global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type);

              null_data_ptr->gmac_null_dtm_ass_status = gmac_dtm_chan_ass_handler(
                                         &global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req,
                                         &null_data_ptr->gmac_null_mr_ul_alloc_cnf_sig,
                                         &null_data_ptr->gmac_null_mac_l1_dtm_chan_ass_req,
                                         &null_data_ptr->gmac_null_mac_l1_freq_config,
                                          gas_id);

              if ( null_data_ptr->gmac_null_dtm_ass_status == NO_DTM_ASS_ERROR)
              {
                null_data_ptr->gmac_null_dtm_tbf_est_type = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type;

                gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);

                if ( ( null_data_ptr->gmac_null_dtm_tbf_est_type == UL_TBF_PRESENT ) ||
                     ( null_data_ptr->gmac_null_dtm_tbf_est_type == UL_DL_TBF_PRESENT ) )
                {
                  gmac_ul_dummy_blk_to_l1(gas_id);
                  gmac_util_send_mr_ul_alloc_cnf(&null_data_ptr->gmac_null_mr_ul_alloc_cnf_sig,gas_id);
                }
                else
                {
                  MSG_GERAN_HIGH_0_G("MAC INFO UL request interrupted by DL est");
                  /*establishing DL block RLC until DL has been connected*/
                  gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);
                }

                gmac_send_mac_l1_dtm_channel_assign_req(&null_data_ptr->gmac_null_mac_l1_dtm_chan_ass_req,gas_id);
                MSG_GERAN_MED_0_G("GMAC State null NULL_DTM_CONNECT_PENDING");
                null_data_ptr->mac_null_substate = NULL_DTM_CONNECT_PENDING;
              }
              else
              {
                gmac_null_rej_dtm_ass(null_data_ptr->gmac_null_dtm_ass_status,gas_id);
                MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
                null_data_ptr->mac_null_substate = NULL_INACTIVE;
              }

            }
            else
            {
              MSG_GERAN_MED_1_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_ASSIGN_REQ dtm tbf type %d",
                                global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_assign_req.dtm_tbf_type);

              gmac_null_rej_dtm_ass(DTM_ASS_REJ_PROTOCOL_ERR,gas_id);
              MSG_GERAN_MED_0_G("GMAC State null  NULL_INACTIVE");
              null_data_ptr->mac_null_substate = NULL_INACTIVE;
            }
            break;

          case GRR_MAC_DTM_UL_DCCH_IND:

            /*GRR is confirming that the UL data is to be sent using GTTP under these    */
            /*conditions the ONLY relevant data in mr_ul_alloc_cnf signal is "dtm_access"*/
            /*Switch to NULL_DTM_CONNECT_PENDING and wait for GRR_MAC_DTM_UL_DCCH_CNF or */
            /*successful GTTP con or  GRR_MAC_GPRS_SUSPENSION  failure case              */
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DTM_UL_DCCH_IND");
            memset(&null_data_ptr->store_rlc_ul_alloc_data, 0, sizeof(null_data_ptr->store_rlc_ul_alloc_data));
            null_data_ptr->store_rlc_ul_alloc_data.dtm_access = TRUE;
            gmac_util_send_mr_ul_alloc_cnf(&null_data_ptr->store_rlc_ul_alloc_data,gas_id);

            MSG_GERAN_MED_0_G("GMAC State null NULL_DTM_GTTP_UL_CON");
            null_data_ptr->mac_null_substate = NULL_DTM_GTTP_UL_CON;
            break;

          case GRR_MAC_DTM_UL_DCCH_CNF:
            MSG_GERAN_MED_0_G("Unexpected sig Rcvd GRR_MAC_DTM_UL_DCCH_CNF");
            break;

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_0_G("GMAC Rejected GRR_MAC_EDTM_PKT_CS_EST_REQ, Not in Contention Resolution");
            gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_CANNOT_SERVICE_REQUEST,gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

#endif /* FEATURE_GSM_DTM */

          case GRR_MAC_SEND_MEAS_RPT:

            /*race condition: when Mac moves out of NULL_GPRS_51_ACTIVE due to DL,DL SB or UL est*/
            /*it sends MAC_status to RR. The assumption is that this will be the next sig in     */
            /*RR's q, hence blocking any attempt to send PMR's. IF this is not the case and T3158*/
            /*expiry is next sig in RR q then MAC can receive GRR_MAC_SEND_MEAS_RPT in the       */
            /*establishment states        */

            MSG_GERAN_MED_0_G("GMAC RCVD MAC Rcvd GRR_MAC_SEND_MEAS_RPT while est UL: fail to send PMR");
            gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
            break;

          default:
            /* unknown signal from GRR */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from GRR %d",signal_id);
            break;
        }
      } /* end of GRR_MAC signals */
      else if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case L1_MAC_TBF_REL_CONFIRM:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_TBF_RELEASE_CONF)
            {
              reset_tbf_parameters(gas_id);

              gmac_null_tbf_rel_confirm(gas_id);
            }
            else
            {

              if(geran_get_nv_recovery_restart_enabled(gas_id))
              {
                MSG_GERAN_ERROR_1_G("MAC error TBF REL CNF %d",global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);
                MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
              }
              else
              {
                ERR_GERAN_FATAL_2_G("MAC error TBF REL CNF %d",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);
              }

            }
          break;
          default:
        /* unknown signal from L1 */
            MSG_GERAN_ERROR_1_G("MAC unexpected signal from L1 %d",signal_id);
          break;
        }
      } /* end of L1_MAC signals */

      break;

    /************************************
    * null state NULL_UPLINK_TBF_ACCESS *
    *************************************/
    case NULL_UPLINK_TBF_ACCESS:
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            reset_tbf_parameters(gas_id);
            gmac_reset_rac_params(gas_id);

            gmac_util_block_rlc(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                GMAC_NULL,__LINE__,gas_id);

            /*IN this state L1 must have rcvd UL config so need to release L1    */
            /*wait in state for rel confirm to completed the releasing actions  */
            /*ie any PCCO or PCCF activities                                     */
            gmac_null_send_release_to_l1(GMAC_NULL_GRR_INITIATED_NO_PS_ACCESS,UL_TBF_ABNORMAL,gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_CELL_RESELECTION,gas_id);
            break;

          case GRR_MAC_DL_TBF_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DL_TBF_EST");

            /*Check for error in DL ass,if no error action the DL est*/
            if(gmac_null_dl_tbf_est_handler(&global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est,
                                            &null_data_ptr->gmac_null_mac_l1_dl_config,
                                            &null_data_ptr->gmac_null_mac_l1_freq_config,
                                             gas_id)  == TRUE)
            {

               MSG_GERAN_MED_0_G("GMAC info abort UL est, rel L1 and wait in state for rel confirm");
               gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

               /*release L1 and wait in state for l1 release confirmation */
               gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_UL_EST_INT_BY_DL_EST,UL_TBF_ABNORMAL,gas_id);
               mac_log_ul_tbf_release(MAC_LOG_UL_PDA_ABORT,gas_id);

               /*save the dl config*/
               null_data_ptr->temp_gm_dl_tbf_est = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est;

            }
            else
            {
              MSG_GERAN_HIGH_0_G("MAC info error in DL ass, Ignore IA, continue with UL est"); 
              gmac_util_send_grr_mac_status(MAC_IDLE_BLOCKED,gas_id);
            }
            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if (gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
            {
              gmac_util_start_pccf_process(SENDING_VIA_UL_PACCH,GMAC_NULL,__LINE__,(uint8)NULL_UPLINK_TBF_ESTAB,gas_id);
              MSG_GERAN_MED_0_G("MAC info PCCF SENT TO L1");
              gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NOT_NEEDED,gas_id);
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GMAC info Failed to CSN encode PCCF");
              /*This is an exception case in the formatting of PCCF which can't be recovered   *
              * from by trying to resent another PCCF. Don't inform GRR of the fialure to send *
              * PCCF wait for 73176 to expire at which point GRR will perform a reselection    */
            }
            break;


          case GRR_MAC_DL_SB_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DL_SB_EST");
            /*Check for error in DL ass,if no error action the DL est*/
            if (gmac_null_dl_sb_est_handler(&global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_sb_est,
                                            &null_data_ptr->gmac_null_mac_l1_dl_sb_config,
                                            &null_data_ptr->gmac_null_mac_l1_freq_config,
                                             gas_id)  == TRUE)
            {

              MSG_GERAN_MED_0_G("GMAC info abort UL est, rel L1 and wait in state for rel confirm");
              /*Block RLC*/
              gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

              /*release L1 and wait in state for l1 release confirmation */
              gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_UL_EST_INT_BY_DL_SB_EST,UL_TBF_ABNORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_PDA_ABORT,gas_id);

              /*save the dl sb config*/
              null_data_ptr->temp_gm_dl_sb_est = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_sb_est;
            }
            else
            {
             /*assignment error*/
             MSG_GERAN_HIGH_0_G("MAC info error DL SB in ass, Ignore IA continue with UL est"); 
             gmac_util_send_grr_mac_status(MAC_IDLE_BLOCKED,gas_id);
            }
            break;

          case GRR_MAC_RACH_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_RACH_REQ");
            gmac_util_send_grr_mac_rach_rej(gas_id);
            break;

          case GRR_MAC_UL_TBF_EST:
            MSG_GERAN_ERROR_0_G("GMAC INFO Not expecting second GRR_MAC_UL_TBF_EST in NULL_UPLINK_TBF_ACCESS");
            break;

          default:
            /* unknown signal from GRR */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from GRR %d",signal_id);
            break;
        }
      } /* end of GRR_MAC signals */
      else if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_FREQ_ASS_CNF, ignored for Freq Cnf Cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
            break;

          case MAC_PH_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");

            if(null_data_ptr->gmac_null_releasing_l1 == FALSE )
            {
              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_TBF_ESTABLISHED)
              {
                gmac_start_t3164(gas_id);

                gmac_util_send_mr_ul_alloc_cnf(&null_data_ptr->store_rlc_ul_alloc_data,gas_id);
                MSG_GERAN_MED_0_G("MAC Sent MAC_NI_UL_1P_ASS");
                global_data_ptr->mac_mac_sig.sig_id = MAC_NI_UL_1P_ASS;
                global_data_ptr->mac_mac_sig.task_id = MS_MAC_MAC;
                gprs_mac_mac_acc_func(&global_data_ptr->mac_mac_sig,gas_id);

                MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
                null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;

                global_data_ptr->mac_mode = IDLE_MODE;
                MSG_GERAN_MED_0_G("MAC mode IDLE");
                (void)mac_log_state(gas_id);
              }
              else
              {
                MSG_GERAN_HIGH_1_G("MAC error ABNORMAL REL connect fail tbf_type_ind= %d",
                                   global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);

                /*Block RLC*/
                gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

                gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,UL_TBF_ABNORMAL,gas_id);
              }

            }
            else
            {
              MSG_GERAN_HIGH_0_G("GMAC info Releasing L1, MAC_PH_CONNECT_IND ignored");
            }
            break;


          case L1_MAC_TBF_REL_CONFIRM:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");

            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_TBF_RELEASE_CONF)
            {
              reset_tbf_parameters(gas_id);

              gmac_null_tbf_rel_confirm(gas_id);
            }
            else
            {
              if(geran_get_nv_recovery_restart_enabled(gas_id))
              {
                MSG_GERAN_ERROR_1_G("MAC error TBF REL CNF %d",global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);
                MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
              }
              else
              {
                ERR_GERAN_FATAL_2_G("MAC error TBF REL CNF %d",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);
              }
            }
            break;

          default:
            /* unknown signal from L1 */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from L1 %d",signal_id);
            break;
        }
      } /* end of L1_MAC signals */
      break;

    /**********************************
    * null state NULL_UPLINK_SB_ESTAB *
    ***********************************/
    case NULL_UPLINK_SB_ESTAB:
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            reset_tbf_parameters(gas_id);
            gmac_reset_rac_params(gas_id);

            /*GRR initiated PS NO ACCESS block RLC irrespective if RLC was    */
            /*previously blocked by MAC because GRR wanted to send UL ctrl msg*/
            gmac_util_block_rlc(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                GMAC_NULL,__LINE__,gas_id);


            /*Indicate to GRR failure to send PMR                               */
            if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PMR)
            {
              gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
            }

            MSG_GERAN_MED_2_G("GMAC gmac_null_ul_sb_to_l1 %d gmac_null_releasing_l1 %d",
                              null_data_ptr->n_flags.gmac_null_ul_sb_to_l1,null_data_ptr->gmac_null_releasing_l1);
            if((null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 ==TRUE ) || (null_data_ptr->gmac_null_releasing_l1 == TRUE))
            {
              /*L1 has rcvd an UL SB config so will need releasing or L1 is releasing */
              /* and GRR_MAC_NO_PS_ACCESS_IND rcvd before rel confirm is rcvd         */
              null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 = FALSE;

              /*NOTE complete any PCCO activity and PCCF activitie will be      */
              /*completed on release confirm don't do it here                   */

              /*L1 has rcvd  config,release L1 and wait in state for l1 release confirmation */
              gmac_null_send_release_to_l1(GMAC_NULL_GRR_INITIATED_NO_PS_ACCESS,UL_TBF_ABNORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_CELL_RESELECTION,gas_id);
            }
            else
            {
              /*L1 has NOT rcvd any UL SB config so will NOT need releasing */

              /*send mac status before attempting to complete PCCO process*/
              gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

              /*complete any PCCO activity and PCCF activities*/
              gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,GRR_INITIATED,GMAC_NULL,__LINE__,gas_id);
              gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,GRR_INITIATED,GMAC_NULL,__LINE__,gas_id);

              MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
              null_data_ptr->mac_null_substate = NULL_INACTIVE;
              (void)mac_log_state(gas_id);
            }

            break;

          case GRR_MAC_UL_SB_EST:
            /*******************************************************************************
            *   Note that this signal is only for PMR and PCCF.  The GRR_MAC_UL_SB_EST for *
            *   PRR was already received in NULL_UPLINK_TBF_ESTAB.                             *
            *******************************************************************************/

            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_UL_SB_EST rcvd UL SB EST to send PMR/PCCF");

            switch ( null_data_ptr->gmac_null_ul_sb_type )
            {
              case NULL_UL_SB_TYPE_PCCF:
                if(gmac_null_handle_ul_sb_est_pccf(gas_id) == TRUE)
                {
                  /******* simulate Freq Conf being received ******************************/
                  gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
                  gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NOT_NEEDED,gas_id);
                  gmac_null_send_sb_config(gas_id);
                  null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 = TRUE;
                }
                else
                {
                  MSG_GERAN_HIGH_0_G("MAC info error in GRR_MAC_UL_SB_EST ass, abort estab");
                  null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_TBF_REL_NORMAL;
                  gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,UL_TBF_ABNORMAL,gas_id);
                  mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_DATA_ERROR,gas_id);
                }
              break;

              case NULL_UL_SB_TYPE_PMR:
                if(gmac_null_handle_ul_sb_est_pmr(gas_id) == TRUE)
                {
                  /******* simulate Freq Conf being received ******************************/
                  gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
                  gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NOT_NEEDED,gas_id);
                  gmac_null_send_sb_config(gas_id);
                  null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 = TRUE;
                }
                else
                {
                  /*ERROR in UL SB assigment, failed to send PMR*/
                  MSG_GERAN_HIGH_0_G("MAC info error in GRR_MAC_UL_SB_EST ass, abort estab");
                  gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                  null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_TBF_REL_NORMAL;
                  gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,UL_TBF_ABNORMAL,gas_id);
                  mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_DATA_ERROR,gas_id);
                }
              break;

              default:
                if(geran_get_nv_recovery_restart_enabled(gas_id))
                {
                  MSG_GERAN_ERROR_1_G("GMAC INFO unxpected gmac_null_ul_sb_type:0x%x",null_data_ptr->gmac_null_ul_sb_type);
                  MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                  geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                }
                else
                {
                  ERR_GERAN_FATAL_2_G("GMAC INFO unxpected gmac_null_ul_sb_type:0x%x",gas_id, null_data_ptr->gmac_null_ul_sb_type);
                }
              break;
            }
            break;


          case GRR_MAC_DL_SB_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DL_SB_EST");

            if (gmac_null_dl_sb_est_handler(&global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_sb_est,
                                            &null_data_ptr->gmac_null_mac_l1_dl_sb_config,
                                            &null_data_ptr->gmac_null_mac_l1_freq_config,
                                             gas_id)  == TRUE)
            {

              /* GSM 04.18 section 3.5.2.1.2:  If DL assignment rcvd during UL est */
              /* then abort UL TBF and continue with DL TBF                        */

              if(null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 == TRUE)
              {
                /*L1 has rcvd an UL SB config so will need releasing */
                null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 = FALSE;

                /*NOTE complete any PCCO activity and PCCF activitie will be      */
                /*completed on release confirm don't do it here                   */

                /*Indicate to GRR failure to send PMR                             */
                if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PMR)
                {
                  gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                  memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
                }

                /*For NULL_UL_SB_TYPE_PCCF and NULL_UL_SB_TYPE_PMR no RLC action  */
                /*needed as RLC will ALREADY have been blocked when request to    */
                /*send PMR/PCCF was rcvd.                                         */

                if ( null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PRR )
                {
                  /*abort UL data est and Block RLC. MAC initiated PS NO access to   */
                  /*perform DL SB operation. RLC will remain blocked till completion */
                  /*of DL SB operation                                               */
                  gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);
                }

                /*L1 has rcvd  config,release L1 and wait in state for l1 release confirmation */
                gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_UL_EST_INT_BY_DL_SB_EST,UL_TBF_ABNORMAL,gas_id);
                mac_log_ul_tbf_release(MAC_LOG_UL_PDA_ABORT,gas_id);

                /*save the DL SB config*/
                null_data_ptr->temp_gm_dl_sb_est = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_sb_est;
              }
              else
              {
                /*L1 has NOT rcvd any UL SB config so will NOT need releasing */

                /*For NULL_UL_SB_TYPE_PCCF and NULL_UL_SB_TYPE_PMR no RLC action  */
                /*needed as RLC will ALREADY have been blocked when request to    */
                /*send PMR/PCCF was rcvd.                                         */

                /*Indicate to GRR failure to send PMR/PCCF*/
                if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PCCF)
                {
                  gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_NULL,__LINE__,gas_id);
                }

                if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PMR)
                {
                  gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                  memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
                }

                if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PRR)
                {
                  /*If performing 2 phase access it's impossible to get into          */
                  /* NULL_UPLINK_SB_ESTAB state without having sent L1 a UL SB config */
                  if(geran_get_nv_recovery_restart_enabled(gas_id))
                  {
                    MSG_GERAN_ERROR_0_G("GMAC unexpected UL SB type NULL_UL_SB_TYPE_PRR");
                    MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                    geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                  }
                  else
                  {
                    ERR_GERAN_FATAL_0_G("GMAC unexpected UL SB type NULL_UL_SB_TYPE_PRR");
                  }
                }

                /*complete any PCCO activity */
                /*Note: switching from UL to DL no need to send Mac_status (idle_blocked) to GRR.*/
                /*MAC sends MAC_GRR_CELL_RESEL_STATUS as it initiated termination of PCCO.       */
                /*Continue with DL and wait for GRR to trigger the return to s-cell              */
                gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,MAC_INITIATED,GMAC_NULL,__LINE__,gas_id);

                /*Send L1 DL SB config*/
                gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
                gmac_send_mac_l1_dl_sb_config(&null_data_ptr->gmac_null_mac_l1_dl_sb_config,gas_id);

                MSG_GERAN_MED_0_G("GMAC State null NULL_DOWNLINK_SB_ESTAB");
                null_data_ptr->mac_null_substate = NULL_DOWNLINK_SB_ESTAB;
                (void)mac_log_state(gas_id);
              }
            }
            else
            {
             /*assignment error*/
              MSG_GERAN_HIGH_0_G("MAC info error in GRR_MAC_DL_SB_EST, Abort Est");

              if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PMR)
              {
                gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
              }

              if ( null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PRR )
              {
                gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);
              }

              null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_TBF_REL_NORMAL;

              gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,UL_TBF_ABNORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_DATA_ERROR,gas_id);
            }

            break;

          case GRR_MAC_DL_TBF_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DL_TBF_EST");

            if(gmac_null_dl_tbf_est_handler(&global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est,
                                            &null_data_ptr->gmac_null_mac_l1_dl_config,
                                            &null_data_ptr->gmac_null_mac_l1_freq_config,
                                             gas_id)  == TRUE)
            {
              if(null_data_ptr->gmac_null_releasing_l1 == TRUE )
              {
                /* Rcvd a subsequent DL assignment before L1 could respond to the release*/
                /* confirm overwrite the previously save DL assignment and wait for rel  */
                /* confirm before sending L1 dl config save the DL config                */
                null_data_ptr->temp_gm_dl_tbf_est = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est;
              }
              else
              {  
                if(null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 == TRUE)
                {
                  /*L1 has rcvd an UL SB config so will need releasing */
                  null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 = FALSE;
  
                  /*NOTE complete any PCCO activity and PCCF activitie will be      */
                  /*completed on release confirm don't do it here                   */
  
                  /*Indicate to GRR failure to send PMR                             */
                  if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PMR)
                  {
                    gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                    memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
                  }
  
                  /*For NULL_UL_SB_TYPE_PCCF and NULL_UL_SB_TYPE_PMR no RLC action  */
                  /*needed as RLC will ALREADY have been blocked when request to    */
                  /*send PMR/PCCF was rcvd.                                         */
  
                  if ( null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PRR )
                  {
                    /*abort UL data est and Block RLC. MAC initiated PS NO access to   */
                    /*perform DL SB operation. RLC will remain blocked till completion */
                    /*of DL SB operation                                               */
                    gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);
                  }
  
                  /*L1 has rcvd  config,release L1 and wait in state for l1 release confirmation */
                  gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_UL_EST_INT_BY_DL_EST,UL_TBF_ABNORMAL,gas_id);
                  mac_log_ul_tbf_release(MAC_LOG_UL_PDA_ABORT,gas_id);
  
                  /*save the DL config*/
                  null_data_ptr->temp_gm_dl_tbf_est = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est;
                }
                else
                {
                  /*L1 has NOT rcvd any UL SB config so will NOT need releasing */
  
                  /*For NULL_UL_SB_TYPE_PCCF and NULL_UL_SB_TYPE_PMR no RLC action  */
                  /*needed as RLC will ALREADY have been blocked when request to    */
                  /*send PMR/PCCF was rcvd.                                         */
  
                  /*Indicate to GRR failure to send PMR/PCCF*/
                  if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PCCF)
                  {
                    gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,GRR_INITIATED,GMAC_NULL,__LINE__,gas_id);
                  }
  
                  if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PMR)
                  {
                    gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                    memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
                  }
  
                  if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PRR)
                  {
                    /*If performing 2 phase access it's impossible to get into          */
                    /* NULL_UPLINK_SB_ESTAB state without having sent L1 a UL SB config */
                    if(geran_get_nv_recovery_restart_enabled(gas_id))
                    {
                      MSG_GERAN_ERROR_0_G("GMAC unexpected UL SB type NULL_UL_SB_TYPE_PRR");
                      MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                      geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                    }
                    else
                    {
                      ERR_GERAN_FATAL_0_G("GMAC unexpected UL SB type NULL_UL_SB_TYPE_PRR");
                    }
                  }
  
                  /*complete any PCCO activity */
                  /*Note: switching from UL to DL no need to send Mac_status (idle_blocked) to GRR.*/
                  /*MAC sends MAC_GRR_CELL_RESEL_STATUS as it initiated termination of PCCO.       */
                  /*Continue with DL and wait for GRR to trigger the return to s-cell              */
                  gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,MAC_INITIATED,GMAC_NULL,__LINE__,gas_id);
  
                  /* Send L1 DL config wait for starting time to occur before */
                  /* configuring RLC dl tbf                                   */
                  gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
                  gmac_send_mac_l1_dl_config(&null_data_ptr->gmac_null_mac_l1_dl_config,gas_id);
                  gmac_null_check_for_polled_dl_ass(gas_id);
  
                  MSG_GERAN_MED_0_G("GMAC State null NULL_DL_TBF_EST");
                  null_data_ptr->mac_null_substate = NULL_DL_TBF_EST;
                  (void)mac_log_state(gas_id);
                }
              }
            }
            else
            {
             /*assignment error*/
              MSG_GERAN_HIGH_0_G("MAC info error in GRR_MAC_DL_TBF_EST, Abort Est");

              if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PMR)
              {
                gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
              }

              if ( null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PRR )
              {
                gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);
              }

              null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_TBF_REL_NORMAL;
              gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,UL_TBF_ABNORMAL,gas_id);
              mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_DATA_ERROR,gas_id);
            }
            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_SEND_PCCF");
            if (gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
            {

              if ( null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PRR )
              {
                gmac_util_start_pccf_process(SENDING_VIA_UL_PACCH,GMAC_NULL,__LINE__,(uint8)NULL_UPLINK_TBF_ESTAB,gas_id);
              }

              /* PCCF has priority over PMR */
              if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PMR)
              {
                gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));

#ifdef FEATURE_GSM_EDTM
                /*Remove PMR form L1's UL cntrl msg queue*/
                gmac_send_mac_l1_delete_ul_ctrl_msg(PACKET_MEASUREMENT_REPORT,gas_id);
#endif /*FEATURE_GSM_EDTM*/

                /*Use PMR's UL SB assigment to send PCCF*/
                gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NOT_NEEDED,gas_id);
              }
            }
            else
            {
              MSG_GERAN_ERROR_0_G("GMAC info Failed to CSN encode PCCF");
              /*This is an exception case in the formatting of PCCF which can't be recovered   *
              * from by trying to resent another PCCF. Don't inform GRR of the fialure to send *
              * PCCF wait for 73176 to expire at which point GRR will perform a reselection    */
            }
            break;

          case GRR_MAC_RACH_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_RACH_REQ");
            gmac_util_send_grr_mac_rach_cnf(gas_id);
            break;

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_0_G("GMAC Rejected GRR_MAC_EDTM_PKT_CS_EST_REQ, Not in Contention Resolution");
            gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_CANNOT_SERVICE_REQUEST,gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/


          case GRR_MAC_SEND_MEAS_RPT:

            /*race condition: when Mac moves out of NULL_GPRS_51_ACTIVE due to DL,DL SB or UL est*/
            /*it sends MAC_status to RR. The assumption is that this will be the next sig in     */
            /*RR's q, hence blocking any attempt to send PMR's. IF this is not the case and T3158*/
            /*expiry is next sig in RR q then MAC can receive GRR_MAC_SEND_MEAS_RPT in the       */
            /*establishment states        */

            MSG_GERAN_MED_0_G("GMAC RCVD MAC Rcvd GRR_MAC_SEND_MEAS_RPT while est UL: fail to send PMR");
            gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
            break;

          case GRR_MAC_UL_TBF_EST:
            /*******************************************************************************
            * Note: GMAC does not expect GRR_MAC_UL_TBF_EST message while waiting for UL   *
            * single block assignment from the network. As per 3GPP 44.060 Section 7.3.2   *
            * "The network shall then respond with either an IMMEDIATE ASSIGNMENT message  *
            * granting a "single block access" on a PDCH or an IMMEDIATE ASSIGNMENT REJECT *
            * message that this signal is only for PMR and PCCF.                           *
            *                                                                              *
            * This additional handling has been added to send MAC_L1_RELEASE to GL1 so that*
            * it can move out of RANDOM ACCESS mode                                        *
            *******************************************************************************/

            MSG_GERAN_ERROR_0_G("GMAC Rcvd unexpected GRR_MAC_UL_TBF_EST");

            switch (null_data_ptr->gmac_null_ul_sb_type)
            {
              case NULL_UL_SB_TYPE_PMR:
                gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_TBF_REL_NORMAL;
                gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,UL_TBF_ABNORMAL,gas_id);
                mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_DATA_ERROR,gas_id);
              break;

              case NULL_UL_SB_TYPE_PCCF:
                null_data_ptr->gmac_null_tbf_rel_cause = GMAC_NULL_TBF_REL_NORMAL;
                gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,UL_TBF_ABNORMAL,gas_id);
                mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_DATA_ERROR,gas_id);
              break;

              default:
                MSG_GERAN_ERROR_0_G("GMAC ignored GRR_MAC_UL_TBF_EST");
              break;
            }
            break;

          default:
            /* unknown signal from GRR */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from GRR %d",signal_id);
            break;
        }
      } /* end of GRR_MAC signals */
      else if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_FREQ_ASS_CNF rxd ignored for Freq Cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
            break;

          case L1_MAC_QUAL_MEASURMENTS:
            if(null_data_ptr->gmac_null_releasing_l1 == FALSE )
            {
              /*still performing 1st half of 2 phase process at this stage*/
              if(global_data_ptr->egprs_2p_sb_mb_alloc == GPRS_SB_ALLOC)
              {
                gmac_send_prr(global_data_ptr->idle_mode_ul_tbf_release_action,FALSE,gas_id);
              }
              else
              {
                gmac_idle_egprs_rac_procedure_2(TRUE,global_data_ptr->c_flags.egprs_rac_retran_req,FALSE,FALSE,gas_id);
              }
            }
            else
            {
              /*Releasing L1 no action wait for L1_MAC_TBF_REL_CONFIRM*/
              MSG_GERAN_HIGH_0_G("GMAC Info L1_MAC_QUAL_MEASURMENTS, No action wait for L1_MAC_TBF_REL_CONFIRM");
            }
          break;

          case MAC_PH_CONNECT_IND:
            if(null_data_ptr->gmac_null_releasing_l1 == FALSE )
            {
              if ( null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PRR )
              {
                MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");
                global_data_ptr->mac_mac_sig.sig_id = MAC_NI_UL_2P_ASS;
                global_data_ptr->mac_mac_sig.task_id = MS_MAC_MAC;
                gprs_mac_mac_acc_func(&global_data_ptr->mac_mac_sig,gas_id);

                MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
                MSG_GERAN_MED_0_G("MAC mode IDLE");
                null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
                global_data_ptr->mac_mode = IDLE_MODE;
                (void)mac_log_state(gas_id);
              }
              else
              {
                /*WHen sending PMR/PCCF no action wait for L1_MAC_SINGLE_BLOCK_SENT*/
                MSG_GERAN_HIGH_0_G("GMAC INFO MAC_PH_CONNECT_IND,No action wait for L1_MAC_SINGLE_BLOCK_SENT");
              }
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GMAC info Releasing L1, MAC_PH_CONNECT_IND ignored");
            }
            break;

          case L1_MAC_SINGLE_BLOCK_SENT:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_SINGLE_BLOCK_SENT");
            /*only applicable if UL SB est is to send PCCF or PMR*/

            if(null_data_ptr->gmac_null_releasing_l1 == FALSE)
            {
              null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 = FALSE;

              if (null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PCCF)
              {
                gmac_util_complete_pccf_process(SUCCESSFULLY_SENT_PCCF,MAC_INITIATED,GMAC_NULL,__LINE__,gas_id);
              }
              else
              {
                gmac_util_send_pmr_cnf_to_grr(MAC_GRR_SUCCESS,gas_id);
                memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
              }

              /*Unblock RLC*/
              gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_NULL,__LINE__,gas_id);

              gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);
              MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
              null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
              (void)mac_log_state(gas_id);
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GMAC info Releasing L1, L1_MAC_SINGLE_BLOCK_SENT ignored");
            }
            break;

          case L1_MAC_51_SB_FAILURE:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_51_SB_FAILURE");

            null_data_ptr->n_flags.gmac_null_ul_sb_to_l1 = FALSE;

            if (null_data_ptr->gmac_null_tbf_rel_cause == GMAC_NULL_NO_RELEASE_IN_PROGRESS)
            {

              if( null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PCCF)
              {
                gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_NULL,__LINE__,gas_id);
              }

              if( null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PMR)
              {
                gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
              }

              if( null_data_ptr->gmac_null_ul_sb_type == NULL_UL_SB_TYPE_PRR)
              {
                MSG_GERAN_HIGH_0_G("GMAC info L1_MAC_51_SB_FAILURE during 2p access");
                /*Originally an ACC_REJ_NO_WAIT_INDICATION. So need to BLOCK RLC*/
                /*followed immediately with an UNBLOCKING                       */
                gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);
              }

              /*Unblock RLC*/
              gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_NULL,__LINE__,gas_id);

              gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

              reset_tbf_parameters(gas_id);
              gmac_reset_rac_params(gas_id);

              MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
              null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
              (void)mac_log_state(gas_id);
            }
            else
            {
              MSG_MED("GMAC releasing L1, so ignoring L1_MAC_51_SB_FAILURE signal",0,0,0);  
            }
            break;

          case L1_MAC_TBF_REL_CONFIRM:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");
            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == UL_TBF_RELEASE_CONF)
            {
              gmac_null_tbf_rel_confirm(gas_id);
            }
            else
            {
              if(geran_get_nv_recovery_restart_enabled(gas_id))
              {
                MSG_GERAN_ERROR_1_G("MAC error TBF REL CNF %d",global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);
                MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
              }
              else
              {
                ERR_GERAN_FATAL_2_G("MAC error TBF REL CNF %d",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);
              }
            }
            break;

          default:
            /* unknown signal from L1 */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from L1 %d",signal_id);
            break;
        }
      } /* end of L1_MAC signals */
      else if (task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {

          case RM_UL_EST_TBF_REQ:
            /*Case where GRR request to est UL SB, MAC sends NO PS ACCESS blocking UL */
            /*est requests but the next signal in MAC's Q is RM_UL_EST_TBF_REQ, as MAC*/
            /*has already sent RLC NO PS ACCESS, MAC will ignore this request and     */
            /*unblock RLC when the UL SB est has completed                            */
            MSG_GERAN_MED_0_G("GMAC Info Rcvd RM_UL_EST_TBF_REQ race condition sig Ignored");
            (void) gmac_util_is_rlc_blocked(GMAC_NULL,__LINE__,gas_id);
            break;

          default:
            /* unknown signal from RLC */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from RLC %d",signal_id);
            break;
        }
      } /* end of RLC_MAC signals */
      break;

    /************************************
    * null state NULL_DOWNLINK_SB_ESTAB *
    *************************************/
    case NULL_DOWNLINK_SB_ESTAB:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("MAC rvcd L1_MAC_FREQ_ASS_CNF, no action");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
            break;

          case MAC_PH_CONNECT_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");
            if (null_data_ptr->gmac_null_releasing_l1 == FALSE)
            {
              /*Switch to IDLE mode to receive DL crtl msg eg PMO*/
              MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
              null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
              MSG_GERAN_MED_0_G("MAC set mode IDLE_MODE");
              global_data_ptr->mac_mode = IDLE_MODE;

              global_data_ptr->c_flags.waiting_for_dl_sb_control_msg = TRUE;

            }
            else
            {
              MSG_GERAN_MED_0_G("GMAC Ignore PH_CONNECT_IND, releasing in NULL_DOWNLINK_SB_ESTAB state");
            }
            break;

          case L1_MAC_51_SB_FAILURE:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_51_SB_FAILURE");

            if (null_data_ptr->gmac_null_tbf_rel_cause == GMAC_NULL_NO_RELEASE_IN_PROGRESS)
            {


              /*Unblock RLC*/
              gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_NULL,__LINE__,gas_id);
              gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

              reset_tbf_parameters(gas_id);
              gmac_reset_rac_params(gas_id);

              MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
              null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
              null_data_ptr->gmac_null_releasing_l1 = FALSE;
              (void)mac_log_state(gas_id);

            }
            else
            {
              MSG_MED("GMAC releasing L1, so ignoring L1_MAC_51_SB_FAILURE signal",0,0,0); 
            }
            break;

          case L1_MAC_TBF_REL_CONFIRM:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");
            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
            {
              gmac_null_tbf_rel_confirm(gas_id);
            }
            else
            {
              MSG_GERAN_HIGH_1_G(" **** MAC info L1 rel cnf for UL, absorb %d *****",
                                 global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);
            }
            break;

          default:
            /* unknown signal from L1 */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from L1 %d",signal_id);
            break;
        }
      } /* end of MAC_L1 signals */
      else if (task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {

          case RM_UL_EST_TBF_REQ:
            /*Case where GRR request to est DL SB, MAC sends NO PS ACCESS blocking UL */
            /*est requests but the next signal in MAC's Q is RM_UL_EST_TBF_REQ, as MAC*/
            /*has already sent RLC NO PS ACCESS, MAC will ignore this request and     */
            /*unblock RLC when the DL SB est has completed                            */
            MSG_GERAN_MED_0_G("GMAC Info Rcvd RM_UL_EST_TBF_REQ race condition sig Ignored");
            (void) gmac_util_is_rlc_blocked(GMAC_NULL,__LINE__,gas_id);
            break;

          default:
            /* unknown internal signal from RLC */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from RLC %d",signal_id);
            break;
        }
      } /* end of RLC_MAC signals */
      else if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            reset_tbf_parameters(gas_id);
            gmac_reset_rac_params(gas_id);

            /*GRR initiated PS NO ACCESS block RLC irrespective if RLC was    */
            /*previously blocked by MAC because GRR wanted to send UL ctrl msg*/
            gmac_util_block_rlc(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                GMAC_NULL,__LINE__,gas_id);

            /*NOTE complete any PCCO activity and PCCF activitie will be      */
            /*completed on release confirm don't do it here                   */

            /*L1 has rcvd DL SB config,release L1 and wait in state for l1 release confirmation */
            gmac_null_send_release_to_l1(GMAC_NULL_GRR_INITIATED_NO_PS_ACCESS,DL_TBF_ABNORMAL,gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_CELL_RESELECTION,gas_id);
            break;

          case GRR_MAC_DL_TBF_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DL_TBF_EST");
            if(gmac_null_dl_tbf_est_handler(&global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est,
                                            &null_data_ptr->gmac_null_mac_l1_dl_config,
                                            &null_data_ptr->gmac_null_mac_l1_freq_config,
                                            gas_id)  == TRUE)
            {
             /* RLC will already be block as a result of performing DL SB*/
             gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
             gmac_send_mac_l1_dl_config(&null_data_ptr->gmac_null_mac_l1_dl_config,gas_id);
             MSG_GERAN_MED_0_G("GMAC State null NULL_DL_TBF_EST");
             gmac_null_check_for_polled_dl_ass(gas_id);
             null_data_ptr->mac_null_substate = NULL_DL_TBF_EST;
            }
            else
            {
             /*assignment error*/
             MSG_GERAN_HIGH_0_G("MAC info error in DL ass, Ignore IA"); 
             gmac_util_send_grr_mac_status(MAC_IDLE_BLOCKED,gas_id);
            }
            break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_HIGH_0_G("GMAC Unexpected GRR_MAC_SEND_PCCF while performing DL SB est. Sig Ignored");
            /*If PCCO fails, t3176 is started and MS has 15sec to return to old S cell. *
            * In this time N/W does not know were the MS is. ON reception of PCCF or    *
            * expiry of t3176 can the N/W make any assumptions on the MS position hence *
            * no DL SB est request be rcvd                                              */
            break;

          case GRR_MAC_RACH_REQ:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_RACH_REQ");
            gmac_util_send_grr_mac_rach_rej(gas_id);
            break;

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_0_G("GMAC Rejected GRR_MAC_EDTM_PKT_CS_EST_REQ, Not in Contention Resolution");
            gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_CANNOT_SERVICE_REQUEST,gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          case GRR_MAC_SEND_MEAS_RPT:

            /*race condition: when Mac moves out of NULL_GPRS_51_ACTIVE due to DL,DL SB or UL est*/
            /*it sends MAC_status to RR. The assumption is that this will be the next sig in     */
            /*RR's q, hence blocking any attempt to send PMR's. IF this is not the case and T3158*/
            /*expiry is next sig in RR q then MAC can receive GRR_MAC_SEND_MEAS_RPT in the       */
            /*establishment states        */

            MSG_GERAN_MED_0_G("GMAC RCVD MAC Rcvd GRR_MAC_SEND_MEAS_RPT while est UL: fail to send PMR");
            gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
            break;

          default:
            /* unknown signal from GRR */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from GRR %d",signal_id);
            break;
        }
      } /* end of GRR_MAC signals */
      break;

    /***************************************
    *      null state NULL_DL_TBF_EST      *
    ***************************************/

    case NULL_DL_TBF_EST:

      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case MAC_PH_CONNECT_IND:

            if (null_data_ptr->gmac_null_releasing_l1 == FALSE)
            {
              MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");
#ifdef FEATURE_DUAL_SIM
              if (global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.connect_status == MAC_L1_CON_STAT_TRM_LOCK_FAIL_DL)
              {
                MSG_GERAN_HIGH_0_G("GMAC DS TRM Lock Fail for DL connect");
                reset_tbf_parameters(gas_id);
                gmac_reset_rac_params(gas_id);

                /*NOTE complete any PCCO activity and PCCF activitie will be      */
                /*completed on release confirm don't do it here                   */

                /*L1 has rcvd DL config,release L1 and wait in state for l1 release confirmation */
                gmac_null_send_release_to_l1(GMAC_NULL_TBF_REL_NORMAL,DL_TBF_ABNORMAL,gas_id);
                mac_log_ul_tbf_release(MAC_LOG_UL_PDA_ABORT,gas_id);
              }
              else
#endif /* FEATURE_DUAL_SIM */
              {
                gmac_util_send_dl_ass_to_rlc(gas_id);

                MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
                null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
                (void)mac_log_state(gas_id);

                MSG_GERAN_MED_0_G("MAC Sent MAC_NT_DL_DYNAMIC ");
                global_data_ptr->mac_mac_sig.sig_id = MAC_NT_DL_DYNAMIC;
                global_data_ptr->mac_mac_sig.task_id = MS_MAC_MAC;
                gprs_mac_mac_acc_func(&global_data_ptr->mac_mac_sig,gas_id);
                gmac_util_send_grr_mac_status(MAC_TRANSFER,gas_id);

                MSG_GERAN_HIGH_1_G( "MAC set MAC_MODE : %d", global_data_ptr->mac_mode);
                global_data_ptr->mac_mode = TRANSFER_MODE;
              }
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GMAC Ignore MAC_PH_CONNECT_IND rxd whilst releasing in NULL_DL_TBF_EST");
            }
            break;

          case L1_MAC_TBF_REL_CONFIRM:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_TBF_REL_CONFIRM");
            if (global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released == DL_TBF_RELEASE_CONF)
            {
              gmac_null_tbf_rel_confirm(gas_id);
            }
            else
            {
              if(geran_get_nv_recovery_restart_enabled(gas_id))
              {
                MSG_GERAN_ERROR_1_G("MAC error TBF REL CNF %d",global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);
                MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
              }
              else
              {
                ERR_GERAN_FATAL_2_G("MAC error TBF REL CNF %d",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_released.tbf_released);
              }
            }
            break;

          case L1_MAC_FREQ_ASS_CNF:
            MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_FREQ_ASS_CNF, ignored for Freq Cnf Cleanup");

            if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
            {
              MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
            }
            break;

          default:
            /* unknown signal from L1 */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from L1 %d",signal_id);
            break;
        }
      } /* end of MAC_L1 signals */
      else if (task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {

          case RM_UL_EST_TBF_REQ:
            /*Case where GRR request to est DL , MAC sends NO PS ACCESS blocking UL   */
            /*est requests but the next signal in MAC's Q is RM_UL_EST_TBF_REQ, as MAC*/
            /*has already sent RLC NO PS ACCESS, MAC will ignore this request and     */
            /*unblock RLC when the DL SB est has completed                            */
            MSG_GERAN_MED_0_G("GMAC Info Rcvd RM_UL_EST_TBF_REQ race condition sig Ignored");
            (void) gmac_util_is_rlc_blocked(GMAC_NULL,__LINE__,gas_id);
            break;

          default:
            /* unknown internal signal from RLC */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from RLC %d",signal_id);
            break;
        }
      } /* end of RLC_MAC signals */
      else if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

            reset_tbf_parameters(gas_id);
            gmac_reset_rac_params(gas_id);

            /*GRR initiated PS NO ACCESS block RLC irrespective if RLC was    */
            /*previously blocked by MAC because GRR wanted to send UL ctrl msg*/
            gmac_util_block_rlc(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                GMAC_NULL,__LINE__,gas_id);

            /*NOTE complete any PCCO activity and PCCF activitie will be      */
            /*completed on release confirm don't do it here                   */

            /*L1 has rcvd DL config,release L1 and wait in state for l1 release confirmation */
            gmac_null_send_release_to_l1(GMAC_NULL_GRR_INITIATED_NO_PS_ACCESS,DL_TBF_ABNORMAL,gas_id);
            mac_log_ul_tbf_release(MAC_LOG_UL_CELL_RESELECTION,gas_id);
            break;

          case GRR_MAC_DL_TBF_EST:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DL_TBF_EST");

            if(gmac_null_dl_tbf_est_handler(&global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est,
                                            &null_data_ptr->gmac_null_mac_l1_dl_config,
                                            &null_data_ptr->gmac_null_mac_l1_freq_config,
                                             gas_id)  == TRUE)
            {
             /* wait for starting time to occur before configuring RLC dl tbf */
             gmac_send_mac_l1_freq_config(&null_data_ptr->gmac_null_mac_l1_freq_config,gas_id);
             gmac_send_mac_l1_dl_config(&null_data_ptr->gmac_null_mac_l1_dl_config,gas_id);
             MSG_GERAN_MED_0_G("GMAC State null NULL_DL_TBF_EST");
             gmac_null_check_for_polled_dl_ass(gas_id);
             null_data_ptr->mac_null_substate = NULL_DL_TBF_EST;
            }
            else
            {
             /*assignment error*/
             MSG_GERAN_HIGH_0_G("MAC info error in DL ass, Ignore IA"); 
             gmac_util_send_grr_mac_status(MAC_IDLE_BLOCKED,gas_id);
            }
            break;

#ifdef FEATURE_GSM_EDTM
          case GRR_MAC_EDTM_PKT_CS_EST_REQ:
            MSG_GERAN_MED_0_G("GMAC Rejected GRR_MAC_EDTM_PKT_CS_EST_REQ, Not in Contention Resolution");
            gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_CANNOT_SERVICE_REQUEST,gas_id);
          break;
#endif /*FEATURE_GSM_EDTM*/

          case GRR_MAC_RACH_REQ:
            /* Prevent GRR performing RACH for UL whilst performing DL TBF EST */
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_RACH_REQ");
            gmac_util_send_grr_mac_rach_rej(gas_id);
          break;

          case GRR_MAC_SEND_PCCF:
            MSG_GERAN_HIGH_0_G("GMAC Unexpected GRR_MAC_SEND_PCCF while performing DL SB est. Sig Ignored");
            /*If PCCO fails, t3176 is started and MS has 15sec to return to old S cell. *
            * In this time N/W does not know were the MS is. ON reception of PCCF or    *
            * expiry of t3176 can the N/W make any assumptions on the MS position hence *
            * no DL SB est request be rcvd                                              */
            break;

          case GRR_MAC_SEND_MEAS_RPT:

            /*race condition: when Mac moves out of NULL_GPRS_51_ACTIVE due to DL,DL SB or UL est*/
            /*it sends MAC_status to RR. The assumption is that this will be the next sig in     */
            /*RR's q, hence blocking any attempt to send PMR's. IF this is not the case and T3158*/
            /*expiry is next sig in RR q then MAC can receive GRR_MAC_SEND_MEAS_RPT in the       */
            /*establishment states        */

            MSG_GERAN_MED_0_G("GMAC RCVD MAC Rcvd GRR_MAC_SEND_MEAS_RPT while est UL: fail to send PMR");
            gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
            break;

          default:
            /* unknown signal from GRR */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from GRR %d",signal_id);
            break;
        }
      } /* end of GRR_MAC signals */
    break;

#ifdef  FEATURE_GSM_DTM
    /***************************************
    * null state NULL_DTM_CONNECT_PENDING *
    ***************************************/

    case NULL_DTM_CONNECT_PENDING:
      if(task_id == MS_MAC_L1)
      {
        switch (signal_id)
        {
          case L1_MAC_FREQ_ASS_CNF:
               /*NO action*/
               MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_FREQ_ASS_CNF");
            break;

          case MAC_PH_CONNECT_IND:
               MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CONNECT_IND");

               if(global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_TBF_ESTABLISHED)
               {
                MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
                null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
                (void)mac_log_state(gas_id);

                MSG_GERAN_MED_0_G("MAC Sent MAC_NT_UL_DTM_DYNAMIC ");
                global_data_ptr->mac_mac_sig.sig_id = MAC_NT_UL_DTM_DYNAMIC;
                global_data_ptr->mac_mac_sig.task_id = MS_MAC_MAC;
                gprs_mac_mac_acc_func(&global_data_ptr->mac_mac_sig,gas_id);
                gmac_util_send_grr_mac_status(MAC_TRANSFER,gas_id);

                global_data_ptr->mac_mode = TRANSFER_MODE;
                MSG_GERAN_HIGH_1_G( "MAC set MAC_MODE : %d", global_data_ptr->mac_mode);

                null_data_ptr->gmac_null_dtm_tbf_est_type = NO_TBF_PRESENT;

               }
               else if(global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == DOWNLINK_TBF_ESTABLISHED)
               {
                gmac_util_send_dl_ass_to_rlc(gas_id);

                MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
                null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
                (void)mac_log_state(gas_id);

                MSG_GERAN_MED_0_G("MAC Sent MAC_NT_DL_DTM_DYNAMIC ");
                global_data_ptr->mac_mac_sig.sig_id = MAC_NT_DL_DTM_DYNAMIC;
                global_data_ptr->mac_mac_sig.task_id = MS_MAC_MAC;
                gprs_mac_mac_acc_func(&global_data_ptr->mac_mac_sig,gas_id);
                gmac_util_send_grr_mac_status(MAC_TRANSFER,gas_id);

                global_data_ptr->mac_mode = TRANSFER_MODE;
                MSG_GERAN_HIGH_1_G( "MAC set MAC_MODE : %d", global_data_ptr->mac_mode);

                null_data_ptr->gmac_null_dtm_tbf_est_type = NO_TBF_PRESENT;
               }
               else if(global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind == UPLINK_DOWNLINK_TBF_ESTABLISHED)
               {
                 gmac_util_send_dl_ass_to_rlc(gas_id);

                 MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
                 null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
                 (void)mac_log_state(gas_id);

                 MSG_GERAN_MED_0_G("MAC Sent MAC_NT_UL_DL_DTM_DYNAMIC ");
                 global_data_ptr->mac_mac_sig.sig_id = MAC_NT_UL_DL_DTM_DYNAMIC;
                 global_data_ptr->mac_mac_sig.task_id = MS_MAC_MAC;
                 gprs_mac_mac_acc_func(&global_data_ptr->mac_mac_sig,gas_id);
                 gmac_util_send_grr_mac_status(MAC_TRANSFER,gas_id);

                 global_data_ptr->mac_mode = TRANSFER_MODE;
                 MSG_GERAN_HIGH_1_G( "MAC set MAC_MODE : %d", global_data_ptr->mac_mode);

                 null_data_ptr->gmac_null_dtm_tbf_est_type = NO_TBF_PRESENT;
               }
               else
               {
                 if(geran_get_nv_recovery_restart_enabled(gas_id))
                 {
                   MSG_GERAN_ERROR_1_G("MAC info Unexpected connection type %d ",
                                       global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                   MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
                   geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
                 }
                 else
                 {
                   ERR_GERAN_FATAL_2_G("MAC info Unexpected connection type %d ",gas_id, global_data_ptr->msg_ptr->l1_mac_sig.msg.connect_ind.tbf_type_ind);
                 }
               }
            break;

#ifdef FEATURE_GSM_EDTM
          case MAC_PH_CS_CONNECT_IND:
            /*Entry into TRANSFER and dual transfer mode is via PH_CONNECT_IND for establishment from NULL*/
            MSG_GERAN_MED_0_G("MAC Rcvd MAC_PH_CS_CONNECT_IND, no action for DTM estab from NULL");
          break;
#endif /*FEATURE_GSM_EDTM*/

          default:
            /* unknown signal from L1 */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from L1 %d",signal_id);
            break;
        }
      } /* end of MAC_L1 signals */
      else if (task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {

          case RM_UL_EST_TBF_REQ:
            /*Case where GRR request to est DTM ASS for DL, MAC sends NO PS ACCESS blocking*/
            /*UL est requests but the next signal in MAC's Q is RM_UL_EST_TBF_REQ, as MAC  */
            /*has already sent RLC NO PS ACCESS, MAC will ignore this request and          */
            /*unblock RLC when the DL  est has completed                                   */
            MSG_GERAN_MED_0_G("GMAC Info Rcvd RM_UL_EST_TBF_REQ race condition sig Ignored");
            (void) gmac_util_is_rlc_blocked(GMAC_NULL,__LINE__,gas_id);
            break;

          default:
            /* unknown internal signal from RLC */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from RLC %d",signal_id);
            break;
        }
      } /* end of RLC_MAC signals */
      else if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_ERROR_0_G("GMAC Unexpedted GRR_MAC_NO_PS_ACCESS_IND during DTM est");
            break;

          case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
            /*Only relevant for the case where MAC has previously rcvd RR packet */
            /*assignment Msg from the network                                    */
            break;

          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_1_G("MAC rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND rel cause %d",
                              global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause);

            /* Note for DTM operation GRR always release L1, The MAC/l1 MAC_L1_RELEASE*/
            /* L1_MAC_TBF_REL_CONFIRM confirm hand shake is not performed             */

            /*DTM GRR_MAC_DTM_CHANNEL_RELEASE_IND indicates to MAC that GRR is in CS */
            /*operation only and hence translates to NO_PS_ACCESS, block RLC and wait*/
            /*for GRR_MAC_PS_ACCESS_REQ at which point RLC will be unblocked         */


            if(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release == FALSE)
            {
              /* NOTE for EDTM, "dtm_ps_release" set to false indicates that the CS   */
              /* connection has terminated and that the PS connection is to continue. */
              /* not applicable to establishment phase of EDTM as no PS connected yet */
              MSG_GERAN_ERROR_1_G("GMAC unepxected. dtm_ps_release %d ",
                                  global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dtm_channel_release_ind.dtm_ps_release);
            }

            reset_tbf_parameters(gas_id);
            gmac_reset_rac_params(gas_id);

            gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

            gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

            MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
            null_data_ptr->mac_null_substate = NULL_INACTIVE;
            (void)mac_log_state(gas_id);

            null_data_ptr->gmac_null_dtm_tbf_est_type = NO_TBF_PRESENT;

            break;

          default:
            /* unknown signal from GRR */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from GRR %d",signal_id);
            break;
        }
      } /* end of GRR_MAC signals */

      break;

    /************************************
    * null state NULL_DTM_GTTP_UL_CON  *
    ************************************/

    case NULL_DTM_GTTP_UL_CON:
      if(task_id == MS_MAC_RR)
      {
        switch (signal_id)
        {

          case GRR_MAC_NO_PS_ACCESS_IND:
            MSG_GERAN_ERROR_0_G("GMAC Unexpedted GRR_MAC_NO_PS_ACCESS_IND during DTM est");
            break;

          case GRR_MAC_DTM_UL_DCCH_CNF:
            MSG_GERAN_MED_0_G("MAC rcvd GRR_MAC_DTM_UL_DCCH_CNF");
            gmac_util_send_ul_release_to_rlc(REL_UL_NORMAL,gas_id);

            if(gmac_util_is_rlc_blocked(GMAC_NULL,__LINE__,gas_id) == BLOCKED)
            {
              /*This is the case where RLC aborted UL GTTP transfer before GRR  */
              /*indicates the transfer was completed successfully, unblock RLC  */
              gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_NULL,__LINE__,gas_id);
            }

            gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);
            MSG_GERAN_MED_0_G("GMAC State null NULL_GPRS_51_ACTIVE");
            null_data_ptr->mac_null_substate = NULL_GPRS_51_ACTIVE;
            (void)mac_log_state(gas_id);
            break;

          case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
            MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_DTM_CHANNEL_RELEASE_IND");
            /*GTTP UL transfer failed before transfer was completed. RLC has rcvd a */
            /*UL config sig so needs to released                                    */

            /*DTM GRR_MAC_DTM_CHANNEL_RELEASE_IND indicates to MAC that GRR is in CS */
            /*operation only and hence translates to NO_PS_ACCESS, block RLC and wait*/
            /*for GRR_MAC_PS_ACCESS_REQ at which point RLC will be unblocked         */

            reset_tbf_parameters(gas_id);
            gmac_reset_rac_params(gas_id);

            gmac_util_block_rlc(NO_DEL,GMAC_NULL,__LINE__,gas_id);

            gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

            MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
            null_data_ptr->mac_null_substate = NULL_INACTIVE;
            (void)mac_log_state(gas_id);
            break;

          default:
            /* unknown signal from GRR */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from GRR %d",signal_id);
            break;
        }
      }
      else if (task_id == MS_MAC_RLC)
      {
        switch (signal_id)
        {
          case RM_UL_RELEASE_IND:
            /*This is the case where RLC termaintes GTTP before GRR. Send     */
            /*NO PS access to RLC and wait for GRR GRR_MAC_DTM_UL_DCCH_CNF    */
            /*or GRR_MAC_DTM_CHANNEL_RELEASE_IND                              */

            /*If GRR_MAC_DTM_UL_DCCH_CNF is rcvd need UNblock RLC             */
            /*IF GRR_MAC_DTM_CHANNEL_RELEASE_IND need to block and unblock RLC*/
            MSG_GERAN_MED_1_G("MAC Rcvd RM_UL_RELEASE_IND cause %d",global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause);
            gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
            break;

          default:
            /* unknown internal signal from RLC */
            MSG_GERAN_HIGH_1_G("MAC unknown signal from RLC %d",signal_id);
            break;
        }
      } /* end of RLC_MAC signals */

    break;

#endif /* FEATURE_GSM_DTM */


      /************************************
       * null state NULL_RESET *
       *************************************/
      case NULL_RESET:
        if(task_id == MS_MAC_RLC)
        {
          switch (signal_id)
          {
            case RM_UL_RESET_CNF:
              null_data_ptr->n_flags.rlc_ul_reset_cnf_pending = FALSE;

              if ( null_data_ptr->n_flags.rlc_dl_reset_cnf_pending == FALSE )
              {
                gmac_complete_reset(gas_id);
                MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
                null_data_ptr->mac_null_substate = NULL_INACTIVE;
                gmac_util_send_grr_reset_cnf(gas_id);
              }
            break;

            case RM_DL_RESET_CNF:
              null_data_ptr->n_flags.rlc_dl_reset_cnf_pending = FALSE;

              if ( null_data_ptr->n_flags.rlc_ul_reset_cnf_pending == FALSE )
              {
                gmac_complete_reset(gas_id);
                MSG_GERAN_MED_0_G("GMAC State null NULL_INACTIVE");
                null_data_ptr->mac_null_substate = NULL_INACTIVE;
                gmac_util_send_grr_reset_cnf(gas_id);
              }
            break;

            default:
              /* all other msgs are ignored*/
            break;
          }
        }
      break;

    default:
      break;
  }
}


/*===========================================================================
===
===  FUNCTION    gmac_null_determine_grr_access_type()
===
===  DESCRIPTION
===  Determines the access type for the Channel Request message.
===  There is a different set of causes for EGPRS and GPRS.
===
===  PARAMS
===  data_req_cause
===  rlc_mode
===  num_of_blocks
===  access_phase
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static uint8 gmac_null_determine_grr_access_type( grr_data_req_cause_t data_req_cause,
                                                  uint8                rlc_mode,
                                                  uint8                num_of_blocks,
                                                  uint8                access_phase,
                                                  gas_id_t             gas_id )
{
  boolean egprs_capable_cell = FALSE;
  boolean egprs_pkt_chan_req = FALSE;
  uint8   access_type        = 0;

  rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL, &egprs_capable_cell, gas_id );

  if ( egprs_capable_cell == TRUE )
  {
    rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_PKT_CHAN_REQ, &egprs_pkt_chan_req, gas_id );

    if ( egprs_pkt_chan_req == TRUE )
    {
      if ( data_req_cause == GRR_CAUSE_LAY3_DATA )
      {
        if ( rlc_mode == GRLC_MODE_UNACK )
        {
          access_type = GMAC_EGPRS_TWO_PHASE_ACCESS;
          MSG_GERAN_HIGH_0_G("GMAC RACH type: EGPRS cell, 11 bit RACH, 2phase");
        }
        else    /* RLC ACK Mode */
        {
          if ( num_of_blocks <= 8 )
          {
              /* short access no longer supported, set to 1-phase in this case to be the
                 same as the GPRS solution */
              access_type = GMAC_EGPRS_ONE_PHASE_ACCESS;
              MSG_GERAN_HIGH_0_G("GMAC RACH type: EGPRS cell, 11 bit RACH, 1phase");
          }
          else
          {
            /* 04.18 sec. 3.5.2.1.2  table indicates access type could be
               1-phase or 2-phase, RLC makes the decision as to which phase to use */
            switch ( access_phase )
            {
              case GMAC_NO_PHASE_ACCESS:
              case GMAC_SHORT_ACCESS: /* We do not support short acc so default to 1 phase*/
              case GMAC_ONE_PHASE_ACCESS:
                access_type = GMAC_EGPRS_ONE_PHASE_ACCESS;
                MSG_GERAN_HIGH_0_G("GMAC RACH type: EGPRS cell, 11 bit RACH, 1phase");
              break;

              case GMAC_SINGLE_BLOCK_ACCESS:
              case GMAC_TWO_PHASE_ACCESS:
                access_type = GMAC_EGPRS_TWO_PHASE_ACCESS;
                MSG_GERAN_HIGH_0_G("GMAC RACH type: EGPRS cell, 11 bit RACH, 2phase");
              break;

              default:
                MSG_GERAN_ERROR_1_G("GMAC UNKNOWN access phase: 0x%x, overriden with EGPRS 11bit 1phase",access_phase);
                access_type = GMAC_EGPRS_ONE_PHASE_ACCESS;
              break;
            }
          }
        }
      }
      else /*GMM Sig, LLC Sig, Page Response or Cell Update */
      {
        /* CR 112368, GCF TC's are being run without the RRPS_MS_ANITE_GCF_FLAG being set and
           are failing as a result.  Commenting out code to prevent the Anite GCF flag from
           modifying the cause to 1 phase.
           The original fix was on: //depot/asic/msmshared/geran/core/gmac/gmacnull.c#32

           and was put in for Paris throughput problems.

        rr_read_public_store( RRPS_MS_ANITE_GCF_FLAG, &gmac_anite_gcf_flg );

        if ( gmac_anite_gcf_flg == TRUE )
        */
        {
          access_type = GMAC_EGPRS_SIGNALLING_ACCESS;
          MSG_GERAN_HIGH_0_G("GMAC RACH type: EGPRS cell, 11 bit RACH, signalling");
        }
        /*
        else
        {
          access_type = GMAC_EGPRS_ONE_PHASE_ACCESS;
          MSG_GERAN_HIGH_0_G("GMAC RACH type: EGPRS cell, 11 bit RACH, 1phase");
        }
        */
      }
    }
    else /* Decision for Channel Req only, EGPRS Packet Channnel Req
            not avail in cell, all access */
    {    /* uses two-phase / single block */
      access_type = GMAC_TWO_PHASE_ACCESS;
      MSG_GERAN_HIGH_0_G("GMAC RACH type: EGPRS cell, 8 bit RACH, 2phase");
      /* rr handles GMAC_TWO_PHASE_ACCESS and GMAC_SINGLE_BLOCK_ACCESS */
      /* as single_block for the channel request */
      /* GMAC_TWO_PHASE_ACCESS type is used here because gmac_null_send_ul_tbf_req */
      /* treats GMAC_SINGLE_BLOCK_ACCESS as being for measurement reports or PCCF's */
    }
  }
  else  /* Not an EGPRS capable cell, make access_type decision using
           existing GPRS code */
  {
    /*ref 04.18 3.5.2.1.2*/
    if( rlc_mode == GRLC_MODE_UNACK)
    {
      access_type = GMAC_SINGLE_BLOCK_ACCESS;
      MSG_GERAN_HIGH_0_G("GMAC RACH type: GPRS cell, 8bit RACH, single block");
    }
    else
    {
      access_type = access_phase;
      MSG_GERAN_HIGH_1_G("GMAC RACH type: GPRS cell, 8bit RACH, access type: 0x%x",access_type);
    }
  }

  return access_type;
}


/* ----------------- ODM INFORMATION ACCESS FUNCTIONS -------------------- */


/*===========================================================================
===
===  FUNCTION    gprs_odm_info_is_assigned_tbf_egprs()
===
===  DESCRIPTION
===  Returns TRUE if the current TBF mode is EGPRS; FALSE otherwise
===
===  DEPENDENCIES
===  None
===
===  RETURN VALUE
===  TRUE/FALSE
===
===  SIDE EFFECTS
===  None
===
===========================================================================*/
boolean gprs_odm_info_is_assigned_tbf_egprs( void )
{
  //gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id); TODO DSDA - need to add as_id to parameter list
  
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;
  boolean egprs_tbf_mode = FALSE;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  egprs_tbf_mode = (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS);

  MSG_GERAN_ERROR_0_G("gprs_odm_info_is_assigned_tbf_egprs default to GERAN_ACCESS_STRATUM_ID_1");

  return egprs_tbf_mode;
}


/*===========================================================================
===
===  FUNCTION    gmac_null_get_null_state()
===
===  DESCRIPTION
===  This is an access function for external entities to get the mac_null_substate.
===
===  PARAMS
===
===
===  RETURN VALUE
===  gprs_mac_null_substates_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
gprs_mac_null_substates_t gmac_null_get_null_state(gas_id_t gas_id)
{
  gmac_null_data_t *null_data_ptr = gmac_get_null_data_ptr(gas_id);

  return null_data_ptr->mac_null_substate;
}

#ifdef PERLUTF
#error code not present
#endif

/*** EOF: don't remove! ***/
