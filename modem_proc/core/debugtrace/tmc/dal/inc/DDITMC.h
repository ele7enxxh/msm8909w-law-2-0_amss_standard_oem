#ifndef DDITMC_H
#define DDITMC_H

//========================================================================
//
//                             Edit History
//
// $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/tmc/dal/inc/DDITMC.h#1 $
//
//========================================================================
//             Copyright (c) 2011 Qualcomm Technologies Incorporated.
//                    All Rights Reserved.
//                    QUALCOMM Proprietary
//========================================================================

#include "DalDevice.h"

#define DALTMC_STRING_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

typedef enum
{
        TMC_RAM_SIZE,
        TMC_READ_PTR,
        TMC_WRITE_PTR,
        TMC_TRIGGER_CTR,
   _PLACEHOLDER_TMCParamType = 0x7fffffff
} TMCParamType;


typedef enum
{
        TMC_TRACESINK_ETB,
        TMC_TRACESINK_RAM,
        TMC_TRACESINK_USB,
   _PLACEHOLDER_TraceSinkMode = 0x7fffffff
} TMCTraceSinkMode;


                     
typedef struct DalTMC DalTMC;
struct DalTMC
{
   struct DalDevice DalDevice;
   DALResult (*GetMode)(DalDeviceHandle * _h,TMCTraceSinkMode *pMode);
   DALResult (*SetMode)(DalDeviceHandle * _h,TMCTraceSinkMode mode);
   DALResult (*GetTraceBuffer)(DalDeviceHandle * _h,uint32 *pdwAddr, uint32 *pdwBufLen);
   DALResult (*SetTraceBuffer)(DalDeviceHandle * _h,uint32 dwAddr, uint32 dwBufLen);

   DALResult (*EnableTrace)(DalDeviceHandle * _h);
   DALResult (*DisableTrace)(DalDeviceHandle * _h);
   DALResult (*ReadTrace)(DalDeviceHandle * _h, uint32  addr, byte * buf, int  nBufLen, int * pnLenRead);
   DALResult (*Flush)(DalDeviceHandle * _h, int  flags);
   DALResult (*GetParam)(DalDeviceHandle * _h, TMCParamType  param, uint32  uVal);
   DALResult (*SetParam)(DalDeviceHandle * _h, TMCParamType  param, uint32  uVal);
};

typedef struct DalTMCHandle DalTMCHandle; 
struct DalTMCHandle 
{
   uint32 dwDalHandleId;
   const DalTMC * pVtbl;
   void * pClientCtxt;
};

#define DAL_TMCDeviceAttach(DevId,hDalDevice)\
        DAL_StringDeviceAttachEx(NULL,DevId,DALTMC_STRING_INTERFACE_VERSION,hDalDevice)


static __inline DALResult
DalTMC_GetMode(DalDeviceHandle * _h,TMCTraceSinkMode *pMode)
{
   return ((DalTMCHandle *)_h)->pVtbl->GetMode( _h,pMode);
}

static __inline DALResult
DalTMC_SetMode(DalDeviceHandle * _h,TMCTraceSinkMode mode)
{
   return ((DalTMCHandle *)_h)->pVtbl->SetMode( _h, mode);
}

static __inline DALResult
DalTMC_GetTraceBuffer(DalDeviceHandle * _h,uint32 *pdwAddr, uint32 *pdwBufLen)
{
   return ((DalTMCHandle *)_h)->pVtbl->GetTraceBuffer(_h,pdwAddr,pdwBufLen);
}

static __inline DALResult
DalTMC_SetTraceBuffer(DalDeviceHandle * _h,uint32 dwAddr, uint32 dwBufLen)
{
   return ((DalTMCHandle *)_h)->pVtbl->SetTraceBuffer( _h,dwAddr,dwBufLen);
}



static __inline DALResult
DalTMC_EnableTrace(DalDeviceHandle * _h)
{
   return ((DalTMCHandle *)_h)->pVtbl->EnableTrace( _h);
}


static __inline DALResult
DalTMC_DisableTrace(DalDeviceHandle * _h)
{
   return ((DalTMCHandle *)_h)->pVtbl->DisableTrace( _h);
}


static __inline DALResult
DalTMC_ReadTrace(DalDeviceHandle * _h, uint32  addr, byte * buf, int  nBufLen, int * pnLenRead)
{
   return ((DalTMCHandle *)_h)->pVtbl->ReadTrace( _h, addr, buf, nBufLen, pnLenRead);
}

#define TMC_FLUSH_NOBLOCK 0
#define TMC_FLUSH_BLOCK   1

static __inline DALResult
DalTMC_Flush(DalDeviceHandle * _h, int  flags)
{
   return ((DalTMCHandle *)_h)->pVtbl->Flush( _h, flags);
}


static __inline DALResult
DalTMC_GetParam(DalDeviceHandle * _h, TMCParamType  param, uint32  uVal)
{
   return ((DalTMCHandle *)_h)->pVtbl->GetParam( _h, param, uVal);
}


static __inline DALResult
DalTMC_SetParam(DalDeviceHandle * _h, TMCParamType  param, uint32  uVal)
{
   return ((DalTMCHandle *)_h)->pVtbl->SetParam( _h, param, uVal);
}


#endif
