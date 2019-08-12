/*****************************************************************************
***
*** TITLE
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_conn_establish.c#1 $
***
*** DESCRIPTION
***
***  Contains extensions to RR connection establishment to support GPRS
***
*** Copyright (c) 2001-2014 QUALCOMM Technologies, Incorporated.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "secapi.h"
#include <stdlib.h>            /* rand() */
#include "rr_seg_load.h"
#include "rr_control.h"
#include "rr_general.h"
#include "rr_l1_send.h"
#include "rr_l2.h"
#include "rr_l2_send.h"
#include "rr_mac_send.h"
#include "rr_gprs_defs.h"
#include "rr_defs.h"
#include "rr_gprs_conn_establish.h"
#include "rr_init.h"
#include "rr_decode_ie.h"
#include "gmac.h"
#include "rr_general.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "gmacgrrsig.h"
#include "ms.h"
#include "rr_mm_send.h"
#include "memory.h"
#include "rr_conn_establish.h"
#include "reg_sim.h"
#include "rr_sys_info_pacch.h"
#include "rr_classmark.h"
#include "rr_resel.h"
#include "rr_multi_sim.h"
#include "gprs_mem.h"
#include "rr_ps_access.h"
#include "rr_ps_domain.h"
#include "stringl/stringl.h"

#include "rr_nv.h"

/*****************************************************************************
***
***     Local Typedefs, #defines & Variables
***
*****************************************************************************/

typedef struct
{
  request_references_T two_part_req_refs;
  tbf_info_T           two_part_info;
  byte                 timing_advance;
  ia_rest_octets_t     ia_rest_octets;
} rr_tbf_est_data_t;

typedef struct
{
  rr_tbf_est_data_t *data_ptr;
  boolean            raching_in_progress;
} rr_tbf_est_static_data_t;

static rr_tbf_est_static_data_t  rr_tbf_est_static_data[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Function Prototypes
***
*****************************************************************************/

static void store_gprs_ia_mobile_alloc(
  frequency_list_T *frequency_list,
  uint8 hsn,
  const gas_id_t gas_id
);
static void process_indirect_freq(
  boolean *decoding_ok,
  boolean *waiting_for_2nd_msg,
  packet_channel_description_t *packet_channel_desc,
  frequency_list_T *frequency_list,
  const gas_id_t gas_id
);

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_ps_access_data_t*
 */
static rr_tbf_est_static_data_t *rr_tbf_est_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_tbf_est_static_data[as_index];
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)

static rr_event_T rr_gprs_ce_map_rr_ce_x2g_cco_failure_cause(
  rr_cco_status_e failure_cause
)
{
  switch (failure_cause)
  {
    case RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY:              return EV_X2G_CCO_FAILURE_CCO_TIMER_EXPIRY;
    case RR_CCO_STATUS_FAILURE_NO_RESPONSE:                   return EV_X2G_CCO_FAILURE_NO_RESPONSE;
    case RR_CCO_STATUS_FAILURE_IA_REJECT:                     return EV_X2G_CCO_FAILURE_IA_REJECT;
    case RR_CCO_STATUS_FAILURE_CONTENTION_RESOLUTION_FAILURE: return EV_X2G_CCO_FAILURE_CONTENTION_RESOLUTION_FAILURE;
    case RR_CCO_STATUS_FAILURE_DSF:                           return EV_X2G_CCO_FAILURE_DSF;
    default:
      MSG_GERAN_ERROR_1("Unexpected failure_cause: %d", failure_cause);
      return EV_NO_EVENT;
  }
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */


/*===========================================================================

FUNCTION grr_init_gprs_conn_establishment

DESCRIPTION
  Initialise the gprs connection establishment module

PARAMS
  None

RETURN VALUE
  None

===========================================================================*/
void grr_init_gprs_conn_establishment(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_tbf_est_static_data_t *rr_tbf_est_data_ptr = rr_tbf_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_tbf_est_data_ptr);

   memset(&rr_tbf_est_data_ptr->data_ptr->two_part_info,0,sizeof(tbf_info_T));
} /* grr_init_gprs_conn_establishment() */



/*===========================================================================

FUNCTION  grr_get_gprs_access_ref

DESCRIPTION
  Returns a GPRS request reference for use in the DL_RANDOM_ACCESS_REQ

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
byte grr_get_gprs_access_ref(
  byte access_reason,
  byte *random_ref_len
)
{
  byte access_ref = 0x00;

  uint8 random;

  if (secapi_get_random(SECAPI_SECURE_RANDOM, &random, 1) != E_SUCCESS)
  {
    ERR_GERAN_FATAL_0("ERROR secapi_get_random() returned FAILURE");
  }

  /**************************************************************/
  /* Access Reason is for Packet Access on CCCH                 */
  /**************************************************************/
  if (access_reason == GMAC_ONE_PHASE_ACCESS)
  {
    access_ref |= 0x78;

    /**************************************************************/
    /* Get a random number which will be used as a random         */
    /* reference. The latter has to be between 0 and 3 (2 bits)   */
    /**************************************************************/

    /* REFER TO 04.18 TABLE 9.1.8.1, CHANNEL REQUEST msg content */
    access_ref |= ((byte)random & 0x03);
    *random_ref_len = 2;
  }
  else if ( (access_reason == GMAC_SINGLE_BLOCK_ACCESS) ||
            (access_reason == GMAC_TWO_PHASE_ACCESS) )
  {
    access_ref |= 0x70;

    /**************************************************************/
    /* Get a random number which will be used as a random         */
    /* reference. The latter has to be between 0 and 7 (3 bits)   */
    /**************************************************************/

    /* REFER TO 04.18 TABLE 9.1.8.1, CHANNEL REQUEST msg content */
    access_ref |= ((byte)random & 0x07);
    *random_ref_len = 3;
  }

  return(access_ref);
}

/*===========================================================================

FUNCTION  grr_get_egprs_access_ref

DESCRIPTION
  Returns an EGPRS request reference for use in the DL_RANDOM_ACCESS_REQ

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
word grr_get_egprs_access_ref(
  byte access_reason,
  byte *random_ref_len,
  byte egprs_radio_priority,
  const gas_id_t gas_id
)
{
  word  access_ref = 0x0000;
  uint8 egprs_multislot_class;
  uint8 random;

  if (secapi_get_random(SECAPI_SECURE_RANDOM, &random, 1) != E_SUCCESS)
  {
    ERR_GERAN_FATAL_0_G("ERROR secapi_get_random() returned FAILURE");
  }

  #ifdef FEATURE_GSM_GPRS_MSC33
  egprs_multislot_class = rr_get_nv_hmsc_egprs_coded_ms_class(RR_GAS_ID_TO_AS_ID);
  #else
  egprs_multislot_class = rr_get_nv_egprs_multislot_class(RR_GAS_ID_TO_AS_ID);
  #endif /* FEATURE_GSM_GPRS_MSC33*/

  egprs_multislot_class = egprs_multislot_class - 1; /* ms class 1 is represented as 0 etc.*/

  /***************************************************************/
  /* Access Reason for EGPRS Packet Access on CCCH               */
  /***************************************************************/

  if (access_reason == GMAC_EGPRS_ONE_PHASE_ACCESS)
  {
    /* EGPRS PACKET CHANNEL REQUEST
       One Phase Access Request Content:

       bit 10    - 0
       bit 5-9   - egprs multislot class
       bit 3-4   - radio priority
       bit 0-2   - random
    */

    /* set egrs multislot class at bits 5-9 */
    access_ref |= (egprs_multislot_class & 0x1f) << 5;

    /* Radio priority is received as values 1-4, convert to 0-3 for air interface */
    egprs_radio_priority -= 1;

    /* set egprs radio priority at bits 3-4 */
    access_ref |= (egprs_radio_priority & 0x03) << 3;

    /* set a 3 bit random element at bits 0-2  */
    access_ref |= (word)random & 0x07;

    *random_ref_len = 3;
  }
  else if (access_reason == GMAC_EGPRS_TWO_PHASE_ACCESS)
  {
    /* EGPRS PACKET CHANNEL REQUEST
       Two Phase Access Request Content:

       bit 5-10  - 110000
       bit 3-4   - radio priority
       bit 0-2   - random
    */

    /* set bits 10-5 for two-phase access */
    access_ref |= 0x0600;

    /* Radio priority is received as values 1-4, convert to 0-3 for air interface */
    egprs_radio_priority -= 1;

    /* set egprs radio priority at bits 3-4 */
    access_ref |= (egprs_radio_priority & 0x03) << 3;

    /* set a 3 bit random element at bits 0-2  */
    access_ref |= (word)random & 0x07;

    *random_ref_len = 3;
  }
  else if (access_reason == GMAC_EGPRS_SIGNALLING_ACCESS)
  {
    /* EGPRS PACKET CHANNEL REQUEST
       Signalling Access Request Content:

       bit 5-10  - 110011
       bit 0-4   - random
    */

    /* set bits 10-5 for two-phase access */
    access_ref |= 0x0660;

    /* set a 5 bit random element at bits 0-4  */
    access_ref |= (word)random & 0x1f;

    *random_ref_len = 5;
  }

  return(access_ref);
}



