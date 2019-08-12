#ifndef GHDI_NVMEM_H
#define GHDI_NVMEM_H
/*===========================================================================

                        GHDI_NVMEM_H

Contains necessary includes and definitions for NVMEM functions and types
that require access to NV implementation and cannot be kept generic
                        
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/01   sbk     New file to support NV mem access added
*============================================================================= */

/*------------------------------------------------------------------------------
** the NV interface functions have been modified to follow the example of the UI NV
** interface functions as closely as possible
**------------------------------------------------------------------------------*/
#include "ghdi_exp.h"
#include "rex.h"
#include "nv.h"


typedef rex_sigs_type ghdi_nvmem_signal_T;
typedef nv_stat_enum_type ghdi_nvmem_status_T;


/*--------------------------------------------------------------------------------
** function pointer signature for task wait functions to wait for NVMEM signal
*-------------------------------------------------------------------------------*/
typedef ghdi_nvmem_signal_T (*ghdi_nvmem_wait_fn_ptr_T)(ghdi_nvmem_signal_T);

/*--------------------------------------------------------------------------------------
**  function pointer signature for task functions to provide default values in case the
**  requested item is not available or the read operation fails
*-------------------------------------------------------------------------------------*/
typedef ghdi_nvmem_status_T (*ghdi_nvmem_default_value_fn_ptr_T)(ghdi_nvmem_status_T, nv_items_enum_type, nv_item_type *);

/* see header */
extern ghdi_status_T ghdi_nvmem_register_task
(
   /* signal that should be used to notify task when NV operation is completed */
   ghdi_nvmem_signal_T                 signal,
   
   /* pointer to task specific wait function for this task (can be NULL) */
   ghdi_nvmem_wait_fn_ptr_T            taskWaitFnPtr,

   /* pointer to task specific default value generation function (can be NULL) */
   ghdi_nvmem_default_value_fn_ptr_T   taskDefaultGenFnPtr
);

#endif /* GHDI_NVMEM_H */
