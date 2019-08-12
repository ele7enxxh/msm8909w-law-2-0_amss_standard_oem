#ifndef FTM_SV_DISPATCH_H
#define FTM_SV_DISPATCH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    F T M  C O M M O N  D I S P A T C H   

GENERAL DESCRIPTION
  This is the header file for the embedded FTM Commands that are SV module
  specific

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/sv/ftm/inc/ftm_sv_dispatch.h#1 $ $DateTime: 

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
04/04/11   kma     Compilation fix
06-28-10   kma     Created
===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FTM_HAS_SV_API

#include "ftmdiag.h"
#include "ftm.h"

typedef  union 
{
  uint16 error_code;
} ftm_sv_result_type;

/*===========================================================================

FUNCTION FTM_SV_DISPATCH

DESCRIPTION
   This function handles requests to run common tests and other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_sv_dispatch(ftm_sv_pkt_type *cmd_ptr);

#endif  /* FTM_HAS_SV_API */
#endif  // FEATURE_FACTORY_TESTMODE
#endif  // FTM_SV_DISPATCH_H
