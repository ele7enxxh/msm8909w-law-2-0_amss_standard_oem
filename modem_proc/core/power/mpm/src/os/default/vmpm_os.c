/*==========================================================================
 
  FILE:       vmpm_os.c

  OVERVIEW:   This file implements default OS specific functions of MPM driver
              and is expected to be used within MPM driver only.

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/os/default/vmpm_os.c#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 
#include "vmpm_internal.h"              /* MPM driver related files */
#include "DDIInterruptController.h"     /* IC and GPIO related files */
#include "DDITlmm.h"
#include "DDIGPIOInt.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"                  /* General DAL files */
#include "DALSys.h"

#include "CoreVerify.h"

/*=========================================================================
 *                   INTERNAL DEFINITIONS & DECLARATIONS
 *========================================================================*/

/**
 * Handle for DAL based Interrupt Controller. This will be used for mapping,
 * registering and triggering interrupts.
 */
static DalDeviceHandle *s_mpmDalIcHandle = NULL;

/**
 * Handle for DAL based GPIO controller. It will be used for mapping and 
 * triggering GPIOs.
 */
static DalDeviceHandle *s_mpmDalGpioHandle = NULL;

/**
 * ISR for rpm to master mpm wakeup interrupt. This function is not doing
 * anything as of now as we trigger interrupts and gpios occured during sleep
 * while coming out of xo shutdown
 */
static void* master_wakeup_isr( DALISRCtx isr_context )
{
  return NULL;
}

/*=========================================================================
 *                   STATIC DEFINITIONS & DECLARATIONS
 *========================================================================*/


/*=========================================================================
 *                   GLOBAL DEFINITIONS & DECLARATIONS
 *========================================================================*/

/**
 * DAL lock handle for the shared message RAM area. This lock must be
 * acquired whenever Message RAM is being modified.
 */
DALSYSSyncHandle mpmSharedMsgRamLock;

/*
 * vmpm_init
 */
void vmpm_init( void )
{
  uint32 mpmPinNum;
  uint32 gpioNum;

  /* Initializing MPM logging */
  vmpm_LogInit();

  /* Obtain target specific data */
  vmpm_TargetDataInit();

  /* Get the MPM interrupt controller hardware in a good state. */
  vmpm_HAL_Init(NULL, (uint8 *)vmpm_IrqCfgRegBase_VA, TRUE);

  /* Creating a lock for Shared Message RAM */
  CORE_DAL_VERIFY( DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
                                     &mpmSharedMsgRamLock, NULL) );

  /* Obtaining DAL handle for Interrupt controller */
  CORE_LOG_DAL_VERIFY(
    DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER, &s_mpmDalIcHandle ),
    vmpm_LogPrintf( 0, "ERROR: Attaching to Interrupt Controller failed" ) 
  );

  /* Obtaining DAL handle for GPIO controller */
  CORE_LOG_DAL_VERIFY(
    DAL_DeviceAttach( DALDEVICEID_GPIOINT, &s_mpmDalGpioHandle ),
    vmpm_LogPrintf( 0, "ERROR: Attaching to GPIO Controller failed" ) 
  );

  /* Inform the local interrupt controller and GPIO controller of the wakeup 
   * interrupts. Those subsystems will let us know when any of these 
   * interrupts gets enabled, disabled or reconfigured. */
  for( mpmPinNum = 0; mpmPinNum < vmpm_getMapTblSize(); mpmPinNum++ )
  {
    if( MPM_OPEN == vmpm_pinMapTbl[mpmPinNum].mpm_pin_type )
    {
      /* Unmapped interrupt/gpio. Skip this table entry */
      continue;
    }

    if( MPM_SUCCESS == vmpm_getGpioNum(mpmPinNum, &gpioNum) )
    {
      /* Inform about the GPIO wakeup. */
      if( DAL_SUCCESS != 
          GPIOInt_MapMPMInterrupt( s_mpmDalGpioHandle, gpioNum, mpmPinNum ) )
      {
        vmpm_LogPrintf( 3, "ERROR: MPM GPIO mapping failed "
                           "(Handle: 0x%x) (MPM Pin Num: %d) (GPIO ID: %u)",
                           s_mpmDalGpioHandle, 
                           mpmPinNum, 
                           gpioNum );
        CORE_VERIFY(0);
      }
    }
    else if( MPM_SUCCESS == vmpm_IsIrq(mpmPinNum) )
    {
      /* Inform about the IRQ wakeup. */
      if( DAL_SUCCESS != 
          DalInterruptController_MapWakeupInterrupt( s_mpmDalIcHandle, 
             (DALInterruptID)vmpm_pinMapTbl[mpmPinNum].local_pin_num, mpmPinNum ) )
      {
        vmpm_LogPrintf( 3, "ERROR: MPM IRQ mapping failed "
                           "(Handle: 0x%x) (MPM Pin Num: %d) (IRQ Num: %u)",
                           s_mpmDalGpioHandle, 
                           mpmPinNum, 
                           vmpm_pinMapTbl[mpmPinNum].local_pin_num );
        CORE_VERIFY(0);
      }
    }

  }

  /* Determine the number of IRQ's & GPIO's */
  vmpm_setNumMappedIrqs();
  vmpm_setNumMappedGpios();

  /* Register for the wakeup interrupt from the RPM. */
  vmpm_registerISR();
}

/**
 * @brief Register the wakeup ISR for the master
 */
void vmpm_registerISR( void )
{
  CORE_LOG_DAL_VERIFY( 
    DalInterruptController_RegisterISR( s_mpmDalIcHandle, 
                                        vmpm_wakeupIrq, 
                                        master_wakeup_isr, 
                                        (DALISRCtx)0, 
                                        vmpm_wakeupIrqFlags ),
    vmpm_LogPrintf( 0, "ERROR: vMPM wakeup interrupt registration failed" ) 
  );
}

/**
 * @brief De-register the wakeup ISR for the master
 */
void vmpm_deRegisterISR( void )
{
  CORE_LOG_DAL_VERIFY( 
    DalInterruptController_Unregister( s_mpmDalIcHandle, 
                                       vmpm_wakeupIrq ),
    vmpm_LogPrintf( 0, "ERROR: vMPM wakeup interrupt de-registration failed" ) 
  );
}

/**
 * vmpm_RetriggerGpio
 */
void vmpm_RetriggerGpio( uint32 gpioNum )
{
  /* Triggering GPIO on the master */
  if( DAL_SUCCESS != GPIOInt_TriggerInterrupt( s_mpmDalGpioHandle, gpioNum ) )
  {
    vmpm_LogPrintf( 1, "ERROR: MPM GPIO trigger failed (GPIO: %u)", 
                    gpioNum );
    CORE_VERIFY(0);
  }

  return;
}

/**
 * vmpm_RetriggerIrq
 */
void vmpm_RetriggerIrq( uint32 irqNum )
{
  /* Triggering interrupt on the master */
  if( DAL_SUCCESS != DalInterruptController_InterruptTrigger(  
                         s_mpmDalIcHandle, (DALInterruptID)irqNum ) )
  {
    vmpm_LogPrintf( 1, "ERROR: MPM interrupt trigger failed (IRQ: %u)", 
                    irqNum );
    CORE_VERIFY(0);
  }

  return;
}
