#ifndef L2_TRANSFER_H
#define L2_TRANSFER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 2   T R A N S F E R   H E A D E R   F I L E

DESCRIPTION
   Contains functions and variables related to information transfer.


Copyright (c) 2001-2014 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_transfer.h_v   1.2   07 Mar 2002 14:20:48   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_transfer.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
10/17/02   JAC       Added T_ack timer in sapi_info
10/19/01   JAC       Initial revision

===========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "l2i.h"

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

void l2_handle_info_reception(l2_store_T *, SAPI_T, l2_channel_type_T);
void l2_handle_normal_rr_reception(l2_store_T *, SAPI_T, l2_channel_type_T);
l2_event_T l2_handle_recovery_rr_reception(l2_store_T *, SAPI_T, l2_channel_type_T);
void l2_handle_normal_rej_reception(l2_store_T *, SAPI_T, l2_channel_type_T);
l2_event_T l2_handle_recovery_rej_reception(l2_store_T *, SAPI_T, l2_channel_type_T);
void l2_handle_I_retransmission(l2_store_T *, SAPI_T, l2_channel_type_T);
void l2_update_N200_channel_type(l2_store_T *, l2_channel_type_T);
l2_event_T l2_check_pending_sequence_error(l2_store_T *, SAPI_T, l2_channel_type_T);

#endif /* L2_TRANSFER_H */

