#ifndef RFC_CARD_CDMA_H
#define RFC_CARD_CDMA_H
/*!
  @file
  rfc_card_cdma.h

  @brief
  <b>Card-specific developers should refer to this interface.</b> This file
  defines the base Class for CDMA, needed to implement RFC for specific RF card 

  @addtogroup RF_RFC_CDMA_BASE_RFCARD_INTF
  @{
*/
/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rfc_card_cdma.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/16/14   spa     Add BC config to the interface
08/25/14   spa     Add num_grfc pointer as arg to generate_tx_on_off_grfc_script 
07/17/14   spa     Added api update_rx_path in class rfc_cdma_card_intf
05/05/14   spa     Check for max WAN devices in rfc_cdma_is_rfm_device_used
05/02/14   spa     Propagate CDMA RFC failure up to RFM init
04/11/14   APU     Enabled AsDIV
04/07/14   APU     Added support for RFM_DEVICES 2 and 3 even when RFM_DEVICES 
                   0 & 1 are absent for CDMA.
11/11/13   APU     Removed hardcoding for RFM_DEVICE_2 to be a Tx device.
10/31/13   shb     Converted TRX common device interface to C++
09/12/13   sar     Fixed compiler warnings.
08/26/13   shb     Updates for new C++ transceiver device interface
07/18/13   zhw     Add AsDiv Support for setting GRFC switches
06/30/13   yzw     Add method get_rffe_device_info() to get RFFE device information
06/06/13   JJ      Clean redundant RFC APIs and create API get_device_objects
05/29/13   spa     Added delay param to APIs for getting GRFC scripts
03/13/13   sn      Added XPT functions
02/13/13   bmg     Refactored QTuner support for generalized antenna tuner
                   interface.
12/19/12   zhw     CDMA HDET support
12/11/12   hdz     Enabled timing support for generate_pa_on_off_grfc_script
                   and generate_tx_on_off_grfc_script
12/04/12   zhw     API for checking init columns in RFC for CDMA Rx/Tx 
11/30/12   adk     Dime RFC updates to get hdet device instance
11/19/12   adk     Dime RFC updates to get tuner device instance
11/06/12   spa     Removed refs to unused enums pa/asm bus type and com type
11/05/12   zhw     Support for PA On/Off GRFC script in RF On/Off event
10/29/12   zhw     Support for query device_config from RFC and device driver
10/23/12   zhw     Remove hardcoding in get_template_id()
10/16/12   zhw     RF On/Off grfc scripting using overwritten rfc signal
09/24/12   spa     Removed hard coding of max devices
09/19/12   spa     Added API create_device_instance for all RF cards
09/18/12   spa     Added APIs to populate band agnostic device RxTx items
09/14/12   spa     Added generic APIs to set Tx and Rx devices
08/28/12   spa     Added implementation to get QPOET object
08/25/12   spa     Added implementation to get pa/asm object and set port map 
08/08/12   spa     Added API to set band port information in base class 
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
07/09/12   spa     Renamed grfc_buf_ptr to script_buf_ptr 
07/06/12   spa     All generate_grfc_script APIs now accept void pointer to 
                   script buffer and no longer have 'is_irat' flag 
05/23/12   sty     Deleted manage_vregs()
                   Added new method get_rf_chip_num()
05/18/12   sr      RFC-EFS code clean-up changes.
05/14/12   zw      Removed unused APIs
05/09/12   zw      Added GRFC scripting generation APIs
05/11/12   sty     Deleted redundant variables, added doxygen comments
05/08/12   zw      Fixed KW Warning
05/05/12   sty     Added support for EFS-RFC
05/03/12   sb      Fixed KW warning
04/05/12   sr      made changes to put all the GPIOs into low power mode during sleep.  
03/07/12   sty     Moved construtor to protected and added default constructor 
                   as private
01/24/12   aro     Overloaded Get Rx warmup function to query enhanced
                   RF Warmup time
