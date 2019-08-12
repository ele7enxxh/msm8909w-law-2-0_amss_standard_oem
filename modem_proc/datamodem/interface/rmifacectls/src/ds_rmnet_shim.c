/*===========================================================================

                D S _ R M N E T _ S H I M . C

DESCRIPTION

  RMNET physical device source file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2005-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/src/ds_rmnet_shim.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/25/14    wc     Enable QMAPv3
05/24/13    wc     Initial file
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ds_rmnet_shim.h"
#include "ds_rmnet_defs.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_qmap.h"

#ifdef FEATURE_DATA_IPA
#include "ipa.h"
#endif /* FEATURE_DATA_IPA */
#ifdef FEATURE_DATA_A2
#include "a2_ipa_if.h"
#include "a2_ul_per.h"
#include "a2_sio_ext.h"
#endif /* FEATURE_DATA_A2 */

/*===========================================================================

                                DATA TYPES

===========================================================================*/


/*===========================================================================

                                 DEFINITIONS

===========================================================================*/

// port usage masks
static uint32 rmnet_logical_port_mask = 0;
static uint32 rmnet_phys_port_mask = 0;

#if defined(FEATURE_DATA_IPA)
sio_port_id_type rmnet_phys_ports[] = {
                         SIO_PORT_IPA_DS_PHYS_1,
                         SIO_PORT_IPA_DS_PHYS_2,
                         SIO_PORT_IPA_DS_PHYS_3,
                         SIO_PORT_IPA_DS_PHYS_4,
                         SIO_PORT_IPA_DS_PHYS_5,
                         SIO_PORT_IPA_DS_PHYS_6,
                         SIO_PORT_IPA_DS_PHYS_7,
                         SIO_PORT_IPA_DS_PHYS_8};

sio_port_id_type rmnet_logical_ports[] = {
                         SIO_PORT_IPA_DS_CH_1,
                         SIO_PORT_IPA_DS_CH_2,
                         SIO_PORT_IPA_DS_CH_3,
                         SIO_PORT_IPA_DS_CH_4,
                         SIO_PORT_IPA_DS_CH_5,
                         SIO_PORT_IPA_DS_CH_6,
                         SIO_PORT_IPA_DS_CH_7,
                         SIO_PORT_IPA_DS_CH_8,
                         SIO_PORT_IPA_DS_CH_9,
                         SIO_PORT_IPA_DS_CH_10,
                         SIO_PORT_IPA_DS_CH_11,
                         SIO_PORT_IPA_DS_CH_12,
                         SIO_PORT_IPA_DS_CH_13,
                         SIO_PORT_IPA_DS_CH_14,
                         SIO_PORT_IPA_DS_CH_15,
                         SIO_PORT_IPA_DS_CH_16,
                         SIO_PORT_IPA_DS_CH_17,
                         SIO_PORT_IPA_DS_CH_18,
                         SIO_PORT_IPA_DS_CH_19,
                         SIO_PORT_IPA_DS_CH_20};

#define rmnet_shim_is_ipa_a2_port(sio_port) \
           (SIO_MAJOR_NUM_OF_DEVICE((sio_port)) == SIO_MAJOR_IPA) 

#elif defined(FEATURE_DATA_A2)
sio_port_id_type rmnet_phys_ports[] = {
                         SIO_PORT_A2_DS_PHYS_1,
                         SIO_PORT_A2_DS_PHYS_2,
                         SIO_PORT_A2_DS_PHYS_3,
                         SIO_PORT_A2_DS_PHYS_4};

sio_port_id_type rmnet_logical_ports[] = {
                         SIO_PORT_A2_DS_CH_1,
                         SIO_PORT_A2_DS_CH_2,
                         SIO_PORT_A2_DS_CH_3,
                         SIO_PORT_A2_DS_CH_4,
                         SIO_PORT_A2_DS_CH_5,
                         SIO_PORT_A2_DS_CH_6,
                         SIO_PORT_A2_DS_CH_7,
                         SIO_PORT_A2_DS_CH_8,
                         SIO_PORT_A2_DS_CH_9,
                         SIO_PORT_A2_DS_CH_10,
                         SIO_PORT_A2_DS_CH_11,
                         SIO_PORT_A2_DS_CH_12,
                         SIO_PORT_A2_DS_CH_13,
                         SIO_PORT_A2_DS_CH_14,
                         SIO_PORT_A2_DS_CH_15,
                         SIO_PORT_A2_DS_CH_16,
                         SIO_PORT_A2_DS_CH_17,
                         SIO_PORT_A2_DS_CH_18,
                         SIO_PORT_A2_DS_CH_19,
                         SIO_PORT_A2_DS_CH_20};

