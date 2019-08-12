#ifndef DDITFUNNEL_H
#define DDITFUNNEL_H

/*=============================================================================
   Copyright (c) 2011 Qualcomm Technologies Incorporated.
   All Rights Reserved.
=============================================================================*/

//========================================================================
//
//                   STM Configuration Driver Interface
//
// @file dditfunnel.ddi
//
//   This file contains the definitions of the constants, types, and
//   interfaces that comprise the interface to the Trace Funnel
//   DAL driver.
//
//========================================================================
//
//                             Edit History
//
// $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/tfunnel/dal/inc/DDITFunnel.h#1 $
//
// when       who     what, where, why
// --------   ---     ----------------------------------------------------
// 07/22/11   ps      Initial revision.
//
//========================================================================
//             Copyright © 2011 Qualcomm Technologies Incorporated.
//                    All Rights Reserved.
//                    QUALCOMM Proprietary
//========================================================================


#include "DalDevice.h"
#include "DALStdDef.h"



#define DALTFUNNEL_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

typedef enum
{
   TFUNNEL_PORT_ENABLE,
   TFUNNEL_PORT_PRIORITY,
   TFUNNEL_PORT_STATUS,
   _PLACEHOLDER_TFunnelControlType = 0x7fffffff
}TFunnelControlType;

#define TFUNNEL_STM_PORT "port_stm"
#define TFUNNEL_KRAIT0_PFT_PORT "port_krait0"
#define TFUNNEL_ITM_PORT "port_itm"


typedef struct {
   const char *pPortStr;
   union {
      DALBOOL bEnable;
      uint32 nPrio;
   }Val;
} TFunnelControlParam;


typedef struct DalTFunnel DalTFunnel;
struct DalTFunnel
{
   struct DalDevice DalDevice;
   DALResult (*Control)(DalDeviceHandle * _h, TFunnelControlType  eCtrl,
    TFunnelControlParam *pCtrlParam);
};

typedef struct DalTFunnelHandle DalTFunnelHandle;
struct DalTFunnelHandle
{
   uint32 dwDalHandleId;
   const DalTFunnel * pVtbl;
   void * pClientCtxt;
};

#define DAL_TFunnelDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALTFUNNEL_INTERFACE_VERSION,hDalDevice)

static __inline DALResult
DalTFunnel_Control(DalDeviceHandle * _h, TFunnelControlType  eCtrl,
 TFunnelControlParam *pCtrlParam)
{
   return ((DalTFunnelHandle *)_h)->pVtbl->Control( _h, eCtrl,pCtrlParam);
}


#endif
