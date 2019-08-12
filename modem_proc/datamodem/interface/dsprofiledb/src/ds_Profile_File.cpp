/*===========================================================================
  @file ds_Profile_File.cpp

  This file implements some basic functionality of File abstract class which is 
  wrapper for various file operations with the profile

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
#include "ds_Profile_File.h"
#include "ds_Profile_FileEFS.h"
#include <stringl/stringl.h>

extern "C"
{
#include "fs_public.h"
#include "fs_sys_types.h"
}

using namespace ds::Profile;
/*===========================================================================

                      INTERNAL DEFINITIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTIONS

==========================================================================*/
const void* File::GetProfileCParamsPtr(const Profile* profile, 
                                        ds_profile_db_tech_type tech) const
{
  return profile->GetCParamsPtr(tech);
}