#define rmnet_shim_is_ipa_a2_port(sio_port) \
           (SIO_MAJOR_NUM_OF_DEVICE((sio_port)) == SIO_MAJOR_A2) 
#else
sio_port_id_type rmnet_phys_ports[] = {SIO_PORT_NULL};
sio_port_id_type rmnet_logical_ports[] = {SIO_PORT_NULL};

#endif

#define RMNET_NUM_PHYS_PORTS \
          (sizeof(rmnet_phys_ports)/sizeof(rmnet_phys_ports[0]))

#define RMNET_NUM_LOGICAL_PORTS \
          (sizeof(rmnet_logical_ports)/sizeof(rmnet_logical_ports[0]))

/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/


/*===========================================================================

                           INTERNAL FUNCTIONS

===========================================================================*/

#if defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2)

/*===========================================================================
FUNCTION RMNET_SHIM_AGG_TO_IPA_HDR

DESCRIPTION
  This function converts RMNET aggregation protocol to IPA header type

PARAMETERS
  rmnet_agg:  rmnet aggregation protocol

DEPENDENCIES
  None

RETURN VALUE
  IPA header type

SIDE EFFECTS
  None.
===========================================================================*/
static ipa_sio_hdr_type_e rmnet_shim_agg_to_ipa_hdr
(
  rmnet_data_agg_enum_type rmnet_agg
)
{
  ipa_sio_hdr_type_e  ipa_hdr = IPA_SIO_HDR_TYPE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (rmnet_agg)
  {
    case RMNET_ENABLE_DATA_AGG_QC_NCM:
      ipa_hdr = IPA_SIO_HDR_TYPE_QCNCM;
      break;

    case RMNET_ENABLE_DATA_AGG_MBIM:
      ipa_hdr = IPA_SIO_HDR_TYPE_MBIM;
      break;

    case RMNET_ENABLE_DATA_AGG_QMAP:
      ipa_hdr = IPA_SIO_HDR_TYPE_QMAP;
      break;

    case RMNET_ENABLE_DATA_AGG_QMAP_V3:
      ipa_hdr = IPA_SIO_HDR_TYPE_QMAP_V3;
      break;

    default:
      ipa_hdr = IPA_SIO_HDR_TYPE_NONE;
      break;
  }

  return ipa_hdr;

} /* rmnet_shim_agg_to_ipa_hdr() */

