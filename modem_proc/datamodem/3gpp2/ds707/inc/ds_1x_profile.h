#ifndef DS_1X_PROFILE_H
#define DS_1X_PROFILE_H
/*===========================================================================

                        D A T A   S E R V I C E S

               1X, I S - 7 0 7  C A L L  P R O F I LE 

                         H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  extended commands specific to IS-707 mode.

Copyright (c) 1995-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/13   sd      ATCoP decoupling changes
01/02/12   msh     Coan: Feature cleanup
07/22/11   dvk     Global Variable Cleanup
02/11/11   ack     Global Variable Cleanup
09/04/09   ss      CMI SU level modifications.
08/28/05   kvd     Initial version of file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

#include "dsati.h"
#include "dsat707mip.h"
#include "dsat707mipctab.h"
#include "msg.h"
#include "ds707_pkt_mgr.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip_v.h"
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================

                      PUBLIC TYPE DECLARATIONS

===========================================================================*/

#define DS_1X_INVALID_ACTIVE_PROFILE_ID   0xFF

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
  FUNCTION DS_1X_PROFILE_INIT

  DESCRIPTION

  DEPENDENCIES
  None

  RETURN VALUE
  None

  SIDE EFFECTS
  None
===========================================================================*/
void ds_1x_profile_init
(
  void
);

/*===========================================================================
  FUNCTION DSAT707_1X_PROFILE_STORE_COPY

  DESCRIPTION

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
);

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
);

#endif /* FEATURE_DATA_IS707     */
#endif /* DS_1X_PROFILE_H */
