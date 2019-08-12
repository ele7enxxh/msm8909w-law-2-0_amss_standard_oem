#ifndef RFWCDMA_MDSP_SYNC_H
#define RFWCDMA_MDSP_SYNC_H

/*! @brief
  @file

  @brief
  This module contains the definitions of the WCDMA RF-mDSP AGC interface.
*/

/*===========================================================================


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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_mdsp_sync.h#1 $
  $DateTime: 2016/12/13 07:59:54 $
  $Author: mplcsds1 $

when       who     what, where, why 
--------   ---     ----------------------------------------------------------
09/09/14   ak      Adding API to return the WCDMA TQ IDLE status.
08/28/14   vbh     Update the def of config_ccs with the device parameter
05/28/14   aa      New TQ pair management APIs for reserve and disassociation.
05/13/14   rmb     Enable W2X and L2W FTM IRAT.
04/14/14   rmb     Fix Compilation Error on Bolt 2.0 
04/14/14   rmb     Enabling W2X FTM IRAT and adding changes for L2W FTM IRAT.
04/03/14   dw      Add dynamic RxLM update support
03/25/14   rmb     Change Gas id to Nas id for G2W structure.
03/25/14   rmb     FTM IRAT 2nd phase checkin. 
02/27/14   rmb     First phase FTM IRAT checkin for G2W.
10/23/13   kai     Add wrapper API config/deconfig rflm wcdma ccs taskQ
09/09/13    dw     More updates for Bolt
08/23/13    dw     Initial Verision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "wl1api.h"
#include "rfwcdma_mdsp_types.h"
#include "geran_test_mode_api.h"

#ifdef FEATURE_LTE
#include "lte_l1_ftm.h"
#endif

/*===========================================================================

                        CONSTANT DEFINITIONS

===========================================================================*/

/*===========================================================================

                        TYPE DECLARATIONS

===========================================================================*/

#if defined(FEATURE_BOLT_MODEM)
/*----------------------------------------------------------------------------*/
/*! Structure types for G2W, L2W and W2X */
typedef struct
{
  /* RxLM buffer index for neighbour */
  uint32 g2w_rxlm_buf_idx;
  /* Freq error in Q6 Hz format */
  int32 freq_err_q6_hz;
  /* Inverse W DL freq */
  uint32 inv_dlo_freq;
  /* pre call back function */
  g2w_pre_cb_fn pre_cb; 
  /* post call back function */
  g2w_post_cb_fn post_cb;
  /* GL1 API converts NAS Id to GAS Id 
  and pass it to WL1; For single SIM, NAS Id = GAS Id
  For Dual Sim, NAS Id may not be equal to GAS Id*/
  sys_modem_as_id_e_type nas_id; 
}rfwcdma_mdsp_sync_g2w_cmd_param_type;

#ifdef FEATURE_LTE
typedef struct
{
  lte_mem_instance_type instance;
  /*! Cells for which measurements need to be made. */
  lte_ml1_md_wcdma_freq_info_s freq_cell_list_ptr;
}rfwcdma_mdsp_sync_l2w_start_w_cmd_param_type;

typedef struct
{
 lte_mem_instance_type instance;
}rfwcdma_mdsp_sync_l2w_stop_w_cmd_param_type;
#endif // FEATURE_LTE

typedef struct
{
  /* the rxlm buffer index used to tune back the primary Rx chain to WCDMA at
     the end of an idle w2x search */
  uint32 rxlm_home_buf_idx;
  /* the rxlm buffer index used to tune back the diversity Rx chain to WCDMA at
     the end of an idle w2x search */
  uint32 rxlm_home_rxd_buf_idx;
  /* determines if the search is w2l or w2g */
  mdsp_sync_w2x_idle_cmd_mode_enum_type mode;
  /* pre-command callback function */
  pre_cb_fn pre_cb;
  /* post-command callback function */
  post_cb_fn post_cb;
}rfwcdma_mdsp_sync_w2x_cmd_param_type;
#endif // FEATURE_BOLT_MODEM

/*===========================================================================

                              FUNCTION DECLARATIONS

===========================================================================*/

/*----------------------------------------------------------------------------*/

boolean rfwcdma_mdsp_sync_init(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper API config/deconfig rflm wcdma ccs taskQ

  @details
  Wrapper API config/deconfig rflm wcdma ccs taskQ according to param passed in
 
  @param state
  flag indicates action to process
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_sync_config_ccs
( 
  rfm_device_enum_type device,
  boolean state
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper API deconfig rflm wcdma ccs taskQ but keep the device association

  @details
  Wrapper API deconfig rflm wcdma ccs taskQ but keep the device association
 
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_ccs_release_tq_pair_reservce_device(void);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper API disassociate the taskQ with the device 

  @details
  Wrapper API disassociate the taskQ with the device
 
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_ccs_tq_pair_disassociate_device(void);



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper API release the TQ completely

  @details
  Wrapper API release the TQ completely
 
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_ccs_release_tq_pair(rfm_device_enum_type device);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls corresponding L1 mdsp_sync_cmd() to send command to FW depending on the input FW command type.

  @details
 
  @param cmd_type
  Type of command to be executed
 
  cmd_param
  Any input parameters associated with command

  @return
  Boolean flag indicating the return status of the API
*/
boolean rfwcdma_mdsp_sync_send_cmd( rfwcdma_mdsp_sync_send_cmd_id cmd_type, void* cmd_param );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function will return the status of the WCDMA TQ pair

  @details 

  @return
  Boolean flag indicating the return status of the TQ pair.
  Returns TRUE if both Tx and Rx TQ are IDLE.
*/

boolean rfwcdma_mdsp_is_tq_idle();




#endif /* RFWCDMA_MDSP_SYNC_H */
