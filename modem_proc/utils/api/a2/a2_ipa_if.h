#ifndef A2_IPA_IF_H
#define A2_IPA_IF_H
/*!
  @file a2_ipa_if.h

  @brief
   Interface file between A2 and IPA.

   This is a temporary interface file introduced for jolokia to adapt
   to a Bolt based data services SW. Since Bolt has IPA driver on the PER
   and Jolokia is a DIME based chip, it is necessary for an intermediate
   interface file that preserves the IPA APIs that DS calls and translate
   them to A2 APIs
 
  @ingroup a2_ext_interface
*/

/*==============================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly
  reserved. By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_ipa_if.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/06/16   am      CR996933 HFN-RESYNC FR 24395 
12/19/14   yjz     CR772093: Add api for ACC_DPL
07/14/14   ars     Initial version 

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <sio.h>

/*==============================================================================

                               MACROS

==============================================================================*/
/*! @brief Ethernet Header Length (in bytes) */
#define IPA_SIO_ETHERNET_HDR_LEN   14

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief IPA return error codes */
typedef enum
{
  /*! Common Error Codes */
  IPA_SUCCESS = 0,
  IPA_ERR_INVALID_ARGUMENT,
  IPA_ERR_LOGICAL_SIO_STREAM_UNKNOWN,
  IPA_ERR_PHYSICAL_SIO_STREAM_UNKNOWN,
  IPA_ERR_PERIPHERAL_PIPES_NOT_FOUND,
  /*! UL Error Codes */
  IPA_ERR_UL_AGGR_HDR_TYPE_NOT_SUPPORTED,
  IPA_ERR_UL_AGGR_PARAMS_INCORRECT,
  /*! DL Error Codes */
  IPA_ERR_DL_AGGR_HDR_TYPE_NOT_SUPPORTED,
  IPA_ERR_DL_AGGR_PARAMS_INCORRECT,
  IPA_ERR_DL_HDR_TYPE_NOT_SUPPORTED,
  /*! IP Filtering Error Codes */
  IPA_ERR_IPFLTR_COMMIT_FAIL,
  IPA_ERR_IPFLTR_RULE_TBL_PENDING_ACTIVATION,
  IPA_ERR_IPFLTR_RULE_TBL_PENDING_REMOTE_ACTIVATION,
  IPA_ERR_IPFLTR_REMOVE_FAIL,
  /*! Generic error */
  IPA_ERR_FAIL,
  IPA_ERR_CODE_MAX
} ipa_err_code_e;

/*! @brief enum: Header Protocols type */
typedef enum
{
  IPA_SIO_HDR_TYPE_NONE = 0,
  IPA_SIO_HDR_TYPE_ETHERNET,
  IPA_SIO_HDR_TYPE_QMAP,
  IPA_SIO_HDR_TYPE_QMAP_V2,
  IPA_SIO_HDR_TYPE_QMAP_V3,
  IPA_SIO_HDR_TYPE_QCNCM,
  IPA_SIO_HDR_TYPE_MBIM,
  IPA_SIO_HDR_TYPE_DIRECT_DMA,
  IPA_SIO_HDR_TYPE_RNDIS,
  IPA_SIO_HDR_TYPE_MAX
} ipa_sio_hdr_type_e;

/*! @brief struct: QMAP Aggregation Parameters */
typedef struct
{
  uint32 max_aggr_bytes;
  uint16 max_aggr_pkts;
} ipa_sio_qmap_params_s;

/*! @brief struct: QMAPv2 Aggregation Parameters */
typedef  ipa_sio_qmap_params_s ipa_sio_qmapv2_params_t;

/*! @brief struct: QMAPv3 Aggregation Parameters */
typedef  ipa_sio_qmap_params_s ipa_sio_qmapv3_params_t;

/*! @brief struct: MBIM Aggregation/Header Parameters */
typedef struct
{
  uint32 max_aggr_bytes;
  uint32 ncm_sig;
  uint32 ndp_sig;
  uint16 max_aggr_pkts;
} ipa_sio_mbim_params_s;

