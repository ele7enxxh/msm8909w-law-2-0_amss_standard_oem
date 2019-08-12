#ifndef RFDEVICE_GSM_TYPE_DEFS_H
#define RFDEVICE_GSM_TYPE_DEFS_H

/*!
  @file
  rfdevice_gsm_intf.h

  @brief
  This is an abstract software interface to control or query the RF
  device for the GSM operation.

  @details
  This is a generic GSM RF device driver interface which consists
  of a collection of data and function pointers for GSM operation. The
  interface is transparent to its clients. In another word, the client
  does not need to know what physical RF devices it's talking to. The dynamic
  binding between the logical radio and the physical radio is done in RFC
  (RF Configuration) which is a combination of an RF Card and an QCT modem. 
 
  RFC knows what specific RF devices are present in an installed RF card.
  It's also aware of which specific RF device is in use for a given GSM
  RF band. Once the connection betwen the logical radio path and the physical
  radio path is determined, RFC will call the explicit RF device driver to
  update the data and function pointers structure.
 
  After the data and function pointer structure is updated, all the device
  driver data and functions within above structure are mapped to an exact RF
  device, thus allowing GSM Main Control, Core and Feature to control or
  query the RF Device.

*/

/*===========================================================================

Copyright (c) 2008 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_gsm_type_defs.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/18/15   ec      Add support for Preload script building in Rx adapter class
03/26/14   sb      Add support for IP2 cal for Dime PM
03/06/14   sc      Remove dependency on rfmeas_sbi_type
11/19/13   dps     Added RFDEVICE_GSM_MAX_TX_SLOTS
08/13/13   sb      GSM RXD changes
08/01/13   sb      Added interface chanegs to support GSM RX diversity
04/24/13   cj      Added rfdevice_gsm_rx_disable_type
01/17/13   cj      Added rfdevice_gsm_rxonly_init_type
10/18/12   lcl     Added support for PLL lock status report
11/16/12   sb      Made changes to convert RF_ON to SSBI  
10/03/12   shb     Added rfdevice_gsm_build_meas_script_v2_type to GSM 
                   function pointer table
08/16/12   shb     Renamed rfdevice_enum_type to rfdevice_id_enum_type
08/07/12   shb     Removed SHIM layer functions from rfdevice_cdma_type_defs.h
                   This file contains data and func table types used by device
08/07/12   shb     Branched rfdevice_cdma_interface.h to 
                   rfdevice_cdma_type_defs.h in rfdevice/interface/api. 
08/02/12   shb     Added rfdevice_gsm_rx/tx_set_band_data
07/30/12   sb      Made burst scripting changes to support SSBI script buffer in RF hal format 
07/16/12   shb     Fixes for compilation after merge with DIME
07/11/12   sr      Added interfaces to push the band-port info device.
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
06/20/12   shb     Updated GSM device enter_mode and set_band APIs to accept 
                   script buffer
05/11/12   sb      Added changes for debug 
03/26/12   av      IRAT code cleanup for W and G. Remove rfdevice_meas.
09/25/11   vb      Support for DAC cal using different Irefs
09/09/11   av      Separate init once and enter_mode api for IRAT cases
08/31/11   vrb     Moving KV calibration algorithm to device driver
07/12/11   vrb     Added critical section locks for RFDevice gsm APIs
07/05/11   vrb     Dual RTR: Added rfgsm_ds as argument to GSM device intf fns
07/01/11   vrb     Replacing direct calls to rfgsm_ds with wrapper functions
04/04/11   bmg     Fixed rfgsmlib_get_kv_cal_code_type to match actual use.
04/01/11   bmg     Added extern "C" for C++ safety.
03/21/11   sr      Added doxygen comments for device library interface.
03/10/11   sr      Moved Tx power related functions to new rfgsm_core_tx_power.c file  
05/12/10   jps     Added tune_rx_direct function pointer  
04/14/10   jps     Changes for GSM Tx SAW/SAW-less support 
02/25/10   sr      made changes to return the uint16 for rfgsmlib_get_env_backoff_type
01/21/10   xw      Lint error fixes  
12/28/09   sr      High/Medium lint errors fix
12/09/09   vrb     Added enum for run-time RTR rev checking to control the 
                   SBI write at end of each burst to turn off the RxPLL
11/30/09   jps     Added function pointer for therm_read 
10/26/08   sr      warnings fix
10/22/08   sr      changed the return datatype to fix warning
10/05/08   xw      Initial version.

============================================================================*/

