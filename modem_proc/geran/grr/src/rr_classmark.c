/*============================================================================
 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_classmark.c#1 $
 $DateTime: 2016/12/13 08:00:21 $
 $Author: mplcsds1 $

 FILE:         rr_classmark.c

 OVERVIEW:     RR classmark utility module

 DEPENDENCIES: None

               Copyright (c) 2008-2014 QUALCOMM Technologies, Inc
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_classmark.h"
#include "rr_l2_send.h"
#include "rr_l2.h"
#include "rr_defs.h"
#include "rr_decode_ie.h"
#include "msg.h"
#include "libprot.h"
#include "aconfig.h"
#include "rr_gprs_defs.h"
#include "rr_general.h"
#ifdef FEATURE_WCDMA
#include "rrcrrif.h"
#endif /* FEATURE_WCDMA */
#include "sys_stru.h"
#include "rr_multi_sim.h"
#include "rr_conn_establish.h"
#include "rr_mm_send.h"
#include "rr_msgr.h"
#include "rr_nv.h"
#include "stringl/stringl.h"

/*----------------------------------------------------------------------------
 * Definitions and Constants
 *--------------------------------------------------------------------------*/

#define RR_CLASSMARK_GSM_SUPPORTED     TRUE

#define RR_CLASSMARK_CHANGE_MIN_LENGTH 6 /* See 3GPP 44.018 section 9.1.11 */

#define RR_CLASSMARK3_MAX_VALUE_LENGTH (N201_DCCH-(RR_CLASSMARK_CHANGE_MIN_LENGTH+2))
#define RR_CLASSMARK3_IEI              0x20

#define RR_CLASSMARK_DEFAULT_ENQUIRY_MASK 0x7F /* send Classmark Change only */

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

// Currently this module only has data storage requirements when FEATURE_WCDMA is defined
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

typedef struct
{
  boolean irat_ho_info_compressed;
} rr_classmark_data_t;

#endif /*  FEATURE_WCDMA || FEATURE_GSM_TDS*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

// Currently this module only has data storage requirements when FEATURE_WCDMA is defined
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

static rr_classmark_data_t rr_classmark_data[NUM_GERAN_DATA_SPACES];

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

// Currently this module only has data storage requirements when FEATURE_WCDMA is defined
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
/*!
 * \brief Return a pointer to the module data
 *
 * \param gas_id
 *
 * \return rr_classmark_data_t*
 */
rr_classmark_data_t *rr_classmark_get_data_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_classmark_data[as_index];
}
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

