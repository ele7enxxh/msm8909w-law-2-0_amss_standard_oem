#ifndef _I2CDRIVERTYPES_H_
#define _I2CDRIVERTYPES_H_
/*=============================================================================

  FILE:   I2cDriverTypes.h

  OVERVIEW:     This file contains the implementation structures for the i2c
                driver API.
  DEPENDENCIES: None.
 
                Copyright c 2015-2016 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
  ===========================================================================*/

/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cDriverTypes.h#1 $
  $DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
   03/15/16 VG     Added ClkRefCnt for clock state
   08/25/15 VG     Created
  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cDriver.h"
#include "I2cError.h"
#include "I2cDevice.h"
#include "I2cSys.h"
//#include "I2cLog.h"
#include "I2cPlatSvc.h"
#include "npa.h"
#include "icbarb.h"
#include "I2cPlatSvcTypes.h"
/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

#define I2CDRV_PROPVALUE_IRQS_EN                 TRUE
#define I2CDRV_PROPVALUE_IRQS_DISABLE            FALSE
#define I2C_CLIENT_COOKIE   0xDEADBEEF

#define RETURN_IF_INVALID_BUS_ID(I2C_BUS_ID)                            \
      if (((I2C_BUS_ID) < I2CDRV_I2C_MIN) || ((I2C_BUS_ID) > I2CDRV_I2C_MAX)) {\
	     return I2CDRV_ERR_INVALID_BUS_ID;                              \
      }

#define RETURN_IF_INVALID_BUS_PTR(I2C_BUS_PTR)                          \
      if (NULL == (I2C_BUS_PTR)) {      return I2CDRV_ERR_NULL_BUS_PTR;}\
      else if ( ((I2C_BUS_PTR)->i2cId < I2CDRV_I2C_MIN) ||                 \
	            ((I2C_BUS_PTR)->i2cId > I2CDRV_I2C_MAX) ) {                \
	                return I2CDRV_ERR_INVALID_BUS_ID;                   \
      }                                                                 \

#define RETURN_IF_INVALID_BUS_ID_PTR(I2C_BUS_ID,I2C_BUS_PTR) \
   RETURN_IF_INVALID_BUS_ID(I2C_BUS_ID) \
   RETURN_IF_INVALID_BUS_PTR(I2C_BUS_PTR)
	             

#define RETURN_IF_INVALID_TRAN(pTransfer,pRes) \
       if ( NULL == pTransfer ) {     return I2CDRV_ERR_INVALID_TRANSFER_POINTER;} \
       else if ( NULL == pRes ) { return I2CDRV_ERR_INVALID_NUMCOMPLETED_POINTER;} \


#define I2C_IS_DEVICE_POWERED(pDesc) \
             (((pDesc)->pwState & I2CDRV_PWSTATE_PNOC_ON) &&\
			  ((pDesc)->pwState & I2CDRV_PWSTATE_I2C_CLKS_ON))



/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/


enum  I2cDrv_Error
{
   I2CDRV_ERR_BASE = I2C_RES_ERROR_CLS_I2C_DRV,

   I2CDRV_ERR_INVALID_TRANSFER_POINTER,
   I2CDRV_ERR_INVALID_NUMCOMPLETED_POINTER,
   I2CDRV_ERR_NOT_IMPLEMENTED,
   I2CDRV_ERR_INVALID_BUS_ID,
   I2CDRV_ERR_DEVICE_NOT_INITIALISED,
   I2CDRV_ERR_DEVICE_NOT_POWERED,
   I2CDRV_ERR_DEVICE_BUSY,
   I2CDRV_ERR_NULL_BUS_PTR,
   I2CDRV_ERR_INVALID_BUS_PTR,
   I2CDRV_ERR_OUT_OF_FREE_OBJS,
   I2CDRV_ERR_INVALID_CLIENT_COOKIE,
   I2CDRV_ERR_INVALID_CMD_DATA,

};

