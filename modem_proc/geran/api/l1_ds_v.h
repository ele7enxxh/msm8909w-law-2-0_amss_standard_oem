#ifndef L1_DS_V_H
#define L1_DS_V_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        L1   D A T A  S E R V I C E S   P R O C E S S I N G
                       H E A D E R   F I L E

DESCRIPTION
   This module contains definitions for the interface between L1 and
   Data Services.


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/l1_ds_v.h#1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
17/11/13   ws       CR582513 GERAN API Unification for modem data services
11-08-24   tjw      Created anew as it was mistakenly thought to be an internal file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "sys.h"

#ifndef nas_exp_h
  /* Required for channel_mode_T */
  #include "nas_exp.h"
#endif

/*===========================================================================

FUNCTION  gl1_get_current_ded_chan_type

DESCRIPTION
  Determines what the current dedicated channel type is.

DEPENDENCIES
===========================================================================*/
#ifdef FEATURE_DUAL_SIM
channel_mode_T gl1_get_current_ded_chan_type( sys_modem_as_id_e_type as_id );
#else
channel_mode_T gl1_get_current_ded_chan_type(void);
#endif /*FEATURE_DUAL_SIM*/

#endif /* L1_DS_V_H */



