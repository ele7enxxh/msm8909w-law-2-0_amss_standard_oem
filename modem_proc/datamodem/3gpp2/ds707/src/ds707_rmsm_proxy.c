/*===========================================================================

                          D S 7 0 7 _ R M S M _ P R O X Y . C

DESCRIPTION

  The Data Services RmSm Proxy'S main operations are to handle 1X Dial String
  Callbacks, set up the port bridge between the MDM and MSM, handle modem 
  control signals such as Data Terminal Ready(DTR), Carrier Detect(CD)
  signals and handle abort commands.

Copyright (c) 2001 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_rmsm.h_v   1.6   23 Jan 2003 16:34:28   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_rmsm_proxy.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
02/08/12    sk      Feature cleanup.
05/26/11    dvk     Global Variables Cleanup
01/07/11    vs     Changes to make dstask free-floating.
10/13/10    op     Migrated to MSG 2.0 macros
10/12/10    vs     Remove q_init for the watermark queue as it is already 
                   initialized inside dsm_queue_init.
05/21/10    vs     Initial File

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DSAT_MDM_FUSION
#define FEATURE_DSM_WM_CB
#include "ds707_rmsm_proxy.h"
#include "ds3gsiolib_ex.h"
#include "ds707_rmsm.h"
#include "ds707_pkt_mgr.h"
#include "sm_mgr.h"
#include "data_msg.h"




/*===========================================================================

                         INTERNAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Fwd and Rev Watermarks used for Packet mode data transfer
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_watermark_type      hsusb_fwd_wmk;
  q_type                  hsusb_fwd_wmk_q;

  dsm_watermark_type      hsusb_rev_wmk;
  q_type                  hsusb_rev_wmk_q;

  dsm_watermark_type      hsuart_fwd_wmk;
  q_type                  hsuart_fwd_wmk_q;

  dsm_watermark_type      hsuart_rev_wmk;
  q_type                  hsuart_rev_wmk_q;

}ds707_rmsm_proxy_info_type;

static ds707_rmsm_proxy_info_type ds707_rmsm_proxy_info;


/*---------------------------------------------------------------------------
  Enumeration of the ports with which RmSm proxy must register to receive 
  modem signals
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_RMSM_PROXY_MODEM_PORT = 0,
  DS707_RMSM_PROXY_FORWARDING_PORT = 1,
  DS707_RMSM_PROXY_MAX_PORT
} ds707_rmsm_proxy_port_type;

/*---------------------------------------------------------------------------
  Function table registered with siolib to manage Modem port 
  And Forwarding Port
---------------------------------------------------------------------------*/
static ds3g_siolib_func_tbl_type ds707_rmsm_proxy_sio_func_table 
                                [DS707_RMSM_PROXY_MAX_PORT];

/*---------------------------------------------------------------------------
  Function table registered with siolib to manage serial port
---------------------------------------------------------------------------*/
static ds3g_siolib_port_e_type forwarding_port_id  = DS3G_SIOLIB_PORT_NONE;

/*---------------------------------------------------------------------------
  The HSUSB SIO port allocated for external data call.
---------------------------------------------------------------------------*/
static ds3g_siolib_portid_e_type ds707_rmsm_proxy_hsusb_port_allocated =
                          DS3G_SIOLIB_PORTID_NONE;

/*---------------------------------------------------------------------------
  Handle to the RmSm Proxy State machine
---------------------------------------------------------------------------*/
static sm_handle_type ds707_rmsm_proxy_sm_handle = 0xFFFFFFFF;

/*---------------------------------------------------------------------------
  Enumeration of the different events in RmSm Proxy State machine
---------------------------------------------------------------------------*/
typedef enum
{
  BRIDGE_NULL_EV = 0,
  DIAL_STRING_RECEIVED_EV,
  BRIDGE_UP_EV,
  ERROR_EV,
  MAX_EV
} ds707_rmsm_proxy_events_type;

/*---------------------------------------------------------------------------
  Enumeration of the error scenarios in RmSm Proxy State machine
---------------------------------------------------------------------------*/
typedef enum
{
  NO_ERROR = 0,
  MODEM_PORT_ALLOCATION_FAILURE,
  FORWARDING_PORT_NOT_AVAILABLE,
  ABORT_CMD_RECEIVED,
  DSAT_ERROR_RECEIVED

} ds707_rmsm_proxy_err_type;

/*Number of entries in the NULL State transition table*/
#define NULL_STATE_NUM_ENTRIES 1

/*Number of entries in the WAITING_FOR_BRIDGE_UP_STATE
  State transition table*/
#define WAITING_FOR_BRIDGE_UP_STATE_NUM_ENTRIES 3

/*Number of entries in the BRIDGE_UP State
  transition table*/
#define BRIDGE_UP_STATE_NUM_ENTRIES 1

/*NULL State transition table*/
static sm_event_state_trans_type ds707_rmsm_proxy_null_state_trans_tbl
                                   [NULL_STATE_NUM_ENTRIES];

/*WAITING FOR BRIDGE UP State transition table*/
static sm_event_state_trans_type 
       ds707_rmsm_proxy_waiting_for_bridge_up_state_trans_tbl
                                  [WAITING_FOR_BRIDGE_UP_STATE_NUM_ENTRIES];

/*BRIDGE UP State transition table*/
static sm_event_state_trans_type ds707_rmsm_proxy_bridge_up_state_trans_tbl 
                                  [BRIDGE_UP_STATE_NUM_ENTRIES];

                               

/*===========================================================================

                         INTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Initialization functions for each state in the state machine. 
  Called on powerup.
---------------------------------------------------------------------------*/

static void ds707_rmsm_proxy_null_state_hdlr_init
(
  void
);

static void ds707_rmsm_proxy_waiting_for_bridge_up_state_hdlr_init
(
  void
);

static void ds707_rmsm_proxy_bridge_up_state_hdlr_init
(
  void
);

/*---------------------------------------------------------------------------
  NULL State Entry Handler Function
---------------------------------------------------------------------------*/
static void ds707_rmsm_proxy_null_entry_hdlr
(
  uint32 cb_data,
  void   *ev_data,
  uint32 event
);

