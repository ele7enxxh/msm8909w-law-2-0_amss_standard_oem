/*!
  @file
  cxm_diag.c

  @brief
  API definitions for CXM's DIAG interface

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/cxm_diag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/15   ckk     Add support for type7[ant_tx]
12/04/14   tak     Added support for TDD LTE in SET_WWAN_TECH_STATE
11/04/13   tak     Type 0 through CCS GRFC register
01/30/13   ckk     Add SET_WWAN_TECH_STATE command
10/26/12   taa     Stubbed out RF calls
10/19/12   btl     Convert timers to non-deferrable
09/21/12   btl     Add originator & receiver commands
           ckk     Initial implementation

===========================================================================*/

/*==========================================================================

                        CXM DIAG USAGE INSTRUCTIONS

The general format of a Diag command is as follows:

  send_data 75 <Diag subsys cmd enum #> <command> 0 <args>

The command number comes from the enum cxm_diag_table_id_type below. The arguments 
are passed in bytestream format, low byte of word first. For example, if you 
want to pass (int16)4 and (int8)5, the command would look something like this:

  send_data 75 91 0x01 0 0x04 0x00 0x05

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <stringl/stringl.h>
#include <diagpkt.h>
#include <diagcmd.h>
#include <atomic_ops.h>
#include <mcpm_api.h>
#include <wci2_uart.h>
#include <wci2_core.h>
#include <wci2_diag.h>
#include <npa.h>
#include <lte_l1_types.h> /* to access LTE_L1_FRAME_STRUCTURE_FS2 */
#include <wcn_coex_mgr.h>

#include "fws.h"
#include "mcs_hwio.h"
#include "wcn_coex_mgr.h"
#include "cxm_utils.h"
#include "cxm_diag.h"
#include "cxm_msgr.h"
#include "coex_algos.h"
#include "cxm_fw_msg.h"
#include "cxm_trace.h"
#include "cxm_intf_types.h"

/*===========================================================================

                        INTERNAL DEFINITIONS & TYPES

===========================================================================*/
#define CXM_COUNTER_ID           1
#define CXM_LTE_FW_COUNTER_ID    2
#define WCN_PRIO_L2VIC_POS       240
#define MSEC_TO_USEC             1000 

#define CXM_FRAME_SYNC_GRFC_BIT          0x20000000  /*GRFC 29. The 29th bit in a 32-bit mask*/
#define CXM_RX_ACTIVE_GRFC_BIT           0x40000000  /*GRFC 30*/
#define CXM_TX_ACTIVE_GRFC_BIT           0x08000000  /*GRFC 27*/

/* list of cxm diag request indexes */
/* gaps indicate commands present on other branches, but not here */
typedef enum
{
  ENABLE_CXM                    = 0x00,
  TRIGGER_WCI2_TYPE0_MSG        = 0x01,
  SEND_ARBITRARY_WCI2_MSG_SEQ   = 0x02,
  SEND_8_ARBITRARY_WCI2_MSGS    = 0x03,
  STOP_CURRENT_TEST             = 0x04,
  ENABLE_CXM_UART_LOOPBACK_MODE = 0x05,
  START_ORIGINATOR_MODE         = 0x06,
  GET_ORIGINATOR_MODE_RESULT    = 0x07,
  START_RECEIVER_MODE           = 0x08,
  SET_CXM_UART_BAUD_RATE        = 0x09,
  SET_WWAN_TECH_STATE           = 0x0A,
  TRIGGER_WCN_PRIO              = 0x0B,
  LOG_FW_COUNTERS               = 0x0C,  
  ENABLE_RF_COMMAND             = 0x0D,
  SEND_SUBFR_NOTIFICATION       = 0x0E,
  SEND_POLICY_MSG               = 0x0F,
  SET_WCI2_TYPE7_FILTER_TIME    = 0x10,
  TEST_WCN_TX_DRIVER            = 0x11,
  SET_TX_ANT_SEL                = 0x12
} cxm_diag_table_id_type;

typedef enum
{
  CXM_DIAG_TX_STICKY_ENABLE = 0,
  CXM_DIAG_TX_STICKY_DISABLE,
  CXM_DIAG_TX_STICKY_START,
  CXM_DIAG_TX_STICKY_STOP
} cxm_diag_tx_sticky_e;

/* npa client for voting uart power state on/off */
STATIC npa_client_handle cxm_diag_uart_client;

