/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This file contains delcarations for ds_1x_profile,  a subset of 707
  AT cmds that cannot be modified during the call.

EXTERNALIZED FUNCTIONS

  None
Copyright (c) 2005 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds_1x_profile.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/13   sc      Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
06/03/13   sd      ATCoP decoupling changes
05/22/12   ac      trial for PW
07/22/11   dvk     Global Variable Cleanup
02/11/11   ack    Global Variable Cleanup
02/27/09   ms      Offtarget Lint fixes.
08/25/05   kvd     Initial version of file.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "ds_1x_profile.h"
#include "dsat707mip.h"
#include "ds707_pkt_mgr.h"

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/
static ds707_mip_profile_type ds_1x_runtime_profile;

/*===========================================================================
                   FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION DS_1X_PROFILE_INIT

  DESCRIPTION
       Initializes static & runtime profile structs for 1X profile.

  DEPENDENCIES
  None

  RETURN VALUE
  None

  SIDE EFFECTS
    called from dstask init should be called before at_init.
===========================================================================*/
void ds_1x_profile_init
(
  void
)
{
  memset(&ds_1x_runtime_profile,0,sizeof(ds707_mip_profile_type));
} /* ds_1x_init_profile */

/*===========================================================================
  FUNCTION DSAT707_1X_PROFILE_STORE_COPY

  DESCRIPTION
        Copies an instance of static profiel into runtime profile, based
        on current active profile.

  DEPENDENCIES
  None

  RETURN VALUE
  None

  SIDE EFFECTS
  None
===========================================================================*/
void ds_1x_profile_store
(
  void
)
{
  ds707_mip_profile_type  *ds_static_profile_ptr = NULL;
/*------------------------------------------------------------------------*/

  ds_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
  if( ds_static_profile_ptr == NULL )
  {
    DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
  }

  memscpy(&ds_1x_runtime_profile, sizeof(ds707_mip_profile_type), 
          ds_static_profile_ptr, sizeof(ds707_mip_profile_type));
} /* ds_1x_profile_store */

/*===========================================================================
  FUNCTION DS_GET_1X_RUNTIME_PROFILE_PTR

  DESCRIPTION
	Accessor function to retrieve ptr to ds_1x_runtime_profile

  DEPENDENCIES
  None

  RETURN VALUE
  Returns address of ds_1x_runtime_profile

  SIDE EFFECTS
  None
===========================================================================*/
ds707_mip_profile_type* ds_get_1x_runtime_profile_ptr
(
  void
)
{
  return &ds_1x_runtime_profile;		
} /* ds_get_1x_runtime_profile_ptr */

/*=========================================================================*/
#endif /* FEATURE_DATA_IS707 */

