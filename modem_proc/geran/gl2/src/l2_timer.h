/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          L 2   T I M E R

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_timer.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   ---       ---------------------------------------------------------
14/09/12   sjw       Created
dd/mm/yy   xxx       Format
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "environ.h"
#include "l2i.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/
/* Timer IDs - must be less than 0x80 due to NAS limit MAXNO_TIMERS */
#define SAPI0_T_ACK    0x10 /* (0x11 for GAS ID 0x01) */
#define SAPI3_T_ACK    0x13 /* (0x14 for GAS ID 0x01) */
#define SAPI0_T_200    0x20 /* (0x21 for GAS ID 0x01) */
#define SAPI3_T_200    0x23 /* (0x24 for GAS ID 0x01) */

#define IS_TACK_TIMER_ID(tid) (((tid) / 0x10) == 1)
#define IS_T200_TIMER_ID(tid) (((tid) / 0x20) == 1)

#define BAD_GL2_TIMER_INDEX 0xFF

extern void        l2_timer_init(void);
extern byte        l2_timer_set_params(l2_store_T *, SAPI_T, l2_channel_type_T, byte);
extern l2_store_T *l2_timer_get_params(byte, SAPI_T *, l2_channel_type_T *, byte *);
extern void        l2_timer_clr_params(byte);

