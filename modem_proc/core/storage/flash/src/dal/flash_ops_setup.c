/**********************************************************************
 * flash_ops_setup.c
 *
 * SERVICES:    Provides flash operation setup API's used by EFS
 *
 * DESCRIPTION: Provides flash operation setup API's used by EFS
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2011-2013 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===================================================================
 *
 *                   EDIT HISTORY FOR MODULE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_ops_setup.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 * when         who     what, where, why
 * --------     ---     ----------------------------------------------
 * 09/08/14     sb      Add feature flag for SPI NOR specific code.
 * 09/26/13     sb      Add synchronization for flash ops start/end
 * 05/03/13     eo      Add flash ops mechanism to enable/disable QPIC clocks
 * 12/04/12     eo      Add run time NAND/SPI NOR driver support 
 * 10/08/12     eo      Disable NPA votes temporarily
 * 04/02/12     sv      Optimize flash ops mechanism
 * 03/19/12     sv      Move power mode and spi close api before npa release
 * 03/07/12     sv      set core cpu mips to 288, configurable from .builds
 * 02/15/12     sv      Reduce the core cpu npa request to 300 mips
 * 01/18/12     sv      Add SPI NOR power management API calls
 * 01/16/12     eo      Add support for open/close of spi handle
 * 01/06/12     sv      Add/update npa calls for core clock and latency
 * 11/23/11     eo      Replace deprecated sleep apis with new sleep voting
 * 03/22/11     sv      Fix issue with NPA release
 * 03/07/11     sv      Initial Version
 *===================================================================*/

 /*===================================================================

===========================================================================*/

#include "core_variation.h"
#include "flash.h"

#include "DDITimer.h"
#include "DALDeviceId.h"

#include "flash_ops_setup.h"
#include "flash_dal_config.h"
#include "flash_clocks.h"
#ifdef FLASH_ENABLE_NOR_SPI
#include "flash_spi_wrapper.h"
#endif

#include "flash_profile_common.h"

#ifndef FEATURE_SPI_NOR_DISABLE_DP_PWR_DWN
  #define ENABLE_SPI_NOR_DP_PWR_DWN
#endif

/* Delay time of 10 * 10^6 usec to enable flash ops voting. */ 
#define FLASH_OPS_VOTE_DELAY_USEC 10000000

/* DAL timer struct to call DAL Timer */
static struct
{
  DalDeviceHandle    *timer_handle;    /* DAL callback timer */
  DALSYSEventHandle  timer_event;      /* DAL callback timer event */
  flash_client_ctxt  *client_ctxt;     /* Flash client context */
  uint32             timer_ops_enable; /* Enable flash operations */ 

  DALSYS_EVENT_OBJECT(timer_object);   /* DAL timer event object */
} dal_timer = {NULL, NULL, NULL, FALSE, FALSE, NULL};  

/* Structure for flash ops syncronization */
static struct 
{
  DALSYS_SYNC_OBJECT(flash_ops_sync_object); /* Synchronizaton object */
  DALSYSSyncHandle handle_flash_ops_sync;    /* Handle to sync object */
  uint32 ops_synclock_init;
} flash_ops_locks = {NULL, NULL, FALSE};

/* DAL handle to call SPI DAL open/close */
DalDeviceHandle *spi_dal_driver_handle = NULL;

#ifdef FLASH_ENABLE_NOR_SPI
/* Flash handle for Power management */
static flash_handle_t handle_flash = NULL;
#endif

/* Counter to store the number of flash_ops_start without an equivalent 
 * flash_ops_end
 */
uint32 flash_ops_counter = 0;

/* Flag to indicate flash ops configs has been executed or not */
uint32 flash_ops_start_flag = FALSE;

/* Flash DAL timer callback function */
void flash_ops_callback(void)
{
  dal_timer.timer_ops_enable = TRUE;
}

/* Flash DAL timer init */
void flash_ops_timer_init(void)
{
  /* check DAL timer handle and flash client context exist */
  if (!dal_timer.timer_handle && dal_timer.client_ctxt)
  {
    dal_timer.timer_ops_enable = FALSE;
    
    /* Create this client's DAL timer handle once. */
    DALSYS_InitMod(NULL);
    
    /* Attach to DAL timer device driver */
    if (DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_TIMER, 
      &dal_timer.timer_handle))
    {
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
        "Flash: Error attaching to DAL Timer device driver!");
      
      dal_timer.timer_handle = NULL;
    }

    /* Create timer event with callback function notification */
    if (dal_timer.timer_handle)
    {
      if (DAL_SUCCESS != DALSYS_EventCreate(
        DALSYS_EVENT_ATTR_CALLBACK_EVENT | DALSYS_EVENT_ATTR_TIMEOUT_EVENT,
        &dal_timer.timer_event, &dal_timer.timer_object))
      {
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
          "Flash: Error creating to DAL timer event");      
      }

      if (DAL_SUCCESS != DALSYS_SetupCallbackEvent(dal_timer.timer_event,
        (DALSYSCallbackFunc)flash_ops_callback, dal_timer.client_ctxt))
      {
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
          "Flash: Error creating to DAL callback event");            
      }

      if (DAL_SUCCESS != DalTimer_Register(dal_timer.timer_handle, 
        dal_timer.timer_event, FLASH_OPS_VOTE_DELAY_USEC))
      {
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
          "Flash: Error invoking DAL timer registet");            
      }
    }
  }
}