/*! @brief struct: QCNCM Aggregation Parameters */
typedef ipa_sio_mbim_params_s ipa_sio_qcncm_params_t;

/*! @brief struct: Ethernet Aggregation/Header Parameters */
typedef struct
{
  uint32 max_aggr_bytes;
  uint16 max_aggr_pkts;
  uint8  ipv4_hdr_present; /* Flag indicating IPv4 Ethernet Header is present or not */
  uint8  ipv6_hdr_present; /* Flag indicating IPv6 Ethernet Header is present or not */
  uint8  ipv4_hdr[IPA_SIO_ETHERNET_HDR_LEN]; /* IPv4 Ethernet Header in Network Byte order */
  uint8  ipv6_hdr[IPA_SIO_ETHERNET_HDR_LEN]; /* IPv6 Ethernet Header in Network Byte order */
} ipa_sio_ethernet_params_s;

/*! @brief union: Aggregation/Header Parameters */
typedef union
{
  ipa_sio_qmap_params_s     qmap;
  ipa_sio_qmapv3_params_t   qmapv2;
  ipa_sio_qmapv3_params_t   qmapv3; 
  ipa_sio_mbim_params_s     mbim;
  ipa_sio_qcncm_params_t    qcncm;
  ipa_sio_ethernet_params_s ether;
} ipa_sio_config_params_u;

/*! @brief struct: Downlink aggregation/header Configuration */
typedef struct
{
  ipa_sio_config_params_u params;
  ipa_sio_hdr_type_e      hdr_type;
  /* Guaranteed trailer byte length */
  uint32                  trailer_pad_len; 
} ipa_sio_dl_config_s;

/*! @brief struct: Uplink aggregation/header Configuration */
typedef struct
{
  ipa_sio_config_params_u params;
  ipa_sio_hdr_type_e      hdr_type;
  boolean                 is_qos_present;     
} ipa_sio_ul_config_s;

/*! @brief struct: SIO endpoint Configuration */
typedef struct
{
  ipa_sio_dl_config_s dl_config;
  ipa_sio_ul_config_s ul_config;
} ipa_sio_endpoint_config_s;

typedef enum
{
  IPA_SIO_CONFIG_ACTION_ADD = 0,
  IPA_SIO_CONFIG_ACTION_ETHERNET_HDR_UPDATE
} ipa_sio_config_action_e;

/*! @brief struct: SIO Configuration mapping received from DS */
typedef struct
{
  uint8                     mux_id;
  uint8                     mux_override;
  sio_stream_id_type        physical_sio_stream;
  ipa_sio_endpoint_config_s *aggr_hdr_config_ptr;
  dsm_watermark_type        *sio_tx_wm_ptr;
  dsm_watermark_type        *sio_rx_wm_ptr;
} ipa_sio_config_map_s;

/*! Enum to disable/enable accelerated DPL */
typedef enum
{
  IPA_ACC_DPL_STATE_DISABLE,
  IPA_ACC_DPL_STATE_ENABLE
} ipa_acc_dpl_state_e;

/*! @brief Enum to capture the direction of DPL in acc_dpl_tx API */
typedef enum
{
  IPA_ACC_DPL_DIR_DL,
  IPA_ACC_DPL_DIR_UL
} ipa_acc_dpl_dir_e;

/*! Parameters passed in the DPL callback functio */
typedef struct
{
  /*! Whether the accelerated DPL path is available or not  */
  uint8  is_available;
  /*! DPL version currently in use */
  uint8  version;
} ipa_acc_dpl_cb_fn_param_s;

/*! @brief Callback function signature to be called by the driver once the 
  accelerated DPL path is avaiable/unavailable */
typedef void (*ipa_acc_dpl_cb_fn_type)
(
  ipa_acc_dpl_cb_fn_param_s *params_ptr
);
/*! @brief struct: Peripheral Endpoint mapping */
typedef struct 
{
  uint8   per_cons_pipe_id;
  uint8   per_prod_pipe_id;
  uint32  per_ep_id;
  dsm_watermark_type  *phy_tx_wm_ptr;
  dsm_watermark_type  *phy_rx_wm_ptr;
} ipa_sio_per_ep_map_s;

