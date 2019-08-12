/**
@file pd_mon_user.c
@brief This file contains the API details for the Protection Domain Monitor
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_mon_user.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "pd_mon_qurt.h"
#include "pd_mon_internal.h"

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

void pd_mon_user_term(void); // forward reference

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Implementation Details
/////////////////////////////////////////////////////////////////////

/**
Initialization of service prior to use
@return
None.
*/
void pd_mon_user_init(void)
{
   secure_memset(&pd_mon_internal_user, 0, sizeof(pd_mon_internal_user)); // Initialize Local Storage

   qurt_pimutex_init(&pd_mon_internal_user.mutex); // serialize access

   qurt_pimutex_lock(&pd_mon_internal_user.mutex); // serialize access

   pd_mon_internal_user.handle = pd_dump_init("PD_DUMP");

   qurt_pimutex_unlock(&pd_mon_internal_user.mutex); // serialize access

   // API Check

   if (PD_DUMP_STATUS_SUCCESS == pd_dump_method_0(pd_mon_internal_user.handle) &&
       PD_DUMP_STATUS_SUCCESS == pd_dump_method_1(pd_mon_internal_user.handle))
   {
      rcinit_register_termfn_group(RCINIT_GROUP_0, pd_mon_user_term);
   }
}

/**
Termination of service following use
@return
None.
*/
void pd_mon_user_term(void)
{
   qurt_pimutex_lock(&pd_mon_internal_user.mutex); // serialize access

   if (PD_DUMP_STATUS_SUCCESS == pd_dump_destroy(pd_mon_internal_user.handle))
   {
      // SUCCESS
   }

   qurt_pimutex_unlock(&pd_mon_internal_user.mutex); // serialize access

   qurt_pimutex_destroy(&pd_mon_internal_user.mutex); // serialize access
}
