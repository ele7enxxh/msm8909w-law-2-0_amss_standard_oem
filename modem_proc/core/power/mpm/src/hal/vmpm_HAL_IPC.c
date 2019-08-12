/*============================================================================
  FILE:         vmpm_HAL_IPC.c
 
  OVERVIEW:     This file contains the function implementation to send an IPC
                to RPM whenever vMPM configuration is changed.
 
  DEPENDENCIES: None
 
                Copyright (c) 2012-2014 QUALCOMM Technologies.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/hal/vmpm_HAL_IPC.c#1 $
============================================================================*/

#include "DDIIPCInt.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "CoreVerify.h"

static DalDeviceHandle *mpm_dal_ipc_handle = NULL;

/**
 * vmpm_HAL_SetupIPC
 */
void vmpm_HAL_SetupIPC(void)
{
  /* Obtaining a DAL handle for IPC interrupt */
  CORE_DAL_VERIFY( DAL_DeviceAttach( DALDEVICEID_IPCINT, 
                                     &mpm_dal_ipc_handle ) );
}

void vmpm_send_interrupt(void)
{
  /* Sending an IPC interrupt to RPM */
  if( NULL != mpm_dal_ipc_handle )
  {
    DalIPCInt_Trigger( mpm_dal_ipc_handle, /* DAL handle for IPC int */
                       DALIPCINT_PROC_RPM, /* Proc to trigger interrupt on */
                       DALIPCINT_GP_MED ); /* Interrupt Id */
  }
  else
  {
    /* An attempt was made to directly send an IPC interrupt to RPM 
     * without getting DAL handle. */
    CORE_VERIFY(0);
  }
}
