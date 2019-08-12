#ifndef RFNV_TDSCDMA_B34_H
#define RFNV_TDSCDMA_B34_H

/*!
  @file
  rfnv_tdscdma_b34.h

  @brief

  @details
  This header file contains definitions  to interface with RF NV for TDSCDMA band 34(2010-2025MHz).

*/

/*===========================================================================

Copyright (c) 2008, 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/nv/inc/rfnv_tdscdma_b34.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/14   jps     Updates for get PA switchpoints API
10/14/14   jps     Add support for GET_PA_SWITCHPOINTS command
05/29/14   ych     Updated for 8 PA gain states
04/19/11   hy      Initial Revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "nv.h"
#include "rfnv_tdscdma.h"

/*===========================================================================

                       PUBLIC DATA DECLARATIONS

===========================================================================*/

extern const rfcommon_rfnv_items_list_type rfnv_tdscdma_b34_rx_items_list[ ];
extern const rfcommon_rfnv_items_list_type rfnv_tdscdma_b34_tx_items_list[ ];

extern rfnv_tdscdma_rx_tbl_type rfnv_tdscdma_b34_rx_tbl;
extern rfnv_tdscdma_tx_tbl_type rfnv_tdscdma_b34_tx_tbl;


/*===========================================================================

                         FUNCTION PROTOTYPES

===========================================================================*/

/*--------------------------------------------------------------------------*/
extern void rfnv_tdscdma_b34_copy_rx_item
(
  rfnv_item_id_enum_type  item_code,    /* Item to put */
  rfnv_item_type          *rfnv_item_ptr   
);

extern void rfnv_tdscdma_b34_copy_tx_item
(
  rfnv_item_id_enum_type  item_code,    /* Item to put */
  rfnv_item_type          *rfnv_item_ptr   
);

uint16 rfnv_tdscdma_b34_rx_list_size_get( void );
uint16 rfnv_tdscdma_b34_tx_list_size_get( void );

void *rfnv_tdscdma_b34_rx_item_table_get( void );
void *rfnv_tdscdma_b34_tx_item_table_get( void );

boolean rfnv_tdscdma_b34_tx_get_new_nv_flag( void );

boolean rfnv_tdscdma_b34_tx_get_pa_swpt(int16* rise_swpt, int16* fall_swpt, int16* active_pa_states);
#endif /* RFNV_TDSCDMA_B34_H */
