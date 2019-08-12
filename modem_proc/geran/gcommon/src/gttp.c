/*==============================================================================

                     G T T P      W R A P P E R     F U N C T I O N S

GENERAL DESCRIPTION

      This module contains the GTTP wrapper functions

EXTERNALIZED FUNCTIONS

      gttp_send_llc_pdu_to_rr()
      gttp_send_dl_pdu_to_llc()

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005-2013 Qualcomm Technologies, Inc.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/src/gttp.c#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
08/18/05   hv     Created

==============================================================================*/

/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef  FEATURE_GSM_DTM

#include "gprsdef.h"
#include "gprsdef_g.h"
#include "gttp.h"
#include "gprs_pdu.h"
#include "gllc.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "rr_public_store.h"
#include "rlc_grr.h"
#include "sys_stru.h"
#include "gprs_mem.h"
#include "gs.h"
#include "err.h"
#include "msg.h"
#include "ms.h"
#include "geran_dual_sim.h"

/*==============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/

/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*==========================================================================

  FUNCTION    gttp_send_llc_pdu_to_rr

  DESCRIPTION

    This function is intended to be used by GRLC to send an LLC PDU to GRR
    using the GTTP primitive RG_UL_DCCH_REQ.

    The LLC PDU in dsm format is stripped into a local buffer of the type
    gttp_llc_pdu_container_t (defined in section 10.5.8.1 of 3GPP TS 44.018)


  PARAMETERS

    dsm_ptr:  pointer to dsm item representing an LLC PDU.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void gttp_send_llc_pdu_to_rr
(
  uint32          tlli,
  dsm_item_type   *dsm_ptr,
  const gas_id_t   gas_id
)
{
  /* Message union which contains RLC to GRR signal(s)
  */
  rlc_grr_sig_t   grr_sig;

  /*-----------------------------------------------------------------------*/

  /* Get length of LLC PDU in octets
  */
  grr_sig.rlc_grr_ul_dcch_req.gttp_llc_pdu_container.llc_pdu_len =
    (uint8)GPRS_PDU_LENGTH(dsm_ptr);

  /* Convert PDU in dsm format to linear buffer
  */
  if (!gprs_pdu_strip_head(
        &dsm_ptr,
        &(grr_sig.rlc_grr_ul_dcch_req.gttp_llc_pdu_container.llc_pdu[0]),
        (uint16)grr_sig.rlc_grr_ul_dcch_req.gttp_llc_pdu_container.llc_pdu_len
                          )
     )
  {
    MSG_GERAN_ERROR_0_G("GTTP Error stripping DSM item representing LLC PDU");
  }

  grr_sig.rlc_grr_ul_dcch_req.tlli  = tlli;

  /* Primitive to send LLC PDU from RLC to RR for delivery via L2
  */
  grr_sig.rlc_grr_ul_dcch_req.message_header.message_set = MS_RLC_RR;
  grr_sig.rlc_grr_ul_dcch_req.message_header.message_id  = (uint8)RG_UL_DCCH_REQ;
  grr_sig.rlc_grr_ul_dcch_req.gas_id                     = gas_id;

  /* Send message to RR
  */
  PUT_IMH_LEN( sizeof(rlc_grr_ul_dcch_req_t) - sizeof(IMH_T),
               &grr_sig.rlc_grr_ul_dcch_req.message_header
             );
  (void)gs_send_message( GS_QUEUE_RR, (void *)&grr_sig, TRUE );

  MSG_GERAN_MED_1_G("GTTP UL sent RG_UL_DCCH_REQ len=%u",
          grr_sig.rlc_grr_ul_dcch_req.gttp_llc_pdu_container.llc_pdu_len);

} /* gttp_send_llc_pdu_to_rr */


/*==========================================================================

  FUNCTION    gttp_send_dl_pdu_to_llc

  DESCRIPTION

    This function is intended to be used by GRR to send a downlink PDU obtained
    via GTTP to LLC. The GRR_DATA_IND primitive borrowed from the RLC/LLC
    interface is used to avoid adding another interface to LLC

  PARAMETERS

    llc_pdu_container_ptr:
        pointer to the LLC PDU Container Information Element
        (defined in section 10.5.8.1 of 3GPP TS 44.018)

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

  DISCUSSION

    Not possible to check for the length of the GTTP message as LLC_PDU length
    varies between 140 and 1530 octets whilst the length in the LLC PDU Container
    is only specified in 8-bit. So We shall only check for ZERO length message
    as we do not want to reject a valid message

===========================================================================*/
void gttp_send_dl_pdu_to_llc
(
  uint32                    tlli,
  gttp_llc_pdu_container_t  *llc_pdu_container_ptr,
  const gas_id_t            gas_id
)
{
  gprs_grr_dl_mes_t   *dl_msg_ptr;

  /*---------------------------------------------------------------------------
  ** Format the PDU into the existing RLC-LLC interface
  **---------------------------------------------------------------------------
  */

  /* Check parameters from RR
  */
  if (llc_pdu_container_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("GTTP container is NULL");
  }
  else if (llc_pdu_container_ptr->llc_pdu_len == 0)
  {
    MSG_GERAN_ERROR_1_G("GTTP Bad PDU len=%u", llc_pdu_container_ptr->llc_pdu_len);
  }
  else
  /* Valid GTTP message: container info OK and valid length
  */
  {
    /* Allocate and construct RLC-LLC-style DL message
    */
    dl_msg_ptr = GPRS_MEM_MALLOC(sizeof(gprs_grr_dl_mes_t));

    if ( dl_msg_ptr == NULL )
    {
      MSG_GERAN_ERROR_0_G("GTTP send_dl_pdu_to_llc() malloc failure");
    }
    else
    {
      /* Construct the LLC PDU in DSM format
      */
      dl_msg_ptr->ll_pdu = gprs_pdu_construct (llc_pdu_container_ptr->llc_pdu,
                                               llc_pdu_container_ptr->llc_pdu_len
                                              );
      if (dl_msg_ptr->ll_pdu == NULL)
      {
        MSG_GERAN_ERROR_0_G("GTTP Error converting PDU to DSM format");
        GPRS_MEM_FREE(dl_msg_ptr);
      }
      else
      {
        /* DSM construction OK
        ** Borrow the RLC/LLC primitive: GRR_DATA_IND
        */
        dl_msg_ptr->prim = GRR_DATA_IND;
        dl_msg_ptr->tlli = tlli;

        /* Add the GAS-ID
        */
        dl_msg_ptr->gas_id = gas_id;

        /* Send message to LLC
        */
        gllc_grr_dl_put_mes_buf(dl_msg_ptr);

        MSG_GERAN_MED_1_G("GTTP DL Sent PDU to LLC len=%u",
                llc_pdu_container_ptr->llc_pdu_len);
      } /* dsm OK */
    } /* malloc OK */
  } /* valid GTTP message */
} /* gttp_send_dl_pdu_to_llc */

#endif /* FEATURE_GSM_DTM */


/* EOF */

