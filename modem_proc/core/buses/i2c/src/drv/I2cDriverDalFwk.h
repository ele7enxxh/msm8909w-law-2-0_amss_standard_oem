#ifndef I2CDRIVERDALFWK_H
#define I2CDRIVERDALFWK_H
/*=============================================================================

  @file   I2cDriverDalFwk.h

          This file has the API for I2C driver.
 
          Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
          All Rights Reserved.
          Qualcomm Confidential and Proprietary

  ===========================================================================*/

/* $Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cDriverDalFwk.h#1 $ */


/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "DALFramework.h"
#include "DALSys.h"
#include "ddii2c.h"
#include "I2cDriver.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

typedef struct DalI2cDrvCtxt DalI2cDrvCtxt;
typedef struct DalI2cDevCtxt DalI2cDevCtxt;
typedef struct DalI2cClientCtxt DalI2cClientCtxt;

typedef struct I2CDALVtbl I2CDALVtbl;
struct I2CDALVtbl
{
  int (*I2C_DriverInit)(DalI2cDrvCtxt *);
  int (*I2C_DriverDeInit)(DalI2cDrvCtxt *);
};

struct DalI2cDevCtxt
{
  //Base Members
  uint32   dwRefs;                                    
  DALDEVICEID DevId;                                  
  uint32   dwDevCtxtRefIdx; 
  DalI2cDrvCtxt  *pDalI2cDrvCtxt;                             
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32 Reserved[16];
  //I2C Dev state can be added by developers here
  
  /* handle to the implementation of the driver */
  I2cDrv_I2cBus i2cBus;
};
/* Declaring a "I2C" Client Context */
struct DalI2cClientCtxt
{
  //Base Members
  uint32  dwRefs;                     
  uint32  dwAccessMode;  
  void *pPortCtxt;
  DalI2cDevCtxt *pDalI2cDevCtxt;            
  DalI2CHandle DalI2CHandle; 
  /* I2C Client state can be added by developers here */
};

struct DalI2cDrvCtxt
{
  //Base Members
  I2CDALVtbl I2CDALVtbl;
  uint32  dwNumDev;
  uint32  dwSizeDevCtxt;
  uint32  bInit;
  uint32  dwRefs;
  DalI2cDevCtxt DalI2cDevCtxt[1]; // piggy back after this struct as neccessary
  /* I2C Drv state can be added by developers here */
};

extern const uint32      uNumI2cBuses;

#endif /* #ifndef I2CDRIVERDALFWK_H */

