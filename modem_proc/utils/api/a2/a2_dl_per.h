#ifndef A2_DL_PER_H
#define A2_DL_PER_H
/*!
  @file a2_dl_per.h

  @brief
  External epsid interface to the DL PER portion of the a2 driver.

  This header file exposes the interface to the "Epsid-specific" interface.
  This interface relates to the data which travels from pdcp->a2 driver and
  from the a2 driver->ps.
  
  @ingroup per_implementation
  @ingroup a2_ext_interface
*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_dl_per.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
06/18/14   ars     MSM QMAP aggr feature for teth rmnet
07/19/13   ars     QMAP FC Feature
07/11/13   ars     QNX bearer ID changes
06/20/13   yjz     Remove A2_DL_PER_MAX_DMA_LEN_IN_BYTES
05/29/13   yjz     CR493599, add a2_dl_per_bridge_stream_v2
01/23/13   ars     QMAP changes for muxing
11/27/12   sb      CR386294: Increasing the size of a2_dl_per_eps_throughput_stat_s
                   fields from uint32 to uint64
10/11/12   yuw     CR401483: Supported unsolicited Packet Filtering
07/13/12   rp      CR371608. Enable/Disable A2 DL_PER bridging via A2 diag
06/05/12   yuw     Added return value for TLP control API
07/12/12   bn      Added in API for RNDIS aggregation and return value for TLP 
                   control API
03/02/12   yuw     Removed FEATURE_A2_LTE_EMBMS from API header file
02/01/12   yuw     Supported eMBMS
01/26/12   bn      added in support for setting max pkt/size per TLP block
09/30/11   bn      Added/Modified in API to reset TLP seg
09/26/11   yuw     Added RLC and DS APIs for eMBMS
09/14/11   bn      Added in API to enable/disable A2 DL TLP
06/07/11   ar      removed a2_dl_per_config()
07/27/10   bn      Added in support throughput statistic for each EPS and API
                   to retrieve it
05/14/10   bn      Removed FEATURE_A2_TEMP_CHECK_IN
05/06/10   bn      Moved A2_DL_PER_USB_IDX_NONE to a2_cfg.h 
04/30/10   bn      Fixing unbridge API to support bridge/unbridge per IPv type
01/12/10   bn      Added in API/code to support unbridging DL PER
05/14/09   sah     Add in some new espresso initialization, featurization, 
                   and split init.
04/30/09   sah     Split "bridge_stream" function.
04/23/09   sah     Move the IST function to private interface.
03/20/09   sah     Add pointer argument to IST to be correct IST function-type
01/29/09   sah     Split internalized interface into new header file.  Make
                   notification per epsid.
01/20/09   sah     Add interface to SIO and reogranize cipher params
07/25/08   sah     Initial Checkin

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <dsm.h>
#include <sio.h>
#include <a2_common.h>
#include <a2_ext.h>

/*===========================================================================

                               MACROS

===========================================================================*/


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                        Per Stream Configuration

===========================================================================*/


/*!
  Ciphering parameters configured for a given PDCP stream

  */
typedef struct
{
  // Routing dest is specified by the bounding of this stream to a port
  boolean enable;
  // Ciph Keystream offset generated internally within A2 driver?
  uint8 ciph_bearer; //! Only 5 bits
  // Ciph direction implicitly specified by the fact it's dl
  uint8 ciph_key_index; //! only 4 bits
  a2_cipher_algo_e ciphering_algorithm;
  // Ciph count, passed with the app_field of the first DSM item of the DSM chain
} a2_dl_per_ciph_param_s;

/*! @brief A2 throughput statistic structure per EPS ID */
typedef struct
{
  /*! total number of IPV4 packets*/
  uint64  ipv4_num_pkts;
  /*! total number of IPV4 bytes*/
  uint64  ipv4_num_bytes;
    /*! total number of IPV6 packets*/
  uint64  ipv6_num_pkts;
    /*! total number of IPV6 bytes*/
  uint64  ipv6_num_bytes;
} a2_dl_per_eps_throughput_stat_s;

/*! @brief define enumuration for DL PER TLP/MBIM action*/
typedef enum
{
  /*! disable TLP */
  A2_DL_PER_TLP_DISABLE = 0,
  /*! enable Qualcomm NCM v2.0 TLP */
  A2_DL_PER_TLP_QC_NCM_V20_ENABLE,
  /*! enable Microsoft NCM v2.0 TLP */
  A2_DL_PER_TLP_MS_MBIM_NCM_V20_ENABLE,
  /*! enable RNDIS aggregation */
  A2_DL_PER_TLP_RNDIS_ENABLE,
  /*! enable QMAP aggregation */
  A2_DL_PER_TLP_QMAP_ENABLE,
} a2_dl_per_tlp_action_e;

