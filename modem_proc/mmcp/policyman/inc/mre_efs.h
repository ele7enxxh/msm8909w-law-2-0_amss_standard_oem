#ifndef _MRE_EFS_H_
#define _MRE_EFS_H_

/**
  @file mre_efs.h

  @brief MRE abstraction layer above EFS
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/mre_efs.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "fs_public.h"
#include "nv.h"
#include "nv_items.h"
#include "sys.h"

typedef enum
{
  MRE_EFS_STATUS_SUCCESS,
  MRE_EFS_STATUS_ERROR,
  MRE_EFS_STATUS_NOFILE
} mre_efs_status_t;


/*-------- mre_efs_get_file --------*/
/**
@brief  Read an EFS item from a given path.

The data is returned in a buffer allocated by this function.  Callers should
call modem_mem_free() to free the data when they are done with it.

@param[in]  pPath   Pointer to path to item to be read.
@param[out] ppData  Pointer to storage for a pointer to the memory buffer
                    that is returned.
@param[in]  pSize   Pointer to storage for the size of buffer returned.

@return
  - MRE_EFS_STATUS_SUCCESS if successful.
  - MRE_EFS_STATUS_NOFILE if the file doesn't exist.
  - MRE_EFS_STATUS_ERROR if another error occurred.
*/
mre_efs_status_t mre_efs_get_file(
  const char             *pPath,
  void                  **ppData,
  size_t                 *pSize,
  sys_modem_as_id_e_type  asubs_id
);


/*-------- mre_efs_put_file --------*/
/**
@brief

@param

@return

*/
mre_efs_status_t mre_efs_put_file(
  char const             *pPath,
  void                   *pData,
  size_t                  dataSize,
  boolean                 createDir,
  sys_modem_as_id_e_type  asubs_id
);


/*-------- mre_efs_file_exists --------*/
/**
@brief  Return whether a given EFS path exists.

@param[in]  pPath Path of file to check for existence.

@return
  TRUE if the file is present, FALSE otherwise.
*/
boolean mre_efs_file_exists(
  char const             *pPath,
  sys_modem_as_id_e_type  asubs_id
);

/*-------- mre_efs_file_delete --------*/
/**
@brief  Delete the given EFS file

@param[in]  pPath Path of file to delete

@return
  TRUE if the file is deleted, FALSE otherwise.
*/
boolean mre_efs_file_delete(
  char const             *pPath,
  sys_modem_as_id_e_type  asubs_id
);

/*-------- mre_efs_deltree --------*/
/**
@brief  Delete an EFS path and the tree below it

@param[in]  pPath   Path to the tip of the tree to delete

@return
  0 if the call is successful, -1 otherwise
*/
int mre_efs_deltree(
  char const             *pPath
);

/*-------- mre_efs_get_nv_item --------*/
/**
@brief  Read an NV item

@param  item_code   Item to get
@param  data_ptr    Pointer where to put the item
@param  context     Context of the subscription

@return

*/
nv_stat_enum_type mre_efs_get_nv_item(
  nv_items_enum_type   item_code,       /* Item to get                      */
  nv_item_type        *data_ptr,        /* Pointer where to put the item    */
  uint16               context          /* Context of the subscription */
);

#endif /* _MRE_EFS_H_ */
