#ifndef IXFILEUTILS_H
#define IXFILEUTILS_H
/*============================================================================
IxFileUtils

ABSTRACT
   Utility functions used by the IxFile and IxFileMgr implementation.

DESCRIPTION
   Utility functions used by the IxFile and IxFileMgr implementation.
   

Copyright (c) 2005 - 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixfile/shared/inc/IxFileUtils.h#1 $

=========================================================================== */
#include "comdef.h"
#include "IxFile.h"
#include "IxFileTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "dbg_util.h"

#define IXFILE_DBG_HIGH(x_fmt, a, b, c)  SMECOM_DBG_MSG3(MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c)
#define IXFILE_DBG_ERR(x_fmt, a, b, c)  SMECOM_DBG_MSG3(MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c)
#define IXFILE_DBG_SPRINTF1 SMECOM_DBG_MSG1

/*==============================================================
FUNCTION:  IxFileIsSFSDrive

DESCRIPTION:
   Returns TRUE if the input path matches the system SFS drive or
   the user SFS drive.
==============================================================*/
boolean IxFileIsSFSDrive (const char *path);

/*==============================================================
FUNCTION:  IxFileIsSysSFSDrive

DESCRIPTION:
   Returns TRUE if the input path matches the system SFS drive.
==============================================================*/
boolean IxFileIsSysSFSDrive (const char *path);

/*==============================================================
FUNCTION:  IxFileGetFilePath

DESCRIPTION:
   Given a path, potentially with a drive string in front, returns
   a mapped path with the drive string replaced by the appropriate
   path prefix. If the input path has no drive string in front or
   if no matching path prefix can be found for the drive string,
   the path returned is identical to the path passed in.
==============================================================*/
IxErrnoType IxFileGetFilePath (char *newPath, const char *oldPath);


#ifdef __cplusplus
}
#endif

#endif /* IXFILEUTILS_H */
