#ifndef _DS_3GPP_RMSM_SIO_RT_ACL_H
#define _DS_3GPP_RMSM_SIO_RT_ACL_H
/*===========================================================================

   C D M A   2 0 0 0   ( 1 x )   S P E C I F I C   M O D E   H A N D L E R

        A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S
 
       F O R   R O U T I N G   V I A   T H E   1 x   I N T E R F  A C E

GENERAL DESCRIPTION
  The 1x specific routing ACL is used to enable policy and address
  based routing across the 1x interface.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_rt_acl.h_v   1.1   11 Sep 2002 12:17:04   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/src/ds_3gpp_rmsm_sio_rt_acl.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/05    ks     Removed featurization
03/08/05   ks      Changed name from ds_netsio_rt_acl.h
12/07/04   usb/jd  Created Module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ps_acl.h"
#include "ps_aclrules.h"


/* <EJECT> */ 
/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/

ACL_DEF( ds_3gpp_rmsm_sio_rt_acl );

ACL_POST_PROC_DEF( ds_3gpp_rmsm_sio_rt_acl_post_proc );

#endif    /* DS_3GPP_RMSM_SIO_RT_ACL_H */


