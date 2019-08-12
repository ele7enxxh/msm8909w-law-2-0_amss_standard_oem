#ifndef DS_PROFILE_FILE_SFS_H
#define DS_PROFILE_FILE_SFS_H
/*===========================================================================
  @file ds_Profile_FileSFS.h

  This file defines SFS File class which is wrapper for various file 
  operations with the profile done on the SFS

                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2014-02-05 bh Created module

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_Profile_File.h"
#include "ds_profile_db.h"
#include "ds_Profile_Profile.h"
#include "ds_Profile_MemManager.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Profile
  {
    class FileSFS : public File
    {
      public:

        DS_PROFILE_DB_USE_MEMORY_MANAGEMENT(PS_MEM_PROFILE_DB_FILE_EFS_TYPE);

        FileSFS():fs_handle(-1), profile_num(-1), is_new(false){};

        FileSFS(ds_profile_db_profile_num_type num, ds_profile_db_subs_type subs, FlagType flag = F_RW);

        static  ds_profile_db_result_type Init();

        virtual ds_profile_db_result_type Open(ds_profile_db_profile_num_type num,
                                               FlagType flag = F_RW,
                                               ds_profile_db_subs_type subs = (ds_profile_db_subs_type)ps_sys_get_default_data_subscription());
        
        virtual bool IsOpen() const ;
        virtual ds_profile_db_result_type Close();
        
        virtual ds_profile_db_result_type Read(Profile** profile) const;        
        virtual ds_profile_db_result_type Delete(ds_profile_db_profile_num_type num,
                                                 ds_profile_db_subs_type subs);

        #ifdef FEATURE_USER_CONFIGURE_EFS
        virtual ds_profile_db_result_type Write(const Profile* profile,
                                                bool is_reset_event,
                                                bool reset_profile=false,
                                                bool is_create = false,
                                                bool is_3gpp2_default_profile = false);

        virtual ds_profile_db_result_type ReadTLV();
        #else
        virtual ds_profile_db_result_type Write(const Profile* profile) const;
        #endif

        virtual ~FileSFS();        

      protected:

        virtual ds_profile_db_result_type ReadAll(Profile** profiles, unsigned int* count,
                                                  ds_profile_db_subs_type subs = 
                                                  (ds_profile_db_subs_type)ps_sys_get_default_data_subscription()); 

      private:


        virtual ds_profile_db_result_type Open(const char* file_name, FlagType flag = F_RW);

        #ifdef FEATURE_USER_CONFIGURE_EFS
        ds_profile_db_result_type Create (const Profile* profile,
                                          bool is_reset_event,
                                          bool reset_profile,
                                          bool is_create,
                                          bool is_3gpp2_default_profile) ;
        #else
        ds_profile_db_result_type Create (const Profile* profile) const;
        #endif
        ds_profile_db_result_type Update (const Profile* profile) const;

        ds_profile_db_result_type WriteParams(const Profile* profile,                                                        
                                              ds_profile_db_tech_type tech) const;

        ds_profile_db_result_type ReadParams(void* c_params,
                                             ds_profile_db_tech_type* tech,                                                      
                                             bool read_gen_params = false) const;
        #ifdef FEATURE_USER_CONFIGURE_EFS
        bool CheckIdentExistsHelper(ds_profile_db_ident_type ident,
                                    uint32 read_tlv,
                                    uint32* offset) const;

        ds_profile_db_result_type GetIdentToUpdateEFS(const Profile *profile,
                                                      ds_profile_db_supported_tech_mask tech_mask,
                                                      uint32 *offset,
                                                      ds_profile_db_efs_header *efs_header,
                                                      uint32 *read_num_tlvs,
                                                      boolean is_reset_event,
                                                      boolean update_gen_ident) const;

        #endif


        mutable int fs_handle;
        ds_profile_db_profile_num_type profile_num;
        bool is_new;

    }; /* class FileSFS */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_FILE_SFS_H */
