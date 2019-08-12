/*===========================================================================
  @file ds_Profile_Platform.cpp

  This file implements abstract class for all platform dependent functionality 
  in ds profile db

  THERE SHOULD BE ONLY ONE FILE DERIVING CLASS WITH CONCRETE IMPLEMENTATION PER
  PLATFORM
  
                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-06-18 am Created module  

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Profile_Platform.h"
#include "ds_Profile_MemManager.h"


using namespace ds::Profile;
/*===========================================================================

                     GLOBAL DECLARATIONS

===========================================================================*/
/*===========================================================================

                     MEMBER FUNCTIONS

===========================================================================*/
Platform* Platform::Instance(void)
{
  return theInstance;
}

/* platform independent initializations */
ds_profile_db_result_type Platform::Init(void)
{
  if (0 != MemoryManager::MemPoolInit())
{
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  return DB_RESULT_SUCCESS;
}