/*
===========================================================================
*/
/**
  @file VCSFwk.c 
  
  DAL framework implementation file for the VCS device driver.
*/
/*  
  ====================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ====================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/vcs/src/VCSFwk.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------


  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/


#include "DALFramework.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDIVCS.h"
#include "VCSDriver.h"
#include "DDIHWIO.h"
#include "DALPropDef.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/


/*
 * Take a DAL driver handle and dereference into the driver context.
 */
#define HANDLE2CTXT(h) \
  &((VCSDALClientCtxt *)(h->pClientCtxt))->pVCSDevCtxt->pVCSDrvCtxt->DrvCtxt


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Driver, device and client contexts
 */
typedef struct VCSDALDrvCtxt    VCSDALDrvCtxt;
typedef struct VCSDALDevCtxt    VCSDALDevCtxt;
typedef struct VCSDALClientCtxt VCSDALClientCtxt;


/*
 * Private "VCS" Vtable
 */
typedef struct VCSDALVtbl VCSDALVtbl;
struct VCSDALVtbl
{
  DALResult (*VCS_DriverInit)(VCSDALDrvCtxt *);
  DALResult (*VCS_DriverDeInit)(VCSDALDrvCtxt *);
};


/*
 * VCS device context.
 */
struct VCSDALDevCtxt
{
  /*
   * Base Members - do not modify!
   */
  uint32           dwRefs;
  DALDEVICEID      DevId;
  uint32           dwDevCtxtRefIdx;
  VCSDALDrvCtxt   *pVCSDrvCtxt;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32           Reserved[16];
};


/*
 * VCS client context.
 */
struct VCSDALClientCtxt
{
  /*
   * Base members - do not modify!
   */
  uint32         dwRefs;
  uint32         dwAccessMode;
  void          *pPortCtxt;
  VCSDALDevCtxt *pVCSDevCtxt;
  DalVCSHandle   DalVCSHandle;
};


/*
 * VCS driver context.
 */
struct VCSDALDrvCtxt
{
  /*
   * Base members - do not modify!
   */
  VCSDALVtbl       VCSDALVtbl;
  uint32           dwNumDev;
  uint32           dwSizeDevCtxt;
  uint32           bInit;
  uint32           dwRefs;
  VCSDALDevCtxt    VCSDevCtxt[1];

  /*
   * Private driver context.
   */
  DalDeviceHandle *phHWIO;
  VCSDrvCtxt       DrvCtxt;
};


/*=========================================================================
      Function Prototypes
==========================================================================*/


DALResult VCS_DeviceAttach(const char *, DALDEVICEID, DalDeviceHandle **);
static DALResult DALVCS_DriverInit(VCSDALDrvCtxt *);
static DALResult DALVCS_DriverDeInit(VCSDALDrvCtxt *);


/*=========================================================================
      Data Declarations
==========================================================================*/


const DALREG_DriverInfo DALVCS_DriverInfo =
{
  VCS_DeviceAttach, 0, NULL
};


static VCSDALDrvCtxt VCS_DriverCtxt =
{
  {
    DALVCS_DriverInit,
    DALVCS_DriverDeInit
  },
  1,
  sizeof(VCSDALDevCtxt)
};


/*=========================================================================
      Functions
==========================================================================*/


static uint32 VCS_AddRef
(
  DalVCSHandle *h
)
{
  return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}


static uint32 VCS_DeviceDetach
(
  DalDeviceHandle *h
)
{
  uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
  return dwref;
}


static DALResult VCS_DeviceInit
(
  DalDeviceHandle *h
)
{
  return DAL_SUCCESS;
}


static DALResult VCS_DeviceDeInit
(
  DalDeviceHandle *h
)
{
  return DAL_SUCCESS;
}


static DALResult VCS_DevicePowerEvent
(
  DalDeviceHandle *h,
  DalPowerCmd      PowerCmd,
  DalPowerDomain   PowerDomain
)
{
  return DAL_SUCCESS;
}