/*===========================================================================

FUNCTION:
  check_tlli

DESCRIPTION:
  Check the TLLI received in the IA Rest Octets against the new and old
  values stored in the GMM data

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static boolean check_tlli(dword dl_ass_tlli, const gas_id_t gas_id)
{
  uint32 tlli;

  // 1st check is against normal TLLI
  tlli = rr_gmminfo_get_tlli(gas_id);

  if (TLLI_IS_VALID(tlli) && (tlli == dl_ass_tlli))
  {
    MSG_GERAN_MED_1_G("TLLI matched against new: 0x%x", dl_ass_tlli);
    return TRUE;
  }

  // 2nd check is against old TLLI
  tlli = rr_gmminfo_get_old_tlli(gas_id);

  if (TLLI_IS_VALID(tlli) && (tlli == dl_ass_tlli))
  {
    MSG_GERAN_MED_1_G("TLLI matched against old: 0x%x", dl_ass_tlli);
    return TRUE;
  }

  return FALSE;
}


/*===========================================================================

FUNCTION:
  match_tlli

DESCRIPTION:
  Decode IA Rest Octets and check if the assignment is downlink.  If not,
  then msg is not matched and return FALSE.  If it is then check that the
  TLLI matches, if it does then return TRUE

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static boolean match_tlli(ie_descriptor_T *orig_current_IE, const gas_id_t gas_id)
{
  ia_rest_octets_t ia_rest_octets;

  uint8 remaining_length;

  /* Leave the pointer passed in as it is, create another
     to increment through to the end of the msg */
  ie_descriptor_T *current_IE = orig_current_IE;

  memset(&ia_rest_octets, 0, sizeof(ia_rest_octets_t));

  current_IE = current_IE->next_IE; /* Now looking at Timing Advance */
  current_IE = current_IE->next_IE; /* Now looking at Mobile Allocation */
  current_IE = current_IE->next_IE; /* Now looking at Starting Time */

  /* Verify that the current IE really is the Starting Time - it may */
  /* not be present, but the IEI must have been loaded by RR earlier */
  if (current_IE->IEI != 0x7C)
  {
    MSG_GERAN_ERROR_1_G("Trying to match TLLI, found IEI(%d) error", (int)current_IE->IEI);
    return(FALSE);
  }

  current_IE = current_IE->next_IE; /* Now looking at IA Rest Octets */
  if (current_IE->start_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("Trying to match TLLI, no Rest Octets");
    return(FALSE);
  }


  remaining_length = rr_length_to_end_of_ota_msg(
    IMMEDIATE_ASSIGNMENT,   // msg_type
    current_IE->start_IE,   // csn1_start_ie_ptr
    IA_REST_OCTETS,         // csn_ie_type
    gas_id
  );

  (void) gcsn1_rr_decoder(
    IA_REST_OCTETS,         // message_type
    current_IE->start_IE,   // csn1_ie_src
    &ia_rest_octets,        // dst
    remaining_length,       // len_to_end_of_buf
    gas_id
  );

  if ((ia_rest_octets.contents_type == packet_downlink_ass) &&
      (check_tlli(ia_rest_octets.contents.packet_downlink_ass.tlli, gas_id)))
  {
    return(TRUE);
  }

  return(FALSE);
} /* end match_tlli() */


/*===========================================================================

FUNCTION:  rr_decode_dedicated_mode_or_tbf

DESCRIPTION:
  Decode the 'Dedicated mode or TBF' field

DEPENDENCIES:

RETURN VALUE:
None

SIDE EFFECTS:

===========================================================================*/
static void rr_decode_dedicated_mode_or_tbf(byte IE, tbf_info_T *tbf_info)
{
  tbf_info->td       = IE & 0x01;
  tbf_info->downlink = (byte) (IE & 0x02) >> 1;
  tbf_info->tma      = (byte) (IE & 0x04) >> 2;
}

/*===========================================================================

FUNCTION:  grr_set_channel_frequency_list

DESCRIPTION:
  Translates the Mobile Allocation into a list of frequencies

DEPENDENCIES:

RETURN VALUE:
  new_event is set to either EV_GPRS_CONNECTION_ABORTION on failure or
  EV_NO_EVENT on success.

SIDE EFFECTS:

===========================================================================*/
static rr_event_T grr_set_channel_frequency_list(
  frequency_list_T *frequency_list_ptr,
  mobile_allocation_T *mobile_allocation_ptr,
  boolean *decoding_ok_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T         new_event = EV_NO_EVENT;
  boolean            has_arfcn_zero = FALSE;
  byte               index;
  byte               ca_index;
  ARFCN_T            channel_number;
  rr_internal_band_T band = BAND_NONE;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  /* assume decoding will be OK */
  *decoding_ok_ptr = TRUE;

  /**************************************************************/
  /* Translate the mobile_allocation into a list of frequencies */
  /**************************************************************/
  frequency_list_ptr->no_of_items = mobile_allocation_ptr->no_of_entries;

  for (index = 0; index < mobile_allocation_ptr->no_of_entries; index++)
  {
    ca_index = mobile_allocation_ptr->MA_C[index] - 1;

    /****************************************************************************/
    /* get the associated channel number from the system information database's */
    /* cell_channel_description                                                 */
    /****************************************************************************/
    channel_number =
      scell_info_ptr->gsm.cell_channel_description.channel_number[ca_index];

    /* store it in the channel description's frequency list */
    frequency_list_ptr->channel[index] = channel_number;
    if (channel_number.num == 0)
    {
      has_arfcn_zero = TRUE;
    }

    /*******************************************************/
    /* Must check the band the channel is in for multiband */
    /*******************************************************/
    if (band == BAND_NONE)
    {
      band = (rr_internal_band_T)GET_ARFCN_BAND(channel_number);

      if (band == BAND_NONE)
      {
        /*************************************************/
        /* This band is not supported, reject assignment */
        /*************************************************/
        new_event = EV_GPRS_CONNECTION_ABORTION;
        *decoding_ok_ptr = FALSE;

        ERR("Band not supported for chan: %d",(int) CONVERT_FROM_ARFCN_T(channel_number),0,0);

        /* exit from loop, no point in continuing since IA will be aborted */
        break;
      }
    }
    else /* not first time through loop */
    {
      /*****************************************************/
      /* For subsequent iterations, check that all entries */
      /* are in the same band                              */
      /*****************************************************/
      if (!rr_check_in_band(band, channel_number.num, gas_id))
      {
        /*******************************************************/
        /* this entry is not in the same band, so RR conection */
        /* establishment failure has occurred                  */
        /*******************************************************/

        new_event = EV_GPRS_CONNECTION_ABORTION;
        *decoding_ok_ptr = FALSE;

        ERR("Chan=%d, Band=%d: Chan in other band",(int) CONVERT_FROM_ARFCN_T(channel_number), (int) band,0);
        break;
      }
    }
  }

  /* ensure that ARFCN 0 is in the right place */
  if (*decoding_ok_ptr && has_arfcn_zero)
  {
    ARFCN_T temp_arfcn_list[MAX_MA_CHANNEL];
    byte    num_freqs = frequency_list_ptr->no_of_items;
  memscpy(temp_arfcn_list,sizeof(temp_arfcn_list), frequency_list_ptr->channel, num_freqs*sizeof(ARFCN_T));

   rr_move_zero_in_frequency_list(temp_arfcn_list, num_freqs, frequency_list_ptr->channel);
  }

  return new_event;
}


/*===========================================================================

FUNCTION:  match_against_tlli

DESCRIPTION:
  Checks whether the IA msg received matches against th TLLI since it didn't
  match against the request references. TLLI from the downlink assignment part
  of the IA Rest Octets within the msg are used.

DEPENDENCIES:

RETURN VALUE:

SIDE EFFECTS:

===========================================================================*/
static boolean match_against_tlli(
  boolean *waiting_for_2nd_msg,
  ie_descriptor_T *current_IE,
  const gas_id_t gas_id
)
{
  byte access_reference;
  word frame_number;
  rr_tbf_est_static_data_t *rr_tbf_est_data_ptr;

  // Obtain a pointer to the module data
  rr_tbf_est_data_ptr = rr_tbf_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_tbf_est_data_ptr);

  /* If expecting the 2nd of a 2 part IA msg... */
  if (*waiting_for_2nd_msg == TRUE)
  {
    /*****************************************/
    /* Decode the request reference IE again */
    /*****************************************/
    rr_decode_request_reference(current_IE->start_IE, &access_reference, &frame_number);

    /* There is no Tlli in the 2nd of a 2 part IA msg since the Rest Octets contain   */
    /* other stuff. Need to compare the request refs rcvd in this msg with those rcvd */
    /* in the previous msg to determine if it is in fact the 2nd of a 2 part IA msg   */
    if ((access_reference == rr_tbf_est_data_ptr->data_ptr->two_part_req_refs.access_reference) &&
        (frame_number == rr_tbf_est_data_ptr->data_ptr->two_part_req_refs.frame_number))
    {
      /* reset request refs for next time */
      rr_tbf_est_data_ptr->data_ptr->two_part_req_refs.access_reference = 0;
      rr_tbf_est_data_ptr->data_ptr->two_part_req_refs.frame_number = 0;
      return TRUE;
    }
    else
    {
      /* This msg didn't match the req_refs from the first, so don't process */
      return FALSE;
    }
  }
  else if (match_tlli(current_IE, gas_id))
  {
    /* For match_tlli to return TRUE, the msg must have been a downlink message. */

    /* If this is a 2 part IA msg, the Request References in this msg will match */
    /* those in the following msg so need to store them for possible use later   */

    /*****************************************/
    /* Decode the request reference IE again */
    /*****************************************/
    rr_decode_request_reference(current_IE->start_IE, &access_reference, &frame_number);

    /* Store for use later */
    rr_tbf_est_data_ptr->data_ptr->two_part_req_refs.access_reference = access_reference;
    rr_tbf_est_data_ptr->data_ptr->two_part_req_refs.frame_number = frame_number;

    /* Initialise flag */
    *waiting_for_2nd_msg = FALSE;
  }
  else
  {
    /* TLLI didn't match so ignore msg and reset flags */
    MSG_GERAN_LOW_0_G("Ignored IA msg");
    rr_tbf_est_data_ptr->data_ptr->two_part_req_refs.access_reference = 0;
    rr_tbf_est_data_ptr->data_ptr->two_part_req_refs.frame_number = 0;
    return FALSE;
  }

  return TRUE;
}

void rr_gprs_two_part_assignment_failure(const gas_id_t gas_id)
{
  rr_tbf_est_static_data_t *rr_tbf_est_data_ptr;

  // Obtain a pointer to the module data
  rr_tbf_est_data_ptr = rr_tbf_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_tbf_est_data_ptr);

  RR_NULL_CHECK_RETURN_VOID(rr_tbf_est_data_ptr->data_ptr);

  if (rr_tbf_est_data_ptr->data_ptr->two_part_info.downlink)
  {
    boolean in_mm_non_drx = rr_gprs_non_drx_mode_requested(gas_id);

    /* NON-DRX mode would have been started when the first part was received */
    if (!in_mm_non_drx)
    {
      MSG_GERAN_MED_0_G("Resetting non-DRX mode for two-part downlink");
      rr_send_mph_stop_mm_non_drx_ind(gas_id);
    }
    else
    {
      /* See 3GPP 44.060 section 5.5.1.5
       * "If the mobile station establishes an RR connection during any of
       *  the non-DRX mode periods, then that period shall continue to run."
       */
      MSG_GERAN_MED_0_G("Two-part downlink failure, preserve non-DRX mode for GMM");
    }
  }

  memset(&rr_tbf_est_data_ptr->data_ptr->two_part_info,0,sizeof(tbf_info_T));

  return;
}

