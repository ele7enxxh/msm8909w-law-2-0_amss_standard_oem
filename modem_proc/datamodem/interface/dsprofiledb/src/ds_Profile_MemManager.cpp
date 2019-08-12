/*===========================================================================
  FILE: ds_Profile_MemManager.cpp

  This file defines functions for translation of Class id to pool id macros.
  It also provides default initializations of PS mem pool objects for all
  the different DS Profile DB objects.
  
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofiledb/src/ds_Profile_MemManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------  
  2013-06-08 am  Created module.

===========================================================================*/
/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/


#include "ds_Profile_MemManager.h"
#include "ds_Profile_FileEFS.h"
#include "ds_Profile_CallbackMgrModem.h"
#include "ds_Profile_DBMgr.h"
#include "ds_Profile_MutexModem.h"
#include "ds_Profile_ParamsGeneral.h"
#include "ds_Profile_Params3GPPTech.h"
#include "ds_Profile_Params3GPP2Tech.h"
#include "ds_Profile_ParamsCommonTech.h"
#include "ds_Profile_Profile.h"
#include "ds_Profile_TechMgr.h"
#include "ds_Profile_TechMgr3GPP.h"
#include "ds_Profile_TechMgr3GPP2.h"
#include "ds_Profile_Log.h"
#include "ds_Profile_NVMgr.h"

using namespace ds::Profile;

/*---------------------------------------------------------------------------
  Macros for sizes of objects of these classes.
---------------------------------------------------------------------------*/
#define PROFILE_DB_CALLBACK_MGR_MODEM_SIZE              ((sizeof(CallbackMgrModem) + 3) & ~3)
#define PROFILE_DB_DBMGR_SIZE                           ((sizeof(DBMgr) + 3) & ~3)
#define PROFILE_DB_FILE_EFS_SIZE                        ((sizeof(FileEFS) + 3) & ~3)
#define PROFILE_DB_MUTEX_MODEM_SIZE                     ((sizeof(MutexModem) + 3) & ~3)
#define PROFILE_DB_PARAMS_3GPP_SIZE                     ((sizeof(Params3GPPTechConcrete) + 3) & ~3)
#define PROFILE_DB_PARAMS_3GPP2_SIZE                    ((sizeof(Params3GPP2TechConcrete) + 3) & ~3)
#define PROFILE_DB_PARAMS_COMMON_SIZE                   ((sizeof(ParamsCommonTechConcrete) + 3) & ~3)
#define PROFILE_DB_PARAMS_GENERAL_SIZE                  ((sizeof(ParamsGeneral) + 3) & ~3)
#define PROFILE_DB_PROFILE_SIZE                         ((sizeof(Profile) + 3) & ~3)
#define PROFILE_DB_TECH_MGR_3GPP_SIZE                   ((sizeof(TechMgr3GPP) + 3) & ~3)
#define PROFILE_DB_TECH_MGR_3GPP2_SIZE                  ((sizeof(TechMgr3GPP2) + 3) & ~3)
#define PROFILE_DB_NVMGR_SIZE                           ((sizeof(NVMgr) + 3) & ~3)
       
/*---------------------------------------------------------------------------
  Macros for number of buffers needed, high and low watermarks.
  These are valid for both high end and low end targets.
---------------------------------------------------------------------------*/
#define PROFILE_DB_CALLBACK_MGR_MODEM_NUM_BUFS   (1)
#define PROFILE_DB_DBMGR_NUM_BUFS                (1)
#define PROFILE_DB_FILE_EFS_NUM_BUFS             (1)
#define PROFILE_DB_MUTEX_MODEM_NUM_BUFS          (2)
#define PROFILE_DB_NVMGR_NUM_BUFS                (1)

/* factor for callback needs and for all kinds of temporary objects (i.e GetProfiles() ) */
#define PROFILE_DB_PROFILE_NUM_BUFS              (DS_PROFILE_DB_MAX_PROFILE_COUNT) * 10 
#define PROFILE_DB_PARAMS_3GPP_NUM_BUFS          (PROFILE_DB_PROFILE_NUM_BUFS)
#define PROFILE_DB_PARAMS_3GPP2_NUM_BUFS         (PROFILE_DB_PROFILE_NUM_BUFS)
#define PROFILE_DB_PARAMS_COMMON_NUM_BUFS        (PROFILE_DB_PROFILE_NUM_BUFS)
#define PROFILE_DB_PARAMS_GENERAL_NUM_BUFS       (PROFILE_DB_PROFILE_NUM_BUFS)


#define PROFILE_DB_TECH_MGR_3GPP_NUM_BUFS        (1)
#define PROFILE_DB_TECH_MGR_3GPP2_NUM_BUFS       (1)

