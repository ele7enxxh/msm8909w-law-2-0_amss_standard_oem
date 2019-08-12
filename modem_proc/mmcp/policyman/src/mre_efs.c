/**
  @file mre_efs.c

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/mre_efs.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_dbg.h"
#include "mre_efs.h"
#include "mre_util.h"

#include "stringl/stringl.h"
#include "stdio.h"
#include "fs_sys_types.h"
#include "fs_diag_access.h"
#include "fs_fcntl.h"
#include "mcfg_fs.h"


/*--------------------
  Command item to NV
--------------------*/
STATIC nv_cmd_type  mre_nv_cmd_buf;


/*-------- to_mcfg_subs --------*/
STATIC mcfg_fs_sub_id_e_type to_mcfg_subs(
  sys_modem_as_id_e_type    asubs_id
)
{
  mcfg_fs_sub_id_e_type  mcfg_subs;

  switch (asubs_id)
  {
    case SYS_MODEM_AS_ID_3:
      mcfg_subs = MCFG_FS_SUBID_2;
      break;

    case SYS_MODEM_AS_ID_2:
      mcfg_subs = MCFG_FS_SUBID_1;
      break;

    case SYS_MODEM_AS_ID_NONE:
      mcfg_subs = MCFG_FS_SUBID_NONE;
      break;

    case SYS_MODEM_AS_ID_1:
    default:
      mcfg_subs = MCFG_FS_SUBID_0;
      break;
  }

  return mcfg_subs;
}

/*-------- mre_efs_get_file --------*/
mre_efs_status_t mre_efs_get_file(
  const char              *pPath,
  void                   **ppData,
  size_t                  *pSize,
  sys_modem_as_id_e_type   asubs_id
)
{
  struct fs_stat          st;
  mre_efs_status_t        status      = MRE_EFS_STATUS_SUCCESS;
  byte                   *pData       = NULL;
  mcfg_fs_status_e_type   mcfg_status;
  mcfg_fs_sub_id_e_type   mcfg_subs;

  memset(&st, 0, sizeof(st));

  mcfg_subs = to_mcfg_subs(asubs_id);

  /* Get the size of the file.
  */
  mcfg_status = mcfg_fs_stat( pPath,
                              &st,
                              MCFG_FS_TYPE_EFS,
                              mcfg_subs );

  if (MCFG_FS_STATUS_OK != mcfg_status)
  {
    MRE_MSG_ERROR_3( SUBS_PREFIX "Error %d from mcfg_fs_stat, efs_err %d",
                     mcfg_subs,
                     mcfg_status,
                     mcfg_fs_errno(MCFG_FS_TYPE_EFS) );

    if (MCFG_FS_STATUS_NOT_ACTIVE == mcfg_status)
    {
      status = MRE_EFS_STATUS_NOFILE;
    }
    else
    {
      status = MRE_EFS_STATUS_ERROR;
    }

    goto Done;
  }

  /* Allocate the buffer. The extra byte serves as a string
   * terminator if the buffer interpreted as a string.
   */
  pData = mre_mem_alloc(st.st_size + 1);

  /* Always return the buffer and length to the caller
   * regardless of whether the read is successful or not.
   */
  *pSize  = st.st_size;
  *ppData = pData;

  /* Read the data
  */
  mcfg_status = mcfg_fs_read( pPath,
                              *ppData,
                              *pSize,
                              MCFG_FS_TYPE_EFS,
                              mcfg_subs );

  /*  Make sure that we read what we expected to.
   */
  if (MCFG_FS_STATUS_OK != mcfg_status)
  {
    MRE_MSG_ERROR_4( SUBS_PREFIX "Error %d from mcfg_fs_read(%d bytes), efs_err %d",
                     mcfg_subs,
                     mcfg_status,
                     *pSize,
                     mcfg_fs_errno(MCFG_FS_TYPE_EFS) );

    status = MRE_EFS_STATUS_ERROR;
  }

Done:
  return status;
}