/*---------------------------------------------------------------------------
  WAITING FOR BRIDGE UP State Entry Handler Function
---------------------------------------------------------------------------*/
static void ds707_rmsm_proxy_waiting_for_bridge_up_entry_hdlr
(
  uint32 cb_data,
  void   *ev_data,
  uint32 event
);

/*---------------------------------------------------------------------------
  BRIDGE UP State Entry Handler Function
---------------------------------------------------------------------------*/
static void ds707_rmsm_proxy_bridge_up_entry_hdlr
(
  uint32 cb_data,
  void   *ev_data,
  uint32 event
);

/*---------------------------------------------------------------------------
  BRIDGE UP State Exit Handler Function
---------------------------------------------------------------------------*/
static void ds707_rmsm_proxy_bridge_up_exit_hdlr
(
  uint32 cb_data,
  void   *ev_data,
  uint32 event
);


/*---------------------------------------------------------------------------
  RMSM PROXY callback functions registered with external modules
  to listen for Carrier Detect (CD) and Data Terminal Ready  (DTR) Signals
---------------------------------------------------------------------------*/

static void ds707_rmsm_proxy_dtr_changed_cb
(
  boolean dtr_status
);

static void ds707_rmsm_proxy_cd_changed_cb
(
  boolean cd_status
);

/*---------------------------------------------------------------------------
  SIO watermark configuration and event hanlding functions
---------------------------------------------------------------------------*/
static void ds707_rmsm_proxy_setup_hsusb_watermarks
(
  dsm_watermark_type *rx_wm_ptr,
  dsm_watermark_type *tx_wm_ptr
);

static void ds707_rmsm_proxy_hsusb_tx_wm_lo_cb
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
);

static void ds707_rmsm_proxy_hsusb_tx_wm_hi_cb
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
);

static void ds707_rmsm_proxy_hsusb_rx_wm_enqueue_cb
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
);

static void ds707_rmsm_proxy_setup_hsuart_watermarks
(
  dsm_watermark_type *rx_wm_ptr,
  dsm_watermark_type *tx_wm_ptr
);

static void ds707_rmsm_proxy_hsuart_tx_wm_lo_cb
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
);

static void ds707_rmsm_proxy_hsuart_tx_wm_hi_cb
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
);

static void ds707_rmsm_proxy_hsuart_rx_wm_enqueue_cb
(
  struct dsm_watermark_type_s *wm_ptr,
  void *user_data_ptr
);


/*===========================================================================
  This function is registered as abort handler with ATCOP. If ATCOP
  receives an AT cmd while processing an abortable cmd, this function gets
  called. 
===========================================================================*/
static void ds707_rmsm_proxy_call_abort_cb
(
  dsat_result_enum_type result_code
);

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_SM_INIT

  DESCRIPTION
    Gets the state machine handle and initializes the state transition table and  
    entry / exit handler functions for the various states.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_sm_init 
(
  void
);

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_DISABLE_INBOUND_FLOW

  DESCRIPTION
    This function disables the inbound flow on modem port and forwarding port.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_disable_inbound_flow
(
  void
);

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_ENABLE_INBOUND_FLOW

  DESCRIPTION
    This function enables the inbound flow on modem port and forwarding port.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_enable_inbound_flow
(
  void
);

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_INIT()

  DESCRIPTION
    This function initializes the RmSM Proxy State Machine and the 
    SIOLIB function table.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsm_proxy_init
(
  void
)