/*--------------------------------------------------------------------------
                         ENABLE CXM UART

Call at the beginning and end of CXM DIAG testing to power on 
and power off the UART. If this command is not called first, UART
will not work if it is powered down.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, ENABLE_CXM)
  boolean         enable_cxm;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, ENABLE_CXM)
  errno_enum_type error;
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                           TRIGGER WCI2 TYPE0 MSG

This interface provides a way for a test tool to poke anyone of the following
hardware registers/bits
- FRAME_SYNC
- TX_ACTIVE
- RX_PRIORITY
to trigger a Type0 WCI2 byte message over the CXM UART.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, TRIGGER_WCI2_TYPE0_MSG)
  boolean         set_frame_sync_bit;
  boolean         set_tx_active_bit;
  boolean         set_rx_priority_bit;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, TRIGGER_WCI2_TYPE0_MSG)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                          SEND ARBITRARY WCI2 MSG SEQ

This interface provides a way for a test tool to trigger mechanism to send
out various sequences  of arbitrary wCI2 type bytes over the CXM UART. 
Sends continously until told to stop.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, SEND_ARBITRARY_WCI2_MSG_SEQ)
  uint8           seq_index;
  uint16          time_apart_ms;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, SEND_ARBITRARY_WCI2_MSG_SEQ)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                           SEND 8 ARBITRARY WCI2 MSGS

This interface provides a way for a test tool to send out 8 random bytes
(specified by tester) over the CXM UART. Sends continuosly until told to 
stop.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, SEND_8_ARBITRARY_WCI2_MSGS)
  uint8           byte0;
  uint8           byte1;
  uint8           byte2;
  uint8           byte3;
  uint8           byte4;
  uint8           byte5;
  uint8           byte6;
  uint8           byte7;
  uint16          time_apart_ms;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, SEND_8_ARBITRARY_WCI2_MSGS)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                           SEND POLICY MSG

This interface provides a way for a test tool to any coex policy.  Each
policy is a bit in the uint32 payload.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, SEND_POLICY_MSG)
  uint64          policy;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, SEND_POLICY_MSG)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                           STOP CURRENT TEST

This interface provides a way for a test tool to stop whatever test is
currently occupying the cxm task
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, STOP_CURRENT_TEST)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, STOP_CURRENT_TEST)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         ENABLE CXM UART's LOOPBACK MODE

This interface provides a way for a test tool to enable/disable CXM UART's
loopback mode.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, ENABLE_CXM_UART_LOOPBACK_MODE)
  boolean         enable_uart_loopback_mode;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, ENABLE_CXM_UART_LOOPBACK_MODE)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         START ORIGINATOR MODE

This interface provides a way to send CXM_DIAG_BURST_LENGTH (4) bytes and confirm the same were 
received correctly. In other words, MDM acts as originator in the test
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, START_ORIGINATOR_MODE)
  uint8           byte0;
  uint8           byte1;
  uint8           byte2;
  uint8           byte3;
  uint16          time_apart_ms;
  uint16          num_times;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, START_ORIGINATOR_MODE)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         GET ORIGINATOR MODE RESULT

This interface provides a way to collect the results of the previously-run
originator mode test
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, GET_ORIGINATOR_MODE_RESULT)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, GET_ORIGINATOR_MODE_RESULT)
  uint16          num_run;
  uint16          num_failed;
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         START RECEIVER MODE

Start test mode where MDM acts in receiver mode, looping back data
through the UART

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, START_RECEIVER_MODE)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, START_RECEIVER_MODE)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         SET CXM UART BAUD RATE

Set the cxm_uart baud rate

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, SET_CXM_UART_BAUD_RATE )
  wci2_uart_baud_type_e baud;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, SET_CXM_UART_BAUD_RATE )
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         SET WWAN TECH STATE

Set the state for various WWAN techs

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, SET_WWAN_TECH_STATE )
  uint8  tech_id; /* Tech ID */
  uint16 ul_freq; /* Tech operating up-link frequency in MHz */
  uint16 ul_bw;   /* Tech operating up-link bandwidth in KHz */
  uint16 dl_freq; /* Tech operating down-link frequency in MHz */
  uint16 dl_bw;   /* Tech operating down-link bandwidth in KHz */
  uint8  tdd;     /* Whether this is for TDD or not */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, SET_WWAN_TECH_STATE )
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         TRIGGER WCN PRIO

Trigger WCN Prio

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, TRIGGER_WCN_PRIO )
  uint8 num_ms;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, TRIGGER_WCN_PRIO )
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         LOG FW COUNTERS

Logs FW counters

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, LOG_FW_COUNTERS )
  uint8 reset;   /*if true, reset CXM and FW counters after logging them*/
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, LOG_FW_COUNTERS )
  uint16 cxm_counter_id;
  uint16 num_cxm_counters;
  uint32 cxm_counters[CXM_CNT_MAX_COUNTERS];    /*cxm counters*/
  uint16 lte_fw_counter_id;
  uint16 num_lte_fw_counters;
  uint32 fw_counters[CXM_CNT_MAX_FW_COUNTERS];  /*FW counters*/
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         ENABLE RF COMMAND

Enables RF command app so that GRFC tasks can be used

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, ENABLE_RF_COMMAND )
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, ENABLE_RF_COMMAND )
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         SEND SUBFRAME NOTIFICATION

Send subframe notification for 1ms wide subframes

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, SEND_SUBFR_NOTIFICATION )
  uint8  subr_transmit[CXM_NUM_NOTIFI_SUBFRS]; /* Transmit on this subframe if it is not zero */
  uint8  subr_adv_time[CXM_NUM_NOTIFI_SUBFRS]; /*Advanced timing for this subframe in 100microseconds*/
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, SEND_SUBFR_NOTIFICATION )
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         SET WCI2 TYPE7 FILTER TIME

Set the filter timeout for deciding whether or not to send the type 7.
Also affects the delay between tech triggering TX change and type 7 
being sent out modem.

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, SET_WCI2_TYPE7_FILTER_TIME )
  uint8  event;
  uint32 num_ms;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, SET_WCI2_TYPE7_FILTER_TIME )
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         TEST WCN TX STICKY BIT DRIVER

Set the filter timeout for deciding whether or not to send the type 7.
Also affects the delay between tech triggering TX change and type 7 
being sent out modem.

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, TEST_WCN_TX_DRIVER)
  uint8 client;
  uint8 event;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, TEST_WCN_TX_DRIVER)
  uint8 tx_rsp_e;
  uint8 tx_state;
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
 	                            SET TX ANT SEL

Set the selection for the transmit (Tx) antenna in use
This will trigger WCI2 type7 message over CXM UART if unique change

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(CXM, SET_TX_ANT_SEL )
  uint8 tx_ant_sel;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CXM, SET_TX_ANT_SEL )
  errno_enum_type error;
DIAGPKT_RSP_END

