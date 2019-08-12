/*===========================================================================
  FILE:         vmpm_target.c
  
  OVERVIEW:     This file contains the function that obtains target specific
                data for mpm driver and assigns them to various structures
                variables so that common driver can use them.

  DEPENDENCY:   DevCfg support. If not available, we may need other 
                implementation of function that initializes target specific
                data.

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/os/default/vmpm_target.c#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 

#include "CoreVerify.h"
#include "DalDevice.h"            /* Header files for DevCfg queries */
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DALSysTypes.h"
#include "DDIHWIO.h"              /* HWIO mapping */
#include "vmpm_internal.h"        /* MPM driver header files */
#include "msmhwiobase.h"
#include "vmpm_src_util.h"

/*===========================================================================
 *                      STATIC VARIABLE DEFINITIONS
 *===========================================================================*/
/** After initialization, this variable will be contain the Virtual Address
 * in message ram for master's mpm configuration registers base.
 * It is this variable that is updated or accessed by the mutator & accessor 
 * functions. */ 
static uint32 s_mpmCfgBaseAddr_VA = 0;

/*==========================================================================
 *                      GLOBAL VARIABLES DECLARATIONS
 *==========================================================================*/

/* After initialization, this variable will contain the base address (virtual
 * or physical) of mpm hw registers in memory. */
uint8* vmpm_HwBaseAddr = NULL;

/* Pointer to mpm<->master interrupt mapping table */
mpm_int_pin_mapping_type *vmpm_pinMapTbl = NULL;

/* Virtual Address in message ram for master's mpm configuration registers base. */
uint32 vmpm_IrqCfgRegBase_VA = 0;

/* RPM wakeup interrupt id */
uint32 vmpm_wakeupIrq = 0;

/* RPM wakeup interrupt flag */
uint32 vmpm_wakeupIrqFlags = 0;

/*==========================================================================
 *                      STATIC FUNCTIONS
 *==========================================================================*/
static void vmpm_findProcVAfromPA( uint32 processorStartPA );
static void vmpm_setProcMpmBaseVA( uint32 proc_mpmBaseVA );

/* Mutator function for static variable s_mpmCfgBaseAddr_VA */
static void vmpm_setProcMpmBaseVA( uint32 proc_mpmBaseVA )
{
   s_mpmCfgBaseAddr_VA =  proc_mpmBaseVA;
}

/**
 * Calculate the Processor's MPM RAM Virtual Address from the 
 * corresponding Physical Address.  
 */
static void vmpm_findProcVAfromPA( uint32 mpmProcRegionStartPA )
{
  uint32 RPM_RAM_StartPA = RPM_SS_MSG_RAM_START_ADDRESS_BASE_PHYS;
  uint32 RPM_RAM_EndVA = 0;
  uint32 RPM_RAM_BaseSize = RPM_SS_MSG_RAM_START_ADDRESS_BASE_SIZE;
  uint32 rpmProcMpmPA_offset;
  DalDeviceHandle *mpmRpmRamDalHandle;

  /* After initialization, this variable will contain the Virtual Address 
   * corresponding to the RPM's shared RAM Base Physical Address */
  uint32 rpmRamBaseVA = 0x0;

  /* Calculate the RPM RAM Physical Addr upper boundary */
  uint32 rpmRamEndPA = RPM_RAM_StartPA + RPM_RAM_BaseSize;

  /* Halt if RPM RAM start addr + size > UINT32 MAX */
  if ( rpmRamEndPA < RPM_RAM_StartPA )
  {
    CORE_VERIFY(0);
  }

  /* Calculate the physical address offset of master MPM Region
   * relative to the RPM RAM start address */
  rpmProcMpmPA_offset = 
               safe_uint32_subtract( mpmProcRegionStartPA,
                                     RPM_RAM_StartPA );
  /* Determine the RPM RAM Base adddress virtual address */
  CORE_DAL_VERIFY( DAL_DeviceAttach( DALDEVICEID_HWIO, &mpmRpmRamDalHandle ) );
  CORE_DAL_VERIFY( DalHWIO_MapRegion( mpmRpmRamDalHandle, 
                                      "RPM_SS_MSG_RAM_START_ADDRESS", 
                                      (uint8 **) &rpmRamBaseVA ) );

  /* Add Processor's address offset to the RPM RAM base address
   * to get the processor's MPM virtual addr (VA) region start */
  uint32 proc_mpmBaseVA = safe_uint32_addition( rpmProcMpmPA_offset, 
                                                rpmRamBaseVA);

  /* Calculate the RPM RAM Virtual Addr upper boundary */
  RPM_RAM_EndVA = safe_uint32_addition( rpmRamBaseVA,
                                        RPM_RAM_BaseSize);

  /* Perform bounds check on result.  If processor's virtual address
   * is outside RPM RAM virtual address range,
   * log an error and halt processor */ 
  if ( proc_mpmBaseVA <= RPM_RAM_EndVA ) 
  {
    vmpm_setProcMpmBaseVA( proc_mpmBaseVA );
    vmpm_LogPrintf( 1,
                    "MPM_SUCCESS: RAM Address within expected addr range: %d",
                    proc_mpmBaseVA );
  }
  else
  {
    vmpm_LogPrintf( 1,
                    "MPM_FAIL: RAM Address out of range error: %d",
                    proc_mpmBaseVA );
    CORE_VERIFY(0);
  }
}

