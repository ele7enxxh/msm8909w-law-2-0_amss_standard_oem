/*=============================================================================	
   Copyright (c) 20011 Qualcomm Technologies Incorporated.	
   All Rights Reserved.
	$Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/stm/dal/inc/DDISTMCfg.h#1 $
=============================================================================*/
#ifndef DDISTMCFG_H
#define DDISTMCFG_H

//========================================================================
//
//                   STM Configuration Driver Interface
//
// @file ddistmcfg.ddi
//
//   This file contains the definitions of the constants, types, and
//   interfaces that comprise the interface to the STM Configuration 
//   DAL driver.
//
//========================================================================
//
//                             Edit History
//
// $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/stm/dal/inc/DDISTMCfg.h#1 $
//
// when       who     what, where, why
// --------   ---     ----------------------------------------------------
// 05/26/11   ts     Initial revision.
//
//========================================================================
//             Copyright © 2011 Qualcomm Technologies Incorporated.
//                    All Rights Reserved.
//                    QUALCOMM Proprietary
//========================================================================


#include "DalDevice.h"
#include "DALDeviceId.h"

#define DALSTMCFG_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

typedef enum
{

   STM_SW_TRACE_EN,           /* Enable 1, Disable 0 */
   STM_HW_EVT_EN,             /* Enable 1, Disable 0 */
   STM_TRIGGER_EN,            /* Enable 1, Disable 0 & Others TBD */
   STM_COMPRESSION_EN,        /* Enable 1, Disable 0 */ 
   STM_TIMESTAMP_EN,          /* Enable 1, Disable 0 */
   STM_TIMESTAMP_FREQ,        /* Integer value of Freq to be used */
   STM_TIMESTAMP_FREQ_PKTGEN, /* Generate a freq trace packet */ 
   STM_SYNC_EN,               /* Enable 1, Disable 0 */
   STM_SYNC_PERIOD_N,         /* N bytes */
   STM_SYNC_PERIOD_2POW_N,    /* 2^N bytes */

   _PLACEHOLDER_STMControlType = 0x7fffffff
}eSTMControlType;

typedef uint32 STMControlType;
                   
typedef struct DalSTMCfg DalSTMCfg;
struct DalSTMCfg
{
   struct DalDevice DalDevice;
   DALResult (*STMControl)(DalDeviceHandle * _h, STMControlType  uCtrl, uint32  uVal);
};

typedef struct DalSTMCfgHandle DalSTMCfgHandle; 
struct DalSTMCfgHandle 
{
   uint32 dwDalHandleId;
   const DalSTMCfg * pVtbl;
   void * pClientCtxt;
};

#define DAL_STMCfgDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALSTMCFG_INTERFACE_VERSION,hDalDevice)

static __inline DALResult
DalSTMCfg_STMControl(DalDeviceHandle * _h, STMControlType  uCtrl, uint32  uVal)
{
   return ((DalSTMCfgHandle *)_h)->pVtbl->STMControl( _h, uCtrl, uVal);
}


#endif