/*===========================================================================
FUNCTION RMNET_SHIM_DATA_FORMAT_TO_IPA_CONFIG

DESCRIPTION
  This function converts RMNET data format to IPA config

PARAMETERS
  data_format:  rmnet data format
  ipa_config:   IPA config 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void rmnet_shim_data_format_to_ipa_config
(
  rmnet_data_format_type     * data_format,
  ipa_sio_endpoint_config_s  * ipa_config
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (data_format == NULL || ipa_config == NULL)
  {
    return;
  }

  memset(ipa_config, 0, sizeof(ipa_sio_endpoint_config_s));

  if (data_format->link_prot == RMNET_ETHERNET_MODE)
  {
    ipa_config->ul_config.hdr_type = IPA_SIO_HDR_TYPE_ETHERNET;
    ipa_config->dl_config.hdr_type = IPA_SIO_HDR_TYPE_ETHERNET;
  }
  else
  {
    ipa_config->ul_config.hdr_type = 
            rmnet_shim_agg_to_ipa_hdr(data_format->ul_data_agg_protocol);
    ipa_config->dl_config.hdr_type =
            rmnet_shim_agg_to_ipa_hdr(data_format->dl_data_agg_protocol);
  }

  // Check if we need to take SW MBIM path on UL
  if ( (ipa_config->ul_config.hdr_type == IPA_SIO_HDR_TYPE_MBIM) &&
       (data_format->mbim_sw_deagg) )
  {
    ipa_config->ul_config.hdr_type = IPA_SIO_HDR_TYPE_DIRECT_DMA;
  }

  // Check if QoS is enabled
  if (data_format->qos)
  {
    ipa_config->ul_config.is_qos_present = TRUE;
  }

  switch (ipa_config->ul_config.hdr_type)
  {
    case IPA_SIO_HDR_TYPE_ETHERNET:
      ipa_config->ul_config.params.ether.max_aggr_pkts =
                         (uint16)data_format->ul_data_agg_max_num;
      ipa_config->ul_config.params.ether.max_aggr_bytes =
                         data_format->ul_data_agg_max_size;
      break;

    case IPA_SIO_HDR_TYPE_QMAP:
      ipa_config->ul_config.params.qmap.max_aggr_pkts =
                         (uint16)data_format->ul_data_agg_max_num;
      ipa_config->ul_config.params.qmap.max_aggr_bytes =
                         data_format->ul_data_agg_max_size;
      break;

    case IPA_SIO_HDR_TYPE_QMAP_V3:
      ipa_config->ul_config.params.qmapv3.max_aggr_pkts =
                         (uint16)data_format->ul_data_agg_max_num;
      ipa_config->ul_config.params.qmapv3.max_aggr_bytes =
                         data_format->ul_data_agg_max_size;
      break;

    case IPA_SIO_HDR_TYPE_QCNCM:
      ipa_config->ul_config.params.qcncm.max_aggr_pkts =
                         (uint16)data_format->ul_data_agg_max_num;
      ipa_config->ul_config.params.qcncm.max_aggr_bytes =
                         data_format->ul_data_agg_max_size;
      ipa_config->ul_config.params.qcncm.ncm_sig = data_format->ncm_signature;
      ipa_config->ul_config.params.qcncm.ndp_sig = data_format->ndp_signature;
      break;

    case IPA_SIO_HDR_TYPE_MBIM:
    case IPA_SIO_HDR_TYPE_DIRECT_DMA:
      ipa_config->ul_config.params.mbim.max_aggr_pkts =
                         (uint16)data_format->ul_data_agg_max_num;
      ipa_config->ul_config.params.mbim.max_aggr_bytes =
                         data_format->ul_data_agg_max_size;
      ipa_config->ul_config.params.mbim.ncm_sig = data_format->ncm_signature;
      ipa_config->ul_config.params.mbim.ndp_sig = data_format->ndp_signature;
      break;

    default:
      break;
  }

  switch (ipa_config->dl_config.hdr_type)
  {
    case IPA_SIO_HDR_TYPE_ETHERNET:
      ipa_config->dl_config.params.ether.max_aggr_pkts =
                         (uint16)data_format->dl_data_agg_max_num;
      ipa_config->dl_config.params.ether.max_aggr_bytes =
                         data_format->dl_data_agg_max_size;
      break;

    case IPA_SIO_HDR_TYPE_QMAP:
      ipa_config->dl_config.params.qmap.max_aggr_pkts =
                         (uint16)data_format->dl_data_agg_max_num;
      ipa_config->dl_config.params.qmap.max_aggr_bytes =
                         data_format->dl_data_agg_max_size;
      ipa_config->dl_config.trailer_pad_len = 
                         data_format->dl_min_padding;
      break;

    case IPA_SIO_HDR_TYPE_QMAP_V3:
      ipa_config->dl_config.params.qmapv3.max_aggr_pkts =
                         (uint16)data_format->dl_data_agg_max_num;
      ipa_config->dl_config.params.qmapv3.max_aggr_bytes =
                         data_format->dl_data_agg_max_size;
      ipa_config->dl_config.trailer_pad_len = 
                         data_format->dl_min_padding;
      break;

    case IPA_SIO_HDR_TYPE_QCNCM:
      ipa_config->dl_config.params.qcncm.max_aggr_pkts =
                         (uint16)data_format->dl_data_agg_max_num;
      ipa_config->dl_config.params.qcncm.max_aggr_bytes =
                         data_format->dl_data_agg_max_size;
      ipa_config->dl_config.params.qcncm.ncm_sig = data_format->ncm_signature;
      ipa_config->dl_config.params.qcncm.ndp_sig = data_format->ndp_signature;
      break;

    case IPA_SIO_HDR_TYPE_MBIM:
      ipa_config->dl_config.params.mbim.max_aggr_pkts =
                         (uint16)data_format->dl_data_agg_max_num;
      ipa_config->dl_config.params.mbim.max_aggr_bytes =
                         data_format->dl_data_agg_max_size;
      ipa_config->dl_config.params.mbim.ncm_sig = data_format->ncm_signature;
      ipa_config->dl_config.params.mbim.ndp_sig = data_format->ndp_signature;
      break;

    default:
      break;
  }

} /* rmnet_shim_data_format_to_ipa_config() */

#endif /* defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2) */

#ifdef FEATURE_DATA_IPA