void rr_gprs_two_part_assignment_complete(const gas_id_t gas_id)
{
  rr_tbf_est_static_data_t *rr_tbf_est_data_ptr;

  // Obtain a pointer to the module data
  rr_tbf_est_data_ptr = rr_tbf_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_tbf_est_data_ptr);

  RR_NULL_CHECK_RETURN_VOID(rr_tbf_est_data_ptr->data_ptr);

  if (rr_tbf_est_data_ptr->data_ptr->two_part_info.downlink)
  {
    boolean in_mm_non_drx = rr_gprs_non_drx_mode_requested(gas_id);

    /* NON-DRX mode would have been started when the first part was received */
    if (!in_mm_non_drx)
    {
      MSG_GERAN_MED_0_G("Two-part downlink complete, non-DRX mode stopped");
      rr_send_mph_stop_mm_non_drx_ind(gas_id);
    }
    else
    {
      /* See 3GPP 44.060 section 5.5.1.5
       * "If the mobile station establishes an RR connection during any of
       *  the non-DRX mode periods, then that period shall continue to run."
       */
      MSG_GERAN_MED_0_G("Two-part downlink complete, preserve non-DRX mode for GMM");
    }
  }

  memset(&rr_tbf_est_data_ptr->data_ptr->two_part_info,0,sizeof(tbf_info_T));

  return;
}

/*===========================================================================

FUNCTION:  set_tma_flags

DESCRIPTION:
  Check if this is the first/second part of a 2 message assignment or a
  single msg assignment and set flags used for decoding specific IE's later

DEPENDENCIES:

RETURN VALUE:
  TRUE if flags set ok and processing of IA msg to continue or FALSE if msg
  procesing to stop and IA msg to be ignored.

SIDE EFFECTS:
  Sets waiting_for_2nd_msg and single_IA_msg flags

===========================================================================*/
static boolean set_tma_flags(
  boolean *waiting_for_2nd_msg,
  tbf_info_T *tbf_info_ptr,
  boolean *single_IA_msg,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_tbf_est_static_data_t *rr_tbf_est_data_ptr = rr_tbf_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_tbf_est_data_ptr);

  if (*waiting_for_2nd_msg == TRUE)
  {
    // The checks below will either result in success or failure of this 2-part IA, but regardless,
    // waiting_for_2nd_msg is reset to FALSE so the 2-part IA procedure is finished.
    // Therefore TMULTIFRM should be stopped
    (void)rr_timer_cancel(TMULTIFRM, gas_id);

    if (tbf_info_ptr->tma == 0)
    {
      /* This is the 2nd msg of a 2 part msg */
      MSG_GERAN_HIGH_0_G("Rcvd 2nd part of a 2 part IA");
      *single_IA_msg = FALSE;
      *waiting_for_2nd_msg = FALSE;
      rr_gprs_two_part_assignment_complete(gas_id);
    }
    else
    {
      /* Expecting the 2nd of a 2 part msg but have got the 1st of a 2 part msg - ignore it */
      MSG_GERAN_HIGH_0_G("Ignored IA msg - incorrect msg sequence");
      *waiting_for_2nd_msg = FALSE;
      rr_gprs_two_part_assignment_failure(gas_id);

      return(FALSE);
    }
  }
  else
  {
    if (tbf_info_ptr->tma == 0)
    {
      /* This should be a single IA msg, no further msgs are required. It is possible */
      /* that this is the 2nd part of a 2 part msg which arrived after TMULTIFRM      */
      /* expired. When this timer expires the waiting_for_2nd_msg flag is set to      */
      /* False. The only way to tell the difference between a single IA msg and       */
      /* the 2nd one rcvd after this timer expired is by the contents of the IA Rest  */
      /* Octets. This msg is discarded if appropriate when IA Rest Octets are decoded */
      *single_IA_msg = TRUE;
      *waiting_for_2nd_msg = FALSE;
      MSG_GERAN_HIGH_0_G("Rcvd single IA msg");
    }
    else
    {
      /* This is the 1st of a 2 part msg, so a 2nd msg is expected within 2 multiframes */
      *waiting_for_2nd_msg = TRUE;
      *single_IA_msg = FALSE;

      /* make a copy of the TBF info that includes the 2-part flag */
      rr_tbf_est_data_ptr->data_ptr->two_part_info = *tbf_info_ptr;

      MSG_GERAN_HIGH_0_G("Rcvd 1st part of a 2 part IA");

      if (tbf_info_ptr->downlink)
      {
        /* If the mobile station receives an IMMEDIATE ASSIGNMENT message and the
         * "Dedicated mode or TBF" information element indicates that this is the first
         * message in a two-message assignment, the mobile station shall continue to
         * listen to the full CCCH (3GPP 04.18 v8.10 section 3.5.2.1.3.1)
         */

        /* only need to start NON-DRX mode if the assignment was for a downlink, since
         * uplink assignments will arrive on the AGCH, which decodes all blocks anyway
         */
        MSG_GERAN_MED_0_G("Non-DRX mode starting for two-part downlink assignment");
        rr_send_mph_start_mm_non_drx_ind(gas_id);
      }

      /* Start the TMULTIFRM timer */
      (void)rr_timer_start(TMULTIFRM, (dword)TMULTIFRM_value, gas_id);
    }
  }

  return(TRUE);
}


/*===========================================================================

FUNCTION:  process_indirect_freq

DESCRIPTION:
  Checks the Change Mark is valid and sets the frequency list for indirect
  encoding.  This is taken from the Global System Information Database.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  Flags and frequency list are updated

===========================================================================*/
static void process_indirect_freq(
  boolean *decoding_ok,
  boolean *waiting_for_2nd_msg,
  packet_channel_description_t *packet_channel_desc,
  frequency_list_T *frequency_list,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t         *scell_info         = rr_gprs_get_scell_info(gas_id);
  gprs_mobile_allocations_t *mobile_allocations = &scell_info->mobile_allocations;
  uint8                      ma_number;
  byte                       list_len;
  byte                       i;

  /********************************************************/
  /* If MA_NUMBER_IND=0, MA_NUMBER=14 is used             */
  /* If MA_NUMBER_IND=1, MA_NUMBER=15 is used             */
  /* See 3GPP 04.18 10.5.2.25a Packet Channel Description */
  /********************************************************/
  if (packet_channel_desc->ma_number_ind == 0)
  {
    ma_number = MA_NUMBER_14;
  }
  else
  {
    ma_number = MA_NUMBER_15;
  }

  /* Check the validity of the change mark, if present */
  if ((MA_NUMBER_14                            == ma_number) &&
      (packet_channel_desc->change_mark_1_flag == valid)     &&
      (packet_channel_desc->change_mark_1      != mobile_allocations->si13_change_mark))
  {
    /* this will result in this IA message being ignored */
    *decoding_ok = FALSE;
    *waiting_for_2nd_msg = FALSE;
    MSG_GERAN_MED_2_G("IA IGNORED:chg mrk %d invalid (SI13=%d)", packet_channel_desc->change_mark_1, mobile_allocations->si13_change_mark);
  }
  else if ((packet_channel_desc->change_mark_1_flag == valid) &&
           (MA_NUMBER_15                            == ma_number))
  {
    /* this will result in this IA message being ignored */
    *decoding_ok = FALSE;
    *waiting_for_2nd_msg = FALSE;
    MSG_GERAN_MED_0_G("IA IGNORED:chg mrk flag not for use with MA NUMBER 15)");
  }
  else
  {
    /******************************************************************************************/
    /* Frequencies used for hopping when indirect encoding is used are taken from information */
    /* received in System Info 13 msg, this info is stored in the global common area          */
    /******************************************************************************************/
    if ( IS_BIT_NUM_SET( mobile_allocations->ma_valid_bitmap, ma_number ) )
    {
      uint8 ma_offset = MAP_MA_NUMBER_TO_OFFSET(ma_number);

      /* Copy frequencies from the Sys Info database */
      list_len = frequency_list->no_of_items = mobile_allocations->gprs_mobile_allocation[ma_offset].list_length;

      for (i = 0; i < list_len; i++)
      {
        /* loop round copying the frequencies across */
        frequency_list->channel[i] = mobile_allocations->gprs_mobile_allocation[ma_offset].ma_number[i];
      }

      /* Take the HSN from the global sys info database for Indirect encoding */
      packet_channel_desc->hsn = mobile_allocations->gprs_mobile_allocation[ma_offset].hsn;
    }
    else
    {
      /* this will result in this IA message being ignored */
      *decoding_ok = FALSE;
      MSG_GERAN_MED_0_G("Error - invalid ma bitmap in IA msg");
    }
  }
}



/*===========================================================================

FUNCTION store_gprs_ia_mobile_alloc

DESCRIPTION
  This function stores a mobile allocation given in a GPRS Immediate Assignment
  message as MA_NUMBER=15.

PARAMS
  frequency_list_T * frequency_list
  uint8 hsn

RETURN VALUE
  None

SIDE EFFECTS
  The Mobile Allocation list is updated

===========================================================================*/

static void store_gprs_ia_mobile_alloc(
  frequency_list_T * frequency_list,
  uint8 hsn,
  const gas_id_t gas_id
)
{
  uint8                     i;
  gprs_mobile_allocation_t  gprs_mobile_alloc;

  for ( i = 0; i < frequency_list->no_of_items; i++ )
  {
    gprs_mobile_alloc.arfcns[i] = frequency_list->channel[i];
  }

  gprs_mobile_alloc.num_of_arfcns = i;
  gprs_mobile_alloc.hsn = hsn;

  rr_gprs_store_mobile_alloc(
    MA_NUMBER_15,
    &gprs_mobile_alloc,
    rr_gprs_get_scell_info(gas_id),
    gas_id
  );

} /* end store_gprs_ia_mobile_alloc() */



