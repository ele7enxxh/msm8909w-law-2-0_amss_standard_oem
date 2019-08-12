#ifndef _DS_RMNET_XPORT_H
#define _DS_RMNET_XPORT_H
/*===========================================================================

                 D S _ R M N E T _ X P O R T . H

DESCRIPTION

  RMNET transport - Definitions header file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2005-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/inc/ds_rmnet_xport.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/24/13    wc     Initial file
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "sio.h"
#include "dsm.h"
#include "ds_rmnet_meta_sm.h" 
#include "dcc_task_defs.h"

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Maximum number of physical data ports
---------------------------------------------------------------------------*/
#define RMNET_PHYS_XPORT_MAX  (8)

/*---------------------------------------------------------------------------
  Physical transport structure
---------------------------------------------------------------------------*/
struct rmnet_xport_type_s;
typedef struct
{
  sio_port_id_type                 sio_port;     // physical channel SIO port
  sio_stream_id_type               stream_id;    // physical channel stream ID
  uint32                           ep_id;        // physical end point ID
  uint32                           consumer_pipe_num;  // channel param
  uint32                           producer_pipe_num;  // channel param
  dsm_watermark_type               rx_wmk;
  q_type                           rx_wmk_q;
  dsm_watermark_type               tx_wmk;
  q_type                           tx_wmk_q;
  rmnet_data_format_type           data_format;  // data format on physical channel
  rmnet_qmap_settings_type         qmap_settings;
  struct rmnet_xport_type_s      * xport_list;   // list of binding logical channels
  boolean                          is_hw_port;   // is hw accelerated port
  boolean                          dtr_is_high;
  uint8                            phys_xport_inst;
  boolean                          rx_fc;        // UL flow controlled
  uint32                           flow_mask;
  uint8                            teth_call_cnt; //No of tethered calls on this physical channel
} rmnet_phys_xport_type;

// Init callback
typedef boolean (* rmnet_xport_init_cb_type)
                     (struct rmnet_xport_type_s *, void *);

// Callback to check if rmnet in a call
typedef boolean (* rmnet_xport_in_call_cb_type) (void *);

// Callback to be invoked when EP config is done
typedef void (* rmnet_xport_ep_cfg_cb_type) (void *);

// Callback to be invoked when DTR changed
typedef void (* rmnet_xport_dtr_cb_type) (void *, boolean);

/*---------------------------------------------------------------------------
  Logical transport structure
---------------------------------------------------------------------------*/
typedef struct rmnet_xport_type_s
{
  uint32                           ep_id;       // binding physical channel ep_id
  uint8                            mux_id;      // logical channel mux ID
  boolean                          inited;
  boolean                          is_hw_port;  // is hw accelerated port
  uint8                            ep_cfg_cnt;  // end point config count
  sio_port_id_type                 sio_port;    // logical channel SIO port
  sio_stream_id_type               stream_id;   // logical channel stream ID
  rmnet_phys_xport_type          * phys_xport;  // binding channel
  dsm_watermark_type               rx_wmk;
  q_type                           rx_wmk_q;
  boolean                          rx_wmk_fc;   // RX wmk flow controlled
  dsm_watermark_type               tx_wmk;
  q_type                           tx_wmk_q;
  boolean                          tx_wmk_fc;   // TX wmk flow controlled
  rmnet_data_format_type           data_format_cache;
  void *                           user_info;
  rmnet_xport_in_call_cb_type      in_call_cb;  // cb to check if in a call
  rmnet_xport_ep_cfg_cb_type       ep_cfg_cb;   // cb for EP config done
  rmnet_xport_dtr_cb_type          dtr_cb;      // cb for DTR change
  struct rmnet_xport_type_s      * next;
} rmnet_xport_type;

#define RMNET_XPORT_WMK_MAX  (456576)

#define RMNET_XPORT_IS_INITED(xport) ((xport) != NULL && (xport)->inited)
#define RMNET_XPORT_IS_ACTIVE(xport) ((xport) != NULL && (xport)->stream_id != SIO_NO_STREAM_ID)
#define RMNET_XPORT_IS_BOUND(xport)  ((xport) != NULL && (xport)->phys_xport != NULL)

#define RMNET_PHYS_XPORT_IS_ACTIVE(phys_xport) \
              ((phys_xport) != NULL && (phys_xport)->stream_id != SIO_NO_STREAM_ID)

#define RMNET_XPORT_EP_ID(xport) \
  ((xport) == NULL ? 0 : (xport)->ep_id)

#define RMNET_XPORT_MUX_ID(xport) \
  ((xport) == NULL ? 0 : (xport)->mux_id)

/*---------------------------------------------------------------------------
  Data format macros
---------------------------------------------------------------------------*/
#define RMNET_XPORT_LINK_PROT(xport) \
  ((xport) == NULL ? RMNET_INVALID_MODE: (xport)->data_format_cache.link_prot)