/*-------- mre_efs_put_file --------*/
mre_efs_status_t mre_efs_put_file(
  char const             *pPath,
  void                   *pData,
  size_t                  dataSize,
  boolean                 createDir,
  sys_modem_as_id_e_type  asubs_id
)
{
  mre_efs_status_t        status = MRE_EFS_STATUS_SUCCESS;
  int                     oFlags = O_WRONLY | O_TRUNC | O_CREAT;
  mcfg_fs_sub_id_e_type   mcfg_subs;
  mcfg_fs_status_e_type   mcfg_status;

  mcfg_subs = to_mcfg_subs(asubs_id);

  if (createDir)
  {
    oFlags |= O_AUTODIR;
  }

  mcfg_status = mcfg_fs_write( pPath,
                               pData,
                               dataSize,
                               oFlags,
                               ALLPERMS,
                               MCFG_FS_TYPE_EFS,
                               mcfg_subs );

  if (MCFG_FS_STATUS_OK != mcfg_status)
  {
    MRE_MSG_ERROR_4( SUBS_PREFIX "Error %d from mcfg_fs_write(%d bytes), efs_err %d",
                     mcfg_subs,
                     mcfg_status,
                     dataSize,
                     mcfg_fs_errno(MCFG_FS_TYPE_EFS) );

    status = MRE_EFS_STATUS_ERROR ;
  }

  return status;
}

/*-------- mre_efs_file_exists --------*/
boolean mre_efs_file_exists(
  char const             *pPath,
  sys_modem_as_id_e_type  asubs_id
)
{
  struct fs_stat          st;
  mcfg_fs_status_e_type   mcfg_status;
  mcfg_fs_sub_id_e_type   mcfg_subs;

  mcfg_subs = to_mcfg_subs(asubs_id);

  mcfg_status = mcfg_fs_stat( pPath,
                              &st,
                              MCFG_FS_TYPE_EFS,
                              mcfg_subs );

  return (MCFG_FS_STATUS_OK == mcfg_status);
}

/*-------- mre_efs_file_delete --------*/
boolean mre_efs_file_delete(
  char const             *pPath,
  sys_modem_as_id_e_type  asubs_id
)
{
  mcfg_fs_status_e_type   mcfg_status;
  mcfg_fs_sub_id_e_type   mcfg_subs;

  mcfg_subs = to_mcfg_subs(asubs_id);

  mcfg_status = mcfg_fs_delete( pPath,
                                MCFG_FS_TYPE_EFS,
                                mcfg_subs );

  return (MCFG_FS_STATUS_OK == mcfg_status);
}

/*-------- mre_efs_deltree --------*/
int mre_efs_deltree(
  char const             *pPath
)
{
  return mcfg_fs_deltree( pPath,
                          MCFG_FS_TYPE_EFS );
}



/*-------- mre_efs_get_nv_item --------*/
nv_stat_enum_type mre_efs_get_nv_item(
  nv_items_enum_type   item_code,       /* Item to get                      */
  nv_item_type        *data_ptr,        /* Pointer where to put the item    */
  uint16               context          /* Context of the subscription */
)
{
  nv_cmd_ext_type   nv_cmd_ext_buf;

  memset( &nv_cmd_ext_buf, 0x00, sizeof(nv_cmd_ext_type) );

  /*-------------------------------------------------------------------------
    Prepare command buffer to get the item from NV.
    -------------------------------------------------------------------------*/
  mre_nv_cmd_buf.cmd        = NV_READ_F;                       /* Read request        */
  mre_nv_cmd_buf.tcb_ptr    = NULL;                            /* NULL for making NV read a sunchronous call */
  mre_nv_cmd_buf.sigs       = (rex_sigs_type)NULL;             /* NULL for making NV read a sunchronous call */
  mre_nv_cmd_buf.done_q_ptr = NULL;                            /* Do not enqueue when done */
  mre_nv_cmd_buf.item       = item_code;                       /* Item to get         */
  mre_nv_cmd_buf.data_ptr   = data_ptr;                        /* Where to return it  */

  /*-------------------------------------------------------------------------
    Issue the command.
    -------------------------------------------------------------------------*/
  nv_cmd_ext_buf.nvcmd   = &mre_nv_cmd_buf;
  nv_cmd_ext_buf.context = context;

  MRE_UT_MSG_2( "nv_cmd %d requested for item %d",
                nv_cmd_ext_buf.nvcmd->cmd,
                nv_cmd_ext_buf.nvcmd->item );

  nv_cmd_ext( &nv_cmd_ext_buf );               /* Issue the request    */

  if ( mre_nv_cmd_buf.status == NV_BUSY_S )
  {
    MRE_MSG_ERROR_2( "NV Read Failed Item %d status %d",
                     mre_nv_cmd_buf.item,
                     mre_nv_cmd_buf.status );
  }

  return mre_nv_cmd_buf.status;
}

