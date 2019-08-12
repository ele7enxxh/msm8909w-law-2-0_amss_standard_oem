#ifndef _GP_PERSIST_OBJ_H_
#define _GP_PERSIST_OBJ_H_

/**
@file   gpPersistObj.h
@brief  Defintions for the commno gpPersistObj structures and formats.
*/

/*=============================================================================
Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/libs/storage/inc/gpPersistObj.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/15   mr      Initial Implementation
=============================================================================*/

#include "gpTypes.h"
#include "gpPersistObjHandler.h"
#include "qsee_log.h"
#include "qsee_timer.h"

#define GPLOG_ERROR(x) \
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "%s:%d res=%x", __func__, __LINE__, x)
  
#define GPLOG_MARKER_START() \
  //qsee_log(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Start)", qsee_get_uptime(), __func__, __LINE__)
#define GPLOG_MARKER_STOP()  \
  //qsee_log(QSEE_LOG_MSG_ERROR, "[%llu ms] %s:%d (Stop)", qsee_get_uptime(), __func__, __LINE__)

/* GP Persistent Object Error Codes */

#endif //_GP_PERSIST_OBJ_H_
