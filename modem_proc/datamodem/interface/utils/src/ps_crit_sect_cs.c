/*==========================================================================*/
/*!
  @file
  ps_crit_sect.c

  @brief
  This file provides REX specific critical section implementation.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_crit_sect_cs.c#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2009-12-03 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "amssassert.h"
#include "ps_crit_sect.h"

#include "ds_Utils_ICritSect.h"
#include "ds_Utils_CCritSect.h"

#include "ds_Utils_IEnv.h"
#include "ds_Utils_env.h"
#include "ds_Utils_StdErr.h"

/*===========================================================================

                          PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
void ps_init_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
)
{
  IEnv* piEnv = 0;
  int nErr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nErr = env_GetCurrent(&piEnv);
  ASSERT(AEE_SUCCESS == nErr);

  nErr = IEnv_CreateInstance(piEnv, AEECLSID_CCritSect,
                             (void**)&crit_sect_ptr->handle);
  ASSERT(AEE_SUCCESS == nErr);

} /* ps_init_crit_section() */

void ps_enter_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ICritSect_Enter((ICritSect*)crit_sect_ptr->handle);

} /* ps_enter_crit_section() */

void ps_leave_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ICritSect_Leave(crit_sect_ptr->handle);

} /* ps_leave_crit_section() */

void ps_destroy_crit_section
(
  ps_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  IQI_RELEASEIF(crit_sect_ptr->handle);

} /* ps_destroy_crit_section() */


