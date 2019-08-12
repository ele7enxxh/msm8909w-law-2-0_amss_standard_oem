#ifndef DIAGCOMM_V_H
#define DIAGCOMM_V_H
/*==========================================================================

              Diagnostic Communications Layer Header

General Description
  API declarations for the diagnostic protocol transport layer.
  
Copyright (c) 2000-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           Edit History
                           
 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_v.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
01/01/14   ph      Fixed inconsistent drain behavior in circular and threshold buffering.
08/31/13   is      Support for peripheral buffering mode
01/03/13   sg      Command Response Channel separation
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/08/12   sr      Support for DCI Override feature(OCH) and 
                   On Board Diagnostics(OBD)- Secure Sim feature   
03/02/12   sg      Added prototype for diagcomm_sio_ctrl_conn_changed()
10/14/11   hm      Diag consumer API - modem side changes 
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
03/04/11   is      Support for Diag over SMD-Lite
02/01/11   sg      Dual mask changes
10/25/10   is      Changes for RDM support in 8960 bring-up
09/28/10   sg      Moved diag_time_get from diagtarget.h to here
09/14/10   is      Added prototype for diagcomm_connect_state()
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
08/09/10   is      Extern diagpkt_filtered_cmd_set to resolve compilation error
07/23/10   mad     Moved declaration for diagpkt_filtered_cmd_set to this file
07/21/10   sg      Added dsm chaining support for rx response packets 
10/31/10   vs      Changes to support modified diagbuf_send_pkt_single_proc
12/13/09   sg      Added prototype for diagcomm_sio_flow_enabled
12/10/09   sg      Removed diagcomm_register_outbound_flow_ctrl()
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/17/09    mad    Featurized includion of customer.h and target.h for WM.
05/07/09   vk      moved declerations into diagcomm.h that were being referred from there
12/12/08    vg     CMI splip into diagcomm_v.h and diagcomm.h
12/16/07    pc     Added support for diag_dtr_enable and diag_dtr_changed_cb.
12/15/06    as     Fixed compiler warnings.
05/15/05    as     Added support for FEATURE_DATA_SERIALIZER
02/09/05   abu     Added support for Diag over IrDA under FEATURE_IRDA flag
12/14/04    sl     Added support for DIAG to hold multiple SIO ports
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
10/24/01   jal     Added ability to flush transmit channel
04/06/01   lad     Introduces types for callback function pointers.
                   Moved ASYNC char definitions from diagi.h.
02/23/01   lad     Created file.

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#ifndef FEATURE_WINCE
#endif

#include "diagcomm_io.h"

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */
typedef void (*diagcomm_notify_fnc_ptr_type)( void );

typedef void (*diagcomm_tx_start_fnc_ptr_type)( void );

typedef void (*diagcomm_tx_stop_fnc_ptr_type)( void );

typedef void (*diagcomm_flush_cb_fnc_ptr_type)( void );


/* Definitions for Half-Duplex Async-HDLC mode.
 * NOTE: Under most circumstances, the communications layer doesn't need to 
 * know about HDLC.  Diag handles this.  However, in the case of routing mode,
 * The communications layer needs to be able to determine the end of a packet
 * in order to forward packets.
 */
#define DIAG_ASYNC_CONTROL_CHAR 0x7E /* Flag character value.  Corresponds 
                                        to the ~ character                  */
#define DIAG_ASYNC_ESC_CHAR     0x7D /* Escape sequence 1st character value */
#define DIAG_ASYNC_ESC_MASK     0x20 /* Escape sequence complement value    */
#define DIAG_MIN_PKT_SIZE       3    /* Command code and CRC                */
#define DIAG_ESC_EXPAND_SIZE    2    /* Size of an expanded escape code     */
#define DIAG_ASYNC_TAIL_SIZE    3    /* CRC plus tail character             */
#define DIAG_FOOTER_SIZE        5    /* 2 byte CRC plus the possibility of 
                                        expanding per AHDLC protocol plus  
                                        the  ASYNC_FLAG                     */
#define DIAG_ESC_EXPAND_SHIFT 1      /* The number of bits to shift for
                                        expanded size due to escaping.      */


extern boolean diagpkt_filtered_cmd_set; /* Defined in diagpkt.c */

