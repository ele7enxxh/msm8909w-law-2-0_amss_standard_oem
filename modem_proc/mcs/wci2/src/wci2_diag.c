/*!
  @file
  wci2_diag.c

  @brief
  API definitions for WCI2 UART's DIAG interface
*/
/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/src/wci2_diag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/21/14   btl     Diag cmds to test external wci2 client i/f
07/08/14   btl     Initial implementation

===========================================================================*/
/*==========================================================================

                        WCI2 DIAG USAGE INSTRUCTIONS

The general format of a Diag command is as follows:

  send_data 75 <Diag subsys cmd enum #> <command> 0 <args>

The command number comes from the enum wci2_diag_table_id_type below. The arguments 
are passed in bytestream format, low byte of word first. For example, if you 
want to pass (int16)4 and (int8)5 into cmd #1, the command would look 
something like this:

  send_data 75 106 0x01 0 0x04 0x00 0x05

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <stringl/stringl.h>
#include <diagpkt.h>
#include <diagcmd.h>
#include <timer.h>
#include <npa_resource.h>

#include "wci2.h"
#include "wci2_utils.h"
#include "wci2_uart.h"
#include "wci2_diag.h"
#include "cxm_utils.h"

/*===========================================================================

                        INTERNAL DEFINITIONS & TYPES

===========================================================================*/
#define WCI2_DIAG_TRY_TAKE_MUTEX() \
  atomic_compare_and_set( &wci2_diag_uart_mutex, 0, 1 )
#define WCI2_DIAG_RELEASE_MUTEX() atomic_set( &wci2_diag_uart_mutex, 0 )

#define WCI2_DIAG_UART_BUFF_LEN 128
#define WCI2_DIAG_BURST_LENGTH  4
#define WCI2_TYPE0 0
#define WCI2_DIAG_ARB_MSG_SEQ_MAX 3

#define WCI2_CLNT_EXT_RECV_SIG 0x01
#define WCI2_CLNT_EXT_SEND_SIG 0x02
#define WCI2_CLNT_EXT_KILL_SIG 0x04
#define WCI2_CLNT_EXT_STACK_SIZE 2048

typedef struct
{
  unsigned int length;
  const uint8 *bytes;
} wci2_diag_send_arb_msg_entry;

typedef enum
{
  WCI2_DIAG_IND_BYTE,
  WCI2_DIAG_FLOOD,
  WCI2_DIAG_ORIGINATOR,
  WCI2_DIAG_RECEIVER
} wci2_diag_test_type;

typedef struct
{
  unsigned int        count;
  unsigned int        length;
  unsigned int        time_apart;
  unsigned int        num_times;
  const uint8        *msg_ptr;
  wci2_diag_test_type test;
} wci2_diag_arb_msg_info_type;

typedef struct
{
  unsigned int num_failed;
  unsigned int num_run;
  unsigned int index;
  uint8        result[WCI2_DIAG_BURST_LENGTH];
} wci2_diag_test_result_type;

typedef struct
{
  rex_tcb_type      *tcb_ptr;
  uint8             *stack_ptr;
  wci2_client_mode_e mode;
  uint8              seq_n;
} wci2_diag_ext_s;

/* function prototype for timer callback */
void wci2_diag_send_arb_msgs_timer_handler( timer_cb_data_type data);

/*--------------------------------------------------------------------------
                          ENABLE UART

Power on/off the UART using NPA vote
--------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_ENABLE_UART)
  uint8           enable;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_ENABLE_UART)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                          SET UART MODE

Set the CXM_UART into WCI2/CXM mode or Normal/Basic mode.
--------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_SET_UART_MODE)
  uint8           enable_cxm;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_SET_UART_MODE)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         ENABLE CXM UART's LOOPBACK MODE

This interface provides a way for a test tool to enable/disable CXM UART's
loopback mode.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_SET_UART_LOOPBACK_MODE)
  boolean         enable_uart_loopback_mode;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_SET_UART_LOOPBACK_MODE)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         SET CXM UART BAUD RATE

Set the wci2_uart baud rate

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_SET_UART_BAUD_RATE )
  wci2_uart_baud_type_e baud;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_SET_UART_BAUD_RATE )
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         SET UART ADVANCED HW

Enable/disable the CXM UART advanced hw

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_SET_UART_ADV_HW )
  boolean enable_hw;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_SET_UART_ADV_HW )
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         SET UART REGISTERED WCI2 RX TYPES

Set the types SIO will notify us about upon WCI2 RX

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_SET_UART_REG_WCI2_RX)
  uint8 type_mask;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_SET_UART_REG_WCI2_RX)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         SET EXTERNAL CLIENT

Create/destroy task and client for testing external wci2 interface

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_SET_EXTERNAL_CLIENT)
  boolean create;
  uint8   mode;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_SET_EXTERNAL_CLIENT)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         EXTERNAL CLIENT SEND PACKET

Send a packet using the external client interface

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_EXT_CLIENT_SEND_PKT)
  uint8 seq_index;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_EXT_CLIENT_SEND_PKT)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         EXTERNAL CLIENT RECV PACKET

Receive a packet using the external client interface

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_EXT_CLIENT_RECV_PKT)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_EXT_CLIENT_RECV_PKT)
  errno_enum_type error;
DIAGPKT_RSP_END



/*===========================================================================


                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
static PACK(void *) wci2_diag_enable_uart(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) wci2_diag_set_uart_mode(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) wci2_diag_set_uart_loopback_mode(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) wci2_diag_set_wci2_uart_baud_rate (PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) wci2_diag_set_uart_adv_hw(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) wci2_diag_set_uart_registered_wci2_rx_types(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) wci2_diag_set_external_client(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) wci2_diag_ext_client_send_pkt(PACK(void*) req_pkt, uint16 pkt_len);

static PACK(void *) wci2_diag_ext_client_recv_pkt(PACK(void*) req_pkt, uint16 pkt_len);

/*===========================================================================

                        INTERNAL VARIABLES

===========================================================================*/
static const diagpkt_user_table_entry_type wci2_diag_tbl[] =
{
  /* start id, end id, callback function */

  {(word)WCI2_ENABLE_UART,
   (word)WCI2_ENABLE_UART,
   wci2_diag_enable_uart},
  {(word)WCI2_SEND_ARBITRARY_MSG_SEQ,
   (word)WCI2_SEND_ARBITRARY_MSG_SEQ,
   wci2_diag_send_arbitrary_wci2_msg_seq},
  {(word)WCI2_SEND_8_ARBITRARY_MSGS,
   (word)WCI2_SEND_8_ARBITRARY_MSGS,
   wci2_diag_send_8_arbitrary_wci2_msgs},
  {(word)WCI2_STOP_CURRENT_TEST,
   (word)WCI2_STOP_CURRENT_TEST,
   wci2_diag_stop_current_test},
  {(word)WCI2_SET_UART_LOOPBACK_MODE,
   (word)WCI2_SET_UART_LOOPBACK_MODE,
   wci2_diag_set_uart_loopback_mode},
  {(word)WCI2_START_ORIGINATOR_MODE,
   (word)WCI2_START_ORIGINATOR_MODE,
   wci2_diag_start_originator_mode},
  {(word)WCI2_GET_ORIGINATOR_MODE_RESULT,
   (word)WCI2_GET_ORIGINATOR_MODE_RESULT,
   wci2_diag_get_originator_mode_result},
  {(word)WCI2_START_RECEIVER_MODE,
   (word)WCI2_START_RECEIVER_MODE,
   wci2_diag_start_receiver_mode},
  {(word)WCI2_SET_UART_BAUD_RATE,
   (word)WCI2_SET_UART_BAUD_RATE,
   wci2_diag_set_wci2_uart_baud_rate},
  {(word)WCI2_SET_UART_MODE,
   (word)WCI2_SET_UART_MODE,
   wci2_diag_set_uart_mode},
  {(word)WCI2_SET_UART_ADV_HW,
   (word)WCI2_SET_UART_ADV_HW,
   wci2_diag_set_uart_adv_hw},
  {(word)WCI2_SET_UART_REG_WCI2_RX,
   (word)WCI2_SET_UART_REG_WCI2_RX,
   wci2_diag_set_uart_registered_wci2_rx_types},
  {(word)WCI2_SET_EXTERNAL_CLIENT,
   (word)WCI2_SET_EXTERNAL_CLIENT,
   wci2_diag_set_external_client},
  {(word)WCI2_EXT_CLIENT_SEND_PKT,
   (word)WCI2_EXT_CLIENT_SEND_PKT,
   wci2_diag_ext_client_send_pkt},
  {(word)WCI2_EXT_CLIENT_RECV_PKT,
   (word)WCI2_EXT_CLIENT_RECV_PKT,
   wci2_diag_ext_client_recv_pkt}
};