12/12/11   sty     Deleted get_recos_for_device as it is redundant
11/29/11   sty     Added API - get_recos_for_device()
09/12/11   sty     Added new method - is_device_tuneable
                   Added rf_warmup_time_in_us in protected section of 
                   rfc_cdma_card_intf
07/20/11   sty     Changed return type for get_tx_lut_table_name
07/13/11   sty     Documentation update 
07/11/11   cd      Separated GPIO/GRFC config functions for sleep and wakeup
07/07/11   sty     Added get_tx_lut_table_name() API
06/29/11   cd      New card interfaces to configure GPIO/GRFC signals using 
                   autogen settings 
05/31/11   aro     Added logical device internal parameter
05/25/11   aro     [1] Changed RFC data structure to be RX and Tx Path dependent
                   [2] Moved dev_param to derived class from base class
05/21/11   aro     Changed RFC interface to handle device parameter
05/21/11   aro     Added interface to return Logical Device parameter
04/28/11   sty     Changed signature of manage_vregs()
03/04/11   aro     Renamed RFC config structure name
02/25/11   sty     Deleted redundant get_tx_path method
02/18/11   sty     Renamed getinstance to get_instance
02/17/11   sty     Renamed rfc_card_cdma.h to rfc_cdma.h
01/25/11   sty     Updated doxygen comments
01/25/11   sty     Fixed stray character which caused compile error. Changed
                   doxy comment
01/25/11   sty     Added copy_grfc_table() utility
01/20/11   sty     Renamed rfc_cdma_card_interface to rfc_cdma_card_intf 
01/19/11   sty     Replaced ptrQ with getinstance
01/10/11   sty     updated comments
01/10/11   sty     Deleted get_mode_config_data method
                   Modified method rx_band_config_get and tx_band_config_get
01/08/11   aro     Changed @ingroup to @addtogroup
01/06/11   sty     Minor update to comments
01/06/11   sty     Renamed rfc_cdma to rfc_cdma_card_intf
01/06/11   sty     Use rfc_card_cdma.h
01/06/11   sty     Updated doxygen comments
01/05/11   sty     Added manage and config VREG methods
01/05/11   sty     Doxygen changes
01/05/11   sty     Deleted config_mode from base class definition
                   Changed return type of command_dispatch to boolean
01/04/11   sty     Added virtual destructor for rfc_cdma, deleted redundant
                   header file
01/04/11   sty     Removed redundant "extern C"
12/20/10   sty     Initial version

============================================================================*/

