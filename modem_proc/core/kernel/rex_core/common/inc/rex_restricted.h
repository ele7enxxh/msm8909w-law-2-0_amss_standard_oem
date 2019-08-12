#ifndef __REX_RESTRICTED_H__
#define __REX_RESTRICTED_H__
/*==========================================================================

                      REX RESTRICTED HEADER FILE

DESCRIPTION   
   This header file is to be included and used by privileged 
   REX clients ex. RTE, Watchdog services etc. This is not a
   general purpose header file. Standard REX clients SHOULD NOT 
   include this header file in their source code or use any 
   datatype or API in this header.

Copyright (c) 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*==========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/kernel/rex_core/common/inc/rex_restricted.h#1 $

===========================================================================*/

#include "rex.h"
/* 
** Include any of the OS restricted APIs. 
** Clients of rex_restricted.h should not include rex_os_restricted.h 
** and always use rex_restricted.h
*/
#include "rex_os_restricted.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*================================================================================================
      OPTIONAL REX RESTRICTED APIs
================================================================================================*/
/**
API used to kill all active REX tasks. Typically called during a reset sequence an example
of which is modem restart.

@return
None.
*/
void rex_kill_all(void);

int rex_get_ipc_info_ref(rex_tcb_type *p_tcb, rex_ipc_info_type **pp_node);
int rex_get_dog_report_val_self(void);
void rex_set_dog_report_val_self(int dog_report_val);
void rex_set_dog_report_val(rex_tcb_type *p_tcb, int dog_report_val);
int rex_get_dog_report_val(rex_tcb_type *p_tcb);

#ifdef __cplusplus
}
#endif

#endif //__REX_RESTRICTED_H__
