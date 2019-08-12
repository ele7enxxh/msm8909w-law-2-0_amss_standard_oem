#ifndef RFDEVICE_LNA_H
#define RFDEVICE_LNA_H
/*!
   @file
   rfdevice_lna.h

   @brief

*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfdevice/interface/main/latest/api/rfdevice_lna.h

when       who    what, where, why
--------   ---   ---------------------------------------------------------------
12/02/14   vv     Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfdevice_class.h"
#include "rf_buffer_intf.h"
#include "rfdevice_cmn_type_defs.h"
#include "rfdevice_lna_cfg.h"

#ifdef __cplusplus

class rfdevice_lna:public rfdevice_class
{
public:
  rfdevice_lna();

  virtual ~rfdevice_lna();
 
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    interface function for lna sleep

    @details
    rfdevice_lna_sleep()
   
    @param rfdevice_lna_cfg
    pointer to a lna cfg object which contains lna specific config info  
   
    @param buff_obj_ptr
    void pointer to CCS buffer object

    @param dev_action
    indicates if it is immediate write to hw or build scripts

    @param script_timing
    timing offset for CCS scripts

    @return
    boolean indicating pass or fail
  */
  virtual boolean sleep
  (
    rfdevice_lna_cfg* lna_cfg_p, 
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action, 
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  )  = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    interface function for lna wakeup

    @details
    rfdevice_lna_wakeup()
   
    @param rfdevice_lna_cfg
    pointer to a lna cfg object which contains lna specific config info  
   
    @param buff_obj_ptr
    void pointer to CCS buffer object

    @param dev_action
    indicates if it is immediate write to hw or build scripts

    @param script_timing
    timing offset for CCS scripts

    @return
    boolean indicating pass or fail
  */
  virtual boolean wakeup
  (
    rfdevice_lna_cfg* lna_cfg_p, 
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action, 
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  )  = 0;


  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    returns the lna gain settings 

    @details
    returns the lna gain settings 

    @param rfdevice_lna_cfg
    pointer to a lna cfg object which contains lna specific config info

    @param buff_obj_ptr
    void pointer to CCS buffer object

    @param dev_action
    indicates if it is immediate write to hw or build scripts

    @param script_timing
    timing offset for CCS scripts

    @return
    boolean indicating pass or fail
  */
  virtual boolean set_lna_gain
  (
    rfdevice_lna_cfg* lna_cfg_p,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  ) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    interface function for lna trigger, to latch the pending settings.

    @details
    rfdevice_lna_trigger()
   
    @param rfdevice_lna_cfg
    pointer to a lna cfg object which contains lna specific config info
   
    @param buff_obj_ptr
    void pointer to CCS buffer object

    @param dev_action
    indicates if it is immediate write to hw or build scripts

    @param script_timing
    time offset for the script where a negative value refers to the time BEFORE the event

    @return
    boolean indicating pass or fail
  */
  virtual boolean trigger
  (
    rfdevice_lna_cfg* lna_cfg_p,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  ) = 0;

 
  /*----------------------------------------------------------------------------*/
   /*!
    @brief
    populates the band map information from rfc

    @details
    populates the band map information from rfc

    @param rfdevice_lna_cfg
    pointer to a lna cfg object which contains lna specific config info

    @param *data_ptr
    band mapping information passed as int32 pointer
    
    @param size
    number of valid words in the *data_ptr
    
    @return
    boolean indicating pass or fail
  */
  virtual boolean set_band_map
  (
    rfdevice_lna_cfg* lna_cfg_p,
    int32 *data_ptr, uint8 size
  ) = 0;


  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    returns the lna gain for the requested system gain

    @details
    the techs would call this api to get the lna gain to system gain mapping for a particular tech and band.

    @param rfdevice_lna_cfg
    pointer to a lna cfg object which contains lna specific config info

    @param *lna_gain
    returns an enum indicating the lna gain
   
    @return
    boolean indicating pass or fail
  */
  virtual boolean get_lna_gain_index
  (
    rfdevice_lna_cfg* lna_cfg_p,
    rfdevice_lna_gain_enum_type *lna_gain
  );

 
};
#endif /* #ifdef __cplusplus */

#endif /* RFDEVICE_LNA_H */