{
  uint8 i;

  ds3g_siolib_port_e_type port_id;

  ds707_rmsm_proxy_sm_init();

  ds707_rmsm_proxy_sio_func_table[DS707_RMSM_PROXY_MODEM_PORT]
             .dtr_changed_sig_handler = ds707_rmsm_proxy_dtr_changed_cb;

  ds707_rmsm_proxy_sio_func_table[DS707_RMSM_PROXY_MODEM_PORT]
             .dtr_changed_sig_handler_ampd0 = ds707_rmsm_proxy_dtr_changed_cb;
  
  ds707_rmsm_proxy_sio_func_table[DS707_RMSM_PROXY_FORWARDING_PORT]
             .cd_changed_sig_handler = ds707_rmsm_proxy_cd_changed_cb;


  for (i=0; i< DS707_RMSM_PROXY_MAX_PORT; i++)
  {
    ds707_rmsm_proxy_sio_func_table[i].at_escape_sequence_handler = NULL;
    ds707_rmsm_proxy_sio_func_table[i].at_return_to_online_data_complete_handler
                                                                  = NULL;
    ds707_rmsm_proxy_sio_func_table[i].at_return_to_online_data_handler = NULL;
    ds707_rmsm_proxy_sio_func_table[i].auto_answer_handler = NULL;
    ds707_rmsm_proxy_sio_func_table[i]
                     .return_to_online_cmd_mode_complete_handler = NULL;
  }

  for (port_id = DS3G_SIOLIB_HSUART1_PORT; port_id <= DS3G_SIOLIB_HSUART3_PORT; port_id ++) 
  {
    if (ds3g_siolib_dsr_assert(port_id) == DS3G_SIOLIB_NO_PORT_OPEN)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Error while calling DSR Assert");
      ASSERT(0);
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Initialize the watermarks and queues 
  -------------------------------------------------------------------------*/

  dsm_queue_init (&ds707_rmsm_proxy_info.hsusb_fwd_wmk,
                  (int)0xFFFFFFFF,
                  &ds707_rmsm_proxy_info.hsusb_fwd_wmk_q);
  ds707_rmsm_proxy_info.hsusb_fwd_wmk.current_cnt = 0;

  dsm_queue_init (&ds707_rmsm_proxy_info.hsusb_rev_wmk,
                  (int)0xFFFFFFFF,
                  &ds707_rmsm_proxy_info.hsusb_rev_wmk_q);
  ds707_rmsm_proxy_info.hsusb_rev_wmk.current_cnt = 0;

  dsm_queue_init (&ds707_rmsm_proxy_info.hsuart_fwd_wmk,
                  (int)0xFFFFFFFF,
                  &ds707_rmsm_proxy_info.hsuart_fwd_wmk_q);
  ds707_rmsm_proxy_info.hsuart_fwd_wmk.current_cnt = 0;

  dsm_queue_init (&ds707_rmsm_proxy_info.hsuart_rev_wmk,
                  (int)0xFFFFFFFF,
                  &ds707_rmsm_proxy_info.hsuart_rev_wmk_q);
  ds707_rmsm_proxy_info.hsuart_rev_wmk.current_cnt = 0;

}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_NULL_STATE_HDLR_INIT

  DESCRIPTION
    Sets the state transition table and entry/exit handler functions 
    for the NULL state. 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/

static void ds707_rmsm_proxy_null_state_hdlr_init
(
  void
)
{
  ds707_rmsm_proxy_null_state_trans_tbl[0].event =
                                   (uint32)DIAL_STRING_RECEIVED_EV;
  ds707_rmsm_proxy_null_state_trans_tbl[0].next_state = 
                                   (uint32)WAITING_FOR_BRIDGE_UP_STATE;

  sm_set_state_hdlr(SM_TYPE_RMSM_PROXY, 
                    (uint32)NULL_STATE, 
                    ds707_rmsm_proxy_null_state_trans_tbl,
                    (uint32)NULL_STATE_NUM_ENTRIES,
                    ds707_rmsm_proxy_null_entry_hdlr,
                    NULL,
                    NULL
                    );

}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_WAITING_FOR_BRIDGE_UP_STATE_HDLR_INIT

  DESCRIPTION
    Sets the state transition table and entry/exit handler functions 
    for the WAITING_FOR_BRIDGE_UP state. 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_waiting_for_bridge_up_state_hdlr_init
(
  void
)
{
  ds707_rmsm_proxy_waiting_for_bridge_up_state_trans_tbl[0].event = 
                                    (uint32)BRIDGE_NULL_EV;
  ds707_rmsm_proxy_waiting_for_bridge_up_state_trans_tbl[0].next_state =
                                    (uint32)NULL_STATE;

  ds707_rmsm_proxy_waiting_for_bridge_up_state_trans_tbl[1].event = 
                                    (uint32)ERROR_EV;
  ds707_rmsm_proxy_waiting_for_bridge_up_state_trans_tbl[1].next_state = 
                                    (uint32)NULL_STATE;

  ds707_rmsm_proxy_waiting_for_bridge_up_state_trans_tbl[2].event = 
                                    (uint32)BRIDGE_UP_EV;
  ds707_rmsm_proxy_waiting_for_bridge_up_state_trans_tbl[2].next_state = 
                                    (uint32)BRIDGE_UP_STATE;

  sm_set_state_hdlr(SM_TYPE_RMSM_PROXY, 
                    (uint32)WAITING_FOR_BRIDGE_UP_STATE, 
                    ds707_rmsm_proxy_waiting_for_bridge_up_state_trans_tbl,
                    (uint32)WAITING_FOR_BRIDGE_UP_STATE_NUM_ENTRIES,
                    ds707_rmsm_proxy_waiting_for_bridge_up_entry_hdlr,
                    NULL,
                    NULL
                    );

}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_BRIDGE_UP_STATE_HDLR_INIT

  DESCRIPTION
    Sets the state transition table and entry/exit handler functions 
    for the BRIDGE_UP state. 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_bridge_up_state_hdlr_init
(
  void
)
{
  ds707_rmsm_proxy_bridge_up_state_trans_tbl[0].event = (uint32)BRIDGE_NULL_EV;
  ds707_rmsm_proxy_bridge_up_state_trans_tbl[0].next_state =(uint32)NULL_STATE;

  sm_set_state_hdlr(SM_TYPE_RMSM_PROXY, 
                    (uint32)BRIDGE_UP_STATE, 
                    ds707_rmsm_proxy_bridge_up_state_trans_tbl,
                    (uint32)BRIDGE_UP_STATE_NUM_ENTRIES,
                    ds707_rmsm_proxy_bridge_up_entry_hdlr,
                    ds707_rmsm_proxy_bridge_up_exit_hdlr,
                    NULL
                    );
}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_SM_INIT

  DESCRIPTION
    Gets the state machine handle and initializes the state transition table and  
    entry / exit handler functions for the various states.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_sm_init 
(
  void
)
{
  
  if (sm_get_handle( SM_TYPE_RMSM_PROXY, 
                     (uint32)MAX_STATE, 
                     SM_INVALID_CB_DATA, 
                     &ds707_rmsm_proxy_sm_handle) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Couldnt obtain an SM Instance Handle");
    ASSERT(0);
    return;
  }

  ds707_rmsm_proxy_null_state_hdlr_init();
  ds707_rmsm_proxy_waiting_for_bridge_up_state_hdlr_init();
  ds707_rmsm_proxy_bridge_up_state_hdlr_init();
}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_DIAL_STR_CB()

  DESCRIPTION
    This function would be called by ATCOP module when it determines that 
    ATD#777 needs to be sent over to 7x30. This function will post a
    post a DIAL_STRING_RECEIVED_EV. If an error occurs while processing
    the DIAL_STRING_RECEIVED_EV, an ERROR_EV would be posted.

  PARAMETERS
    dial_string: The dial string
    digit_mode: Flag to tell whether dial string contains digits only

  RETURN VALUE
    Abort handler callback function.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_proxy_call_abort_cb_type  ds707_rmsm_proxy_dial_str_cb
(
  const byte                      *dial_string,
  boolean                          digit_mode,
  dsat_dial_modifier_info_type    *modifiers
)
{
  ds707_rmsm_proxy_err_type err_no = NO_ERROR;

  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Posting DIAL_STRING_RECEIVED_EV");
  sm_post_event(ds707_rmsm_proxy_sm_handle, 
                (uint32)DIAL_STRING_RECEIVED_EV, 
                &err_no);

  if (err_no != NO_ERROR) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Error while processing "
                    "DIAL_STRING_RECEIVED_EV.Posting ERROR_EV");

    sm_set_cb_data(ds707_rmsm_proxy_sm_handle,
                  (uint32)err_no);
    sm_post_event(ds707_rmsm_proxy_sm_handle, 
                 (uint32)ERROR_EV, 
                  NULL);
    return NULL;
  }

  else
  {
    return (ds707_rmsm_proxy_call_abort_cb);
  }
}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_CALL_ABORT_CB()

  DESCRIPTION
    This function is registered as abort handler with ATCOP. If ATCOP
    receives an AT cmd while processing an abortable cmd, this function gets
    called.  It is registered along with the dial string handler. It posts
    BRIDGE_NULL_EV to RMSM Proxy.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_call_abort_cb
