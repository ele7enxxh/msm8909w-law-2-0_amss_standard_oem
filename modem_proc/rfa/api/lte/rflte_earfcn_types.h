/*!
  @file
  rflte_earfcn_types.h

  @brief
  Type definitions related to EARFCNs.
*/

/*===========================================================================

  Copyright (c) 2015 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/lte/rflte_earfcn_types.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/02/16   bmg     Added common RFLTE_INVALID_EARFCN definition
09/28/15   bmg     Created
===========================================================================*/

#ifndef LTE_EARFCN_TYPES_H
#define LTE_EARFCN_TYPES_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This value represents an invalid EARFCN value.
*/
#define RFLTE_INVALID_EARFCN 0xFFFFFFFF

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Defines the size of EARFCN as per the
  3GPP Release 10 spec CR RP-130237/CR-01257
*/
typedef uint32 rfcom_lte_earfcn_type;

#endif /* LTE_EARFCN_TYPES_H */

