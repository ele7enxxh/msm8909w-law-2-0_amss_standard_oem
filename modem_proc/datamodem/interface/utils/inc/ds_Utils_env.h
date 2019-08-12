#ifndef AEEENV_H
#define AEEENV_H
/*==================================================================
        Copyright 2007-2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary and Confidential
====================================================================

DESCRIPTION:  CreateInstance without a provided IEnv

==================================================================*/

#include "ds_Utils_IEnv.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

int env_GetCurrent(IEnv** ppiEnv);
int env_CreateInstance(AEECLSID cls, void** ppo);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


/*=====================================================================
INTERFACE DOCUMENTATION
=======================================================================

env Interface

  Allows the caller to create objects even if they cannot provide an
  IEnv.

=======================================================================

env_GetCurrent()

Description:
  Obtains the current environment.

Prototype:
   int env_GetCurrent(IEnv** ppiEnv)

Parameters:
   ppIEnv[out]: Current IEnv

Return value:
    AEE_SUCCESS if current IEnv obtained, otherwise a failure-specific
     error value.

Comments:  
   None

See Also:
   IEnv Interface
   
=======================================================================

env_CreateInstance()

Description:
  Instantiates an instance of the class specified, if possible.

Prototype:
   int env_CreateInstance(AEECLSID cls, void** ppo)

Parameters:
   cls :      The AEECLSID of the class to instantiate
   ppo[out]:  pointer to be filled with new instance of the class
              requested

Return value:
    AEE_SUCCESS if instance of class created, otherwise a failure-specific
     error value.

Comments:  
   None

See Also:
   IEnv Interface, IEnv_CreateInstance()

=======================================================================*/

#endif /* #ifndef AEEENV_H */