/* npa client for voting uart power state on/off */
STATIC npa_client_handle wci2_diag_uart_client;

/* resources for communication between wci2_diag_send* functions and
   their timer handlers */
static wci2_diag_arb_msg_info_type wci2_diag_msg_info;

/* used to tell wci2_diag_send...() functions to stop */
volatile static boolean wci2_diag_continue_test_flag;

/* store message to send from send_8_arbitrary_wci2_msgs */
static uint8 wci2_diag_send_8_arb_msgs_byte[8];

/* Msg sequences for wci2_diag_send_arbitrary_wci2_msg_seq.
   sequences used in the msg table below */
static const uint8 wci2_diag_seq0[] = { 
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
  0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,
  0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,
  0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
  0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31 
};
static const uint8 wci2_diag_seq1[] = { 
  0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF 
};
static const uint8 wci2_diag_seq2[] = {
  //0x00, /* type 0 - FRAME_SYNC[0], RX_PRIORITY[0], TX_ACTIVE[0] 0b00000000 */
  //0x08, /* type 0 - FRAME_SYNC[1], RX_PRIORITY[0], TX_ACTIVE[0] 0b00001000 */
  //0x10, /* type 0 - FRAME_SYNC[0], RX_PRIORITY[1], TX_ACTIVE[0] 0b00010000 */
  //0x18, /* type 0 - FRAME_SYNC[1], RX_PRIORITY[1], TX_ACTIVE[0] 0b00011000 */
  //0x20, /* type 0 - FRAME_SYNC[0], RX_PRIORITY[0], TX_ACTIVE[1] 0b00100000 */
  //0x28, /* type 0 - FRAME_SYNC[1], RX_PRIORITY[0], TX_ACTIVE[1] 0b00101000 */
  //0x30, /* type 0 - FRAME_SYNC[0], RX_PRIORITY[1], TX_ACTIVE[1] 0b00110000 */
  //0x38, /* type 0 - FRAME_SYNC[1], RX_PRIORITY[1], TX_ACTIVE[1] 0b00111000 */
  0x09, /* type 1 - request to resend type0 0b00001001 */
  0x01, /* type 1 - do not resend type0     0b00000001 */
  0x02, /* type 2 - lower nibble set to 0   0b00000010 */
  0xf2, /* type 2 - lower nibble set to f   0b11110010 */
  0x0a, /* type 2 - higher nibble set to 0  0b00001010 */
  0xfa, /* type 2 - higher nibble set to f  0b11111010 */
  0x03, /* type 3 - with no data            0b00000011 */
  0xfb, /* type 3 - with max duration       0b11111011 */
  0x06, /* type 6 - with no data            0b00000110 */
  0x26  /* type 6 - with SFN 4              0b00100110 */
};

/* hard-coded test cases for send_arbitrary_wci2_msg_seq */
static const wci2_diag_send_arb_msg_entry wci2_diag_send_arb_msg_table[WCI2_DIAG_ARB_MSG_SEQ_MAX] = 
{
  { sizeof(wci2_diag_seq0), wci2_diag_seq0 },
  { sizeof(wci2_diag_seq1), wci2_diag_seq1 },
  { sizeof(wci2_diag_seq2), wci2_diag_seq2 }
};

static timer_group_type wci2_nondeferrable_timer_group;
static timer_type wci2_diag_send_arb_msgs_timer;

