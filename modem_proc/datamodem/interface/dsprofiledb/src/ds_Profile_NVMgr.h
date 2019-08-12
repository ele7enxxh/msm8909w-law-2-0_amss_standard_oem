#ifndef DS_PROFILE_NV_MGR_H
#define DS_PROFILE_NV_MGR_H
/*===========================================================================
  @file ds_Profile_NVMgr.h

  This file defines NV MGR class which is responsible for dispatching 
  the calls for EFS NV management

                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-01-16 ez Created module
  2013-04-10 am Added implementation

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_profile_db.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define DB_MAX_NV_FILE_PATH_LEN 72
#define DS_PROFILE_DB_NV_DIR "/nv/item_files/modem/data/dsprofiledb/"
/*======================================================================================================
  DS PROFILE NV RELATED DEFINITIONS
=======================================================================================================*/
typedef enum ds_profile_db_nv_item_e {
  DS_PROFILE_DB_NVI_ITEM_START  =  0, /* reserved for internal use only */

  DS_PROFILE_DB_ENABLE_SFS,     /* boolean */
  /* Add new items here */
  /* below items reserved for internal use only */
  DS_PROFILE_DB_NVI_ITEM_END, 
  DS_PROFILE_DB_NVI_ITEM_BIG = 0x7FFFFFFF
} ds_profile_db_nv_item_e_type;


#define DS_PROFILE_DB_NV_ITEM_FNAME_INDEX(item) ((item - DS_PROFILE_DB_NVI_ITEM_START) - 1)
#define DS_PROFILE_DB_NV_NUM_ITEMS              DS_PROFILE_DB_NV_ITEM_FNAME_INDEX(DS_PROFILE_DB_NVI_ITEM_END)
#define DS_PROFILE_DB_NV_FILENAME_MAX_LEN       (128)
#define DS_PROFILE_DB_NV_MAX_EFS_PATH_SIZE      (512)


using namespace ds::Profile;


namespace ds
{
  namespace Profile
  {
    class NVMgr
    {
 
      public:

        DS_PROFILE_DB_USE_MEMORY_MANAGEMENT(PS_MEM_PROFILE_DB_NV_MGR_TYPE);

        static ds_profile_db_result_type InitEFSConfFile();

        /*Check to see if EPC Profiles are supported*/
        uint8  GetEPCSupportVal();

        /*Read EFS file file and store the values in global DS_Profile_DB state*/
        void ReadEFSFile(ds_profile_db_efs_file_e_type ds_profile_db_file_type);

        ds_profile_db_result_type ReadFromNV(ds_profile_db_nv_item_e_type nv_file,
                                             void *                       data_ptr,
                                              fs_size_t                    data_size,
                                              ds_profile_db_subs_type      subs);

        ds_profile_db_result_type BuildPathFromNV( ds_profile_db_nv_item_e_type item,
                                                   char *                       path,
                                                   uint32                       path_len);

        ds_profile_db_result_type GetNVContent( const char * path,
                                                void *       data_ptr,
                                                fs_size_t    data_size,
                                                ds_profile_db_subs_type subs);
       
    private:
       uint8 ds_profile_db_epc_support;
       

    }; /* class NVMgr */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_NV_MGR_H */
