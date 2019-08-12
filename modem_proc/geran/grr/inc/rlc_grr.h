/*==============================================================================

                 R L C    G R R    I N T E R F A C E    H E A D E R

GENERAL DESCRIPTION

      Interface Definition between RLC and GRR modules

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005-2013 Qualcomm Technologies, Inc.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/inc/rlc_grr.h#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
08/15/05   hv     Initial version

==============================================================================*/

#ifndef INC_RLC_GRR_H
#define INC_RLC_GRR_H


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


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "gttp.h"           /* gttp_llc_pdu_container_t */

typedef enum
{
#ifdef FEATURE_GSM_DTM
  /*
  Primitive for RLC to request GRR to send an LLC PDU to L2 (GTTP transfer)
  */
  RG_UL_DCCH_REQ,
#endif /* #ifdef FEATURE_GSM_DTM */
  RG_MAX
} rlc_grr_sig_id_t;

#ifdef FEATURE_GSM_DTM

/*************************************************************************
** Message struct for RLC to send the RG_UL_DCCH_REQ primitive to GRR
**************************************************************************
*/
typedef struct
{
  IMH_T                     message_header;
  uint32                    tlli;
  gttp_llc_pdu_container_t  gttp_llc_pdu_container;
  gas_id_t                  gas_id;
} rlc_grr_ul_dcch_req_t;

#endif /* #ifdef FEATURE_GSM_DTM */

/*************************************************************************
*  Union of all RLC to GRR messages
*************************************************************************/

#ifdef FEATURE_GSM_DTM

typedef union
{
  rlc_grr_ul_dcch_req_t  rlc_grr_ul_dcch_req;

} rlc_grr_sig_t;

#endif /* #ifdef FEATURE_GSM_DTM */

/*==============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

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



#endif /* INC_RLC_GRR_H */

/* EOF */
