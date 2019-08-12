#ifndef __GERAN_DUAL_SIM_H
#define __GERAN_DUAL_SIM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            GERAN DUAL SIM HEADER

GENERAL DESCRIPTION
  Provides common definitions for use by GERAN entities supporting Dual SIM

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/geran_dual_sim.h#1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
26/11/13    ssh     CR581870: Force idle G sub to wake-up (enter non-drx) when W/G enters BG traffic
28/11/12    SJW     Refactored for QDSP6 DSDX porting
dd/mm/yy    SJW     Sample text for edit history
===========================================================================*/
 
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif
 
/* global "GERAN Access Stratum ID" type */
typedef enum
{
  GERAN_ACCESS_STRATUM_ID_1         = 0x00,
  GERAN_ACCESS_STRATUM_ID_2         = 0x01,
  GERAN_ACCESS_STRATUM_ID_3         = 0x02,
  GERAN_ACCESS_STRATUM_ID_UNDEFINED = 0xFF
} gas_id_t;

extern void gl1_drx_force_rude_wake_for_ta(void);

#endif /* __GERAN_DUAL_SIM_H */