(
  dsat_result_enum_type result_code
)
{

  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                  "Call abort callback. result_code = %d", result_code);
   
  if (result_code == DSAT_NO_CARRIER) 
  {
    sm_set_cb_data(ds707_rmsm_proxy_sm_handle,
                     (uint32)ABORT_CMD_RECEIVED);
    sm_post_event(ds707_rmsm_proxy_sm_handle, 
                 (uint32)ERROR_EV, 
                  NULL);
  }

  else if (result_code == DSAT_ERROR) 
  {
    sm_set_cb_data(ds707_rmsm_proxy_sm_handle,
                     (uint32)DSAT_ERROR_RECEIVED);
    sm_post_event(ds707_rmsm_proxy_sm_handle, 
                 (uint32)ERROR_EV, 
                  NULL);
  }
  else
  {
    ASSERT(0);
  }
}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_ATD_CONNECT_CB()

  DESCRIPTION
    This function is called by ATCOP module when it receives a DSAT_CONNECT
    result code from 7x30 side. This function would now post a BRIDGE_UP_EV.

  PARAMETERS

  RETURN VALUE
    Abort handler callback.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsm_proxy_atd_connect_cb
(
  void
)
{
  ds707_rmsm_proxy_err_type err_no = NO_ERROR;
  DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                  "ATD Connect Callback. Posting BRIDGE_UP_EV");
  sm_post_event(ds707_rmsm_proxy_sm_handle, 
                (uint32)BRIDGE_UP_EV, 
                &err_no);

  if (err_no != NO_ERROR) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "Error while processing BRIDGE_UP_EV.Posting ERROR_EV");

    sm_set_cb_data(ds707_rmsm_proxy_sm_handle,
                   (uint32)err_no);

    sm_post_event(ds707_rmsm_proxy_sm_handle, 
                 (uint32)ERROR_EV, 
                  NULL);
    return;
  }
}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_DTR_CHANGED_CB()

  DESCRIPTION
    This function processes the change in dtr state.  If DTR is deasserted
    RMSM Proxy forwards the signal to 7x30 by deasserting DSR signal on HSUART 
    port. This would initiate call termination on 7x30.
    DTR Assert is ignored.

  PARAMETERS
    is_asserted: flag which indicates current DTR state -
                 TRUE: DTR asserted, FALSE: DTR deasserted

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void ds707_rmsm_proxy_dtr_changed_cb
(
  boolean dtr_status
)
{
  if (dtr_status == FALSE) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"ds707_rmsm_proxy_dtr_changed_cb called");
    (void)ds3g_siolib_dsr_deassert(forwarding_port_id);
  }
}


/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_CD_CHANGED_CB()

  DESCRIPTION
    This function processes the change in CD state.  If CD is deasserted
    RmSm proxy posts a BRIDGE_NULL_EV. Other CD signals are ignored.

  PARAMETERS
    is_asserted: flag which indicates current CD state -
                 TRUE: CD asserted, FALSE: CD deasserted

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void ds707_rmsm_proxy_cd_changed_cb
(
  boolean cd_status
)
{
  if (cd_status == FALSE) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "CD DEASSERT Received. Posting BRIDGE_NULL_EV");
    sm_post_event(ds707_rmsm_proxy_sm_handle, 
                 (uint32)BRIDGE_NULL_EV, 
                 NULL);
  }
}

/*===========================================================================
  FUNCTION DS707_RMSM_WAITING_FOR_BRIDGE_UP_ENTRY_HDLR()

  DESCRIPTION
    This function is called while entering the WAITING_FOR_BRIDGE_UP state. 
    It allocates the Modem Port (between TE and MDM), fetches the forwarding port 
    (between MDM and MSM) and sets the state of the modem port to indicate 
    that a call is coming up.

  PARAMETERS
    cb_data - CALLBACK Data
    ev_data - EVENT Data. This is used to indicate errors while processing, 
              if any.
    event   - Event which caused the state transition.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_waiting_for_bridge_up_entry_hdlr
(
  uint32 cb_data,
  void   *ev_data,
  uint32 event
)
{

  ds707_rmsm_proxy_err_type *err_no = (ds707_rmsm_proxy_err_type *)ev_data;

  ds3g_siolib_port_alloc_info_s_type alloc_info; /* SIOLIB port allocation */

  /*Allocate the HSUSB SIOLIB port. Return NULL on failure*/
  memset( (void*)&alloc_info,
          0x0,
          sizeof(ds3g_siolib_port_alloc_info_s_type));

  alloc_info.mode = DS3G_SIOLIB_PORT_ALLOC_MODE_AUTOMATIC;
  alloc_info.call_dir = DS3G_SIOLIB_PORT_CALL_DIR_ORIGINATED;
  
  if(DS3G_SIOLIB_PORTID_NONE ==
       (ds707_rmsm_proxy_hsusb_port_allocated =
        ds3g_siolib_allocate_port( &alloc_info )) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Unable to allocate HSUSB port");
    *err_no = MODEM_PORT_ALLOCATION_FAILURE;
    return;
  }

  /*Set the state information on SIOLIB ports to indicate a call coming up*/
  ds3g_siolib_set_call_coming_up( TRUE );

  *err_no = NO_ERROR;
  return;
}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_BRIDGE_UP_ENTRY_HDLR()

  DESCRIPTION
    This function is called while entering the BRIDGE_UP state. 
    It registers the SIO Function tables to listen for DTR, CD signals,
    changes the mode to PKT, setups up appropriate watermarks, sends the result
    code to TE and asserts the CD signal on modem port. 

  PARAMETERS
    cb_data - CALLBACK Data
    ev_data - EVENT Data. This is used to indicate errors while processing, 
              if any.
    event   - Event which caused the state transition.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_bridge_up_entry_hdlr