static wci2_diag_test_result_type wci2_diag_test_result = {0, 0, 0, {0, 0, 0, 0}};

/* mutex to control use of the wci2 uart during diag tests */
static atomic_word_t wci2_diag_uart_mutex;

/* struct to control external wci2 client interface testing */
static wci2_diag_ext_s wci2_diag_ext;

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  wci2_diag_init

===========================================================================*/
/*!
  @brief
    To initialize WCI2's DIAG interface

  @return
    None
*/
/*=========================================================================*/
void wci2_diag_init( void )
{
  /*-----------------------------------------------------------------------*/
  DIAGPKT_DISPATCH_TABLE_REGISTER((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                                   wci2_diag_tbl); 

  /* Set up timer to send test wci2 msgs at specified interval */
  timer_group_set_deferrable( &wci2_nondeferrable_timer_group, FALSE );
  timer_group_enable( &wci2_nondeferrable_timer_group );
  timer_def( &wci2_diag_send_arb_msgs_timer, &wci2_nondeferrable_timer_group, 
             NULL, 0, wci2_diag_send_arb_msgs_timer_handler, 0 );

  /* initialize to zero */
  atomic_set( &wci2_diag_uart_mutex, 0 );

  /* create npa client for turning on/off uart */
  wci2_diag_uart_client = npa_create_sync_client(
                           "/modem/mcs/cxm_uart",
                           "wci2_diag",
                           NPA_CLIENT_REQUIRED );
  return;
}

/*===========================================================================

  FUNCTION:  wci2_diag_deinit

===========================================================================*/
/*!
  @brief
    To de-initialize WCI2's DIAG interface

  @return
    None
*/

/*=========================================================================*/
void wci2_diag_deinit( void )
{
  /*-----------------------------------------------------------------------*/
  timer_clr( &wci2_diag_send_arb_msgs_timer, T_USEC );
  wci2_uart_set_diag_rx( FALSE );
  return;
}

/*===========================================================================

  FUNCTION:  wci2_diag_send_arb_msgs_timer_handler

===========================================================================*/
/*!
  @brief
    timer handler to periodically send bytes for 
    wci2_diag_send_8_arbitrary_wci2_msgs()
    Loops until told to stop. From timer context.

  @return
    void
*/

/*=========================================================================*/
void wci2_diag_send_arb_msgs_timer_handler (
  timer_cb_data_type              data
)
{
  uint8 array[WCI2_DIAG_UART_BUFF_LEN];
  boolean recv_success = TRUE;
  int i;

  /*-----------------------------------------------------------------------*/

  /* check if space available in uart */
  if( !wci2_is_uart_tx_full() )
  {
    switch(wci2_diag_msg_info.test)
    {
      case WCI2_DIAG_FLOOD:
        /* fill buffer array, then send to uart */
        for(i = 0; i < WCI2_DIAG_UART_BUFF_LEN; i++)
        {
          array[i] = wci2_diag_msg_info.msg_ptr[wci2_diag_msg_info.count++];
          wci2_diag_msg_info.count = wci2_diag_msg_info.count % wci2_diag_msg_info.length;
        }

        wci2_uart_basic_send( array, WCI2_DIAG_UART_BUFF_LEN );
        break;

      case WCI2_DIAG_ORIGINATOR:
        /* check result of prev attempt */
        if( wci2_diag_test_result.num_run > 0 )
        {
          for(i = 0; i < WCI2_DIAG_BURST_LENGTH; i++)
          {
            if( wci2_diag_test_result.result[i] != wci2_diag_msg_info.msg_ptr[i] )
            {
              recv_success = FALSE;
            }
            wci2_diag_test_result.result[i] = 0; /* reset for next round */
          }
          if( !recv_success || wci2_diag_test_result.index != WCI2_DIAG_BURST_LENGTH )
          {
            wci2_diag_test_result.num_failed++;
          }
          wci2_diag_test_result.index = 0; /* reset for next round */
        }

        /* repeat test specified number of times */
        if( wci2_diag_test_result.num_run < wci2_diag_msg_info.num_times )
        {
          /* send array of WCI2_DIAG_BURST_LENGTH bytes at once */
          wci2_uart_basic_send( wci2_diag_msg_info.msg_ptr, wci2_diag_msg_info.length );
          wci2_diag_test_result.num_run++;
        }
        else
        {
          /* need to schedule one more to check results of final send */
          wci2_diag_continue_test_flag = FALSE;
        }
        break;

      case WCI2_DIAG_IND_BYTE:
        /* send one byte at a time */
        wci2_uart_basic_send( (uint8*)&wci2_diag_msg_info.msg_ptr[wci2_diag_msg_info.count], 1 );
        wci2_diag_msg_info.count++;
        wci2_diag_msg_info.count = wci2_diag_msg_info.count % wci2_diag_msg_info.length;
        break;

      default:
        /* unknown test */
        WCI2_ASSERT(0);
        break;
    }
  }

  /* keep sending until told to stop */
  if( !wci2_diag_continue_test_flag )
  {
    /* finished test; stop timer, flush buff, & release mutex */
    timer_clr(&wci2_diag_send_arb_msgs_timer, T_USEC);
    /* disable special diag rx */
    wci2_uart_set_diag_rx( FALSE );
    WCI2_DIAG_RELEASE_MUTEX();
    WCI2_MSG_1( MED, "DIAG, Finished test %d", wci2_diag_msg_info.test );
  }

  return;
}

/*===========================================================================

  FUNCTION:  wci2_diag_process_uart_rx_data

===========================================================================*/
/*!
  @brief
    routes data appropriately within wci2_diag

  @return
    TRUE if data was processed, FALSE if wci2_diag didn't want it
*/

/*=========================================================================*/
void wci2_diag_process_uart_rx_data (
  uint8 byte
)
{
  /*-----------------------------------------------------------------------*/
  WCI2_MSG_3( MED, "DIAG, RX byte 0x%x in test %d mutex %d",
              byte, wci2_diag_msg_info.test, 
              atomic_read(&wci2_diag_uart_mutex) );
  switch( wci2_diag_msg_info.test )
  {
    case WCI2_DIAG_ORIGINATOR:
      /* check index bounds */
      if(wci2_diag_test_result.index >= WCI2_DIAG_BURST_LENGTH)
      {
        /* caught by test result handler if index != BURST_LENGTH */
        wci2_diag_test_result.index++;
      }
      else
      {
        wci2_diag_test_result.result[wci2_diag_test_result.index++] = byte;
      }
      break;

    case WCI2_DIAG_RECEIVER:
      /* loop data back around */
      wci2_uart_basic_send( &byte, 1 );
      break;

    default:
      break;
  }
}

/*=============================================================================

                      DIAG COMMAND IMPLEMENTATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  wci2_diag_enable_uart

=============================================================================*/
/*!
    @brief
    Power on/off the UART using NPA vote

    @return
    void
*/
/*=========================================================================*/
static PACK(void *) wci2_diag_enable_uart(
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_ENABLE_UART_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_ENABLE_UART_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_ENABLE_UART_req_type) )
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_ENABLE_UART_req_type *) req_pkt;

    if( req_ptr->enable )
    {
      npa_issue_required_request( wci2_diag_uart_client, WCI2_UART_NPA_REQ_ON );
    }
    else
    {
      npa_issue_required_request( wci2_diag_uart_client, WCI2_UART_NPA_REQ_OFF );
    }
  }
  else
  {
    estatus = E_BAD_DATA;
  }
  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_ENABLE_UART_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_ENABLE_UART,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_ENABLE_UART_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return rsp_ptr;
} /* wci2_diag_enable_uart */

