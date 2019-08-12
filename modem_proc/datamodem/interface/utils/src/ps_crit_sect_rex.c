/*==========================================================================*/
/*!
  @file 
  ps_crit_sect.c

  @brief
  This file provides REX specific critical section implementation.

  Copyright (c) 2009 - 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_crit_sect_rex.c#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  10/11/11   su  To fix KW issues 
  12/24/10   ua  Fixed compilation errors. 
  12/03/09   hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "ps_crit_sect.h"
#include "ps_system_heap.h"
#include "amssassert.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                          PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
#if !defined(TEST_FRAMEWORK) || !defined(FEATURE_QUBE)
#include "rex.h"
/*---------------------------------------------------------------------------
  REX specific critical section implementation
---------------------------------------------------------------------------*/
void ps_init_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == crit_sect_ptr)
  {
    DATA_ERR_FATAL("ps_init_crit_section(): critical section pointer is NULL");
    return;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(crit_sect_ptr->handle,
                                  sizeof (rex_crit_sect_type),
                                  rex_crit_sect_type*);

  if (NULL == crit_sect_ptr->handle )
  {
    return;
  }


  (void) memset (crit_sect_ptr->handle, 0, sizeof (rex_crit_sect_type));

  rex_init_crit_sect ((rex_crit_sect_type *) crit_sect_ptr->handle);


  
} /* ps_init_crit_section() */


void ps_enter_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == crit_sect_ptr || NULL == crit_sect_ptr->handle)
  {
    DATA_ERR_FATAL("ps_enter_crit_section(): critical section or handle is NULL");
    return;
  }

  rex_enter_crit_sect ((rex_crit_sect_type *) crit_sect_ptr->handle);

} /* ps_enter_crit_section() */

void ps_leave_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == crit_sect_ptr || NULL == crit_sect_ptr->handle)
  {
    DATA_ERR_FATAL("ps_leave_crit_section(): critical section or handle is NULL");
    return;
  }

  rex_leave_crit_sect ((rex_crit_sect_type *) crit_sect_ptr->handle);

} /* ps_leave_crit_section() */

void ps_destroy_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == crit_sect_ptr || NULL == crit_sect_ptr->handle)
  {
    DATA_ERR_FATAL("ps_destroy_crit_section(): critical section or handle is NULL");
    return;
  }

  (void) rex_del_crit_sect ((rex_crit_sect_type *) crit_sect_ptr->handle);

  PS_SYSTEM_HEAP_MEM_FREE (crit_sect_ptr->handle);
  
} /* ps_destroy_crit_section() */

#endif /* if !defined(TEST_FRAMEWORK) || !defined(FEATURE_QUBE) */

