#ifndef TF_DSM_MAIN_H
#define TF_DSM_MAIN_H
/*!
  @file tf_dsm_main.h

  @brief Contains protoype for all the print functions needed for the tests.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/core.mpss/3.5.c12.3/memory/dsm/test/tf_dsm_main.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/09   ag      Initial Revision. Ported from linux tests.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "dsm.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

void tf_print_buf(uint8 * buf, int len);
void tf_dsm_print_item(dsm_item_type * item);
void tf_dsm_print_packet(dsm_item_type * item);
void tf_dsm_print_pool(dsm_mempool_id_enum_type pool);
void tf_dsm_print_wm ( dsm_watermark_type *wm_ptr );
int tf_dsm_cmp_item( dsm_item_type * item1, dsm_item_type * item2 );
int tf_dsm_cmp_packet_naive( dsm_item_type * item1, dsm_item_type * item2 );


#endif /* TF_DSM_MAIN_H */
