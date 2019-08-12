#ifndef QFE_GSM_INTF_H
#define QFE_GSM_INTF_H


/*!
  @file
  qfe_gsm_intf.h

  @brief
  This is an abstract software interface to control or query the QFE
  device for the GSM operation.

  @details
  This is a generic GSM QFE device driver interface which consists
  of a collection of data and function pointers for GSM operation. The
  interface is transparent to its clients. In another word, the client
  does not need to know what physical QFE devices it's talking to. 
 
  RFC knows what specific QFE devices are present in an installed RF card.
  It's also aware of which specific QFE device is in use for a given GSM
  RF band. Once the connection betwen the logical radio path and the physical
  radio path is determined, RFC will call the explicit QFE device driver to
  update the data and function pointers structure.
 
  After the data and function pointer structure is updated, all the device
  driver data and functions within above structure are mapped to an exact RF
  device, thus allowing GSM Main Control, Core and Feature to control or
  query the QFE Device.

*/


/*===========================================================================

Copyright (c) 2009, 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/qfe_intf/inc/qfe_gsm_intf.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/17/12   sn      Function rename
12/17/11   kg      Using "qfe_intf_cmd.h" 
09/13/11   kg      Initial version.

============================================================================*/

#include "rfcom.h"
#include "qfe_intf_cmd.h"
#include "rfgsm_core_types.h"
#include "rfcommon_locks.h"
#include "rfdevice_gsm_intf.h"
#include "qfe_cmn_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rfgsm_qfe qfe_gsm_type;




typedef boolean (*qfegsmlib_init_once_type)(qfe_gsm_type *gsmdev);
void qfe_gsm_init_once(void);

typedef void (*qfegsmlib_set_sbi_burst_script_type)(qfe_gsm_type *gsmdev, rfgsmlib_sbi_burst_struct *script_infor);
void qfe_gsm_set_sbi_burst_script(rfgsmlib_sbi_burst_struct *script_infor);

typedef int32 (*qfegsmlib_cmd_dispatch_type)(qfe_gsm_type *gsmdev, rf_path_enum_type path, qfe_cmd_enum_type cmd, void *data); 
int32 qfe_gsm_cmd_dispatch(rf_path_enum_type path, qfe_cmd_enum_type cmd, void *data );

typedef void (*qfegsmlib_set_band_type)( qfe_gsm_type *gsmdev, rfcom_gsm_band_type band);
void qfe_gsm_set_tx_band(rfcom_gsm_band_type band);

typedef void (*qfegsmlib_set_tx_pwr_type)(qfe_gsm_type *gsmdev,uint8 pa_range,rfgsm_modulation_type mod_type,
                          uint8 slot_num,uint8 set_script);
void qfe_gsm_set_pa_range(uint8 pa_range,rfgsm_modulation_type mod_type,
                          uint8 slot_num,uint8 set_script);

typedef void (*qfegsmlib_set_rx_band_type)( qfe_gsm_type *gsmdev, rfcom_gsm_band_type band);
void qfe_gsm_set_rx_band(rfcom_gsm_band_type band);

typedef void (*qfegsmlib_tx_setup_burst_script_type)(qfe_gsm_type *gsmdev, rfgsmlib_sbi_burst_struct *script_infor);
void qfe_gsm_tx_setup_burst_script(rfgsmlib_sbi_burst_struct *script_infor);
	
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function programs the RF device for trasmit operation during calibration.

  @details
  This function programs the RF device for trasmit operation during calibration.

  @param
  'uint16' : GSM channel information.
 
  @retval 'rfcom_gsm_band_type' : GSM band.

  @pre
  RF device should be setup for GSM.

  @post
  RF device will be setup for Tx cal operation.
*/

typedef struct
{
  //called in rfgsm_core:
  qfegsmlib_init_once_type  init_once_fp;
  qfegsmlib_set_sbi_burst_script_type  set_sbi_burst_script_fp;
  qfegsmlib_cmd_dispatch_type cmd_dispatch_fp;

  //called in rfgsm_core_tx:
  qfegsmlib_set_band_type  set_tx_band_fp;
  qfegsmlib_set_tx_pwr_type  set_tx_pwr_fp;

  //called in rfgsm_core_rx: not defined
   qfegsmlib_set_rx_band_type  set_rx_band_fp;

 qfegsmlib_tx_setup_burst_script_type tx_setup_burst_script_fp;
  
} qfe_gsm_func_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF Device GSM related interface type.

  @details
  A RF Device independent interface will ve provided to GSM Main Control, Core
  and Feature modules through these functions pointers. These function pointers
  will be initialized by the specific RF device whenever there is change in
  band.
*/
struct rfgsm_qfe
{
  rf_lock_data_type *critical_sect;

  const qfe_enum_type device_id;   /*!< RF device being used for rx */

  const qfe_gsm_func_tbl_type* const func_tbl;

  void* const data; /*!< Void pointer to device data which can be device specific */  
};


/*----------------------------------------------------------------------------*/
extern qfe_gsm_type *rfgsm_qfe_ds;

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_GSM_INTF_H */