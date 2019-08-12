/********************************************************************
 * @file dcvs_os.h
 *
 * @brief OS interface header file for Dynamic Clock and
 * Voltage Scaling(DCVS) service.
 *
 * Copyright (C) 2010 - 2011 by Qualcomm Technologies, Inc. All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/inc/dcvs_os.h#1 $
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    ---------------------------------------------------
04/26/11   ss      Fixing Q6 compiler warnings.
02/18/11   ss      Adding context based processing for DCVS.
03/30/10   ss      Adding interfaces DCVSOSTicksToMsConvert and 
                   DCVSOSStartUpDelay.
02/21/09   ss      Removing redundant definitions and interface 
                   declarations.
01/20/10   bjs     Initial check in.
========================================================================*/
#ifndef DCVS_OS_H
#define DCVS_OS_H

#include "DALSys.h"
#include "dcvs_core.h"

typedef void* DCVSOSTimerHandle ;

typedef void (*DCVSOSCallback)(DCVS_CONTEXT* pv);

typedef void (*dcvs_timer_cb)(DCVS_CONTEXT*);


/*********************************************************************
  @brief This function initializes the DCVS OS interface.
  Initializes the OS layer for DCVS.

  @param context: The DCVS data context.

 *********************************************************************/
void DCVSOSInitialize(DCVS_CONTEXT* context);

/******************************************************************//**
  @brief This function allows firing of delayed callback.
  Callback defined at cb is asynchronously fired after delay_ms 
  milliseconds.

  @param cb: The callback which is invoked after the delay.
  @param delay_ms: The delay in ms.
  @param context: The DCVS data context.

 *********************************************************************/
void DCVSOSCallbackCreate(DCVSOSCallback cb, uint32 delay_ms, DCVS_CONTEXT* context);

/******************************************************************//**
 * @brief OS interface to query processor utilization.
 * 
 *
 *********************************************************************/
void DCVSOSIdleInfo(uint32* idle_override);


/**
  @brief DCVSOSTicksToMsConvert
  
  This function is used to convert system ticks into ms . 

  @param id: the system ticks to be converted. 
  
  @return the ms value for the ticks passed in. 
      

*/
uint32 DCVSOSTicksToMsConvert(uint32 sysTicks);


/**
  @brief DCVSOSStartUpDelay
  
  This function is used to define the OS specific delay (in ms) 
  before starting the DCVS algorithm. 
  
  @return the ms value that is used as delay before the start of the
  DCVS algorithm.
      
*/
uint32 DCVSOSStartUpDelay(void);


/**
   @brief DCVSOSCriticalSectionEnter - This API is used for
   synchronization within the OS layer. It is called before 
   entering a critical section. The implementation of this API 
   uses locking based on OS specific basis.
   
   @param  context: The The DCVS data context.

   @return : None.

*/
void DCVSOSCriticalSectionEnter(DCVS_CONTEXT* context);


/**
   @brief DCVSOSCriticalSectionExit - This API is used for
   synchronization within the OS layer. It is called before 
   exiting a critical section.

   @param context: The The DCVS data context.

   @return : None.

*/
void DCVSOSCriticalSectionExit(DCVS_CONTEXT* context);

/**
   @brief DCVSOSSleepTickGet - Used to return the accumulated idle time of the system
   since the system is turned on. 
   @param context: The The DCVS data context.
   @return : The accumulated sleep ticks of the system.

*/
uint32 DCVSOSSleepTickGet(DCVS_CONTEXT* context);

/**
   @brief DCVSOSSystemTickGet - Used to return the the number of system ticks since the system is 
   turned on.
   @param context: The OAL The DCVS data context.
   @return : The accumulated system ticks.

*/
uint32 DCVSOSSystemTickGet(DCVS_CONTEXT* context);

/**
   @brief DCVSOSTimerCreate - This API is used to create a
   DCVS timer . DCVSOSTimerStart is used to start the timer.

   @param cb: Callback function to be invoked after the timer
              expires.
   @param context: The The DCVS data context. 

   @return : Timer handle to the created DCVS timer.

*/
DCVSOSTimerHandle DCVSOSTimerCreate(DCVSOSCallback cb, DCVS_CONTEXT* context);


/**
   @brief DCVSOSTimerDestroy - This API is used to delete
   the DCVS timer. The DCVS timer handle would be invalid after 
   this call .
   @param Handle: Timer handle of DCVS timer to be deleted.
   @param context: The The DCVS data context.

   @return : TRUE : Handle succesfully deleted.
             FALSE : Handle could not be deleted.

*/
DALBOOL DCVSOSTimerDestroy(DCVSOSTimerHandle Handle, DCVS_CONTEXT* context);

/**
   @brief DCVSOSTimerStart - Used to start the OS timer used for DCVS
   sampling .
    
   @param Handle : The timer handle used to start a DCVS timer .
   @param time_ms : Time duration information in ms. 
   @param context: The The DCVS data context. 
   @return : 
   TRUE: Timer started succesfully .
   FALSE: Timer couldnt be started. 

*/
DALBOOL DCVSOSTimerStart(DCVSOSTimerHandle Handle, uint32 time_ms, DCVS_CONTEXT* context); 

/**
   @brief DCVSOSTimerStop - Used to stop the OS timer used for DCVS
   sampling .

   @param Handle : The timer handle used to start a DCVS timer .
   @param context: The The DCVS data context. 
   @return : void


*/
void DCVSOSTimerStop(DCVSOSTimerHandle Handle, DCVS_CONTEXT* context);

#endif /* DCVS_OS_H */