/* Flash Operations get flash client context */
void flash_ops_get_handle(uint32 *client_ctxt_data)
{
  if (!dal_timer.client_ctxt)
  {
    dal_timer.client_ctxt = (flash_client_ctxt *)client_ctxt_data;
  }
}

/* Flash Operations init */
void flash_ops_init(void)
{
  flash_clocks_init();

#ifdef FLASH_ENABLE_NOR_SPI
  if (flash_dal_config.config == FLASH_DAL_CONFIG_NOR)
  {
    /* check if spi dal driver handle exists */
    if (!spi_dal_driver_handle)
    {
      /* Create this client's SPI DAL handle once.
       */
      spi_dal_driver_handle = flash_spi_dev_init();
    }
      
#ifdef ENABLE_SPI_NOR_DP_PWR_DWN
    /* Attach the Flash device */
    if ((!handle_flash) && spi_dal_driver_handle)
    {
      if (flash_device_attach(DALDEVICEID_FLASH_DEVICE_1, &handle_flash) !=
        FLASH_DEVICE_DONE)
      {
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR, 
          "Flash: foi - Flash attach failed!");
      }
    
      /* Open the 0:ALL partition. The open partition is required here because
       * some of the vector tables are initialized only during open paritition.
       */
      if (flash_open_partition(handle_flash, (unsigned char*)"0:ALL") !=
        FLASH_DEVICE_DONE)
      {
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
          "Flash: foi -Flash open partition failed.!");
      }
    
      /* Flash client context set */
      dal_timer.client_ctxt = handle_flash->flash_client_ctxt;      
    } 
#endif
  }
#endif
  
  /* Flash DAL timer init */
  flash_ops_timer_init();
  
  if (flash_ops_locks.ops_synclock_init == FALSE)
  { 
    if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
      &(flash_ops_locks.handle_flash_ops_sync),
      &(flash_ops_locks.flash_ops_sync_object)))
    {
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
        "Flash_OPS:SyncLock Create Failed!");
    }
    flash_ops_locks.ops_synclock_init = TRUE;
  }	
}

/* Flash Operations deinit */
void flash_ops_deinit(void)
{
  flash_clocks_deinit();
  
#ifdef FLASH_ENABLE_NOR_SPI
  if (flash_dal_config.config == FLASH_DAL_CONFIG_NOR)
  {
#ifdef ENABLE_SPI_NOR_DP_PWR_DWN
    if (handle_flash)
    {
      /* Close the Flash device */
      if (flash_device_close(handle_flash) != FLASH_DEVICE_DONE)
      {
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
          "Flash: fod - Flash close failed.!");
      }
  
      /* Detach the Flash device */
      if (flash_device_detach(handle_flash) != FLASH_DEVICE_DONE)
      {
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
          "Flash: fod - Flash detach failed.!");
      }
      handle_flash = NULL;
    }
#endif
  
    /* check if spi dal driver handle exists */
    if (spi_dal_driver_handle)
    {
      spi_dal_driver_handle = NULL;
    }
  }
#endif
 
  /* if dal timer handle exists, free timer event and detach */
  if (dal_timer.timer_handle)
  {
    if (DALSYS_DestroyObject(&dal_timer.timer_event) != 
      DAL_SUCCESS)
    {
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
        "Flash: Error Freeing DAL Timer Event!");
    }
    dal_timer.timer_event = NULL;
    
    if(DAL_DeviceDetach((DalDeviceHandle*)dal_timer.timer_handle) !=
      DAL_SUCCESS)
    {
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
        "Flash: Error DAL Timer Detach!");
    }
    dal_timer.timer_handle = NULL;
    dal_timer.client_ctxt = NULL;
    dal_timer.timer_ops_enable = FALSE;
  }

  if (flash_ops_locks.ops_synclock_init == TRUE)
  { 
    if (DALSYS_DestroyObject(flash_ops_locks.handle_flash_ops_sync) != 
      DAL_SUCCESS)
    {
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
        "Flash: Error Freeing DAL Timer Event!");
    }
	flash_ops_locks.ops_synclock_init = FALSE;
  }
}

