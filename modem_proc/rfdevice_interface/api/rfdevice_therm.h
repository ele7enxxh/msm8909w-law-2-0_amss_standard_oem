#ifndef RFDEVICE_THERM_H
#define RFDEVICE_THERM_H
/*!
   @file
   rfdevice_therm.h
   
   @brief
   The therm interface is designed to support RF Temp comp as well as thermal mitigation.
   
   @details
   Interface provides support for
   1) Setup
   2) Temperature reading
   3) Clean up
   
   This interface document will enable access to ADCs of various chips such as
   - WTR
   - Tuner
   - ASM (3rd party and QFE)
   - PA (3rd party and QFE)
   - Standalone therm devices
   
   Each physical chip will expose this interface via the therm logical component.
   These objects can be accessed in a few wasys
   1) RFC can be queried for a list of devices supporting thermistor reads
   2) RFC can provide a list of devices which support thermistor that are actively used by a specific band & path
   
   It is required that the ADC for thermistor is not shared with any other asynchronous operations.
   For example, QFE1520 which uses the same ADC for K-sensor, TDET and therm is NOT supported.
   The thermistor circuity must not require the setup of any other component of chip in question. 
   i.e. The chip need only to be powered on 
		- 1.8 v for 3rd party
		- 1.8v and 2.7 for QFE devices
   
   Possible use cases/ call flows are-
   1) Single immediate read 
		- Setup therm
		- Read therm 
		- Clean up therm
   2) Single script read 
		- Setup therm
		- Request therm read script
		- External client executes therm script when appropriate
		- Read values are returned to the process read API as a script
		- Driver calculates the temp read in degrees C
		- Clean up therm
   3) Multiple immediate therm reads
		- Setup therm
		- Read therm 1st time
		- Read therm 2st time
		- Read therm 3st time
		- Clean up therm
   
   The interface provides temperature in  degrees C
   Each supporting chip must maintain its own ADC to degrees C conversion.
   

*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_therm.h#1 $

when       who    what, where, why
--------   ---   ---------------------------------------------------------------
05/27/14   aca    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfdevice_class.h"
#include "rf_buffer_intf.h"
#include "rfdevice_cmn_type_defs.h"

#ifdef __cplusplus

extern "C" {
#include "rf_hal_rffe.h"
}
/*===========================================================================
                           Configuration class definitions
===========================================================================*/

class rfdevice_therm_setup_cfg: public rfa
{
public:

  rfdevice_therm_setup_cfg();  

  virtual ~rfdevice_therm_setup_cfg();
  
  /* Setup params */
  uint8 num_avg;  /* Averaging required */
  
  /* Std script params */
  rf_device_execution_type dev_action;  /* Request for script or immediate execution */  
  rf_buffer_intf *buff_obj_ptr;  /* Pointer to the buffer to return script */
  int16 script_timing;  /* Timing offset required */
};
/*----------------------------------------------------------------------------*/

class rfdevice_therm_clean_up_cfg: public rfa
{
public:

  rfdevice_therm_clean_up_cfg();  

  virtual ~rfdevice_therm_clean_up_cfg();  
  
  /* Std script params */
  rf_device_execution_type dev_action;  /* Request for script or immediate execution */  
  rf_buffer_intf *buff_obj_ptr;  /* Pointer to the buffer to return script */
  int16 script_timing;  /* Timing offset required */
};
/*----------------------------------------------------------------------------*/

class rfdevice_read_imm_cfg: public rfa
{
public:

  rfdevice_read_imm_cfg();  

  virtual ~rfdevice_read_imm_cfg();  
  
  /* Setup params */
  uint8 num_avg;  /* Averaging required */
  
  /* Return param */
  int16 temp_read_C;  /* Temperature in degrees C*/
};
/*----------------------------------------------------------------------------*/

class rfdevice_read_script_cfg: public rfa
{
public:

  rfdevice_read_script_cfg();  

  virtual ~rfdevice_read_script_cfg();  
  
