#ifndef QFE_WCDMA_INTF_H
#define QFE_WCDMA_INTF_H

/*!
  @file
  qfe_wcdma_intf.h

  @brief
  This is an abstract software interface to control or query the RF
  device for the WCDMA operation.

  @details
  This is a generic WCDMA QFE device driver interface which consists
  of a collection of data and function pointers for WCDMA operation. The
  interface is transparent to its clients. In another word, the client
  does not need to know what physical QFE devices it's talking to. 
 
  RFC knows what specific QFE devices are present in an installed RF card.
  It's also aware of which specific QFE device is in use for a given WCDMA
  RF band. Once the connection betwen the logical radio path and the physical
  radio path is determined, RFC will call the explicit QFE device driver to
  update the data and function pointers structure.
 
  After the data and function pointer structure is updated, all the device
  driver data and functions within above structure are mapped to an exact RF
  device, thus allowing WCDMA Main Control, Core and Feature to control or
  query the QFE Device.

*/


/*===========================================================================

Copyright (c) 2009, 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/qfe_intf/inc/qfe_wcdma_intf.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/17/12   sn      Added function argument
03/28/12   sn      Added critical_sect lock in rfwcdma_qfedev type
02/01/12   sn      Added two function pointer types for IRAT measurements
12/17/11   kg      Using "qfe_intf_cmd.h" 
01/11/12   kg      Changing qfe1320_wcdma_cmd_dispatch to void
12/06/11   kg      Rename function qfe_wcdma_get_tx_agc_lut_table
09/13/11   kg      Initial version.

============================================================================*/

#include "rfcom.h"
#include "qfe_intf_cmd.h"
#include "rfcommon_locks.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct
  {
    uint16 reg;
    uint16 data[4]; /* TODO:remove hardcoding */
  }qfe_wcdma_tx_agc_lut_type;

  typedef struct rfwcdma_qfedev rfwcdma_qfe_type;


  typedef void (*qfe_wcdma_init_ptr_type)
  (
  rfwcdma_qfe_type *txdev,
  rf_path_enum_type path
  );
  void qfe_wcdma_init(rf_path_enum_type path );


  typedef void (*qfe_wcdma_enter_mode_ptr_type)
  (
  rfwcdma_qfe_type *txdev,
  rfcom_mode_enum_type mode
  );

  void qfe_wcdma_enter_mode(rfcom_mode_enum_type mode);

  typedef void (*qfe_wcdma_cmd_dispatch_func_ptr_type)
  (
  rfwcdma_qfe_type *padev,
  rf_path_enum_type path,
  qfe_cmd_enum_type cmd,
  void *data
  );
  void qfe_wcdma_cmd_dispatch( rf_path_enum_type path, 
                               qfe_cmd_enum_type cmd, 
                               void *data );

  typedef void (*qfe_wcdma_set_band_ptr_type)
  (
  rfwcdma_qfe_type *padev,
  rfcom_wcdma_band_type band
  );
  void qfe_wcdma_set_band(rfcom_wcdma_band_type band );


  typedef void (*qfe_wcdma_enable_func_ptr_type)
  (
  rfwcdma_qfe_type *padev,
  boolean enable
  );
  void qfe_wcdma_pa_enable( boolean enable );


  typedef void (*qfe_wcdma_get_lut_table_func_ptr_type)
  (
  rfwcdma_qfe_type *padev,
  rfcom_wcdma_band_type band,
  qfe_wcdma_tx_agc_lut_type *lut_table
  );
  void qfe_wcdma_get_tx_agc_lut_table(rfcom_wcdma_band_type band, qfe_wcdma_tx_agc_lut_type *lut_table );

  typedef void (*qfe_wcdma_set_range_func_ptr_type)
  (
  rfwcdma_qfe_type *padev,
  uint8 range,
  rfcom_wcdma_band_type band
  );
  void qfe_wcdma_set_pa_range( uint8 range,rfcom_wcdma_band_type band );

  typedef void (* qfe_wcdma_build_irat_script_type)
  (
    rfwcdma_qfe_type *padev, 
    qfe_meas_cmd_type* qfe_meas_cmd_ptr
  );

  void qfe_wcdma_build_irat_script(qfe_meas_cmd_type* qfe_meas_cmd_ptr);


  typedef struct
  {
    qfe_wcdma_init_ptr_type init_fp; 
    qfe_wcdma_enter_mode_ptr_type enter_mode_fp;
    qfe_wcdma_set_band_ptr_type  set_band_fp;
    qfe_wcdma_enable_func_ptr_type  enable_fp;
    qfe_wcdma_get_lut_table_func_ptr_type get_agc_lut_table_fp;
    qfe_wcdma_cmd_dispatch_func_ptr_type cmd_dispatch_fp;
    qfe_wcdma_set_range_func_ptr_type set_range_fp;
    qfe_wcdma_build_irat_script_type build_irat_script_fp;  
  } rfwcdma_qfe_func_tbl_type;


  struct rfwcdma_qfedev
  {
    rf_lock_data_type *critical_sect;

    qfe_enum_type device_id;    /*!< RF device being used for rx  */

    rfwcdma_qfe_func_tbl_type *func_tbl;

    void *data; /*!< Void pointer to device data which can be device specific . TODO : Typecast it */
  };
  extern rfwcdma_qfe_type *rfwcdma_qfe;

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_WCDMA_INTF_H */