static DALResult VCS_DeviceOpen
(
  DalDeviceHandle *h,
  uint32           mode
)
{
  return DAL_SUCCESS;
}


static DALResult VCS_DeviceClose
(
  DalDeviceHandle *h
)
{
  return DAL_SUCCESS;
}


static DALResult VCS_DeviceInfo
(
  DalDeviceHandle *h,
  DalDeviceInfo   *info,
  uint32           infoSize
)
{
   info->Version = DALVCS_INTERFACE_VERSION;
   return DAL_SUCCESS;
}


static DALResult VCS_DeviceSysRequest
(
  DalDeviceHandle *h,
  DalSysReq        ReqIdx,
  const void      *SrcBuf,
  uint32           SrcBufLen,
  void            *DestBuf,
  uint32           DestBufLen,
  uint32          *DestBufLenReq
)
{
   return DAL_SUCCESS;
}


static void DALVCS_GetStubFlags
(
  VCSDrvCtxt *pDrvCtxt
)
{
  DALResult            eResult;
  VCSPropertyValueType PropVal;

  /*-----------------------------------------------------------------------*/
  /* Get the stub api flag.                                                */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_GetPropertyValue("VCSStubAPI", &PropVal);
  if (eResult == DAL_SUCCESS)
  {
    if ( ((uint32)PropVal) == TRUE )
    {
      VCS_GLOBAL_FLAG_SET(STUB_API);

      DALSYS_LogEvent(0, DALSYS_LOGEVENT_INFO,
        "DALLOG Device VCS: VCS_GLOBAL_FLAG_STUB_API set - stubbing out API.");
    }
  }
}


