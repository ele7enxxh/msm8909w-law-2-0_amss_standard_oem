

/*!
  @file
  rfnv_tdscdma_b40_b.h

  @brief

  @details
  This header file contains definitions to interface with RF NV for TDSCDMA B40_B.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Technologies Proprietary

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
#if 0

#ifndef RFNV_TDSCDMA_B40_B_H
#define RFNV_TDSCDMA_B40_B_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "nv.h"
#include "customer.h"
#include "rfnv_tdscdma.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  global to hold TDSCDMA B40_B NV items list

*/
extern const rfcommon_rfnv_items_list_type rfnv_tdscdma_b40_b_tx_items_list[];
/*----------------------------------------------------------------------------*/
/*!
  @brief
  global to hold the TDSCDMA B40_B items table

*/
extern rfnv_tdscdma_tx_tbl_type rfnv_tdscdma_b40_b_tx_tbl;

/*===========================================================================

                            FUNCTION PROTOTYPES

===========================================================================*/
void rfnv_tdscdma_b40_b_copy_tx_item
(
rfnv_item_id_enum_type  item_code,       //!< NV Item to read
rfnv_item_type        *rfnv_item_ptr     //!< Pointer where to get the item
);

uint16 rfnv_tdscdma_b40_b_tx_list_size_get( void );

void *rfnv_tdscdma_b40_b_tx_item_table_get( void );

#endif /* RFNV_TDSCDMA_B40_B_H */

#endif