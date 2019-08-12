#ifndef L2_TRANSMIT_H
#define L2_TRANSMIT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 2   T R A N S M I T   H E A D E R   F I L E

DESCRIPTION
   Contains functions and variables related to transmitting a frame.


Copyright (c) 2001-2014 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_transmit.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
28/09/11   SJW      Cancel pending access confirmations when restarting
29/10/10   SJW      Added support for Dual SIM
01/07/03   JAC      Added l2_send_SACCH
===========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "geran_variation.h"
#include "customer.h"
#include "l2i.h"
#include "l2_transfer.h"
#include "l2_l1.h"
#include "l2_l1_g.h"

/* =======================================================================
**                        Function Declarations
** ======================================================================= */
void         l2_handle_optional_send_timeout(l2_store_T *, l2_channel_type_T);
void         l2_check_transmit_on_receive(l2_store_T *);
void         l2_handle_ph_ready_to_send_ind(l2_store_T *, l2_channel_type_T);
void         l2_send_DCCH(l2_store_T *);
void         l2_send_SACCH(l2_store_T *);
void         l2_reset_transmit_flags(l2_store_T *, SAPI_T);
#endif

