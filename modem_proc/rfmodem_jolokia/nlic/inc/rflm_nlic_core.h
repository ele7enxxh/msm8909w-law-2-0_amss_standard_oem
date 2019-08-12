#ifndef RFLM_NLIC_CORE_H
#define RFLM_NLIC_CORE_H

/*!
  @file
  rflm_nlic_core.h

  @brief
  Export the API necessary for calling modules to get the NLIC dynamic settings
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/nlic/inc/rflm_nlic_core.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/28/14   ska     Adding fix for reading correct NLIC delay value
09/26/14   dr      Featurizing LTE code
09/17/14   dr      Fixing non LTE config for Jolokia
06/24/14   bsh     Add new elements to NLIC return params
05/01/14   bsh     NLIC status to ML1
03/14/14   bsh     NlIC changes: code clean up 
03/12/14   bsh     Initial check in

==============================================================================*/

#include "rflm_nlic_settings_ag.h"
#include "rfcom.h"

#ifdef FEATURE_LTE
#include "rflte_mc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define FEATURE_RFSW_LTE_NLIC_ENABLED               1
#define RFLM_NLIC_NV_PARAM_DATA_LIST_MAX_SIZE       25      /*This is the size of total param sets defined in nv defintion and static xml. */

/*NV Structure for NLIC*/
typedef PACK (struct)
{
    /*PCC Band Enum*/
    uint8                   pcc_band;
    /*SCC Band Enum*/
    uint8                   scc_band;
    /*Victim Carrier Index*/
    uint8                   victim_carrier_index;
    /* Tx Power threshold */
    int8                    tx_pwr_threshold;		   
    /* Linear m value*/
    int8                    linear_m;
    /* Linear q value */
    int16                   linear_q;
    /*Kernel Select*/
    uint8                   kernel_sel;
    /*Antenna Select*/
    uint8                   antenna_sel;
    /*Reserved place holders*/
    uint32                  reserve_1;
    uint32                  reserve_2;

}rfnv_rflm_nlic_nv_params_type;

typedef struct
{
    /*Number of elements */
    uint8                   num_elememts;
    /*PCC Band Enum*/
    uint8                   pcc_band;
    /*SCC Band Enum*/
    uint8                   scc_band;
    /*Victim Carrier Index*/
    uint8                   victim_carrier_index;
    /* Tx Power threshold */
    int8                    tx_pwr_threshold;		   
    /* Linear m value*/
    int8                    linear_m;
    /* Linear q value */
    int16                   linear_q;
    /*Kernel Select*/
    uint8                   kernel_sel;
    /*Antenna Select*/
    uint8                   antenna_sel;
    /*Reserved place holders*/
    uint32                  reserve_1;
    uint32                  reserve_2;

}rflm_nlic_nv_params_type;

typedef PACK (struct)
{
  /* Delay values: 2D array of number of ul x dl BW possible */
  int16 delay_value[NLIC_NUM_LTE_BWS][NLIC_NUM_LTE_BWS];
}rflm_nlic_nv_delay_type;


/*----------------------------------------------------------------------------*/
#ifdef FEATURE_LTE
boolean rflm_nlic_get_dynamic_settings( uint32 tx_aggr_freq, 
                                        uint32 rx_vict_freq, 
                                        uint32 rx_aggr_greq,
                                        uint32 ul_bw,
                                        uint32 dl_bw, 
                                        rflm_nlic_data_type *nlic_data, 
                                        int32 rssi,
                                        nlic_input_params_per_sf *nlic_input_info );

void rflm_nlic_update_dynamic_settings(nlic_input_params_per_sf *nlic_input_info, nlic_return_params_per_sf *nlic_return_info);

boolean rflm_nlic_core_get_nlic_status(nlic_status_input_params *nlic_input_params, nlic_status_return_params *nlic_return_params);

void rflm_nlic_update_static_settings(rflte_mc_carrier_info_type carrier_info,
                                      rfcom_lte_band_type rx_band,
                                      rfcom_lte_bw_type rx_bw);

rfcommon_nv_status_type rflm_nlic_core_retrieve_nv_items( rex_tcb_type *task_ptr, 
                                                          rex_sigs_type task_wait_sig,
                                                          void (*task_wait_handler)( rex_sigs_type ) );

#endif

uint8 rflm_nlic_conv_bw_to_nlic_bw(rfcom_lte_bw_type input_bw);


#ifdef __cplusplus
}
#endif

#endif /*RFLM_NLIC_CORE_H*/