/* Flash Operations start - will be called by EFS before any
 * Flash operation
 */
void flash_ops_start(void)
{
  /* Synchronize with other clients */
  DALSYS_SyncEnter(flash_ops_locks.handle_flash_ops_sync);

  /* Increment flash ops counter */
  flash_ops_counter++;

  /* Leave Synchronization */
  DALSYS_SyncLeave(flash_ops_locks.handle_flash_ops_sync);
}

/* Flash Operations begin operation - will be called in Flash driver before
 * any Flash operation begins. This helps to defer the configurations,
 * npa requests etc until an actual Flash operation happens so that any 
 * redundant execution of them without any flash operation can be avoided.
 */
void flash_ops_begin_op(void)
{
  FLASH_CORE_PROFILE_START(FLASH_PROFILE_OPS_START, 1);
  
  /* Execute only when the flash ops_counter is not 0 i.e. flash_ops_start
   * is called without a matching flash_ops_end and flash_ops_start_flag
   * is not set
   */
  if ((flash_ops_counter != 0) && (flash_ops_start_flag == FALSE))
  {
    /* Set the flash_ops_start flag to indicate that the init/config
     * function for flash operations has been invoked
     */
    flash_ops_start_flag = TRUE;

    /* Since the NPA modules are not not ready by the time the EFS calls
     * flash_ops_init, the required npa handles may not he created at that
     * time. So call flash_ops_init everytime flash_ops_start is called.
     * This will create the required npa handles if not created already.
     */
    flash_ops_init();

    /* Request vote on all related npa nodes */
    flash_clocks_request();
    
#ifdef FLASH_ENABLE_NOR_SPI
    if (flash_dal_config.config == FLASH_DAL_CONFIG_NOR)
    {
      if (spi_dal_driver_handle)
      {
        /* Dal open SPI dal driver */
        flash_spi_dev_open();
      }

#ifdef ENABLE_SPI_NOR_DP_PWR_DWN
      if (handle_flash && (dal_timer.timer_ops_enable == TRUE))
      {
        /* Set flash power mode to Stand-by mode so that the flash device can  
         * accept commands
         */  
        if (flash_set_power_mode(handle_flash, FLASH_POWER_NORMAL_MODE) != 
          FLASH_DEVICE_DONE)
        {
          DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
            "Flash: fos - Failed to set power mode to STANDBY.!");
        }
      }
#endif
    }
#endif    
  }
  
  FLASH_CORE_PROFILE_END(FLASH_PROFILE_OPS_START);
}

/* Flash Operations end - will be called by EFS after any
 * Flash operation
 */
void flash_ops_end(void)
{
  /* Synchronize with other clients */
  DALSYS_SyncEnter(flash_ops_locks.handle_flash_ops_sync);

  FLASH_CORE_PROFILE_START(FLASH_PROFILE_OPS_END, 1);

  if (flash_ops_counter == 0)
  { 
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
      "Flash: foe - flash_ops_end called without flash_ops_start.!");
  }

  /* Decrement flash ops counter */
  flash_ops_counter--;
  
  /* Execute only when flash_ops_counter is 0 i.e. no more flash_ops_start()
   * is there without a flash_ops_end() and flash_ops_start_flag has been set
   */
  if ((flash_ops_counter == 0) && (flash_ops_start_flag == TRUE))
  {
#ifdef FLASH_ENABLE_NOR_SPI
    if (flash_dal_config.config == FLASH_DAL_CONFIG_NOR)
    {
#ifdef ENABLE_SPI_NOR_DP_PWR_DWN
      if (handle_flash && (dal_timer.timer_ops_enable == TRUE))
      {
        /* Set flash power mode to Deep Power Down to reduce power 
         * consumption 
         */
        if (flash_set_power_mode(handle_flash, FLASH_POWER_LOW_PWR_MODE) != 
          FLASH_DEVICE_DONE)
        {
          DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
            "Flash: foe - Failed to set power mode to DPD.!");
        }
      }
#endif

      if (spi_dal_driver_handle)
      {
        /* Dal close SPI dal driver */
        flash_spi_dev_close();
      }
    }
#endif    
    /* Release vote on all related npa nodes */
    flash_clocks_request_complete();
    
    /* Reset the flash_ops_start flag */
    flash_ops_start_flag = FALSE;
  }
  
  FLASH_CORE_PROFILE_END(FLASH_PROFILE_OPS_END);

  /* Leave Synchronization */
  DALSYS_SyncLeave(flash_ops_locks.handle_flash_ops_sync);
}