/*! @brief define enumuration for Call state */
/* ARS: TO BE REMOVED (DS DEP)*/
typedef enum
{
  A2_DL_PER_CALL_STATE_TETH_DOWN = 0,
  A2_DL_PER_CALL_STATE_TETH_UP,
  A2_DL_PER_CALL_STATE_MAX
} a2_dl_per_call_state_e;

/*  Call back function that needs to be called when 
      A2 suspend is complete for the specified espid  */
typedef void (* a2_dl_per_suspend_complete_cb)
(  uint8 epsid,
   uint32 callback_data
);

/*===========================================================================

  FUNCTION:  a2_dl_per_init

===========================================================================*/
/*!
    @brief
    Initializes the downlink peripheral block

    @return
    None
*/
/*=========================================================================*/
void a2_dl_per_init(void);

/*===========================================================================

  FUNCTION:  a2_dl_per_config

===========================================================================*/
/*!
    @brief
    Configures the downlink peripheral block

    @return
    None
*/
/*=========================================================================*/
void a2_dl_per_config(void);

/*==============================================================================

  FUNCTION: a2_dl_per_register_pdcp_watermark 

==============================================================================*/
/*!
  @brief 
  Opens the stream between PDCP and the a2 driver, and registers the PDCP/A2 
  watermark.  
  
  The PS/DS watermark must be registered before the incoming portion, via the 
  a2_dl_per_register_ds_watermark() function.

  A2 driver will not register for any callbacks in the pdcp watermark, 
  it is pdcp’s responsibility to notify the a2 driver that there is data via 
  the a2_dl_per_pdcp_data_notify() function.

*/
/*============================================================================*/
void a2_dl_per_register_pdcp_watermark 
(
  /*! The epsid this watermark is being registered for */
  uint8 epsid,
  /*! The watermark being registered to pass data from PDCP to A2 */
  dsm_watermark_type* pdcp_watermark
);


/*==============================================================================

  FUNCTION:  a2_dl_per_get_pdcp_watermark

==============================================================================*/
/*!
  @brief
  Get the PDCP watermark from the a2 driver.  

  @deprecated This function is not to be used in production code.

*/
/*============================================================================*/
dsm_watermark_type* a2_dl_per_get_pdcp_watermark
(
  uint8 epsid
);


/*==============================================================================

  FUNCTION:  a2_dl_per_deregister_pdcp_watermark

==============================================================================*/
/*!
  @brief
  Deregisters the ds/ps watermark from the a2 driver.  

  The a2 driver will deallocate all packets remaining in this watermark.  
  This watermark cannot be reused immediately after deregistration since the 
  a2 driver may be busy freeing the items.  This function should be called 
  before deregistering the ds watermark via a2_dl_per_deregister_ds_watermark().

*/
/*============================================================================*/
void a2_dl_per_deregister_pdcp_watermark
(
  /*! The epsid on which this watermark should be deregistered. */
  uint8 epsid
);


/*==============================================================================

  FUNCTION:  a2_dl_per_pdcp_data_notify

==============================================================================*/
/*!
  @brief
  Notifies the a2 driver that there is data pending in one of the pdcp 
  watermarks for the a2 driver to transmit.

*/
/*============================================================================*/
void a2_dl_per_pdcp_data_notify 
( 
  uint8 epsid 
);

/*==============================================================================

  FUNCTION:  a2_dl_per_tag_cipher_params

==============================================================================*/
/*!
  @brief
  Tags the passed in pdcp_sdu_ptr with the provided ciphering parameters
  to be used when transmitting.

  This will overwrite any data in the app pointer and app fields in the
  passed in dsm item.

*/
/*============================================================================*/
void a2_dl_per_tag_cipher_params 
(
  /*! The pcdcp sdu the ciphering parameters apply for. */
  dsm_item_type* pdcp_sdu_ptr, 
  /*! Count C per-sdu ciphering parameter */
  uint32 count_c,
  /*! The ciphering bearer id (5 bits) */
  uint8 ciph_bearer,
  /*! The ciphering key index (4 bits) */
  uint8 ciph_key_idx,
  /*! The ciphering algorithm to tag with */
  a2_cipher_algo_e cipher_algo
);

