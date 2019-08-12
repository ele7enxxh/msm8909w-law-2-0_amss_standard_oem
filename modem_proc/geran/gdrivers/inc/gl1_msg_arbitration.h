 #ifndef GL1_MSG_ARBITRATION_H
#define GL1_MSG_ARBITRATION_H
/*========================================================================
                      GSM GPRS NPL1 MSG LAYER ARBITRATION
DESCRIPTION
   This module defines the arbitration interface exposed by the the GSM/GPRS
   NPL1 MSG LAYER. The interface allows an external arbitration component
   to be installed in the GSM/GPRS NPL1 MSG LAYER and perform Rx and Tx
   arbitration against external access stratums.

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_msg_arbitration.h#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
03/07/14   og        CR652720. L2G DSDA bring-up updates.
02/07/14   mc        CR.687340 : BOLT 2.0 : COEX : Mass Merge of COEX changes from DI4.0
22/05/14   cah       Bolt 2.0: FEATURE_GERAN_DRDSDS
26/03/14   ab        CR642231 : COEX : GL1 Arbitrator to allow upto 10 Monitor Freq required for X2G to prevent Invalid Freq Id
                     and possible memory corruption
21/03/14   mc        CR.588853 : COEX : GL1 changes for Enhanced FCCH algorithm
05/03/14   mc        CR.583282 : Limit the number of PSCAN monitors per frame to that which can be COEX protected in X+G DSDA
04/11/13   mc        CR.556742 G+G COEX : Enhancements for monitor positioning and improved SDCCH/FACCH conflict detection
05/11/13   js        CR.555528 : COEX : CXM Priority and Freq ID for Async Rx
01/10/13   sk        CR552259 CR sync from 1.0/1.1
23/09/13   sk        2.0 syncup from 1.1
15/08/13   sk        CR524039 GBTA bringup changes
29/07/13   sk        CR519663 Partial QBTA bringup changes
05/04/13   mc        CR.470155 : COEX Support - GERAN arbitrator framework - PS integration
25/03/13   mc        CR.464496 : COEX Support - GERAN arbitrator SW CXM integration
13/03/13   mc        CR.459487 : COEX Support - GERAN arbitrator framework
========================================================================== */

#include "geran_variation.h"
#ifdef CUST_H
#include "customer.h"
#endif

#include "gl1_defs.h"
#include "gl1_defs_g.h"
#include "gl1_hw.h"
#include "gl1_hw_g.h"
#include "geran_dual_sim.h"

typedef enum {ARBITRATE_RX, ARBITRATE_TX, ARBITRATE_MONITORS, ARBITRATE_ACQ, ARBITRATE_SCH, ARBITRATE_ASYNCRX, ARBITRATE_MONITORS_HP} gl1_msg_arbitrated_event_t;

typedef void ( *gl1_msg_arbitration_reg_t )(gas_id_t);
typedef void ( *gl1_msg_arbitration_notify_t )(gl1_msg_arbitrated_event_t, gas_id_t);

#define MAX_MONITOR_NOZONES 2

#define MAX_MEAS_PER_FRAME_INC_IRAT  MAX_CM_RSSI_MEASUREMENTS

typedef struct
{
  boolean                       configured;
  gl1_hw_channel_type           channel_type;
  uint32*                       priority;
  uint32*                       desense_id;
  uint8                         burst_num;
} gl1_msg_arbitrated_rx_slot_t;

typedef struct
{
  boolean                       configured;
  gl1_hw_channel_type           channel_type;
  uint32*                       priority;
  uint32*                       desense_id;
  int16*                        backoff_desense;
  int16*                        backoff_vbatt;
  uint8                         burst_num;
  geran_pdu_priority_t          pdu_prio;
} gl1_msg_arbitrated_tx_slot_t;

typedef struct
{
  ARFCN_T                       arfcn;
  uint32                        search_width;
  gl1_msg_arbitrated_rx_slot_t  slots[GL1_DEFS_SLOTS_IN_FRAME];
} gl1_msg_arbitrated_rx_t;

typedef struct
{
  ARFCN_T                       arfcn;
  uint16                        timing_advance;
  int16                         power;
  gl1_msg_arbitrated_tx_slot_t  slots[GL1_DEFS_SLOTS_IN_FRAME];
} gl1_msg_arbitrated_tx_t;

typedef struct
{
  ARFCN_T                       arfcn;
  int32                         qs_position;
  uint32                        priority;
  uint32                        desense_id;
  uint32                        skip;
} gl1_msg_arbitrated_monitor_t;

typedef struct
{
  ARFCN_T*                      arfcns;
  gl1_msg_arbitrated_monitor_t  monitor[MAX_MEAS_PER_FRAME_INC_IRAT];
  int32                         count;
  int32                         rf_setup_time;
  int32                         guard_period;
  int32                         monitor_duration;
  int32                         max_per_frame;
  int32                         nozone_start[MAX_MONITOR_NOZONES];
  int32                         nozone_end[MAX_MONITOR_NOZONES];
  int32                         nozone_count;
} gl1_msg_arbitrated_monitors_t;

typedef struct
{
  ARFCN_T                       arfcn;
  boolean                       enhanced_fcch_detection;
  uint32                        priority;
  uint32                        desense_id;
} gl1_msg_arbitrated_acq_t;

typedef struct
{
  ARFCN_T                       arfcn;
  uint16                        timing_offset;
  uint32                        priority;
  uint32                        desense_id;
} gl1_msg_arbitrated_sch_t;

typedef struct
{
  ARFCN_T                       arfcn;
  uint16                        timing_offset;
  uint32                        priority;
  uint32                        desense_id;
} gl1_msg_arbitrated_asyncrx_t;

typedef struct
{
  gl1_msg_arbitrated_rx_t       rx;
  gl1_msg_arbitrated_tx_t       tx;
  gl1_msg_arbitrated_monitors_t monitors;
  gl1_msg_arbitrated_acq_t      acq;
  gl1_msg_arbitrated_sch_t      sch;
  gl1_msg_arbitrated_asyncrx_t  async_rx;

  gl1_msg_arbitration_notify_t  notify_event;

  gl1_msg_arbitration_reg_t     registration;
  gl1_msg_arbitration_reg_t     deregistration;


} gl1_msg_arbitration_t;

gl1_msg_arbitration_t* gl1_msg_get_arbitration(gas_id_t gas_id);

void gl1_msg_arbitration_init(gas_id_t gas_id);

void gl1_msg_cfg_arbitration(gl1_msg_arbitration_reg_t registration, gl1_msg_arbitration_reg_t deregistration, gas_id_t gas_id);

#endif /* GL1_MSG_ARBITRATION_H */
