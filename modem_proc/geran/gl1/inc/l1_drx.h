#ifndef L1_DRX_H
#define L1_DRX_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      L A Y E R  1  D R X

GENERAL DESCRIPTION
   This module contains procedures to support DRX in Layer 1.
   Together these functions make up the DRX manager.

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_drx.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
28/10/14   cgc      CR745548 GL1 make gl1_drx_require_and_wait_for_next_tick() conditional on awake
06/08/14   df       CR536059 Allow Asynchronous sleep for all idle activities
10/03/14   am       CR624678 TX-ON should happen in full frame
25/02/14   am       CR615566 Activities should not clear their DRX entries based on reference
11/12/13   ap       CR586661 GPRS suspension during SI read after W2G reselection
05/12/13   cs       CR584551 Use correct gas_id for GPS Timetagging in SS mode
05/11/13   cja      CR571501 In DRX on do not turn Tx off if leaving idle in the same frame
19/07/13   gk       CR515617:Don't delay the interrupt if W has the lock and its a rude wakeup
23/10/12   npt      CR413635 - Port sleep code for DIME modem
17/08/12   ip       CR384421 Disable GSM sleep during CCO activity
22/06/12   ip       CR362050 - Set tx state to DRX_TX_ON_ALWAYS when comming from NULL
28/03/12   cja      CR316437 Inhibit sleep on RAT change until it is complete
08/03/12   ip       CR326699 - Code changed for wake up recovery mechanism
31/08/11   cja      CR304401. Wait for frame tick so FW not deactivated when suspended.
20/06/11   cs       DSDS CR287999 Allow sleep to start upon completion of reserve_at
14/06/11   npt      CR287491 Move post tick processing for asynch into DPC to
                    allow for mdsp sleep cmd isr to be processed
28/04/11   pg       Fix compiler warnings
04/04/11   nf       CR280679 added gl1_drx_get_next_tick_req
14/02/11   cs       Slight redesign of asynch int to allow for a silent frame
                    interrupt for the frame after a failed asynch attempt
09/12/10   cs       CR264682 Add support for asynch pch
23/11/09   ip       Call DAL_Device_attach from task context
02/03/07   cs       Add support for new CGPS timetag
03/29/06   gfr      Added gl1_drx_first_awake_frame
04/12/05   gfr      Added sys_stru.h to get definition of IMH_T
04/11/05   gfr      Added prototype for gl1_drx_process_timetag_request
02/25/03   gw       Added prototype for function gl1_drx_require_TX().
02/12/03   gw       Changed prototype for gl1drx_report_te to support new
                    FEE timing error functionality.
01/02/03   gw       Added argument to gl1_drx_post_tick().
10/28/02   gw       Added prototype for gl1_drx_post_tick().
06/10/02   gw       Removed parameter from gl1_drx_tick().  Added prototype
                    for gl1_drx_report_te.
10/10/01   gw       Initial version.
===========================================================================*/

#include "geran_variation.h"
#include "comdef.h"    /* for types */
#include "sys_stru.h"
#include "geran_multi_sim.h"

void gl1_drx_reset( gas_id_t gas_id );

void gl1_drx_on( boolean keep_tx_on, gas_id_t gas_id );

void gl1_drx_off(boolean  turn_tx_on, gas_id_t gas_id );

void gl1_set_tx_on_always ( gas_id_t gas_id );

boolean gl1_drx_tick( gas_id_t gas_id );

void gl1_drx_post_tick( boolean in_task_context, gas_id_t gas_id );

void gl1_drx_task_active( gas_id_t gas_id );

void gl1_drx_task_inactive( gas_id_t gas_id );

void gl1_drx_require_next_tick( gas_id_t gas_id );

void gl1_drx_require_and_wait_for_next_tick( gas_id_t gas_id );

void gl1_drx_require_and_wait_for_next_tick_conditional( gas_id_t gas_id );

uint8 gl1_drx_require_tick( uint32 frame_number, gas_id_t gas_id );

void gl1_drx_release_tick( uint8 reference, gas_id_t gas_id );

void gl1_drx_release_fn_tick( uint32 frame_number, gas_id_t gas_id );

void gl1_drx_report_te( int16 te[], uint16 snr[], uint16 num, gas_id_t gas_id );

void gl1_drx_require_TX( uint32 num_frames, gas_id_t gas_id );

boolean gl1_drx_first_awake_frame( gas_id_t gas_id );

void gl1_drx_set_frame_number_adjustment ( int32 num, gas_id_t gas_id );

boolean gl1_is_drx_on( gas_id_t gas_id );

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
void gl1_drx_rf_reinitialised( gas_id_t gas_id );

boolean gl1_drx_cm_starting( gas_id_t gas_id );
void    gl1_drx_cm_shutdown( gas_id_t gas_id );
boolean l1_is_delaying_interrupt_needed( gas_id_t gas_id );
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

void gl1_drx_process_timetag_request (IMH_T *msg_header);

boolean gl1_sleep_active ( gas_id_t gas_id );
extern boolean gl1_drx_is_gsm_asleep( gas_id_t gas_id );

extern void gl1_drx_set_pch_offset( int16 asynch_pch_offset, gas_id_t gas_id );
extern void gl1_drx_asynch_post_tick( gas_id_t gas_id );
extern void gl1_drx_task_sleep( boolean called_from_asynch_dpc, gas_id_t gas_id );
extern void gl1_drx_update_counters( int32 bump, boolean do_gstmr, gas_id_t gas_id );
extern int16 gl1_drx_get_pch_offset( gas_id_t gas_id );

extern boolean gl1_drx_asynch_int_sleep_possible_precheck( gas_id_t gas_id );

extern void gl1_drx_set_asynch_int_ignore_gstmr( boolean asynch_int_ignore_next_gstmr, gas_id_t gas_id );
extern boolean gl1_drx_get_asynch_int_ignore_gstmr( gas_id_t gas_id );

extern boolean gl1_drx_get_next_tick_req ( gas_id_t gas_id );

extern void gl1_drx_set_sleep_inhibit (boolean sleep_inhibit, gas_id_t gas_id);
extern boolean gl1_drx_get_sleep_inhibit(void);

extern void gl1_drx_set_sleep_inhibit_for_cco_req (boolean sleep_cco_inhibit, gas_id_t gas_id);

void gl1_drx_set_sleep_inhibit_for_trm(boolean sleep_inhibit, gas_id_t gas_id);
boolean gl1_drx_get_sleep_inhibit_for_trm(void);

extern void    gl1_drx_post_tick_dpc( gas_id_t gas_id );
#endif /* L1_DRX_H */