/*==============================================================================

  FUNCTION:  a2_dl_per_tag_no_cipher_params

==============================================================================*/
/*!
  @brief
  Tags the passed in pdcp_sdu_ptr with no ciphering parameters.

  This should be done with all packets not meant to be deciphered.

*/
/*============================================================================*/
void a2_dl_per_tag_no_cipher_params 
(
  /*! The pcdcp sdu which no ciphering is to be done for */
  dsm_item_type* pdcp_sdu_ptr
 );

/*===========================================================================

                         PS/DS interface

===========================================================================*/

/*==============================================================================

  FUNCTION: a2_dl_per_register_ds_watermark 

==============================================================================*/
/*!
  @brief 
  Registers the watermark between the A2 and DS/PS.
  
  This watermark will contain the data which arrives from PDCP and is 
  not sent out a USB port.  This would happen if there is a filtering
  rule match, or if this stream is not bridged.  Each watermark is 
  paired with a pdcp watermark, and are indexed via epsid.
  watermark.  
  The corresponding pdcp watermark is registered via 
  a2_dl_per_register_ds_watermark().

*/
/*============================================================================*/
void a2_dl_per_register_ds_watermark 
(
  /*! The epsid this watermark is being registered for */
  uint8 epsid,
  /*! The watermark being registered to pass data from A2 to DS */
  dsm_watermark_type* ds_watermark
);


/*==============================================================================

  FUNCTION:  a2_dl_per_deregister_ds_watermark

==============================================================================*/
/*!
  @brief
  Deregisters the ds/ps watermark from the a2 driver.  

*/
/*============================================================================*/
void a2_dl_per_deregister_ds_watermark
(
  /*! The epsid on which this watermark should be deregistered. */
  uint8 epsid
);

/*==============================================================================

  FUNCTION:  a2_dl_per_bridge_stream

==============================================================================*/
/*!
  @brief
  Bridges/unbridges traffic on a specific espid stream and sends the data
  directly out the specified usb port.

  Note: The bridging will be done per IPv type. So there could be multiple 
  bridgings per EPSID.
  Upon bridging any data which does not match the ip-filter rules will be 
  sent out the specified usb port based upon ip-version.  Any data which
  matches a programmed filter rules or partial/unknown will be destined for 
  to the external memory meaning going back to DS/PS. 

  The ipfilter interface is defined in a2_ipfilter.h.

  The input ports are:
  - SIO_PORT_A2_RMNET_1,SIO_PORT_A2_RMNET_2,SIO_PORT_A2_RMNET_3,SIO_PORT_A2_MDM
  and SIO_PORT_NULL. 
  Note that for input port SIO_PORT_NULL, data will go to external memory 
  regardless of the IP filter status

*/
/*============================================================================*/
void a2_dl_per_bridge_stream
(
  /*! The epsid on which data is to be bridged. */
  uint8 epsid,
  /*! The ip type for which data is to be bridged */
  a2_ip_type_e ip_type,
  /*! The port to send usb port to bridge traffic on to */
  sio_port_id_type port_id
);

/*==============================================================================

  FUNCTION:  a2_dl_per_unbridge_stream

==============================================================================*/
/*!
  @brief
  Unbridges traffic on a specific espid stream and the unbridged data will be
  sent to external memory

  Note: Unbridging will be done per IPv type. If there is no active bridging
  associated with the ipput EPSID, IP filtering will be disable. Data will be 
  destined for the specified destination based on the input port_id.

  The input ports are:
  - SIO_PORT_A2_RMNET_1,SIO_PORT_A2_RMNET_2,SIO_PORT_A2_RMNET_3,SIO_PORT_A2_MDM
  and SIO_PORT_NULL. The input port is unused. It is for future expansion

*/
/*============================================================================*/
void a2_dl_per_unbridge_stream
(
  /*! The epsid on which data is to be unbridged. */
  uint8 epsid,
  /*! The ip type for which data is to be unbridged */
  a2_ip_type_e ip_type,
  /*! The port for unbridging traffic on to */
  sio_port_id_type port_id
);

/*==============================================================================

  FUNCTION:  a2_dl_per_get_eps_throughput_stat

==============================================================================*/
/*!
  @brief
  This function is called to retrieve the throughput statistic data per 
  EPS ID. When bridging, the statistic info will be reset. It will remain 
  intact when doing unbridging.

  Note: 
  For optimization, there is no mutex protection so the statistic info can
  temporary be lagged (The packet number and byte number can be temporary not 
  in sync)

  The statistic info will be reset each bridging.

  Reference to a2_dl_per_eps_throughput_stat_s data structure for detail of 
  statistic info. 
*/
/*============================================================================*/
void a2_dl_per_get_eps_throughput_stat
(
  /*! The epsid to retrieve stat data */
  uint8 epsid,
  /*! Pointer to external data to copy the stat info into */
  a2_dl_per_eps_throughput_stat_s  *stat_ptr
);