#define PROFILE_DB_CALLBACK_MGR_MODEM_HIGH_WM   PROFILE_DB_CALLBACK_MGR_MODEM_NUM_BUFS
#define PROFILE_DB_DBMGR_HIGH_WM                PROFILE_DB_DBMGR_NUM_BUFS
#define PROFILE_DB_FILE_EFS_HIGH_WM             PROFILE_DB_FILE_EFS_NUM_BUFS
#define PROFILE_DB_MUTEX_MODEM_HIGH_WM          PROFILE_DB_MUTEX_MODEM_NUM_BUFS
#define PROFILE_DB_PARAMS_3GPP_HIGH_WM          PROFILE_DB_PARAMS_3GPP_NUM_BUFS
#define PROFILE_DB_PARAMS_3GPP2_HIGH_WM         PROFILE_DB_PARAMS_3GPP2_NUM_BUFS
#define PROFILE_DB_PARAMS_COMMON_HIGH_WM        PROFILE_DB_PARAMS_COMMON_NUM_BUFS
#define PROFILE_DB_PARAMS_GENERAL_HIGH_WM       PROFILE_DB_PARAMS_GENERAL_NUM_BUFS
#define PROFILE_DB_PROFILE_HIGH_WM              PROFILE_DB_PROFILE_NUM_BUFS
#define PROFILE_DB_TECH_MGR_3GPP_HIGH_WM        PROFILE_DB_TECH_MGR_3GPP_NUM_BUFS
#define PROFILE_DB_TECH_MGR_3GPP2_HIGH_WM       PROFILE_DB_TECH_MGR_3GPP2_NUM_BUFS
#define PROFILE_DB_NVMGR_HIGH_WM                PROFILE_DB_NVMGR_NUM_BUFS

#define PROFILE_DB_CALLBACK_MGR_MODEM_LOW_WM   0
#define PROFILE_DB_DBMGR_LOW_WM                0
#define PROFILE_DB_FILE_EFS_LOW_WM             0
#define PROFILE_DB_MUTEX_MODEM_LOW_WM          0
#define PROFILE_DB_PARAMS_3GPP_LOW_WM          0
#define PROFILE_DB_PARAMS_3GPP2_LOW_WM         0
#define PROFILE_DB_PARAMS_COMMON_LOW_WM        0
#define PROFILE_DB_PARAMS_GENERAL_LOW_WM       0
#define PROFILE_DB_PROFILE_LOW_WM              0
#define PROFILE_DB_TECH_MGR_3GPP_LOW_WM        0
#define PROFILE_DB_TECH_MGR_3GPP2_LOW_WM       0
#define PROFILE_DB_NVMGR_LOW_WM                0


/*---------------------------------------------------------------------------
  Allocate memory to hold different ds profile db objects along with ps_mem header.
---------------------------------------------------------------------------*/
static int callback_mgr_modem_buf [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_CALLBACK_MGR_MODEM_NUM_BUFS, 
                                                           PROFILE_DB_CALLBACK_MGR_MODEM_SIZE)];
static int dbmgr_buf              [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_DBMGR_NUM_BUFS, 
                                                           PROFILE_DB_DBMGR_SIZE)];
static int file_efs_buf           [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_FILE_EFS_NUM_BUFS, 
                                                           PROFILE_DB_FILE_EFS_SIZE)];
static int mutex_modem_buf        [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_MUTEX_MODEM_NUM_BUFS, 
                                                           PROFILE_DB_MUTEX_MODEM_SIZE)];
static int params_3gpp_buf        [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_PARAMS_3GPP_NUM_BUFS, 
                                                           PROFILE_DB_PARAMS_3GPP_SIZE)];
static int params_3gpp2_buf       [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_PARAMS_3GPP2_NUM_BUFS, 
                                                           PROFILE_DB_PARAMS_3GPP2_SIZE)];
static int params_common_buf      [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_PARAMS_COMMON_NUM_BUFS, 
                                                           PROFILE_DB_PARAMS_COMMON_SIZE)];
static int params_general_buf     [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_PARAMS_GENERAL_NUM_BUFS, 
                                                           PROFILE_DB_PARAMS_GENERAL_SIZE)];
static int profile_buf            [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_PROFILE_NUM_BUFS, 
                                                           PROFILE_DB_PROFILE_SIZE)];
static int tech_mgr_3gpp_buf      [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_TECH_MGR_3GPP_NUM_BUFS, 
                                                           PROFILE_DB_TECH_MGR_3GPP_SIZE)];
static int tech_mgr_3gpp2_buf     [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_TECH_MGR_3GPP2_NUM_BUFS, 
                                                           PROFILE_DB_TECH_MGR_3GPP2_SIZE)];