(
  uint32 cb_data,
  void   *ev_data,
  uint32 event
)
{
  ds707_extif_bearer_tech_rate_type bearer_tech_query;
  dsat_connect_rate_s_type connect_rate; /* reporting connect rate */
  boolean dtr_status = TRUE;

  ds707_rmsm_proxy_err_type *err_no = (ds707_rmsm_proxy_err_type *)ev_data;

  memset((void *)&connect_rate,0x0, sizeof(dsat_connect_rate_s_type));


  /*Obtain the port on which the dial string needs to be forwarded */
  if (DS3G_SIOLIB_PORT_NONE == 
     (forwarding_port_id = ds3g_siolib_get_forwarding_port(
                           ds3g_siolib_get_ds3g_siolib_modem_port())))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Forwarding port cannot be DS3G_SIOLIB_PORT_NONE");
    ASSERT(0);
    *err_no = FORWARDING_PORT_NOT_AVAILABLE;
    return;
  }

  (void)ds3g_siolib_ex_register_callback_func(
               &ds707_rmsm_proxy_sio_func_table[DS707_RMSM_PROXY_MODEM_PORT],
               ds3g_siolib_get_ds3g_siolib_modem_port());

  (void)ds3g_siolib_ex_register_callback_func(
           &ds707_rmsm_proxy_sio_func_table[DS707_RMSM_PROXY_FORWARDING_PORT],
            forwarding_port_id);


  ds707_rmsm_proxy_setup_hsusb_watermarks(&ds707_rmsm_proxy_info.hsusb_rev_wmk, 
                                          &ds707_rmsm_proxy_info.hsusb_fwd_wmk);

  ds707_rmsm_proxy_setup_hsuart_watermarks(&ds707_rmsm_proxy_info.hsuart_fwd_wmk, 
                                          &ds707_rmsm_proxy_info.hsuart_rev_wmk);


  ds3g_siolib_change_forwarding_port_state(forwarding_port_id,
                                        DS3G_SIOLIB_FORWARDING_PORT_PKT_STATE);

 (void)ds3g_siolib_ex_change_mode( SIO_DS_PKT_MODE,
       &ds707_rmsm_proxy_info.hsusb_rev_wmk,
       &ds707_rmsm_proxy_info.hsusb_fwd_wmk,
       NULL, ds3g_siolib_get_ds3g_siolib_modem_port() );

 (void)ds3g_siolib_ex_change_mode( SIO_DS_PKT_MODE,
       &ds707_rmsm_proxy_info.hsuart_fwd_wmk,
       &ds707_rmsm_proxy_info.hsuart_rev_wmk,
       NULL, forwarding_port_id );


  bearer_tech_query.bearer_tech = DS707_EXTIF_1X;

  connect_rate.call_type = DSAT_CALL_TYPE_1X;
  (void)ds707_extif_get_bearer_tech_max_rate( &bearer_tech_query );
  connect_rate.rate.cdma_call.dl_rate = 
                               bearer_tech_query.max_rx_bearer_tech_rate;
  connect_rate.rate.cdma_call.ul_rate = 
                               bearer_tech_query.max_tx_bearer_tech_rate;

  /* Currently, not updating the protocol field as it is of no use. */
  dsat_report_rate_on_connect(connect_rate);
  dsat_send_result_code( DSAT_CONNECT );
  (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_ON );

  (void)ds3g_siolib_is_dtr_asserted(&dtr_status);

  if (dtr_status == TRUE) 
  {
    (void) ds3g_siolib_dsr_assert(forwarding_port_id);
  }
  else if (dtr_status == FALSE) 
  {
    (void) ds3g_siolib_dsr_deassert(forwarding_port_id);
  }

  *err_no = NO_ERROR;
  return;
}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_BRIDGE_UP_EXIT_HDLR()

  DESCRIPTION
    This function is called while leaving the BRIDGE_UP state. 
    It deregisters the SIO Function tables, changes the mode to AUTODETECT,
    switches to AUTODETECT watermarks and makes the Forwarding port available
    again.

  PARAMETERS
    cb_data - CALLBACK Data
    ev_data - EVENT Data. This is used to indicate errors while processing, 
              if any.
    event   - Event which caused the state transition.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_bridge_up_exit_hdlr
(
  uint32 cb_data,
  void   *ev_data,
  uint32 event
)
{

  (void)ds3g_siolib_dsr_assert(forwarding_port_id);
  ds707_rmsm_proxy_disable_inbound_flow();

  (void) ds3g_siolib_ex_change_mode( SIO_DS_AUTODETECT_MODE,
                                     NULL,
                                     NULL,
                                     NULL,
                                     ds3g_siolib_get_ds3g_siolib_modem_port());

  (void) ds3g_siolib_ex_change_mode( SIO_DS_AUTODETECT_MODE,
                                     NULL,
                                     NULL,
                                     NULL,
                                     forwarding_port_id);

  dsm_empty_queue(&ds707_rmsm_proxy_info.hsusb_fwd_wmk);
  dsm_empty_queue(&ds707_rmsm_proxy_info.hsusb_rev_wmk);
  dsm_empty_queue(&ds707_rmsm_proxy_info.hsuart_fwd_wmk);
  dsm_empty_queue(&ds707_rmsm_proxy_info.hsuart_rev_wmk);

  if(ds707_rmsm_proxy_hsusb_port_allocated != DS3G_SIOLIB_PORTID_NONE)
  {
    (void) ds3g_siolib_ex_deregister_callback_func(
              &ds707_rmsm_proxy_sio_func_table[DS707_RMSM_PROXY_MODEM_PORT],
              ds3g_siolib_get_ds3g_siolib_modem_port());

  }

  if (forwarding_port_id != DS3G_SIOLIB_PORT_NONE ) 
  {
    (void) ds3g_siolib_ex_deregister_callback_func
       (&ds707_rmsm_proxy_sio_func_table[DS707_RMSM_PROXY_FORWARDING_PORT],
       forwarding_port_id);
  }

}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_NULL_ENTRY_HDLR()

  DESCRIPTION
    This function is called while entering the NULL state. 
    It releases the modem port and forwarding port and Deasserts the CD signal.

  PARAMETERS
    cb_data - CALLBACK Data
    ev_data - EVENT Data. This is used to indicate errors while processing, 
              if any.
    event   - Event which caused the state transition.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_null_entry_hdlr
