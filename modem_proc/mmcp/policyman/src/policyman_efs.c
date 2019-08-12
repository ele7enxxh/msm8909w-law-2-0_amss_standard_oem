/**
  @file policyman_efs.c

  @brief PM abstraction layer above EFS
*/

/*
    Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_efs.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_efs.h"
#include "policyman_dbg.h"
#include "policyman_efs.h"
#include "fs_diag_access.h"

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */


/*-------- pfnObliterateFunc --------*/
typedef size_t (*pfnObliterateFunc)(
  const char             *pPath,
  sys_modem_as_id_e_type  subsId
);

typedef struct
{
  const char        *pPath;
  pfnObliterateFunc  entryDeleteHandler;
  boolean            fileIsXml;
} efs_file_entry;

/*===========================================================================
FUNCTION policyman_diag_access_check_cb()

DESCRIPTION
  This function is called by the Diag module as a result of any Diag access
  of the policyman EFS directory. It is initially registered with Diag for
  knowing the access of policyman EFS directory.

PARAMETERS
  file_name_ptr: Name of the file that needs to be permitted
  op_type      : File operation type

RETURN VALUE
  TRUE:  If we want to allow access
  FALSE: If we want to reject access

DEPENDENCIES
  policyman service must be registered with Diag module via
  diag_fs_register_access_check API.

SIDE EFFECTS
  None

===========================================================================*/
STATIC boolean policyman_diag_access_check_cb(
  char      *file_name_ptr,
  uint32     op_type
)
{
  /* The conditions for rejecting are these:
     1. File name is of interest to us
     2. Operation is one of write, delete, rename, chmod */

  boolean ret =   (op_type == DIAG_FS_CHECK_OPEN)
               || (op_type == DIAG_FS_CHECK_DISP_DIRS)
               || (op_type == DIAG_FS_CHECK_DISP_FILES)
               || (op_type == DIAG_FS_CHECK_ITERATE)
               || (op_type == DIAG_FS_CHECK_READ)
               || (op_type == DIAG_FS_CHECK_GET_ATTRIB)
               || (op_type == DIAG_FS_CHECK_CLOSE)
               || (op_type == DIAG_FS_CHECK_OPENDIR)
               || (op_type == DIAG_FS_CHECK_READDIR)
               || (op_type == DIAG_FS_CHECK_CLOSEDIR)
               || (op_type == DIAG_FS_CHECK_STAT)
               || (op_type == DIAG_FS_CHECK_LSTAT)
               || (op_type == DIAG_FS_CHECK_FSTAT)
               || (op_type == DIAG_FS_CHECK_STATFS)
               || (op_type == DIAG_FS_CHECK_ACCESS)
               ;

  POLICYMAN_MSG_HIGH_2( "policyman_diag_access_check_cb op_type %d ret %d",
                        op_type,
                        ret );

  return ret;
} /* policyman_diag_access_check_cb */


/*-------- policyman_efs_initialize --------*/
void policyman_efs_initialize(
  void
)
{
  static  const char  *efs_directory_list[] = {POLICYMAN_EFS_ROOT_DIR};
  struct  fs_stat      file_stat;

  /*  If a file named "locked" is in the policyman directory, restrict
   *  access to the directory via diag.
   */
  if (efs_stat(POLICYMAN_LOCKED_EFS_PATH, &file_stat) == 0)
  {
    diag_fs_register_access_check( efs_directory_list,
                                   ARR_SIZE(efs_directory_list),
                                   policyman_diag_access_check_cb );
  }
}


/*-------- policyman_efs_deinitialize --------*/
void policyman_efs_deinitialize(
  void
)
{
}

/*-------- policyman_efs_obliterate --------*/
int policyman_efs_obliterate(
  void
)
{
  return mre_efs_deltree(POLICYMAN_EFS_ROOT_DIR);
}

/*-------- policyman_efs_obliterate_dir --------*/
STATIC size_t policyman_efs_obliterate_dir(
  const char             *pPath,
  sys_modem_as_id_e_type  subsId
)
{
  return (   (SYS_MODEM_AS_ID_NONE == subsId)
          && (mre_efs_deltree(pPath) == 0) ) ? 1 : 0;
}

/*-------- policyman_efs_obliterate_device_file --------*/
STATIC size_t policyman_efs_obliterate_device_file(
  const char             *pPath,
  sys_modem_as_id_e_type  subsId
)
{
  return (   (SYS_MODEM_AS_ID_NONE == subsId)
          && mre_efs_file_delete(pPath, subsId) ) ? 1 : 0;
}

/*-------- policyman_efs_obliterate_subs_file --------*/
STATIC size_t policyman_efs_obliterate_subs_file(
  const char             *pPath,
  sys_modem_as_id_e_type  subsId
)
{
  return (   (SYS_MODEM_AS_ID_NONE != subsId)
          && mre_efs_file_delete(pPath, subsId) ) ? 1 : 0;
}