#define RMNET_XPORT_QOS_ENABLED(xport) \
  ((xport) == NULL ? FALSE : (xport)->data_format_cache.qos)

#define RMNET_XPORT_TE_FC_ENABLED(xport) \
  ((xport) == NULL ? FALSE : (xport)->data_format_cache.te_flow_control)

#define RMNET_XPORT_UL_DATA_AGG(xport) \
  ((xport) == NULL ? RMNET_DISABLE_DATA_AGG : (xport)->data_format_cache.ul_data_agg_protocol)

#define RMNET_XPORT_DL_DATA_AGG(xport) \
  ((xport) == NULL ? RMNET_DISABLE_DATA_AGG : (xport)->data_format_cache.dl_data_agg_protocol)

#define RMNET_XPORT_NDP_SIG(xport) \
  ((xport) == NULL ? 0 : (xport)->data_format_cache.ndp_signature)

#define RMNET_XPORT_MBIM_SW_DEAGG(xport) \
  ((xport) == NULL ? FALSE : (xport)->data_format_cache.mbim_sw_deagg)

#define RMNET_XPORT_IN_BAND_FC(xport) \
  (((xport) == NULL || (xport)->phys_xport == NULL) ? FALSE : (xport)->phys_xport->qmap_settings.in_band_fc)

#define RMNET_XPORT_EP_CONFIGURING(xport) \
  ((xport) == NULL ? FALSE : (xport)->ep_cfg_cnt > 0)

#define RMNET_XPORT_QOS_HEADER_TYPE(xport) \
  ((xport) == NULL ? RMNET_QOS_FORMAT_6_BYTE : (xport)->data_format_cache.qos_format)