/*==============================================================================

  FUNCTION:  a2_dl_per_control_tlp

==============================================================================*/
/*!
  @brief
  This function is called by client to enable/Disable A2 DL TLP for given
  endpoint, also it is used to update the NCM/NDP sigs

@return:
      TRUE  - Support TLP on the specific port
      FALSE - Do not support TLP on the specific port
*/
/*============================================================================*/
boolean a2_dl_per_control_tlp
(
  /*! enable/disable TLP*/
  a2_dl_per_tlp_action_e action,
  /*! sio port ID */
  sio_port_id_type       port_id,
  /*! NCM header sig value */
  uint32                 ncm_sig,
  /*! NDP header sig value */
  uint32                 ndp_sig
);

/*==============================================================================

  FUNCTION:  a2_dl_per_reset_seq_tlp

==============================================================================*/
/*!
  @brief
  This function is called by client to reset the sequence number in NCM header 

  Note: 
 
 
*/
/*============================================================================*/
void a2_dl_per_reset_seq_tlp
(
  /*! sio port ID */
  sio_port_id_type  port_id
);

/*==============================================================================

  FUNCTION: a2_dl_per_register_embms_watermark 

==============================================================================*/
/*!
  @brief 
  Opens the stream between RLC and the a2 driver for eMBMS only, and registers
  the RLC/A2 watermark. 
  
  A2 driver will not register for any callbacks in the pdcp watermark, 
  it is pdcp’s responsibility to notify the a2 driver that there is data via 
  the a2_dl_per_embms_data_notify() function.

  @return
  none
*/
/*============================================================================*/
void a2_dl_per_register_embms_watermark 
(
  /*! The watermark being registered to pass eMBMS data from RLC to A2 */
  dsm_watermark_type* embms_wm
);


/*==============================================================================

  FUNCTION:  a2_dl_per_deregister_embms_watermark

==============================================================================*/
/*!
  @brief
  Deregisters the RLC watermark from the a2 driver.  

  @return
  none
*/
/*============================================================================*/
void a2_dl_per_deregister_embms_watermark ( void );

/*==============================================================================

  FUNCTION:  a2_dl_per_embms_data_notify

==============================================================================*/
/*!
  @brief
  This API is used to notify A2 that eMBMS data is available.
  The fixed port will be used for eMBMS and no eps id is needed. 

  @return
  none
*/
/*============================================================================*/
void a2_dl_per_embms_data_notify  ( void );

/*==============================================================================

  FUNCTION:  a2_dl_per_embms_bridge_stream

==============================================================================*/
/*!
  @brief
  This API is used to setup bridge for eMBMS data

  @return
  none
*/
/*============================================================================*/
void a2_dl_per_embms_bridge_stream
(
  /*! The port to be sent for bridge traffic on to */
  sio_port_id_type port_id
);

/*==============================================================================

  FUNCTION:  a2_dl_per_embms_unbridge_stream

==============================================================================*/
/*!
  @brief
  This API is used to unbridge for eMBMS data

  @return
  none
*/
/*============================================================================*/
void a2_dl_per_embms_unbridge_stream
(
  /*! The port to be sent for bridge traffic on to */
  sio_port_id_type port_id
);

/*==============================================================================

  FUNCTION:  a2_dl_per_set_thresholds_tlp

==============================================================================*/
/*!
  @brief
  This function is called by client to set the maximum number of packets and
  bytes per TLP block

  Note: 
 
 
*/
/*============================================================================*/
void a2_dl_per_set_thresholds_tlp
(
  /*! sio port ID */
  sio_port_id_type       port_id,
  /*! max of IP packet per TLP block. max_num_pkt = 0 means no impose on limit*/
  uint16                 max_num_pkt,
  /*! max number of byte per TLP block*/
  uint16                 max_num_byte
);

/*==============================================================================

  FUNCTION:  a2_dl_per_forced_bridge

==============================================================================*/
/*!
  @brief
  Bridges traffic. The API goes through the different streams and enables
  bridging on the streams that are currently enabled.
 
  Calls a2_dl_per_bridge_stream internally

  Bridges/unbridges traffic on a specific espid stream and sends the data
  directly out the specified usb port.

  Upon bridging any data which does not match the ip-filter rules will be
  sent out the specified usb port based upon ip-version.  Any data which
  matches a programmed filter rules will be destined for the location
  specified in the rule.  The ipfilter interface is defined in a2_ipfilter.h.

*/
/*============================================================================*/
void a2_dl_per_forced_bridge
(
  void
);

