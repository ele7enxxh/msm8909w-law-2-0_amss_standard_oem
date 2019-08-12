/*==========================================================================*/
/*!
  @file
  qmi_pbm_crit_sect_rex.c

  @brief
  This file provides REX specific critical section implementation.

  Copyright (c) 2011QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
  QUALCOMM Technologies Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm_crit_sect_rex.c#1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  03/02/2010  rt  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "qmi_pbm_crit_sect.h"

#include "modem_mem.h"
#include "amssassert.h"

#if 1
#include "rex.h"
#endif
/*===========================================================================

                          PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
#if 1
static rex_crit_sect_type qmi_crit_section;
/*---------------------------------------------------------------------------
  REX specific critical section implementation
---------------------------------------------------------------------------*/
void qmi_pbm_init_crit_section
(
  qmi_pbm_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  crit_sect_ptr->handle = (rex_crit_sect_type *)&qmi_crit_section;
  memset(crit_sect_ptr->handle,0,sizeof(qmi_crit_section));
  rex_init_crit_sect ((rex_crit_sect_type *)
                      crit_sect_ptr->handle);

} /* qmi_pbm_init_crit_section() */


void qmi_pbm_enter_crit_section
(
  qmi_pbm_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect ((rex_crit_sect_type *)
                       crit_sect_ptr->handle);

} /* qmi_pbm_enter_crit_section() */

void qmi_pbm_leave_crit_section
(
  qmi_pbm_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_leave_crit_sect ((rex_crit_sect_type *)
                       crit_sect_ptr->handle);

} /* qmi_pbm_leave_crit_section() */

void qmi_pbm_destroy_crit_section
(
  qmi_pbm_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  modem_mem_free (crit_sect_ptr->handle, MODEM_MEM_CLIENT_QMI_PBM_CRIT);


} /* qmi_pbm_destroy_crit_section() */

#endif /* if !defined(TEST_FRAMEWORK) || !defined(FEATURE_QUBE) */