/*===========================================================================

  FUNCTION:  wci2_diag_send_arbitrary_wci2_msg_seq

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to send arbitrary stream of WCI2 byte message
    note: cannot run at same time as wci2_diag_send_8_arbitrary_wci2_msgs,
          and loops until signalled to stop

  @return
    DIAG_RSP_PKT pointer 
*/

/*=========================================================================*/
PACK(void *) wci2_diag_send_arbitrary_wci2_msg_seq (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  errno_enum_type estatus = E_SUCCESS;
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_SEND_ARBITRARY_MSG_SEQ_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_SEND_ARBITRARY_MSG_SEQ_req_type *req_ptr;
  const wci2_diag_send_arb_msg_entry *msg_seq;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_SEND_ARBITRARY_MSG_SEQ_req_type) )
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_SEND_ARBITRARY_MSG_SEQ_req_type *) req_pkt;

    if( req_ptr->seq_index < WCI2_DIAG_ARB_MSG_SEQ_MAX )
    {
      /* attempt to obtain mutex flag - only one use at a time allowed */
      if( WCI2_DIAG_TRY_TAKE_MUTEX() )
      {

        /* initialize info to send bytes in timer handler */
        msg_seq = &wci2_diag_send_arb_msg_table[req_ptr->seq_index];
        wci2_diag_msg_info.count      = 0;
        wci2_diag_msg_info.msg_ptr    = msg_seq->bytes;
        wci2_diag_msg_info.length     = msg_seq->length;
        wci2_diag_msg_info.time_apart = 1000 * req_ptr->time_apart_ms;
        wci2_diag_msg_info.test       = WCI2_DIAG_IND_BYTE;
        wci2_diag_continue_test_flag  = TRUE;

        WCI2_MSG_2( MED, "DIAG, Sending arb WCI2 msg seq, time_apart_ms=%i, seq=%i", 
                    req_ptr->time_apart_ms, req_ptr->seq_index );

        /* check if we are supposed to flood instead of send at interval */
        if(wci2_diag_msg_info.time_apart == 0)
        {
          /* send signal to wci2_task to flood uart */
          /* at 3Mbps, this is faster than uart can process */
          wci2_diag_msg_info.time_apart = 10 * WCI2_DIAG_UART_BUFF_LEN;
          wci2_diag_msg_info.test = WCI2_DIAG_FLOOD;
        }

        /* start periodic timer */
        timer_set( &wci2_diag_send_arb_msgs_timer,
                 (timetick_type) wci2_diag_msg_info.time_apart,
                 (timetick_type) wci2_diag_msg_info.time_apart,
                 T_USEC );
      }
      else
      {
        estatus = E_IN_PROGRESS;
      }
    }
    else
    {
      estatus = E_INVALID_ARG;
    }
  }
  else
  {
    estatus = E_BAD_DATA;
  }
  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_SEND_ARBITRARY_MSG_SEQ_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_SEND_ARBITRARY_MSG_SEQ,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_SEND_ARBITRARY_MSG_SEQ_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);

} /* wci2_diag_send_arbitrary_wci2_msg_seq */