/*===========================================================================
FUNCTION RMNET_SHIM_IPA_QMAP_CMD_RESP_CB

DESCRIPTION
  This is the callback invoked when QMAP cmd response is received

PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void rment_shim_ipa_qmap_cmd_resp_cb
(
  sio_stream_id_type     stream_id,
  ipa_qmap_hdr_s       * qmap_hdr_ptr,
  ipa_qmap_cmd_info_s  * qmap_cmd_info_ptr,
  dsm_item_type        * payload_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (qmap_cmd_info_ptr != NULL && payload_ptr != NULL &&
      qmap_cmd_info_ptr->type == IPA_QMAP_CMD_TYPE_ACK)
  {
    ds_qmap_cmd_resp_cb(
              stream_id,
              (uint8)qmap_cmd_info_ptr->name,
              (uint8)qmap_cmd_info_ptr->type,
              qmap_cmd_info_ptr->trans_id,
              payload_ptr);
  }
  else
  {
    ASSERT(0);
  }

} /* rment_shim_ipa_qmap_cmd_resp_cb() */
#endif /* FEATURE_DATA_IPA */

#ifdef FEATURE_DATA_A2

/*===========================================================================
FUNCTION RMNET_SHIM_A2_QMAP_CMD_RESP_CB

DESCRIPTION
  This is the callback invoked when QMAP cmd response is received

PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void rmnet_shim_a2_qmap_cmd_resp_cb
(
  sio_stream_id_type              stream_id,
  uint8                           mux_id,
  uint8                           cmd_name,   
  a2_common_qmap_ctrl_cmd_type_e  cmd_type,
  dsm_item_type*                  payload_ptr,
  uint32                          trans_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (payload_ptr != NULL &&
      cmd_type == A2_COMMON_QMAP_CTRL_CMD_ACK)
  {
    ds_qmap_cmd_resp_cb(
              stream_id,
              (uint8)cmd_name,
              (uint8)cmd_type,
              trans_id,
              payload_ptr);
  }
  else
  {
    ASSERT(0);
  }

} /* rment_shim_a2_qmap_cmd_resp_cb() */
#endif /* FEATURE_DATA_A2 */

/*===========================================================================

                           EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_SHIM_GET_PHYS_SIO_PORT()

  DESCRIPTION
    This function assigns a physical SIO port

  PARAMETERS
    None

  RETURN VALUE
    Physical SIO port

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
sio_port_id_type rmnet_shim_get_phys_sio_port
(
  void
)
{
  int  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < RMNET_NUM_PHYS_PORTS; i++)
  {
    if ((rmnet_phys_port_mask & (1 << i)) == 0)
    {
      rmnet_phys_port_mask |= (1 << i);
      return rmnet_phys_ports[i];
    }
  }

  return SIO_PORT_NULL;

} /* rmnet_shim_get_phys_sio_port() */

/*===========================================================================
  FUNCTION RMNET_SHIM_FREE_PHYS_SIO_PORT()

  DESCRIPTION
    This function frees a physical SIO port

  PARAMETERS
    sio_port: physical SIO port id 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_shim_free_phys_sio_port
(
  sio_port_id_type  sio_port
)
{
  int  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < RMNET_NUM_PHYS_PORTS; i++)
  {
    if (rmnet_phys_ports[i] == sio_port)
    {
      if ((rmnet_phys_port_mask & (1 << i)) == 0)
      {
        LOG_MSG_ERROR_1("SIO port 0x%x not in use", sio_port);
      }
      rmnet_phys_port_mask &= ~(1 << i);
      return;
    }
  }

  LOG_MSG_ERROR_1("SIO port 0x%x not found", sio_port);

} /* rmnet_shim_free_phys_sio_port() */

/*===========================================================================
  FUNCTION RMNET_SHIM_GET_LOGICAL_SIO_PORT()

  DESCRIPTION
    This function assigns a logical SIO port

  PARAMETERS
    None

  RETURN VALUE
    Logical SIO port

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
sio_port_id_type rmnet_shim_get_logical_sio_port
(
  void
)
{
  int  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < RMNET_NUM_LOGICAL_PORTS; i++)
  {
    if ((rmnet_logical_port_mask & (1 << i)) == 0)
    {
      rmnet_logical_port_mask |= (1 << i);
      return rmnet_logical_ports[i];
    }
  }

  return SIO_PORT_NULL;

} /* rmnet_shim_get_logical_sio_port() */

/*===========================================================================
  FUNCTION RMNET_SHIM_FREE_LOGICAL_SIO_PORT()

  DESCRIPTION
    This function frees a logical SIO port

  PARAMETERS
    sio_port: logical SIO port id 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_shim_free_logical_sio_port
(
  sio_port_id_type  sio_port
)
{
  int  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < RMNET_NUM_LOGICAL_PORTS; i++)
  {
    if (rmnet_logical_ports[i] == sio_port)
    {
      if ((rmnet_logical_port_mask & (1 << i)) == 0)
      {
        LOG_MSG_ERROR_1("SIO port 0x%x not in use", sio_port);
      }
      rmnet_logical_port_mask &= ~(1 << i);
      return;
    }
  }

  LOG_MSG_ERROR_1("SIO port 0x%x not found", sio_port);

} /* rmnet_shim_free_logical_sio_port() */

