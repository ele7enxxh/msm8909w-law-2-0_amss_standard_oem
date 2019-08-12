/*! \file rr_ce_paging.c

  Functionally contained in this modules covers
   - decooding of Paging Request messages
   - saving of CS and PS pages
   - forwarding pages to NAS
   - control of L1 Page Mode with respect to OTA Page Mode

                Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_ce_paging.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "rr_ce_paging.h"
#include "rr_sys_info_ncell.h"
#include "rr_defs.h"
#include "rr_l1.h"
#include "rr_seg_load.h"
#include "csn.h"
#include "ghdi_exp.h"
#ifdef FEATURE_DUAL_SIM
#include "rr_multi_sim.h"
#endif /* FEATURE_DUAL_SIM */
#include "rr_if.h"
#include "rr_sim.h"
#include "rr_decode_ie.h"
#include "libprot.h"
#include "sys_cnst.h"
#include "rr_log.h"
#include "rr_l1_send.h"
#include "gprs_mem.h"
#include "rr_mm_send.h"
#include "rr_conn_establish.h"
#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
#include "rr_gprs_main.h"
#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */
#include "stringl/stringl.h"


#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
extern void ghdi_ds_read_imeisv(byte *imeisv, sys_modem_as_id_e_type as_id);
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define PAGING_MATCHED_IDENTITY_1 1
#define PAGING_MATCHED_IDENTITY_2 2
#define PAGING_MATCHED_IDENTITY_3 3
#define PAGING_MATCHED_IDENTITY_4 4

/* This macro defines the size of the array that must be passed to the function
read_imeisv() - IMEISV has 16 (BCD) digits while IMEI has 15 digits. */
#define RR_IMEISV_SIZE 10

#define RR_DECREMENT_OCTET_COUNT(cnt,amt) if ((cnt) >= (amt)) {(cnt)-= (amt);}

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  paging_mode_T ota_page_mode;
  rr_page_info_t page_info;
  rr_page_info_t *saved_cs_page_info_ptr;
  rr_page_info_t *saved_ps_page_info_ptr;
} rr_paging_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_paging_data_t rr_paging_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_ps_access_data_t*
 */
static rr_paging_data_t *rr_ce_paging_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_paging_data[as_index];
}

/*!
 * \brief Prepares a buffer suitable for decoding P1/P2 Rest Octets.
 *
 * \param rest_octets_ptr - Input buffer
 * \param num_octets - Length of input buffer contents
 * \param rest_octets_buf_ptr - Output buffer
 */
static void rr_ce_prepare_rest_octets_buffer(
  const byte *rest_octets_ptr,
  byte num_octets,
  byte *rest_octets_buf_ptr
)
{
  /* set the whole buffer to 0x2B so that the bits appears as "L" values */
  memset(rest_octets_buf_ptr, 0x2B, N201_MAX);

  if ((num_octets < N201_MAX) && (rest_octets_ptr != NULL))
  {
    /* copy over the real octets */
    if (num_octets == 1)
    {
      *rest_octets_buf_ptr = *rest_octets_ptr;
    }
    else
    {
      memscpy(rest_octets_buf_ptr, N201_MAX, rest_octets_ptr, num_octets);
    }
  }

  return;
}

/*!
 * \brief Compares the two TMSI IE values (arrays of four bytes).
 *
 * \param tmsi1
 * \param tmsi2
 *
 * \return boolean - TRUE if tmsi1=tmsi2; FALSE otherwise
 */
static boolean rr_ce_match_paging_tmsi(byte *tmsi1, byte *tmsi2)
{
  int index;
  for (index = 0; index < 4; index++)
  {
    if (tmsi1[index] != tmsi2[index])
    {
      return FALSE;
    }
  }

  return TRUE;
} /* rr_match_paging_tmsi */

/*!
 * \brief Checks if the OTA identity given matches the identity of the mobile.
 *
 * \param IE - byte array containing Mobile Identity IE from OTA message
 * \param decoding_ok - updated as FALSE if an error occurred; TRUE otherwise
 * \param paging_id_type - updated to show which paging identity type matched
 *
 * \return boolean - TRUE if the identity matches that of the mobile; FALSE otherwise
 */
static boolean rr_ce_check_identity(
  byte *IE,
  boolean *decoding_ok,
  rr_identity_type_e *paging_id_type,
  const gas_id_t gas_id
)
{
  byte  identity_length;
  byte  type_of_identity;
  byte  index;
  byte  tmsi[TMSI_SIZE];
  byte  tmsi_len;  /* range 0-4 */
  byte  tmsi_shift;
  byte  imsi[MM_SIM_IMSI_SIZE];

  boolean current_mobile = FALSE;

  identity_length = IE[0];
  type_of_identity = IE[1] & 0x07;

  *decoding_ok = TRUE;

  switch (type_of_identity)
  {
    case IMSI :
    {
      if (rr_siminfo_get_imsi(imsi,sizeof(imsi), gas_id))
      {
        current_mobile = TRUE;

        for (index = 0; index < identity_length + 1 && index < MM_SIM_IMSI_SIZE; ++index)
        {
          if (imsi[index] != IE[index])
          {
            current_mobile = FALSE;
            break;
          }
        }

        if (current_mobile == TRUE)
        {
          *paging_id_type = RR_IDENTITY_TYPE_IMSI;
        }
      }

      break;
    }

    case IMEI :
    {
      uint8 current_identity[NV_UE_IMEI_SIZE];

      memset(current_identity, 0, NV_UE_IMEI_SIZE);
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
      (void)ghdi_ds_read_imei(current_identity, geran_map_gas_id_to_nas_id(gas_id));
#else
      (void)ghdi_read_imei(current_identity);
#endif /* FEATURE_DUAL_SIM && FEATURE_DUAL_IMEI */
      current_mobile = TRUE;

      for (index = 0; index < identity_length + 1 && index < NV_UE_IMEI_SIZE; index++)
      {
        if (current_identity[index] != IE[index])
        {
          current_mobile = FALSE;
          break;
        }
      }

      if (current_mobile == TRUE)
      {
        *paging_id_type = RR_IDENTITY_TYPE_IMEI;
      }

      break;
    }

    case IMEISV :
    {
      uint8 current_identity[RR_IMEISV_SIZE];

      memset(current_identity, 0, RR_IMEISV_SIZE);
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
      ghdi_ds_read_imeisv(current_identity, geran_map_gas_id_to_nas_id(gas_id));
#else
      read_imeisv(current_identity);
#endif /* FEATURE_DUAL_SIM && FEATURE_DUAL_IMEI */
      current_mobile = TRUE;

      for (index = 0; index < identity_length + 1 && index < RR_IMEISV_SIZE; index++)
      {
        if (current_identity[index] != IE[index])
        {
          current_mobile = FALSE;
          break;
        }
      }

      if (current_mobile == TRUE)
      {
        *paging_id_type = RR_IDENTITY_TYPE_IMEISV;
      }

      break;
    }

    case TMSI :
    {
      uint8 current_identity[MM_SIM_TMSI_SIZE];

      memset(current_identity, 0, MM_SIM_TMSI_SIZE);

      /* extract TMSI from IE */
      memset(tmsi, 0x00, TMSI_SIZE);

      if (identity_length != 0)
      {
        tmsi_len = identity_length - 1;  /* don't count id byte */
      }
      else
      {
        tmsi_len = 0;
      }

      tmsi_shift = TMSI_SIZE - tmsi_len;

      if (tmsi_len > TMSI_SIZE)
      {
        MSG_GERAN_ERROR_1_G("Invalid TMSI length: %d",tmsi_len);
      }
      else
      {
        size_t num_bytes = MIN(tmsi_len, TMSI_SIZE-tmsi_shift);

        /* for LINT's benefit, add a check to avoid buffer overrun */
        if ((num_bytes > 0) && (tmsi_shift+num_bytes <= TMSI_SIZE))
        {
          //Using memscpy rather memcpy 
		  memscpy(tmsi + tmsi_shift,sizeof(tmsi)-tmsi_shift,IE + 2, num_bytes);		  
        }
        else
        {
          MSG_GERAN_ERROR_3_G("BAD TMSI buffer op: %d %d %d", tmsi_len, tmsi_shift, num_bytes);
        }
      }

      /* extract TMSI from SIM preset to ff to avoid uninitialised data */
      current_mobile = FALSE;
      memset( current_identity, 0xff, sizeof(current_identity));

      if (rr_siminfo_get_tmsi(
            current_identity,   // tmsi
            sizeof(current_identity),
            gas_id              // gas_id
          )
         )
      {
        if (!memcmp(tmsi, current_identity, TMSI_SIZE))
        {
          current_mobile = TRUE;
          *paging_id_type = RR_IDENTITY_TYPE_TMSI;
        }
        else
        {
          /* Compare current_identity to the P-TMSI value stored by MM */
          current_mobile = rr_ce_compare_ota_ptmsi(
            IE+2,       // IE
            gas_id      // gas_id
          );

          if (current_mobile == TRUE)
          {
            *paging_id_type = RR_IDENTITY_TYPE_PTMSI;
          }
        }
      }

      break;
    }

    case NO_IDENTITY :
    {
      break;
    }

    default :
    {

      /***********************************/
      /* Those other values are reserved */
      /***********************************/
      *decoding_ok = FALSE;
    }
  }

  return current_mobile;
}