/*===========================================================================


                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
static PACK(void *) cxm_diag_trigger_wci2_type0_msg(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_send_arbitrary_wci2_msg_seq(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_send_8_arbitrary_wci2_msgs(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_send_policy_msg(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_stop_current_test(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_enable_cxm_uart_loopback_mode(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_start_originator_mode (PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_get_originator_mode_result (PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_start_receiver_mode (PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_set_cxm_uart_baud_rate (PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_enable_cxm (PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_set_wwan_tech_state (PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_trigger_wcn_prio (PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_log_fw_counters (PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_enable_rf_command (PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_send_subfr_notification (PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_set_wci2_type7_filter_time(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_test_wcn_tx_driver(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) cxm_diag_set_tx_ant_sel(PACK(void*) req_pkt, uint16 pkt_len);

/*===========================================================================

                        INTERNAL VARIABLES

===========================================================================*/
static const diagpkt_user_table_entry_type cxm_diag_tbl[] =
{
  /* start id, end id, callback function */
  {(word)TRIGGER_WCI2_TYPE0_MSG,
   (word)TRIGGER_WCI2_TYPE0_MSG,
   cxm_diag_trigger_wci2_type0_msg},
  {(word)SEND_ARBITRARY_WCI2_MSG_SEQ,
   (word)SEND_ARBITRARY_WCI2_MSG_SEQ,
   cxm_diag_send_arbitrary_wci2_msg_seq},
  {(word)SEND_8_ARBITRARY_WCI2_MSGS,
   (word)SEND_8_ARBITRARY_WCI2_MSGS,
   cxm_diag_send_8_arbitrary_wci2_msgs},
  {(word)SEND_POLICY_MSG,
   (word)SEND_POLICY_MSG,
   cxm_diag_send_policy_msg},
  {(word)STOP_CURRENT_TEST,
   (word)STOP_CURRENT_TEST,
   cxm_diag_stop_current_test},
  {(word)ENABLE_CXM_UART_LOOPBACK_MODE,
   (word)ENABLE_CXM_UART_LOOPBACK_MODE,
   cxm_diag_enable_cxm_uart_loopback_mode},
  {(word)START_ORIGINATOR_MODE,
   (word)START_ORIGINATOR_MODE,
   cxm_diag_start_originator_mode},
  {(word)GET_ORIGINATOR_MODE_RESULT,
   (word)GET_ORIGINATOR_MODE_RESULT,
   cxm_diag_get_originator_mode_result},
  {(word)START_RECEIVER_MODE,
   (word)START_RECEIVER_MODE,
   cxm_diag_start_receiver_mode},
  {(word)SET_CXM_UART_BAUD_RATE,
   (word)SET_CXM_UART_BAUD_RATE,
   cxm_diag_set_cxm_uart_baud_rate},
  {(word)ENABLE_CXM,
   (word)ENABLE_CXM,
   cxm_diag_enable_cxm},
  {(word)SET_WWAN_TECH_STATE,
   (word)SET_WWAN_TECH_STATE,
   cxm_diag_set_wwan_tech_state},
  {(word)TRIGGER_WCN_PRIO,
   (word)TRIGGER_WCN_PRIO,
   cxm_diag_trigger_wcn_prio},
  {(word)LOG_FW_COUNTERS,
   (word)LOG_FW_COUNTERS,
   cxm_diag_log_fw_counters},  
  {(word)ENABLE_RF_COMMAND,
   (word)ENABLE_RF_COMMAND,
   cxm_diag_enable_rf_command},
  {(word)SEND_SUBFR_NOTIFICATION,
   (word)SEND_SUBFR_NOTIFICATION,
   cxm_diag_send_subfr_notification},
  {(word)SET_WCI2_TYPE7_FILTER_TIME,
   (word)SET_WCI2_TYPE7_FILTER_TIME,
   cxm_diag_set_wci2_type7_filter_time},
  {(word)TEST_WCN_TX_DRIVER,
   (word)TEST_WCN_TX_DRIVER,
   cxm_diag_test_wcn_tx_driver},
  {(word)SET_TX_ANT_SEL,
   (word)SET_TX_ANT_SEL,
   cxm_diag_set_tx_ant_sel}
};

/* resources for communication between cxm_diag_send* functions and
   their timer handlers */
static cxm_diag_arb_msg_info_type cxm_diag_msg_info;

/* used to tell cxm_diag_send...() functions to stop */
volatile static boolean cxm_diag_continue_test_flag;

static timer_type cxm_diag_send_arb_msgs_timer;

/* mutex to control use of the cxm uart during diag tests */
static atomic_word_t cxm_diag_uart_mutex;

/* store the subrame notification parameters */
static cxm_diag_subfr_notifi_type cxm_diag_subr_notifi_params;
static const uint32 cxm_diag_subfr_notifi_timing_ustmr[] 
                           = {0, 19200, 38400, 57600, 76800}; /*1ms apart subframes*/

/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  cxm_diag_init

