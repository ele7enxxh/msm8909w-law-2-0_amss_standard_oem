/*==============================================================================

FILE:      DalTimetickFwk.c

DESCRIPTION: This file implements a Timetick DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2014 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DalTimetick.h"
#include "string.h" //for memset warning
#include <stringl/stringl.h>

DALResult 
Timetick_DalTimetick_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
Timetick_DalTimetick_AddRef(DalTimetickHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
Timetick_DalTimetick_Detach(DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	return dwref;
}

static DALResult 
Timetick_DalTimetick_Init(DalDeviceHandle *h)
{  
   return Timetick_DeviceInit(h->pClientCtxt);
}

static DALResult 
Timetick_DalTimetick_DeInit(DalDeviceHandle *h)
{
   return Timetick_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
Timetick_DalTimetick_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return Timetick_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
Timetick_DalTimetick_Open(DalDeviceHandle* h, uint32 mode)
{
    return Timetick_Open(h->pClientCtxt,mode);
}

static DALResult 
Timetick_DalTimetick_Close(DalDeviceHandle* h)
{
    return Timetick_Close(h->pClientCtxt);
}

static DALResult 
Timetick_DalTimetick_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALTIMETICK_INTERFACE_VERSION;
   return Timetick_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
Timetick_DalTimetick_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTimetick Interface. 
------------------------------------------------------------------------------*/
static DALResult 
Timetick_DalTimetick_GetMinRearmSclk( DalDeviceHandle * h,  DalTimetickTime32Type   *pMinRearm)
{
	return Timetick_GetMinRearmSclk(((DalTimetickHandle *)h)->pClientCtxt, pMinRearm);
}
static DALResult 
Timetick_DalTimetick_CvtToTimetick32( DalDeviceHandle * h, DalTimetickTime32Type  time, DalTimetickUnitType  unit, DalTimetickTime32Type *  time_ret) 
{
	return Timetick_CvtToTimetick32(((DalTimetickHandle *)h)->pClientCtxt, time, unit, time_ret);
}
static DALResult 
Timetick_DalTimetick_CvtFromTimetick32( DalDeviceHandle * h, DalTimetickTime32Type  time, DalTimetickUnitType  unit, DalTimetickTime32Type *  time_ret) 
{
	return Timetick_CvtFromTimetick32(((DalTimetickHandle *)h)->pClientCtxt, time, unit, time_ret);
}
static DALResult 
Timetick_DalTimetick_GetRaw( DalDeviceHandle * h, DalTimetickTime32Type *  tick) 
{
	return Timetick_GetRaw(((DalTimetickHandle *)h)->pClientCtxt, tick);
}
static DALResult
Timetick_DalTimetick_UpdateFreq( DalDeviceHandle * h,   uint32 nFreq)
{
	return Timetick_UpdateFreq(((DalTimetickHandle *)h)->pClientCtxt, nFreq);
}

