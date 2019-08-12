#ifndef GHDI_NVMEM_V_H
#define GHDI_NVMEM_V_H
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

#include "mmcp_variation.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"


#include "rex.h"
#include "nv.h"

#include "ghdi_nvmem.h"

typedef rex_tcb_type ghdi_task_T;


/*---------------------------------------------------------------------------------
** struct defines the information managed by NVMEM module for each registered task
**-------------------------------------------------------------------------------*/
typedef struct
{
   /* pointer to the task or NULL if its an empty entry */
   ghdi_task_T                        *taskPtr;
   
   /* signal that should be used to notify task when NV operation is completed */
   ghdi_nvmem_signal_T                 signal;
   
   /* pointer to task specific wait function for this task (can be NULL) */
   ghdi_nvmem_wait_fn_ptr_T            taskWaitFnPtr;

   /* pointer to task specific default value generation function (can be NULL) */
   ghdi_nvmem_default_value_fn_ptr_T   taskDefaultGenFnPtr;
} ghdi_nvmem_registered_task_info_T;


/* see header */
extern void ghdi_nvmem_initialize(void);

#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
/* see definition */
extern ghdi_nvmem_status_T ghdi_nvmem_read_imp_per_subs
(
   nv_items_enum_type                  parameter_id,
   nv_item_type                        *data_ptr,
   ghdi_nvmem_signal_T                 signal,
   ghdi_nvmem_wait_fn_ptr_T            waitfn_ptr,
   ghdi_nvmem_default_value_fn_ptr_T   defaultfn_ptr,
   uint16                              context_id
);

/* see definition */
extern ghdi_nvmem_status_T ghdi_nvmem_write_imp_per_subs
(
   nv_items_enum_type                  parameter_id,
   nv_item_type                        *data_p,
   ghdi_nvmem_signal_T                 signal,
   ghdi_nvmem_wait_fn_ptr_T            waitfn_p,
   uint16                              context_id
);
#endif

/* see definition */
extern ghdi_nvmem_status_T ghdi_nvmem_read_imp
(
   nv_items_enum_type                  parameter_id,
   nv_item_type                        *data_ptr,
   ghdi_nvmem_signal_T                 signal,
   ghdi_nvmem_wait_fn_ptr_T            waitfn_ptr,
   ghdi_nvmem_default_value_fn_ptr_T   defaultfn_ptr
);

/* see definition */
extern ghdi_nvmem_status_T ghdi_nvmem_write_imp
(
   nv_items_enum_type                  parameter_id,
   nv_item_type                        *data_p,
   ghdi_nvmem_signal_T                 signal,
   ghdi_nvmem_wait_fn_ptr_T            waitfn_p
);

/* see definition */
extern ghdi_nvmem_status_T ghdi_nvmem_replace_imp
(
   nv_items_enum_type                  parameter_id,
   nv_item_type                        *data_p,
   ghdi_nvmem_signal_T                 signal,
   ghdi_nvmem_wait_fn_ptr_T            waitfn_p
);

/* see definition */
extern ghdi_nvmem_status_T ghdi_nvmem_free_imp
(
   nv_items_enum_type                  parameter_id,
   nv_item_type                        *data_p,
   ghdi_nvmem_signal_T                 signal,
   ghdi_nvmem_wait_fn_ptr_T            waitfn_p
);

#endif /* GHDI_NVMEM_H */
