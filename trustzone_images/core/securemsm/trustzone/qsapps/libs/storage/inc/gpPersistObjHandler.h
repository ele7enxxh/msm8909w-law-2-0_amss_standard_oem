#ifndef _GP_PERSIST_OBJ_HANDLER_H_
#define _GP_PERSIST_OBJ_HANDLER_H_

/**
@file   gpPersistObjHandler.h
@brief  Defintions for the gp Handler module which manages open object handles.
*/

/*=============================================================================
Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/libs/storage/inc/gpPersistObjHandler.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/15   mr      Initial Implementation
=============================================================================*/

#include "gpTypes.h"
#include "qsee_cipher.h"

#define MAX_OPEN_CLIENT_HANDLES 128
#define FILE_NAME_SIZE 32

typedef struct
{
  uint32  version;
  uint32  fd;
  uint8   objectID[TEE_OBJECT_ID_MAX_LEN];
  uint8   fileName[FILE_NAME_SIZE];
  uint8   cKey[QSEE_AES256_KEY_SIZE];
  uint8   hKey[QSEE_AES256_KEY_SIZE];
  boolean keyInit;
} gpHandle;


#endif //_GP_PERSIST_OBJ_HANDLER_H_