/*===========================================================================
  FUNCTION RMNET_SHIM_SET_PER_MAPPING()

  DESCRIPTION
    This function configs driver of SIO to peripheral mapping

  PARAMETERS
    phys_xport:   rmnet phys xport structure

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_set_per_mapping
(
  rmnet_phys_xport_type   * phys_xport
)
{
#if defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2)
  ipa_sio_per_ep_map_s  per_ep_map;
#endif /* defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (phys_xport == NULL)
  {
    return FALSE;
  }

#if defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2)
  if (rmnet_shim_is_ipa_a2_port(phys_xport->sio_port))
  {
    memset(&per_ep_map, 0, sizeof(per_ep_map));
    per_ep_map.per_cons_pipe_id = (uint8)phys_xport->consumer_pipe_num;
    per_ep_map.per_prod_pipe_id = (uint8)phys_xport->producer_pipe_num;
    per_ep_map.per_ep_id        = phys_xport->ep_id;
    per_ep_map.phy_tx_wm_ptr    = &phys_xport->tx_wmk;
    per_ep_map.phy_rx_wm_ptr    = &phys_xport->rx_wmk;
    if (ipa_sio_set_per_ep_mapping(
              phys_xport->stream_id, &per_ep_map) != IPA_SUCCESS)
    {
      LOG_MSG_ERROR_1("Failed to set IPA EP (0x%x) mapping",
                      phys_xport->ep_id);
      return FALSE;
    }
    else
    {
      LOG_MSG_INFO1_1("IPA Per EP 0x%x mapping set", phys_xport->ep_id);
    }
  }
#endif /* defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2) */

  return TRUE;

} /* rmnet_shim_set_per_mapping() */

/*===========================================================================
  FUNCTION RMNET_SHIM_SET_DATA_FORMAT()

  DESCRIPTION
    This function configs driver's data format

  PARAMETERS
    phys_xport:   rmnet phys xport structure
    data_format:  pointer to data format

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_set_data_format
(
  rmnet_phys_xport_type   * phys_xport,
  rmnet_data_format_type  * data_format
)
{
#if defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2)
  ipa_sio_hdr_type_e  ipa_ul_hdr_type;
  ipa_sio_hdr_type_e  ipa_dl_hdr_type;
  uint8               max_ul_aggr_pkts = 0;
  uint32              max_ul_aggr_bytes = 0;
  uint8               max_dl_aggr_pkts = 0;
  uint32              max_dl_aggr_bytes = 0;
  uint32              max_dl_min_padding = 0;
#endif /* defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (phys_xport == NULL || data_format == NULL)
  {
    return FALSE;
  }

#if defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2)
  if (rmnet_shim_is_ipa_a2_port(phys_xport->sio_port))
  {
    if (data_format->link_prot == RMNET_ETHERNET_MODE)
    {
      ipa_ul_hdr_type = IPA_SIO_HDR_TYPE_ETHERNET;
      ipa_dl_hdr_type = IPA_SIO_HDR_TYPE_ETHERNET;
      data_format->ul_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
      data_format->dl_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
    }
    else
    {
      ipa_ul_hdr_type = rmnet_shim_agg_to_ipa_hdr(
                            data_format->ul_data_agg_protocol);
      ipa_dl_hdr_type = rmnet_shim_agg_to_ipa_hdr(
                            data_format->dl_data_agg_protocol);

      if (ipa_ul_hdr_type == IPA_SIO_HDR_TYPE_NONE)
      {
        data_format->ul_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
      }
      if (ipa_dl_hdr_type == IPA_SIO_HDR_TYPE_NONE)
      {
        data_format->dl_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
      }
    }

    data_format->mbim_sw_deagg = FALSE;
    if (ipa_sio_get_ul_aggr_prot_support(
                     phys_xport->ep_id,
                     ipa_ul_hdr_type,
                     &max_ul_aggr_pkts,
                     &max_ul_aggr_bytes) == IPA_SUCCESS)
    {
      data_format->ul_data_agg_max_num  = max_ul_aggr_pkts;
      data_format->ul_data_agg_max_size = max_ul_aggr_bytes;
    }
    else if ( (ipa_ul_hdr_type == IPA_SIO_HDR_TYPE_MBIM) &&
              (ipa_sio_get_ul_aggr_prot_support(
                      phys_xport->ep_id,
                      IPA_SIO_HDR_TYPE_DIRECT_DMA,
                      &max_ul_aggr_pkts,
                      &max_ul_aggr_bytes) == IPA_SUCCESS) )
    {
      // Using MBIM SW deaggregation on UL
      data_format->mbim_sw_deagg = TRUE;
      data_format->ul_data_agg_max_num  = max_ul_aggr_pkts;
      data_format->ul_data_agg_max_size = max_ul_aggr_bytes;
    }
    else
    {
      data_format->ul_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
      data_format->ul_data_agg_max_num  = 0;
      data_format->ul_data_agg_max_size = 0;
    }

    if (ipa_sio_get_dl_aggr_prot_support(
                 phys_xport->ep_id,
                 ipa_dl_hdr_type,
                 &max_dl_aggr_pkts,
                 &max_dl_aggr_bytes,
                 &max_dl_min_padding) != IPA_SUCCESS)
    {
      data_format->dl_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
      data_format->dl_data_agg_max_num  = 0;
      data_format->dl_data_agg_max_size = 0;
      data_format->dl_min_padding = 0;
    }
    else
    {
      if (data_format->dl_data_agg_max_num > max_dl_aggr_pkts)
      {
        data_format->dl_data_agg_max_num  = max_dl_aggr_pkts;
      }
      if (data_format->dl_data_agg_max_size > max_dl_aggr_bytes)
      {
        data_format->dl_data_agg_max_size = max_dl_aggr_bytes;
      }
      if (data_format->dl_min_padding > max_dl_min_padding)
      {
        data_format->dl_min_padding = max_dl_min_padding;
      }
    }

    return TRUE;
  }
#endif /* defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2) */

  data_format->ul_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
  data_format->dl_data_agg_protocol = RMNET_DISABLE_DATA_AGG;
  data_format->dl_data_agg_max_num  = 0;
  data_format->dl_data_agg_max_size = 0;
  data_format->ul_data_agg_max_num  = 0;
  data_format->ul_data_agg_max_size = 0;
  data_format->dl_min_padding       = 0;

  return TRUE;

} /* rmnet_shim_set_data_format() */

