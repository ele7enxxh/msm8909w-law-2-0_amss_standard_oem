#ifndef TP_ANTENNA_TUNER_H
#define TP_ANTENNA_TUNER_H
/*!
   @file
   tp_antenna_tuner.h

   @brief
   tp antenna tuner device driver header file

*/

/*===========================================================================

Copyright (c) 2012 - 2014 by  Qualcomm Technologies, Inc (QTI).  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$ $

when       who   what, where, why
--------   ---   -------------------------------------------------------------------
08/20/14   ndb    Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
07/18/14   aca   Clean up
07/14/14   aca   Clean up
06/25/14   vv    Updated program_usid_list() API to new physical device config structure
06/12/14   cc    New sequence for TP_BUFFER
05/13/14   cc    Added TP_BUFFER support 
05/13/14   vv    Physical device support for tp_tuner
03/25/14   yb    Added support for linearizer
11/27/13   yb    Added support for default tune code when tuner QCN is missing
                 Removed Low Power API
10/29/13   ndb   Added the support for dynamic tune code size
10/14/13   vs    Updated definition for low power API
10/10/13   vs    Added definition for low_power API
08/27/13   vs    Removed API for rffe buffer device 
8/7/13      vs   updated support for tune code override

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "rfdevice_antenna_tuner.h"
#include "rf_rffe_common.h"
#include "rfcommon_locks.h"
#include "sys.h"
#include "rfc_common.h"
#include "rfdevice_physical_device.h"
#ifdef __cplusplus
}
#endif

/* Data structure to hold the tuner config.*/ 
typedef struct
{
  uint8 c1;
  uint8 c2;
  uint8 c3;
  uint8 c4;
}tp_antenna_tuner_tune_codes;


typedef struct
{
  uint8 version;
  PACK(union)
  {
    PACK(struct)
    {
      uint8 c1_tune_code;
      uint8 c2_tune_code;
      uint8 switch_config;
    }version_0;
  }tunecode_config;
}tp_antenna_tuner_tunecode_config_data_type;


/* Data structure to hold the override tune code values .Currently support Version 0 format  */ 

typedef struct
{
 PACK(union)
  { 
   tp_antenna_tuner_tune_codes tuner_data;
  }antenna_tuner_data_v0;
 uint8 tp_antenna_tuner_override_flag;
}tp_antenna_tuner_tunecode_override_info_type;

#define RFDEVICE_TPTUNER_MAX_REG_SIZE 20


typedef struct
{
  uint16 *addr;
  int16 *data;
} tptuner_reg_settings_type;

/*API for rffe buffer device programming*/
boolean 
program_usid_list
(
  rfc_phy_device_info_type* cfg
);

class tp_antenna_tuner: public rfdevice_antenna_tuner
{
  public:

  tp_antenna_tuner( rfc_device_cfg_info_type* cfg,
                         rf_path_enum_type rf_path,
                         boolean extended_cmd,
                         uint8 half_rate
                       );
  /*overloaded constructor for physical device creation on Bolt*/
  tp_antenna_tuner(
                     rfdevice_physical_device* phy_obj_ptr,
                     rfc_phy_device_info_type* phy_device_info, 
                     rfc_logical_device_info_type* logical_device_info
                   );
  /*To store the parent physical device object*/
  rfdevice_physical_device* rfdevice_phy_obj_ptr;

  ~tp_antenna_tuner();

  void init(void);
  
  boolean init( rf_buffer_intf *buff_obj_ptr, rf_device_execution_type dev_action, int16 script_timing = 0 );

  boolean rx_mode( rf_buffer_intf *buff_obj_ptr,
                   rf_device_execution_type dev_action,
                   int16 script_timing = 0,
                   rfcom_mode_enum_type mode = RFCOM_PARKED_MODE,
                   uint32 rx_freq = 0 );

  boolean rx_init( rf_buffer_intf *buff_obj_ptr,   
	                rfcom_mode_enum_type mode,
	                rf_device_execution_type dev_action,
	                int16 script_timing = 0);
  
  boolean rx_disable( rf_buffer_intf *buff_obj_ptr, rf_device_execution_type dev_action, int16 script_timing = 0 );//for sleep is not used

  boolean tx_mode_enable( rfcom_mode_enum_type mode, uint32 tx_freq,  
                          rf_buffer_intf *buff_obj_ptr, 
                          rf_device_execution_type dev_action,
                          int16 script_timing = 0,
                          rfcom_band_type_u band = rfcom_band_type_u());

  boolean tx_mode_disable( rf_buffer_intf *buff_obj_ptr,             
                           rf_device_execution_type dev_action,
                           int16 script_timing = 0 );
  
  boolean disable( rf_buffer_intf *buff_obj_ptr, rf_device_execution_type dev_action, int16 script_timing = 0 );

  boolean program_tune_code(uint8* tune_code_ptr, uint8 tune_code_size,rf_buffer_intf *buff_obj_ptr, rf_device_execution_type dev_action, int16 script_timing); 

  /*! Program default tune code */
  boolean program_default_tune_code(uint8* tune_code_ptr,
                                    uint8 *tune_code_size_ptr,
                                    rf_buffer_intf *buff_obj_ptr,
                                    rf_device_execution_type dev_action,
                                    int16 script_timing);

  uint16 get_script_size(void);
  /*! Read chip version of the antenna tuner */
  int16 read_chip_rev(void);
 rfdevice_antenna_tuner_device_override_status_type tune_code_override(uint8 override_flag,
   	                                                                void *data,
 	                                                                uint8* tuner_nv_ptr,
 	                                                                uint8* tuner_id_ptr);

  rfdevice_antenna_tuner_device_num_type get_antenna_tuner_instance_val( void );

  protected: 

  uint8 rffe_bus;       /*! RFFE channel/bus information */    

  uint8 default_usid;   /*! RFFE Default Unique Slave ID */
  uint8 assigned_usid;  /*! RFFE Programmable Unique Slave ID*/ 
  uint8 gsid;
  uint8 half_rate;      /*!RFFE rate*/
  rf_rffe_data_type rffe_script[RFDEVICE_TPTUNER_MAX_REG_SIZE];

  /* Override tuner code values */
  tp_antenna_tuner_tunecode_override_info_type tuner_tunecode_override_data;

  /* Func to compute the tune code values based on the tune code config and update nv values */
  rfdevice_antenna_tuner_device_override_status_type get_tuner_tune_codewords(void *data,
                                                                              uint8* tuner_nv_ptr );
  private:
     
  void reset_rffe_script(void);
};

#endif 