/*! @brief QMAP Cmd Names */
typedef enum
{
  IPA_QMAP_CMD_NAME_MIN,
  /*! Qmap Flow disable cmd */
  IPA_QMAP_CMD_NAME_FLOW_DIS,
  /*! Qmap Flow enable cmd */
  IPA_QMAP_CMD_NAME_FLOW_EN,
  IPA_QMAP_CMD_NAME_MAX
} ipa_qmap_cmd_name_e;

/*! @brief enum: to indicate Flow Action or current Flow State */
typedef enum
{
  IPA_FLOW_STATE_DISABLE = 0,
  IPA_FLOW_STATE_ENABLE
} ipa_flow_state_e;

/*! @brief Function prototype to be triggered by IPA for
  detecting potential HFN mismatch.
  recover_success: TRUE whenever number of consecutive valid packets cross the
                   configured threshold after an HFN excpetion has occured.
                   FALSE when the number of HFN exceptions
                   cross configured threshold
  */
typedef void (*ipa_wan_bearer_err_ind_cb_type)
(
 /*! uid on which the error is detected. */
  uint8 uid,
  /* recover_success to indicate whether HFN exception has been recovered or not */
  boolean recover_success
);

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
/*=============================================================================

  FUNCTION:  IPA_SIO_SET_PER_EP_MAPPING

=============================================================================*/
/*!
    @brief
    This API is invoked to set the mapping of physical SIO stream to peripheral
    endpoint id and peripheral pipe pair. 
 
    @return
    None
*/
/*===========================================================================*/
ipa_err_code_e  ipa_sio_set_per_ep_mapping
(
  sio_stream_id_type    phys_stream_id,  
  ipa_sio_per_ep_map_s* per_ep_map_ptr
);

/*===========================================================================

  FUNCTION:  ipa_sio_set_sio_config_mapping

===========================================================================*/
/*!
    @brief
    This API is invoked by DS to set the configuration on the given logical
    SIO channel.
    This function assumes the caller has allocated the memory for the
    argument and will free the memory when the function returns.
 
    @return
    IPA_SUCCESS, when all params passed error checking
    IPA_ERR_XXX, to specify the first error encountered while parsing the
                 parameters
*/ 
/*=========================================================================*/
ipa_err_code_e ipa_sio_set_sio_config_mapping 
(
  ipa_sio_config_action_e action,
  sio_stream_id_type      logical_stream_id,  
  ipa_sio_config_map_s    *config_map_ptr
);

/*===========================================================================

  FUNCTION:  IPA_SIO_GET_UL_AGGR_PROT_SUPPORT

===========================================================================*/
/*!
    @brief
    This API is invoked by DS to check if the given UL aggregation
    protocol is supported by IPA.
 
    @return
 
    IPA_SUCCESS, when both UL aggr protocol is supported.
    IPA_ERR_UL_AGGR_TYPE_NOT_SUPPORTED, if UL aggr type is not supported.
*/ 
/*=========================================================================*/
ipa_err_code_e  ipa_sio_get_ul_aggr_prot_support
(
  uint32             per_ep_id,
  ipa_sio_hdr_type_e ul_aggr_prot,
  uint8              *max_ul_aggr_pkts,
  uint32             *max_ul_aggr_bytes
);

/*===========================================================================

  FUNCTION:  IPA_SIO_GET_DL_AGGR_PROT_SUPPORT

===========================================================================*/
/*!
    @brief
    This API is invoked by DS to check if the given DL aggregation
    protocol is supported by IPA.
 
    @return
 
    IPA_SUCCESS, when DL aggr protocol is supported.
    IPA_ERR_DL_AGGR_TYPE_NOT_SUPPORTED, if DL aggr type is not supported.
*/ 
/*=========================================================================*/
ipa_err_code_e  ipa_sio_get_dl_aggr_prot_support
(
  uint32             per_ep_id,
  ipa_sio_hdr_type_e dl_aggr_prot,
  uint8              *max_dl_aggr_pkts,
  uint32             *max_dl_aggr_bytes,
  uint32             *trailer_pad_len
);

