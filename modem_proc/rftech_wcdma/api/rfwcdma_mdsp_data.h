#ifndef RFWCDMA_MDSP_DATA_H
#define RFWCDMA_MDSP_DATA_H

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_mdsp_data.h#1 $
  $DateTime: 2016/12/13 07:59:54 $
  $Author: mplcsds1 $

when       who     what, where, why 
--------   ---     ---------------------------------------------------------- 
06/12/14   zhh     Pass event size while allocate memory for CCS task
12/13/13    dw     Merge type definition between RFLM and RFWCDMA_MDSP
10/23/13   kai     Add flag to indicate if rflm wcdma ccs taskQ is initialized
10/15/13    dw     [1]Make DM tech buffer inteface array of LM handles in mdsp data for more visibility in debugging
                   [2]Added flag to track if RxAGC cell has been added
09/16/13    dw     Add support for tech DM buffer
08/29/13    dw     Update with FED DM support
08/23/13    dw     Initial Verision
==============================================================================*/

#include "comdef.h"
#include "rfcommon_locks.h"
#include "rfwcdma_mdsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of RF MDSP-FW intferface data
 
  @details
  This structure will be used to hold all interface data pointers between 
    RF-MDSP module and WFW shared memory/FED data manager
*/
typedef struct
{
  /*!< Base address for the WFW-SW shared memory interface*/
  WfwSwIntfStruct* wfw_intf_ptr;

  /*!< FED DM WCDMA tech buffer interface pointer per LM handle */
  rflm_wcdma_dm_buf_type* dm_intf_ptr[RXLM_MAX_BUFFERS];

}rfwcdma_mdsp_fw_intf_type;


  /*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition for RF MDSP async status data
 
  @details
  This structure will be used to hold all global status variable needed
  for async FW interface 
*/
typedef struct
{
  /*!< This flag will be used to synchronize with FW HDET action counter */
  uint8 hdet_convstarted_flag;

  /*!< Flag to indicate cell has been added to FED RxAGC module for the given LM handle */
  boolean rxagc_cell_added[RXLM_MAX_BUFFERS];

}rfwcdma_mdsp_data_status_type;

    /*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition for RF MDSP sync cmd status data
 
  @details
  This structure will be used to hold all global status variable needed
  for sync FW cmd interface
*/
typedef struct
{
  /*!< This flag will be used to track FW AGC cmd status */
  boolean rfwcdma_mdsp_agc_cmd_status;

  /*!< This flag will be used to track FW IQ capture cmd status */
  boolean rfwcdma_mdsp_iq_cmd_status;

}rfwcdma_mdsp_cmd_status_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to hold data pertaining to RF-MDSP for WCDMA.
 
  @details
  This defines the structure to hold data pertaining to WCDMA RF-MDSP. All data
  in this structure can be accessed only through the accessor function.
*/
typedef struct
{
  /*!< This structure will be used to hold all interface data pointers between 
    RF-MDSP module and WFW shared memory/FED data manager */
  rfwcdma_mdsp_fw_intf_type intf_data;

  /*!< This structure will be used to hold all global status variable for 
    async FW interface*/
  rfwcdma_mdsp_data_status_type data_status;

  /*!< This structure will be used to hold all global status variable for 
    aync FW interface*/
  rfwcdma_mdsp_cmd_status_type cmd_status;

  /*!< This structure will be used to hold all CCS event status data for 
    CCS event FW interface*/
  rfwcdma_mdsp_ccs_event_type event_settings[RF_WCDMA_EVENT_NUM];

  /*!< Critical section lock for RFWCDMA-MDSP module */
  rf_lock_data_type rfwcdma_mdsp_lock;

  /*!< Flag to indicate if mdsp data module is initialized */
  boolean rfwcdma_mdsp_data_initialized;

  /*!< Flag to indicate if rflm wcdma ccs taskQ is initialized */
  boolean rflm_wcdma_ccs_taskq_initialized;

}rfwcdma_mdsp_data_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Initialize MDSP Data structure
 
  @details
  This function will intialize the RF WCDMA MDSP Data structure to default state
 
  @return
  Status of MDSP Data initialization
*/
boolean
rfwcdma_mdsp_init_data
(
  void
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Get the pointer to the RF WCDMA MDSP data
 
  @details
  This function will return the pointer to the RF WCDMA MDSP global data structure
 
  @return
  pointer to the RF WCDMA MDSP global data structure
*/
rfwcdma_mdsp_data_type* rfwcdma_mdsp_get_data
(
  void
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Funtion to query if RF WCDMA-MDSP module is initialized
 
  @details
 
  @return
  TRUE if RF WCDMA-MDSP module is initialized
*/
boolean rfwcdma_mdsp_is_initialized
(
  void
);


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Allocate DM buffers for RF-MDSP Rx config parameter storage per LM handle
 
  @details
  This function will allocate Rx data buffers for RF MDSP config data to be passed
  to FED DM, and update the buffer address to the MDSP data
  Base DM buffer interface has to be initialized 
 
  @return
  Flag indicating the function execution success
*/

boolean rfwcdma_mdsp_allocate_dm_rx_buf
(
  rflm_dm_handle_id_t rflm_handle
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Allocate DM buffers for RF-MDSP Tx config parameter storage per LM handle
 
  @details
  This function will allocate Tx data buffers for RF MDSP config data to be passed
  to FED DM, and update the buffer address to the MDSP data
  Base DM buffer interface has to be initialized 
 
  @return
  Flag indicating the function execution success
*/

boolean rfwcdma_mdsp_allocate_dm_tx_buf
(
  rflm_dm_handle_id_t rflm_handle
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Funtion to do RF WCDMA data module cleanup
 
  @details
  Free up allocated memory allocated for RFWCDMA MDSP. To be caalled when WCDMA
  stack is teared down.
 
  @return
  TRUE if Rcleanup is successful
*/
boolean rfwcdma_mdsp_data_cleanup
(
  void
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Funtion to init event_setting of rfwcdma_mdsp_data_ptr
 
  @details
  init event_settint of rfwcdma_mdsp_data_ptr
 
  @return
  TRUE if init event setting successful
*/
boolean
rfwcdma_mdsp_data_init_event_setting
(
  rflm_wcdma_event_id_type event_id,
  rfwcdma_mdsp_ccs_event_unique_setting_type* event_setting
);
#endif /* RFWCDMA_MDSP_DATA_H */
