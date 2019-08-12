#ifndef SMEM_EXT_H 
#define SMEM_EXT_H

/*===========================================================================

                S H A R E D   M E M O R Y   E X T E R N A L

DESCRIPTION
    Declares the shared memory external (non-library) apis.

Copyright (c) 2009, 2011 by Qualcomm Technologies, Incorporated.  All 
Rights Reserved.

===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/mproc/smem/src/smem_ext.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/12   bm      Remove smem_os.h inclusion as it is not required for this
                   header file.
04/07/11   tl      Include smem_os.h, rather than comdef.h, since not every OS
                   has comdef.h
04/08/09   nnv     Initial version.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

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
extern void *smem_ext_get_smem_base( void );

/*===========================================================================
FUNCTION      smem_ext_get_smem_size
                                                                             
DESCRIPTION   Returns the shared memory size

ARGUMENTS     None
                  
DEPENDENCIES  None

RETURN VALUE  shared memory size
  
SIDE EFFECTS  None
===========================================================================*/
extern uint32 smem_ext_get_smem_size( void );

#endif /* SMEM_EXT_H */