===========================================================================*/
/*!
  @brief
    To initialize CXM's DIAG interface

  @return
    None
*/
/*=========================================================================*/
void cxm_diag_init ( void )
{
  /*-----------------------------------------------------------------------*/
  DIAGPKT_DISPATCH_TABLE_REGISTER((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                                   cxm_diag_tbl); 

  /* initialize to zero */
  atomic_set( &cxm_diag_uart_mutex, 0 );

  /* Set up timer to send test wci2 msgs at specified interval */
  timer_def( &cxm_diag_send_arb_msgs_timer, &cxm_nondeferrable_timer_group, 
             &cxm_tcb, CXM_DIAG_SEND_SIG, NULL, 0 );

  /* create npa client for turning on/off uart */
  cxm_diag_uart_client = npa_create_sync_client(
                           "/modem/mcs/cxm_uart",
                           "cxm_diag",
                           NPA_CLIENT_REQUIRED );

  return;
} /* cxm_diag_init */

/*===========================================================================

  FUNCTION:  cxm_diag_deinit

===========================================================================*/
/*!
  @brief
    To de-initialize CXM's DIAG interface

  @return
    None
*/

/*=========================================================================*/
void cxm_diag_deinit (
  void
)
{
  /*-----------------------------------------------------------------------*/
  timer_clr ( &cxm_diag_send_arb_msgs_timer, T_USEC );
  return;
} /* cxm_diag_deinit */

/*===========================================================================

  FUNCTION:  cxm_diag_send_arb_msgs_timer_handler

===========================================================================*/
/*!
  @brief
    timer handler to periodically send bytes for cxm_diag_send_8_arbitrary_wci2_msgs()
    Loops until told to stop. Actually called from CXM_TASK context, not interrupt

  @return
    void
*/

/*=========================================================================*/
void cxm_diag_send_arb_msgs_timer_handler (
  void
)
{
  cxm_coex_tech_tx_adv_ntc_ind_s adv_ind;
  unsigned int nxt_subr_time_apart;
  uint32 curr_ustmr;
  /*-----------------------------------------------------------------------*/
  /* check if space available in uart */
  if( !wci2_is_uart_tx_full() )
  {
    switch(cxm_diag_msg_info.test)
    {
      case CXM_DIAG_SUBFR_NOTIFI:
        /*send notification*/  
        memset( &adv_ind, 0, sizeof( cxm_coex_tech_tx_adv_ntc_ind_s ) );
        adv_ind.params.lte.transmit = cxm_diag_subr_notifi_params.transmit[cxm_diag_subr_notifi_params.index];
        adv_ind.params.lte.ustmr_time = cxm_diag_subfr_notifi_timing_ustmr[cxm_diag_subr_notifi_params.index];
        adv_ind.params.lte.rbs = 0; 
        adv_ind.params.lte.tx_power = 32767;

        /*send message*/
        cxm_msgr_send_msg( &adv_ind.msg_hdr,
                           MCS_CXM_COEX_TECH_TX_ADV_NTC_IND,
                           sizeof(cxm_coex_tech_tx_adv_ntc_ind_s) );

        //timer_clr(&cxm_diag_send_arb_msgs_timer, T_USEC);
        cxm_diag_subr_notifi_params.index++;
        if (cxm_diag_subr_notifi_params.index < CXM_NUM_NOTIFI_SUBFRS)
        {
          /*set timer to the next subframe*/
          curr_ustmr = COEX_READ_USTMR_TIME();
          /*determine the duration in micros we want to wait before sending the first indication*/
          nxt_subr_time_apart = ( COEX_SUB_USTMR(cxm_diag_subr_notifi_params.timing_ustmr[cxm_diag_subr_notifi_params.index], curr_ustmr) ) 
                        / 19.2;

          CXM_MSG_2( LOW, "cur_ustmr: %d, nxt time apart: %d", 
             curr_ustmr, nxt_subr_time_apart);

          timer_set( &cxm_diag_send_arb_msgs_timer,
               (timetick_type) nxt_subr_time_apart,
                0,
                T_USEC );
        }
        break;

      default:
        /* unknown test */
        CXM_ASSERT(0);
        break;
    }
  }

  /* keep sending until told to stop */
  if( !cxm_diag_continue_test_flag && cxm_diag_msg_info.test!=CXM_DIAG_SUBFR_NOTIFI )
  {
    /* finished test; stop timer, flush buff, & release mutex */
    timer_clr(&cxm_diag_send_arb_msgs_timer, T_USEC);
    atomic_set(&cxm_diag_uart_mutex, 0);
  }

  return;
}

/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  cxm_diag_trigger_wci2_type0_msg

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to trigger WCI2 Type0 byte message

  @return
    DIAG_RSP_PKT pointer 
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_trigger_wci2_type0_msg (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  errno_enum_type estatus = E_SUCCESS;
  DIAG_SUBSYS_CXM_TRIGGER_WCI2_TYPE0_MSG_rsp_type *rsp_ptr = NULL;
  DIAG_SUBSYS_CXM_TRIGGER_WCI2_TYPE0_MSG_req_type *req_ptr = NULL;
  uint32          grfc_set_mask   = 0x00000000;
  uint32          grfc_clear_mask = 0x00000000;
  /*-----------------------------------------------------------------------*/
  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_TRIGGER_WCI2_TYPE0_MSG_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    req_ptr = (DIAG_SUBSYS_CXM_TRIGGER_WCI2_TYPE0_MSG_req_type*) req_pkt;
    if (req_ptr->set_frame_sync_bit)
    {
      grfc_set_mask |= CXM_FRAME_SYNC_GRFC_BIT;
    }
    else
    {
      grfc_clear_mask |= CXM_FRAME_SYNC_GRFC_BIT;
    }
    if (req_ptr->set_rx_priority_bit)
    {
      grfc_set_mask |= CXM_RX_ACTIVE_GRFC_BIT;
    }
    else
    {
      grfc_clear_mask |= CXM_RX_ACTIVE_GRFC_BIT;
    }
    if (req_ptr->set_tx_active_bit)
    {
      grfc_set_mask |= CXM_TX_ACTIVE_GRFC_BIT;
    }
    else
    {
      grfc_clear_mask |= CXM_TX_ACTIVE_GRFC_BIT;
    }

    CXM_MSG_2( LOW, "Send type 0 set: %x, clear: %x",
                       grfc_set_mask, grfc_clear_mask);
#if defined(HWIO_CCS_GRFC_SET_OUT)
    HWIO_CCS_GRFC_CLR_OUT(grfc_clear_mask);
    HWIO_CCS_GRFC_SET_OUT(grfc_set_mask);
#endif
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_TRIGGER_WCI2_TYPE0_MSG_rsp_type *)
  diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
		       (diagpkt_subsys_cmd_code_type)TRIGGER_WCI2_TYPE0_MSG,
		        sizeof(DIAG_SUBSYS_CXM_TRIGGER_WCI2_TYPE0_MSG_rsp_type));

  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);

} /* cxm_diag_trigger_wci2_type0_msg */

/*===========================================================================

  FUNCTION:  cxm_diag_send_arbitrary_wci2_msg_seq

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to send arbitrary stream of WCI2 byte message
    note: cannot run at same time as cxm_diag_send_8_arbitrary_wci2_msgs,
          and loops until signalled to stop

  @return
    DIAG_RSP_PKT pointer 
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_send_arbitrary_wci2_msg_seq (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_CXM_SEND_ARBITRARY_WCI2_MSG_SEQ_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_SEND_ARBITRARY_MSG_SEQ_rsp_type *temp_resp_ptr;
  /*-----------------------------------------------------------------------*/
  /* unfortunately this resp doesn't contain the correct cmd and subsys ids.
   * need to free this and make a new one. */
  temp_resp_ptr = (DIAG_SUBSYS_WCI2_WCI2_SEND_ARBITRARY_MSG_SEQ_rsp_type*) 
    wci2_diag_send_arbitrary_wci2_msg_seq( req_pkt, pkt_len );

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_SEND_ARBITRARY_WCI2_MSG_SEQ_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)SEND_ARBITRARY_WCI2_MSG_SEQ,
                         sizeof(DIAG_SUBSYS_CXM_SEND_ARBITRARY_WCI2_MSG_SEQ_rsp_type));
  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = temp_resp_ptr->error;
  diagpkt_free( temp_resp_ptr );

  return(rsp_ptr);

} /* cxm_diag_send_arbitrary_wci2_msg_seq */

/*===========================================================================

  FUNCTION:  cxm_diag_send_8_arbitrary_wci2_msgs

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to send tester provided 8 arbitrary WCI2 bytes
    note: cannot run at same time as cxm_diag_send_arbitrary_wci2_msg_seq,
          and loops until signalled to stop

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_send_8_arbitrary_wci2_msgs (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_CXM_SEND_8_ARBITRARY_WCI2_MSGS_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_SEND_8_ARBITRARY_MSGS_rsp_type *temp_resp_ptr;
  /*-----------------------------------------------------------------------*/
  /* unfortunately this resp doesn't contain the correct cmd and subsys ids.
   * need to free this and make a new one. */
  temp_resp_ptr = (DIAG_SUBSYS_WCI2_WCI2_SEND_8_ARBITRARY_MSGS_rsp_type*) 
    wci2_diag_send_8_arbitrary_wci2_msgs( req_pkt, pkt_len );

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_SEND_8_ARBITRARY_WCI2_MSGS_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)SEND_8_ARBITRARY_WCI2_MSGS,
                         sizeof(DIAG_SUBSYS_CXM_SEND_8_ARBITRARY_WCI2_MSGS_rsp_type));
  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = temp_resp_ptr->error;
  diagpkt_free( temp_resp_ptr );

  return(rsp_ptr);
} /* cxm_diag_send_8_arbitrary_wci2_msgs */