/*---------------------------------------------------------------------------
  RmNet xport commands
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_XPORT_CMD_PORT_OPEN    = 1,
  RMNET_XPORT_CMD_PORT_CLOSE   = 2,
  RMNET_XPORT_CMD_EP_CFG_DONE  = 3, 
  RMNET_XPORT_CMD_DTR_CHANGED  = 4
} rmnet_xport_cmd_e_type;

typedef struct
{
  rmnet_xport_cmd_e_type   cmd;
  uint16                   port_id;
  uint32                   ep_id;
  uint32                   consumer_pipe_num;
  uint32                   producer_pipe_num;
  sio_stream_id_type       stream_id;
  boolean                  dtr_asserted;
} rmnet_xport_cmd_type;

/*---------------------------------------------------------------------------
  The ep_id data type used in QMI messages. This is internally converted
  to uint32. The MSB is ep_type, and the 3 LSB bytes are iface_id.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32  ep_type;
  uint32  iface_id;
} qmi_ep_id_type;

#define QMI_EP_ID_VALID(qmi_ep_id) \
    ((((qmi_ep_id).ep_type & 0xFFFFFF00) == 0) && (((qmi_ep_id).iface_id & 0xFF000000) == 0)) 

#define QMI_EP_ID_TO_INTERNAL_EP_ID(qmi_ep_id) \
    ((((qmi_ep_id).ep_type & 0xFF) << 24) | ((qmi_ep_id).iface_id & 0xFFFFFF)) 

/*===========================================================================

                           EXTERNAL FUNCTIONS 

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_OPEN_CLOSE_CMD()

  DESCRIPTION
    This function sends a command to DCC task to open or close
    a physical data port

  PARAMETERS
    cmd:                rmnet xport open or close command
    port_id:            SIO port ID
    ep_id:              End point ID
    consumer_pipe_num:  EP consumer pipe number
    producer_pipe_num:  EP producer pipe number

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_open_close_cmd
(
  rmnet_xport_cmd_e_type  cmd,
  sio_port_id_type        port_id,
  uint32                  ep_id,
  uint32                  consumer_pipe_num,
  uint32                  producer_pipe_num
);

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_DTR_CMD()

  DESCRIPTION
    This function sends a command to DCC task to handle DTR event on a
    physical data channel.

  PARAMETERS
    ep_id:           End point ID
    dtr_asserted:    Is DTR asserted

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_dtr_cmd
(
  uint32    ep_id,
  boolean   dtr_asserted
);

/*===========================================================================
  FUNCTION RMNET_XPORT_CMD_HDLR()

  DESCRIPTION
    This is the command handler for rmnet xport commands

  PARAMETERS
    cmd:                DCC_RMNET_XPORT_CMD
    user_data_ptr:      Pointer to rmnet_xport_cmd_type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_xport_cmd_hdlr
(
  dcc_cmd_enum_type   cmd,
  void              * user_data_ptr
);

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_FROM_EP_ID()

  DESCRIPTION
    This function retrieves the rmnet physical xport from EP id.

  PARAMETERS
    ep_id:    End point ID

  RETURN VALUE
    Pointer to the rmnet phys xport which has the specified ep_id

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
rmnet_phys_xport_type * rmnet_phys_xport_from_ep_id
(
  uint32  ep_id
);

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_SET_DATA_FORMAT()

  DESCRIPTION
    This function sets the data format of a physical xport

  PARAMETERS
    ep_id:              End point ID
    data_format:        Data format structure (NULL for default)
    data_format_mask:   Data format mask
    client_type:        Set data format client

  RETURN VALUE
    TRUE: Success
    False: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_set_data_format
(
  uint32                    ep_id,
  rmnet_data_format_type  * data_format,
  uint32                    data_format_mask,
  rmnet_set_data_fmt_e_type client_type
);

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_GET_DATA_FORMAT()

  DESCRIPTION
    This function queries the data format of a physical xport

  PARAMETERS
    ep_id:           End point ID
    data_format:     Pointer to data format result structure

  RETURN VALUE
    TRUE: Success
    False: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_get_data_format
(
  uint32                    ep_id,
  rmnet_data_format_type  * data_format
);

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_DATA_FORMAT_CLIENT()

  DESCRIPTION
    This function queries the last client who sets the data format

  PARAMETERS
    ep_id:           End point ID

  RETURN VALUE
    Data format client type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
rmnet_set_data_fmt_e_type rmnet_phys_xport_data_format_client
(
  uint32  ep_id
);

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_GET_QMAP_SETTINGS()

  DESCRIPTION
    This function queries QMAP settings

  PARAMETERS
    ep_id:           End point ID
    qmap_settings:   Pointer to QMAP settings result structure

  RETURN VALUE
    TRUE: Success
    False: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_get_qmap_settings
(
  uint32                          ep_id,
  rmnet_qmap_settings_type      * qmap_settings
);

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_SET_QMAP_SETTINGS()

  DESCRIPTION
    This function sets QMAP settings of a physical xport

  PARAMETERS
    ep_id:              End point ID
    qmap_settings:      QMAP settings structure

  RETURN VALUE
    TRUE: Success
    False: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_phys_xport_set_qmap_settings
(
  uint32                          ep_id,
  rmnet_qmap_settings_type      * qmap_settings
);

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_SET_FLOW_MASK()

  DESCRIPTION
    Set flow mask on the physical rx port

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_phys_xport_set_flow_mask
(
  rmnet_phys_xport_type  * phys_xport,
  uint32                   mask,
  boolean                  disable
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_INIT()

  DESCRIPTION
    This function initialize a logical data transport

  PARAMETERS
    xport_info:   Pointer to rmnet xport structure
    init_cb:      Callback function to init the structure
    user_data:    User data for the rmnet xport

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_logical_xport_init
(
  rmnet_xport_type          * xport_info,
  rmnet_xport_init_cb_type    init_cb,
  void                      * user_data
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_OPEN()

  DESCRIPTION
    This function opens a logical data transport

  PARAMETERS
    xport_info:  Pointer to rmnet xport structure

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    rmnet xport must be initialized

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_logical_xport_open
(
  rmnet_xport_type  * xport_info
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_CLOSE()

  DESCRIPTION
    This function closes a logical data transport

  PARAMETERS
    xport_info:  Pointer to the rmnet xport structure

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_logical_xport_close
(
  rmnet_xport_type    * xport_info
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_BIND()

  DESCRIPTION
    This function binds a logical data port to a physical port

  PARAMETERS
    xport_info:  rmnet logical xport
    phys_xport:  rmnet physical xport
    mux_id:      mux_id of the binding

  RETURN VALUE
    TRUE: Success
    FALSE: Failure    

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_logical_xport_bind
(
  rmnet_xport_type       * xport_info,
  rmnet_phys_xport_type  * phys_xport,
  uint8                    mux_id
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_UNBIND()

  DESCRIPTION
    This function unbinds a logical data port from the phys port

  PARAMETERS
    xport_info:  Pointer to the rmnet xport structure

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_logical_xport_unbind
(
  rmnet_xport_type       * xport_info
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_UPDATE_ETH_HEADER()

  DESCRIPTION
    This function updates Ethernet header

  PARAMETERS
    xport_info:   rmnet xport structure

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_logical_xport_update_eth_header
(
  rmnet_xport_type       * xport_info,
  void                   * eth_hdr,
  uint8                    eth_hdr_len,
  boolean                  is_v6
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_SEND_QMAP_CMD()

  DESCRIPTION
    This function sends a QMAP command

  PARAMETERS
    xport_info:   rmnet xport structure
    cmd_size:     size of the command
    cmd_ptr:      pointer to the QMAP command

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_logical_xport_send_qmap_cmd
(
  rmnet_xport_type     * xport_info,
  uint16                 cmd_size,
  void                 * cmd_ptr
);

/*===========================================================================
FUNCTION RMNET_LOGICAL_XPORT_GET_PORT

DESCRIPTION
  This function gets the port number associated to the rmiface

PARAMETERS
  xport_info: ptr to rmnet xport

DEPENDENCIES
  None

RETURN VALUE
  sio_port_id_type - the associated port

SIDE EFFECTS
  None.
===========================================================================*/
sio_port_id_type rmnet_logical_xport_get_port
(
  rmnet_xport_type  * xport_info
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_CLEAR_WMK_QUEUES()

  DESCRIPTION
    Clear any data present in the rx and tx wmks.

  PARAMETERS
    xport_info: ptr to rmnet xport

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_logical_xport_clear_wmk_queues
(
  rmnet_xport_type  * xport_info
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_GET_WMK_QUEUES()

  DESCRIPTION
    This function returns the pointer to the RX and TX watermark Queues.

  PARAMETERS
    xport_info: ptr to rmnet xport
    rx_wmk (OUT): pointer to RX watermark.
    tx_wmk (OUT): pointer to TX watermark.

  RETURN VALUE
    Failure : -1
    Success :  0

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int16 rmnet_logical_xport_get_wmk_queues
(
  rmnet_xport_type     * xport_info,
  dsm_watermark_type  ** rx_wmk,
  dsm_watermark_type  ** tx_wmk
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_SET_RX_CB()

  DESCRIPTION
    Specify the callback function that should be invoked when data arrives
    on the data path associated with rmnet xport

  PARAMETERS
    xport_info       :  ptr to rmnet xport
    rx_cb            :  callback to be called when data arrives on the data
                        path associated with rmnet xport
                        1st param = dsm pointer to received packet
                        2nd param = user_data provided
    user_data        :  void * to be passed in the 2nd parameter each time
                        rx_cb is invoked

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    Clears any data previously queued in the rx watermark
===========================================================================*/
void rmnet_logical_xport_set_rx_cb
(
  rmnet_xport_type  * xport_info,
  void (* rx_cb)( dsm_watermark_type *, void * ),
  void              * user_data
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_SET_RX_FLOW()

  DESCRIPTION
    Flow control the link specified by rmnet xport.

  PARAMETERS
    xport_info       : ptr to rmnet xport
    disable          :  FALSE = enable flow
                        TRUE  = disable flow

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_logical_xport_set_rx_flow
(
  rmnet_xport_type  * xport_info,
  boolean             disable
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_TRANSMIT()

  DESCRIPTION
    Send data in 'tx_item' over the data channel associated with provided
    rmnet xport.

  PARAMETERS
    xport_info       :  ptr to rmnet xport
    tx_item          :  data to be transmitted

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_logical_xport_transmit
(
  rmnet_xport_type  * xport_info,
  dsm_item_type    ** tx_item
);

/*===========================================================================
  FUNCTION RMNET_XPORT_DEFAULT_RX_CB()

  DESCRIPTION
    Default recieve function for rmnet instance.  Simply frees received
    data.  Controller of rmnet instance must register a real receive
    callback via rmnet_xport_set_rx_cb() in order to connect the data link
    to the link layer data protcol above.

  PARAMETERS
    rx_wm_ptr           -  receive watermark on which rx data is enqueued
    rx_wm_ptr_user_data -  ignored

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_xport_default_rx_cb
(
  dsm_watermark_type *  rx_wm_ptr,
  void *                rx_wm_user_data
);

/*===========================================================================
  FUNCTION RMNET_LOGICAL_XPORT_FROM_PHYS_PORT()

  DESCRIPTION
    Retrieves logical port information given physical port and mux id.

  PARAMETERS
    phys_xport           -  Pointer to physical port
    ips_id               -  Mux id

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
rmnet_xport_type * rmnet_logical_xport_from_phys_port
(
  rmnet_phys_xport_type   * phys_xport,
  uint8                     ips_id
);

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_INC_TETH_CALL_CNT()

  DESCRIPTION
    Increments count of number of tethered calls on this physical port.

  PARAMETERS
    phys_xport           -  Pointer to physical port

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_phys_xport_inc_teth_call_cnt
(
  rmnet_phys_xport_type   * phys_xport
);

/*===========================================================================
  FUNCTION RMNET_PHYS_XPORT_DEC_TETH_CALL_CNT()

  DESCRIPTION
    Decrements count of number of tethered calls on this physical port.

  PARAMETERS
    phys_xport           -  Pointer to physical port

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_phys_xport_dec_teth_call_cnt
(
  rmnet_phys_xport_type   * phys_xport
);

#endif /* _DS_RMNET_XPORT_H */

