#ifndef RR_INTER_HO_IF_H
#define RR_INTER_HO_IF_H

/*============================================================================
  @file rr_inter_ho_if.h

  Interface for the rr_inter_ho module.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_inter_ho_if.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"
#include "nas_exp.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Input messages. */
typedef enum
{
  RR_INTER_HO_IMSG_COUNT
} rr_inter_ho_imsg_e;

typedef union
{
  rr_message_header_t           header;
} rr_inter_ho_imsg_u;

/* Output messages. */
typedef enum
{
  RR_INTER_HO_OMSG_COUNT
} rr_inter_ho_omsg_e;

typedef union
{
  rr_message_header_t           header;
} rr_inter_ho_omsg_u;

/* Union of all input and output messages. */
typedef union
{
  rr_inter_ho_imsg_u imsg;
  rr_inter_ho_omsg_u omsg;
} rr_inter_ho_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


#endif /* #endif RR_INTER_HO_IF_H */

/* EOF */

