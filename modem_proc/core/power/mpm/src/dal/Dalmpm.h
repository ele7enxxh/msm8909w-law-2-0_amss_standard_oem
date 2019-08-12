#ifndef DALMPM_H
#define DALMPM_H
/*==============================================================================
  FILE:         Dalmpm.h
  
  OVERVIEW:     Function and data structure declarations
 
  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/dal/Dalmpm.h#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 
#include "DALFramework.h"
#include "DDImpm.h"
#include "vmpm.h"

/*------------------------------------------------------------------------------
Declaring a "mpm" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct mpmDrvCtxt mpmDrvCtxt;
typedef struct mpmDevCtxt mpmDevCtxt;
typedef struct mpmClientCtxt mpmClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "mpm" Vtable
------------------------------------------------------------------------------*/
typedef struct mpmDALVtbl mpmDALVtbl;
struct mpmDALVtbl
{
  int (*mpm_DriverInit)(mpmDrvCtxt *);
  int (*mpm_DriverDeInit)(mpmDrvCtxt *);
};

struct mpmDevCtxt
{
  //Base Members
  uint32   dwRefs;                                    
  DALDEVICEID DevId;                                  
  uint32   dwDevCtxtRefIdx; 
  mpmDrvCtxt  *pmpmDrvCtxt;                             
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32 Reserved[16];
  //mpm Dev state can be added by developers here
};

struct mpmDrvCtxt
{
  //Base Members
  mpmDALVtbl mpmDALVtbl;
  uint32  dwNumDev;
  uint32  dwSizeDevCtxt;
  uint32  bInit;
  uint32  dwRefs;
  mpmDevCtxt mpmDevCtxt[1];
  //mpm Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "mpm" Client Context
------------------------------------------------------------------------------*/
struct mpmClientCtxt
{
  //Base Members
  uint32  dwRefs;                     
  uint32  dwAccessMode;  
  void *pPortCtxt;
  mpmDevCtxt *pmpmDevCtxt;            
  DalmpmHandle DalmpmHandle; 
  //mpm Client state can be added by developers here
};

DALResult mpm_DriverInit(mpmDrvCtxt *);
DALResult mpm_DriverDeInit(mpmDrvCtxt *);
DALResult mpm_DeviceInit(mpmClientCtxt *);
DALResult mpm_DeviceDeInit(mpmClientCtxt *);
DALResult mpm_Reset(mpmClientCtxt *);
DALResult mpm_PowerEvent(mpmClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult mpm_Open(mpmClientCtxt *, uint32);
DALResult mpm_Close(mpmClientCtxt *);
DALResult mpm_Info(mpmClientCtxt *,DalDeviceInfo *, uint32);
DALResult mpm_InheritObjects(mpmClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);

/*------------------------------------------------------------------------------
Functions specific to Dalmpm interface
------------------------------------------------------------------------------*/
DALResult mpm_ConfigPin( mpmClientCtxt *, uint32 , mpm_pin_config_type * );
DALResult mpm_ConfigAndEnablePin( mpmClientCtxt *, uint32, mpm_pin_config_type * );
DALResult mpm_EnablePin( mpmClientCtxt *, uint32 );
DALResult mpm_DisablePin( mpmClientCtxt *, uint32 );
DALResult mpm_GetNumMappedIrq( mpmClientCtxt *, uint32 * );
DALResult mpm_GetNumMappedGpio( mpmClientCtxt *, uint32 * );
DALResult mpm_MapInterrupts( mpmClientCtxt *, mpm_config_info_type *, uint32 , uint32 * );
DALResult mpm_SetupInterrupts( mpmClientCtxt *, mpm_config_info_type *, uint32 );
DALResult mpm_QueryPinMapping( mpmClientCtxt *, uint32 , mpm_pin_type, uint32 * );
DALResult mpm_QueryPinConfigInfo( mpmClientCtxt *, uint32 , mpm_pin_query_info_type * );
DALResult mpm_AddIrqPinToMappingTbl( mpmClientCtxt *, uint32, mpm_int_pin_mapping_type *);
#endif /*DALmpm_H*/