/* ------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
    extern "C" {
#endif

void diagcomm_sio_close_cb ( void );

boolean diagcomm_connect_state( void );

void diagcomm_sio_open_init( diagcomm_io_conn_type * conn );

void diagcomm_sio_conn_changed( diagcomm_io_conn_type * conn , boolean connected );

void diagcomm_sio_ctrl_conn_changed( diagcomm_io_conn_type * conn , boolean connected ); 

diagcomm_io_conn_type * diagcomm_get_sio_conn( diagcomm_enum_port_type port_num );


/*===========================================================================

FUNCTION DIAGCOMM_SIO_INIT

DESCRIPTION
  This function initializes the diagnostic transport layer.

===========================================================================*/
void diagcomm_sio_init ( diagcomm_enum_port_type port_num );

/*===========================================================================

FUNCTION DIAGCOMM_REGISTER_INBOUND

DESCRIPTION
  This given callback will be called when inbound data is available from 
  the comm layer.

===========================================================================*/
void diagcomm_register_inbound( 
   diagcomm_notify_fnc_ptr_type inbound_pkt_cb
);

/*===========================================================================

FUNCTION DIAGCOMM_SIO_OPEN

DESCRIPTION
  Opens the communications port.

===========================================================================*/
boolean diagcomm_sio_open (diagcomm_enum_port_type port_num);

/*===========================================================================

FUNCTION DIAGCOMM_FLUSH_RX

DESCRIPTION
  Flushes the communications port's receive channel.

===========================================================================*/
void diagcomm_flush_rx (
  diagcomm_flush_cb_fnc_ptr_type flush_fp
);

/*===========================================================================

FUNCTION DIAGCOMM_FLUSH_TX

DESCRIPTION
  Flushes the communications port's transmit channel.

===========================================================================*/
void diagcomm_flush_tx (
  diagcomm_flush_cb_fnc_ptr_type flush_fp, unsigned int timeoutVal
);

/*===========================================================================

FUNCTION DIAGCOMM_CLOSE

DESCRIPTION
  Closes the communications port.

===========================================================================*/
void diagcomm_close ( void );


/*===========================================================================

FUNCTION DIAGCOMM_CLOSE_DCI

DESCRIPTION
  Closes the DCI port.

===========================================================================*/
void diagcomm_close_dci ( void );

/*===========================================================================

FUNCTION DIAGCOMM_STATUS

DESCRIPTION
  This function returns TRUE if the port passed in is open.

===========================================================================*/
boolean diagcomm_status (diagcomm_enum_port_type port);

/*===========================================================================

FUNCTION DIAGCOMM_SIO_FLOW_ENABLED

DESCRIPTION
  This function returns TRUE if the flow of traffic is enabled .

===========================================================================*/
boolean diagcomm_sio_flow_enabled(void);

/*===========================================================================
FUNCTION diagcomm_buffering_tx_wmq_flow_enabled

DESCRIPTION
   This function tells whether the flow to diagcomm_sio_tx_wmq is enabled 
   or not.
   
PARAMETERS
    port_num - Port num (DIAGCOMM_PORT_1, DIAGCOMM_PORT_2, ...)
    
RETURN VALUE
  TRUE - Flow to SIO Tx wmq is enabled
  FALSE - Otherwise 
  
===========================================================================*/
boolean diagcomm_buffering_tx_wmq_flow_enabled( diagcomm_enum_port_type port_num );

/*===========================================================================
FUNCTION DIAGCOMM_INCR_WMQ_FLOW_CTRL_COUNT

DESCRIPTION
  This function increments the internal variable diagcomm_wmq_flow_ctrl_count, which keeps
  track of the # of times flow control is triggered.
    
PARAMETERS
  port_num
  
RETURN VALUE
  The current wmq flow control count.
  
===========================================================================*/
uint32 diagcomm_incr_wmq_flow_ctrl_count( diagcomm_enum_port_type port_num );

/*===========================================================================

FUNCTION DIAGCOMM_OUTBOUND_ALLOC

DESCRIPTION
  These functions requests a buffer from the diag communications layer
  
PARAMETERS 
  count -Number of bytes to be allocated 
  is_chain- Requesting a chain if variable is TRUE 
  length - length of the data in the dsm
  priority - indicates whether this data is of high priority over others in queue
  is_cmd_rsp_type -tells whether data is cmd rsp type or other(f3/logs/events)  
===========================================================================*/
dsm_item_type *
 diagcomm_outbound_alloc ( 
  unsigned int *count,      /* Number of bytes allocated */
  boolean is_chain,             /* Requesting a chain if variable is TRUE*/
  boolean priority,
  boolean is_cmd_rsp_type
);