(
  uint32 cb_data,
  void   *ev_data,
  uint32 event
)
{

  if(ds707_rmsm_proxy_hsusb_port_allocated == DS3G_SIOLIB_PORTID_NONE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Modem port was never allocated");
    ASSERT(0);
    return;
  }
  
  if (event == BRIDGE_NULL_EV) 
  {
    (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
    ds3g_siolib_set_call_coming_up( FALSE );

    ds707_rmsm_proxy_enable_inbound_flow();

    ds3g_siolib_release_port ( ds707_rmsm_proxy_hsusb_port_allocated );
    ds707_rmsm_proxy_hsusb_port_allocated = DS3G_SIOLIB_PORTID_NONE;
 
    if (forwarding_port_id != DS3G_SIOLIB_PORT_NONE ) 
    {
      ds3g_siolib_release_forwarding_port (forwarding_port_id);
      forwarding_port_id = DS3G_SIOLIB_PORT_NONE;
    }
  }
  
  else if (event == ERROR_EV) 
  {
    switch(cb_data)
    {
      case MODEM_PORT_ALLOCATION_FAILURE:
        break;

      case FORWARDING_PORT_NOT_AVAILABLE:
        ds707_rmsm_proxy_disable_inbound_flow();
        (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
        ds3g_siolib_set_call_coming_up( FALSE );
        ds707_rmsm_proxy_enable_inbound_flow();
        ds3g_siolib_release_port ( ds707_rmsm_proxy_hsusb_port_allocated );
        ds707_rmsm_proxy_hsusb_port_allocated = DS3G_SIOLIB_PORTID_NONE;
        break;

      case ABORT_CMD_RECEIVED: 
        ds707_rmsm_proxy_disable_inbound_flow();
        (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
        ds3g_siolib_set_call_coming_up( FALSE );
        dsat_send_result_code (DSAT_NO_CARRIER);
        ds707_rmsm_proxy_enable_inbound_flow();
        ds3g_siolib_release_port ( ds707_rmsm_proxy_hsusb_port_allocated );
        ds707_rmsm_proxy_hsusb_port_allocated = DS3G_SIOLIB_PORTID_NONE;
        if (forwarding_port_id != DS3G_SIOLIB_PORT_NONE ) 
        {
          ds3g_siolib_release_forwarding_port (forwarding_port_id);
          forwarding_port_id = DS3G_SIOLIB_PORT_NONE;
        }
        break;

      case DSAT_ERROR: 
        ds707_rmsm_proxy_disable_inbound_flow();
        (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
        ds3g_siolib_set_call_coming_up( FALSE );
        dsat_send_result_code (DSAT_ERROR);
        ds3g_siolib_release_port ( ds707_rmsm_proxy_hsusb_port_allocated );
        ds707_rmsm_proxy_hsusb_port_allocated = DS3G_SIOLIB_PORTID_NONE;
        ds707_rmsm_proxy_enable_inbound_flow();
        if (forwarding_port_id != DS3G_SIOLIB_PORT_NONE ) 
        {
         ds3g_siolib_release_forwarding_port (forwarding_port_id);
         forwarding_port_id = DS3G_SIOLIB_PORT_NONE;
        }
        break;

      default:
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Error event received with an "
                        "Invalid error code");
        ASSERT(0);
        break;
    }
  }
}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_SETUP_HSUSB_WATERMARKS()

  DESCRIPTION
    This function configures the SIO packet mode watermark items
    to operate for the 1X calls handled by RMSM PROXY.

  PARAMETERS
    *tx_wm_ptr - ptr to Tx watermark (network to TE)
    *rx_wm_ptr - ptr to Rx watermark (TE to network)

  RETURN VALUE
    None

  DEPENDENCIES
    Hi/Lo Watermark Settings: The Lo and Hi watermark values and the don't
    exceed values for each watermark is inter-dependent with the DSM small
    and large item counts (DSM_LARGE_ITEM_CNT and DSM_SMALL_ITEM_CNT) in the
    DSM module. Any change to lo/hi watermark or don't exceed values can
    cause ERR_FATALs due to insufficient memory items available.

  SIDE EFFECTS
    None
===========================================================================*/

static void ds707_rmsm_proxy_setup_hsusb_watermarks
(
  dsm_watermark_type *rx_wm_ptr,
  dsm_watermark_type *tx_wm_ptr
)
{
  sio_ioctl_param_type ds707_rmsm_proxy_sio_ioctl_param;
  /*-------------------------------------------------------------------------
    Make sure that the watermarks passed in are empty
  -------------------------------------------------------------------------*/
  if(rx_wm_ptr->current_cnt != 0)
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, "HSUSB Rx 0x%x not empty, %d bytes",
                    rx_wm_ptr, rx_wm_ptr->current_cnt);
    dsm_empty_queue(rx_wm_ptr);
  }
  if(tx_wm_ptr->current_cnt != 0)
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, "HSUSB Tx 0x%x not empty, %d bytes",
                    tx_wm_ptr, tx_wm_ptr->current_cnt);
    dsm_empty_queue(tx_wm_ptr);
  }


  ds707_rmsm_proxy_sio_ioctl_param.flow_ctl.tx_flow = SIO_FCTL_BEST;
  ds707_rmsm_proxy_sio_ioctl_param.flow_ctl.rx_flow = SIO_FCTL_BEST;

  ds3g_siolib_ioctl( SIO_IOCTL_GET_FLOW_CTL, &ds707_rmsm_proxy_sio_ioctl_param );

  /*-------------------------------------------------------------------------
                             SETUP SIO TX WATERMARK
  -------------------------------------------------------------------------*/

  tx_wm_ptr->lo_watermark    = DS707_RMSM_REVA_FWD_LO_WATERMARK;
  tx_wm_ptr->hi_watermark    = DS707_RMSM_REVA_FWD_HI_WATERMARK;
  tx_wm_ptr->dont_exceed_cnt = DS707_RMSM_REVA_FWD_DONT_EXCEED_CNT;

  tx_wm_ptr->lowater_func_ptr      = ds707_rmsm_proxy_hsusb_tx_wm_lo_cb;
  tx_wm_ptr->hiwater_func_ptr      = ds707_rmsm_proxy_hsusb_tx_wm_hi_cb;
  tx_wm_ptr->each_enqueue_func_ptr = NULL;
  tx_wm_ptr->gone_empty_func_ptr   = NULL;
  tx_wm_ptr->non_empty_func_ptr    = NULL;

  
  /*-------------------------------------------------------------------------
                             SETUP SIO RX WATERMARK
  -------------------------------------------------------------------------*/

  rx_wm_ptr->lo_watermark    = DS707_RMSM_REVA_REV_LO_WATERMARK;
  rx_wm_ptr->hi_watermark    = DS707_RMSM_REVA_REV_HI_WATERMARK;
  rx_wm_ptr->dont_exceed_cnt = DS707_RMSM_REVA_REV_DONT_EXCEED_CNT;

  rx_wm_ptr->lowater_func_ptr      = NULL;
  rx_wm_ptr->hiwater_func_ptr      = NULL;
  rx_wm_ptr->each_enqueue_func_ptr = ds707_rmsm_proxy_hsusb_rx_wm_enqueue_cb;
  rx_wm_ptr->gone_empty_func_ptr   = NULL;
  rx_wm_ptr->non_empty_func_ptr    = NULL;
  

