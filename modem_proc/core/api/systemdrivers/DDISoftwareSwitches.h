#ifndef __DALISOFTWARESWITCHES_H__
#define __DALISOFTWARESWITCHES_H__
/*==============================================================================

FILE:      DDISoftwareSwitches.h

DESCRIPTION: This file contanis the API signatures for the Software Switches
             DAL.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/DDISoftwareSwitches.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/09   aratin  Created to separate Dip switches from legacy bio and 
                   consolidate in the remaining DIAG dip switch functionality.
==============================================================================*/
#include "DalDevice.h"

#define DALSOFTWARESWITCHES_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)
/*
** The following switch enumerations are used by other modules set various driver 
** functionalities in software. Please note that these switches have no hardware 
** components
*/
typedef enum {

  DAL_SOFTWARE_SWITCH_1     = 0x80,   /* SWITCH 1 IF loopback*/                                                  
  DAL_SOFTWARE_SWITCH_2     = 0x40,   /* SWITCH 2 Reload reverse link long code each 80-ms frame */              
  DAL_SOFTWARE_SWITCH_3     = 0x20,   /* SWITCH 3 Forward link scrambling disable */                             
  DAL_SOFTWARE_SWITCH_4     = 0x10,   /* SWITCH 4 Forward link puncturing disable */                             
  DAL_SOFTWARE_SWITCH_5     = 0x08,   /* SWITCH 5 Reverse link power control disable ie. switch to disable close loop power */
  DAL_SOFTWARE_SWITCH_6     = 0x04,   /* SWITCH 6 Turn Tx power on during acquisition and sync channel */        
  DAL_SOFTWARE_SWITCH_7     = 0x02,   /* SWITCH 7 Sounder mode */                                                
  DAL_SOFTWARE_SWITCH_8     = 0x01,   /* SWITCH 8 Force reverse link voice to full rate */                       
  DAL_SOFTWARE_SWITCH_9     = 0x0100, /* SWITCH 9 Turn off Sleep mode operation */                               
  DAL_SOFTWARE_SWITCH_10    = 0x0200, /* SWITCH 10 Turn off Vocoder */ 
  DAL_SOFTWARE_SWITCH_11    = 0x0400,
  DAL_SOFTWARE_SWITCH_12    = 0x0800,
  DAL_SOFTWARE_SWITCH_13    = 0x1000,
  DAL_SOFTWARE_SWITCH_14    = 0x2000,
  DAL_SOFTWARE_SWITCH_15    = 0x4000,
  DAL_SOFTWARE_SWITCH_16    = 0x8000, 
  DAL_MAX_NUMBER_OF_SWITCHES = 0xFFFF,
  DAL_PLACEHOLDER_DALSoftwareSwitchType = 0x7fffffff
}DALSoftwareSwitchType;

typedef struct DalSoftwareSwitches DalSoftwareSwitches;
struct DalSoftwareSwitches
{
   DalDevice DalDevice;
   DALResult (*SetSwitch)(DalDeviceHandle * _h, uint32  software_switch);
   DALResult (*ClearSwitch)(DalDeviceHandle * _h, uint32  software_switch);
   DALResult (*SetAll)(DalDeviceHandle * _h, uint32  software_switch);
   DALResult (*GetMask)(DalDeviceHandle * _h, uint32  software_switch,  void * state, uint32  size);
};

typedef struct DalSoftwareSwitchesHandle DalSoftwareSwitchesHandle; 
struct DalSoftwareSwitchesHandle 
{
   uint32 dwDalHandleId;
   const DalSoftwareSwitches * pVtbl;
   void * pClientCtxt;
};

#define DAL_SoftwareSwitchesDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALSOFTWARESWITCHES_INTERFACE_VERSION,hDalDevice)

static __inline DALResult
DalSoftwareSwitches_SetSwitch(DalDeviceHandle * _h, uint32  software_switch)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((DalSoftwareSwitchesHandle *)_h)->pVtbl, SetSwitch ), _h, software_switch);
   }
   return ((DalSoftwareSwitchesHandle *)_h)->pVtbl->SetSwitch( _h, software_switch);
}
static __inline DALResult
DalSoftwareSwitches_ClearSwitch(DalDeviceHandle * _h, uint32  software_switch)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((DalSoftwareSwitchesHandle *)_h)->pVtbl, ClearSwitch ), _h, software_switch);
   }
   return ((DalSoftwareSwitchesHandle *)_h)->pVtbl->ClearSwitch( _h, software_switch);
}
static __inline DALResult
DalSoftwareSwitches_SetAll(DalDeviceHandle * _h, uint32  software_switch)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((DalSoftwareSwitchesHandle *)_h)->pVtbl, SetAll ), _h, software_switch);
   }
   return ((DalSoftwareSwitchesHandle *)_h)->pVtbl->SetAll( _h, software_switch);
}
static __inline DALResult
DalSoftwareSwitches_GetMask(DalDeviceHandle * _h, uint32  software_switch,  void * state)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_6(DALVTBLIDX(((DalSoftwareSwitchesHandle *)_h)->pVtbl, GetMask ), _h, software_switch, (void * )state, 1);
   }
   return ((DalSoftwareSwitchesHandle *)_h)->pVtbl->GetMask( _h, software_switch, state, 1);
}
#endif
