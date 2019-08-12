#ifndef DS_PROFILE_PARAMS_TECH_H
#define DS_PROFILE_PARAMS_TECH_H
/*===========================================================================
  @file ds_Profile_Params.h

  This file defines abstract ParamsTech class which provides interface for all
  Technology Param classes
  

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
#include "ds_profile_db.h"
#include "ds_Profile_ParamsGeneral.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
  namespace Profile
  {
    class ParamsTech : public Params
    {
      public:

        virtual ds_profile_db_result_type SetParam(
          ds_profile_db_ident_type   ident,
          const ds_profile_db_info_type *info,
          boolean is_set);

        virtual ~ParamsTech() = 0;

      protected:

        ParamsTech(const ParamsGeneral* paramsGen):paramsGeneral(paramsGen){};

        static const ds_profile_db_gen_params_type* GetGeneralCParamsPtr(const ParamsGeneral* paramsGeneral);
   
        const ParamsGeneral* paramsGeneral;

      private:    
        
        friend class Profile;

    }; /* class ParamsTech */
  } /* namespace Profile */
} /* namespace ds */

inline ds::Profile::ParamsTech::~ParamsTech(){}

#endif /* DS_PROFILE_PARAMS_TECH_H */