/*===========================================================================

FUNCTION: is_iamsg_sb

DESCRIPTION:
  Determines whether the downlink Immediate Assignment message received is a
  Single Block assignment or a TBF.  The only parameters present for a single
  block are the tlli and the tbf_starting_time.

DEPENDENCIES

RETURN VALUE
  returns TRUE if DL IA msg is a Single Block

SIDE EFFECTS

===========================================================================*/
static boolean is_iamsg_sb(ia_rest_octets_packet_downlink_ass_t dl_ass)
{
  if ( (dl_ass.tfi_assignment_flag == not_valid) &&
       (dl_ass.timing_advance_index_flag == not_valid) &&
       (dl_ass.tbf_starting_time_flag == valid) &&
       (dl_ass.dwnlk_pwr_ctrl_flag == not_valid) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION: rr_gprs_validate_immediate_assignment

DESCRIPTION:
  Ensures that a GPRS IA contains an allocation type that we can handle

DEPENDENCIES

RETURN VALUE
  TRUE if the immediate assignment contains a supported allocation type

SIDE EFFECTS
  None
===========================================================================*/
static boolean rr_gprs_validate_immediate_assignment(
  ia_rest_octets_t *iaro_ptr,
  const gas_id_t gas_id
)
{
  boolean bValid = TRUE;
  boolean egprs_capable_cell = rr_get_nv_egprs_enabled(RR_GAS_ID_TO_AS_ID) && rr_cell_supports_egprs(gas_id);

  if (iaro_ptr->contents_type == packet_uplink_ass)
  {
    if (iaro_ptr->contents.packet_uplink_ass.block_alloc_flag == BLOCK_ALLOC_FLAG_MULTI)
    {
      bValid = (boolean)(iaro_ptr->contents.packet_uplink_ass.block_alloc.multi_block_alloc.alloc_flag != fixed);
    }
  }
  /* confusing defs in csn.h. for EGPRS PUA, multi: multi block allocation, single: dynamic allocation
                              for GPRS PUA, multi: dynamic allocation, single: single block allocation */
  else if (iaro_ptr->contents_type == egprs_packet_uplink_ass)
  {
#ifdef FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED
    bValid = FALSE;
#else
    if (iaro_ptr->contents.egprs_packet_uplink_ass.block_alloc_flag == BLOCK_ALLOC_FLAG_SINGLE)
    {
      bValid = (boolean)(iaro_ptr->contents.egprs_packet_uplink_ass.block_alloc.single_block_alloc.alloc_flag != fixed);
    }
    if (egprs_capable_cell == FALSE)
    {
      MSG_GERAN_HIGH_0_G("RR EGPRS IA rxd when EGPRS not supported by MS/NW");
      bValid = FALSE;
    }

#endif /* FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */
  }
  else if (iaro_ptr->contents_type == packet_downlink_ass)
  {
    if (iaro_ptr->contents.packet_downlink_ass.egprs_tbf_mode_flag == TRUE)
    {
#ifdef FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED
      bValid = FALSE;
#else
      if (egprs_capable_cell == FALSE)
      {
        MSG_GERAN_HIGH_0_G("RR EGPRS IA rxd when EGPRS not supported by MS/NW");
        bValid = FALSE;
      }
#endif /* FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */
    }
  }


  return(bValid);
}


/**
  @brief Sends a DL Imm Ass to MAC (either GRR_MAC_DL_SB_EST or GRR_MAC_DL_TBF_EST)
 */
void grr_send_gprs_dl_ia_to_mac(
  packet_channel_description_t *packet_ch_desc,
  ia_rest_octets_t             *ia_rest_octets,
  frequency_list_T             *frequency_list,
  byte                          timing_advance,
  const gas_id_t                gas_id
)
{

  /* Delete any saved MAC_GRR_UL_TBF_REQ message */
  rr_delete_saved_mac_ul_tbf_req(gas_id);

  /* Determine whether the DL assignment is a SB or TBF */
  if(is_iamsg_sb(ia_rest_octets->contents.packet_downlink_ass))
  {
    rr_send_grr_mac_dl_sb_est(
      packet_ch_desc,
      ia_rest_octets,
      frequency_list,
      timing_advance,
      gas_id
    );
  }
  else
  {
    rr_send_grr_mac_dl_tbf_est(
      packet_ch_desc,
      ia_rest_octets,
      frequency_list,
      timing_advance,
      gas_id
    );
  }
}


/*===========================================================================

FUNCTION: grr_respond_to_gprs_immediate_assignment

DESCRIPTION:
  Processes the Immediate Assignment OTA message (when it is a GPRS msg) and
  decodes the information in the message. If the message is destined to the
  mobile and decoded correctly then it defines a PDCH assignment which is
  then forwarded to MAC

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
rr_event_T grr_respond_to_gprs_immediate_assignment(
  ie_descriptor_T *current_IE,
  byte dedicated_mode_or_tbf_ie,
  boolean *waiting_for_2nd_msg,
  byte no_of_rach_attempts,
  request_references_T  *request_references_array,
  const gas_id_t gas_id
  )
{
  mobile_allocation_T mobile_allocation = {0};
  rr_event_T new_event = EV_NO_EVENT;
  boolean decoding_ok = TRUE;
  boolean single_IA_msg = FALSE;
  boolean match_reference;
  tbf_info_T tbf_info;

  uint8 remaining_length = 0;

  packet_channel_description_t packet_channel_desc;
  ia_rest_octets_t temp_ia_rest_octets;
  frequency_list_T frequency_list;
  boolean compressed_irat_ho_info = FALSE;

  rr_tbf_est_static_data_t *rr_tbf_est_data_ptr;

  // Obtain a pointer to the module data
  rr_tbf_est_data_ptr = rr_tbf_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_tbf_est_data_ptr);

  // Sanity check that control/data structures have been setup
  RR_NULL_CHECK_RETURN_PARAM(rr_tbf_est_data_ptr->data_ptr, EV_NO_EVENT);

  /*************************/
  /* Initialise structures */
  /*************************/
  memset(&temp_ia_rest_octets, 0, sizeof(temp_ia_rest_octets));
  memset(&frequency_list, 0, sizeof(frequency_list));
  memset(&packet_channel_desc, 0, sizeof(packet_channel_desc));

  /******************************************/
  /* Decode the Dedicated mode or TBF field */
  /******************************************/
  rr_decode_dedicated_mode_or_tbf(dedicated_mode_or_tbf_ie, &tbf_info);

  /*****************************************/
  /* Decode the Packet Channel Description */
  /*****************************************/


  remaining_length = rr_length_to_end_of_ota_msg(
    IMMEDIATE_ASSIGNMENT,         // msg_type
    current_IE->start_IE,         // csn1_start_ie_ptr
    PACKET_CHANNEL_DESCRIPTION,   // csn_ie_type
    gas_id
  );

  (void) gcsn1_rr_decoder(
    PACKET_CHANNEL_DESCRIPTION,   // message_type
    current_IE->start_IE,         // csn1_ie_src
    &packet_channel_desc,         // dst
    remaining_length,             // len_to_end_of_buf
    gas_id
  );

  /*********************************************/
  /* have a look on the next request reference */
  /*********************************************/
  current_IE = current_IE->next_IE;

  /*******************************************************************/
  /* Determine if the immediate assignment request reference matches */
  /* any of the last 3 random access (CHANNEL_REQUEST) messages sent */
  /* to the network. IAs with the downlink flag set are NOT checked  */
  /*******************************************************************/
  if (tbf_info.downlink == 0)
  {

   /* If accessing with the EGPRS Packet Channel Request then the Immediate Assignment must be matched
      using the Extended RA.  When set to '0111 1111' the Request Reference field indicates that the Extended RA
      may be contained in the Rest Octets.  If it is not in the RO then the IA is assumed to be unrelated
      to the EGPRS Packet Channel Request */
   if (rr_ce_egprs_access_valid(gas_id))
   {
     word    extended_ra = 0;
     boolean extended_ra_found = FALSE;
     byte    access_reference = 0;
     word    frame_number = 0;

     rr_decode_request_reference(current_IE->start_IE,&access_reference,&frame_number);

     if ( access_reference == EGPRS_EXTENDED_RA_INDICATOR )
     {
       ie_descriptor_T *temp_IE_ptr = current_IE;

       /* advance to the IA Rest Octets IE*/
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Timing Advance */
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Mobile Allocation */
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Starting Time*/
       temp_IE_ptr = temp_IE_ptr->next_IE; /* IA Rest Octets */


       remaining_length = rr_length_to_end_of_ota_msg(
         IMMEDIATE_ASSIGNMENT,    // msg_type
         temp_IE_ptr->start_IE,   // csn1_start_ie_ptr
         IA_REST_OCTETS,          // csn_ie_type
         gas_id
       );

       (void) gcsn1_rr_decoder(
         IA_REST_OCTETS,          // message_type
         temp_IE_ptr->start_IE,   // csn1_ie_src
         &temp_ia_rest_octets,    // dst
         remaining_length,        // len_to_end_of_buf
         gas_id
       );

       /* Look for Extended RA field */
       switch ( temp_ia_rest_octets.contents_type)
       {
         case egprs_packet_uplink_ass:
           extended_ra = temp_ia_rest_octets.contents.egprs_packet_uplink_ass.extended_ra;
           extended_ra_found = TRUE;
         break;

         case packet_downlink_ass:
           extended_ra_found = FALSE;
         break;

         case packet_uplink_ass: /* full EDGE solution for packet_uplink_ass not in place yet, code COMMENTED out */
           if ( temp_ia_rest_octets.contents.packet_uplink_ass.extended_ra_flag == valid )
           {
             extended_ra = temp_ia_rest_octets.contents.packet_uplink_ass.extended_ra;
             extended_ra_found = TRUE;
           }
         break;

         case second_part_packet_ass:
           if ( ( temp_ia_rest_octets.contents.second_part_packet_ass.additions_for_r99 == valid ) &&
                ( temp_ia_rest_octets.contents.second_part_packet_ass.extended_ra_flag == valid ) )
           {
               extended_ra = temp_ia_rest_octets.contents.second_part_packet_ass.extended_ra;
               extended_ra_found = TRUE;
           }
         break;

         case compressed_interRAT_ho_info:
           compressed_irat_ho_info = temp_ia_rest_octets.contents.compressed_irat_ho_info;
         break;

         case freq_params_before_time:
           compressed_irat_ho_info = temp_ia_rest_octets.contents.freq_params_before_time.compressed_irat_ho_info;
         break;

         default:
           /*extended_ra_found already set to FALSE at declaration*/
         break;
       }
     }

     if ( extended_ra_found )
     {
       match_reference =
         rr_match_IA_extended_request_reference(
           request_references_array, no_of_rach_attempts, extended_ra, frame_number );
     }
     else
     {
       /* Extended RA not found in the Immediate Assignment RO, assume that this IA is not related to the EGPRS
          Packet Channel Request. 44.018 sect. 10.5.2.16.1 OR this is a DL ass*/
       match_reference = FALSE;
     }
   }
   else
   {
     match_reference =
       rr_match_IA_request_reference(
         request_references_array,current_IE->start_IE, no_of_rach_attempts);
   }

  }
  else
  {
    match_reference = FALSE;
  }

  if (match_reference)
  {
    MSG_GERAN_HIGH_0_G("Matched IA using Request Refs");
  }
  else
  {
    /*******************************************/
    /* Check if the msg matches using the TLLI */
    /*******************************************/
    if (!match_against_tlli(waiting_for_2nd_msg, current_IE, gas_id))
    {
      MSG_GERAN_LOW_1_G("Ignored IA msg - no match (dnl=%d)", tbf_info.downlink);
      return EV_NO_EVENT;
    }
    else
    {
      MSG_GERAN_HIGH_0_G("Matched IA using TLLI");
    }
  }

  /* To get this far, the msg must have been matched,
   * either against the request_references or TLLI
   */

  MSG_GERAN_HIGH_0_G("Immed. Assign is for mobile");

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
  rr_classmark_set_irat_ho_info_compressed(compressed_irat_ho_info, gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

  /*****************************************************/
  /* Set flags associated with handling 2 part IA msgs */
  /*****************************************************/
  if (set_tma_flags(
        waiting_for_2nd_msg,
        &tbf_info,
        &single_IA_msg,
        gas_id
      ) == FALSE)
  {
    /* IA msg to be ignored */
    return EV_NO_EVENT;
  }

  /******************/
  /* Timing Advance */
  /******************/
  current_IE = current_IE->next_IE;
  if (*waiting_for_2nd_msg || single_IA_msg)
  {
    /* Only interested in decoding this in the first of a 2 part or a single part message */
    rr_tbf_est_data_ptr->data_ptr->timing_advance = *(current_IE->start_IE) & 0x3f;
  }

  /********************************************/
  /* Only decode Mobile Allocation if this is */
  /* a single msg or the 2nd of a 2 part msg  */
  /********************************************/
  if (!(*waiting_for_2nd_msg) || single_IA_msg)
  {
    /*********************/
    /* Mobile Allocation */
    /*********************/
    current_IE = current_IE->next_IE;

    if ( packet_channel_desc.rf_channel_config == CONFIG_DIRECT_ENCODING )
    {
      /* DIRECT ENCODING                                                                      */
      /* The packet channel description contains: TN, TSC, HSN, MAIO                          */
      /* The mobile allocation is derived from the mobile allocation IE which follows the     */
      /* packet channel description IE. This contains a list of indexes into the Cell         */
      /* Allocation (from SI1).                                                               */
      /* The mobile allocation must be stored, as it can be referenced in a future assignment */
      /* message using indirect encoding, and refering to MA_NUMBER=15                        */

      rr_decode_mobile_allocation(current_IE->start_IE, &mobile_allocation);

      /* Get the list of frequencies for the channel  */
      if (mobile_allocation.no_of_entries > 0)
      {
        new_event = grr_set_channel_frequency_list(
          &frequency_list,
          &mobile_allocation,
          &decoding_ok,
          gas_id
        );
      }
      else
      {
        decoding_ok = FALSE;
        new_event = EV_GPRS_CONNECTION_ABORTION;
        MSG_GERAN_ERROR_0_G("frequency hopping with 0 frequency in mobile allocation");
      }

      if (decoding_ok)
      {
        /* Store this mobile allocation for future reference */
        store_gprs_ia_mobile_alloc(
          &frequency_list,
          packet_channel_desc.hsn,
          gas_id
        );
      }
    }
    else
    if ( packet_channel_desc.rf_channel_config == CONFIG_INDIRECT_ENCODING )
    {
      /* INDIRECT ENCODING                                                                    */
      /* The packet channel description contains: TN, TSC, MAIO, MA_NUMBER_IND, CHANGE_MARK_1 */
      /* The mobile allocation used is one which has been previously received, and which one  */
      /* to use is indicated by MA_NUMBER_IND. If MA_NUMBER_IND=0, the mobile allocation is   */
      /* the one associated with MA_NUMBER=14. If MA_NUMBER_IND=1, the mobile allocation is   */

      /* Get the list of frequencies for the channel and check Change Mark */
      process_indirect_freq(
        &decoding_ok,
        waiting_for_2nd_msg,
        &packet_channel_desc,
        &frequency_list,
        gas_id
      );
    }
    else
    if ( packet_channel_desc.rf_channel_config == CONFIG_NON_HOPPING )
    {
      /* NON-HOPPING                                                                          */
      /* The packet channel description contains: TN, TSC, ARFCN                              */
      /* Only the ARFCN is required for non-hopping. It is stored as the first member of the  */
      /* frequency list                                                                       */

      frequency_list.no_of_items = 1;
      CONVERT_TO_ARFCN_T( frequency_list.channel[0], packet_channel_desc.arfcn );

      if ((rr_internal_band_T)GET_ARFCN_BAND(packet_channel_desc.arfcn) == BAND_NONE)
      {
        new_event = EV_GPRS_CONNECTION_ABORTION;
        decoding_ok = FALSE;

#ifndef PERLUTF
        MSG_GERAN_ERROR_1_G("Band not supported for chan: %d",(int) CONVERT_FROM_ARFCN_T(packet_channel_desc.arfcn));
#else
        #error code not present
#endif
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("pkt_chan_desc: bad chan config (%d)",packet_channel_desc.rf_channel_config);

      decoding_ok = FALSE;
    }
  }/*if (!*waiting_for_2nd_msg)*/
  else
  {
    /* Not interested in the Mobile Allocation on the 1st  */
    /* of a 2 part msg but still need to step over this IE */
    current_IE = current_IE->next_IE;
  }

  current_IE = current_IE->next_IE;

 /*********************************************************/
 /*Starting Time is ignored for GPRS, so just step over it*/
 /*********************************************************/
  current_IE = current_IE->next_IE;

 /*****************************/
 /* Decode the IA Rest Octets */
 /*****************************/
  if (((*waiting_for_2nd_msg) && (tbf_info.tma == 1)) || single_IA_msg)
  {
    /* Only decode the IA Rest Octets in the first part of a 2 part msg, or for a single */
    /* IA msg. Second msg contains the 2nd Part Packet Assignment which is only for EGPRS */
    if ((current_IE->start_IE != NULL) && (decoding_ok))
    {

      remaining_length = rr_length_to_end_of_ota_msg(
        IMMEDIATE_ASSIGNMENT,   // msg_type
        current_IE->start_IE,   // csn1_start_ie_ptr
        IA_REST_OCTETS,         // csn_ie_type
        gas_id
      );

      (void) gcsn1_rr_decoder(
        IA_REST_OCTETS,         // message_type
        current_IE->start_IE,   // csn1_ie_src
        &temp_ia_rest_octets,   // dst
        remaining_length,       // len_to_end_of_buf
        gas_id
      );

      /* validate the assignment type here (need to discard certain allocation types) */
      if (rr_gprs_validate_immediate_assignment(&temp_ia_rest_octets, gas_id) == FALSE)
      {
        MSG_GERAN_MED_0_G("Ignored IA msg - invalid assignment type");
        return EV_NO_EVENT;
      }

      if ((temp_ia_rest_octets.contents_type == second_part_packet_ass) &&
          (single_IA_msg == TRUE))
      {
        /* Discard this msg as it was the 2nd part of a 2 part */
        /* multiframe msg rcvd after TMULTIFRM has expired     */
        MSG_GERAN_LOW_0_G("Discarded IA msg, rcvd 2nd of 2 part after 2 multiframes");
        *waiting_for_2nd_msg = FALSE;

        rr_gprs_two_part_assignment_failure(gas_id);

        return new_event;
      }
      else
      {
        /* Copy IA Rest Octets from temp value */
        rr_tbf_est_data_ptr->data_ptr->ia_rest_octets = temp_ia_rest_octets;
      }
    }
  }

  /***********************************************/
  /* Once both parts of a 2 part msg or a single */
  /* IA msg has been rcvd, send message to MAC   */
  /***********************************************/
  if ((*waiting_for_2nd_msg == FALSE) && (decoding_ok))
  {
    new_event = EV_GPRS_IMMEDIATE_ASSIGNMENT;


    if ( rr_tbf_est_data_ptr->data_ptr->ia_rest_octets.contents_type == egprs_packet_uplink_ass)
    {
      MSG_GERAN_HIGH_0_G("Clearing IA EGPRS RACH references");
      rr_reset_rach_references(gas_id);

      /* confusing defs in csn.h. for EGPRS PUA, multi: multi block allocation, single: dynamic allocation
                                  for GPRS PUA, multi: dynamic allocation, single: single block allocation */
      if ( rr_tbf_est_data_ptr->data_ptr->ia_rest_octets.contents.egprs_packet_uplink_ass.block_alloc_flag == BLOCK_ALLOC_FLAG_MULTI )
      {
        rr_send_grr_mac_ul_sb_est(
          &packet_channel_desc,
          &rr_tbf_est_data_ptr->data_ptr->ia_rest_octets,
          &frequency_list,
          rr_tbf_est_data_ptr->data_ptr->timing_advance,
          gas_id
        );

      }
      else
      {
        rr_send_grr_mac_ul_tbf_est(
          &packet_channel_desc,
          &rr_tbf_est_data_ptr->data_ptr->ia_rest_octets,
          &frequency_list,
          rr_tbf_est_data_ptr->data_ptr->timing_advance,
          no_of_rach_attempts,
          gas_id
        );
      }
    } else

    if (rr_tbf_est_data_ptr->data_ptr->ia_rest_octets.contents_type == packet_uplink_ass)        /* Uplink */
    {
      MSG_GERAN_HIGH_0_G("Clearing IA RACH references");
      rr_reset_rach_references(gas_id);

      if (rr_tbf_est_data_ptr->data_ptr->ia_rest_octets.contents.packet_uplink_ass.block_alloc_flag == BLOCK_ALLOC_FLAG_SINGLE)
      {
        rr_send_grr_mac_ul_sb_est(
          &packet_channel_desc,
          &rr_tbf_est_data_ptr->data_ptr->ia_rest_octets,
          &frequency_list,
          rr_tbf_est_data_ptr->data_ptr->timing_advance,
          gas_id
        );
      }
      else
      {
        rr_send_grr_mac_ul_tbf_est(
          &packet_channel_desc,
          &rr_tbf_est_data_ptr->data_ptr->ia_rest_octets,
          &frequency_list,
          rr_tbf_est_data_ptr->data_ptr->timing_advance,
          no_of_rach_attempts,
          gas_id
        );
      }
    }
    else if (rr_tbf_est_data_ptr->data_ptr->ia_rest_octets.contents_type == packet_downlink_ass) /* Downlink */
    {
      if (check_tlli(
            rr_tbf_est_data_ptr->data_ptr->ia_rest_octets.contents.packet_downlink_ass.tlli,
            gas_id
          ) == FALSE)
      {
        MSG_GERAN_LOW_0_G("Ignored IA msg - TLLI doesn't match");
        new_event = EV_NO_EVENT;
      }
      else
      {
        // Reception of a DL IA cancels T3142 (if running)
        if (rr_timer_is_running(T3142, gas_id))
        {
          MSG_GERAN_HIGH_0_G("DL IA cancels T3142");
          rr_timer_cancel(T3142, gas_id);
        }

        /*
        If RR is performing some procedure, then it is not safe to pass a DL Imm Ass to MAC, as RR
        may not be in the correct state for MAC/L1 to enter Packet Transfer (e.g. if RR is performing
        background PLMN search)
        */
        if (rr_background_plmn_search_in_progress(gas_id))
        {
          if (rr_mac_dl_tbf_est_is_allowed(gas_id))
          {
            // Send the GPRS IA to RR. This abort the PLMN search procedure and pass this onto GRR
            // once back in Packet Idle
            rr_send_imsg_gprs_ia_ind(
              &packet_channel_desc,
              &rr_tbf_est_data_ptr->data_ptr->ia_rest_octets,
              &frequency_list,
              rr_tbf_est_data_ptr->data_ptr->timing_advance,
              gas_id
            );
          }
          else
          {
            MSG_GERAN_HIGH_0_G("PS is blocked - Imm Ass ignored");
            new_event = EV_NO_EVENT;
          }

          new_event = EV_NO_EVENT;
        }
        else
        {
          if ((grr_is_idle_camped(gas_id) || grr_tbf_est_in_progress(gas_id))
              && !rr_resel_reselection_is_pending(gas_id))
          {
            /**
             * Check if DL access is allowed. These checks include
             *  - the cell supports GPRS
             *  - there is no block due to Thermal Mitigation
             *  - the service domain allows PS
             *  - etc
             * Note: PS access could have been blocked due to a previous IA Reject received which started
             * T3142. Reception of a DL IA cancels T3142 and so PS can be unblocked.
             */
            if (rr_mac_dl_tbf_est_is_allowed(gas_id))
            {
              // Check if PS access is blocked. As DL is allowed, unblock it if so
              if (rr_mac_ps_access_blocked(gas_id))
              {
                // Send request to GRR to unblock PS access
                rr_grr_send_imsg_ps_access_req(
                  FALSE,   // purge_gmm_signalling
                  NULL,    // confirm_cb_fn
                  gas_id   // gas_id
                );
              }

              // Send the GPRS IA to GRR. This will pass this onto MAC
              // Note: It is OK to send this request immediately after requesting GRR to unblock PS access.
              // GRR will not process the RR_GRR_IMSG_GPRS_IA_IND until PS access is unblocked
              rr_grr_send_imsg_gprs_ia_ind(
                &packet_channel_desc,
                &rr_tbf_est_data_ptr->data_ptr->ia_rest_octets,
                &frequency_list,
                rr_tbf_est_data_ptr->data_ptr->timing_advance,
                gas_id
              );
            }
            else
            {
              MSG_GERAN_HIGH_0_G("PS is blocked - Imm Ass ignored");
              new_event = EV_NO_EVENT;
            }
          }
          else
          {
            MSG_GERAN_HIGH_0_G("RR state not compatible with Packet Transfer - Imm Ass ignored");
            new_event = EV_NO_EVENT;
          }
        }
      }
    }
    else
    {
      MSG_GERAN_MED_0_G("Unknown message type received in Packet IA msg");
      new_event = EV_NO_EVENT;
    }
  }

  return new_event;
} /* grr_respond_to_gprs_immediate_assignment */



/*===========================================================================

FUNCTION  grr_process_mac_grr_ul_tbf_req_msg

DESCRIPTION
  Processes the event MAC_GRR_UL_TBF_REQ - sets access_reason, checks
  radio_priority against Global Sys Info database and checks access_class.

DEPENDENCIES
  None

RETURN VALUE
  rr_event_T

SIDE EFFECTS
  Sets new_con_est_event which determines

===========================================================================*/
rr_event_T grr_process_mac_grr_ul_tbf_req_msg(
  mac_grr_ul_tbf_req_t *mac_grr_ul_tbf_req,
  const gas_id_t gas_id
)
{
  rr_event_T event_out = EV_GPRS_INIT_ACCESS_PROCEDURE;

  /*
  Perform a check if cell access is allowed.
  Note: This does not include T3142 - this is done after the RACH_REQ/CNF handshake, see later comment
  */
#ifdef FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED
  if (IS_ENHANCED_GPRS_ACCESS_REASON(mac_grr_ul_tbf_req->channel_req_info.access_type))
  {
    MSG_GERAN_ERROR_0_G("EGPRS requested, but not in classmark: check NV(2508)");
    event_out = EV_GPRS_ACCESS_NOT_POSSIBLE;
  }
  else
#endif /* FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */
  {
    /* Check that the access class allows access to the cell */
    access_result_T access_result = rr_check_access_class_for_raching_for_tbf(gas_id);

    if ((access_result != NORMAL_CALLS_ALLOWED) && (access_result != ALL_CALLS_ALLOWED))
    {
      MSG_GERAN_HIGH_0_G("Access Class check failed for UL TBF REQ");
      event_out = EV_GPRS_ACCESS_NOT_POSSIBLE;
    }
  }

  /* Check that the Radio Priority is acceptable */
  if ( event_out == EV_GPRS_INIT_ACCESS_PROCEDURE )
  {
    gprs_scell_info_t * scell_info = rr_gprs_get_scell_info(gas_id);
    byte priority_access_thr = scell_info->bcch_sys_info.priority_access_thr;
    uint8 radio_priority = mac_grr_ul_tbf_req->channel_req_info.radio_priority;

    /**************************************************************/
    /* From 51.010 section 41.2.1.1 - Permission to access the    */
    /* network / priority classes                                 */
    /* 0 0 1 No packet access allowed                             */
    /* 0 1 0 No packet access allowed                             */
    /* 0 1 1 Packet access is allowed for priority level 1;       */
    /* 1 0 0 Packet access is allowed for priority level 1 to 2;  */
    /* 1 0 1 Packet access is allowed for priority level 1 to 3;  */
    /* 1 1 0 Packet access is allowed for priority level 1 to 4;  */
    /* 1 1 1 Spare, shall be interpreted as packet access allowed */
    /**************************************************************/
    switch (priority_access_thr)
    {
      case 1:
      case 2:
        event_out = EV_GPRS_ACCESS_NOT_POSSIBLE;
        break;

      case 3:
        if (radio_priority != 1)
        {
          event_out = EV_GPRS_ACCESS_NOT_POSSIBLE;
        }
        break;

      case 4:
        if (radio_priority > 2)
        {
          event_out = EV_GPRS_ACCESS_NOT_POSSIBLE;
        }
        break;

      case 5:
        if (radio_priority > 3)
        {
          event_out = EV_GPRS_ACCESS_NOT_POSSIBLE;
        }
        break;

      case 6:
        if (radio_priority > 4)
        {
          event_out = EV_GPRS_ACCESS_NOT_POSSIBLE;
        }
        break;

     case 7:
       /* Do nothing, access is allowed */
       break;

     default:
       event_out = EV_GPRS_ACCESS_NOT_POSSIBLE;
    }

    if ( event_out == EV_GPRS_ACCESS_NOT_POSSIBLE )
    {
      MSG_GERAN_HIGH_2_G("Radio Priority check failed for UL TBF REQ (pat=%d,rp%d)",
               priority_access_thr,radio_priority);
    }
  }

  if ( event_out == EV_GPRS_INIT_ACCESS_PROCEDURE )
  {
    /* Set RR CE stablishment cause, either GMAC_ONE_PHASE_ACCESS */
    /* GMAC_TWO_PHASE_ACCESS or GMAC_SINGLE_BLOCK_ACCESS */
    rr_ce_set_access_reason(
      mac_grr_ul_tbf_req->channel_req_info.access_type,
      gas_id
    );
    rr_ce_set_pkt_est_cause(
      mac_grr_ul_tbf_req->channel_req_info.pkt_est_cause,
      gas_id
    );

    rr_ce_set_egprs_radio_priority(
      mac_grr_ul_tbf_req->channel_req_info.radio_priority,
      gas_id
    );
  }
  else
  {
    // If result is not EV_GPRS_INIT_ACCESS_PROCEDURE then access is not allowed
    // Access is rejected back to MAC & RLC. Either one PDU or all PDUs are deleted
    // depending on whether this is GMM signalling or user data
    // The reasons for deleting all PDUs are
    // a) GMM signalling is highest priority so no point in retrying
    // b) The deletion feedback mechanism employed when DEL_ONE is used will
    // regenerate the PDU, resulting in an infinite loop (DEL_ALL does not feedback)
    if (mac_grr_ul_tbf_req->channel_req_info.pkt_est_cause == RR_PKT_EST_CAUSE_USER_DATA)
    {
      rr_send_grr_mac_no_ps_access_ind(
        DEL_ONE,
        gas_id
      );
    }
    else
    {
      rr_send_grr_mac_no_ps_access_ind(
        DEL_ALL,
        gas_id
      );
    }

    rr_send_grr_mac_ps_access_req(
      FALSE,
      gas_id
    );
  }

  return event_out;

} /* end grr_process_mac_grr_ul_tbf_req_msg() */


/*===========================================================================

FUNCTION  abort_tbf_est

DESCRIPTION
  This function is called to abort a PS conn est in progress.
  RACHing is aborted, MM non-DRX cleared, and the TBF est rejected to MAC.

PARAMS
  None

RETURN VALUE
  None

===========================================================================*/
static rr_event_T abort_tbf_est(const gas_id_t gas_id)
{
  rr_event_T rr_event;
  rr_tbf_est_static_data_t *rr_tbf_est_data_ptr;

  // Obtain a pointer to the module data
  rr_tbf_est_data_ptr = rr_tbf_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_tbf_est_data_ptr);

  /* abort PS connection and reset RR's CE state machine */
  (void) rr_connection_establishment_control(
    EV_GPRS_RESET_RR_CE_STATE,
    NULL,
    gas_id
  );

  if (rr_tbf_est_data_ptr->raching_in_progress)
  {
    rr_event = EV_TBF_CONN_EST_ABORTED_NON_CAMPED;
    /* Stop RACHes in L1 and clear NON-DRX mode */
    rr_send_mph_abort_ra_req(gas_id);
    rr_gprs_clear_l1_in_mm_non_drx(gas_id);
  }
  else
  {
    rr_event = EV_TBF_CONN_EST_ABORTED_CAMPED;
  }

  /**
   * MAC does not need to be informed here as it will be sent a
   * GRR_MAC_NO_PS_ACCESS_IND soon.
   */

  return rr_event;

} /* end abort_tbf_est() */



/*===========================================================================

FUNCTION  grr_conn_est_control

DESCRIPTION
  This function is called to establish a TBF

PARAMS
  event, event_data - input event and associated data

RETURN VALUE
  event for caller

===========================================================================*/
rr_event_T grr_conn_est_control(
  rr_event_T event_in,
  void *event_data,
  const gas_id_t gas_id
)
{
  byte   message_set = 0;
  byte   message_id = 0;

  rr_event_T  event_out = EV_NO_EVENT;
  rr_event_T  new_event = EV_NO_EVENT;

  rr_tbf_est_static_data_t *rr_tbf_est_data_ptr;

  // Obtain a pointer to the module data
  rr_tbf_est_data_ptr = rr_tbf_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_tbf_est_data_ptr);

  if ( event_in == EV_INPUT_MESSAGE )
  {
    IMH_T *message_header = (IMH_T *)event_data;

    message_set = message_header->message_set;
    message_id  = message_header->message_id;
  }

  while ( event_in != EV_NO_EVENT )
  {
    if ( event_in == EV_INPUT_MESSAGE )
    {
      /*****************************************************************/
      /* MAC->GRR: MAC_GRR_UL_TBF_REQ                                  */
      /* Passed thru from grr_control (in GRR_CAMPED state).           */
      /*****************************************************************/
      if ( (message_set == MS_MAC_RR) &&
           (message_id == (byte)MAC_GRR_UL_TBF_REQ) &&
           (event_data != NULL) )
      {
        mac_grr_ul_tbf_req_t *mac_grr_ul_tbf_req = (mac_grr_ul_tbf_req_t *) event_data;

        /*
        Process the UL TBF request, and determine if an access attempt is allowed.
        The possible return events here are:
        EV_GPRS_INIT_ACCESS_PROCEDURE
         - RACHing is allowed for access
        EV_GPRS_ACCESS_NOT_POSSIBLE
         - Access is not allowed, due to: T3142 running; Access Class; Radio Priority
        */
        new_event = grr_process_mac_grr_ul_tbf_req_msg(
          mac_grr_ul_tbf_req,
          gas_id
        );
      }

      else if ( (message_set == MS_MAC_RR) &&
                (message_id == (byte)MAC_GRR_RACH_CNF) )
      {
        /*
        Check if T3142 is running. If so, access to the cell is not allowed.
        (TRUE means access is blocked. If access is blocked, MAC is blocked by sending
        NO_PS_ACCESS_IND from gprs_mac_suspension_control).
        Note: This T3142 check is performed after the RACH_REQ/CNF handshake, to stop the case
        there MAC is blocked with NO_PS_ACCESS_IND due to T3142 running, but actually, it has
        just started processing a DL Imm Ass, and therefore the UL TBF REQ can be ignored.
        */
        if (!rr_mac_ul_tbf_est_is_allowed(gas_id))
        {
          MSG_GERAN_HIGH_0_G("UL request blocked because access not allowed");
          event_out = EV_GRR51_GO_TO_PACKET_IDLE;
        }
        else
        {
          new_event = rr_connection_establishment_control(
            EV_GPRS_INIT_ACCESS_PROCEDURE,
            NULL,
            gas_id
          );

          rr_tbf_est_data_ptr->raching_in_progress = TRUE;
        }
      }

      else if ( (message_set == MS_MAC_RR) &&
                (message_id == (byte)MAC_GRR_RACH_REJ) )
      {
        MSG_GERAN_HIGH_0_G("GRR UL TBF conn est abandoned");
        event_out = EV_GRR51_GO_TO_PACKET_IDLE;
      }

      /*****************************************************************/
      /* L2->RR: DL_RANDOM_ACCESS_CNF                                  */
      /* Confirmation from L2 that L1 is RACHing                       */
      /* Just pass to Conn Est - there is no expected response         */
      /*****************************************************************/
      else if (message_set == MS_RR_L2 && message_id == (byte)DL_RANDOM_ACCESS_CNF)
      {
        (void) rr_connection_establishment_control(
          EV_INPUT_MESSAGE,
          event_data,
          gas_id
        );
      }

      /*****************************************************************/
      /* L2->RR: DL_UNIT_DATA_IND                                      */
      /* EV_GPRS_IMMEDIATE_ASSIGNMENT is returned if the message       */
      /* contains the expected GPRS immediate assignment.              */
      /*****************************************************************/
      else if (message_set == MS_RR_L2 && message_id == (byte)DL_UNIT_DATA_IND)
      {
        new_event = rr_connection_establishment_control(
          EV_INPUT_MESSAGE,
          event_data,
          gas_id
        );
      }

      /*****************************************************************/
      /* L1->RR: MPH_BLOCK_QUALITY_IND                                  */
      /* Pass this message to connection est state machine              */
      /*****************************************************************/
      else if (message_set == MS_RR_L1 && message_id == (byte)MPH_BLOCK_QUALITY_IND)
      {
        event_out = rr_connection_establishment_control(
          EV_INPUT_MESSAGE,
          event_data,
          gas_id
        );
      }

      /***********************************/
      /* Handle Timers that have expired */
      /***********************************/
      else if ((message_set == MS_TIMER) && (message_id == MID_TIMER_EXPIRY))
      {
        timer_expiry_T *timer_expiry = (timer_expiry_T *) event_data;

        if (timer_expiry->timer_id == TMULTIFRM)
        {
          /* Timer for 2nd of a 2 part IA msg */
          new_event = rr_connection_establishment_control(
            EV_TMULTIFRM_EXPIRY,
            NULL,
            gas_id
          );
        }
        else if (timer_expiry->timer_id == T3146)
        {
          /* EV_FORCE_CELL_RESELECTION or RR_EV_START_RESELECTION_POWER_SCAN is returned */
          new_event = rr_connection_establishment_control(
            EV_T3146_EXPIRY,
            NULL,
            gas_id
          );
        }
        else if (timer_expiry->timer_id == T3174)
        {
          new_event = rr_connection_establishment_control(
            EV_T3174_EXPIRY,
            NULL,
            gas_id
          );
        }
        else if (timer_expiry->timer_id == T3176)
        {
          /* EV_FORCE_CELL_RESELECTION is returned */
          new_event = rr_connection_establishment_control(
            EV_T3176_EXPIRY,
            NULL,
            gas_id
          );
        }
        else if (timer_expiry->timer_id == T3158)
        {
          /* NC measurement report control is notified */
          (void) rr_gprs_nc_measurement_control(
            EV_T3158_TIMEOUT,
            NULL,
            gas_id
          );
        }
      }

      /*****************************************************************/
      /* L1->RR: MPH_SURROUND_MEAS_IND or MPH_SERVING_IDLE_MEAS_IND    */
      /* Layer 1 is providing serving/surrounding cell measurements    */
      /* At present these signals are just absorbed without processing */
      /*****************************************************************/
      else if (((message_set == MS_RR_L1) && (message_id == (byte)MPH_SURROUND_MEAS_IND)) ||
               ((message_set == MS_RR_L1) && (message_id == (byte)MPH_SERVING_IDLE_MEAS_IND)))
      {
        MSG_GERAN_MED_1_G("L1 measurement (%02X) ignored - conn. pending", message_id);
        /* may eventually need to pass these signals on to reselection engine */
      }

      else if (message_set == MS_RR_RR)
      {
        rr_cmd_bdy_type * message = (rr_cmd_bdy_type *) event_data;
        switch (message->rr.header.rr_message_set)
        {
          case RR_GRR_IMSG:
          {
            switch (message->rr.header.imh.message_id)
            {
              case RR_GRR_IMSG_NO_PS_ACCESS_REQ:
              {
                // No PS access request (could be TLLI unassignment, CS-only)

                rr_grr_imsg_no_ps_access_req_t *imsg = &message->rr.grr.imsg.no_ps_access_req;

                // Cancel the current TBF establishment request and delete PDUs as required
                if (imsg->del_all_pdus)
                {
                  rr_send_grr_mac_no_ps_access_ind(
                    DEL_ALL,
                    gas_id
                  );
                }
                else
                {
                  rr_send_grr_mac_no_ps_access_ind(
                    NO_DEL,
                    gas_id
                  );
                }

                // Call the call-back function (if one was supplied)
                // TODO - this should be done once MAC has confirmed
                CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);

                // Instruct L1 to stop RACHing
                event_out = abort_tbf_est(gas_id);
                break;
              } /* RR_GRR_IMSG_NO_PS_ACCESS_REQ */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message, "(stateless)");
              }
            }
            break;
          }

          case RR_CE_OMSG:
          {
            switch (message->rr.header.imh.message_id)
            {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
              case RR_CE_OMSG_X2G_CCO_STATUS_IND:
              {
                switch (message->rr.ce.omsg.x2g_cco_status_ind.status)
                {
                  case RR_CCO_STATUS_SUCCESS:
                  {
                    MSG_GERAN_ERROR_0_G("Unexpected success indication (normally only expected for CSFB...)");
                    break;
                  } /* RR_CCO_STATUS_SUCCESS */

                  default:
                  {
                    event_out = rr_gprs_ce_map_rr_ce_x2g_cco_failure_cause(
                      message->rr.ce.omsg.x2g_cco_status_ind.status
                    );
                  }
                }
                break;
              } /* RR_CE_OMSG_X2G_CCO_STATUS_IND */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message, "(stateless)");
              }
            }
            break;
          } /* RR_CE_OMSG */

          case RR_PS_IMSG:
          case RR_PS_OMSG:
          case RR_L1_IDLE_IMSG:
          case RR_L1_IDLE_OMSG:
          {
            event_out = rr_connection_establishment_control(
              EV_INPUT_MESSAGE,
              event_data,
              gas_id
            );
            break;
          }

          default:
          {
            LOG_UNEXPECTED_MESSAGE(message, "(stateless)");
          }
        }
      }

      else
      {
        /* unexpected message received while in this state */
        MSG_GERAN_ERROR_2_G("Unexpected Msg:Id(%d,%d)", (int) message_set, (int) message_id);
      }
    }

    else
    /*****************************************************************/
    /* EV_GPRS_INIT_ACCESS_PROCEDURE                                 */
    /* Access to the cell is allowed - start the RACH procedure      */
    /*****************************************************************/
    if ( event_in == EV_GPRS_INIT_ACCESS_PROCEDURE)
    {
      /* Send GRR_MAC_RACH_REQ to MAC to confirm it is ok to proceed */
      rr_send_grr_mac_rach_req(gas_id);

      /* Move the GRR main state to GRR_CONNECTION_PENDING */
      event_out = EV_GRR_EST_REQ;
    }

    else
    /*****************************************************************/
    /* EV_GPRS_ACCESS_NOT_POSSIBLE                                   */
    /* Access to the cell is not possible, due to:                   */
    /*  - T3142 running                                              */
    /*  - Access Class check failed                                  */
    /*  - Radio Priority check failed                                */
    /*****************************************************************/
    if ( event_in == EV_GPRS_ACCESS_NOT_POSSIBLE )
    {
      /* No action required - MAC has already been send GRR_MAC_UL_TBF_EST_REJ */
      ;
    }

    else
    /*****************************************************************/
    /* RR_EV_MM_REQ_RECEIVED_ABORT                                   */
    /* The PS conn est needs to be abandoned to handle a message     */
    /* from MM (e.g. RR_EST_REQ, RR_DEACT_REQ, etc)                  */
    /*****************************************************************/
    if ( event_in == RR_EV_MM_REQ_RECEIVED_ABORT )
    {
      // Instruct L1 to stop RACHing
      event_out = abort_tbf_est(gas_id);
    }

    else
    /*****************************************************************/
    /* EV_GRR_EST_REQ                                                */
    /* This is generated is response to a MAC_GRR_UL_TBF_REQ from    */
    /* MAC. RACHing has begun to gain access for GPRS. Move to the   */
    /* GRR_CE_PS_CONN_EST state and process future messages there    */
    /*****************************************************************/
    if ( event_in == EV_GRR_EST_REQ )
    {
      MSG_GERAN_MED_0_G("Waiting for GPRS Imm Ass...");
    }

    else
    /*****************************************************************/
    /* EV_GRR51_GO_TO_PACKET_IDLE                                    */
    /* This event is generated in response to some conn est process  */
    /* which has now completed, so GRR has nothing left to do, so    */
    /* return to packet idle.                                        */
    /* (e.g. A GPRS Immediate Assignment has been received in        */
    /* response to the RACHs sent to gain access. MAC has been sent  */
    /* either a GRR_MAC_UL_TBF_EST or a GRR_MAC_UL_SB_EST).          */
    /*****************************************************************/
    if ( event_in == EV_GRR51_GO_TO_PACKET_IDLE )
    {
      event_out = EV_GRR51_GO_TO_PACKET_IDLE;
    }

    else
    /*****************************************************************/
    /* EV_FORCE_CELL_RESELECTION                                     */
    /* This event is generated in response to a T3146 expiry, after  */
    /* the max RACH attempts have been made. Reselect to a new cell  */
    /*****************************************************************/
    if ( event_in == EV_FORCE_CELL_RESELECTION )
    {
      event_out = EV_FORCE_CELL_RESELECTION;
    }

    else
    /*****************************************************************/
    /* RR_EV_START_RESELECTION_POWER_SCAN                            */
    /*****************************************************************/
    if ( event_in == RR_EV_START_RESELECTION_POWER_SCAN )
    {
      event_out = RR_EV_START_RESELECTION_POWER_SCAN;
    }

    else
    /*****************************************************************/
    /* EV_TBF_CONN_EST_FAILURE                                        */
    /*****************************************************************/
    if ( event_in == EV_TBF_CONN_EST_FAILURE )
    {
      event_out = EV_GRR51_GO_TO_PACKET_IDLE;
    }

    else
    /*****************************************************************/
    /* EV_GO_TO_IDLE                                                 */
    /*****************************************************************/
    if ( event_in == EV_GO_TO_IDLE )
    {
      event_out = EV_GRR51_GO_TO_PACKET_IDLE;
    }

    else
    /*****************************************************************/
    /* PCCO timer expiry handling                                    */
    /*****************************************************************/
    if ( ( event_in == EV_T3174_TIMEOUT ) ||
         ( event_in == EV_T3176_TIMEOUT ) ||
         ( event_in == EV_PCCO_FAILED ) )
    {
      event_out = event_in;
    }

    else
    /*****************************************************************/
    /* EV_RESET_SOFTWARE                                             */
    /* Clean-up as required                                          */
    /*****************************************************************/
    if ( event_in == EV_RESET_SOFTWARE )
    {
      event_in = EV_NO_EVENT;
    }

    if ( new_event != EV_NO_EVENT )
    {
      event_in = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event_in = EV_NO_EVENT;
    }
  }

  if ( ( event_out == EV_GRR51_GO_TO_PACKET_IDLE ) ||
       ( event_out == EV_FORCE_CELL_RESELECTION ) ||
       ( event_out == RR_EV_START_RESELECTION_POWER_SCAN ) ||
       ( event_out == EV_DSF_RESELECTION ) || 
       ( event_out == EV_T3174_TIMEOUT ) ||
       ( event_out == EV_T3176_TIMEOUT ) ||
       ( event_out == EV_TBF_CONN_EST_ABORTED_CAMPED ) ||
       ( event_out == EV_TBF_CONN_EST_ABORTED_NON_CAMPED ) ||
       ( event_out == EV_PCCO_FAILED )
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
       ||
       ( event_out == EV_X2G_CCO_FAILURE_CCO_TIMER_EXPIRY ) ||
       ( event_out == EV_X2G_CCO_FAILURE_NO_RESPONSE ) ||
       ( event_out == EV_X2G_CCO_FAILURE_IA_REJECT ) ||
       ( event_out == EV_X2G_CCO_FAILURE_CONTENTION_RESOLUTION_FAILURE ) ||
       ( event_out == EV_X2G_CCO_FAILURE_DSF )
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_SGLTE
       ||
       ( event_out == EV_SGLTE_X2G_CCO_FAILED_GO_TO_IDLE )
#endif /* FEATURE_SGLTE */
       )

  {
    rr_ce_reset_egprs_access(gas_id);

    // Check if expecting 2nd part of IA
    if (rr_timer_is_running(TMULTIFRM, gas_id) == FALSE)
    {
      // Discard the memory allocated for the control/data structure
      rr_tbf_est_discard_data(gas_id);
    }

    rr_tbf_est_data_ptr->raching_in_progress = FALSE;
  }

  return event_out;

} /* end grr_conn_est_control() */


