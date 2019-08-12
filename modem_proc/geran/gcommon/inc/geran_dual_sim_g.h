/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            GERAN DUAL SIM HEADER

GENERAL DESCRIPTION
  Provides common definitions for use by GERAN entities supporting Dual SIM

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2014 QUALCOMM Technologies, Incorporated. All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/geran_dual_sim_g.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
28/11/12    SJW     Refactored for QDSP6 DSDX porting
dd/mm/yy    SJW     Sample text for edit history
===========================================================================*/
#ifndef __GERAN_DUAL_SIM_H__
#define __GERAN_DUAL_SIM_H__

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif
#include "geran_multi_sim.h"

extern gas_id_t check_gas_id(const gas_id_t gas_id);

#endif /* __GERAN_DUAL_SIM_H__ */