dsm_item_type *
 diagcomm_outbound_alloc_dci ( 
  unsigned int *count,      /* Number of bytes allocated */
  boolean is_chain,             /* Requesting a chain if variable is TRUE*/
  boolean priority,
  boolean is_cmd_rsp_type
);
/*===========================================================================

FUNCTION DIAGCOMM_SEND

DESCRIPTION
  This function sends the given stream.  No framing is done by this 
  function.

PARAMETERS 
  ptr-pointer to the dsm item
  length - length of the data in the dsm
  is_cmd_rsp_type -tells whether data is cmd rsp type or other(f3/logs/events) 
  port_num - Port number
===========================================================================*/
void diagcomm_send ( 
  dsm_item_type *ptr, 
  unsigned int length,
  boolean is_cmd_rsp_type,
  diagcomm_enum_port_type port_num
);

/*===========================================================================

FUNCTION DIAGCOMM_INBOUND

DESCRIPTION
  This function returns a pointer to some inbound data.  If no inbound 
  data is available, NULL is returned.

  This data stream is not packet delimited.  The stream may contain more 
  than one packet, or a parital packet.

===========================================================================*/
void * diagcomm_inbound ( 
  unsigned int *stream_len, /* number of bytes in the stream (if non-NULL)  */
  diagcomm_enum_port_type port_num /* Port on which packet is received */
);

/*===========================================================================

FUNCTION DIAGCOMM_INBOUND_COMPLETE

DESCRIPTION
  This function tells the diag communications layer that the caller is done
  with the buffer returned by diagcomm_send() may now be released.

===========================================================================*/
void diagcomm_inbound_complete ( 
  void *ptr
);

/*===========================================================================

FUNCTION DIAGCOMM_GET_RX_WMQ_CNT

DESCRIPTION
  This function returns the rx watermark queue count.

===========================================================================*/
uint32 diagcomm_get_rx_wmq_cnt(diagcomm_enum_port_type port_num);

void diagcomm_sio_tx_flush( void);

/*===========================================================================
FUNCTION DIAGCOMM_RESET_FLOW_CTRL_COUNT

DESCRIPTION
  This function resets the internal variable diagcomm_flow_ctrl_count, which keeps
  track of the # of times flow control is triggered.

===========================================================================*/
void diagcomm_reset_flow_ctrl_count( void );

/*===========================================================================
FUNCTION DIAGCOMM_GET_FLOW_CTRL_COUNT

DESCRIPTION
  This function returns the internal variable diagcomm_flow_ctrl_count, which keeps
  track of the # of times flow control is triggered.

===========================================================================*/
uint32 diagcomm_get_flow_ctrl_count( diagcomm_port_type port_type );

/*===========================================================================
FUNCTION DIAGCOMM_INCR_FLOW_CTRL_COUNT

DESCRIPTION
    This function increments the internal variable diagcomm_flow_ctrl_count, which keeps
    track of the # of times flow control is triggered.
    
===========================================================================*/
uint32 diagcomm_incr_flow_ctrl_count( diagcomm_port_type port_type );

void diagcomm_inbound_pkt_notify (dsm_item_type ** item_ptr);

void diagcomm_sio_empty_tx_wm_queue( diagcomm_enum_port_type port_num );

/*===========================================================================

FUNCTION DIAGCOMM_SIO_TX_DSM_LEVEL_REG (internal)

DESCRIPTION
  This function regsiteres and de-registers DSM pool level callbacks per
  stream/port id.
  
PARAMETERS
  port_num - port id to register/re-register
  reg_level - TRUE, register level callbacks.
              FALSE, de-register level callbacks.
  
RETURN VALUE
  NONE
  
DEPENDENCIES
  Update info in diag_tx_mode[] first before calling this function.
  
===========================================================================*/
void diagcomm_sio_tx_dsm_level_reg( uint8 port_num, boolean reg_level );

#ifdef __cplusplus
    }
#endif
#endif /*DIAGCOMM_V_H*/