/*==========================================================================
 *                      GLOBAL FUNCTIONS
 *==========================================================================*/
/* Accessor function for static variable s_mpmCfgBaseAddr_VA */
uint32 vmpm_getProcMpmBaseVA( void )
{
   return s_mpmCfgBaseAddr_VA;
}

/*
 * Obtains target specific data and use them to intialize various variables.
 */
void vmpm_TargetDataInit( void )
{
  DALSYS_PROPERTY_HANDLE_DECLARE(mpmDevCfgHandle);
  DALSYSPropertyVar prop;

  /* Physical Address in message RAM for processor's MPM config register base */
  uint32 mpmProcRegionStartPA = 0x0;

  /* Obtaining a DAL handle for our driver */
  CORE_DAL_VERIFY( DALSYS_GetDALPropertyHandleStr( "/dev/core/power/mpm", 
                                                   mpmDevCfgHandle ) );

  /* Set pointer to mpm mapping table */
  CORE_DAL_VERIFY( DALSYS_GetPropertyValue( mpmDevCfgHandle, 
                                            "vmpm_pinNumMappingTable", 
                                            0, &prop ) );
  vmpm_pinMapTbl = (mpm_int_pin_mapping_type *)prop.Val.pStruct;

  /* Determine the MPM IRQ/GPIO table size */
  vmpm_setMapTblSize();

  /* Get the Physical Address (PA) of the processors MPM region in RPM RAM */
  CORE_DAL_VERIFY( DALSYS_GetPropertyValue( mpmDevCfgHandle, 
                                            "vmpm_procRegionStartPA", 
                                            0, &prop ) );
  mpmProcRegionStartPA = (uint32)prop.Val.dwVal;

  /* Determine the processors MPM RAM Virtual Address (VA) from PA */
  vmpm_findProcVAfromPA( mpmProcRegionStartPA );

  vmpm_IrqCfgRegBase_VA = vmpm_getProcMpmBaseVA();
  
  /* Getting rpm wakeup interrupt id */
  CORE_DAL_VERIFY( DALSYS_GetPropertyValue( mpmDevCfgHandle, 
                                            "vmpm_wakeupIrq", 
                                            0, &prop ) );
  vmpm_wakeupIrq = (uint32)prop.Val.dwVal;

  /* Getting rpm wakeup interrupt trigger type */
  CORE_DAL_VERIFY( DALSYS_GetPropertyValue( mpmDevCfgHandle, 
                                            "vmpm_wakeupIrqFlags", 
                                            0, &prop ) );
  vmpm_wakeupIrqFlags = (uint32)prop.Val.dwVal;
}

/**
 * Maps the MPM HW to memory and sets the global variable vmpm_HwBaseAddr to
 * contain that virtual address.
 */
void vmpm_MapHwRegBase( void )
{
  DalDeviceHandle *mpmHw_DAL_Handle;

  CORE_DAL_VERIFY( DAL_DeviceAttach( DALDEVICEID_HWIO, &mpmHw_DAL_Handle ) );
  CORE_DAL_VERIFY( DalHWIO_MapRegion( mpmHw_DAL_Handle, 
                                      "MPM2_MPM", &vmpm_HwBaseAddr ) );
}