#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Reset the total_rcvd_cnt, as a packet data call can really set this
    number very high.
  -------------------------------------------------------------------------*/
  rx_wm_ptr->total_rcvd_cnt        = 0;
  tx_wm_ptr->total_rcvd_cnt        = 0;
#endif

}

/*===========================================================================
FUNCTION      DS707_RMSM_PROXY_HSUSB_TX_WM_LO_CB

DESCRIPTION   Called (via function pointer) when the Tx watermark reaches 
              LOW threshold.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds707_rmsm_proxy_hsusb_tx_wm_lo_cb
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  DATA_IS707_MSG2(MSG_LEGACY_HIGH,"Low HSUSB Tx watermark: %x ,"
                  "Count: %d. Enable the inbound Flow.",
                  wm_ptr,
                  wm_ptr->current_cnt);

  ds3g_siolib_set_port_inbound_flow(forwarding_port_id,
                                    DS_FLOW_PKT_WM_MASK, 
                                    DS_FLOW_ENABLE );
}

/*===========================================================================
FUNCTION      DS707_RMSM_PROXY_HSUSB_TX_WM_HI_CB

DESCRIPTION   Called (via function pointer) when the Tx watermark reaches 
              HIGH threshold.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds707_rmsm_proxy_hsusb_tx_wm_hi_cb
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "High HSUSB Tx watermark:%x , "
                  "Count: %d. Disable the inbound Flow.",
                  wm_ptr,
                  wm_ptr->current_cnt);

  ds3g_siolib_set_port_inbound_flow(forwarding_port_id,
                                    DS_FLOW_PKT_WM_MASK, 
                                    DS_FLOW_DISABLE );
}

/*===========================================================================
FUNCTION      DS707_RMSM_PROXY_HSUSB_RX_WM_ENQUEUE_CB

DESCRIPTION   Called (via function pointer) when a Dsm item is enqueued on
              the Rx watermark 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds707_rmsm_proxy_hsusb_rx_wm_enqueue_cb
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  dsm_item_type *dsm_item_ptr = dsm_dequeue(wm_ptr);
  if (dsm_item_ptr == NULL) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "NULL returned by dsm_dequeue in HSUSB Rx WM Enqueue CB");
    ASSERT(0);
    return;
  }

  ds3g_siolib_send_data_to_port(NULL,dsm_item_ptr, FALSE, forwarding_port_id);
 
}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_SETUP_HSUART_WATERMARKS()

  DESCRIPTION
    This function configures the SIO packet mode watermark items
    to operate for the 1X calls handled by RMSM PROXY.

  PARAMETERS
    *rx_wm_ptr - ptr to Rx watermark (network to TE)
    *tx_wm_ptr - ptr to Tx watermark (TE to network)

  RETURN VALUE
    None

  DEPENDENCIES
    Hi/Lo Watermark Settings: The Lo and Hi watermark values and the don't
    exceed values for each watermark is inter-dependent with the DSM small
    and large item counts (DSM_LARGE_ITEM_CNT and DSM_SMALL_ITEM_CNT) in the
    DSM module. Any change to lo/hi watermark or don't exceed values can
    cause ERR_FATALs due to insufficient memory items available.

  SIDE EFFECTS
    None
===========================================================================*/