typedef enum I2cDrv_OperationalStateType
{
	I2CDRV_OPSTATE_OBJ_FREE     =   0x0,
	I2CDRV_OPSTATE_DEV_OPEN     =   0x1,
	I2CDRV_OPSTATE_DEV_RW       =   0x2,
	I2CDRV_OPSTATE_DEV_BUSY   =   I2CDRV_OPSTATE_DEV_OPEN | I2CDRV_OPSTATE_DEV_RW,
	I2CDRV_OPSTATE_OBJ_IN_USE   =   0x80,

} I2cDrv_OperationalStateType;

typedef enum I2cDrv_PowerStateType
{
	I2CDRV_PWSTATE_OFF          =   0x0,
	I2CDRV_PWSTATE_PNOC_ON      =   0x1,
	I2CDRV_PWSTATE_I2C_CLKS_ON  =   0x2,
} I2cDrv_PowerStateType;

typedef struct I2cDrv_DriverProperty
{
   I2cDrv_I2cBusId              eDevId;
   boolean                      bInterruptBased;
   uint32                       uInterruptId;
} I2cDrv_DriverProperty;

#define I2CDRV_DRIVERPROPERTY_INIT(DRV_ID, IRQ_EN, IRQ_NUM)       \
{                                                                 \
/* .eDevId   = */                     (DRV_ID),                   \
/* .bInterruptBased = */              (IRQ_EN),                   \
/* .uInterruptId    = */              (IRQ_NUM),                  \
}                                                                 \

typedef struct I2cDrv_DescType I2cDrv_DescType;
typedef struct I2cDrv_DescType* I2cDrv_PDescType;
struct I2cDrv_DescType
{
   uint32                       uOpenCnt;
   uint32                       clkRefCnt;
   I2cDev_State                 devState;
   //I2CLOG_DevType   *pDevLog;
   I2cSys_SyncType              serialReqSync;/* Serialize client requests. */
   I2cSys_SyncType              serialOpSync; /* Serialize qup operations. */
   I2cSys_EventType             qupEvent;
   int32                        opState; /*values from I2cDrv_OperationalStateType*/
   I2cDrv_PowerStateType        pwState;
   
   I2cDrv_DriverProperty       *pDrvProps;
   I2cPlat_DescType             plat;
   I2cPlat_PropertyType     	 *pPlatProps;
   I2cDev_Device                dev;
   I2cDev_PropertyType         *pDevProps;
   I2cDrv_DescType             *pNext;
};

#define I2CDRV_DESC_INIT(DRV_ID, IRQ_EN, IRQ_NUM,PPLAT_PROPS, PDEV_PROPS)    \
{                                                                      \
     /* .uOpenCnt = */                     0,                          \
     /* .devState = */                     I2CDEV_STATE_INIT,  \
     /* .serialReqSync  = */               I2CSYS_SYNC_INIT,           \
     /* .serialOpSync   = */               I2CSYS_SYNC_INIT,           \
     /* .qupEvent = */                     I2CSYS_EVENT_INIT,          \
     /* .opState  = */                     I2CDRV_OPSTATE_OBJ_FREE,    \
     /* .pwState  = */                     I2CDRV_PWSTATE_OFF,         \
     /* . pDrvProps = */                   NULL,                       \
	  /* .plat     = */                     I2CPLAT_DESC_INIT,          \
	  /* .pPlatProps = */                   PPLAT_PROPS,\
	  /* .dev      = */                     I2CDEV_DESC_INIT,           \
	  /* .devProps = */                     PDEV_PROPS, \
	  /* .pNext    = */                     NULL                        \
}

/*-------------------------------------------------------------------------
 * Static Variable Definitions
 * ----------------------------------------------------------------------*/

extern I2cDrv_DescType          *aI2cDev[I2CDRV_I2C_NUM];
extern I2cSys_SyncType           i2cDrvSync;
extern boolean                   bSyncInitialised;

#endif //_I2CDRIVERTYPES_H_

