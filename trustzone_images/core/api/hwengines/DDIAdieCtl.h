#ifndef __DDIADIECTL_H__
#define __DDIADIECTL_H__

/*===============================================================================

 FILE: DDIAdieCtl.h

 DESCRIPTION:  
   AdieCtl Device Interface

===============================================================================

                             Edit History

 $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/hwengines/DDIAdieCtl.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/24/09   nk      Fix to get DalRemoteInterface working with AdieCtl driver
04/17/09   nk      Added history and HEADER tag, changed ifndef to DDIADIECTL
03/13/09   nk      Created

===============================================================================
                   Copyright © 2008 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===============================================================================*/
#include "DalDevice.h"
#include "AdieCtlRegDefs.h" 

#define DALADIECTL_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)


typedef enum DALAdieCtlOpStatus DALAdieCtlOpStatus;
enum  DALAdieCtlOpStatus
{
	ADIECTL_SUCCESS                     =       0x00, 
	ADIECTL_ERR_NOT_REGISTERED          =       0x01,
	ADIECTL_ERR_READ                    =       0x02,
	ADIECTL_ERR_WRITE                   =       0x03,
	ADIECTL_ERR_NO_ACCESS               =       0x04,
	ADIECTL_ERR_PARAMETER_INVALID       =       0x05,
	ADIECTL_ERR_UNKNOWN                 = 0x7FFFFFFF 
};


typedef enum DALAdieCtlClientId DALAdieCtlClientId;
enum  DALAdieCtlClientId
{
	ADIECTL_BT              =       0x01, 
	ADIECTL_FM              =       0x02,
	ADIECTL_CDC             =       0x03,
	ADIECTL_TSADC           =       0x04,
	ADIECTL_CLIENT_INVALID  = 0x7FFFFFFF 
};


typedef struct DALAdieCtlClientDesc DALAdieCtlClientDesc; 
struct DALAdieCtlClientDesc
{
	DALAdieCtlClientId  clientId;       		
};

typedef struct DALAdieCtlReadWriteData DALAdieCtlReadWriteData; 
struct DALAdieCtlReadWriteData
{
	uint32  uAddress; 
	uint32  udata;   
	uint32  uMask;
};

typedef struct DalAdieCtl DalAdieCtl;
struct DalAdieCtl
{
   DalDevice DalDevice;
   DALResult (*Register)(DalDeviceHandle * _h,  const DALAdieCtlClientDesc * pClientDesc, uint32 ilen1);
   DALResult (*Write)(DalDeviceHandle * _h,  const DALAdieCtlReadWriteData * paWriteData,  const uint32  nElements);
   DALResult (*Read)(DalDeviceHandle * _h,  const DALAdieCtlReadWriteData * paReadData,  const uint32  nReadDataElements,  uint32 * paResult,  const uint32  nResultElements);
};

typedef struct DalAdieCtlHandle DalAdieCtlHandle; 
struct DalAdieCtlHandle 
{
   uint32 dwDalHandleId;
   const DalAdieCtl * pVtbl;
   void * pClientCtxt;
};

#define DAL_AdieCtlDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALADIECTL_INTERFACE_VERSION,hDalDevice)

static __inline DALResult
DalAdieCtl_Register(DalDeviceHandle * _h,  const DALAdieCtlClientDesc * pClientDesc)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_5(DALVTBLIDX(((DalAdieCtlHandle *)_h)->pVtbl, Register ), _h, (void * )pClientDesc,sizeof(  DALAdieCtlClientDesc ));
   }
   return ((DalAdieCtlHandle *)_h)->pVtbl->Register( _h, pClientDesc,sizeof(  DALAdieCtlClientDesc ));
}
static __inline DALResult
DalAdieCtl_Write(DalDeviceHandle * _h,  const DALAdieCtlReadWriteData * paWriteData,  const uint32  nElements)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_5(DALVTBLIDX(((DalAdieCtlHandle *)_h)->pVtbl, Write ), _h, (void * )paWriteData, nElements*sizeof( DALAdieCtlReadWriteData ));
   }
   return ((DalAdieCtlHandle *)_h)->pVtbl->Write( _h, paWriteData, nElements*sizeof( DALAdieCtlReadWriteData ));
}
static __inline DALResult
DalAdieCtl_Read(DalDeviceHandle * _h,  const DALAdieCtlReadWriteData * paReadData,  const uint32  nReadDataElements,  uint32 * paResult,  const uint32  nResultElements)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_8(DALVTBLIDX(((DalAdieCtlHandle *)_h)->pVtbl, Read ), _h, (void * )paReadData, nReadDataElements * sizeof( DALAdieCtlReadWriteData ), (void * )paResult, nResultElements * sizeof( uint32 ));
   }
   return ((DalAdieCtlHandle *)_h)->pVtbl->Read( _h, paReadData, nReadDataElements * sizeof( DALAdieCtlReadWriteData ), paResult, nResultElements * sizeof( uint32 ));
}
#endif