#include "rfcom.h"
#include "rfdevice_intf_cmd.h"
#include "rfgsm_core_types.h"
//#include "rfcommon_locks.h"
//#include "rfdevice_cmn_intf.h"
#include "rfdevice_cmn_type_defs.h"
#include "rf_buffer_intf.h"
#include "rfc_card_gsm.h"

#ifdef __cplusplus
extern "C" {
#endif


/* The following macros are for KV calibration */
#define RFDEVICE_GSM_KV_CAL_SIGN  0x4000    /* 15 bits 2's complement number */
#define RFDEVICE_GSM_KV_CAL_MASK  0x7FFF    /* 15 bit number */
#define RFDEVICE_GSM_KV_CAL_PER_TX_ON  0x0C     /* Averaging over 12 readings per run */
#define RFDEVICE_GSM_KV_CAL_TX_ON_OFF  0x0A      /* 10 tx on/off run.  Each run will turn the TX on and off */
#define RFDEVICE_GSM_KV_CAL_DELAY      0x01      /* 1 ms delay between each KV read */
#define RFDEVICE_GSM_KV_CAL_SIGN_EXT  0xFFFF8000  /* 15 bit to 32 bit sign extention */
#define RFDEVICE_GSM_KV_CAL_NUM_OF_RERUN_MAX  0x30  /* Max number of re-run*/
#define RFDEVICE_GSM_KV_CAL_MAX_DELTA  0x28      /* Max KV delta from the mean */

#define RFDEVICE_GSM_MAX_TX_SLOTS 5

typedef struct
{
  rfdevice_dac_iref_val iref_val;
  rfcom_gsm_band_type band; 
} rfdevice_gsm_dac_cal_iref_type;

/*----------------------------------------------------------------------------*/
/*! @brief
    This data structure holds all info needed for generating GSM irat script for
    both startup and cleanupo
*/ 
typedef struct 
{
   rfcom_device_enum_type device; 
   /*!<  Device on which GSM will be tuned  */

   rfcom_mode_enum_type   source_tech;
   /*!<  Specify source technology for IRAT */

   uint32                 source_band;
   /*!< Specify source technology band*/

   rfcom_mode_enum_type   target_tech;
   /*!<  Specify target technology for IRAT */

   uint32                 target_band;
   /*!<  Specify target technology for IRAT */

   uint16                 arfcn;
   /*!<  DL Channel on which GSM will be tuned  */

} rfdevice_gsm_meas_param_type;
/*---------------------------------------------------------------------------*/
/*!
  @brief
  Forward declaration of device structure types to use in function pointers
  definitions
*/
typedef struct rfgsm_dev rfdevice_gsm_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure containing the physical device pointers for path0 (TX+PRx)and path1(DRX) for each instance of the device
  
  @details
  Depending upon the number of instances of the device in RFC, we will create an array of this structure.
 
*/

typedef struct
{
  rfdevice_gsm_type* gsm_device_path0;
  rfdevice_gsm_type* gsm_device_path1;

}
rfdevice_gsm_ptr_list_type;

/*----------------------------------------------------------------------------*/

/*!
  @brief
  Enums specifying whether PRX or DRX path configuration for all supported bands on a given WTR/WFR device is onchip or offchip
  
  @details
  For example, in RXD CA config 1 case: PRX is always on WTR1625, DRX high bands on WFR1620 (LNA) + WTR1625 (PLL+down-converter).
  So, PRX is on chip config, DRX is off chip config.
*/

typedef enum
{
  PATH_ON_CHIP_CFG,
  PATH_OFF_CHIP_CFG,
  PATH_INVALID_CFG
}rfdevice_path_config_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  It configure the RF Device for the GSM mode of operation.

  @details
  It configure the RF device for GSM technology mode of operation. It initializes the RF device for
  Rx and Tx.

  @param None.

  @retval TRUE for successful RF device initialization for GSM else FALSE .

  @pre
  SBI clock must be enabled.

  @post
  RF device will be setup for GSM mode of operation.
*/
typedef boolean (*rfgsmlib_init_type)(rfdevice_gsm_type *gsmdev);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  It configure the RF Device for the GSM mode of operation.

  @details
  It configure the RF device for GSM technology mode of operation. It initializes the RF device for
  Rx and Tx.

  @param *gsmdev
  "this" pointer to GSM device whose data is to be extracted
 
  @param execution_type
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script to enter GSM mode
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally

  @return init_success
  Indicates the success of Enter Mode

  @pre
  SBI clock must be enabled.

  @post
  RF device will be setup for GSM mode of operation.
*/
typedef boolean (*rfgsmlib_enter_mode_type)
(
  rfdevice_gsm_type *gsmdev,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  It configure the burst script for the specified type of burst.

  @details
  It configure the burst script for the specified type of burst. This includes setting-up the RF
  device regsiters and setting-up the timing information for each setting in the burst script. 

  @param 'script_infor': burst type information.

  @retval None.

  @pre
  RF device should be configured to for GSM mode of operation.

  @post
  Burst script is setup for the requested burst type.
*/
typedef void (*rfgsmlib_set_sbi_burst_script_type)(rfdevice_gsm_type *gsmdev, rfgsmlib_sbi_burst_struct *script_infor, rf_buffer_intf *buffer);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform the requested generic command opearation on the RF device.

  @details
  Perform the requested generic command opearation on the RF device.

  @param
  'rf_path_enum_type': RF device path.
  'rfdevice_cmd_enum_type': generic command to operate on.
  'void *' : any data associated with the command.
 
  @retval status of the requested command.

  @pre
  RF device should be configured to for GSM mode of operation.

  @post
  Requested command is completed.
*/
typedef int32 (*rfgsmlib_cmd_dispatch_type)(rfdevice_gsm_type *gsmdev, rf_path_enum_type, rfdevice_cmd_enum_type, void *); 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads the thermistor value from the RF Device.

  @details
  This function reads the thermistor value from the RF Device.

  @param None.
 
  @retval Thermister value.

  @pre
  RF device should be configured to for GSM mode of operation.

  @post
  No change.
*/
typedef uint16 (*rfgsmlib_gsm_therm_read_type)(rfdevice_gsm_type *gsmdev);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure the RF device for the specified GSM band of operation.

  @details
  Configure the RF device for the specified GSM band of operation.

  @param band
  GSM band for which RF device need to be configured.
 
  @param execution_type
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script to set tx band
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally
 
  @retval None.

  @pre
  RF device should be configured to for GSM mode of operation.

  @post
  RF device will be setup for the requested GSM band of operation.
*/
typedef void (*rfgsmlib_set_band_type)
( 
  rfdevice_gsm_type *gsmdev, 
  rfcom_gsm_band_type band,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate the burst script with the RF device settings for the specified Rx channel.

  @details
  Populate the burst script with the RF device settings for the specified Rx channel.

  @param 'uint16' : GSm rx channel information.
 
  @retval None.

  @pre
  RF device should be configured to for GSM mode of operation.

  @post
  Burst script will be setup for the specified Rx channel.
*/
typedef void (*rfgsmlib_tune_rx_type)(rfdevice_gsm_type *gsmdev, uint16);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate the burst script with the RF device settings for the specified rx gain range.

  @details
  Populate the burst script with the RF device settings for the specified rx gain range.

  @param 'int32' : GSm rx gain information.
 
  @retval None.

  @pre
  RF device should be configured to for GSM mode of operation.

  @post
  Burst script will be setup for the specified gain.
*/
typedef void (*rfgsmlib_set_rx_gain_range_type)(rfdevice_gsm_type *gsmdev, int32);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate the burst script with the RF device settings for the specified Tx channel.

  @details
  Populate the burst script with the RF device settings for the specified Tx channel.

  @param 'uint16' : GSm Tx channel information.
 
  @retval None.

  @pre
  RF device should be configured to for GSM mode of operation.

  @post
  Burst script will be setup for the specified Tx channel.
*/
typedef void (*rfgsmlib_tune_tx_type)(rfdevice_gsm_type *gsmdev, uint16);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate the burst script with the RF device settings for the specified Tx gain information.

  @details
  Populate the burst script with the RF device settings for the specified Tx gain information.

  @param
  'uint16' : GSm Tx gain information.
  'rfgsm_modulation_type' : type of GSM modulation for which gain settings need to be setup.
  'uint8' : uplink slot number for which gain setting need to be setup.
  'boolean' : Flag to indicate whether it is in Calibration mode or not.
 
  @retval None.

  @pre
  RF device should be configured to for GSM mode of operation.

  @post
  Burst script will be setup for the specified gain information.
*/
typedef void (*rfgsmlib_set_rgi_type)(rfdevice_gsm_type *gsmdev, uint16, 
                                      rfgsm_modulation_type, uint8, boolean);
/*----------------------------------------------------------------------------*/
typedef boolean (*rfgsmlib_dump_ssbi_type)( rfdevice_gsm_type *gsmdev,uint8 );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function enable/disable kv adaptation calibration depending on the flag.

  @details
  This function enable/disable kv adaptation calibration depending on the flag.

  @param
  'boolean' : if 'TRUE' kv cal will be enabled otherwise disabled.
 
  @retval None.

  @pre
  RF device should be configured to for GSM mode of operation.

  @post
  RF device will be setup with KV cal enabled/disabled.
*/
typedef void (*rfgsmlib_enable_kv_cal_type)(rfdevice_gsm_type *gsmdev, boolean);

/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function executes the KV Calibration routine

  @param
  'rfgsmlib_kv_code_type' : Pointer to populate KV code into.
  'uint8 ' : Number of iterations to execute and average KV cal over.
 
*/
typedef boolean (*rfgsmlib_execute_kv_cal_type)(rfdevice_gsm_type *gsmdev, rfgsmlib_kv_code_type *kv_value, uint8 num_iter);

/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function set the kv calibration code based on the value passed in.

  @details
   This function set the kv calibration code based on the value passed in.

  @param
  'uint16' : KV code.
 
  @retval None.

  @pre
  RF device should be setup to enable the KV Cal..

  @post
  RF device is configured with the new KV Code.
*/
typedef void (*rfgsmlib_set_kv_cal_code_type)(rfdevice_gsm_type *gsmdev, uint16 kvcode);

/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function computes the final KV code based on the average KV and 
   acquisition code..

  @details
   This function computes the final KV code based on the average KV and 
   acquisition code.

  @param
  'final_kv' : KV code.
  'final_r1' : acquisition code.
 
  @retval None.

  @pre
  RF device should be setup to enable the KV Cal..

  @post
  No Change.
*/
typedef int16 (*rfgsmlib_compute_final_kv_type)(rfdevice_gsm_type *gsmdev, int16 final_kv, uint16 final_r1);

/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function returns the start time for first transaction sent during a TX burst.

  @details
   This function returns the start time for first transaction sent during a TX burst.

  @param None.
 
  @retval None.

  @pre
  RF device should be setup for GSM.

  @post
  No Change.
*/
typedef int16 (*rfgsmlib_get_tx_sbi_start_delta_type)(rfdevice_gsm_type *gsmdev);

/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function turns on/off Phase modulation in RF Device.

  @details
   This function turns on/off Phase modulation in RF Device.

  @param
  'on_off' : TRUE - Phase modulation is ON, else OFF.
 
  @retval None.

  @pre
  RF device should be setup for GSM.

  @post
  No Change.
*/
typedef void (*rfgsmlib_set_tx_phase_const_type) (rfdevice_gsm_type *gsmdev, boolean on_off, rf_buffer_intf *buffer);

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
typedef rfcom_gsm_band_type (*rfgsmlib_gsm_prog_cal_tx_burst_sbi_type)(rfdevice_gsm_type *gsmdev,uint16,int8, rf_buffer_intf *buffer);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The function generates SBI to switch mode to GSM. It is used for XtoG
  startup and GtoX cleanup

  @details
  The function generates SBI to switch mode to GSM. It is used for XtoG
  startup and GtoX cleanup
  
  @param *rfgsmds
  Pointer to GSM device that needs to be programmed

  @param gsm_meas_param
  Parameters needed to generate the script
 
  @param *script
  Pointer to script to hold the startup/cleanup scripts
 
  @param *current_size
  Holds the current size of the script so that the script can be appended from *current_size,
  this function also updates *current_size after the scripts is updated
 
  @param max_script_size
  Max script size that *script can hold
 
*/
typedef void (*rfdevice_gsm_build_meas_script_type) 
(
  rfdevice_gsm_type *rfgsmds, 
  rfdevice_gsm_meas_param_type gsm_meas_param, 
  void *script,
  uint16 *current_size,
  uint16 max_script_size
);
/*----------------------------------------------------------------------------*/

typedef void (*rfdevice_gsm_setup_tx_trans_script_type) 
(
  rfdevice_gsm_type* gsmdev,
  rf_buffer_intf* script_buffer
);
/*----------------------------------------------------------------------------*/

typedef void (*rfdevice_gsm_setup_tx_stop_script_type) 
(
  rfdevice_gsm_type* gsmdev,
  rf_buffer_intf* script_buffer
);
/*----------------------------------------------------------------------------*/
typedef void (*rfdevice_gsm_setup_rx_stop_script_type) 
(
  rfdevice_gsm_type* gsmdev,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
typedef void (*rfdevice_set_start_script_type)
(
  rfdevice_gsm_type *gsmdev,
  rfgsmlib_sbi_burst_struct *script_infor,
  rf_buffer_intf *buffer
);
/*----------------------------------------------------------------------------*/
typedef rfcom_gsm_band_type (*rfdevice_gsm_prog_cal_tx_start_script_type)
(
  rfdevice_gsm_type *gsmdev,
  uint16,
  int8, 
  rf_buffer_intf *buffer
);
/*----------------------------------------------------------------------------*/

typedef uint16 (*rfdevice_gsm_get_therm_read_type)
(
 rfdevice_gsm_type *gsmdev,
 rf_buffer_intf *buffer
);
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configurae the RF device for the specified GSM Band and channel.

  @details
  Configurae the RF device for the specified GSM Band and channel.

  @param
  'rfcom_gsm_band_type' : GSM band.
  'uint16' : GSM channel information.
 
  @retval N.

  @pre
  RF device should be setup for GSM.

  @post
  RF device should be setup for the specified GSM band.
*/
typedef void (*rfgsmlib_tune_rx_direct_type) (rfdevice_gsm_type *gsmdev, 
                                              rfcom_gsm_band_type, uint16, boolean);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  The function sets the rfc_gsm_core_ptr to point to the right data
   structure, containing, mon/burst timing info, etc.

  @details
  The function sets the rfc_gsm_core_ptr to point to the right data
   structure, containing, mon/burst timing info, etc.
  
  @param *rfgsmds
  Pointer to GSM device that needs to be programmed

  @param *rfc_gsm_core_ptr
  ptr to the rfc data structure which has the timing info.
*/
typedef void (*rfdevice_gsm_set_rfc_gsm_core_ptr_type) (rfdevice_gsm_type *gsmdev,
                                                        rfc_gsm_core_config_type *rfc_gsm_core_ptr);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Configures rf gsm device to operate on the specified band/port 
   (rx_band_port) for the given band (rfcom_band)
   
   @details
   Ensures rfm_band and tx_band_port are valid and updates instance data to
   store this band/port info for future use
 
   @param *gsmdev
   GSM device to be updated

   @param rfcom_band
   rfcom_band whose port is being set
 
   @param rx_band_port
   rx band/port enum for the rfdevice.
 
   @param tx_band_port
   tx band/port enum for the rfdevice
*/
typedef void (*rfdevice_gsm_set_band_port_type) (rfdevice_gsm_type *gsmdev,
                                                 rfcom_gsm_band_type rfcom_band,
                                                 uint8 rx_band_port,
                                                 uint8 tx_band_port);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Push band specific data to device from RFC
   
   @param *gsmdev
   GSM device to be updated

   @param rfcom_band
   rfcom_band whose port is being set
 
   @param data_array
   Array of int32 containing device Rx data for band
   
   @param array_size
   Size of array of int32 data to be pushed to device
*/
typedef boolean (*rfdevice_gsm_set_band_data_type) 
(
  rfdevice_gsm_type* gsmdev,
  rfcom_gsm_band_type band,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generates GSM RX preload script in rf_buffer_intf format

  @details
  For use by RUMI builds. Script is populated in to the rf_buffer_intf* that
  is passed in. Essentially a GSM enter mode script
 
  @param gsmdev
  "this" pointer to GSM device
 
  @param gsm_meas_param
  Measurement related parameters that may be used to build script
 
  @param buf
  rf_buffer where script will be populated
*/
typedef boolean (*rfdevice_gsm_build_preload_script_type)
(
  rfdevice_gsm_type *gsmdev,
  rfdevice_gsm_meas_param_type gsm_meas_param,
  rf_buffer_intf* buf
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generates GSM RX measurement script in rf_buffer_intf format

  @details
  For use by DIME builds. Script is populated in to the rf_buffer_intf* that
  is passed in. Essentially a GSM enter mode script
 
  @param gsmdev
  "this" pointer to GSM device
 
  @param gsm_meas_param
  Measurement related parameters that may be used to build script
 
  @param buf
  rf_buffer where script will be populated
*/
typedef boolean (*rfdevice_gsm_build_meas_script_v2_type)
(
  rfdevice_gsm_type *gsmdev,
  rfdevice_gsm_meas_param_type gsm_meas_param,
  rf_buffer_intf* buf
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  It configure the RF Device for the GSM mode of operation.

  @details
  It configure the RF device for GSM technology mode of operation. It initializes the RF device for
  Rx and Tx.

  @param *gsmdev
  "this" pointer to GSM device whose data is to be extracted
 
  @param execution_type
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script to enter GSM mode
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally

  @return init_success
  Indicates the success of Enter Mode

  @pre
  SBI clock must be enabled.

  @post
  RF device will be setup for GSM mode of operation.
*/
typedef boolean (*rfgsmlib_tx_only_enter_mode_type)
(
  rfdevice_gsm_type *gsmdev,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);
/*----------------------------------------------------------------------------*/
/*!
  @brief
   Function definition for device driver interface to get GSM PLL lock status.

   @param *gsmdev
   GSM device to be updated

   @param rf_ibuf
   Buffer to be filled in by device driver.
 
   @param execution_type
   Create script or send out the transactions
*/
typedef boolean (*rfdevice_gsm_get_pll_lock_seq_type) (rfdevice_gsm_type *gsmdev,
                                                       rf_buffer_intf *rf_ibuf,
                                                       rf_device_execution_type execution_type
                                                       );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configures RF Device corresponding to target WTR for GSM mode of operation (only Rx).

  @param *gsmdev
  "this" pointer to GSM device whose data is to be extracted
 
  @param execution_type
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script to enter GSM mode
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally

  @return init_success
  Indicates the success of Enter Mode

  @pre
  SBI clock must be enabled.

  @post
  RF device will be setup for GSM mode of operation.
*/
typedef boolean (*rfdevice_gsm_rxonly_init_type)(  rfdevice_gsm_type *gsmdev,
											rf_device_execution_type execution_type,
											rf_buffer_intf* script_buffer);


/*----------------------------------------------------------------------------*/
/*!
  @brief
 Disable rx.

  @param *gsmdev
  "this" pointer to GSM device whose data is to be extracted
 
  @param execution_type
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script to enter GSM mode
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally

  @return init_success
  Indicates the success of Enter Mode

  @pre
  SBI clock must be enabled.

  @post
  rx will be disabled on WTR.
*/
typedef boolean (*rfdevice_gsm_rx_disable_type)(  rfdevice_gsm_type *gsmdev,
											rf_device_execution_type execution_type,
											rf_buffer_intf* script_buffer);
/*----------------------------------------------------------------------------*/



typedef boolean (*rfdevice_gsm_setup_ip2_cal)(  rfdevice_gsm_type *gsmdev,
                                                rf_device_execution_type execution_type,
                                                rf_buffer_intf* script_buffer,
                                                uint8 therm_bin_idx,
                                                int32 min_im2_step_idx);

typedef boolean (*rfdevice_gsm_cleanup_ip2_cal)(  rfdevice_gsm_type *gsmdev,
                                                rf_device_execution_type execution_type,
                                                rf_buffer_intf* script_buffer
                                               );

typedef boolean (*rfdevice_gsm_populate_ip2_cal_step)(  rfdevice_gsm_type *gsmdev,
                                                rf_device_execution_type execution_type,
                                                rf_buffer_intf* script_buffer,
                                                uint8 cal_step_idx
                                               );


/*----------------------------------------------------------------------------*/







/*!
  @brief
  Structure of GSM interface function pointers
*/

typedef struct
{
  //called in rfgsm_core:
  rfgsmlib_init_type  init_once_fp;
  rfgsmlib_enter_mode_type  enter_mode_fp;
  rfgsmlib_set_sbi_burst_script_type  set_sbi_burst_script_fp;
  rfgsmlib_cmd_dispatch_type cmd_dispatch_fp;
  rfgsmlib_gsm_therm_read_type therm_read_fp;
  rfgsmlib_tx_only_enter_mode_type  enter_mode_tx_only_fp;

  //called in rfgsm_core_rx:
  rfgsmlib_set_band_type  set_rx_band_fp;
  rfgsmlib_tune_rx_type  tune_rx_fp;
  rfgsmlib_set_rx_gain_range_type  set_rx_gain_range_fp;

  //called in rfgsm_core_rx:
  rfgsmlib_set_band_type  set_tx_band_fp;
  rfgsmlib_tune_tx_type  tune_tx_fp;
  rfgsmlib_set_rgi_type  set_rgi_fp;

  // KV cal support for FTM
  rfgsmlib_enable_kv_cal_type enable_kv_cal_fp;
  rfgsmlib_execute_kv_cal_type execute_kv_cal_fp;
  rfgsmlib_set_kv_cal_code_type set_kv_cal_code_fp;
  rfgsmlib_compute_final_kv_type compute_final_kv_cal_fp;

  // FTM support
  rfgsmlib_get_tx_sbi_start_delta_type get_tx_sbi_start_delta_fp;
  rfgsmlib_set_tx_phase_const_type set_tx_phase_const_fp;
  rfgsmlib_gsm_prog_cal_tx_burst_sbi_type prog_cal_tx_burst_sbi_fp;

  //Tune RX direct
  rfgsmlib_tune_rx_direct_type tune_rx_direct_fp;

  //Dump SSBIs
  rfgsmlib_dump_ssbi_type dump_ssbi_fp;

  //Build tune to GSM scripts
  rfdevice_gsm_build_meas_script_type build_meas_script_fp;

  // set rfc core ptr
  rfdevice_gsm_set_rfc_gsm_core_ptr_type set_rfc_gsm_core_ptr_fp;

  // set band port info
  rfdevice_gsm_set_band_port_type set_band_port_fp;

  // set rx band data
  rfdevice_gsm_set_band_data_type set_rx_band_data_fp;

  // set tx band data
  rfdevice_gsm_set_band_data_type set_tx_band_data_fp;

  // build meas script in rf_buffer_intf format
  rfdevice_gsm_build_meas_script_v2_type build_meas_script_v2_fp;

  // build preload script in rf_buffer_intf format
  rfdevice_gsm_build_preload_script_type build_preload_script_fp;

  /* get the PLL lock status read sequence */
  rfdevice_gsm_get_pll_lock_seq_type get_pll_lock_seq_fp;
  //build tx transition scripts
  rfdevice_gsm_setup_tx_trans_script_type setup_tx_trans_script_fp;

  //build tx stop scripts
  rfdevice_gsm_setup_tx_stop_script_type setup_tx_stop_script_fp;

  //build rx stop scripts
  rfdevice_gsm_setup_rx_stop_script_type setup_rx_stop_script_fp;

  //build tx/rx start scripts
  rfdevice_set_start_script_type setup_start_script_fp;

  //build Tx cal script
  rfdevice_gsm_prog_cal_tx_start_script_type setup_tx_cal_start_script_fp;

  //get thermistor reading
  rfdevice_gsm_get_therm_read_type get_therm_read_fp;

  //init second WTR for RSSI measurements
  rfdevice_gsm_rxonly_init_type rxonly_init_fp;

  //Disable rx
  rfdevice_gsm_rx_disable_type  rx_disable_fp;


  // setup IP2 cal
  rfdevice_gsm_setup_ip2_cal  setup_ip2_cal_fp;

  // setup IP2 cal step
  rfdevice_gsm_populate_ip2_cal_step populate_ip2_cal_step_fp;

  // cleaup IP2 cal
  rfdevice_gsm_cleanup_ip2_cal cleanup_ip2_cal_fp;


} rfdevice_gsm_func_tbl_type;


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
struct rfgsm_dev
{
  rf_lock_data_type *critical_sect;

  const rfdevice_id_enum_type device_id;   /*!< RF device being used for rx */

  const rfdevice_gsm_func_tbl_type* const func_tbl;

  void* const data; /*!< Void pointer to device data which can be device specific */  
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfgsm_ds: GSM specific RF device structure interface
*/
extern rfdevice_gsm_type *rfgsm_ds;

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_GSM_TYPE_DEFS_H */