/*===========================================================================

FUNCTION  rr_encode_classmark

DESCRIPTION   Encodes Classmark 2 and/or Classmark 3 into output structures

DEPENDENCIES

RETURN VALUE  TRUE if the requested classmark was valid

SIDE EFFECTS

===========================================================================*/
boolean rr_encode_classmark(
  rr_classmark_encode_t cm_type,
  classmark_2_ie_T *cm2_ptr,
  classmark_3_ie_T *cm3_ptr,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr;
  boolean            wcdma_support;
  boolean            rel99_support;
  boolean            cm_valid = FALSE;
  classmark_params_T cm_params;

  /* validate global data pointers */
  scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  if (scell_info_ptr == NULL)
  {
    return(FALSE);
  }

  /* determine whether WCDMA support is currently enabled on the UE */
#ifdef FEATURE_WCDMA
#ifdef FEATURE_SGLTE
  if (rr_is_sglte_mode_active(gas_id))
  {
    wcdma_support = FALSE;
  }
  else
#endif /* FEATURE_SGLTE */
  {
    wcdma_support = rr_is_wcdma_enabled(gas_id);
  }
#else
  wcdma_support = FALSE;
#endif /* FEATURE_WCDMA */

  /* determine whether MCSR indicates Release 99 or later */
  if (scell_info_ptr->gsm.control_channel_description.MSCR == 1)
  {
    rel99_support = TRUE;
  }
  else
  {
    rel99_support = FALSE;
  }

  /* set up classmark encoding parameters for the active subscription */
  cm_params.gsm_supported   = RR_CLASSMARK_GSM_SUPPORTED;
  cm_params.MSCR99          = rel99_support;
  cm_params.wcdma_supported = wcdma_support;
  cm_params.wcdma_in_use    = FALSE;
  cm_params.as_id           = geran_map_gas_id_to_nas_id(gas_id);
  cm_params.rat             = SYS_RAT_GSM_RADIO_ACCESS;

  /* if Classmark 2 was requested, encode it now */
  if ((cm_type == RR_CLASSMARK_2_ONLY) ||
      (cm_type == RR_CLASSMARK_2_AND_3))
  {
    if (cm2_ptr != NULL)
    {
      gs_read_coded_classmark_2_with_rat(cm2_ptr, &cm_params);
      cm_valid = TRUE;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Unable to output Classmark 2 to NULL ptr");
    }
  }

  /* if Classmark 3 was requested, encode it now */
  if ((cm_type == RR_CLASSMARK_3_ONLY) ||
      (cm_type == RR_CLASSMARK_2_AND_3))
  {
    if (cm3_ptr != NULL)
    {
      cm_valid = gs_read_coded_classmark_3_with_rat(cm3_ptr, &cm_params);

      /* set the classmark 3 valid flag in the classmark 2 data */
      if ((cm_type == RR_CLASSMARK_2_AND_3) && (cm2_ptr != NULL))
      {
        if (cm_valid == TRUE)
        {
          cm2_ptr->data[2] |= 0x80;
        }
        else
        {
          cm2_ptr->data[2] &= 0x7F;
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Unable to output Classmark 3 to NULL ptr");
    }
  }

  return(cm_valid);
}

/*===========================================================================

FUNCTION  rr_send_geran_classmark_change

DESCRIPTION   Encode a CLASSMARK CHANGE message and send it to the network

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void rr_send_geran_classmark_change(
  const gas_id_t gas_id
)
{
  classmark_2_ie_T   classmark_2;
  classmark_3_ie_T   classmark_3;
  boolean            classmark_3_valid;
  byte               index = 0;
  byte               c_index;
  byte               message_length;
  byte               layer3_message[N201_DCCH];

  RR_ZERO_STRUCTURE(classmark_2);
  RR_ZERO_STRUCTURE(classmark_3);

  /* read Classmark 2 and Classmark 3 (optional) */
  classmark_3_valid = rr_encode_classmark(
    RR_CLASSMARK_2_AND_3,
    &classmark_2,
    &classmark_3,
    gas_id
  );

  layer3_message[index] = RADIO_RESSOURCES;
  index++;

  layer3_message[index] = CLASSMARK_CHANGE;
  index++;

  layer3_message[index] = classmark_2.length;
  index++;

  for (c_index = 0;
       (c_index < classmark_2.length) && (index < N201_DCCH) && (c_index < 3); /* 3 is hard-coded data length in sys_stru.h */
       c_index++)
  {
    layer3_message[index] = classmark_2.data[c_index];
    index++;
  }

  /* if classmark 3 information should be included */
  if (classmark_3_valid == TRUE)
  {
    /********************************************/
    /* The classmark 3 information is available */
    /********************************************/

    /* IEI for Classmark 3 (optional IE) */
    if (index < N201_DCCH)
    {
      layer3_message[index] = RR_CLASSMARK3_IEI;
      index++;
    }

    if (classmark_3.length > RR_CLASSMARK3_MAX_VALUE_LENGTH)
    {
      /* the value part of classmark 3 should never exceed 12 octets */
      /* since there are only 14 octets available in total, and the  */
      /* message uses the TLV type 4 format, which requires 2 octets */
      /* for the IEI and length information. Trim the length here.   */
      MSG_GERAN_ERROR_1_G("WARNING: Classmark 3 exceeds expected max length(%d)",
                (int)classmark_3.length);
      classmark_3.length = RR_CLASSMARK3_MAX_VALUE_LENGTH;
    }

    /* length of Classmark 3 value part (i.e. not including the length itself) */
    if (index < N201_DCCH)
    {
      layer3_message[index] = classmark_3.length;
      index++;
    }

    for (c_index = 0;
         (c_index < classmark_3.length) && (index < N201_DCCH);
         c_index++)
    {
      layer3_message[index] = classmark_3.data[c_index];
      index++;
    }

    /* message length: 6 for header/classmark2, 2 for classmark3 IEI + length, + classmark 3 contents */
    message_length = RR_CLASSMARK_CHANGE_MIN_LENGTH + 2 + classmark_3.length;
  }
  else /* classmark 3 is not included */
  {
    /* header + classmark 2 */
    message_length = RR_CLASSMARK_CHANGE_MIN_LENGTH;
  }

  MSG_GERAN_HIGH_1_G("OMsg(NW,CLASSMARK_CHANGE:CM3=%1d)",(int)classmark_3_valid);

  (void) rr_send_dl_data_req(
    layer3_message,
    message_length,
    SAPI0,
    DCCH,
    gas_id
  );
} /* end rr_send_geran_classmark_change() */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

/*===========================================================================

 FUNCTION  rr_send_utran_classmark_change

 DESCRIPTION Sends a UTRAN classmark change message to network if dual mode
             enabled.

 DEPENDENCIES  None.

 RETURN VALUE  None.

 SIDE EFFECTS  None.

 ===========================================================================*/
static void rr_send_utran_classmark_change(
  const gas_id_t gas_id
)
{
  byte index = 0;
  byte c_index;
  byte message_length = 0;
  byte layer3_message[MAX_OCTETS_L3_MESSAGE];
  uint8 handover_info_len = 0;
  byte *handover_info_ptr = NULL;
  rr_classmark_data_t *rr_classmark_data_ptr;
#if defined (FEATURE_WCDMA) && defined (FEATURE_DUAL_SIM)
  sys_modem_as_id_e_type as_id = geran_map_gas_id_to_nas_id(gas_id);
#endif /* FEATURE_WCDMA && FEATURE_DUAL_SIM*/
#ifdef FEATURE_GSM_TDS
  boolean send_utran_tdd_classmark = FALSE;

  if (rr_is_utran_tdd_enabled(gas_id) &&
      rr_is_wcdma_enabled(gas_id) )
  {
    send_utran_tdd_classmark = rr_is_utran_tdd_enabled_in_nw_plmn(gas_id);
    MSG_GERAN_MED_1_G("RR W+T enabled use PLMN decision, Send T classmark = %d",send_utran_tdd_classmark);
  }
  else if (rr_is_utran_tdd_enabled(gas_id))
  {
    send_utran_tdd_classmark = TRUE;
     MSG_GERAN_MED_0_G("RR T only enabled send T classmark");
  }
  else if (rr_is_wcdma_enabled(gas_id))
  {
    send_utran_tdd_classmark = FALSE;
    MSG_GERAN_MED_0_G("RR W only enabled send W classmark");
  }
  else
  {
    send_utran_tdd_classmark = FALSE;
    MSG_GERAN_ERROR_0_G("RR no T or W support, default to sending W classmark");
  }
#endif /*FEATURE_GSM_TDS*/

  // Obtain a pointer to the module data
  rr_classmark_data_ptr = rr_classmark_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_classmark_data_ptr);

  /***************************************************************/
  /* This module has to encode a UTRAN_CLASSMARK_CHANGE message  */
  /* and send it to the network                                  */
  /***************************************************************/

  /************************************************************/
  /* Encode the Protocol Discriminator and the Skip Indicator */
  /************************************************************/

  layer3_message[index] = RADIO_RESSOURCES;
  index++;

  /***************************/
  /* Encode the message type */
  /***************************/

  layer3_message[index] = UTRAN_CLASSMARK_CHANGE;
  index++;

  /*************************************************************/
  /* Encode the UTRAN configuration status Information Element */
  /*************************************************************/

#ifdef FEATURE_GSM_TDS
  if (send_utran_tdd_classmark)
  {
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
    tdsrrc_get_classmark_change_handover_info_sub_id(
      &handover_info_ptr,
      &handover_info_len,
      rr_classmark_data_ptr->irat_ho_info_compressed,
      as_id
    );
#else
    tdsrrc_get_classmark_change_handover_info(
      &handover_info_ptr,
      &handover_info_len,
      rr_classmark_data_ptr->irat_ho_info_compressed
    );
#endif /* FEATURE_TDS_SUB_ID_INTERFACE */
  }
  else
#endif /*FEATURE_GSM_TDS*/
  {
#ifdef FEATURE_WCDMA
#ifdef FEATURE_DUAL_SIM
    rrc_get_classmark_change_handover_info_sub_id(
      &handover_info_ptr,
      &handover_info_len,
      rr_classmark_data_ptr->irat_ho_info_compressed,
      as_id
    );
#else
    rrc_get_classmark_change_handover_info(
      &handover_info_ptr,
      &handover_info_len,
      rr_classmark_data_ptr->irat_ho_info_compressed
    );
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_WCDMA */
  }

  /* sanity check the returned data from WCDMA access stratum */
  if ((handover_info_len == 0) ||
      (handover_info_ptr == NULL))
  {
    MSG_GERAN_ERROR_1_G("RRC indicates no UTRAN classmark available (len=%d)", (int)handover_info_len);
    return;
  }

  layer3_message[index] = handover_info_len;
  index++;

  /* check the remaining space in the L3 message */
  if (handover_info_len > (MAX_OCTETS_L3_MESSAGE-index))
  {
    /* unable to encode this much data - exit now */
    MSG_GERAN_ERROR_0_G("UTRAN Classmark Change too long - aborted");
	
#ifdef FEATURE_GSM_TDS
    if (send_utran_tdd_classmark)
    {
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
      tdsrrc_delete_classmark_change_handover_info_sub_id(as_id);
#else
      tdsrrc_delete_classmark_change_handover_info();
#endif /* FEATURE_TDS_SUB_ID_INTERFACE */
    }
    else
#endif /*FEATURE_GSM_TDS*/
    {
#ifdef FEATURE_WCDMA
#ifdef FEATURE_DUAL_SIM
      rrc_delete_classmark_change_handover_info_sub_id(as_id);
#else
      rrc_delete_classmark_change_handover_info();
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_WCDMA */
    }
    return;
  }

  for (c_index = 0; c_index < handover_info_len; c_index++)
  {
    if(handover_info_ptr != NULL)
    {
      layer3_message[index] = *(handover_info_ptr + c_index);
      index++;
    }
  }

  /* RRC allocates the buffer. RR calls this function to free it */

#ifdef FEATURE_GSM_TDS
  if (send_utran_tdd_classmark)
  {
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
    tdsrrc_delete_classmark_change_handover_info_sub_id(as_id);
#else
    tdsrrc_delete_classmark_change_handover_info();
#endif /* FEATURE_TDS_SUB_ID_INTERFACE */
  }
  else
#endif /*FEATURE_GSM_TDS*/
  {
#ifdef FEATURE_WCDMA
#ifdef FEATURE_DUAL_SIM
    rrc_delete_classmark_change_handover_info_sub_id(as_id);
#else
    rrc_delete_classmark_change_handover_info();
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_WCDMA */
  }

  MSG_GERAN_HIGH_0_G("OMsg(NW,UTRAN_CLASSMARK_CHANGE)");

  /* message length: PD & Skip + type + LI + handover_info */
  message_length = 3 + handover_info_len;

  (void) rr_send_dl_data_req(
    layer3_message,
    message_length,
    SAPI0,
    DCCH,
    gas_id
  );

} /* end rr_send_utran_classmark_change() */

