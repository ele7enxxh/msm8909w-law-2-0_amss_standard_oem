/*===========================================================================

              S Y S T E M   G W   M O D U L E  

DESCRIPTION
  This header file contains functions  that are shared between Call Manager,
  Call Manager clients and the GSM/WCDMA protocol stacks.

Copyright (c) 2002 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
02/20/09   rm      Lint clean post CMI changes
07/01/08   rm      Mainlining of MBMS changes
03/30/07   rp     Initial revision
===========================================================================*/


#include "mmcp_variation.h"
#include "sys.h"
#include "sys_v.h"
/*lint -esym(766,sys_gw.h)*/
#include "sys_gw.h"
/*lint -esym(766,sys_gw_v.h)*/
#include "sys_gw_v.h"
#include "string.h"
#include "cmidbg.h" 
#include "cmi.h"

#ifdef CM_GW_SUPPORTED
#if defined (FEATURE_MBMS)
#error code not present
#endif /*#if defined (FEATURE_MBMS)*/
#endif /* #ifdef CM_GW_SUPPORTED*/