/*===========================================================================

  FUNCTION:  wci2_diag_send_8_arbitrary_wci2_msgs

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to send tester provided 8 arbitrary WCI2 bytes
    note: cannot run at same time as wci2_diag_send_arbitrary_wci2_msg_seq,
          and loops until signalled to stop

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
PACK(void *) wci2_diag_send_8_arbitrary_wci2_msgs (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  errno_enum_type estatus = E_SUCCESS;
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_SEND_8_ARBITRARY_MSGS_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_SEND_8_ARBITRARY_MSGS_req_type *req_ptr;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_SEND_8_ARBITRARY_MSGS_req_type) )
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_SEND_8_ARBITRARY_MSGS_req_type *) req_pkt;

    /* attempt to obtain mutex flag - only one use at a time allowed */
    if( WCI2_DIAG_TRY_TAKE_MUTEX() )
    {
      /* initialize info to send msg bytes in handlers */
      wci2_diag_msg_info.count      = 0;
      wci2_diag_msg_info.length     = 8;
      wci2_diag_msg_info.msg_ptr    = wci2_diag_send_8_arb_msgs_byte;
      wci2_diag_msg_info.time_apart = 1000 * req_ptr->time_apart_ms;
      wci2_diag_msg_info.test       = WCI2_DIAG_IND_BYTE;
      wci2_diag_continue_test_flag  = TRUE;

      /* save bytes to send over uart */
      wci2_diag_send_8_arb_msgs_byte[0] = req_ptr->byte0;
      wci2_diag_send_8_arb_msgs_byte[1] = req_ptr->byte1;
      wci2_diag_send_8_arb_msgs_byte[2] = req_ptr->byte2;
      wci2_diag_send_8_arb_msgs_byte[3] = req_ptr->byte3;
      wci2_diag_send_8_arb_msgs_byte[4] = req_ptr->byte4;
      wci2_diag_send_8_arb_msgs_byte[5] = req_ptr->byte5;
      wci2_diag_send_8_arb_msgs_byte[6] = req_ptr->byte6;
      wci2_diag_send_8_arb_msgs_byte[7] = req_ptr->byte7;

      /* check if we are supposed to flood instead of send at interval */
      if(wci2_diag_msg_info.time_apart == 0)
      {
        /* send signal to wci2_task to flood wci2 uart */
        wci2_diag_msg_info.time_apart = 10 * WCI2_DIAG_UART_BUFF_LEN;
        wci2_diag_msg_info.test       = WCI2_DIAG_FLOOD;
      }

      /* start periodic timer */
      timer_set( &wci2_diag_send_arb_msgs_timer,
                 (timetick_type) wci2_diag_msg_info.time_apart,
                 (timetick_type) wci2_diag_msg_info.time_apart,
                 T_USEC );
    }
    else
    {
      /* compare unsuccessful -> mutex in use */
      estatus = E_IN_PROGRESS;
    }

    WCI2_MSG_2( MED, "DIAG, Sending 8 arb WCI2 msgs, time_apart_ms=%i retval=%d", 
                req_ptr->time_apart_ms, estatus );
  }
  else
  {
    estatus = E_BAD_DATA;
  }
  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_SEND_8_ARBITRARY_MSGS_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_SEND_8_ARBITRARY_MSGS,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_SEND_8_ARBITRARY_MSGS_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* wci2_diag_send_8_arbitrary_wci2_msgs */

/*===========================================================================

  FUNCTION:  wci2_diag_stop_current_test


===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to stop sending test msg bytes to wci2_uart.

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
PACK(void *) wci2_diag_stop_current_test (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_STOP_CURRENT_TEST_rsp_type *rsp_ptr;
  /*-----------------------------------------------------------------------*/
  WCI2_MSG_0( MED, "DIAG, Stopping current test" );

  if( (WCI2_DIAG_RECEIVER == wci2_diag_msg_info.test) )
  {
    WCI2_DIAG_RELEASE_MUTEX();
  }

  /* disable special diag rx */
  wci2_uart_set_diag_rx( FALSE );

  /* signal helper functions to stop sending messages */
  wci2_diag_continue_test_flag = FALSE;

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_STOP_CURRENT_TEST_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_STOP_CURRENT_TEST,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_STOP_CURRENT_TEST_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );

  return(rsp_ptr);
} /* wci2_diag_stop_current_test */

/*===========================================================================

  FUNCTION:  wci2_diag_set_uart_loopback_mode 

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to enable/disable loopback mode for WCI2 UART

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) wci2_diag_set_uart_loopback_mode (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  errno_enum_type estatus = E_SUCCESS;
  DIAG_SUBSYS_WCI2_WCI2_SET_UART_LOOPBACK_MODE_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_SET_UART_LOOPBACK_MODE_req_type *req_ptr;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_UART_LOOPBACK_MODE_req_type) )
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_UART_LOOPBACK_MODE_req_type*) req_pkt;

    /* Determine whether to enable or disable loopback mode */
    wci2_uart_set_loopback_mode( req_ptr->enable_uart_loopback_mode );
    WCI2_MSG_1( MED, "DIAG, Internal uart loopback mode set: %d",
                req_ptr->enable_uart_loopback_mode );
  }
  else
  {
    estatus = E_BAD_DATA;
  }
  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_UART_LOOPBACK_MODE_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_SET_UART_LOOPBACK_MODE,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_UART_LOOPBACK_MODE_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* wci2_diag_set_uart_loopback_mode */

/*===========================================================================

  FUNCTION:  wci2_diag_start_originator_mode

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to send WCI2_DIAG_BURST_LENGTH (4) bytes 
    and confirm the same were received correctly

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
PACK(void *) wci2_diag_start_originator_mode (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  errno_enum_type estatus = E_SUCCESS;
  DIAG_SUBSYS_WCI2_WCI2_START_ORIGINATOR_MODE_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_START_ORIGINATOR_MODE_req_type *req_ptr;
  uint8 byte;
  int i;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_START_ORIGINATOR_MODE_req_type) )
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_START_ORIGINATOR_MODE_req_type *) req_pkt;

    /* attempt to obtain mutex flag - only one use at a time allowed */
    if( WCI2_DIAG_TRY_TAKE_MUTEX() )
    {
      /* enable special diag rx */
      wci2_uart_set_diag_rx( TRUE );

      /* initialize test result info */
      wci2_diag_test_result.num_run    = 0;
      wci2_diag_test_result.num_failed = 0;
      wci2_diag_test_result.index      = 0;

      /* Set up timer handler to perform test */
      wci2_diag_msg_info.count      = 0;
      wci2_diag_msg_info.length     = WCI2_DIAG_BURST_LENGTH;
      wci2_diag_msg_info.msg_ptr    = wci2_diag_send_8_arb_msgs_byte;
      wci2_diag_msg_info.time_apart = 1000 * req_ptr->time_apart_ms;
      wci2_diag_msg_info.num_times  = req_ptr->num_times;
      wci2_diag_msg_info.test       = WCI2_DIAG_ORIGINATOR;
      wci2_diag_continue_test_flag  = TRUE;

      /* save bytes to send over uart */
      wci2_diag_send_8_arb_msgs_byte[0] = req_ptr->byte0;
      wci2_diag_send_8_arb_msgs_byte[1] = req_ptr->byte1;
      wci2_diag_send_8_arb_msgs_byte[2] = req_ptr->byte2;
      wci2_diag_send_8_arb_msgs_byte[3] = req_ptr->byte3;

      /* check args: */
      /* type 0 messages are not supported in loopback testing */
      for( i = 0; i < WCI2_DIAG_BURST_LENGTH; i++ )
      {
        byte = wci2_diag_send_8_arb_msgs_byte[i] & WCI2_TYPE_BITS_MASK;
        if( WCI2_TYPE0 == byte )
        {
          estatus = E_INVALID_ARG;
        }
      }

      if(wci2_diag_msg_info.time_apart == 0)
      {
        /* flooding not supported */
        estatus = E_NOT_ALLOWED;
      }

      if( E_SUCCESS == estatus )
      {
        /* start periodic timer */
        timer_set( &wci2_diag_send_arb_msgs_timer,
                 (timetick_type) wci2_diag_msg_info.time_apart,
                 (timetick_type) wci2_diag_msg_info.time_apart,
                 T_USEC );
      }
      else
      {
        /* invalid args - release mutex */
        WCI2_DIAG_RELEASE_MUTEX();
      }
    }
    else
    {
      /* compare unsuccessful -> mutex in use */
      estatus = E_IN_PROGRESS;
    }

    WCI2_MSG_3( MED, "DIAG, Starting originator mode; burst time apart=%d(ms), num times=%d retval=%d", 
                req_ptr->time_apart_ms, req_ptr->num_times, estatus );
  }
  else
  {
    estatus = E_BAD_DATA;
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_START_ORIGINATOR_MODE_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_START_ORIGINATOR_MODE,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_START_ORIGINATOR_MODE_rsp_type));

  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* wci2_diag_start_originator_mode */