void rr_classmark_set_irat_ho_info_compressed(
  boolean compressed,
  const gas_id_t gas_id
)
{
  rr_classmark_data_t *rr_classmark_data_ptr;

  // Obtain a pointer to the module data
  rr_classmark_data_ptr = rr_classmark_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_classmark_data_ptr);

  MSG_GERAN_HIGH_1_G("irat_ho_info_compressed = %d", (int)compressed);
  rr_classmark_data_ptr->irat_ho_info_compressed = compressed;
}

boolean rr_classmark_get_irat_ho_info_compressed(const gas_id_t gas_id)
{
  rr_classmark_data_t *rr_classmark_data_ptr;

  // Obtain a pointer to the module data
  rr_classmark_data_ptr = rr_classmark_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_classmark_data_ptr);

  return rr_classmark_data_ptr->irat_ho_info_compressed;
}

#endif /*  FEATURE_WCDMA || FEATURE_GSM_TDS*/


/*===========================================================================

 FUNCTION  rr_decode_classmark_enquiry

 DESCRIPTION Decodes the OTA message CLASSMARK ENQUIRY

 DEPENDENCIES  None.

 RETURN VALUE  None.

 SIDE EFFECTS  None.

 ===========================================================================*/
static void rr_decode_classmark_enquiry(
  boolean *geran_ptr,
  boolean *utran_ptr,
  boolean *cdma2k_ptr,
  const gas_id_t gas_id
)
{
  ie_descriptor_T *current_IE;

  /* sanity check the parameters */
  if ((geran_ptr  != NULL) &&
      (utran_ptr  != NULL) &&
      (cdma2k_ptr != NULL))
  {
    *geran_ptr  = FALSE;
    *utran_ptr  = FALSE;
    *cdma2k_ptr = FALSE;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Invalid parameters to classmark enquiry decoder");
    return;
  }

  /****************************************************/
  /* get a pointer to the first IE of the OTA message */
  /*****************************************************/
  current_IE = rr_get_first_ie_of_message(CLASSMARK_ENQUIRY, gas_id);

  if (current_IE != NULL)
  {
    byte cm_enquiry_mask;

    /* extract the Classmark Enquiry Mask - see 3GPP 44.018 Table 10.5.2.7c.2 */
    if (current_IE->start_IE != NULL)
    {
      cm_enquiry_mask = current_IE->start_IE[1];
    }
    else
    {
      /* 3GPP 44.018 section 3.4.11.2 states that if no mask is provided */
      /* then this indicates a request for CLASSMARK CHANGE message only */
      /* Note that this mask used inverted logic (0=on, 1=off)           */
      cm_enquiry_mask = RR_CLASSMARK_DEFAULT_ENQUIRY_MASK;
    }

    MSG_GERAN_HIGH_1_G("Using classmark enquiry mask 0x%02X", (int)cm_enquiry_mask);

    /* determine whether CLASSMARK CHANGE message is requested */
    if ((cm_enquiry_mask & 0x80) != 0x80)
    {
      *geran_ptr = TRUE;
    }

    /* determine whether UTRAN CLASSMARK CHANGE message is requested */
    if ((cm_enquiry_mask & 0x70) != 0x70)
    {
      *utran_ptr = TRUE;
    }

    /* determine whether CDMA2000 CLASSMARK CHANGE message is requested */
    if ((cm_enquiry_mask & 0x08) != 0x08)
    {
      *cdma2k_ptr = TRUE;
    }
  }

  return;
} /* end rr_decode_classmark_enquiry() */

