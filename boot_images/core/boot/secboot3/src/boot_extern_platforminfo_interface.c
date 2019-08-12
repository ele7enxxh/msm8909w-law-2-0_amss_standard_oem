/*===========================================================================

                    BOOT EXTERN PLATFORMINFO DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external platform info drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_platforminfo_interface.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/13   dh      Add boot_DalPlatformInfo_CDTConfigPreDDR and boot_DalPlatformInfo_CDTConfigPostDDR
10/18/11   dh      Initial Creation.
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "DDIPlatformInfo.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_DalPlatformInfo_CDTConfigPreDDR

** ==========================================================================
*/
/**
  Configures platform information from memory.

  This function configures the PlatformInfo DAL with the platform information
  located at the address contained in the parameter pMem.

  @param[in] _h   Handle to the PlatformInfo DAL device.
  @param[in] pMem Address to the location in memory that contains 
                  platform information.

  @return
  DAL_SUCCESS -- PlatformInfo DAL was configured successfully. \n
  DAL_ERROR   -- Invalid parameter.

  @dependencies
  The parameter pMem must point to a location in memory that is formatted
  according to the Platform ID block of the EEPROM software Configuration
  Data Table (CDT).

*/
DALResult boot_DalPlatformInfo_CDTConfigPreDDR(DalDeviceHandle * _h,  void * pMem)
{
  return DalPlatformInfo_CDTConfigPreDDR(_h, pMem);
}


/*===========================================================================

**  Function :  boot_DalPlatformInfo_CDTConfigPostDDR

** ==========================================================================
*/
/**
  Configures platform information from memory.

  This function configures the PlatformInfo DAL with the platform information
  located at the address contained in the parameter pMem.

  @param[in] _h   Handle to the PlatformInfo DAL device.
  @param[in] pMem Address to the location in memory that contains 
                  platform information.

  @return
  DAL_SUCCESS -- PlatformInfo DAL was configured successfully. \n
  DAL_ERROR   -- Invalid parameter.

  @dependencies
  The parameter pMem must point to a location in memory that is formatted
  according to the Platform ID block of the EEPROM software Configuration
  Data Table (CDT).

*/
DALResult boot_DalPlatformInfo_CDTConfigPostDDR(DalDeviceHandle * _h,  void * pMem)
{
  return DalPlatformInfo_CDTConfigPostDDR(_h, pMem);
}



