/*==========================================================================*/
/*!
  @file
  qmi_wms_crit_sect_rex.c

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/qmi_wms/src/qmi_wms_crit_sect_rex.c#1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  03/02/2010  rt  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "qmi_wms_crit_sect.h"
#include "modem_mem.h"
#include "amssassert.h"
#include "rex.h"
/*===========================================================================

                          PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  REX specific critical section implementation
---------------------------------------------------------------------------*/
void qmi_wms_init_crit_section
(
  qmi_wms_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == crit_sect_ptr)
  {
    return;
  }

  crit_sect_ptr->handle =
    (rex_crit_sect_type *)
    modem_mem_alloc(sizeof(rex_crit_sect_type), MODEM_MEM_CLIENT_QMI_WMS_CRIT);

  rex_init_crit_sect ((rex_crit_sect_type *)
                      crit_sect_ptr->handle);

} /* qmi_wms_init_crit_section() */


void qmi_wms_enter_crit_section
(
  qmi_wms_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == crit_sect_ptr)
  {
    return;
  }

  rex_enter_crit_sect ((rex_crit_sect_type *)
                       crit_sect_ptr->handle);

} /* qmi_wms_enter_crit_section() */

void qmi_wms_leave_crit_section
(
  qmi_wms_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == crit_sect_ptr)
  {
    return;
  }

  rex_leave_crit_sect ((rex_crit_sect_type *)
                       crit_sect_ptr->handle);

} /* qmi_wms_leave_crit_section() */

void qmi_wms_destroy_crit_section
(
  qmi_wms_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == crit_sect_ptr)
  {
    return;
  }

  modem_mem_free (crit_sect_ptr->handle, MODEM_MEM_CLIENT_QMI_WMS_CRIT);


} /* qmi_wms_destroy_crit_section() */
