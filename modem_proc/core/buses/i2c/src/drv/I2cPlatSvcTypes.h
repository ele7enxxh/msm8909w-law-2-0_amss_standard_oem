#ifndef _I2CPLATSVCTYPES_H_
#define _I2CPLATSVCTYPES_H_
/*=============================================================================

  @file   I2cPlatSvcTypes.h

   This file contains the types for the platform services.
 
   Copyright (c) 2015-2016 Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary

  ===========================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cPlatSvcTypes.h#1 $ */

/*=========================================================================
  EDIT HISTORY FOR MODULE

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  03/15/16 VG     Added error code enums for clk state
  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cTypes.h"
#include "I2cError.h"

#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DDIClock.h"
#include "DDIHWIO.h"

#include "npa.h"
#include "icbarb.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

typedef enum I2cPlat_Error
{
   I2CPLAT_ERROR_BASE = I2C_RES_ERROR_CLS_DEV_PLATFORM,
   
   I2CPLAT_ERROR_INVALID_DEVICE_INDEX,
   I2CPLAT_ERROR_DAL_GET_PROPERTY_HANDLE,
   I2CPLAT_ERROR_DAL_GET_PROPERTY_VALUE,
   I2CPLAT_ERROR_ATTACH_TO_DALHWIO,
   I2CPLAT_ERROR_FAILED_TO_MAP_BLOCK_HWIO,
   I2CPLAT_ERROR_DETACH_FROM_DALHWIO,
   I2CPLAT_ERROR_INVALID_POWER_STATE,

   I2CPLAT_ERROR_FAILED_TO_ATTACH_TO_TLMM,
   I2CPLAT_ERROR_FAILED_TO_DETTACH_FROM_TLMM,
   I2CPLAT_ERROR_FAILED_ATTACH_TO_IRQCTRL,
   I2CPLAT_ERROR_FAILED_TO_REGISTER_IST,
   I2CPLAT_ERROR_FAILED_TO_UNREGISTER_IST,
   I2CPLAT_ERROR_IRQCTRL_NOT_INITIALISED,
   I2CPLAT_ERROR_FAILED_TO_OPEN_TLMM,
   I2CPLAT_ERROR_FAILED_TO_CLOSE_TLMM,
   I2CPLAT_ERROR_FAILED_TO_CONFIGURE_GPIO,
   I2CPLAT_ERROR_FAILED_TO_ENABLE_CLOCK,
   I2CPLAT_ERROR_FAILED_TO_DISABLE_CLOCK,
   I2CPLAT_ERROR_FAILED_TO_CREATE_PNOC_CLIENT,
   I2CPLAT_ERROR_PNOC_CLIENT_NOT_CREATED,
   I2CPLAT_ERROR_FAILED_TO_ENABLE_AHB_CLK,
   I2CPLAT_ERROR_FAILED_TO_ENABLE_APPS_CLK,
   I2CPLAT_ERROR_FAILED_TO_DISABLE_AHB_CLK,
   I2CPLAT_ERROR_FAILED_TO_DISABLE_APPS_CLK,
   I2CPLAT_ERROR_FAILED_TO_ISSUE_PNOC_REQ,
   
} I2cPlat_Error;

typedef enum I2cPlat_PowerStates
{
   I2CPLAT_POWER_STATE_0,/**< Bus and clocks off. */
   I2CPLAT_POWER_STATE_1,/**< Bus on clocks off. */
   I2CPLAT_POWER_STATE_2,/**< Bus on clocks on. */
} I2cPlat_PowerStates;

typedef enum I2cPlat_TargetInitState
{
   I2CPLAT_TGT_INIT_NOT_DONE = 0,
   I2CPLAT_TGT_INIT_CLK_ALLOCATED       = 0x01,
   I2CPLAT_TGT_INIT_HWIO_ALLOCATED      = 0x02,
   I2CPLAT_TGT_INIT_GPIO_INITED         = 0x04,
   I2CPLAT_TGT_INIT_PNOC_INITED         = 0x08,
   I2CPLAT_TGT_INIT_HWIO_INITED         = 0x20,

} I2cPlat_TargetInitState;

typedef struct I2cPlat_PropertyType
{
   uint32                aGpioCfg[2];
   uint32                uNumGpios;
   const char*           pPeriphSsAddrName;
   uint32                coreOffset;
   char                  *pQupHClkName;
   char                  *pQupAppClkName;
   uint32                uAppClkFreqKhz;
   DalDeviceHandle       *pClkHandle;
   ClockIdType           QupHClkId;
   ClockIdType           QupAppClkId;
   boolean               bPnocEnable;
   uint64                uPnocIBVal;
   uint64                uPnocABVal;
   uint32                uPnocMaster;
   uint32                uPnocSlave;
   uint32                uNumdevices;
} I2cPlat_PropertyType ;

// This INIT function isn't used. Update if you want to use it.
#define I2CPLAT_PROPERTY_INIT(GPIO1_CFG,GPIO2_CFG,GPIO_NUM,PERIPH_SS_NAME, CORE_OFFSET,AHB_CLK,APPS_CLK) \
{                                              \
   /* .aGpioCfg[2]= */ {GPIO1_CFG,GPIO2_CFG }, \
   /* .uNumGpios  = */ GPIO_NUM,               \
   /* .pPeriphSsAddrName = */ PERIPH_SS_NAME,  \
   /* .coreOffset        = */ CORE_OFFSET,  \
   /* .pClkHandle        = */ AHB_CLK,         \
}                                              \


typedef struct I2cPlat_DescType
{
   uint32                        tmpPowerState;
   I2cPlat_TargetInitState       initState;
   uint8                         *pQupAddr;
   npa_client_handle             hNpaClient;
   I2cPlat_PropertyType          props;
} I2cPlat_DescType;

#define I2CPLAT_DESC_INIT                                        \
{                                                                \
  /* .tmpPowerState = */        0,                               \
  /* initState      = */        I2CPLAT_TGT_INIT_NOT_DONE,       \
  /* qQupAddr       = */        NULL,                            \
  /* props          = */        {0,{0,0},0,NULL},                \
}                                                                \

typedef void(*IST_HOOK)(void*);
typedef void(*ISR_HOOK)(void*);



#endif /* _I2CPLATSVCTYPES_H_ */

