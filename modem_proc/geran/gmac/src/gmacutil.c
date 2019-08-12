/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC UTILITY FUNCTIONS
***
***
*** DESCRIPTION
***
***  Contains utility functions that can be used throught MAC.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  None identified
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  None identified
***
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacutil.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 09/11/10   sjw     Updated for Dual SIM support
*** 04/07/01   tlx     Created
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "grlcx.h"

#include "gs.h"
#include "gmac.h"
#include "gmacdef.h"
#include "gmacrb.h"
#include "gmacutil.h"
#include "gmaclog.h"

/* included for the new RAC function*/
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "aconfig.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "gmm.h"
#include "gmacidle_if.h"

#include "gmacnull.h"
#include "gmactransfer_if.h"

#include "gmacproc.h"

#ifdef FEATURE_GSM_DTM
#include "cause.h"
#endif /* FEATURE_GSM_DTM */

#ifdef  DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */
#ifdef PERLUTF
#error code not present
#endif
#include "gmacds.h"
#include "geran_multi_sim.h"
#include "stringl/stringl.h"


#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

#include "gtmrs.h"
#include "gtmrs_g.h"
/*========================================================================== */
/* Check FEATURE assumptions */
/*========================================================================== */

#ifdef FEATURE_PRR_RAC_SIZE_REDUCTION  /* OFF */
  #pragma warning ("FEATURE_PRR_RAC_SIZE_REDUCTION assumed undefined but is")
#endif

/*****************************************************************************
***
***     Public Data Declarations
***
*****************************************************************************/

#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/

#ifdef FEATURE_GPRS_PS_HANDOVER
typedef enum
{
  PSHO_VALIDATION_PASS,
  PSHO_VALIDATION_FAIL
} gmac_pkt_ps_ho_result_t;
#endif /*FEATURE_GPRS_PS_HANDOVER*/


/*****************************************************************************
***
***     Private Macro Declarations
***
*****************************************************************************/

#define PSI_INSTANCE_FROM3BITS(octet)    (((octet) & 0x38) >> 3) /* bitmap 00111000 */
#define PSI_INSTANCE_FROM4BITS(octet)    (((octet) & 0x3C) >> 2) /* bitmap 00111100 */

/*===========================================================================
===
===  MACRO         MULTISLOT_CLASS_MIN_RX_CHECK
===
===  DESCRIPTION
===  GSM 05.02 section 6.4.2.2.
===  If there are 'm' timeslots allocated for reception and 'n' timeslots
===  allocated for transmission, there shall be Min(m,n) rx and tx timeslots
===  with the same TN.
===
===  This macro checks for the case where there are a minimum of rx timeslots
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  - TRUE if expression succeeds
===  - FALSE if expression fails
===
===  SIDE EFFECTS
===
===
===========================================================================*/
#define MULTISLOT_CLASS_MIN_RX_CHECK(ul_ts,dl_ts)                          \
                                      ((ul_ts | dl_ts) == ul_ts)

/*===========================================================================
===
===  MACRO         MULTISLOT_CLASS_MIN_TX_CHECK
===
===  DESCRIPTION
===  GSM 05.02 section 6.4.2.2.
===  If there are 'm' timeslots allocated for reception and 'n' timeslots
===  allocated for transmission, there shall be Min(m,n) rx and tx timeslots
===  with the same TN.
===
===  This macro checks for the case where there are a minimum of tx timeslots
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  - TRUE if expression succeeds
===  - FALSE if expression fails
===
===  SIDE EFFECTS
===
===
===========================================================================*/
#define MULTISLOT_CLASS_MIN_TX_CHECK(ul_ts,dl_ts)                          \
                                      ((ul_ts | dl_ts) == dl_ts)

#define FULL_RAC_LIST_SIZE 8

#define NO_BAND 0xff

/*lint -esym(750,GMAC_PGSM_MAXNO_ARFCNS)*/
#define GMAC_PGSM_MAXNO_ARFCNS 124 /* PGSM-900 */

/*lint -esym(750,GMAC_PGSM_BASE_ARFCN)*/
#define GMAC_PGSM_BASE_ARFCN   1

/*lint -esym(750,GMAC_EGSM_BASE_ARFCN)*/
#define GMAC_EGSM_BASE_ARFCN   0

#define GMAC_EGSM_MAX_ARFCN_LOWER 124
#define GMAC_EGSM_MIN_ARFCN_UPPER 975
#define GMAC_EGSM_MAX_ARFCN_UPPER 1023

#define GMAC_DCS_MAXNO_ARFCNS  374 /* DCS-1800 */
#define GMAC_DCS_BASE_ARFCN    512

#define GMAC_PCS_MAXNO_ARFCNS  299 /* PCS-1900 */
#define GMAC_PCS_BASE_ARFCN    512

#define GMAC_GSM_850_MAXNO_ARFCNS  124 /* GSM-850 */
#define GMAC_GSM_850_BASE_ARFCN    128

/*lint -esym(750,GMAC_BASE_ARFCN)*/
  #define GMAC_BASE_ARFCN   GMAC_EGSM_BASE_ARFCN


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/
static const uint8 full_rac_list[FULL_RAC_LIST_SIZE] = { ACCESS_TECH_GSM_P,
                                                         ACCESS_TECH_GSM_E,
                                                         ACCESS_TECH_GSM_R,
                                                         ACCESS_TECH_GSM_1800,
                                                         ACCESS_TECH_GSM_1900,
                                                         ACCESS_TECH_GSM_450,
                                                         ACCESS_TECH_GSM_480,
                                                         ACCESS_TECH_GSM_850};

gmac_util_data_t gmac_util_data[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/
static boolean gmac_util_compare_freq_params(frequency_information_T *freq_list_A,
                                             frequency_information_T *freq_list_B,
                                             gas_id_t gas_id);
static pua_data_error_t gmac_util_freq_arfcn_valid(freq_params_t *csn_freq_param_ptr,
                                                   frequency_information_T *ml_freq_config_info_ptr,
                                                   gas_id_t gas_id);
static pua_data_error_t gmac_util_load_freq_sig_data(rrps_frequency_list_t *rrps_freq_list_ptr,
                                                     frequency_information_T *ml_freq_config_info_ptr,
                                                     encoding_type_e freq_alloc_type,
                                                     gas_id_t gas_id);
static pua_data_error_t gmac_util_freq_indirect_valid(freq_params_t *csn_freq_param_ptr,
                                                      frequency_information_T *ml_freq_config_info_ptr,
                                                      gas_id_t gas_id);
static boolean gmac_util_change_mark_1_and_2_valid(freq_params_t *csn_freq_param_ptr,
                                                   gas_id_t gas_id);
static boolean supported_band_valid(boolean inhibit_pgsm, sys_band_T _band,
                                    gas_id_t gas_id);
static sys_band_T convert_rac_band_to_rr_format(uint8 nw_band,
                                                gas_id_t gas_id);

static void get_additional_rac_nw_list(uint8 *nw_band_list_ptr,uint8 *nw_band_count_ptr,
                                       uint8 nw_band_list_size,
                                       gas_id_t gas_id);

static void check_for_bcch_inclusion( uint8 *nw_bands_count_ptr, uint8 *nw_bands_ptr,
                                      gas_id_t gas_id );
static boolean modify_nw_list_for_pgsm_inhibit( uint8 band_count, uint8 *nw_bands_ptr,
                                                gas_id_t gas_id );

#ifdef PERLUTF
#error code not present
#else
static void modify_nw_list_for_1800_1900_band_combination_for_egprs( uint8 band_count, uint8 *nw_bands_ptr, gas_id_t gas_id );
static void modify_nw_list_for_1800_1900_band_combination_for_gprs( uint8 band_count, uint8 *nw_bands_ptr, gas_id_t gas_id );
#endif /*PERLUTF*/


static void gmac_send_mr_no_ps_access_ind(delete_pdu_cause_t delete_pdu_cause, gas_id_t gas_id);
static void gmac_send_mr_ps_access_req(action_on_ps_access_t action, gas_id_t gas_id);

#ifdef FEATURE_GSM_DTM
/*===========================================================================

FUNCTION gmac_util_get_eda_from_dtm_ass

DESCRIPTION
  Gets the EDA flag from UL or DL DTM assignments.

PARAMS
 dtm_ass_ptr                   Rcvd DTM signal data.
 eda_ptr                       Value of EDA is written to this return ptr.

RETURN VALUE
 boolean - indicates if function successfully obtained the EDA.

SIDE EFFECTS
  None

===========================================================================*/
static boolean gmac_util_get_eda_from_dtm_ass( gm_dtm_channel_assign_req_t *dtm_ass_ptr,
                                               uint8 *eda_ptr,
                                               gas_id_t gas_id);

/*=============================================================================

FUNCTION gmac_util_dtm_reverse_ts_bitmap()

DESCRIPTION
  Creates a reversed timeslot bitmap.

PARAMS
  input_ts_bmap :- timeslot bitmap to be reversed.

RETURN VALUE
  uint8 - reversed bitmap

SIDE EFFECTS
  NONE

=============================================================================*/
static uint8 gmac_util_dtm_reverse_ts_bitmap( uint8 input_ts_bmap, gas_id_t gas_id );

/*===========================================================================

FUNCTION gmac_util_dtm_ms_class_check

DESCRIPTION
  Performs multislot checking on the DTM Channel Ass.

PARAMS
 dtm_ass_ptr                   Rcvd DTM signal data.

RETURN VALUE
 boolean - indicates if the multislot check passed .

SIDE EFFECTS
  If the multislot check is successful the following globals are set:
    gmac_local_ul_ts_alloc
    gmac_local_dl_ts_alloc
    gmac_dtm_cs_ts_alloc

===========================================================================*/
static boolean gmac_util_dtm_ms_class_check( gm_dtm_channel_assign_req_t *dtm_ass_ptr,
                                             gas_id_t                     gas_id );

#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
static boolean gmac_get_ul_ts_alloc_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources,
                                                  uint8                          *ts_alloc,
                                                  gas_id_t                        gas_id);

static boolean gmac_get_usf_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources,
                                          mac_l1_psho_req_t              *psho_req,
                                          gas_id_t                        gas_id);

static boolean gmac_get_cs_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources,
                                         uint8                          *cs,
                                         gas_id_t                        gas_id);

static boolean gmac_get_egprs_mcs_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources,
                                                uint8                          *egprs_mcs,
                                                gas_id_t                        gas_id);

static boolean gmac_get_ul_ws_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources,
                                            uint8                          *ul_ws,
                                            gas_id_t                        gas_id);

static boolean gmac_get_dl_ws_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources,
                                            uint8                          *dl_ws,
                                            gas_id_t                        gas_id);
static gmac_pkt_ps_ho_result_t gmac_util_psho_validate_psho_req(ps_handover_radio_resources_t  *ps_handover_resources,
                                                                boolean                        *control_ack_ptr,
                                                                gas_id_t                        gas_id);
static void gmac_util_psho_populate_psho_req_config_msgs(ps_handover_radio_resources_t  *ps_handover_resources,
                                                         mac_l1_psho_req_t              *psho_req_ptr,
                                                         gas_id_t                        gas_id );


#endif /*FEATURE_GPRS_PS_HANDOVER*/

static void gmac_nacc_release_all_containers(gas_id_t gas_id);


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION    gmac_get_util_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to util data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_util_data_t*
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static gmac_util_data_t* gmac_get_util_data_ptr(gas_id_t gas_id)
{
  gmac_util_data_t *util_data_ptr = &gmac_util_data[0];

  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    util_data_ptr = &gmac_util_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)];
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Util bad gas_id:%d",gas_id);
  }

  return util_data_ptr;
}

#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/

/*===========================================================================
===
===  FUNCTION    gmac_init_util_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) util data.
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
void gmac_init_util_data_non_zero( gas_id_t gas_id )
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  util_data_ptr->gmac_rlc_blocked_flag = BLOCKED;
  util_data_ptr->gmac_pcco_state = NO_PCCO_ACTIVE;
  util_data_ptr->gmac_pccf_state = NO_PCCF_BEING_SENT;
}

/*===========================================================================
===
===  FUNCTION    gmac_init_util_data()
===
===  DESCRIPTION
===  Initialises Util data.  Used for Lower layer resets.
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
void gmac_init_util_data( gas_id_t gas_id )
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  /* frequency parameters - checks new list agains list in use for differences */
  util_data_ptr->identical_freq_list = FALSE;
  memset( util_data_ptr->nw_bands, 0, sizeof(util_data_ptr->nw_bands) );
  util_data_ptr->nw_bands_count = 0;
  memset( util_data_ptr->csn_encoded_rac_prr_msg, 0, sizeof(util_data_ptr->csn_encoded_rac_prr_msg) );
  memset( util_data_ptr->csn_encoded_rac_arac_msg, 0, sizeof(util_data_ptr->csn_encoded_rac_arac_msg) );
  memset( &util_data_ptr->prr_ie, 0, sizeof(util_data_ptr->prr_ie) );
  memset( &util_data_ptr->arac_ie, 0, sizeof(util_data_ptr->arac_ie) );
  memset( &util_data_ptr->scell_info, 0, sizeof(util_data_ptr->scell_info) );
  memset( util_data_ptr->ncell_info, 0, sizeof(util_data_ptr->ncell_info) );

  util_data_ptr->gmac_rlc_blocked_flag = BLOCKED;
  util_data_ptr->gmac_pcco_state = NO_PCCO_ACTIVE;
  util_data_ptr->gmac_pccf_state = NO_PCCF_BEING_SENT;
  util_data_ptr->gmac_current_page_mode = 0;

  /* Do not reset gmac_active_gas_id here because it is required to populate
     the MAC_GRR_RESET_CNF signal.  gmac_active_gas_id is reset after MAC_GRR_RESET_CNF
     has been sent*/
}

/*===========================================================================
===
===  FUNCTION    mac_complete_reset()
===
===  DESCRIPTION
===  Initialises all MAC data and flags.
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
void gmac_complete_reset( gas_id_t gas_id )
{
  /*reset all timers*/
  t3200_reset_handler(gas_id);
  gmac_reset_stop_all_timers(gas_id);

  /*Clear all data*/
  gmac_init_util_data(gas_id);
  gmac_init_null_data(gas_id);
  gmac_init_idle_data(gas_id);
  gmac_init_idle_util_data(gas_id);
  gmac_init_transfer_data(gas_id);
  gmac_init_mac_proc_data(gas_id);
  gmac_init_rb_data(gas_id);
  gmac_init_task_data(gas_id);
  gmac_init_tran_ul_data(gas_id);
  gmac_init_tran_dl_data(gas_id);

  /*Clear all flags*/
  gmac_init_transfer_flags(gas_id);
  gmac_init_idle_flags(gas_id);
  gmac_init_common_flags(gas_id);
  gmac_init_null_flags(gas_id);

  gmac_reset_clear_task_queue(gas_id);
}

/*===========================================================================
===
===  FUNCTION      reset_tbf_parameters()
===
===  DESCRIPTION
===  Resets the current_tbf_mode for EGPRS and the current_test_mode for both
===  EGPRS and GPRS.  This function should be called after all
===  TBF's have been released.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===  void
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void reset_tbf_parameters( gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("GMAC resetting TBF parameters");

  global_data_ptr->current_tbf_mode = TBF_MODE_INVALID;
  global_data_ptr->current_test_mode = TEST_MODE_OFF;
  global_data_ptr->current_rcvd_dl_ctrl_ack = FALSE;

}


/*===========================================================================
===
===  FUNCTION      gmac_util_compare_freq_params()
===
===  DESCRIPTION
===  Compares two given frequency lists to establish if they contain the
===  same values. This takes into account the number of channels incase
===  there are less than 64 channels (do not compare whole array!).
===
===  DEPENDENCIES
===
===  RETURN VALUE
===  Returns the error cause and recover type (pua_data_error_t):
===  - TRUE if lists A and B are the same
===  - FALSE if the lists are different
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static boolean gmac_util_compare_freq_params(frequency_information_T *freq_list_A,frequency_information_T *freq_list_B,
                                             gas_id_t gas_id)
{

  uint8 i;

  if ((freq_list_A->hopping_flag == freq_list_B->hopping_flag) &&
      (freq_list_A->tsc == freq_list_B->tsc) &&
      (freq_list_A->maio == freq_list_B->maio) &&
      (freq_list_A->hsn == freq_list_B->hsn) &&
      (freq_list_A->frequency_list.no_of_items == freq_list_B->frequency_list.no_of_items))
  {


   for(i = 0; i< freq_list_B->frequency_list.no_of_items; i++)
   {
     if(freq_list_A->frequency_list.channel[i].num != freq_list_B->frequency_list.channel[i].num)
     {
        MSG_GERAN_MED_2_G("Info a_list freq %x  b_list freq %x ",
                          freq_list_A->frequency_list.channel[i].num,freq_list_B->frequency_list.channel[i].num);
        return (FALSE);
     }
     if(freq_list_A->frequency_list.channel[i].band !=freq_list_B->frequency_list.channel[i].band)
     {
        MSG_GERAN_MED_2_G("Info a_list freq %x  b_list band %x ",
                          freq_list_A->frequency_list.channel[i].band,freq_list_B->frequency_list.channel[i].band);
        return (FALSE);
     }
   }

  }
  else
  {
    return (FALSE);
  }
  return (TRUE);
}

/*===========================================================================
===
===  FUNCTION      gmac_util_freq_arfcn_valid()
===
===  DESCRIPTION
===
===  for absolute frequency , the frequency value received in assignment msg
===  will always be valid ie between 0 and 1023 as arfcn is Represented by a
===  10 bit field
===
===  NOTE: set MAIO even though not used.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===  Returns the error cause and recover type (pua_data_error_t):
===  - NO_ERRORS
===  - FREQ_ERROR_ACCESS_RETRY
===  - FREQ_ERROR_PARTIAL_SYS_ACQ
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static pua_data_error_t gmac_util_freq_arfcn_valid(freq_params_t *csn_freq_param_ptr, frequency_information_T *ml_freq_config_info_ptr,
                                                   gas_id_t gas_id)
{
  pua_data_error_t return_value = NO_ERRORS;

  /* hopping flag set to FALSE indicates absolute frequency in MAC_L1_FREQ_CONFIG */
  ml_freq_config_info_ptr->hopping_flag = FALSE;
  ml_freq_config_info_ptr->frequency_list.no_of_items = 1;
  ml_freq_config_info_ptr->tsc = csn_freq_param_ptr->tsc;
  ml_freq_config_info_ptr->maio = 0;
   ml_freq_config_info_ptr->frequency_list.channel[0] = csn_freq_param_ptr->arfcn;
  return (return_value);
}

/*===========================================================================
===
===  FUNCTION      gmac_util_load_freq_sig_data()
===
===  DESCRIPTION
===
===  This function checks the frequency list retrieved from publis store
===  for validity and loads the mac_l1_freq_config signal with the frequency
===  Data retrieved form the public store ONLY. The calling entity is
===  responsible for loading any data that's only available in the dl msg.
===
===  (Note:- for all frequency assignments other than "absolute" a hopping
===  sequence is required. (ref 4.60 5.5.1.7)
===
===  Also for direct_2 frequency assignment the HSN is obtained from
===  the dl msg
===
===  DEPENDENCIES
===
===  RETURN VALUE
===  Returns the error cause and recover type (pua_data_error_t):
===  - NO_ERRORS
===  - FREQ_ERROR_ACCESS_RETRY
===  - FREQ_ERROR_PARTIAL_SYS_ACQ
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static pua_data_error_t gmac_util_load_freq_sig_data(rrps_frequency_list_t *rrps_freq_list_ptr,
                                                     frequency_information_T *ml_freq_config_info_ptr,
                                                     encoding_type_e freq_alloc_type,
                                                     gas_id_t gas_id)
{
  pua_data_error_t return_value = NO_ERRORS;

  /*load MAC_L1_FREQ_CONFIG signal data retrived from GRR public store */
  ml_freq_config_info_ptr->hopping_flag = TRUE;
  if((freq_alloc_type == direct1_valid) || (freq_alloc_type == indirect_valid))
  {
    ml_freq_config_info_ptr->hsn = rrps_freq_list_ptr->hsn;
  }
  ml_freq_config_info_ptr->frequency_list.no_of_items = rrps_freq_list_ptr->list_length;
  memscpy(ml_freq_config_info_ptr->frequency_list.channel,
          sizeof(ml_freq_config_info_ptr->frequency_list.channel),
          rrps_freq_list_ptr->ma_number,
          sizeof(rrps_freq_list_ptr->ma_number));
  return(return_value);
}

/*===========================================================================
===
===  FUNCTION      gmac_util_freq_indirect_valid()
===
===  DESCRIPTION
===
===  When Change_mark_1 and 2 are received in assignment msg their numerical
===  values refer to the same SI change mark. Which of the 2 type of SI change
===  mark is determined by the MA number. MA 0 to 13 = PSI 2 , MA 14 = SI 13
===  or PSI 13, MA = 15 change_mark_1 and 2 are not valid.
===
===  If both change marks are present they are both checked against the change
===  marks stored in the public store.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===  Returns the error cause and recover type (pua_data_error_t):
===  - NO_ERRORS
===  - FREQ_ERROR_ACCESS_RETRY
===  - FREQ_ERROR_PARTIAL_SYS_ACQ
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static pua_data_error_t gmac_util_freq_indirect_valid(freq_params_t           *csn_freq_param_ptr,
                                                      frequency_information_T *ml_freq_config_info_ptr,
                                                      gas_id_t                 gas_id)
{
  /*pointer "frequency list" held in grr's public store */
  rrps_frequency_list_t *frequency_list_ptr;

  /* local store for "valid_ma_numbers_bit_map" value read from public store */
  uint16 valid_ma_numbers_bit_map;

  /* used to check if ma_number is in ma_list */
  uint16 ma_mask = 1;

  pua_data_error_t return_value = NO_ERRORS;

  if (csn_freq_param_ptr->indir_enc.ma_number < 14)
  {
    return_value = FREQ_ERROR_ACCESS_RETRY;
  }
  else
  if (csn_freq_param_ptr->indir_enc.ma_number == 14)
  {
    rr_read_public_store(RRPS_VALID_MA_BIT_MAP,&valid_ma_numbers_bit_map,gas_id);

    /* check MA-number is stored list */
    if (((valid_ma_numbers_bit_map) & (ma_mask << csn_freq_param_ptr->indir_enc.ma_number)) !=0)
    {
      if(gmac_util_change_mark_1_and_2_valid(csn_freq_param_ptr,gas_id) == TRUE)
      {
        frequency_list_ptr = rr_read_public_store_freq_list(  csn_freq_param_ptr->indir_enc.ma_number,
                                                              gas_id );

        if(frequency_list_ptr == NULL)
        {
          return_value = FREQ_ERROR_PARTIAL_SYS_ACQ;
          MSG_GERAN_ERROR_0_G("GMAC error: No frequency list in public store");
        }
        else
        {
          return_value =  gmac_util_load_freq_sig_data(frequency_list_ptr,ml_freq_config_info_ptr,indirect_valid,
                                                       gas_id);
        }
      }
      else
      {
        return_value = FREQ_ERROR_PARTIAL_SYS_ACQ;
        MSG_GERAN_MED_0_G("MAC error change_mark_1 or 2 mismatch ");
      }
    }
    else
    {
      return_value = FREQ_ERROR_PARTIAL_SYS_ACQ;
      MSG_GERAN_LOW_0_G("MAC error MA number not in storted list");
    }
  }
  else
  {
    /* MA number is 15, use MA frequency allocation Previously */
    /* received in either a PUA, PDA or PTR message            */
    if(csn_freq_param_ptr->indir_enc.change_mark_flag == no_change_mark_valid)
    {
      frequency_list_ptr = rr_read_public_store_freq_list(15,gas_id);

      if(frequency_list_ptr == NULL)
      {
        return_value = FREQ_ERROR_PARTIAL_SYS_ACQ;
        MSG_GERAN_ERROR_0_G("GMAC error: No frequency list in public store");
      }
      else
      {
        return_value =  gmac_util_load_freq_sig_data(frequency_list_ptr,ml_freq_config_info_ptr,indirect_valid,
                                                     gas_id);
      }

    }
    else
    {
      return_value = FREQ_ERROR_ACCESS_RETRY;
      MSG_GERAN_LOW_0_G("MAC error MA=15 & change_mark present");
    }
  }
  return(return_value);
}

/*===========================================================================
===
===  FUNCTION      gmac_util_check_change_mark_1_and_2()
===
===  DESCRIPTION
===  Checks for valid CHANGE_MARK in frequency parameters against those
===  stored in GRR (received in broadcast information). If no change mark
===  is present then no check is required and the function shall return
===  TRUE.
===
===  NOTE: change mark 2 cannot be present without change mark 1
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  - TRUE if change mark for freq params and GRR stored values match
===  - FALSE if change mark values differ
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static boolean gmac_util_change_mark_1_and_2_valid(freq_params_t *csn_freq_param_ptr,
                                                   gas_id_t       gas_id)
{
  boolean result = TRUE;

  rrps_change_mark_t  change_mark;

  /* MA_NUMBER = 14 */
  rr_read_public_store(RRPS_CHANGE_MARK_SI13,&change_mark,gas_id);

  if(csn_freq_param_ptr->indir_enc.change_mark_flag == change_mark_1_valid)
  {
    if(csn_freq_param_ptr->indir_enc.change_mark_1 != change_mark)
    {
      result = FALSE;
    }
  }
  else if(csn_freq_param_ptr->indir_enc.change_mark_flag == both_change_marks_valid)
  {
    if((csn_freq_param_ptr->indir_enc.change_mark_1 != change_mark) &&
       (csn_freq_param_ptr->indir_enc.change_mark_2 != change_mark))
    {
      result = FALSE;
    }
  }
  return(result);
}

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_send_data_ind_to_grr
===
===  DESCRIPTION
===  Send data block to GRR with signal MAC_GRR_DATA_IND
===
===  DEPENDENCIES
===  csn_decoder_result - global decoded CSN.1 message
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_send_data_ind_to_grr(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Sent MAC_GRR_DATA_IND");
  global_data_ptr->grr_sig.mac_grr_data_ind.data_ptr = global_data_ptr->csn_decoder_result;
#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/

  global_data_ptr->grr_sig.mac_grr_data_ind.bcch_arfcn = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.bcch_arfcn;
  global_data_ptr->grr_sig.mac_grr_data_ind.message_header.message_id = (uint8)MAC_GRR_DATA_IND;
  global_data_ptr->grr_sig.mac_grr_data_ind.message_header.message_set = MS_MAC_RR;
  gprs_grr_mac_acc_func(&global_data_ptr->grr_sig,gas_id);
}
/*===========================================================================
===
===  FUNCTION      gmac_util_send_ul_release_to_rlc
===
===  DESCRIPTION
===  Send access reject to RLC with signal MR_UL_RELEASE_IND.
===  The signal contains a cause value.
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
void gmac_util_send_ul_release_to_rlc(rm_tbf_release_t rel_cause,gas_id_t gas_id)
{
  mac_rlc_msg_t  *mac_rlc_sig_ptr;

  mac_rlc_sig_ptr = grlc_get_cmd_buf_mac();
  if (mac_rlc_sig_ptr != NULL)
  {
    MSG_GERAN_MED_1_G("MAC Sent MR_UL_RELEASE_IND %d",rel_cause);
    mac_rlc_sig_ptr->msg.ul_release_ind.sig = MR_UL_RELEASE_IND;
    mac_rlc_sig_ptr->msg.ul_release_ind.cause = rel_cause;
    gmac_rlc_mac_insert_gas_id(mac_rlc_sig_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mac_rlc_sig_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_util_send_dl_ass_to_rlc
===
===  DESCRIPTION
===  Send DL assignment indication to RLC with signal MR_DL_ASS_IND.
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
void gmac_util_send_dl_ass_to_rlc(gas_id_t gas_id)
{
  mac_rlc_msg_t *mac_rlc_sig_ptr;
  uint32        local_rrps_tlli;

  /*TLLI in DL CNF to RLC*/
  /*for logging only*/
  uint32 old_local_rrps_tlli;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);
  rr_read_public_store(RRPS_TLLI_OLD,&old_local_rrps_tlli,gas_id);

  mac_rlc_sig_ptr = grlc_get_cmd_buf_mac();
  if (mac_rlc_sig_ptr != NULL)
  {

    MSG_GERAN_HIGH_3_G("MAC  gmac_store_dl_ass_tlli %x,L_tlli %x o_tlli %x",
                       global_data_ptr->gmac_store_dl_ass_tlli,local_rrps_tlli,old_local_rrps_tlli);
    MSG_GERAN_MED_2_G("MAC Sent MR_DL_ASS_IND tbf_mode = %d, ws = %d",
                      global_data_ptr->current_tbf_mode,global_data_ptr->current_egprs_dl_ws);

    MSG_GERAN_MED_2_G("gmac_util_send_dl_ass_to_rlc bit map dl ts = 0x%x ,  num dl ts %d ",
                      global_data_ptr->gmac_local_dl_ts_alloc,
                      gmac_utils_calculate_nts(global_data_ptr->gmac_local_dl_ts_alloc, gas_id));
    mac_rlc_sig_ptr->msg.dl_ass_ind.num_dl_ts = gmac_utils_calculate_nts(global_data_ptr->gmac_local_dl_ts_alloc,
                                                                         gas_id);

    if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
    {
        mac_rlc_sig_ptr->msg.dl_ass_ind.tbf_mode = global_data_ptr->current_tbf_mode;
        mac_rlc_sig_ptr->msg.dl_ass_ind.ws = global_data_ptr->current_egprs_dl_ws;
    }
    else if (global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
    {
       mac_rlc_sig_ptr->msg.dl_ass_ind.tbf_mode = global_data_ptr->current_tbf_mode;
    }
    else
    {
       if(geran_get_nv_recovery_restart_enabled(gas_id))
       {
         MSG_GERAN_ERROR_1_G("Invalid TBF mode = %d",global_data_ptr->current_tbf_mode);
         MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
         geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
       }
       else
       {
         ERR_GERAN_FATAL_2_G("Invalid TBF mode = %d",gas_id, global_data_ptr->current_tbf_mode);
       }
    }

    mac_rlc_sig_ptr->msg.dl_ass_ind.ctrl_ack = global_data_ptr->current_rcvd_dl_ctrl_ack;

    mac_rlc_sig_ptr->msg.dl_ass_ind.sig = MR_DL_ASS_IND;

    mac_rlc_sig_ptr->msg.dl_ass_ind.rlc_mode = global_data_ptr->gmac_store_dl_rlc_mode;
    mac_rlc_sig_ptr->msg.dl_ass_ind.tlli = global_data_ptr->gmac_store_dl_ass_tlli;
    gmac_rlc_mac_insert_gas_id(mac_rlc_sig_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mac_rlc_sig_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_power_control_params_ie
===
===  DESCRIPTION
===  Handles power control parameters IE
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
void gmac_power_control_params_ie(pwr_ctrl_paramtrs_t *pwr_ctrl_in, power_control_params_T *pwr_ctrl,
                                  gas_id_t gas_id)
{
  pwr_ctrl->present = TRUE;

  if (pwr_ctrl_in->alpha > PWR_CTRL_ALPHA_MAX_VALUE)
  {
    pwr_ctrl->alpha = PWR_CTRL_ALPHA_MAX_VALUE;
  }
  else
  {
    pwr_ctrl->alpha = pwr_ctrl_in->alpha;
  }
  if (pwr_ctrl_in->gamma_bitmap & GAMMA_TN0_POS)
  {
    pwr_ctrl->gamma_TN0.present = TRUE;
    pwr_ctrl->gamma_TN0.gamma = pwr_ctrl_in->gamma_tn0;
  }
  else
  {
    pwr_ctrl->gamma_TN0.present = FALSE;
  }
  if (pwr_ctrl_in->gamma_bitmap & GAMMA_TN1_POS)
  {
    pwr_ctrl->gamma_TN1.present = TRUE;
    pwr_ctrl->gamma_TN1.gamma = pwr_ctrl_in->gamma_tn1;
  }
  else
  {
    pwr_ctrl->gamma_TN1.present = FALSE;
  }
  if (pwr_ctrl_in->gamma_bitmap & GAMMA_TN2_POS)
  {
    pwr_ctrl->gamma_TN2.present = TRUE;
    pwr_ctrl->gamma_TN2.gamma = pwr_ctrl_in->gamma_tn2;
  }
  else
  {
    pwr_ctrl->gamma_TN2.present = FALSE;
  }
  if (pwr_ctrl_in->gamma_bitmap & GAMMA_TN3_POS)
  {
    pwr_ctrl->gamma_TN3.present = TRUE;
    pwr_ctrl->gamma_TN3.gamma = pwr_ctrl_in->gamma_tn3;
  }
  else
  {
    pwr_ctrl->gamma_TN3.present = FALSE;
  }
  if (pwr_ctrl_in->gamma_bitmap & GAMMA_TN4_POS)
  {
    pwr_ctrl->gamma_TN4.present = TRUE;
    pwr_ctrl->gamma_TN4.gamma = pwr_ctrl_in->gamma_tn4;
  }
  else
  {
    pwr_ctrl->gamma_TN4.present = FALSE;
  }
  if (pwr_ctrl_in->gamma_bitmap & GAMMA_TN5_POS)
  {
    pwr_ctrl->gamma_TN5.present = TRUE;
    pwr_ctrl->gamma_TN5.gamma = pwr_ctrl_in->gamma_tn5;
  }
  else
  {
    pwr_ctrl->gamma_TN5.present = FALSE;
  }
  if (pwr_ctrl_in->gamma_bitmap & GAMMA_TN6_POS)
  {
    pwr_ctrl->gamma_TN6.present = TRUE;
    pwr_ctrl->gamma_TN6.gamma = pwr_ctrl_in->gamma_tn6;
  }
  else
  {
    pwr_ctrl->gamma_TN6.present = FALSE;
  }
  if (pwr_ctrl_in->gamma_bitmap & GAMMA_TN7_POS)
  {
    pwr_ctrl->gamma_TN7.present = TRUE;
    pwr_ctrl->gamma_TN7.gamma = pwr_ctrl_in->gamma_tn7;
  }
  else
  {
    pwr_ctrl->gamma_TN7.present = FALSE;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_packet_ta_ie
===
===  DESCRIPTION
===  Handles packet timing advance IE
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
boolean gmac_packet_ta_ie(pkt_timing_adv_t *pkt_ta_in, timing_advance_params_T *ta_params,
                          gas_id_t gas_id)
{
  boolean params_present = FALSE;

  if (pkt_ta_in->valid_options == all_timing_params_valid)
  {
    params_present = TRUE;
    ta_params->ta_value_ie.present = TRUE;
    ta_params->cont_ta_ie.present = TRUE;
    ta_params->ta_value_ie.ta_value = pkt_ta_in->timing_adv_value;
    ta_params->cont_ta_ie.tai = pkt_ta_in->timing_adv_index;
    ta_params->cont_ta_ie.ta_timeslot = pkt_ta_in->timing_adv_timeslot;
  }
  else if (pkt_ta_in->valid_options == timing_value_valid)
  {
    params_present = TRUE;
    ta_params->ta_value_ie.present = TRUE;
    ta_params->cont_ta_ie.present = FALSE;
    ta_params->ta_value_ie.ta_value = pkt_ta_in->timing_adv_value;
  }
  else if (pkt_ta_in->valid_options == timing_indx_ts_valid)
  {
    params_present = TRUE;
    ta_params->ta_value_ie.present = FALSE;
    ta_params->cont_ta_ie.present = TRUE;
    ta_params->cont_ta_ie.tai = pkt_ta_in->timing_adv_index;
    ta_params->cont_ta_ie.ta_timeslot = pkt_ta_in->timing_adv_timeslot;
  }
  else
  {
    ta_params->ta_value_ie.present = FALSE;
    ta_params->cont_ta_ie.present = FALSE;
  }
  return (params_present);
}

/*===========================================================================
===
===  FUNCTION      gmac_util_handle_power_ctrl_TA_IE
===
===  DESCRIPTION
===  The data received in PACKET TIMING ADVANCE IE is copied
===  into the global timing advance parameter structure for uplink TA.
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
boolean gmac_util_handle_power_ctrl_TA_IE(pkt_timing_adv_t pkt_ta_in, global_packet_ta_T *ta_params,
                                          gas_id_t gas_id)
{
  boolean params_present = FALSE;

  if (pkt_ta_in.valid_options == all_timing_params_valid)
  {
    params_present = TRUE;
    ta_params->tav.present = TRUE;
    ta_params->uplink_tai.present = TRUE;
    ta_params->downlink_tai.present = FALSE;
    ta_params->tav.ta_value = pkt_ta_in.timing_adv_value;
    ta_params->uplink_tai.tai = pkt_ta_in.timing_adv_index;
    ta_params->uplink_tai.ta_timeslot = pkt_ta_in.timing_adv_timeslot;
  }
  else if (pkt_ta_in.valid_options == timing_value_valid)
  {
    params_present = TRUE;
    ta_params->tav.present = TRUE;
    ta_params->uplink_tai.present = FALSE;
    ta_params->downlink_tai.present = FALSE;
    ta_params->tav.ta_value = pkt_ta_in.timing_adv_value;
  }
  else if (pkt_ta_in.valid_options == timing_indx_ts_valid)
  {
    params_present = TRUE;
    ta_params->tav.present = FALSE;
    ta_params->uplink_tai.present = TRUE;
    ta_params->downlink_tai.present = FALSE;
    ta_params->uplink_tai.tai = pkt_ta_in.timing_adv_index;
    ta_params->uplink_tai.ta_timeslot = pkt_ta_in.timing_adv_timeslot;
  }
  else
  {
    ta_params->tav.present = FALSE;
    ta_params->uplink_tai.present = FALSE;
    ta_params->downlink_tai.present = FALSE;
  }
  return (params_present);
}

/*===========================================================================
===
===  FUNCTION      gmac_read_acknack_desc_ie
===
===  DESCRIPTION
===  Handles decoding ack/nack description IE received in PUAN
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
void gmac_read_acknack_desc_ie(ack_nack_desc_t acknack_data_in, rm_acknack_desc_t *acknack_desc_ie,
                               gas_id_t gas_id)
{
  acknack_desc_ie->fai = acknack_data_in.final_ack_ind;
  acknack_desc_ie->ssn = acknack_data_in.strt_seq_num;
  memscpy(acknack_desc_ie->rbb,sizeof(acknack_desc_ie->rbb),
          acknack_data_in.received_blk_btmp, RM_RBB_ARRAY_LEN);

}

/*===========================================================================
===
===  FUNCTION      gmac_compile_acknack_desc_ie
===
===  DESCRIPTION
===  Compile and encode ACK/NACK description IE for PDAN message
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
void gmac_compile_acknack_desc_ie(ack_nack_desc_t *acknack_ie, rm_acknack_desc_t acknack_desc_in,
                                  gas_id_t gas_id)
{
  acknack_ie->final_ack_ind = acknack_desc_in.fai;
  acknack_ie->strt_seq_num = acknack_desc_in.ssn;
  memscpy(acknack_ie->received_blk_btmp,sizeof(acknack_ie->received_blk_btmp),
          acknack_desc_in.rbb , RM_RBB_ARRAY_LEN);
}

/*===========================================================================
===
===  FUNCTION      gmac_global_power_ctrl_params_ie
===
===  DESCRIPTION
===  Handles power control parameters IE
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
void gmac_global_power_ctrl_params_ie(global_pwr_ctrl_t *gl_pwr_ctrl_in, rrps_global_power_ctrl_params_t *global_pwr_ctrl,
                                      gas_id_t gas_id)
{
  if (gl_pwr_ctrl_in->alpha > PWR_CTRL_ALPHA_MAX_VALUE)
  {
    global_pwr_ctrl->alpha = PWR_CTRL_ALPHA_MAX_VALUE;
  }
  else
  {
    global_pwr_ctrl->alpha = gl_pwr_ctrl_in->alpha;
  }

  if (gl_pwr_ctrl_in->alpha > T_AVG_W_MAX_VALUE)
  {
    global_pwr_ctrl->t_avg_w = T_AVG_W_MAX_VALUE;
  }
  else
  {
    global_pwr_ctrl->t_avg_w = gl_pwr_ctrl_in->t_avg_w;
  }

  if (gl_pwr_ctrl_in->alpha > T_AVG_T_MAX_VALUE)
  {
    global_pwr_ctrl->t_avg_t = T_AVG_T_MAX_VALUE;
  }
  else
  {
    global_pwr_ctrl->t_avg_t = gl_pwr_ctrl_in->t_avg_t;
  }

  global_pwr_ctrl->pb = gl_pwr_ctrl_in->pb;
  global_pwr_ctrl->pc_meas_chan = gl_pwr_ctrl_in->pc_meas_chan;
  global_pwr_ctrl->int_meas_channel_list_avail = gl_pwr_ctrl_in->int_meas_chn_list;
  global_pwr_ctrl->n_avg_i = gl_pwr_ctrl_in->n_avg_i;
}

/*===========================================================================
===
===  FUNCTION      gmac_global_packet_ta_ie
===
===  DESCRIPTION
===  Handles power control parameters IE
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
void gmac_global_packet_ta_ie(global_pkt_timing_t *gl_pkt_ta_in, global_packet_ta_T *global_packet_ta,
                              gas_id_t gas_id)
{
  if (gl_pkt_ta_in->ta_flag)
  {
    global_packet_ta->tav.present = TRUE;
    global_packet_ta->tav.ta_value = gl_pkt_ta_in->ta_value;
  }
  else
  {
    global_packet_ta->tav.present = FALSE;
  }
  if (gl_pkt_ta_in->uplink_ta_flag)
  {
    global_packet_ta->uplink_tai.present = TRUE;
    global_packet_ta->uplink_tai.tai = gl_pkt_ta_in->uplink_ta_index;
    global_packet_ta->uplink_tai.ta_timeslot = gl_pkt_ta_in->uplink_ta_ts_num;
  }
  else
  {
    global_packet_ta->uplink_tai.present = FALSE;
  }
  if (gl_pkt_ta_in->dwnlink_ta_flag)
  {
    global_packet_ta->downlink_tai.present = TRUE;
    global_packet_ta->downlink_tai.tai = gl_pkt_ta_in->dwnlink_ta_index;
    global_packet_ta->downlink_tai.ta_timeslot = gl_pkt_ta_in->dwnlink_ta_ts_num;
  }
  else
  {
    global_packet_ta->downlink_tai.present = FALSE;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_ul_dummy_blk_to_l1
===
===  DESCRIPTION
===  Handles sending MAC_L1_DUMMY_CTRL_BLOCK to L1.
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
void gmac_ul_dummy_blk_to_l1(gas_id_t gas_id)
{
  int_data_t    input_data;
  mac_l1_sig_t  mac_l1_sig;
  uint32        tlli_store;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  mac_l1_dummy_ctrl_block_t *dcb_msg = &(mac_l1_sig.mac_l1_dummy_ctrl_block);

  rr_read_public_store(RRPS_TLLI,&tlli_store,gas_id);
  input_data.type.pkt_uplk_dummy_cntrl_blk.tlli = tlli_store;

  if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
  {
    dcb_msg->dummy_ctrl_block[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
  }
  else
  {
    dcb_msg->dummy_ctrl_block[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
  }

  if (gcsn1_encoder(PACKET_UPLK_DUMMY_CNTRL_BLK, &(dcb_msg->dummy_ctrl_block[1]), &input_data) == PASS)
  {

    /* call log-on-demand function */
    (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                     PACKET_UPLK_DUMMY_CNTRL_BLK,
                                     (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                     &(dcb_msg->dummy_ctrl_block[1]),
                                       gas_id);

    /* send MAC_L1_DUMMY_CTRL_BLOCK message to L1 */
    MSG_GERAN_MED_0_G("MAC Sent MAC_L1_DUMMY_CTRL_BLOCK");
    mac_l1_sig.mac_l1_dummy_ctrl_block.message_header.message_id = (uint8)MAC_L1_DUMMY_CTRL_BLOCK;
    mac_l1_sig.mac_l1_dummy_ctrl_block.message_header.message_set = MS_MAC_L1;
    gprs_l1_mac_acc_func(&mac_l1_sig,gas_id);
  }
  else
  {
    if(geran_get_nv_recovery_restart_enabled(gas_id))
    {
      MSG_GERAN_ERROR_0_G("MAC error CSN.1 encode PDDCB");
      MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
      geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
    }
    else
    {
      ERR_GERAN_FATAL_0_G("MAC error CSN.1 encode PDDCB");
    }
  }
}


/*===========================================================================
===
===  FUNCTION      gmac_util_global_tfi
===
===  DESCRIPTION
===  Checks global TFI value against TFI values for UL or DL TBF.
===
===  DEPENDENCIES
===  dl_tfi_valid is an indication that a DL TBF is in progress
===  ul_tfi_valid is an indication that a UL TBF is in progress
===
===  RETURN VALUE
===  - TRUE if TFI match, otherwise FALSE;
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_util_global_tfi(global_tfi_t rxd_global_tfi,
                             gas_id_t gas_id)
{
  uint8 tfi_match = FALSE;

  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
  if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
  {

    if (rxd_global_tfi.direction == uplink_valid)
    {
      if ((rxd_global_tfi.uplink_tfi == global_data_ptr->gmac_local_tfi_store.ul_tfi) &&
          (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid))
      {
        if ( ( ((uint8)(0x80 >> global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot)) &
                global_data_ptr->gmac_local_ul_ts_alloc ) == 0 )
        {
          /* Ctrl msg with UL TFI is received on a PDCH that is NOT in the UL TBF */
          tfi_match = FALSE;
          MSG_GERAN_HIGH_3_G("GMAC UL TFI rxd on TS NOT in UL TBF, rxd TS:%d, UL TFI:%d, UL TS curr bmap:0x%x",
                             global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot,
                             rxd_global_tfi.uplink_tfi,global_data_ptr->gmac_local_ul_ts_alloc);
        }
        else
        {
          tfi_match = TRUE;
        }
      }
    }
    if (rxd_global_tfi.direction == dwnlink_valid)
    {
      if ((rxd_global_tfi.dwnlink_tfi == global_data_ptr->gmac_local_tfi_store.dl_tfi) &&
          (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid))
      {
        if ( ( ((uint8)(0x80 >> global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot)) &
               global_data_ptr->gmac_local_dl_ts_alloc ) == 0 )
        {
           /* Ctrl msg with DL TFI is received on a PDCH that is NOT in the DL TBF */
           tfi_match = FALSE;
           MSG_GERAN_HIGH_3_G("GMAC DL TFI rxd on TS NOT in DL TBF, rxd TS:%d, DL TFI:%d, DL TS curr bmap:0x%x",
                              global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot,
                              rxd_global_tfi.dwnlink_tfi,global_data_ptr->gmac_local_dl_ts_alloc);
        }
        else
        {
          tfi_match = TRUE;
        }
      }
    }
  }
  else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
  {
    tfi_match = TRUE;
  }
  else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
  {
    tfi_match = FALSE;
  }

  return (tfi_match);
}

/*===========================================================================
===
===  FUNCTION      gmac_update_local_ta_params
===
===  DESCRIPTION
===  MAC stores TIMING ADVANCE parameters locally. Compare recently
===  received TA parameters with local store. If changed then update
===  local store.
===
===  DEPENDENCIES
===  gmac_local_l1_ta_params - local store for TA parameters
===
===  RETURN VALUE
===  - TRUE if parameters updated
===  - FALSE if no change
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_update_local_ta_params(pkt_timing_adv_t *pkt_ta_param_ptr,
                                    gas_id_t gas_id)
{
  /* See GSM 04.60 section 12.12
  ** - If TAV present then must inform L1 regardless of whether it was previously present.
  ** - If TAV not present, no change and therefore do not inform L1.
  ** - If TAI present and previously was not, start continuous TA and inform L1.
  ** - If TAI not present and previously was, stop continuous TA and inform L1.
  */
   boolean params_updated = FALSE;
   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

   if ((pkt_ta_param_ptr->valid_options == timing_value_valid) ||
       (pkt_ta_param_ptr->valid_options == all_timing_params_valid))
   {
     /* change in TAV */
     global_data_ptr->gmac_local_l1_ta_params.timing_adv_value = pkt_ta_param_ptr->timing_adv_value;
     params_updated = TRUE;
   }

   if (((global_data_ptr->gmac_local_l1_ta_params.valid_options == timing_indx_ts_valid) ||
        (global_data_ptr->gmac_local_l1_ta_params.valid_options == all_timing_params_valid)) &&
       ((pkt_ta_param_ptr->valid_options != timing_indx_ts_valid) ||
        (pkt_ta_param_ptr->valid_options != all_timing_params_valid)))
   {
     /* stop continuous TA */
     global_data_ptr->gmac_local_l1_ta_params.timing_adv_index = pkt_ta_param_ptr->timing_adv_index;
     global_data_ptr->gmac_local_l1_ta_params.timing_adv_timeslot = pkt_ta_param_ptr->timing_adv_timeslot;
     params_updated = TRUE;
   }

   if (((global_data_ptr->gmac_local_l1_ta_params.valid_options != timing_indx_ts_valid) ||
        (global_data_ptr->gmac_local_l1_ta_params.valid_options != all_timing_params_valid)) &&
       ((pkt_ta_param_ptr->valid_options == timing_indx_ts_valid) ||
        (pkt_ta_param_ptr->valid_options == all_timing_params_valid)))
   {
     /* start continuous TA */
     global_data_ptr->gmac_local_l1_ta_params.timing_adv_index = pkt_ta_param_ptr->timing_adv_index;
     global_data_ptr->gmac_local_l1_ta_params.timing_adv_timeslot = pkt_ta_param_ptr->timing_adv_timeslot;
     params_updated = TRUE;
   }
   global_data_ptr->gmac_local_l1_ta_params.valid_options = pkt_ta_param_ptr->valid_options;
   return(params_updated);
}


/*===========================================================================
===
===  FUNCTION      gmac_utils_calculate_ts_alloc_ws
===
===  DESCRIPTION
===  Calculate the window size of a timeslot allocation.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  num_ts - number of calculated timeslots
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint8 gmac_utils_calculate_ts_alloc_ws( uint8 ts_alloc,
                                        gas_id_t gas_id )
{
  uint8 i = 0;
  uint8 high_bit_pos = 0;
  uint8 low_bit_pos = 0;
  uint8 window_size = 0;

  if ( ts_alloc != 0 )
  {
    for ( i=0; i<8; i++ )
    {
      if ( ( ( ts_alloc << i ) & 0x80 ) == 0x80 )
      {
        high_bit_pos = 7 - i;
        break;
      }
    }

    for ( i=0; i<8; i++ )
    {
      if ( ( ( ts_alloc >> i ) & 0x01 ) == 0x01 )
      {
        low_bit_pos = i;
        break;
      }
    }

    window_size = ( high_bit_pos - low_bit_pos ) + 1;
  }

  return window_size;
}

/*===========================================================================
===
===  FUNCTION      gmac_utils_get_lowest_ts_allocated
===
===  DESCRIPTION
===  Returns the bit position of the lowest allocated TS.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  num_ts - number of calculated timeslots
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint8 gmac_utils_get_lowest_ts_allocated(uint8 ts_alloc,
                                         gas_id_t gas_id)
{
  uint8 i = 0;
  uint8 lowest_ts_pos = 0;

  if ( ts_alloc != 0 )
  {
    for ( i=0; i<8; i++ )
    {
      if ( ( ( ts_alloc << i ) & 0x80 ) == 0x80 )
      {
        lowest_ts_pos = i;
        break;
      }
    }
  }

  return lowest_ts_pos;

}

#ifdef FEATURE_GSM_GPRS_MSC33
/*===========================================================================
===
===  FUNCTION      gmac_utils_get_second_lowest_ts_allocated
===
===  DESCRIPTION
===  Returns the bit position of the second lowest allocated TS.  This function
===  should only be called when there are more than 1 timeslots in ts_alloc.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  num_ts - ts number of second lowest allocated timeslot.
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint8 gmac_utils_get_second_lowest_ts_allocated(uint8 ts_alloc,
                                                gas_id_t gas_id)
{
  uint8 i = 0;
  uint8 ts_pos = 0;
  boolean lowest_found = FALSE;

  if ( gmac_utils_calculate_nts(ts_alloc, gas_id) > 1 )
  {
    for ( i=0; i<8; i++ )
    {
      if ( ( ( ts_alloc << i ) & 0x80 ) == 0x80 )
      {
        if (lowest_found)
        {
          ts_pos = i;
          break;
        }
        else
        {
          lowest_found = TRUE;
        }
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("GMAC Error in Function, Less than 2 timeslots allocated");
  }

  return ts_pos;
}
#endif /*FEATURE_GSM_GPRS_MSC33*/


/*===========================================================================
===
===  FUNCTION      gmac_utils_calculate_nts
===
===  DESCRIPTION
===  Calculate the number of allocated timeslots from the given
===  timeslot allocation.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  num_ts - number of calculated timeslots
===
===  SIDE EFFECTS
===
===
===========================================================================*/
uint8 gmac_utils_calculate_nts(uint8 ts_alloc, gas_id_t gas_id)
{
  uint8 num_ts = 0;
  uint8 i;

  for (i=0; i<8; i++)
  {
    if (((ts_alloc >> i) & 0x01) == 0x01)
    {
      num_ts++;
    }
  }
  return (num_ts);
}

/*===========================================================================
===
===  FUNCTION      gmac_util_tbf_starting_ie
===
===  DESCRIPTION
===  ref 4.60 (chapter 12.21)
===
===  DEPENDENCIES
===  Calculated the starting time according to relative or absolute frame
===  number encoding.
===
===  - tbf_exist flag: If a TBF currently exists then if the starting time
===  has passed then this is not an error (as it is if TBF does not already
===  exist), hence the input flag.
===
===  RETURN VALUE
===  - TRUE  if staring time is error free
===  - FALSE if starting time calculation has failed
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_util_tbf_starting_ie(start_frame_number_t *start_time_ptr, uint32 *des_signal_data_ptr, boolean tbf_exist,
                                  gas_id_t gas_id)
{
  boolean result = FALSE;
  uint32  n =0;
  uint16  k =0;
  int8    t1;
  int8    t2;
  int8    t3;
  int32   starting_time_fn = 0;    /* ST received represented as mod 42432 of the absolute fn */
  int32   reception_fn_mod_42432;  /* absolute fn received covered to a mod 42432 format */

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /**********************************************************************
  * note the absoulte frame number recived in pkt request ref is a mod  *
  * 42432 repesentaion and is code as 4.08 10.5.2.38  "Starting Time"   *
  **********************************************************************/
  if(start_time_ptr->abs_rel_flag == absolute_valid)
  {
    t1 = (start_time_ptr->absolute) >> 11;
    t3 = (((start_time_ptr->absolute) & 0x07e0) >> 5);
    t2 = ((start_time_ptr->absolute) & 0x001f);

    if((t3 - t2) < 0)
    {
      starting_time_fn =(51* ((t3 - t2) + 26)) + (t3 + (51*26*t1));
    }
    else
    {
      starting_time_fn =(51* ((t3 - t2) % 26)) + (t3 + (51*26*t1));
    }

    /**********************************************************************
    * frame number the dl msg received on is the absolute frame number FN *
    * starting time frame number is in represented as (FN )mod 42432      *
    **********************************************************************/
    reception_fn_mod_42432 = ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no) % 42432);
    if((starting_time_fn - reception_fn_mod_42432) < 0)
    {
      if((((starting_time_fn - reception_fn_mod_42432) + 42432) >= 0)
          && (((starting_time_fn - reception_fn_mod_42432) + 42432) <= 31623))
      {
        /* starting time has NOT elapsed */
        *des_signal_data_ptr = (uint32)((starting_time_fn - reception_fn_mod_42432) + 42432)
                                                  + (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no);
        result = TRUE;
      }
      else if((((starting_time_fn - reception_fn_mod_42432) + 42432) >= 32024)
              && (((starting_time_fn - reception_fn_mod_42432) + 42432) <= 42431))
      {
        /* starting time has elapsed */
        if (tbf_exist)
        {
          *des_signal_data_ptr = (uint32)((starting_time_fn - reception_fn_mod_42432) + 42432)
                                                  + (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no);
          result = TRUE;
        }
        else
        {
          MSG_GERAN_ERROR_0_G("MAC error TBF starting time already occurred");
        }
      }
    }
    else
    {
      if( (((starting_time_fn - reception_fn_mod_42432) % 42432) <= 31623) )
      {
        /* starting time has NOT elapsed */
        *des_signal_data_ptr = ((uint32)(starting_time_fn - reception_fn_mod_42432) % 42432)
                                          + (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no);
        result = TRUE;
      }
      else if((((starting_time_fn - reception_fn_mod_42432) % 42432) >= 32024)
            && (((starting_time_fn - reception_fn_mod_42432) % 42432) <= 42431))
      {
        /* starting time has elapsed */
        if (tbf_exist)
        {
          /* starting time has NOT elapsed */
          *des_signal_data_ptr = ((uint32)(starting_time_fn - reception_fn_mod_42432) % 42432)
            + (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no);
          result = TRUE;
        }
        else
        {
          MSG_GERAN_ERROR_0_G("MAC error TBF starting time already occurred");
        }
      }
    }
  }
  else
  {
    /* relative fn encoding */
    /* if the starting time corresponds to an idle frame l1 */
    /* add 1 to the staring time                            */
    n = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no;
    k = start_time_ptr->relative;

    if(k != 0)
    {
      if (((k % 3) == 0) || ((k % 3) == 1))
      {
        *des_signal_data_ptr = (n + 4 + 4*k + k/3);
      }
      else
      {
        *des_signal_data_ptr = (n + 5 + 4*k + k/3);
      }
      result = TRUE;
    }
    else
    {
#ifdef PERLUTF
      #error code not present
#else
      MSG_GERAN_ERROR_0_G("MAC error TBF starting time");
#endif /*PERLUTF*/
    }
  }
  return (result);
}

/*===========================================================================
===
===  FUNCTION      gmac_global_dynamic_alloc_ie()
===
===  DESCRIPTION
===
===  loads MAC_L1_DYNAMIC_TBF_CONFIG with
===  dl power control parmas (po and pr_mode )
===  timeslot assignment
===  USF alloc,
===  usf granularity
===  ul power control params alpha and gamma where applicable.
===
===  DEPENDENCIES
===
===  Requires the signal pointer to the MAC_L1_DYNAMIC_TBF_CONFIG signal
===  The dl msg data after its been decoded by csn.1
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_global_dynamic_alloc_ie(mac_dynamic_tbf_information_T *ul_tbf_msg_ptr,
                                  packet_uplink_assignment_t    *pua_ptr,
                                  gas_id_t                       gas_id)
{
  dynamic_alloc_t  *dyn_alloc_ptr;
  dyn_alloc_ptr = &(pua_ptr->dynamic_alloc);

  /* intialise structures */
  memset ( ul_tbf_msg_ptr->usf, 0, sizeof(ul_tbf_msg_ptr->usf) );
  memset ( &ul_tbf_msg_ptr->power_control_params, 0, sizeof(ul_tbf_msg_ptr->power_control_params));
  memset ( &ul_tbf_msg_ptr->dl_ctrl_param, 0, sizeof(ul_tbf_msg_ptr->dl_ctrl_param));

#ifdef FEATURE_GSM_DTM
  /* dynamic alloc type*/
  if( dyn_alloc_ptr->ext_dynamic_alloc == GMAC_EDA_IND_DYNAMIC_ALLOC )
  {
    ul_tbf_msg_ptr->mac_mode = MAC_L1_MAC_MODE_DYNAMIC;
  }
  else
  {
    ul_tbf_msg_ptr->mac_mode = MAC_L1_MAC_MODE_EXTENDED_DYNAMIC;
  }
#endif /*FEATURE_GSM_DTM*/

  if (dyn_alloc_ptr->po_prmode_flag == valid)
  {
    ul_tbf_msg_ptr->dl_ctrl_param.params_valid = TRUE;
    ul_tbf_msg_ptr->dl_ctrl_param.po = dyn_alloc_ptr->po;
    ul_tbf_msg_ptr->dl_ctrl_param.pr_mode = dyn_alloc_ptr->pr_mode;
  }
  else
  {
    ul_tbf_msg_ptr->dl_ctrl_param.params_valid = FALSE;
  }

  ul_tbf_msg_ptr->usf_granularity = dyn_alloc_ptr->usf_granularity;

  if (dyn_alloc_ptr->timeslot_alloc_flag == usf_only)
  {
    ul_tbf_msg_ptr->power_control_params.present = FALSE;

    if (dyn_alloc_ptr->usf_bitmap & 0x01)
    {
      ul_tbf_msg_ptr->usf[0].present = TRUE;
      ul_tbf_msg_ptr->usf[0].usf_value = dyn_alloc_ptr->usf_tn0;
    }
    else
    {
      ul_tbf_msg_ptr->usf[0].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x02)
    {
      ul_tbf_msg_ptr->usf[1].present = TRUE;
      ul_tbf_msg_ptr->usf[1].usf_value = dyn_alloc_ptr->usf_tn1;
    }
    else
    {
      ul_tbf_msg_ptr->usf[1].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x04)
    {
      ul_tbf_msg_ptr->usf[2].present = TRUE;
      ul_tbf_msg_ptr->usf[2].usf_value = dyn_alloc_ptr->usf_tn2;
    }
    else
    {
      ul_tbf_msg_ptr->usf[2].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x08)
    {
      ul_tbf_msg_ptr->usf[3].present = TRUE;
      ul_tbf_msg_ptr->usf[3].usf_value = dyn_alloc_ptr->usf_tn3;
    }
    else
    {
      ul_tbf_msg_ptr->usf[3].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x10)
    {
      ul_tbf_msg_ptr->usf[4].present = TRUE;
      ul_tbf_msg_ptr->usf[4].usf_value = dyn_alloc_ptr->usf_tn4;
    }
    else
    {
      ul_tbf_msg_ptr->usf[4].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x20)
    {
      ul_tbf_msg_ptr->usf[5].present = TRUE;
      ul_tbf_msg_ptr->usf[5].usf_value = dyn_alloc_ptr->usf_tn5;
    }
    else
    {
      ul_tbf_msg_ptr->usf[5].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x40)
    {
      ul_tbf_msg_ptr->usf[6].present = TRUE;
      ul_tbf_msg_ptr->usf[6].usf_value = dyn_alloc_ptr->usf_tn6;
    }
    else
    {
      ul_tbf_msg_ptr->usf[6].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x80)
    {
      ul_tbf_msg_ptr->usf[7].present = TRUE;
      ul_tbf_msg_ptr->usf[7].usf_value = dyn_alloc_ptr->usf_tn7;
    }
    else
    {
      ul_tbf_msg_ptr->usf[7].present = FALSE;
    }
  }
  else
  {
    ul_tbf_msg_ptr->power_control_params.present = TRUE;
    ul_tbf_msg_ptr->power_control_params.alpha = dyn_alloc_ptr->alpha;

    if (dyn_alloc_ptr->usf_bitmap & 0x01)
    {
      ul_tbf_msg_ptr->usf[0].present = TRUE;
      ul_tbf_msg_ptr->usf[0].usf_value = dyn_alloc_ptr->usf_tn0;
      ul_tbf_msg_ptr->power_control_params.gamma_TN0.present = TRUE;
      ul_tbf_msg_ptr->power_control_params.gamma_TN0.gamma = dyn_alloc_ptr->gamma_tn0;
    }
    else
    {
      ul_tbf_msg_ptr->power_control_params.gamma_TN0.present = FALSE;
      ul_tbf_msg_ptr->usf[0].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x02)
    {
      ul_tbf_msg_ptr->usf[1].present = TRUE;
      ul_tbf_msg_ptr->usf[1].usf_value = dyn_alloc_ptr->usf_tn1;
      ul_tbf_msg_ptr->power_control_params.gamma_TN1.present = TRUE;
      ul_tbf_msg_ptr->power_control_params.gamma_TN1.gamma = dyn_alloc_ptr->gamma_tn1;
    }
    else
    {
      ul_tbf_msg_ptr->power_control_params.gamma_TN1.present = FALSE;
      ul_tbf_msg_ptr->usf[1].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x04)
    {
      ul_tbf_msg_ptr->usf[2].present = TRUE;
      ul_tbf_msg_ptr->usf[2].usf_value = dyn_alloc_ptr->usf_tn2;
      ul_tbf_msg_ptr->power_control_params.gamma_TN2.present = TRUE;
      ul_tbf_msg_ptr->power_control_params.gamma_TN2.gamma = dyn_alloc_ptr->gamma_tn2;
    }
    else
    {
      ul_tbf_msg_ptr->power_control_params.gamma_TN2.present = FALSE;
      ul_tbf_msg_ptr->usf[2].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x08)
    {
      ul_tbf_msg_ptr->usf[3].present = TRUE;
      ul_tbf_msg_ptr->usf[3].usf_value = dyn_alloc_ptr->usf_tn3;
      ul_tbf_msg_ptr->power_control_params.gamma_TN3.present = TRUE;
      ul_tbf_msg_ptr->power_control_params.gamma_TN3.gamma = dyn_alloc_ptr->gamma_tn3;
    }
    else
    {
      ul_tbf_msg_ptr->power_control_params.gamma_TN3.present = FALSE;
      ul_tbf_msg_ptr->usf[3].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x10)
    {
      ul_tbf_msg_ptr->usf[4].present = TRUE;
      ul_tbf_msg_ptr->usf[4].usf_value = dyn_alloc_ptr->usf_tn4;
      ul_tbf_msg_ptr->power_control_params.gamma_TN4.present = TRUE;
      ul_tbf_msg_ptr->power_control_params.gamma_TN4.gamma = dyn_alloc_ptr->gamma_tn4;
    }
    else
    {
      ul_tbf_msg_ptr->power_control_params.gamma_TN4.present = FALSE;
      ul_tbf_msg_ptr->usf[4].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x20)
    {
      ul_tbf_msg_ptr->usf[5].present = TRUE;
      ul_tbf_msg_ptr->usf[5].usf_value = dyn_alloc_ptr->usf_tn5;
      ul_tbf_msg_ptr->power_control_params.gamma_TN5.present = TRUE;
      ul_tbf_msg_ptr->power_control_params.gamma_TN5.gamma = dyn_alloc_ptr->gamma_tn5;
    }
    else
    {
      ul_tbf_msg_ptr->power_control_params.gamma_TN5.present = FALSE;
      ul_tbf_msg_ptr->usf[5].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x40)
    {
      ul_tbf_msg_ptr->usf[6].present = TRUE;
      ul_tbf_msg_ptr->usf[6].usf_value = dyn_alloc_ptr->usf_tn6;
      ul_tbf_msg_ptr->power_control_params.gamma_TN6.present = TRUE;
      ul_tbf_msg_ptr->power_control_params.gamma_TN6.gamma = dyn_alloc_ptr->gamma_tn6;
    }
    else
    {
      ul_tbf_msg_ptr->power_control_params.gamma_TN6.present = FALSE;
      ul_tbf_msg_ptr->usf[6].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x80)
    {
      ul_tbf_msg_ptr->usf[7].present = TRUE;
      ul_tbf_msg_ptr->usf[7].usf_value = dyn_alloc_ptr->usf_tn7;
      ul_tbf_msg_ptr->power_control_params.gamma_TN7.present = TRUE;
      ul_tbf_msg_ptr->power_control_params.gamma_TN7.gamma = dyn_alloc_ptr->gamma_tn7;
    }
    else
    {
      ul_tbf_msg_ptr->power_control_params.gamma_TN7.present = FALSE;
      ul_tbf_msg_ptr->usf[7].present = FALSE;
    }
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_pkt_tmslt_reconfig_dynamic_alloc_ie()
===
===  DESCRIPTION
===
===  loads MAC_L1_TIMESLOT_RECONFIGURE with
===  dl power control parmas (po and pr_mode )
===  timeslot assignment
===  USF alloc,
===  usf granularity
===  ul power control params alpha and gamma where applicable.
===
===  DEPENDENCIES
===
===  Requires the signal pointer to the MAC_L1_TIMESLOT_RECONFIGURE signal
===  The dl msg data after its been decoded by csn.1
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_pkt_tmslt_reconfig_dynamic_alloc_ie(mac_l1_ts_reconfig_t *ts_reconfig_ptr,
                                                 gas_id_t gas_id)
{
  boolean result = TRUE;

  dynamic_alloc_t  *dyn_alloc_ptr;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  dyn_alloc_ptr = &(((packet_timeslot_reconfigure_t*)global_data_ptr->csn_dec_data_ptr)->dynamic_alloc);

  if (dyn_alloc_ptr->po_prmode_flag == valid)
  {
    ts_reconfig_ptr->dl_ctrl_param.params_valid = TRUE;
    ts_reconfig_ptr->dl_ctrl_param.po = dyn_alloc_ptr->po;
    ts_reconfig_ptr->dl_ctrl_param.pr_mode = dyn_alloc_ptr->pr_mode;
  }
  else
  {
    ts_reconfig_ptr->dl_ctrl_param.params_valid = FALSE;
  }

  if (dyn_alloc_ptr->tbf_start_time_flag == valid )
  {
   if (gmac_util_tbf_starting_ie(&(dyn_alloc_ptr->tbf_start_frm_num), &(ts_reconfig_ptr->starting_time), TRUE, gas_id))
   {
     ts_reconfig_ptr->starting_time_present = TRUE;
   }
   else
   {
     result = FALSE;
   }
  }

  ts_reconfig_ptr->usf_granularity = dyn_alloc_ptr->usf_granularity;

  if (dyn_alloc_ptr->timeslot_alloc_flag == usf_only)
  {
    ts_reconfig_ptr->power_control_params.present = FALSE;

    if (dyn_alloc_ptr->usf_bitmap & 0x01)
    {
      ts_reconfig_ptr->usf[0].present = TRUE;
      ts_reconfig_ptr->usf[0].usf_value = dyn_alloc_ptr->usf_tn0;
    }
    else
    {
      ts_reconfig_ptr->usf[0].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x02)
    {
      ts_reconfig_ptr->usf[1].present = TRUE;
      ts_reconfig_ptr->usf[1].usf_value = dyn_alloc_ptr->usf_tn1;
    }
    else
    {
      ts_reconfig_ptr->usf[1].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x04)
    {
      ts_reconfig_ptr->usf[2].present = TRUE;
      ts_reconfig_ptr->usf[2].usf_value = dyn_alloc_ptr->usf_tn2;
    }
    else
    {
      ts_reconfig_ptr->usf[2].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x08)
    {
      ts_reconfig_ptr->usf[3].present = TRUE;
      ts_reconfig_ptr->usf[3].usf_value = dyn_alloc_ptr->usf_tn3;
    }
    else
    {
      ts_reconfig_ptr->usf[3].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x10)
    {
      ts_reconfig_ptr->usf[4].present = TRUE;
      ts_reconfig_ptr->usf[4].usf_value = dyn_alloc_ptr->usf_tn4;
    }
    else
    {
      ts_reconfig_ptr->usf[4].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x20)
    {
      ts_reconfig_ptr->usf[5].present = TRUE;
      ts_reconfig_ptr->usf[5].usf_value = dyn_alloc_ptr->usf_tn5;
    }
    else
    {
      ts_reconfig_ptr->usf[5].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x40)
    {
      ts_reconfig_ptr->usf[6].present = TRUE;
      ts_reconfig_ptr->usf[6].usf_value = dyn_alloc_ptr->usf_tn6;
    }
    else
    {
      ts_reconfig_ptr->usf[6].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x80)
    {
      ts_reconfig_ptr->usf[7].present = TRUE;
      ts_reconfig_ptr->usf[7].usf_value = dyn_alloc_ptr->usf_tn7;
    }
    else
    {
      ts_reconfig_ptr->usf[7].present = FALSE;
    }
  }
  else
  {
    ts_reconfig_ptr->power_control_params.present = TRUE;
    ts_reconfig_ptr->power_control_params.alpha = dyn_alloc_ptr->alpha;

    if (dyn_alloc_ptr->usf_bitmap & 0x01)
    {
      ts_reconfig_ptr->usf[0].present = TRUE;
      ts_reconfig_ptr->usf[0].usf_value = dyn_alloc_ptr->usf_tn0;
      ts_reconfig_ptr->power_control_params.gamma_TN0.present = TRUE;
      ts_reconfig_ptr->power_control_params.gamma_TN0.gamma = dyn_alloc_ptr->gamma_tn0;
    }
    else
    {
      ts_reconfig_ptr->power_control_params.gamma_TN0.present = FALSE;
      ts_reconfig_ptr->usf[0].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x02)
    {
      ts_reconfig_ptr->usf[1].present = TRUE;
      ts_reconfig_ptr->usf[1].usf_value = dyn_alloc_ptr->usf_tn1;
      ts_reconfig_ptr->power_control_params.gamma_TN1.present = TRUE;
      ts_reconfig_ptr->power_control_params.gamma_TN1.gamma = dyn_alloc_ptr->gamma_tn1;
    }
    else
    {
      ts_reconfig_ptr->power_control_params.gamma_TN1.present = FALSE;
      ts_reconfig_ptr->usf[1].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x04)
    {
      ts_reconfig_ptr->usf[2].present = TRUE;
      ts_reconfig_ptr->usf[2].usf_value = dyn_alloc_ptr->usf_tn2;
      ts_reconfig_ptr->power_control_params.gamma_TN2.present = TRUE;
      ts_reconfig_ptr->power_control_params.gamma_TN2.gamma = dyn_alloc_ptr->gamma_tn2;
    }
    else
    {
      ts_reconfig_ptr->power_control_params.gamma_TN2.present = FALSE;
      ts_reconfig_ptr->usf[2].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x08)
    {
      ts_reconfig_ptr->usf[3].present = TRUE;
      ts_reconfig_ptr->usf[3].usf_value = dyn_alloc_ptr->usf_tn3;
      ts_reconfig_ptr->power_control_params.gamma_TN3.present = TRUE;
      ts_reconfig_ptr->power_control_params.gamma_TN3.gamma = dyn_alloc_ptr->gamma_tn3;
    }
    else
    {
      ts_reconfig_ptr->power_control_params.gamma_TN3.present = FALSE;
      ts_reconfig_ptr->usf[3].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x10)
    {
      ts_reconfig_ptr->usf[4].present = TRUE;
      ts_reconfig_ptr->usf[4].usf_value = dyn_alloc_ptr->usf_tn4;
      ts_reconfig_ptr->power_control_params.gamma_TN4.present = TRUE;
      ts_reconfig_ptr->power_control_params.gamma_TN4.gamma = dyn_alloc_ptr->gamma_tn4;
    }
    else
    {
      ts_reconfig_ptr->power_control_params.gamma_TN4.present = FALSE;
      ts_reconfig_ptr->usf[4].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x20)
    {
      ts_reconfig_ptr->usf[5].present = TRUE;
      ts_reconfig_ptr->usf[5].usf_value = dyn_alloc_ptr->usf_tn5;
      ts_reconfig_ptr->power_control_params.gamma_TN5.present = TRUE;
      ts_reconfig_ptr->power_control_params.gamma_TN5.gamma = dyn_alloc_ptr->gamma_tn5;
    }
    else
    {
      ts_reconfig_ptr->power_control_params.gamma_TN5.present = FALSE;
      ts_reconfig_ptr->usf[5].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x40)
    {
      ts_reconfig_ptr->usf[6].present = TRUE;
      ts_reconfig_ptr->usf[6].usf_value = dyn_alloc_ptr->usf_tn6;
      ts_reconfig_ptr->power_control_params.gamma_TN6.present = TRUE;
      ts_reconfig_ptr->power_control_params.gamma_TN6.gamma = dyn_alloc_ptr->gamma_tn6;
    }
    else
    {
      ts_reconfig_ptr->power_control_params.gamma_TN6.present = FALSE;
      ts_reconfig_ptr->usf[6].present = FALSE;
    }
    if (dyn_alloc_ptr->usf_bitmap & 0x80)
    {
      ts_reconfig_ptr->usf[7].present = TRUE;
      ts_reconfig_ptr->usf[7].usf_value = dyn_alloc_ptr->usf_tn7;
      ts_reconfig_ptr->power_control_params.gamma_TN7.present = TRUE;
      ts_reconfig_ptr->power_control_params.gamma_TN7.gamma = dyn_alloc_ptr->gamma_tn7;
    }
    else
    {
      ts_reconfig_ptr->power_control_params.gamma_TN7.present = FALSE;
      ts_reconfig_ptr->usf[7].present = FALSE;
    }
  }
  return (result);
}

/*===========================================================================
===
===  FUNCTION      gmac_util_multislot_class_check()
===
===  DESCRIPTION
===  Performs a dynamic multislot class check for all combinations of UL,DL and UL+DL.
===  If DTM MS class checking is to be performed then the dtm_ms_class_check should
===  be set to TRUE and the cs_ts_alloc populated with the ts bitmap of the CS timeslot.
===  If eda is set to TRUE then the function performs MS class checking with Extended
===  Dynamic Allocation.
===
===  ul_ts_alloc, dl_ts_alloc, cs_ts_alloc are all in a timeslot bitmap format:
===  MSB TS 0 -->  LSB TS 7
===
===
===  PARAMS
===    ul_ts_alloc  - UL TS allocation in ts bitmap format
===    dl_ts_alloc  - DL TS allocation in ts bitmap format
===    eda          - Indication of Extended Dynamic Alloc (0 - OFF, 1 - ON )
===    cs_ts_alloc  - CS TS allocation in ts bitmap format
===    dtm_ms_class_check - TRUE - include cs_ts_alloc
===
===  RETURN VALUE
===  - TRUE if multislot class check succeeds
===  - FALSE for multislot class violation
===
===  SIDE EFFECTS
===   Sets the following globals if the current mac_mode is TRANSFER_MODE:
===     gmac_local_ul_realloc_ts
===     gmac_local_ul_realloc_dl_ctrl_ts
===     gmac_local_dl_reassign_ts
===
===   Otherwise if the mac_mode is NULL_MODE or IDLE_MODE set globals:
===     gmac_local_dl_ts_alloc
===     gmac_local_ul_ts_alloc
===     gmac_local_ul_dl_ctrl_ts
===
===
===========================================================================*/

#ifdef FEATURE_GPRS_PS_HANDOVER
boolean gmac_util_multislot_class_check(uint8   ul_ts_alloc,
                                        uint8   dl_ts_alloc,
                                        uint8   eda,
                                        uint8   cs_ts_alloc,
                                        boolean dtm_ms_class_check,
                                        gmac_psho_update_params_reqd_t psho_update_params_reqd,
                                        gas_id_t gas_id)
#else
boolean gmac_util_multislot_class_check(uint8   ul_ts_alloc,
                                        uint8   dl_ts_alloc,
                                        uint8   eda,
                                        uint8   cs_ts_alloc,
                                        boolean dtm_ms_class_check,
                                        gas_id_t gas_id)
#endif /*FEATURE_GPRS_PS_HANDOVER*/
{
  /* Local Variables*/
  boolean result = FALSE;

#ifdef FEATURE_GSM_DTM
  uint8   num_cs_ts;
#endif /*FEATURE_GSM_DTM*/

  uint16  ul_ts, dl_ts;
  uint8   total_dl_ts_alloc = 0;

  uint8 local_ms_class_sum = 0;
  uint8 local_ms_class_max_ts_tx = 0;
  uint8 local_ms_class_max_ts_rx = 0;
  uint8 local_ms_class_ts_tta = 0;
  uint8 local_ms_class_ts_ttb = 0;
  uint8 local_ms_class_ts_tra = 0;
  uint8 local_ms_class_ts_trb = 0;
#ifdef FEATURE_GSM_GPRS_MSC33
  uint8 local_ms_class = 0;
#endif /*FEATURE_GSM_GPRS_MSC33*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /*Local Defines*/
  #define UL_EXT_DYN_ALLOC_NUM_TS 4

  MSG_GERAN_HIGH_3_G("GMAC MS class check, ul_ts_alloc:0x%x, dl_ts_alloc:0x%x, cs_ts_alloc:0x%x",ul_ts_alloc,dl_ts_alloc,cs_ts_alloc);

#ifdef FEATURE_GSM_DTM
  if ( dtm_ms_class_check == TRUE )
  {
    /* DTM multislot class*/
    local_ms_class_sum = global_data_ptr->gmac_dtm_ms_class_sum;
    local_ms_class_max_ts_tx = global_data_ptr->gmac_dtm_ms_class_max_ts_tx;
    local_ms_class_max_ts_rx = global_data_ptr->gmac_dtm_ms_class_max_ts_rx;
    local_ms_class_ts_tta = global_data_ptr->gmac_dtm_ms_class_ts_tta;
    local_ms_class_ts_ttb = global_data_ptr->gmac_dtm_ms_class_ts_ttb;
    local_ms_class_ts_tra = global_data_ptr->gmac_dtm_ms_class_ts_tra;
    local_ms_class_ts_trb = global_data_ptr->gmac_dtm_ms_class_ts_trb;
  }
  else
#endif /*FEATURE_GSM_DTM*/
  {
    /* EGPRS multislot class */
    if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
    {
      MSG_GERAN_MED_0_G("EGPRS_MULTISLOT checking");

#ifdef FEATURE_GSM_GPRS_MSC33
      local_ms_class = global_data_ptr->gmac_egprs_ms_class;
#endif /*FEATURE_GSM_GPRS_MSC33*/
      local_ms_class_sum = global_data_ptr->gmac_egprs_ms_class_sum;
      local_ms_class_max_ts_tx = global_data_ptr->gmac_egprs_ms_class_max_ts_tx;
      local_ms_class_max_ts_rx = global_data_ptr->gmac_egprs_ms_class_max_ts_rx;
      local_ms_class_ts_tta = global_data_ptr->gmac_egprs_ms_class_ts_tta;
      local_ms_class_ts_ttb = global_data_ptr->gmac_egprs_ms_class_ts_ttb;
      local_ms_class_ts_tra = global_data_ptr->gmac_egprs_ms_class_ts_tra;
      local_ms_class_ts_trb = global_data_ptr->gmac_egprs_ms_class_ts_trb;
    }
    else  /* GPRS multislot class */
    {
#ifdef FEATURE_GSM_GPRS_MSC33
      local_ms_class = global_data_ptr->gmac_ms_class;
#endif /*FEATURE_GSM_GPRS_MSC33*/
      local_ms_class_sum = global_data_ptr->gmac_ms_class_sum;
      local_ms_class_max_ts_tx = global_data_ptr->gmac_ms_class_max_ts_tx;
      local_ms_class_max_ts_rx = global_data_ptr->gmac_ms_class_max_ts_rx;
      local_ms_class_ts_tta = global_data_ptr->gmac_ms_class_ts_tta;
      local_ms_class_ts_ttb = global_data_ptr->gmac_ms_class_ts_ttb;
      local_ms_class_ts_tra = global_data_ptr->gmac_ms_class_ts_tra;
      local_ms_class_ts_trb = global_data_ptr->gmac_ms_class_ts_trb;
    }
  }

#ifdef FEATURE_GSM_DTM
  if ( dtm_ms_class_check == TRUE )
  {
    /* Check that there is only one CS TS*/
    num_cs_ts = gmac_utils_calculate_nts(cs_ts_alloc,gas_id);

    if ( num_cs_ts != 1 )
    {
      MSG_GERAN_HIGH_2_G("MAC ERR MS Class, Err in number of CS TS, cs_ts_alloc:0x%x, num_of_cs_ts:%d",cs_ts_alloc,num_cs_ts);

      return (result);
    }

    /* Check that the same UL TS has not been allocated to CS and PS */
    if ( ( ul_ts_alloc & cs_ts_alloc ) != 0 )
    {
      MSG_GERAN_HIGH_3_G("MAC ERR MS Class, CS+PS UL TS Clash, UL slot alloc:0x%x, DL slot alloc:0x%x, CS TS alloc:0x%x",
                         ul_ts_alloc,dl_ts_alloc,cs_ts_alloc);

      return (result);
    }

    /* Check that the same DL TS has not been allocated to CS and PS */
    if ( ( dl_ts_alloc & cs_ts_alloc ) != 0 )
    {
      MSG_GERAN_HIGH_3_G("MAC ERR MS Class, CS+PS DL TS Clash, UL slot alloc:0x%x, DL slot alloc:0x%x, CS TS alloc:0x%x",
                         ul_ts_alloc,dl_ts_alloc,cs_ts_alloc);

      return (result);
    }
  }
#endif /*FEATURE_GSM_DTM*/

  if (eda == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC)
  {
#ifdef FEATURE_GSM_GPRS_MSC33
    /* Assignments with UL window size of 5 have to use shifted USF */
    if (gmac_utils_calculate_ts_alloc_ws(ul_ts_alloc,gas_id) == GMAC_SHIFTED_USF_NUM_OF_UL_TS)
    {
      /* Shifted USF, monitor is on second lowest timeslot */
      total_dl_ts_alloc = 0x80 >> gmac_utils_get_second_lowest_ts_allocated(ul_ts_alloc,gas_id);
  }
  else
#endif /*FEATURE_GSM_GPRS_MSC33*/
  {
    if (ul_ts_alloc != 0)
    {
          /* Extended Dynamic, monitor is on the lowest timeslot */
      total_dl_ts_alloc = 0x80 >> gmac_utils_get_lowest_ts_allocated(ul_ts_alloc,gas_id);
    }
  }

    /* Add allocated DL ts into total dl ts */
    total_dl_ts_alloc |= dl_ts_alloc;

    if ( dtm_ms_class_check == TRUE )
    {
      /* put CS slot into the allocation */
      ul_ts_alloc |= cs_ts_alloc;
      dl_ts_alloc |= cs_ts_alloc;

      /* add CS timeslot into total dl ts*/
      total_dl_ts_alloc |= cs_ts_alloc;
    }
  }
  else /*Extended Dynamic Allocation Check*/
  {
    if ( dtm_ms_class_check == TRUE )
    {
      /* put CS slot into the allocation */
      ul_ts_alloc |= cs_ts_alloc;
      dl_ts_alloc |= cs_ts_alloc;
    }

    total_dl_ts_alloc = (dl_ts_alloc | ul_ts_alloc);
  }


    /*TBD MIN RX check, L1 unsure of see Will/Osama*/
  /* num_dl_ts = gmac_utils_calculate_nts(dl_ts_alloc); */
  /*if ((num_ul_ts > num_dl_ts))
  {
    if ((boolean)MULTISLOT_CLASS_MIN_RX_CHECK(ul_ts_alloc,total_dl_ts_alloc) == FALSE)
    {
      MSG_GERAN_HIGH_2_G("MAC error MULTISLOT UL 0x%x DL 0x%xx",ul_ts_alloc,dl_ts_alloc);
      return (result);
    }
  }
  else
  {
    if ((boolean)MULTISLOT_CLASS_MIN_TX_CHECK(ul_ts_alloc,total_dl_ts_alloc) == FALSE)
    {
      MSG_GERAN_HIGH_2_G("MAC error MULTISLOT UL 0x%x DL 0x%x",ul_ts_alloc,dl_ts_alloc);
      return (result);
    }
  } */

  /***************************** Number of RX TS Check**********************************/
  if (gmac_utils_calculate_nts(total_dl_ts_alloc,gas_id) > local_ms_class_max_ts_rx)
  {
    MSG_GERAN_HIGH_2_G("MAC ERR MS Class, RX to large, DL slot alloc:0x%x, Allowed RX:%d",total_dl_ts_alloc,local_ms_class_max_ts_rx);
    return (result);
  }
  /*************************************************************************************/

  /***************************** Number of TX TS Check**********************************/
  if (gmac_utils_calculate_nts(ul_ts_alloc,gas_id) > local_ms_class_max_ts_tx)
  {
    MSG_GERAN_HIGH_2_G("MAC ERR MS Class, TX to large, UL slot alloc:0x%x, Allowed TX:%d",ul_ts_alloc,local_ms_class_max_ts_tx);
    return (result);
  }
  /*************************************************************************************/

  /***************************** RX Boundary check *************************************/
  if ( (total_dl_ts_alloc & GMAC_MS_BOUNDARY_CHECK) == GMAC_MS_BOUNDARY_CHECK)
  {
    MSG_GERAN_HIGH_1_G("MAC ERR MS Class, RX Boundary check, DL TS alloc:0x%x",total_dl_ts_alloc);
    return (result);
  }
  /*************************************************************************************/

  /***************************** TX Boundary check *************************************/
  if ( (ul_ts_alloc & GMAC_MS_BOUNDARY_CHECK) == GMAC_MS_BOUNDARY_CHECK)
  {
    MSG_GERAN_HIGH_1_G("MAC ERR MS Class, TX Boundary check, UL TS alloc:0x%x",ul_ts_alloc);
    return (result);
  }
  /*************************************************************************************/

  /***************************** RX Window Size Check **********************************/
  if ( gmac_utils_calculate_ts_alloc_ws(total_dl_ts_alloc,gas_id) > local_ms_class_max_ts_rx )
  {
    /* fail */
    MSG_GERAN_HIGH_2_G("MAC ERR MS Class, RX Window, Total DL TS alloc:0x%x, Allowed RX Window Size:%d",
                       total_dl_ts_alloc,local_ms_class_max_ts_rx);
    return (result);
  }
  /*************************************************************************************/

  /***************************** TX Window Size Check **********************************/
  if ( gmac_utils_calculate_ts_alloc_ws(ul_ts_alloc,gas_id) > local_ms_class_max_ts_tx )
  {
    /* fail */
    MSG_GERAN_HIGH_2_G("MAC ERR MS Class, TX Window, UL TS alloc:0x%x, Allowed TX Window size:%d",ul_ts_alloc,local_ms_class_max_ts_tx);
    return (result);
  }
  /*************************************************************************************/

  /***************************** SUM Check ****************************************/
  if ( ( gmac_utils_calculate_nts(ul_ts_alloc,gas_id) + gmac_utils_calculate_nts(total_dl_ts_alloc,gas_id) ) > local_ms_class_sum )
  {
    MSG_GERAN_HIGH_3_G("MAC ERR MS Class, in SUM, UL TS alloc:0x%x, DL TS alloc:0x%x,Allowed SUM:%d",
                       ul_ts_alloc,total_dl_ts_alloc,local_ms_class_sum);
    return (result);
  }
  /*************************************************************************************/

  /***************************** TS Clash Check ****************************************/
  ul_ts = ul_ts_alloc;
  ul_ts |= (uint16)ul_ts_alloc << 8;

  if (eda == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC )
  {
    /* Include the DL USF TS' in the clash calculation for EDA */
    dl_ts = (uint16)total_dl_ts_alloc << 3;
  }
  else
  {
  /* Don't include the DL USF TS' in the clash calculation */
  dl_ts = (uint16)dl_ts_alloc << 3;
  }

  /* check TS clash */
  if ( ( ul_ts & dl_ts ) != 0 )
  {
    MSG_GERAN_HIGH_2_G("MAC ERR MS Class, TS clash, UL TS alloc:0x%x, DL TS alloc:0x%x",ul_ts,dl_ts);
    return (result);
  }
  /*************************************************************************************/

  /****************************** TRA/B TTA/B Checks ***********************************/
  /* include dl USF detects in Tra/b Tta/b calculations*/
  dl_ts = (uint16)total_dl_ts_alloc << 3;

#ifdef FEATURE_GSM_GPRS_MSC33
  /* TTA check only applies to class 12 MS's with 4 UL TS.  Class 33-34 MS's with 4 UL TS use TRA check. */
  if ( ( gmac_utils_calculate_ts_alloc_ws(ul_ts_alloc,gas_id) == UL_EXT_DYN_ALLOC_NUM_TS ) &&
       ( local_ms_class == MULTISLOT_CLASS_12 ) )
#else
  if ( gmac_utils_calculate_ts_alloc_ws(ul_ts_alloc,gas_id) == UL_EXT_DYN_ALLOC_NUM_TS )
#endif /*FEATURE_GSM_GPRS_MSC33*/
  {
    /* check TRB */
    if (
       ((dl_ts & (ul_ts >> local_ms_class_ts_trb )) != 0 ) ||
       ((dl_ts & (ul_ts << local_ms_class_ts_trb )) != 0)
       )
    {
      MSG_GERAN_HIGH_3_G("MAC ERR MS Class, TRB failed, UL TS alloc:0x%x, DL TS alloc:0x%x, TRB:%d",ul_ts,dl_ts,local_ms_class_ts_trb);
      return (result);
    }

    /* Check TTA */
    if ( (dl_ts & (ul_ts << local_ms_class_ts_tta )) != 0 )
    {
      MSG_GERAN_HIGH_3_G("MAC ERR MS Class, TTA failed, UL TS alloc:0x%x, DL TS alloc:0x%x, TTA:%d",ul_ts,dl_ts,local_ms_class_ts_tta);
      return (result);
    }
  }
  else
  {
    /* Check TRA*/
    if ( (dl_ts & (ul_ts >> local_ms_class_ts_tra )) != 0 )
    {
       MSG_GERAN_HIGH_3_G("MAC ERR MS Class, TRA failed, UL TS alloc:0x%x, DL TS alloc:0x%x, TRA:%d",ul_ts,dl_ts,local_ms_class_ts_tra);
       return (result);
    }

    /* check TTB */
    if  (
        ((dl_ts & (ul_ts >> local_ms_class_ts_ttb )) != 0) ||
        ((dl_ts & (ul_ts << local_ms_class_ts_ttb )) != 0)
        )
    {
      MSG_GERAN_HIGH_3_G("MAC ERR MS Class, TTB failed, UL TS alloc:0x%x, DL TS alloc:0x%x, TTB:%d",ul_ts,dl_ts,local_ms_class_ts_ttb);
      return (result);
    }
  }
  /*************************************************************************************/

  if ( global_data_ptr->mac_mode == TRANSFER_MODE )
  {
    /* set up realloc variables when in TRANSFER mode*/
    /* These variables are used by the following macros:
         GMAC_UPDATE_UL_TS_REALLOC
         GMAC_UPDATE_DL_TS_REASS
         GMAC_UPDATE_UL_DLCTRL_TS_REALLOC

       to update the global ts variables:
         gmac_local_ul_ts_alloc
         gmac_local_ul_dl_ctrl_ts
         gmac_local_dl_ts_alloc

       when the PH_CONNECT_IND signal is received in Transfer */

    /*Remove CS ts before saving TS alloc*/
#ifdef FEATURE_GPRS_PS_HANDOVER
    if(psho_update_params_reqd == PSHO_UPDATE_PARAMS_REQUIRED)
    {
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_ts = ul_ts_alloc & (~cs_ts_alloc);
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_assign_ts = dl_ts_alloc & (~cs_ts_alloc);

      /* Set for legacy reasons.  Looks like this variable is never accessed*/
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_dl_ctrl_ts = ul_ts_alloc & (~cs_ts_alloc);
    }
    else
#endif /*FEATURE_GPRS_PS_HANDOVER*/
    {
      global_data_ptr->gmac_local_ul_realloc_ts = ul_ts_alloc & (~cs_ts_alloc);
      global_data_ptr->gmac_local_dl_reassign_ts = dl_ts_alloc & (~cs_ts_alloc);

      /* Set for legacy reasons.  Looks like this variable is never accessed*/
      global_data_ptr->gmac_local_ul_realloc_dl_ctrl_ts = ul_ts_alloc & (~cs_ts_alloc);
    }
  }
  else
  {
    {
      if ( global_data_ptr->mac_mode == IDLE_MODE )
      {
        if(idle_ul_realloc_in_progress(gas_id) == TRUE)
        {
          MSG_GERAN_MED_0_G("GMAC info Idle UL re-alloc ");
          /* only reallocating UL so don't need to set gmac_local_dl_ts_alloc */
          /* and gmac_local_ul_dl_ctrl_ts which is legacy var and is not used  */
          global_data_ptr->gmac_local_ul_realloc_ts = ul_ts_alloc & (~cs_ts_alloc);

          /*Don't update gmac_local_ul_ts_alloc as we are reallocating*/
          global_data_ptr->gmac_local_dl_ts_alloc   = dl_ts_alloc & (~cs_ts_alloc);
        }
        else
        {
          /* Set the global ts variables directly because we are */
          /* establishing a PS connection from NULL or IDLE      */
          /*Remove CS ts before saving TS alloc                  */
          global_data_ptr->gmac_local_dl_ts_alloc   = dl_ts_alloc & (~cs_ts_alloc);
          global_data_ptr->gmac_local_ul_ts_alloc   = ul_ts_alloc & (~cs_ts_alloc);
        }
      }

      if ( global_data_ptr->mac_mode == NULL_MODE )
      {
        /* Set the global ts variables directly because we are */
        /* establishing a PS connection from NULL or IDLE      */
        /*Remove CS ts before saving TS alloc                  */
        global_data_ptr->gmac_local_dl_ts_alloc   = dl_ts_alloc & (~cs_ts_alloc);
        global_data_ptr->gmac_local_ul_ts_alloc   = ul_ts_alloc & (~cs_ts_alloc);
      }
    }
  }

  result = TRUE;

  return (result);
}

/*===========================================================================
===
===  FUNCTION      gmac_send_prr
===
===  DESCRIPTION
===  Reallocation request from RLC results in sending a PACKET RESOURCE
===  REQUEST to the network. Compile the message, start T3168 then send
===  message to L1.
===
===  DEPENDENCIES
===  msg_ptr - received message from RLC
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/

void gmac_send_prr(idle_mode_ul_tbf_release_action_t re_est_action,
                   boolean                           idle_realloc_req,
                   gas_id_t                          gas_id)
{
  int_data_t          input_data;
  mac_l1_sig_t        ml_ph_data_sig;
  mac_l1_sig_t        ml_rrbp_sig;
  rrps_change_mark_t  change_mark;
  uint8               prr_encoding = FAIL;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  mac_ph_data_req_t   *data_req_msg = &(ml_ph_data_sig.mac_ph_data_req);

  packet_resource_request_t *prr_ptr = &(input_data.type.pkt_resource_req);

  mac_l1_pacch_rrbp_t   *rrbp_sig_ptr = &(ml_rrbp_sig.mac_l1_pacch_rrbp);

  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  /***************************************
  *   compile packet resource request    *
  ***************************************/

  if ( ( global_data_ptr->mac_mode == IDLE_MODE ) ||
       ( global_data_ptr->mac_mode == NULL_MODE ) )
  {
    /*PRR formt when sent from idle*/
    prr_ptr->access_type_flag = valid;


    /*conversion of RLC access requested */
    /*to access type used in PRR         */
    if(global_data_ptr->access_requested == two_phase)
    {
      prr_ptr->access_type = 0;
    }
    else if(global_data_ptr->access_requested == page_response)
    {
      prr_ptr->access_type = 1;
    }
    else if(global_data_ptr->access_requested == cell_update)
    {
      prr_ptr->access_type = 2;
    }
    else  if(global_data_ptr->access_requested == mm_proc)
    {
      prr_ptr->access_type = 3;
    }
    else if( (global_data_ptr->access_requested == short_acc) || (global_data_ptr->access_requested == one_phase) )
    {
      /*to handle the case where short access or 1 phase access are requested by rlc in 51 mode*/
      prr_ptr->access_type = 0;
    }
    else
    {
      /* Defaulted to two phase. This will catch the case were the requested access is 1-phase and the MS
         was forced to two-phase*/
      prr_ptr->access_type = 0;

      /*Access requested no supported for 2 phase access*/
      MSG_GERAN_HIGH_0_G("MAC Info error in 2p access type");
    }

    prr_ptr->tfi_tlli_flag = CSN_USE_TLLI;

    prr_ptr->tlli = global_data_ptr->gmac_store_ul_est_tlli;

    if(idle_realloc_req == TRUE)
    {
      /* Sending PRR as part of re allocation request rcvd during    *
      * contention  resolution resolution phase Don't include RAC IE *
      * in PRR ie act as if in transfer  4.60 7.1.2.3a               */
      MSG_GERAN_MED_0_G("GMAC info, resource realloc during cont res: RAC IE not included in PRR");
      prr_ptr->ms_radio_access_cap_flag = not_valid;
    }
    else
    {
      gmac_idle_egprs_rac_procedure_2(FALSE,FALSE,FALSE,TRUE,gas_id);

      if ( util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits == 0 )
      {
#ifdef PERLUTF
        #error code not present
#else
        MSG_GERAN_ERROR_0_G("GMAC info, RAC IE not included in PRR");
#endif /*PERLUTF*/
        prr_ptr->ms_radio_access_cap_flag = not_valid;
      }
      else
      {
        MSG_GERAN_HIGH_0_G("GMAC info, RAC IE included in PRR");
        prr_ptr->ms_radio_access_cap_flag = valid;

        prr_ptr->access_capability.ms_cap_len_bits = util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits;
        memscpy(prr_ptr->access_capability.ms_cap,sizeof(prr_ptr->access_capability.ms_cap),
                util_data_ptr->prr_ie.ms_ra_cap_ie, sizeof(util_data_ptr->prr_ie.ms_ra_cap_ie));
      }
    }

  }
  else if(global_data_ptr->mac_mode == TRANSFER_MODE)
  {
    /*PRR formt when sent from transfer*/
    prr_ptr->access_type_flag = not_valid;

    prr_ptr->tfi_tlli_flag = CSN_USE_GLOBAL_TFI;
    prr_ptr->global_tfi.direction = uplink_valid;
    prr_ptr->global_tfi.uplink_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;

    prr_ptr->ms_radio_access_cap_flag = not_valid;
  }

  prr_ptr->chan_req_desc = global_data_ptr->gmac_local_stored_ul_chan_req;

  rr_read_public_store(RRPS_CHANGE_MARK_SI13,&change_mark,gas_id);

  if (change_mark == RRPS_CHANGE_MARK_NOT_VALID)
  {
    prr_ptr->change_mark_flag = not_valid;
  }
  else
  {
    prr_ptr->change_mark_flag = valid;
    prr_ptr->change_mark = change_mark;
  }

  /*********************************
  *    add  QUALITY REPORT         *
  *********************************/

  prr_ptr->c_value = global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.c_value;

  /*ref 4.60 11.2.16*/
  if( ( global_data_ptr->mac_mode == IDLE_MODE ) ||
      ( global_data_ptr->mac_mode == NULL_MODE ) )
  {
    prr_ptr->sign_var_flag = not_valid;
  }
  else
  {
    if(global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.sign_var == GMAC_INVALID_SIGN_VAR)
    {
      prr_ptr->sign_var_flag = not_valid;
    }
    else
    {
      prr_ptr->sign_var_flag = valid;
      prr_ptr->sign_var = global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.sign_var;
    }
  }

  prr_ptr->i_level_tn_bitmap = 0x00;

  if (global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn0.i_lev_ind)
  {
    prr_ptr->i_level_tn[0] = global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn0.i_lev_tn;
    prr_ptr->i_level_tn_bitmap |= 0x01;
  }
  if (global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn1.i_lev_ind)
  {
    prr_ptr->i_level_tn[1] = global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn1.i_lev_tn;
    prr_ptr->i_level_tn_bitmap |= 0x02;
  }
  if (global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn2.i_lev_ind)
  {
    prr_ptr->i_level_tn[2] = global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn2.i_lev_tn;
    prr_ptr->i_level_tn_bitmap |= 0x04;
  }
  if (global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn3.i_lev_ind)
  {
    prr_ptr->i_level_tn[3] = global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn3.i_lev_tn;
    prr_ptr->i_level_tn_bitmap |= 0x08;
  }
  if (global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn4.i_lev_ind)
  {
    prr_ptr->i_level_tn[4] = global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn4.i_lev_tn;
    prr_ptr->i_level_tn_bitmap |= 0x10;
  }
  if (global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn5.i_lev_ind)
  {
    prr_ptr->i_level_tn[5] = global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn5.i_lev_tn;
    prr_ptr->i_level_tn_bitmap |= 0x20;
  }
  if (global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn6.i_lev_ind)
  {
    prr_ptr->i_level_tn[6] = global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn6.i_lev_tn;
    prr_ptr->i_level_tn_bitmap |= 0x40;
  }
  if (global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn7.i_lev_ind)
  {
    prr_ptr->i_level_tn[7] = global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.i_lev_meas.i_lev_tn7.i_lev_tn;
    prr_ptr->i_level_tn_bitmap |= 0x80;
  }

  /* R99 EXTENSION */

  /*release 99*/
  if ( global_data_ptr->pfi_present )
  {
    /* include rel 99 extension with contents of PFI only*/
    prr_ptr->include_egprs = TRUE;
    prr_ptr->egprs_bep_link_qual_present = FALSE;
    prr_ptr->egprs_ts_link_qual_present = FALSE;
    prr_ptr->pfi_present = TRUE;
    prr_ptr->pfi = global_data_ptr->pfi;
    prr_ptr->additional_rac_info = FALSE;
    prr_ptr->rac_retran = FALSE;
  }
  else
  {
    prr_ptr->include_egprs = FALSE;
  }




  /*********************************
  *      CSN encode PRR ul msg     *
  *********************************/

#ifdef PERLUTF
  #error code not present
#endif /*PERLUTF*/

  if((re_est_action == NO_RE_EST) || (re_est_action == RE_EST_WAIT_FOR_SB_ALLOC))
  {
    prr_encoding = gcsn1_encoder(PACKET_RESOURCE_REQUEST, &(data_req_msg->dl_ctrl_block[1]), &input_data);
    if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
    {
      data_req_msg->dl_ctrl_block[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
    }
    else
    {
      data_req_msg->dl_ctrl_block[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
    }

    /* call log-on-demand function */
    (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                     PACKET_RESOURCE_REQUEST,
                                     (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                     &(data_req_msg->dl_ctrl_block[1]),
                                      gas_id);

  }
  else if (re_est_action == RE_EST_SEND_PRR)
  {
    prr_encoding = gcsn1_encoder(PACKET_RESOURCE_REQUEST, &(rrbp_sig_ptr->pacch_ab_data[1]), &input_data);
    if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
    {
      rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
    }
    else
    {
      rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
    }

    /* call log-on-demand function */
    (void)mac_log_signalling_message(PACCH_RRBP_CHANNEL,
                                     PACKET_RESOURCE_REQUEST,
                                     (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                     &(rrbp_sig_ptr->pacch_ab_data[1]),
                                      gas_id);
  }

  /***************************************
  * send PRR either via SB alloc OR RRBP *
  ***************************************/

  if(prr_encoding == PASS)
  {

    if (global_data_ptr->mac_mode == TRANSFER_MODE)
    {
      /*Note if PRR sent from transfer then t3168 is started when first USF detected is rcvd*/
      /*after L1 has been loaded with the PRR msg and not at the point of load L1 with PRR  */
      MSG_GERAN_MED_0_G("MAC info PRR sent from Transfer");
    }
    else
    {
      /*event logging changes*/
      mac_log_prr_event(global_data_ptr->access_requested, gas_id);
    }

    if((re_est_action == NO_RE_EST) || (re_est_action == RE_EST_WAIT_FOR_SB_ALLOC))
    {
      MSG_GERAN_MED_0_G("MAC Sent MAC_PH_DATA_REQ with PRR");
#ifdef FEATURE_GSM_EDTM
      ml_ph_data_sig.mac_ph_data_req.message_type = PACKET_RESOURCE_REQUEST;
#endif /*FEATURE_GSM_EDTM*/
      ml_ph_data_sig.mac_ph_data_req.message_header.message_id = (uint8)MAC_PH_DATA_REQ;
      ml_ph_data_sig.mac_ph_data_req.message_header.message_set = MS_MAC_L1;
      gprs_l1_mac_acc_func(&ml_ph_data_sig, gas_id);
    }
    else /* re_est_action == RE_EST_SEND_PRR */
    {
      MSG_GERAN_MED_0_G( "MAC Sent MAC_L1_PACCH_RRBP with PRR");
      rrbp_sig_ptr->rrbp_msg = PRR_MSG;
      rrbp_sig_ptr->ts_of_dl_msg = global_data_ptr->prr_re_est_rrbp_ts;
      rrbp_sig_ptr->absolute_fn = global_data_ptr->prr_re_est_rrbp_fn;
      rrbp_sig_ptr->msg_burst_type = NB;

#ifdef FEATURE_GSM_EDTM
      rrbp_sig_ptr->message_type = PACKET_RESOURCE_REQUEST;
#endif /*FEATURE_GSM_EDTM*/

      ml_rrbp_sig.mac_ph_data_req.message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
      ml_rrbp_sig.mac_ph_data_req.message_header.message_set = MS_MAC_L1;
      gprs_l1_mac_acc_func(&ml_rrbp_sig, gas_id);
    }
  }
  else
  {
    if(geran_get_nv_recovery_restart_enabled(gas_id))
    {
      MSG_GERAN_ERROR_0_G("MAC error CSN.1 encode PRR");
      MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
      geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
    }
    else
    {
      ERR_GERAN_FATAL_0_G("MAC error CSN.1 encode PRR");
    }
  }
}

/*===========================================================================
===
===  FUNCTION    gmac_send_mac_grr_abnormal_release()
===
===  DESCRIPTION
===  Send MAC_GRR_ABNORMAL_RELEASE to GRR to indicate that MAC has been
===  released abnormally.
===
===  PARAMS
===
===    mac_release_cause: cause value
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_send_mac_grr_abnormal_release
(
   rr_abnormal_release_cause_t  mac_release_cause,
   gas_id_t                     gas_id
)
{
  mac_grr_sig_t mac_grr_sig;

  MSG_GERAN_MED_1_G("MAC Sent MAC_GRR_ABNORMAL_RELEASE mac_release_cause=%d", mac_release_cause);
  mac_grr_sig.mac_grr_abnormal_release.cause = mac_release_cause;
  mac_grr_sig.mac_grr_abnormal_release.message_header.message_set = MS_MAC_RR;
  mac_grr_sig.mac_grr_abnormal_release.message_header.message_id = (uint8)MAC_GRR_ABNORMAL_RELEASE;
  gprs_grr_mac_acc_func(&mac_grr_sig, gas_id);
}

/*===========================================================================
===
===  FUNCTION    gmac_util_send_mac_l1_release()
===
===  DESCRIPTION
===  Send MAC_L1_RELEASE to L1 to release its physical resources
===
===  PARAMS
===
===    tbf_release_desc: UL or|and DL TBF release indicator
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_mac_l1_release
(
    release_tbf_t                tbf_release_desc,
    gas_id_t                     gas_id
)
{
  mac_l1_sig_t         mac_l1_sig;      /* mac to l1 signal   */
  gmac_global_data_t  *global_data_ptr = gmac_get_global_data_ptr(gas_id);

#ifdef FEATURE_GSM_DTM
  if ( global_data_ptr->mac_mode == TRANSFER_MODE)
  {
    gmac_tran_rel_state_handle_rel_req( tbf_release_desc, gas_id );
  }
#endif /*FEATURE_GSM_DTM*/

  /*event logging changes*/
  mac_log_packet_tbf_rel_event( tbf_release_desc, gas_id );

  MSG_GERAN_MED_1_G("MAC Sent MAC_L1_RELEASE %d", tbf_release_desc);
  mac_l1_sig.mac_l1_release.release_tbf = tbf_release_desc;
  mac_l1_sig.mac_l1_release.message_header.message_set = MS_MAC_L1;
  mac_l1_sig.mac_l1_release.message_header.message_id = (uint8)MAC_L1_RELEASE;
  gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
}

/*===========================================================================
===
===  FUNCTION      gmac_util_send_dl_release_to_rlc
===
===  DESCRIPTION
===  Send access reject to RLC with signal MR_UL_RELEASE_IND.
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
void gmac_util_send_dl_release_to_rlc(gas_id_t gas_id)
{
  mac_rlc_msg_t  *mac_rlc_sig_ptr;

  mac_rlc_sig_ptr = grlc_get_cmd_buf_mac();
  if (mac_rlc_sig_ptr != NULL)
  {
    MSG_GERAN_MED_0_G("MAC Sent MR_DL_RELEASE_IND");
    mac_rlc_sig_ptr->msg.dl_release_ind.sig = MR_DL_RELEASE_IND;
    gmac_rlc_mac_insert_gas_id(mac_rlc_sig_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mac_rlc_sig_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_util_send_grr_reset_cnf()
===
===  DESCRIPTION
===  Send MAC_GRR_RESET_CNF to GRR to indicate that MAC has completed its
===  reset and is ready to recveive a camp on cell.
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
void gmac_util_send_grr_reset_cnf( gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Sent MAC_GRR_RESET_CNF %d");
  global_data_ptr->grr_sig.mac_grr_reset_cnf.message_header.message_set = MS_MAC_RR;
  global_data_ptr->grr_sig.mac_grr_reset_cnf.message_header.message_id = (uint8)MAC_GRR_RESET_CNF;

  gprs_grr_mac_acc_func(&global_data_ptr->grr_sig, gas_id);
}


/*===========================================================================
===
===  FUNCTION      gmac_util_send_grr_mac_status()
===
===  DESCRIPTION
===  Send MAC_GRR_MAC_STATUS to GRR to indicate the current MAC status.
===
===  DEPENDENCIES
===
===  global definition used by idle mode to store mac to grr signal .
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_grr_mac_status(rr_mac_status_t status,
                                   gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_1_G("MAC Sent MAC_GRR_MAC_STATUS %d",status);
  global_data_ptr->grr_sig.mac_grr_mac_status.mac_status = status;
  global_data_ptr->grr_sig.mac_grr_mac_status.message_header.message_set = MS_MAC_RR;
  global_data_ptr->grr_sig.mac_grr_mac_status.message_header.message_id = (uint8)MAC_GRR_MAC_STATUS;
  gprs_grr_mac_acc_func(&global_data_ptr->grr_sig,gas_id);
}

/*===========================================================================
===
===  FUNCTION      gmac_util_frequency_param_ie()
===
===  DESCRIPTION
===  Checks the frequency parameters received in an assignment message
===  and returns the appropriate value associated with the validity of
===  the frequency assignment
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - pointer to the CSN.1 decoded logical channel message
===
===  RETURN VALUE
===  Returns the error cause and recover type (pua_data_error_t):
===  - NO_ERRORS
===  - FREQ_ERROR_ACCESS_RETRY
===  - FREQ_ERROR_PARTIAL_SYS_ACQ
===
===  SIDE EFFECTS
===
===
===========================================================================*/
pua_data_error_t gmac_util_frequency_param_ie(boolean compare_flag,boolean send_freq_config,
                                              gas_id_t gas_id)
{
  pua_data_error_t return_value = NO_ERRORS;

  /* pointer "frequency list" held in GRR public store */
  rrps_frequency_list_t   *frequency_list_ptr;

  /* frequency params from CSN.1 decoded assigment msg */
  freq_params_t           *csn_freq_param_ptr;

  mac_l1_sig_t            freq_config_sig;
  frequency_information_T *ml_freq_config_info_ptr;
#ifdef FEATURE_GPRS_PS_HANDOVER
  ps_handover_command_t          *ps_handover_ptr       = NULL;
  ps_handover_radio_resources_t  *ps_handover_resources_ptr = NULL;
#endif /*FEATURE_GPRS_PS_HANDOVER*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  ml_freq_config_info_ptr = &(freq_config_sig.mac_l1_freq_config.frequency_information);

  /* retrieve frequency parameters from the dl assignment msg*/
  if(global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_UPLINK_ASSIGNMENT )
  {
    csn_freq_param_ptr = &(((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_params);
  }
  else if(global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_DOWNLINK_ASSIGNMENT)
  {
    csn_freq_param_ptr = &(((packet_downlink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_params);
  }
  else if(global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_TIMESLOT_RECONFIGURE)
  {
    csn_freq_param_ptr = &(((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_params);
  }
#ifdef FEATURE_GSM_EDTM
  else if(global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_CS_RELEASE)
  {
    csn_freq_param_ptr = &(((packet_cs_release_ind_t *)global_data_ptr->csn_dec_data_ptr)->freq_params);
  }
#endif /*FEATURE_GSM_EDTM*/
#ifdef FEATURE_GPRS_PS_HANDOVER
  else if(global_data_ptr->msg_ptr->grr_mac_sig.sig_id == GRR_MAC_PSHO_REQ)
  {
#ifdef PERLUTF
    #error code not present
#else
    ps_handover_ptr = &(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_psho_req.psho_cmd);
#endif
    ps_handover_resources_ptr = &(ps_handover_ptr->payload.a_gb.ps_handover_radio_resources);
    csn_freq_param_ptr = &(ps_handover_resources_ptr->freq_params);
  }
#endif /*FEATURE_GPRS_PS_HANDOVER*/
  else
  {
    /* unknown message type */
    return_value = FREQ_ERROR_PARTIAL_SYS_ACQ;
    return (return_value);
  }

  /********************************************************************************
  *       determine and load "MAC_L1_FREQ_CONFIG" signal data for the UL TBF      *
  *********************************************************************************/

  /* clear MAC_L1_FREQ_CONFIG sig buffer */
  memset (ml_freq_config_info_ptr,0, sizeof(frequency_information_T));

  switch (csn_freq_param_ptr->encoding_type)
  {
    case arfcn_valid:
      return_value = gmac_util_freq_arfcn_valid(csn_freq_param_ptr,ml_freq_config_info_ptr,
                                                gas_id);
      break;

    case direct1_valid:
       /*GPRS Mobile Allocation*/
      ml_freq_config_info_ptr->maio = csn_freq_param_ptr->dir_enc1.maio;
      ml_freq_config_info_ptr->tsc = csn_freq_param_ptr->tsc ;
      frequency_list_ptr = rr_packet_assignment_mobile_alloc(direct1_valid,&(csn_freq_param_ptr->dir_enc1),&(util_data_ptr->identical_freq_list),
                                                             gas_id );

      if(frequency_list_ptr != NULL)
      {
        return_value =  gmac_util_load_freq_sig_data(frequency_list_ptr,ml_freq_config_info_ptr,direct1_valid,
                                                     gas_id);
      }
      else
      {
        return_value = FREQ_ERROR_PARTIAL_SYS_ACQ;
        MSG_GERAN_LOW_0_G("MAC info freq param MA number");
      }
      break;

    case direct2_valid:
      /*MA Frequency List contents*/
      ml_freq_config_info_ptr->maio = csn_freq_param_ptr->dir_enc2.maio;
      ml_freq_config_info_ptr->hsn = csn_freq_param_ptr->dir_enc2.hsn;
      ml_freq_config_info_ptr->tsc = csn_freq_param_ptr->tsc ;
      frequency_list_ptr = rr_packet_assignment_mobile_alloc( direct2_valid, &(csn_freq_param_ptr->dir_enc2), &(util_data_ptr->identical_freq_list),
                                                              gas_id );

      if(frequency_list_ptr != NULL)
      {
        return_value = gmac_util_load_freq_sig_data(frequency_list_ptr,ml_freq_config_info_ptr,direct2_valid,gas_id);
      }
      else
      {
        return_value = FREQ_ERROR_PARTIAL_SYS_ACQ;
        MSG_GERAN_LOW_0_G("MAC info freq param MA number");
      }
      break;

    case indirect_valid:
      ml_freq_config_info_ptr->maio = csn_freq_param_ptr->indir_enc.maio;
      ml_freq_config_info_ptr->tsc = csn_freq_param_ptr->tsc ;
      return_value = gmac_util_freq_indirect_valid(csn_freq_param_ptr,ml_freq_config_info_ptr,gas_id);
      break;

    default:
      MSG_GERAN_LOW_0_G("MAC info freq param encode error");
      return_value = FREQ_ERROR_PARTIAL_SYS_ACQ;
      break;
  }

  /********************************************************************************
  *  signal sending part depending on the validity of the frequency assigment     *
  *********************************************************************************/
  if ((compare_flag == TRUE) &&
      (gmac_util_compare_freq_params(&global_data_ptr->gmac_store_freq_params_in_use,ml_freq_config_info_ptr,gas_id)) == FALSE)
  {
    return_value = FREQ_ERROR_DIFFERENT_PARAMS;
  }

  /********************************************************************************
  *  Check Frequency Band of assignment                                           *
  *********************************************************************************/
  if ( ( global_data_ptr->mac_mode == IDLE_MODE ) ||
       ( global_data_ptr->mac_mode == TRANSFER_MODE ) )
  {
    if ( return_value == NO_ERRORS )
    {
      if ( gmac_gprs_check_frequency_params(
              &ml_freq_config_info_ptr->frequency_list,
               ml_freq_config_info_ptr->hopping_flag,
               gas_id ) == FALSE )
      {
        MSG_GERAN_MED_0_G("MAC Info Freq Band Check Passed");
      }
      else
      {
        MSG_GERAN_MED_0_G("MAC Info Freq Band Check FAILED");
        return_value = FREQ_CONFIG_FAILED_CHECK;
      }
    }
  }

  /* If compare flag set then frequencies must be the same as currently in used
  ** therefore no need to send parameters to L1
  */

  if( (return_value == NO_ERRORS) && (send_freq_config == TRUE))
  {
    global_data_ptr->gmac_store_freq_params_in_use = *ml_freq_config_info_ptr;

    MSG_GERAN_MED_0_G("MAC Sent MAC_L1_FREQ_CONFIG");

    if(global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_UPLINK_ASSIGNMENT)
    {
       freq_config_sig.mac_l1_freq_config.tbf_freq_cnf = UL_FREQ_CONFIG;
    }
    else if (global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_DOWNLINK_ASSIGNMENT)
    {
     freq_config_sig.mac_l1_freq_config.tbf_freq_cnf = DL_FREQ_CONFIG;
    }
    else if (global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_TIMESLOT_RECONFIGURE)
    {
     freq_config_sig.mac_l1_freq_config.tbf_freq_cnf = UL_DL_FREQ_CONFIG;
    }
#ifdef FEATURE_GPRS_PS_HANDOVER
    else if(global_data_ptr->msg_ptr->grr_mac_sig.sig_id == GRR_MAC_PSHO_REQ)
    {
      if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
      {
        freq_config_sig.mac_l1_freq_config.tbf_freq_cnf = UL_DL_FREQ_CONFIG;
      }
      else
      {
        freq_config_sig.mac_l1_freq_config.tbf_freq_cnf = UL_FREQ_CONFIG;
      }
    }
#endif /*FEATURE_GPRS_PS_HANDOVER*/
#ifdef FEATURE_GSM_DTM
    freq_config_sig.mac_l1_freq_config.dtm_freq_param_present = FALSE;
#endif /*FEATURE_GSM_DTM*/

    freq_config_sig.mac_l1_freq_config.reuse_sb_frq_alloc = FALSE;
    freq_config_sig.mac_l1_freq_config.message_header.message_set = MS_MAC_L1;
    freq_config_sig.mac_l1_freq_config.message_header.message_id = (uint8)MAC_L1_FREQ_CONFIG;
    gprs_l1_mac_acc_func(&freq_config_sig,gas_id);
  }
  return (return_value);
}

/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_pccf_on_pacch_rrbp()
===
===  DESCRIPTION
===  Function used by transfer to send a pccf during an active downlink tbf
===  The PCCF is sent in a PACCH RRBP message to the network
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_pccf_on_pacch_rrbp(uint8 ts, uint8 rrbp, uint32 fn,
                                           gas_id_t gas_id)
{
  mac_l1_sig_t        ml_rrbp_sig;
  mac_grr_sig_t       mac_grr_sig;

  mac_l1_pacch_rrbp_t *rrbp_sig_ptr = &(ml_rrbp_sig.mac_l1_pacch_rrbp);
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_PACCH_RRBP");

  rrbp_sig_ptr->rrbp_msg = PCCF_MSG;
  rrbp_sig_ptr->ts_of_dl_msg = ts;
  rrbp_sig_ptr->absolute_fn = gmac_rrbp_absolute_fn(fn, rrbp,gas_id);
  rrbp_sig_ptr->msg_burst_type = NB;

  /* copy the CSN.1 encoded PMR into the PACCH_RRBP message */
  memscpy(rrbp_sig_ptr->pacch_ab_data,sizeof(rrbp_sig_ptr->pacch_ab_data),
          global_data_ptr->gmac_local_pccf_params.csn_encoded_pccf,
          SIZE_OF_CONTROL_RADIO_BLOCK);


#ifdef FEATURE_GSM_EDTM
  ml_rrbp_sig.mac_l1_pacch_rrbp.message_type = PACKET_CELL_CHANGE_FAILURE;
#endif /*FEATURE_GSM_EDTM*/

  ml_rrbp_sig.mac_l1_pacch_rrbp.message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
  ml_rrbp_sig.mac_l1_pacch_rrbp.message_header.message_set = MS_MAC_L1;
  gprs_l1_mac_acc_func(&ml_rrbp_sig,gas_id);

  global_data_ptr->gmac_local_pccf_params.pccf_pending = FALSE;
#ifdef FEATURE_GPRS_PS_HANDOVER
  if(global_data_ptr->gmac_local_pccf_params.pccf_cnf_needed == GRR_CNF_NEEDED)
  {
#endif /*FEATURE_GPRS_PS_HANDOVER*/
  /* call log-on-demand function */
  (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                   PACKET_CELL_CHANGE_FAILURE,
                                   (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                   &(rrbp_sig_ptr->pacch_ab_data[1]),
                                   gas_id);

  MSG_GERAN_MED_0_G( "MAC Sent MAC_GRR_SENT_PCCF");
  mac_grr_sig.mac_grr_pccf_sent.message_header.message_set = MS_MAC_RR;
  mac_grr_sig.mac_grr_pccf_sent.message_header.message_id = (uint8)MAC_GRR_PCCF_SENT;
  gprs_grr_mac_acc_func(&mac_grr_sig,gas_id);
#ifdef FEATURE_GPRS_PS_HANDOVER
    global_data_ptr->gmac_local_pccf_params.pccf_cnf_needed = GRR_CNF_NOT_NEEDED;
  }
#endif /*FEATURE_GPRS_PS_HANDOVER*/
}

/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_pccn_on_pacch_rrbp()
===
===  DESCRIPTION
===
===  Function used by transfer to send a PACKET_CELL_CHANGE_NOTIFICATION
===  during an active downlink tbf. The pkt ms tbf_status is sent in a
===  PACCH RRBP message to the network
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_pccn_on_pacch_rrbp(uint8 ts, uint8 rrbp, uint32 fn,
                                           gas_id_t gas_id)
{
  mac_grr_sig_t grr_sig;

  mac_l1_sig_t        ml_rrbp_sig;

  mac_l1_pacch_rrbp_t *rrbp_sig_ptr = &(ml_rrbp_sig.mac_l1_pacch_rrbp);
  gmac_global_data_t  *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_PACCH_RRBP with PCCN");

  rrbp_sig_ptr->rrbp_msg = PCCF_MSG; //?
  rrbp_sig_ptr->ts_of_dl_msg = ts;
  rrbp_sig_ptr->absolute_fn = gmac_rrbp_absolute_fn(fn, rrbp,gas_id);
  rrbp_sig_ptr->msg_burst_type = NB;

  /* copy the CSN.1 encoded PCCN into the PACCH_RRBP message */
  memscpy(rrbp_sig_ptr->pacch_ab_data,sizeof(rrbp_sig_ptr->pacch_ab_data),
         global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn,
         SIZE_OF_CONTROL_RADIO_BLOCK);

#ifdef FEATURE_GSM_EDTM
  ml_rrbp_sig.mac_l1_pacch_rrbp.message_type = PACKET_CELL_CHANGE_NOTIFICATION;
#endif /*FEATURE_GSM_EDTM*/

  ml_rrbp_sig.mac_l1_pacch_rrbp.message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
  ml_rrbp_sig.mac_l1_pacch_rrbp.message_header.message_set = MS_MAC_L1;
  gprs_l1_mac_acc_func(&ml_rrbp_sig,gas_id);





  global_data_ptr->gmac_local_pccn_params.pccn_pending = NO_PENDING_PCCN;


  MSG_GERAN_MED_0_G("MAC Sent MAC_GRR_NCELL_DATA_CNF ");
  memset(&(grr_sig.mac_grr_ncell_data_cnf),0,sizeof (mac_grr_ncell_data_cnf_t));
  grr_sig.mac_grr_ncell_data_cnf.container_id = SERVING_CELL_CONTAINER;
  grr_sig.mac_grr_ncell_data_cnf.message_header.message_set = MS_MAC_RR;
  grr_sig.mac_grr_ncell_data_cnf.message_header.message_id = (uint8)MAC_GRR_NCELL_DATA_CNF;
  gprs_grr_mac_acc_func(&grr_sig,gas_id);

  memset(&global_data_ptr->gmac_local_pccn_params,0,sizeof(gmac_pccn_params_t));

  MSG_GERAN_MED_0_G(" pccn_pending = NO_PENDING_PCCN");

  /* call log-on-demand function */
  (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                   PACKET_CELL_CHANGE_NOTIFICATION,
                                   (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                   &(rrbp_sig_ptr->pacch_ab_data[1]),
                                    gas_id);

}


/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_pss_on_pacch_rrbp()
===
===  DESCRIPTION
===
===  Function used by transfer to send a PACKET_SI_STATUS
===  during an active downlink tbf. The pkt ms tbf_status is sent in a
===  PACCH RRBP message to the network
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_pss_on_pacch_rrbp(uint8 ts, uint8 rrbp, uint32 fn,
                                          gas_id_t gas_id)
{
  mac_l1_sig_t        ml_rrbp_sig;

  mac_l1_pacch_rrbp_t *rrbp_sig_ptr = &(ml_rrbp_sig.mac_l1_pacch_rrbp);
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_PACCH_RRBP with PSS");

  rrbp_sig_ptr->rrbp_msg = PCCF_MSG; //?
  rrbp_sig_ptr->ts_of_dl_msg = ts;
  rrbp_sig_ptr->absolute_fn = gmac_rrbp_absolute_fn(fn, rrbp, gas_id);
  rrbp_sig_ptr->msg_burst_type = NB;

  /* copy the CSN.1 encoded PCCN into the PACCH_RRBP message */
  memscpy(rrbp_sig_ptr->pacch_ab_data,sizeof(rrbp_sig_ptr->pacch_ab_data),
         global_data_ptr->gmac_local_pss_params.csn_encoded_pss,
         SIZE_OF_CONTROL_RADIO_BLOCK);

#ifdef FEATURE_GSM_EDTM
  ml_rrbp_sig.mac_l1_pacch_rrbp.message_type = PACKET_SI_STATUS;
#endif /*FEATURE_GSM_EDTM*/

  ml_rrbp_sig.mac_l1_pacch_rrbp.message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
  ml_rrbp_sig.mac_l1_pacch_rrbp.message_header.message_set = MS_MAC_L1;
  gprs_l1_mac_acc_func(&ml_rrbp_sig, gas_id);

  global_data_ptr->gmac_local_pss_params.pss_pending = FALSE;

  /* call log-on-demand function */
  (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                   PACKET_SI_STATUS,
                                   (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                   &(rrbp_sig_ptr->pacch_ab_data[1]),
                                    gas_id);

}


/*===========================================================================
===
===  FUNCTION    gmac_send_pmr_cnf_to_grr()
===
===  DESCRIPTION
===  Send measurement report confirmation to GRR. The result indicates
===  whether the measurement report was sent or not.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_pmr_cnf_to_grr(success_failure_t result,
                                   gas_id_t gas_id)
{
  mac_grr_sig_t       mac_grr_sig;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_1_G("MAC Sent MAC_GRR_MEAS_RPT_CNF result %d", result);
  mac_grr_sig.mac_grr_meas_rpt_cnf.status = result;
  mac_grr_sig.mac_grr_meas_rpt_cnf.message_header.message_set = MS_MAC_RR;
  mac_grr_sig.mac_grr_meas_rpt_cnf.message_header.message_id = (uint8)MAC_GRR_MEAS_RPT_CNF;
  gprs_grr_mac_acc_func(&mac_grr_sig,gas_id);
  global_data_ptr->gmac_local_pmr_params.pmr_pending = FALSE;
}



/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_pmr_on_pacch_rrbp()
===
===  DESCRIPTION
===  Function used by transfer to send a pmr during an active downlink tbf
===  The pmr is sent in a PACCH RRBP message to the network
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_pmr_on_pacch_rrbp(uint8 ts, uint8 rrbp, uint32 fn,
                                          gas_id_t gas_id)
{
  mac_l1_sig_t        ml_rrbp_sig;

  mac_l1_pacch_rrbp_t *rrbp_sig_ptr = &(ml_rrbp_sig.mac_l1_pacch_rrbp);
  gmac_global_data_t  *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_PACCH_RRBP");

  rrbp_sig_ptr->rrbp_msg = PMR_MSG;
  rrbp_sig_ptr->ts_of_dl_msg = ts;
  rrbp_sig_ptr->absolute_fn = gmac_rrbp_absolute_fn(fn, rrbp,gas_id);
  rrbp_sig_ptr->msg_burst_type = NB;

  if (global_data_ptr->gmac_local_pmr_params.msg_type_is_pmr)
  {
    (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                     PACKET_MEASUREMENT_REPORT,
                                     (byte)(sizeof(packet_measurement_report_t)),
                                     &(rrbp_sig_ptr->pacch_ab_data[1]),
                                     gas_id);

#ifdef FEATURE_GSM_EDTM
    ml_rrbp_sig.mac_l1_pacch_rrbp.message_type = PACKET_MEASUREMENT_REPORT;
#endif /*FEATURE_GSM_EDTM*/
  }
  else
  {
    (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                     PACKET_ENHANCED_MEASUREMENT_REPORT,
                                     (byte)(sizeof(packet_enhanced_measurement_report_t)),
                                     &(rrbp_sig_ptr->pacch_ab_data[1]),
                                     gas_id);
#ifdef FEATURE_GSM_EDTM
    ml_rrbp_sig.mac_l1_pacch_rrbp.message_type = PACKET_ENHANCED_MEASUREMENT_REPORT;
#endif /*FEATURE_GSM_EDTM*/
  }

  /* copy the CSN.1 encoded PMR into the PACCH_RRBP message */
  memscpy(rrbp_sig_ptr->pacch_ab_data,sizeof(rrbp_sig_ptr->pacch_ab_data),
          global_data_ptr->gmac_local_pmr_params.csn_encoded_pmr,
          SIZE_OF_CONTROL_RADIO_BLOCK);

  ml_rrbp_sig.mac_l1_pacch_rrbp.message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
  ml_rrbp_sig.mac_l1_pacch_rrbp.message_header.message_set = MS_MAC_L1;
  gprs_l1_mac_acc_func(&ml_rrbp_sig,gas_id);

  global_data_ptr->gmac_local_pmr_params.pmr_pending = FALSE;
}


/*===========================================================================
===
===  FUNCTION   gmac_util_send_ul_reset_req_to_rlc()
===
===  DESCRIPTION
===  Send MR_UL_RESET_REQ to RLC.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_ul_reset_req_to_rlc( gas_id_t gas_id )
{
  mac_rlc_msg_t                 *mr_ul_reset_req_ptr;

  mr_ul_reset_req_ptr = grlc_get_cmd_buf_mac();
  if (mr_ul_reset_req_ptr != NULL)
  {
    memset(mr_ul_reset_req_ptr, 0, sizeof(mac_rlc_msg_t));

    MSG_GERAN_MED_0_G("MAC Sent MR_UL_RESET_REQ");
    mr_ul_reset_req_ptr->msg.ul_reset_req.sig = MR_UL_RESET_REQ;
    gmac_rlc_mac_insert_gas_id(mr_ul_reset_req_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mr_ul_reset_req_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}



/*===========================================================================
===
===  FUNCTION   gmac_util_send_dl_reset_req_to_rlc()
===
===  DESCRIPTION
===  Send MR_DL_RESET_REQ to RLC.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_dl_reset_req_to_rlc( gas_id_t gas_id )
{
  mac_rlc_msg_t                 *mr_dl_reset_req_ptr;

  mr_dl_reset_req_ptr = grlc_get_cmd_buf_mac();
  if (mr_dl_reset_req_ptr != NULL)
  {
    memset(mr_dl_reset_req_ptr, 0, sizeof(mac_rlc_msg_t));

    MSG_GERAN_MED_0_G("MAC Sent MR_DL_RESET_REQ");
    mr_dl_reset_req_ptr->msg.dl_reset_req.sig = MR_DL_RESET_REQ;
    gmac_rlc_mac_insert_gas_id(mr_dl_reset_req_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mr_dl_reset_req_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}
/*===========================================================================
===
===  FUNCTION      gprs_grr_mac_acc_func
===
===  DESCRIPTION
===  Used by MAC to send messages to GRR. MAC is required to add the IMH
===  header to the message before sending it to GRR using gs_send_message.
===  Populates the gas_id field with gmac_active_gas_id.
===
===  INPUT
===  Pointer to GRR message structure of the message to be sent
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
void gprs_grr_mac_acc_func(mac_grr_sig_t *common_msg_ptr,
                           gas_id_t       gas_id)
{
  gmac_store_trace_send_msg_buf(gas_id,MS_MAC_RR,common_msg_ptr);

  gmac_grr_mac_msg_insert_gas_id(common_msg_ptr,gas_id);

  /* Format IMH header */
  switch (common_msg_ptr->mac_grr_cell_resel_status.message_header.message_id)
  {
    case MAC_GRR_DATA_IND :
      PUT_IMH_LEN( sizeof( mac_grr_data_ind_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_data_ind.message_header );
      break;

    case MAC_GRR_CELL_RESEL_STATUS:
      PUT_IMH_LEN( sizeof( mac_grr_cell_resel_status_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_cell_resel_status.message_header );
      break;

    case MAC_GRR_MAC_STATUS:
      PUT_IMH_LEN( sizeof( mac_grr_mac_status_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_mac_status.message_header );
      break;

    case MAC_GRR_PAGING_NOTIFICATION:
      PUT_IMH_LEN( sizeof( mac_grr_paging_notification_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_paging_notification.message_header );
      break;

    case MAC_GRR_ABNORMAL_RELEASE:
      PUT_IMH_LEN( sizeof( mac_grr_abnormal_release_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_abnormal_release.message_header );
      break;

    case MAC_GRR_MEAS_RPT_CNF:
      PUT_IMH_LEN( sizeof( mac_grr_meas_rpt_cnf_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_meas_rpt_cnf.message_header );
      break;

    case MAC_GRR_UL_TBF_REQ:
      PUT_IMH_LEN( sizeof( mac_grr_ul_tbf_req_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_ul_tbf_req.message_header );
      break;

    case MAC_GRR_STORE_UPDATE:
      PUT_IMH_LEN( sizeof( mac_grr_store_update_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_store_update.message_header );
      break;

    case MAC_GRR_PCCF_SENT:
      PUT_IMH_LEN( sizeof( mac_grr_pccf_sent_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_pccf_sent.message_header );
      break;

    case MAC_GRR_TBF_REJ:
      PUT_IMH_LEN( sizeof( mac_grr_tbf_rej_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_tbf_rej.message_header );
    break;

    case MAC_GRR_TBF_ACCEPT:
      PUT_IMH_LEN( sizeof( mac_grr_tbf_accept_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_tbf_accept.message_header );
    break;

    case MAC_GRR_RACH_CNF:
      PUT_IMH_LEN( sizeof( mac_grr_rach_cnf_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_rach_cnf.message_header );
    break;

    case MAC_GRR_RACH_REJ:
      PUT_IMH_LEN( sizeof( mac_grr_rach_rej_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_rach_rej.message_header );
    break;

    case MAC_GRR_RESET_CNF:
      PUT_IMH_LEN( sizeof( mac_grr_reset_cnf_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_reset_cnf.message_header );
      break;

    case MAC_GRR_NCELL_DATA_IND:
      PUT_IMH_LEN( sizeof( mac_grr_ncell_data_ind_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_ncell_data_ind.message_header );
      break;


    case MAC_GRR_NCELL_DATA_CNF:
      PUT_IMH_LEN( sizeof( mac_grr_ncell_data_cnf_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_ncell_data_cnf.message_header );
      break;



#ifdef FEATURE_GSM_DTM
    case MAC_GRR_DTM_CHANNEL_ASSIGN_REJ:
      PUT_IMH_LEN( sizeof( mac_grr_dtm_channel_assign_rej_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_dtm_channel_assign_rej.message_header );
      break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
     case MAC_GRR_EDTM_PKT_CS_EST_REJ:
       PUT_IMH_LEN( sizeof( mac_grr_edtm_pkt_cs_est_rej_t ) - sizeof( IMH_T ),
         &common_msg_ptr->mac_grr_edtm_pkt_cs_est_rej.message_header );
       break;

     case MAC_GRR_EDTM_CS_RELEASE_IND:
       PUT_IMH_LEN( sizeof( mac_grr_edtm_cs_release_ind_t ) - sizeof( IMH_T ),
         &common_msg_ptr->mac_grr_edtm_cs_release_ind.message_header );
       break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case MAC_GRR_PSHO_CNF:
      PUT_IMH_LEN( sizeof( mac_grr_psho_cnf_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_psho_cnf.message_header );
      break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

    case MAC_GRR_PS_ACCESS_CNF:
      PUT_IMH_LEN( sizeof( mac_grr_ps_access_cnf_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_grr_ps_access_cnf.message_header );
      break;

    default:
      MSG_GERAN_ERROR_2_G( "Unexpected sig rx. idle task id %d sig id %d" ,
                           common_msg_ptr->mac_grr_ul_tbf_req.message_header.message_set,
                           common_msg_ptr->mac_grr_ul_tbf_req.message_header.message_id);
      break;
  }

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#else

  /* Send message to RR */
  (void)gs_send_message( GS_QUEUE_RR, (void *)common_msg_ptr, TRUE );

#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */
}

/*===========================================================================
===
===  FUNCTION      gprs_l1_mac_acc_func
===
===  DESCRIPTION
===  Used by MAC to send messages to L1. MAC is required to add the IMH
===  header to the message before sending it to L1 using gs_send_message
===
===  INPUT
===  Pointer to L1 message structure of the message to be sent
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
void gprs_l1_mac_acc_func(mac_l1_sig_t *common_msg_ptr,
                          gas_id_t      gas_id)
{
  gs_queue_id_T l1_gs_queue = gmac_map_gas_id_to_layer1_queue(gas_id);

  gmac_store_trace_send_msg_buf(gas_id,MS_MAC_L1,common_msg_ptr);

  gmac_l1_mac_insert_gas_id(common_msg_ptr,gas_id);

  /* Format IMH header */
  switch (common_msg_ptr->mac_l1_freq_config.message_header.message_id)
  {
    case MAC_L1_FREQ_CONFIG :
      PUT_IMH_LEN( sizeof( mac_l1_freq_config_t) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_freq_config.message_header );
      break;

    case MAC_L1_DYNAMIC_TBF_CONFIG:
      PUT_IMH_LEN( sizeof( mac_l1_dynamic_tbf_config_t) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_dynamic_tbf_config.message_header );
      break;

    case MAC_L1_FIXED_TBF_CONFIG:
      PUT_IMH_LEN( sizeof( mac_l1_fixed_tbf_config_t) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_fixed_tbf_config.message_header );
      break;

    case MAC_L1_SINGLE_BLOCK_CONFIG:
      PUT_IMH_LEN( sizeof( mac_l1_single_block_config_t) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_single_block_config.message_header );
      break;

    case MAC_L1_DL_SINGLE_BLOCK:
      PUT_IMH_LEN( sizeof( mac_l1_dl_single_block_t) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_dl_single_block.message_header );
      break;

    case MAC_L1_DL_TBF_CONFIG:
      PUT_IMH_LEN( sizeof( mac_l1_dl_tbf_config_t) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_dl_tbf_config.message_header );
      break;

    case MAC_L1_PACCH_RRBP:
      PUT_IMH_LEN( sizeof( mac_l1_pacch_rrbp_t) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_pacch_rrbp.message_header );
      break;

    case MAC_L1_CS_RECONFIG:
      PUT_IMH_LEN( sizeof( mac_l1_cs_reconfig_t) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_cs_reconfig.message_header );
      break;

    case MAC_L1_POWER_CONTROL:
      PUT_IMH_LEN( sizeof( mac_l1_pwr_ctrl_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_power_control.message_header );
      break;

    case MAC_L1_PAGE_MODE:
      PUT_IMH_LEN( sizeof( mac_l1_page_mode_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_page_mode.message_header );
      break;

    case MAC_L1_PDCH_REL_TS:
      PUT_IMH_LEN( sizeof( mac_l1_pdch_rel_ts_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_pdch_rel_ts.message_header );
      break;

    case MAC_L1_DUMMY_CTRL_BLOCK:
      PUT_IMH_LEN( sizeof( mac_l1_dummy_ctrl_block_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_dummy_ctrl_block.message_header );
      break;

    case MAC_L1_RELEASE:
      PUT_IMH_LEN( sizeof( mac_l1_release_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_release.message_header );
      break;

    case MAC_L1_QUALITY_RPT:
      PUT_IMH_LEN( sizeof( mac_l1_quality_rpt_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_quality_rpt.message_header );
      break;

    case MAC_L1_TS_RECONFIG:
      PUT_IMH_LEN( sizeof( mac_l1_ts_reconfig_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_ts_reconfig.message_header );
      break;

    case MAC_PH_RANDOM_ACCESS_REQ:
      PUT_IMH_LEN( sizeof( mac_ph_random_access_req_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_ph_random_access_req.message_header );
      break;

    case MAC_PH_DATA_REQ:
      PUT_IMH_LEN( sizeof( mac_ph_data_req_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_ph_data_req.message_header );
      break;

    case MAC_L1_CONTENTION_RES:
      PUT_IMH_LEN( sizeof( mac_l1_contention_res_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_contention_res.message_header );
    break;

    case MAC_L1_STOP_RLC_TX:
      PUT_IMH_LEN( sizeof( mac_l1_stop_rlc_tx_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_stop_rlc_tx.message_header );
    break;

    case MAC_L1_RANDOM_ACCESS_ABORT:
      PUT_IMH_LEN( sizeof( mac_l1_random_access_abort_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_random_access_abort.message_header );
      break;

#ifdef FEATURE_GSM_DTM
    case MAC_L1_DTM_CHANNEL_ASSIGN_REQ:
      PUT_IMH_LEN( sizeof( mac_l1_dtm_channel_assign_req_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_dtm_channel_assign_req.message_header );
      break;

#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
    case MAC_L1_EDTM_CS_REL_TBF_ALLOC:
      PUT_IMH_LEN( sizeof( mac_l1_edtm_cs_rel_tbf_alloc_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_edtm_cs_rel_tbf_alloc.message_header );
      break;

    case MAC_L1_DELETE_UL_CTRL_MSG:
      PUT_IMH_LEN( sizeof( mac_l1_delete_ul_ctrl_msg_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_delete_ul_ctrl_msg.message_header );
      break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case MAC_L1_PSHO_REQ:
      PUT_IMH_LEN( sizeof( mac_l1_psho_req_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_psho_req.message_header );
      break;

    case MAC_L1_PSHO_STATUS:
      PUT_IMH_LEN( sizeof( mac_l1_psho_status_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_psho_status.message_header );
      break;

    case MAC_L1_PSHO_CANCEL_PENDING_CONFIG:
      PUT_IMH_LEN( sizeof( mac_l1_psho_cancel_pending_config_t ) - sizeof( IMH_T ),
        &common_msg_ptr->mac_l1_psho_cancel_pending_config.message_header );
      break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/
    default:
      MSG_GERAN_ERROR_2_G( "Unexpected sig rx. idle task id %d sig id %d" ,
        &common_msg_ptr->mac_l1_freq_config.message_header.message_set,
        &common_msg_ptr->mac_l1_freq_config.message_header.message_id);
      break;

  } /*end switch*/
#ifdef FEATURE_L1SIM_SIMULATION
gl1_simulator_state_machine(common_msg_ptr,NULL,0);
#endif /*FEATURE_L1SIM_SIMULATION*/
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#else

  /* Send message to L1 */
  (void)gs_send_message(l1_gs_queue, (void *)common_msg_ptr, TRUE);

#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */
}

/*===========================================================================
===
===  FUNCTION      determine_access_requested()
===
===  DESCRIPTION
===
===  Determine access type request by RLC for an ul tbf est.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  "access_req"
===
===  SIDE EFFECTS
===
===========================================================================*/
access_type_e  determine_access_requested(gas_id_t gas_id)
{
   access_type_e access_req = one_phase;  /*none*/

   boolean egprs_capable_cell = FALSE;
   boolean egprs_pkt_chan_req = FALSE;
   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

   /*access granted store channel request description 12.7*/
   global_data_ptr->gmac_local_stored_ul_chan_req = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.chan_req_desc;
   global_data_ptr->gmac_util_data_req_cause = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.data_req_cause;

  /*store tlli passed by RLC for checking in contention res*/
  global_data_ptr->rlc_ul_est_tlli = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.tlli;

  MSG_GERAN_MED_2_G("MAC info UL est tlli %x, gmac_store_ul_est_tlli %x",
                    global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.tlli,global_data_ptr->gmac_store_ul_est_tlli);

  rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL, &egprs_capable_cell, gas_id );

  if ( egprs_capable_cell )
  {
    rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_PKT_CHAN_REQ, &egprs_pkt_chan_req, gas_id );

    if ( egprs_pkt_chan_req )
    {
      if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.data_req_cause == GRR_CAUSE_LAY3_DATA )
      {
        if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.chan_req_desc.rlc_mode == GRLC_MODE_UNACK )
        {
          access_req = two_phase;
        }
        else    /* RLC ACK Mode */
        {
          if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.num_of_blocks <= 8 )
          {
            /* short access no longer supported, set to 1-phase in this condition, same as GPRS solution */
            access_req = one_phase;
          }
          else
          {
            /* 04.18 sec. 3.5.2.1.2  table indicates access type could be 1-phase or 2-phase */
            /* set to two_phase here so that it is similar to GPRS */
            switch ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.access_phase )
            {
              case GMAC_NO_PHASE_ACCESS:
              case GMAC_SHORT_ACCESS: /* We do not support short acc so default to 1 phase*/
              case GMAC_ONE_PHASE_ACCESS:
                access_req = one_phase;
              break;

              case GMAC_SINGLE_BLOCK_ACCESS:
              case GMAC_TWO_PHASE_ACCESS:
                access_req = two_phase;
              break;

              default:
                MSG_GERAN_ERROR_1_G("GMAC UNKNOWN access phase: 0x%x, overriden with EGPRS 11bit 1phase",
                                    global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.access_phase);
                access_req = one_phase;
              break;
            }
          }
        }
      }
      else /*GMM Sig, LLC Sig, Page Response or Cell Update */
      {
        access_req = signalling;
      }
    }
    else /* Decision for Channel Req only, EGPRS Packet Channnel Req not avail in cell, all access */
    {    /* uses two-phase / single block */

      access_req = two_phase; /* rr handles GMAC_TWO_PHASE_ACCESS and GMAC_SINGLE_BLOCK_ACCESS */
                              /* as single_block for the channel request */
                              /* GMAC_TWO_PHASE_ACCESS type is used here because gmac_null_send_ul_tbf_req */
                              /* treats GMAC_SINGLE_BLOCK_ACCESS as being for measurement reports or PCCF's */
    }
  }
  else /* Not an EGPRS Cell */
  {
    /*determine access requested*/
    if((global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.data_req_cause == GRR_CAUSE_LAY3_DATA) ||
       (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.data_req_cause == GRR_CAUSE_LLC_SIGNALLING))
    {
      if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.access_phase == GMAC_ONE_PHASE_ACCESS)
          access_req = one_phase;

      else if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.access_phase == GMAC_TWO_PHASE_ACCESS)
          access_req = two_phase;

      else if (global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.access_phase == GMAC_SHORT_ACCESS)
          access_req = short_acc;
    }
    else
    {
      if(global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.data_req_cause == GRR_CAUSE_GMM_SIGNALLING)
         access_req = mm_proc;

      else if(global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.data_req_cause == GRR_CAUSE_GMM_CELL_UPDATE)
         access_req = cell_update;

      else if(global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_est_tbf_req.data_req_cause == GRR_CAUSE_GMM_PAGE_RESPONSE)
          access_req = page_response;
    }
  }

  return (access_req);
}

/*===========================================================================
===
===  FUNCTION      gmac_idle_pkt_polling_request_handler()
===
===  DESCRIPTION
===
===  Validates the Pkt polling request dl msg when received as part of a DL
===  assignment process, Determines if PCA should be sent in AB format to the
===  network for the purpose of obtaining TA for the DL.
===
===  DEPENDENCIES
===
===  the globally store csn.1 decoded data
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_idle_pkt_polling_request_handler(gas_id_t gas_id)
{
  boolean   csn_error = FALSE;
  access_burst_t ab_type;
  uint8     rrbp_value;
  uint8     ab_control_ack_value = 3;   /*As not estblishing UL the format */
                                        /*for control ack type is 3 when   */
                                        /*sending access burst             */

  uint8     nb_control_ack_value = 3;   /* NB formatted PCA msg,  */
  /* control_ack_value = 3; */
  uint32    local_rrps_tlli = 0;
  mac_header_tfi_t mac_header_tfi_check;

  boolean   ppr_for_ms = TRUE;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /**************************************************************
  *                  Received signal info                       *
  ***************************************************************/

  /*    csn.1 decoded pkt_polling_request msg                  */
  packet_polling_request_t *ppr_ptr;

  /**************************************************************
  *                 local send signal info                      *
  ***************************************************************/

  /*  mac_l1_pacc_rrbp */
  mac_l1_sig_t                ml_sig;
  mac_l1_pacch_rrbp_t         *ml_pacch_rrbp_sig_ptr;

  /**************************************************************
  *              Assign received  signal  pointers              *
  **************************************************************/

  /*     received pkt_polling_request msg                      */
  ppr_ptr = (packet_polling_request_t *)global_data_ptr->csn_dec_data_ptr;

  /**************************************************************
  *              Assign send signal  pointers                  *
  **************************************************************/

  ml_pacch_rrbp_sig_ptr = &(ml_sig.mac_l1_pacch_rrbp);


  mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);


  /* when a dl is being established, and a pk polling request   */
  /* is received the MS is either addressed by tlli or dl_tfi   */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {


    if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
    {


        if(((packet_polling_request_t *)global_data_ptr->csn_dec_data_ptr)->tlli_gtfi_pkt_ref_valid == tlli_valid)
        {
          /*          check tlli matches                */
          rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);

          if(((packet_polling_request_t *)global_data_ptr->csn_dec_data_ptr)->tlli != local_rrps_tlli)
          {
            MSG_GERAN_LOW_0_G("ERROR :- PKT POLLING REQUEST, TLLI mismatch");
            ppr_for_ms = FALSE;
          }
        }
        else if((((packet_polling_request_t *)global_data_ptr->csn_dec_data_ptr)->tlli_gtfi_pkt_ref_valid == global_tfi_valid) &&
                (((packet_polling_request_t *)global_data_ptr->csn_dec_data_ptr)->global_tfi.direction == dwnlink_valid))
        {
          /*           check dl_tfi matches              */
          if (gmac_util_global_tfi(((packet_polling_request_t *)global_data_ptr->csn_dec_data_ptr)->global_tfi, gas_id) == FALSE)
          {
            MSG_GERAN_LOW_0_G("ERROR :- PKT POLLING REQUEST, DL_TFI mismatch");
            ppr_for_ms = FALSE;
          }
        }

        else if((((packet_polling_request_t *)global_data_ptr->csn_dec_data_ptr)->tlli_gtfi_pkt_ref_valid == global_tfi_valid) &&
                (((packet_polling_request_t *)global_data_ptr->csn_dec_data_ptr)->global_tfi.direction == uplink_valid))
        {
          /*Only in transfer can pkt polling be addressed by ul tfi*/
          if(global_data_ptr->mac_mode == TRANSFER_MODE)
          {
            /*           check ul_tfi matches              */
            if (gmac_util_global_tfi(((packet_polling_request_t *)global_data_ptr->csn_dec_data_ptr)->global_tfi, gas_id) == FALSE)
            {
              MSG_GERAN_LOW_0_G("ERROR :- PKT POLLING REQUEST, DL_TFI mismatch");
              ppr_for_ms = FALSE;
            }
          }
          else
          {
            MSG_GERAN_LOW_1_G("ERROR :- PKT POLLING REQUEST, Addressed by UL TFI in mode:0x%x",global_data_ptr->mac_mode);
            ppr_for_ms = FALSE;
          }
        }
        else
        {
          MSG_GERAN_LOW_0_G("ERROR :- PKT POLLING REQUEST incorrectly address MS");
          ppr_for_ms = FALSE;
        }


    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
    {
      ppr_for_ms = TRUE;
      /*With payload type of "10" and RBSN of "0" control ack value is "10"*/
      ab_control_ack_value = 2;
    }
    else
    {
      /*tfi in mac header did not match*/
      ppr_for_ms = FALSE;
    }

  }
  else
  {
    MSG_GERAN_LOW_0_G("ERROR :- PKT POLLING REQUEST incorrectly address MS");
    ppr_for_ms = FALSE;
  }

  if((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0x08) == 0)
  {
    ppr_for_ms = FALSE;
    MSG_GERAN_HIGH_0_G("MAC Info PKT POLLING REQ. S/P not set. Ignore message");
  }


  if(ppr_for_ms == TRUE)
  {
    MSG_GERAN_MED_0_G( "MAC Rcvd PKT_POLLING_REQ ");
    /* no addressing error, format the PCA relative */
    /* to the info provide in pk polling request    */
    rr_read_public_store(RRPS_CELL_OPTIONS_ACCESS_BURST_TYPE,&ab_type,gas_id);

    if( ppr_ptr->type_of_ack == (uint8)ACCESS_BURST)
    {
      /*note when sending PCA msg in AB format, there is no RLC/MAC */
      /*     control header added                                   */
      if( ab_type == EIGHT_BIT)
      {
        ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = PCA_8_BIT_AB_FORMAT + ab_control_ack_value;
        ml_pacch_rrbp_sig_ptr->msg_burst_type = AB_8_BIT;
      }
      else
      {
        ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = PCA_11_BIT_AB_FORMAT_BITS_9_TO_2;
        ml_pacch_rrbp_sig_ptr->pacch_ab_data[1] = PCA_11_BIT_AB_FORMAT_BITS_1 + ab_control_ack_value;
        ml_pacch_rrbp_sig_ptr->msg_burst_type = AB_11_BIT;
      }
    }
    else
    {
      rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);

      global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.up_ctrl_ack =  nb_control_ack_value;
      global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.tlli = local_rrps_tlli;

      /*          copy csn.1 encoded pca into mac_l1_pacc_rrbp signal      */
      if (gcsn1_encoder(PACKET_CONTROL_ACKNOWLEDGE,&(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]),&global_data_ptr->csn_encoder_ul_msg) == PASS)
      {
        /*          load rlc/mac control header         */
        if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
        {
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
        }
        else
        {
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
        }

        ml_pacch_rrbp_sig_ptr->msg_burst_type = NB;

        /* call log-on-demand function */
        (void)mac_log_signalling_message(PACCH_RRBP_CHANNEL,
                                         PACKET_CONTROL_ACKNOWLEDGE,
                                         (byte)MAX_SIZE_OF_CONTROL_BLOCK_DATA,
                                         &(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]),
                                          gas_id);
      }
      else
      {
        MSG_GERAN_LOW_0_G( "MAC info CSN1 ul msg encoder error");
        csn_error = TRUE;
      }
    }

    /*form the 1st octet of the rlc/mac dl control header determine the rrbp value*/
    rrbp_value = (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0x30) >> 4;

    /* absolute frame number rrbp is to be tx on  and ts dl msg received on  */
    ml_pacch_rrbp_sig_ptr->absolute_fn = gmac_rrbp_absolute_fn(global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no, rrbp_value,
                                                               gas_id);
    ml_pacch_rrbp_sig_ptr->ts_of_dl_msg = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot;
    ml_pacch_rrbp_sig_ptr->rrbp_msg = PCA_DL_MSG;

    if(csn_error == FALSE)
    {
#ifdef FEATURE_GSM_EDTM
      ml_pacch_rrbp_sig_ptr->message_type = PACKET_CONTROL_ACKNOWLEDGE;
#endif /*FEATURE_GSM_EDTM*/

      /*       send PCA via rrbp         */
      MSG_GERAN_MED_0_G("MAC Sent MAC_L1_PACCH_RRBP");
      ml_sig.mac_l1_pacch_rrbp.message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
      ml_sig.mac_l1_pacch_rrbp.message_header.message_set = MS_MAC_L1;
      gprs_l1_mac_acc_func(&ml_sig, gas_id);
    }
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_util_pmo_handler()
===
===  DESCRIPTION
===  Sends GRR PMO msg if its correctly addressed.
===
===  DEPENDENCIES
===  ptr_csn_decode_dl_msg - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_util_pmo_handler(gas_id_t gas_id)
{
  uint32 local_rrps_tlli;
  boolean addr_match = FALSE;


  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;

  packet_measurement_order_t *pmo_data_ptr;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  pmo_data_ptr = (packet_measurement_order_t *)global_data_ptr->csn_dec_data_ptr;

  /* ADDRESS GLOBAL TFI, TLLI */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {

    mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
    if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
    {

        switch (pmo_data_ptr->tlli_gtfi_pkt_ref_valid)
        {
          case tlli_valid:
            /* if stored TLLI = received TLLI then match else ignore message */
            rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);
            if (local_rrps_tlli == pmo_data_ptr->tlli)
            {
              addr_match = TRUE;
            }
            break;

          case global_tfi_valid:
            /* if stored TFI = received TFI then match else ignore message */
            addr_match = gmac_util_global_tfi(pmo_data_ptr->global_tfi,gas_id);
            break;

          default:
            break;
        }

    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
    {
      addr_match = TRUE;
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
    {
      addr_match = FALSE;
    }

  }

  if (addr_match)
  {
    MSG_GERAN_MED_0_G("MAC Rcvd PACKET_MEASUREMENT_ORDER");
    gmac_send_data_ind_to_grr(gas_id);
    rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);
  }

}


/*===========================================================================
===
===  FUNCTION      gmac_util_pkt_paging_on_pacch()
===
===  DESCRIPTION
===  Handles the receipt of PACKET PAGING REQUEST on PACCH during
===  packet transfer. Note that this will only allow CS paging to
===  succeed. Send to GRR for actioning GRR_MAC_PAGING_NOTIFICATION.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_util_pkt_paging_on_pacch(gas_id_t gas_id)
{
  mac_grr_sig_t                 mac_grr_sig;
  mac_grr_paging_notification_t *mac_grr_paging_notification_ptr;

  packet_paging_request_t       *ppr_data_ptr;

  uint8   repeated_page_idx = 0;
  uint8   ppr_mobile_identity[16];
  uint8   local_rrps_imsi[16];
  uint32  local_rrps_tmsi;
  boolean cs_page_found = FALSE;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  ppr_data_ptr = (packet_paging_request_t*)global_data_ptr->csn_dec_data_ptr;
  mac_grr_paging_notification_ptr = &(mac_grr_sig.mac_grr_paging_notification);

  while ((cs_page_found == FALSE) && (repeated_page_idx < ppr_data_ptr->number_of_pages))
  {
    switch (ppr_data_ptr->repeated_page[repeated_page_idx].ptmsi_tmsi_mi_flag)
    {
      case ptmsi_valid:
      case ptmsi_mi_valid:
        /* PS PAGE - error, ignore page information */
        break;

      case tmsi_valid:
        /* CS PAGE to establish a RR connection using TMSI */
        rr_read_public_store(RRPS_TMSI,&local_rrps_tmsi,gas_id);
        if(ppr_data_ptr->repeated_page[repeated_page_idx].tmsi == local_rrps_tmsi)
        {
          mac_grr_paging_notification_ptr->paging_type = RR_CS_PAGING;
          mac_grr_paging_notification_ptr->channel_needed = ppr_data_ptr->repeated_page[repeated_page_idx].channel_needed;
          mac_grr_paging_notification_ptr->identity_type = TMSI;

          if(ppr_data_ptr->repeated_page[repeated_page_idx].emlpp_flag == valid)
          {
            mac_grr_paging_notification_ptr->emlpp_present = TRUE;
            mac_grr_paging_notification_ptr->emlpp_priority = ppr_data_ptr->repeated_page[repeated_page_idx].emlpp_priority;
          }
          else
          {
            mac_grr_paging_notification_ptr->emlpp_present = FALSE;
          }
          cs_page_found = TRUE;
          MSG_GERAN_HIGH_0_G("MAC info PACCH CS PAGE tmsi");

          MSG_GERAN_MED_0_G("MAC Sent MAC_GRR_PAGING_NOTIFICATION");
          mac_grr_sig.mac_grr_paging_notification.message_header.message_set = MS_MAC_RR;
          mac_grr_sig.mac_grr_paging_notification.message_header.message_id = (uint8)MAC_GRR_PAGING_NOTIFICATION;
          gprs_grr_mac_acc_func(&mac_grr_sig,gas_id);
        }
        break;

      case tmsi_mi_valid:
        /* CS PAGE establish a RR connection using IMSI */
        /* default value for spare digit positions in a IMSI is 0xf */
        /* only does when MS addressed by IMSI                      */
        memset(ppr_mobile_identity,0xff,sizeof(ppr_mobile_identity));
        memset(local_rrps_imsi,0xff,sizeof(local_rrps_imsi));

        rr_read_public_store(RRPS_SIM_IMSI,local_rrps_imsi,gas_id);

        memscpy(ppr_mobile_identity,sizeof(ppr_mobile_identity),
                ppr_data_ptr->repeated_page[repeated_page_idx].mobile_identity,
                ppr_data_ptr->repeated_page[repeated_page_idx].length_of_mi);

        if(memcmp((ppr_mobile_identity),(local_rrps_imsi),sizeof(local_rrps_imsi)) == 0)
        {
          mac_grr_paging_notification_ptr->paging_type = RR_CS_PAGING;
          mac_grr_paging_notification_ptr->channel_needed = ppr_data_ptr->repeated_page[repeated_page_idx].channel_needed;
          mac_grr_paging_notification_ptr->identity_type = IMSI;

          if(ppr_data_ptr->repeated_page[repeated_page_idx].emlpp_flag == valid)
          {
            mac_grr_paging_notification_ptr->emlpp_present = TRUE;
            mac_grr_paging_notification_ptr->emlpp_priority = ppr_data_ptr->repeated_page[repeated_page_idx].emlpp_priority;
          }
          else
          {
            mac_grr_paging_notification_ptr->emlpp_present = FALSE;
          }
          cs_page_found = TRUE;
          MSG_GERAN_HIGH_0_G("MAC info PACCH CS PAGE tmsi_mi");

          MSG_GERAN_MED_0_G("MAC Sent MAC_GRR_PAGING_NOTIFICATION");
          mac_grr_sig.mac_grr_paging_notification.message_header.message_set = MS_MAC_RR;
          mac_grr_sig.mac_grr_paging_notification.message_header.message_id = (uint8)MAC_GRR_PAGING_NOTIFICATION;
          gprs_grr_mac_acc_func(&mac_grr_sig, gas_id);
        }
        break;

      default:
        MSG_GERAN_ERROR_0_G("MAC error invalid PAGE info - abort");
        cs_page_found = TRUE;
        break;
    }
    repeated_page_idx++;
  }
}


/*===========================================================================
===
===  FUNCTION      gmac_util_check_pcco_pmo_addressing()
===
===  DESCRIPTION
===
===  Checks that the PMO ro PCCO addresses the MS correctly, used by gmactask
===  to determine when csn.1 buffer should be cleared by MAC or GRR
===
===  DEPENDENCIES
===  ptr_csn_decode_dl_msg - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_util_check_pcco_pmo_addressing(gas_id_t gas_id)
{

 uint32 local_rrps_tlli;
 packet_cell_change_order_t *pcco_data_ptr;
 packet_measurement_order_t *pmo_data_ptr;

 boolean addresses_ms = FALSE;

 mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;
 gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

 if (global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_MEASUREMENT_ORDER)
 {
    pmo_data_ptr = (packet_measurement_order_t *)global_data_ptr->csn_dec_data_ptr;

    /* ADDRESS GLOBAL TFI, TLLI */
    if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
    {

      mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
      if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
      {

        switch (pmo_data_ptr->tlli_gtfi_pkt_ref_valid)
        {
          case tlli_valid:
            /* if stored TLLI = received TLLI then match else ignore message */
            rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);
            if (local_rrps_tlli == pmo_data_ptr->tlli)
            {
              addresses_ms = TRUE;
            }
            break;

          case global_tfi_valid:
            /* if stored TFI = received TFI then match else ignore message */
            addresses_ms = gmac_util_global_tfi(pmo_data_ptr->global_tfi,gas_id);
            break;

          default:
            break;
        }

      }
      else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
      {
        addresses_ms = TRUE;
      }
      else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
      {
        addresses_ms = FALSE;
      }

    }
 }
 else if(global_data_ptr->csn_dec_hdr_ptr->message_type == PACKET_CELL_CHANGE_ORDER)
 {

     pcco_data_ptr = (packet_cell_change_order_t *)global_data_ptr->csn_dec_data_ptr;

     switch (global_data_ptr->mac_mode)
     {
      case NULL_MODE:
        MSG_GERAN_ERROR_0_G("GMAC PCCO received in NULL Mode, NOT expected");
        break;

      case IDLE_MODE:
        /*note checking done in gmac_idle_check_pcco_addressing*/
        addresses_ms = gmac_idle_check_pcco_addressing(gas_id);
        break;

      case TRANSFER_MODE:
           /* ADDRESS GLOBAL TFI, TLLI */
        if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
        {

          mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
          if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
          {

            switch (pcco_data_ptr->tlli_gtfi_pkt_ref_valid)
            {
              case tlli_valid:
                addresses_ms = gmac_match_old_or_new_tlli(pcco_data_ptr->tlli, gas_id);
                break;

              case global_tfi_valid:
                /* if stored TFI = received TFI then match else ignore message */
                addresses_ms = gmac_util_global_tfi(pcco_data_ptr->global_tfi, gas_id);
                break;

              default:
                break;
            }

          }
          else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
          {
            addresses_ms = TRUE;
          }
          else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
          {
            addresses_ms = FALSE;
          }

        }
        break;

      default:
        break;

     }//end switch
  }




 return(addresses_ms);
}







/*===========================================================================
===
===  FUNCTION      gmac_mac_control_header_tfi_checking()
===
===  DESCRIPTION
===
===  RETURN VALUE
===
===  True if TFI matched.
===  note if true then control ack value to be used is in PCA is "2"
=== With payload type of "10" and RBSN of "0" control ack value is "10"
===  SIDE EFFECTS
===
===========================================================================*/
mac_header_tfi_t gmac_mac_control_header_tfi_checking(gas_id_t gas_id)
{
  mac_header_tfi_t match_found = MAC_HEADER_TFI_NOT_VALID;
  uint8 dl_ctrl_block_tfi = 0;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /*if payload type bits  = "10" and AC bit = "1" check TFI value in MAC header*/
  if (  (((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0xc0) >> 6) == 0x02)
        && ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] & 0x1) == 0x01)  )
  {

    MSG_GERAN_MED_3_G("MAC info MAC ctrl headers 0x%x 0x%x 0x%x",
                      global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0],
                      global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1],
                      global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2]);


     if ( ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] & 0x1) == 0x01)
           && ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] & 0x80) == 0) )
     {
       MSG_GERAN_MED_0_G("MAC info Unsegmented msg, optional octets used for PWR ");
     }


     if ( global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot > 7 )
     {
       MSG_GERAN_ERROR_1_G("GMAC INFO timeslot returned by L1 is out of range: 0x%x",global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot);
     }

     /*contians the d bit*/
     dl_ctrl_block_tfi = (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2] & 0x3f );

     /*use tfi rcvd in the mac control header for the DL mac control block*/
     if( (dl_ctrl_block_tfi & 0x01) == 1)
     {

        MSG_GERAN_MED_1_G("MAC info MAC control header contains DL TFI = %d ",((dl_ctrl_block_tfi & 0x3e)>> 1));

        /*           check dl_tfi matches              */
        if( (global_data_ptr->gmac_local_tfi_store.dl_tfi == ((dl_ctrl_block_tfi & 0x3e)>> 1))
            && (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE))
        {
            if ( ( ((uint8)(0x80 >> global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot)) & global_data_ptr->gmac_local_dl_ts_alloc ) == 0 )
            {
               /* Ctrl msg with DL TFI is received on a PDCH that is NOT in the DL TBF */
               match_found = MAC_HEADER_TFI_MATCH_NOT_FOUND;
               MSG_GERAN_HIGH_3_G("GMAC DL TFI rxd on TS NOT in DL TBF, rxd TS:%d, DL TFI:%d, DL TS curr bmap:0x%x",
                                  global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot,
                                  global_data_ptr->gmac_local_tfi_store.dl_tfi,
                                  global_data_ptr->gmac_local_dl_ts_alloc);
            }
            else
            {
               match_found = MAC_HEADER_TFI_MATCH_FOUND;
            }
        }
        else
        {
           /*With payload type of "10" and RBSN of "0" control ack value is "10"*/
           match_found = MAC_HEADER_TFI_MATCH_NOT_FOUND;
           MSG_GERAN_HIGH_1_G("ERROR :MT= %d  DL_TFI mismatch in DL mac ctrl header",global_data_ptr->csn_dec_hdr_ptr->message_type);
        }
     }
     else
     {
        MSG_GERAN_MED_1_G("MAC info MAC control header contains UL TFI = %d ",((dl_ctrl_block_tfi & 0x3e)>> 1));

        /*           check ul_tfi matches              */
        if((global_data_ptr->gmac_local_tfi_store.ul_tfi == ((dl_ctrl_block_tfi & 0x3e)>> 1))
            && (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE))
        {
          if ( ( ((uint8)(0x80 >> global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot)) & global_data_ptr->gmac_local_ul_ts_alloc ) == 0 )
          {
             /* Ctrl msg with UL TFI is received on a PDCH that is NOT in the UL TBF */
             match_found = MAC_HEADER_TFI_MATCH_NOT_FOUND;
             MSG_GERAN_HIGH_3_G("GMAC UL TFI rxd on TS NOT in UL TBF, rxd TS:%d, UL TFI:%d, UL TS curr bmap:0x%x",
                                global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot,
                                global_data_ptr->gmac_local_tfi_store.ul_tfi,
                                global_data_ptr->gmac_local_ul_ts_alloc);
          }
          else
          {
             match_found = MAC_HEADER_TFI_MATCH_FOUND;
          }
        }
        else
        {
          match_found = MAC_HEADER_TFI_MATCH_NOT_FOUND;
          MSG_GERAN_HIGH_1_G("ERROR :MT= %d  UL_TFI mismatch in DL mac ctrl header",global_data_ptr->csn_dec_hdr_ptr->message_type);
        }
     }
  }


  return(match_found);
}

/*===========================================================================
===
===  FUNCTION         gmac_egprs_decode_ul_window_size
===
===  DESCRIPTION
===
===  Converts CSN.1 PUA bit representation of Window size into a numerical value
===
===  DEPENDENCIES
===
===  csn_dec_data_ptr. The decode CSN.1 structure for PUA
===
===  RETURN VALUE
===
===  Decode value of window size
===
===  SIDE EFFECTS
===
===
===========================================================================*/
/*cr134062*/
uint16 gmac_egprs_decode_ul_window_size(uint8 encoded_ws, uint8 usf_bitmap,
                                        gas_id_t gas_id)
{

  uint16 assigned_egprs_ws = WS_ALLOCATION_INVALID;
  uint8 num_ts_allocated = 0;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /*determine the Number of TS allocated */
  num_ts_allocated = gmac_utils_calculate_nts(usf_bitmap,gas_id);

  /*decode the egprs window size rcvd in the assigment msg*/
  assigned_egprs_ws = ((encoded_ws *32) + 64);

  /* If a TBF is in progress, current_egprs_ul_ws has a valid WS. A PTR can reduce the
  ** UL TS assignment but UL window size should remain the same.
  */
  if (global_data_ptr->current_egprs_ul_ws != WS_ALLOCATION_INVALID && assigned_egprs_ws <= global_data_ptr->current_egprs_ul_ws)
  {
      assigned_egprs_ws = global_data_ptr->current_egprs_ul_ws;
  }
  else
  {
     /*check EGPRS UL WS assigned does not exceed MAX allowed for the MULTISLOCT class of UE*/
    if(gmac_check_assigned_egprs_ws(global_data_ptr->gmac_egprs_ms_class_max_ts_tx,assigned_egprs_ws,gas_id) == FAIL_RESULT)
    {
      assigned_egprs_ws = WS_ALLOCATION_INVALID;
      MSG_GERAN_MED_3_G("MAC ERR in EGPRS UL WS. no_ts =%d,ul ws = %d, gmac_egprs_ms_class_max_ts_rx %d",
                        num_ts_allocated,assigned_egprs_ws,global_data_ptr->gmac_egprs_ms_class_max_ts_rx);
      /* avoid compiler warnings about unused variables when MSG
         is defined away to nothing */
      (void)num_ts_allocated;
    }

  }

  return(assigned_egprs_ws);
}

/*===========================================================================
===
===  FUNCTION         gmac_egprs_decode_dl_window_size
===
===  DESCRIPTION
===
===  Converts CSN.1 PUA bit representation of Window size into a numerical value
===
===  DEPENDENCIES
===
===  csn_dec_data_ptr. The decode CSN.1 structure for PDA
===
===  RETURN VALUE
===
===  Decode value of window size
===
===  SIDE EFFECTS
===
===
===========================================================================*/

/*cr134062*/
uint16 gmac_egprs_decode_dl_window_size(uint8 encoded_ws, uint8 dl_ts_alloc, gas_id_t gas_id)
{
  uint16 assigned_egprs_ws = WS_ALLOCATION_INVALID;
  uint8 num_ts_allocated = 0;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  num_ts_allocated = gmac_utils_calculate_nts(dl_ts_alloc,gas_id);

  /*decode the egprs window size rcvd in the assigment msg*/
  assigned_egprs_ws = ((encoded_ws *32) + 64);

  /* If a TBF is in progress, current_egprs_dl_ws has a valid WS. A PTR can reduce the
  ** DL TS assignment but DL window size should remain the same.
  ** Example: WS=480 for 4DL 1UL. If PTR has a config. of 3DL 2UL WS should remain
  ** WS=480.
  */

  if(global_data_ptr->current_egprs_dl_ws != WS_ALLOCATION_INVALID && assigned_egprs_ws <= global_data_ptr->current_egprs_dl_ws)
  {
    assigned_egprs_ws = global_data_ptr->current_egprs_dl_ws;
  }
  else
  {
    /*check EGPRS DL WS assigned does not exceed MAX allowed for the MULTISLOCT class of UE*/
    if(gmac_check_assigned_egprs_ws(global_data_ptr->gmac_egprs_ms_class_max_ts_rx,assigned_egprs_ws,gas_id) == FAIL_RESULT)
    {
      assigned_egprs_ws = WS_ALLOCATION_INVALID;
      MSG_GERAN_MED_3_G("GMAC ERR in EGPRS DL WS. no_ts =%d, dl ws = %d gmac_egprs_ms_class_max_ts_rx %d",
                        num_ts_allocated,assigned_egprs_ws,global_data_ptr->gmac_egprs_ms_class_max_ts_rx);
      /* avoid compiler warnings about unused variables when MSG
         is defined away to nothing */
      (void)num_ts_allocated;
    }
  }
  return(assigned_egprs_ws);
}

/*cr134062*/
/*===========================================================================
===
===  FUNCTION         gmac_check_assigned_egprs_ws()
===
===  DESCRIPTION
===
===  Determine if the assigned egprs window size is with in the range supported
===  by the multislot class of the UE
===
===  max_ts_for_ms_class is the max TS for the multislot capability of the UE.
===  Important Note :  it's up to the calling function to ensure whether
===  it is the maximum number of DL timeslots or the maximum number of UL
===  timeslots. That's being checked
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  PASS_RESULT: The egprs window size is within the allowable limits
===               dictated by the multi slot class and is greater than the
===               minimum WS that's allowable
===
===  FAIL_RESULT: The assigned gprs window greater then the allowable for the
===               Mutilslot class of the UE or is
===               not is not greater the minimum allowable size of 64.
===
===  SIDE EFFECTS
===========================================================================*/

gmac_result_t gmac_check_assigned_egprs_ws(uint8 max_ts_for_ms_class, uint16 assigned_egprs_ws,
                                           gas_id_t gas_id)
{

  #define         MINIMUM_WINDOW_SIZE 64
  uint16          max_allowable_ws = 0;
  gmac_result_t   use_assigned_egprs_ws = PASS_RESULT;

  uint16          actual_egprs_ms_class =0;  /*Use for F3 debug*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_2_G("GMAC info: assigned_egprs_ws = %d, max_ts_for_ms_class %d",assigned_egprs_ws,max_ts_for_ms_class);

  /*look up the MAX WS using UE's Multislot class to determine the MAX TS allowable*/
  switch (max_ts_for_ms_class)
  {
    case 1:  max_allowable_ws = 192;
      break;

    case 2:  max_allowable_ws = 256;
      break;

    case 3:  max_allowable_ws = 384;
      break;

    case 4:  max_allowable_ws = 512;
      break;

    case 5:  max_allowable_ws = 640;
      break;

    case 6:  max_allowable_ws = 768;
      break;

    case 7:  max_allowable_ws = 896;
      break;

    case 8:  max_allowable_ws = 1024;
      break;

    default: MSG_GERAN_ERROR_1_G("GMAC ERR Number of TS exceeds theoretical max ,max_ts_for_ms_class =%d",max_ts_for_ms_class);
      break;
  }

  if( (assigned_egprs_ws < MINIMUM_WINDOW_SIZE) || (assigned_egprs_ws > max_allowable_ws))
  {
    use_assigned_egprs_ws = FAIL_RESULT;

    /*note "gmac_egprs_ms_class" stores the csn.1 encode rfepresetation for MS class*/
    /*where 0 counting is used ie 0 equates to 1. Hence the addition of 1 to convert */
    /*back to the actual value*/
    actual_egprs_ms_class = global_data_ptr->gmac_egprs_ms_class + 1;

    MSG_GERAN_ERROR_3_G("GMAC ERR in WS assigment: MS class %d max ts for MS class %d, assigned_egprs_ws %d",
                        actual_egprs_ms_class,max_ts_for_ms_class,assigned_egprs_ws);
  }

 return (use_assigned_egprs_ws);
}


/*===========================================================================
===
===  FUNCTION         gmac_egprs_fai_set()
===
===  DESCRIPTION
===
===  The FAI bit is set in the ACK/NACK description however when operating in
===  EGPRS the MS can be assigned either an EGPRS or GPRS TBF which means that
===  the PUAN may contain an EGPRS ack/nack des or a GPRS ack/nack des structure.
===
===  This function determines the the mode of the TBF and checks the relevant
===  structure to see if the FAi has been set.
===
===  DEPENDENCIES
===
===  csn_dec_data_ptr. The decode CSN.1 structure for PUAN ack/nack descriptions
===
===  RETURN VALUE
===
===  TRUE if FAI is set.
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_egprs_fai_set(packet_uplink_ack_nack_t *puan_data_ptr, gas_id_t gas_id)
{
   boolean fai_bit_set = FALSE;
   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

   /*check for fai and s/p bit*/
   if (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
   {
      if (puan_data_ptr->egprs_ack_nack_des.fai == 1 )
         fai_bit_set = TRUE;
   }
   else if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
   {
      if (puan_data_ptr->ack_nack_desc.final_ack_ind == 1 )
         fai_bit_set = TRUE;
   }

   return (fai_bit_set);
}


/*===========================================================================
===
===  FUNCTION      gmac_send_rac_prr
===
===  DESCRIPTION
===
===  Function is used to send a PRR when it is part of the  Radio Access Capability process
===
===  DEPENDENCIES
===
===  ms_net_ra_cap_ie
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_send_rac_prr
(
 idle_mode_ul_tbf_release_action_t re_est_action,
 boolean send_additional_rac_info,
 boolean two_phase_access,
 gas_id_t gas_id
)
{
  int_data_t          input_data;
  mac_l1_sig_t        ml_ph_data_sig;
  mac_l1_sig_t        ml_rrbp_sig;
  rrps_change_mark_t  change_mark;
  uint8               prr_encoding = FAIL;
  boolean             egprs_capable_cell = FALSE;
  boolean             egprs_pkt_chan_req = FALSE;

  mac_ph_data_req_t   *data_req_msg = &(ml_ph_data_sig.mac_ph_data_req);

  packet_resource_request_t *prr_ptr = &(input_data.type.pkt_resource_req);

  mac_l1_pacch_rrbp_t   *rrbp_sig_ptr = &(ml_rrbp_sig.mac_l1_pacch_rrbp);

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  /***************************************
  *   compile packet resource request    *
  ***************************************/

  if ( (global_data_ptr->mac_mode == IDLE_MODE) ||
       (global_data_ptr->mac_mode == NULL_MODE) )
  {
    if ( two_phase_access == TRUE )
    {
      /*PRR formt when sent from idle*/
      prr_ptr->access_type_flag = valid;

      rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL, &egprs_capable_cell, gas_id );
      rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_PKT_CHAN_REQ, &egprs_pkt_chan_req, gas_id );

      /* If we are in an EGPRS cell with no 11 bit RACH support then the access_requested will always
         be 2 phase.  Use gmac_util_data_req_cause instead of access_requested to find out what the
         access reason is for the ACCESS_TYPE field in the PRR instead of access_requested*/
      if (( egprs_capable_cell == TRUE ) &&
          ( egprs_pkt_chan_req == FALSE ))
      {
        if ( global_data_ptr->gmac_util_data_req_cause == GRR_CAUSE_GMM_SIGNALLING )
        {
          prr_ptr->access_type = GMAC_PRR_ACC_TYPE_MM_PROC;
        }
        else if ( global_data_ptr->gmac_util_data_req_cause == GRR_CAUSE_GMM_PAGE_RESPONSE )
        {
          prr_ptr->access_type = GMAC_PRR_ACC_TYPE_PAGE_RESP;
        }
        else if ( global_data_ptr->gmac_util_data_req_cause == GRR_CAUSE_GMM_CELL_UPDATE )
        {
          prr_ptr->access_type = GMAC_PRR_ACC_TYPE_CELL_UPD;
        }
        else
        {
          /* default to 2-phase for all other req causes */
          prr_ptr->access_type = GMAC_PRR_ACC_TYPE_TWO_PHASE;
        }

        MSG_GERAN_HIGH_1_G("GMAC EGPRS cell 8 bit RACH, ACC_TYPE for PRR set to:%d",prr_ptr->access_type);
      }
      else
      {
        /*conversion of RLC access requested */
        /*to access type used in PRR         */
        if(global_data_ptr->access_requested == two_phase)
        {
          prr_ptr->access_type = GMAC_PRR_ACC_TYPE_TWO_PHASE;
        }
        else if( global_data_ptr->access_requested == one_phase )
        {
          /*to handle the case where short access or 1 phase access are
            requested by rlc in 51 mode*/
          prr_ptr->access_type = GMAC_PRR_ACC_TYPE_TWO_PHASE;
        }
        else if ( global_data_ptr->access_requested == signalling )
        {
          if ( global_data_ptr->gmac_util_data_req_cause == GRR_CAUSE_GMM_SIGNALLING )
          {
            prr_ptr->access_type = GMAC_PRR_ACC_TYPE_MM_PROC;
          }
          else if ( global_data_ptr->gmac_util_data_req_cause == GRR_CAUSE_GMM_PAGE_RESPONSE )
          {
            prr_ptr->access_type = GMAC_PRR_ACC_TYPE_PAGE_RESP;
          }
          else if ( global_data_ptr->gmac_util_data_req_cause == GRR_CAUSE_GMM_CELL_UPDATE )
          {
            prr_ptr->access_type = GMAC_PRR_ACC_TYPE_CELL_UPD;
          }
          else
          {
            /* default to 2-phase for all other req causes */
            prr_ptr->access_type = GMAC_PRR_ACC_TYPE_TWO_PHASE;
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("GMAC info PRR RAC Acc Req Invalid :%d", prr_ptr->access_type);
          prr_ptr->access_type_flag = not_valid;
        }
      }
    }
    else
    {
      /* acc type not valid in 1-phase */
      prr_ptr->access_type_flag = not_valid;
      MSG_GERAN_HIGH_0_G("GMAC acc_type NOT inc in PRR, doing 1-phase access");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("GMAC ERROR gmac_send_rac_prr called in TRANSFER Mode");
    prr_ptr->access_type_flag = not_valid;
  }

#ifdef PERLUTF
  #error code not present
#endif /*PERLUTF*/

  /*add TLLI*/
  prr_ptr->tfi_tlli_flag = CSN_USE_TLLI;

  prr_ptr->tlli = global_data_ptr->gmac_store_ul_est_tlli;

  /*add RAC info*/
  /*ref 4.60  11.2.16 see MS Radio Access Capability used for 2 phase access*/
  if ( util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits == 0 )
  {
    prr_ptr->ms_radio_access_cap_flag = not_valid;
    MSG_GERAN_HIGH_0_G("GMAC info, RAC IE not included in PRR");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("GMAC info, RAC IE included in PRR");
    prr_ptr->ms_radio_access_cap_flag = valid;

    prr_ptr->access_capability.ms_cap_len_bits = util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits;

    memscpy( prr_ptr->access_capability.ms_cap,sizeof(prr_ptr->access_capability.ms_cap),
             util_data_ptr->prr_ie.ms_ra_cap_ie, sizeof(util_data_ptr->prr_ie.ms_ra_cap_ie));

  }

  /*add channel request description*/
  prr_ptr->chan_req_desc = global_data_ptr->gmac_local_stored_ul_chan_req;

  /*add class mark*/
  rr_read_public_store(RRPS_CHANGE_MARK_SI13,&change_mark,gas_id);

  if (change_mark == RRPS_CHANGE_MARK_NOT_VALID)
  {
    prr_ptr->change_mark_flag = not_valid;
  }
  else
  {
    prr_ptr->change_mark_flag = valid;
    prr_ptr->change_mark = change_mark;
  }

  /*********************************
  *    add  QUALITY REPORT         *
  *********************************/

  prr_ptr->c_value = global_data_ptr->msg_ptr->l1_mac_sig.msg.qual_measurements.c_value;

  /*SIGN_VAR not valid while establishing TBF ref 4.60 11.2.16*/
  prr_ptr->sign_var_flag = not_valid;

  /*I LEVEL. not valid while establishing TBF ref 4.60 11.2.16*/
  prr_ptr->i_level_tn_bitmap = 0x00;

  /*********************************
  *    add  EGPRS pramaters        *
  *********************************/

  /*add EGPRS pramaters to PRR NOTE function only called if doing EGPRS establishment*/
  prr_ptr->include_egprs = TRUE;

  /*BEP Link Quality. not valid while establishing TBF ref 4.60 11.2.16*/
  prr_ptr->egprs_bep_link_qual_present = FALSE;

  /*Timeslot Link Quality. not valid while establishing TBF ref 4.60 11.2.16*/
  prr_ptr->egprs_ts_link_qual_present = FALSE;

  if ( global_data_ptr->pfi_present )
  {
    prr_ptr->pfi_present = TRUE;
    prr_ptr->pfi = global_data_ptr->pfi;
  }
  else
  {
    prr_ptr->pfi_present = FALSE;
  }

  /* if method 2 used the use of these two flags needs to be reconsidered*/
  prr_ptr->additional_rac_info = send_additional_rac_info;


  prr_ptr->rac_retran = FALSE;

  /*******************************************
  *csn encode PRR and add mac control header *
  *******************************************/

#ifdef PERLUTF
  #error code not present
#endif /*PERLUTF*/

  prr_encoding = gcsn1_encoder(PACKET_RESOURCE_REQUEST, &(util_data_ptr->csn_encoded_rac_prr_msg[1]), &input_data);

  /***************************************
  * send PRR either via SB alloc OR RRBP *
  ***************************************/
  if(prr_encoding == PASS)
  {
    /* add mac control header*/
    if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
    {
      util_data_ptr->csn_encoded_rac_prr_msg[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
    }
    else
    {
      util_data_ptr->csn_encoded_rac_prr_msg[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
    }

    if((re_est_action == NO_RE_EST) || (re_est_action == RE_EST_WAIT_FOR_SB_ALLOC))
    {

      memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),util_data_ptr->csn_encoded_rac_prr_msg,23);


      MSG_GERAN_MED_0_G("MAC Sent MAC_PH_DATA_REQ with PRR");
#ifdef FEATURE_GSM_EDTM
      ml_ph_data_sig.mac_ph_data_req.message_type = PACKET_RESOURCE_REQUEST;
#endif /*FEATURE_GSM_EDTM*/
      ml_ph_data_sig.mac_ph_data_req.message_header.message_id = (uint8)MAC_PH_DATA_REQ;
      ml_ph_data_sig.mac_ph_data_req.message_header.message_set = MS_MAC_L1;
      gprs_l1_mac_acc_func(&ml_ph_data_sig,gas_id);

      /* call log-on-demand function */
      (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                       PACKET_RESOURCE_REQUEST,
                                       (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                       &(data_req_msg->dl_ctrl_block[1]),
                                        gas_id);

    }
    else if (re_est_action == RE_EST_SEND_PRR)
    {

      memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),util_data_ptr->csn_encoded_rac_prr_msg,23);

      MSG_GERAN_MED_0_G( "MAC Sent MAC_L1_PACCH_RRBP with PRR");
      rrbp_sig_ptr->rrbp_msg = PRR_MSG;
      rrbp_sig_ptr->ts_of_dl_msg = global_data_ptr->prr_re_est_rrbp_ts;
      rrbp_sig_ptr->absolute_fn = global_data_ptr->prr_re_est_rrbp_fn;
      rrbp_sig_ptr->msg_burst_type = NB;
#ifdef FEATURE_GSM_EDTM
      rrbp_sig_ptr->message_type = PACKET_RESOURCE_REQUEST;
#endif /*FEATURE_GSM_EDTM*/
      ml_rrbp_sig.mac_ph_data_req.message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
      ml_rrbp_sig.mac_ph_data_req.message_header.message_set = MS_MAC_L1;
      gprs_l1_mac_acc_func(&ml_rrbp_sig,gas_id);

      /* call log-on-demand function */
      (void)mac_log_signalling_message(PACCH_RRBP_CHANNEL,
                                       PACKET_RESOURCE_REQUEST,
                                       (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                       &(rrbp_sig_ptr->pacch_ab_data[1]),
                                        gas_id);
    }

    global_data_ptr->c_flags.egprs_prr_sent = TRUE;

  }
  else
  {
    if(geran_get_nv_recovery_restart_enabled(gas_id))
    {
      MSG_GERAN_ERROR_0_G("MAC error CSN.1 encode PRR");
      MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
      geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
    }
    else
    {
      ERR_GERAN_FATAL_0_G("MAC error CSN.1 encode PRR");
    }
  }

}

/*===========================================================================
===
===  FUNCTION    gmac_calc_pkt_resource_req_size
===
===  DESCRIPTION
===
===  Function is part of the Radio Access Capability process. It calcuates the size in
===  bits of the PRR WITHOUT the RAC IE included.
===
===  The elements to be include in the PRR are the relevent ones when sending a PRR as
===  part of the Radio Access Capability process.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  Size in bits of PRR without RAC IE.
===
===  SIDE EFFECTS
===
===========================================================================*/
uint8 gmac_calc_pkt_resource_req_size(boolean two_phase_access, boolean format_gprs_prr,
                                      gas_id_t gas_id)
{

  #define MAX_POS_PTR ((uint16) (22 * 8))

  uint8              prr_size = 0;
  rrps_change_mark_t change_mark;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  prr_size = prr_size + 6;       /*add message type field 6 bits */

  /*ACCESS_TYPE */
  /*ref 4.60 11.2.16.2 only used in response to a single block or Multi block assignment.*/
  prr_size = prr_size + 1;       /*add present/not present flag*/
  if (two_phase_access == TRUE)
  {
    prr_size = prr_size + 2;     /*add ACCESS_TYPE field : bit (2) */
  }

  /*Global TFI or TLLI */
  /*ref 4.60 7.1.2.2.1a if PRR sent as part of RAC process during 1 phase or 2phase then TLLI*/
  /*is to be included untill contetnion reslotuion has completed after which it is optional  */
  /*either TFi or TLLI can be used                                                           */
  prr_size = prr_size + 1;     /*  add present/not present flag*/
  prr_size = prr_size + 32;    /*  add  tlli field : 32 bits.  */

  /*MS Radio Access Capability 2*/
  prr_size = prr_size + 1;       /*flag add present/not present flag*/
  /****************************************************************************/
  /* DO NOT NCLUDE MS Radio Access Capability 2 in calcualtion of PRR size    */
  /*                          IF IT IS PRSENT                                 */
  /****************************************************************************/

  /* Insert Channel Request Description */
  prr_size = prr_size + 24;      /* PEAK_THROUGHPUT_CLASS field : bit (4)  *
                                 *  RADIO_PRIORITY              : bit (2)  *
                                 *  RLC_MODE                    : bit (1)  *
                                 *  LLC_ PDU_TYPE               : bit (1)  *
                                 *  RLC_OCTET_COUNT             : bit (16) */

  /* CHANGE_MARK */
  rr_read_public_store(RRPS_CHANGE_MARK_SI13,&change_mark,gas_id);

  prr_size = prr_size + 1;      /* add present/not present flag */

  if (change_mark != RRPS_CHANGE_MARK_NOT_VALID)
  {
    prr_size = prr_size + 2;    /*add  CHANGE_MARK field : bit (2) */
  }

  /*C_VALUE */
  prr_size = prr_size + 6;     /* add C_VALUE field : bit (6) */

  /*SIGN_VAR */
  /* ref 4.60 ref 4.60 11.2.16.2 SIGN_VAR not valid for EGPRS .*/
  prr_size = prr_size + 1;    /* add present/not present flag*/

  /* I_LEVEL_TN . quality measurements are NOT available while initiating UL est only the C LEVEL*/
  prr_size = prr_size + 8;     /* add I_LEVEL_TN present/not present flag */
                               /* note a flags 1 for each time slot       */

  /* if we are calculating the length of a GPRS PRR (format_gprs_prr is TRUE) and PFI is valid then include rel 99 contents.
     if format_gprs_prr is FALSE we are calculating the length of an EGPRS PRR and rel 99 cont will always be present*/
  if ( ( ( TRUE == format_gprs_prr ) && (global_data_ptr->pfi_present) ) ||
         ( FALSE == format_gprs_prr) )
  {
    prr_size = prr_size + 1;         /* add EGPRS present /not present flag   */
                                     /* note flag only valid for EGPRS        */

    /* note: sending PRR as part of UL est, then there are no EGPRS       */
    /* measurements so only add the present/not present flag to the calc  */
    /* ref 4.60 ref 4.60 11.2.16.2                                        */

    /* EGPRS BEP Link Quality Measurements */
    prr_size = prr_size + 1;         /*present/not prsesent flag*/

    /*EGPRS Timeslot Link Quality Measurements*/
    prr_size = prr_size + 1;         /*present/not prsesent flag*/

    /* PFI: bit(7) */
    prr_size = prr_size + 1; /*PFI present/not present bit*/
    if ( global_data_ptr->pfi_present )
    {
      prr_size = prr_size + 7;
    }

    /*ADDITIONAL MS RAC INFORMATION AVAILABLE*/
    prr_size = prr_size + 1;

    /*RETRANSMISSION OF PRR */
    prr_size = prr_size + 1;
  }
  else
  {
    prr_size = prr_size + 1; /* EGPRS NOT present add present /not present flag   */
  }

  MSG_GERAN_MED_2_G("GMAC info PRR without RAC in bits:%d PRR MAX data bits:%d",(prr_size), (MAX_POS_PTR));

  return(prr_size);
}

/*===========================================================================
===
===  FUNCTION    gmac_send_add_rac_info_msg()
===
===  DESCRIPTION
===
===  Function is called to send additional RAC information msg to the network as
===  part of the Radio Access Capability process
===
===  DEPENDENCIES
===
===  ms_net_ra_cap_ie
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_send_add_rac_info_msg( idle_mode_ul_tbf_release_action_t re_est_action,gas_id_t gas_id)
{

  int_data_t          input_data;
  mac_l1_sig_t        ml_ph_data_sig;
  mac_l1_sig_t        ml_rrbp_sig;
  uint8               add_rac_encoding = FAIL;

  mac_ph_data_req_t   *data_req_msg = &(ml_ph_data_sig.mac_ph_data_req);

  egprs_pkt_add_rac_info_t *add_rac_ptr = &(input_data.type.egprs_pkt_add_rac_info);

  mac_l1_pacch_rrbp_t   *rrbp_sig_ptr = &(ml_rrbp_sig.mac_l1_pacch_rrbp);

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  //< Additional MS Radio Access Capabilities message content > ::=
  //{ 0 < Global TFI : < Global TFI IE > >
  //| 1 < TLLI : < TLLI IE > > }
  //< MS Radio Access Capability 2 : < MS Radio Access Capability 2 IE > >
  //< padding bits > ;

  /*will always include TLLI*/
  add_rac_ptr->tlli_valid_flg = TRUE;

  add_rac_ptr->tlli = global_data_ptr->gmac_store_ul_est_tlli;

  /*ref 4.60  11.2.16 see MS Radio Access Capability used for 2 phase access*/
  add_rac_ptr->access_capability.ms_cap_len_bits = util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits;

  memscpy( add_rac_ptr->access_capability.ms_cap,sizeof(add_rac_ptr->access_capability.ms_cap),
           util_data_ptr->arac_ie.ms_ra_cap_ie, sizeof(util_data_ptr->arac_ie.ms_ra_cap_ie));


  /**********************************************************************
  *csn encode additional RAC information msg and add mac control header *
  **********************************************************************/

  add_rac_encoding = gcsn1_encoder(ADDITIONAL_MS_RAC,&(util_data_ptr->csn_encoded_rac_arac_msg[1]),&input_data);


  /***************************************
  * send PRR either via SB alloc OR RRBP *
  ***************************************/
  if(add_rac_encoding == PASS)
  {
    global_data_ptr->c_flags.egprs_arac_sent = TRUE;

    if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
    {
      util_data_ptr->csn_encoded_rac_arac_msg[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
    }
    else
    {
      util_data_ptr->csn_encoded_rac_arac_msg[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
    }

    if((re_est_action == NO_RE_EST) || (re_est_action == RE_EST_WAIT_FOR_SB_ALLOC))
    {

      memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),util_data_ptr->csn_encoded_rac_arac_msg,23);

#ifdef FEATURE_GSM_EDTM
      ml_ph_data_sig.mac_ph_data_req.message_type = ADDITIONAL_MS_RAC;
#endif /*FEATURE_GSM_EDTM*/

      MSG_GERAN_MED_0_G("MAC Sent MAC_PH_DATA_REQ with Add RAC msg");
      ml_ph_data_sig.mac_ph_data_req.message_header.message_id = (uint8)MAC_PH_DATA_REQ;
      ml_ph_data_sig.mac_ph_data_req.message_header.message_set = MS_MAC_L1;
      gprs_l1_mac_acc_func(&ml_ph_data_sig,gas_id);

      /* call log-on-demand function */
      (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                       ADDITIONAL_MS_RAC,
                                       (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                       &(data_req_msg->dl_ctrl_block[1]),
                                        gas_id);

    }
    else if (re_est_action == RE_EST_SEND_PRR)
    {

      memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),util_data_ptr->csn_encoded_rac_arac_msg,23);

      MSG_GERAN_MED_0_G( "MAC Sent MAC_L1_PACCH_RRBP with Add RAC msg");
      rrbp_sig_ptr->rrbp_msg = PRR_MSG;
      rrbp_sig_ptr->ts_of_dl_msg = global_data_ptr->prr_re_est_rrbp_ts;
      rrbp_sig_ptr->absolute_fn = global_data_ptr->prr_re_est_rrbp_fn;
      rrbp_sig_ptr->msg_burst_type = NB;
#ifdef FEATURE_GSM_EDTM
      rrbp_sig_ptr->message_type = ADDITIONAL_MS_RAC;
#endif /*FEATURE_GSM_EDTM*/
      ml_rrbp_sig.mac_ph_data_req.message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
      ml_rrbp_sig.mac_ph_data_req.message_header.message_set = MS_MAC_L1;
      gprs_l1_mac_acc_func(&ml_rrbp_sig,gas_id);

      /* call log-on-demand function */
      (void)mac_log_signalling_message(PACCH_RRBP_CHANNEL,
                                       ADDITIONAL_MS_RAC,
                                       (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                       &(rrbp_sig_ptr->pacch_ab_data[1]),
                                        gas_id);
    }

  }
  else
  {
    if(geran_get_nv_recovery_restart_enabled(gas_id))
    {
      MSG_GERAN_ERROR_0_G("MAC error CSN.1 encode Add RAC msg");
      MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
      geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
    }
    else
    {
      ERR_GERAN_FATAL_0_G("MAC error CSN.1 encode Add RAC msg");
    }
  }
}

/*===========================================================================
===
===  FUNCTION    gmac_retran_prr_msg()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_retran_prr_msg(gas_id_t gas_id)
{
  mac_l1_sig_t        ml_ph_data_sig;
  mac_ph_data_req_t   *data_req_msg = &(ml_ph_data_sig.mac_ph_data_req);
  gmac_util_data_t    *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC sent MAC_PH_DATA_REQ with re-tran of PRR msg");

  memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),util_data_ptr->csn_encoded_rac_prr_msg,23);

#ifdef FEATURE_GSM_EDTM
  ml_ph_data_sig.mac_ph_data_req.message_type = PACKET_RESOURCE_REQUEST;
#endif /*FEATURE_GSM_EDTM*/

  ml_ph_data_sig.mac_ph_data_req.message_header.message_id = (uint8)MAC_PH_DATA_REQ;
  ml_ph_data_sig.mac_ph_data_req.message_header.message_set = MS_MAC_L1;
  gprs_l1_mac_acc_func(&ml_ph_data_sig,gas_id);

  /* call log-on-demand function */
  (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                   PACKET_RESOURCE_REQUEST,
                                   (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                   &(data_req_msg->dl_ctrl_block[1]),
                                    gas_id);
}

/*===========================================================================
===
===  FUNCTION    gmac_retran_arac_msg()
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_retran_arac_msg(gas_id_t gas_id)
{
  mac_l1_sig_t        ml_ph_data_sig;
  mac_ph_data_req_t   *data_req_msg = &(ml_ph_data_sig.mac_ph_data_req);
  gmac_util_data_t    *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Sent MAC_PH_DATA_REQ with re-tran of Add RAC msg");

  memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),util_data_ptr->csn_encoded_rac_arac_msg,23);

#ifdef FEATURE_GSM_EDTM
  ml_ph_data_sig.mac_ph_data_req.message_type = ADDITIONAL_MS_RAC;
#endif /*FEATURE_GSM_EDTM*/

  ml_ph_data_sig.mac_ph_data_req.message_header.message_id = (uint8)MAC_PH_DATA_REQ;
  ml_ph_data_sig.mac_ph_data_req.message_header.message_set = MS_MAC_L1;
  gprs_l1_mac_acc_func(&ml_ph_data_sig,gas_id);

  /* call log-on-demand function */
  (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                   ADDITIONAL_MS_RAC,
                                   (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                   &(data_req_msg->dl_ctrl_block[1]),
                                    gas_id);
}


/*===========================================================================
===
===  FUNCTION    supported_band_valid()
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/

static boolean supported_band_valid(boolean inhibit_pgsm, sys_band_T band,
                                    gas_id_t gas_id)
{
  boolean band_valid = FALSE;

  if ( rr_gapi_is_supported_band(band,gas_id) )
  {
    if ( ( band == SYS_BAND_PGSM_900 ) &&
         ( inhibit_pgsm ) )
    {
      band_valid = FALSE;
    }
    else
    {
      band_valid = TRUE;
    }
  }
  else
  {
    band_valid = FALSE;
  }

  MSG_GERAN_HIGH_3_G("GMAC DEBUG, band:%d, inhibit_pgsm:%d, band_valid:%d",band,inhibit_pgsm,band_valid);

  return band_valid;
}

/*===========================================================================
===
===  FUNCTION    convert_rac_band_to_rr_format()
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/

static sys_band_T convert_rac_band_to_rr_format(uint8 nw_band,
                                                gas_id_t gas_id)
{
  sys_band_T rr_band = SYS_BAND_NONE;

  switch ( nw_band )
  {
    case ACCESS_TECH_GSM_P:
      rr_band = SYS_BAND_PGSM_900;
    break;

    case ACCESS_TECH_GSM_E:
      rr_band = SYS_BAND_EGSM_900;
    break;

    case ACCESS_TECH_GSM_R:
      rr_band = SYS_BAND_NONE;
    break;

    case ACCESS_TECH_GSM_1800:
      rr_band = SYS_BAND_DCS_1800;
    break;

    case ACCESS_TECH_GSM_1900:
      rr_band = SYS_BAND_PCS_1900;
    break;

    case ACCESS_TECH_GSM_450:
      rr_band = SYS_BAND_NONE;
    break;

    case ACCESS_TECH_GSM_480:
      rr_band = SYS_BAND_NONE;
    break;

    case ACCESS_TECH_GSM_850:
      rr_band = SYS_BAND_CELL_850;
    break;

    case NO_BAND:
      rr_band = SYS_BAND_NONE;
    break;

    default:
      MSG_GERAN_ERROR_1_G("GMAC cannot convert RR band: %d",nw_band);
      rr_band = SYS_BAND_NONE;
    break;
  }

  return rr_band;
}

/*===========================================================================
===
===  FUNCTION    get_additional_rac_nw_list
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void get_additional_rac_nw_list(uint8 *nw_band_list_ptr,uint8 *nw_band_count_ptr,
                                       uint8 nw_band_list_size,
                                       gas_id_t gas_id)
{
  uint8 i;

  /*set the first band in the list to the BCCH band*/
  rr_read_public_store(RRPS_MS_BCCH_BAND,nw_band_list_ptr,gas_id);

  MSG_GERAN_HIGH_1_G("GMAC DEBUG, rac function, BCCH band:%d", nw_band_list_ptr[0]);

  /*set the rest of the list to a copy of the full NW band list*/
  memscpy(nw_band_list_ptr+1,nw_band_list_size-1, full_rac_list, FULL_RAC_LIST_SIZE);

  /*find the bcch band in the full NW band list and set it to NO_BAND as we don't want it
    to be present twice in the list*/
  for(i=1; i < (FULL_RAC_LIST_SIZE+1); i++)
  {
    if ( nw_band_list_ptr[i] == nw_band_list_ptr[0] )
    {
      nw_band_list_ptr[i] = NO_BAND;
      break;
    }
  }

  *nw_band_count_ptr = FULL_RAC_LIST_SIZE+1;
}


/*===========================================================================
===
===  FUNCTION    check_for_bcch_inclusion
===
===  DESCRIPTION
===
===  Checks to see if any of the NW requested bands are supported.  If none
===  are supported use the currently camped band.   This is an EGPRS requirement
===  see 44.060b 12.30.
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void check_for_bcch_inclusion( uint8 *nw_bands_count_ptr, uint8 *nw_bands_ptr,
                                      gas_id_t gas_id )
{
  uint8 i = 0;
  sys_band_T  rr_band;
  boolean at_least_one_band_supported = FALSE;

  /*KW cr657371*/
  if(*nw_bands_count_ptr > MAX_ACCESS_TECH_TYPE)
  {
    *nw_bands_count_ptr = MAX_ACCESS_TECH_TYPE;
  }

  /* check to see if any bands are supported*/
  for ( i=0; i<*nw_bands_count_ptr; i++ )
  {
     rr_band = convert_rac_band_to_rr_format(nw_bands_ptr[i],gas_id);

     if ( rr_gapi_is_supported_band(rr_band,gas_id) )
     {
        at_least_one_band_supported = TRUE;
        break;
     }
  }

  /* No bands are supported so add the BCCH band to the nw list,
     remove other bands as they are not supported*/
  if (!at_least_one_band_supported)
  {
    rr_read_public_store(RRPS_MS_BCCH_BAND,nw_bands_ptr,gas_id);
    *nw_bands_count_ptr = 1;
  }
}


/*===========================================================================
===
===  FUNCTION    modify_nw_list_for_1800_1900_band_combination_for_gprs
===
===  DESCRIPTION
===
===  Checks if both 1800 and 1900 are in the network req list and are supported
===  by the MS.  If the BCCH band is 1800 or 1900 the respective opposite band (1800 or 1900)
===  is remvoed.  If the BCCH band is not 1800 or 1900 then the NON RR 1800 or 1900
===  preferred band is removed.
===
===
===  PARAMS
===     nw_bands_count  - number of bands in nw_bands_ptr.
===     nw_bands_ptr    - array of nw bands.
===
===
===  RETURN VALUE
===     NONE
===
===  SIDE EFFECTS
===    NONE
===
===========================================================================*/
#ifdef PERLUTF
#error code not present
#else
static void modify_nw_list_for_1800_1900_band_combination_for_gprs( uint8 band_count, uint8 *nw_bands_ptr, gas_id_t gas_id )
#endif /*PERLUTF*/
{
  boolean gsm_1800_supp_in_nw_list = FALSE;
  boolean gsm_1900_supp_in_nw_list = FALSE;
  uint8 i = 0;
  sys_band_T rr_band;
  uint8 remove_band;
  uint8 bcch_band;
  uint8 nw_band_indicator = 0;

  for ( i=0; i<band_count; i++ )
  {
     if ( nw_bands_ptr[i] == ACCESS_TECH_GSM_1800 )
     {
       rr_band = convert_rac_band_to_rr_format(nw_bands_ptr[i], gas_id);

       if ( rr_gapi_is_supported_band(rr_band,gas_id) )
       {
         gsm_1800_supp_in_nw_list = TRUE;
       }
     }
     else if ( nw_bands_ptr[i] == ACCESS_TECH_GSM_1900 )
     {
       rr_band = convert_rac_band_to_rr_format(nw_bands_ptr[i], gas_id);

       if ( rr_gapi_is_supported_band(rr_band,gas_id) )
       {
         gsm_1900_supp_in_nw_list = TRUE;
       }
     }
  }

  if ( gsm_1800_supp_in_nw_list && gsm_1900_supp_in_nw_list )
  {

    rr_read_public_store(RRPS_MS_BCCH_BAND,&bcch_band, gas_id);

    /* gprs has bcch band at index 0*/

    if ( bcch_band == ACCESS_TECH_GSM_1900 )
    {
      remove_band = ACCESS_TECH_GSM_1800;
    }
    else if ( bcch_band == ACCESS_TECH_GSM_1800 )
    {
      remove_band = ACCESS_TECH_GSM_1900;
    }
    else
    {
      rr_read_public_store(RRPS_NW_BAND_IND,&nw_band_indicator,gas_id);

      if ( nw_band_indicator == ACCESS_TECH_GSM_1900 )
      {
        remove_band = ACCESS_TECH_GSM_1800;
      }
      else
      {
        remove_band = ACCESS_TECH_GSM_1900;
      }
    }

    /* gprs has bcch band at index 0 so start at index 1*/
    for ( i=1; i<band_count; i++ )
    {
      if ( nw_bands_ptr[i] == remove_band )
      {
        nw_bands_ptr[i] = NO_BAND;
        break;
      }
    }
  }
}



/*===========================================================================
===
===  FUNCTION    modify_nw_list_for_1800_1900_band_combination_for_egprs
===
===  DESCRIPTION
===
===  Checks if both 1800 and 1900 are in the network req list and are supported
===  by the MS.  Removes the none RR preferred band from the list.
===
===
===  PARAMS
===     nw_bands_count  - number of bands in nw_bands_ptr.
===     nw_bands_ptr    - array of nw bands.
===
===
===  RETURN VALUE
===     NONE
===
===  SIDE EFFECTS
===    NONE
===
===========================================================================*/
#ifdef PERLUTF
#error code not present
#else
static void modify_nw_list_for_1800_1900_band_combination_for_egprs( uint8 band_count, uint8 *nw_bands_ptr, gas_id_t gas_id )
#endif /*PERLUTF*/
{
  boolean gsm_1800_supp_in_nw_list = FALSE;
  boolean gsm_1900_supp_in_nw_list = FALSE;
  uint8 i = 0;
  sys_band_T rr_band;
  uint8 remove_band;
  uint8 nw_band_indicator = 0;

  for ( i=0; i<band_count; i++ )
  {
     if ( nw_bands_ptr[i] == ACCESS_TECH_GSM_1800 )
     {
       rr_band = convert_rac_band_to_rr_format(nw_bands_ptr[i],gas_id);

       if ( rr_gapi_is_supported_band(rr_band,gas_id) )
       {
         gsm_1800_supp_in_nw_list = TRUE;
       }
     }
     else if ( nw_bands_ptr[i] == ACCESS_TECH_GSM_1900 )
     {
       rr_band = convert_rac_band_to_rr_format(nw_bands_ptr[i],gas_id);

       if ( rr_gapi_is_supported_band(rr_band,gas_id) )
       {
         gsm_1900_supp_in_nw_list = TRUE;
       }
     }
  }

  if ( gsm_1800_supp_in_nw_list && gsm_1900_supp_in_nw_list )
  {
    rr_read_public_store(RRPS_NW_BAND_IND,&nw_band_indicator,gas_id);

    if ( nw_band_indicator == ACCESS_TECH_GSM_1900 )
    {
      remove_band = ACCESS_TECH_GSM_1800;
    }
    else
    {
      remove_band = ACCESS_TECH_GSM_1900;
    }

    for ( i=0; i<band_count; i++ )
    {
      if ( nw_bands_ptr[i] == remove_band )
      {
        nw_bands_ptr[i] = NO_BAND;
        break;
      }
    }
  }
}

/*===========================================================================
===
===  FUNCTION    modify_nw_list_for_pgsm_inhibit
===
===  DESCRIPTION
===
===  Checks if both PGSM and EGSM are in the network req list and are supported
===  by the MS.  If this is the case then PGSM is inhibited.
===
===
===  PARAMS
===     nw_bands_count  - number of bands in nw_bands_ptr.
===     nw_bands_ptr    - array of nw bands.
===
===
===  RETURN VALUE
===     TRUE - inhibit PGSM
===     FALSE - don't inhibit PGSM
===
===  SIDE EFFECTS
===    NONE
===
===========================================================================*/
static boolean modify_nw_list_for_pgsm_inhibit( uint8 band_count, uint8 *nw_bands_ptr,
                                                gas_id_t gas_id )
{
  boolean egsm_supp_in_nw_list = FALSE;
  boolean inhibit_pgsm         = FALSE;
  uint8 i = 0;
  sys_band_T rr_band;

  for ( i=0; i<band_count; i++ )
  {
     if ( nw_bands_ptr[i] == ACCESS_TECH_GSM_E )
     {
       rr_band = convert_rac_band_to_rr_format(nw_bands_ptr[i],gas_id);

       if ( rr_gapi_is_supported_band(rr_band,gas_id) )
       {
         egsm_supp_in_nw_list = TRUE;
       }
     }
  }

  if ( egsm_supp_in_nw_list == TRUE )
  {
    for ( i=0; i<band_count; i++ )
    {
      if ( nw_bands_ptr[i] == ACCESS_TECH_GSM_P )
      {
        nw_bands_ptr[i] = NO_BAND;
        inhibit_pgsm = TRUE;
        break;
      }
    }
  }

  return inhibit_pgsm;

}

/*===========================================================================
===
===  FUNCTION      gmac_util_set_access_tech_info
===
===  DESCRIPTION
===
===  Wrapper function for GMM's set_access_tech_info(), which deals with
===  various featurisation of the required parameters.
===  Note that SMS_VALUE and SM_VALUE are not required in the encoding as
===  they are CS only values.  See ETSI CR 637 for doc 24.008
===
===  RETURN VALUE  None
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_util_set_access_tech_info
(
csn_encoder_T  *encoder,
byte            access_technology,
byte            rf_power_class,
hw_classmark_T *hw_classmark,
boolean        *A5_already_included,
boolean        *gprs_msclass_already_included,
word            max_rac_length,
gas_id_t        gas_id
)
{
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id;

  /* for DUAL SIM, determine which Active Subscription ID is in use */
  as_id = geran_map_gas_id_to_nas_id(gas_id);

  return(set_access_tech_info_per_subs(encoder,
                                       access_technology,
                                       0, /* revision level indication */
                                       rf_power_class,
                                       hw_classmark,
                                       A5_already_included,
                                       gprs_msclass_already_included,
                                       max_rac_length,
                                       as_id));

#else /* not a DUAL_SIM build */

  return(set_access_tech_info(encoder,
                              access_technology,
                              0, /* revision level indication */
                              rf_power_class,
                              hw_classmark,
                              A5_already_included,
                              gprs_msclass_already_included
                             ,max_rac_length
                             ));

#endif /* FEATURE_DUAL_SIM */
}

/*===========================================================================
===
===  FUNCTION      gmac_get_8psk_power_class
===
===  DESCRIPTION
===
===  Determines the EGPRS 8PSK power class from the information provided in
===  RR public store.
===
===  RETURN VALUE  byte - 8PSK power class
===
===  SIDE EFFECTS
===
===========================================================================*/
static byte gmac_get_8psk_power_class(gas_id_t gas_id)
{
  byte gmac_8psk_capability = 0;

  boolean egprs_supported = FALSE;
  boolean egprs_8psk_ul_supported = FALSE;
  uint8 egprs_8psk_power_capability = 0;

  rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL, &egprs_supported, gas_id );

  if (egprs_supported)
  {
     rr_read_public_store(RRPS_MS_EGPRS_8PSK_UL_ENABLED,&egprs_8psk_ul_supported, gas_id);

     /* 8PSK Power Class indicates whether or not 8PSK mod in uplink is
     ** supported by the phone. Based on that, the power class shall be set.
     */
     if (egprs_8psk_ul_supported)
     {
       rr_read_public_store(RRPS_MS_EGPRS_8PSK_POWER_CAP,&egprs_8psk_power_capability, gas_id);
       gmac_8psk_capability = egprs_8psk_power_capability; /* Power Class supported by the phone */
     }
     else
     {
       gmac_8psk_capability = 0; /*8PSK UL disabled*/
     }
  }
  else
  {
    gmac_8psk_capability = 0; /*EDGE disabled*/
  }

  return gmac_8psk_capability;
}

/*===========================================================================
===
===  FUNCTION      gmac_get_gpsk_power_class
===
===  DESCRIPTION
===
===  Determines the GPSK power class using the RR function: rr_gapi_get_band_rf_power_class()
===
===  RETURN VALUE  byte - GPSK power class
===
===  SIDE EFFECTS
===
===========================================================================*/
static byte gmac_get_gpsk_power_class(byte access_tech,
                                      gas_id_t gas_id)
{
  byte rf_power_capability = 0;
  sys_band_T  rr_band;
  byte rf_power_class = 0;

  rr_band = convert_rac_band_to_rr_format(access_tech,gas_id);
  rf_power_class = rr_gapi_get_band_rf_power_class((rr_internal_band_T)rr_band,gas_id);

  switch (rf_power_class)
  {
    case CLASS_1:
      rf_power_capability = 1;
      break;
    case CLASS_2:
      rf_power_capability = 2;
      break;
    case CLASS_3:
      rf_power_capability = 3;
      break;
    case CLASS_4:
      rf_power_capability = 4;
      break;
    case CLASS_5:
      rf_power_capability = 5;
      break;
    default:
      ERR("Invalid power class=%d",(int)rf_power_class,0,0);
      break;
  } /* switch */

  return rf_power_capability;
}

typedef enum
{
  MAC_ADD_RAC_ALL_RACS_ENCODED = 1,
  MAC_ADD_RAC_NO_RACS_ENCODED
} mac_add_rac_result_t;

/* Acc Tech Type (1111)        - 4
   Length Field                - 7*/
#define MAC_RAC_ADD_CODE_FIXED_BITS 11

/* Additional Acc Tech presence - 1
   Acc tech type                - 4
   GMSK Power Class             - 3
   8PSK Power Class             - 2*/
#define MAC_RAC_ADD_CODE_ACC_TECH_BITS 10

/* Additional Acc Tech presence - 1*/
#define MAC_RAC_ADD_ACC_TECH_LAST_PRESENCE_BIT 1

/*===========================================================================
===
===  FUNCTION      gmac_encode_add_rac
===
===  DESCRIPTION
===
===  Encodes the additional or alternative encoding of the Radio Access
===  Capabilities IE.
===
===  RETURN VALUE  mac_add_rac_result_t
===
===  SIDE EFFECTS
===
===========================================================================*/
static mac_add_rac_result_t gmac_encode_add_rac(csn_encoder_T  *encoder ,
                                                byte           *access_tech,
                                                byte            acc_tech_num,
                                                word            max_rac_length,
                                                boolean         inhibit_pgsm,
                                                gas_id_t        gas_id
)
{
   uint8 add_rac_total_size = 0;
   uint8 i = 0;
   uint8 num_supported_bands = 0;

   /*find the number of supported bands*/
   for (i=0; i<acc_tech_num; i++)
   {
     sys_band_T rr_band = convert_rac_band_to_rr_format(access_tech[i],gas_id);

     if ( supported_band_valid(inhibit_pgsm, rr_band, gas_id) == TRUE )
     {
       num_supported_bands++;
     }
   }

   /*Calculate if we can fit in the requested bands*/
   add_rac_total_size =
     MAC_RAC_ADD_CODE_FIXED_BITS + (num_supported_bands * MAC_RAC_ADD_CODE_ACC_TECH_BITS) + MAC_RAC_ADD_ACC_TECH_LAST_PRESENCE_BIT;

   if (add_rac_total_size > max_rac_length)
   {
     MSG_GERAN_HIGH_3_G("MAC ADD RAC No space left: max_rac_len:%d, add_rac_tot_size:%d, num_rac:%d",
                        max_rac_length, add_rac_total_size, num_supported_bands);
     return MAC_ADD_RAC_NO_RACS_ENCODED;
   }

   MSG_GERAN_HIGH_3_G("RR formatting %d RAC's in Additional RAC format, avail size %d, rac length %d",
                      num_supported_bands, max_rac_length, add_rac_total_size);

   /*Access Technology Type*/
   csn_encode_bits(encoder, 4, (byte)0x0f); /*indicates additional technologies*/

   /*Length Field*/
   csn_encode_bits(encoder, 7, (byte)((num_supported_bands * MAC_RAC_ADD_CODE_ACC_TECH_BITS) + MAC_RAC_ADD_ACC_TECH_LAST_PRESENCE_BIT));

   /*encode the RAC's in order*/
   for (i=0; i<acc_tech_num; i++)
   {
     sys_band_T rr_band = convert_rac_band_to_rr_format(access_tech[i], gas_id);

     if ( supported_band_valid(inhibit_pgsm, rr_band, gas_id) == TRUE )
     {
       /*Additional Access Tech Presence*/
       csn_encode_bit(encoder, (byte) 1 );

       /*Access Tech Type*/
       csn_encode_bits(encoder, 4, (byte)access_tech[i]);

       /*GSMK Power Class*/
       csn_encode_bits(encoder, 3, gmac_get_gpsk_power_class(access_tech[i], gas_id));

       /*8PSK Power Class*/
       csn_encode_bits(encoder, 2, gmac_get_8psk_power_class(gas_id));
     }
   }

   /*Additional Access Tech Presence*/
   csn_encode_bit(encoder, (byte) 0 );

   return MAC_ADD_RAC_ALL_RACS_ENCODED;
}

/*===========================================================================
===
===  FUNCTION      gmac_idle_egprs_rac_procedure_2()
===
===  DESCRIPTION
===
===  This function is called when initiating the Radio Access Capability process.
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_idle_egprs_rac_procedure_2
(
boolean two_phase_access,
boolean arac_retran,
boolean reformat_arac,
boolean format_gprs_rac,
gas_id_t gas_id
)
{
  #define RAC_PRESENT_BIT_LENGTH 1

  ghdi_status_T       status;
  csn_encoder_T       prr_encoder;
  csn_encoder_T       arac_encoder;
  boolean             prr_full = FALSE;
  uint8               i = 0;
  boolean             inhibit_pgsm = FALSE;
  uint16              start_of_rac_bit_pos = 0;
  uint16              end_of_rac_bit_pos = 0;
  uint8               max_prr_size_avail = 0;
  uint8               max_arac_size_avail = 0;
  hw_classmark_T      hw_classmark;
  sys_band_T          rr_band;
  boolean             a5_already_included = TRUE; /* CR 91485. PRR has a length limitation so A5 bits are not included*/
  boolean             gprs_msclass_already_included = FALSE;
  uint8               old_byte_pos = 0;
  uint8               old_bit_pos = 0;
  boolean             first_band_included = FALSE;
  boolean             truncated = FALSE;
  word                truncate_length = 0; /* Length in bits of max RAC allowed, 0 - means do not truncate */
  boolean             inc_add_rac_as_first_in_arac = FALSE;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_util_data_t   *util_data_ptr = gmac_get_util_data_ptr(gas_id);
  uint8               rac_length = 0;

  /* It is an error to have arac_retran set when this function is called*/
  if(arac_retran == FALSE)
  {

    /**************************************************************************************
    *                 Parameter Setting and Initialisation                                *
    **************************************************************************************/

    if (format_gprs_rac)
    {
      get_additional_rac_nw_list(util_data_ptr->nw_bands,&util_data_ptr->nw_bands_count,sizeof(util_data_ptr->nw_bands),gas_id);

      modify_nw_list_for_1800_1900_band_combination_for_gprs( util_data_ptr->nw_bands_count, util_data_ptr->nw_bands, gas_id );
    }
    else if(global_data_ptr->access_tech_req_present == TRUE) /* work out the NW list, number of bands requested */
    {
      MSG_GERAN_MED_1_G("MAC info, NW requested:%d bands in RAC",global_data_ptr->access_tech_req_count);
      /* NW list is the number of bands requested in the UL assignment msg*/
      memscpy(util_data_ptr->nw_bands,sizeof(util_data_ptr->nw_bands),
              global_data_ptr->access_tech_req, global_data_ptr->access_tech_req_count);

      util_data_ptr->nw_bands_count = global_data_ptr->access_tech_req_count;

      /* if none of the bands requested by the NW are supported then include
         the BCCH band*/
      check_for_bcch_inclusion( &util_data_ptr->nw_bands_count,util_data_ptr->nw_bands, gas_id );

      modify_nw_list_for_1800_1900_band_combination_for_egprs( util_data_ptr->nw_bands_count, util_data_ptr->nw_bands,
                                                               gas_id );
    }
    else
    {
      MSG_GERAN_MED_0_G("MAC info, No bands req by NW, using BCCH band");
      /* No bands have been requested so use the BCCH band currently camped on*/
      rr_read_public_store(RRPS_MS_BCCH_BAND,util_data_ptr->nw_bands,gas_id);
      util_data_ptr->nw_bands_count = 1;
    }

    if ( modify_nw_list_for_pgsm_inhibit( util_data_ptr->nw_bands_count, util_data_ptr->nw_bands, gas_id ) )
    {
      MSG_GERAN_MED_0_G("GMAC INFO, inhibiting PGSM in the nw_band list");
      inhibit_pgsm = TRUE;
    }

    /* work out free space avail in the PRR */
    max_prr_size_avail = 176 - gmac_calc_pkt_resource_req_size(TRUE,format_gprs_rac,gas_id);

    /* work out free space avail in the ARAC */
    max_arac_size_avail =  176 - ( SIZE_OF_ARAC_MESSAGE_TYPE + PRESENT_NOT_PRESENT_BIT + SIZE_OF_ARAC_TLLI );

    /* memset prr, arac ie's */
    memset( (void*)&util_data_ptr->prr_ie, 0, sizeof(ms_cap_t) );
    memset( (void*)&util_data_ptr->arac_ie, 0, sizeof(ms_cap_t) );

    /* ghdi_read_hw_classmark*/
    status = ghdi_read_hw_classmark( &hw_classmark);
    if (status != GHDI_SUCCESS)
    {
      ERR("ERROR: Unable to read classmark: status=%d",status,0,0);
    }

    /*csn encoder init*/
    csn_encoder_init(&prr_encoder, util_data_ptr->prr_ie.ms_ra_cap_ie, sizeof(util_data_ptr->prr_ie.ms_ra_cap_ie));
    csn_encoder_init(&arac_encoder, util_data_ptr->arac_ie.ms_ra_cap_ie, sizeof(util_data_ptr->arac_ie.ms_ra_cap_ie));

    prr_full = FALSE;

   /**************************************************************************************
    *                 Main loop for constructing PRR and ARAC RACs                       *
    **************************************************************************************/
    for ( i=0; i<util_data_ptr->nw_bands_count; i++ )
    {
      /************ Constructing a PRR RAC *******************/
      if ( (!prr_full) && (!reformat_arac) )
      {
        rr_band = convert_rac_band_to_rr_format(util_data_ptr->nw_bands[i],gas_id);

        if ( supported_band_valid(inhibit_pgsm, rr_band, gas_id) )
        {
          MSG_GERAN_HIGH_1_G("GMAC formatting PRR RAC band:%d",util_data_ptr->nw_bands[i]);

          if ( first_band_included )
          {
            mac_add_rac_result_t result = {0};

            prr_encoder.byte_pos = old_byte_pos;
            prr_encoder.bit_pos = old_bit_pos;


            start_of_rac_bit_pos = csn_bit_pos(&prr_encoder);

            /*MS RA value part struct presence*/
            csn_encode_bit(&prr_encoder, (byte) 1 );

            if (util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits >= max_prr_size_avail)
            {
              rac_length = 0;
            }
            else
            {
              rac_length = max_prr_size_avail - util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits - RAC_PRESENT_BIT_LENGTH;
            }

            /*First band was successfuly included, try and add the other bands using the
                additional Radio access capabilities struct*/
            result = gmac_encode_add_rac(&prr_encoder ,
                                          &util_data_ptr->nw_bands[i],
                                          (util_data_ptr->nw_bands_count-i),
                                           rac_length,
                                           inhibit_pgsm,
                                           gas_id);

            if (result == MAC_ADD_RAC_ALL_RACS_ENCODED)
            {
              /* end_of_rac_bit_pos - start_of_rac_bit_pos = size of the rac in bits */
              end_of_rac_bit_pos = csn_bit_pos(&prr_encoder);

              /* add current rac to the overall size*/
              util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits = util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits + ( end_of_rac_bit_pos - start_of_rac_bit_pos );

              /*Encoding has finished ARAC not required*/
              /*MS RA value part struct presence*/
              csn_encode_bit(&prr_encoder, (byte) 0 );
              break;
            }
            else if (result == MAC_ADD_RAC_NO_RACS_ENCODED)
            {
              /*Additional RAC didn't fit in the PRR, include in the ARAC*/
              prr_encoder.byte_pos = old_byte_pos;
              prr_encoder.bit_pos = old_bit_pos;

              /*MS RA value part struct presence*/
              csn_encode_bit(&prr_encoder, (byte) 0 );

              /* reset the old bit/byte position for the ARAC*/
              old_byte_pos = 0;
              old_bit_pos = 0;
              first_band_included = TRUE;

              /*GP-100152 clarification.  If the PRR contains a complete (untruncated) band
                then the ARAC can start with the additional RAC struct*/
              inc_add_rac_as_first_in_arac = TRUE;

              prr_full = TRUE;
            }

            truncate_length = 0; /*no size specified*/
          }
          else
          {
            /* Don't write a 1 bit as this is the first band to be included*/

            first_band_included = TRUE;
            /*only the first access tech of the RAC is allowed to be truncated*/
            truncate_length = max_prr_size_avail - RAC_PRESENT_BIT_LENGTH;
          }

          if (!prr_full)
          {
            start_of_rac_bit_pos = csn_bit_pos(&prr_encoder);

            /* record old values in case the RAC is too big and is included in the ARAC instead*/
            truncated = gmac_util_set_access_tech_info(&prr_encoder,
                                                       util_data_ptr->nw_bands[i],
                                                       rr_gapi_get_band_rf_power_class((rr_internal_band_T)rr_band,gas_id),
                                                       &hw_classmark,
                                                       &a5_already_included,
                                                       &gprs_msclass_already_included,
                                                       truncate_length,
                                                       gas_id);

            /* end_of_rac_bit_pos - start_of_rac_bit_pos = size of the rac in bits */
            end_of_rac_bit_pos = csn_bit_pos(&prr_encoder);

            /* add current rac to the overall size*/
            util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits = util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits + ( (end_of_rac_bit_pos - start_of_rac_bit_pos ) + RAC_PRESENT_BIT_LENGTH );

            if ( util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits > max_prr_size_avail )
            {
              /* the current RAC is too large so it is not being included in the PRR*/
              if (truncate_length != 0)
              {
                MSG_GERAN_ERROR_2_G("GMAC requested truncated RAC len:%d received len:%d",
                                    truncate_length,(end_of_rac_bit_pos - start_of_rac_bit_pos));
              }

              /* subtract current rac from the overall size*/
              util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits = util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits - ( (end_of_rac_bit_pos - start_of_rac_bit_pos ) + RAC_PRESENT_BIT_LENGTH );

              if ( util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits == 0 )
              {
                MSG_GERAN_ERROR_0_G("GMAC GPRS PRR, NO RAC bands included, PRR too long!");
              }

              MSG_GERAN_HIGH_1_G("GMAC RAC band:%d not included in PRR not enough space",util_data_ptr->nw_bands[i]);

              prr_full = TRUE; /* start formatting the ARAC*/
              first_band_included = FALSE;
              a5_already_included = FALSE;
              gprs_msclass_already_included = FALSE;

              /* set the prr csn bit position to the end of the previous rac in order to write the 0 bit*/
              prr_encoder.byte_pos = old_byte_pos;
              prr_encoder.bit_pos = old_bit_pos;

              csn_encode_bit(&prr_encoder, (byte) 0 );

              /* reset the old bit/byte position for the ARAC*/
              old_byte_pos = 0;
              old_bit_pos = 0;
            }
            else
            {
              /* set the old byte/bit pos to the end of the current RAC*/
              old_byte_pos = prr_encoder.byte_pos;
              old_bit_pos = prr_encoder.bit_pos;

              if (truncated)
              {
                MSG_GERAN_HIGH_1_G("GMAC PRR band:%d Truncated RAC",util_data_ptr->nw_bands[i]);

                prr_full = TRUE; /*if the first RAC was truncated then we need to format the ARAC with
                                   the same band again*/

                first_band_included = FALSE;
                a5_already_included = FALSE;
                gprs_msclass_already_included = FALSE;

                /* reset the old bit/byte position for the ARAC*/
                old_byte_pos = 0;
                old_bit_pos = 0;
              }

              /* Write a 0 bit at the end to indicate no further RAC's in PRR */
              csn_encode_bit(&prr_encoder, (byte) 0 );
            }
          } /*!prr_full*/
        }
      }

      /***************** Constructing an ARAC RAC ***********************/
      if ( ( prr_full ) || ( reformat_arac ) )
      {
        rr_band = convert_rac_band_to_rr_format(util_data_ptr->nw_bands[i],gas_id);

        if ( supported_band_valid(inhibit_pgsm, rr_band, gas_id) )
        {
          MSG_GERAN_HIGH_1_G("GMAC formatting ARAC RAC band:%d",util_data_ptr->nw_bands[i]);

          if ( first_band_included || inc_add_rac_as_first_in_arac)
          {
            mac_add_rac_result_t result = {0};

            /* write a 1 bit as there is a band before the current one in the ARAC*/
            arac_encoder.byte_pos = old_byte_pos;
            arac_encoder.bit_pos = old_bit_pos;

            start_of_rac_bit_pos = csn_bit_pos(&arac_encoder);

            if (!inc_add_rac_as_first_in_arac)
            {
              /*MS RA value part struct presence*/
              csn_encode_bit(&arac_encoder, (byte) 1 );
            }

            inc_add_rac_as_first_in_arac = FALSE;

            /*First band was successfuly included, try and add the other bands using the
                additional Radio access capabilities struct*/
            result = gmac_encode_add_rac(&arac_encoder,
                                          &util_data_ptr->nw_bands[i],
                                          util_data_ptr->nw_bands_count-i,
                                          (max_arac_size_avail - util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits - RAC_PRESENT_BIT_LENGTH),
                                          inhibit_pgsm,
                                          gas_id);

            if (result == MAC_ADD_RAC_ALL_RACS_ENCODED)
            {
              /*Encoding has finished*/
              /* end_of_rac_bit_pos - start_of_rac_bit_pos = size of the rac in bits */
              end_of_rac_bit_pos = csn_bit_pos(&arac_encoder);

              /* add current rac to the overall size*/
              util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits = util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits + ( end_of_rac_bit_pos - start_of_rac_bit_pos );

              /*MS RA value part struct presence*/
              csn_encode_bit(&arac_encoder, (byte) 0 );

              break;
            }
            else if (result == MAC_ADD_RAC_NO_RACS_ENCODED)
            {
              /*Additional RAC didn't fit in the PRR, include in the ARAC*/
              arac_encoder.byte_pos = old_byte_pos;
              arac_encoder.bit_pos = old_bit_pos;

              /*MS RA value part struct presence*/
              csn_encode_bit(&arac_encoder, (byte) 0 );

              /*Additional RAC didn't fit in the ARAC, this is a problem!!!!*/
              MSG_GERAN_ERROR_1_G("MAC could not fit Additional RAC into the ARAC, num_bands:%d",util_data_ptr->nw_bands_count-i);
              break;
            }
          }
          else
          {
            /* Don't write a 1 bit as this is the first band to be included*/
            first_band_included = TRUE;
          }

          start_of_rac_bit_pos = csn_bit_pos(&arac_encoder);

          (void)gmac_util_set_access_tech_info(&arac_encoder,
                                               util_data_ptr->nw_bands[i],
                                               rr_gapi_get_band_rf_power_class((rr_internal_band_T)rr_band,gas_id),
                                               &hw_classmark,
                                               &a5_already_included,
                                               &gprs_msclass_already_included,
                                               0,
                                               gas_id); /* truncation not allowed in ARAC */

          end_of_rac_bit_pos = csn_bit_pos(&arac_encoder);

          /* end_of_rac_bit_pos - start_of_rac_bit_pos = size of the rac in bits */
          util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits = util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits + ( (end_of_rac_bit_pos - start_of_rac_bit_pos) + RAC_PRESENT_BIT_LENGTH );

          if ( util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits > max_arac_size_avail )
          {
            /* end_of_rac_bit_pos - start_of_rac_bit_pos = size of the rac in bits */
            util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits = util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits - ( (end_of_rac_bit_pos - start_of_rac_bit_pos) + RAC_PRESENT_BIT_LENGTH );

            /* set the arac csn bit position to the end of the previous rac in order to write the 0 bit*/
            arac_encoder.byte_pos = old_byte_pos;
            arac_encoder.bit_pos = old_bit_pos;

            MSG_GERAN_HIGH_1_G("GMAC RAC band:%d not included in ARAC not enough space",util_data_ptr->nw_bands[i]);

            csn_encode_bit(&arac_encoder, (byte) 0 );
            break;
          }
          else
          {
            old_byte_pos = arac_encoder.byte_pos;
            old_bit_pos = arac_encoder.bit_pos;

            csn_encode_bit(&arac_encoder, (byte) 0 );
          }
        }
      }
    }

    /**************************************************************************************
    *                 2 phase access handling of PRR and ARAC msg sending                 *
    **************************************************************************************/

    if(!format_gprs_rac)
    {
      if(two_phase_access == TRUE)
      {
        if ( reformat_arac == FALSE )
        {
          /*2 phase access handling of PRR and ARAC msg sending*/
          /* arac length is not zero so the rac is split across prr and arac msgs*/
          if( util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits != 0 )
          {
            MSG_GERAN_MED_0_G("MAC info EGPRS sent PRR, not all RAC info contained in PRR");

            gmac_send_rac_prr(NO_RE_EST,TRUE,two_phase_access,gas_id);

            if(global_data_ptr->egprs_2p_sb_mb_alloc == MB_ALLOC_OF_2_BLOCKS)
            {
                MSG_GERAN_MED_0_G("MAC info EGPRS sent ARAC with RAC info");
                gmac_send_add_rac_info_msg(NO_RE_EST,gas_id);
            }
            else if( (global_data_ptr->egprs_2p_sb_mb_alloc == EGPRS_SB_ALLOC) || (global_data_ptr->egprs_2p_sb_mb_alloc == MB_ALLOC_OF_1_BLOCK))
            {
              /*Under these conditions when the NW sends the PUA containing the UL tbf allocation it may*/
              /*request the transmission of the ARAC msg that could not be sent ref 4.60 7.1.3.2.1*/
              /*ie wait for the PUA with the dynamic alloc to be rcvd at which point build the ARAC at that point*/
              MSG_GERAN_MED_0_G("MAC info EGPRS 2p rac,1 block alloc,but PRR and ARAC required,only PRR sent");
            }
          }
          else
          {
            /*ALL additional RAC info can fits into PRR*/
            MSG_GERAN_HIGH_0_G("MAC info EGPRS sent PRR, contained all RAC info");
            gmac_send_rac_prr(NO_RE_EST,FALSE,two_phase_access,gas_id);

            if(global_data_ptr->egprs_2p_sb_mb_alloc == MB_ALLOC_OF_2_BLOCKS)
            {
              /*always send dummy control block if a mb alloc of 2 blocks is rcvd, done when freq_cnf is rcvd*/
              MSG_GERAN_MED_0_G("MAC info EGPRS 2p rac, 2 block alloc , but only PRR required, send dummy control");
            }
          }
        }
        else
        {
          MSG_GERAN_MED_0_G("MAC info EGPRS reformatting the ARAC RAC");
        }
      }
      else
      {

        /**************************************************************************************
        *                 1 phase access handling of PRR and ARAC msg sending                 *
        **************************************************************************************/

        if( util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits != 0 )
        {
          /*re-set the calculated MS RA cap length ind to indicate the length of the RAC Ie's */
          /*to be included in the PRR (note the csn1 encoder functionality uses the length ind*/
          /*to extract the RAC information and put it into the PRR and hence automatically    */
          /*removes the RAc IE's that are not to be include in the PRR                        */

          MSG_GERAN_MED_0_G("MAC info EGPRS sent PRR with RAC info and ARAC msg ");

          gmac_send_rac_prr(NO_RE_EST,TRUE,two_phase_access,gas_id);

          gmac_send_add_rac_info_msg(NO_RE_EST,gas_id);
        }
        else
        {
          /*ALL additional RAC info can fits into PRR*/
          MSG_GERAN_MED_0_G("MAC info EGPRS PRR contained all RAC info");

          gmac_send_rac_prr(NO_RE_EST,FALSE,two_phase_access,gas_id);
        }
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("MAC Should not handle ARAC retrans in this function");
  }
}


/*===========================================================================
===
===  FUNCTION    gmac_reset_rac_params()
===
===  Should be called when releasing the UL tbf OR when moving to TRANSFER mode
===
===  DEPENDENCIES
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_reset_rac_params(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  global_data_ptr->c_flags.egprs_rac_retran_req = FALSE;
  global_data_ptr->c_flags.egprs_prr_sent = FALSE;
  global_data_ptr->c_flags.egprs_arac_sent = FALSE;
  global_data_ptr->egprs_2p_sb_mb_alloc = GPRS_SB_ALLOC;

  /*data rcvd form PUA*/
  global_data_ptr->access_tech_req_present = FALSE;
  global_data_ptr->access_tech_req_count = 0;
  memset(global_data_ptr->access_tech_req,0,MAX_ACCESS_TECH_TYPE);

  util_data_ptr->nw_bands_count = 0;
  memset(util_data_ptr->nw_bands, 0, MAX_ACCESS_TECH_TYPE);

  /*reset prr ie*/
  util_data_ptr->prr_ie.ms_ra_cap_ie_len_bits = 0;
  memset(util_data_ptr->prr_ie.ms_ra_cap_ie,0,MAX_MS_RA_CAP_IE_LEN);

  /*reset arac ie*/
  util_data_ptr->arac_ie.ms_ra_cap_ie_len_bits = 0;
  memset(util_data_ptr->arac_ie.ms_ra_cap_ie,0,MAX_MS_RA_CAP_IE_LEN);

  /*used to store the csn encoded PRR msg sent as part of the RAC process*/
  memset(util_data_ptr->csn_encoded_rac_prr_msg,0,23);

  /*used to store the csn encoded ARAC msg sent as part of the RAC process*/
  memset(util_data_ptr->csn_encoded_rac_arac_msg,0,23);
}

/*===========================================================================
===
===  FUNCTION      gmac_egprs_cont_res_pua_freq_param_ie_util()
===
===  DESCRIPTION
===  Checks the frequency parameters received in an PUA assignment message,
===  rcvd as part of EGPRS 1p contetnion res, and returns the appropriate
===  value associated with the validity of the frequency assignment
===
===  DEPENDENCIES
===  NOTE this function is not dependent on
===       csn_dec_hdr_ptr - pointer to the CSN.1 decoded logical channel message
===
===  RETURN VALUE
===  Returns the error cause and recover type (pua_data_error_t):
===  - NO_ERRORS
===  - FREQ_ERROR_ACCESS_RETRY
===  - FREQ_ERROR_PARTIAL_SYS_ACQ
===
===  SIDE EFFECTS
===
===========================================================================*/
pua_data_error_t gmac_egprs_cont_res_pua_freq_param_ie_util(boolean        compare_flag,
                                                            boolean        send_freq_config,
                                                            freq_params_t *csn_pua_freq_param_ptr,
                                                            gas_id_t       gas_id)
{
  pua_data_error_t return_value = NO_ERRORS;

  /* pointer "frequency list" held in GRR public store */
  rrps_frequency_list_t   *frequency_list_ptr;

  mac_l1_sig_t            freq_config_sig;
  frequency_information_T *ml_freq_config_info_ptr;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);
  ml_freq_config_info_ptr = &(freq_config_sig.mac_l1_freq_config.frequency_information);

  /********************************************************************************
  *       determine and load "MAC_L1_FREQ_CONFIG" signal data for the UL TBF      *
  *********************************************************************************/

  /* clear MAC_L1_FREQ_CONFIG sig buffer */
  memset (ml_freq_config_info_ptr,0, sizeof(frequency_information_T));

  switch (csn_pua_freq_param_ptr->encoding_type)
  {
    case arfcn_valid:
      return_value = gmac_util_freq_arfcn_valid(csn_pua_freq_param_ptr,ml_freq_config_info_ptr,gas_id);
      break;

    case direct1_valid:
       /*GPRS Mobile Allocation*/
      ml_freq_config_info_ptr->maio = csn_pua_freq_param_ptr->dir_enc1.maio;
      ml_freq_config_info_ptr->tsc = csn_pua_freq_param_ptr->tsc ;
      frequency_list_ptr = rr_packet_assignment_mobile_alloc(direct1_valid,
                                                             &(csn_pua_freq_param_ptr->dir_enc1),
                                                             &(util_data_ptr->identical_freq_list),
                                                              gas_id );

      if(frequency_list_ptr != NULL)
      {
        return_value =  gmac_util_load_freq_sig_data(frequency_list_ptr,
                                                     ml_freq_config_info_ptr,direct1_valid,
                                                     gas_id);
      }
      else
      {
        return_value = FREQ_ERROR_PARTIAL_SYS_ACQ;
        MSG_GERAN_LOW_0_G("MAC info freq param MA number");
      }
      break;

    case direct2_valid:
      /*MA Frequency List contents*/
      ml_freq_config_info_ptr->maio = csn_pua_freq_param_ptr->dir_enc2.maio;
      ml_freq_config_info_ptr->hsn = csn_pua_freq_param_ptr->dir_enc2.hsn;
      ml_freq_config_info_ptr->tsc = csn_pua_freq_param_ptr->tsc ;
      frequency_list_ptr = rr_packet_assignment_mobile_alloc( direct2_valid,
                                                              &(csn_pua_freq_param_ptr->dir_enc2),
                                                              &(util_data_ptr->identical_freq_list),
                                                              gas_id );

      if(frequency_list_ptr != NULL)
      {
        return_value = gmac_util_load_freq_sig_data(frequency_list_ptr,
                                                    ml_freq_config_info_ptr,
                                                    direct2_valid,
                                                    gas_id);
      }
      else
      {
        return_value = FREQ_ERROR_PARTIAL_SYS_ACQ;
        MSG_GERAN_LOW_0_G("MAC info freq param MA number");
      }
      break;

    case indirect_valid:
      ml_freq_config_info_ptr->maio = csn_pua_freq_param_ptr->indir_enc.maio;
      ml_freq_config_info_ptr->tsc = csn_pua_freq_param_ptr->tsc ;
      return_value = gmac_util_freq_indirect_valid(csn_pua_freq_param_ptr,
                                                   ml_freq_config_info_ptr,
                                                   gas_id);
      break;

    default:
      MSG_GERAN_LOW_0_G("MAC info freq param encode error");
      return_value = FREQ_ERROR_PARTIAL_SYS_ACQ;
      break;
  }

  /********************************************************************************
  *  signal sending part depending on the validity of the frequency assigment     *
  *********************************************************************************/
  if ((compare_flag == TRUE) &&
      (gmac_util_compare_freq_params(&global_data_ptr->gmac_store_freq_params_in_use,
                                     ml_freq_config_info_ptr,
                                     gas_id)) == FALSE)
  {
    return_value = FREQ_ERROR_DIFFERENT_PARAMS;
  }

  /* If compare flag set then frequencies must be the same as currently in used
  ** therefore no need to send parameters to L1
  */

  if( (return_value == NO_ERRORS) && (send_freq_config == TRUE))
  {
    global_data_ptr->gmac_store_freq_params_in_use = *ml_freq_config_info_ptr;

    MSG_GERAN_MED_0_G("MAC Sent MAC_L1_FREQ_CONFIG");
    freq_config_sig.mac_l1_freq_config.tbf_freq_cnf = UL_FREQ_CONFIG;
    freq_config_sig.mac_l1_freq_config.reuse_sb_frq_alloc = FALSE;
    freq_config_sig.mac_l1_freq_config.message_header.message_set = MS_MAC_L1;
    freq_config_sig.mac_l1_freq_config.message_header.message_id = (uint8)MAC_L1_FREQ_CONFIG;
    gprs_l1_mac_acc_func(&freq_config_sig, gas_id);
  }
  return (return_value);
}


/*===========================================================================

FUNCTION gmac_csn_data_ind_to_grr

DESCRIPTION
  Sends a MAC_GRR_DATA_IND containing data from the supplied CSN decode structure

PARAMS
  *csn_decoder_ptr - pointer to a CSN decoder output structure
  source_arfcn     - the ARFCN that the DATA_IND should appear to be from

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
static void gmac_csn_data_ind_to_grr(csn_decoder_ptrs_t *csn_decoder_ptr, ARFCN_T source_arfcn, gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  global_data_ptr->grr_sig.mac_grr_data_ind.data_ptr = *csn_decoder_ptr;
  global_data_ptr->grr_sig.mac_grr_data_ind.bcch_arfcn = source_arfcn;

  global_data_ptr->grr_sig.mac_grr_data_ind.message_header.message_id  = (uint8)MAC_GRR_DATA_IND;
  global_data_ptr->grr_sig.mac_grr_data_ind.message_header.message_set = MS_MAC_RR;

  gprs_grr_mac_acc_func(&global_data_ptr->grr_sig, gas_id);

  return;
}

/*===========================================================================

FUNCTION gmac_util_send_serving_si

DESCRIPTION
  Forwards the contents of a PACKET_SERVING_CELL_SI control message to GRR

PARAMS
   csn_result_ptr - pointer to the output from the CSN decoder
   arfcn          - the ARFCN that the DATA_IND should appear to be from

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
void gmac_util_send_serving_si(csn_decoder_ptrs_t *csn_result_ptr, ARFCN_T arfcn,
                               gas_id_t gas_id)
{
  gmac_csn_data_ind_to_grr(csn_result_ptr, arfcn, gas_id);

  return;
}

/*===========================================================================

FUNCTION gmac_util_init_container

DESCRIPTION
  Initialises the contents of a cell_data_t structure

PARAMS
  *cell_data_ptr - pointer to cell data that should be initialised

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
static void gmac_util_init_container(cell_data_t *cell_data_ptr, gas_id_t gas_id)
{
  uint8 i;

  cell_data_ptr->container_valid = FALSE;
  cell_data_ptr->arfcn.num = 0xFFFF;
  cell_data_ptr->arfcn.band = (sys_band_T)0xFF;
  cell_data_ptr->si_container.data_ptr            = NULL;
  cell_data_ptr->si_container.num_octets          = 0;
  cell_data_ptr->si_container.num_segments        = 0;
  cell_data_ptr->si_container.segment_end_points  = NULL;
  cell_data_ptr->si_container.current_msgs        = NULL;
  cell_data_ptr->si_container.previous_msgs       = NULL;
  cell_data_ptr->si_container.num_previous        = 0;
  cell_data_ptr->si_container.num_current         = 0;

  cell_data_ptr->psi_container.data_ptr           = NULL;
  cell_data_ptr->psi_container.num_octets         = 0;
  cell_data_ptr->psi_container.num_segments       = 0;
  cell_data_ptr->psi_container.segment_end_points = NULL;
  cell_data_ptr->psi_container.current_msgs       = NULL;
  cell_data_ptr->psi_container.previous_msgs      = NULL;
  cell_data_ptr->psi_container.num_previous       = 0;
  cell_data_ptr->psi_container.num_current        = 0;

  cell_data_ptr->instances_rcvd = 0x00000000;
  for (i = 0; i < MAX_NUMBER_CELL_DATA_INSTANCES; i++)
  {
    cell_data_ptr->instance_ptrs[i] = NULL;
  }

  return;
}

/*===========================================================================

FUNCTION gmac_util_free_instance_ptrs

DESCRIPTION
  Frees the msg instance ptrs for a specified container

PARAMS
  *cell_data_ptr - pointer to container being freed

RETURN VALUE
  None

SIDE EFFECTS
  Also resets the "instances_rcvd" bitmap for the supplied container

===========================================================================*/
static void gmac_util_free_instance_ptrs(cell_data_t *cell_data_ptr,
                                         gas_id_t     gas_id)
{
  uint8 index;

  for (index = 0; index < MAX_NUMBER_CELL_DATA_INSTANCES; index++)
  {
    if (cell_data_ptr->instance_ptrs[index])
    {
      GPRS_MEM_FREE(cell_data_ptr->instance_ptrs[index]);
    }
  }

  return;
}

/*===========================================================================

FUNCTION gmac_util_free_container_ptrs

DESCRIPTION
  Frees the bytestream and segment endpoint ptrs for a specified container

PARAMS
  *cell_data_ptr - pointer to container being freed

RETURN VALUE
  None

SIDE EFFECTS
  Resets the count values for segment data and endpoints

===========================================================================*/
static void gmac_util_free_container_ptrs(cell_data_t *cell_data_ptr,
                                          gas_id_t     gas_id)
{
  if (cell_data_ptr->si_container.data_ptr)
  {
    GPRS_MEM_FREE(cell_data_ptr->si_container.data_ptr);
  }

  if (cell_data_ptr->si_container.segment_end_points)
  {
    GPRS_MEM_FREE(cell_data_ptr->si_container.segment_end_points);
  }

  if (cell_data_ptr->si_container.current_msgs)
  {
    GPRS_MEM_FREE(cell_data_ptr->si_container.current_msgs);
  }

  if (cell_data_ptr->si_container.previous_msgs)
  {
    GPRS_MEM_FREE(cell_data_ptr->si_container.previous_msgs);
  }

  if (cell_data_ptr->psi_container.data_ptr)
  {
    GPRS_MEM_FREE(cell_data_ptr->psi_container.data_ptr);
  }

  if (cell_data_ptr->psi_container.segment_end_points)
  {
    GPRS_MEM_FREE(cell_data_ptr->psi_container.segment_end_points);
  }

  if (cell_data_ptr->psi_container.current_msgs)
  {
    GPRS_MEM_FREE(cell_data_ptr->psi_container.current_msgs);
  }

  if (cell_data_ptr->psi_container.previous_msgs)
  {
    GPRS_MEM_FREE(cell_data_ptr->psi_container.previous_msgs);
  }

  return;
}

/*===========================================================================

FUNCTION gmac_util_clone_container_repetition_struct

DESCRIPTION
  Duplicates a container_repetition_struct, including its data

PARAMS
  *dst_ptr - pointer to the destination container repetition struct
  *src_ptr - pointer to the source container repetition struct

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
static void gmac_util_clone_container_repetition_struct(container_repetition_struct_t *dst_ptr,
                                                        container_repetition_struct_t *src_ptr,
                                                        gas_id_t                       gas_id)
{
  uint8 num_structs;

  dst_ptr->octet_counts[BCCH_PD]   = src_ptr->octet_counts[BCCH_PD];
  dst_ptr->octet_counts[PBCCH_PD]  = src_ptr->octet_counts[PBCCH_PD];
  dst_ptr->struct_counts[BCCH_PD]  = src_ptr->struct_counts[BCCH_PD];
  dst_ptr->struct_counts[PBCCH_PD] = src_ptr->struct_counts[PBCCH_PD];

  num_structs = src_ptr->num_data_structs;

  memscpy(dst_ptr->data_structs,sizeof(dst_ptr->data_structs),
          src_ptr->data_structs, num_structs * sizeof(container_data_t));

  dst_ptr->num_data_structs = num_structs;
  dst_ptr->last_struct = src_ptr->last_struct;

  return;
}

/*===========================================================================

FUNCTION gmac_nacc_extract_pncd_instance_data

DESCRIPTION
  Allocates memory and duplicates the contents of a PNCD message instance

PARAMS
  *pncd_ptr - pointer to the original message

RETURN VALUE
  Ptr to new copy of the container repetition structure

SIDE EFFECTS
  None

===========================================================================*/
static container_repetition_struct_t * gmac_nacc_extract_pncd_instance_data(packet_ncell_data_t *pncd_ptr,
                                                                            gas_id_t             gas_id)
{
  container_repetition_struct_t *new_ptr;

  new_ptr = (container_repetition_struct_t *)GPRS_MEM_MALLOC(sizeof(container_repetition_struct_t));
  if (new_ptr)
  {
    /* copy the container repetition structure */
    gmac_util_clone_container_repetition_struct(new_ptr, &(pncd_ptr->container_repetition_struct),
                                                gas_id);
  }

  return(new_ptr);
}

/*===========================================================================

FUNCTION gmac_util_extract_pscd_instance_data

DESCRIPTION
  Allocates memory and duplicates the contents of a PSCD message instance

PARAMS
  *pscd_ptr - pointer to the original message

RETURN VALUE
  Ptr to new copy of the container repetition structure

SIDE EFFECTS
  None

===========================================================================*/
static container_repetition_struct_t * gmac_util_extract_pscd_instance_data(packet_scell_data_t *pscd_ptr,
                                                                            gas_id_t             gas_id)
{
  container_repetition_struct_t *new_ptr;

  new_ptr = (container_repetition_struct_t *)GPRS_MEM_MALLOC(sizeof(container_repetition_struct_t));
  if (new_ptr)
  {
    /* copy the container repetition structure */
    gmac_util_clone_container_repetition_struct(new_ptr, &(pscd_ptr->container_repetition_struct),
                                                gas_id);
  }

  return(new_ptr);
}

/*===========================================================================

FUNCTION gmac_util_extract_si_index

DESCRIPTION
  Extracts the instance index from a SI message

PARAMS
  msg_type - the message type
  msg_ptr  - pointer to L3 message data

RETURN VALUE
  Extracted index, or NO_INSTANCE_REQUIRED if not applicable

SIDE EFFECTS
  None

===========================================================================*/
static byte gmac_util_extract_si_index(byte msg_type, byte *l3_msg_ptr, uint8 num_octets,
                                       gas_id_t gas_id)
{
  byte index;

  if ((msg_type == 0x03) && (num_octets > 17))     /* SYSTEM_INFORMATION_TYPE_2_TER */
  {
    index = (*(l3_msg_ptr+17)) >> 3;                /* first byte of SI2ter Rest Octets */
    index &= 0x07;                                  /* extract 3 bits */
  }
  else if ((msg_type == 0x07) && (num_octets > 1)) /* SYSTEM_INFORMATION_TYPE_2_QUATER */
  {
    index = (*(l3_msg_ptr+1)) >> 1;                 /* first byte of SI2qtr Rest Octets */
    index &= 0x0F;                                  /* extract 4 bits */
  }
  else
  {
    index = NO_INSTANCE_REQUIRED;
  }

  return(index);
}

/*===========================================================================

FUNCTION gmac_util_extract_psi_index

DESCRIPTION
  Extracts the instance index from a PSI message

PARAMS
  msg_type   - the message type being sought
  data_octet - the relevant octet where the instance index is located

RETURN VALUE
  Extracted index, or NO_INSTANCE_REQUIRED if not applicable

SIDE EFFECTS
  None

===========================================================================*/
static byte gmac_util_extract_psi_index(byte msg_type, byte data_octet,
                                        gas_id_t gas_id)
{
  byte index;

  if ((msg_type == PACKET_PSI2) ||
      (msg_type == PACKET_PSI4) ||
      (msg_type == PACKET_PSI5))
      /* also PSI6, 7 and 8 */
  {
    index = PSI_INSTANCE_FROM3BITS(data_octet);
  }
  else if ((msg_type == PACKET_PSI3_BIS) ||
           (msg_type == PACKET_PSI3_QUATER))
           /* also PSI3ter */
  {
    index = PSI_INSTANCE_FROM4BITS(data_octet);
  }
  else
  {
    index = NO_INSTANCE_REQUIRED;
  }

  return(index);
}

/*===========================================================================

FUNCTION gmac_util_extract_msg_from_container

DESCRIPTION
  Attempts to extract a coherent SI/PSI message from a container

PARAMS
  cell_data_ptr - pointer to the cell data that should be searched
  pd            - the protocol discriminator (BCCH_PD or PBCCH_PD)
  segment       - relevant segment number
 *output_ptr    - ptr to buffer for message contents (if available)

RETURN VALUE
  Number of octets available for the specified message (0 indicates "not found")

SIDE EFFECTS
  None

===========================================================================*/
static uint16 gmac_util_extract_msg_from_container(cell_data_t *cell_data_ptr,
                                                   uint8 pd,
                                                   uint16 segment,
                                                   byte *output_ptr,
                                                   int8 output_buffer_size,
                                                   gas_id_t gas_id)

{
  container_t *container_ptr;
  byte        *start_ptr;
  uint16       num_octets  = 0;
  uint16       start_point;
  uint8        offset;
  byte         octet_limit;

  /* set up variables depending on protocol discriminator */
  if (pd == BCCH_PD)
  {
    container_ptr = &(cell_data_ptr->si_container);
    octet_limit = N201_MAX - 2; /* SI messages have a two byte header (length + PD/skip) */
  }
  else
  {
    container_ptr = &(cell_data_ptr->psi_container);
    octet_limit = MAX_SIZE_OF_CONTROL_BLOCK_DATA; /* PSI messages are complete RLC ctrl blk */
  }

  /* range check the specified segment */
  if (segment >= container_ptr->num_segments)
  {
    return(0);
  }

  /* determine segment start point */
  if (segment == 0)
  {
    start_point = 0;
  }
  else
  {
    start_point = container_ptr->segment_end_points[segment-1] + 1;
  }

  num_octets = (container_ptr->segment_end_points[segment] - start_point) + 1;

  /* caller might pass a NULL output ptr just to see if we have data */
  /* and in that case, the actual number of octets reported is not critical */
  if (output_ptr)
  {
    if (num_octets > octet_limit)
    {
      /* avoid copying more than maximum number of octets */
      num_octets = octet_limit;
    }
    start_ptr  = container_ptr->data_ptr + start_point;

    if (pd == BCCH_PD)
    {
      /* insert the L2 pseudo length in the first octet */
      uint8 len_octet = (uint8)(num_octets << 2);

      *(output_ptr+0) = len_octet | 0x01;

      /* second octet is the RR protocol discriminator and skip indicator */
      *(output_ptr+1) = 0x06;

      offset = 2;
    }
    else
    {
      offset = 0;
    }

    memset(output_ptr+offset, 0x2B, octet_limit);
    memscpy(output_ptr+offset,(output_buffer_size - offset ) , start_ptr, num_octets);
    num_octets += offset;
  }

  return(num_octets);
}

/*===========================================================================

FUNCTION gmac_util_count_consecutive_instances

DESCRIPTION
  Counts the number of consecutive 1s in an instance bitmap (starting from LSB)

PARAMS
  bitmap - 32-bit bitmap

RETURN VALUE
  Number of consecutive bits

SIDE EFFECTS
  None

===========================================================================*/
static uint8 gmac_util_count_consecutive_instances(uint32 bitmap,
                                                   gas_id_t gas_id)
{
  uint32 mask  = 0x00000001;
  uint8  count = 0;

  while (mask)
  {
    if (mask & bitmap)
    {
      count++;
    }
    else
    {
      /* stop as soon as a zero is encountered */
      break;
    }
    mask = mask << 1;
  }

  return(count);
}

/*===========================================================================

FUNCTION gmac_util_extract_msg_types

DESCRIPTION
  Extracts a list of msg types (and instances) for the SI and PSI containers

PARAMS
  *cell_data_ptr - pointer to cell data that should be processed

RETURN VALUE
  VOID

SIDE EFFECTS
  Allocates chunks of memory to hold SI and PSI lists of current msg types

===========================================================================*/
static void gmac_util_extract_msg_types(cell_data_t *cell_data_ptr,
                                        gas_id_t gas_id)
{
  msg_type_t *msg_type_ptr;
  uint16      start_point;
  uint16      segment;
  uint16      num_msgs;
  uint8       msg_type;
  uint8       msg_inst;
  uint8       num_octets;

  /* Extract the SI msg types */
  num_msgs = cell_data_ptr->si_container.num_segments;
  if (num_msgs)
  {
    /* save the previous msg list and segment counts */
    if (cell_data_ptr->si_container.previous_msgs)
    {
      GPRS_MEM_FREE(cell_data_ptr->si_container.previous_msgs);
    }
    cell_data_ptr->si_container.previous_msgs = cell_data_ptr->si_container.current_msgs;
    cell_data_ptr->si_container.num_previous  = cell_data_ptr->si_container.num_current;

    msg_type_ptr = (msg_type_t *)GPRS_MEM_MALLOC(num_msgs * sizeof(msg_type_t));
  }
  else
  {
    msg_type_ptr = NULL;
  }

  if (msg_type_ptr)
  {
    /* record start of newly formed list of types */
    cell_data_ptr->si_container.current_msgs = msg_type_ptr;
    cell_data_ptr->si_container.num_current  = num_msgs;
    start_point = 0;

    for (segment = 0; segment < num_msgs; segment++, msg_type_ptr++)
    {
      num_octets = (uint8)(cell_data_ptr->si_container.segment_end_points[segment] - start_point) + 1;

      start_point += SI_MSG_TYPE_OCTET;

      msg_type = cell_data_ptr->si_container.data_ptr[start_point];
      msg_type = msg_type & SI_MSG_TYPE_MASK;
      msg_type = msg_type >> SI_MSG_TYPE_SHIFT;

      msg_type_ptr->msg_type = msg_type;
      msg_type_ptr->msg_inst = gmac_util_extract_si_index(msg_type, cell_data_ptr->si_container.data_ptr+start_point, num_octets,
                                                          gas_id);
      msg_type_ptr->segment  = segment;

      start_point = cell_data_ptr->si_container.segment_end_points[segment] + 1;
    }
  }

  /* Extract the PSI msg types */
  num_msgs = cell_data_ptr->psi_container.num_segments;
  if (num_msgs)
  {
    if (cell_data_ptr->psi_container.previous_msgs)
    {
      GPRS_MEM_FREE(cell_data_ptr->psi_container.previous_msgs);
    }
    cell_data_ptr->psi_container.previous_msgs = cell_data_ptr->psi_container.current_msgs;
    cell_data_ptr->psi_container.num_previous  = cell_data_ptr->psi_container.num_current;

    msg_type_ptr = (msg_type_t *)GPRS_MEM_MALLOC(num_msgs * sizeof(msg_type_t));
  }
  else
  {
    msg_type_ptr = NULL;
  }

  if (msg_type_ptr)
  {
    /* record start of newly formed list of types */
    cell_data_ptr->psi_container.current_msgs = msg_type_ptr;
    cell_data_ptr->psi_container.num_current  = num_msgs;

    start_point = 0;

    for (segment = 0; segment < num_msgs; segment++, msg_type_ptr++)
    {
      start_point += PSI_MSG_TYPE_OCTET;

      msg_type = cell_data_ptr->psi_container.data_ptr[start_point];
      msg_type = msg_type & PSI_MSG_TYPE_MASK;
      msg_type = msg_type >> PSI_MSG_TYPE_SHIFT;

      msg_inst = cell_data_ptr->psi_container.data_ptr[start_point+PSI_INSTANCE_OFFSET];
      msg_inst = gmac_util_extract_psi_index(msg_type, msg_inst, gas_id);

      msg_type_ptr->msg_type = msg_type;
      msg_type_ptr->msg_inst = msg_inst;
      msg_type_ptr->segment  = segment;

      start_point = cell_data_ptr->psi_container.segment_end_points[segment] + 1;
    }
  }

  return;
}

/*===========================================================================

FUNCTION gmac_util_build_container

DESCRIPTION
  Flattens the octets from every instance and repetition struct into one stream

PARAMS
  *cell_data_ptr - pointer to cell data that should be built
  complete_ptr   - pointer to boolean output variable that indicates container is complete

RETURN VALUE
  Number of final message segments observed in the bytestream

SIDE EFFECTS
  Allocates a chunk of memory to hold the container's bytestream

===========================================================================*/
static uint8 gmac_util_build_container(cell_data_t *cell_data_ptr,
                                       boolean *complete_ptr,
                                       gas_id_t gas_id)
{
  container_repetition_struct_t *instance_ptr;
  boolean                        bFinal;
  byte                          *si_output_ptr          = NULL;
  byte                          *psi_output_ptr         = NULL;
  uint16                        *si_segment_end_points  = NULL;
  uint16                        *psi_segment_end_points = NULL;
  uint16                         num_si_octets          = 0;
  uint16                         num_psi_octets         = 0;
  uint8                          num_si_segments        = 0;
  uint8                          num_psi_segments       = 0;
  uint8                          msgs_built             = 0;
  uint8                          num_octets;
  uint8                          num_instances;
  uint8                          i;
  uint8                          j;
  uint8                          si_output_malloc_size = 0;
  uint8                          psi_output_malloc_size = 0;

  /* determine how many consecutive instances we have received so far */
  num_instances = gmac_util_count_consecutive_instances(cell_data_ptr->instances_rcvd,
                                                        gas_id);


  if (num_instances > MAX_NUMBER_CELL_DATA_INSTANCES)
  {
   /*the number of instances is determiend via a 32 bit map and should not exceed 32 */
   MSG_GERAN_ERROR_1_G("GMAC Info: Number of cell data instances of 32 exceeded .  num_instances = %d",num_instances);
   return (0);
  }

  /* determine the maximum number of octets and structures we have for each output type */
  /* the CSN decoder has prepared some of this data during the decoding process */
  for (i = 0; i < num_instances; i++)
  {
    instance_ptr = cell_data_ptr->instance_ptrs[i];

    num_si_octets  += instance_ptr->octet_counts[BCCH_PD];
    num_psi_octets += instance_ptr->octet_counts[PBCCH_PD];

    num_si_segments  += instance_ptr->struct_counts[BCCH_PD];
    num_psi_segments += instance_ptr->struct_counts[PBCCH_PD];

    if (instance_ptr->last_struct)
    {
      /* the last repetition structure is being included in this build */
      /* which means the container must now be complete: set the flag  */
      *complete_ptr = TRUE;
    }
  }

  /* allocate stream memory for each output type */
  if (num_si_octets)
  {
    si_output_ptr = (byte *)GPRS_MEM_MALLOC(num_si_octets * sizeof(byte));
    si_output_malloc_size = num_si_octets;
  }

  if (num_psi_octets)
  {
    psi_output_ptr = (byte *)GPRS_MEM_MALLOC(num_psi_octets * sizeof(byte));
    psi_output_malloc_size = num_psi_octets;
  }

  /* allocate segment end point memory for each output type */
  if (num_si_segments)
  {
    si_segment_end_points  = (uint16 *)GPRS_MEM_MALLOC(num_si_segments * sizeof(uint16));
  }

  if (num_psi_segments)
  {
    psi_segment_end_points = (uint16 *)GPRS_MEM_MALLOC(num_psi_segments * sizeof(uint16));
  }

  /* keep track of the number of octets copied over */
  num_si_octets  = 0;
  num_psi_octets = 0;

  /* and the number of segments */
  num_si_segments  = 0;
  num_psi_segments = 0;

  for (i = 0; i < num_instances; i++)
  {
    instance_ptr = cell_data_ptr->instance_ptrs[i];

    for (j = 0; j < instance_ptr->num_data_structs; j++)
    {
      if (instance_ptr->data_structs[j].cd_length != CONTAINER_DATA_FILLS_MSG)
      {
        /* this container repetition structure has a finite length */
        /* and therefore represents a final segment                */
        msgs_built++;
        bFinal = TRUE;
      }
      else
      {
        bFinal = FALSE;
      }

      /* determine the exact number of octets in this repetition structure */
      num_octets = instance_ptr->data_structs[j].ex_length;

      if (instance_ptr->data_structs[j].pd == BCCH_PD)
      {
        /* SI SEGMENT */
        if (si_output_ptr)
        {
          if ( (num_octets > sizeof (instance_ptr->data_structs[j].data)) ||
               ((num_si_octets + num_octets) > si_output_malloc_size) )
          {
            MSG_GERAN_ERROR_3_G("memcpy out of bound num_octets:%d, num_si_octets:%d si_output_malloc_size:%d",
                                  num_octets, num_si_octets, si_output_malloc_size);
            break;
          }

          /* concatenate to SI output stream */
          memscpy(si_output_ptr + num_si_octets,
                  si_output_malloc_size - num_si_octets,
                  instance_ptr->data_structs[j].data,
                  num_octets);
        }
        num_si_octets += num_octets;

        if (bFinal)
        {
          if (si_segment_end_points)
          {
            /* store the last valid position as the segment end point */
            si_segment_end_points[num_si_segments] = num_si_octets - 1;
          }
          num_si_segments++;
        }
      }
      else
      {
        /* PSI SEGMENT */
        if (psi_output_ptr)
        {
          if ( (num_octets > sizeof (instance_ptr->data_structs[j].data)) ||
               ((num_psi_octets + num_octets) > psi_output_malloc_size) )
          {
            MSG_GERAN_ERROR_3_G("memcpy out of bound num_octets:%d, num_psi_octets:%d si_output_malloc_size:%d",
                                 num_octets, num_psi_octets, psi_output_malloc_size);
            break;
          }

          /* concatenate to PSI output stream */
          memscpy(psi_output_ptr + num_psi_octets,
                  psi_output_malloc_size - num_psi_octets,
                  instance_ptr->data_structs[j].data,
                  num_octets);
        }
        num_psi_octets += num_octets;

        if (bFinal)
        {
          if (psi_segment_end_points)
          {
            /* store the last valid position as the segment end point */
            psi_segment_end_points[num_psi_segments] = num_psi_octets - 1;
          }
          num_psi_segments++;
        }
      }
    }
  }

  /* now we have two streams: one for SIs and one for PSIs */
  /* created from all the repetition structures available  */

  /* store the SI stream details, freeing any previous allocations */
  if (cell_data_ptr->si_container.data_ptr)
  {
    GPRS_MEM_FREE(cell_data_ptr->si_container.data_ptr);
  }
  cell_data_ptr->si_container.data_ptr   = si_output_ptr;
  cell_data_ptr->si_container.num_octets = num_si_octets;

  if (cell_data_ptr->si_container.segment_end_points)
  {
    GPRS_MEM_FREE(cell_data_ptr->si_container.segment_end_points);
  }
  cell_data_ptr->si_container.segment_end_points = si_segment_end_points;
  cell_data_ptr->si_container.num_segments       = num_si_segments;

  /* store the PSI stream details, freeing any previous allocations */
  if (cell_data_ptr->psi_container.data_ptr)
  {
    GPRS_MEM_FREE(cell_data_ptr->psi_container.data_ptr);
  }
  cell_data_ptr->psi_container.data_ptr   = psi_output_ptr;
  cell_data_ptr->psi_container.num_octets = num_psi_octets;

  if (cell_data_ptr->psi_container.segment_end_points)
  {
    GPRS_MEM_FREE(cell_data_ptr->psi_container.segment_end_points);
  }
  cell_data_ptr->psi_container.segment_end_points = psi_segment_end_points;
  cell_data_ptr->psi_container.num_segments       = num_psi_segments;

  /* return a count of the number of complete messages now available */
  return(msgs_built);
}

/*===========================================================================

FUNCTION gmac_util_determine_new_msgs

DESCRIPTION
  Compares a container's current and previous msgs lists to determine changes

PARAMS
  *container_ptr - pointer to container to be processed
  *count_ptr     - output variable (holds count of new msgs found)

RETURN VALUE
  Pointer to a list of the new msg types that were found

SIDE EFFECTS
  Allocates a chunk of memory to hold the list of new msgs. Caller must free.

===========================================================================*/
static msg_type_t *gmac_util_determine_new_msgs(container_t *container_ptr, uint8 *count_ptr,
                                                gas_id_t gas_id)
{
  msg_type_t *new_list_ptr;
  msg_type_t *current_ptr;
  msg_type_t *previous_ptr;
  boolean     bFound;
  uint16      current_msg;
  uint16      previous_msg;
  uint8       count = (uint8)container_ptr->num_segments;

  if (count)
  {
    new_list_ptr = (msg_type_t *)GPRS_MEM_MALLOC(count * sizeof(msg_type_t));
  }
  else
  {
    new_list_ptr = NULL;
  }

  count = 0;
  if (new_list_ptr)
  {
    current_ptr = container_ptr->current_msgs;

    for (current_msg = 0; current_msg < container_ptr->num_segments; current_msg++, current_ptr++)
    {
      bFound = FALSE;
      previous_ptr = container_ptr->previous_msgs;

      if (previous_ptr)
      {
        /* try and find the current msg type/inst in the list of previous types */
        for (previous_msg = 0; previous_msg < container_ptr->num_previous; previous_msg++, previous_ptr++)
        {
          if ((current_ptr->msg_type == previous_ptr->msg_type) &&
              (current_ptr->msg_inst == previous_ptr->msg_inst))
          {
            bFound = TRUE;
            break; /* early exit from inner loop */
          }
        }
      }

      if (bFound == FALSE)
      {
        /* copy contents of current_ptr to the output list */
        new_list_ptr[count].msg_type = current_ptr->msg_type;
        new_list_ptr[count].msg_inst = current_ptr->msg_inst;
        new_list_ptr[count].segment  = current_ptr->segment;
        count++;
      }
    }
  }

  if ((count == 0) && new_list_ptr)
  {
    /* memory was allocated, but no new messages were found */
    GPRS_MEM_FREE(new_list_ptr);
    new_list_ptr = NULL;
  }

  *count_ptr = count;
  return(new_list_ptr);
}

/*===========================================================================

FUNCTION gmac_nacc_ncell_data_ind_to_grr

DESCRIPTION
  Sends a MAC_GRR_NCELL_DATA_IND containing data from the supplied CSN decode structure

PARAMS
  *csn_decoder_ptr - pointer to a CSN decoder output structure
  container_id     - the container that generated the data
  protocol         - protocol type for data (BCCH_PD or PBCCH_PD)

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
static void gmac_nacc_ncell_data_ind_to_grr(csn_decoder_ptrs_t *csn_decoder_ptr, uint8 container_id, uint8 protocol,
                                            gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("NACC sent MAC_GRR_NCELL_DATA_IND");
  if (csn_decoder_ptr)
  {
    global_data_ptr->grr_sig.mac_grr_ncell_data_ind.data_ptr = *csn_decoder_ptr;
  }
  else
  {
    memset(&(global_data_ptr->grr_sig.mac_grr_ncell_data_ind.data_ptr),
           0,
           sizeof(csn_decoder_ptrs_t));
  }
  global_data_ptr->grr_sig.mac_grr_ncell_data_ind.container_id = container_id;
  global_data_ptr->grr_sig.mac_grr_ncell_data_ind.protocol     = protocol;

  global_data_ptr->grr_sig.mac_grr_ncell_data_ind.message_header.message_id  = (uint8)MAC_GRR_NCELL_DATA_IND;
  global_data_ptr->grr_sig.mac_grr_ncell_data_ind.message_header.message_set = MS_MAC_RR;

  gprs_grr_mac_acc_func(&global_data_ptr->grr_sig, gas_id);

  return;
}


/*===========================================================================

FUNCTION gmac_util_forward_packet_si_data

DESCRIPTION
  Creates a PACKET_SI_DATA message and forwards it to GRR (as a MAC_GRR_DATA_IND)

PARAMS
  container_id  - ID of container to dispatch messages from
  cell_data_ptr - pointer to cell data structure to be processed
  msg_type_ptr  - pointer to current msg

RETURN VALUE
  VOID

SIDE EFFECTS
  Generates inter-task messages

===========================================================================*/
static void gmac_util_forward_packet_si_data(uint8 container_id, cell_data_t *cell_data_ptr, msg_type_t *msg_type_ptr,
                                             gas_id_t gas_id)
{
  uint16 num_octets;
  uint8  si_buffer[sizeof(packet_si_data_t)+2];

  /* extract the current message segment into a local buffer */
  uint8 si_buffer_size = sizeof(si_buffer);

  num_octets = gmac_util_extract_msg_from_container(cell_data_ptr, BCCH_PD,
                                                    msg_type_ptr->segment,
                                                    si_buffer+2,
                                                    si_buffer_size,
                                                    gas_id);


  if (num_octets)
  {
    csn_decoder_ptrs_t csn_data;
    uint16             dummy;

    /* load the first two octets of the buffer with info for the CSN decoder */
    dummy = ((uint8)PACKET_SI_DATA) << 2;
    si_buffer[0] = (uint8)dummy;
    si_buffer[1] = (uint8)num_octets;

    /* call the decoder and get a result structure */

    /*si_buffer has an additional 2 octets + container data . so size of buffer to be decoded is num_octets + 2*/
    if(num_octets + 2 > sizeof(si_buffer))
    {
      csn_data = gcsn1_decoder(si_buffer, &dummy, sizeof(si_buffer), gas_id);
    }
    else
    {
      csn_data = gcsn1_decoder(si_buffer, &dummy, (num_octets + 2), gas_id);
    }

    if (container_id == SERVING_CELL_CONTAINER)
    {
      gmac_csn_data_ind_to_grr(&csn_data, cell_data_ptr->arfcn,gas_id);
    }
    else
    {
      gmac_nacc_ncell_data_ind_to_grr(&csn_data, container_id, BCCH_PD,gas_id);
    }
  }

  return;
}

/*===========================================================================

FUNCTION gmac_util_check_for_new_messages_and_dispatch

DESCRIPTION
  Finds new messages in the container and dispatches them to GRR

PARAMS
  container_id  - ID of container to dispatch messages from
  cell_data_ptr - pointer to cell data structure to be processed

RETURN VALUE
  VOID

SIDE EFFECTS
  Generates inter-task messages

===========================================================================*/
static void gmac_util_check_for_new_messages_and_dispatch(uint8 container_id, cell_data_t *cell_data_ptr,
                                                          gas_id_t gas_id)
{
  msg_type_t *new_msg_list;
  msg_type_t *msg_type_ptr;
  uint8       count;
  uint8       index;

  /* Check for new SI messages and dispatch them to GRR
   * The messages are passed over as encoded hex strings:
   * they can then be decoded using RR's own CSN1 decoder
   */
  new_msg_list = gmac_util_determine_new_msgs(&(cell_data_ptr->si_container), &count,gas_id);
  if (new_msg_list)
  {
    /* dispatch new SI messages as DATA_IND with PACKET_SI_DATA content */
    msg_type_ptr = new_msg_list;
    for (index = 0; index < count; index++, msg_type_ptr++)
    {
      gmac_util_forward_packet_si_data(container_id,
                                       cell_data_ptr,
                                       msg_type_ptr,
                                       gas_id);
    }
    GPRS_MEM_FREE(new_msg_list);
  } /* end of new SI processing */

  /* Check for new PSI messages and dispatch them to GRR
   * The messages are passed to the CSN1 decoder first
   * and then the resulting structures are dispatched
   * in the usual way using MAC_GRR_DATA_IND signals
   */
  new_msg_list = gmac_util_determine_new_msgs(&(cell_data_ptr->psi_container), &count, gas_id);
  if (new_msg_list)
  {
    uint16 num_bits = 0;
    uint16 num_octets;
    uint8  psi_octets[MAX_SIZE_OF_CONTROL_BLOCK_DATA];

    uint8 psi_octets_buffer_size = sizeof(psi_octets);

    /* dispatch new PSI messages */
    msg_type_ptr = new_msg_list;
    for (index = 0; index < count; index++, msg_type_ptr++)
    {
      num_octets = gmac_util_extract_msg_from_container(cell_data_ptr, PBCCH_PD,
                                                        msg_type_ptr->segment,
                                                        psi_octets,
                                                        psi_octets_buffer_size,
                                                        gas_id);


      if (num_octets)
      {
        csn_decoder_ptrs_t csn_data;

        /* call the CSN1 decoder */

        if(num_octets > sizeof(psi_octets))
        {
          csn_data = gcsn1_decoder(psi_octets, &num_bits, sizeof(psi_octets), gas_id);
        }
        else
        {
          csn_data = gcsn1_decoder(psi_octets, &num_bits,num_octets, gas_id);
        }

        if (container_id == SERVING_CELL_CONTAINER)
        {
          gmac_csn_data_ind_to_grr(&csn_data, cell_data_ptr->arfcn,
                                    gas_id);
        }
        else
        {
          gmac_nacc_ncell_data_ind_to_grr(&csn_data, container_id, PBCCH_PD, gas_id);
        }
      }
    }
    GPRS_MEM_FREE(new_msg_list);
  } /* end of new PSI processing */

  return;
}


/*===========================================================================

FUNCTION gmac_util_trigger_cell_data

DESCRIPTION
  Dispatch SIs and PSIs from the specified container to GRR

PARAMS
  container_id  - container ID to extract data from
  bReleaseAfter - TRUE if container should be released immediately afterwards

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
static void gmac_util_trigger_cell_data(uint8 container_id, boolean bReleaseAfter,
                                        gas_id_t gas_id)
{
  cell_data_t *cell_data_ptr = NULL;
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  if (container_id == SERVING_CELL_CONTAINER)
  {
    cell_data_ptr = &util_data_ptr->scell_info;
  }
  else if (container_id < SERVING_CELL_CONTAINER)
  {
    cell_data_ptr = &(util_data_ptr->ncell_info[container_id].cell_data);
  }

  if (cell_data_ptr)
  {
    /* build list of the message types available in the container */
    gmac_util_extract_msg_types(cell_data_ptr,gas_id);

    /* check for new messages since last call to the trigger, and dispatch them to GRR */
    gmac_util_check_for_new_messages_and_dispatch(container_id, cell_data_ptr,gas_id);
  }

  if (bReleaseAfter)
  {
    gmac_util_release_container(container_id,gas_id);
  }

  return;
}

/*===========================================================================

FUNCTION gmac_validate_container_sequence

DESCRIPTION
  Validates PSCD or PNCD instances to ensure they are received in sequence

PARAMS
  *cell_data_ptr - pointer to the container
  container_id   - numerical ID of the container
  rcvd_index     - the instance index just received for this container

RETURN VALUE
  VOID

SIDE EFFECTS
  May release all dynamic memory and reset the container

===========================================================================*/
static void gmac_validate_container_sequence(cell_data_t *cell_data_ptr, uint8 container_id, uint8 rcvd_index,
                                             gas_id_t gas_id)
{
  /* verify input parameters */
  if ((container_id >  MAX_NUMBER_OF_NACC_NCELLS)      || /* note this also allows the serving container ID */
      (rcvd_index   >= MAX_NUMBER_CELL_DATA_INSTANCES) ||
      (cell_data_ptr == NULL))
  {
    MSG_GERAN_ERROR_0_G("Invalid parameters for container sequence validation");
    return;
  }

  if (cell_data_ptr->container_valid == TRUE)
  {
    /* check that sequencing of the received container indices is correct */
    if (rcvd_index == 0)
    {
      /* invalidate the container, it is about to restart */
      cell_data_ptr->container_valid = FALSE;
    }
    else
    {
      uint8 next_index;

      for (next_index = rcvd_index+1; next_index < MAX_NUMBER_CELL_DATA_INSTANCES; next_index++)
      {
        if (cell_data_ptr->instance_ptrs[next_index] != NULL)
        {
          /* a higher-numbered instance has been received already */
          /* which implies the instances arrived out of sequence  */
          cell_data_ptr->container_valid = FALSE;
          break;
        }
      }
    }

    /* if the container has now been flagged as invalid, release it */
    if (cell_data_ptr->container_valid == FALSE)
    {
      if (container_id == SERVING_CELL_CONTAINER)
      {
        MSG_GERAN_ERROR_1_G("PCSD container instance %d has triggered a restart", (int)rcvd_index);
      }
      else
      {
        MSG_GERAN_ERROR_2_G("PNCD container ID %d instance %d has triggered a restart", (int)container_id, (int)rcvd_index);
      }
      gmac_util_release_container(container_id,gas_id);
    }
  }

  return;
}

/*===========================================================================

FUNCTION gmac_nacc_validate_arfcn_and_bsic

DESCRIPTION
  Validates the ARFCN and BSIC of received PNCD instance, to ensure they match
  with any parameters previously provided for the same container

PARAMS
  *ncell_info_ptr - pointer to a neighbour cell container structure
  *pncd_ptr       - pointer to the decoded message structure

RETURN VALUE
  VOID

SIDE EFFECTS
  May release all dynamic memory and reset the container

===========================================================================*/
static void gmac_nacc_validate_arfcn_and_bsic(ncell_data_t *ncell_info_ptr,
                                              packet_ncell_data_t *pncd_ptr,
                                              gas_id_t gas_id)
{
  if ((ncell_info_ptr == NULL) || (pncd_ptr == NULL))
  {
    MSG_GERAN_ERROR_0_G("NACC: invalid pointers for ARFCN and BSIC validation");
    return;
  }

  /* proceed with validation if the container already has some valid content */
  if ((ncell_info_ptr->cell_data.container_valid == TRUE) &&
      (pncd_ptr->arfcn_bsic_included             == TRUE))
  {
    /* check whether the supplied ARFCN or BSIC has changed, since */
    /* the spec states that inconsistent data resets the container */
    if (ncell_info_ptr->ncell_arfcn.num != 0xFFFF)
    {
      /* check whether the channel number has changed */
      if (ncell_info_ptr->ncell_arfcn.num != pncd_ptr->arfcn.num)
      {
        ncell_info_ptr->cell_data.container_valid = FALSE;
      }
      /* check whether the band has changed */
      else if (ncell_info_ptr->ncell_arfcn.band != pncd_ptr->arfcn.band)
      {
        ncell_info_ptr->cell_data.container_valid = FALSE;
      }

      /* check whether the BSIC has changed */
      if (ncell_info_ptr->ncell_bsic != pncd_ptr->bsic)
      {
        ncell_info_ptr->cell_data.container_valid = FALSE;
      }
    }

    /* if the container has now been flagged as invalid, release it */
    if (ncell_info_ptr->cell_data.container_valid == FALSE)
    {
      MSG_GERAN_ERROR_1_G("PNCD container ID %d reset due to ARFCN or BSIC mismatch", (int)(pncd_ptr->container_id));
      gmac_util_release_container(pncd_ptr->container_id,gas_id);
    }
  }

  return;
}

/*===========================================================================

FUNCTION gmac_nacc_handle_pkt_ncell_data_instance

DESCRIPTION
  Processes an incoming PNCD instance (received from MAC during a TBF)

PARAMS
  *pncd_ptr   - pointer to the decoded message structure
  arfcn       - ARFCN where PNCD message was received
  id_ptr      - output pointer to store received container ID
  release_ptr - pointer to boolean output variable that indicates whether to release

RETURN VALUE
  Number of coherent SI or PSI messages that can be built with the instances rcvd

SIDE EFFECTS
  None

===========================================================================*/
static uint8 gmac_nacc_handle_pkt_ncell_data_instance(packet_ncell_data_t *pncd_ptr,
                                                      ARFCN_T arfcn,
                                                      uint8 *id_ptr,
                                                      boolean *release_ptr,
                                                      gas_id_t gas_id)
{
  container_repetition_struct_t *instance_ptr;
  ncell_data_t                  *ncell_info_ptr;
  boolean                        was_empty = FALSE;
  uint8                          msgs_built = 0;
  uint8                          container_id;
  uint8                          container_index;
  gmac_util_data_t              *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  /* determine the container ID and index for this instance */
  container_id    = pncd_ptr->container_id;
  container_index = pncd_ptr->container_index;

  if ((container_id    >= MAX_NUMBER_OF_NACC_NCELLS) ||
      (container_index >= MAX_NUMBER_CELL_DATA_INSTANCES))
  {
    MSG_GERAN_ERROR_2_G("Invalid container ID or index %d %d", container_id, container_index);
    return(0);
  }

  ncell_info_ptr = &(util_data_ptr->ncell_info[container_id]);

  /* See 3GPP spec CR GP-050422: rules for PNCD container validity */
  gmac_validate_container_sequence(&(ncell_info_ptr->cell_data), container_id, container_index, gas_id);
  gmac_nacc_validate_arfcn_and_bsic(ncell_info_ptr, pncd_ptr, gas_id);

  instance_ptr = ncell_info_ptr->cell_data.instance_ptrs[container_index];

  if (instance_ptr)
  {
    /* already got this instance, so free it up and replace */
    GPRS_MEM_FREE(instance_ptr);
  }

  /* extract the container repetition structure from the incoming message */
  instance_ptr = gmac_nacc_extract_pncd_instance_data(pncd_ptr, gas_id);

  if (instance_ptr)
  {
    /* flag the container as valid */
    ncell_info_ptr->cell_data.container_valid = TRUE;

    /* store the ARFCN of the cell this message was RECEIVED on */
    /* as this may be important for determining band support later */
    ncell_info_ptr->cell_data.arfcn = arfcn;

    /* determine whether this was the first message received for this container */
    if (ncell_info_ptr->cell_data.instances_rcvd == 0x00000000)
    {
      was_empty = TRUE;
    }

    /* store the new instance and set relevant flag in bitmap */
    ncell_info_ptr->cell_data.instance_ptrs[container_index] = instance_ptr;
    ncell_info_ptr->cell_data.instances_rcvd |= (0x00000001 << container_index);

    /* check whether the new instance provided a ncell ARFCN and BSIC */
    if (pncd_ptr->arfcn_bsic_included)
    {
      ncell_info_ptr->ncell_arfcn = pncd_ptr->arfcn;
      ncell_info_ptr->ncell_bsic  = pncd_ptr->bsic;
    }
    else
    {
      MSG_GERAN_HIGH_3_G("Rcvd Pkt NCell Data instance %d for container %d (SCELL=%d)", container_index, container_id, arfcn.num);
    }

    if (ncell_info_ptr->cell_data.instances_rcvd & 0x00000001)
    {
      /* once the first instance is available,   */
      /* try and build the container data stream */
      msgs_built = gmac_util_build_container(&(ncell_info_ptr->cell_data), release_ptr, gas_id);
    }

    /* copy container_id to the output */
    if (id_ptr)
    {
      *id_ptr = container_id;
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Unable to allocate memory for PNCD: ignoring this msg");
  }

  /* trigger an indication for GRR if required */
  if ((was_empty == TRUE) && (msgs_built == 0))
  {
    /* if this was the first message instance, but does not contain enough */
    /* data to trigger a complete SI or PSI, send a "dummy" indication now */
    /* This allows GRR to manage the 30 second timer for the container.    */
    gmac_nacc_ncell_data_ind_to_grr(NULL, container_id, DUMMY_PD, gas_id);
  }

  /* return the number of available msgs to the caller */
  return(msgs_built);
}

/*===========================================================================

FUNCTION gmac_util_handle_pkt_scell_data_instance

DESCRIPTION
  Processes an incoming PSCD instance (received from MAC during a TBF)

PARAMS
  *pscd_ptr   - pointer to the decoded message structure
  arfcn       - ARFCN where PSCD message was received
  release_ptr - pointer to boolean output variable that indicates whether to release

RETURN VALUE
  Number of coherent SI or PSI messages that can be built with the instances rcvd

SIDE EFFECTS
  None

===========================================================================*/
static uint8 gmac_util_handle_pkt_scell_data_instance(packet_scell_data_t *pscd_ptr,
                                                      ARFCN_T arfcn,
                                                      boolean *release_ptr,
                                                      gas_id_t gas_id)
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);
  container_repetition_struct_t *instance_ptr;
  uint8                          msgs_built = 0;
  uint8                          container_index;

  /* determine the container index for this instance */
  container_index = pscd_ptr->container_index;

  if (container_index >= MAX_NUMBER_CELL_DATA_INSTANCES)
  {
    MSG_GERAN_ERROR_1_G("Invalid container index %d", container_index);
    return(0);
  }

  /* See 3GPP spec CR GP-041158: rules for PSCD container validity */
  gmac_validate_container_sequence(&util_data_ptr->scell_info, SERVING_CELL_CONTAINER, container_index, gas_id);

  instance_ptr = util_data_ptr->scell_info.instance_ptrs[container_index];

  if (instance_ptr)
  {
    /* already got this instance, so free it up and replace */
    GPRS_MEM_FREE(instance_ptr);
  }

  /* extract the container repetition structure from the incoming message */
  instance_ptr = gmac_util_extract_pscd_instance_data(pscd_ptr, gas_id);

  if (instance_ptr)
  {
    /* flag the container as valid */
    util_data_ptr->scell_info.container_valid = TRUE;

    /* store the source ARFCN */
    util_data_ptr->scell_info.arfcn = arfcn;

    /* store the new instance and set relevant flag in bitmap */
    util_data_ptr->scell_info.instance_ptrs[container_index] = instance_ptr;
    util_data_ptr->scell_info.instances_rcvd |= (0x00000001 << container_index);

    MSG_GERAN_HIGH_1_G("Rcvd Pkt Scell Data instance %d", container_index);

    if (util_data_ptr->scell_info.instances_rcvd & 0x00000001)
    {
      /* once the first instance is available,   */
      /* try and build the container data stream */
      msgs_built = gmac_util_build_container(&util_data_ptr->scell_info, release_ptr, gas_id);
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Unable to allocate memory for PSCD: ignoring this msg");
  }

  return(msgs_built);
}

/* NACC FUNCTIONS AVAILABLE TO EXTERNAL MODULES */

/*===========================================================================

FUNCTION gmac_util_init_cell_data

DESCRIPTION
  Initialises the cell data stores within MAC

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmac_util_init_cell_data(gas_id_t gas_id)
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);
  ncell_data_t *ncell_info_ptr = util_data_ptr->ncell_info;
  uint8         i;

  for (i = 0; i < MAX_NUMBER_OF_NACC_NCELLS; i++, ncell_info_ptr++)
  {
    /* initialise each neighbour cell store in turn */
    ncell_info_ptr->ncell_arfcn.num  = 0xFFFF;
    ncell_info_ptr->ncell_arfcn.band = (sys_band_T)0xFF;
    ncell_info_ptr->ncell_bsic = 0xFF;

    gmac_util_init_container(&(ncell_info_ptr->cell_data), gas_id);
  }

  /* initialise the serving cell container */
  gmac_util_init_container(&util_data_ptr->scell_info, gas_id);

  return;
}

/*===========================================================================

FUNCTION gmac_util_release_container

DESCRIPTION
  Indicates that the specified container ID is no longer required

PARAMS
  container_id - the container that should be released

RETURN VALUE
  Void

SIDE EFFECTS
  The memory allocations for the entire container are freed.

===========================================================================*/
void gmac_util_release_container(uint8 container_id,
                                 gas_id_t gas_id)
{
  cell_data_t *cell_data_ptr = NULL;
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  if (container_id == SERVING_CELL_CONTAINER)
  {
    cell_data_ptr = &util_data_ptr->scell_info;
  }
  else if (container_id < SERVING_CELL_CONTAINER)
  {
    ncell_data_t *ncell_info_ptr;

    ncell_info_ptr = &(util_data_ptr->ncell_info[container_id]);

    /* reset ARFCN and BSIC information */
    ncell_info_ptr->ncell_arfcn.num  = 0xFFFF;
    ncell_info_ptr->ncell_arfcn.band = (sys_band_T)0xFF;
    ncell_info_ptr->ncell_bsic = 0xFF;

    cell_data_ptr = &(ncell_info_ptr->cell_data);
  }

  if (cell_data_ptr)
  {
    if (cell_data_ptr->container_valid)
    {
      /* output debug only if the container is valid */
      MSG_GERAN_MED_1_G("Releasing cell data for container %1d", container_id);
    }

    /* free cloned message instance pointers */
    gmac_util_free_instance_ptrs(cell_data_ptr,gas_id);

    /* free container bytestream, segment and msg type pointers */
    gmac_util_free_container_ptrs(cell_data_ptr,gas_id);

    /* re-initialise the cell data contents */
    gmac_util_init_container(cell_data_ptr,gas_id);
  }

  return;
}

/*===========================================================================

FUNCTION gmac_nacc_release_all_containers

DESCRIPTION
  Releases every NACC container

PARAMS
  include_scell - whether to release the serving cell container

RETURN VALUE
  Void

SIDE EFFECTS
  The memory allocations for EVERY container are freed.

===========================================================================*/
static void gmac_nacc_release_all_containers(gas_id_t gas_id)
{
  uint8 container_id;

  for (container_id = 0; container_id < MAX_NUMBER_OF_NACC_NCELLS; container_id++)
  {
    gmac_util_release_container(container_id,gas_id);
  }

  return;
}

/*===========================================================================

FUNCTION gmac_nacc_respond_to_release_req

DESCRIPTION
  Responds to a NACC release request from GRR: releases the specified container

PARAMS
  signal_ptr - pointer to signal union containing NCELL_RELEASE_REQ from GRR

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
void gmac_nacc_respond_to_release_req(mac_signals_u *signal_ptr,
                                      gas_id_t       gas_id)
{
  gm_nacc_release_req_t *release_req_ptr;

  release_req_ptr = (gm_nacc_release_req_t *)&(signal_ptr->grr_mac_sig.msg.gm_nacc_release_req);

  if (release_req_ptr->release_flags == NACC_RELEASE_ONE)
  {
    /* GRR has requested that MAC now releases a single NCELL container */
    gmac_util_release_container(release_req_ptr->container_id, gas_id);
  }
  else
  {
    /* GRR has requested that MAC now releases all NCELL containers */
    gmac_nacc_release_all_containers(gas_id);
  }

  return;
}


/*===========================================================================

FUNCTION gmac_util_handle_pkt_cell_data

DESCRIPTION
  Handles PSCD or PNCD in MAC task loop: functionised to keep task code tidier.

PARAMS
  msg_type   - type extracted by CSN decoder from control message
  bcch_arfcn - the ARFCN reported by L1 in the PH_DATA_IND

RETURN VALUE
  Void

SIDE EFFECTS
  May trigger dispatch of data to GRR

===========================================================================*/
void gmac_util_handle_pkt_cell_data(uint8 msg_type,
                                    ARFCN_T bcch_arfcn,
                                    gas_id_t gas_id)
{
  uint8 container_id = SERVING_CELL_CONTAINER;
  uint8 trigger_msg  = 0;
  boolean bRelease   = FALSE;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  if (gmac_util_check_tfi(msg_type,gas_id))
  {
    if (msg_type == PACKET_SERVING_CELL_DATA)
    {
      trigger_msg = gmac_util_handle_pkt_scell_data_instance((packet_scell_data_t *)global_data_ptr->csn_dec_data_ptr,
                                                             bcch_arfcn, &bRelease,
                                                             gas_id);
    }
    else
    {
      trigger_msg = gmac_nacc_handle_pkt_ncell_data_instance((packet_ncell_data_t *)global_data_ptr->csn_dec_data_ptr,
                                                             bcch_arfcn,
                                                             &container_id, &bRelease,
                                                             gas_id);
    }
  }

  /* free the CSN memory here: info now stored in global data */
  free_csn_decoder_ptrs_t(global_data_ptr->csn_dec_result_ptr);

  /* if the container has been built into one or more complete messages, call the trigger */
  if (trigger_msg)
  {
    /* trigger new SI and PSI messages to be sent to GRR */
    gmac_util_trigger_cell_data(container_id, bRelease,gas_id);
  }

  return;
}

/*===========================================================================

FUNCTION gmac_util_check_tfi

DESCRIPTION
  Checks TFI in Pkt Scell Data (or Pkt Cell Change Continue or Pkt NCell Data for NACC)

PARAMS
  message_type - either PACKET_SERVING_CELL_DATA     or
                        PACKET_CELL_CHANGE_CONTINUE  or
                        PACKET_NEIGHBOUR_CELL_DATA


RETURN VALUE
  TRUE if the TFI in the message matches the current global store in MAC

SIDE EFFECTS
  None

===========================================================================*/
boolean gmac_util_check_tfi(uint8 message_type, gas_id_t gas_id)
{
  boolean addresses_ms = FALSE;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  if (message_type == PACKET_SERVING_CELL_DATA)
  {
    packet_scell_data_t *pscd_ptr;

    pscd_ptr = (packet_scell_data_t *)global_data_ptr->csn_dec_data_ptr;

    addresses_ms = gmac_util_global_tfi(pscd_ptr->global_tfi,gas_id);
  }
  else if (message_type == PACKET_CELL_CHANGE_CONTINUE)
  {
    packet_cell_change_continue_t *pccc_ptr;

    pccc_ptr = (packet_cell_change_continue_t *)global_data_ptr->csn_dec_data_ptr;

    addresses_ms = gmac_util_global_tfi(pccc_ptr->global_tfi,gas_id);
  }
  else if (message_type == PACKET_NEIGHBOUR_CELL_DATA)
  {
    packet_ncell_data_t *pncd_ptr;

    pncd_ptr = (packet_ncell_data_t *)global_data_ptr->csn_dec_data_ptr;

    addresses_ms = gmac_util_global_tfi(pncd_ptr->global_tfi,gas_id);
  }

  return(addresses_ms);
}

/*=============================================================================

FUNCTION gmac_mac_l1_freq_config()

DESCRIPTION
  Sends MAC_L1_DTM_CHANNEL_ASSIGN_REQ to L1

PARAMS
  sig_data_ptr signal data to be sent in mac_l1_freq_config

RETURN VALUE

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_l1_freq_config(mac_l1_freq_config_t *sig_data_ptr, gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  IMH_T  *l1_sig_message_header_ptr;

  l1_sig_message_header_ptr = &(global_data_ptr->l1_sig.mac_l1_freq_config.message_header);

  global_data_ptr->l1_sig.mac_l1_freq_config = *sig_data_ptr;

#ifdef FEATURE_GSM_DTM
  MSG_GERAN_MED_1_G("MAC Sent MAC_L1_FREQ_CONFIG, freq present = %d",sig_data_ptr->dtm_freq_param_present);
#else
  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_FREQ_CONFIG");
#endif /*FEATURE_GSM_DTM*/

  l1_sig_message_header_ptr->message_set = MS_MAC_L1;
  l1_sig_message_header_ptr->message_id = (uint8)MAC_L1_FREQ_CONFIG;
  gprs_l1_mac_acc_func(&global_data_ptr->l1_sig, gas_id);

}

#ifdef FEATURE_GSM_DTM
/*=============================================================================

FUNCTION gmac_send_mac_grr_dtm_channel_assign_rej()

DESCRIPTION
  Constructs and sends MAC_GRR_DTM_CHANNEL_ASSIGN_REJ to GRR. Used when a an error
  in the assignment data has caused MACC to abort the establishment.

PARAMS
  dtm_ass_status

RETURN VALUE

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_grr_dtm_channel_assign_rej(dtm_ass_status_t dtm_ass_status,
                                              gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /*MAP dtm_ass_status to RR Cause values. These are defined in cause.h (services\utils)*/
  /*See 44.018 10.5.2.3.1 RR Cause*/
  if(dtm_ass_status == DTM_ASS_REJ_FREQ_NOT_IMPLEMENTED)
  {
    global_data_ptr->grr_sig.mac_grr_dtm_channel_assign_rej.RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
  }
  else if(dtm_ass_status == DTM_ASS_REJ_PROTOCOL_ERR)
  {
    global_data_ptr->grr_sig.mac_grr_dtm_channel_assign_rej.RR_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;
  }
  else if(dtm_ass_status == DTM_ASS_REJ_CHAN_MODE_UNACCEPTABLE)
  {
    global_data_ptr->grr_sig.mac_grr_dtm_channel_assign_rej.RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
  }
  else if(dtm_ass_status == DTM_ASS_REJ_NO_CELL_ALLOC_AVAILABLE)
  {
    global_data_ptr->grr_sig.mac_grr_dtm_channel_assign_rej.RR_cause = RR_NO_CELL_ALLOCATION_AVAILABLE;
  }

  MSG_GERAN_MED_2_G("MAC Sent MAC_GRR_DTM_CHANNEL_ASSIGN_REJ reason %d dtm_ass_status %d",
                    global_data_ptr->grr_sig.mac_grr_dtm_channel_assign_rej.RR_cause,dtm_ass_status);

  global_data_ptr->grr_sig.mac_grr_dtm_channel_assign_rej.message_header.message_set = MS_MAC_RR;
  global_data_ptr->grr_sig.mac_grr_dtm_channel_assign_rej.message_header.message_id = (uint8)MAC_GRR_DTM_CHANNEL_ASSIGN_REJ;
  gprs_grr_mac_acc_func(&global_data_ptr->grr_sig, gas_id);

}

/*=============================================================================

FUNCTION gmac_mac_l1_dtm_channel_assign_req()

DESCRIPTION
  Sends MAC_L1_DTM_CHANNEL_ASSIGN_REQ to L1

PARAMS
  l1_sig_data_ptr :- signal data to be sent in MAC_L1_DTM_CHANNEL_ASSIGN_REQ

RETURN VALUE

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_l1_dtm_channel_assign_req
(
 mac_l1_dtm_channel_assign_req_t *l1_sig_data_ptr,
 gas_id_t                         gas_id
)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  IMH_T  *l1_sig_message_header_ptr;

  l1_sig_message_header_ptr = &(global_data_ptr->l1_sig.mac_l1_dtm_channel_assign_req.message_header);

  global_data_ptr->l1_sig.mac_l1_dtm_channel_assign_req = *l1_sig_data_ptr;

  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_DTM_CHANNEL_ASSIGN_REQ");
  l1_sig_message_header_ptr->message_set = MS_MAC_L1;
  l1_sig_message_header_ptr->message_id = (uint8)MAC_L1_DTM_CHANNEL_ASSIGN_REQ;
  gprs_l1_mac_acc_func(&global_data_ptr->l1_sig, gas_id);

}

/*=============================================================================

FUNCTION gmac_util_dtm_reverse_ts_bitmap()

DESCRIPTION
  Creates a reversed timeslot bitmap.

PARAMS
  input_ts_bmap :- timeslot bitmap to be reversed.

RETURN VALUE
  uint8 - reversed bitmap

SIDE EFFECTS
  NONE

=============================================================================*/
static uint8 gmac_util_dtm_reverse_ts_bitmap( uint8 input_ts_bmap,
                                              gas_id_t gas_id )
{
  uint8 local_ts_mask = 0x80;
  uint8 local_usf_mask = 0x01;
  uint8 local_reversed_ts_bmap = 0;
  uint8 local_loop_cnt = 0;

  /* reverse usf_bitmap to coincide with TS bitmap */
  for (local_loop_cnt=0; local_loop_cnt<8; local_loop_cnt++)
  {
    if (input_ts_bmap & local_usf_mask)
    {
      local_reversed_ts_bmap |= local_ts_mask;
    }
    local_usf_mask <<= 1;
    local_ts_mask >>= 1;
  }
  return local_reversed_ts_bmap;
}


/*===========================================================================

FUNCTION gmac_util_get_eda_from_dtm_ass

DESCRIPTION
  Gets the EDA flag from UL or DL DTM assignments.

PARAMS
 dtm_ass_ptr                   Rcvd DTM signal data.
 eda                           Value of EDA is written to this return ptr.

RETURN VALUE
 boolean - indicates if function successfully obtained the EDA.

SIDE EFFECTS
  None

===========================================================================*/
static boolean gmac_util_get_eda_from_dtm_ass( gm_dtm_channel_assign_req_t *dtm_ass_ptr,
                                               uint8                       *eda_ptr,
                                               gas_id_t                     gas_id)
{
  /* local variables */
  boolean local_get_eda_status = TRUE;

  /* overcome line length probs*/
  uint8 local_alloc_type = dtm_ass_ptr->gm_dtm_ul_tbf_ass.allocation_type;
  uint8 local_ul_eda_flag =  dtm_ass_ptr->gm_dtm_ul_tbf_ass.allocation.
                             dynamic_alloc.ext_dynamic_alloc;
  uint8 local_mac_mode = dtm_ass_ptr->gm_dtm_dl_tbf_ass.mac_mode;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /* get the EDA value */
  switch(dtm_ass_ptr->dtm_tbf_type)
  {
    case UL_TBF_PRESENT:

      if ( local_alloc_type == GMAC_RR_PUA_DYNAMIC_ALLOCATION )
      {
        *eda_ptr = local_ul_eda_flag;
      }
      else
      {
        MSG_GERAN_HIGH_1_G("MAC info Unexpected UL alloc type %d",local_alloc_type);
        local_get_eda_status = FALSE;
      }
    break;

    case DL_TBF_PRESENT:
      /*MAC_MODE in DTM Assignment Command should be ignored for rel6 EDA */
      *eda_ptr = GMAC_MAC_MODE_DYNAMIC_ALLOC;
    break;

    case UL_DL_TBF_PRESENT:
      if (
           (
             ( local_ul_eda_flag == GMAC_MAC_MODE_DYNAMIC_ALLOC ) ||
             ( local_ul_eda_flag == GMAC_MAC_MODE_EXTENDED_DYNAMIC_ALLOC )
           ) &&
           ( local_alloc_type == GMAC_RR_PUA_DYNAMIC_ALLOCATION )
         )
      {
        *eda_ptr = local_ul_eda_flag;
      }
      else
      {
        MSG_GERAN_HIGH_1_G("MAC info invalid MAC MODE in DL Ass, mac_mode: %d",local_mac_mode);
        local_get_eda_status = FALSE;
      }
    break;

    case NO_TBF_PRESENT:
#ifdef FEATURE_GSM_EDTM
      if ( global_data_ptr->gmac_edtm_enabled &&
           global_data_ptr->mac_mode == TRANSFER_MODE )
      {
        /* get the mac mode from the global store*/
        *eda_ptr = global_data_ptr->gmac_current_mac_mode;
      }
      else
#endif /*FEATURE_GSM_EDTM*/
      {
        MSG_GERAN_ERROR_0_G("MAC info NO_TBF_PRESENT dtm_tbf_type");
        local_get_eda_status = FALSE;
      }
    break;

    default:
      MSG_GERAN_ERROR_1_G("MAC invalid dtm_tbf_type: %d",dtm_ass_ptr->dtm_tbf_type);
      local_get_eda_status = FALSE;
    break;
  }

  return local_get_eda_status;
}

/*===========================================================================

FUNCTION gmac_util_dtm_ms_class_check

DESCRIPTION
  Performs multislot checking on the DTM Channel Ass.

PARAMS
 dtm_ass_ptr                   Rcvd DTM signal data.

RETURN VALUE
 boolean - indicates if the multislot check passed .

SIDE EFFECTS
  If the multislot check is successful the following globals are set:
    gmac_local_ul_ts_alloc
    gmac_local_dl_ts_alloc
    gmac_dtm_cs_ts_alloc

===========================================================================*/
static boolean gmac_util_dtm_ms_class_check( gm_dtm_channel_assign_req_t *dtm_ass_ptr,
                                             gas_id_t gas_id )
{
  /* local variables */
  uint8                   local_eda = 0;
  rr_pua_dynamic_alloc_t *local_dyn_ptr;
  uint8                   local_ul_ts_alloc = 0;
  uint8                   local_dl_ts_alloc = 0;
  boolean                 result = TRUE;
  gmac_global_data_t     *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  if ( gmac_util_get_eda_from_dtm_ass( dtm_ass_ptr, &local_eda, gas_id ) == TRUE )
  {
    local_dyn_ptr = &dtm_ass_ptr->gm_dtm_ul_tbf_ass.allocation.dynamic_alloc;

    switch(dtm_ass_ptr->dtm_tbf_type)
    {
      case UL_TBF_PRESENT:
        if ( local_dyn_ptr->timeslot_alloc_flag == usf_only )
        {
          local_ul_ts_alloc = gmac_util_dtm_reverse_ts_bitmap(local_dyn_ptr->usf_bitmap, gas_id);
        }
        else
        {
          local_ul_ts_alloc = gmac_util_dtm_reverse_ts_bitmap(local_dyn_ptr->gamma_bitmap, gas_id);
        }
        local_dl_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
      break;

      case DL_TBF_PRESENT:
        local_ul_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
        local_dl_ts_alloc = dtm_ass_ptr->gm_dtm_dl_tbf_ass.timeslot_allocation;
      break;

      case UL_DL_TBF_PRESENT:
        if ( local_dyn_ptr->timeslot_alloc_flag == usf_only )
        {
          local_ul_ts_alloc = gmac_util_dtm_reverse_ts_bitmap(local_dyn_ptr->usf_bitmap, gas_id);
        }
        else
        {
          local_ul_ts_alloc = gmac_util_dtm_reverse_ts_bitmap(local_dyn_ptr->gamma_bitmap, gas_id);
        }
        local_dl_ts_alloc = dtm_ass_ptr->gm_dtm_dl_tbf_ass.timeslot_allocation;
      break;

      case NO_TBF_PRESENT:
#ifdef FEATURE_GSM_EDTM
        if ( global_data_ptr->gmac_edtm_enabled &&
             global_data_ptr->mac_mode == TRANSFER_MODE )
        {
          /* use the current ts allocation */
          local_ul_ts_alloc = global_data_ptr->gmac_local_ul_ts_alloc;
          local_dl_ts_alloc = global_data_ptr->gmac_local_dl_ts_alloc;
        }
        else
#endif /*FEATURE_GSM_EDTM*/
        {
          MSG_GERAN_ERROR_0_G("MAC info NO_TBF_PRESENT dtm_tbf_type");
          result = FALSE;
        }
      break;

      default:
        MSG_GERAN_ERROR_1_G("MAC DTM Invalid dtm_tbf_type: %d",dtm_ass_ptr->dtm_tbf_type);
        result = FALSE;
      break;

    }

    if ( result == TRUE )
    {
#ifdef FEATURE_GPRS_PS_HANDOVER
      if ( gmac_util_multislot_class_check(local_ul_ts_alloc,
                                           local_dl_ts_alloc,
                                           local_eda,
                                           dtm_ass_ptr->current_cs_timeslot,
                                           TRUE, PSHO_UPDATE_PARAMS_NOT_REQUIRED,
                                           gas_id ) == TRUE )
#else
      if ( gmac_util_multislot_class_check(local_ul_ts_alloc,
                                           local_dl_ts_alloc,
                                           local_eda,
                                           dtm_ass_ptr->current_cs_timeslot,
                                           TRUE,
                                           gas_id ) == TRUE )
#endif /*FEATURE_GPRS_PS_HANDOVER*/
      {
        /* the multislot check passed so write the cs timeslot
           bitmap to the global variable
        */
        global_data_ptr->gmac_dtm_cs_ts_alloc = dtm_ass_ptr->current_cs_timeslot;
      }
      else
      {
        MSG_GERAN_HIGH_0_G("MAC DTM Err, MS Class check Failed");
        result = FALSE;
      }
    }
  }
  else
  {
    result = FALSE;
  }
  return result;
}

 /*===========================================================================

FUNCTION gmac_dtm_chan_ass_handler

DESCRIPTION
  Take the assignment data rcvd from GRR and uses it to construct mac_l1_freq_config
  destined for l1.

  Determines what pkt assigment information and what CS data is to be included
  mac_l1_dtm_chan_ass_req

PARAMS
 dtm_ass_ptr                   Rcvd signal data
 mr_ul_alloc_cnf_sig_ptr       Send signal data
 mac_l1_dtm_chan_ass_req_ptr   Send signal data
 mac_l1_freq_config_ptr        Send signal data

RETURN VALUE
 dtm_ass_status = status of assigment .

SIDE EFFECTS
  Accesses current_tbf_mode.

===========================================================================*/
dtm_ass_status_t gmac_dtm_chan_ass_handler
(
  gm_dtm_channel_assign_req_t     *dtm_ass_ptr,                 /*Rcvd DTM ass      */
  mr_ul_alloc_cnf_t               *mr_ul_alloc_cnf_sig_ptr,     /*ul config for rlc */
  mac_l1_dtm_channel_assign_req_t *mac_l1_dtm_chan_ass_req_ptr, /*DTM config for L1 */
  mac_l1_freq_config_t            *mac_l1_freq_config_ptr,      /*fREQ CONFIG FOR L1*/
  gas_id_t                         gas_id
)
{

  dtm_ass_status_t                 dtm_ass_status = NO_DTM_ASS_ERROR;
                                                 /*  NO_DTM_ASS_ERROR,
                                                     DTM_ASS_REJ_FREQ_NOT_IMPLEMENTED,
                                                     DTM_ASS_REJ_PROTOCOL_ERR,
                                                     DTM_ASS_REJ_CHAN_MODE_UNACCEPTABLE,
                                                     DTM_ASS_REJ_NO_CELL_ALLOC_AVAILABLE
                                                  */
  tbf_mode_T                       dtm_tbf_mode = TBF_MODE_GPRS;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /****************           Initalise signals              ************/

  memset(mr_ul_alloc_cnf_sig_ptr, 0, sizeof(mr_ul_alloc_cnf_t));
  memset(mac_l1_dtm_chan_ass_req_ptr, 0, sizeof(mac_l1_dtm_channel_assign_req_t));

  /***************          generic L1 signal data              ***********/

  /*dtm_tbf_type = UL ass only, dl ass only or UL + DL ass*/
  mac_l1_dtm_chan_ass_req_ptr->dtm_tbf_type = dtm_ass_ptr->dtm_tbf_type;

  /* cell options data */
  mac_l1_dtm_chan_ass_req_ptr->gprs_l1_cell_opt = dtm_ass_ptr->gprs_l1_cell_opt;

  /* cs data */
  mac_l1_dtm_chan_ass_req_ptr->cs_present = dtm_ass_ptr->cs_present;
  mac_l1_dtm_chan_ass_req_ptr->cs_desc = dtm_ass_ptr->cs_desc;

  /* Frequency params. NOTE :- GRR has already performed frequency checking           */

  /* Maps frequency params rcvd in DTM ass command into freq config MAC_L1_FREQ_CONFIG*/
  gmac_dtm_freq_handling( dtm_ass_ptr->dtm_freq_present,
                          &(dtm_ass_ptr->dtm_freq_info),
                          mac_l1_freq_config_ptr,
                          dtm_ass_ptr->dtm_tbf_type,
                          gas_id );


#ifdef FEATURE_GSM_EDTM
  /*Check for no PS description in DTM Ass Cmd*/
  if ( global_data_ptr->gmac_edtm_enabled &&
       global_data_ptr->mac_mode == TRANSFER_MODE )
  {
    /*If doing EDTM (as advertised to the NW) then it is ok for the DTM Ass Cmd to not specify any PS*/
    MSG_GERAN_MED_1_G("GMAC EDTM DTM Ass Cmd in transfer with tbf_type: 0x%x",dtm_ass_ptr->dtm_tbf_type);
  }
  else
  {
    /* Check if no PS is defined if doing normal DTM*/
    if (dtm_ass_ptr->dtm_tbf_type == NO_TBF_PRESENT )
    {
      MSG_GERAN_ERROR_0_G("GMAC DTM no TBF present in DTM Ass Cmd");
      dtm_ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
    }
  }

  if(dtm_ass_status == NO_DTM_ASS_ERROR)
#endif /*FEATURE_GSM_EDTM*/
  {
    /*Multi slot checking*/
    if ( gmac_util_dtm_ms_class_check( dtm_ass_ptr, gas_id ) == FALSE )
    {
#ifdef FEATURE_GSM_EDTM
      if ( global_data_ptr->gmac_edtm_enabled )
      {
        dtm_ass_status = DTM_ASS_REJ_PROTOCOL_ERR;  /* This is for rel 6*/
      }
      else
#endif /*FEATURE_GSM_EDTM*/
      {
        dtm_ass_status = DTM_ASS_REJ_CHAN_MODE_UNACCEPTABLE; /* This is for rel 4*/
      }
    }
  }

  if(dtm_ass_status == NO_DTM_ASS_ERROR)
  {
    dtm_ass_status = gmac_dtm_check_for_egprs_ass_clash(dtm_ass_ptr, gas_id);

    if ( global_data_ptr->mac_mode == TRANSFER_MODE )
    {
      /* Check for EGPRS/GPRS mode change */
      if ( dtm_ass_status == NO_DTM_ASS_ERROR )
      {
        if ( ( dtm_ass_ptr->dtm_tbf_type ==  UL_TBF_PRESENT ) ||
             ( dtm_ass_ptr->dtm_tbf_type ==  UL_DL_TBF_PRESENT ) )
        {
          if ( dtm_ass_ptr->gm_dtm_ul_tbf_ass.egprs_flag == valid )
          {
            dtm_tbf_mode = TBF_MODE_EGPRS;
          }
          else
          {
            dtm_tbf_mode = TBF_MODE_GPRS;
          }
        }
        else if ( dtm_ass_ptr->dtm_tbf_type ==  DL_TBF_PRESENT  )
        {
          if ( dtm_ass_ptr->gm_dtm_dl_tbf_ass.egprs_tbf_mode_flag == valid )
          {
            dtm_tbf_mode = TBF_MODE_EGPRS;
          }
          else
          {
            dtm_tbf_mode = TBF_MODE_GPRS;
          }
        }
        else
        {
#ifdef FEATURE_GSM_EDTM
          if ( global_data_ptr->gmac_edtm_enabled &&
               global_data_ptr->mac_mode == TRANSFER_MODE )
          {
            dtm_tbf_mode = global_data_ptr->current_tbf_mode;
          }
          else
#endif /*FEATURE_GSM_EDTM*/
          {
            dtm_ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
            MSG_GERAN_ERROR_1_G("GMAC DTM, dtm_ass_ptr->dtm_tbf_type is invalid: 0X%X",dtm_ass_ptr->dtm_tbf_type);
          }
        }

        if ( dtm_ass_status == NO_DTM_ASS_ERROR )
        {
          if ( global_data_ptr->current_tbf_mode != dtm_tbf_mode )
          {
            dtm_ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
            MSG_GERAN_HIGH_2_G("GMAC DTM, Illegal change of TBF Mode, current:0x%x, dtm:0x%x",global_data_ptr->current_tbf_mode, dtm_tbf_mode);
          }
        }
      }
    }
  }

  if(dtm_ass_status == NO_DTM_ASS_ERROR)
  {
    switch(dtm_ass_ptr->dtm_tbf_type)
    {
      case UL_TBF_PRESENT:
        global_data_ptr->gmac_current_mac_mode = dtm_ass_ptr->gm_dtm_ul_tbf_ass.allocation.dynamic_alloc.ext_dynamic_alloc;
        dtm_ass_status = gmac_dtm_ul_ass_handler(
                         &(dtm_ass_ptr->gm_dtm_ul_tbf_ass),
                         mr_ul_alloc_cnf_sig_ptr,
                         &(mac_l1_dtm_chan_ass_req_ptr->ul_tbf),
                         gas_id);
        break;

      case DL_TBF_PRESENT:
        global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
        dtm_ass_status = gmac_dtm_dl_ass_handler(
                         &(dtm_ass_ptr->gm_dtm_dl_tbf_ass),
                         &(mac_l1_dtm_chan_ass_req_ptr->dl_tbf),
                         gas_id);
        break;

      case UL_DL_TBF_PRESENT:
        dtm_ass_status = gmac_dtm_ul_ass_handler(
                         &(dtm_ass_ptr->gm_dtm_ul_tbf_ass),
                         mr_ul_alloc_cnf_sig_ptr,
                         &(mac_l1_dtm_chan_ass_req_ptr->ul_tbf),
                         gas_id);

        if (dtm_ass_status == NO_DTM_ASS_ERROR)
        {
          global_data_ptr->gmac_current_mac_mode = dtm_ass_ptr->gm_dtm_ul_tbf_ass.allocation.dynamic_alloc.ext_dynamic_alloc;
          dtm_ass_status = gmac_dtm_dl_ass_handler(
                           &(dtm_ass_ptr->gm_dtm_dl_tbf_ass),
                           &(mac_l1_dtm_chan_ass_req_ptr->dl_tbf),
                           gas_id);
        }
        break;

        case NO_TBF_PRESENT:
#ifdef FEATURE_GSM_EDTM
          if ( global_data_ptr->gmac_edtm_enabled &&
              global_data_ptr-> mac_mode == TRANSFER_MODE )
          {
            /*No Tbfs defined so no need to call handlers*/
          }
          else
#endif /*FEATURE_GSM_EDTM*/
          {
            MSG_GERAN_ERROR_0_G("MAC Info dtm_tbf_type is NO_TBF_PRESENT");
            dtm_ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
          }
        break;

      default:
        MSG_GERAN_ERROR_1_G("MAC info Invalid dtm_tbf_type : %d",dtm_ass_ptr->dtm_tbf_type);
        dtm_ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
        break;

    } /*end of dtm_tbf_type switch*/
  }

  return (dtm_ass_status);
}

/*===========================================================================

FUNCTION gmac_dtm_ul_ass_handler

DESCRIPTION
  Take the UL assignment data rcvd from GRR and uses it to construct
  MAC_L1_DTM_CHANNEL_ASSIGN_REQ destined for l1
  MR_UL_ALLOC_CNF destined for RLC

PARAMS
  dtm_ul_ass_ptr          :-pkt ul ass rcvd from grr
  mr_ul_alloc_cnf_sig_ptr :-MR_UL_ALLOC_CNF to be send to rlc
  ul_tbf_ptr              :-MAC_L1_DTM_CHANNEL_ASSIGN_REQ to be sent to l1

RETURN VALUE
  dtm_ass_status

SIDE EFFECTS
  None

===========================================================================*/

dtm_ass_status_t gmac_dtm_ul_ass_handler
(
  rr_packet_uplink_assignment_t  *dtm_ul_ass_ptr,          /*pkt ul ass rcvd from grr*/
  mr_ul_alloc_cnf_t              *mr_ul_alloc_cnf_sig_ptr, /*ul config to be send to rlc*/
  mac_dynamic_tbf_information_T  *ul_tbf_ptr,              /*ul config to be sent to l1*/
  gas_id_t                        gas_id
)
{
  rr_pua_dynamic_alloc_t       *dtm_ul_dyn_ptr = &dtm_ul_ass_ptr->allocation.dynamic_alloc;
  dtm_ass_status_t             dtm_ass_status = NO_DTM_ASS_ERROR;
  gmac_global_data_t          *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /***************************************************************************
  * The type so UL allocation.  ref 044.018   10.5.2.25c.1:
  *
  *    01 Dynamic Allocation        (subdivied in dyn alloc struct)
  *    10 reserved
  *    11 reserved
  *    00 Extension                 (reserved)
  ****************************************************************************/

  if(dtm_ul_ass_ptr->allocation_type == GMAC_RR_PUA_DYNAMIC_ALLOCATION )
  {
    /****************************************************************************
    *                  Populate MR_UL_ALLOC_CNF msg                             *
    ****************************************************************************/

    mr_ul_alloc_cnf_sig_ptr->sig = MR_UL_ALLOC_CNF;

    if( dtm_ul_ass_ptr->egprs_flag == valid )
    {
      mr_ul_alloc_cnf_sig_ptr->tbf_mode  = TBF_MODE_EGPRS;

      if ( gmac_util_egprs_mcs_valid(dtm_ul_ass_ptr->egprs_mcs_mode, gas_id) == FALSE )
      {
        return (DTM_ASS_REJ_PROTOCOL_ERR);
      }
      else
      {
        mr_ul_alloc_cnf_sig_ptr->mcs = dtm_ul_ass_ptr->egprs_mcs_mode;
      }

      mr_ul_alloc_cnf_sig_ptr->resegment = dtm_ul_ass_ptr->resegment;


      if(dtm_ul_dyn_ptr->timeslot_alloc_flag == usf_only)
      {
        global_data_ptr->current_egprs_ul_ws = gmac_egprs_decode_ul_window_size(
                                dtm_ul_ass_ptr->epgrs_win_sz,
                                dtm_ul_dyn_ptr->usf_bitmap,
                                gas_id);
      }
      else
      {
        global_data_ptr->current_egprs_ul_ws = gmac_egprs_decode_ul_window_size(
                                dtm_ul_ass_ptr->epgrs_win_sz,
                                dtm_ul_dyn_ptr->gamma_bitmap,
                                gas_id);
      }

      if(global_data_ptr->current_egprs_ul_ws != WS_ALLOCATION_INVALID )
      {
        mr_ul_alloc_cnf_sig_ptr->ws = global_data_ptr->current_egprs_ul_ws;
      }
      else
      {
        return (DTM_ASS_REJ_PROTOCOL_ERR) ;
      }
    }
    else
    {
      mr_ul_alloc_cnf_sig_ptr->tbf_mode = TBF_MODE_GPRS;
    }

    if ( dtm_ul_dyn_ptr->ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC )
    {
      mr_ul_alloc_cnf_sig_ptr->access_mode = RM_EXT_DYNAMIC_ALLOC;
    }
    else
    {
      mr_ul_alloc_cnf_sig_ptr->access_mode = RM_DYNAMIC_ALLOC;
    }

    /********************************************************************************
    * NO such thing as ACCESS_TYPE when establishing an UL via DTM. In DTM REQUEST  *
    * msg sent on DCCH, PACKET_ESTABLISHMENT_CAUSE is used. ref 10.5.2.8b.2: 44.018.*
    * Because Channel Request Description is sent in DTM request assume DTM ul est  *
    * is a 2 phase access procedure                                                 *
    ********************************************************************************/
    mr_ul_alloc_cnf_sig_ptr->access_phase = GMAC_TWO_PHASE_ACCESS;

    /* RLC DATA BLOCKS GRANTED is not used in DTM so set tbf_type to open ended */
    mr_ul_alloc_cnf_sig_ptr->tbf_type = RM_OPEN_ENDED_TBF;

    mr_ul_alloc_cnf_sig_ptr->cs = dtm_ul_ass_ptr->channel_coding_command;

    /* DTM is effectively 2p access process hence no TLLI's to be include in RLC data */
    mr_ul_alloc_cnf_sig_ptr->ti = RM_TLLI_NOT_PRESENT;
    mr_ul_alloc_cnf_sig_ptr->tlli = 0;
    mr_ul_alloc_cnf_sig_ptr->tlli_block_cs = dtm_ul_ass_ptr->tlli_block_channel_coding;


    /* MAC mode dependent param handling */
    if(global_data_ptr->mac_mode == NULL_MODE)
    {
      mr_ul_alloc_cnf_sig_ptr->retry = (uint8)global_data_ptr->c_flags.mac_ctrl_header_r_bit;

      if( dtm_ul_dyn_ptr->uplk_tfi_assignment_flag == valid )
      {
        mr_ul_alloc_cnf_sig_ptr->ul_tfi = dtm_ul_dyn_ptr->uplk_tfi_assignment;
        global_data_ptr->gmac_local_tfi_store.ul_tfi = dtm_ul_dyn_ptr->uplk_tfi_assignment;
        global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = TRUE;
      }
      else
      {
        return DTM_ASS_REJ_PROTOCOL_ERR;
      }

    }
    else if(global_data_ptr->mac_mode == TRANSFER_MODE)
    {
      mr_ul_alloc_cnf_sig_ptr->retry = 0;

      /* If MAC is in Transfer(DTM) this function shall only be called
         if an UL TBF is currently active.  So, if there is
         no TFI assigned in the DTM Chan Ass, the gmac_re_assign_ul_tfi
         is set to the current assigned TFI.

          This may require so modification when EDTM is implemented */
      if ( dtm_ul_dyn_ptr->uplk_tfi_assignment_flag == valid )
      {
        global_data_ptr->gmac_re_assign_ul_tfi = dtm_ul_dyn_ptr->uplk_tfi_assignment;
      }
      else
      {
        global_data_ptr->gmac_re_assign_ul_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("GMAC DTM Chan Ass Handler called in wrong mac_mode: %d",global_data_ptr->mac_mode);

      /*Should never be in IDLE_MODE*/
      return DTM_ASS_REJ_PROTOCOL_ERR;
    }

    if(dtm_ul_dyn_ptr->timeslot_alloc_flag == usf_only)
    {
      mr_ul_alloc_cnf_sig_ptr->num_timeslots = gmac_utils_calculate_nts(
                                               dtm_ul_dyn_ptr->usf_bitmap,
                                               gas_id);
    }
    else
    {
      mr_ul_alloc_cnf_sig_ptr->num_timeslots = gmac_utils_calculate_nts(
                                               dtm_ul_dyn_ptr->gamma_bitmap,
                                               gas_id);
    }


    /* IE "RLC_DATA_BLOCKS_GRANTED" idicates if ass is open ended/close ended. Close  */
    /* ended has been removed form spec.Hence blocks_granted will always be open ended*/
    mr_ul_alloc_cnf_sig_ptr->blocks_granted = RM_OPEN_ENDED_TBF;

    /* Not relevent applies to fixed alloc only */
    mr_ul_alloc_cnf_sig_ptr->final_alloc = 0;

    mr_ul_alloc_cnf_sig_ptr->usf_granularity = dtm_ul_dyn_ptr->usf_granularity;

    /* dcch_access is only relevant for UL est VIA DTM using GTTP protocol */
    mr_ul_alloc_cnf_sig_ptr->dtm_access = FALSE;


    /*******************************************************************************
    *             Populate UL_TBF struct of  MAC_L1_DTM_CHANNEL_ASSIGN_REQ         *
    *******************************************************************************/

    gmac_dtm_ul_dyn_alloc_ie(dtm_ul_dyn_ptr,ul_tbf_ptr,gas_id);

    /*  TA info is not valid for DTM operation, L1 get it via CS connection */
    ul_tbf_ptr->timing_advance_params.ta_value_ie.present = FALSE;
    ul_tbf_ptr->timing_advance_params.cont_ta_ie.present = FALSE;

    /* Any TA paramaters rcvd viA PUA,PDA,PTR or PPCTA while in DTM mode will be *
    *  ignored by L1 If rcvd during TRANSFER MAC is to act as normal and send L1 *
    *  the TA info, L1 will ignore it                                           */
    global_data_ptr->gmac_local_l1_ta_params.valid_options = no_timing_params_valid;

    if( dtm_ul_ass_ptr->egprs_flag == valid )
    {
      /* BEP2 not in DTM msg*/
      ul_tbf_ptr->bep_period2.present = FALSE;
      ul_tbf_ptr->bep_period2.bep_period2 = 0;
      ul_tbf_ptr->tbf_mode = TBF_MODE_EGPRS;
    }
    else
    {
      ul_tbf_ptr->tbf_mode = TBF_MODE_GPRS;
    }

    ul_tbf_ptr->rlc_mode = (mac_rlc_mode_T)global_data_ptr->gmac_local_stored_ul_chan_req.rlc_mode;

    /*        TEST MODE params NOT applicable for DTM    */
    ul_tbf_ptr->test_mode = TEST_MODE_OFF;
    ul_tbf_ptr->ts_offset = 0;


    /*        MAC  Global Data Settting        */
    if( dtm_ul_ass_ptr->egprs_flag == valid )
    {
      global_data_ptr->current_tbf_mode = TBF_MODE_EGPRS;

      if ( global_data_ptr->mac_mode == NULL_MODE )
      {
        global_data_ptr->gmac_local_cs_setting = dtm_ul_ass_ptr->egprs_mcs_mode;
      }
    }
    else
    {
      global_data_ptr->current_tbf_mode = TBF_MODE_GPRS;

      if ( global_data_ptr->mac_mode == NULL_MODE )
      {
        global_data_ptr->gmac_local_cs_setting = dtm_ul_ass_ptr->channel_coding_command;
      }
    }

  }
  else if (dtm_ul_ass_ptr->allocation_type == GMAC_RR_PUA_RESERVED_1)
  {
    MSG_GERAN_HIGH_1_G("MAC info Unexpected UL alloc type %d",dtm_ul_ass_ptr->allocation_type);
    dtm_ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
  }
  else
  {
    MSG_GERAN_HIGH_1_G("MAC info reserved UL alloc type %d",dtm_ul_ass_ptr->allocation_type);
    dtm_ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
  }

  return (dtm_ass_status);
}



/*==================================================================================

FUNCTION gmac_dtm_ul_dyn_alloc_ie

DESCRIPTION
  Take the UL dynamic assignment data rcvd from GRR and uses it to construct the UL tbf
  configuration data portion of MAC_L1_DTM_CHANNEL_ASSIGN_REQ destined for l1

PARAMS
  rr_pua_dyn_alloc_ie_ptr :- pkt ul ass data rcvd from grr
  ul_tbf_ptr              :- ul tbf config data to be sent to l1

RETURN VALUE
  None

SIDE EFFECTS
  None

=====================================================================================*/
void gmac_dtm_ul_dyn_alloc_ie
(
  rr_pua_dynamic_alloc_t         *rr_pua_dyn_alloc_ie_ptr, /*pkt ul ass rcvd from grr */
  mac_dynamic_tbf_information_T  *ul_tbf_ptr,              /*ul tbf config data for l1*/
  gas_id_t                        gas_id
)
{
    uint8 usf_mask = 0x01;
    uint8 i;



    /* dynamic alloc type*/
    if(rr_pua_dyn_alloc_ie_ptr->ext_dynamic_alloc == GMAC_EDA_IND_DYNAMIC_ALLOC )
    {
      ul_tbf_ptr->mac_mode = MAC_L1_MAC_MODE_DYNAMIC;
    }
    else
    {
      ul_tbf_ptr->mac_mode = MAC_L1_MAC_MODE_EXTENDED_DYNAMIC;
    }

    /*DL power control*/
     /* mac_l1_dtm_chan_ass_req_ptr->dl_ctrl_param = not set in this function yet */
    if (rr_pua_dyn_alloc_ie_ptr->po_prmode_flag == valid)
    {
      ul_tbf_ptr->dl_ctrl_param.params_valid = TRUE;
      ul_tbf_ptr->dl_ctrl_param.po = rr_pua_dyn_alloc_ie_ptr->po;
      ul_tbf_ptr->dl_ctrl_param.pr_mode = rr_pua_dyn_alloc_ie_ptr->pr_mode;
    }
    else
    {
      ul_tbf_ptr->dl_ctrl_param.params_valid = FALSE;
    }



    /* usf_granularity */
    ul_tbf_ptr->usf_granularity = rr_pua_dyn_alloc_ie_ptr->usf_granularity;

    /* ul tfi ass. not sent to L1*/

    /* starting time :- Not applcable for PKT UL assigments rcvd via DTM mode*/
    ul_tbf_ptr->tbf_start_time_valid = FALSE;
    ul_tbf_ptr->starting_time = 0;

    /*    TS alloc     */
    if(rr_pua_dyn_alloc_ie_ptr->timeslot_alloc_flag == usf_only)
    {
      /**********************************************************************
      * populate USF bitmap :-                                              *
      * assuming that USF mask from GRR is TN7 high bit, -> TN0 low bit AND *
      * the usf_tn array has TN0 at index 0 and TN7 at index 6.             *
      **********************************************************************/

      usf_mask = 0x01;

      for ( i=0; i<8; i++ )
      {
        if ( ( rr_pua_dyn_alloc_ie_ptr->usf_bitmap & usf_mask ) == usf_mask )
        {
          ul_tbf_ptr->usf[i].present = TRUE;
          ul_tbf_ptr->usf[i].usf_value = rr_pua_dyn_alloc_ie_ptr->usf_tn[i];
        }
        else
        {
          ul_tbf_ptr->usf[i].present = FALSE;
          ul_tbf_ptr->usf[i].usf_value = 0;
        }

        usf_mask <<= 1;
      }
    }
    else /* gamma */
    {
      ul_tbf_ptr->power_control_params.present = TRUE;
      ul_tbf_ptr->power_control_params.alpha = rr_pua_dyn_alloc_ie_ptr->alpha;

      if (rr_pua_dyn_alloc_ie_ptr->gamma_bitmap & 0x01)
      {
        ul_tbf_ptr->usf[0].present = TRUE;
        ul_tbf_ptr->usf[0].usf_value = rr_pua_dyn_alloc_ie_ptr->usf_tn[0];
        ul_tbf_ptr->power_control_params.gamma_TN0.present = TRUE;
        ul_tbf_ptr->power_control_params.gamma_TN0.gamma = rr_pua_dyn_alloc_ie_ptr->gamma_tn[0];
      }
      else
      {
        ul_tbf_ptr->power_control_params.gamma_TN0.present = FALSE;
        ul_tbf_ptr->usf[0].present = FALSE;
      }
      if (rr_pua_dyn_alloc_ie_ptr->gamma_bitmap & 0x02)
      {
        ul_tbf_ptr->usf[1].present = TRUE;
        ul_tbf_ptr->usf[1].usf_value = rr_pua_dyn_alloc_ie_ptr->usf_tn[1];
        ul_tbf_ptr->power_control_params.gamma_TN1.present = TRUE;
        ul_tbf_ptr->power_control_params.gamma_TN1.gamma = rr_pua_dyn_alloc_ie_ptr->gamma_tn[1];
      }
      else
      {
        ul_tbf_ptr->power_control_params.gamma_TN1.present = FALSE;
        ul_tbf_ptr->usf[1].present = FALSE;
      }
      if (rr_pua_dyn_alloc_ie_ptr->gamma_bitmap & 0x04)
      {
        ul_tbf_ptr->usf[2].present = TRUE;
        ul_tbf_ptr->usf[2].usf_value = rr_pua_dyn_alloc_ie_ptr->usf_tn[2];
        ul_tbf_ptr->power_control_params.gamma_TN2.present = TRUE;
        ul_tbf_ptr->power_control_params.gamma_TN2.gamma = rr_pua_dyn_alloc_ie_ptr->gamma_tn[2];
      }
      else
      {
        ul_tbf_ptr->power_control_params.gamma_TN2.present = FALSE;
        ul_tbf_ptr->usf[2].present = FALSE;
      }
      if (rr_pua_dyn_alloc_ie_ptr->gamma_bitmap & 0x08)
      {
        ul_tbf_ptr->usf[3].present = TRUE;
        ul_tbf_ptr->usf[3].usf_value = rr_pua_dyn_alloc_ie_ptr->usf_tn[3];
        ul_tbf_ptr->power_control_params.gamma_TN3.present = TRUE;
        ul_tbf_ptr->power_control_params.gamma_TN3.gamma = rr_pua_dyn_alloc_ie_ptr->gamma_tn[3];
      }
      else
      {
        ul_tbf_ptr->power_control_params.gamma_TN3.present = FALSE;
        ul_tbf_ptr->usf[3].present = FALSE;
      }
      if (rr_pua_dyn_alloc_ie_ptr->gamma_bitmap & 0x10)
      {
        ul_tbf_ptr->usf[4].present = TRUE;
        ul_tbf_ptr->usf[4].usf_value = rr_pua_dyn_alloc_ie_ptr->usf_tn[4];
        ul_tbf_ptr->power_control_params.gamma_TN4.present = TRUE;
        ul_tbf_ptr->power_control_params.gamma_TN4.gamma = rr_pua_dyn_alloc_ie_ptr->gamma_tn[4];
      }
      else
      {
        ul_tbf_ptr->power_control_params.gamma_TN4.present = FALSE;
        ul_tbf_ptr->usf[4].present = FALSE;
      }
      if (rr_pua_dyn_alloc_ie_ptr->gamma_bitmap & 0x20)
      {
        ul_tbf_ptr->usf[5].present = TRUE;
        ul_tbf_ptr->usf[5].usf_value = rr_pua_dyn_alloc_ie_ptr->usf_tn[5];
        ul_tbf_ptr->power_control_params.gamma_TN5.present = TRUE;
        ul_tbf_ptr->power_control_params.gamma_TN5.gamma = rr_pua_dyn_alloc_ie_ptr->gamma_tn[5];
      }
      else
      {
        ul_tbf_ptr->power_control_params.gamma_TN5.present = FALSE;
        ul_tbf_ptr->usf[5].present = FALSE;
      }
      if (rr_pua_dyn_alloc_ie_ptr->gamma_bitmap & 0x40)
      {
        ul_tbf_ptr->usf[6].present = TRUE;
        ul_tbf_ptr->usf[6].usf_value = rr_pua_dyn_alloc_ie_ptr->usf_tn[6];
        ul_tbf_ptr->power_control_params.gamma_TN6.present = TRUE;
        ul_tbf_ptr->power_control_params.gamma_TN6.gamma = rr_pua_dyn_alloc_ie_ptr->gamma_tn[6];
      }
      else
      {
        ul_tbf_ptr->power_control_params.gamma_TN6.present = FALSE;
        ul_tbf_ptr->usf[6].present = FALSE;
      }
      if (rr_pua_dyn_alloc_ie_ptr->gamma_bitmap & 0x80)
      {
        ul_tbf_ptr->usf[7].present = TRUE;
        ul_tbf_ptr->usf[7].usf_value = rr_pua_dyn_alloc_ie_ptr->usf_tn[7];
        ul_tbf_ptr->power_control_params.gamma_TN7.present = TRUE;
        ul_tbf_ptr->power_control_params.gamma_TN7.gamma = rr_pua_dyn_alloc_ie_ptr->gamma_tn[7];
      }
      else
      {
        ul_tbf_ptr->power_control_params.gamma_TN7.present = FALSE;
        ul_tbf_ptr->usf[7].present = FALSE;
      }
    }
}


/*===========================================================================

FUNCTION gmac_dtm_dl_ass_handler

DESCRIPTION
  Take the DL assignment data rcvd from GRR and uses it to construct the DL tbf
  configuration data portion of MAC_L1_DTM_CHANNEL_ASSIGN_REQ destined for L1

PARAMS
  dtm_dl_ass_ptr :-  DL assigment data rcvd form GRR
  dl_tbf_ptr     :-  DL assigment data to be sent to L1

RETURN VALUE
  dtm_ass_status

SIDE EFFECTS
  None

===========================================================================*/

dtm_ass_status_t gmac_dtm_dl_ass_handler
(
  rr_packet_downlink_assignment_t  *dtm_dl_ass_ptr,
  mac_downlink_tbf_t               *dl_tbf_ptr,
  gas_id_t                          gas_id
)
{
  dtm_ass_status_t                 dtm_ass_status = NO_DTM_ASS_ERROR;
  uint32                           rrps_tlli = 0;
  uint16                           assigned_dl_ws =0;

  dwnlk_pwr_ctrl_t                 *assigned_dl_pwr_ctrl;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  assigned_dl_pwr_ctrl = &(dtm_dl_ass_ptr->dwnlk_pwr_ctrl);



  /******************************************************************************
  *                  Populate MR_DL_ASS_IND msg                                 *
  ******************************************************************************/
  /*Store tbf mode and egprs window size*/
  if(dtm_dl_ass_ptr->egprs_tbf_mode_flag == valid)
  {
    global_data_ptr->current_tbf_mode = TBF_MODE_EGPRS;

    /*Check validity of DL ws*/
    assigned_dl_ws = gmac_egprs_decode_dl_window_size(
                     dtm_dl_ass_ptr->egprs_win_sz,
                     dtm_dl_ass_ptr->timeslot_allocation,
                     gas_id);

    /*Save DL window size, NOTE when the ass has bee rcvd in NULL there */
    /*is no previous DL window size to take into consideration */
    if (assigned_dl_ws != WS_ALLOCATION_INVALID)
    {
       global_data_ptr->current_egprs_dl_ws = assigned_dl_ws;
    }
    else
    {
      return(DTM_ASS_REJ_PROTOCOL_ERR);
    }
  }
  else
  {
    global_data_ptr->current_tbf_mode = TBF_MODE_GPRS;
  }

  /*store RLC mode*/
  global_data_ptr->gmac_store_dl_rlc_mode = dtm_dl_ass_ptr->rlc_mode;

  /*get TLLI */
  rr_read_public_store(RRPS_TLLI,&rrps_tlli,gas_id);
  global_data_ptr->gmac_store_dl_ass_tlli = rrps_tlli;
  MSG_GERAN_HIGH_3_G("MAC gmac_store_ul_est_tlli 0x%x, gmac_store_dl_ass_tlli 0x%x  PS TLLI 0x%x",
                     global_data_ptr->gmac_store_ul_est_tlli,global_data_ptr->gmac_store_dl_ass_tlli,rrps_tlli);

  /*******************************************************************************
  *             Populate DL_TBF struct of  MAC_L1_DTM_CHANNEL_ASSIGN_REQ         *
  *******************************************************************************/


  /*ignore the MAC_MODE in DL_TBF struct for rel6 EDA, ensure DL mac_mode is
    the same ul mac_mode for legacy reasons*/
  dl_tbf_ptr->mac_mode = global_data_ptr->gmac_current_mac_mode;
  dl_tbf_ptr->ts_allocation = dtm_dl_ass_ptr->timeslot_allocation;

  /*  DL power control data                    */
  if(dtm_dl_ass_ptr->dwnlk_pwr_ctrl_flag == valid)
  {
    dl_tbf_ptr->downlink_pwr.params_valid = TRUE;
    dl_tbf_ptr->downlink_pwr.po = assigned_dl_pwr_ctrl->po;
    dl_tbf_ptr->downlink_pwr.bts_pwr_ctrl_mode = assigned_dl_pwr_ctrl->bts_pwr_ctrl_mode;
    dl_tbf_ptr->downlink_pwr.pr_mode = assigned_dl_pwr_ctrl->pr_mode;
  }
  else
  {
    dl_tbf_ptr->downlink_pwr.params_valid = FALSE;
  }

  /*  starting time N/A for DTM          */
  dl_tbf_ptr->tbf_start_time_valid = FALSE;
  dl_tbf_ptr->tbf_start_time = 0;

  /*******************************************************************************
  *    TA data N/A for DTM. L1 gets this info via CS connection                  *
  * NOTE: Any TA paramaters rcvd viA PUA,PDA,PTR or PPCTA while in DTM mode will *
  * be ignored by L1. If rcvd during TRANSFER MAC is to act as normal and send L1*
  * the TA info, L1 will ignore it                                               *
  *******************************************************************************/
  dl_tbf_ptr->ta_params.ta_value_ie.present = FALSE;
  dl_tbf_ptr->ta_params.cont_ta_ie.present = FALSE;

  global_data_ptr->gmac_local_l1_ta_params.valid_options = no_timing_params_valid;


  /*  power control data                           */
  if(dtm_dl_ass_ptr->pwr_ctrl_paramtrs_flag == valid)
  {
    dl_tbf_ptr->power_control.present = TRUE;
    gmac_power_control_params_ie(&(dtm_dl_ass_ptr->pwr_ctrl_paramtrs),
                                 &(dl_tbf_ptr->power_control),
                                 gas_id);
  }
  else
  {
    dl_tbf_ptr->power_control.present = FALSE;
  }

  if(global_data_ptr->mac_mode == NULL_MODE)
  {
    if(dtm_dl_ass_ptr->downlink_tfi_assignment_flag == valid)
    {
      dl_tbf_ptr->dl_tfi_present = TRUE;
      dl_tbf_ptr->dl_tfi = dtm_dl_ass_ptr->downlink_tfi_assignment;

      global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = TRUE;
      global_data_ptr->gmac_local_tfi_store.dl_tfi = dtm_dl_ass_ptr->downlink_tfi_assignment;
    }
    else
    {
      return DTM_ASS_REJ_PROTOCOL_ERR;
    }
  }
  else if(global_data_ptr->mac_mode == TRANSFER_MODE)
  {
    if ( dtm_dl_ass_ptr->downlink_tfi_assignment_flag == valid )
    {
      global_data_ptr->gmac_re_assign_dl_tfi = dtm_dl_ass_ptr->downlink_tfi_assignment;
      dl_tbf_ptr->dl_tfi_present = TRUE;
      dl_tbf_ptr->dl_tfi = dtm_dl_ass_ptr->downlink_tfi_assignment;
    }
    else
    {
      global_data_ptr->gmac_re_assign_dl_tfi = global_data_ptr->gmac_local_tfi_store.dl_tfi;
      dl_tbf_ptr->dl_tfi_present = FALSE;
    }
  }
  else
  {
    /*Should never be in IDLE_MODE*/
    MSG_GERAN_ERROR_1_G("GMAC DTM Chan Ass Handler called in wrong mac_mode: %d",global_data_ptr->mac_mode);
    return DTM_ASS_REJ_PROTOCOL_ERR;
  }

  /*NOTE in rel 6 the MEASUREMENT IE's in RR PACKET DOWNLINK ASSIGNMENT have     */
  /*     been removed                                                            */


  /*  EGPRS                                     */
  if(dtm_dl_ass_ptr->egprs_tbf_mode_flag == valid)
  {
    /* BEP not part of RR PACKET DOWNLINK ASSIGNMENT */
    dl_tbf_ptr->bep_period2.present = FALSE;
    dl_tbf_ptr->link_qual_meas_mode = dtm_dl_ass_ptr->link_quality_measurement_mode;
    dl_tbf_ptr->tbf_mode = TBF_MODE_EGPRS;
    dl_tbf_ptr->rlc_mode = (mac_rlc_mode_T)dtm_dl_ass_ptr->rlc_mode;
  }
  else
  {
    dl_tbf_ptr->tbf_mode = TBF_MODE_GPRS;
    dl_tbf_ptr->rlc_mode = RLC_MODE_INVALID;
  }

  /* note Packet Extended Timing Advance N/A not supported by MS */
  if(dtm_dl_ass_ptr->packet_ext_timing_adv_flag == valid)
  {
    MSG_GERAN_HIGH_0_G("MAC info rcvd Packet Extended Timing Advance");
  }


 return (dtm_ass_status);
}

/*===========================================================================

FUNCTION gmac_dtm_freq_handling

DESCRIPTION
 Takes the frequency assigment rcvd from GRR and constructs the
 mac_l1_freq_config signal

PARAMS
 freq_info_present :- is frequency data present
 dtm_freq_ass_ptr  :- freq info rcvd form GRR
 freq_config_ptr   :- freq config struct for l1
 tbf_type          :- tbf being establsihed (ul,dl or both)
RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/

void gmac_dtm_freq_handling
(
   boolean                       freq_info_present,
   frequency_information_T       *dtm_freq_ass_ptr,  /*DTM freq rcvd from GRR   */
   mac_l1_freq_config_t          *freq_config_ptr,   /*freq config struct for l1*/
   dtm_tbf_type_t                dtm_tbf_type,
   gas_id_t                      gas_id
)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  memset(freq_config_ptr,0,sizeof(mac_l1_freq_config_t));

  if(freq_info_present == TRUE)
  {

    /*            Store global freq params for use in transfer                */
    global_data_ptr->gmac_store_freq_params_in_use.tsc = dtm_freq_ass_ptr->tsc;
    global_data_ptr->gmac_store_freq_params_in_use.hopping_flag = dtm_freq_ass_ptr->hopping_flag;
    global_data_ptr->gmac_store_freq_params_in_use.maio = dtm_freq_ass_ptr->maio;
    global_data_ptr->gmac_store_freq_params_in_use.hsn = dtm_freq_ass_ptr->hsn;

    global_data_ptr->gmac_store_freq_params_in_use.frequency_list = dtm_freq_ass_ptr->frequency_list;

    /*    Load MAC_L1_FREQ_CONFIG with frequency information                  */
    freq_config_ptr->reuse_sb_frq_alloc = FALSE;
    freq_config_ptr->dtm_freq_param_present = TRUE;

    freq_config_ptr->frequency_information = *dtm_freq_ass_ptr;
  }
  else
  {
    freq_config_ptr->dtm_freq_param_present = FALSE;
  }

  /*MAP */
  if(dtm_tbf_type == UL_TBF_PRESENT)
  {
    freq_config_ptr->tbf_freq_cnf = UL_FREQ_CONFIG;
  }
  else if (dtm_tbf_type == DL_TBF_PRESENT)
  {
    freq_config_ptr->tbf_freq_cnf = DL_FREQ_CONFIG;
  }
  else if (dtm_tbf_type == UL_DL_TBF_PRESENT)
  {
    freq_config_ptr->tbf_freq_cnf = UL_DL_FREQ_CONFIG;
  }
  else
  {
     /*NO_TBF_PRESENT*/
  }

}


/*===========================================================================

FUNCTION gmac_dtm_check_for_egprs_ass_clash

DESCRIPTION
  Checks for an EGPRS assignment clash in UL and DL DTM assignments.

PARAMS
  dtm_ass_ptr - DTM Assigment.

RETURN VALUE
  dtm_ass_status_t - DTM Assignment Status.  Returns DTM_ASS_REJ_PROTOCOL_ERR if
                     there is an EGPRS clash.

SIDE EFFECTS
  None

===========================================================================*/

dtm_ass_status_t gmac_dtm_check_for_egprs_ass_clash(
                   gm_dtm_channel_assign_req_t *dtm_ass_ptr,
                   gas_id_t                     gas_id )
{
  dtm_ass_status_t dtm_ass_status = NO_DTM_ASS_ERROR;

  if ( dtm_ass_ptr->dtm_tbf_type == UL_DL_TBF_PRESENT )
  {
    if ( dtm_ass_ptr->gm_dtm_ul_tbf_ass.egprs_flag !=
         dtm_ass_ptr->gm_dtm_dl_tbf_ass.egprs_tbf_mode_flag )
    {
      MSG_GERAN_HIGH_2_G("GMAC DTM, EGPRS ass clash in msg, UL EGPRS ASS: %d, DL EGPRS ASS: %d",
                         dtm_ass_ptr->gm_dtm_ul_tbf_ass.egprs_flag,dtm_ass_ptr->gm_dtm_dl_tbf_ass.egprs_tbf_mode_flag);
      dtm_ass_status = DTM_ASS_REJ_PROTOCOL_ERR;
    }
  }

  return dtm_ass_status;
}

#endif /* FEATURE_GSM_DTM */

/*=============================================================================

FUNCTION gmac_send_mac_l1_sb_config()

DESCRIPTION
   Sends MAC_L1_SINGLE_BLOCK_CONFIG to L1
PARAMS
   sig_data_ptr

RETURN VALUE

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_l1_sb_config(mac_l1_single_block_config_t *sig_data_ptr,
                                gas_id_t                      gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  IMH_T  *l1_sig_message_header_ptr;

  l1_sig_message_header_ptr = &(global_data_ptr->l1_sig.mac_l1_single_block_config.message_header);

  global_data_ptr->l1_sig.mac_l1_single_block_config = *sig_data_ptr;

  l1_sig_message_header_ptr->message_set = MS_MAC_L1;
  l1_sig_message_header_ptr->message_id = (uint8)MAC_L1_SINGLE_BLOCK_CONFIG;
  gprs_l1_mac_acc_func(&global_data_ptr->l1_sig,gas_id);

}

/*=============================================================================

FUNCTION gmac_send_mac_l1_dyn_config()

DESCRIPTION
   Sends MAC_L1_DYNAMIC_TBF_CONFIG to L1
PARAMS
   sig_data_ptr

RETURN VALUE

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_l1_dyn_config(mac_l1_dynamic_tbf_config_t *sig_data_ptr,
                                 gas_id_t                     gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  IMH_T  *l1_sig_message_header_ptr;

  l1_sig_message_header_ptr = &(global_data_ptr->l1_sig.mac_l1_dynamic_tbf_config.message_header);

  global_data_ptr->l1_sig.mac_l1_dynamic_tbf_config = *sig_data_ptr;

  l1_sig_message_header_ptr->message_set = MS_MAC_L1;
  l1_sig_message_header_ptr->message_id = (uint8)MAC_L1_DYNAMIC_TBF_CONFIG;
  gprs_l1_mac_acc_func(&global_data_ptr->l1_sig, gas_id);

}


/*===========================================================================

FUNCTION gmac_gprs_check_frequency_params

DESCRIPTION
  This function checks for invalid frequencies not supprted by the hardware.
DEPENDENCIES
  None

RETURN VALUE
  TRUE   : if an ivalid frequency has been detected.
  FALSE  : if all frequencies are valid.

SIDE EFFECTS
  None

===========================================================================*/

boolean gmac_gprs_check_frequency_params(
                                        frequency_list_T *frequency_list_ptr,
                                        boolean hopping_flag,
                                        gas_id_t gas_id
                                        )
{

  uint8 ma_list_length,i;
  ARFCN_T assigned_arfcn;
  boolean error;

  ARFCN_T check_arfcn; /*Used for band checking the frequency list*/

  /* if Frequency Hopping is not used */
  if (!hopping_flag)
  {
    assigned_arfcn = frequency_list_ptr->channel[0];

/***********************************************************************************
*  Note this def was modified from: #ifdef FEATURE_GSM_GPRS_MULTIBAND              *
*    because the definition of GMAC_BASE_ARFCN at the top of this file is               *
*    conditional on FEATURE_GSM_MULTIBAND being defined.  If FEATURE_GSM_MULTIBAND *
*    only is defined it causes a compilation error.  Note that this is currently   *
*    a shortfall in l1i.h where it was copied from.  The reason it is modified     *
*    here is that the PERL UTF host environment won't build with                   *
*    FEATURE_GSM_GPRS_MULTIBAND defined                                            *
***********************************************************************************/

    /* Check that ARFCN is a valid for EGSM or DCS bands
     * i.e. is within 0 -> 124 975 -> 1023 for EGSM
     * or within 512 -> 885 for DCS
     */
    if ( (assigned_arfcn.num <= GMAC_EGSM_MAX_ARFCN_LOWER) ||
         ((assigned_arfcn.num >= GMAC_EGSM_MIN_ARFCN_UPPER)  && (assigned_arfcn.num <= GMAC_EGSM_MAX_ARFCN_UPPER)) ||
         ((assigned_arfcn.num >= GMAC_DCS_BASE_ARFCN) && (assigned_arfcn.num < (GMAC_DCS_BASE_ARFCN + GMAC_DCS_MAXNO_ARFCNS)))
         ||
         ((assigned_arfcn.num >= GMAC_PCS_BASE_ARFCN) && (assigned_arfcn.num < (GMAC_PCS_BASE_ARFCN + GMAC_PCS_MAXNO_ARFCNS))) ||
         ((assigned_arfcn.num >= GMAC_GSM_850_BASE_ARFCN) && (assigned_arfcn.num < (GMAC_GSM_850_BASE_ARFCN + GMAC_GSM_850_MAXNO_ARFCNS)))
       )
    {
      /* Frequency is within range therefor no error */
      error = FALSE;
      MSG_GERAN_LOW_1_G("freq [%d]",assigned_arfcn.num);
    } else
    {
      /* Frequency is not in range therefore report error */
      error = TRUE;
    }

  } else
  {

    /*
   * Check for invalid Hopping frequencies( i.e. those not in supported rf band) in MA list
   */
    /* intialise while vars */
    i=0;
    error=FALSE;

    /* Check frequencies against the first in the list as they all have to be the same */
    check_arfcn = frequency_list_ptr->channel[0];

    /* Setup the length of the MA list*/
    ma_list_length = frequency_list_ptr->no_of_items;
    while (error == FALSE)
    {
      assigned_arfcn = frequency_list_ptr->channel[i];

/***********************************************************************************
*  Note this def was modified from: #ifdef FEATURE_GSM_GPRS_MULTIBAND              *
*    because the definition of GMAC_BASE_ARFCN at the top of this file is               *
*    conditional on FEATURE_GSM_MULTIBAND being defined.  If FEATURE_GSM_MULTIBAND *
*    only is defined it causes a compilation error.  Note that this is currently   *
*    a shortfall in l1i.h where it was copied from.  The reason it is modified     *
*    here is that the PERL UTF host environment won't build with                   *
*    FEATURE_GSM_GPRS_MULTIBAND defined                                            *
***********************************************************************************/

      /* Check that ARFCN is a valid for EGSM or DCS bands
        * i.e. is within 0 -> 124 975 -> 1023 for EGSM
        * or within 512 -> 885 for DCS
      */
      if ( (assigned_arfcn.num <= GMAC_EGSM_MAX_ARFCN_LOWER) ||
           ((assigned_arfcn.num >= GMAC_EGSM_MIN_ARFCN_UPPER)  && (assigned_arfcn.num <= GMAC_EGSM_MAX_ARFCN_UPPER)) ||
           ((assigned_arfcn.num >= GMAC_DCS_BASE_ARFCN) && (assigned_arfcn.num < (GMAC_DCS_BASE_ARFCN + GMAC_DCS_MAXNO_ARFCNS)))
           ||
           ((assigned_arfcn.num >= GMAC_PCS_BASE_ARFCN) && (assigned_arfcn.num < (GMAC_PCS_BASE_ARFCN + GMAC_PCS_MAXNO_ARFCNS))) ||
           ((assigned_arfcn.num >= GMAC_GSM_850_BASE_ARFCN) && (assigned_arfcn.num < (GMAC_GSM_850_BASE_ARFCN + GMAC_GSM_850_MAXNO_ARFCNS)))
         )
      {
        /* Frequency is within range therefor no error */
        error = FALSE;
        MSG_GERAN_LOW_1_G("freq [%d]",assigned_arfcn.num);
      } else
      {
        /* Frequency is not in range therefore report error */
        error = TRUE;
      }

      /* Check if the assignment has ARFCN's in more than one Band */
      if (assigned_arfcn.band != check_arfcn.band)
      {
        error = TRUE;
        MSG_GERAN_ERROR_2_G("GMAC Freq Err Diff Bands in Assignment Check ARFCN %d, Band %d",check_arfcn.num,check_arfcn.band);
        MSG_GERAN_ERROR_2_G("GMAC Freq Err Diff Bands in Assignment Current ARFCN %d, Band %d",assigned_arfcn.num,assigned_arfcn.band);
      }

      /* Guard against case were all frequencies are not supported */
      if (assigned_arfcn.band == SYS_BAND_NONE)
      {
        error = TRUE;
        MSG_GERAN_ERROR_1_G("GMAC Freq Err ARFCN %d not supported",assigned_arfcn.num);
      }

      /* increment to next frequency in hopping list */
      i++;

      /* If we have reached the end of the frequency list then stop */
      if (i >= ma_list_length)
        break;
    } /* End of While */


  } /* end of else */

  return(error);
}
/*===========================================================================

FUNCTION gmac_send_mac_l1_dl_config

DESCRIPTION
  Sends mac_l1_dl_config

PARAMS
  l1_dl_tbf_cnf_ptr - DL Assignment parameters for L1.

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/

void gmac_send_mac_l1_dl_config(mac_l1_dl_tbf_config_t *l1_dl_tbf_cnf_ptr,
                                gas_id_t                gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  IMH_T  *l1_sig_message_header_ptr;

  MSG_GERAN_MED_0_G( "MAC sent MAC_L1_DL_TBF_CONFIG");

  l1_sig_message_header_ptr = &(global_data_ptr->l1_sig.mac_l1_dl_tbf_config.message_header);
  global_data_ptr->l1_sig.mac_l1_dl_tbf_config = *l1_dl_tbf_cnf_ptr;

  l1_sig_message_header_ptr->message_set = MS_MAC_L1;
  l1_sig_message_header_ptr->message_id = (uint8)MAC_L1_DL_TBF_CONFIG;
  gprs_l1_mac_acc_func(&global_data_ptr->l1_sig, gas_id);

}

#ifdef FEATURE_GSM_EDTM
/*===========================================================================
===
===  FUNCTION    gmac_transfer_send_edtm_pcr_on_pacch_rrbp()
===
===  DESCRIPTION
===
===  Function used by transfer to send a PACKET_CS_REQUEST message
===  during an active downlink tbf.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_edtm_pcr_on_pacch_rrbp(uint8 ts, uint8 rrbp, uint32 fn, gas_id_t gas_id)
{
  mac_l1_sig_t        ml_rrbp_sig;

  mac_l1_pacch_rrbp_t *rrbp_sig_ptr = &(ml_rrbp_sig.mac_l1_pacch_rrbp);

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_PACCH_RRBP with EDTM PKT CS REQ");

  rrbp_sig_ptr->rrbp_msg = PCCF_MSG;
  rrbp_sig_ptr->ts_of_dl_msg = ts;
  rrbp_sig_ptr->absolute_fn = gmac_rrbp_absolute_fn(fn, rrbp, gas_id);
  rrbp_sig_ptr->msg_burst_type = NB;
  rrbp_sig_ptr->message_type = PACKET_CS_REQUEST;

  /* copy the CSN.1 encoded PCCN into the PACCH_RRBP message */
  memscpy(rrbp_sig_ptr->pacch_ab_data,sizeof(rrbp_sig_ptr->pacch_ab_data),
          global_data_ptr->gmac_local_edtm_pcr_params.csn_encoded_edtm_pcr,
          SIZE_OF_CONTROL_RADIO_BLOCK);


  ml_rrbp_sig.mac_ph_data_req.message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
  ml_rrbp_sig.mac_ph_data_req.message_header.message_set = MS_MAC_L1;
  gprs_l1_mac_acc_func(&ml_rrbp_sig, gas_id);

  global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_pending = FALSE;

  /* call log-on-demand function */
  (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                   PACKET_CS_REQUEST,
                                   (byte)SIZE_OF_CONTROL_RADIO_BLOCK,
                                   &(rrbp_sig_ptr->pacch_ab_data[1]),
                                   gas_id);

}

  /*===========================================================================
===
===  FUNCTION      gmac_util_check_pkt_cs_cmd_addressing()
===
===  DESCRIPTION
===
===  Checks that the Packet CS Command addresses the mobile.
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_util_check_pkt_cs_cmd_addressing(gas_id_t gas_id)
{
  boolean addr_match = FALSE;

  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;

  packet_cs_command_t *pkt_cs_cmd_ptr;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  pkt_cs_cmd_ptr = (packet_cs_command_t *)global_data_ptr->csn_dec_data_ptr;

  /* ADDRESS GLOBAL TFI, TLLI */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {
    mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);

    if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
    {
      addr_match = gmac_util_global_tfi(pkt_cs_cmd_ptr->global_tfi, gas_id);
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
    {
      addr_match = TRUE;
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
    {
      addr_match = FALSE;
    }
  }

  return(addr_match);
}


/*=============================================================================

FUNCTION gmac_send_mac_grr_edtm_pkt_cs_est_rej()

DESCRIPTION
  Constructs and sends MAC_GRR_EDTM_PKT_CS_EST_REJ to GRR.

PARAMS
  cs_rej_cause

RETURN VALUE
  void

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_grr_edtm_pkt_cs_est_rej(mac_grr_edtm_cs_rej_cause_t cs_rej_cause,
                                           gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_1_G("MAC Sent MAC_GRR_EDTM_PKT_CS_EST_REJ cs_rej_cause %d",cs_rej_cause);

  global_data_ptr->grr_sig.mac_grr_edtm_pkt_cs_est_rej.reject_cause = cs_rej_cause;
  global_data_ptr->grr_sig.mac_grr_edtm_pkt_cs_est_rej.message_header.message_set = MS_MAC_RR;
  global_data_ptr->grr_sig.mac_grr_edtm_pkt_cs_est_rej.message_header.message_id = (uint8)MAC_GRR_EDTM_PKT_CS_EST_REJ;
  gprs_grr_mac_acc_func(&global_data_ptr->grr_sig, gas_id);
}

/*=============================================================================

FUNCTION gmac_send_mac_grr_edtm_cs_release_ind()

DESCRIPTION
  Constructs and sends MAC_GRR_EDTM_CS_RELEASE_IND to GRR.

PARAMS
  cs_rej_cause

RETURN VALUE
  void

SIDE EFFECTS
=============================================================================*/

void gmac_send_mac_grr_edtm_cs_release_ind(mac_grr_edtm_cs_rel_cause_t cs_rel_cause,
                                           gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_1_G("MAC Sent MAC_GRR_EDTM_CS_RELEASE_IND cs_rel_cause: 0x%x",cs_rel_cause);

  global_data_ptr->grr_sig.mac_grr_edtm_cs_release_ind.release_cause = cs_rel_cause;
  global_data_ptr->grr_sig.mac_grr_edtm_cs_release_ind.message_header.message_set = MS_MAC_RR;
  global_data_ptr->grr_sig.mac_grr_edtm_cs_release_ind.message_header.message_id = (uint8)MAC_GRR_EDTM_CS_RELEASE_IND;
  gprs_grr_mac_acc_func(&global_data_ptr->grr_sig, gas_id);
}

/*=============================================================================

FUNCTION gmac_send_mac_l1_delete_ul_ctrl_msg()

DESCRIPTION
  Constructs and sends MAC_L1_DELETE_UL_CTRL_MSG to GRR.

PARAMS
  msg_type

RETURN VALUE
  void

SIDE EFFECTS
=============================================================================*/
void gmac_send_mac_l1_delete_ul_ctrl_msg(uint8 msg_type,
                                         gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  IMH_T  *l1_sig_message_header_ptr;

  MSG_GERAN_MED_1_G( "MAC sent MAC_L1_DELETE_UL_CTRL_MSG, msg_type: 0x%x",msg_type);

  global_data_ptr->l1_sig.mac_l1_delete_ul_ctrl_msg.message_type = msg_type;

  l1_sig_message_header_ptr = &(global_data_ptr->l1_sig.mac_l1_delete_ul_ctrl_msg.message_header);
  l1_sig_message_header_ptr->message_set = MS_MAC_L1;
  l1_sig_message_header_ptr->message_id = (uint8)MAC_L1_DELETE_UL_CTRL_MSG;

  gprs_l1_mac_acc_func(&global_data_ptr->l1_sig, gas_id);
}


#endif /*FEATURE_GSM_EDTM*/
/*===========================================================================
===
===  FUNCTION    gmac_util_encode_ul_control_msg_handler()
===
===  DESCRIPTION
===  This function encodes the different uplink control messages
===  and puts the encoded message into a temporary storage.
===  This function returns the success or failure of the csn encoding
===
===  DEPENDENCIES
===  Depending upon the following global structures
===  gmac_local_pccf_params, gmac_local_pss_params,
===  gmac_local_pmr_params, gmac_local_pccn_params
===
===  RETURN VALUE
===  uint8 returns FAIL or PASS
===  SIDE EFFECTS
===
===
===========================================================================*/

uint8 gmac_util_encode_ul_control_msg_handler(ul_chan_msg_t msg_type,
                                              boolean dl_only_present,
                                              gas_id_t gas_id)
{
  int_data_t          input_data;
  uint8               csn_encoding = FAIL;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  switch(msg_type)
  {
    case PCCF:

           input_data.type.pkt_cell_chg_failure = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pccf.pccf;

           csn_encoding = gcsn1_encoder(PACKET_CELL_CHANGE_FAILURE, &(global_data_ptr->gmac_local_pccf_params.csn_encoded_pccf[1]), &input_data);

           if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
           {
                global_data_ptr->gmac_local_pccf_params.csn_encoded_pccf[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
           }
           else
           {
                global_data_ptr->gmac_local_pccf_params.csn_encoded_pccf[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
           }
       break;

    case PSS:
        if(dl_only_present)
        {
            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pss_req.pss_msg.global_tfi.direction = dwnlink_valid;
            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pss_req.pss_msg.global_tfi.dwnlink_tfi = global_data_ptr->gmac_local_tfi_store.dl_tfi;
        }
        else
        {
            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pss_req.pss_msg.global_tfi.direction = uplink_valid;
            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pss_req.pss_msg.global_tfi.uplink_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
        }

       input_data.type.pkt_si_status = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pss_req.pss_msg;

       csn_encoding = gcsn1_encoder(PACKET_SI_STATUS, &(global_data_ptr->gmac_local_pss_params.csn_encoded_pss[1]), &input_data);


       if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
       {
            global_data_ptr->gmac_local_pss_params.csn_encoded_pss[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
       }
       else
       {
            global_data_ptr->gmac_local_pss_params.csn_encoded_pss[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
       }

       break;

    case PMR:
          /* clear local pmr data */
        memset(&global_data_ptr->gmac_local_pmr_params, 0, sizeof(gmac_pmr_params_t));

          /* copy pmr params from grr msg */
         global_data_ptr->gmac_local_pmr_params.nco = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_meas_rpt.nco;
         global_data_ptr->gmac_local_pmr_params.nc_non_drx_period = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_meas_rpt.nc_non_drx_period;

        if (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_meas_rpt.msg_type == REPORT_TYPE_PMR)
        {
            global_data_ptr->gmac_local_pmr_params.msg_type_is_pmr = TRUE;

            /* load pmr struct in to csn buffer ready for encoding */
            global_data_ptr->csn_encoder_ul_msg.type.pkt_meas_report = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_meas_rpt.msg.pmr;

            if (gcsn1_encoder(PACKET_MEASUREMENT_REPORT,
                              &(global_data_ptr->gmac_local_pmr_params.csn_encoded_pmr[1]),
                              &(global_data_ptr->csn_encoder_ul_msg)) == PASS)
            {
              csn_encoding = PASS;
            }
            else
            {
                MSG_GERAN_MED_0_G( "MAC sent MAC_GRR_MEAS_RPT_CNF ");
                     gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
            }
        }
        else if (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_meas_rpt.msg_type == REPORT_TYPE_PEMR)
        {
            global_data_ptr->gmac_local_pmr_params.msg_type_is_pmr = FALSE;

            /* load pmr struct in to csn buffer ready for encoding */
            global_data_ptr->csn_encoder_ul_msg.type.pkt_enhanced_measurement_report = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_meas_rpt.msg.pemr;

            if (gcsn1_encoder(PACKET_ENHANCED_MEASUREMENT_REPORT,
                              &(global_data_ptr->gmac_local_pmr_params.csn_encoded_pmr[1]),
                              &(global_data_ptr->csn_encoder_ul_msg)) == PASS)
            {
              csn_encoding = PASS;
            }
        }
        else
        {
            MSG_GERAN_ERROR_1_G("Unknown report type: %d", global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_meas_rpt.msg_type);
        }

          /***********************************************************
          *  CSN.1 encode PMR and store result into csn_ecoded_pmr   *
          *  ready for use when MAC_PH_DATA_REQ is sent to L1        *
          ***********************************************************/
        if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
        {
              global_data_ptr->gmac_local_pmr_params.csn_encoded_pmr[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
        }
        else
        {
              global_data_ptr->gmac_local_pmr_params.csn_encoded_pmr[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
        }
        break;
    case PCCN:

          /*ADD the global TFI*/
        if(dl_only_present)
        {
            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pccn_req.pccn_msg.global_tfi.direction = dwnlink_valid;
            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pccn_req.pccn_msg.global_tfi.dwnlink_tfi = global_data_ptr->gmac_local_tfi_store.dl_tfi;
        }
        else
        {
            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pccn_req.pccn_msg.global_tfi.direction = uplink_valid;
            global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pccn_req.pccn_msg.global_tfi.uplink_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
        }


        input_data.type.pkt_cell_change_notification = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_send_pccn_req.pccn_msg;

        csn_encoding = gcsn1_encoder(PACKET_CELL_CHANGE_NOTIFICATION, &(global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[1]), &input_data);

        if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
        {
            global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
        }
        else
        {
            global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
        }
        break;
#ifdef FEATURE_GSM_EDTM
    case PCR:
      input_data.type.pkt_cs_request.establishment_cause = global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause;
      /*ADD the global TFI*/
      if(dl_only_present)
      {
            input_data.type.pkt_cs_request.global_tfi.direction = dwnlink_valid;
            input_data.type.pkt_cs_request.global_tfi.dwnlink_tfi = global_data_ptr->gmac_local_tfi_store.dl_tfi;
      }
      else
      {
           input_data.type.pkt_cs_request.global_tfi.direction = uplink_valid;
           input_data.type.pkt_cs_request.global_tfi.uplink_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
      }

      csn_encoding = gcsn1_encoder(PACKET_CS_REQUEST,
                                      &(global_data_ptr->gmac_local_edtm_pcr_params.csn_encoded_edtm_pcr[1]), &input_data);


      if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
      {
            global_data_ptr->gmac_local_edtm_pcr_params.csn_encoded_edtm_pcr[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
      }
      else
      {
            global_data_ptr->gmac_local_edtm_pcr_params.csn_encoded_edtm_pcr[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
      }
      break;
#endif
    default:
    break;

    }

  return(csn_encoding);
}

/*===========================================================================
===
===  FUNCTION    gmac_util_send_ul_control_msg_handler()
===
===  DESCRIPTION
===  This function sends the encoded uplink message to layer1
===  This function also switches PCCN message from
===  uplink to downlink and vice versa
===
===  DEPENDENCIES
===  Depending upon the following global structures
===  gmac_local_pccf_params, gmac_local_pss_params,
===  gmac_local_pmr_params, gmac_local_pccn_params
===
===  RETURN VALUE
===  void
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_send_ul_control_msg_handler(ul_chan_msg_t msg_type,
                                           existing_tbf_type_t existing_tbf_type,
                                           gmac_grr_cnf_needed_t grr_cnf_needed,
                                           gas_id_t gas_id)
{
  mac_l1_sig_t        ml_ph_data_sig;
  mac_grr_sig_t       mac_grr_sig;
  boolean         send_ph_data_req = FALSE;
  boolean         send_pccf_cnf = FALSE;
  boolean         send_pmr_cnf = FALSE;
#ifdef FEATURE_GSM_EDTM
  uint8           ph_data_req_msg_type = 0;
#endif /*FEATURE_GSM_EDTM*/

  transfer_flags_t     transfer_flags;
  mac_ph_data_req_t   *data_req_msg = &(ml_ph_data_sig.mac_ph_data_req);
  gmac_global_data_t  *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  switch(msg_type)
    {
    case PCCF:

#ifdef FEATURE_GSM_EDTM
        ph_data_req_msg_type = PACKET_CELL_CHANGE_FAILURE;
#endif /*FEATURE_GSM_EDTM*/

        if (existing_tbf_type == UL_PRESENT)
        {
            memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),
                    global_data_ptr->gmac_local_pccf_params.csn_encoded_pccf, SIZE_OF_CONTROL_RADIO_BLOCK);

            send_ph_data_req = TRUE;
            global_data_ptr->gmac_local_pccf_params.pccf_pending = FALSE;
            /* call log-on-demand function */
              (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                       PACKET_CELL_CHANGE_FAILURE,
                                       SIZE_OF_CONTROL_RADIO_BLOCK,
                                       &(global_data_ptr->gmac_local_pccf_params.csn_encoded_pccf[1]),
                                         gas_id);
            if(grr_cnf_needed == GRR_CNF_NEEDED)
            {
              send_pccf_cnf = TRUE;
            }
        }
        else
        {
            global_data_ptr->gmac_local_pccf_params.pccf_pending= TRUE;
#ifdef FEATURE_GPRS_PS_HANDOVER
            global_data_ptr->gmac_local_pccf_params.pccf_cnf_needed = grr_cnf_needed;
#endif /*FEATURE_GPRS_PS_HANDOVER*/
        }

        break;

    case PSS:

#ifdef FEATURE_GSM_EDTM
        ph_data_req_msg_type = PACKET_SI_STATUS;
#endif /*FEATURE_GSM_EDTM*/

        if (existing_tbf_type == UL_PRESENT)
        {
            memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),
                    global_data_ptr->gmac_local_pss_params.csn_encoded_pss, SIZE_OF_CONTROL_RADIO_BLOCK);

            send_ph_data_req = TRUE;
            global_data_ptr->gmac_local_pss_params.pss_pending = FALSE;
            /* call log-on-demand function */
              (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                       PACKET_SI_STATUS,
                                       SIZE_OF_CONTROL_RADIO_BLOCK,
                                       &(global_data_ptr->gmac_local_pss_params.csn_encoded_pss[1]),
                                         gas_id);
        }
        else
        {
            global_data_ptr->gmac_local_pss_params.pss_pending = TRUE;
        }

        break;

    case PMR:

#ifdef FEATURE_GSM_EDTM
        if(global_data_ptr->gmac_local_pmr_params.msg_type_is_pmr == TRUE)
        {
          ph_data_req_msg_type = PACKET_MEASUREMENT_REPORT;
        }
        else
        {
          ph_data_req_msg_type = PACKET_ENHANCED_MEASUREMENT_REPORT;
        }
#endif /*FEATURE_GSM_EDTM*/

        if(global_data_ptr->gmac_local_pmr_params.pmr_pending == TRUE)
        {
            memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),
                    global_data_ptr->gmac_local_pmr_params.csn_encoded_pmr, SIZE_OF_CONTROL_RADIO_BLOCK);

            send_ph_data_req = TRUE;
            global_data_ptr->gmac_local_pmr_params.pmr_pending = FALSE;
            send_pmr_cnf = TRUE;
        }
        else
        {
            if (existing_tbf_type == UL_PRESENT)
            {
                memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),
                        global_data_ptr->gmac_local_pmr_params.csn_encoded_pmr, SIZE_OF_CONTROL_RADIO_BLOCK);

                send_ph_data_req = TRUE;
                global_data_ptr->gmac_local_pmr_params.pmr_pending = FALSE;
                if(global_data_ptr->gmac_local_pmr_params.msg_type_is_pmr == TRUE)
                {
                    /* call log-on-demand function */
                      (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                           PACKET_MEASUREMENT_REPORT,
                                           SIZE_OF_CONTROL_RADIO_BLOCK,
                                           &(global_data_ptr->gmac_local_pmr_params.csn_encoded_pmr[1]),
                                             gas_id);
                }
                else
                {
                    /* call log-on-demand function */
                      (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                           PACKET_ENHANCED_MEASUREMENT_REPORT,
                                           SIZE_OF_CONTROL_RADIO_BLOCK,
                                           &(global_data_ptr->gmac_local_pmr_params.csn_encoded_pmr[1]),
                                             gas_id);
                }

                if(grr_cnf_needed == GRR_CNF_NEEDED)
                {
                  send_pmr_cnf = TRUE;
                }
            }
            else
            {
                global_data_ptr->gmac_local_pmr_params.pmr_pending = TRUE;
            }
        }

        break;
    case PCCN:

#ifdef FEATURE_GSM_EDTM
        ph_data_req_msg_type = PACKET_CELL_CHANGE_NOTIFICATION;
#endif /*FEATURE_GSM_EDTM*/

        if (existing_tbf_type == UL_PRESENT)
        {
            if(global_data_ptr->gmac_local_pccn_params.pccn_pending == PCCN_PENDING_DL)
            {
                /*set ul/dl tfi present to UL present, clear bit 5 of TFI*/
                global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[1] = global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn [1] & 0xfc;
                /*clear bits 0-4 of TFI*/
                global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[2] = global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn [2] & 0x0f;

                /*switch bit 5 of tfi*/
                global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[1] =
                  global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn [1] | ((global_data_ptr->gmac_local_tfi_store.ul_tfi & 0x10) >> 4);
                /*switch bits 4 to 1 of tfi */
                global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[2] =
                  global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn [2] | ((global_data_ptr->gmac_local_tfi_store.ul_tfi & 0x0f) << 4);
            }

            memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),
                    global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn, SIZE_OF_CONTROL_RADIO_BLOCK);

            send_ph_data_req = TRUE;
            global_data_ptr->gmac_local_pccn_params.pccn_pending = PCCN_PENDING_UL;

            /* call log-on-demand function */
            (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                             PACKET_CELL_CHANGE_NOTIFICATION,
                                             SIZE_OF_CONTROL_RADIO_BLOCK,
                                             &(global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[1]),
                                             gas_id);


        }
        else
        {
            if(global_data_ptr->gmac_local_pccn_params.pccn_pending == PCCN_PENDING_UL)
            {
                    /*set ul/dl tfi present to DL present*/
                    global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[1] =
                      global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn [1] | 0x02;

                    /*clear bit 5 of TFI*/
                    global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[1] =
                      global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn [1] & 0xfe;

                    /*clear bits 0-4 of TFI*/
                    global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[2] =
                      global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn [2] & 0x0f;

                    /*switch bit 5 of tfi*/
                    global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[1] =
                      global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn [1] | ((global_data_ptr->gmac_local_tfi_store.dl_tfi & 0x10) >> 4);

                    /*switch bits 4 to 1 of tfi */
                    global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn[2] =
                      global_data_ptr->gmac_local_pccn_params.csn_encoded_pccn [2] | ((global_data_ptr->gmac_local_tfi_store.dl_tfi & 0x0f) << 4);

                    transfer_flags = gmac_tran_get_t_flags(gas_id);

                   if(transfer_flags.gmac_transfer_dl_fai_set == TRUE)
                   {
                       global_data_ptr->gmac_local_pccn_params.pccn_pending = NO_PENDING_PCCN;
                   }
                   else
                   {
                       global_data_ptr->gmac_local_pccn_params.pccn_pending = PCCN_PENDING_DL;
                   }
            }
            else if(global_data_ptr->gmac_local_pccn_params.pccn_pending == NO_PENDING_PCCN)
            {
                global_data_ptr->gmac_local_pccn_params.pccn_pending = PCCN_PENDING_DL;
            }
        }

        break;
#ifdef FEATURE_GSM_EDTM
    case PCR:

        ph_data_req_msg_type = PACKET_CS_REQUEST;

        if (existing_tbf_type == UL_PRESENT)
        {
             MSG_GERAN_MED_1_G("MAC Sent MAC_PH_DATA_REQ with EDTM PACKET CS REQUEST gmac_local_edtm_pcr_params.edtm_pcr_cause: 0x%x",
                               global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause);
             memscpy(data_req_msg->dl_ctrl_block,sizeof(data_req_msg->dl_ctrl_block),
                     global_data_ptr->gmac_local_edtm_pcr_params.csn_encoded_edtm_pcr, SIZE_OF_CONTROL_RADIO_BLOCK);

             send_ph_data_req = TRUE;


             /* call log-on-demand function */
             (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                         PACKET_CHANNEL_REQUEST,
                                         SIZE_OF_CONTROL_RADIO_BLOCK,
                                         &(global_data_ptr->gmac_local_edtm_pcr_params.csn_encoded_edtm_pcr[1]),
                                         gas_id);

              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_pending = FALSE;
        }
        else
        {
              global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_pending = TRUE;
        }
        break;
#endif
     default:
        MSG_GERAN_ERROR_0_G("Received wrong ul msg type to ul msg send hdlr");
        break;
    }
    if(send_pccf_cnf == TRUE)
    {
        mac_grr_sig.mac_grr_pccf_sent.message_header.message_set = MS_MAC_RR;
        mac_grr_sig.mac_grr_pccf_sent.message_header.message_id = (uint8)MAC_GRR_PCCF_SENT;
        gprs_grr_mac_acc_func(&mac_grr_sig,gas_id);
    }
    if(send_pmr_cnf == TRUE)
    {
        mac_grr_sig.mac_grr_meas_rpt_cnf.status = MAC_GRR_SUCCESS;
        mac_grr_sig.mac_grr_meas_rpt_cnf.message_header.message_set = MS_MAC_RR;
        mac_grr_sig.mac_grr_meas_rpt_cnf.message_header.message_id = (uint8)MAC_GRR_MEAS_RPT_CNF;
        gprs_grr_mac_acc_func(&mac_grr_sig,gas_id);
    }
    if(send_ph_data_req == TRUE)
    {
#ifdef FEATURE_GSM_EDTM
        ml_ph_data_sig.mac_ph_data_req.message_type = ph_data_req_msg_type;
#endif /*FEATURE_GSM_EDTM*/
        ml_ph_data_sig.mac_ph_data_req.message_header.message_id = (uint8)MAC_PH_DATA_REQ;
        ml_ph_data_sig.mac_ph_data_req.message_header.message_set = MS_MAC_L1;
        gprs_l1_mac_acc_func(&ml_ph_data_sig,gas_id);
    }



 }
/*===========================================================================
===
===  FUNCTION      gmac_util_check_rlc_blocked_flag
===
===  DESCRIPTION
===  This function checks the current state of the rlc_blocked_flag with
===  the expected state.  MSG_ERROR is called if they do not match.
===
===  PARAMETERS
===  rlc_block_flag_check - expected rlc blocked state.
===
===  DEPENDENCIES
===  None
===
===  RETURN VALUE
===  PASS_RESULT - requested value matches expected value.
===  FAIL_RESULT - requested value doesn't match expected value.
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
gmac_result_t gmac_util_check_rlc_blocked_flag(gmac_rlc_blocked_t expected_rlc_block_flag,
                                               gmac_filenames_t   filename,
                                               uint32             linenumber,
                                               gas_id_t           gas_id)
{
  gmac_result_t rlc_block_flag_check_result = FAIL_RESULT;
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  if ( util_data_ptr->gmac_rlc_blocked_flag == expected_rlc_block_flag )
  {
    rlc_block_flag_check_result = PASS_RESULT;
    MSG_GERAN_MED_3_G("GMAC rlc_blocked_flag check PASS current val: %d, filename: %d, line: %d",
                      util_data_ptr->gmac_rlc_blocked_flag,filename,linenumber);
  }
  else
  {
    rlc_block_flag_check_result = FAIL_RESULT;
    MSG_GERAN_ERROR_3_G("GMAC rlc_blocked_flag check FAIL current val: %d, filename: %d, line: %d",
                        util_data_ptr->gmac_rlc_blocked_flag,filename,linenumber);
  }

  return rlc_block_flag_check_result;
}


/*===========================================================================
===
===  FUNCTION      gmac_util_is_rlc_blocked
===
===  DESCRIPTION
===  This function returns the current blocked state of RLC.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  gmac_rlc_blocked_t - current state of gmac_rlc_blocked_flag
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
gmac_rlc_blocked_t gmac_util_is_rlc_blocked(gmac_filenames_t filename,
                                            uint32           linenumber,
                                            gas_id_t         gas_id)
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  if (util_data_ptr->gmac_rlc_blocked_flag == BLOCKED)
  {
    MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag currently set to BLOCKED, filename: %d, line: %d",filename,linenumber);
  }
  else
  {
    MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag currently set to UNBLOCKED, filename: %d, line: %d",filename,linenumber);
  }

  return util_data_ptr->gmac_rlc_blocked_flag;
}

/*===========================================================================
===
===  FUNCTION      gmac_util_unblock_rlc
===
===  DESCRIPTION
===  This function sends MR_NO_PS_ACCESS_IND to RLC and sets rlc_blocked_flag
===  to UNBLOCKED.  If RLC is already UNBLOCKED when this is called then no signal
===  is sent to RLC.
===
===  action - NO_CELL_CHANGE_NO_PURGE           -   no cell reselection has happened between no service to service .
===              CELL_CHANGE_PURGE_GMM_SIG        -  cell reselection has happened between no service to service
===              NO_CELL_CHANGE_PURGE_GMM_SIG -   no cell reselection has happened but grlc needs to delete gmm messages.
===                                                                                Special case for T3142 AND T3172 timers
===
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  None
===
===  SIDE EFFECTS
===  gmac_rlc_blocked_flag is set
===
===========================================================================*/
void gmac_util_unblock_rlc(action_on_ps_access_t action,
                           gmac_filenames_t filename,
                           uint32 linenumber,
                           gas_id_t gas_id)
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  if (util_data_ptr->gmac_rlc_blocked_flag == BLOCKED)
  {
    util_data_ptr->gmac_rlc_blocked_flag = UNBLOCKED;

    /* Print F3 Message based on filename*/
    switch (filename)
    {
      case GMAC_IDLE:
         MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag = UNBLOCKED, action: %d, gmacidle line: %d",action,linenumber);
      break;

      case GMAC_IDLEUTIL:
         MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag = UNBLOCKED, action: %d, gmacidleutil line: %d",action,linenumber);
      break;

      case GMAC_NULL:
         MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag = UNBLOCKED, action: %d, gmacnull line: %d",action,linenumber);
      break;

      case GMAC_PROC:
         MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag = UNBLOCKED, action: %d, gmacproc line: %d",action,linenumber);
      break;

      case GMAC_TASK:
         MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag = UNBLOCKED, action: %d, gmactask line: %d",action,linenumber);
      break;

      case GMAC_TRAN:
         MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag = UNBLOCKED, action: %d, gmactran line: %d",action,linenumber);
      break;

      case GMAC_TRAN_C:
         MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag = UNBLOCKED, action: %d, gmactran_c line: %d",action,linenumber);
      break;

      case GMAC_TRAN_UL:
         MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag = UNBLOCKED, action: %d, gmactran_ul line: %d",action,linenumber);
      break;

      case GMAC_TRAN_DL:
         MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag = UNBLOCKED, action: %d, gmactran_dl line: %d",action,linenumber);
      break;

      case GMAC_UTIL:
         MSG_GERAN_MED_2_G("GMAC rlc_blocked_flag = UNBLOCKED, action: %d, gmacutil line: %d",action,linenumber);
      break;

      default:
        MSG_GERAN_ERROR_1_G("GMAC Error unknown filename: 0x%x",filename);
    }
    gmac_send_mr_ps_access_req(action, gas_id);
  }
  else
  {
    /* low occurrence message print filename as enum */
    MSG_GERAN_HIGH_3_G("GMAC gmac_util_unblock_rlc called when already unblocked, action: %d, filename: %d, line: %d",
                       action,filename,linenumber);
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_util_block_rlc
===
===  DESCRIPTION
===  This function sends MR_NO_PS_ACCESS_IND to RLC and sets rlc_blocked_flag
===  flag to TRUE.  If RLC is already blocked when this is called a MR_NO_PS_ACCESS_IND
===  is still sent so that delete_pdu_cause can be updated in RLC.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  None
===
===
===  SIDE EFFECTS
===  gmac_rlc_blocked_flag is set
===
===========================================================================*/
void gmac_util_block_rlc(delete_pdu_cause_t delete_pdu_cause,
                         gmac_filenames_t   filename,
                         uint32             linenumber,
                         gas_id_t           gas_id)
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  if (util_data_ptr->gmac_rlc_blocked_flag == BLOCKED)
  {
    /* low occurrence message print filename as enum */
    MSG_GERAN_MED_2_G("GMAC RLC already blocked send subsequent MR_NO_PS_ACCESS_IND, filename: %d, line: %d",filename,linenumber);
  }
  else
  {
    /* Print F3 Message based on filename*/
    switch (filename)
    {
      case GMAC_IDLE:
         MSG_GERAN_MED_1_G("GMAC rlc_blocked_flag = BLOCKED, gmacidle line: %d",linenumber);
      break;

      case GMAC_IDLEUTIL:
         MSG_GERAN_MED_1_G("GMAC rlc_blocked_flag = BLOCKED, gmacidleutil line: %d",linenumber);
      break;

      case GMAC_NULL:
         MSG_GERAN_MED_1_G("GMAC rlc_blocked_flag = BLOCKED, gmacnull line: %d",linenumber);
      break;

      case GMAC_PROC:
         MSG_GERAN_MED_1_G("GMAC rlc_blocked_flag = BLOCKED, gmacproc line: %d",linenumber);
      break;

      case GMAC_TASK:
         MSG_GERAN_MED_1_G("GMAC rlc_blocked_flag = BLOCKED, gmactask line: %d",linenumber);
      break;

      case GMAC_TRAN:
         MSG_GERAN_MED_1_G("GMAC rlc_blocked_flag = BLOCKED, gmactran line: %d",linenumber);
      break;

      case GMAC_TRAN_C:
         MSG_GERAN_MED_1_G("GMAC rlc_blocked_flag = BLOCKED, gmactran_c line: %d",linenumber);
      break;

      case GMAC_TRAN_UL:
         MSG_GERAN_MED_1_G("GMAC rlc_blocked_flag = BLOCKED, gmactran_ul line: %d",linenumber);
      break;

      case GMAC_TRAN_DL:
         MSG_GERAN_MED_1_G("GMAC rlc_blocked_flag = BLOCKED, gmactran_dl line: %d",linenumber);
      break;

      case GMAC_UTIL:
         MSG_GERAN_MED_1_G("GMAC rlc_blocked_flag = BLOCKED, gmacutil line: %d",linenumber);
      break;

      default:
        MSG_GERAN_ERROR_1_G("GMAC Error unknown filename: %d",filename);
    }

    util_data_ptr->gmac_rlc_blocked_flag = BLOCKED;
  }

  gmac_send_mr_no_ps_access_ind(delete_pdu_cause, gas_id);
}

/*===========================================================================
===
===  FUNCTION      gmac_send_mr_no_ps_access_ind
===
===  DESCRIPTION
===  Sends MR_NO_PS_ACCESS_IND to RLC.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  NONE
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
static void gmac_send_mr_no_ps_access_ind(delete_pdu_cause_t delete_pdu_cause,
                                          gas_id_t           gas_id)
{
  mac_rlc_msg_t  *mac_rlc_sig_ptr = NULL;
  mac_rlc_sig_ptr = grlc_get_cmd_buf_mac();
  if (mac_rlc_sig_ptr != NULL)
  {
    switch (delete_pdu_cause)
    {
     case NO_DEL:
       MSG_GERAN_MED_0_G("GMAC Sent MR_NO_PS_ACCESS_IND, del pdu: NO_DEL");
     break;

     case DEL_ONE:
       MSG_GERAN_MED_0_G("GMAC Sent MR_NO_PS_ACCESS_IND, del pdu: DEL_ONE");
     break;

     case DEL_ALL:
       MSG_GERAN_MED_0_G("GMAC Sent MR_NO_PS_ACCESS_IND, del pdu: DEL_ALL");
     break;

     default:
       MSG_GERAN_ERROR_1_G("GMAC Error bad delete_pdu_cause: 0x%x",delete_pdu_cause);
     break;
    }

    mac_rlc_sig_ptr->msg.no_ps_access_ind.sig = MR_NO_PS_ACCESS_IND;
    mac_rlc_sig_ptr->msg.no_ps_access_ind.delete_pdu_flag = delete_pdu_cause;
    gmac_rlc_mac_insert_gas_id(mac_rlc_sig_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mac_rlc_sig_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}


/*===========================================================================
===
===  FUNCTION      gmac_send_mr_ps_access_req
===
===  DESCRIPTION
===  Sends MR_NO_PS_ACCESS_IND to RLC.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  NONE
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
static void gmac_send_mr_ps_access_req(action_on_ps_access_t action,
                                       gas_id_t              gas_id)
{
  mac_rlc_msg_t  *mac_rlc_sig_ptr = NULL;
  mac_rlc_sig_ptr = grlc_get_cmd_buf_mac();
  if (mac_rlc_sig_ptr != NULL)
  {
    MSG_GERAN_MED_0_G("GMAC Sent MR_PS_ACCESS_REQ");
    mac_rlc_sig_ptr->msg.ps_access_req.sig = MR_PS_ACCESS_REQ;
    mac_rlc_sig_ptr->msg.ps_access_req.action = action;

    gmac_rlc_mac_insert_gas_id(mac_rlc_sig_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mac_rlc_sig_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("GMAC Error MALLOC failed");
  }
}


/*===========================================================================
===
===  FUNCTION      gmac_send_mac_grr_ps_access_cnf
===
===  DESCRIPTION
===  Sends MR_NO_PS_ACCESS_IND to RLC.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  NONE
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
void gmac_send_mac_grr_ps_access_cnf(gas_id_t gas_id)
{
  mac_grr_sig_t grr_sig;

  MSG_GERAN_MED_0_G("MAC Sent MAC_GRR_PS_ACCESS_CNF");
  grr_sig.mac_grr_mac_status.message_header.message_set = MS_MAC_RR;
  grr_sig.mac_grr_mac_status.message_header.message_id = (uint8)MAC_GRR_PS_ACCESS_CNF;
  gprs_grr_mac_acc_func(&grr_sig, gas_id);
}


/*===========================================================================
===
===  FUNCTION    gmac_util_start_pcco_process()
===
===  DESCRIPTION
===  This function is used to set the state of the "gmac_pcco_state".
===  and is called on initiation of the PCCO procedure.
===
===  PCCO procedure can ONLY be initiated on reception of GRR_MAC_PS_ACCESS_REQ
===  with signal parameter "pcco_in_progress" set to TRUE.
===
===  DEPENDENCIES
===  gmac_pcco_state (True = PCCO is currently in progress)
===
===  RETURN VALUE None
===
===  SIDE EFFECTS None
===========================================================================*/

void gmac_util_start_pcco_process
(
boolean           activate_pcco,        /*PCCO active indication  rcvd in GRR_MAC_PS_ACCESS*/
gmac_filenames_t  file_name,            /*file name of function caller*/
uint32            line_no,
uint8             state,                /*current state of MAC mode */
gas_id_t          gas_id
)
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  if(util_data_ptr->gmac_pcco_state == NO_PCCO_ACTIVE)
  {
    if(activate_pcco == TRUE)
    {
      util_data_ptr->gmac_pcco_state = PCCO_ACTIVE;
      MSG_GERAN_MED_3_G("GMAC PCCO started gmac_pcco_state = PCCO_ACTIVE, file %d line %d mode state %d",file_name,line_no,state);
    }
  }
  else
  {
    if(activate_pcco == TRUE)
    {
      MSG_GERAN_ERROR_3_G("GMAC unexpected PCCO already in progress, file %d line %d mode state %d",file_name,line_no,state);
    }
    else
    {
      MSG_GERAN_ERROR_3_G("GMAC Incorrect termination PCCO process, file %d line %d mode state %d",file_name,line_no,state);
    }
  }
}

/*===========================================================================
===
===  FUNCTION    gmac_util_complete_pcco_process()
===
===  DESCRIPTION
===  This function is called when the sending of PCCO has deemed to be completed
===  either by a successful contention resolution OR the failure of PCCO.
===
===  For successful completion of PCCO MAC is obliged to ALWAYS send
===  MAC_GRR_CELL_RESEL_STATUS
==
===  For failure PCCO :-
===
===  If GRR initiated termination , (ie GRR sends GRR_MAC_NO_PS_ACCESS) for
===  these conditions MAC is NOT required to send MAC_GRR_CELL_RESEL_STATUS to
===  indicate to GRR the failure of PCCO.
===
===  IF MAC initiated termination , then MAC is obliged to send
===  MAC_GRR_CELL_RESEL_STATUS to GRR indicating failure reason.
===
===  DEPENDENCIES
===  gmac_pcco_state
===
===  RETURN VALUE
===  void
===  SIDE EFFECTS
===
===========================================================================*/

void gmac_util_complete_pcco_process
(
 pcco_status_t pcco_status,
 gmac_no_access_initiator_t     initiated_by,
 gmac_filenames_t               file_name,         /*file name of function caller*/
 uint32                         line_no,
 gas_id_t                       gas_id
)
{
  mac_grr_sig_t mac_grr_sig;
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  if(util_data_ptr->gmac_pcco_state == PCCO_ACTIVE )
  {
     util_data_ptr->gmac_pcco_state = NO_PCCO_ACTIVE;

    if(initiated_by == MAC_INITIATED)
    {
      MSG_GERAN_MED_2_G("GMAC info MAC initiated termination of PCCO file %d line %d ",file_name,line_no);

      MSG_GERAN_MED_1_G("GMAC Sent MAC_GRR_CELL_RESEL_STATUS, status %d",pcco_status);
      mac_grr_sig.mac_grr_cell_resel_status.status = pcco_status;
      mac_grr_sig.mac_grr_cell_resel_status.message_header.message_set = MS_MAC_RR;
      mac_grr_sig.mac_grr_cell_resel_status.message_header.message_id = (uint8)MAC_GRR_CELL_RESEL_STATUS;
      gprs_grr_mac_acc_func(&mac_grr_sig, gas_id);
    }
    else
    {
     MSG_GERAN_MED_2_G("GMAC info GRR initiated termination of PCCO file %d line %d ",file_name,line_no);
    }
  }
  else
  {
    MSG_GERAN_MED_0_G("GMAC info NO PCCO process to terminate");
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_util_get_pcco_state()
===
===  DESCRIPTION
===  This function returns the current PCCF state.
===
===  DEPENDENCIES
===  gmac_pcco_state
===
===  RETURN VALUE
===
===  PCCO_ACTIVE
===  NO_PCCO_ACTIVE
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
gmac_pcco_state_t gmac_util_get_pcco_state(gas_id_t gas_id)
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  MSG_GERAN_MED_1_G("GMAC IPA gmac_pcco_state currently set to: %d",util_data_ptr->gmac_pcco_state);
  return(util_data_ptr->gmac_pcco_state);
}

/*===========================================================================
===
===  FUNCTION      gmac_util_get_pccf_state()
===
===  DESCRIPTION
===  This function returns the current PCCF state.
===
===  DEPENDENCIES
===  gmac_pccf_state
===
===  RETURN VALUE
===
===  NO_PCCF_BEING_SENT
===  SENDING_VIA_SB
===  SENDING_VIA_UL_PACCH
===
===  SIDE EFFECTS
===  NONE
===
===========================================================================*/
gmac_pccf_state_t gmac_util_get_pccf_state(gas_id_t gas_id)
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  MSG_GERAN_MED_1_G("GMAC IPA gmac_pccf_state currently set to: %d",util_data_ptr->gmac_pccf_state);
  return util_data_ptr->gmac_pccf_state;
}

/*===========================================================================
===
===  FUNCTION    gmac_util_start_pccf_process()
===
===  DESCRIPTION
===  This function is used to set the state of the "gmac_pccf_state".
===  and is called on initiation PCCF.
===
===  PCCF can ONLY be initiated on reception of GRR_MAC_SEND_PCCF
===
===  DEPENDENCIES
===
===  gmac_pccf_state
===
===  RETURN VALUE None
===
===  SIDE EFFECTS None
===========================================================================*/

void gmac_util_start_pccf_process
(
gmac_pccf_state_t  pccf_tx_method, /*Method by which PCCF is to be sent to N/W*/
gmac_filenames_t   file_name,        /*file name of function caller*/
uint32             line_no,
uint8              state,            /*current state of MAC mode */
gas_id_t           gas_id
)
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  switch (util_data_ptr->gmac_pccf_state)
  {
   case NO_PCCF_BEING_SENT:
     if(pccf_tx_method == SENDING_VIA_SB)
     {
       MSG_GERAN_MED_3_G("GMAC gmac_pccf_state = SENDING_VIA_SB, file %d line %d mdoe state %d",file_name,line_no,state);
     }

     if(pccf_tx_method == SENDING_VIA_UL_PACCH)
     {
       MSG_GERAN_MED_3_G("GMAC gmac_pccf_state = SENDING_VIA_UL_PACCH, file %d line %d mode state %d",file_name,line_no,state);
     }
     util_data_ptr->gmac_pccf_state = pccf_tx_method;
     break;

   case SENDING_VIA_SB:
     if(pccf_tx_method == NO_PCCF_BEING_SENT)
     {
       MSG_GERAN_ERROR_3_G("GMAC Incorrect termination PCCF process, file %d line %d mode state %d",file_name,line_no,state);
     }

     if(pccf_tx_method == SENDING_VIA_UL_PACCH)
     {
       MSG_GERAN_ERROR_3_G("GMAC PCCF tx via SB,rcvd request to tx via UL PACCH , file %d line %d mode state %d",file_name,line_no,state);
     }
     break;

   case SENDING_VIA_UL_PACCH:
     if(pccf_tx_method == NO_PCCF_BEING_SENT)
     {
       MSG_GERAN_ERROR_3_G("GMAC Incorrect termination  PCCF process, file %d line %d mode state %d",file_name,line_no,state);
     }

     if(pccf_tx_method == SENDING_VIA_SB)
     {
       MSG_GERAN_ERROR_3_G("GMAC PCCF tx via UL PACCH,rcvd request to tx via SB, file %d line %d mode state %d",file_name,line_no,state);
     }
     break;

   default:
     break;

  }
}


/*===========================================================================
===
===  FUNCTION    gmac_util_complete_pccf_process()
===
===  DESCRIPTION
===  This function is called when the sending of PCCF has deemed to be completed
===  either by a successful sending of PCCF OR the failure to send the PCCF.
===
===  PCCF procedure can be terminated by GRR using GRR_MAC_NO_PS_ACCESS and for
===  these conditions MAC is not required to indicate to GRR the failure of PCCF.
===
===  If MAC terminated the sending of PCCF then, it is obliged to send
===  MAC_GRR_PCCF_SENT(retry) to GRR indicating failure.
===
===  If the sending of PCCF then, MAC is  obliged to send
===  MAC_GRR_PCCF_SENT(no retry) to GRR indicating success.
===
===  In the case where the PCCF sending fails and GRR is sent MAC_GRR_PCCF_SENT with
===  an unsuccessful indication. This will immediately cause GRR to re-request the
===  sending PCCF. This process will be repeated until T3716 expires at which point
===  GRR will perform a re-selection
===
===  Hence it is essential that for failure to send case, L1 has been released
===  before sending MAC_GRR_PCCF_SENT to GRR.
===
===  DEPENDENCIES
===  gmac_pccf_state
===
===  RETURN VALUE
===  void
===  SIDE EFFECTS
===
===
===========================================================================*/

void gmac_util_complete_pccf_process
(
 pccf_status_t pccf_status,
 gmac_no_access_initiator_t   initiated_by,    /*True = MAC initiated, False = GRR initiated*/
 gmac_filenames_t             file_name,        /*file name of function caller*/
 uint32                       line_no,
 gas_id_t                     gas_id
)
{

  mac_grr_sig_t grr_sig;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  if(util_data_ptr->gmac_pccf_state == SENDING_VIA_SB || util_data_ptr->gmac_pccf_state == SENDING_VIA_UL_PACCH)
  {

    if(initiated_by == MAC_INITIATED)
    {
      if(pccf_status == SUCCESSFULLY_SENT_PCCF)
      {
        MSG_GERAN_MED_3_G("GMAC Sent MAC_GRR_SENT_PCCF successful gmac_pccf_state= %d file %d line %d",
                          util_data_ptr->gmac_pccf_state,file_name,line_no);
      }
      else
      {
        MSG_GERAN_MED_3_G("GMAC Sent MAC_GRR_SENT_PCCF failed gmac_pccf_state= %d file %d line %d",
                          util_data_ptr->gmac_pccf_state,file_name,line_no);
      }
    }
    else
    {
      MSG_GERAN_MED_0_G("GMAC Info GRR terminated sending of PCCF, no need to Sent MAC_GRR_SENT_PCCF");
    }

    /*Reset gmac_pccf_state to default value*/
    util_data_ptr->gmac_pccf_state =  NO_PCCF_BEING_SENT;
    memset(&global_data_ptr->gmac_local_pccf_params,0,sizeof(gmac_pccf_params_t));

    /*If GRR terminates sending of PCCF, no need to send MAC_GRR_PCCF_SENT*/
    if(initiated_by == MAC_INITIATED)
    {
      grr_sig.mac_grr_pccf_sent.pccf_status = pccf_status;
      grr_sig.mac_grr_pccf_sent.message_header.message_set = MS_MAC_RR;
      grr_sig.mac_grr_pccf_sent.message_header.message_id = (uint8)MAC_GRR_PCCF_SENT;
      gprs_grr_mac_acc_func(&grr_sig,gas_id);
      global_data_ptr->gmac_local_pccf_params.pccf_pending = FALSE;
    }
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_util_initialise_page_mode()
===
===  DESCRIPTION
===
===  On GRR_MAC_PS_ACCESS_REQ initialise gmac_current_page_mode
===
===  DEPENDENCIES
===  gmac_current_page_mode
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_initialise_page_mode(gas_id_t gas_id)
{
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  util_data_ptr->gmac_current_page_mode = PAGING_REORGANIZATION;
}


/*===========================================================================
===
===  FUNCTION      gmac_util_page_mode_handler()
===
===  DESCRIPTION
===  On reception of any DL msg in idle mode the "page mode" has to be
===  evaluated and passed to L1 if it differs form the locally stored
===  value.
===  The default at power up is page mode = PAGING_REORGANIZATION
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_util_page_mode_handler(gas_id_t gas_id)
{
  mac_l1_sig_t   send_mac_l1_sig;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_util_data_t *util_data_ptr = gmac_get_util_data_ptr(gas_id);

  if (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.gprs_l1_page_group_flag == TRUE)
  {
    global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.gprs_l1_page_group_flag = FALSE;


    if((global_data_ptr->csn_dec_hdr_ptr->error_code != UNKNOWN_MESSAGE_TYPE) &&
     (util_data_ptr->gmac_current_page_mode != global_data_ptr->csn_dec_hdr_ptr->page_mode))
    {
      /*update local copy*/
      util_data_ptr->gmac_current_page_mode =  global_data_ptr->csn_dec_hdr_ptr->page_mode;

      /*send MAC_L1_PAGE_MODE */
      MSG_GERAN_MED_0_G("MAC Sent MAC_L1_PAGE_MODE");
      send_mac_l1_sig.mac_l1_page_mode.current_page_mode = global_data_ptr->csn_dec_hdr_ptr->page_mode;
      send_mac_l1_sig.mac_l1_page_mode.message_header.message_id = (uint8)MAC_L1_PAGE_MODE;
      send_mac_l1_sig.mac_l1_page_mode.message_header.message_set = MS_MAC_L1;
      gprs_l1_mac_acc_func(&send_mac_l1_sig, gas_id);
    }
  }
}
#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      gmac_util_send_grlcul_psho_start_ind()
===
===  DESCRIPTION
===  This function sends MR_UL_PSHO_START_IND signal to GRLC UL task
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
void gmac_util_send_grlcul_psho_start_ind(gas_id_t gas_id)
{
  mac_rlc_msg_t  *mac_rlc_sig_ptr;

  mac_rlc_sig_ptr = grlc_get_cmd_buf_mac();
  if (mac_rlc_sig_ptr != NULL)
  {
    MSG_GERAN_MED_0_G("MAC Sent MR_UL_PSHO_START_IND");
    mac_rlc_sig_ptr->msg.ul_psho_start_ind.sig = MR_UL_PSHO_START_IND;
    gmac_rlc_mac_insert_gas_id(mac_rlc_sig_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mac_rlc_sig_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_util_send_grlcdl_psho_start_ind()
===
===  DESCRIPTION
===  This function sends MR_DL_PSHO_START_IND signal to GRLC DL task
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
void gmac_util_send_grlcdl_psho_start_ind(gas_id_t gas_id)
{
  mac_rlc_msg_t  *mac_rlc_sig_ptr;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  mac_rlc_sig_ptr = grlc_get_cmd_buf_mac();

  if (mac_rlc_sig_ptr != NULL)
  {
    MSG_GERAN_MED_0_G("MAC Sent MR_DL_PSHO_START_IND");
    mac_rlc_sig_ptr->msg.dl_psho_start_ind.sig = MR_DL_PSHO_START_IND;
    mac_rlc_sig_ptr->msg.dl_psho_start_ind.tbf_mode = global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.tbf_mode;
    gmac_rlc_mac_insert_gas_id(mac_rlc_sig_ptr,gas_id);
    grlc_put_cmd_mac(gas_id,mac_rlc_sig_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_get_ul_ts_alloc_from_psho_req()
===
===  DESCRIPTION
===    Gets the timeslot allocation from the PSHO Multiple UL Assignment
===    structure.  This function gets the timeslot if only 1 TBF is
===    defined.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_ul_ts_alloc_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources_ptr,
                                                  uint8                          *ts_alloc,
                                                  gas_id_t                        gas_id)
{
  boolean local_ts_set            = FALSE;
  boolean finding_next_global_ts  = FALSE;
  uint8 ts_mask                   = 0;
  uint8 num_ts_counter            = 0;
  uint8 global_ts_offset          = 0;
  ul_tbf_assignment_t             *ul_tbf_assignment_ptr    = NULL;
  timeslot_description_t          *timeslot_description_ptr = NULL;

  *ts_alloc = 0;
  /***** validate variables ******/

  if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
  {
    ul_tbf_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.ul_tbf_assignments[0]);
    /* Check that Global Pkt TS description for UL is present*/
    if (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.timeslot_description_present == FALSE)
    {
      MSG_GERAN_ERROR_0_G("GMAC PSHO, Global TS desc is not present");
      return FALSE;
    }

    timeslot_description_ptr = &(ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.timeslot_description);
  }
  else if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_GPRS)
  {
    ul_tbf_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.gprs_mode.ul_tbf_assignments[0]);
    /* Check that Global Pkt TS description for UL is present*/
    if (ps_handover_resources_ptr->tbf_mode.gprs_mode.timeslot_description_present == FALSE)
    {
      MSG_GERAN_HIGH_0_G("GMAC PSHO, Global TS desc is not present");
      return FALSE;
    }
    timeslot_description_ptr = &(ps_handover_resources_ptr->tbf_mode.gprs_mode.timeslot_description);
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GMAC PSHO, Internal ERROR, ps_handover_resources_ptr->type_of_mode = 0x%x",ps_handover_resources_ptr->type_of_mode);
    return FALSE;
  }

  /* Check that at least one TS has been assigned */
  if (timeslot_description_ptr->ms_timeslot_allocation == 0)
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO, Global TS desc contains no timeslots");
    return FALSE;
  }

  /* Check that the number of TS in the Global TS Alloc is consistent with num_timeslots from CSN */
  if ( gmac_utils_calculate_nts(timeslot_description_ptr->ms_timeslot_allocation, gas_id)
     != ul_tbf_assignment_ptr->num_timeslots )
  {
    MSG_GERAN_HIGH_2_G("GMAC PSHO, num of TS in glob != num_timeslots, num_timeslots:0x%x, global_ts_alloc:0x%x",
                       ul_tbf_assignment_ptr->num_timeslots,
                       timeslot_description_ptr->ms_timeslot_allocation);
    return FALSE;
  }

  /*TBF_TIMESLOT_ALLOCTION in Uplink_TBF_Assignment_struct is present*/
  if (ul_tbf_assignment_ptr->timeslot_allocation_present == TRUE)
  {
    global_ts_offset = 7;

    /* set the start position for the TBF_TIMESLOT_ALLOCTION based on the number of timeslots*/
    ts_mask = (uint8)(0x01 << (ul_tbf_assignment_ptr->num_timeslots - 1));

    /* Loop around the number of TS in the TBF_TIMESLOT_ALLOCTION */
    for (num_ts_counter = 0; num_ts_counter < ul_tbf_assignment_ptr->num_timeslots; num_ts_counter++)
    {
      if ( (ul_tbf_assignment_ptr->timeslot_allocation & ts_mask) == 0 )
      {
        local_ts_set = 0;
      }
      else
      {
        local_ts_set = 1;
      }

      ts_mask = ts_mask >> 1;

      finding_next_global_ts = TRUE;

      while (finding_next_global_ts)
      {
        /* found next global ts descr allocated timeslot */
        if ( (timeslot_description_ptr->ms_timeslot_allocation &
             (0x01 << global_ts_offset)) != 0 )
        {
          /* Set the time slot from the global ts descr. */
          *ts_alloc |= (uint8)(local_ts_set << (global_ts_offset));
          finding_next_global_ts = FALSE;
        }

        if ( global_ts_offset > 0 )
        {
          global_ts_offset--;
        }
        else
        {
          finding_next_global_ts = FALSE;
        }
      }
    }
  }
  else
  {
    /* set TS from global time slot descr*/
    *ts_alloc = timeslot_description_ptr->ms_timeslot_allocation;
  }

  return TRUE;
}
/*===========================================================================
===
===  FUNCTION      gmac_get_usf_from_psho_req()
===
===  DESCRIPTION
===    Gets the usf from the PS HANDOVER COMMADN Multiple UL Assignment
===    structure.  The usf_allocation decoded by CSN is mapped into the
===    mac_l1_psho_req_t usf array using the
===    assigned timeslots (see Table 11.2.37.2 44.060).  The usf_allocation can
===    either be contain different USFs or the same USF for all timeslots.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_usf_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources_ptr,
                                          mac_l1_psho_req_t              *psho_req_ptr,
                                          gas_id_t                        gas_id)
{
  uint8                   ul_ts_alloc           = 0;
  uint8                   num_ts_alloc          = 0;
  uint8                   usf_count             = 0;
  boolean                 finding_next_ts       = FALSE;
  uint8                   ts_offset             = 0;
  ul_tbf_assignment_t    *ul_tbf_assignment_ptr = NULL;

  if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
  {
    ul_tbf_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.ul_tbf_assignments[0]);
  }
  else
  {
    ul_tbf_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.gprs_mode.ul_tbf_assignments[0]);
  }

  /* Get the timeslot allocation from the PSHO msg */
  if ( gmac_get_ul_ts_alloc_from_psho_req(ps_handover_resources_ptr, &ul_ts_alloc, gas_id) == FALSE )
  {
    MSG_GERAN_HIGH_0_G("GMAC Psho req, Error getting UL TS");
    return FALSE;
  }

  /* Check that the number of timeslots in the allocation is consistent with the number of USF allocations */
  if (ul_tbf_assignment_ptr->usf_allocation_per_slot == 1)
  {
    num_ts_alloc = gmac_utils_calculate_nts(ul_ts_alloc, gas_id);

    if ( num_ts_alloc != ul_tbf_assignment_ptr->num_usf_allocations )
    {
      MSG_GERAN_HIGH_2_G("GMAC PSHO, Num of UL TS(%d) != Num of USF alloc(%d)",num_ts_alloc,ul_tbf_assignment_ptr->num_usf_allocations);
      return FALSE;
    }
  }
  else
  {
    num_ts_alloc = gmac_utils_calculate_nts(ul_ts_alloc, gas_id);
  }

  memset(psho_req_ptr->ul_tbf.usf,0,sizeof(psho_req_ptr->ul_tbf.usf));

  /* Start offset at TS 0 */
  ts_offset = 0;

  /* count through the number of timeslots allocated */
  for ( usf_count=0; usf_count < num_ts_alloc; usf_count++)
  {
    finding_next_ts = TRUE;

    /* find the next allocated timeslot */ 
    while ((finding_next_ts) && (ts_offset <= 7))
    {
      if ( (ul_ts_alloc & (0x80 >> ts_offset) ) != 0 )
      {
        if (ul_tbf_assignment_ptr->usf_allocation_per_slot == 1 )
        {
          /* Different USF assigned to each allocated timeslots */
          psho_req_ptr->ul_tbf.usf[ts_offset].usf_value = ul_tbf_assignment_ptr->usf_allocation[usf_count];
        }
        else
        {
          /* Same USF is assigned to all allocated timeslots */
          psho_req_ptr->ul_tbf.usf[ts_offset].usf_value = ul_tbf_assignment_ptr->usf_allocation[0];
        }

        psho_req_ptr->ul_tbf.usf[ts_offset].present = TRUE;

        /* exit while loop and assign next usf value */
        finding_next_ts = FALSE;
      }

      /* Reached last offset at TS 7 */
      if (ts_offset == 7)
      {
        /* Reached the last timeslot, check that the usf_count for loop is going to exit */
        if ( (usf_count+1) != num_ts_alloc)
        {
          MSG_GERAN_ERROR_2_G("GMAC PSHO, Internal failure in usf routine, usf_count:%d, num_ts_alloc:%d",usf_count,num_ts_alloc);
          return FALSE;
        }
        finding_next_ts = FALSE;
      }
      else
      {
        /* increment through TS 0 to TS 7 */
        ts_offset++;
      }
    }
  }

  return TRUE;
}

/*===========================================================================
===
===  FUNCTION      gmac_util_psho_validate_psho_req()
===
===  DESCRIPTION
===  Validates the content of the PS HANDOVER COMMAND message.
===  Performs multislot and frequency checking.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    BOOLEAN (TRUE or FALSE)
===
===  SIDE EFFECTS
===  Frequency checking function updates: gmac_store_freq_params_in_use
===
===
===========================================================================*/
static gmac_pkt_ps_ho_result_t gmac_util_psho_validate_psho_req(ps_handover_radio_resources_t  *ps_handover_resources_ptr,
                                                                boolean                        *control_ack_ptr,
                                                                gas_id_t                        gas_id)
{
  /*local data*/
  uint8 ul_ts_alloc               = 0;
  uint8 dl_ts_alloc               = 0;
  uint8 local_eda                 = 0;
  uint8 ul_egprs_ws               = 0;
  uint8 dl_egprs_ws               = 0;
  uint8 coding_scheme             = 0;
  uint16 decoded_ul_ws            = 0;
  uint16 decoded_dl_ws            = 0;
  dl_assignment_t                 *dl_assignment_ptr = NULL;
  ul_tbf_assignment_t             *ul_tbf_assignment_ptr = NULL;
  mac_l1_sig_t                     mac_l1_msg;
  gmac_global_data_t              *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  if((global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
     && (ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
     && (ps_handover_resources_ptr->rlc_reset == PSHO_RLC_NOT_RESET))
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO command, EGPRS resources received when tbf mode is GPRS and RLC reset flag is not set. so ignoring");
    return PSHO_VALIDATION_FAIL;
  }

  if((global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
     && (ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_GPRS)
     && (ps_handover_resources_ptr->rlc_reset == PSHO_RLC_NOT_RESET))
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO command, GPRS resources received when tbf mode is EGPRS and RLC reset flag is not set. so ignoring");
    return PSHO_VALIDATION_FAIL;
  }

  if(grlc_ul_is_cv_active(gas_id) == TRUE)
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO command, Countdown is ongoing at GRLC so rejecting PSHO Message");
    return PSHO_VALIDATION_FAIL;
  }

  /*Check that no more than 1 multiple UL TBF structure is assigned*/
  if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
  {
    if (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.num_ul_tbf_assignments == 0)
    {
      MSG_GERAN_HIGH_1_G("GMAC PSHO command, %d EGPRS UL TBFs assigned ",
                         ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.num_ul_tbf_assignments);
      return PSHO_VALIDATION_FAIL;
    }
    if (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.num_ul_tbf_assignments > 1)
    {
      MSG_GERAN_HIGH_1_G("GMAC PSHO command, %d mult EGPRS UL TBFs assigned ",
                         ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.num_ul_tbf_assignments);
      return PSHO_VALIDATION_FAIL;
    }
    if (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.num_ul_tbf_assignments == 1)
    {
      ul_tbf_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.ul_tbf_assignments[0]);
    }
  }
  else
  {
    if (ps_handover_resources_ptr->tbf_mode.gprs_mode.num_ul_tbf_assignments == 0)
    {
      MSG_GERAN_HIGH_1_G("GMAC PSHO command, %d GPRS UL TBFs assigned ",ps_handover_resources_ptr->tbf_mode.gprs_mode.num_ul_tbf_assignments);
      return PSHO_VALIDATION_FAIL;
    }

    if (ps_handover_resources_ptr->tbf_mode.gprs_mode.num_ul_tbf_assignments > 1)
    {
      MSG_GERAN_HIGH_1_G("GMAC PSHO command, %d mult GPRS UL TBFs assigned ",
                         ps_handover_resources_ptr->tbf_mode.gprs_mode.num_ul_tbf_assignments);
      return PSHO_VALIDATION_FAIL;
    }
    if (ps_handover_resources_ptr->tbf_mode.gprs_mode.num_ul_tbf_assignments == 1)
    {
      ul_tbf_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.gprs_mode.ul_tbf_assignments[0]);
    }
  }

  /*Check that no more than 1 multiple DL TBF structure is assigned*/
  if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
  {
    if ((ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf_present) &&
        (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.num_dl_assignments > 1))
    {
      MSG_GERAN_HIGH_1_G("GMAC PSHO command, %d mult EGPRS DL TBFs assigned ",
                         ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.num_dl_assignments);
      return PSHO_VALIDATION_FAIL;
    }

    if ((ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf_present) &&
        (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.num_dl_assignments == 1))
    {
      dl_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.dl_assignments[0] );
    }
  }
  else
  {
    if (ps_handover_resources_ptr->tbf_mode.gprs_mode.num_dl_assignments > 1)
    {
      MSG_GERAN_HIGH_1_G("GMAC PSHO command, %d mult GPRS DL TBFs assigned ",ps_handover_resources_ptr->tbf_mode.gprs_mode.num_dl_assignments);
      return PSHO_VALIDATION_FAIL;
    }
    if (ps_handover_resources_ptr->tbf_mode.gprs_mode.num_dl_assignments == 1)
    {
      dl_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.gprs_mode.dl_assignments[0]);
    }
  }

  if(dl_assignment_ptr != NULL)
  {
     global_data_ptr->current_rcvd_dl_ctrl_ack = dl_assignment_ptr->dl_tbf_assignment.control_ack;

    if (dl_assignment_ptr->dl_tbf_assignment.control_ack == TRUE)
    {
      /*T3192 must be running if CTRL_ACK is set*/
      if (grlc_dl_t3192_status(gas_id) != TMR_ACTIVE)
      {
        MSG_GERAN_HIGH_0_G("GMAC PSHO command, DL TBF CTRL ACK set, T3192 NOT running, Ab Rel ");
        return PSHO_VALIDATION_FAIL;
      }

      if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == FALSE)
      {
        MSG_GERAN_HIGH_0_G("GMAC PSHO command, DL TBF CTRL ACK set, Current DL TFI NOT valid, Ab Rel");
        return PSHO_VALIDATION_FAIL;
      }

      /*PSHO TFI must match the current active TFI*/
      if (dl_assignment_ptr->dl_tbf_assignment.tfi_assignment != global_data_ptr->gmac_local_tfi_store.dl_tfi)
      {
        MSG_GERAN_HIGH_2_G("GMAC PSHO command, DL TBF CTRL ACK set, TFIs do NOT match, PSHO DL TFI:0x%x, Current DL TFI:0x%x, Ab Rel ",
                           dl_assignment_ptr->dl_tbf_assignment.tfi_assignment,
                           global_data_ptr->gmac_local_tfi_store.dl_tfi);
        return PSHO_VALIDATION_FAIL;
      }
      *control_ack_ptr = TRUE;
    }
    else
    {
      if (grlc_dl_t3192_status(gas_id) == TMR_ACTIVE)
      {
        MSG_GERAN_HIGH_0_G("GMAC PSHO command, DL TBF CTRL ACK NOT set, T3192 running,  ");
        return PSHO_VALIDATION_FAIL;
      }
      *control_ack_ptr = FALSE;
    }
  }

  if((ul_tbf_assignment_ptr != NULL)&&(dl_assignment_ptr != NULL)
     &&(*control_ack_ptr == FALSE))
  {
    global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type = PSHO_UL_DL_TBF;
  }
  else
  {
    global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type = PSHO_UL_TBF;
  }

  /* Check that the UL timeslots have been allocated correctly */
  if ( gmac_get_ul_ts_alloc_from_psho_req(ps_handover_resources_ptr,&ul_ts_alloc,gas_id) == FALSE )
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO REQ, error in UL TS alloc, ignore ");
    return PSHO_VALIDATION_FAIL;
  }

  /* Check that the USF can be retrieved from the PSHO msg */
  if (gmac_get_usf_from_psho_req(ps_handover_resources_ptr,
                                 &(mac_l1_msg.mac_l1_psho_req),
                                 gas_id) == FALSE )
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO REQ, error getting USF from msg, ignore ");
    return PSHO_VALIDATION_FAIL;
  }

  /* Multislot class checking */

  /* Get the downlink timeslots*/
  if ( global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
  {
    dl_ts_alloc = dl_assignment_ptr->timeslot_allocation;
  }
  else
  {
    dl_ts_alloc = 0;
  }

  /* Get the EDA mode */
  local_eda = ps_handover_resources_ptr->ext_dynamic_allocation;

  /* MULTI SLOT CLASS checking */
  /* PSHO MS CLASS CHECK flag set to true */
  if ( gmac_util_multislot_class_check(ul_ts_alloc,
                                       dl_ts_alloc,
                                       local_eda,
                                       0,
                                       FALSE, PSHO_UPDATE_PARAMS_REQUIRED,
                                       gas_id ) == FALSE )
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO, error in multislot class, ignore PSHO message ");
    return PSHO_VALIDATION_FAIL;
  }
  /* FREQ PARAMS checking */
  if (gmac_util_frequency_param_ie(FALSE,FALSE,gas_id) != NO_ERRORS )
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO, error in freq params, ignore PSHO message");
    return PSHO_VALIDATION_FAIL;
  }
  /* DL RLC MODE checking */
  if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
  {
    if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_type == PSHO_GTOG)
    {
      if((ps_handover_resources_ptr->rlc_reset == PSHO_RLC_NOT_RESET)
          &&((global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate == TRANSFER_DL)
          ||(global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate == TRANSFER_CONCURRENT_DYNAMIC_TRANSFER)))
      {
        /*network commanded no reset of RLC or there is an active dl tbf
        in the mobile. Compare the stored rlc mode with the received rlc mode
        in the assignment. if they are different then ignore the message*/
        if(global_data_ptr->gmac_store_dl_rlc_mode != dl_assignment_ptr->dl_tbf_assignment.downlink_rlc_mode)
        {
          MSG_GERAN_HIGH_0_G("GMAC PSHO, error in received dl rlc mode, ignore PSHO message");
          return PSHO_VALIDATION_FAIL;
        }
      }
    }
    if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
    {
      if((ps_handover_resources_ptr->rlc_reset == PSHO_RLC_RESET)
          ||(global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate == TRANSFER_UL_DYNAMIC)
          ||(global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate == TRANSFER_INACTIVE))
      {
        /*network commanded reset of RLC or there is no dl tbf active in mobile
        This means network should give a WS in the assignment. if there is no
        WS then ignore the assignment*/
        /* DL EGPRS window size checking */
        if ( gmac_get_dl_ws_from_psho_req(ps_handover_resources_ptr, &dl_egprs_ws, gas_id) == FALSE )
        {
          MSG_GERAN_HIGH_0_G("GMAC PSHO, DL EGPRS window size not present, ignore PSHO message ");
          return PSHO_VALIDATION_FAIL;
        }
        decoded_dl_ws = ((dl_egprs_ws *32) + 64);

        if(gmac_check_assigned_egprs_ws(global_data_ptr->gmac_egprs_ms_class_max_ts_rx,decoded_dl_ws,gas_id) == FAIL_RESULT)
          {
            MSG_GERAN_HIGH_0_G("GMAC PSHO, DL EGPRS window size invalid, ignore PSHO message ");
            return PSHO_VALIDATION_FAIL;
          }
        }
      else
      {
        /*network commandad no reset of RLC or there is an active dl tbf
        in the mobile. if there is no WS in the assignment then use the
        existing ws otherwise validate the given WS from the assignment*/
        if ( gmac_get_dl_ws_from_psho_req(ps_handover_resources_ptr, &dl_egprs_ws, gas_id) == TRUE )
        {
          if ( gmac_egprs_decode_dl_window_size(dl_egprs_ws, dl_ts_alloc, gas_id) == WS_ALLOCATION_INVALID )
          {
            MSG_GERAN_HIGH_0_G("GMAC PSHO, DL EGPRS window size invalid, ignore PSHO message ");
            return PSHO_VALIDATION_FAIL;
          }
        }
      }
    }
  }
  if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
  {
    if((ps_handover_resources_ptr->rlc_reset == PSHO_RLC_RESET)
        ||(global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate == TRANSFER_DL)
        ||(global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate == TRANSFER_INACTIVE))
    {
      /*network commandad reset of RLC or there is no ul tbf active in mobile
        This means network should give a WS in the assignment. if there is no
        WS then ignore the assignment*/
      /* UL EGPRS window size checking */
      if ( gmac_get_ul_ws_from_psho_req(ps_handover_resources_ptr, &ul_egprs_ws, gas_id) == FALSE )
      {
        MSG_GERAN_HIGH_0_G("GMAC PSHO, UL EGPRS window size not present, ignore PSHO message ");
        return PSHO_VALIDATION_FAIL;
      }
      /*There is WS in the assignment so validate the given WS */
      decoded_ul_ws = ((ul_egprs_ws *32) + 64);
      if(gmac_check_assigned_egprs_ws(global_data_ptr->gmac_egprs_ms_class_max_ts_tx,decoded_ul_ws, gas_id) == FAIL_RESULT)
        {
          MSG_GERAN_HIGH_0_G("GMAC PSHO, UL EGPRS window size invalid, ignore PSHO message ");
          return PSHO_VALIDATION_FAIL;
        }
      }
    else
    {
      /*network commanded no reset of RLC or there is an active ul tbf
        in the mobile. if there is no WS in the assignment then use the
        existing ws otherwise validate the given WS from the assignment*/
      if ( gmac_get_ul_ws_from_psho_req(ps_handover_resources_ptr, &ul_egprs_ws, gas_id) == TRUE )
      {
        if ( gmac_egprs_decode_ul_window_size(ul_egprs_ws, ul_ts_alloc, gas_id) == WS_ALLOCATION_INVALID )
        {
          MSG_GERAN_HIGH_0_G("GMAC PSHO, UL EGPRS window size invalid, ignore PSHO message ");
          return PSHO_VALIDATION_FAIL;
        }
      }
    }
    /* UL EGPRS MCS checking */
    if (gmac_get_egprs_mcs_from_psho_req(ps_handover_resources_ptr,&coding_scheme, gas_id) == FALSE)
    {
      MSG_GERAN_HIGH_0_G("GMAC PSHO, UL EGPRS MCS missing, ignore PSHO message ");
      return PSHO_VALIDATION_FAIL;
    }
  }
  else
  {
    /* UL GPRS CS checking */
    if (gmac_get_cs_from_psho_req(ps_handover_resources_ptr,&coding_scheme, gas_id) == FALSE)
    {
      MSG_GERAN_HIGH_0_G("GMAC PSHO, error in coding scheme, ignore PSHO message");
      return PSHO_VALIDATION_FAIL;
    }
  }
  if((global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
     && (ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
     && (ps_handover_resources_ptr->rlc_reset == PSHO_RLC_RESET))
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO command changing the TBF MODE from GPRS to EGPRS");

  }

  if((global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
     && (ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_GPRS)
     && (ps_handover_resources_ptr->rlc_reset == PSHO_RLC_RESET))
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO command changing the TBF MODE from EGPRS to GPRS");

  }
  return PSHO_VALIDATION_PASS;
}
/*===========================================================================
===
===  FUNCTION      gmac_get_cs_from_psho_req()
===
===  DESCRIPTION
===    Gets the coding scheme (cs) from the PSHO message.  If the CS is
===    present in the Multiple Uplink Assignment Struct then this is taken in preference
===    to the CS in the main body of the PSHO msg.
===    This function gets the CS if only 1 TBF is defined.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_cs_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources_ptr,
                                         uint8                          *cs,
                                         gas_id_t                        gas_id)
{
  boolean status = TRUE;

  if (ps_handover_resources_ptr->tbf_mode.gprs_mode.ul_tbf_assignments[0].channel_coding_present)
  {
    *cs = ps_handover_resources_ptr->tbf_mode.gprs_mode.ul_tbf_assignments[0].channel_coding_command;
  }
  else if (ps_handover_resources_ptr->tbf_mode.gprs_mode.channel_coding_command_present)
  {
    *cs = ps_handover_resources_ptr->tbf_mode.gprs_mode.channel_coding_command;
  }
  else
  {
    status = FALSE;
  }

  return status;
}


/*===========================================================================
===
===  FUNCTION      gmac_get_egprs_mcs_from_psho_req()
===
===  DESCRIPTION
===    Gets the EGPRS MCS from the PSHOmessage.  If the MCS is
===    present in the Multiple Uplink Assignment Struct then this is taken in preference
===    to the MCS in the main body of the PSHO msg.
===    This function gets the MCS if only 1 TBF is defined.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_egprs_mcs_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources_ptr,
                                                uint8                          *egprs_mcs,
                                                gas_id_t                        gas_id)
{
  boolean status = TRUE;

  if (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.ul_tbf_assignments[0].egprs_channel_coding_command)
  {
    *egprs_mcs = ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.ul_tbf_assignments[0].egprs_channel_coding_command;
  }
  else if (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.egprs_channel_coding_command)
  {
    *egprs_mcs = ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.egprs_channel_coding_command;
  }
  else
  {
    status = FALSE;
  }

  if (status == TRUE)
  {
    if ( gmac_util_egprs_mcs_valid(*egprs_mcs, gas_id) == FALSE )
    {
      MSG_GERAN_ERROR_0_G("GMAC info PSHO REQ EGPRS MCS invalid");
      status = FALSE;
    }
  }

  return status;
}

/*===========================================================================
===
===  FUNCTION      gmac_get_ul_ws_from_psho_req()
===
===  DESCRIPTION
===    Gets the UL EGPRS window size from the PSHO message.  If the WS is
===    present in the Multiple Uplink Assignment Struct then this is taken in preference
===    to the window size in the main body of the PSHO msg.
===    This function gets the window size if only 1 TBF is defined.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_ul_ws_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources_ptr,
                                            uint8                          *ul_ws,
                                            gas_id_t                        gas_id)
{
  boolean status = TRUE;

  if (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.ul_tbf_assignments[0].ul_egprs_window_size_present)
  {
    *ul_ws = ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.ul_tbf_assignments[0].ul_egprs_window_size;
  }
  else if (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.ul_egprs_window_size_present)
  {
    *ul_ws = ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.ul_egprs_window_size;
  }
  else
  {
    status = FALSE;
  }

  return status;
}


/*===========================================================================
===
===  FUNCTION      gmac_get_dl_ws_from_psho_req()
===
===  DESCRIPTION
===    Gets the DL EGPRS window size from the PSHO message.  If the WS is
===    present in the Multiple Dplink Assignment Struct then this is taken in preference
===    to the window size in the main body of the PSHO msg.
===    This function gets the window size if only 1 TBF is defined.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    boolean - TRUE - PASS, FALSE - FAIL
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_get_dl_ws_from_psho_req(ps_handover_radio_resources_t  *ps_handover_resources_ptr,
                                            uint8                          *dl_ws,
                                            gas_id_t                        gas_id)
{
  boolean status = TRUE;

  if (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.dl_assignments[0].dl_tbf_assignment.dl_egprs_window_size_present)
  {
    *dl_ws = ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.dl_assignments[0].dl_tbf_assignment.dl_egprs_window_size;
  }
  else if (ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.dl_egprs_window_size_present)
  {
    *dl_ws = ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.dl_egprs_window_size;
  }
  else
  {
    status = FALSE;
  }

  return status;
}
/*===========================================================================
===
===  FUNCTION      gmac_util_psho_populate_psho_req_config_msgs()
===
===  DESCRIPTION
===  Populates messages associated with the PS HANDOVER COMMAND procedure.
===  Messages populated: mac_l1_psho_req_t (psho_req_ptr)
===                      mr_ul_alloc_cnf_t (gmac_ps_ho.psho_config_params.psho_ul_alloc_info)
===                      mr_dl_ass_ind_t (gmac_ps_ho.psho_config_params.psho_dl_ass_info)
===  Updates Global data:
===    gmac_ps_ho.psho_config_params structure
===    gmac_ps_ho.psho_control_params structure
===
===  RETURN VALUE
===    NONE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gmac_util_psho_populate_psho_req_config_msgs(ps_handover_radio_resources_t  *ps_handover_resources_ptr,
                                                         mac_l1_psho_req_t              *psho_req_ptr,
                                                         gas_id_t                        gas_id )
{
  pwr_ctrl_paramtrs_t    *psho_pwr_cntl_ptr         = NULL;
  uint8                   ul_egprs_ws               = 0;
  uint8                   ul_ts_alloc               = 0;
  uint16                  decoded_ul_ws             = 0;
  uint8                   coding_scheme             = 0;
  uint8                   dl_egprs_ws               = 0;
  timeslot_description_t *timeslot_description_ptr  = NULL;
  dl_assignment_t        *dl_assignment_ptr         = NULL;
  ul_tbf_assignment_t    *ul_tbf_assignment_ptr     = NULL;
  gmac_global_data_t     *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /*Assign ul tbf assignment and dl assignment structures from ps handover resources*/
  if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
  {
    ul_tbf_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.ul_tbf_assignments[0]);
    if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
    {
      dl_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.dl_assignments[0] );
    }
  }
  else
  {
    ul_tbf_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.gprs_mode.ul_tbf_assignments[0]);
    if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
    {
      dl_assignment_ptr = &(ps_handover_resources_ptr->tbf_mode.gprs_mode.dl_assignments[0]);
    }
  }
  memset(psho_req_ptr,0,sizeof(mac_l1_psho_req_t));
  /*************************************************************/
  /***** Populate MAC_L1_PSHO_REQ message *****/
  /***** Populate psho_control_params structure*****/
  /*************************************************************/
  /**** psho_tbf_type ****/
  psho_req_ptr->psho_tbf_type = global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type;
  if(ps_handover_resources_ptr->si == 0)
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO SYNC type is PSHO NON SYNC");
    psho_req_ptr->psho_sync_type = PSHO_NON_SYNC;
    global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type = PSHO_NON_SYNC;
  }
  else if(ps_handover_resources_ptr->si == 1)
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO SYNC TYPE is PSHO SYNC");
    psho_req_ptr->psho_sync_type = PSHO_SYNC;
    global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type = PSHO_SYNC;
  }
  else if(ps_handover_resources_ptr->si == 2)
  {
    MSG_GERAN_HIGH_0_G("GMAC PSHO SYNC TYPE is PSHO PRE SYNC");
    psho_req_ptr->psho_sync_type = PSHO_PRE_SYNC;
    global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type = PSHO_PRE_SYNC;
  }
  /**** normal_cell_indication ****/
  psho_req_ptr->normal_cell_indication = ps_handover_resources_ptr->nci;
  /**** psho_type ****/
  psho_req_ptr->psho_type = global_data_ptr->gmac_ps_ho.psho_control_params.psho_type;
  /**** arfcn ****/
  psho_req_ptr->psho_arfcn.num = ps_handover_resources_ptr->arfcn.num;
  psho_req_ptr->psho_arfcn.band = ps_handover_resources_ptr->arfcn.band;
  /**** bsic ****/
  psho_req_ptr->psho_bsic = ps_handover_resources_ptr->bsic;
  /**** handover_reference_present ****/
  psho_req_ptr->handover_reference_present = ps_handover_resources_ptr->handover_reference_present;

  if(psho_req_ptr->handover_reference_present)
  {
    psho_req_ptr->handover_reference = ps_handover_resources_ptr->handover_reference;
  }

  /*fill extended timing advance*/
  psho_req_ptr->pkt_ext_ta.pkt_ext_ta_present = ps_handover_resources_ptr->pkt_ext_timing_advance_present;

  if(psho_req_ptr->pkt_ext_ta.pkt_ext_ta_present)
  {
    psho_req_ptr->pkt_ext_ta.pkt_ext_ta = ps_handover_resources_ptr->pkt_ext_timing_advance;
  }

  /**************************************/
  /**** Fill up the DL TBF parameters ****/
  /**************************************/
  if(psho_req_ptr->psho_tbf_type == PSHO_UL_DL_TBF )
  {
    psho_req_ptr->dl_tbf.mac_mode = ps_handover_resources_ptr->ext_dynamic_allocation;

    /**** ts_allocation ****/
    psho_req_ptr->dl_tbf.ts_allocation = dl_assignment_ptr->timeslot_allocation;

    /**** downlink_pwr ****/
    psho_req_ptr->dl_tbf.downlink_pwr.params_valid = FALSE; /* Not present in mult DL assignment */

    /**** tbf_start_time_valid ****/
    psho_req_ptr->dl_tbf.tbf_start_time_valid = FALSE; /* No start time in PSHO COMMAND */

    /**** ta_params ****/
    if(ps_handover_resources_ptr->global_pkt_timing.ta_flag == valid)
    {
      psho_req_ptr->dl_tbf.ta_params.ta_value_ie.present = TRUE;
      psho_req_ptr->dl_tbf.ta_params.ta_value_ie.ta_value = ps_handover_resources_ptr->global_pkt_timing.ta_value;
    }
    else
    {
      psho_req_ptr->dl_tbf.ta_params.ta_value_ie.present = FALSE;
    }
    /**** global_pkt_timing ****/
    if(ps_handover_resources_ptr->global_pkt_timing.dwnlink_ta_flag == valid)
    {
      psho_req_ptr->dl_tbf.ta_params.cont_ta_ie.present = TRUE;
      psho_req_ptr->dl_tbf.ta_params.cont_ta_ie.ta_timeslot = ps_handover_resources_ptr->global_pkt_timing.dwnlink_ta_ts_num;
      psho_req_ptr->dl_tbf.ta_params.cont_ta_ie.tai = ps_handover_resources_ptr->global_pkt_timing.dwnlink_ta_index;
    }
    else
    {
      psho_req_ptr->dl_tbf.ta_params.cont_ta_ie.present = FALSE;
    }

    /**** power_control ****/
    psho_req_ptr->dl_tbf.power_control.present = FALSE; /* power control not in mult DL assignment */

    /**** dl_tfi ****/
    psho_req_ptr->dl_tbf.dl_tfi_present = TRUE;
    psho_req_ptr->dl_tbf.dl_tfi = dl_assignment_ptr->dl_tbf_assignment.tfi_assignment;
    /**** psho_dl_rlc_mode ****/
    if((ps_handover_resources_ptr->rlc_reset == PSHO_RLC_RESET)
      ||(global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate == TRANSFER_UL_DYNAMIC)
      ||(global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate == TRANSFER_INACTIVE))
    {
      /*network commanded reset of RLC or there is no dl tbf active
        This means gmac needs to take the rlc mode given in the message*/
      if (dl_assignment_ptr->dl_tbf_assignment.downlink_rlc_mode == 0)
      {
        global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_rlc_mode = RLC_ACK;
      }
      else
      {
        global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_rlc_mode = RLC_UNACK;
      }
    }
    else
    {
      /*network commanded no reset of RLC or there is an active dl tbf
        in the mobile. so ignore the rlc mode given in the message
        and use the existing rlc mode to populate psho_dl_rlc_mode param*/
      /**** rlc_mode ****/
      if (global_data_ptr->gmac_store_dl_rlc_mode == 0)
      {
        global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_rlc_mode = RLC_ACK;
      }
      else
      {
        global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_rlc_mode = RLC_UNACK;
      }
    }

    /* Set up EGPRS fields */
    if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
    {
      /**** bep_period2 from dl assignment structure****/
      if(ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.bep_period2_present_flag)
      {
        psho_req_ptr->dl_tbf.bep_period2.present = TRUE;
        psho_req_ptr->dl_tbf.bep_period2.bep_period2 = ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.bep_period2;
      }
      else
      {
        psho_req_ptr->dl_tbf.bep_period2.present = FALSE;
      }
      if(psho_req_ptr->dl_tbf.bep_period2.present == FALSE)
      {
        /**** There is no bep_period2 from dl assignment structure
         so take bep_period2 from ul assignment structure****/
        if(ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.bep_period2_present_flag)
        {
          psho_req_ptr->dl_tbf.bep_period2.present = TRUE;
          psho_req_ptr->dl_tbf.bep_period2.bep_period2 = ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.bep_period2;
        }
        else
        {
          psho_req_ptr->dl_tbf.bep_period2.present = FALSE;
        }
      }
      /**** tbf_mode ****/
      psho_req_ptr->dl_tbf.tbf_mode = TBF_MODE_EGPRS;

      /***** link_qual_meas_mode ****/
      psho_req_ptr->dl_tbf.link_qual_meas_mode = ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_dl_tbf.link_quality_meas_mode;
      /***** rlc_mode ****/
      psho_req_ptr->dl_tbf.rlc_mode = global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_rlc_mode;
    }
    else
    {
      /**** tbf_mode ****/
      psho_req_ptr->dl_tbf.tbf_mode = TBF_MODE_GPRS;
    }
  }
  /**************************************/
  /**** Set up the UL TBF parameters ****/
  /**************************************/
  /**** usf ****/
  /* return already checked in validate funciton*/
  (void)gmac_get_usf_from_psho_req(ps_handover_resources_ptr,
                                   psho_req_ptr,
                                   gas_id);
  /**** power_control_params ****/
  if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
  {
    timeslot_description_ptr = &(ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.timeslot_description);
  }
  else
  {
    timeslot_description_ptr = &(ps_handover_resources_ptr->tbf_mode.gprs_mode.timeslot_description);
  }
  if(timeslot_description_ptr->power_control_included == TRUE)
  {
    psho_pwr_cntl_ptr = &(timeslot_description_ptr->power_control_params);

    psho_req_ptr->ul_tbf.power_control_params.present   = TRUE;

    if(( timeslot_description_ptr->ms_timeslot_allocation & 0x80 ) != 0)
    {
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN0.present = TRUE;
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN0.gamma = psho_pwr_cntl_ptr->gamma_tn0;
    }

    if(( timeslot_description_ptr->ms_timeslot_allocation & 0x40 ) != 0)
    {
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN1.present = TRUE;
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN1.gamma = psho_pwr_cntl_ptr->gamma_tn1;
    }

    if(( timeslot_description_ptr->ms_timeslot_allocation & 0x20 ) != 0)
    {
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN2.present = TRUE;
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN2.gamma = psho_pwr_cntl_ptr->gamma_tn2;
    }

    if(( timeslot_description_ptr->ms_timeslot_allocation & 0x10 ) != 0)
    {
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN3.present = TRUE;
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN3.gamma = psho_pwr_cntl_ptr->gamma_tn3;
    }

    if(( timeslot_description_ptr->ms_timeslot_allocation & 0x08 ) != 0)
    {
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN4.present = TRUE;
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN4.gamma = psho_pwr_cntl_ptr->gamma_tn4;
    }

    if(( timeslot_description_ptr->ms_timeslot_allocation & 0x04 ) != 0)
    {
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN5.present = TRUE;
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN5.gamma = psho_pwr_cntl_ptr->gamma_tn5;
    }

    if(( timeslot_description_ptr->ms_timeslot_allocation & 0x02 ) != 0)
    {
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN6.present = TRUE;
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN6.gamma = psho_pwr_cntl_ptr->gamma_tn6;
    }

    if(( timeslot_description_ptr->ms_timeslot_allocation & 0x01 ) != 0)
    {
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN7.present = TRUE;
      psho_req_ptr->ul_tbf.power_control_params.gamma_TN7.gamma = psho_pwr_cntl_ptr->gamma_tn7;
    }

    psho_req_ptr->ul_tbf.power_control_params.alpha = psho_pwr_cntl_ptr->alpha;
  }
  else
  {
    psho_req_ptr->ul_tbf.power_control_params.present = FALSE;
  }

  /**** timing_advance_params ****/
  if(ps_handover_resources_ptr->global_pkt_timing.ta_flag == valid)
  {
    psho_req_ptr->ul_tbf.timing_advance_params.ta_value_ie.present = TRUE;
    psho_req_ptr->ul_tbf.timing_advance_params.ta_value_ie.ta_value = ps_handover_resources_ptr->global_pkt_timing.ta_value;
  }
  else
  {
    psho_req_ptr->ul_tbf.timing_advance_params.ta_value_ie.present = FALSE;
  }

  if(ps_handover_resources_ptr->global_pkt_timing.uplink_ta_flag == valid)
  {
    psho_req_ptr->ul_tbf.timing_advance_params.cont_ta_ie.present = TRUE;
    psho_req_ptr->ul_tbf.timing_advance_params.cont_ta_ie.ta_timeslot = ps_handover_resources_ptr->global_pkt_timing.uplink_ta_ts_num;
    psho_req_ptr->ul_tbf.timing_advance_params.cont_ta_ie.tai = ps_handover_resources_ptr->global_pkt_timing.uplink_ta_index;
  }
  else
  {
    psho_req_ptr->ul_tbf.timing_advance_params.cont_ta_ie.present = FALSE;
  }

  /**** usf granularity ****/
  psho_req_ptr->ul_tbf.usf_granularity =  ul_tbf_assignment_ptr->usf_granularity;

  /**** tbf_start_time_valid ****/
  psho_req_ptr->ul_tbf.tbf_start_time_valid = FALSE; /* No starting time in PSHO msg */

  /**** dl_ctrl_param ****/
  if(ps_handover_resources_ptr->pwr_control_present == TRUE )
  {
    psho_req_ptr->ul_tbf.dl_ctrl_param.params_valid = TRUE;
    psho_req_ptr->ul_tbf.dl_ctrl_param.po = ps_handover_resources_ptr->po;
    psho_req_ptr->ul_tbf.dl_ctrl_param.pr_mode = ps_handover_resources_ptr->pr_mode;
  }
  else
  {
    psho_req_ptr->ul_tbf.dl_ctrl_param.params_valid = FALSE;
  }

  /**** psho_ul_rlc_mode ****/
  if((ps_handover_resources_ptr->rlc_reset == PSHO_RLC_RESET)
     ||(global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate == TRANSFER_DL)
     ||(global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate == TRANSFER_INACTIVE))
  {
    /*network commanded reset of RLC or there is no ul tbf active
      This means gmac needs to take the rlc mode given in the message*/
    if(ul_tbf_assignment_ptr->rlc_mode == GRLC_MODE_ACK)
    {
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_rlc_mode = RLC_ACK;
    }
    else
    {
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_rlc_mode = RLC_UNACK;
    }
  }
  else
  {
    /*network commanded no reset of RLC or there is an active ul tbf
      in the mobile. so ignore the rlc mode given in the message
      and use the existing rlc mode to populate psho req*/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_rlc_mode = (mac_rlc_mode_T)global_data_ptr->gmac_local_stored_ul_chan_req.rlc_mode;
  }

  /* Set up EGPRS fields */
  if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
  {
    /**** bep_period2 ****/
    if(ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.bep_period2_present_flag)
    {
      psho_req_ptr->ul_tbf.bep_period2.present = TRUE;
      psho_req_ptr->ul_tbf.bep_period2.bep_period2 = ps_handover_resources_ptr->tbf_mode.egprs_mode.egprs_ul_tbf.bep_period2;
    }
    else
    {
      psho_req_ptr->ul_tbf.bep_period2.present = FALSE;
    }

    /**** tbf_mode ****/
    psho_req_ptr->ul_tbf.tbf_mode = TBF_MODE_EGPRS;
    /**** rlc_mode ****/
    psho_req_ptr->ul_tbf.rlc_mode = global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_rlc_mode;
  }
  else
  {
    psho_req_ptr->ul_tbf.tbf_mode = TBF_MODE_GPRS;
  }

  /**** test_mode ****/
  psho_req_ptr->ul_tbf.test_mode = global_data_ptr->current_test_mode;

  /**** ts_offest ****/
  psho_req_ptr->ul_tbf.ts_offset = global_data_ptr->gmac_test_mode_ts_offset;

  /*** srb mode ****/
  psho_req_ptr->ul_tbf.srb_mode = global_data_ptr->gmac_test_mode_srb_mode;

  /**** mac_mode ****/
#ifdef FEATURE_GSM_DTM
  psho_req_ptr->ul_tbf.mac_mode = ps_handover_resources_ptr->ext_dynamic_allocation;
#endif /*FEATURE_GSM_DTM*/

  /*************************************************************/
  /*********** Populate the MR_UL_ALLOC_CNF message ************/
  /*************************************************************/
  if(( psho_req_ptr->psho_tbf_type == PSHO_UL_DL_TBF)||
       ( psho_req_ptr->psho_tbf_type == PSHO_UL_TBF))
  {
    memset(&(global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info),0,
           sizeof(global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info));

    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.sig = MR_UL_ALLOC_CNF;

    if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
    {
      /**** tbf_mode ****/
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.tbf_mode = TBF_MODE_EGPRS;
    }
    else
    {
      /**** tbf_mode ****/
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.tbf_mode = TBF_MODE_GPRS;
    }

#ifdef FEATURE_GSM_DTM
    /**** dtm_access ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.dtm_access = FALSE;
#endif /*FEATURE_GSM_DTM*/

    /* Get the UL timeslot alloc for use later*/
    (void)gmac_get_ul_ts_alloc_from_psho_req(ps_handover_resources_ptr, &ul_ts_alloc, gas_id);

    /* Set up EGPRS parameters */
    if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
    {
      /**** mcs ****/
      /* return already checked in validate funciton*/
      (void)gmac_get_egprs_mcs_from_psho_req(ps_handover_resources_ptr,
                                             &global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.mcs,
                                             gas_id);

      /**** resegment ****/
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.resegment = 0;

      /**** ws ****/
      if(gmac_get_ul_ws_from_psho_req(ps_handover_resources_ptr, &ul_egprs_ws, gas_id) == TRUE)
      {
        /**** There is a WS given in the assignment use this ws to calculate the actual ws value ****/
        decoded_ul_ws = ((ul_egprs_ws *32) + 64);
      }
      else
      {
        /**** There is no window size given in the assignment so use the current window size ****/
        decoded_ul_ws = global_data_ptr->current_egprs_ul_ws;
      }
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.ws = decoded_ul_ws;
    }

    /**** access_mode ****/
    if(ps_handover_resources_ptr->ext_dynamic_allocation == TRUE)
    {
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.access_mode = RM_EXT_DYNAMIC_ALLOC;
    }
    else
    {
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.access_mode = RM_DYNAMIC_ALLOC;
    }

    /**** access_phase ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.access_phase = GMAC_NO_PHASE_ACCESS;

    /**** tbf_type ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.tbf_type = RM_OPEN_ENDED_TBF;
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.blocks_granted = 0;

    /**** cs ****/
    if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_GPRS)
    {
      /* return already checked in validate funciton*/
      (void)gmac_get_cs_from_psho_req(ps_handover_resources_ptr,&coding_scheme,gas_id);
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.cs = coding_scheme;
    }

    /**** ti ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.ti = 0; /* Unused */

    /**** tlli ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.tlli = 0; /*Unused*/

    /**** tlli_block_cs ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.tlli_block_cs = RM_TLLI_USE_CS_COMMANDED;

    /**** retry ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.retry = 0; /* Unused */

    /**** ul_tfi ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.ul_tfi = ul_tbf_assignment_ptr->tfi_assignment;

    /**** num_timeslots ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.num_timeslots = gmac_utils_calculate_nts(ul_ts_alloc,gas_id);

    /**** final_alloc ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.final_alloc = 0; /* Fixed alloc only */

    /**** usf_granularity ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_info.usf_granularity = ul_tbf_assignment_ptr->usf_granularity;
  }
  /**** psho_assign_ul_tfi ****/
  global_data_ptr->gmac_ps_ho.psho_config_params.psho_assign_ul_tfi = ul_tbf_assignment_ptr->tfi_assignment;
  /*************************************************************/
  /**************** Populate psho_dl_ass_info structue**********/
  /*************************************************************/
  if(psho_req_ptr->psho_tbf_type == PSHO_UL_DL_TBF)
  {

    global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_present = TRUE;

    /**** Populate dl assign ind parameters ****/
    if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_EGPRS)
    {

      if(gmac_get_dl_ws_from_psho_req(ps_handover_resources_ptr, &dl_egprs_ws, gas_id) == TRUE)
      {
        /**** There is a WS given in the assignment use this ws to calculate the actual ws value ****/
        global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_info.ws = ((dl_egprs_ws *32) + 64);
      }
      else
      {
        /**** There is no window size given in the assignment so use the current window size ****/
        global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_info.ws = global_data_ptr->current_egprs_dl_ws;
      }
      /**** tbf_mode****/
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_info.tbf_mode = TBF_MODE_EGPRS;
    }
    else if(ps_handover_resources_ptr->type_of_mode == PSHO_TBF_MODE_GPRS)
    {

      /**** tbf_mode****/
      global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_info.tbf_mode = TBF_MODE_GPRS;
    }


    /**** dl ass ind needs to be sent to GRLC after ps ho successful. so fill  psho_dl_ass_info structure****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_info.sig = MR_DL_ASS_IND;
    /**** rlc_mode****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_info.rlc_mode = (uint8)global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_rlc_mode;
    /**** tlli****/
    rr_read_public_store(RRPS_TLLI,&(global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_info.tlli), gas_id);


    MSG_GERAN_MED_2_G("ps HO  map dl ts = 0x%x ,  num dl ts %d ",
                      global_data_ptr->gmac_local_dl_ts_alloc,
                      gmac_utils_calculate_nts(global_data_ptr->gmac_local_dl_ts_alloc,gas_id));
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_ass_info.num_dl_ts = gmac_utils_calculate_nts(global_data_ptr->gmac_local_dl_ts_alloc,gas_id);

    /**** psho_assign_dl_tfi ****/
    global_data_ptr->gmac_ps_ho.psho_config_params.psho_assign_dl_tfi = dl_assignment_ptr->dl_tbf_assignment.tfi_assignment;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_util_ps_ho_req_handler()
===
===  DESCRIPTION
===  Handles the PS HANDOVER COMMAND message.  Performs validation of PS HANDOVER
===  COMMAND message. if validation success then it builds the MAC_L1_PSHO_REQ message
===  and send this message to L1. if validation fails then it sends PSHO STATUS with
===  FAILURE to GRR.
===
===  CTRL_ACK handling:
===  The requirements for CTRL_ACK are in 44.060 11.2.37.2.
===  If CTRL_ACK is set then MAC will check that T3192 is running, the TFI
===  specified matches and that the PSHO is received on the DL PACCH.  If this
===  is the case then the DL TBF is released.
===
===  If CTRL_ACK is not set but T3192 is running then GMAC ignores PSHANDOVER command and
===  sends failure to GRR
===
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    boolean (TRUE or FALSE)
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_util_ps_ho_req_handler(gprs_mac_transfer_substates_t mac_transfer_substate,
                                    gas_id_t                      gas_id)
{
  /* Local Data */
  ps_handover_command_t              *ps_handover_ptr       = NULL;
  boolean                             ctrl_ack_set          = FALSE;
  ps_handover_to_a_gb_mode_payload_t *a_gb_ptr              = NULL;
  mac_l1_sig_t                        mac_l1_msg;
  mac_l1_psho_req_t                  *psho_req_ptr          = &(mac_l1_msg.mac_l1_psho_req);
  gmac_pkt_ps_ho_result_t             gmac_pkt_ps_ho_result = PSHO_VALIDATION_PASS;
  gmac_global_data_t                 *global_data_ptr = gmac_get_global_data_ptr(gas_id);
#ifdef PERLUTF
  #error code not present
#else
  ps_handover_ptr = &(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_psho_req.psho_cmd);
#endif


  if(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_psho_req.psho_type == PSHO_GTOG)
  {
    if(ps_handover_ptr->target_type == CSN_PSHO_GSM_TARGET_CELL)
    {
      /* Initialise PSHO STRUCTURE*/
      gmac_util_initialise_psho_structure(gas_id);
      /* Set psho type and psho_initiated_tran_substate flags*/
      global_data_ptr->gmac_ps_ho.psho_control_params.psho_type = global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_psho_req.psho_type;
      global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate = mac_transfer_substate;

      a_gb_ptr = &(ps_handover_ptr->payload.a_gb);

      /*populate PCCF parameters*/
      global_data_ptr->gmac_ps_ho.psho_pccf_params.psho_arfcn.num = a_gb_ptr->ps_handover_radio_resources.arfcn.num;
      global_data_ptr->gmac_ps_ho.psho_pccf_params.psho_arfcn.band = a_gb_ptr->ps_handover_radio_resources.arfcn.band;
      global_data_ptr->gmac_ps_ho.psho_pccf_params.psho_bsic  = a_gb_ptr->ps_handover_radio_resources.bsic;

#ifdef PERLUTF
    #error code not present
#endif
      /*validating PSHO COMMAND message*/
      gmac_pkt_ps_ho_result = gmac_util_psho_validate_psho_req(&(a_gb_ptr->ps_handover_radio_resources),&ctrl_ack_set,gas_id);

      if ( gmac_pkt_ps_ho_result == PSHO_VALIDATION_PASS )
      {
        MSG_GERAN_MED_0_G("MAC PSHO message validation successful");
        global_data_ptr->gmac_ps_ho.psho_config_params.psho_reset_flag = a_gb_ptr->ps_handover_radio_resources.rlc_reset;

        global_data_ptr->gmac_ps_ho.psho_config_params.psho_nas_container_flag = a_gb_ptr->nas_container_present;
        /*validation successful so populating the MAC L1 PSHO REQ message*/
        gmac_util_psho_populate_psho_req_config_msgs(&(a_gb_ptr->ps_handover_radio_resources),psho_req_ptr,gas_id);

        if(ctrl_ack_set == TRUE)
        {
          MSG_GERAN_MED_0_G("MAC control ack is set in the PSHO message. so intiating DL release procedure");
          /*CONTROL ACK is set means GMAC needs to release DL TBF immediately
            so GMAC sends dl release to RLC and DL release req to L1*/
          gmac_transfer_dl_release_clean(gas_id);
          global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;

          gmac_util_send_dl_release_to_rlc(gas_id);

          gmac_util_send_mac_l1_release(DL_TBF_NORMAL,gas_id);

          /*control_ack_set flag in psho_control_params structure needs to set here.
            This flag prevents of sending DL release ind to GRLC after PSHO Procedure
            successful with only UL TBF.*/
          global_data_ptr->gmac_ps_ho.psho_control_params.control_ack_set = TRUE;
        }
        else
        {
          global_data_ptr->gmac_ps_ho.psho_control_params.control_ack_set = FALSE;
        }
        /* Send MAC_L1_FREQ_CONFIG */
        (void)gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);

        global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate = PSHO_WAITING_FOR_L1_CNF;

        MSG_GERAN_MED_0_G("GMAC PSHO State = PSHO_WAITING_FOR_L1_CNF ");

        rr_read_public_store(RRPS_CELL_OPTIONS_ACCESS_BURST_TYPE,&mac_l1_msg.mac_l1_psho_req.psho_access_burst_type,gas_id);

        /* send MAC_L1_PSHO_REQ with parameters */
        mac_l1_msg.mac_l1_psho_req.message_header.message_set = MS_MAC_L1;
        mac_l1_msg.mac_l1_psho_req.message_header.message_id =
        (uint8)MAC_L1_PSHO_REQ;
        gprs_l1_mac_acc_func(&mac_l1_msg,gas_id);
#ifdef PERLUTF
        #error code not present
#endif
        return TRUE;
      }
      else
      {
        MSG_GERAN_MED_0_G("MAC PSHO message validation failes so set pccf cause");
        global_data_ptr->gmac_ps_ho.psho_pccf_params.cause = PSHO_FAILURE_OTHERS;
        /*validation fails so send PSHO CNF with PSHO FAILURE to GRR and reset PSHO structure*/
        gmac_util_send_grr_psho_cnf(PSHO_FAILURE,gas_id);
#ifdef PERLUTF
        #error code not present
#endif
        return FALSE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("MAC Rcvd psho type is GTOG/WTOG cell but PS HO COMMAND does not have GSM/GPRS resources");
    }
  }
  return TRUE;
}
#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/
/*===========================================================================
===
===  FUNCTION      gmac_util_send_grr_psho_cnf()
===
===  DESCRIPTION
===  This function sends MAC_GRR_PSHO_CNF signal to GRR
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
void gmac_util_send_grr_psho_cnf(psho_status_t psho_status,
                                 gas_id_t      gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_1_G("MAC Sent MAC_GRR_PSHO_CNF %d",psho_status);

  global_data_ptr->grr_sig.mac_grr_psho_cnf.psho_status = psho_status;
  global_data_ptr->grr_sig.mac_grr_psho_cnf.message_header.message_set = MS_MAC_RR;
  global_data_ptr->grr_sig.mac_grr_psho_cnf.message_header.message_id = (uint8)MAC_GRR_PSHO_CNF;

  gprs_grr_mac_acc_func(&global_data_ptr->grr_sig, gas_id);
}
/*===========================================================================
===
===  FUNCTION      gmac_util_initialise_psho_structure()
===
===  DESCRIPTION
===  This function initialises the gmac_ps_ho structure
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
void gmac_util_initialise_psho_structure(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC initialising PSHO STRUCTURE");

  global_data_ptr->gmac_ps_ho.psho_control_params.tbf_rel_cnf_ptr = NULL;

  global_data_ptr->gmac_ps_ho.psho_control_params.ul_release_ind_ptr = NULL;

  global_data_ptr->gmac_ps_ho.psho_control_params.dl_release_ind_ptr = NULL;

  global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate = PSHO_NOT_ACTIVE;
  MSG_GERAN_MED_0_G("GMAC PSHO State = PSHO_NOT_ACTIVE ");

  global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate = TRANSFER_INACTIVE;

  global_data_ptr->gmac_ps_ho.psho_control_params.psho_type = PSHO_NONE;

  global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type = PSHO_NO_TBF;

  global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type = PSHO_NO_SYNC;

  memset( &(global_data_ptr->gmac_ps_ho.psho_config_params), 0, sizeof(gmac_psho_config_params_t) );
  memset( &(global_data_ptr->gmac_ps_ho.psho_pccf_params), 0, sizeof(gmac_psho_pccf_params_t) );
}
/*===========================================================================
===
===  FUNCTION      gmac_util_reset_psho_structure()
===
===  DESCRIPTION
===  This function reset the gmac_ps_ho structure
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
void gmac_util_reset_psho_structure(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC reset PSHO STRUCTURE");

  if(global_data_ptr->gmac_ps_ho.psho_control_params.tbf_rel_cnf_ptr != NULL)
  {
    GPRS_MEM_FREE(global_data_ptr->gmac_ps_ho.psho_control_params.tbf_rel_cnf_ptr);
    global_data_ptr->gmac_ps_ho.psho_control_params.tbf_rel_cnf_ptr = NULL;
  }
  if(global_data_ptr->gmac_ps_ho.psho_control_params.ul_release_ind_ptr != NULL)
  {
    GPRS_MEM_FREE(global_data_ptr->gmac_ps_ho.psho_control_params.ul_release_ind_ptr);
    global_data_ptr->gmac_ps_ho.psho_control_params.ul_release_ind_ptr = NULL;
  }
  if(global_data_ptr->gmac_ps_ho.psho_control_params.dl_release_ind_ptr != NULL)
  {
    GPRS_MEM_FREE(global_data_ptr->gmac_ps_ho.psho_control_params.dl_release_ind_ptr);
    global_data_ptr->gmac_ps_ho.psho_control_params.dl_release_ind_ptr = NULL;
  }

  global_data_ptr->gmac_ps_ho.psho_control_params.psho_substate = PSHO_NOT_ACTIVE;

  global_data_ptr->gmac_ps_ho.psho_control_params.psho_initiated_tran_substate = TRANSFER_INACTIVE;
  global_data_ptr->gmac_ps_ho.psho_control_params.psho_type = PSHO_NONE;
  global_data_ptr->gmac_ps_ho.psho_control_params.psho_sync_type = PSHO_NO_SYNC;
  global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type = PSHO_NO_TBF;
  memset( &(global_data_ptr->gmac_ps_ho.psho_config_params), 0, sizeof(gmac_psho_config_params_t) );
  memset( &(global_data_ptr->gmac_ps_ho.psho_pccf_params), 0, sizeof(gmac_psho_pccf_params_t) );
}
/*===========================================================================
===
===  FUNCTION      gmac_util_mac_control_header_pkt_phy_info_tfi_checking()
===
===  DESCRIPTION
===  This function validates the tfi checking in the header of PKT PHYSICAL
===  INFORMATION message
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  mac_header_tfi_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
mac_header_tfi_t gmac_util_mac_control_header_pkt_phy_info_tfi_checking(gas_id_t gas_id)
{
  mac_header_tfi_t match_found = MAC_HEADER_TFI_NOT_VALID;
  uint8 dl_ctrl_block_tfi = 0;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /*if payload type bits  = "10" and AC bit = "1" check TFI value in MAC header*/
  MSG_GERAN_MED_3_G("MAC info MAC ctrl headers 0x%x 0x%x 0x%x",
                    global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0],
                    global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1],
                    global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2]);

  /*if payload type bits  = "10" and AC bit = "1" check TFI value in MAC header*/
  if (  (((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0xc0) >> 6) == 0x02)
        && ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] & 0x1) == 0x01)  )
  {

     if ( ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] & 0x1) == 0x01)
           && ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] & 0x80) == 0) )
     {
       MSG_GERAN_MED_0_G("MAC info Unsegmented msg, optional octets used for PWR ");
     }


     if ( global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot > 7 )
     {
       MSG_GERAN_ERROR_1_G("GMAC INFO timeslot returned by L1 is out of range: 0x%x",
                           global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot);
     }

     /*contians the d bit*/
     dl_ctrl_block_tfi = (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2] & 0x3f );

     /*use tfi rcvd in the mac control header for the DL mac control block*/
     if( (dl_ctrl_block_tfi & 0x01) == 1)
     {

        MSG_GERAN_MED_1_G("MAC info MAC control header contains DL TFI = %d ",((dl_ctrl_block_tfi & 0x3e)>> 1));

        /*           check dl_tfi matches              */
        if( (global_data_ptr->gmac_ps_ho.psho_config_params.psho_assign_dl_tfi == ((dl_ctrl_block_tfi & 0x3e)>> 1))
            && (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE))
        {
            if ( ( ((uint8)(0x80 >> global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot)) &
                   global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_assign_ts ) == 0 )
            {
               /* Ctrl msg with DL TFI is received on a PDCH that is NOT in the DL TBF */
               match_found = MAC_HEADER_TFI_MATCH_NOT_FOUND;
               MSG_GERAN_HIGH_3_G("GMAC DL TFI rxd on TS NOT in DL TBF, rxd TS:%d, DL TFI:%d, DL TS curr bmap:0x%x",
                                  global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot,
                                  global_data_ptr->gmac_ps_ho.psho_config_params.psho_assign_dl_tfi,
                                  global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_assign_ts);
            }
            else
            {
               match_found = MAC_HEADER_TFI_MATCH_FOUND;
            }
        }
        else
        {
           /*With payload type of "10" and RBSN of "0" control ack value is "10"*/
           match_found = MAC_HEADER_TFI_MATCH_NOT_FOUND;
           MSG_GERAN_HIGH_1_G("ERROR :MT= %d  DL_TFI mismatch in DL mac ctrl header",global_data_ptr->csn_dec_hdr_ptr->message_type);
        }
     }
     else
     {
        MSG_GERAN_MED_1_G("MAC info MAC control header contains UL TFI = %d ",((dl_ctrl_block_tfi & 0x3e)>> 1));

        /*           check ul_tfi matches              */
        if((global_data_ptr->gmac_ps_ho.psho_config_params.psho_assign_ul_tfi == ((dl_ctrl_block_tfi & 0x3e)>> 1))
            && (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE))
        {
          if ( ( ((uint8)(0x80 >> global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot)) &
                 global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_ts ) == 0 )
          {
             /* Ctrl msg with UL TFI is received on a PDCH that is NOT in the UL TBF */
             match_found = MAC_HEADER_TFI_MATCH_NOT_FOUND;
             MSG_GERAN_HIGH_3_G("GMAC UL TFI rxd on TS NOT in UL TBF, rxd TS:%d, UL TFI:%d, UL TS curr bmap:0x%x",
                                global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot,
                                global_data_ptr->gmac_ps_ho.psho_config_params.psho_assign_ul_tfi,
                                global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_ts);
          }
          else
          {
             match_found = MAC_HEADER_TFI_MATCH_FOUND;
          }
        }
        else
        {
          match_found = MAC_HEADER_TFI_MATCH_NOT_FOUND;
          MSG_GERAN_HIGH_1_G("ERROR :MT= %d  UL_TFI mismatch in DL mac ctrl header",global_data_ptr->csn_dec_hdr_ptr->message_type);
        }
     }
  }
  return(match_found);
}
/*===========================================================================
===
===  FUNCTION      gmac_util_check_pkt_phy_info_global_tfi()
===
===  DESCRIPTION
===  This function validates the tfi received in the global tfi field in
===  PKT PHYSICAL INFORMATION message
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  mac_header_tfi_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_util_check_pkt_phy_info_global_tfi(global_tfi_t rxd_global_tfi,
                                                gas_id_t     gas_id)
{
  uint8 tfi_match = FALSE;

  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  mac_header_tfi_check = gmac_util_mac_control_header_pkt_phy_info_tfi_checking(gas_id);
  if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
  {
    if (rxd_global_tfi.direction == uplink_valid)
    {
      if (rxd_global_tfi.uplink_tfi == global_data_ptr->gmac_ps_ho.psho_config_params.psho_assign_ul_tfi)
      {
        if ( ( ((uint8)(0x80 >> global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot)) &
               global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_ts ) == 0 )
        {
          /* Ctrl msg with UL TFI is received on a PDCH that is NOT in the UL TBF */
          tfi_match = FALSE;
          MSG_GERAN_HIGH_3_G("GMAC UL TFI rxd on TS NOT in UL TBF, rxd TS:%d, UL TFI:%d, UL TS curr bmap:0x%x",
                             global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot,
                             rxd_global_tfi.uplink_tfi,
                             global_data_ptr->gmac_ps_ho.psho_config_params.psho_ul_alloc_ts);
        }
        else
        {
          tfi_match = TRUE;
        }
      }
    }
    if (rxd_global_tfi.direction == dwnlink_valid)
    {
      if(global_data_ptr->gmac_ps_ho.psho_control_params.psho_tbf_type == PSHO_UL_DL_TBF)
      {
        if (rxd_global_tfi.dwnlink_tfi == global_data_ptr->gmac_ps_ho.psho_config_params.psho_assign_dl_tfi)
        {
          if ( ( ((uint8)(0x80 >> global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot)) &
                 global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_assign_ts ) == 0 )
          {
             /* Ctrl msg with DL TFI is received on a PDCH that is NOT in the DL TBF */
             tfi_match = FALSE;
             MSG_GERAN_HIGH_3_G("GMAC DL TFI rxd on TS NOT in DL TBF, rxd TS:%d, DL TFI:%d, DL TS curr bmap:0x%x",
                                global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot,
                                rxd_global_tfi.dwnlink_tfi,
                                global_data_ptr->gmac_ps_ho.psho_config_params.psho_dl_assign_ts);
          }
          else
          {
            tfi_match = TRUE;
          }
        }
      }
    }
  }
  else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
  {
    tfi_match = TRUE;
  }
  else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
  {
    tfi_match = FALSE;
  }

  return (tfi_match);
}
#endif /*FEATURE_GPRS_PS_HANDOVER*/

/*===========================================================================
===
===  FUNCTION  gmac_util_send_grr_mac_rach_rej
===
===  DESCRIPTION
===  Send MAC_GRR_RACH_REJ to GRR.
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
void gmac_util_send_grr_mac_rach_rej(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Sent MAC_GRR_RACH_REJ");
  global_data_ptr->grr_sig.mac_grr_mac_status.message_header.message_set = MS_MAC_RR;
  global_data_ptr->grr_sig.mac_grr_mac_status.message_header.message_id = (uint8)MAC_GRR_RACH_REJ;
  gprs_grr_mac_acc_func(&global_data_ptr->grr_sig, gas_id);
}

/*===========================================================================
===
===  FUNCTION      gmac_access_func_get_num_dl_timeslots()
===
===  DESCRIPTION
===
===    MAC access function for RLC.
===    Returns the number of DL timeslots currently assigned in MAC.
===    Note that this access function contains a loop.
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    uint8 - Number of DL timeslots (range 0..8)
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 gmac_access_func_get_num_dl_timeslots (gas_id_t gas_id)
{
  uint8 num_ts = 0;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  num_ts = gmac_utils_calculate_nts(global_data_ptr->gmac_local_dl_ts_alloc,gas_id);

  return num_ts;
}

/*===========================================================================
===
===  FUNCTION      gmac_match_old_or_new_tlli()
===
===  DESCRIPTION
===
===    Match old (RRPS_TLLI_OLD) or new (RRPS_TLLI) TLLI from the public store.
===
===  PARAMETERS
===
===    uint32 tlli  - TLLI to be matched.
===
===  RETURN VALUE
===
===    boolean - Address matches old or new TLLI
===
===  SIDE EFFECTS
===
===    Calls rr_read_public_store().
===
===========================================================================*/
boolean gmac_match_old_or_new_tlli(uint32 tlli,
                                   gas_id_t gas_id)
{
  uint32 local_rrps_tlli = 0;
  uint32 old_local_rrps_tlli = 0;
  boolean addr_match = FALSE;


  if(tlli == (uint32)0xFFFFFFFF)
  {
    MSG_GERAN_HIGH_1_G("GMAC MSG TLLI %x, invalid",tlli);
  }
  else
  {
    /**** Check TLLI matches with new local TLLI store ****/
    rr_read_public_store(RRPS_TLLI,&local_rrps_tlli, gas_id);

    MSG_GERAN_HIGH_2_G("GMAC MSG TLLI %x, local_rrps_tlli %x",tlli,local_rrps_tlli);

    /*check tlli matches*/
    if(tlli == local_rrps_tlli)
    {
      MSG_GERAN_HIGH_0_G("GMAC Info TLLI matched");
      addr_match = TRUE;
    }
    else
    {
      /**** Check TLLI matches with old local TLLI store ****/
      rr_read_public_store(RRPS_TLLI_OLD,&old_local_rrps_tlli, gas_id);

      MSG_GERAN_HIGH_2_G("GMAC MSG TLLI %x, old_local_rrps_tlli %x",tlli,old_local_rrps_tlli);

      /*check tlli matches*/
      if(tlli == old_local_rrps_tlli)
      {
        MSG_GERAN_HIGH_0_G("GMAC Info TLLI matched");
        addr_match = TRUE;
      }
    }
  }

  return addr_match;
}

/*===========================================================================
===
===  FUNCTION    gmac_util_egprs_mcs_valid()
===
===  DESCRIPTION
===  Checks that an EGPRS Modulation Coding Scheme (MCS) can be supported by
===  the MS supported and cell supported options.
===
===  PARAMS
===  uint8 egprs_mcs - encoded egprs mcs value (see 44.060 12.10d)
===
===  RETURN VALUE
===  boolean - TRUE EGPRS MCS Valid
===            FALSE EGPRS MCS Invalid
===
===  SIDE EFFECTS
===  Calls to read_public_store for RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL,
===  RRPS_MS_EGPRS_8PSK_UL_ENABLED.
===
===========================================================================*/
boolean gmac_util_egprs_mcs_valid(uint8 egprs_mcs,
                                  gas_id_t gas_id)
{
  #define EGPRS_MCS_4 0x03
  #define EGPRS_MCS_6_9 0x0a

  boolean egprs_capable_cell = FALSE;
  boolean eight_psk_ul_capable = FALSE;
  boolean mcs_valid = FALSE;

  rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL, &egprs_capable_cell, gas_id );
  rr_read_public_store( RRPS_MS_EGPRS_8PSK_UL_ENABLED, &eight_psk_ul_capable, gas_id);

  if ( egprs_capable_cell == TRUE )
  {
    if ( egprs_mcs > EGPRS_MCS_6_9 )
    {
      MSG_GERAN_ERROR_1_G("GMAC EGPRS MCS Invalid, Out of Range MCS: 0x%x",egprs_mcs);
      mcs_valid = FALSE;
    }
    else if ( egprs_mcs > EGPRS_MCS_4 )
    {
      if ( eight_psk_ul_capable == TRUE )
      {
        mcs_valid = TRUE; /*MCS5-9 require 8PSK UL capability*/
      }
      else
      {
        MSG_GERAN_ERROR_1_G("GMAC EGPRS MCS Invalid, 8PSK in UL not supported for MCS: 0x%x",egprs_mcs);
        mcs_valid = FALSE;
      }
    }
    else
    {
      mcs_valid = TRUE; /*MCS1-4 don't require 8PSK UL capability*/
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GMAC EGPRS MCS invalid, EGPRS not supported in cell",egprs_mcs);
    mcs_valid = FALSE; /*EGPRS must be capable in cell to receive an EGPRS ass*/
  }

  return mcs_valid;
}

/*===========================================================================
===
===  FUNCTION      gmac_eng_mode_update_tbf_info()
===
===  DESCRIPTION
===
===  Updates the eng mode when either tbf mode changes or mac mode
===
===  PARAMETERS
===
===     update_tbf_mode       = GPRS or EGPRS and invlaid = 0xFF
===     update_mac_mode  = Dynamic or extended dynamic and invlaid = 0xff
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/

 void gmac_eng_mode_update_tbf_info(
                                    uint8       update_mac_mode,
                                    tbf_mode_T  update_tbf_mode,
                                    boolean     set_to_invalid,
                                    gas_id_t    gas_id
                                  )
{
  #define INVALID     0xff
  eng_mode_tbf_info_t tbf_info = {0};

  if (set_to_invalid == TRUE)
  {
      tbf_info.mac_mode = INVALID;
      tbf_info.tbf_mode = INVALID;
  }
  else
  {
      tbf_info.mac_mode = update_mac_mode;
      tbf_info.tbf_mode = (uint8)update_tbf_mode;
  }

  geran_eng_mode_data_write( ENG_MODE_TBF_INFO,(void*)&tbf_info,TRUE,gas_id);

}

/*===========================================================================
===
===  FUNCTION      gmac_restart_ul_await_ass_guard
===
===  DESCRIPTION
===
===  Takes the starting times rcvd in IA and converts it to an absolute FN.
===  Uses this with the current FN to determine the time in milliseconds
===  to the staring of the UL tbf being established from NULL mode.
===
===  Calculate the starting time in milliseconds.
===
===  adjusts the starting time from mod42432 format to the absolute
===  FN as specified in (GSM 04.18 10.5.2.38)
===
===  NOTE: THIS FUNCTION ONLY CONVERTS STARTING TIMES RCVD IN IA'S
===
===  NOTE: FOR gprs and EGPRS there will always be a valid starting time
===        present in the IA for SB.
===
===        This is not the case for a dynamic allocation. Where it is
===        possible to receive an IA that does not have a staring time.
===        In this case the default is 13frame minimum reaction time.
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/

void gmac_restart_ul_await_ass_guard
(
boolean starting_time_present,
uint32 starting_time_fn,
gas_id_t gas_id
)
{

   #define  MODULO42432  42432U
   #define  GMAC_TEMP_DELAY_OS  4
   #define  FIVE_SECONDS_WIGGLE_ROOM 5000

   uint32   st_minus_fn_mod42432;       /* Used to store the ST - FNmod42432 value */
   boolean  start_time_elapsed = FALSE; /* Used to return ST elapsed decision to caller */
   uint32  time_in_ms =0;
   uint32  current_fn = 0;

   uint32 FNmod42432 = 0;

   /*get current FN to be used*/
   current_fn = GSTMR_GET_FN_GERAN(gas_id);

  /*************************************************************
  * convert starting time rcvd in IA into to an absolute FN    *
  **************************************************************/
  FNmod42432 = ( current_fn % MODULO42432);

  /*If no starting time then the minimum response time is 13 frames according to the spec.*/
  if(starting_time_present == FALSE)
  {
     starting_time_fn = ADD_FN( current_fn, 13 );
  }

  if( starting_time_fn < FNmod42432 )
  {
        /* Calculate the difference, no need for signed arithmetic */
        st_minus_fn_mod42432 = ( starting_time_fn + MODULO42432 ) - FNmod42432;

        if( st_minus_fn_mod42432 >= MODULO42432 )
        {
            st_minus_fn_mod42432 -= MODULO42432;
        }
  }
  else
  {
        /* Use the modulo operation just incase the start time
         * is not within the 42432 modulo frame range.
         */
        st_minus_fn_mod42432 = ( starting_time_fn - FNmod42432 ) % MODULO42432;
  }

  if( st_minus_fn_mod42432 <= 31623 )
  {
        /* This covers the range 0 to 31623 (inclusive) */

        /* starting time has NOT elapsed */
        starting_time_fn = ADD_FN( current_fn, st_minus_fn_mod42432 );
  }
  else
  {
        /* This covers the range 31624 to 32023 (inclusive )
         * and the range 32024 to 42431 ( inclusive ).
         */

        /* Warning: This is valid only when TEMP_DELAY_OS < 2715648 */
        starting_time_fn = ADD_FN( current_fn, GMAC_TEMP_DELAY_OS );

        start_time_elapsed = TRUE;
  }


   /**************************************************************
   *  calculate the time in milliseconds to the stating time.    *
   **************************************************************/

   if(start_time_elapsed == FALSE)
   {
     time_in_ms = (( SUB_FN( starting_time_fn, current_fn) * 5) + FIVE_SECONDS_WIGGLE_ROOM);

     MSG_GERAN_MED_3_G(" GMAC info ST_fn = %d  current_FN %d sub_fn %d ",
                starting_time_fn,current_fn,SUB_FN( starting_time_fn, current_fn));

     grlc_ul_restart_await_ass_guard(gas_id, time_in_ms);
   }
   else
   {
     MSG_GERAN_MED_0_G("GMAC UL ST rcvd in IA has elapsed. Don't restart guard timer");
   }
}

/*** EOF: don't remove! ***/
