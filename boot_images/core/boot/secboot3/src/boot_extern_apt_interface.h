#ifndef BOOT_EXTERN_APT_INTERFACE_H
#define BOOT_EXTERN_APT_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN APT DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external APT drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/13   jihok   Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

  FUNCTION boot_apt_test

  DESCRIPTION
        This function performs initialization for APT configuration

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
#ifdef FEATURE_BOOT_EXTERN_APT_COMPLETED
  void boot_apt_test(void);
#else
  static inline void boot_apt_test(void)
  {
  }
#endif

#endif /* BOOT_EXTERN_APT_INTERFACE_H */
