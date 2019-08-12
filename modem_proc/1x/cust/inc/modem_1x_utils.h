
#ifndef MODEM_1X_UTILS_H
#define MODEM_1X_UTILS_H
/*===========================================================================
                      M O D E M   1 X   U T I L S

DESCRIPTION
  This file contains 1x modem common utilities.

  Copyright (c) 2009 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cust/inc/modem_1x_utils.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/11   srk     Feature clean up: Mainlined FEATURE_MODEM_1X_SUPPORTS_Q6
06/28/11   jtm     Removed off-target specific definitions.
04/21/10   jtm     Added off-target specific definitions.
03/30/10   jtm     Replaced FEATURE_QDSP6 with FEATURE_MODEM_1X_SUPPORTS_Q6.
12/29/09   mca     Added ONEX_CVT_Q2N()
11/16/09   vks     Initial revision.

===========================================================================*/

/* Common */
#include "customer.h"

/* 1X */
#include "modem_1x_defs.h"

/* Other */
#include "err.h"



/*===========================================================================
                                 MACROS
===========================================================================*/

/* macro to prevent compiler warnings when the payload parameter is not used
   by state entry, exit, and transition functions, or when a variable is only
   used in MSG_XXXX calls that may be compiled out */
#define MODEM_1X_UNUSED(i) (i)=(i)

/* REVISIT: Add an another ASSERT that doesn't do an error fatal? */
/* macro to do error fatal when assert condition fails */
#define ONEX_ASSERT( xx_exp ) \
  if( !(xx_exp) ) \
  { \
    ERR_FATAL( "Assertion " #xx_exp " failed", 0, 0, 0 ); \
  }

/* Replaces the old QW_CVT_Q2N() for native 64-bit architectures */
#define ONEX_CVT_Q2N( qw_value ) (((uint64)(qw_hi(qw_value)) << 32) + \
                                   (uint64)(qw_lo(qw_value)))

#endif /* MODEM_1X_UTILS_H */
