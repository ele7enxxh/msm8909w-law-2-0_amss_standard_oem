#ifndef DS707_RT_ACL_H
#define DS707_RT_ACL_H
/*===========================================================================

   C D M A   2 0 0 0   ( 1 x )   S P E C I F I C   M O D E   H A N D L E R

        A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S
 
       F O R   R O U T I N G   V I A   T H E   1 x   I N T E R F  A C E

GENERAL DESCRIPTION
  The 1x specific routing ACL is used to enable policy and address
  based routing across the 1x interface.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_rt_acl.h_v   1.1   11 Sep 2002 12:17:04   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_rt_acl.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/08    psng   Updated to support new DENY_WITH_ANY_REASON macro.
08/09/04    mct    Added logical iface support.
07/20/03    usb    Added new function declarations used by ACL.
08/30/02    om     Created Module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

#include "ps_acl.h"
#include "sys.h"

/* <EJECT> */ 
/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/

ACL_DEF( ds707_rt_acl );

ACL_DEF( ds707_any_acl );

ACL_POST_PROC_DEF( ds707_rt_acl_post_proc );

ACL_IS_AVAILABLE_DEF2( ds707_rt_is_iface_available );

ACL_CFG_SUPPORTED_DEF2( ds707_rt_is_cfg_supported );

ACL_CFG_SHAREABLE_DEF( ds707_rt_is_cfg_shareable );

ACL_IS_AVAILABLE_DEF2( ds707_rt_acl_use_selected_iface );

ACL_CFG_SUPPORTED_DEF2(ds707_rt_is_cfg_supported_and_iface_available);

ACL_IS_AVAILABLE_DEF2 ( ds707_rt_is_pref_sys );

/*===========================================================================
FUNCTION DS707_IS_CLAT_SUPPORTED

DESCRIPTION
  This function returns the CLAT_SUPPORTED field for the passed profile or
  resolved profile passed in Policy

PARAMETERS
  arg_val_ptr - IOCTL payload pointer
  subs_id     - subscription id
  ps_error    - incase any error occured, details of it

DEPENDENCIES
  None.

RETURN VALUE
  0  - Success
  -1 - Failure
       DS_EFAULT   if NULL pointers passed.
       DS_ENETDOWN if PDN contexts are not valid.

SIDE EFFECTS
  None.
===========================================================================*/
int ds707_rt_is_clat_supported
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
);

#endif    /* FEATURE_DATA_IS707 */
#endif    /* DS707_RT_ACL_H */