/*===========================================================================

  FUNCTION:  cxm_diag_send_policy_msg

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to set any of the available target
    coex policies.  Each policy is represented in one bit of the 32bit
    payload

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_send_policy_msg (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_SEND_POLICY_MSG_rsp_type *rsp_ptr;
  DIAG_SUBSYS_CXM_SEND_POLICY_MSG_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_SEND_POLICY_MSG_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_CXM_SEND_POLICY_MSG_req_type *) req_pkt;

    coex_send_diag_policy( req_ptr->policy );
  }

  rsp_ptr = (DIAG_SUBSYS_CXM_SEND_POLICY_MSG_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)SEND_POLICY_MSG,
                         sizeof(DIAG_SUBSYS_CXM_SEND_POLICY_MSG_rsp_type));
  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);

} /* cxm_diag_send_policy_msg */

/*===========================================================================

  FUNCTION:  cxm_diag_stop_current_test


===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to stop sending test msg bytes to cxm_uart.

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_stop_current_test (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_CXM_STOP_CURRENT_TEST_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_STOP_CURRENT_TEST_rsp_type *temp_resp_ptr;
  /*-----------------------------------------------------------------------*/
  /* unfortunately this resp doesn't contain the correct cmd and subsys ids.
   * need to free this and make a new one. */
  temp_resp_ptr = (DIAG_SUBSYS_WCI2_WCI2_STOP_CURRENT_TEST_rsp_type *) 
    wci2_diag_stop_current_test( req_pkt, pkt_len );
  diagpkt_free( temp_resp_ptr );

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_STOP_CURRENT_TEST_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)STOP_CURRENT_TEST,
                         sizeof(DIAG_SUBSYS_CXM_STOP_CURRENT_TEST_rsp_type));
  CXM_ASSERT( NULL != rsp_ptr );

  return(rsp_ptr);
} /* cxm_diag_stop_current_test */

/*===========================================================================

  FUNCTION:  cxm_diag_enable_cxm_uart_loopback_mode

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to enable/disable loopback mode for CXM UART

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_enable_cxm_uart_loopback_mode (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_ENABLE_CXM_UART_LOOPBACK_MODE_rsp_type *rsp_ptr;
  DIAG_SUBSYS_CXM_ENABLE_CXM_UART_LOOPBACK_MODE_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_ENABLE_CXM_UART_LOOPBACK_MODE_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_CXM_ENABLE_CXM_UART_LOOPBACK_MODE_req_type*) req_pkt;

    /* Determine whether to enable or disable loopback mode */
    wci2_uart_set_loopback_mode( req_ptr->enable_uart_loopback_mode );
    CXM_MSG_1( LOW, "DIAG, Internal uart loopback mode set: %d",
               req_ptr->enable_uart_loopback_mode );
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_ENABLE_CXM_UART_LOOPBACK_MODE_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)ENABLE_CXM_UART_LOOPBACK_MODE,
                         sizeof(DIAG_SUBSYS_CXM_ENABLE_CXM_UART_LOOPBACK_MODE_rsp_type));
  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* cxm_diag_enable_cxm_uart_loopback_mode */

/*===========================================================================

  FUNCTION:  cxm_diag_start_originator_mode

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to send CXM_DIAG_BURST_LENGTH (4) bytes and confirm the same were
    received correctly

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_start_originator_mode (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_START_ORIGINATOR_MODE_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_START_ORIGINATOR_MODE_rsp_type *temp_resp_ptr;
  /*-----------------------------------------------------------------------*/
  /* unfortunately this resp doesn't contain the correct cmd and subsys ids.
   * need to free this and make a new one. */
  temp_resp_ptr = (DIAG_SUBSYS_WCI2_WCI2_START_ORIGINATOR_MODE_rsp_type*) 
    wci2_diag_start_originator_mode( req_pkt, pkt_len );

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_START_ORIGINATOR_MODE_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)START_ORIGINATOR_MODE,
                         sizeof(DIAG_SUBSYS_CXM_START_ORIGINATOR_MODE_rsp_type));
  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = temp_resp_ptr->error;
  diagpkt_free( temp_resp_ptr );

  return(rsp_ptr);
} /* cxm_diag_start_originator_mode */

/*===========================================================================

  FUNCTION:  cxm_diag_get_originator_mode_result

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to check results of previously-issued
    originator_mode command

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_get_originator_mode_result (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_GET_ORIGINATOR_MODE_RESULT_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_GET_ORIGINATOR_MODE_RESULT_rsp_type *temp_resp_ptr;
  /*-----------------------------------------------------------------------*/
  /* unfortunately this resp doesn't contain the correct cmd and subsys ids.
   * need to free this and make a new one. */
  temp_resp_ptr = (DIAG_SUBSYS_WCI2_WCI2_GET_ORIGINATOR_MODE_RESULT_rsp_type*) 
    wci2_diag_get_originator_mode_result( req_pkt, pkt_len );

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_GET_ORIGINATOR_MODE_RESULT_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)GET_ORIGINATOR_MODE_RESULT,
                         sizeof(DIAG_SUBSYS_CXM_GET_ORIGINATOR_MODE_RESULT_rsp_type));
  CXM_ASSERT( NULL != rsp_ptr );
  /* return results of previous test */
  rsp_ptr->num_run = temp_resp_ptr->num_run;
  rsp_ptr->num_failed = temp_resp_ptr->num_failed;
  rsp_ptr->error = temp_resp_ptr->error;
  diagpkt_free( temp_resp_ptr );

  return(rsp_ptr);
}