/*!
 * \brief Logs an identity match on a paging message.
 *
 * \param identity_num - Indication of which identity within the OTA message matched.
 */
static void rr_ce_log_page_match(
  const int identity_num,
  const gas_id_t gas_id
)
{
  MSG_GERAN_HIGH_1_G("Page is for mobile (ID%1d)", identity_num);
  return;
}

/*!
 * \brief Processes message PAGING REQUEST TYPE 1.
 *
 *  Called when a Paging Request Type 1 OTA message is received from the network. These messages contain
 *  potentially two Mobile Identities encoded in the Mobile Identity IE format.
 *
 * \param own_paging_block - Set to TRUE if the DL_UNIT_DATA_IND was received on the mobile's paging block
 * \param page_info_ptr - Updated with the details of the page
 *
 * \return boolean - TRUE if the page is for the mobile
 */
static boolean rr_ce_process_page_request_type_1(
  boolean own_paging_block,
  rr_page_info_t *page_info_ptr,
  const gas_id_t gas_id
)
{
  ie_descriptor_T *current_IE;
  ie_descriptor_T *identity_IE = NULL;
  paging_mode_T paging_mode;
  boolean decoding_ok = TRUE;
  boolean current_mobile = FALSE;
  byte channel_needed_1;
  byte channel_needed_2;
  byte remaining_octets = 0;
  p1_rest_octets_t p1_rest_octets;
  boolean match_1 = FALSE;
  boolean match_2 = FALSE;
  rr_identity_type_e page_identity_type;
  rr_page_domain_e page_domain;

  uint8 remaining_length = 0;

  if (page_info_ptr != NULL)
  {
    page_info_ptr->domain = RR_PAGE_DOMAIN_NONE;
    page_info_ptr->identity_type = RR_IDENTITY_TYPE_NONE;
    memset(page_info_ptr->identity, 0, sizeof(page_info_ptr->identity));
    page_info_ptr->channel_needed = 0;
  }

  /* Initialise rest octets */
  memset(&p1_rest_octets, 0, sizeof(p1_rest_octets));

  /* get a pointer to the first IE and length of the OTA message */
  current_IE = rr_get_first_ie_and_length(
    PAGING_REQUEST_TYPE_1,    // msg_type
    &remaining_octets,        // length_ptr
    gas_id                    // gas_id
  );

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  /* adjust the remaining octet count for the header and first IE */
  RR_DECREMENT_OCTET_COUNT(remaining_octets, 3);
  RR_DECREMENT_OCTET_COUNT(remaining_octets, current_IE->IE_length);

  /*******************************************/
  /* Pick up the Paging Mode and the Channel */
  /* Needed informations                     */
  /*******************************************/
  paging_mode    = (byte) (*(current_IE->start_IE) & 0x0F);
  channel_needed_1 = (byte) ((*(current_IE->start_IE) & 0x30) >> 4);
  channel_needed_2 = (byte) ((*(current_IE->start_IE) & 0xC0) >> 6);

  /***************************************************/
  /* Update the page mode if it is needed by sending */
  /* the appropriate message to the layer 1          */
  /***************************************************/
  rr_ce_check_for_page_mode_update(
    own_paging_block,     // own_paging_block
    paging_mode,          // ota_page_mode
    gas_id                // gas_id
  );

  /* Decode the first mobile identity */
  /* the IE has length and value only */
  current_IE = current_IE->next_IE;

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  RR_DECREMENT_OCTET_COUNT(remaining_octets, current_IE->start_IE[0]+1);

  current_mobile = rr_ce_check_identity(
    current_IE->start_IE,     // IE
    &decoding_ok,             // decoding_ok
    &page_identity_type,      // paging_id_type,
    gas_id                    // gas_id
  );

  if (current_mobile == TRUE)
  {
    match_1 = TRUE;

    rr_ce_log_page_match(
      PAGING_MATCHED_IDENTITY_1,    // identity
      gas_id                        // gas_id
    );

    // The first mobile identity matches the mobile's identity
    identity_IE = current_IE;

    if (page_info_ptr != NULL)
    {
      page_info_ptr->channel_needed = rr_ce_map_paging_request_channel_needed(
        channel_needed_1    // channel_needed_value
      );
    }
  }

  // Decode the second mobile identity (only if first didn't match)
  current_IE = current_IE->next_IE;

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  if (current_IE->start_IE != NULL)
  {
    RR_DECREMENT_OCTET_COUNT(remaining_octets, current_IE->start_IE[0]+2);
  }

  if ((decoding_ok == TRUE) && (current_mobile == FALSE))
  {
    /* second identity is optional and may not be present */
    if (current_IE->start_IE != NULL)
    {
      current_mobile = rr_ce_check_identity(
        current_IE->start_IE,   // IE
        &decoding_ok,           // decoding_ok
        &page_identity_type,    // paging_id_type
        gas_id                  // gas_id
      );

      if (current_mobile == TRUE)
      {
        match_2 = TRUE;

        rr_ce_log_page_match(
          PAGING_MATCHED_IDENTITY_2,    // identity
          gas_id                        // gas_id
        );

        // Second mobile identity matches the mobile
        identity_IE = current_IE;

        if (page_info_ptr != NULL)
        {
          page_info_ptr->channel_needed = rr_ce_map_paging_request_channel_needed(
            channel_needed_2    // channel_needed_value
          );
        }
      }
    }
  }

  if ((current_mobile == TRUE) && (decoding_ok == TRUE))
  {
    /* Current mobile is addressed in page request and message decoded */

    /* Move to next IE which is P1 Rest Octets and decode these for use later */
    current_IE = current_IE->next_IE;

    if (current_IE == NULL)
    {
      MSG_GERAN_ERROR_0_G("current_IE == NULL");
      return EV_NO_EVENT;
    }

    if (current_IE->start_IE != NULL)
    {
      byte *rest_octets_ptr;

      remaining_length =
        rr_length_to_end_of_ota_msg(
          PAGING_REQUEST_TYPE_1,    // msg_type
          current_IE->start_IE,     // csn1_start_ie_ptr
          P1_REST_OCTETS,           // csn_ie_type
          gas_id
        );

      // Obtain a temporary buffer to decode the rest octets
      rest_octets_ptr = GPRS_MEM_MALLOC(N201_MAX);
      RR_NULL_CHECK_FATAL(rest_octets_ptr);

      rr_ce_prepare_rest_octets_buffer(
        current_IE->start_IE,     // rest_octets_ptr
        remaining_octets,         // num_octets
        rest_octets_ptr           // rest_octets_buf_ptr
      );

      (void) gcsn1_rr_decoder(
        P1_REST_OCTETS,     // message_type
        rest_octets_ptr,    // csn1_ie_src
        &p1_rest_octets,    // dst
        remaining_length,   // len_to_end_of_buf
        gas_id
      );

      // The temporary buffer is no longer needed
      GPRS_MEM_FREE(rest_octets_ptr);
    }

    // The identity of the mobile matches the identity in the PAGING_REQUEST msg
    if (identity_IE != NULL)
    {
      byte identity_length = 1 + identity_IE->start_IE[0]; /* include the length octet in the count */
      byte index;

      // Sanity-check that the identity length is not greater than the allocated storage
      if (identity_length > sizeof(page_info_ptr->identity))
      {
        MSG_GERAN_ERROR_1_G("identity_length = %d", identity_length);
        return EV_NO_EVENT;
      }

      // Store the identity. This is used when sending the page response
      if (page_info_ptr != NULL)
      {
        for (index = 0; index < identity_length; index++)
        {
          page_info_ptr->identity[index] = identity_IE->start_IE[index];
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Expected a valid identity");
    }

    // Assume this is a CS page until it is determined otherwise
    page_domain = RR_PAGE_DOMAIN_CS;

    /* Check whether the page is PS page and change event accordingly */
    if (current_IE->start_IE != NULL)
    {
      if (page_identity_type == RR_IDENTITY_TYPE_IMSI)
      {
        /* Check the P1 Rest Octets to see if page was a Packet Page */
        if ( (match_1 && p1_rest_octets.packet_page_ind_1) ||
             (match_2 && p1_rest_octets.packet_page_ind_2) )
        {
          // This is a PS page, but addressed by IMSI
          page_domain = RR_PAGE_DOMAIN_PS;
        }
        /* Else, must be a CS Page, with IMSI as Mobile ID, leave event as is */
      }
      else if (page_identity_type == RR_IDENTITY_TYPE_PTMSI)
      {
        // This is a PS page, addressed by P-TMSI
        page_domain = RR_PAGE_DOMAIN_PS;
      }
    }

    if (page_info_ptr != NULL)
    {
      page_info_ptr->domain = page_domain;
      page_info_ptr->identity_type = page_identity_type;
    }

    return TRUE;   // page is for mobile
  }
  /* Else just ignore the page request */

  return FALSE;   // page is not for mobile (or invalid)
} /* rr_ce_process_page_request_type_1 */

/*!
 * \brief Processes message PAGING REQUEST TYPE 2.
 *
 *  Called when a Paging Request Type 2 OTA message is received from the network. This message can address
 *  two MSs using TMSI/P-TMSI, and optionally a third MS using regular Mobile Identity IE.
 *
 * \param own_paging_block - Set to TRUE if the DL_UNIT_DATA_IND was received on the mobile's paging block
 * \param page_info_ptr - Updated with the details of the page
 *
 * \return boolean - TRUE if the page is for the mobile
 */
static boolean rr_ce_process_page_request_type_2(
  boolean own_paging_block,
  rr_page_info_t *page_info_ptr,
  const gas_id_t gas_id
)
{
  ie_descriptor_T *current_IE;

  /* the IE that matches the current mobile identity */
  ie_descriptor_T *identity_IE = NULL;
  boolean current_mobile = FALSE;
  boolean rr_sim_info_available;
  boolean decoding_ok = TRUE;
  paging_mode_T paging_mode;
  byte current_identity[RR_MAX_IDENTITY_SIZE];
  boolean match_1 = FALSE;
  boolean match_2 = FALSE;
  byte channel_needed_1;
  byte channel_needed_2;
  byte channel_needed_3;
  byte CNI;
  byte remaining_octets = 0;
  p2_rest_octets_t p2_rest_octets;
  rr_identity_type_e page_identity_type;
  rr_page_domain_e page_domain;

  uint8 remaining_length = 0;

  if (page_info_ptr != NULL)
  {
    page_info_ptr->domain = RR_PAGE_DOMAIN_NONE;
    page_info_ptr->identity_type = RR_IDENTITY_TYPE_NONE;
    memset(page_info_ptr->identity, 0, sizeof(page_info_ptr->identity));
    page_info_ptr->channel_needed = 0;
  }

  /* Initialise rest octets */
  memset(&p2_rest_octets, 0, sizeof(p2_rest_octets));

  /* get a pointer to the first IE and length of the OTA message */
  current_IE = rr_get_first_ie_and_length(
    PAGING_REQUEST_TYPE_2,    // msg_type
    &remaining_octets,        // length_ptr
    gas_id                    // gas_id
  );

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  /* adjust the remaining octet count for the header and first IE */
  RR_DECREMENT_OCTET_COUNT(remaining_octets, 3);
  RR_DECREMENT_OCTET_COUNT(remaining_octets, current_IE->IE_length);

  /* Decode the Paging Mode and the Channel Needed information */
  paging_mode    = (byte) (*(current_IE->start_IE) & 0x0F);
  channel_needed_1 = (byte) ((*(current_IE->start_IE) & 0x30) >> 4);
  channel_needed_2 = (byte) ((*(current_IE->start_IE) & 0xC0) >> 6);

  /* Update the L1 page mode if required */
  rr_ce_check_for_page_mode_update(own_paging_block, paging_mode, gas_id);

  /* Decode the first mobile identity : */
  /* This is a TMSI or P-TMSI           */
  current_IE = current_IE->next_IE;

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  RR_DECREMENT_OCTET_COUNT(remaining_octets, current_IE->IE_length);

  /* Get the current TMSI value from the SIM */
  rr_sim_info_available = rr_siminfo_get_tmsi(
    current_identity,   // tmsi
    sizeof(current_identity),
    gas_id              // gas_id
  );

  /* try to match the mobile's TMSI against the paged TMSI */
  if (rr_sim_info_available &&
      rr_ce_match_paging_tmsi(
        current_identity,       // tmsi1
        current_IE->start_IE    // tmsi2
      ))
  {
    rr_ce_log_page_match(
      PAGING_MATCHED_IDENTITY_1,    // identity
      gas_id                        // gas_id
    );
    current_mobile  = TRUE;
    match_1         = TRUE;
    identity_IE     = current_IE;
    page_identity_type = RR_IDENTITY_TYPE_TMSI;
  }

  /* try to match the mobile's identity against the P-TMSI */
  else if (rr_sim_info_available == TRUE)
  {
    current_mobile = rr_ce_compare_ota_ptmsi(
      current_IE->start_IE,     // IE
      gas_id                    // gas_id
    );

    if (current_mobile == TRUE)
    {
      rr_ce_log_page_match(
        PAGING_MATCHED_IDENTITY_1,    // identity
        gas_id                        // gas_id
      );
      match_1 = TRUE;
      identity_IE = current_IE;
      page_identity_type = RR_IDENTITY_TYPE_PTMSI;
    }
  }

  /* Decode the second mobile identity : */
  /* This is a TMSI or P-TMSI           */
  current_IE = current_IE->next_IE;

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  RR_DECREMENT_OCTET_COUNT(remaining_octets, current_IE->IE_length);

  if (current_mobile == FALSE)
  {
    if (rr_sim_info_available &&
        rr_ce_match_paging_tmsi(
          current_identity,       // tmsi1
          current_IE->start_IE    // tmsi2
        ))
    {
      rr_ce_log_page_match(
        PAGING_MATCHED_IDENTITY_2,  // identity
        gas_id                      // gas_id
      );
      match_2         = TRUE;
      current_mobile  = TRUE;
      identity_IE     = current_IE;
      page_identity_type = RR_IDENTITY_TYPE_TMSI;
    }
    else if (rr_sim_info_available == TRUE)
    {
      /* try to match the mobile's identity against the P-TMSI */
      current_mobile = rr_ce_compare_ota_ptmsi(
         current_IE->start_IE,  // IE
         gas_id                 // gas_id
       );

      if (current_mobile == TRUE)
      {
        rr_ce_log_page_match(
          PAGING_MATCHED_IDENTITY_2,    // identity
          gas_id                        // gas_id
        );
        match_2 = TRUE;
        identity_IE = current_IE;
        page_identity_type = RR_IDENTITY_TYPE_PTMSI;
      }
    }
  }

  /* Map the channel needed value based on the matching identity (if any) */
  if (match_1 == TRUE)
  {
    if (page_info_ptr)
    {
      page_info_ptr->channel_needed = rr_ce_map_paging_request_channel_needed(
        channel_needed_1    // channel_needed_value
      );
    }
  }
  else if (match_2 == TRUE)
  {
    if (page_info_ptr)
    {
      page_info_ptr->channel_needed = rr_ce_map_paging_request_channel_needed(
        channel_needed_2    // channel_needed_value
      );
    }
  }

  // Decode the third mobile identity if present & required
  current_IE = current_IE->next_IE;

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  if (current_IE->start_IE != NULL)
  {
    /* reduce the remaining octet count by the identity length   */
    /* and by a further 2 to cater for the IEI and length octets */
    RR_DECREMENT_OCTET_COUNT(remaining_octets, current_IE->start_IE[0]+2);
  }

  if (current_mobile == FALSE)
  {
    /* the third identity is optional, so it may not be present */
    if (current_IE->start_IE != NULL)
    {
      /* The third identity is encoded as a mobile identity IE */
      current_mobile = rr_ce_check_identity(
        current_IE->start_IE,     // IE
        &decoding_ok,             // decoding_ok
        &page_identity_type,      // paging_id_type
        gas_id                    // gas_id
      );

      if ((current_mobile == TRUE) && (decoding_ok == TRUE))
      {
        rr_ce_log_page_match(
          PAGING_MATCHED_IDENTITY_3,    // identity
          gas_id                        // gas_id
        );
        identity_IE = current_IE;
      }
    }
  }

  if ((current_mobile == TRUE) && (decoding_ok == TRUE))
  {
    /* current mobile is addressed in page request and message decoded */

    byte index;

    /* The identity of the mobile matches the identity in the PAGING_REQUEST msg */

    if ((match_1 == TRUE) || (match_2 == TRUE))
    {
      /* identity_IE should have been defined */
      if (identity_IE != NULL)
      {
        // Store the identity. This is used when sending the page response
        if (page_info_ptr != NULL)
        {
          /* Special TMSI values used for first two identities in */
          /* PagingRequest type 2. Each is four bytes of TMSI.    */
          page_info_ptr->identity[0] = 5;
          page_info_ptr->identity[1] = 0xF4;

          for (index = 2; index < 6; index++)
          {
            page_info_ptr->identity[index] = identity_IE->start_IE[index - 2];
          }
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("IdentityIE should be defined");
      }
    }
    else /* mobile matched third identity */
    {
      byte *rest_octets_ptr;

      /* identity IE should be defined */
      if (identity_IE != NULL)
      {
        // Store the identity. This is used when sending the page response
        if (page_info_ptr != NULL)
        {
          /* The mobile matched the third identity, which is encoded using */
          /* regular mobile identity format                                */
          byte identity_length = 1 + identity_IE->start_IE[0]; /* include the length octet in the count */
          byte loop_index;

          /* identity will be copied to the RR global store, which holds RR_MAX_IDENTITY octets only */
          if (identity_length > RR_MAX_IDENTITY_SIZE)
          {
            MSG_GERAN_ERROR_1_G("Mobile identity length %d too long,Ignoring page message", identity_length-1);
            return EV_NO_EVENT;
          }

          for (loop_index = 0; loop_index < identity_length; loop_index++)
          {
            page_info_ptr->identity[loop_index] = identity_IE->start_IE[loop_index];
          }
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("IdentityIE should be defined");
      }

      /* decode the channel needed, in the P2 rest octets */
      /* since the mobile matched the third identity      */
      current_IE = current_IE->next_IE;

      if (current_IE == NULL)
      {
        MSG_GERAN_ERROR_0_G("current_IE == NULL");
        return EV_NO_EVENT;
      }

      CNI = (byte) (*(current_IE->start_IE) & 0x80);
      if (CNI == 0)
      {
        channel_needed_3 = 0;
      }
      else
      {
        channel_needed_3 = (byte) ((*(current_IE->start_IE) & 0x60) >> 5);
      }

      if (page_info_ptr != NULL)
      {
        page_info_ptr->channel_needed = rr_ce_map_paging_request_channel_needed(
          channel_needed_3    // channel_needed_value
        );
      }

      remaining_length = rr_length_to_end_of_ota_msg(
        PAGING_REQUEST_TYPE_2,      // msg_type
        current_IE->start_IE,       // csn1_start_ie_ptr
        P2_REST_OCTETS,             // csn_ie_type
        gas_id                      // gas_id
      );

      // Obtain a temporary buffer to decode the rest octets
      rest_octets_ptr = GPRS_MEM_MALLOC(N201_MAX);
      RR_NULL_CHECK_FATAL(rest_octets_ptr);

      /* Decode the P2 Rest Octets to access the Packet Page Indication field */
      rr_ce_prepare_rest_octets_buffer(
        current_IE->start_IE,       // rest_octets_ptr
        remaining_octets,           // num_octets
        rest_octets_ptr             // rest_octets_buf_ptr
      );

      (void) gcsn1_rr_decoder(
        P2_REST_OCTETS,     // message_type
        rest_octets_ptr,    // csn1_ie_src
        &p2_rest_octets,    // dst
        remaining_length,   // len_to_end_of_buf
        gas_id
      );

      // The temporary buffer is no longer needed
      GPRS_MEM_FREE(rest_octets_ptr);

    } /* mobile matched third identity */

    // Assume this is a CS page until it is determined otherwise
    page_domain = RR_PAGE_DOMAIN_CS;

    /* Check whether the page is PS page and change event accordingly */
    if (page_identity_type == RR_IDENTITY_TYPE_IMSI)
    {
      /* Must be the 3rd Mobile Id that was matched, since 1st and 2nd have to be TMSI/P-TMSI type */
      if (p2_rest_octets.packet_page_ind_3)
      {
        // This is a PS page, but addressed by IMSI
        page_domain = RR_PAGE_DOMAIN_PS;
      }
      /* Else, must be a CS Page, with IMSI as Mobile ID, leave event as is */
    }
    else if (page_identity_type == RR_IDENTITY_TYPE_PTMSI)
    {
      // This is a PS page, addressed by P-TMSI
      page_domain = RR_PAGE_DOMAIN_PS;
    }

    if (page_info_ptr != NULL)
    {
      page_info_ptr->domain = page_domain;
      page_info_ptr->identity_type = page_identity_type;
    }

    return TRUE;    // page is for mobile
  }/* Else just ignore the page request */

  return FALSE;   // page is not for mobile (or invalid)
} /* rr_ce_process_page_request_type_2 */

/*!
 * \brief Processes message PAGING REQUEST TYPE 3.
 *
 *  Called when a Paging Request Type 3 OTA message is received from the network. This message contains four
 *  mobile identity slots encoded as TMSI/PTMSI IEs.
 *
 * \param own_paging_block - Set to TRUE if the DL_UNIT_DATA_IND was received on the mobile's paging block
 * \param page_info_ptr - Updated with the details of the page
 *
 * \return boolean - TRUE if the page is for the mobile
 */
static boolean rr_ce_process_page_request_type_3(
  boolean own_paging_block,
  rr_page_info_t *page_info_ptr,
  const gas_id_t gas_id
)
{
  ie_descriptor_T *current_IE;
  ie_descriptor_T *identity_IE = NULL;

  paging_mode_T paging_mode;
  boolean current_mobile = FALSE;
  boolean rr_sim_info_available;
  byte index;
  boolean match_1 = FALSE;
  boolean match_2 = FALSE;
  boolean match_3 = FALSE;
  boolean match_4 = FALSE;
  byte channel_needed_1;
  byte channel_needed_2;
  byte channel_needed_3;
  byte channel_needed_4;
  byte current_identity[RR_MAX_IDENTITY_SIZE];
  byte CNI;

  rr_identity_type_e page_identity_type = RR_IDENTITY_TYPE_NONE;
  rr_page_domain_e page_domain;

  if (page_info_ptr != NULL)
  {
    page_info_ptr->domain = RR_PAGE_DOMAIN_NONE;
    page_info_ptr->identity_type = RR_IDENTITY_TYPE_NONE;
    memset(page_info_ptr->identity, 0, sizeof(page_info_ptr->identity));
    page_info_ptr->channel_needed = 0;
  }

  /* get a pointer to the first IE and length of the OTA message */
  current_IE = rr_get_first_ie_and_length(
    PAGING_REQUEST_TYPE_3,    // msg_type
    NULL,                     // length_ptr
    gas_id                    // gas_id
  );

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  /* Decode the Paging Mode and the Channel Needed information */
  paging_mode    = (byte) (*(current_IE->start_IE) & 0x0F);
  channel_needed_1 = (byte) ((*(current_IE->start_IE) & 0x30) >> 4);
  channel_needed_2 = (byte) ((*(current_IE->start_IE) & 0xC0) >> 6);

  // Update the L1 page mode if required
  rr_ce_check_for_page_mode_update(
    own_paging_block,     // own_paging_block
    paging_mode,          // ota_paging_mode
    gas_id                // gas_id
  );

  /* Decode the first mobile identity */
  current_IE = current_IE->next_IE;

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  rr_sim_info_available = rr_siminfo_get_tmsi(
    current_identity,   // tmsi
    sizeof(current_identity),
    gas_id              // gas_id
  );

  /* If the SIM access fails, the TMSI is invalid and cannot be used to */
  /* match against the identities in the page request                   */
  if (rr_sim_info_available != TRUE)
  {
    current_mobile = FALSE;
  }
  else
  {
    /* SIM access succeeded, TMSI is valid */
    if (rr_ce_match_paging_tmsi(
          current_identity,       // tmsi1
          current_IE->start_IE    // tmsi2
        ))
    {
      rr_ce_log_page_match(
        PAGING_MATCHED_IDENTITY_1,    // identity
        gas_id                        // gas_id
      );
      current_mobile  = TRUE;
      match_1         = TRUE;
      identity_IE     = current_IE;
      page_identity_type = RR_IDENTITY_TYPE_TMSI;
    }
    else
    {
      /* try to match the mobile's identity against the P-TMSI */
      current_mobile = rr_ce_compare_ota_ptmsi(
        current_IE->start_IE,     // IE
        gas_id                    // gas_id
      );

      if (current_mobile == TRUE)
      {
        rr_ce_log_page_match(
          PAGING_MATCHED_IDENTITY_1,    // identity
          gas_id                        // gas_id
        );
        match_1 = TRUE;
        identity_IE = current_IE;
        page_identity_type = RR_IDENTITY_TYPE_PTMSI;
      }
    }

    /* Decode the second mobile identity */
    if (current_mobile == FALSE)
    {
      current_IE = current_IE->next_IE;

      if (current_IE == NULL)
      {
        MSG_GERAN_ERROR_0_G("current_IE == NULL");
        return EV_NO_EVENT;
      }

      if (rr_ce_match_paging_tmsi(
            current_identity,       // tmsi1
            current_IE->start_IE    // tmsi2
          ))
      {
        rr_ce_log_page_match(
          PAGING_MATCHED_IDENTITY_2,    // identity
          gas_id                        // gas_id
        );
        current_mobile  = TRUE;
        match_2         = TRUE;
        identity_IE     = current_IE;
        page_identity_type = RR_IDENTITY_TYPE_TMSI;
      }
      else
      {
        /* try to match the mobile's identity against the P-TMSI */
        current_mobile = rr_ce_compare_ota_ptmsi(
          current_IE->start_IE,   // IE
          gas_id                  // gas_id
        );

        if (current_mobile == TRUE)
        {
          rr_ce_log_page_match(
            PAGING_MATCHED_IDENTITY_2,    // identity
            gas_id                        // gas_id
          );
          match_2 = TRUE;
          identity_IE = current_IE;
          page_identity_type = RR_IDENTITY_TYPE_PTMSI;
        }
      }
    }

    /* Decode the third mobile identity */
    if (current_mobile == FALSE)
    {
      current_IE = current_IE->next_IE;

      if (current_IE == NULL)
      {
        MSG_GERAN_ERROR_0_G("current_IE == NULL");
        return EV_NO_EVENT;
      }

      if (rr_ce_match_paging_tmsi(
            current_identity,       // tmsi1
            current_IE->start_IE    // tmsi2
          ))
      {
        rr_ce_log_page_match(
          PAGING_MATCHED_IDENTITY_3,    // identity
          gas_id                        // gas_id
        );
        current_mobile  = TRUE;
        match_3         = TRUE;
        identity_IE     = current_IE;
        page_identity_type = RR_IDENTITY_TYPE_TMSI;
      }
      else
      {
        /* try to match the mobile's identity against the P-TMSI */
        current_mobile = rr_ce_compare_ota_ptmsi(
          current_IE->start_IE,   // IE
          gas_id                  // gas_id
        );

        if (current_mobile == TRUE)
        {
          rr_ce_log_page_match(
            PAGING_MATCHED_IDENTITY_3,    // identity
            gas_id                        // gas_id
          );
          match_3 = TRUE;
          identity_IE = current_IE;
          page_identity_type = RR_IDENTITY_TYPE_PTMSI;
        }
      }
    }

    /* Decode the fourth mobile identity */
    if (current_mobile == FALSE)
    {
      current_IE = current_IE->next_IE;

      if (current_IE == NULL)
      {
        MSG_GERAN_ERROR_0_G("current_IE == NULL");
        return EV_NO_EVENT;
      }

      if (rr_ce_match_paging_tmsi(
            current_identity,       // tmsi1
            current_IE->start_IE    // tmsi2
          ))
      {
        rr_ce_log_page_match(
          PAGING_MATCHED_IDENTITY_4,    // identity
          gas_id                        // gas_id
        );
        current_mobile  = TRUE;
        match_4         = TRUE;
        identity_IE     = current_IE;
        page_identity_type = RR_IDENTITY_TYPE_TMSI;
      }
      else
      {
        /* try to match the mobile's identity against the P-TMSI */
        current_mobile = rr_ce_compare_ota_ptmsi(
          current_IE->start_IE,   // IE
          gas_id                  // gas_id
        );

        if (current_mobile == TRUE)
        {
          rr_ce_log_page_match(
            PAGING_MATCHED_IDENTITY_4,    // identity
            gas_id                        // gas_id
          );
          match_4 = TRUE;
          identity_IE = current_IE;
          page_identity_type = RR_IDENTITY_TYPE_PTMSI;
        }
      }
    }
  }

  if (current_mobile == TRUE)
  {
    /* The identity of the mobile matches the */
    /* identity in the PAGING_REQUEST msg     */

    /* identity IE should be defined */
    if (identity_IE != NULL)
    {
      if (page_info_ptr != NULL)
      {
        /* All of the identities are encoded using TMSI IE format */
        page_info_ptr->identity[0] = 5;
        page_info_ptr->identity[1] = 0xF4;

        for (index = 2; index < 6; index++)
        {
          page_info_ptr->identity[index] = identity_IE->start_IE[index - 2];
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("IdentityIE should be defined");
    }

    /* Map the channel_needed value based on the matching identity.             */
    /* The channel needed values for identities 3&4 is in the P3 rest octets IE */
    if (match_1 == TRUE)
    {
      if (page_info_ptr != NULL)
      {
        page_info_ptr->channel_needed = rr_ce_map_paging_request_channel_needed(
          channel_needed_1    // channel_needed_value
        );
      }
    }
    else if (match_2 == TRUE)
    {
      if (page_info_ptr != NULL)
      {
        page_info_ptr->channel_needed = rr_ce_map_paging_request_channel_needed(
          channel_needed_2    // channel_needed_value
        );
      }
    }
    else if (match_3 == TRUE)
    {
      /* Need to get the channel needed for identity 3 in P3 rest octets */

      /* The next IE contains the Mobile Identity 4 */
      current_IE = current_IE->next_IE;

      if (current_IE == NULL)
      {
        MSG_GERAN_ERROR_0_G("current_IE == NULL");
        return EV_NO_EVENT;
      }

      /* The next IE contains the P3 Rest Octets   */
      current_IE = current_IE->next_IE;

      if (current_IE == NULL)
      {
        MSG_GERAN_ERROR_0_G("current_IE == NULL");
        return EV_NO_EVENT;
      }

      CNI = *(current_IE->start_IE) & 0x80;

      if (CNI == 0)
      {
        channel_needed_3 = 0;
      }
      else
      {
        channel_needed_3 = (byte) (*(current_IE->start_IE) & 0x60) >> 5;
      }

      if (page_info_ptr != NULL)
      {
        page_info_ptr->channel_needed = rr_ce_map_paging_request_channel_needed(
          channel_needed_3    // channel_needed_value
        );
      }
    }
    else if (match_4 == TRUE)
    {
      /*******************************************************************/
      /* Need to get the channel needed for identity 4 in P3 rest octets */
      /*******************************************************************/

      /*********************************************/
      /* The next IE contains the P3 Rest Octets   */
      /*********************************************/
      current_IE = current_IE->next_IE;

      if (current_IE == NULL)
      {
        MSG_GERAN_ERROR_0_G("current_IE == NULL");
        return EV_NO_EVENT;
      }

      CNI = *(current_IE->start_IE) & 0x80;

      if (CNI == 0)
      {
        channel_needed_4 = 0;
      }
      else
      {
        channel_needed_4 = (byte) (*(current_IE->start_IE) & 0x18) >> 3;
      }

      if (page_info_ptr != NULL)
      {
        page_info_ptr->channel_needed = rr_ce_map_paging_request_channel_needed(
          channel_needed_4    // channel_needed_value
        );
      }
    }
    else
    {
      /* none of the identities match, fatal error */
      MSG_GERAN_ERROR_0_G("Matched current mobile, but no identity match,Ignoring page message");
      return EV_NO_EVENT;
    }

    // Assume this is a CS page until it is determined otherwise
    page_domain = RR_PAGE_DOMAIN_CS;

    /* Check whether the page is PS page and change event accordingly */
    /* Could be any of the 4 Mobile Id's, check the P-TMSI flag */
    if (page_identity_type == RR_IDENTITY_TYPE_PTMSI)
    {
      // This is a PS page, addressed by P-TMSI
      page_domain = RR_PAGE_DOMAIN_PS;
    }

    if (page_info_ptr != NULL)
    {
      page_info_ptr->domain = page_domain;
      page_info_ptr->identity_type = page_identity_type;
    }

    return TRUE;    // page is for mobile
  } /* Else the page is not meant for mobile, so should be ignored */

  return FALSE;   // page is not for mobile (or invalid)
} /* rr_ce_process_page_request_type_3 */

/*!
 * \brief Updates the L1 Page Mode to 'page_mode'. The local copy of OTA Page Mode is also updated.
 *
 * \param page_mode - The new Page Mode
 */
static void rr_ce_update_l1_page_mode(paging_mode_T page_mode, const gas_id_t gas_id)
{
  // Obtain the pointer to the module data
  rr_paging_data_t *rr_paging_data_ptr = rr_ce_paging_get_data_ptr(gas_id);

  // Update L1 with the page mode
  rr_send_mph_change_page_mode_req(
    page_mode,    // paging_mode
    gas_id        // gas_id
  );

  // Update the local copy of the OTA page mode. This is used to compare with the next Page Mode received
  // from an OTA message
  rr_paging_data_ptr->ota_page_mode = page_mode;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

// Checks to see if the received mobile identity is the same as the P-TMSI
boolean rr_ce_compare_ota_ptmsi(byte *IE, const gas_id_t gas_id)
{
  byte sim_ptmsi[PTMSI_SIZE];

  boolean match = FALSE;

  if (rr_siminfo_get_ptmsi(
        sim_ptmsi,    // ptmsi
        sizeof(sim_ptmsi),
        gas_id        // gas_id
      )
     )
  {
    if (memcmp(IE, sim_ptmsi, PTMSI_SIZE) == 0)
    {
      match = TRUE;
    }
  }

  return match;
}

// Checks to see if the received mobile identity is the same as the IMEI/IMSI/TMSI/P-TMSI
boolean rr_ce_compare_ota_mobile_identity(
  byte *IE,
  boolean *decoding_ok,
  rr_identity_type_e *paging_id_type,
  const gas_id_t gas_id
)
{
  boolean current_mobile;

  current_mobile = rr_ce_check_identity(
    IE,               // IE
    decoding_ok,      // decoding_ok
    paging_id_type,   // paging_id_type
    gas_id            // gas_id
  );

  return ((*decoding_ok) && (current_mobile));
}

// Processes a Paging Request message received from the network
void rr_ce_process_page_request(byte message_type, boolean own_paging_block, const gas_id_t gas_id)
{
  rr_page_info_t page_info;
  boolean page_is_for_mobile = FALSE;

  switch (message_type)
  {
    case PAGING_REQUEST_TYPE_1:
    {
      page_is_for_mobile = rr_ce_process_page_request_type_1(
        own_paging_block,   // own_paging_block
        &page_info,         // page_info_ptr
        gas_id              // gas_id
      );
      break;
    }

    case PAGING_REQUEST_TYPE_2:
    {
      page_is_for_mobile = rr_ce_process_page_request_type_2(
        own_paging_block,   // own_paging_block
        &page_info,         // page_info_ptr
        gas_id              // gas_id
      );
      break;
    }

    case PAGING_REQUEST_TYPE_3:
    {
      page_is_for_mobile = rr_ce_process_page_request_type_3(
        own_paging_block,   // own_paging_block
        &page_info,         // page_info_ptr
        gas_id              // gas_id
      );
      break;
    }

    default: ;
  }

  if (page_is_for_mobile)
  {
    // Obtain the pointer to the module data
    rr_paging_data_t *rr_paging_data_ptr = rr_ce_paging_get_data_ptr(gas_id);

    RR_NULL_CHECK_FATAL(rr_paging_data_ptr);

    // The page is for the mobile. Determine if a CS or PS page
    if (page_info.domain == RR_PAGE_DOMAIN_PS)
    {
      rr_log_gprs_page_received(gas_id);
    }
    else
    {
      rr_log_page_received(gas_id);
    }

    // Store the page details locally. This is to allow querying by other modules
    rr_paging_data_ptr->page_info = page_info;

    // Send RR_IMSG_PAGE_IND into RR
    rr_send_imsg_page_ind(
      &page_info,     // page_info_ptr
      gas_id          // gas_id
    );
  }
}

// Maps the channel_needed information received in paging request messages
byte rr_ce_map_paging_request_channel_needed(byte paging_channel_needed_value)
{
  if (paging_channel_needed_value == 0)
  {
    return ANY_CHANNEL;
  }
  else if (paging_channel_needed_value == 1)
  {
    return SDCCH_NEEDED;
  }
  else if (paging_channel_needed_value == 2)
  {
    return FULL_RATE_TRAFFIC;
  }
  else if (paging_channel_needed_value == 3)
  {
    return FULL_RATE_OR_HALF_RATE_TRAFFIC;
  }
  else
  {
    MSG_GERAN_ERROR_1("Bad value(%d) for ChannelNeeded", (int) paging_channel_needed_value);
  }

  return ANY_CHANNEL;
}

// Saves a page so that the indication to NAS can be delayed
void rr_ce_save_page(rr_page_info_t *page_info_ptr, const gas_id_t gas_id)
{
  if (page_info_ptr != NULL)
  {
    // Obtain the pointer to the module data
    rr_paging_data_t *rr_paging_data_ptr = rr_ce_paging_get_data_ptr(gas_id);

    RR_NULL_CHECK_FATAL(rr_paging_data_ptr);

    if (page_info_ptr->domain == RR_PAGE_DOMAIN_CS)
    {
      /**
       * Check if there is any storage allocated for the page, and if not, allocate it. 
       * Normally this would not be already allocated, but if multiple pages are received before they are 
       * forwarded to NAS, a page would be stored here already, which can be over-written) 
       */
      if (rr_paging_data_ptr->saved_cs_page_info_ptr == NULL)
      {
        rr_paging_data_ptr->saved_cs_page_info_ptr = GPRS_MEM_CALLOC(sizeof(rr_page_info_t));
      }

      if (rr_paging_data_ptr->saved_cs_page_info_ptr != NULL)
      {
        (*(rr_paging_data_ptr->saved_cs_page_info_ptr)) = (*page_info_ptr);

        MSG_GERAN_HIGH_0_G("CS page saved");
      }
    }
    else
    if (page_info_ptr->domain == RR_PAGE_DOMAIN_PS)
    {
      /**
       * Check if there is any storage allocated for the page, and if not, allocate it. 
       * Normally this would not be already allocated, but if multiple pages are received before they are 
       * forwarded to NAS, a page would be stored here already, which can be over-written) 
       */
      if (rr_paging_data_ptr->saved_ps_page_info_ptr == NULL)
      {
        rr_paging_data_ptr->saved_ps_page_info_ptr = GPRS_MEM_CALLOC(sizeof(rr_page_info_t));
      }

      if (rr_paging_data_ptr->saved_ps_page_info_ptr != NULL)
      {
        (*(rr_paging_data_ptr->saved_ps_page_info_ptr)) = (*page_info_ptr);

        MSG_GERAN_HIGH_0_G("PS page saved");
      }
    }
  }
}

// Discards any saved pages
void rr_ce_discard_saved_pages(const gas_id_t gas_id)
{
  // Obtain the pointer to the module data
  rr_paging_data_t *rr_paging_data_ptr = rr_ce_paging_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_paging_data_ptr);

  if (rr_paging_data_ptr->saved_cs_page_info_ptr != NULL)
  {
    GPRS_MEM_FREE(rr_paging_data_ptr->saved_cs_page_info_ptr);
    rr_paging_data_ptr->saved_cs_page_info_ptr = NULL;
  }

  if (rr_paging_data_ptr->saved_ps_page_info_ptr != NULL)
  {
    GPRS_MEM_FREE(rr_paging_data_ptr->saved_ps_page_info_ptr);
    rr_paging_data_ptr->saved_ps_page_info_ptr = NULL;
  }
}

// Sends any saved pages to NAS, in the order CS page then PS page
void rr_ce_send_saved_pages_to_nas(const gas_id_t gas_id)
{
  // Obtain the pointer to the module data
  rr_paging_data_t *rr_paging_data_ptr = rr_ce_paging_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_paging_data_ptr);

  if (rr_paging_data_ptr->saved_cs_page_info_ptr != NULL)
  {
    rr_ce_send_page_to_nas(
      rr_paging_data_ptr->saved_cs_page_info_ptr,   // page_info_ptr
      gas_id                                        // gas_id
    );

    // Discard the saved page
    GPRS_MEM_FREE(rr_paging_data_ptr->saved_cs_page_info_ptr);
    rr_paging_data_ptr->saved_cs_page_info_ptr = NULL;
  }

  if (rr_paging_data_ptr->saved_ps_page_info_ptr != NULL)
  {
    rr_ce_send_page_to_nas(rr_paging_data_ptr->saved_ps_page_info_ptr, gas_id);

    // Discard the saved page
    GPRS_MEM_FREE(rr_paging_data_ptr->saved_ps_page_info_ptr);
    rr_paging_data_ptr->saved_ps_page_info_ptr = NULL;
  }
}

// Sends a RR_PAGING_IND message to NAS
void rr_ce_send_page_to_nas(rr_page_info_t *page_info_ptr, const gas_id_t gas_id)
{
  RR_NULL_CHECK_RETURN_VOID(page_info_ptr);

  if (page_info_ptr->domain == RR_PAGE_DOMAIN_CS)
  {
    // Store the page details in preparation for a CS page response
    rr_ce_store_cs_page_info(
      page_info_ptr->identity,        // identity
      page_info_ptr->channel_needed,  // channel_needed
      gas_id                          // gas_id
    );
  }

  rr_send_paging_ind(
    page_info_ptr->domain,          // domain
    page_info_ptr->identity_type,   // identity_type
    gas_id                          // gas_id
  );
}

// Checks if the broadcast Page Mode has been changed. RR_IMSG_PAGE_MODE_IND is output if so
void rr_ce_check_for_page_mode_update(
  boolean         own_paging_block,
  byte            ota_page_mode,
  const gas_id_t  gas_id
)
{
  /* only look at paging mode value if OTA message was received in MS's own paging block */
  if (own_paging_block)
  {
    paging_mode_T page_mode;

    // Obtain the pointer to the module data
    rr_paging_data_t *rr_paging_data_ptr = rr_ce_paging_get_data_ptr(gas_id);

    RR_NULL_CHECK_FATAL(rr_paging_data_ptr);

    /* Mask off 'spare' bits - see GSM 04.18 10.5.2.26 */
    switch ( ota_page_mode & 0x03 )
    {
      case 0:   page_mode = NORMAL_PAGING;          break;
      case 1:   page_mode = EXTENDED_PAGING;        break;
      case 2:   page_mode = PAGING_REORGANIZATION;  break;
      default:  page_mode = SAME_AS_BEFORE;         break;
    }

    if ( (page_mode != SAME_AS_BEFORE ) &&
         (page_mode != rr_paging_data_ptr->ota_page_mode) )
    {
      if (page_mode == PAGING_REORGANIZATION)
      {
        MSG_GERAN_HIGH_0_G("PageMode=PAGING_REORGANIZATION");
      }
      else if (page_mode == EXTENDED_PAGING)
      {
        MSG_GERAN_HIGH_0_G("PageMode=EXTENDED_PAGING");
      }
      else if (page_mode == NORMAL_PAGING)
      {
        MSG_GERAN_HIGH_0_G("PageMode=NORMAL_PAGING");
      }
#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
      if(rr_macinfo_get_mac_status(gas_id) != MAC_TRANSFER)
#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */      
      {
        // Send RR_IMSG_PAGE_MODE_IND into RR to be handled according to state
        // This means L1 may or may not be updated with the new page mode
        if (!((page_mode == PAGING_REORGANIZATION)&& (get_waiting_for_surround_response(gas_id))))
        {
          rr_send_imsg_page_mode_ind(
            rr_paging_data_ptr->ota_page_mode,    // old_page_mode
            page_mode,                            // new_page_mode
            gas_id                                // gas_id
          );
        }
      }
      /*
      Note:

      The local copy of the OTA page mode is NOT updated here. The processing now is as follows:
      RR_IMSG_PAGE_MODE_IND will be received in RR-CONTROL and handled according to state. There are
      two options:
      1. Ignore it. This means 'ota_page_mode' will not get updated to match the broadcast value, and
         therefore next time a OTA message containin the Page Mode is read and compared, and will be
         seen to have changed, generating another RR_IMSG_PAGE_MODE_IND
      2. Update the L1 Page Mode to match the OTA Page Mode. This is done by calling
         rr_process_page_mode_change(), which will also update 'ota_page_mode'
      */
    }
  }
} /* rr_check_for_page_mode_update */

// Returns the local copy of the OTA Page Mode
paging_mode_T rr_ce_get_ota_page_mode(const gas_id_t gas_id)
{
  // Obtain the pointer to the module data
  rr_paging_data_t *rr_paging_data_ptr = rr_ce_paging_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_paging_data_ptr);

  return rr_paging_data_ptr->ota_page_mode;
}

// Update the local copy of the OTA page mode
void rr_ce_set_ota_page_mode(paging_mode_T new_page_mode, const gas_id_t gas_id)
{
  // Obtain the pointer to the module data
  rr_paging_data_t *rr_paging_data_ptr = rr_ce_paging_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_paging_data_ptr);

  rr_paging_data_ptr->ota_page_mode = new_page_mode;
}

// Compares OTA Page Mode with 'compare_page_mode' and if equal, changes the Page Mode to 'new_page_mode'
boolean rr_ce_compare_and_update_l1_page_mode(
  paging_mode_T compare_page_mode,
  paging_mode_T new_page_mode,
  const gas_id_t gas_id
)
{
  // Obtain the pointer to the module data
  rr_paging_data_t *rr_paging_data_ptr = rr_ce_paging_get_data_ptr(gas_id);

  if (rr_paging_data_ptr->ota_page_mode == compare_page_mode)
  {
    // Obtain the pointer to the module data
    rr_paging_data_t *rr_paging_data_ptr = rr_ce_paging_get_data_ptr(gas_id);

    RR_NULL_CHECK_FATAL(rr_paging_data_ptr);

    // Update L1 with the page mode
    rr_send_mph_change_page_mode_req(
      new_page_mode,    // paging_mode
      gas_id            // gas_id
    );

    // Update the local copy of the OTA page mode. This is used to compare with the next Page Mode received
    // from an OTA message
    rr_paging_data_ptr->ota_page_mode = new_page_mode;

    return TRUE;    // Page Mode was updated
  }

  return FALSE;   // Page Mode was not updated
}

// Update the L1 Page Mode to 'new page mode'
void rr_ce_process_page_mode_change(
  paging_mode_T old_page_mode,
  paging_mode_T new_page_mode,
  const gas_id_t gas_id
)
{
  // Update L1 with the page mode
  rr_ce_update_l1_page_mode(
    new_page_mode,    // page_mode
    gas_id            // gas_id
  );

  // Update sys info handler with the page mode
  // It requires 'old' and 'new' page modes so it can determine whether it is entering/exiting Paging Reorg
  // without storing local copies of the page mode
  rr_gprs_sys_info_new_paging_mode(
    old_page_mode,    // old_page_mode
    new_page_mode,    // new_page_mode
    gas_id            // gas_id
  );
}

// Initialisation function
void rr_ce_paging_init(
  const gas_id_t gas_id
)
{
  // Obtain the pointer to the module data
  rr_paging_data_t *rr_paging_data_ptr = rr_ce_paging_get_data_ptr(gas_id);

  rr_paging_data_ptr->ota_page_mode = NORMAL_PAGING;
  rr_ce_discard_saved_pages(gas_id);
}

// Initialisation function. Should be called once at RR task start
void rr_ce_paging_task_start_init(
  void
)
{
  // Zero the control/data structure(s)
  memset(rr_paging_data, 0, sizeof(rr_paging_data));

  // Setup any specific values not setup by the general module initialisation function
}

/* EOF */