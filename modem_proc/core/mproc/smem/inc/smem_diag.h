#ifndef SMEM_DIAG_H
#define SMEM_DIAG_H

/*===========================================================================

                    Shared Memory Diag Header File

    Define the APIs exported for DIAG.


 Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/smem/inc/smem_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/28/06   ptm     Initial version - derived from smem.h#7.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smem_diag_get_message
                                                                             
DESCRIPTION   Copy a diag message from shared memory.

ARGUMENTS     char * message - pointer to destination string
              uint32 len - length of buffer pointed to by message
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  None
===========================================================================*/
void smem_diag_get_message( char *message, uint32 len );

/*===========================================================================
FUNCTION      smem_diag_set_message
                                                                             
DESCRIPTION   Copy a diag message into shared memory.

ARGUMENTS     char * message - pointer to string to copy to shared memory.
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  Writes message in shared memory and does memory barrier so the
              the message is guaranteed to be in shared memory on return.
===========================================================================*/
void smem_diag_set_message( char *message );

#endif /* SMEM_DIAG_H */