/*===========================================================================

  FUNCTION:  cxm_diag_start_receiver_mode

===========================================================================*/
/*!
  @brief
    start test mode where MDM acts in receiver mode, looping back data
    through the UART

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_start_receiver_mode (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_CXM_START_RECEIVER_MODE_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_START_RECEIVER_MODE_rsp_type *temp_resp_ptr;
  /*-----------------------------------------------------------------------*/
  /* unfortunately this resp doesn't contain the correct cmd and subsys ids.
   * need to free this and make a new one. */
  temp_resp_ptr = (DIAG_SUBSYS_WCI2_WCI2_START_RECEIVER_MODE_rsp_type*) 
    wci2_diag_start_receiver_mode( req_pkt, pkt_len );

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_START_RECEIVER_MODE_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)START_RECEIVER_MODE,
                         sizeof(DIAG_SUBSYS_CXM_START_RECEIVER_MODE_rsp_type));
  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = temp_resp_ptr->error;
  diagpkt_free( temp_resp_ptr );

  return(rsp_ptr);
} /* cxm_diag_start_receiver_mode */

/*===========================================================================

  FUNCTION:  cxm_diag_set_cxm_uart_baud_rate

===========================================================================*/
/*!
  @brief
    Provide way to set cxm_uart's baud rate through the diag interface

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_set_cxm_uart_baud_rate (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_CXM_SET_CXM_UART_BAUD_RATE_rsp_type *rsp_ptr;
  DIAG_SUBSYS_CXM_SET_CXM_UART_BAUD_RATE_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;

  /*-----------------------------------------------------------------------*/

  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_SET_CXM_UART_BAUD_RATE_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_CXM_SET_CXM_UART_BAUD_RATE_req_type *) req_pkt;

    /* args are checked in cxm_uart_set_baud */
    estatus = wci2_uart_set_baud( req_ptr->baud );
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_SET_CXM_UART_BAUD_RATE_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)SET_CXM_UART_BAUD_RATE,
                         sizeof(DIAG_SUBSYS_CXM_SET_CXM_UART_BAUD_RATE_rsp_type));

  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  /*-----------------------------------------------------------------------*/

  return rsp_ptr;
} /* cxm_diag_set_cxm_uart_baud_rate */

/*===========================================================================

  FUNCTION:  cxm_diag_enable_cxm

===========================================================================*/
/*!
  @brief
    Power up/down cxm_uart to begin/end DIAG testing. Some diag commands will
    not work if UART is powered down. Also stops any currently-running test

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_enable_cxm (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_ENABLE_CXM_rsp_type *rsp_ptr;
  DIAG_SUBSYS_CXM_ENABLE_CXM_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  mcpm_request_parms_type   mcpm_params;
  void                     *temp_resp_ptr;
  /*-----------------------------------------------------------------------*/
  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_ENABLE_CXM_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_CXM_ENABLE_CXM_req_type*) req_pkt;
    /*Initialize mcpm paramters*/
    memset( &mcpm_params, 0, sizeof( mcpm_request_parms_type ) );
    /* Determine whether to enable or disable loopback mode */
    if( req_ptr->enable_cxm )
    {
      npa_issue_required_request( cxm_diag_uart_client, WCI2_UART_NPA_REQ_ON );

      /*turn on the clocks required to do a direct GRFC write*/
      MCPM_Config_Modem(MCPM_RF_START_REQ, &mcpm_params);
    }
    else
    {
      /* stop any currently running tests */
      temp_resp_ptr = wci2_diag_stop_current_test( NULL, 0 );
      diagpkt_free( temp_resp_ptr );

      npa_issue_required_request( cxm_diag_uart_client, WCI2_UART_NPA_REQ_OFF );

      /*turn off the clocks required to do a direct GRFC write*/
      MCPM_Config_Modem(MCPM_RF_STOP_REQ, &mcpm_params);
    }
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_ENABLE_CXM_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)ENABLE_CXM,
                         sizeof(DIAG_SUBSYS_CXM_ENABLE_CXM_rsp_type));

  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* cxm_diag_enable_cxm */

/*===========================================================================

  FUNCTION:  cxm_diag_set_wwan_tech_state

===========================================================================*/
/*!
  @brief
    Allows testers to set various WWAN tech states to test COEX functionality

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_set_wwan_tech_state (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_SET_WWAN_TECH_STATE_rsp_type *rsp_ptr;
  DIAG_SUBSYS_CXM_SET_WWAN_TECH_STATE_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  cxm_wwan_tech_state_ind_s wwan_tech_state;
  uint8 cntr = 0;
  /*-----------------------------------------------------------------------*/
  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_SET_WWAN_TECH_STATE_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_CXM_SET_WWAN_TECH_STATE_req_type*) req_pkt;
    memset( &wwan_tech_state, 0, sizeof( cxm_wwan_tech_state_ind_s ) );

    wwan_tech_state.tech_id = (cxm_tech_type) req_ptr->tech_id;
    if( (0 == req_ptr->ul_freq || 0 == req_ptr->ul_bw) && (0 == req_ptr->dl_freq || 0 == req_ptr->dl_bw) )
    {
      wwan_tech_state.tech_data.num_link_info_sets = 0;
    }
    else
    {
      if( 0 != req_ptr->ul_freq && 0 != req_ptr->ul_bw )
      {
        wwan_tech_state.tech_data.link_list[cntr].frequency = (uint32)(req_ptr->ul_freq * 1000);
        wwan_tech_state.tech_data.link_list[cntr].bandwidth = (uint32)(req_ptr->ul_bw * 1000);
        wwan_tech_state.tech_data.link_list[cntr].band = 0;
        wwan_tech_state.tech_data.link_list[cntr].channel = 0; 
        wwan_tech_state.tech_data.link_list[cntr].direction = CXM_LNK_DRCTN_UL;
        cntr ++;
      }

      if( 0 != req_ptr->dl_freq && 0 != req_ptr->dl_bw )
      {
        wwan_tech_state.tech_data.link_list[cntr].frequency = (uint32)(req_ptr->dl_freq * 1000);
        wwan_tech_state.tech_data.link_list[cntr].bandwidth = (uint32)(req_ptr->dl_bw * 1000);
        wwan_tech_state.tech_data.link_list[cntr].band = 0;
        wwan_tech_state.tech_data.link_list[cntr].channel = 0; 
        wwan_tech_state.tech_data.link_list[cntr].direction = CXM_LNK_DRCTN_DL;
        cntr ++;
      }
      wwan_tech_state.tech_data.num_link_info_sets = cntr;

      if ( req_ptr->tdd != 0 )
      {
        wwan_tech_state.tech_data.params.lte_params.frame_structure = LTE_L1_FRAME_STRUCTURE_FS2;
      }
    }
  }

  estatus = cxm_msgr_send_msg( &wwan_tech_state.msg_hdr,
                               MCS_CXM_WWAN_TECH_STATE_IND,
                               sizeof(cxm_wwan_tech_state_ind_s) );

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_SET_WWAN_TECH_STATE_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)SET_WWAN_TECH_STATE,
                         sizeof(DIAG_SUBSYS_CXM_SET_WWAN_TECH_STATE_rsp_type));

  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* cxm_diag_set_wwan_tech_state */

