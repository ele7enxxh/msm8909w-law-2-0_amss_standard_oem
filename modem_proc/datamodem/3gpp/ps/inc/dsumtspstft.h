#ifndef DSUMTSPSTFT_H
#define DSUMTSPSTFT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  U M T S ( W C D M A / G P R S )  P A C K E T - S W I T C H E D   D A T A

                  T F T   F U N C T I O N   H A N D L E R 

  
GENERAL DESCRIPTION

  This file contains TFT data and function definitions for UMTS (wcdma/gprs)
  Packet-Switched DATA for PDP Type IP.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 - 2008 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/dsumtspstft.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/24/08    ss     Off target Lint fix
11/06/06    vk     Corrected signature of dsumtsps_sec_qos_tft_create().
08/01/06    rr     Fixed Lint errors/warnings.
09/02/05    rc     Created File. Added support for Mobile Initiated QOS
                   modification
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#if defined (FEATURE_DATA_WCDMA_PS) ||  defined (FEATURE_GSM_GPRS)
#include "cm.h"
#include "ps_iface_defs.h"
#include "ps_ipfltr_defs.h"
#include "dsumtspsqos.h"
#include "dstask_v.h"



#ifdef FEATURE_NW_INITIATED_QOS

/* Structure to extract the filter ids and save it localy in DS */
typedef struct
{
  byte tx_filter_ids[MAX_PACKET_FILTERS];
  byte rx_filter_ids[MAX_PACKET_FILTERS];
  uint8 tx_num_filters;
  uint8 rx_num_filters;
  byte direction[MAX_PACKET_FILTERS];
  byte op_code;
}dsumtsps_filter_details_type;

/* Strucutre to save the received MT secondary details to be answered 
   in the answer handler */
typedef struct
{
  cm_call_id_type         call_id;

  uint32                  ps_call_instance;

  qos_T                   qos;
    /**< Quality of Service */

  qos_T                   minimum_qos;
    /**< The minimum quality of service allowed for the
       PDP context
    */

  pdp_address_T           pdp_addr;
    /**< Packet data protocol address */

  protocol_cfg_options_T config_options;
    /**< Protocol configuration options */

  sys_pdp_header_comp_e_type h_comp;
    /**< IP Header Compression Required
    **
    ** Clients need to check for CM_API_PDP_COMPR before
    ** using this enum
    */

  sys_pdp_data_comp_e_type   d_comp;
    /**< Data Compression Required
    **
    ** Clients need to check for CM_API_PDP_COMPR before
    ** using this enum
    */

  tft_type_T             tft;
    /**< Traffic flow template */

  nsapi_T                primary_nsapi;
    /**< Primary NSAPI */

  cm_call_id_type        primary_call_id;
    /**< Call Id of the primary PDP context.
       This field is only used if this is
       a secondary PDP context origination
    */

  uint16                   profile_number;
    /**< The PDP profile number used for this
    ** PDP context
    */

  byte                   pdp_group_id;
    /**< Clients check CM_API_SEC_PDP for SEC PDP support
    ** Group id of the primary context
    ** that is associated with the
    ** secondary PDP context origination
    */

  uint16                   assoc_pri_profile_num;
   /**< Clients check CM_API_SEC_PDP for SEC PDP support
   ** Valid for only Sec PDP context
   ** Sec PDP origination contains requested qos and
   ** minimum_qos.
   ** FALSE - Nw has allocated qos in between requested and minimum_qos
   ** TRUE  - Nw has allocated different qos
   */

}dsumtsps_mt_sec_table_type;

#endif /* FEATURE_NW_INITIATED_QOS */

/*===========================================================================
FUNCTION DSUMTSPS_FILL_ORIGINATION_TFT_PARAMS

DESCRIPTION
  This function fills up the TFT parameters in the origination parameters 
  based on the PDP context specified (only for a secondary PDP context 
  
PARAMETERS
  pdp_cid          - PDP Context id 
  *orig_params_ptr - Ptr to Call origination parameters
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE - Success
  FALSE - Failure
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void dsumtsps_fill_origination_tft_params
(
  unsigned char                pdp_cid,       
  cm_gw_ps_orig_params_s_type  *orig_params_ptr
  /*lint -esym(715, pdp_cid) */
);