/*===========================================================================

  FUNCTION:  wci2_diag_get_originator_mode_result

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to check results of previously-issued
    originator_mode command

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
PACK(void *) wci2_diag_get_originator_mode_result (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  errno_enum_type estatus = E_SUCCESS;
  DIAG_SUBSYS_WCI2_WCI2_GET_ORIGINATOR_MODE_RESULT_rsp_type *rsp_ptr;
  /*-----------------------------------------------------------------------*/
  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_GET_ORIGINATOR_MODE_RESULT_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_GET_ORIGINATOR_MODE_RESULT,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_GET_ORIGINATOR_MODE_RESULT_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  /* return results of previous test */
  rsp_ptr->num_run    = (uint16) wci2_diag_test_result.num_run;
  rsp_ptr->num_failed = (uint16) wci2_diag_test_result.num_failed;
  rsp_ptr->error = estatus;

  return(rsp_ptr);
}

/*===========================================================================

  FUNCTION:  wci2_diag_start_receiver_mode

===========================================================================*/
/*!
  @brief
    start test mode where MDM acts in receiver mode, looping back data
    through the UART

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
PACK(void *) wci2_diag_start_receiver_mode (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_START_RECEIVER_MODE_rsp_type *rsp_ptr;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  /* attempt to obtain mutex flag - only one use at a time allowed */
  if( WCI2_DIAG_TRY_TAKE_MUTEX() )
  {
    WCI2_MSG_0( MED, "DIAG, Starting receiver loopback test mode" );
    wci2_diag_msg_info.test = WCI2_DIAG_RECEIVER;

    /* enable special diag rx */
    wci2_uart_set_diag_rx( TRUE );
  }
  else
  {
    /* compare unsuccessful -> mutex in use */
    estatus = E_IN_PROGRESS;
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_START_RECEIVER_MODE_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_START_RECEIVER_MODE,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_START_RECEIVER_MODE_rsp_type));

  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return(rsp_ptr);
} /* wci2_diag_start_receiver_mode */

/*===========================================================================

  FUNCTION:  wci2_diag_set_wci2_uart_baud_rate

===========================================================================*/
/*!
  @brief
    Provide way to set wci2_uart's baud rate through the diag interface

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
static PACK(void *) wci2_diag_set_wci2_uart_baud_rate (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_SET_UART_BAUD_RATE_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_SET_UART_BAUD_RATE_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_UART_BAUD_RATE_req_type) )
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_UART_BAUD_RATE_req_type *) req_pkt;

    /* args are checked in wci2_uart_set_baud */
    estatus = wci2_uart_set_baud( req_ptr->baud );
  }
  else
  {
    estatus = E_BAD_DATA;
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_UART_BAUD_RATE_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_SET_UART_BAUD_RATE,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_UART_BAUD_RATE_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return rsp_ptr;
} /* wci2_diag_set_wci2_uart_baud_rate */

/*=============================================================================

  FUNCTION:  wci2_diag_set_uart_mode

=============================================================================*/
/*!
    @brief
    Set the UART into basic (normal mode) or WCI2/CXM mode.

    @return
    void
*/
/*===========================================================================*/
static PACK(void *) wci2_diag_set_uart_mode(
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_SET_UART_MODE_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_SET_UART_MODE_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_UART_MODE_req_type) )
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_UART_MODE_req_type *) req_pkt;

    wci2_uart_set_cxm_mode( req_ptr->enable_cxm );
  }
  else
  {
    estatus = E_BAD_DATA;
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_UART_MODE_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_SET_UART_MODE,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_UART_MODE_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return rsp_ptr;
} /* wci2_diag_set_uart_mode */

/*=============================================================================

  FUNCTION:  wci2_diag_set_uart_adv_hw

=============================================================================*/
/*!
    @brief
    Enable/diable advanced CXM_UART hardware (nibblization, direct reg rx)

    @return
    void
*/
/*===========================================================================*/
static PACK(void *) wci2_diag_set_uart_adv_hw(
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_SET_UART_ADV_HW_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_SET_UART_ADV_HW_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_UART_ADV_HW_req_type) )
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_UART_ADV_HW_req_type *) req_pkt;

    wci2_uart_set_adv_hw( req_ptr->enable_hw );
  }
  else
  {
    estatus = E_BAD_DATA;
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_UART_ADV_HW_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_SET_UART_ADV_HW,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_UART_ADV_HW_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return rsp_ptr;
} /* wci2_diag_set_uart_mode */

