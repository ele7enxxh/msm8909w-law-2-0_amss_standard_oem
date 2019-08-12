/*=============================================================================
  Copyright (c) 2009 Qualcomm Technologies Incorporated.
           All Rights Reserved.

                  $Header: //components/rel/core.mpss/3.5.c12.3/dal/drivers/TestDrivers/StringDevice/src/DalTestString.c#1 $
=============================================================================*/

#include "DALFramework.h"
#include "DALDevice.h"
#include "DDIStringDevice.h"
#include "string.h" //for memset warning
#include <stringl/stringl.h>

/*------------------------------------------------------------------------------
Declaring a "String" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct StringDrvCtxt StringDrvCtxt;
typedef struct StringDevCtxt StringDevCtxt;
typedef struct StringClientCtxt StringClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "String" Vtable
------------------------------------------------------------------------------*/
typedef struct StringDALVtbl StringDALVtbl;
struct StringDALVtbl
{
   int (*String_DriverInit)(StringDrvCtxt *);
   int (*String_DriverDeInit)(StringDrvCtxt *);
};

struct StringDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   StringDrvCtxt  *pStringDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   void *hDALSystem;
   void *hDALTimer;
   void *hDALInterrupt;
   void * pSystemTbl;
   void * pDefaulrkLoop;
   const char *strDeviceName;
   uint32 Reserved[16-6];
};

struct StringDrvCtxt
{
   //Base Members
   StringDALVtbl StringDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   StringDevCtxt StringDevCtxt[3];
   //String Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "String" Client Context
------------------------------------------------------------------------------*/
struct StringClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   StringDevCtxt *pStringDevCtxt;
   DalStringDeviceHandle DalStringDeviceHandle;
   //String Client state can be added by developers here
};




/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
String_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/

//static DALDEVICEID DalString_DeviceId[1] = {0xDEADBEEF};
const DALREG_DriverInfo
DALString_DriverInfo = { String_DeviceAttach,0, NULL };


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/


static uint32
String_AddRef(DalStringDeviceHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult
String_DriverInit(StringDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

DALResult
String_DriverDeInit(StringDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
String_DeviceDetach(DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
    if(0 == dwref)  {
    //Release the client ctxt;
       DALSYS_Free(h->pClientCtxt);
   }
   return dwref;
}

static DALResult
String_DeviceInit(DalDeviceHandle *h)
{
   return DAL_SUCCESS;
}

static DALResult
String_DeviceDeInit(DalDeviceHandle *h)
{
   return DAL_SUCCESS;
}

static DALResult
String_DevicePowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
String_DeviceOpen(DalDeviceHandle* h, uint32 mode)
{
   return DAL_SUCCESS;
}

static DALResult
String_DeviceClose(DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
String_DeviceInfo(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALSTRINGDEVICE_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
String_DeviceSysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalStringDevice Interface.
------------------------------------------------------------------------------*/



static DALResult 
String_ReadTest( DalDeviceHandle * h, void * dest_buffer, uint32  blen) 
{
   StringClientCtxt *pClientCtxt = ((DalStringDeviceHandle *)h)->pClientCtxt;
   if(strlcpy((char *)dest_buffer,pClientCtxt->pStringDevCtxt->strDeviceName,blen) < blen)
      return DAL_SUCCESS;
   else
      return DAL_ERROR;
}


static DALResult 
String_WriteTest( DalDeviceHandle * h, void * src_buffer, uint32  blen) 
{
   return DAL_SUCCESS;
}

static void
String_InitInterface(StringClientCtxt* pclientCtxt)
{
   static const DalStringDevice vtbl =
   {
      {
         String_DeviceAttach,
         String_DeviceDetach,
         String_DeviceInit,
         String_DeviceDeInit,
         String_DeviceOpen,
         String_DeviceClose,
         String_DeviceInfo,
         String_DevicePowerEvent,
         String_DeviceSysRequest
      },
   
      String_ReadTest,
      String_WriteTest
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalStringDeviceHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalStringDeviceHandle.pVtbl  = &vtbl;
   pclientCtxt->DalStringDeviceHandle.pClientCtxt = pclientCtxt;
}

DALResult
String_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{

   DALResult nErr;

   static StringDrvCtxt drvCtxt = {{String_DriverInit,
                                      String_DriverDeInit
                                     },3,
                                     sizeof(StringDevCtxt)};

   StringClientCtxt *pclientCtxt =  NULL;
          
   nErr = DALSYS_Malloc(sizeof(StringClientCtxt),(void **)&pclientCtxt); //Malloc will error fatal upon failure
   DALSYS_memset(pclientCtxt,0,sizeof(StringClientCtxt));
   *phDalDevice = NULL;
   nErr = DALFW_AttachToStringDevice(pszArg, (DALDrvCtxt *)&drvCtxt, (DALClientCtxt *)pclientCtxt); 
   if (DAL_SUCCESS == nErr)
   {
      String_InitInterface(pclientCtxt);
      String_AddRef(&(pclientCtxt->DalStringDeviceHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalStringDeviceHandle);
   }
   return nErr;
}



