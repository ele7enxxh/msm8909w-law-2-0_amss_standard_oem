#ifndef L2_MSG_H
#define L2_MSG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 2   M E S S A G E   H E A D E R   F I L E

DESCRIPTION
   Contains functions and variables related to decoding, encoding, receiving,
   and sending a frame.


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_msg.h_v   1.5   05 Jun 2002 10:58:08   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_msg.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
29/10/10   SJW       Added support for Dual SIM
02/11/03   JAC       Added short header support
10/17/02   JAC       Moved some functions to l2_transmit.c l2_general.c
01/25/02   JAC       Fixed l3_message_to_send size to MAX_OCTETS_L3_MESSAGE 
12/17/01   JAC       Made l3_message_to_send a static buffer
10/19/01   JAC       Initial version

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "l2i.h"
#include "l2_transfer.h"

/* -----------------------------------------------------------------------
** Typedefs
** ----------------------------------------------------------------------- */
typedef enum
{
  SAPI_ESTABLISHED,
  SAPI_SUSPENDED
} suspension_state_T;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

void l2_decode_frame(l2_store_T *, byte *, frame_type_T *, SAPI_T *, boolean *, uint32);
void l2_clear_send_queue(sapi_info_T *, l2_channel_type_T);
void l2_clear_send_buffer(sapi_info_T *, l2_channel_type_T);
void l2_store_new_l3_message(l2_store_T *, SAPI_T, l2_channel_type_T, suspension_state_T);

void l2_code_sabm(sapi_info_T *, channel_info_T *, boolean, byte *, byte);
void l2_code_ua(byte, sapi_info_T *, channel_info_T *);
void l2_code_dm(byte, sapi_info_T *, channel_info_T *);
void l2_code_disc(sapi_info_T *, channel_info_T *, boolean);
void l2_code_ui(sapi_info_T *, channel_info_T *, boolean, byte *, byte);
void l2_code_i(byte PF, boolean, sapi_info_T *, channel_info_T *, boolean);
void l2_code_rr(byte, byte, sapi_info_T *, channel_info_T *);
void l2_code_rej(byte, byte, sapi_info_T *, channel_info_T *);

void l2_next_segment(l2_store_T *, sapi_info_T *, channel_info_T *);

void l2_seed_random(l2_store_T *);

#endif