/*=============================================================================

  FUNCTION:  wci2_diag_set_uart_registered_wci2_rx_types

=============================================================================*/
/*!
    @brief
    Set which types WCI2 service will be notified about from SIO upon RX

    @return
    void
*/
/*===========================================================================*/
static PACK(void *) wci2_diag_set_uart_registered_wci2_rx_types(
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_SET_UART_REG_WCI2_RX_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_SET_UART_REG_WCI2_RX_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_UART_REG_WCI2_RX_req_type) )
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_UART_REG_WCI2_RX_req_type *) req_pkt;

    wci2_uart_set_wci2_rx_types( req_ptr->type_mask );
  }
  else
  {
    estatus = E_BAD_DATA;
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_UART_REG_WCI2_RX_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_SET_UART_REG_WCI2_RX,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_UART_REG_WCI2_RX_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return rsp_ptr;
}

/*=============================================================================

  FUNCTION:  wci2_diag_external_client_task

=============================================================================*/
/*!
    @brief
    Main function for task to receive/send for testing external wci2 client

    @return
    void
*/
/*===========================================================================*/
void wci2_diag_external_client_task( rex_task_func_param_type param )
{
  const wci2_diag_send_arb_msg_entry *msg_seq;
  rex_sigs_type  sigs = WCI2_CLNT_EXT_RECV_SIG | WCI2_CLNT_EXT_SEND_SIG | WCI2_CLNT_EXT_KILL_SIG;
  rex_sigs_type  rx_sigs;
  unsigned int   i;
  dsm_item_type *rx_basic;
  uint8         *rx_t2;
  uint16         rx_len;
  uint8          rx_seq_n;
  uint8          rx_chksm;
  int16          dsm_return;
  /*-----------------------------------------------------------------------*/
  WCI2_MSG_1( HIGH, "Ext Client test task init, mode=%d", wci2_diag_ext.mode );
  while( 1 )
  {
    rx_sigs = rex_wait( sigs );
    rex_clr_sigs( wci2_diag_ext.tcb_ptr, sigs);
    if( rx_sigs & WCI2_CLNT_EXT_RECV_SIG )
    {
      WCI2_MSG_1( HIGH, "Ext Client test task waiting on RX; mode=%d", 
                  wci2_diag_ext.mode );
      if( wci2_diag_ext.mode == WCI2_CLIENT_MODE_BASIC )
      {
        /* execute blocking receive call, free dsm packet when done */
        wci2_basic_client_recv_pkt( &rx_basic );
        dsm_return = dsm_pull8( &rx_basic );
        while( -1 != dsm_return )
        {
          WCI2_MSG_1( MED, "RX byte: 0x%x", (uint8)dsm_return );
          /* try to get next byte in prep for next loop iteration */
          dsm_return = dsm_pull8( &rx_basic );
        }
      }
      else
      {
        /* execute blocking receive call */
        wci2_type2_slip_client_recv_pkt( &rx_t2, &rx_len, &rx_seq_n, &rx_chksm );
        WCI2_MSG_3( HIGH, "Ext test RX SLIP: len=%d seq_n=%d chksm=%d", 
                     rx_len, rx_seq_n, rx_chksm );
        for( i = 0; i < rx_len; i++ )
        {
          WCI2_MSG_1( MED, "RX byte: 0x%x", rx_t2[i] );
        }
      }
    }
    if( rx_sigs & WCI2_CLNT_EXT_SEND_SIG )
    {
      /* send indicated arbitrary sequence either as SLIP t2 or basic */
      WCI2_MSG_2( HIGH, "Ext test task: sending seq %d mode=%d", 
                  wci2_diag_ext.seq_n, wci2_diag_ext.mode );
      msg_seq = &wci2_diag_send_arb_msg_table[wci2_diag_ext.seq_n];
      for( i = 0; i < msg_seq->length; i++ )
      {
        WCI2_MSG_1( MED, "TX 0x%x", msg_seq->bytes[i] );
      }
      if( wci2_diag_ext.mode == WCI2_CLIENT_MODE_BASIC )
      {
        wci2_basic_client_send_pkt( (void*)msg_seq->bytes, msg_seq->length );
      }
      else
      {
        wci2_type2_slip_client_send_pkt( (void*)msg_seq->bytes, msg_seq->length );
      }
    }
    if( rx_sigs & WCI2_CLNT_EXT_KILL_SIG )
    {
      WCI2_MSG_0( HIGH, "Killing ext client test thread" );
      break;
    }
  }

  return;
}