/*===========================================================================
  FUNCTION RMNET_SHIM_SET_PORT_MAPPING()

  DESCRIPTION
    This function configs driver of logical to physical port mapping

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
boolean rmnet_shim_set_port_mapping
(
  rmnet_xport_type        * xport_info
)
{
  rmnet_phys_xport_type   * phys_xport;
#if defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2)
  ipa_sio_config_map_s      ipa_port_mapping;
  ipa_sio_endpoint_config_s ipa_port_aggr; 
#endif /* defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!RMNET_XPORT_IS_BOUND(xport_info))
  {
    LOG_MSG_ERROR_0("RmNet xport is not bound");
    return FALSE;
  }

  phys_xport = xport_info->phys_xport;

  if ( !RMNET_XPORT_IS_ACTIVE(xport_info) ||
       !RMNET_PHYS_XPORT_IS_ACTIVE(phys_xport) )
  {
    LOG_MSG_ERROR_0("RMNET xports not active");
    return FALSE;
  }

#if defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2)
  if ( rmnet_shim_is_ipa_a2_port(xport_info->sio_port) &&
       rmnet_shim_is_ipa_a2_port(phys_xport->sio_port) )
  {
    memset(&ipa_port_mapping, 0, sizeof(ipa_port_mapping));

    ipa_port_mapping.mux_id              = xport_info->mux_id;
    ipa_port_mapping.mux_override        = 0;
    ipa_port_mapping.physical_sio_stream = phys_xport->stream_id;
    ipa_port_mapping.aggr_hdr_config_ptr = &ipa_port_aggr;
    ipa_port_mapping.sio_tx_wm_ptr       = &xport_info->tx_wmk;
    ipa_port_mapping.sio_rx_wm_ptr       = &xport_info->rx_wmk;

    rmnet_shim_data_format_to_ipa_config(
               &phys_xport->data_format, &ipa_port_aggr);

    if (ipa_sio_set_sio_config_mapping(
                     IPA_SIO_CONFIG_ACTION_ADD,
                     xport_info->stream_id,
                     &ipa_port_mapping) == IPA_SUCCESS)
    {
#ifdef FEATURE_DATA_IPA
/* For A2 this operation is synchronous and it will not invoke the cfg_done cb
   so there is nothing to keep track */
      xport_info->ep_cfg_cnt++;
