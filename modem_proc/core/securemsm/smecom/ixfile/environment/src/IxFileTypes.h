#ifndef IXFILETYPES_H
#define IXFILETYPES_H
/*============================================================================
IxFileTypes

ABSTRACT
   The IxFileTypes interface file for chipset specific libraries.

DESCRIPTION
   
Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixfile/environment/src/IxFileTypes.h#1 $

=========================================================================== */
#include "comdef.h"
#include "IxSFSFile.h"

#ifdef FEATURE_SEC_SFS
#include "sfs_api.h"
#endif

/*===============================================
*  EXTERNAL DEFINATIONS
===============================================*/
#include "IxErrno.h"
#include "IxSysUtils.h"
#include "UxBuf.h"
#include "UxCommon.h"

extern "C"
{
   #include "secdiag.h"
   #include "ixipc.h"
   #include "ipcmacros.h"
   #include "fs_public.h"
   #include "fs_stdio.h"
   #include "fs_errno.h"
}

#include "str_util.h"
#include <string.h>
#include "ipcrex.h"
#include "ipcutil.h"
#include "dbg_util.h"
#include "task.h"
#include "rex.h"
#include "rte.h"

/*==============================================
* Common Defines
===============================================*/

#define IXFILE_DBG_HIGH(x_fmt, a, b, c)  SMECOM_DBG_MSG3(MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c)
#define IXFILE_DBG_ERR(x_fmt, a, b, c)  SMECOM_DBG_MSG3(MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c)
#define IXFILE_DBG_SPRINTF1 SMECOM_DBG_MSG1

/*=============================================== 
*  FEATURE SEC SFS ENUM
===============================================*/
typedef enum
{
  SEC_SFS_ENABLED =  0,
  SEC_SFS_DISABLED
} E_FileSystemType;

INLINE E_FileSystemType getFileSystemType() 
{
  #ifdef FEATURE_SEC_SFS
     return ((E_FileSystemType)SEC_SFS_ENABLED);
  #else
     return ((E_FileSystemType)SEC_SFS_DISABLED);
  #endif
}

#endif /* IXFILETYPES_H */