  /* Setup params */
  uint8 num_avg;  /* Averaging required */
  
  /* Return param */
  int16 temp_read_C;  /* Temperature in degrees C */
  
  /* Std script params */
  rf_device_execution_type dev_action;  /* Request for script or immediate execution */  
  rf_buffer_intf *buff_obj_ptr;  /* Pointer to the buffer to return script */
  int16 script_timing;  /* Timing offset required */
};
/*----------------------------------------------------------------------------*/

class rfdevice_process_read_script_cfg: public rfa
{
public:

  rfdevice_process_read_script_cfg();  

  virtual ~rfdevice_process_read_script_cfg();  
  
  /* Setup params */
  uint8 num_avg;  /* Averaging that was requested */
  
  /* Return param */
  int16 temp_read_C;  /* Temperature in degrees C*/
  
  /* Raw script params */
  rf_hal_bus_rffe_type* raw_script; /* Script with raw reads returned by tech */  
  uint8 num_transactions; /* Number of entries in the raw_script to be processed */
};
/*===========================================================================
                           THERM class definition
===========================================================================*/

class rfdevice_therm:public rfdevice_class
{
public:

    rfdevice_therm();

    virtual ~rfdevice_therm();

    /*----------------------------------------------------------------------------*/
    /*!
      @brief
      interface function for therm setup

      @details
      Function to setup the thermistor for reads.
	  This function can be called for immediate execution or to generate the required script
	  

      @param rfdevice_therm_setup_cfg
      object of the therm setup cfg class which provides all required params (see above)

      @return
      boolean indicating pass or fail
    */
    virtual boolean setup_therm (rfdevice_therm_setup_cfg* therm_setup_cfg_p)  = 0;

	/*----------------------------------------------------------------------------*/
    /*!
      @brief
      interface function for therm clean up

      @details
      Function to clean up the thermistor after reads.
	  This function can be called for immediate execution or to generate the required script
	  

      @param rfdevice_therm_clean_up_cfg
      object of the therm clean up cfg class which provides all required params (see above)

      @return
      boolean indicating pass or fail
    */
    virtual boolean clean_up_therm (rfdevice_therm_clean_up_cfg* therm_clean_up_cfg_p)  = 0;

    /*----------------------------------------------------------------------------*/
    /*!
      @brief
      interface function for immediate therm read	  

      @details
      Function to read thermistor immediately.
	  Temperature is returned in degrees C as a part of the request structure

      @param rfdevice_read_imm_cfg
      object of the therm read  imm cfg class which provides all required params (see above)

      @return
      boolean indicating pass or fail
    */
        
    virtual boolean read_therm_immediate (rfdevice_read_imm_cfg* therm_read_cfg_p)  = 0;

    /*----------------------------------------------------------------------------*/
    /*!
      @brief
      interface function for script based therm read	  

      @details
      Function to generate a therm read script
	  Script may be executed by caller or passed on to FW/DM for time specific execution

      @param rfdevice_read_script_cfg
      object of the therm read script cfg class which provides all required params (see above)

      @return
      boolean indicating pass or fail
    */

    
    virtual boolean read_therm_script (rfdevice_read_script_cfg* therm_read_cfg_p)  = 0;
	
	/*----------------------------------------------------------------------------*/
    /*!
      @brief
      interface function for process scripts that were executed outside the driver

      @details
      Function to convert the raw reads into degrees C 
	  Input script will be error checked for 
	  1) Number of registers being provided
	  2) Address of the registers that were read
	  3) Basic sanity as defined by chip's systems w.r.t. data validity
	  

      @param rfdevice_process_read_script_cfg
      object of the process read therm cfg class which provides all required params (see above)

      @return
      boolean indicating pass or fail
    */

    
    virtual boolean process_therm_read_script (rfdevice_process_read_script_cfg* therm_read_cfg_p)  = 0;
};
#endif /* #ifdef __cplusplus */
#endif /* RFDEVICE_THERM_H */