void grr_start_t3146(const gas_id_t gas_id)
{
  if (!rr_timer_is_running(T3146, gas_id))
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
    word t3146_timeout_ms = rr_compute_rach_timeout_value(&scell_info_ptr->gsm);

    MSG_GERAN_HIGH_1_G("Starting T3146(%d)",(int) t3146_timeout_ms);
    (void)rr_timer_start(T3146, (dword)(t3146_timeout_ms), gas_id);
  }

} /* end grr_start_t3146() */


/**
 * Called when this module is activated.
 * Note: The primary purpose of this function is to initialise
 * control/data structures, which may include allocating the storage for them.
 * It can be assumed that the pointer to the control/data structure has been
 * setup according to the data instance required.
 */
void rr_tbf_est_activate_init(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_tbf_est_static_data_t *rr_tbf_est_data_ptr = rr_tbf_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_tbf_est_data_ptr);

  // Storage should not have been allocated already
  RR_ASSERT_FATAL(rr_tbf_est_data_ptr->data_ptr == NULL);

  // Allocate storage for the control/data structure
  rr_tbf_est_data_ptr->data_ptr = GPRS_MEM_CALLOC(sizeof(rr_tbf_est_data_t));

} /* end rr_tbf_est_activate_init() */


/**
 * Called when this module is deactivated.
 * Note: The primary purpose of this function is to free any memory allocated
 * for the control/data structure
 */
