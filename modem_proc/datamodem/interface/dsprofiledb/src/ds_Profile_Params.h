#ifndef DS_PROFILE_PARAMS_H
#define DS_PROFILE_PARAMS_H
/*===========================================================================
  @file ds_Profile_Params.h

  This file defines abstract Params class which is a general interface for 
  all various param subcomponents of Profile class
  Each Params subcomponent implement a certain set of Profile parameters, it 
  should be completely independent on others
  

                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-04-22 am Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stddef.h> 
#include "ds_profile_db.h"
#include "ds_profile_db_internal.h"

extern "C"
{
#include "ds_util.h"
}

/*===========================================================================

                     VARIOUS PUBLIC DEFINES

===========================================================================*/
/* below MACRO's are using during param map initialization in every Param class
   responsible for a particular technology 
   
   Each technology has map of identifiers along with their info, which is 
   size and offset of a field associated with that ident in a tech specific 
   struct   
*/
#define DB_FIELD_OFFSET_IN_STRUCT(str, field) \
        (unsigned int)((char*)&(str.field) - (char*)(&str))

/* should be always invoked first, before using any other macros */
#define DB_FILL_PARAM_MAP_INIT(struct_type) struct_type params;

/* makes sure, no other info was filled for this ident previously */
#define DB_FILL_PARAM_VALIDATE_UNIQUE(ident) \
        if (mapParamsInfo[ident - MAP_START_RANGE].len) {\
        return DB_RESULT_FAIL;\
        } \

/* makes sure the ident is within the valid range for the particular tech
   we are trying to fill */
#define DB_FILL_PARAM_VALIDATE_WITHIN_RANGE(ident) \
        if (MAP_START_RANGE > ident && \
            MAP_END_RANGE   < ident  ) {\
          return DB_RESULT_FAIL;\
        } \

/* fills info for a particular ident inside param map
   the ident will be associated with the offset and size
   of the provided field */
#define DB_FILL_PARAM_MAP(ident, field) \
        DB_FILL_PARAM_VALIDATE_WITHIN_RANGE(ident) \
        DB_FILL_PARAM_VALIDATE_UNIQUE(ident) \
        mapParamsInfo[ident - MAP_START_RANGE] = \
        ParamInfo(DB_FIELD_OFFSET_IN_STRUCT(params, field), \
        sizeof(params.field));

/* same as above, however the size is provided by user, rather than 
   filling the sizeof field (required in some special cases 
   where a particular ident is only filling a part of some field in
   tech struct) */
