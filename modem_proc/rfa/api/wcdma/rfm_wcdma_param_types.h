#ifndef RFM_WCDMA_PARAM_TYPES_H
#define RFM_WCDMA_PARAM_TYPES_H
/*! @brief
  @file

  @brief
  This module contains the definitions of the WCDMA RFM interface parameters.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/wcdma/rfm_wcdma_param_types.h#1 $

when       who     what, where, why 
-------   ---     -------------------------------------------------------------
08/14/14  vbh     Added Tx rfm api interface structures
08/05/14  vbh     Updated the sleep params structure to take a flag to skip AGC disable
04/11/14  vbh     [FR17754] Added interface structure for tune 
3/05/14   vbh     Initial revision

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "rfcom.h"
#include "rfm_types.h"



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Identifies individual carriers of a multi-carrier WCDMA radio.
*/
typedef enum
{

  RFM_WCDMA_CARRIER_0 = 0,
  /*!< Carrier 0 */

  RFM_WCDMA_CARRIER_1 = 1,
  /*!< Carrier 1 */

  RFM_WCDMA_CARRIER_2 = 2,
  /*!< Carrier 2 */

  RFM_WCDMA_CARRIER_MAX

} rfm_wcdma_carrier_type;

/*----------------------------------------------------------------------------*/

/* XPT mode conifguration - APT/EPT/ET */
typedef enum
{
  /*! @brief APT mode */
  RFM_TXAGC_APT,

  /*! @brief EPT mode */
  RFM_TXAGC_EPT, 

  /*! @brief ET mode */
  RFM_TXAGC_ET, 

  /*! @brief max xPT mode */
  RFM_TXAGC_MAX_XPT_MODE  
}rfm_wcdma_xpt_mode_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration to specify the device type for enter mode */
typedef enum
{
  RFM_WCDMA_RX_DEVICE = 0,
  /*!< Primary Rx device */

  RFM_WCDMA_TX_DEVICE = 1,
  /*!< Tx device */

  RFM_WCDMA_INVALID_DEVICE
  /*!< INVALID DEVICE */

} rfm_wcdma_rx_tx_device_type;

/*----------------------------------------------------------------------------*/
/*! Macro defining maximum RFM devices supported for concurrent wakeup */
#define RFM_WCDMA_MAX_DEVICE RFM_DEVICE_4

/*----------------------------------------------------------------------------*/
/*! Macro defining maximum carriers supported on a device */
#define RFM_WCDMA_MAX_CARRIERS 3

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Macro type definition for carrier mask. 

  @details
  Defintion of each bit is given by #rfm_wcdma_carrier_type. For example, if carrier 0 and carrier 2
  is to be enabled, carrier mask can be computed as
  (1<<RFM_WCDMA_CARRIER_0)|(1<<RFM_WCDMA_CARRIER_2)
*/
#define rfm_wcdma_carrier_mask_type uint8

/*! @cond vbhide */

/*----------------------------------------------------------------------------*/
/*! Structure to hold the parameters passed by L1 to wakeup WCDMA Rx  */
typedef struct
{
  struct
  {
     boolean is_valid;
     /*!< Flag to indicate if wakeup shall happen on this device */
 
     lm_handle_type rxlm_handle;
     /*!< RxLM buffer to be used to configure the modem hardware */
 
  } api_params[RFM_WCDMA_MAX_DEVICE];
  /*!< Structure to hold wakeup relevant parameters on all supported RFM devices */

}rfm_wcdma_rx_wakeup_params;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the parameters passed by L1 to put WCDMA Rx to sleep  */
typedef struct
{
  struct
  {
     boolean is_valid;
     /*!< Flag to indicate if sleep shall happen on this device */
 
     lm_handle_type rxlm_handle;
     /*!< RxLM buffer to be used to configure the modem hardware */
 
     boolean skip_agc_disable; 
 
  } api_params[RFM_WCDMA_MAX_DEVICE];
  /*!< Structure to hold sleep relevant parameters on all supported RFM devices */

}rfm_wcdma_rx_sleep_params;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the parameters passed by L1 to tune WCDMA Rx */
typedef struct
{
  struct
  {
     boolean is_valid;
     /*!< Flag to indicate if sleep shall happen on this device */
 
     lm_handle_type rxlm_handle;
     /*!< RxLM buffer to be used to configure the modem hardware */

     uint16 channel[RFM_WCDMA_MAX_CARRIERS];
     /*!< List of channels which need to be tuned on the particular device*/

     uint8 carrier_mask;
     /*!< Carrier mask to indicate the number of enabled carriers (0/1/2)*/
 
  } api_params[RFM_WCDMA_MAX_DEVICE];
  /*!< Structure to hold sleep relevant parameters on all supported RFM devices */

}rfm_wcdma_rx_tune_params;

/*----------------------------------------------------------------------------*/

/*! @endcond */

/*! Structure to hold the parameters passed by L1 to initialize WCDMA Tx */
typedef struct
{

  rfm_device_enum_type device;
  /*!< Variable to indicate the RF device to be initialzed for Tx   */

  lm_handle_type txlm_handle;
  /*!< TxLM buffer to be used to configure the modem hardware */

  rfm_wcdma_carrier_mask_type carrier_mask;
  /*!< Index to indicate the carrier configuration on the device*/

}rfm_wcdma_tx_init_params;
/*----------------------------------------------------------------------------*/

/*! Structure to hold the parameters passed by L1 to deinit WCDMA Tx */
typedef struct
{
  
  rfm_device_enum_type device;
  /*!< Variable to indicate the RF device to be initialzed for Tx   */

}rfm_wcdma_tx_deinit_params; 
/*----------------------------------------------------------------------------*/

/*! Structure to hold the parameters passed by L1 to enable WCDMA Tx */
typedef struct
{
  
  rfm_device_enum_type device;
  /*!< Variable to indicate the RF device to be initialzed for Tx   */

}rfm_wcdma_tx_enable_params; 
/*----------------------------------------------------------------------------*/


/*! Structure to hold the parameters passed by L1 to disable WCDMA Tx */
typedef struct
{
  
  rfm_device_enum_type device;
  /*!< Variable to indicate the RF device to be initialzed for Tx   */

}rfm_wcdma_tx_disable_params; 
/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8        xpt_mode;
  uint8        pa_range;

}rfm_wcdma_txagc_log_param_type;
/*----------------------------------------------------------------------------*/




#ifdef __cplusplus
}
#endif

#endif