/*===========================================================================

  FUNCTION:  cxm_diag_trigger_wcn_prio

===========================================================================*/
/*!
  @brief
    Fakes WCN Prio

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_trigger_wcn_prio (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_TRIGGER_WCN_PRIO_req_type *req_ptr;
  DIAG_SUBSYS_CXM_TRIGGER_WCN_PRIO_rsp_type *rsp_ptr;
  errno_enum_type estatus = E_SUCCESS;
  cxm_trigger_wcn_prio_ind_s msgr_msg;
  /*-----------------------------------------------------------------------*/
  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_TRIGGER_WCN_PRIO_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    req_ptr = (DIAG_SUBSYS_CXM_TRIGGER_WCN_PRIO_req_type*) req_pkt;
    memset( &msgr_msg, 0, sizeof( cxm_trigger_wcn_prio_ind_s ) );

  msgr_msg.l2vic_bit_pos = WCN_PRIO_L2VIC_POS;
  msgr_msg.assert_time = req_ptr->num_ms * MSEC_TO_USEC;

    estatus = cxm_msgr_send_msg( &msgr_msg.msg_hdr,
                                 MCS_CXM_COEX_TRIGGER_WCN_PRIO_IND,
                                 sizeof(cxm_trigger_wcn_prio_ind_s) );

    CXM_MSG_1(LOW, "Sent WCN prio trigger for %d usecs", msgr_msg.assert_time);
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_TRIGGER_WCN_PRIO_rsp_type *)
  diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)TRIGGER_WCN_PRIO,
                         sizeof(DIAG_SUBSYS_CXM_TRIGGER_WCN_PRIO_rsp_type));

  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* cxm_diag_trigger_wcn_prio */

/*===========================================================================

  FUNCTION:  cxm_diag_log_fw_counters

===========================================================================*/
/*!
  @brief
    Send 5 subframe notification messages to CXM with specified transmission time
    and TX mode.

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_log_fw_counters (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_LOG_FW_COUNTERS_rsp_type *rsp_ptr;
  DIAG_SUBSYS_CXM_LOG_FW_COUNTERS_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  cxm_reset_fw_counters_ind_s msgr_msg;
  /*-----------------------------------------------------------------------*/
  /*log all counters*/
  cxm_counter_log();

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_LOG_FW_COUNTERS_rsp_type *)
  diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)LOG_FW_COUNTERS,
                         sizeof(DIAG_SUBSYS_CXM_LOG_FW_COUNTERS_rsp_type));

  CXM_ASSERT( NULL != rsp_ptr );
  /*get counters for the response*/
  cxm_get_lte_counters(rsp_ptr->fw_counters, rsp_ptr->cxm_counters);
  rsp_ptr->cxm_counter_id = (uint16) CXM_COUNTER_ID;
  rsp_ptr->num_cxm_counters = (uint16) CXM_CNT_MAX_COUNTERS;
  rsp_ptr->lte_fw_counter_id = (uint16) CXM_LTE_FW_COUNTER_ID;
  rsp_ptr->num_lte_fw_counters = (uint16) CXM_CNT_MAX_FW_COUNTERS;

  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_LOG_FW_COUNTERS_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    req_ptr = (DIAG_SUBSYS_CXM_LOG_FW_COUNTERS_req_type*) req_pkt;
    if (req_ptr->reset)
    {
      /*reset all counters*/
      cxm_counter_reset(CXM_CNT_MAX_COUNTERS);

      msgr_msg.tech = CXM_TECH_LTE;
      msgr_msg.reset_mask = CXM_RESET_CNT_FRAME_SYNC | CXM_RESET_CNT_WCN_PRIO_BLANK_REQ |\
                             CXM_RESET_CNT_TX_ACTIVE | CXM_RESET_CNT_WCN_PRIO_BLANK_DENIED |\
                             CXM_RESET_CNT_RX_PRIO;
      cxm_msgr_send_msg( &msgr_msg.msg_hdr,
                          MCS_CXM_COEX_FW_COUNTERS_RESET_IND,
                          sizeof(cxm_reset_fw_counters_ind_s) );
    }
  }

  /*error code*/
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* cxm_diag_log_fw_counters */

/*===========================================================================

  FUNCTION:  cxm_diag_enable_rf_command

===========================================================================*/
/*!
  @brief
    Enables RF command app and intializes the task queue. Needed to send grfc
    commands when LTE is not active

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_enable_rf_command (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_ENABLE_RF_COMMAND_rsp_type *rsp_ptr;
  errno_enum_type estatus = E_SUCCESS;  
  /*-----------------------------------------------------------------------*/  
  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_ENABLE_RF_COMMAND_rsp_type *)
  diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)ENABLE_RF_COMMAND,
                         sizeof(DIAG_SUBSYS_CXM_ENABLE_RF_COMMAND_rsp_type));

  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* cxm_diag_enable_rf_command */

/*===========================================================================

  FUNCTION:  cxm_diag_send_subfr_notification

===========================================================================*/
/*!
  @brief
    Send 5 subframe notification messages to CXM with specified transmission time
    and TX mode.

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_send_subfr_notification (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_SEND_SUBFR_NOTIFICATION_rsp_type *rsp_ptr;
  DIAG_SUBSYS_CXM_SEND_SUBFR_NOTIFICATION_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  uint32 curr_ustmr;
  uint8 i;
  unsigned int time_apart;
  /*-----------------------------------------------------------------------*/
  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_SEND_SUBFR_NOTIFICATION_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_CXM_SEND_SUBFR_NOTIFICATION_req_type*) req_pkt;
    cxm_diag_msg_info.test = CXM_DIAG_SUBFR_NOTIFI;
    cxm_diag_subr_notifi_params.index = 0;
  
    /* set up coex subframe diag parameters */
    for (i=0; i<CXM_NUM_NOTIFI_SUBFRS; i++)
    {
      cxm_diag_subr_notifi_params.transmit[i] = req_ptr->subr_transmit[i];

      /*determine the time in ustmr of when we want to send this notification*/
      cxm_diag_subr_notifi_params.timing_ustmr[i] = COEX_SUB_USTMR(cxm_diag_subfr_notifi_timing_ustmr[i],
                                             (req_ptr->subr_adv_time[i] * COEX_CONVERT_100MICROSECS_USTMR) );
    }

    curr_ustmr = COEX_READ_USTMR_TIME();
    /*determine the duration in micros we want to wait before sending the first indication*/
    time_apart = ( COEX_SUB_USTMR(cxm_diag_subr_notifi_params.timing_ustmr[0], curr_ustmr) ) 
                 / 19.2;

    CXM_MSG_2( LOW, "cur_ustmr: %d, time apart: %d", 
               curr_ustmr, time_apart);
  
    /* start timing to the first subframe notification */
    timer_set( &cxm_diag_send_arb_msgs_timer,
               (timetick_type) time_apart,
               0,
               T_USEC );
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_SEND_SUBFR_NOTIFICATION_rsp_type *)
  diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)SEND_SUBFR_NOTIFICATION,
                         sizeof(DIAG_SUBSYS_CXM_SEND_SUBFR_NOTIFICATION_rsp_type));

  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* cxm_diag_send_subfr_notification */

