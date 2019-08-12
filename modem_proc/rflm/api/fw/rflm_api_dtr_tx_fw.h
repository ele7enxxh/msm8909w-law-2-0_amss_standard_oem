/*!
  @file
  rflm_dtr_tx_fw_intf.h

  @brief
  TX Link manager RFLM API definitions
  
  @detail
  API's for following Link Manager related API's are provided:
  - XO parameter query
  - chain usage query
  - NB configuration
  
  @todo
  - comment function prototypes

*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/fw/rflm_api_dtr_tx_fw.h#1 $

when       who     what, where, why
11/06/13   st      LMEM Info Storage for DRIF Group A/B
09/09/14   sar     Updated RFLM_API_DTR_TX_VERSION macro to use TX instead of RX.
--------   ---     ----------------------------------------------------------
==============================================================================*/

#ifndef RFLM_API_DTR_TX_H
#define RFLM_API_DTR_TX_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "rflm_api_cmn.h"
#include "rflm_dtr_tx_lmem.h"
#include "txlm_intf.h"

/*! @brief Version of the rflm dtr tx module.
*/
#define RFLM_API_DTR_TX_VERSION    \
              ((RFLM_DTR_TX_MAJOR_REV << 16) | RFLM_DTR_TX_FW_INTF_REV )



/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================

FUNCTION:  rflm_dtr_tx_get_group_settings

==============================================================================*/
/*!
  @brief
  Get DTR TX XO related parameters

  @detail
 
  @return
  rflm error code
*/
/*============================================================================*/
extern rflm_err_t rflm_dtr_tx_get_group_settings( rflm_handle_tx_t handle,
                                                  rflm_dtr_tx_fw_export_group_id group_id,
                                                  uint32 *addr );


/*==============================================================================

FUNCTION:  rflm_dtr_tx_configure_chain

==============================================================================*/
/*!
  @brief
  Configure Chain

  @detail
  TBD
 
  @return
  rflm error code
*/
/*============================================================================*/
extern rflm_err_t rflm_dtr_tx_configure_chain( rflm_handle_tx_t handle,
											   rflm_dtr_tx_indices_struct *tx_indices,											   
                                               uint32 config_mask,
                                               /*! Pointer to block of data need to be DMA transferred */
  										       rflm_dtr_tx_xfer_list_s * txfe_xfer_list_ptr);

/*==============================================================================

FUNCTION:  rflm_dtr_tx_txdac_reactivate

==============================================================================*/
/*!
  @brief
  Reactivate TX DAC

  @detail
  TBD
 
  @return
  void
*/
/*============================================================================*/
extern void rflm_dtr_tx_txdac_reactivate(txlm_chain_type chain, boolean blocking);

/*==============================================================================

FUNCTION:  rflm_dtr_tx_txdac_power_down

==============================================================================*/
/*!
  @brief
  Turn off TX DAC

  @detail
  TBD
 
  @return
  void
*/
/*============================================================================*/
extern void rflm_dtr_tx_txdac_power_down(txlm_chain_type chain);

/*==============================================================================

FUNCTION:  rflm_dtr_tx_set_lmem_start_addr

==============================================================================*/
/*!
  @brief
  Stores LMEM DRIF Group A (Set 0/1/2 for all techs) 

  @detail
  FW Calls this API Once for each Chain
 
  @return
  rflm error code

*/
/*============================================================================*/
void rflm_dtr_tx_set_lmem_start_addr(
	                              rflm_dtr_tx_indices_struct *tx_indices,
								  /*! Pointer to LMEM start physical address */
                                  uint32 * lmem0_start_addr);

/*==============================================================================

FUNCTION:  rflm_dtr_tx_set_lmem_drif3_start_addr

==============================================================================*/
/*!
  @brief
  Stores LMEM DRIF Group B (Extra Sets for LTE)

  @detail
  FW Calls this API Once for each Chain
 
  @return
  rflm error code


*/
/*============================================================================*/

void rflm_dtr_tx_set_lmem_drif3_start_addr (
	                              rflm_dtr_tx_indices_struct *tx_indices,
								  /*! Pointer to LMEM start physical address */
                                  uint32 * lmem0_start_addr);

/*==============================================================================

FUNCTION:  rflm_dtr_tx_set_lmem_info

==============================================================================*/
/*!
    @brief
  DEPRECATED API

  @detail
  DEPRECATED API
 
  @return
  DEPRECATED API
*/

/*============================================================================*/


extern rflm_err_t rflm_dtr_tx_set_lmem_info
  (rflm_dtr_tx_lmem_drif_grp_start_info_t* dtr_tx_lmem_info);

/*==============================================================================

FUNCTION:  rflm_dtx_tx_set_shared_mem_addr

==============================================================================*/
/*!
  @brief
  Stores LMEM DRIF Group A and B (Set 0/1/2 for all techs) 

  @detail
  FW Calls this API Once
 
  @return
  void
*/
/*============================================================================*/
extern void rflm_dtr_tx_set_shared_mem_addr( const rflm_dtr_tx_set_shared_mem_input_t *in, rflm_dtr_tx_set_shared_mem_output_t *out);

/*==============================================================================

FUNCTION:  rflm_dtx_tx_get_shared_mem_addr

==============================================================================*/
/*!
  @brief
  Retreive LMEM DRIF Group A and B (Set 0/1/2 for all techs) 

  @detail
  Retreive LMEM DRIF Group A and B based on the handle.
 
  @return
  void
*/
/*============================================================================*/
extern void rflm_dtr_tx_get_shared_mem_addr(lm_handle_type txlm_handle, const rflm_dtr_tx_get_shared_mem_input_t* in, rflm_dtr_tx_get_shared_mem_output_t* out);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_API_DTR_TX_H */
