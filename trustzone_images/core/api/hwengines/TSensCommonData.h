#ifndef __TSENSCOMMONDATA_H__
#define __TSENSCOMMONDATA_H__

/*============================================================================
  @file TsensCommonData.h
 
  TSENS Common Data interface template.
  
               Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/hwengines/TSensCommonData.h#1 $ */
#include "comdef.h"
typedef enum
{
   TSENS_SENSOR1=1,
   TSENS_SENSOR2,
   TSENS_SENSOR3,
   TSENS_SENSOR4,
   TSENS_SENSOR5,
   TSENS_FORCE_32_BIT = 0x7FFFFFFF
} TsensSensorType;

#define TSENS_SENSOR1MASK (1 << TSENS_SENSOR1)
#define TSENS_SENSOR2MASK (1 << TSENS_SENSOR2)
#define TSENS_SENSOR3MASK (1 << TSENS_SENSOR3)
#define TSENS_SENSOR4MASK (1 << TSENS_SENSOR4)
#define TSENS_SENSOR5MASK (1 << TSENS_SENSOR5)

typedef enum
{
  TSENS_LWR_LIMIT_REACHED=0,
  TSENS_UPR_LIMIT_REACHED,
  TSENS_ERR_EVT_TRIGGERED = 0x7FFFFFFF
} TsensEventTriggeredType;

typedef struct
{
   int32 MinDegreeCelsius;
   int32 MaxDegreeCelsius;
} TsensTempRangeType;
#endif
