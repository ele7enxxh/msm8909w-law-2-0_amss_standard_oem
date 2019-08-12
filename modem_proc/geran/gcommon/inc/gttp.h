/*==============================================================================

                     G T T P     W R A P P E R    H E A D E R

GENERAL DESCRIPTION

      GTTP wrapper function definition

EXTERNALIZED FUNCTIONS

      gttp_send_llc_pdu_to_rr()
      gttp_send_pdu_to_llc()

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005-2013 Qualcomm Technologies, Inc.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/gttp.h#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
08/18/05   hv     Created

==============================================================================*/

#ifndef INC_GTTP_H
#define INC_GTTP_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef  FEATURE_GSM_DTM

#include "gprsdef.h"
#include "gprsdef_g.h"
#include "dsm.h"

/*==============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

  
/*-----------------------------------------------------------------------------
** Maximum number of octets in an L3 message as defined in section 8.8.5 of
** 3GPP TS 44.006. Note this is applicable to ACK mode only which would be
** the mode for delivering of GMM signalling messages via GTTP
**-----------------------------------------------------------------------------
*/
#define L3_MSG_MAX_LEN  251

/*------------------------------------------------------------------------------
** GTTP LLC PDU Container Information Element as defined in section 10.5.8.1 of 
** 3GPP TS 44.018
**
** This struct is intended to be filled by GRLC and passed to RR to be delivered
** to L2
**------------------------------------------------------------------------------
*/
typedef struct
{
  uint8   llc_pdu_len;
  uint8   llc_pdu[L3_MSG_MAX_LEN];

} gttp_llc_pdu_container_t;


/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/

/*==============================================================================

                               PUBLIC FUNCTIONS

==============================================================================*/

/*==========================================================================

  FUNCTION    gttp_send_llc_pdu_to_rr

  DESCRIPTION

    This function is intended to be used by GRLC to send an LLC PDU to GRR 
    using the GTTP primitive RG_UL_DCCH_REQ.
    
    The LLC PDU in dsm format is stripped into a local buffer of the type 
    gttp_llc_pdu_container_t (defined in section 10.5.8.1 of 3GPP TS 44.018)
    

  PARAMETERS

    tlli:     TLLI to be inserted in the GPRS Information message
  
    dsm_ptr:  pointer to dsm item representing an LLC PDU.

    gas_id:   GERAN Access Stratum Identifier

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
extern void gttp_send_llc_pdu_to_rr
(
  uint32          tlli,
  dsm_item_type   *dsm_ptr,
  const gas_id_t   gas_id
);


/*==========================================================================

  FUNCTION    gttp_send_dl_pdu_to_llc

  DESCRIPTION
    
    This function is intended to be used by GRR to send a downlink PDU obtained
    via GTTP to LLC. The GRR_DATA_IND primitive borrowed from the RLC/LLC 
    interface is used to avoid adding another interface to LLC
    
  PARAMETERS

    tlli:
        TLLI contained in the GPRS Information message

    llc_pdu_container_ptr:
        pointer to the LLC PDU Container Information Element
        (defined in section 10.5.8.1 of 3GPP TS 44.018)

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
extern void gttp_send_dl_pdu_to_llc
(
  uint32                    tlli,
  gttp_llc_pdu_container_t  *llc_pdu_container_ptr,
  const gas_id_t            gas_id
);


#endif /* FEATURE_GSM_DTM */

#endif /* INC_GTTP_H */

/* EOF */