#ifdef  FEATURE_NW_INITIATED_QOS
/*===========================================================================
FUNCTION DSUMTSPS_FILL_V4_FLOW_FROM_TFT_PARAMS_FOR_NW_SEC

DESCRIPTION
   Convert the nw given filter parameters to setup the Flow in PS
   TFT parameters for IPV4 context activation and modification
    
PARAMETERS
  flow_ptr       - Ptr to the flow
  tft_params_ptr - Pointer to the TFT to populate
  filter_offset  - Offset at which to start processing the filter
  num_filters    - Number of filters to process and populate 
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE  - SUCCESS
  FALSE - FAILURE

SIDE EFFECTS 
  
  
===========================================================================*/
boolean dsumtsps_fill_v4_flow_from_tft_params_for_nw_sec
(
  ip_filter_type               *list_ptr,
  const packet_filter_type_T    filter_list,
  packet_filter_direction_T     direction,
  sm_status_T                   *cause
);
/*===========================================================================
FUNCTION  DSUMTSPS_MT_MODIFY_TFT_PARAMS

DESCRIPTION

  This function process the received MODIFY operation on TFT's. The current
  filter list is not stored in DS. If the operation is other than CREATE,
  all of the current Rx and Tx filters are pulled from PS and then the 
  received operation is performed and a final list of updated filters is sent
  back to PS for further validation and storing.

PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsumtsps_mt_modify_tft_params
(
qos_spec_type                 *recvd_params,
qos_spec_type                 *mod_qos_params,
dsumtsps_filter_details_type  recvd_fltr_ids,
dsumtsps_filter_details_type  *curr_fltr_ids,
ps_flow_type                  *flow_ptr,
ps_iface_type                 *iface_ptr,
boolean                       modify_flag,
sm_status_T                   *cause
);

/*===========================================================================
FUNCTION DSUMTSPS_VALIDATE_MT_TFT_PARAMS

DESCRIPTION

  This function validates the TFT rules based on 24.008 section 6.1.3. All the
  abnormal cases needs to be handled and correct error cause needs to be sent
  back. 
    
PARAMETERS

  *cause          - OUT param with correct error code
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE  - SUCCESS
  FALSE - FAILURE

SIDE EFFECTS 
  
  
===========================================================================*/
#if 0
boolean dsumtsps_validate_mt_tft_params
(
  ds_cmd_enum_type             cmd_id,
  ds_umts_tft_opcode_enum_type op_code,
  byte                        recvd_num_filters,
  dsumtsps_filter_details_type filter_ids,
  sm_status_T                  *cause
);
#endif 

/*===========================================================================
FUNCTION DSUMTSPS_CONV_NETWORK_TFT_TO_APP_TFT

DESCRIPTION

This function validates the TFT rules based on 24.008 section 6.1.3. All the
abnormal cases needs to be handled and correct error cause needs to be sent
back. 

PARAMETERS

  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE  - SUCCESS
  FALSE - FAILURE

SIDE EFFECTS 
  
  
===========================================================================*/
boolean dsumtsps_conv_network_tft_to_app_tft
(
  const cm_call_mode_info_s_type *mode_info_ptr,
  ip_filter_spec_type            *rx_fltr,
  ip_filter_spec_type            *tx_fltr,
  sm_status_T                    *cause
);
#endif /* FEATURE_NW_INITIATED_QOS */

/*===========================================================================
FUNCTION DSUMTSPS_SEC_QOS_TFT_CREATE

DESCRIPTION
  This function creates a TFT for a IPv4 or IPv6 secondary context during 
  MO context activation or modification 
  
  For the "create new TFT" operation, the packet filter list shall contain 
  a variable number of packet filters. 
  Each packet filter is of variable length and consists of
    - a packet filter identifier (1 octet);
    - a packet filter evaluation precedence (1 octet);
    - the length of the packet filter contents (1 octet); and
    - the packet filter contents itself (v octets).
        
PARAMETERS
  flow_ptr          - Ptr to the flow
  tft_params_ptr    - Pointer to the TFT to populate
  call_instance     - Index into the call table.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE  - SUCCESS
  FALSE - FAILURE

SIDE EFFECTS 
  
  
===========================================================================*/
boolean dsumtsps_sec_qos_tft_create
(
  ps_flow_type          *flow_ptr,
  tft_type_T            *tft_params_ptr
#ifdef FEATURE_DATA_PS_IPV6
  ,
  uint32                call_instance
#endif
);


/*===========================================================================
FUNCTION DSUMTSPS_SEC_QOS_MODIFY_TFT_CREATE

DESCRIPTION
  This function creates a TFT for a IPv4 or IPv6 secondary context during 
  MO context modification 
  
  For the "create new TFT" operation, the packet filter list shall contain 
  a variable number of packet filters. 
  Each packet filter is of variable length and consists of
    - a packet filter identifier (1 octet);
    - a packet filter evaluation precedence (1 octet);
    - the length of the packet filter contents (1 octet); and
    - the packet filter contents itself (v octets).
        
PARAMETERS
  flow_ptr          - Ptr to the flow
  tft_params_ptr    - Pointer to the TFT to populate
  call_instance     - Index into the call table.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE  - SUCCESS
  FALSE - FAILURE

SIDE EFFECTS 
  
  
===========================================================================*/
boolean dsumtsps_sec_qos_modify_tft_create
(
  ps_flow_type          *flow_ptr,
  tft_type_T            *tft_params_ptr,
  uint32                call_instance
);