/*===========================================================================

 FUNCTION  rr_perform_classmark_sending

 DESCRIPTION Sends classmark messages required for particular clients

 DEPENDENCIES  None.

 RETURN VALUE  None.

 SIDE EFFECTS  None.

 ===========================================================================*/
void rr_perform_classmark_sending(
  rr_classmark_client_t enquiry_client,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  boolean             send_geran = FALSE;
  boolean             send_utran = FALSE;
  boolean             send_cdma2k = FALSE;

  switch(enquiry_client)
  {
  case RR_CLASSMARK_CLIENT_ECSC:
    if (ES_IND == CECS_IMPLEMENTED)
    {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      /* under Multi-Standby conditions, optimise this handler to avoid */
      /* Early Classmark sending if this connection is for IMSI Detach, */
      /* which could save some time during a power-off detach procedure */
      if (geran_in_multi_standby_mode())
      {
        if (rr_ce_access_reason(gas_id) == (byte)MM_RR_IMSI_DETACH)
        {
          MSG_GERAN_HIGH_0_G("Avoiding ECSC during IMSI Detach in multi-standby mode");
          return;
        }
      }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

      /*************************************************************/
      /* System Information parameter indicates if early classmark */
      /* sending is supported by the network. Only send classmark  */
      /* information if ECSC is supported                          */
      /*************************************************************/
      if (scell_info_ptr->gsm.ecsc == TRUE)
      {
        MSG_GERAN_HIGH_0_G("Perform Early Classmark Sending");

        /* GERAN classmark is always sent */
        send_geran = TRUE;

        /* UTRAN classmark can also be sent if there is no ECS Restriction */
        send_utran = scell_info_ptr->gsm.ecsr;
      }
    }
    break;

  case RR_CLASSMARK_CLIENT_NETW:
    /* Handle the OTA Classmark Enquiry */
    rr_decode_classmark_enquiry(&send_geran, &send_utran, &send_cdma2k, gas_id);
    break;

  case RR_CLASSMARK_CLIENT_GHDI:
    /* Handle the request from the GHDI module */
    send_geran = TRUE;
    send_utran = TRUE;
    break;

  default:
    MSG_GERAN_ERROR_1_G("Unknown classmark enquiry client %d", (int)enquiry_client);
    break;
  }

  /* to complete the client enquiry, send each classmark that was requested */
  if (send_geran == TRUE)
  {
    rr_send_geran_classmark_change(gas_id);
  }

  /* UTRAN Classmark is sent if WCDMA support is currently enabled. */
  if ((send_utran == TRUE) && ( rr_is_wcdma_enabled(gas_id) || rr_is_utran_tdd_enabled(gas_id) ))
  {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
    rr_send_utran_classmark_change(gas_id);
#else
    MSG_GERAN_LOW_0_G("UTRAN Classmark Change will not be sent");
#endif /*  FEATURE_WCDMA || FEATURE_GSM_TDS*/
  }

  if (send_cdma2k == TRUE)
  {
    /* this classmark is not supported yet */
    MSG_GERAN_ERROR_0_G("CDMA2000 Classmark Change will not be sent");
  }

  return;
} /* end rr_perform_classmark_sending() */