static int nvmgr_buf              [PS_MEM_GET_TOT_SIZE_OPT(PROFILE_DB_NVMGR_NUM_BUFS, 
                                                           PROFILE_DB_NVMGR_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*---------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to actual object array.
---------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type      * callback_mgr_modem_buf_hdr_ptr [PROFILE_DB_CALLBACK_MGR_MODEM_NUM_BUFS];
static CallbackMgrModem         * callback_mgr_modem_buf_ptr     [PROFILE_DB_CALLBACK_MGR_MODEM_NUM_BUFS];

static ps_mem_buf_hdr_type      * dbmgr_buf_hdr_ptr              [PROFILE_DB_DBMGR_NUM_BUFS];
static DBMgr                    * dbmgr_buf_ptr                  [PROFILE_DB_DBMGR_NUM_BUFS];

static ps_mem_buf_hdr_type      * file_efs_buf_hdr_ptr           [PROFILE_DB_FILE_EFS_NUM_BUFS];
static FileEFS                  * file_efs_buf_ptr               [PROFILE_DB_FILE_EFS_NUM_BUFS];

static ps_mem_buf_hdr_type      * mutex_modem_buf_hdr_ptr        [PROFILE_DB_MUTEX_MODEM_NUM_BUFS];
static MutexModem               * mutex_modem_buf_ptr            [PROFILE_DB_MUTEX_MODEM_NUM_BUFS];

static ps_mem_buf_hdr_type      * params_3gpp_buf_hdr_ptr        [PROFILE_DB_PARAMS_3GPP_NUM_BUFS];
static Params3GPPTechConcrete   * params_3gpp_buf_ptr            [PROFILE_DB_PARAMS_3GPP_NUM_BUFS];

static ps_mem_buf_hdr_type      * params_3gpp2_buf_hdr_ptr       [PROFILE_DB_PARAMS_3GPP2_NUM_BUFS];
static Params3GPPTechConcrete   * params_3gpp2_buf_ptr           [PROFILE_DB_PARAMS_3GPP2_NUM_BUFS];

static ps_mem_buf_hdr_type      * params_common_buf_hdr_ptr      [PROFILE_DB_PARAMS_COMMON_NUM_BUFS];
static ParamsCommonTechConcrete * params_common_buf_ptr          [PROFILE_DB_PARAMS_COMMON_NUM_BUFS];
/* ***/

static ps_mem_buf_hdr_type      * params_general_buf_hdr_ptr     [PROFILE_DB_PARAMS_GENERAL_NUM_BUFS];
static ParamsGeneral            * params_general_buf_ptr         [PROFILE_DB_PARAMS_GENERAL_NUM_BUFS];

static ps_mem_buf_hdr_type      * profile_buf_hdr_ptr            [PROFILE_DB_PROFILE_NUM_BUFS];
static Profile                  * profile_buf_ptr                [PROFILE_DB_PROFILE_NUM_BUFS];

static ps_mem_buf_hdr_type      * tech_mgr_3gpp_buf_buf_hdr_ptr  [PROFILE_DB_TECH_MGR_3GPP_NUM_BUFS];
static TechMgr3GPP              * tech_mgr_3gpp_buf_buf_ptr      [PROFILE_DB_TECH_MGR_3GPP_NUM_BUFS];

static ps_mem_buf_hdr_type      * tech_mgr_3gpp2_buf_buf_hdr_ptr [PROFILE_DB_TECH_MGR_3GPP2_NUM_BUFS];
static TechMgr3GPP2             * tech_mgr_3gpp2_buf_buf_ptr     [PROFILE_DB_TECH_MGR_3GPP2_NUM_BUFS];

static ps_mem_buf_hdr_type      * nvmgr_buf_hdr_ptr              [PROFILE_DB_NVMGR_NUM_BUFS];
static NVMgr                    * nvmgr_buf_ptr                  [PROFILE_DB_NVMGR_NUM_BUFS];

#endif /* FEATURE_DATA_PS_MEM_DEBUG */


int MemoryManager::MemPoolInit()    
{
    
  /*-------------------------------------------------------------------------
    Initialize ds profile db mem pools
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_CALLBACK_MGR_MODEM_TYPE,
                           callback_mgr_modem_buf,
                           PROFILE_DB_CALLBACK_MGR_MODEM_SIZE,
                           PROFILE_DB_CALLBACK_MGR_MODEM_NUM_BUFS,
                           PROFILE_DB_CALLBACK_MGR_MODEM_HIGH_WM,
                           PROFILE_DB_CALLBACK_MGR_MODEM_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) callback_mgr_modem_buf_hdr_ptr,
                           (int *) callback_mgr_modem_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_DBMGR_TYPE,
                           dbmgr_buf,
                           PROFILE_DB_DBMGR_SIZE,
                           PROFILE_DB_DBMGR_NUM_BUFS,
                           PROFILE_DB_DBMGR_HIGH_WM,
                           PROFILE_DB_DBMGR_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) dbmgr_buf_hdr_ptr,
                           (int *) dbmgr_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_FILE_EFS_TYPE,
                           file_efs_buf,
                           PROFILE_DB_FILE_EFS_SIZE,
                           PROFILE_DB_FILE_EFS_NUM_BUFS,
                           PROFILE_DB_FILE_EFS_HIGH_WM,
                           PROFILE_DB_FILE_EFS_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) file_efs_buf_hdr_ptr,
                           (int *) file_efs_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_MUTEX_MODEM_TYPE,
                           mutex_modem_buf,
                           PROFILE_DB_MUTEX_MODEM_SIZE,
                           PROFILE_DB_MUTEX_MODEM_NUM_BUFS,
                           PROFILE_DB_MUTEX_MODEM_HIGH_WM,
                           PROFILE_DB_MUTEX_MODEM_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) mutex_modem_buf_hdr_ptr,
                           (int *) mutex_modem_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_PARAMS_3GPP_TYPE,
                           params_3gpp_buf,
                           PROFILE_DB_PARAMS_3GPP_SIZE,
                           PROFILE_DB_PARAMS_3GPP_NUM_BUFS,
                           PROFILE_DB_PARAMS_3GPP_HIGH_WM,
                           PROFILE_DB_PARAMS_3GPP_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) params_3gpp_buf_hdr_ptr,
                           (int *) params_3gpp_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_PARAMS_3GPP2_TYPE,
                           params_3gpp2_buf,
                           PROFILE_DB_PARAMS_3GPP2_SIZE,
                           PROFILE_DB_PARAMS_3GPP2_NUM_BUFS,
                           PROFILE_DB_PARAMS_3GPP2_HIGH_WM,
                           PROFILE_DB_PARAMS_3GPP2_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) params_3gpp2_buf_hdr_ptr,
                           (int *) params_3gpp2_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_PARAMS_COMMON_TYPE,
                           params_common_buf,
                           PROFILE_DB_PARAMS_COMMON_SIZE,
                           PROFILE_DB_PARAMS_COMMON_NUM_BUFS,
                           PROFILE_DB_PARAMS_COMMON_HIGH_WM,
                           PROFILE_DB_PARAMS_COMMON_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) params_common_buf_hdr_ptr,
                           (int *) params_common_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_PARAMS_GENERAL_TYPE,
                           params_general_buf,
                           PROFILE_DB_PARAMS_GENERAL_SIZE,
                           PROFILE_DB_PARAMS_GENERAL_NUM_BUFS,
                           PROFILE_DB_PARAMS_GENERAL_HIGH_WM,
                           PROFILE_DB_PARAMS_GENERAL_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) params_general_buf_hdr_ptr,
                           (int *) params_general_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_PROFILE_TYPE,
                           profile_buf,
                           PROFILE_DB_PROFILE_SIZE,
                           PROFILE_DB_PROFILE_NUM_BUFS,
                           PROFILE_DB_PROFILE_HIGH_WM,
                           PROFILE_DB_PROFILE_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) profile_buf_hdr_ptr,
                           (int *) profile_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_TECH_MGR_3GPP_TYPE,
                           tech_mgr_3gpp_buf,
                           PROFILE_DB_TECH_MGR_3GPP_SIZE,
                           PROFILE_DB_TECH_MGR_3GPP_NUM_BUFS,
                           PROFILE_DB_TECH_MGR_3GPP_HIGH_WM,
                           PROFILE_DB_TECH_MGR_3GPP_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) tech_mgr_3gpp_buf_buf_hdr_ptr,
                           (int *) tech_mgr_3gpp_buf_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_TECH_MGR_3GPP2_TYPE,
                           tech_mgr_3gpp2_buf,
                           PROFILE_DB_TECH_MGR_3GPP2_SIZE,
                           PROFILE_DB_TECH_MGR_3GPP2_NUM_BUFS,
                           PROFILE_DB_TECH_MGR_3GPP2_HIGH_WM,
                           PROFILE_DB_TECH_MGR_3GPP2_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) tech_mgr_3gpp2_buf_buf_hdr_ptr,
                           (int *) tech_mgr_3gpp2_buf_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PROFILE_DB_NV_MGR_TYPE,
                           nvmgr_buf,
                           PROFILE_DB_NVMGR_SIZE,
                           PROFILE_DB_NVMGR_NUM_BUFS,
                           PROFILE_DB_NVMGR_HIGH_WM,
                           PROFILE_DB_NVMGR_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) nvmgr_buf_hdr_ptr,
                           (int *) nvmgr_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }


  return 0;

bail:

  PROFILE_DB_LOG_ERROR("MemPoolInit(): ERROR while initializing memory pools");

  return -1;

} /* MemPoolInit() */