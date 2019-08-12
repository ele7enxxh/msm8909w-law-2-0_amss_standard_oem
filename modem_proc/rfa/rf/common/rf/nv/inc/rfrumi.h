/*!
  @file
  rfrumi.h

  @brief
  This header file contains definitions  to interface with rfrumi.c

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header:  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/08/15   mmd     Enabling RUMI featurization
10/24/13   vv      Removing RUMI featurization
07/26/13   bsh     Initial checkin
===========================================================================*/



#include "comdef.h"
#include "nv.h"
#include "rfcommon_core_types.h"
#include "rfcommon_nv.h"

#ifdef T_RUMI_EMULATION
boolean rfrumi_nv_get_item(rfnv_item_id_enum_type item_code, nv_item_type *rumi_item_ptr);

#endif 

/* RFRUMI_H */