#endif /* FEATURE_DATA_IPA */
      LOG_MSG_INFO1_1("IPA set SIO config mapping succeeded cnt %d",
                      xport_info->ep_cfg_cnt);
      return TRUE;
    }
    else
    {
      LOG_MSG_ERROR_0("IPA set SIO config mapping failed");
      return FALSE;
    }
  }
#endif /* defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2) */

  LOG_MSG_ERROR_0("Not supported port configuration");
  return FALSE;

} /* rmnet_shim_set_port_mapping() */

/*===========================================================================
  FUNCTION RMNET_SHIM_UPDATE_ETH_HEADER()

  DESCRIPTION
    This function updates Ethenet header.

  PARAMETERS
    xport_info:   rmnet xport structure
    eth_hdr:      Ethenet header
    eth_hdr_len:  Ethenet header length
    is_v6:        Is v6 call

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_update_eth_header
(
  rmnet_xport_type        * xport_info,
  void                    * eth_hdr,
  uint8                     eth_hdr_len,
  boolean                   is_v6
)
{
  rmnet_phys_xport_type   * phys_xport;
#if defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2)
  ipa_sio_config_map_s      ipa_port_mapping;
  ipa_sio_endpoint_config_s ipa_port_aggr; 
#endif /* defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!RMNET_XPORT_IS_BOUND(xport_info))
  {
    LOG_MSG_ERROR_0("RmNet xport is not bound");
    return FALSE;
  }

  phys_xport = xport_info->phys_xport;

  if ( !RMNET_XPORT_IS_ACTIVE(xport_info) ||
       !RMNET_PHYS_XPORT_IS_ACTIVE(phys_xport) )
  {
    LOG_MSG_ERROR_0("RMNET xports not active");
    return FALSE;
  }

  if (phys_xport->data_format.link_prot != RMNET_ETHERNET_MODE)
  {
    LOG_MSG_ERROR_0("Not in Ethernet mode");
    return FALSE;
  }

#if defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2)
  if ( rmnet_shim_is_ipa_a2_port(xport_info->sio_port) &&
       rmnet_shim_is_ipa_a2_port(phys_xport->sio_port) )
  {
    memset(&ipa_port_mapping, 0, sizeof(ipa_port_mapping));
    memset(&ipa_port_aggr, 0, sizeof(ipa_port_aggr));
    ipa_port_mapping.aggr_hdr_config_ptr = &ipa_port_aggr;

    rmnet_shim_data_format_to_ipa_config(
               &phys_xport->data_format, &ipa_port_aggr);

    if (is_v6)
    {
      ipa_port_aggr.dl_config.params.ether.ipv6_hdr_present = 1;
      memscpy(ipa_port_aggr.dl_config.params.ether.ipv6_hdr,
              IPA_SIO_ETHERNET_HDR_LEN,
              eth_hdr,
              eth_hdr_len);
    }
    else
    {
      ipa_port_aggr.dl_config.params.ether.ipv4_hdr_present = 1;
      memscpy(ipa_port_aggr.dl_config.params.ether.ipv4_hdr,
              IPA_SIO_ETHERNET_HDR_LEN,
              eth_hdr,
              eth_hdr_len);
    }

    if (ipa_sio_set_sio_config_mapping(
                       IPA_SIO_CONFIG_ACTION_ETHERNET_HDR_UPDATE,
                       xport_info->stream_id,
                       &ipa_port_mapping) == IPA_SUCCESS)
    {
      LOG_MSG_INFO1_0("IPA update Eth header succeeded");
      return TRUE;
    }
    else
    {
      LOG_MSG_ERROR_0("IPA udpate Eth header failed");
      return FALSE;
    }
  }
#endif /* defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2) */

  LOG_MSG_ERROR_0("Not supported port configuration");
  return FALSE;

} /* rmnet_shim_update_eth_header() */

/*===========================================================================
  FUNCTION RMNET_SHIM_REGISTER_EP_CFG_DONE_CB()

  DESCRIPTION
    This function registers EP config done callback with the driver

  PARAMETERS
    cb_fn:    callback function
    cb_data:  callback data

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_register_ep_cfg_done_cb
(
  void (* cb_fn)(sio_stream_id_type, uint32),
  uint32  cb_data
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DATA_IPA
  if (ipa_sio_register_ep_config_done_cb(
                       cb_fn, cb_data) != IPA_SUCCESS)
  {
    LOG_MSG_ERROR_0("Failed to register EP cfg done cb");
    return FALSE;
  }
  else
  {
    LOG_MSG_INFO1_0("IPA EP config callback registered");
  }
#endif /* FEATURE_DATA_IPA */

  return TRUE;

} /* rmnet_shim_register_ep_cfg_done_cb() */