#ifdef __cplusplus
extern "C" 
{
  #include "rfc_pm.h"
#endif

#include "rfa.h"
#include "rfc_cdma.h"
#include "rfc_common.h"
#include "rfdevice_class.h"

/*----------------------------------------------------------------------------*/
/*! @brief 
    Define equivalent to convert uint8 into rfm_device_enum_type.
  */
inline rfm_device_enum_type 
GET_RFM_DEVICE( uint8 x ) { return (rfm_device_enum_type)(x) ; }

/*! @brief 
    Define equivalent to return MAX paths supported by CDMA
    CDMA Driver supports Paths 0 through 3 currently */
const rf_path_enum_type RF_CDMA_MAX_PATH_SUPPORTED = RF_PATH_3 ;

/*!
  @name Base class definition for RFC CDMA

  @brief This is the base class to be used by all RFC cards for CDMA
  
*/
/*! @{ */

/*!
  @brief
  Class definition for the RFC base class
*/
class rfc_cdma_card_intf : public rfa
{
public:

  /*! Method that creates the RFC class and returns a pointer to it */ 
  static rfc_cdma_card_intf * 
    create_device_instance(  /*!prep time in us*/
                             uint32 prep_time_in_us,
                             /*!exec time in us*/ 
                             uint32 exec_time_in_us,
                             /*! api success/failure */
                             boolean *card_init_status );  


  /*! Method that returns a pointer to the RFC class that has been instantiated
      when register_rfc() was called */ 
  static rfc_cdma_card_intf * get_instance( void );  

  /*! method to retrieve RF WU time */
  int32 get_warmup_time( rfm_device_enum_type device/*!< RF device */ );

  /*! method to retrieve Enhanced RF WU time */
  boolean get_warmup_time( rfm_device_enum_type device,
                           /*!< RF device */ 
                           rfm_mode_enum_type rf_mode,
                           /*!< RF Mode */ 
                           void * const rf_wu_data
                           /*!< RF Warm Up Data */ );

  /*! method to retrieve ability of RF card to tune based on device and
      band */
  boolean is_device_tuneable(rfm_device_enum_type device,
                                   /*!< RF device */ 
                                   rfm_cdma_band_class_type band
                                   /*!< RF band */ );

  /*! method to create and setup device interface for CDMA */
  boolean set_cdma_device_and_interface( boolean reg_this
                                         /*!instantiate class*/
                                        );

  /*! method to get pointers to device objects */
  boolean get_device_objects( rfm_device_enum_type dev_type,
                           rfc_cdma_logical_device_params_type* logical_dev_ptr,
                           uint8 alt_path,
                           uint64 bc_config
                          );


  /*! method to create and setup Rx device interface for CDMA */
  boolean set_cdma_rx_device( rfc_cfg_params_type band_path_cfg
                              /*!Band path configuration*/,
                              uint64 bc_config
                             );

  /*! method to create and setup Tx device interface for CDMA */
  boolean set_cdma_tx_device( rfc_cfg_params_type band_path_cfg,
                              /*!Band path configuration*/
                              uint64 bc_config
                             );

  /*! method to set Band Agnostic Rx information for CDMA*/
  boolean set_rx_band_agnostic_info( rfm_device_enum_type logical_device,
                                     /*!Logical device*/
                                     rfc_device_info_type *device_info_ptr
                                     /*!device info for this configuration*/
                                   );

  /*! method to set Band Agnostic Tx information for CDMA*/
  boolean set_tx_band_agnostic_info( rfm_device_enum_type logical_device,
                                     /*!Logical device*/
                                     rfc_device_info_type *device_info_ptr
                                     /*!device info for this configuration*/
                                   );

  /*! method to retrieve supported sub-classes for a given device */
  uint32 get_sub_class_for_band(rfm_device_enum_type device,
                                   /*!< RF device */ 
                                   rfm_cdma_band_class_type band
                                   /*!< RF band */ );

  /*! method to retrieve table_enum based on band and pa-state */
  boolean get_tx_lut_table_name(   
                            /*! RF mode for which LUT query is made */
                            rfm_mode_enum_type const mode,
                            /*! RF device on which the operation is to be done*/
                            rfm_device_enum_type const device,
                            /*! RF band for which LUT is to be retrieved */
                            rfm_cdma_band_class_type const band,
                            /*! PA State for which LUT is to be retrieved */
                            rfm_pa_gain_state_type const pa_state,
                            /*! pointer to table-name enum */
                            int* table_name_ptr,
                            /*! pointer to LUT type enum */
                            rfdevice_cdma_lut_type* table_type );

  /*! method to retrieve Logical Device Parameter */
  rfc_cdma_logical_device_params_type* get_logical_dev_param(
                                                     rfm_device_enum_type device
                                                     /*!< RF device */ );  

  /*! method to generate Rx Wakeup grfc Script */
  boolean generate_rx_wakeup_grfc_script ( 
                                            rfm_device_enum_type device,
                                            /*!< RF device */ 
                                            rfm_cdma_band_class_type band, 
                                            /*!< RF band */ 
                                            rf_buffer_intf *script_buf_ptr,
                                           /*!< output script that contains required settings */ 
                                            int16 delay
                                           /*!< delay in us of the first transaction */ 
                                            );

  /*! method to generate Rx Sleep grfc Script */
  boolean generate_rx_sleep_grfc_script ( 
                                  rfm_device_enum_type device, 
                                  /*!< RF device */ 
                                  rfm_cdma_band_class_type band, 
                                  /*!< RF band */ 
                                  rf_buffer_intf *script_buf_ptr,
                                 /*!< output script that contains required settings */ 
                                  int16 delay
                                 /*!< delay in us of the first transaction */ 
                                  );

  /*! method to generate Tx Wakeup grfc Script */
  boolean generate_tx_wakeup_grfc_script ( 
                                   rfm_device_enum_type device,
                                   /*!< RF device */ 
                                   rfm_cdma_band_class_type band, 
                                   /*!< RF band */ 
                                   rf_buffer_intf *script_buf_ptr,
                                  /*!< output script that contains required settings */ 
                                   int16 delay
                                  /*!< delay in us of the first transaction */ 
                                   );

  /*! method to generate Tx Sleep grfc Script */
  boolean generate_tx_sleep_grfc_script ( 
                                  rfm_device_enum_type device,
                                  /*!< RF device */ 
                                  rfm_cdma_band_class_type band, 
                                  /*!< RF band */ 
                                  rf_buffer_intf *script_buf_ptr,
                                 /*!< output script that contains required settings */ 
                                  int16 delay
                                 /*!< delay in us of the first transaction */ 
                                );

  /*! method to retrieve TX ON/OFF grfc Script */
  boolean generate_tx_on_off_grfc_script (
                                   rfm_device_enum_type device,
                                   /*!< RF device */ 
                                   rfm_cdma_band_class_type band, 
                                   /*!< RF band */ 
                                   boolean tx_on,
                                   /*!< TX ON or OFF */ 
                                   rf_buffer_intf *script_buf_ptr,
                                   /*!< output script that contains required settings */
                                   int16 delay,
                                   /*!<delay in us of the first transaction>*/
                                   int32 *num_grfc
                                   /*! <Number of GRFC signals for on/off> */
                                   );
  
  /*! method to retrieve PA ON/OFF grfc Script */
  boolean generate_pa_on_off_grfc_script ( 
                                   rfm_device_enum_type device,
                                   /*!< RF device */ 
                                   rfm_cdma_band_class_type band, 
                                   /*!< RF band */ 
                                   boolean pa_on,
                                   /*!< PA ON or OFF */ 
                                   rf_buffer_intf *script_buf_ptr,
                                   /*!< output script that contains required settings */ 
                                   int16 delay
                                   /*!< time delay relative to previous 
                                                              transaction */
                                   );
  #ifdef FEATURE_RF_ASDIV
  /*! method to retrieve AsDiv grfc Script */
  boolean generate_asdiv_xsw_script ( 
                                   rfcommon_asdiv_position_type position,
                                   rf_buffer_intf *asdiv_script_ptr,
                                   void *cb_data
                                   );
  #endif /* FEATURE_RF_ASDIV */

  /*! Returns the GRFC number for a specific signal type based on device and 
      band info */
  int32 get_grfc_engine_num ( 
                              rfm_device_enum_type device,
                              /*!< RF device */ 
                              rfm_cdma_band_class_type band,
                              /*!< RF band */ 
                              rfc_signal_type sig_type
    /*!< Indicates which RFC signal type is being reverse mapped to modem GRFC*/ 
                            );

  /*! method to retrieve vreg path */
  rf_path_enum_type get_vreg_path(rfm_device_enum_type device /*!<RF device*/ );


  /*! method to retrieve RF chio number to bse used for vreg operations */
  rfc_rf_chip_number get_rf_chip_num ( rfm_device_enum_type device 
                                       /*!<RF device*/ );

  /*! method to retrieve maximum number of WTR devices */
  uint8
  rfc_cdma_get_max_wtr_devices () ;

  /*! method to find out if the RF device is an Rx device  */
  boolean 
  is_rfm_device_rx ( rfm_device_enum_type device ) ;

  /*! method to find out if the RF device is an Tx device  */
  boolean 
  is_rfm_device_tx ( rfm_device_enum_type device ) ;

  /*! method to update rx path as per concurrency manager recommendation  */
  boolean 
  update_rx_path 
  ( 
    rfm_device_enum_type device,
    rf_card_band_type curr_band, 
    uint8 alt_path
  );

  boolean 
  setup_fb_path
  (
     rfm_device_enum_type device_fb, 
  	 rfm_cdma_band_class_type band, 
  	 boolean et_enable
   ) ;												 
  
  boolean 
  setup_fb_device
  (
     rfm_device_enum_type device, 
     rfm_device_enum_type device_fb,
     rfm_cdma_band_class_type band, 
     uint32 tx_chan,
     rfdevice_rxtx_common_class**  fb_device_ptr
  );

  boolean 
  get_rffe_device_info
  (
	  rfm_device_enum_type rfm_dev, 
	  rfm_cdma_band_class_type band, 
	  rfdevice_type_enum_type rffe_device_type,
	  uint16 *mfg_id,
	  uint16 *prd_id,
	  uint16 *rev_id
  );

  boolean
  rfc_cdma_set_xsw_band_ports
  (
    rfm_device_enum_type   device ,
    rfcom_band_type_u&	   band_u ,
 	  rfc_cfg_params_type&   band_path_cfg 
  ) ;


  /*! method to update rx path as per concurrency manager 
    recommendation for a given band */
  boolean
  recreate_rx_chain_with_updated_alt_path
  (
     rfm_device_enum_type   device        ,
     rfc_cfg_params_type&   band_path_cfg ,
     uint8                  alt_path
  ) ;

  /*! method to current Rx path for a given RFM device. */ 
  uint8 
  get_current_rx_path 
  ( 
     rfm_device_enum_type       device ,
     rfm_cdma_band_class_type   band 
  ) ; 


  /*! Setter method for current path per RFM device for all 
    bands*/
  void 
  rfc_cmda_set_current_rx_path_for_device 
  ( 
     uint8 rfm_device 
  ) ; 


  /*! Setter method for current path per RFM device */
  void 
  rfc_cmda_set_current_rx_path_for_device 
  ( 
     uint8 rfm_device , 
     rfm_cdma_band_class_type new_band ,
     uint8 curr_path 
  ) ; 
  
  /*! destructor method */
  virtual ~rfc_cdma_card_intf();
  
protected:

  /*! Constructor for the base class - it is protected, to ensure that NO  
      entity other than a derived class can instantiate this object */
  rfc_cdma_card_intf( 
         /*! estimate of time (in microsecs) for prep_wakeup_rx() to complete */ 
         uint32 prep_time,  
         /*! estimate of time (in microsecs) for exec_wakeup_rx() to complete */ 
         uint32 wakeup_time,
         /*! Card init success/failure */
         boolean *card_init_status  );


  /*! Pointer to the list of memory location that holds the logical params for 
      all valid devices - the number of valid devices is decided by 
      rf_max_valid_devices*/
  rfc_cdma_logical_device_params_type* logical_dev_param_ptr;

  /*! method to instantiate the object */
  void register_rfc(rfc_cdma_card_intf *);

  /*! method to set correct sub-class infor for given band */
  boolean set_sub_bandclass_info( rfm_device_enum_type device /*!< RF device */,
                                  rfm_cdma_band_class_type band /*!<RF band*/ );

  /*! method to set LUT table type for given band and PA state */
  boolean set_lut_type_for_band_pa_state 
                                ( 
                                  rfm_device_enum_type device,/*!< RF device */
                                  rfm_cdma_band_class_type rf_band,
                                  /*!< RF band */
                                  rfm_pa_gain_state_type const pa_state,
                                  /*!< PA State for which LUT type it to be set */ 
                                  rfdevice_cdma_lut_type table_type 
                                  /*!< LUT table type - hi or lo-power */
                                );

  /*! method to retrieve Logical Device Internal Parameter */
  rfc_cdma_logical_device_internal_params_type* get_logical_dev_internal_param
                                                (
                                                  rfm_device_enum_type device
                                                  /*!< RF device */ 
                                                );

  /*! Method to find out if the NVs are enabled for CDMA card to 
    be constructed. If CDMA BC config is set, TRUE is returned. */
  boolean static rfc_cdma_card_is_cdma_enabled_by_nv ( ) ; 

  /*! Setter method for MAX CDMA RFM devices */
  void rfc_cmda_set_rfm_max_valid_devices ( uint8 max_rfm_devices ) 
  { rf_max_valid_devices = max_rfm_devices ; }

  /*! Getter method for MAX CDMA RFM devices */
  uint8 rfc_cdma_get_rfm_max_valid_devices () { return rf_max_valid_devices ; }

  /*! Method to find out if the given RFM device is a valid CDMA 
    device or not */
  boolean rfc_cdma_is_rfm_device_used ( uint8 rfm_dev  )
  { /* Device must be in the supported range */
    if( rfm_dev < RFM_MAX_WAN_DEVICES )
    {
      return rfc_cdma_valid_rfm_devices[rfm_dev] ; 
    }
    else
    {
      return FALSE;
    }
  }


private:

  /*! added default constructor - this is private to ensure that derived classes 
      do not instantiate an object without the requisite constructor parameters
      such as prep_wakeup_time.
   
      Trying to create a class without the required parameters will cause a
      compile error
  */
  rfc_cdma_card_intf()
  {

    this->dev_int_param_ptr = NULL; 
    this->rf_max_valid_devices = 0;
    this->rf_warmup_exec_time_in_us = 0;
    this->logical_dev_param_ptr = NULL;
    this->per_device_sub_bc_info_ptr = NULL;
    this->rf_warmup_prep_time_in_us = 0; 
    this->tx_lut_type_to_use_ptr = NULL;
  };

  
  /*! pointer to the class rfc_cdma_card_intf */
  static rfc_cdma_card_intf *rfc_cdma_ptr;

  /*! rf warmup time Prep time in microsecs */
  uint32 rf_warmup_prep_time_in_us;

  /*! rf warmup time exec time in microsecs */
  uint32 rf_warmup_exec_time_in_us;

  /*! valid devices that can do Rx and/or Tx */
  uint8 rf_max_valid_devices;

  /*! Structure containing the RFC-Internal Logical Device Parameter */
  rfc_cdma_logical_device_internal_params_type* dev_int_param_ptr;

  /*! structure used to specify supported sub-classes for each band and for 
      supported device */ 
  rfc_cdma_sub_band_class_info_type* per_device_sub_bc_info_ptr;

  /*! structure used to specify the Tx look-up table to use for a given 
      band and PA state */
  rfc_cdma_lut_to_use_for_pa_state_type* tx_lut_type_to_use_ptr;

  /*! Constructs the various member to the intended values*/ 
  boolean rfc_cdma_construct_cdma_device();

  /*! Populates various logical device params */
  boolean rfc_cdma_populate_logical_device ( uint8 , uint8& , uint8& ,
                                             rfc_rf_chip_number , 
                                             rf_path_enum_type,
                                             uint64 ) ;
   
   

  /*! helper that checks for rx/tx init column in RFC table. CDMA does not
      use these columns anymore, so will return FALSE if it finds anything */
  boolean is_init_column_empty() ;

  /*! Mallocs and populate pointers for various rfc_card components */
  boolean rfc_cdma_allocate_mem_for_device ( ) ;

  /*! Array with info for all the RF devices storing if the 
    device in question is capable of Rx or Tx */
  boolean is_rfm_device_rx_tx [RFM_MAX_WAN_DEVICES][RFC_CONFIG_RXTX_MAX] ;

  /*! Array with info for all the RF devices supported by the RF card */
  boolean rfc_cdma_valid_rfm_devices[RFM_MAX_WAN_DEVICES] ;

  /*! This Array contains the current path configured out of 
    the possible alt paths */
  uint8 current_path[RFM_MAX_WAN_DEVICES][RFM_CDMA_MAX_BAND] ;

};

#ifdef __cplusplus
}
#endif

/*! @} */

/*! @} */

#endif /* #ifndef RFC_CARD_CDMA_H */
