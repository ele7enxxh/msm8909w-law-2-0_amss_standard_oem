#ifndef QFE_CDMA_INTF_H
#define QFE_CDMA_INTF_H

/*!
  @file
  qfe_cdma_intf.h

  @brief
  This is an abstract software interface to control or query the QFE
  device for the CDMA operation.

  @details
  This is a generic CDMA QFE device driver interface which consists
  of a collection of data and function pointers for CDMA operation. The
  interface is transparent to its clients. In another word, the client
  does not need to know what physical QFE devices it's talking to. 
 
  RFC knows what specific QFE devices are present in an installed RF card.
  It's also aware of which specific QFE device is in use for a given CDMA
  RF band. Once the connection betwen the logical radio path and the physical
  radio path is determined, RFC will call the explicit QFE device driver to
  update the data and function pointers structure.
 
  After the data and function pointer structure is updated, all the device
  driver data and functions within above structure are mapped to an exact RF
  device, thus allowing CDMA Main Control, Core and Feature to control or
  query the QFE Device.
*/


/*===========================================================================

  Copyright (c) 2012 - ? by Qualcomm Technologies, Inc.  All Rights Reserved.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/qfe_intf/inc/qfe_cdma_intf.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/19/12   zhw     QFE1320 scripting support
04/13/12   Saul    QFE. Initial support.

============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "qfe_intf_cmd.h"
#include "rfm_cdma_band_types.h"
#include "rfdevice_cmn_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QFE_CDMA_NUM_PA_RANGES 4
/*----------------------------------------------------------------------------*/
/*! @brief
    This data structure holds all info pertaining to PA range control.
*/
typedef struct
{
  /*! register address */
  uint16 reg;
  /*! register data */
  uint16 data[QFE_CDMA_NUM_PA_RANGES];
}qfe_cdma_pa_range_ctrl_type;

/*----------------------------------------------------------------------------*/
/*! @brief
    This data structure holds all info pertaining PA ON/Off control.
*/
typedef struct 
{
  /*! PA On/Off address */
  uint8 pa_onoff_addr;
  /*! PA On data */
  uint8 pa_on_data;
  /*! PA Off data */	
  uint8 pa_off_data;
}qfe_cdma_get_pa_onoff_ctrl_type;

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Forward declaration of device structure types to use in function pointers
  definitions
*/
typedef struct rfqfe_cdma rfqfe_cdma_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides generic get and set functionality for the device specific
  parameters.

  @param *qfe_device
  Pointer to QFE device

  @param cmd
  Specifies the command to performed by the device.

  @param data
  Input and/or Output parameter for the command.
*/
typedef boolean (*rfqfe_cdma_cmd_dispatch_func_ptr_type)
(
  rfqfe_cdma_type *qfe_device,
  qfe_cmd_enum_type cmd,
  void *data
);

/*----------------------------------------------------------------------------*/
boolean rfqfe_cdma_cmd_dispatch
( 
  rfqfe_cdma_type *qfe_device,
  qfe_cmd_enum_type cmd,
  void *data 
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configures device with wake-up tx settings.

  @param *qfe_device
  Pointer to QFE device 

  @param band
  Band class to determine which settings to use.
*/
typedef boolean (*rfqfe_cdma_wakeup_tx_ptr_type)
(
  rfqfe_cdma_type *qfe_device,
  rfm_cdma_band_class_type band,
  rfdevice_script_type *script_data_ptr
);

/*----------------------------------------------------------------------------*/
boolean rfqfe_cdma_wakeup_tx
(
  rfqfe_cdma_type *qfe_device,
  rfm_cdma_band_class_type band,
  rfdevice_script_type *script_data_ptr
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configures device with sleep tx settings.

  @param *qfe_device
  Pointer to QFE device 
*/
typedef boolean (*rfqfe_cdma_sleep_tx_ptr_type)
(
  rfqfe_cdma_type *qfe_device,
  rfdevice_script_type *script_data_ptr
);

/*----------------------------------------------------------------------------*/
boolean rfqfe_cdma_sleep_tx
(
  rfqfe_cdma_type *qfe_device,
  rfdevice_script_type *script_data_ptr
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retrieves QFE PA range control table.

  @param *qfe_device
  Pointer to QFE device 

  @param band
  Band class to determine which settings to use.
 
  @param pa_range_controls
  The container on which to place PA range control items.
*/
typedef boolean (*rfqfe_cdma_get_pa_range_ctrl_func_ptr_type)
(
  rfqfe_cdma_type *qfe_device,
  rfm_cdma_band_class_type band,
  qfe_cdma_pa_range_ctrl_type *pa_range_controls
);

/*----------------------------------------------------------------------------*/
boolean rfqfe_cdma_get_pa_range_ctrl_table
(
  rfqfe_cdma_type *qfe_device,
  rfm_cdma_band_class_type band,
  qfe_cdma_pa_range_ctrl_type *pa_range_controls
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configures device with wake-up rx settings.

  @param *qfe_device
  Pointer to QFE device
 
  @param band
  Band class to determine which settings to use.
 
  @param script_data_ptr
  Ptr to the storage which holds generated SBI scripts.
*/
typedef boolean (*rfqfe_cdma_wakeup_rx_ptr_type)
(
  rfqfe_cdma_type *qfe_device,
  rfm_cdma_band_class_type band,
  rfdevice_script_type *script_data_ptr
);

/*----------------------------------------------------------------------------*/
boolean rfqfe_cdma_wakeup_rx
(
  rfqfe_cdma_type *qfe_device,
  rfm_cdma_band_class_type band,
  rfdevice_script_type *script_data_ptr
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configures device with sleep rx settings.

  @param *qfe_device
  Pointer to QFE device 
*/
typedef boolean (*rfqfe_cdma_sleep_rx_ptr_type)
(
  rfqfe_cdma_type *qfe_device,
  rfdevice_script_type *script_data_ptr
);

/*----------------------------------------------------------------------------*/
boolean rfqfe_cdma_sleep_rx
(
  rfqfe_cdma_type *qfe_device,
  rfdevice_script_type *script_data_ptr
);

/*----------------------------------------------------------------------------*/
/*! @brief
    This data structure holds all info pertaining to CDMA QFE interface functions.
*/
typedef struct
{ 
  /*! Generic dispatch */
  rfqfe_cdma_cmd_dispatch_func_ptr_type cmd_dispatch_fp;
  /*! To Wake-up Tx */
  rfqfe_cdma_wakeup_tx_ptr_type wakup_tx_fp;
  /*! To Sleep Tx */
  rfqfe_cdma_sleep_tx_ptr_type sleep_tx_fp;
  /*! Obtain QFE PA range control table */
  rfqfe_cdma_get_pa_range_ctrl_func_ptr_type get_pa_range_ctrl_table_fp;
  /*! To Wake-up Rx */
  rfqfe_cdma_wakeup_rx_ptr_type wakup_rx_fp;
  /*! To Sleep Rx */
  rfqfe_cdma_sleep_rx_ptr_type sleep_rx_fp;
} rfqfe_cdma_func_tbl_type;

/*----------------------------------------------------------------------------*/
/*! @brief
    This data structure holds all info pertaining CDMA QFE device.
*/
struct rfqfe_cdma
{
  /*! Specific QFE defice in use */
  qfe_enum_type device_id;    

  /*! Ptr to table containing interfaces for the device */
  rfqfe_cdma_func_tbl_type *func_tbl;

  /*! Void pointer to device data which can be device specific.*/
  void *device_data;
};

#ifdef __cplusplus
}
#endif

#endif /* QFE_CDMA_INTF_H */