/*=============================================================================

  FUNCTION:  wci2_diag_set_external_client

=============================================================================*/
/*!
    @brief
    Set up external client for testing -- launch test task for send/recv, 
    kill CXM task; or teardown when done

    @return
    void
*/
/*===========================================================================*/
static PACK(void *) wci2_diag_set_external_client(
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_SET_EXTERNAL_CLIENT_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_SET_EXTERNAL_CLIENT_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  wci2_error_e retval = WCI2_E_SUCCESS;
  int rex_retval;
  rex_task_attributes_type rex_attr;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_EXTERNAL_CLIENT_req_type) )
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_EXTERNAL_CLIENT_req_type *) req_pkt;

    WCI2_MSG_2( HIGH, "External Client create=%d mode=%d", req_ptr->create, 
                req_ptr->mode );

    if( req_ptr->create && wci2_diag_ext.tcb_ptr == NULL )
    {
      /* if a client not already started, kill CXM (if basic), 
       * launch test task */
      wci2_diag_ext.mode = req_ptr->mode;
      if( req_ptr->mode == WCI2_CLIENT_MODE_BASIC )
      {
        /* kill CXM task, wait 2s for task teardown to complete */
        (void) rex_set_sigs( &cxm_tcb, CXM_TASK_STOP_SIG );
        rex_sleep(2000);
        retval = wci2_basic_client_create();
      }
      else
      {
        retval = wci2_type2_slip_client_create();
      }

      wci2_diag_ext.tcb_ptr = (rex_tcb_type*) WCI2_MEM_ALLOC( sizeof(rex_tcb_type) );
      WCI2_ASSERT( NULL != wci2_diag_ext.tcb_ptr );
      wci2_diag_ext.stack_ptr = (uint8*) WCI2_MEM_ALLOC( WCI2_CLNT_EXT_STACK_SIZE );
      WCI2_ASSERT( NULL != wci2_diag_ext.stack_ptr );
      rex_retval = rex_task_attr_init( &rex_attr );
      WCI2_ASSERT( rex_retval == REX_SUCCESS );
      rex_retval = rex_task_attr_set_task_name( &rex_attr, "wci2_tst" );
      WCI2_ASSERT( rex_retval == REX_SUCCESS );
      rex_retval = rex_task_attr_set_prio( &rex_attr, 100 );
      WCI2_ASSERT( rex_retval == REX_SUCCESS );
      rex_retval = rex_task_attr_set_stack_size( &rex_attr, WCI2_CLNT_EXT_STACK_SIZE );
      WCI2_ASSERT( rex_retval == REX_SUCCESS );
      rex_retval = rex_task_attr_set_stack_addr( &rex_attr, 
                                                 (rex_stack_pointer_type)wci2_diag_ext.stack_ptr );
      WCI2_ASSERT( rex_retval == REX_SUCCESS );
      rex_retval = rex_task_attr_set_entry_func( &rex_attr, 
         wci2_diag_external_client_task, 0 );
      WCI2_ASSERT( rex_retval == REX_SUCCESS );
      rex_retval = rex_common_def_task( wci2_diag_ext.tcb_ptr, &rex_attr );
      WCI2_ASSERT( rex_retval == REX_SUCCESS );
    }
    else if( !req_ptr->create && wci2_diag_ext.tcb_ptr != NULL )
    {
      /* if a client not already stopped, kill test task and tear down client */
      rex_set_sigs( wci2_diag_ext.tcb_ptr, WCI2_CLNT_EXT_KILL_SIG );
      rex_sleep(2000);
      WCI2_MEM_FREE( wci2_diag_ext.tcb_ptr );
      WCI2_MEM_FREE( wci2_diag_ext.stack_ptr );
      wci2_diag_ext.tcb_ptr = NULL;
      if( req_ptr->mode == WCI2_CLIENT_MODE_BASIC )
      {
        retval = wci2_basic_client_destroy();
      }
      else
      {
        retval = wci2_type2_slip_client_destroy();
      }
    }
    else
    {
      WCI2_MSG_0( ERROR, "External client already created/destroyed" );
      estatus = E_FAILURE;
    }

    if( retval != WCI2_E_SUCCESS )
    {
      WCI2_MSG_1( ERROR, "Create/destroy failed, retval=%d", retval );
      estatus = E_FAILURE;
    }
  }
  else
  {
    estatus = E_BAD_DATA;
  }
  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_SET_EXTERNAL_CLIENT_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_SET_EXTERNAL_CLIENT,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_SET_EXTERNAL_CLIENT_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return rsp_ptr;
}

/*=============================================================================

  FUNCTION:  wci2_diag_ext_client_send_pkt

=============================================================================*/
/*!
    @brief
    Set up external client for testing -- launch test task for send/recv, 
    kill CXM task; or teardown when done

    @return
    void
*/
/*===========================================================================*/
static PACK(void *) wci2_diag_ext_client_send_pkt(
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_SEND_PKT_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_SEND_PKT_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_SEND_PKT_req_type) )
  {
  
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_SEND_PKT_req_type *) req_pkt;

    if( wci2_diag_ext.tcb_ptr != NULL )
    {
      /* save seq number to send, then signal helper task to do the work */
      if( req_ptr->seq_index < WCI2_DIAG_ARB_MSG_SEQ_MAX )
      {
        wci2_diag_ext.seq_n = req_ptr->seq_index;
        rex_set_sigs( wci2_diag_ext.tcb_ptr, WCI2_CLNT_EXT_SEND_SIG );
      }
      else
      {
        estatus = E_INVALID_ARG;
      }
    }
    else
    {
      /* need to create the helper task and set up client first */
      WCI2_MSG_0( ERROR, "External client test task not running" );
      estatus = E_NOT_ALLOWED;
    }
  }
  else
  {
    estatus = E_BAD_DATA;
  }
  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_SEND_PKT_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_EXT_CLIENT_SEND_PKT,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_SEND_PKT_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return rsp_ptr;
}

/*=============================================================================

  FUNCTION:  wci2_diag_ext_client_recv_pkt

=============================================================================*/
/*!
    @brief
    Set up external client for testing -- launch test task for send/recv, 
    kill CXM task; or teardown when done

    @return
    void
*/
/*===========================================================================*/
static PACK(void *) wci2_diag_ext_client_recv_pkt(
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_RECV_PKT_rsp_type *rsp_ptr;
  DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_RECV_PKT_req_type *req_ptr;
  errno_enum_type estatus = E_SUCCESS;
  /*-----------------------------------------------------------------------*/

  if ( pkt_len == sizeof(DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_RECV_PKT_req_type) )
  {
    /* De-mystify req_pkt */
    req_ptr = (DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_RECV_PKT_req_type *) req_pkt;

    if( wci2_diag_ext.tcb_ptr != NULL )
    {
      /* signal helper task to do the work */
      rex_set_sigs( wci2_diag_ext.tcb_ptr, WCI2_CLNT_EXT_RECV_SIG );
    }
    else
    {
      /* need to create the helper task and set up client first */
      WCI2_MSG_0( ERROR, "External client test task not running" );
      estatus = E_NOT_ALLOWED;
    }
  }
  else
  {
    estatus = E_BAD_DATA;
  }
  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_RECV_PKT_rsp_type *)
    diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_WCI2,
                         (diagpkt_subsys_cmd_code_type)WCI2_EXT_CLIENT_RECV_PKT,
                         sizeof(DIAG_SUBSYS_WCI2_WCI2_EXT_CLIENT_RECV_PKT_rsp_type));
  WCI2_ASSERT( NULL != rsp_ptr );
  rsp_ptr->error = estatus;

  return rsp_ptr;
}