/*=============================================================================

  FUNCTION:  ipa_sio_send_qmap_cmd_req

=============================================================================*/
/*!
    @brief
    This API is invoked by DS to send a QMAP cmd request to a given
    peripheral endpoint
 
    @param sio_stream_id, Stream id on which the QMAP cmd is sent
    @param cmd_name, Cmd Name request to send
    @param cmd_payload_ptr, Cmd payload dsm pointer
    @param trans_id, Transaction ID - Will be filled by the driver for new
                                      requests.
                                      Needs to be filled by the client for
                                      retransmissions
    @param is_retx, Boolean to indicate whether it is a retransmission or not

    @return Following can be the possible values in the return code:
 
    @retval IPA_SUCCESS
    @retval IPA_ERR_LOGICAL_SIO_STREAM_UNKNOWN, when sio_stream id is unknown
    @retval IPA_ERR_FAIL, Internal failure in sending the QMAP cmd    
*/ 
/*===========================================================================*/
ipa_err_code_e ipa_sio_send_qmap_cmd_req
(
  sio_stream_id_type  sio_stream_id, 
  ipa_qmap_cmd_name_e cmd_name,
  dsm_item_type       *cmd_payload_ptr,
  uint32              *trans_id,
  uint8               is_retx  
);

/*=============================================================================

  FUNCTION:  ipa_sio_set_ul_phys_flow_control

=============================================================================*/
/*!
    @brief
    This API is invoked by DS to flow control the UL traffic on a Phys SIO stream
    on which the watermark level has hit high threshold.

    @param phys_stream_id, Phys stream id corresp to phys sio watermark which 
                           needs to be flow controlled.
    @param flow_action,   0 if flow disable is requested, 1 if flow enable is
                          requested

    @return Following can be the possible values in the return code:

    @retval IPA_SUCCESS
    @retval IPA_ERR_LOGICAL_SIO_STREAM_UNKNOWN, when sio_stream id is unknown
    @retval IPA_ERR_UL_FLOW_ALREADY_ENABLED,    when flow_action requests for
                                                enabling the flow and flow is
                                                already enabled on the given
                                                SIO stream.
    @retval IPA_ERR_UL_FLOW_ALREADY_DISABLED,   when flow_action requests for
                                                disabling the flow and flow
                                                is already disabled on the
                                                given SIO stream.
*/
/*===========================================================================*/
ipa_err_code_e ipa_sio_set_ul_phys_flow_control
(
  sio_stream_id_type phys_stream_id,
  ipa_flow_state_e   flow_action
);
/*===========================================================================

  FUNCTION:  IPA_SIO_CONFIG_DPL

===========================================================================*/
/*!
    @brief
    This API is invoked by DS to configure accelerated DPL on the given
    SIO port.
    The DPL IID parameters for this SIO port needs to be
    passed to the driver for accelerated DPL

    @param sio_stream_id, SIO stream ID of the logical port
    @param ifname, Interface name associated to this SIO port's DPL IID
    @param num_bytes, Number of DPL bytes to be logged 

    @return Following can be possible values in the return code:

    @retval IPA_SUCCESS, when DPL config for the SIO stream ID is successful.
    @retval IPA_ERR_INVALID_ARGUMENT, when SIO stream ID is invalid
*/ 
/*=========================================================================*/
ipa_err_code_e ipa_sio_config_dpl
(
  sio_stream_id_type  sio_stream_id,
  uint8               ifname,
  uint16              num_bytes 
);

/*===========================================================================

  FUNCTION:  IPA_WAN_DS_REGISTER_DPL_WM

===========================================================================*/
/*!
    @brief
    This API is invoked by DS to register DPL watermark with IPA driver.

    @return Following can be possible values in the return code:
 
    @retval IPA_SUCCESS, when DPL wm registration is successful
    @retval IPA_ERR_INVALID_ARGUMENT, when DPL watermark is invalid. 
*/ 
/*=========================================================================*/
void ipa_wan_ds_register_dpl_wm
(
  dsm_watermark_type *dpl_wm_ptr
);