static DALResult DALVCS_DriverInit
(
  VCSDALDrvCtxt *drvCtxt
)
{
  DALResult   eRes;
  VCSDrvCtxt *pDrvCtxt = &drvCtxt->DrvCtxt;

  /*-----------------------------------------------------------------------*/
  /* Get our property handle.                                              */
  /*-----------------------------------------------------------------------*/

  eRes =
    DALSYS_GetDALPropertyHandleStr(
      "VCS",
      VCS_DriverCtxt.VCSDevCtxt[0].hProp);

  if (eRes != DAL_SUCCESS)
  {
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Attach to the HWIO DAL.  This is done here since the common code may  */
  /* live in environments without the DAL framework.                       */
  /*-----------------------------------------------------------------------*/

  DAL_DeviceAttach(DALDEVICEID_HWIO, &VCS_DriverCtxt.phHWIO);

  /*-----------------------------------------------------------------------*/
  /* Get the stub flags.                                                   */
  /*-----------------------------------------------------------------------*/

  DALVCS_GetStubFlags(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Perform driver initialization.                                        */
  /*-----------------------------------------------------------------------*/

  if (VCS_GLOBAL_FLAG_IS_SET(STUB_API))
  {
    eRes = VCSStub_DriverInit(pDrvCtxt);
  }
  else
  {
    eRes = VCS_DriverInit(pDrvCtxt);
  }

  return eRes;
}


static DALResult DALVCS_DriverDeInit
(
  VCSDALDrvCtxt *drvCtxt
)
{ 
  DALResult   eRes;
  VCSDrvCtxt *pDrvCtxt = &drvCtxt->DrvCtxt;

  if (VCS_GLOBAL_FLAG_IS_SET(STUB_API))
  {
    eRes = VCSStub_DriverDeInit(pDrvCtxt);
  }
  else
  {
    eRes = VCS_DriverDeInit(pDrvCtxt);
  }

  return eRes;
}


static DALResult DALVCS_SetRailMode
(
  DalDeviceHandle *h,
  VCSRailType      eRail,
  VCSRailModeType  eMode
)
{
  return VCS_SetRailMode(HANDLE2CTXT(h), eRail, eMode);
}


static DALResult DALVCS_SetRailVoltage
(
  DalDeviceHandle *h,
  VCSRailType      eRail,
  uint32           nVoltageUV
)
{
  return VCS_SetRailVoltage(HANDLE2CTXT(h), eRail, nVoltageUV);
}


static DALResult DALVCS_SetCPUCorner
(
  DalDeviceHandle *h,
  ClockCPUType     eCPU,
  VCSCornerType    eCorner
)
{
  return VCS_SetCPUCorner(HANDLE2CTXT(h), eCPU, eCorner);
}


static DALResult DALVCS_AddLDORestriction
(
  DalDeviceHandle *h,
  ClockCPUType     eCPU
)
{
  return VCS_AddLDORestriction(HANDLE2CTXT(h), eCPU);
}


static DALResult DALVCS_RemoveLDORestriction
(
  DalDeviceHandle *h,
  ClockCPUType     eCPU
)
{
  return VCS_RemoveLDORestriction(HANDLE2CTXT(h), eCPU);
}


/*-----------------------------------------------------------------------*/
/* Stubs.                                                                */
/*-----------------------------------------------------------------------*/


static DALResult DALVCSStub_SetRailMode
(
  DalDeviceHandle *h,
  VCSRailType      eRail,
  VCSRailModeType  eMode
)
{
  return DAL_SUCCESS;
}


static DALResult DALVCSStub_SetRailVoltage
(
  DalDeviceHandle *h,
  VCSRailType      eRail,
  uint32           nVoltageUV
)
{
  return DAL_SUCCESS;
}


static DALResult DALVCSStub_SetCPUCorner
(
  DalDeviceHandle *h,
  ClockCPUType     eCPU,
  VCSCornerType    eCorner
)
{
  return DAL_SUCCESS;
}


static DALResult DALVCSStub_AddLDORestriction
(
  DalDeviceHandle *h,
  ClockCPUType     eCPU
)
{
  return DAL_SUCCESS;
}


static DALResult DALVCSStub_RemoveLDORestriction
(
  DalDeviceHandle *h,
  ClockCPUType     eCPU
)
{
  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : VCS_InitInterface
** =========================================================================*/
/**
  Initialize the client context for communicating to the DAL.
 
  This function initializes the given client context.  It is invoked
  internally when a client attaches to the dal.
 
  @param *pclientCtxt [in] -- Pointer to the DAL client context.

  @return
  None.

  @dependencies
  None.
*/ 

static void VCS_InitInterface
(
  VCSDALClientCtxt* pclientCtxt
)
{
  VCSDrvCtxt  *pDrvCtxt = &pclientCtxt->pVCSDevCtxt->pVCSDrvCtxt->DrvCtxt;
  static DalVCS vtbl =
  {
    {
      VCS_DeviceAttach,
      VCS_DeviceDetach,
      VCS_DeviceInit,
      VCS_DeviceDeInit,
      VCS_DeviceOpen,
      VCS_DeviceClose,
      VCS_DeviceInfo,
      VCS_DevicePowerEvent,
      VCS_DeviceSysRequest
    },

    DALVCS_SetRailMode,
    DALVCS_SetRailVoltage,
    DALVCS_SetCPUCorner,
    DALVCS_AddLDORestriction,
    DALVCS_RemoveLDORestriction
  };

  /*-----------------------------------------------------------------------*/
  /* Stub out the DAL API if the API stub flag is set.                     */
  /*-----------------------------------------------------------------------*/

  if (VCS_GLOBAL_FLAG_IS_SET(STUB_API))
  {
    vtbl.SetRailMode          = DALVCSStub_SetRailMode;
    vtbl.SetRailVoltage       = DALVCSStub_SetRailVoltage;
    vtbl.SetCPUCorner         = DALVCSStub_SetCPUCorner;
    vtbl.AddLDORestriction    = DALVCSStub_AddLDORestriction;
    vtbl.RemoveLDORestriction = DALVCSStub_RemoveLDORestriction;
  }

  pclientCtxt->DalVCSHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
  pclientCtxt->DalVCSHandle.pVtbl  = &vtbl;
  pclientCtxt->DalVCSHandle.pClientCtxt = pclientCtxt;

} /* END VCS_InitInterface */


/* =========================================================================
**  Function : VCS_DeviceAttach
** =========================================================================*/
/**
  Internal DAL framework function for attaching to the VCS DAL.
 
  This function attaches to the VCS DAL and initializes the given
  device handle.
 
  @param *pszArg[in] -- String passed by user during attach.
  @param DeviceId -- The device id, should be DALDEVICEID_VCS
  @param phDalDevice[out] -- DAL device handle to fill in.

  @return
  None.

  @dependencies
  None.
*/ 

DALResult VCS_DeviceAttach
(
  const char       *pszArg,
  DALDEVICEID       DeviceId,
  DalDeviceHandle **phDalDevice
)
{
   DALResult nErr;
   static VCSDALClientCtxt ClientCtxt;

   VCSDALClientCtxt *pClientCtxt = &ClientCtxt;

   *phDalDevice = NULL;

   nErr = DALFW_AttachToStringDevice(
            pszArg, (DALDrvCtxt *)&VCS_DriverCtxt,
            (DALClientCtxt *)pClientCtxt);

   if (DAL_SUCCESS == nErr)
   {
      VCS_InitInterface(pClientCtxt);
      VCS_AddRef(&(pClientCtxt->DalVCSHandle));
      *phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalVCSHandle);
   }

   return nErr;

} /* END VCS_DeviceAttach */


/* =========================================================================
**  Function : VCS_GetPropertyValue
** =========================================================================*/
/*
  See VCSDriver.h
*/ 

DALResult VCS_GetPropertyValue
(
  const char           *szName,
  VCSPropertyValueType *pValue
)
{
  DALResult         eRes;
  DALSYSPropertyVar Var;

  eRes = DALSYS_GetPropertyValue(
    VCS_DriverCtxt.VCSDevCtxt[0].hProp, szName, 0, &Var); 

  if (eRes != DAL_SUCCESS)
  {
    return eRes;
  }

  if (Var.dwType == DALPROP_ATTR_TYPE_STRUCT_PTR)
  {
    *pValue = (VCSPropertyValueType)Var.Val.pStruct;
  }
  else if (Var.dwType == DALPROP_ATTR_TYPE_UINT32)
  {
    *pValue = (VCSPropertyValueType)Var.Val.dwVal;
  }

  return DAL_SUCCESS;

} /* END VCS_GetPropertyValue */


/* =========================================================================
**  Function : VCS_MapHWIORegion
** =========================================================================*/
/*
  See VCSDriver.h
*/ 

DALResult VCS_MapHWIORegion
(
  uint32  nPhysAddr,
  uint32  nSizeBytes,
  uint32 *pnVirtAddr
)
{
  DALResult eRes = DAL_SUCCESS;

  /*-----------------------------------------------------------------------*/
  /* If we attached to the HWIO DAL, then map the regions.                 */
  /*-----------------------------------------------------------------------*/

  if (VCS_DriverCtxt.phHWIO != NULL)
  {
    eRes = DalHWIO_MapRegionByAddress(
      VCS_DriverCtxt.phHWIO, (uint8 *)nPhysAddr, (uint8 **)pnVirtAddr);
  }

  /*-----------------------------------------------------------------------*/
  /* No HWIO DAL, so assume base address is the physical address.          */
  /*-----------------------------------------------------------------------*/

  else
  {
    *pnVirtAddr = nPhysAddr;
  }

  return eRes;

} /* END VCS_MapHWIORegion */


/*=========================================================================
**  Function : VCS_GetDrvCtxt
** =========================================================================*/
/*
  See VCSDriver.h
*/ 

VCSDrvCtxt *VCS_GetDrvCtxt
(
  void
)
{
  return &VCS_DriverCtxt.DrvCtxt;
}

