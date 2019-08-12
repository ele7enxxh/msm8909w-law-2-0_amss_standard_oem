#ifndef RFCOMMON_EFS_H
#define RFCOMMON_EFS_H

/*!
  @file
  rfcommon_efs.h

  @brief
  This modules contains prototypes for the common RF EFS functions.
*/

/*==============================================================================
Copyright (c) 2012-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_efs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/28/14   cd      Provide common EFS API to query EFS file size
08/27/12   jr      Added rfcommon_efs_put API
05/07/12   sr      Added the rfc_revision support.
03/05/12   sr      Initial Revision.

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "rfcom.h"

/**
 * Structure for information filled in by the efs_stat() */
typedef struct
{
  uint32  st_size;      /**< EFS file size in bytes */
}rfcommon_efs_fstat_type;

/* read the efs file   */
int32 rfcommon_efs_get(const char *file_name, void *data, int32 data_max_size);

/* write the efs file   */
boolean rfcommon_efs_put(const char *rfc_efs_file_name, void *data, int32 data_max_size, boolean append_flag);

/* get file stat */
int32 rfcommon_efs_get_size(const char *file_name, 
                             rfcommon_efs_fstat_type *fs_buf);

#ifdef __cplusplus
}
#endif



#endif
