#ifndef RFLM_DTR_RX_COMMON_RESOURCES_H
#define RFLM_DTR_RX_COMMON_RESOURCES_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
   @file
   rflm_dtr_rx_common_resources.h

   @brief
   This file implements the Rx common resource management.
   These are platform dependent.

*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/lm/inc/rflm_dtr_rx_common_resources.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/09/14   dej     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

/* for RFLM_DTR_RX_NUM_CMNNOTCH_BLOCKS: */
#include "rflm_dtr_rx_fw_intf_ag.h"
/* for RXLM_MAX_BUFFERS and lm_types: */
#include "rxlm_intf.h"

/*! @brief MACRO to indicate Common Notches are disabled for a particular group */
#define RFLM_DTR_RX_DISABLED_CMNNOTCH_WB_IDX 0xF

/*! @brief Structure to hold information for 
           RxFE Common Notch filters owners */
typedef struct
{
  uint8 primary_handle;
  uint8 irat_handle;
  uint8 wb_idx;
} rflm_dtr_rx_cmnnotch_info_struct;

extern rflm_dtr_rx_cmnnotch_info_struct cmn_notch_alloc[RFLM_DTR_RX_NUM_CMNNOTCH_BLOCKS];

void rflm_dtr_rx_common_notch_init(void);

lm_status_type rflm_dtr_rx_common_notch_allocate( lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32* cmn_notch_dyn_block_id );

lm_status_type rflm_dtr_rx_common_notch_deallocate( lm_handle_type rxlm_handle, rxlm_chain_type chain, uint32 cmn_notch_dyn_block_id );

#ifdef __cplusplus
}
#endif



#endif


