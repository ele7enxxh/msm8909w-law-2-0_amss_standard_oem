/*==========================================================================*/
/*!
  @file 
  qmi_crit_sect_rex.c

  @brief
  This file provides REX specific critical section implementation.

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/qmi_crit_sect_rex.c#1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  02/17/12    sb  Fixed Compiler and KW Warnings.
  03/02/2010  rt  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#include "qmi_crit_sect.h"

#include "modem_mem.h"
#include "amssassert.h"
#include "ds_Utils_DebugMsg.h"

#if !defined(TEST_FRAMEWORK) || !defined(FEATURE_QUBE)
#include "rex.h"
#endif
/*===========================================================================

                          PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
#if !defined(TEST_FRAMEWORK) || !defined(FEATURE_QUBE)
/*---------------------------------------------------------------------------
  REX specific critical section implementation
---------------------------------------------------------------------------*/
void qmi_init_crit_section
(
  qmi_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  crit_sect_ptr->handle = 
    (rex_crit_sect_type *) 
    modem_mem_alloc(sizeof(rex_crit_sect_type), MODEM_MEM_CLIENT_QMI_CRIT);

  if (NULL == crit_sect_ptr->handle )
  {
    DATA_ERR_FATAL("qmi_init_crit_section(): Unable to allocate memory");
    return;
  }
  
  (void) memset (crit_sect_ptr->handle, 0, sizeof (rex_crit_sect_type));
  
  rex_init_crit_sect ((rex_crit_sect_type *) crit_sect_ptr->handle);
					  
} /* qmi_init_crit_section() */


void qmi_enter_crit_section
(
  qmi_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect ((rex_crit_sect_type *) 
                       crit_sect_ptr->handle);

} /* qmi_enter_crit_section() */

void qmi_leave_crit_section
(
  qmi_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_leave_crit_sect ((rex_crit_sect_type *) 
                       crit_sect_ptr->handle);

} /* qmi_leave_crit_section() */

void qmi_destroy_crit_section
(
  qmi_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  modem_mem_free (crit_sect_ptr->handle, MODEM_MEM_CLIENT_QMI_CRIT);

  
} /* qmi_destroy_crit_section() */

#endif /* if !defined(TEST_FRAMEWORK) || !defined(FEATURE_QUBE) */