#ifdef FEATURE_LTE
void rr_handle_lte_cs_capabilities_req(
  geran_grr_lte_cs_capabilities_req_t *req_ptr,
  const gas_id_t gas_id
)
{
  geran_grr_lte_cs_capabilities_rsp_t rsp;
  classmark_params_T                  cm_params;
  errno_enum_type                     result;
  classmark_2_ie_T                    classmark_2;
  classmark_3_ie_T                    classmark_3;
  boolean                             classmark_3_valid;

  memset(&rsp, 0, sizeof(geran_grr_lte_cs_capabilities_rsp_t));

  msgr_init_hdr(
    &rsp.hdr,
    MSGR_GERAN_GRR,
    GERAN_GRR_LTE_CS_CAPABILITIES_RSP
  );

  /* set up common classmark parameters for the active subscription */
  cm_params.gsm_supported   = req_ptr->rat_capabilities.geran_is_supported;  
  cm_params.wcdma_supported = req_ptr->rat_capabilities.wcdma_is_supported;
  cm_params.MSCR99          = TRUE; /* assume R99 as LTE is Rel 8 */
  cm_params.as_id           = geran_map_gas_id_to_nas_id(gas_id);
  cm_params.rat             = SYS_RAT_GSM_RADIO_ACCESS;
  cm_params.wcdma_in_use    = TRUE;

  gs_read_coded_classmark_2_with_rat(&classmark_2, &cm_params);

  /* always read classmark 3 - function returns TRUE if any options are set */
  classmark_3_valid = gs_read_coded_classmark_3_with_rat(&classmark_3, &cm_params);

  /* set the classmark 3 valid flag in the classmark 2 data */
  if (classmark_3_valid == TRUE)
  {
    classmark_2.data[2] |= 0x80;
  }
  else
  {
    classmark_2.data[2] &= 0x7F;
  }

  rsp.capabilities_container[0] = 0x33; /* Mobile station classmark 2 IEI */
  rsp.capabilities_container[1] = 3;    /* Length of the classmark 2 is always set to 3. */
  rsp.capabilities_container[2] = classmark_2.data[0];
  rsp.capabilities_container[3] = classmark_2.data[1];
  rsp.capabilities_container[4] = classmark_2.data[2];

  //Using memscpy rather memcpy 
  memscpy(
         &rsp.capabilities_container[5],
         sizeof(rsp.capabilities_container)-5,
         classmark_3.data,
         MIN(GERAN_CS_MAX_CAP_CONTAINER_LENGTH - 5, classmark_3.length)
         );

  rsp.capabilities_container_length = 5 + classmark_3.length;
  if (rsp.capabilities_container_length > GERAN_CS_MAX_CAP_CONTAINER_LENGTH)
  {
    MSG_GERAN_ERROR_2_G("CS Capabilities overflow, truncating length %d to %d",
      rsp.capabilities_container_length,
      GERAN_CS_MAX_CAP_CONTAINER_LENGTH);
    rsp.capabilities_container_length = GERAN_CS_MAX_CAP_CONTAINER_LENGTH;
  }
  else
  {
    MSG_GERAN_MED_1_G("CS Capabilities length: %d", rsp.capabilities_container_length);
  }

  result = rr_msgr_send(&rsp.hdr, sizeof(rsp), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }

  return;
}