void rr_tbf_est_discard_data(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_tbf_est_static_data_t *rr_tbf_est_data_ptr = rr_tbf_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_tbf_est_data_ptr);

  if (rr_tbf_est_data_ptr->data_ptr != NULL)
  {
    // Discard any storage allocate control/data structure
    GPRS_MEM_FREE(rr_tbf_est_data_ptr->data_ptr);
    rr_tbf_est_data_ptr->data_ptr = NULL;
  }

} /* end rr_tbf_est_discard_data() */


/**
 * Initialisation function called when RR task is started.
 * Note: The primary purpose of this function is to initialise
 * pointers to control/data structures. This should also take into
 * account any second instance of data, as this is a one-off
 * initialisation.
 */
void rr_tbf_est_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(rr_tbf_est_static_data, 0, sizeof(rr_tbf_est_static_data));

} /* end rr_tbf_est_task_start_init() */


boolean grr_return_tbf_data_structure_pointer(const gas_id_t gas_id)
{

 // Obtain a pointer to the module data
 rr_tbf_est_static_data_t *rr_tbf_est_data_ptr = rr_tbf_est_get_data_ptr(gas_id);
 RR_NULL_CHECK_FATAL(rr_tbf_est_data_ptr);

 if (rr_tbf_est_data_ptr->data_ptr == NULL)
   {
    MSG_HIGH("rr_tbf_est_data_ptr->data_ptr r set to NULL",0,0,0);
    return (TRUE);
   }
 else
   {
     return(FALSE);
   }
}


/* EOF */

