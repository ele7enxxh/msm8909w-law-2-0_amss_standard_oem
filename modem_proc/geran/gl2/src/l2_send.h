#ifndef L2_SEND_H
#define L2_SEND_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 2   S E N D   H E A D E R   F I L E

DESCRIPTION
   Contains functions that send messages to L1 or RR.


Copyright (c) 2001-2014 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_send.h_v   1.4   05 Jun 2002 10:57:04   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_send.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
29/10/10   SJW       Added support for Dual SIM
02/11/03   JAC       Added short header support
04/30/02   JAC       Added byte parameter in l2_send_dl_unit_data_ind for tc field
10/19/01   JAC       Initial version

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "rr_l2.h"
#include "rr_l2_g.h"
#include "rr_l2_l1.h"
#include "l2i.h"
#include "cb_l2.h"

/* =======================================================================
**                        Type Declarations
** ======================================================================= */


/* =======================================================================
**                        Function Declarations
** ======================================================================= */
void l2_send_mdl_error_ind(l2_store_T *, error_code_T);
void l2_send_dl_release_ind(l2_store_T *, SAPI_T, release_mode_T);
void l2_send_dl_release_cnf(l2_store_T *, SAPI_T, release_mode_T);
void l2_send_dl_establish_cnf(l2_store_T *, establish_mode_T);
void l2_send_dl_establish_ind(l2_store_T *, SAPI_T, l2_channel_type_T, boolean);
void l2_send_dl_unit_data_ind(l2_store_T *, boolean, ARFCN_T, byte, boolean);
void l2_send_dl_suspend_cnf(l2_store_T *);
void l2_send_dl_resume_cnf(l2_store_T *);
void l2_send_dl_reconnect_cnf(l2_store_T *);
void l2_send_dl_data_ind(l2_store_T *, SAPI_T, l2_channel_type_T);
void l2_send_dl_data_ack_ind(l2_store_T *, SAPI_T, message_entry_T*);
void l2_send_ph_data_req(l2_store_T *, SAPI_T, channel_info_T*, byte);
void l2_send_ph_stop_sacch_measurement_report(l2_store_T *);

l2_store_T * l2_send_dl_random_access_cnf(ph_random_access_cnf_T *);
void l2_send_ph_random_access_req(const dl_random_access_req_T *);
void l2_send_ph_random_access_ind(const dl_random_access_req_T *, const boolean);

void l2_send_dl_cb_block_ind(l2_store_T *, byte *,uint32);

void l2_set_sacch_repetition_order(l2_store_T *, byte);
void l2_crc_sacch_failure(l2_store_T *);

l2_store_T * l2_install_demux_callbacks(const dl_data_mux_ind_T *);

/* =======================================================================
**                        Macros
** ======================================================================= */

/* use a macro to send MDL ERROR INDs to RR, as this will allow the debug output
 * to show the line where the error originated, rather than in the send function
 */
#define L2_SEND_MDL_ERROR_IND(l2ptr,err_cause) {MSG_MED("MDL_ERROR_IND %d",(int)(err_cause),0,0);l2_send_mdl_error_ind((l2ptr),(err_cause));}

/* some MDL ERROR INDs can be safely dropped, as no further action is required
 * this will saves a bit of the overhead involved with sending a message to RR
 */
#define L2_DROP_MDL_ERROR_IND(err_cause) {MSG_LOW("Discarded MDL_ERROR_IND %d",(int)(err_cause),0,0);}

#endif