/*-------- policyman_efs_obliterate_persist_file --------*/
STATIC size_t policyman_efs_obliterate_persist_file(
  const char             *pPath,
  sys_modem_as_id_e_type  subsId
)
{
  int    startSubs       = subsId;
  int    endSubs         = subsId;
  int    subsIdx;
  size_t deleteSuccesses = 0;

  if (SYS_MODEM_AS_ID_NONE == subsId)
  {
    startSubs = SYS_MODEM_AS_ID_1;
    endSubs   = POLICYMAN_NUM_SUBS;
  }

  subsIdx = startSubs;

  do
  {
    if (mre_efs_file_delete(pPath, (sys_modem_as_id_e_type)subsIdx))
    {
      deleteSuccesses++;
    }

    subsIdx++;

  } while (subsIdx < endSubs);

  return deleteSuccesses;
}

/*-------- policyman_efs_count_files_to_delete --------*/
STATIC size_t policyman_efs_count_files_to_delete(
  pfnObliterateFunc       pHandler,
  sys_modem_as_id_e_type  subsId
)
{
  return (   (pHandler == policyman_efs_obliterate_persist_file)
          && (SYS_MODEM_AS_ID_NONE == subsId) ) ? POLICYMAN_NUM_SUBS : 1;
}

STATIC efs_file_entry efs_files[] = {
    //   Path                             deleteHandler
    { POLICYMAN_DEVICE_CONFIG_EFS_PATH,   policyman_efs_obliterate_device_file,  TRUE  },
    { POLICYMAN_DEVICE_CONFIG_NV_PATH,    policyman_efs_obliterate_device_file,  FALSE },
    { POLICYMAN_SEGMENT_LOADING_EFS_PATH, policyman_efs_obliterate_device_file,  TRUE  },
    { POLICYMAN_RESTRICTIONS_EFS_PATH,    policyman_efs_obliterate_device_file,  TRUE  },
    { POLICYMAN_RAT_ORDER_NV_PATH,        (pfnObliterateFunc)NULL,               FALSE },
    { POLICYMAN_TIMERS_EFS_DIR,           policyman_efs_obliterate_dir,          FALSE },
    { POLICYMAN_CARRIER_POLICY_EFS_PATH,  policyman_efs_obliterate_subs_file,    TRUE  },
    { POLICYMAN_RAT_ORDER_EFS_PATH,       policyman_efs_obliterate_subs_file,    TRUE  },
    { POLICYMAN_CURRENT_MCC_NV_PATH,      policyman_efs_obliterate_persist_file, FALSE },
    { POLICYMAN_CALL_MODE_NV_PATH,        policyman_efs_obliterate_persist_file, FALSE },
    { POLICYMAN_RAT_MASK_NV_PATH,         policyman_efs_obliterate_persist_file, FALSE },
    { POLICYMAN_SVC_MODE_NV_PATH,         policyman_efs_obliterate_persist_file, FALSE },
    { POLICYMAN_UE_MODE_NV_PATH,          policyman_efs_obliterate_persist_file, FALSE },
};

/*-------- policyman_efs_obliterate_subs_ext --------*/
int policyman_efs_obliterate_subs_ext(
  sys_modem_as_id_e_type  subsId,
  boolean                 deleteXml
)
{
  int      fileIndex;
  int      deleteFiles        = 0;
  int      deleteSuccesses    = 0;
  int      deleteDirs         = 0;

  for (fileIndex = 0; fileIndex < ARR_SIZE(efs_files); ++fileIndex)
  {
    efs_file_entry    *indexed_efs_file;
    pfnObliterateFunc  pHandler;

    indexed_efs_file = &efs_files[fileIndex];
    pHandler         = indexed_efs_file->entryDeleteHandler;

    if (  NULL != pHandler 
       && (!indexed_efs_file->fileIsXml || deleteXml)
       )
    {
      if (pHandler == policyman_efs_obliterate_dir)
      {
        deleteDirs++;
      }
      else
      {
        deleteFiles += policyman_efs_count_files_to_delete(pHandler, subsId);
      }

      deleteSuccesses += pHandler(indexed_efs_file->pPath, subsId);
    }
  }

  POLICYMAN_MSG_HIGH_5( SUBS_PREFIX "Obliterate_subs_ext: deleteXML(T/F) %d, dirs %d, files %d, successes %d",
                        subsId,
                        deleteXml,
                        deleteDirs,
                        deleteFiles,
                        deleteSuccesses );

  return deleteSuccesses;
}

/*-------- policyman_efs_obliterate_subs --------*/
int policyman_efs_obliterate_subs(
  sys_modem_as_id_e_type  subsId
)
{
  return policyman_efs_obliterate_subs_ext(subsId, TRUE);
}