/*===========================================================================

  FUNCTION:  IPA_WAN_DS_CONFIG_DPL

===========================================================================*/
/*!
    @brief
    This API is invoked by DS to configure DPL on the given bearer.
    The DPL IID parameters for this bearer needs to be
    passed to the driver for accelerated DPL

    @param uid, UID of the bearer 
    @param ifname, Interface name associated to this bearer's DPL IID
    @param num_bytes, Number of DPL bytes to be logged (Only for legacy DPL)
    @param cb_data, CB data (Only for legacy DPL)

    @return Following can be possible values in the return code:

    @retval IPA_SUCCESS, when DPL config for the bearer is successful.
    @retval IPA_ERR_INVALID_ARGUMENT, when uid is invalid. 
*/ 
/*=========================================================================*/
ipa_err_code_e ipa_wan_ds_config_dpl
(
  uint8  uid,
  uint8  ifname,
  uint16 num_bytes,
  uint32 cb_data
);

/*=============================================================================

  FUNCTION:  ipa_acc_dpl_reg_cb

=============================================================================*/
/*!
    @brief
    This API is invoked by clients to register for notification cb from IPA
    driver when accelerated DPL path is available
 
    @param acc_dpl_cb_fn, Callback function to be invoked when the driver
                          activates the accelerated DPL path

    @return Following can be the possible values in the return code:

    @retval IPA_SUCCESS
    @retval IPA_ERR_FAIL, Internal API failure
*/ 
/*===========================================================================*/
ipa_err_code_e ipa_acc_dpl_reg_cb
(
  ipa_acc_dpl_cb_fn_type acc_dpl_cb_fn
);

/*=============================================================================

  FUNCTION:  ipa_acc_dpl_configure

=============================================================================*/
/*!
    @brief
    This API is invoked by DS to enable/disable the accelerated DPL path.
    This state is maintained across the cable disconnect
 
    @param state, Accelerated DPL state 

    @return Following can be the possible values in the return code:

    @retval IPA_SUCCESS
    @retval IPA_ERR_FAIL, Internal API failure
*/ 
/*===========================================================================*/
ipa_err_code_e ipa_acc_dpl_configure
(
  ipa_acc_dpl_state_e state
);

/*=============================================================================

  FUNCTION:  ipa_acc_dpl_tx

=============================================================================*/
/*!
    @brief
    This API is invoked by clients to send a DPL log packet via the accelerated
    DPL path.

    @param dir, Direction with this packet's DPL IID
    @param ifname, Interface name associated with this packet's DPL IID
    @param pkt_len, Total Length of the packet to be logged
    @param pkt_ptr, DSM pointer of the packet

    @return Following can be the possible values in the return code:

    @retval IPA_SUCCESS
    @retval IPA_ERR_INVALID_ARGUMENT, when payload_ptr is NULL or payload_len = 0
    @retval IPA_ERR_FAIL, Internal API failure
*/ 
/*===========================================================================*/
ipa_err_code_e ipa_acc_dpl_tx
(
  ipa_acc_dpl_dir_e dir,
  uint8             ifname, 
  uint16            pkt_len,
  dsm_item_type     *pkt_dsm_ptr 
);

/*=============================================================================

  FUNCTION:  ipa_wan_pdcp_register_bearer_err_ind

=============================================================================*/
/*!
    @brief
    This API will send request to register error callback 
    function if there is malformed packet in DL
*/ 
/*===========================================================================*/
void ipa_wan_register_bearer_err_ind
(
  uint32 err_pkt_threshold,
  ipa_wan_bearer_err_ind_cb_type err_ind_cb_fn_ptr
);

/*=============================================================================

  FUNCTION:  ipa_wan_deregister_bearer_err_ind

=============================================================================*/
/*!
    @brief
    This API will send derequest to register error callback 
    function if there is malformed packet in DL
*/ 
/*===========================================================================*/
void ipa_wan_deregister_bearer_err_ind
(
  void
);

#endif /* A2_IPA_IF_H */