/*===========================================================================

  FUNCTION:  cxm_diag_set_wci2_type7_filter_time

===========================================================================*/
/*!
  @brief
    Set the filter timeout for deciding whether or not to send the type 7.
    Also affects the delay between tech triggering TX change and type 7 
    being sent out modem.

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_set_wci2_type7_filter_time (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_SET_WCI2_TYPE7_FILTER_TIME_req_type *req_ptr;
  DIAG_SUBSYS_CXM_SET_WCI2_TYPE7_FILTER_TIME_rsp_type *rsp_ptr;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_SET_WCI2_TYPE7_FILTER_TIME_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    req_ptr = (DIAG_SUBSYS_CXM_SET_WCI2_TYPE7_FILTER_TIME_req_type*) req_pkt;

    CXM_MSG_2( MED, "Set WCI2 T7 filter time for event %d to %u ms",
               req_ptr->event,
               req_ptr->num_ms );
    coex_set_wci2_type7_filter_time( req_ptr->event, req_ptr->num_ms );
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_SET_WCI2_TYPE7_FILTER_TIME_rsp_type *)
  diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)SET_WCI2_TYPE7_FILTER_TIME,
                         sizeof(DIAG_SUBSYS_CXM_SET_WCI2_TYPE7_FILTER_TIME_rsp_type));
  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* cxm_diag_set_wci2_type7_filter_time */

/*===========================================================================

  FUNCTION:  cxm_diag_test_wcn_tx_driver

===========================================================================*/
/*!
  @brief
    Enable/disable/start/stop diag client for the purposes of testing
    the WCN TX sticky bit driver

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_test_wcn_tx_driver (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_TEST_WCN_TX_DRIVER_req_type *req_ptr;
  DIAG_SUBSYS_CXM_TEST_WCN_TX_DRIVER_rsp_type *rsp_ptr;
  cxm_wcn_tx_rsp_codes_e tx_e     = CXM_WCN_TX_RSP_SUCCESS;
  cxm_wcn_tx_state_e     tx_state = CXM_WCN_TX_STATE_INACTIVE;
  errno_enum_type        estatus  = E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_TEST_WCN_TX_DRIVER_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    req_ptr = (DIAG_SUBSYS_CXM_TEST_WCN_TX_DRIVER_req_type*) req_pkt;

    CXM_MSG_2( MED, "Diag testing TX sticky bit driver, client=%d event=%d",
               req_ptr->client, req_ptr->event );

    switch( (cxm_diag_tx_sticky_e) req_ptr->event )
    {
      case CXM_DIAG_TX_STICKY_ENABLE:
        tx_e = cxm_wcn_tx_enable( req_ptr->client );
        break;

      case CXM_DIAG_TX_STICKY_DISABLE:
        tx_e = cxm_wcn_tx_disable( req_ptr->client );
        break;

      case CXM_DIAG_TX_STICKY_START:
        tx_e = cxm_wcn_tx_start( req_ptr->client );
        break;

      case CXM_DIAG_TX_STICKY_STOP:
        tx_e = cxm_wcn_tx_stop( req_ptr->client, &tx_state );
        break;

      default:
        tx_e = CXM_WCN_TX_RSP_NOT_SUPPORTED;
        break;
    }
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_TEST_WCN_TX_DRIVER_rsp_type *)
  diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)TEST_WCN_TX_DRIVER,
                         sizeof(DIAG_SUBSYS_CXM_TEST_WCN_TX_DRIVER_rsp_type));
  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->tx_rsp_e = tx_e;
  rsp_ptr->tx_state = tx_state;
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* cxm_diag_test_wcn_tx_driver */

/*===========================================================================

  FUNCTION:  cxm_diag_set_tx_ant_sel

===========================================================================*/
/*!
  @brief
    Set the selection for the transmit (Tx) antenna in use
    This will trigger WCI2 type7 message over CXM UART

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) cxm_diag_set_tx_ant_sel (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  DIAG_SUBSYS_CXM_SET_TX_ANT_SEL_req_type *req_ptr;
  DIAG_SUBSYS_CXM_SET_TX_ANT_SEL_rsp_type *rsp_ptr;
  cxm_tx_ant_sel_e tx_ant_sel;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  /* before doing anything, validate size of request packet */
  if( pkt_len != sizeof(DIAG_SUBSYS_CXM_SET_TX_ANT_SEL_req_type) )
  {
    estatus = E_BAD_DATA;
  }
  else
  {
    req_ptr = (DIAG_SUBSYS_CXM_SET_TX_ANT_SEL_req_type*) req_pkt;

    CXM_MSG_1( MED, "Set WCI2 T7 Tx Ant Sel to %d",
               req_ptr->tx_ant_sel );

    tx_ant_sel= (req_ptr->tx_ant_sel == 0)? CXM_TX_ANT_SEL_X : CXM_TX_ANT_SEL_Y;
    cxm_set_tx_ant_sel( tx_ant_sel );
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_CXM_SET_TX_ANT_SEL_rsp_type *)
  diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_CXM,
                         (diagpkt_subsys_cmd_code_type)SET_TX_ANT_SEL,
                         sizeof(DIAG_SUBSYS_CXM_SET_TX_ANT_SEL_rsp_type));
  CXM_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* cxm_diag_set_tx_ant_sel */