static DALResult 
Timetick_DalTimetick_InitTimetick64( DalDeviceHandle * h) 
{
	return Timetick_InitTimetick64(((DalTimetickHandle *)h)->pClientCtxt);
}
static DALResult 
Timetick_DalTimetick_GetTimetick64( DalDeviceHandle * h,  DalTimetickTime64Type * ticks) 
{
	return Timetick_GetTimetick64(((DalTimetickHandle *)h)->pClientCtxt, ticks);
}
static DALResult 
Timetick_DalTimetick_Enable( DalDeviceHandle * h, boolean  enable) 
{
	return Timetick_Enable(((DalTimetickHandle *)h)->pClientCtxt, enable);
}
static DALResult 
Timetick_DalTimetick_IsEnabled( DalDeviceHandle * h, boolean * pbIsEnabled) 
{
	return Timetick_IsEnabled(((DalTimetickHandle *)h)->pClientCtxt, pbIsEnabled);
}
static DALResult 
Timetick_DalTimetick_SetNextInterrupt( DalDeviceHandle * h, DalTimetickTime32Type  match_count, DalTimetickTime32Type  ticks_now,  DalTimetickTime32Type * match_val) 
{
	return Timetick_SetNextInterrupt(((DalTimetickHandle *)h)->pClientCtxt, match_count, ticks_now, match_val);
}
static DALResult 
Timetick_DalTimetick_SetOffset( DalDeviceHandle * h, DalTimetickTime32Type  timetick_delta) 
{
	return Timetick_SetOffset(((DalTimetickHandle *)h)->pClientCtxt, timetick_delta);
}
static DALResult 
Timetick_DalTimetick_GetOffset( DalDeviceHandle * h, DalTimetickTime32Type  *timetick_delta) 
{
	return Timetick_GetOffset(((DalTimetickHandle *)h)->pClientCtxt, timetick_delta);
}
static DALResult 
Timetick_DalTimetick_Get( DalDeviceHandle * h, DalTimetickTime32Type *  count) 
{
	return Timetick_Get(((DalTimetickHandle *)h)->pClientCtxt, count);
}
static DALResult 
Timetick_DalTimetick_GetElapsed( DalDeviceHandle * h, DalTimetickTime32Type  start, DalTimetickUnitType  unit, DalTimetickTime32Type *  diff) 
{
	return Timetick_GetElapsed(((DalTimetickHandle *)h)->pClientCtxt, start, unit, diff);
}
static DALResult 
Timetick_DalTimetick_Diff( DalDeviceHandle * h, DalTimetickTime32Type  start, DalTimetickTime32Type  end, DalTimetickUnitType  unit, DalTimetickTime32Type *  diff) 
{
	return Timetick_Diff(((DalTimetickHandle *)h)->pClientCtxt, start, end, unit, diff);
}
static DALResult 
Timetick_DalTimetick_GetMs( DalDeviceHandle * h, DalTimetickTime32Type *  ms) 
{
	return Timetick_GetMs(((DalTimetickHandle *)h)->pClientCtxt, ms);
}
static DALResult 
Timetick_DalTimetick_GetFreq( DalDeviceHandle * h, uint32  *freq) 
{
	return Timetick_GetFreq(((DalTimetickHandle *)h)->pClientCtxt, freq);
}
static DALResult 
Timetick_DalTimetick_CvtToTimetick64( DalDeviceHandle * h, DalTimetickTime64Type  time,  DalTimetickUnitType unit, DalTimetickTime64Type *time_ret)
{
	return Timetick_CvtToTimetick64(((DalTimetickHandle *)h)->pClientCtxt, time, unit, time_ret);
}
static DALResult 
Timetick_DalTimetick_CvtFromTimetick64( DalDeviceHandle * h, DalTimetickTime64Type  time,  DalTimetickUnitType unit, DalTimetickTime64Type *time_ret)
{
	return Timetick_CvtFromTimetick64(((DalTimetickHandle *)h)->pClientCtxt, time, unit, time_ret);
}
static DALResult 
Timetick_DalTimetick_UpdateBlastSigId( DalDeviceHandle * h, uint32  sig_id)
{
	return Timetick_UpdateBlastSigId(((DalTimetickHandle *)h)->pClientCtxt, sig_id);
}
static DALResult 
Timetick_DalTimetick_CvtToUsec( DalDeviceHandle * h, DalTimetickTime32Type time, DalTimetickUnitType unit, DalTimetickTime64Type * time_ret)
{
	return Timetick_CvtToUsec(((DalTimetickHandle *)h)->pClientCtxt, time, unit, time_ret );
}

static DALResult
Timetick_DalTimetick_CvtFromUsec( DalDeviceHandle * h, DalTimetickTime64Type time, DalTimetickUnitType unit, DalTimetickTime32Type * time_ret)
{
	return Timetick_CvtFromUsec(((DalTimetickHandle *)h)->pClientCtxt, time, unit, time_ret );
}
static DALResult 
Timetick_DalTimetick_GetMatchValue( DalDeviceHandle * h, DalTimetickTime32Type *  match_value) 
{
	return Timetick_GetMatchValue(((DalTimetickHandle *)h)->pClientCtxt, match_value);
}
static DALResult 
Timetick_DalTimetick_TicksToPrecUs( DalDeviceHandle * h, DalTimetickTime32Type  nTicks, uint64 *pus) 
{
	return Timetick_TicksToPrecUs(((DalTimetickHandle *)h)->pClientCtxt, nTicks, pus);
}
static DALResult 
Timetick_DalTimetick_SetNextInterrupt64( DalDeviceHandle * h, DalTimetickTime64Type  match_count, DalTimetickTime64Type  ticks_now,  DalTimetickTime64Type * match_val) 
{
	return Timetick_SetNextInterrupt64(((DalTimetickHandle *)h)->pClientCtxt, match_count, ticks_now, match_val);
}
static DALResult 
Timetick_DalTimetick_GetMatchValue64( DalDeviceHandle * h, DalTimetickTime64Type * match_val) 
{
	return Timetick_GetMatchValue64(((DalTimetickHandle *)h)->pClientCtxt, match_val);
}
static DALResult 
Timetick_DalTimetick_RegFreqChangeNotification( DalDeviceHandle * h, DALSYSCallbackFunc cbFunc, DALSYSCallbackFuncCtx cbFuncCtx)
{
	return Timetick_RegFreqChangeNotification(((DalTimetickHandle *)h)->pClientCtxt, cbFunc, cbFuncCtx);
}
static DALResult 
Timetick_DalTimetick_GetInterruptVectorNum( DalDeviceHandle * h, uint32 *num)
{
	return Timetick_GetInterruptVectorNum(((DalTimetickHandle *)h)->pClientCtxt, num);
}