/*===========================================================================
FUNCTION DSUMTSPS_SEC_QOS_MODIFY_TFT_DELETE

DESCRIPTION
  This function deletes an existing TFT for a IPv4 or IPv6 secondary context 
  during MO context modification.
  For the "delete existing TFT" operation, the packet filter list shall be 
  empty and the number of packet filters shall be coded as 0. 
    
PARAMETERS
  tft_params_ptr    - Pointer to the TFT to populate
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE - SUCCESS
  FALSE - FAILURE
  
SIDE EFFECTS 
  
  
===========================================================================*/
boolean dsumtsps_sec_qos_modify_tft_delete
(
  tft_type_T                         *tft_params_ptr
);


/*===========================================================================
FUNCTION DSUMTSPS_SEC_QOS_MODIFY_TFT_DELETE_FLTR

DESCRIPTION
  This function deletes filters from an existing TFT for a IPv4 or IPv6 
  secondary context during MO context modification.
  For the "delete packet filters from existing TFT" operation, the packet 
  filter list shall contain a variable number of packet filter identifiers.  
  The number of packet filters shall be greater than 0 and less than or 
  equal to 8.
  
PARAMETERS
  tft_params_ptr - Pointer to the TFT to populate
  filter_offset  - Offset at which to start processing the filter
  num_filters    - Number of filters to process and populate 
  
DEPENDENCIES
  
RETURN VALUE
  TRUE - SUCCESS
  FALSE - FAILURE
  
SIDE EFFECTS 
  
  
===========================================================================*/
boolean dsumtsps_sec_qos_modify_tft_delete_fltr
(
  tft_type_T                         *tft_params_ptr,
  uint8                              filter_offset,
  uint8                              num_filters
);


/*===========================================================================
  FUNCTION DSUMTSPS_SEC_QOS_MODIFICATION_TFT_REPLACE

DESCRIPTION
  Process the application supplied QoS. Convert them into UMTS technology
  specific parameters. Sort the QoS to find the minimum and requested sets
    
PARAMETERS
  flow_ptr       - Ptr to the flow
  tft_params_ptr - Pointer to the TFT to populate
  filter_offset  - Offset at which to start processing the filter
  call_instance  - Index of the call in the call table
  num_filters    - Number of filters to process and populate 
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE - SUCCESS
  FALSE - FAILURE
  
SIDE EFFECTS 
  
  
===========================================================================*/
boolean dsumtsps_sec_qos_modify_tft_replace
(
  ps_flow_type                       *flow_ptr,
  tft_type_T                         *tft_params_ptr,
  uint32                             call_instance,
  uint8                              filter_offset,
  uint8                              num_filters
);


/*===========================================================================
  FUNCTION DSUMTSPS_SEC_QOS_MODIFICATION_TFT_ADD

DESCRIPTION
  Process the application supplied QoS. Convert them into UMTS technology
  specific parameters. Sort the QoS to find the minimum and requested sets
    
PARAMETERS
  flow_ptr       - Ptr to the flow
  tft_params_ptr - Pointer to the TFT to populate
  call_instance  - Index of the call in the call table
  filter_offset  - Offset at which to start processing the filter
  num_filters    - Number of filters to process and populate 
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE - SUCCESS
  FALSE - FAILURE
  
SIDE EFFECTS 
  
  
===========================================================================*/
boolean dsumtsps_sec_qos_modify_tft_add
(
  ps_flow_type                       *flow_ptr,
  tft_type_T                         *tft_params_ptr,
  uint32                             call_instance,
  uint8                              filter_offset,
  uint8                              num_filters
);

/*===========================================================================
FUNCTION DSUMTSPS_SEC_QOS_MODIFICATION_TFT_MODIFY

DESCRIPTION
  This function modifies filters of an existing TFT for a IPv4 or IPv6 
  secondary context during MO context modification.
  Since currently our mechanism for deterrmining precedence is a FIFO queue,
  when an application requests a modification of some filters, the
  precedence could have changed for all the filters for that context.
  Thus, some of the filter operations will have to be done as a 2-stage 
  process.
  Three types of filter modification can occur:
  1. Old Filter Count = New Filter Count - This would be a simple "Replace
     packet filters" operation.
  2. Old Filter Count < New Filter Count - In this case, the old filters 
     will first be replaced with the new filters and then the
     new filters will be added. This will be a 2-stage modification procedure
  3. Old Filter Count > New Filter Count - In this case, the old filters 
     will first be deleted and then the existing filters will be replaced
     with new ones. This will be a 2-stage modification procedure
    
PARAMETERS
  flow_ptr       - Ptr to the flow
  tft_params_ptr - Pointer to the TFT to populate
  call_instance  - Index of the call in the call table
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE - SUCCESS
  FALSE - FAILURE
  
SIDE EFFECTS 
  
  
===========================================================================*/
boolean dsumtsps_sec_qos_modify_tft_modify
(
  ps_flow_type                       *flow_ptr,
  tft_type_T                         *tft_params_ptr,
  uint32                             call_instance
);

#endif
#endif