/*==============================================================================

  FUNCTION:  a2_dl_per_forced_unbridge

==============================================================================*/
/*!
  @brief
  Unbridges traffic. The API goes through the different streams and disables
  bridging on the streams that are currently enabled.

  Calls a2_dl_per_unbridge_stream internally

  Unbridges traffic on a specific espid stream and the unbridged data will be
  sent to external memory

  Note: Unbridging will be done per IPv type. If there is no active bridging
  associated with the ipput EPSID, IP filtering will be disable. Data will be
  destined for the specified destination based on the input port_id.

  The USB ports are: 0, 1, 2 or A2_DL_PER_USB_IDX_NONE
  The input USB port is unused. It is for future expansion

*/
/*============================================================================*/
void a2_dl_per_forced_unbridge
(
  void
);

/*===========================================================================

  FUNCTION:  a2_dl_per_register_suspend_cb

===========================================================================*/
/*!
    @brief
    API to register a suspend callback function by data module

    @note
    This CB function will be called in interrupt context, please make sure 
    this function doesn't have many complex thiigs and simple enough

   @arguments:
    a2_dl_per_suspend_complete_cb     : callback function

   @return:
    A2_SUCCESS
    A2_ERROR_INVALID_FIRST_ARGUMENT
*/
/*=========================================================================*/
a2_error_e a2_dl_per_register_suspend_cb
(
  /*! Call back function pointer to be invoked when a2 suspend is complete*/
  a2_dl_per_suspend_complete_cb suspend_cb
);
 
/*===========================================================================

  FUNCTION:  a2_dl_per_suspend_eps_bearer

===========================================================================*/
/*!
    @brief
      API to suspend A2 from picking data from PDCP WM.
      A2 invokes a2_dl_per_suspend_complete_cb when 
      suspend is complete for the specified espid

   @arguments:
     epsid      : The eps bear id on which suspend mode will be actived
     cb_data  :  The pointer of call back data that caller needs

   @return:
     A2_SUCCESS
     A2_ERROR_INVALID_FIRST_ARGUMENT   invalid eps bear id
     A2_ERROR_FAILURE  [Not ready for suspend, 
                    CB function is not registered or bridge is enabled 
                    needs to check data logic]
*/
/*=========================================================================*/
a2_error_e a2_dl_per_suspend_eps_bearer
(
   /* ! The epsid on which data is to be suspended.*/
   uint8 epsid,
   /* ! callback data to be passed on when data suspend is complete*/
   uint32 callback_data
);

/*===========================================================================

  FUNCTION:  a2_dl_per_send_qmap_ctrl_cmd

===========================================================================*/
/*!
    @brief
    Used to send a QMAP control command on the DL.
 
    @param
    port_id:  Port ID on which command needs to be sent
    mux_id :  Corresponding mux id for the port of interest
    cmd_name: Command name as indicated in the QMAP control command spec
              1 = Command to enable flow control, i.e. flow disable
              2 = Command to disable flow control, i.e. flow enable
 
    cmd_type: Command type as indicated in the QMAP control command spec

    dsm_ptr: Pointer to the DSM item which contains the command payload. A2 will
             insert the QMAP control header(incl QMAP hdr) to this payload.
             DSM item should be allocated by DS/client and must also include an offset
             of A2_COMMON_QMAP_CONTROL_HDR_SIZE (12 bytes) in the beginning of the
             item for inserting the header. This is needed to avoid chaining of header
             and payload
 
    is_retx: Set if this is a re-transmission of an already sent command.
 
    @return
    0 - A2_SUCCESS
    1 - A2_ERROR_FAILURE
*/
/*=========================================================================*/
a2_error_e    a2_dl_per_send_qmap_ctrl_cmd
(
  sio_port_id_type                port_id,
  uint8                           mux_id,
  uint8                           cmd_name,   
  a2_common_qmap_ctrl_cmd_type_e  cmd_type,
  dsm_item_type*                  dsm_ptr,
  boolean                         is_retx,
  uint32                          trans_id
);

/*==============================================================================

  FUNCTION:  a2_dl_per_notify_call_state ARS: TO BE REMOVED (DS DEP)

==============================================================================*/
/*!
  @brief
  API to convey to A2 the nature of call and the state of call
  Called by DS to indicate if a tethered call is UP/DOWN

*/
/*============================================================================*/
void a2_dl_per_notify_call_state
(
  a2_dl_per_call_state_e   call_state,
  sio_port_id_type         port_id
);
#endif /* A2_DL_PER_H */