static void 
Timetick_InitInterface(TimetickClientCtxt* pclientCtxt)
{
    static const DalTimetick vtbl = {
       {
          Timetick_DalTimetick_Attach,
          Timetick_DalTimetick_Detach,
          Timetick_DalTimetick_Init,
          Timetick_DalTimetick_DeInit,
          Timetick_DalTimetick_Open,
          Timetick_DalTimetick_Close,
          Timetick_DalTimetick_Info,
          Timetick_DalTimetick_PowerEvent,
          Timetick_DalTimetick_SysRequest
        } ,
        Timetick_DalTimetick_GetMinRearmSclk,
        Timetick_DalTimetick_CvtToTimetick32,
        Timetick_DalTimetick_CvtFromTimetick32,
        Timetick_DalTimetick_GetRaw,
        Timetick_DalTimetick_Get,
        Timetick_DalTimetick_GetElapsed,
        Timetick_DalTimetick_Diff,
        Timetick_DalTimetick_InitTimetick64,
        Timetick_DalTimetick_GetTimetick64,
        Timetick_DalTimetick_GetMs,
        Timetick_DalTimetick_UpdateFreq,
        Timetick_DalTimetick_SetOffset,
        Timetick_DalTimetick_GetOffset,
        Timetick_DalTimetick_SetNextInterrupt,
        Timetick_DalTimetick_Enable,
        Timetick_DalTimetick_IsEnabled,
        Timetick_DalTimetick_GetFreq,
        Timetick_DalTimetick_CvtToTimetick64,
        Timetick_DalTimetick_CvtFromTimetick64,
        Timetick_DalTimetick_UpdateBlastSigId,
        Timetick_DalTimetick_CvtToUsec,
        Timetick_DalTimetick_CvtFromUsec,
        Timetick_DalTimetick_GetMatchValue,
        Timetick_DalTimetick_TicksToPrecUs,
        Timetick_DalTimetick_SetNextInterrupt64,
        Timetick_DalTimetick_GetMatchValue64,
        Timetick_DalTimetick_RegFreqChangeNotification,
        Timetick_DalTimetick_GetInterruptVectorNum
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalTimetickHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalTimetickHandle.pVtbl  = &vtbl;
    pclientCtxt->DalTimetickHandle.pClientCtxt = pclientCtxt;
}

DALResult 
Timetick_DalTimetick_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
	static TimetickDrvCtxt drvCtxt = {{Timetick_DriverInit,
									   Timetick_DriverDeInit
									  },DALTIMETICK_NUM_DEVICES,
									  sizeof(TimetickDevCtxt)};
        static TimetickClientCtxt clientCtxt[DALTIMETICK_NUM_DEVICES];
    TimetickClientCtxt *pclientCtxt;

    switch(DeviceId)
    {
      case DALDEVICEID_QTIMER:
          pclientCtxt = &clientCtxt[0];

          nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
			                        (DALClientCtxt *)pclientCtxt);
          break;

        case 0:
           pclientCtxt = &clientCtxt[1];

           nErr = DALFW_AttachToStringDevice(pszArg, (DALDrvCtxt *)&drvCtxt, (DALClientCtxt *)pclientCtxt);
           if(nErr == DAL_SUCCESS)
           {
               strlcpy(pclientCtxt->pTimetickDevCtxt->strDeviceName, pszArg,DALTIMETICK_MAX_STRING_ID_LENGTH );
           }

           break;

      default:
          return DAL_ERROR;

    }

    *phDalDevice = NULL;

    if (DAL_SUCCESS == nErr)
    {
      Timetick_InitInterface(pclientCtxt);
	  Timetick_DalTimetick_AddRef(&(pclientCtxt->DalTimetickHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalTimetickHandle);
    }
    return nErr;
}