/*===========================================================================
  FUNCTION RMNET_SHIM_REGISTER_QMAP_CMD_CB()

  DESCRIPTION
    This function registers QMAP cmd callbacks

  PARAMETERS
    None

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_register_qmap_cmd_cb
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DATA_IPA
    if ( (ipa_sio_register_qmap_cmd_hdlr_fn(
                   IPA_QMAP_CMD_NAME_FLOW_EN,
                   IPA_QMAP_CMD_TYPE_ACK,
                   rment_shim_ipa_qmap_cmd_resp_cb) != IPA_SUCCESS) ||
         (ipa_sio_register_qmap_cmd_hdlr_fn(
                   IPA_QMAP_CMD_NAME_FLOW_DIS,
                   IPA_QMAP_CMD_TYPE_ACK,
                   rment_shim_ipa_qmap_cmd_resp_cb) != IPA_SUCCESS) )
    {
      return FALSE;
    }
#endif /* FEATURE_DATA_IPA */
#ifdef FEATURE_DATA_A2
  a2_ul_per_register_qmap_rsp_cb(rmnet_shim_a2_qmap_cmd_resp_cb);
#endif /* FEATURE_DATA_A2 */

    return TRUE;

} /* rmnet_shim_register_qmap_cmd_cb() */

/*===========================================================================
  FUNCTION RMNET_SHIM_SEND_QMAP_CMD()

  DESCRIPTION
    This function sends a QMAP cmd

  PARAMETERS
    None

  RETURN VALUE
    TRUE: Success
    FALSE: Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_send_qmap_cmd
(
  rmnet_xport_type  * xport_info,
  uint8               cmd,
  uint8               cmd_type,
  dsm_item_type     * pkt,
  uint32            * tx_id,
  boolean             is_retx
)
{
  boolean  result = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2)
  if (RMNET_XPORT_IS_ACTIVE(xport_info) &&
      rmnet_shim_is_ipa_a2_port(xport_info->sio_port))
  {
    if (ipa_sio_send_qmap_cmd_req(
                xport_info->stream_id,
                (ipa_qmap_cmd_name_e)cmd,
                pkt,
                tx_id,
                is_retx) == IPA_SUCCESS)
    {
      result = TRUE;
    }
  }
#endif /* defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2) */

  return result;

} /* rmnet_shim_send_qmap_cmd() */

/*===========================================================================
  FUNCTION RMNET_SHIM_SET_PHYS_RX_FLOW()

  DESCRIPTION
    This function sets phys port UL flow control state

  PARAMETERS
    phys_xport: phys xport info
    disable:    is flow disable

  RETURN VALUE
    TRUE if success

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_shim_set_phys_rx_flow
(
  rmnet_phys_xport_type  * phys_xport,
  boolean                  disable
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!RMNET_PHYS_XPORT_IS_ACTIVE(phys_xport))
  {
    return FALSE;
  }

#if defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2)
  if (rmnet_shim_is_ipa_a2_port(phys_xport->sio_port))
  {
    if (ipa_sio_set_ul_phys_flow_control(
            phys_xport->stream_id,
            disable ? IPA_FLOW_STATE_DISABLE : IPA_FLOW_STATE_ENABLE) != IPA_SUCCESS)
    {
      LOG_MSG_ERROR_0("Failed to set IPA phys flow ctrl");
      return FALSE;
    }
    LOG_MSG_INFO2_1("RX PHYS FLOW CTRL: %c", disable ? '+' : '-');
  }
#endif /* defined(FEATURE_DATA_IPA) || defined(FEATURE_DATA_A2) */

  return TRUE;

} /* rmnet_shim_set_phys_rx_flow() */

/*===========================================================================
  FUNCTION RMNET_SHIM_NOTIFY_TETHERED_CALL_STATE()

  DESCRIPTION
    This function notifies the driver if the first tethered call is coming up/
    last tethered call is being torn down

  PARAMETERS
    phys_xport: phys xport info
    is_call_up: is call coming up

  RETURN VALUE
    NONE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_shim_notify_tethered_call_state
(
  rmnet_phys_xport_type  * phys_xport,
  boolean                  is_call_up
)
{
#ifdef FEATURE_DATA_A2
  if (is_call_up)
  {
    a2_sio_notify_call_state(A2_SIO_CALL_STATE_TETH_UP, 
                             phys_xport->sio_port);
  }
  else
  {
    a2_sio_notify_call_state(A2_SIO_CALL_STATE_TETH_DOWN, 
                             phys_xport->sio_port);
  }
#endif /* FEATURE_DATA_A2 */
}