void rr_handle_lte_ps_capabilities_req(
  geran_grr_lte_ps_capabilities_req_t *req_ptr,
  const gas_id_t gas_id
)
{
  if (req_ptr != NULL)
  {
    /* LTE (multimode subscription) is asking for GPRS Radio Access Capabilities
     * this information is requested from NAS asynchronously, and when the
     * response comes back from GMM, GRR forwards the data to LTE immediately
     */
    rr_send_gmm_gprs_ms_ra_cap_req(gas_id);
  }

  return;
}

static void rr_convert_nas_ra_cap_to_lte_format(rr_gmm_gprs_ms_ra_cap_ie_ind_T *ind_ptr, gmm_ie_LV_ms_radio_access_caps_T *cap_ptr)
{
  if ((ind_ptr != NULL) && (cap_ptr != NULL))
  {
    word length;

    /* initialise the output buffer */
    memset(cap_ptr, 0, sizeof(gmm_ie_LV_ms_radio_access_caps_T));

    /* calculate length in bytes (NAS format uses bits) */
    length = (ind_ptr->ms_ra_cap_ie_len_bits >> 3);
    if ((ind_ptr->ms_ra_cap_ie_len_bits % 8) != 0)
    {
      length++;
    }

    /* copy the data over if there is enough space */
    if ((length <= sizeof(cap_ptr->value)) &&
        (ind_ptr->ms_ra_cap_ie != NULL))
    {
      cap_ptr->length = length;
      memscpy(cap_ptr->value, sizeof(cap_ptr->value),
              ind_ptr->ms_ra_cap_ie, length);
    }
    else
    {
      MSG_ERROR("MS Radio Access capabilities %d too long for LTE response", (int)length, 0, 0);
    }
  }
  
  return;
}

