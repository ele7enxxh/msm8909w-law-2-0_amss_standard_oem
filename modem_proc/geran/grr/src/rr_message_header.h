#ifndef RR_MESSAGE_HEADER_H
#define RR_MESSAGE_HEADER_H

/*============================================================================
  @file rr_message_header.h

  Contains definition of common header that must appear as the first item
  in every signal sent on the RR signalling interface (MS_RR_RR).

                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_message_header.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "nas_exp.h"
#include "rr_message_set.h"
#include "geran_dual_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  IMH_T imh;
  rr_message_set_e rr_message_set;
  gas_id_t         gas_id; /* the GERAN AS ID that SENT the message */
} rr_message_header_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#endif /* #ifndef RR_MESSAGE_HEADER_H */

/* EOF */