static void ds707_rmsm_proxy_setup_hsuart_watermarks
(
  dsm_watermark_type *rx_wm_ptr,
  dsm_watermark_type *tx_wm_ptr
)
{
  sio_ioctl_param_type ds707_rmsm_proxy_sio_ioctl_param;
  /*-------------------------------------------------------------------------
    Make sure that the watermarks passed in are empty
  -------------------------------------------------------------------------*/
  if(rx_wm_ptr->current_cnt != 0)
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, "HSUART Rx WM 0x%x not empty, %d bytes", 
                    rx_wm_ptr, rx_wm_ptr->current_cnt);
    dsm_empty_queue(rx_wm_ptr);
  }
  if(tx_wm_ptr->current_cnt != 0)
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, "HSUART Tx WM 0x%x not empty, %d bytes",
                    tx_wm_ptr, tx_wm_ptr->current_cnt);
    dsm_empty_queue(tx_wm_ptr);
  }

  ds707_rmsm_proxy_sio_ioctl_param.flow_ctl.tx_flow = SIO_FCTL_BEST;
  ds707_rmsm_proxy_sio_ioctl_param.flow_ctl.rx_flow = SIO_FCTL_BEST;

  ds3g_siolib_ioctl( SIO_IOCTL_GET_FLOW_CTL, &ds707_rmsm_proxy_sio_ioctl_param );

  /*-------------------------------------------------------------------------
                             SETUP SIO TX WATERMARK
  -------------------------------------------------------------------------*/

  tx_wm_ptr->lo_watermark    = DS707_RMSM_REVA_REV_LO_WATERMARK;
  tx_wm_ptr->hi_watermark    = DS707_RMSM_REVA_REV_HI_WATERMARK;
  tx_wm_ptr->dont_exceed_cnt = DS707_RMSM_REVA_REV_DONT_EXCEED_CNT;

  tx_wm_ptr->lowater_func_ptr      = ds707_rmsm_proxy_hsuart_tx_wm_lo_cb;
  tx_wm_ptr->hiwater_func_ptr      = ds707_rmsm_proxy_hsuart_tx_wm_hi_cb;
  tx_wm_ptr->each_enqueue_func_ptr = NULL;
  tx_wm_ptr->gone_empty_func_ptr   = NULL;
  tx_wm_ptr->non_empty_func_ptr    = NULL;

  
  /*-------------------------------------------------------------------------
                             SETUP SIO RX WATERMARK
  -------------------------------------------------------------------------*/

  rx_wm_ptr->lo_watermark    = DS707_RMSM_REVA_FWD_LO_WATERMARK;
  rx_wm_ptr->hi_watermark    = DS707_RMSM_REVA_FWD_HI_WATERMARK;
  rx_wm_ptr->dont_exceed_cnt = DS707_RMSM_REVA_FWD_DONT_EXCEED_CNT;

  rx_wm_ptr->lowater_func_ptr      = NULL;
  rx_wm_ptr->hiwater_func_ptr      = NULL;
  rx_wm_ptr->each_enqueue_func_ptr = ds707_rmsm_proxy_hsuart_rx_wm_enqueue_cb;
  rx_wm_ptr->gone_empty_func_ptr   = NULL;
  rx_wm_ptr->non_empty_func_ptr    = NULL;
  

#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Reset the total_rcvd_cnt, as a packet data call can really set this
    number very high.
  -------------------------------------------------------------------------*/
  rx_wm_ptr->total_rcvd_cnt        = 0;
  tx_wm_ptr->total_rcvd_cnt        = 0;
#endif

}

/*===========================================================================
FUNCTION      DS707_RMSM_PROXY_HSUART_TX_WM_LO_CB

DESCRIPTION   Called (via function pointer) when the Tx watermark reaches 
              LOW threshold.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds707_rmsm_proxy_hsuart_tx_wm_lo_cb
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  DATA_IS707_MSG2(MSG_LEGACY_HIGH,"Low HSUART Tx watermark:%x , "
                  "Count: %d. Disable the inbound Flow.",
                  wm_ptr,
                  wm_ptr->current_cnt);

  ds3g_siolib_set_port_inbound_flow( ds3g_siolib_get_ds3g_siolib_modem_port(),
                                     DS_FLOW_PKT_WM_MASK,
                                     DS_FLOW_ENABLE );
}

/*===========================================================================
FUNCTION      DS707_RMSM_PROXY_HSUART_TX_WM_HI_CB

DESCRIPTION   Called (via function pointer) when the Tx watermark reaches 
              HIGH threshold.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds707_rmsm_proxy_hsuart_tx_wm_hi_cb
(
  struct dsm_watermark_type_s *wm_ptr,
  void *user_data_ptr
)
{
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "High HSUART Tx watermark:%x , Count: %d. "
                  "Disable the inbound Flow.",
                  wm_ptr,
                  wm_ptr->current_cnt);

  ds3g_siolib_set_port_inbound_flow( ds3g_siolib_get_ds3g_siolib_modem_port(),
                                     DS_FLOW_PKT_WM_MASK, 
                                     DS_FLOW_DISABLE );
}

/*===========================================================================
FUNCTION      DS707_RMSM_PROXY_HSUART_RX_WM_ENQUEUE_CB

DESCRIPTION   Called (via function pointer) when Data is enqueued on the Rx 
              watermark.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds707_rmsm_proxy_hsuart_rx_wm_enqueue_cb
(
  struct dsm_watermark_type_s *wm_ptr,
  void *user_data_ptr
)
{
  dsm_item_type *dsm_item_ptr = dsm_dequeue(wm_ptr);
  if (dsm_item_ptr == NULL) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "NULL returned by dsm_dequeue in HSUART Rx WM Enqueue CB");
    ASSERT(0);
    return;
  }

  ds3g_siolib_send_data_to_port(NULL,dsm_item_ptr, FALSE, 
                                ds3g_siolib_get_ds3g_siolib_modem_port());
}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_DISABLE_INBOUND_FLOW

  DESCRIPTION
    This function disables the inbound flow on modem port and forwarding port.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_disable_inbound_flow
(
  void
)
{
  ds3g_siolib_set_port_inbound_flow(ds3g_siolib_get_ds3g_siolib_modem_port(),
                                    DS_FLOW_707_RMSM_MASK, 
                                    DS_FLOW_DISABLE );

  ds3g_siolib_set_port_inbound_flow(forwarding_port_id,
                                    DS_FLOW_707_RMSM_MASK, 
                                    DS_FLOW_DISABLE );
}

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_ENABLE_INBOUND_FLOW

  DESCRIPTION
    This function enables the inbound flow on modem port and forwarding port.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds707_rmsm_proxy_enable_inbound_flow
(
  void
)
{

  ds3g_siolib_set_port_inbound_flow( ds3g_siolib_get_ds3g_siolib_modem_port(),
                                     (DS_FLOW_ALL_MASK
                                     & ~DS_FLOW_DSM_LEVEL_MASKS),
                                     DS_FLOW_ENABLE );

  ds3g_siolib_set_port_inbound_flow( forwarding_port_id,
                                     (DS_FLOW_ALL_MASK
                                      & ~DS_FLOW_DSM_LEVEL_MASKS),
                                      DS_FLOW_ENABLE );
}

#endif /* FEATURE_DSAT_MDM_FUSION */