#define DB_FILL_PARAM_MAP_WITH_SPECIAL_SIZE(ident, field, size) \
        DB_FILL_PARAM_VALIDATE_WITHIN_RANGE(ident) \
        DB_FILL_PARAM_VALIDATE_UNIQUE(ident) \
        mapParamsInfo[ident - MAP_START_RANGE] = \
        ParamInfo(DB_FIELD_OFFSET_IN_STRUCT(params, field), \
        size);
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
  namespace Profile
  {
    class Params
    {
      public:

        Params():changed_idents_list(NULL) {};

        ds_profile_db_result_type InitInstance();        

        //************************************
        // Method:      SetParam
        // FullName:    ds::Profile::Params::SetParam
        // Description: Set a particular param 
        //  
        // Access:      virtual protected 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_ident_type ident
        // Parameter:   const ds_profile_db_info_type * info - buffer and len
        //              len and buffer should be no more than the maximum len 
        //              of the ident
        // Parameter:   boolean is_set - TODO what is this param for
        //************************************
        virtual ds_profile_db_result_type SetParam(
          ds_profile_db_ident_type   ident,
          const ds_profile_db_info_type *info,
          boolean is_set);

        //************************************
        // Method:      GetParam
        // FullName:    ds::Profile::Params::GetParam
        // Description: Get a particular param
        //  
        // Access:      virtual protected 
        // Returns:     ds_profile_db_result_type
        // Qualifier:   const
        // Parameter:   ds_profile_db_ident_type ident
        // Parameter:   ds_profile_db_info_type * info - 
        //              input/output param
        //              input - buffer and len, should be enough
        //              to store the requested ident
        //              output - filled buffer and updated len 
        //              (in case the actual len is smaller than the provided
        //************************************
        virtual ds_profile_db_result_type GetParam (
          ds_profile_db_ident_type   ident,
          ds_profile_db_info_type    *info) const;

        virtual bool CompareParam (
          ds_profile_db_ident_type         ident,
          const ds_profile_db_info_type    *info) const;

        virtual ds_profile_db_result_type GetFieldInfo (
          ds_profile_db_ident_type   ident,
          unsigned int* offset, 
          uint16* len,
          void** buf) const; 

        virtual void GetCParams(void* params) const;

        ds_util_list_hndl_type GetChangedIdentsList () const;
        
        virtual ~Params() = 0;

      protected:

        ds_profile_db_result_type InitInstance(ds_util_list_hndl_type list);

        /* list to track param changes */
        ds_util_list_hndl_type changed_idents_list;        

        virtual Params* Clone() const = 0;
        
        //************************************
        // Method:      set_fn_type
        // FullName:    ds::Profile::Params::set_fn_type
        // Description: Provides special behavior for a particular ident during set operation
        //              like setting additional validity flags, or even additional params etc
        //              Can't fail
        //  
        // Access:      protected         
        //************************************
        typedef void                      (*set_fn_type)(
          ds_profile_db_ident_type       ident, 
          const ds_profile_db_info_type* info,
          void*                          params,
          boolean is_set);

        //************************************
        // Method:      get_fn_type
        // FullName:    ds::Profile::Params::get_fn_type
        // Description: Provides special behavior during get operation
        //              Like updating len inside info, checking value of some validity flag, 
        //              etc
        //              Unlike special set function - can fail. 
        //  
        // Access:      protected         
        //************************************
        typedef ds_profile_db_result_type (*get_fn_type)(
          ds_profile_db_ident_type       ident, 
          ds_profile_db_info_type*       info,
          const void*                    params);

        typedef bool (*compare_fn_type)(
          ds_profile_db_ident_type             ident, 
          const ds_profile_db_info_type*       info1,
          const ds_profile_db_info_type*       info2);
 
        struct ParamInfo {
          ParamInfo(
            unsigned int o = 0, size_t l = 0):
            offset(o), len(l){}

          unsigned int offset;
          size_t len;
        }; /* struct ParamInfo */

        ds_profile_db_result_type SetParam(
          ParamInfo* mapParamsInfo,
          void* params,
          ds_profile_db_ident_type   ident,
          const ds_profile_db_info_type *info,
          const ds_profile_db_gen_params_type* gen_params = NULL,
          boolean is_set = true);

        ds_profile_db_result_type GetParam (
          ParamInfo* mapParamsInfo,
          const void* params,
          ds_profile_db_ident_type   ident,
          ds_profile_db_info_type    *info) const;

        bool CompareParam(
          ParamInfo* mapParamsInfo,
          ds_profile_db_ident_type ident,
          const ds_profile_db_info_type *compare_info) const;

        ds_profile_db_result_type TrackParamChange(ds_profile_db_ident_type ident);

        static ds_profile_db_result_type ValidateCParams(
          validate_fn_type                     validation_fn,
          ds_profile_db_ident_type             start_range,
          ParamInfo*                           mapParamsInfo,
          unsigned int                         map_number_of_elements, 
          const void*                          params,
          const ds_profile_db_gen_params_type* gen_params = NULL);

        ds_profile_db_result_type GetFieldInfo (
          ParamInfo* mapParamsInfo,
          const void* params,
          ds_profile_db_ident_type   ident,
          unsigned int* offset, 
          uint16* len,
          void** buf) const;   

        virtual const void* GetCParamsPtr() const = 0;

        virtual ParamInfo* const
          GetMapParamsPtr() const = 0;

        virtual validate_fn_type GetValidateFn() const = 0;
        virtual set_fn_type      GetSpecialSetFn() const = 0;
        virtual get_fn_type      GetSpecialGetFn() const = 0;
        virtual compare_fn_type  GetCompareFn() const = 0;
        
        virtual size_t GetCStructSize() const = 0;

      private:
        
        Params(const Params&):changed_idents_list(NULL){};

        /* do not define, this is to prevent copying the object */
        Params& operator=(const Params&);

        virtual bool IsParamReadOnly(ds_profile_db_ident_type ident) const;                

        void ResetParamTracking();

        ds_profile_db_result_type GetParamInfo(
          ParamInfo* mapParamsInfo,
          ds_profile_db_ident_type   ident,
          ParamInfo** paramInfo) const;

        virtual int IdentToIndex(ds_profile_db_ident_type ident) const = 0;

        /* Profile needs to access Params private methods */
        friend class Profile;


    }; /* class Params */
  } /* namespace Profile */
} /* namespace ds */



#endif /* DS_PROFILE_PARAMS_H */