void rr_send_lte_ps_capabilities_rsp(rr_gmm_gprs_ms_ra_cap_ie_ind_T *ind_ptr, const gas_id_t gas_id)
{
  geran_grr_lte_ps_capabilities_rsp_t rsp;
  gmm_ie_LV_ms_radio_access_caps_T    ms_radio_access_caps;
  errno_enum_type                     result;

  RR_ZERO_STRUCTURE(rsp);

  msgr_init_hdr(
    &rsp.hdr,
    MSGR_GERAN_GRR,
    GERAN_GRR_LTE_PS_CAPABILITIES_RSP
  );

  RR_ZERO_STRUCTURE(ms_radio_access_caps);

  /* convert the data received in the NAS indication to the format LTE requires */
  rr_convert_nas_ra_cap_to_lte_format(ind_ptr, &ms_radio_access_caps);  

  MSG_GERAN_MED_1_G("PS Capabilities length: %d", ms_radio_access_caps.length);

  rsp.capabilities_container_length = MIN(ms_radio_access_caps.length, GERAN_PS_MAX_CAP_CONTAINER_LENGTH);
  
  memscpy(rsp.capabilities_container,
          sizeof(rsp.capabilities_container),
          ms_radio_access_caps.value,
          rsp.capabilities_container_length);

#ifdef FEATURE_GSM_DTM
  rsp.geran_dtm_is_supported  = rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID);
  MSG_GERAN_MED_1_G("DTM supported: %d", (int)(rsp.geran_dtm_is_supported));
#else
  rsp.geran_dtm_is_supported  = FALSE;
#endif /* FEATURE_GSM_DTM */	
	
  result = rr_msgr_send(&rsp.hdr, sizeof(rsp), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }

  return;
}

#endif /* FEATURE_LTE */

/* EOF */

