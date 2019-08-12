#ifndef __DAL_TSENS_H__
#define __DAL_TSENS_H__
/**
  @file DALTsens.h

  Function and data structure declarations for TSENS DAL.
  
  This file contains data structures and functions used to configure, control, 
  and query temperature sensors.
*/ 
/*============================================================================
               Copyright (c) 2010-2012, 2014 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/tsens/dal/src/DALTsens.h#1 $ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DDITsens.h"
#include "DDIInterruptController.h"
#include "DALQueue.h"
#include "DDIHWIO.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
/* Declaring a Tsens driver, device and client context */
typedef struct TsensDrvCtxt TsensDrvCtxt;
typedef struct TsensDevCtxt TsensDevCtxt;
typedef struct TsensClientCtxt TsensClientCtxt;

/* Declaring a private Tsens Vtable */
typedef struct TsensDALVtbl TsensDALVtbl;
struct TsensDALVtbl
{
   int (*Tsens_DriverInit)(TsensDrvCtxt *);
   int (*Tsens_DriverDeInit)(TsensDrvCtxt *);
};

/* Tsens device context */
struct TsensDevCtxt
{
   //Base Members
   uint32 dwRefs;                                    
   DALDEVICEID DevId;                                  
   uint32 dwDevCtxtRefIdx; 
   TsensDrvCtxt *pTsensDrvCtxt;                             
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   
   //Tsens Dev state can be added by developers here
};

/* Tsens driver context */
struct TsensDrvCtxt
{
   //Base Members
   TsensDALVtbl TsensDALVtbl;
   uint32 dwNumDev;
   uint32 dwSizeDevCtxt;
   uint32 bInit;
   uint32 dwRefs;
   TsensDevCtxt TsensDevCtxt[1];
};

/* Tsens client context */
struct TsensClientCtxt
{
   //Base Members
   uint32 dwRefs;                     
   uint32 dwAccessMode;  
   void *pPortCtxt;
   TsensDevCtxt *pTsensDevCtxt;            
   DalTsensHandle DalTsensHandle; 
   uint32 nClient;
};

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/* Functions specific to the DAL interface */
DALResult Tsens_DriverInit(TsensDrvCtxt *);
DALResult Tsens_DriverDeInit(TsensDrvCtxt *);
DALResult Tsens_DeviceInit(TsensClientCtxt *);
DALResult Tsens_DeviceDeInit(TsensClientCtxt *);
DALResult Tsens_Reset(TsensClientCtxt *);
DALResult Tsens_PowerEvent(TsensClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult Tsens_RegisterClient(TsensClientCtxt * pCtxt);
DALResult Tsens_DeregisterClient(TsensClientCtxt * pCtxt);
DALResult Tsens_Open(TsensClientCtxt *, uint32);
DALResult Tsens_Close(TsensClientCtxt *);
DALResult Tsens_Info(TsensClientCtxt *,DalDeviceInfo *, uint32);
DALResult Tsens_InheritObjects(TsensClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);

/* Functions specific to the DalTsens interface */
DALResult Tsens_GetTemp(TsensClientCtxt *, uint32, TsensTempType *);
DALResult Tsens_GetNumSensors(TsensClientCtxt *, uint32 *);

#endif
