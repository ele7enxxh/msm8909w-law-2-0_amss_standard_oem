/*===========================================================================

                S H A R E D   M E M O R Y   E X T E R N A L

DESCRIPTION
    Defines the shared memory external (non-library) apis.

Copyright (c) 2009, 2012 by Qualcomm Technologies, Incorporated.  All 
Rights Reserved.

===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/mproc/smem/src/smem_ext.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/04/12   bt      Add OS abstraction for retrieval of SMEM address and size.
04/08/09   nnv     Initial version.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "comdef.h" 
#include "smem_os_common.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS
                      
===========================================================================*/

/*===========================================================================
FUNCTION      smem_ext_get_smem_base
                                                                             
DESCRIPTION   Returns the shared memory base physical address

ARGUMENTS     None
                  
DEPENDENCIES  None

RETURN VALUE  shared memory base physical address
  
SIDE EFFECTS  None
===========================================================================*/
void *smem_ext_get_smem_base( void )
{
  return smem_os_ext_get_smem_base();

} /* smem_ext_get_smem_base */

/*===========================================================================
FUNCTION      smem_ext_get_smem_size
                                                                             
DESCRIPTION   Returns the shared memory size

ARGUMENTS     None
                  
DEPENDENCIES  None

RETURN VALUE  shared memory size
  
SIDE EFFECTS  None
===========================================================================*/
uint32 smem_ext_get_smem_size( void )
{
  return smem_os_ext_get_smem_size();

} /* smem_ext_get_smem_size */

