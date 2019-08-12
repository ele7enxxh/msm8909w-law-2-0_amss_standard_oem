#ifndef DS_EPC_RT_ACL_H
#define DS_EPC_RT_ACL_H
/*===========================================================================

        E P C   S P E C I F I C   M O D E   H A N D L E R

        A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S
 
       F O R   R O U T I N G   V I A   T H E   1 x   I N T E R F  A C E

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_rt_acl.h_v   1.1   11 Sep 2002 12:17:04   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_rt_acl.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/28/09    sk     Created Module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "ps_acl.h"
#include "ps_aclrules.h"

/* <EJECT> */ 
/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/

ACL_DEF( ds_epc_rt_acl );

ACL_DEF( ds_epc_in_rt_acl );

ACL_POST_PROC_DEF( ds_epc_rt_acl_post_proc );

ACL_CFG_SUPPORTED_DEF2( ds_epc_rt_is_cfg_supported );

#endif /*FEATURE_EPC_HANDOFF*/
#endif    /* DS_EPC_RT_ACL_H */

