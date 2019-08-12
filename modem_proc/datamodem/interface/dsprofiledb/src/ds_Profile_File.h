#ifndef DS_PROFILE_FILE_H
#define DS_PROFILE_FILE_H
/*===========================================================================
  @file ds_Profile_File.h

  This file defines File abstract class which is wrapper for various file 
  operations with the profile

                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-04-14 am Created module

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_profile_db.h"
#include "ds_Profile_Profile.h"
#include "time.h"
#include "time_svc.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "ps_utils.h"
#ifdef __cplusplus
}
#endif

#ifdef FEATURE_USER_CONFIGURE_EFS
#define READ_FROM_INDEX(offset_ptr, type, data) \
      memscpy(&data,sizeof(type) , offset_ptr, sizeof(type));
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Profile
  {
    class File
    {    
      public:

        typedef enum {
          F_CREATE,
          F_CREATE_OVERWRITE,
          F_READ,
          F_WRITE,
          F_WRITE_CREATE,
          F_RW          
        } FlagType;  

        typedef enum {
          ERR_NO_FAIL,
          ERR_DIR_CREATE,
          ERR_DIR_ACCESS,
          ERR_FILE_ACCESS,
          ERR_PATH_LEN_EXCESS,
          ERR_STRING_FAIL,
          ERR_PARSE_FILENAME 
        } ReadErrFlag;

        File(){};
        File(ds_profile_db_profile_num_type num, FlagType flag = F_RW){};

        virtual ds_profile_db_result_type Open(ds_profile_db_profile_num_type num,
                                               FlagType flag = F_RW,
                                               ds_profile_db_subs_type subs = DB_PROFILE_ACTIVE_SUBSCRIPTION_1) = 0;
        virtual bool IsOpen() const = 0;
        virtual ds_profile_db_result_type Close() = 0;
        #ifdef FEATURE_USER_CONFIGURE_EFS
        virtual ds_profile_db_result_type Write(const Profile* profile,
                                                bool  is_reset_event,
                                                bool  reset_profile = false,
                                                bool is_create = false,
                                                bool is_3gpp2_default_profile = false) = 0;

        virtual ds_profile_db_result_type ReadTLV() = 0;
        #else
        virtual ds_profile_db_result_type Write(const Profile* profile) = 0;
        #endif
        virtual ds_profile_db_result_type Read(Profile** profile) const = 0;
        
        virtual ds_profile_db_result_type Delete(ds_profile_db_profile_num_type num,
                                                 ds_profile_db_subs_type subs) = 0;

        virtual ~File() = 0;

      protected:
         const void* GetProfileCParamsPtr(const Profile* profile, 
                                         ds_profile_db_tech_type tech) const;

        virtual ds_profile_db_result_type ReadAll(Profile** profiles, unsigned int* count,
                                                  ds_profile_db_subs_type subs = DB_PROFILE_ACTIVE_SUBSCRIPTION_1) = 0;
      
        friend class DBMgr;
    }; /* class File */
  } /* namespace Profile */
} /* namespace ds */
inline ds::Profile::File::~File(){}

#endif /* DS_PROFILE_FILE_H */
