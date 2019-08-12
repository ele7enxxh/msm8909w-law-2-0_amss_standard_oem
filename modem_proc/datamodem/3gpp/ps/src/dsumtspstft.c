/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  U M T S ( W C D M A / G P R S )  P A C K E T - S W I T C H E D   D A T A

                  T F T   F U N C T I O N   H A N D L E R 

  
GENERAL DESCRIPTION

  This file contains TFT data and function definitions for UMTS (wcdma/gprs)
  Packet-Switched DATA for PDP Type IP.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 - 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/dsumtspstft.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/06/10   ttv     Added messages, which explain reason for asserting.
06/02/10    ss     Klocwork error fix.
05/04/10    ua     Correcting NW Init QoS merging issues. 
07/04/09    pp     hton/ntoh* macros replaced with ps_hton/ntoh*.
03/05/09    ua     Fixed to report error on QoS request spec with only Tx parameters.
05/11/09    ms     Updated CommonData variable names.
11/24/08    ss     Off target Lint fix
08/01/06    rr     Fixed Lint errors/warnings.
06/07/06    aw     Removed the TFT tasklock since it is being used inside
                   the TFT function now.placed the tasklocks with rex critical sections.
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


#include "dsumtspstft.h"
#include "ps_flow.h"
#include "ds_3gppi_utils.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_iface_addr_v6.h"
/*---------------------------------------------------------------------------
  Maximum Number of IPV6 filters that can be filled in PDP context activate
  message
---------------------------------------------------------------------------*/
#define MAX_PACKET_FILTERS_IPV6 4
#endif


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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    The purpose of the traffic flow template information element is to 
    specify the TFT parameters and operations for a PDP context.. The IE is
    defined in 10.5.6.12.
  -------------------------------------------------------------------------*/
  /* initialize the data */
  orig_params_ptr->tft.valid                            = FALSE;
  orig_params_ptr->tft.tft_total_length                 = 0;
  orig_params_ptr->tft.operation_code                   = 0;
  orig_params_ptr->tft.num_filters                      = 0;
  orig_params_ptr->tft.filter_list[0].identifier        = 0;
  orig_params_ptr->tft.filter_list[0].eval_precedence   = 0;
  orig_params_ptr->tft.filter_list[0].content_length    = 0;
  orig_params_ptr->tft.filter_list[0].filter_contents[0]= 0;

  return;
} /* dsumtsps_fill_origination_tft_params() */


/*===========================================================================
FUNCTION DSUMTSPS_FILL_V4_TFT_PARAMS_FOR_SEC

DESCRIPTION
   Convert the application given filter parameters to setup the Call Manager 
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
LOCAL boolean dsumtsps_fill_v4_tft_params_for_sec
(
  ps_flow_type                 *flow_ptr,
  tft_type_T                   *tft_params_ptr,
  uint8                        filter_offset,
  byte                         num_filters,
  boolean                      modify_flag
)
{
  ip_filter_type       *filter;
  uint8                tft_len = 1, fltr_len;
  uint16               temp_portnum, temp_portrng; 
                                     /* to convert network to host order   */
  uint32               temp_address,temp_addr_mask;
                                     /* to convert network to host order   */
  uint8                filter_count, precedence = 0;
  uint8                filter_id;    /* Filter Identifier                  */
  void                 *handle;      /* Handle to the filter               */
  void                 *new_handle;  /* Handle to the next filter          */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Initialize the filter count and filter id
  -----------------------------------------------------------------------*/
  filter_count = 0;

  filter_id = filter_offset;

  /* Rx Filters must be non zero */
  if ( num_filters == 0)
  {
    DS_3GPP_MSG0_ERROR("ERROR: Zero Rx filters received");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Get the handle to the first filter.
  -----------------------------------------------------------------------*/
  handle = ps_flow_get_rx_fltr_handle(flow_ptr, IP_V4,modify_flag);

  /*-----------------------------------------------------------------------
    Loop though the filters till you get to the filter offset at which you
    want to start populating the filters.
  -----------------------------------------------------------------------*/
  while (filter_offset > 0) 
  {
    if(ps_flow_get_rx_fltr_by_handle(flow_ptr,
                                     IP_V4,
                                     handle,
                                     modify_flag,
                                     &filter,
                                     &precedence,
                                     &new_handle) == FALSE)
    {
      DS_3GPP_MSG1_HIGH("Could not get RX filter by handle for phys link %d",
                         flow_ptr);
      return FALSE;
    }

    handle = new_handle;
    filter_offset--;
  }

  /*-----------------------------------------------------------------------
    Loop till the number of filters specified have been processed.
  -----------------------------------------------------------------------*/
  while ((num_filters > 0) && (filter_count < MAX_PACKET_FILTERS))
  {
    /*---------------------------------------------------------------------
       We can send only 8 filters during the origination request. If the 
       application specifies more than 8 filters we would have to split them 
       into two: send the first 8 filters in the current origination request 
       and send the rest in the context modification procedure later. In the 
       current impl we will ignore the rest of the filters and just send 8
       in origination request. Modification added later (enhance) 
    ---------------------------------------------------------------------*/
    fltr_len = 0;

    ASSERT(handle != NULL);

    if(ps_flow_get_rx_fltr_by_handle(flow_ptr,
                                   IP_V4,
                                   handle,
                                   modify_flag,
                                   &filter,
                                   &precedence,
                                   &new_handle) == FALSE)
    {
      DS_3GPP_MSG1_HIGH("Could not get RX filter by handle for phys link %d",
                         flow_ptr);
      return FALSE;
    }

    handle = new_handle;

    ASSERT ( filter->ip_vsn == IP_V4 );

    /*-----------------------------------------------------------------------
      The packet filter identifier field is used to identify each packet 
      filter in a TFT.
      Set the Packet Filter Identifier 
    -----------------------------------------------------------------------*/
    tft_params_ptr->filter_list[filter_count].identifier = filter_id+1;

    /*-----------------------------------------------------------------------
      The packet filter evaluation precedence field is used to specify the 
      precedence for the packet filter among all packet filters in all TFTs 
      associated with this PDP address. We currently set the precedence
      of FIFO manner.
    -----------------------------------------------------------------------*/
    tft_params_ptr->filter_list[filter_count].eval_precedence =
                                                                 precedence;
    /*----------------------------------------------------------------------- 
      Set the Packet Filter IPV4 Source Address and Mask 
    -----------------------------------------------------------------------*/
    if ( filter->ip_hdr.v4.field_mask & (int)IPFLTR_MASK_IP4_SRC_ADDR )
    {
       temp_address = ps_ntohl(filter->ip_hdr.v4.src.addr.ps_s_addr);
      /*--------------------------------------------------------------------- 
        Set the Packet Filter Identifier Mask : IPV4 Addr 
      ---------------------------------------------------------------------*/
      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = (int)DS_UMTS_TFT_IPV4_ADDR;

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] =
                                    (byte) ((temp_address & 0xFF000000 ) >> 24);

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] =
                                     (byte)(( temp_address & 0x00FF0000 ) >> 16);

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] =
                                     (byte) (( temp_address & 0x0000FF00 ) >> 8);

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] =
                                        (byte) (( temp_address & 0x000000FF ));

      /*--------------------------------------------------------------------- 
        Set the Packet Filter Address Mask 
      ---------------------------------------------------------------------*/
      temp_addr_mask = ps_ntohl(filter->ip_hdr.v4.src.subnet_mask.ps_s_addr);

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] =
                                   (byte) (( temp_addr_mask & 0xFF000000 ) >> 24);

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] =
                                   (byte)(( temp_addr_mask & 0x00FF0000 ) >> 16);

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] =
                                    (byte)(( temp_addr_mask & 0x0000FF00 ) >> 8);

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] =
                                       (byte) (( temp_addr_mask & 0x000000FF ));

      DS_3GPP_MSG3_HIGH ( " Filter %d : SRC ADDR : %d ",
                                          filter_count, temp_address, 0);
      DS_3GPP_MSG3_HIGH ( " Filter %d : SNET MSK : %d ",
                                        filter_count, temp_addr_mask, 0);
    }
    /*-----------------------------------------------------------------------
      Set the Packet Filter next header protocol 
    -----------------------------------------------------------------------*/
    if ( filter->ip_hdr.v4.field_mask & (int)IPFLTR_MASK_IP4_NEXT_HDR_PROT )
    {
      /*--------------------------------------------------------------------- 
        Set the Packet Filter Identifier Mask : Next Header Type 
      ---------------------------------------------------------------------*/
      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = (int)DS_UMTS_TFT_NEXT_HDR_TYPE;

      tft_params_ptr->filter_list[filter_count].
        filter_contents[fltr_len++] = filter->ip_hdr.v4.next_hdr_prot;
      DS_3GPP_MSG3_HIGH ( " Filter %d : NXT HDR PROTOCOL : %d ",
                 filter_count, filter->ip_hdr.v4.next_hdr_prot, 0);

      /*--------------------------------------------------------------------- 
        Set the Packet Filter destination and Source port range 
      ---------------------------------------------------------------------*/
      if ( filter->ip_hdr.v4.next_hdr_prot == (int)PS_IPPROTO_TCP )
      {
        /*------------------------------------------------------------------- 
          TCP destination Port has been set 
        -------------------------------------------------------------------*/
        if ( filter->next_prot_hdr.tcp.field_mask & (int)IPFLTR_MASK_TCP_DST_PORT )
        {
          if ( filter->next_prot_hdr.tcp.dst.range == 0 )
          {
            temp_portnum =0;

            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Single Dest Port 
            ---------------------------------------------------------------*/
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SINGLE_DEST_PORT;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.tcp.dst.port );

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);

            DS_3GPP_MSG2_HIGH ("Filter %d: SINGLE TCP DEST PORT: %d",
                               filter_count, temp_portnum);
          }
          else
          {
            /*---------------------------------------------------------------
              Set the Packet Filter Identifier Mask : Dest Port Range Type 
            ---------------------------------------------------------------*/
            temp_portnum = 0;
            temp_portrng = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_DEST_PORT_RANGE;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.tcp.dst.port );
            temp_portrng = filter->next_prot_hdr.tcp.dst.range;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =
              ((temp_portnum + temp_portrng) & 0xFF00) >> 8;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =
              ( ( temp_portnum + temp_portrng ) & 0x00FF);

            DS_3GPP_MSG3_HIGH ( " Filter %d : TCP DEST PORT RANGE: %d, %d ",
                       filter_count, temp_portnum, temp_portnum+temp_portrng);
          }
        } /* if (DST PORT) */

        /*------------------------------------------------------------------- 
          TCP source Port has been set 
        -------------------------------------------------------------------*/
        if ( filter->next_prot_hdr.tcp.field_mask & (int)IPFLTR_MASK_TCP_SRC_PORT )
        {
          if ( filter->next_prot_hdr.tcp.src.range == 0 )
          {
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Single Source Port 
            ---------------------------------------------------------------*/
            temp_portnum = 0;

            tft_params_ptr->filter_list[filter_count].\
                filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SINGLE_SRC_PORT;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.tcp.src.port );

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);

            DS_3GPP_MSG3_HIGH ( " Filter %d : SINGLE TCP SRC PORT: %d ",
                       filter_count, temp_portnum, 0);
          }
          else
          {
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Source Port Range 
            ---------------------------------------------------------------*/
            temp_portnum = 0;
            temp_portrng = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SRC_PORT_RANGE;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.tcp.src.port );
            temp_portrng = filter->next_prot_hdr.tcp.src.range;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =
              ( ( temp_portnum + temp_portrng) & 0xFF00) >> 8;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =
              ( ( temp_portnum + temp_portrng) & 0x00FF);

            DS_3GPP_MSG3_HIGH ( " Filter %d : TCP SRC PORT RANGE: %d, %d ",
                       filter_count, temp_portnum, temp_portnum+temp_portrng);
          }
        }
      }
      else if ( filter->ip_hdr.v4.next_hdr_prot == (int)PS_IPPROTO_UDP )
      {
        /*------------------------------------------------------------------- 
          UDP destination Port has been set 
        -------------------------------------------------------------------*/
        if ( filter->next_prot_hdr.udp.field_mask & (int)IPFLTR_MASK_UDP_DST_PORT )
        {
          if ( filter->next_prot_hdr.udp.dst.range == 0 )
          {
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Single Dest Port 
            ---------------------------------------------------------------*/
            temp_portnum = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SINGLE_DEST_PORT;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.udp.dst.port );

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);

            DS_3GPP_MSG3_HIGH ( " Filter %d : SINGLE UDP DEST PORT : %d ",
                       filter_count, temp_portnum, 0);
          }
          else
          {
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Dest Port Range Type 
            ---------------------------------------------------------------*/
            temp_portnum = 0;
            temp_portrng = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_DEST_PORT_RANGE;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.udp.dst.port );
            temp_portrng = filter->next_prot_hdr.udp.dst.range;


            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =
              ( ( temp_portnum + temp_portrng) & 0xFF00) >> 8;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =
              ( ( temp_portnum + temp_portrng) & 0x00FF);

            DS_3GPP_MSG3_HIGH ( " Filter %d : UDP DEST PORT RANGE: %d, %d ",
                       filter_count, temp_portnum, temp_portnum+temp_portrng);
          }
        }

        /*------------------------------------------------------------------- 
          UDP Source Port has been set 
        -------------------------------------------------------------------*/
        if ( filter->next_prot_hdr.udp.field_mask & (int)IPFLTR_MASK_UDP_SRC_PORT )
        {
          if ( filter->next_prot_hdr.udp.src.range == 0 )
          {
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Single Source Port 
            ---------------------------------------------------------------*/
            temp_portnum = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SINGLE_SRC_PORT;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.udp.src.port );

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);

            DS_3GPP_MSG3_HIGH ( " Filter %d : SINGLE UDP SRC PORT: %d ",
                       filter_count, temp_portnum, 0);
          }
          else
          {
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Source Port Range Type 
            ---------------------------------------------------------------*/
            temp_portnum = 0;
            temp_portrng = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SRC_PORT_RANGE;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.udp.src.port );
            temp_portrng = filter->next_prot_hdr.udp.src.range;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =
              ((temp_portnum + temp_portrng) & 0xFF00) >> 8;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =
              ((temp_portnum + temp_portrng) & 0x00FF);

             DS_3GPP_MSG3_HIGH ( " Filter %d : UDP SRC PORT RANGE: %d, %d ",
                       filter_count, temp_portnum, temp_portnum+temp_portrng);
          }
        }
      } /* if(PS_IPPROTO_UDP) */
    } /* IF (IPFLTR_MASK_IP4_NEXT_HDR_PROT) */

    /*----------------------------------------------------------------------- 
      Set the Packet Filter Security Parameter Index (Supported in IPV6) 
    -----------------------------------------------------------------------*/

    /*----------------------------------------------------------------------- 
      Set the Packet Filter type of service: value and mask
    -----------------------------------------------------------------------*/
    if ( filter->ip_hdr.v4.field_mask & (int)IPFLTR_MASK_IP4_TOS )
    {
      /*--------------------------------------------------------------------- 
        Set the Packet Filter Identifier Mask : Type of Service 
      ---------------------------------------------------------------------*/
      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = (int)DS_UMTS_TFT_TYPE_OF_SRVC;

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = filter->ip_hdr.v4.tos.val;

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = filter->ip_hdr.v4.tos.mask;

      DS_3GPP_MSG3_HIGH ( " Filter %d : TOS VAL: %d, TOS MASK: %d ",
                 filter_count, filter->ip_hdr.v4.tos.val,
                 filter->ip_hdr.v4.tos.mask);
    }

    if ( fltr_len == 0 )
    {
      DS_3GPP_MSG1_ERROR("Invalid filter Components, Filter Length: %d",
                          fltr_len);
      return FALSE;
    }
    else
    {
      DS_3GPP_MSG1_HIGH("Filter Component Length: %d", fltr_len);
    }

    tft_params_ptr->filter_list[filter_count].content_length = fltr_len;

    ASSERT( fltr_len < MAX_FILTER_CONTENT_LENGTH );
    tft_len += fltr_len;

    /*------------------------------------------------------------------------- 
      Increment the filter count
    -------------------------------------------------------------------------*/
    filter_count++;

    /*------------------------------------------------------------------------- 
      Increment the filter identifier
    -------------------------------------------------------------------------*/
    filter_id++;

    /*------------------------------------------------------------------------- 
      Decrement to get the number of filters left to process.
    -------------------------------------------------------------------------*/
    num_filters--;

  } /* while */

  /*------------------------------------------------------------------------- 
    Update the num_filters field with the filter count.
  -------------------------------------------------------------------------*/
  tft_params_ptr->num_filters         = filter_count;

  /*------------------------------------------------------------------------- 
    The length of the packet filter contents field contains the binary coded
    representation of the length of the packet filter contents field of a 
    packet filter.     
    Set the total TFT length : Header + Length of total number of filters 
  -------------------------------------------------------------------------*/
  tft_params_ptr->tft_total_length = tft_len;

  DS_3GPP_MSG1_HIGH("Total Filter Length: %d", tft_len);

  return TRUE;
} /* dsumtsps_fill_v4_tft_params_for_sec() */

#ifdef  FEATURE_NW_INITIATED_QOS
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
  ds_cmd_enum_type             cmd_id,             /* Indicates if MT secondary 
                                                      or MT modify */
  ds_umts_tft_opcode_enum_type op_code,            /* Operation code received*/
  byte                         recvd_num_filters,  /* num filters received  */
  dsumtsps_filter_details_type filter_ids,         /* Extracted filter id's */
  sm_status_T                  *cause              /* SM cause  */
)
{
  uint8 i = 0;
  uint8 j = 0;
  uint16 total_num_filters = 
        filter_ids.rx_num_filters + filter_ids.tx_num_filters;
  
  if ( cmd_id == DS_CM_CALL_INCOM_CMD ) /* MT secondary request  */
  {
    if ( op_code != DS_UMTS_TFT_OPCODE_CREATE )
    {
      *cause = SM_SEMANTIC_ERR_IN_TFT;
      DS_3GPP_MSG1_ERROR("Invalid opcode received in MT secondary",
                          op_code);
      return FALSE;
    }

    if ( 0 == recvd_num_filters )
    {
      *cause = SM_SYNTACTIC_ERR_IN_TFT;
      DS_3GPP_MSG0_ERROR("Num of filters cannot be 0 for opcode CREATE");
      return FALSE;
    }
  } /* DS_CM_CALL_INCOM_CMD */
  else if ( cmd_id == DS_CM_CALL_NW_MODIFY_REQ )
  {
    if ( op_code == DS_UMTS_TFT_OPCODE_DEL_TFT )
    {
      *cause = SM_SEMANTIC_ERR_IN_TFT;
      DS_3GPP_MSG0_ERROR("Opcode DELETE TFT is not expected in MODIFY REQ");
      return FALSE;
    }

    if ( op_code != DS_UMTS_TFT_OPCODE_NO_TFT )
    {
      if ( 0 == recvd_num_filters )  
      {
        *cause = SM_SYNTACTIC_ERR_IN_TFT;
        DS_3GPP_MSG1_ERROR("Num of filters cannot be 0 for opcode %d",
                            op_code);
        return FALSE;
      }
    }
  }/* DS_CM_CALL_NW_MODIFY_REQ */

  switch ( op_code )
  {
    case DS_UMTS_TFT_OPCODE_CREATE:
    case DS_UMTS_TFT_OPCODE_ADD:
    {
      /*  Loop to find identical packet filters */
      for ( i=0;i< filter_ids.tx_num_filters;++i)
      {
        for ( j=i+1;j<filter_ids.tx_num_filters;++j)
        {
          if ( filter_ids.tx_filter_ids[i] == 
                  filter_ids.tx_filter_ids[j] )
          {
            DS_3GPP_MSG1_ERROR("Create/Add filter with Tx identical filter ids %d",
                                filter_ids.tx_filter_ids[i]);
            *cause =SM_SYNTACTIC_ERR_IN_FILTER;
            return FALSE;
          }
        }
      }

      for ( i=0;i< filter_ids.rx_num_filters;++i)
      {
        for ( j=i+1;j<filter_ids.rx_num_filters;++j)
        {
          if ( filter_ids.rx_filter_ids[i] == 
                  filter_ids.rx_filter_ids[j] )
          {
            DS_3GPP_MSG1_ERROR("Create/Add filter with Rx identical filter ids %d",
                                filter_ids.rx_filter_ids[i]);
            *cause =SM_SYNTACTIC_ERR_IN_FILTER;
            return FALSE;
          }
        }
      }
    }
      break;
    case DS_UMTS_TFT_OPCODE_DEL_FILTER:
    {
      if ( ( 0 == filter_ids.rx_num_filters ) ||
           ( 0 == filter_ids.tx_num_filters ))
      {
        *cause = SM_SEMANTIC_ERR_IN_TFT;
        DS_3GPP_MSG0_ERROR("Num of filters cannot be 0 for opcode DELETE PACKET FILTER");
         return FALSE;
      }
    }
      break;
    case DS_UMTS_TFT_OPCODE_NO_TFT:
    {
      if ( recvd_num_filters )
      {
        *cause = SM_SYNTACTIC_ERR_IN_TFT;
        DS_3GPP_MSG1_ERROR("Number of filters %d received for opcode NO TFT",
                            op_code);
        return FALSE;
      }
    }
      break;
  }/* opeartion */

  /* Validate MAX number of supported filters */
  for(i=0;i<filter_ids.rx_num_filters;++i)
  {
    for(j=0;j<filter_ids.tx_num_filters;++j)
    {
      /* find out unique filters */
      if ( filter_ids.rx_filter_ids[i] == filter_ids.tx_filter_ids[j] )
      {
        total_num_filters--;
      }
    }

  }
  if ( total_num_filters > MAX_PACKET_FILTERS )
  {
    DS_3GPP_MSG1_ERROR("ERROR: Max filters reached %d",
                        total_num_filters);
    *cause = SM_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  
  DS_3GPP_MSG1_HIGH("Validation Passed for TFT's with operation %d",
                     op_code);
  return TRUE;
}/* dsumtsps_validate_mt_tft_params */
#endif 
/*===========================================================================
FUNCTION DSUMTSPS_CONV_NETWORK_TFT_TO_APP_TFT

DESCRIPTION

  This function converts NW TFT parameters to PS TFT. This is applicable
  for MT secondary and MT modify procedures. 

  Currently it is supported for IPV4 only. 
  
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
)
{
  uint8 i = 0;
  uint32 direction;
  
  /* Need to differentiate between IPV4 and IPV6 */
    if ( mode_info_ptr->info.gw_ps_call.tft.valid == TRUE )
    {
      if (mode_info_ptr->info.gw_ps_call.tft.num_filters > MAX_PACKET_FILTERS)
      {
        DS_3GPP_MSG0_ERROR("Num filter more than MAX");
       *cause = SM_SEMANTIC_ERR_IN_TFT;
        return FALSE;
      }
      for ( i=0; i <  mode_info_ptr->info.gw_ps_call.tft.num_filters; ++i )
      { 
        direction = (uint32)
                    mode_info_ptr->info.gw_ps_call.tft.filter_list[i].direction;
  
        switch( direction )
        {
          case FILTER_UPLINK :
            if ( FALSE == dsumtsps_fill_v4_flow_from_tft_params_for_nw_sec(
            &(tx_fltr->list_ptr[tx_fltr->num_filters]),  
                         mode_info_ptr->info.gw_ps_call.tft.filter_list[i],
                         FILTER_UPLINK,
                         cause) )
            {
              DS_3GPP_MSG0_ERROR(" FILTER_UPLINK Invalid TFT Params");
              return FALSE; 
              
            }
            ++tx_fltr->num_filters;
            break;
          case FILTER_DOWNLINK:
          case FILTER_PRE_REL7:
            if ( FALSE == dsumtsps_fill_v4_flow_from_tft_params_for_nw_sec(
            &(rx_fltr->list_ptr[rx_fltr->num_filters]),
                         mode_info_ptr->info.gw_ps_call.tft.filter_list[i],
                         FILTER_DOWNLINK,
                         cause) )
            {
              DS_3GPP_MSG0_ERROR(" FILTER_DOWNLINK/FILTER_PRE_REL7 Invalid TFT Params");
              return FALSE;
            }
            ++rx_fltr->num_filters;
            break;
          case FILTER_BIDIRECTIONAL:
            if ( FALSE == dsumtsps_fill_v4_flow_from_tft_params_for_nw_sec(
            &(tx_fltr->list_ptr[tx_fltr->num_filters]),
                         mode_info_ptr->info.gw_ps_call.tft.filter_list[i],
                         FILTER_DOWNLINK,
                         cause))
            {
              DS_3GPP_MSG0_ERROR("FILTER_BIDIRECTIONAL, DOWNLINK  Invalid TFT Params");
              return FALSE;
            }
            if ( FALSE == dsumtsps_fill_v4_flow_from_tft_params_for_nw_sec(
            &(rx_fltr->list_ptr[rx_fltr->num_filters]),
                         mode_info_ptr->info.gw_ps_call.tft.filter_list[i],
                         FILTER_UPLINK,
                         cause) )
            {
              DS_3GPP_MSG0_ERROR("FILTER_BIDIRECTIONAL, FILTER_UPLINK Invalid TFT Params");
              return FALSE;
            }
            ++tx_fltr->num_filters;
            ++rx_fltr->num_filters;
            break;
          default:
          /* We need to return ERROR this should not happen */
	        DATA_ERR_FATAL("Invalid tft filter direction");
        }/* switch (direction)*/
  
      }/* for loop */
      DS_3GPP_MSG3_HIGH("Received Filters = %d Num of Uplink filters = %d, Downlink =%d", 
                    mode_info_ptr->info.gw_ps_call.tft.num_filters,
                    tx_fltr->num_filters,
                    rx_fltr->num_filters);
    }/* if ( tft.valid == TRUE )*/
    else
    {
      DS_3GPP_MSG0_ERROR("Invalid filters received");
      *cause = SM_SYNTACTIC_ERR_IN_TFT;
      return FALSE;
    }
  return TRUE;
} /* dsumtsps_conv_network_tft_app_tft */

/*===========================================================================
FUNCTION DSUMTSPS_FILL_V4_FLOW_FROM_TFT_PARAMS_FOR_NW_SEC

DESCRIPTION
   Convert the nw given filter parameters to setup the Flow in PS
   TFT parameters for IPV4 context activation and modification
    
PARAMETERS
 *list_ptr  ...................- Ptr to one list to be updated 
 filter_list ................. - Received filter list 
 direction   ..................- Direction
 *cause    ....................- Output pointer for Cause

DEPENDENCIES

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
)
{
  uint8                fltr_len = 0;
  uint16               temp_portnum = 0;
  uint16               temp_portrng = 0; 
  uint32               temp_address = 0;
  uint32               temp_addr_mask = 0;
  /* Source Port and Destination port details */
  uint16  src_port = 0;
  uint16  src_port_range = 0;
  uint16  dest_port = 0;
  uint16  dest_port_range = 0;
  uint8   temp_hdr_prot = PS_NO_NEXT_HDR;
  boolean s_valid = FALSE;
  boolean  d_valid = FALSE;

  
    ASSERT( list_ptr != NULL );

  /*-----------------------------------------------------------------------
    Initialize the filter count and filter id
  -----------------------------------------------------------------------*/
  list_ptr->ip_vsn = IP_V4;
  DS_3GPP_MSG1_HIGH("Filling V4 TFT's for content length %d", 
                     filter_list.content_length);

  if ( filter_list.content_length == 0)
  {
    *cause = SM_SYNTACTIC_ERR_IN_TFT;
    return FALSE;
  }

  while( fltr_len < (filter_list.content_length -1))
  {
    /*-----------------------------------------------------------------------
      Fill up IP Header details
    -----------------------------------------------------------------------*/
    if ( filter_list.filter_contents[fltr_len] == (byte)DS_UMTS_TFT_IPV4_ADDR )
    {
      ++fltr_len;

      if ( filter_list.content_length >= fltr_len + 8 )
      {
        DS_3GPP_MSG0_HIGH("DS_UMTS_TFT_IPV4_ADDR Valid");
        if ( direction == FILTER_DOWNLINK )
        {
          list_ptr->ip_hdr.v4.field_mask = 
             (ipfltr_ip4_hdr_field_mask_type)IPFLTR_MASK_IP4_SRC_ADDR;
        }
        else if ( direction == FILTER_UPLINK )
        {
          list_ptr->ip_hdr.v4.field_mask = 
              (ipfltr_ip4_hdr_field_mask_type)IPFLTR_MASK_IP4_DST_ADDR;
        }
  
          /*--------------------------------------------------------------------- 
            Set the Packet Filter Identifier Mask : IPV4 Addr 
          ---------------------------------------------------------------------*/
        temp_address  = (uint32)(filter_list.filter_contents[fltr_len++] << 24);
        temp_address |= (uint32)(filter_list.filter_contents[fltr_len++] << 16);
        temp_address |= (uint32)( filter_list.filter_contents[fltr_len++]<< 8 );
        temp_address |= (uint32)( filter_list.filter_contents[fltr_len++]);
  
        if ( direction == FILTER_UPLINK )
        {
          list_ptr->ip_hdr.v4.dst.addr.ps_s_addr = ps_htonl(temp_address);
          DS_3GPP_MSG2_HIGH("Uplink filter: IPV4 DST ADDR = %d content length = %d",
                             temp_address,filter_list.content_length);
        }
        else if ( direction == FILTER_DOWNLINK )
        {
          list_ptr->ip_hdr.v4.src.addr.ps_s_addr = htonl(temp_address);
          DS_3GPP_MSG1_HIGH("Downlink filter: IP4 SRC ADDR = %d",
                             temp_address);
        }
        /*--------------------------------------------------------------------- 
          Set the Packet Filter Address Mask 
        ---------------------------------------------------------------------*/
        temp_addr_mask  = (uint32)(filter_list.filter_contents[fltr_len++]<<24);
        temp_addr_mask |= (uint32)(filter_list.filter_contents[fltr_len++]<<16);
        temp_addr_mask |= (uint32)(filter_list.filter_contents[fltr_len++]<<8);
        temp_addr_mask |= (uint32)(filter_list.filter_contents[fltr_len++]);
      }
      if ( direction == FILTER_UPLINK )
      {
        list_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr = htonl(temp_addr_mask);
        DS_3GPP_MSG1_HIGH("Uplink packet filter address = %d", 
                           temp_addr_mask);
      }
      else if ( direction == FILTER_DOWNLINK )
      {
        list_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr = htonl(temp_addr_mask);
        DS_3GPP_MSG1_HIGH("Downlink packet filter address = %d", 
                           temp_addr_mask);
      }
    }/* DS_UMTS_TFT_IPV4_ADDR */
    else if ( filter_list.filter_contents[fltr_len] == DS_UMTS_TFT_IPV6_ADDR )
    {
      DS_3GPP_MSG0_ERROR("IPV6 currently not supported");
      *cause = SM_SEMANTIC_ERR_IN_FILTER;
      return FALSE;
    }
    else if ( filter_list.filter_contents[fltr_len] == DS_UMTS_TFT_FLOW_LABEL)
    {
      DS_3GPP_MSG0_ERROR("TFT FLOW LABEL not supported");
      *cause = SM_SEMANTIC_ERR_IN_FILTER;
      return FALSE;
    }
    /*----------------------------------------------------------------------- 
      Set the Packet Filter type of service: value and mask
    -----------------------------------------------------------------------*/
    else if(filter_list.filter_contents[fltr_len] == DS_UMTS_TFT_TYPE_OF_SRVC)
    {
      ++fltr_len;
      if ( filter_list.content_length >= fltr_len + 2 )
      {
        DS_3GPP_MSG0_HIGH("DS_UMTS_TFT_TYPE_OF_SRVC Valid");
        list_ptr->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_TOS;
        list_ptr->ip_hdr.v4.tos.val =  filter_list.filter_contents[fltr_len++];
        list_ptr->ip_hdr.v4.tos.mask = filter_list.filter_contents[fltr_len++];
        DS_3GPP_MSG2_HIGH("TOS value = %d, TOS mask = %d",
                           list_ptr->ip_hdr.v4.tos.val,
                           list_ptr->ip_hdr.v4.tos.mask);
      }
      else
      {
        list_ptr->ip_hdr.v4.err_mask |= IPFLTR_MASK_IP4_TOS;
        DS_3GPP_MSG0_ERROR("Invalid TOS");
      }
    }
    /*-----------------------------------------------------------------------
               Fill up Next header protocol details
    -----------------------------------------------------------------------*/
    else if ( filter_list.filter_contents[fltr_len] == 
                                             DS_UMTS_TFT_NEXT_HDR_TYPE )
    {
        ++fltr_len;
        list_ptr->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
        list_ptr->ip_hdr.v4.next_hdr_prot = 
                                filter_list.filter_contents[fltr_len++];
        /* Also save in temporary variable  */
        temp_hdr_prot = list_ptr->ip_hdr.v4.next_hdr_prot;
        DS_3GPP_MSG1_HIGH("DS_UMTS_TFT_NEXT_HDR_TYPE Valid %d",
                                 list_ptr->ip_hdr.v4.next_hdr_prot);
    }
    /*-----------------------------------------------------------------------
                 Fill up Single Dest Port Details 
    -----------------------------------------------------------------------*/
    else if ( filter_list.filter_contents[fltr_len] == 
                                          DS_UMTS_TFT_SINGLE_DEST_PORT )
    {
      ++fltr_len;
      {
        DS_3GPP_MSG0_HIGH("DS_UMTS_TFT_SINGLE_DEST_PORT Valid");

        temp_portnum = (uint16)(filter_list.filter_contents[fltr_len++] <<8);
        temp_portnum |= (uint16)(filter_list.filter_contents[fltr_len++]);
        temp_portrng = 0;
        if ( direction == FILTER_UPLINK )
        {
          s_valid = TRUE;
          src_port = ps_htons(temp_portnum);
          src_port_range = 0;
          DS_3GPP_MSG2_HIGH("Uplink :cmd id %d SRC port = %d",
                             DS_UMTS_TFT_SINGLE_DEST_PORT, 
                             temp_portnum);
        }
        else if ( direction == FILTER_DOWNLINK )
        {
          d_valid = TRUE;
          dest_port = htons(temp_portnum);
          dest_port_range = 0;
          DS_3GPP_MSG2_HIGH("Downlink: cmd id %d DEST port = %d",
                             DS_UMTS_TFT_SINGLE_DEST_PORT,
                             temp_portnum);
        }
      }
    }
    /*-----------------------------------------------------------------------
                 Fill up Dest Port Range Details 
    -----------------------------------------------------------------------*/
    else if (filter_list.filter_contents[fltr_len] == 
                                     DS_UMTS_TFT_DEST_PORT_RANGE )
    {
      ++fltr_len;
      //if ( list_ptr->ip_hdr.v4.next_hdr_prot == (int)IPPROTO_TCP )
      {
        DS_3GPP_MSG0_HIGH(" DS_UMTS_TFT_DEST_PORT_RANGE Valid");

        temp_portnum  = (uint16)(filter_list.filter_contents[fltr_len++] <<8);
        temp_portnum |= (uint16)(filter_list.filter_contents[fltr_len++]);
        temp_portrng  = (uint16)(filter_list.filter_contents[fltr_len++]<<8);
        temp_portrng |= (uint16)filter_list.filter_contents[fltr_len++];

        if ( direction == FILTER_UPLINK )
        {
          s_valid = TRUE;
          src_port = htons(temp_portnum);
          src_port_range = temp_portrng - temp_portnum;
          DS_3GPP_MSG2_HIGH("Uplink: SRC PORT RANGE port=%d range=%d ",
                             temp_portnum, src_port_range);
        }
        else if ( direction == FILTER_DOWNLINK )
        {
          d_valid = TRUE;
          dest_port = htons(temp_portnum);
          dest_port_range = temp_portrng - temp_portnum;
          DS_3GPP_MSG2_HIGH("Downlink TCP: DEST PORT RANGE port=%d range=%d",
                             temp_portnum, dest_port_range);
        }
      }
    }
    /*-----------------------------------------------------------------------
                 Fill up Single Source Port Details 
    -----------------------------------------------------------------------*/
    else if ( filter_list.filter_contents[fltr_len] == 
                                    DS_UMTS_TFT_SINGLE_SRC_PORT )
    {
      ++fltr_len;
      {
        DS_3GPP_MSG0_HIGH("DS_UMTS_TFT_SINGLE_SRC_PORT Valid");

        temp_portnum = (uint16)(filter_list.filter_contents[fltr_len++] <<8);
        temp_portnum |= (uint16)(filter_list.filter_contents[fltr_len++]);
        if ( direction == FILTER_UPLINK )
        {
          d_valid = TRUE;
          dest_port = htons(temp_portnum);
          dest_port_range = 0;
          DS_3GPP_MSG2_HIGH("Uplink : cmd id %d DST PORT port= %d",
                             DS_UMTS_TFT_SINGLE_SRC_PORT, temp_portnum);
        }
        else if ( direction == FILTER_DOWNLINK )
        {
          s_valid = TRUE;
          src_port = htons(temp_portnum);
          src_port_range = 0;
          DS_3GPP_MSG2_HIGH("Downlink : cmd id %d SRC PORT port= %d field = %d",
                             DS_UMTS_TFT_SINGLE_SRC_PORT,temp_portnum);
        }
      }
    }
    /*-----------------------------------------------------------------------
                 Fill up Src Port Rnage Details 
    -----------------------------------------------------------------------*/
    else if (filter_list.filter_contents[fltr_len] == 
                                         DS_UMTS_TFT_SRC_PORT_RANGE )
    {
      ++fltr_len;
      {
        DS_3GPP_MSG0_HIGH("DS_UMTS_TFT_SRC_PORT_RANGE Valid");

        temp_portnum = (uint16)(filter_list.filter_contents[fltr_len++] <<8);
        temp_portnum |= (uint16)(filter_list.filter_contents[fltr_len++]);
        temp_portrng = (uint16)(filter_list.filter_contents[fltr_len++]<<8);
        temp_portrng |=  (uint16)filter_list.filter_contents[fltr_len++];
        if ( direction == FILTER_UPLINK )
        {
          d_valid = TRUE;
          dest_port = htons(temp_portnum);
          dest_port_range = temp_portrng - temp_portnum;
          DS_3GPP_MSG2_HIGH("Uplink TCP: DST PORT RANGE port= %d, range= %d",
                             temp_portnum, dest_port_range);
        }
        else if ( direction == FILTER_DOWNLINK )
        {
          s_valid = TRUE;
          src_port = htons(temp_portnum);
          src_port_range = temp_portrng - temp_portnum;
          DS_3GPP_MSG2_HIGH("Downlink TCP: SRC PORT RANGE port= %d,range= %d ",
                             temp_portnum, src_port_range);
        }
      }
    }
    else
    {
      DS_3GPP_MSG2_ERROR("Invalid Params in TFT fltr len= %d, content= %d",
                          fltr_len,filter_list.filter_contents[fltr_len]);
      *cause = SM_SEMANTIC_ERR_IN_FILTER;
      return FALSE;
    }
  }


  /* --------------------------------------------------------------------------
    Populate the Src, Dest port details into corresponding protocol structure
-----------------------------------------------------------------------------*/

  if ( temp_hdr_prot != PS_NO_NEXT_HDR )
  {
    switch ( temp_hdr_prot )
    {
    case PS_IPPROTO_UDP:
      {
        if ( TRUE == s_valid )
        {
          DS_3GPP_MSG3_HIGH(" Protocl UDP Source valid SRC Port %d Range %d field %d ",
                              src_port,src_port_range,IPFLTR_MASK_UDP_SRC_PORT);
          list_ptr->next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_SRC_PORT;
          list_ptr->next_prot_hdr.udp.src.port = src_port;
          list_ptr->next_prot_hdr.udp.src.range = src_port_range;
        }
        if ( TRUE == d_valid )
        {
          DS_3GPP_MSG3_HIGH(" Protocl UDP DST valid DEST Port %d Range %d Field %d",
                       dest_port,dest_port_range,IPFLTR_MASK_UDP_DST_PORT);
          list_ptr->next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_DST_PORT;
          list_ptr->next_prot_hdr.udp.dst.port = dest_port;
          list_ptr->next_prot_hdr.udp.dst.range = dest_port_range;
        }
      }
      break;
    case PS_IPPROTO_TCP:
      {
        if ( TRUE == s_valid )
        {
          DS_3GPP_MSG3_HIGH(" Protocl TCP Source valid SRC Port %d Range %d field %d ",
                              src_port,src_port_range,IPFLTR_MASK_TCP_SRC_PORT);
          list_ptr->next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_SRC_PORT;
          list_ptr->next_prot_hdr.tcp.src.port = src_port;
          list_ptr->next_prot_hdr.tcp.src.range = src_port_range;
        }
        if ( TRUE == d_valid )
        {
          DS_3GPP_MSG3_HIGH(" Protocl TCP DST valid DEST Port %d Range %d Field %d",
                          dest_port,dest_port_range,IPFLTR_MASK_TCP_DST_PORT);
          list_ptr->next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_DST_PORT;
          list_ptr->next_prot_hdr.tcp.dst.port = dest_port;
          list_ptr->next_prot_hdr.tcp.dst.range = dest_port_range;
        }
      }
      break;
    case PS_IPPROTO_ICMP:
    {
      DS_3GPP_MSG0_HIGH("Protocol ICMP ");
      list_ptr->next_prot_hdr.icmp.field_mask |= IPFLTR_MASK_ICMP_NONE;
    }
      break;
    default:
      DS_3GPP_MSG1_ERROR("Invalid next header protocol %d",temp_hdr_prot);
      *cause = SM_SEMANTIC_ERR_IN_FILTER;
      return FALSE;
    }
  }

  return TRUE;
} /* dsumtsps_fill_v4_tft_params_for_sec() */

  /*===========================================================================
  FUNCTION  DSUMTSPS_MT_MODIFY_TFT_ADD
  
  DESCRIPTION
  This function performs the operation ADD in MODIFY request. 
  Add the new Rx, Tx filters to the existing filters. 
  PARAMETERS
  
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None.
  
  ===========================================================================*/
LOCAL void dsumtsps_mt_modify_tft_add
(
  qos_spec_type                 *recvd_params,
  qos_spec_type                 *mod_qos_params,
  dsumtsps_filter_details_type  recvd_fltr_ids,
  dsumtsps_filter_details_type  *curr_fltr_ids
)
{
  ip_filter_spec_type  *mod_rx_fltr = NULL;
  ip_filter_spec_type   *mod_tx_fltr = NULL;
  uint8 i = 0;

  mod_rx_fltr = &(mod_qos_params->rx.fltr_template);
  mod_tx_fltr = &(mod_qos_params->tx.fltr_template);

  DS_3GPP_MSG0_HIGH("Modify is for OPCODE_ADD");

  /*----------------------------------------------------------------------
        Add packet filters: Received packet filters must be added to
        the existing packet filters 
  -----------------------------------------------------------------------*/
  if ( recvd_fltr_ids.rx_num_filters )
  {
    mod_qos_params->field_mask |= 
        (qos_spec_field_mask_type)QOS_MODIFY_MASK_RX_FLTR_MODIFY;
  
    for ( i = 0; i< recvd_fltr_ids.rx_num_filters; ++i)
    {
      DS_3GPP_MSG1_HIGH("Adding Rx filter id %d to current",
                         recvd_fltr_ids.rx_filter_ids[i]);
      memscpy((void *)&(mod_rx_fltr->list_ptr[mod_rx_fltr->num_filters]),
	      sizeof(mod_rx_fltr->list_ptr[mod_rx_fltr->num_filters]),
              (const void *)&(recvd_params->rx.fltr_template.list_ptr[i]),
              sizeof(recvd_params->rx.fltr_template.list_ptr[i]));
      mod_rx_fltr->num_filters++;
    
      curr_fltr_ids->rx_filter_ids[curr_fltr_ids->rx_num_filters++] = 
                                 recvd_fltr_ids.rx_filter_ids[i];
    }
  }
  if ( recvd_fltr_ids.tx_num_filters )
  {
    mod_qos_params->field_mask |= 
        (qos_spec_field_mask_type)QOS_MODIFY_MASK_TX_FLTR_MODIFY;
    for ( i = 0; i< recvd_fltr_ids.tx_num_filters; ++i)
    {
      DS_3GPP_MSG1_HIGH("Adding Tx filter id %d to current",
                         recvd_fltr_ids.tx_filter_ids[i]);
      memscpy((void *)&(mod_tx_fltr->list_ptr[mod_tx_fltr->num_filters]),
	      sizeof(mod_tx_fltr->list_ptr[mod_tx_fltr->num_filters]),
              (const void *)&(recvd_params->tx.fltr_template.list_ptr[i]),
              sizeof(recvd_params->tx.fltr_template.list_ptr[i]));
      mod_tx_fltr->num_filters++;
    
      curr_fltr_ids->tx_filter_ids[curr_fltr_ids->tx_num_filters++] = 
                           recvd_fltr_ids.tx_filter_ids[i];
    }
  }
}/* dsumtsps_mt_modify_tft_add */
    /*===========================================================================
  FUNCTION  DSUMTSPS_MT_MODIFY_TFT_REPLACE
  
  DESCRIPTION
  This function performs the operation REPLACE/ADD in MODIFY request. 
  Replace operation, matches the current filter to be replaced if not found, 
  the new Rx, Tx filters are added to the existing filters.
  
  PARAMETERS
  
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None.
  
  ===========================================================================*/
LOCAL void dsumtsps_mt_modify_tft_replace
(
  qos_spec_type                 *recvd_params,
  qos_spec_type                 *mod_qos_params,
  dsumtsps_filter_details_type  recvd_fltr_ids,
  dsumtsps_filter_details_type  *curr_fltr_ids
)
{
  ip_filter_spec_type  *mod_rx_fltr = NULL;
  ip_filter_spec_type   *mod_tx_fltr = NULL;
  boolean replaced = FALSE;
  uint8 i = 0;
  uint8 j =0;

  mod_rx_fltr = &(mod_qos_params->rx.fltr_template);
  mod_tx_fltr = &(mod_qos_params->tx.fltr_template);

  DS_3GPP_MSG1_HIGH("Modify is for opdcode %d",
                     recvd_fltr_ids.op_code);

  /* ------------------------------------------------------------------------
       Replace Operation: Search to find the filterd id in the current
       filter list, if found, replace the current filters with received
       filters. If the match is not found, then the recevd filter needs to 
       be added. Increment the number of filters. 
  ---------------------------------------------------------------------------*/
  if ( recvd_fltr_ids.rx_num_filters )
  {
    mod_qos_params->field_mask |= 
        (qos_spec_field_mask_type)QOS_MODIFY_MASK_RX_FLTR_MODIFY;

    for (i=0;i < recvd_fltr_ids.rx_num_filters;++i)
    {
      for (replaced = FALSE,j=0;j < curr_fltr_ids->rx_num_filters; ++j)
      {
        if ( recvd_fltr_ids.rx_filter_ids[i] == curr_fltr_ids->rx_filter_ids[j])
        {
          DS_3GPP_MSG1_HIGH("Rx Filter id %d is getting replaced ",
                             recvd_fltr_ids.rx_filter_ids[i]);
          memscpy((void *)&(mod_rx_fltr->list_ptr[j]), 
		  sizeof(mod_rx_fltr->list_ptr[j]),
                  (const void *)&(recvd_params->rx.fltr_template.list_ptr[i]),
                  sizeof(recvd_params->rx.fltr_template.list_ptr[i]));
          replaced = TRUE;
          break;
        } /* match */
      } /* inner for */
      if (!replaced)
      {
        DS_3GPP_MSG1_HIGH("Rx Filter %d not found for replace, ADDING it",
                           recvd_fltr_ids.rx_filter_ids[i]);
        memscpy((void *)&(mod_rx_fltr->list_ptr[mod_rx_fltr->num_filters]),
	        sizeof(mod_rx_fltr->list_ptr[mod_rx_fltr->num_filters]),
                (const void *)&(recvd_params->rx.fltr_template.list_ptr[i]),
                sizeof(recvd_params->rx.fltr_template.list_ptr[i]));
        mod_rx_fltr->num_filters++;
        
        curr_fltr_ids->rx_filter_ids[curr_fltr_ids->rx_num_filters++] = 
                                   recvd_fltr_ids.rx_filter_ids[i];
      }/* replaced */
    }/* outer for */
  }/* rx_num_filters */
  if ( recvd_fltr_ids.tx_num_filters )
  {
    mod_qos_params->field_mask |= 
        (qos_spec_field_mask_type)QOS_MODIFY_MASK_TX_FLTR_MODIFY;

    for (i=0;i < recvd_fltr_ids.tx_num_filters;++i)
    {
      for (replaced = FALSE,j=0;j < curr_fltr_ids->tx_num_filters; ++j)
      {
        if ( recvd_fltr_ids.tx_filter_ids[i] == curr_fltr_ids->tx_filter_ids[j])
        {
          DS_3GPP_MSG1_HIGH("Tx Filter id %d is getting replaced ",
                             recvd_fltr_ids.tx_filter_ids[i]);
          memscpy((void *)&(mod_tx_fltr->list_ptr[j]), 
		  sizeof(mod_tx_fltr->list_ptr[j]),
                  (const void *)&(recvd_params->tx.fltr_template.list_ptr[i]),
                  sizeof(recvd_params->tx.fltr_template.list_ptr[i]));
          replaced = TRUE;
          break;
        }/* match */
      }/* inner for */
      if (!replaced )
      {
        DS_3GPP_MSG1_HIGH("Tx Filter %d not found for replace, ADDING it",
                           recvd_fltr_ids.tx_filter_ids[i]);

         memscpy((void *)&(mod_tx_fltr->list_ptr[mod_tx_fltr->num_filters]),
		 sizeof(mod_tx_fltr->list_ptr[mod_tx_fltr->num_filters]),
                 (const void *)&(recvd_params->tx.fltr_template.list_ptr[i]),
                 sizeof(recvd_params->tx.fltr_template.list_ptr[i]));
        mod_tx_fltr->num_filters++;
        curr_fltr_ids->tx_filter_ids[curr_fltr_ids->tx_num_filters++] = 
                           recvd_fltr_ids.tx_filter_ids[i];
       }/* replaced */
    }/* outer for */
  }/* tx_num_filters */
}/* dsumtsps_mt_modify_tft_replace */
  /*===========================================================================
  FUNCTION  DSUMTSPS_MT_MODIFY_TFT_DELETE_FILTER
  
  DESCRIPTION
  This function performs the operation DELETE filters in MODIFY request. 
  Delete filters, deletes the matched filter ids in the current list,
  if the filter ids are not matched it is ignored. 
  PARAMETERS
  
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None.
  
  ===========================================================================*/
LOCAL void dsumtsps_mt_modify_tft_delete_filter
(
  qos_spec_type                 *recvd_params,
  qos_spec_type                 *mod_qos_params,
  dsumtsps_filter_details_type  recvd_fltr_ids,
  dsumtsps_filter_details_type  *curr_fltr_ids
)
{
  ip_filter_spec_type  *mod_rx_fltr = NULL;
  ip_filter_spec_type   *mod_tx_fltr = NULL;
  uint8 i = 0;
  uint8 j = 0;
  uint8 k = 0;

  mod_rx_fltr = &(mod_qos_params->rx.fltr_template);
  mod_tx_fltr = &(mod_qos_params->tx.fltr_template);

  DS_3GPP_MSG0_HIGH("Modify is for OPCODE_DELETE");

  /* ---------------------------------------------------------------------
       DELETE: Remove the filter id and filter 
  
       Move up all the filter id's from the id to be deleted and peform
       similar operation on the filters
  -----------------------------------------------------------------------*/
  for (i=0;i < recvd_fltr_ids.rx_num_filters;++i)
  {
    for (j=0;j < curr_fltr_ids->rx_num_filters; ++j)
    {
      if ( recvd_fltr_ids.rx_filter_ids[i] == 
                                      curr_fltr_ids->rx_filter_ids[j])
      {
        DS_3GPP_MSG3_HIGH("Rx filter id %d is getting deleted",
                                   recvd_fltr_ids.rx_filter_ids[i],0,0);
        for (k= j; (k  < MAX_PACKET_FILTERS -1) && (k < curr_fltr_ids->rx_num_filters - 1);++k)
        {
          curr_fltr_ids->rx_filter_ids[k] = 
                                      curr_fltr_ids->rx_filter_ids[k+1];
          memscpy((void *)&(mod_rx_fltr->list_ptr[k]), 
		  sizeof(mod_rx_fltr->list_ptr[k]),
                  (const void *)&(mod_rx_fltr->list_ptr[k+1]),
                  sizeof (mod_rx_fltr->list_ptr[k+1]));
        }
        curr_fltr_ids->rx_num_filters--;
        mod_rx_fltr->num_filters--;
        break;
      }
    }
  }/* rx_filters */
  
  for (i=0;i < recvd_fltr_ids.tx_num_filters;++i)
  {
    for (j=0;j < curr_fltr_ids->tx_num_filters; ++j)
    {
      if ( recvd_fltr_ids.tx_filter_ids[i] == 
                                      curr_fltr_ids->tx_filter_ids[j])
      {
        DS_3GPP_MSG1_HIGH("Tx filter id %d is getting deleted",
                           recvd_fltr_ids.tx_filter_ids[i]);
        for (k= j; (k < MAX_PACKET_FILTERS -1 ) && (k < curr_fltr_ids->tx_num_filters - 1);++k)
        {
          curr_fltr_ids->tx_filter_ids[k] = 
                                     curr_fltr_ids->tx_filter_ids[k+1];
          memscpy((void *)&(mod_tx_fltr->list_ptr[k]), 
		  sizeof(mod_tx_fltr->list_ptr[k]),
                  (const void *)&(mod_tx_fltr->list_ptr[k+1]),
                  sizeof (mod_tx_fltr->list_ptr[k+1]));
        }
        curr_fltr_ids->tx_num_filters--;
        mod_tx_fltr->num_filters--;
        break;
      }
    }
  }/* tx_filters */
  
  if ( curr_fltr_ids->rx_num_filters )
  {
    mod_qos_params->field_mask |= 
        (qos_spec_field_mask_type)QOS_MODIFY_MASK_RX_FLTR_MODIFY;
  }
  
  if ( curr_fltr_ids->tx_num_filters )
  {
    mod_qos_params->field_mask |= 
        (qos_spec_field_mask_type)QOS_MODIFY_MASK_TX_FLTR_MODIFY;
  }
}/* dsumtsps_mt_modify_tft_delete_filter */
  /*===========================================================================
  FUNCTION  DSUMTSPS_MT_MODIFY_TFT_CREATE
  
  DESCRIPTION
  This function performs the operation CREATE in MODIFY request. 
  Create operation, overwrites the current filters with the newly received.
  PARAMETERS
  
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None.
  
  ===========================================================================*/
LOCAL void dsumtsps_mt_modify_tft_create
(
  qos_spec_type                 *recvd_params,
  qos_spec_type                 *mod_qos_params,
  dsumtsps_filter_details_type  recvd_fltr_ids,
  dsumtsps_filter_details_type  *curr_fltr_ids

)
{
  ip_filter_spec_type  *mod_rx_fltr = NULL;
  ip_filter_spec_type   *mod_tx_fltr = NULL;

  mod_rx_fltr = &(mod_qos_params->rx.fltr_template);
  mod_tx_fltr = &(mod_qos_params->tx.fltr_template);

  DS_3GPP_MSG0_HIGH("Modify is for OPCODE_CREATE");
  /* ------------------------------------------------------------------
       CREATE: Process this new create operation and if succesfull
               delete the old TFT's
  
      Copy the new TFT's received and completely over write current.
  --------------------------------------------------------------------*/

  if ( recvd_fltr_ids.rx_num_filters )
  {
    mod_qos_params->field_mask |= 
        (qos_spec_field_mask_type)QOS_MODIFY_MASK_RX_FLTR_MODIFY;
    memscpy((void *)&(mod_rx_fltr->list_ptr[0]),
            sizeof(mod_rx_fltr->list_ptr[0]),
            (const void *)&(recvd_params->rx.fltr_template.list_ptr[0]),
            (sizeof(recvd_params->rx.fltr_template.list_ptr[0]) * 
                        recvd_fltr_ids.rx_num_filters) );
  }
  
  if ( recvd_fltr_ids.tx_num_filters )
  {
    mod_qos_params->field_mask |= 
        (qos_spec_field_mask_type)QOS_MODIFY_MASK_TX_FLTR_MODIFY;
    memscpy((void *)&(mod_tx_fltr->list_ptr[0]),
            sizeof(mod_tx_fltr->list_ptr[0]),
            (const void *)&(recvd_params->tx.fltr_template.list_ptr[0]),
            (sizeof(recvd_params->tx.fltr_template.list_ptr[0]) * 
                        recvd_fltr_ids.tx_num_filters) );
  }
  memscpy ( (void *)curr_fltr_ids,
  	    sizeof(dsumtsps_filter_details_type),
            (const void *)&recvd_fltr_ids,
            sizeof(dsumtsps_filter_details_type) );
  
  mod_tx_fltr->num_filters = 
                     recvd_fltr_ids.tx_num_filters;
  mod_rx_fltr->num_filters = 
                     recvd_fltr_ids.rx_num_filters;
  
  DS_3GPP_MSG2_HIGH("Replaced with received %d tx filters and %d rx filters",
               curr_fltr_ids->tx_num_filters,
               curr_fltr_ids->rx_num_filters);

}/* dsumtsps_mt_modify_tft_create */

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
)
{
  ip_filter_type       *filter;
  uint8                precedence = 0;
  void                 *rx_handle;      /* Handle to the Rx filter         */
  void                 *tx_handle;      /* Handle to the Tx filter         */
  void                 *handle;      /* Handle to the Tx filter         */
  void                 *new_handle;/* Handle to the next Tx filter         */
  uint8                found_match = 0;
  ip_filter_spec_type  *mod_rx_fltr = NULL;
  ip_filter_spec_type   *mod_tx_fltr = NULL;
  uint8 i,j; 


  ASSERT( (recvd_params != NULL ) &&( mod_qos_params != NULL ));
  ASSERT ( curr_fltr_ids != NULL );
  ASSERT ( (flow_ptr != NULL) &&(iface_ptr !=NULL));
  ASSERT ( cause != NULL);

  mod_rx_fltr = &(mod_qos_params->rx.fltr_template);
  mod_tx_fltr = &(mod_qos_params->tx.fltr_template);
  /* --------------------------------------------------------------------
    1. Get Tx Handle
    2. Get Rx List 
    3. Preapre the Tx filter list from handle and assign filter id's
    4. Check the operation code and prepare the final set of filters
   ----------------------------------------------------------------------*/

  DS_3GPP_MSG3_HIGH("In dsumtsps_mt_modify_filter_params with current %d rcvd %d ",
                      curr_fltr_ids->rx_num_filters,
                      recvd_fltr_ids.rx_num_filters,0);

  if ( recvd_fltr_ids.op_code == (byte)DS_UMTS_TFT_OPCODE_CREATE ) 
  {
    dsumtsps_mt_modify_tft_create(recvd_params,
                                  mod_qos_params,
                                  recvd_fltr_ids,
                                  curr_fltr_ids );
    return TRUE;
  }


  /* ---------------------------------------------------------------------
                      Search for matching filter ID's for opcode ADD 
    -----------------------------------------------------------------------*/
  if ( recvd_fltr_ids.op_code == (byte)DS_UMTS_TFT_OPCODE_ADD)
  {
    for (i=0;i < recvd_fltr_ids.rx_num_filters;++i)
    {
      for (j=0;j < recvd_fltr_ids.rx_num_filters; ++j)
      {
        if ( (recvd_fltr_ids.rx_filter_ids[j] == recvd_fltr_ids.rx_filter_ids[i]) 
               && (i != j))
        {
          found_match = recvd_fltr_ids.rx_filter_ids[j];
          break;
        }
      }
    }
      for (i=0;i < recvd_fltr_ids.tx_num_filters;++i)
    {
      for (j=0;j < recvd_fltr_ids.tx_num_filters; ++j)
      {
          if ( recvd_fltr_ids.tx_filter_ids[j] == recvd_fltr_ids.tx_filter_ids[i]
               && (i != j))
        {
          found_match = recvd_fltr_ids.tx_filter_ids[j];
          break;
        }
      }
    }
    
    if ( found_match )
    {
      if ( recvd_fltr_ids.op_code == (byte)DS_UMTS_TFT_OPCODE_ADD )
      {
        DS_3GPP_MSG1_ERROR("Duplicate Filter id %d in new request Operation ADD Failed",
                            found_match);
        *cause = SM_SYNTACTIC_ERR_IN_FILTER;
        return FALSE;
      }
    }
  }
  /* ------------------------------------------------------------------
           Get the current Rx and Tx filters from PS and align 
           with filters ID's maintained by DS. 
           Once all the filters are pulled, perform operations and 
           update the filters and filter id's accordingly 
  --------------------------------------------------------------------*/
  rx_handle = ps_flow_get_rx_fltr_handle(flow_ptr, IP_V4,FALSE);
  tx_handle = ps_iface_get_tx_fltr_handle(iface_ptr, flow_ptr, IP_V4, FALSE );

  handle = rx_handle;
  while ( handle != NULL )  /* Get Rx Filter by handle  */
  {
    if(ps_flow_get_rx_fltr_by_handle(flow_ptr,
                                     IP_V4,
                                     handle,
                                     modify_flag,
                                     &filter,
                                     &precedence,
                                     &new_handle) == FALSE)
    {
      DS_3GPP_MSG1_HIGH("Could not get RX filter by handle for phys link %d",
                         flow_ptr);
      *cause = SM_ACTIVATION_REJECTED_UNSPECIFIED;
      return FALSE;
    }
    handle = new_handle;

    memscpy((void *)&(mod_rx_fltr->list_ptr[mod_rx_fltr->num_filters]),
            sizeof(mod_rx_fltr->list_ptr[mod_rx_fltr->num_filters]),
            (const void *)filter, sizeof (ip_filter_type));
    mod_rx_fltr->num_filters++;

  } /* while ( handle != NULL ) */

  DS_3GPP_MSG1_HIGH("Retreived %d Rx filters from PS ",
                     mod_rx_fltr->num_filters);

  handle = tx_handle;
  while ( handle != NULL ) 
  {
    if(ps_iface_get_tx_fltr_by_handle(iface_ptr,
                                      flow_ptr,
                                      IP_V4,
                                      handle,
                                      modify_flag,
                                     &filter,
                                     &precedence,
                                     &new_handle) == FALSE)
    {
      DS_3GPP_MSG1_HIGH("Could not get RX filter by handle for phys link %d",
                                                 flow_ptr);
     *cause = SM_ACTIVATION_REJECTED_UNSPECIFIED;
      return FALSE;
    }
    handle = new_handle;
    memscpy((void *)&(mod_tx_fltr->list_ptr[mod_tx_fltr->num_filters]),
	    sizeof(mod_tx_fltr->list_ptr[mod_tx_fltr->num_filters]),
            (const void *)filter, sizeof (ip_filter_type));
    mod_tx_fltr->num_filters++;
  }
  DS_3GPP_MSG1_HIGH("Retreived %d Tx filters from PS ", 
                     mod_tx_fltr->num_filters);

  /* -----------------------------------------------------------------------
               Perform operation based on opcode receieved 
  -------------------------------------------------------------------------*/
  switch ( recvd_fltr_ids.op_code )
  {
  /* ---------------------------------------------------------------------
      REPLACE: Match the filter ID to be replaced and replace the filters
      If there is no match, add the filters to the current as per 24.008

      ADD: As per 24.008, ADD filter is similar to REPLACE
          1. If a filter id in new filter list is already present in 
             current filters, then delete the current filter and add
             the new one; which is equivalent to replace. 
  -----------------------------------------------------------------------*/
  case DS_UMTS_TFT_OPCODE_ADD:
  case DS_UMTS_TFT_OPCODE_REPLACE:
  {
      dsumtsps_mt_modify_tft_replace(recvd_params,
                                     mod_qos_params,
                                     recvd_fltr_ids,
                                     curr_fltr_ids );
  }/* OPCODE_REPLACE */
    break;

  case DS_UMTS_TFT_OPCODE_DEL_FILTER:
  {
    dsumtsps_mt_modify_tft_delete_filter(recvd_params,
                                   mod_qos_params,
                                   recvd_fltr_ids,
                                   curr_fltr_ids );
  } 
    break;
  default:
    DS_3GPP_MSG1_ERROR("Invalid Operation received %d ",
                        recvd_fltr_ids.op_code);
    *cause = SM_SYNTACTIC_ERR_IN_TFT;
    break;
  }
  return TRUE;
}/* dsumtsps_mt_modify_filter_params */
#endif /* FEATURE_NW_INIT_QOS */

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION DSUMTSPS_FILL_V6_TFT_PARAMS_FOR_SEC

DESCRIPTION
   Convert the application given filter parameters to setup the Call Manager 
   TFT parameters for IPV6 context activation and modification
    
PARAMETERS
  flow_ptr       - Ptr to the flow
  tft_params_ptr - Pointer to the TFT to populate
  filter_offset  - Offset at which to start processing the filter
  num_filters    - Number of filters to process and populate 
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE - Success
  FALSE - Failure
  
SIDE EFFECTS 
  None.
===========================================================================*/
LOCAL boolean dsumtsps_fill_v6_tft_params_for_sec
(
  ps_flow_type                 *flow_ptr,
  tft_type_T                   *tft_params_ptr,
  uint8                        filter_offset,
  byte                         num_filters,
  boolean                      modify_flag
)
{
  uint8                filter_count, loop, precedence = 0;
  ip_filter_type       *filter;
  uint8                tft_len = 1, fltr_len, ip6_addr_len,ip6_prefix_len ;
  uint16               temp_portnum, temp_portrng; 
                                     /* to convert network to host order   */
  uint8                filter_id;    /* Filter Identifier                  */
  void                 *handle;      /* Handle to the filter               */
  void                 *new_handle;  /* Handle to the next filter          */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ip6_addr_len = 16;

  /*-----------------------------------------------------------------------
    Initialize the filter count and filter id
  -----------------------------------------------------------------------*/
  filter_count = 0;

  filter_id = filter_offset;

  /* Rx Filters must be non zero */
  if ( num_filters == 0)
  {
    DS_3GPP_MSG0_ERROR("ERROR: Zero Rx filters received ");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Get the handle to the first filter.
  -----------------------------------------------------------------------*/
  handle = ps_flow_get_rx_fltr_handle(flow_ptr, IP_V6, modify_flag);

  /*-----------------------------------------------------------------------
    Loop though the filters till you get to the filter offset at which you
    want to start populating the filters.
  -----------------------------------------------------------------------*/
  while (filter_offset > 0) 
  {
    if(ps_flow_get_rx_fltr_by_handle(flow_ptr,
                                     IP_V4,
                                     handle,
                                     modify_flag,
                                     &filter,
                                     &precedence,
                                     &new_handle) == FALSE)
    {
      DS_3GPP_MSG1_HIGH("Could not get RX filter by handle for phys link %d",
                         flow_ptr);
      return FALSE;
    }

    handle = new_handle;
    filter_offset--;
  }


  /*-----------------------------------------------------------------------
    Loop till the number of filters specified have been processed.
  -----------------------------------------------------------------------*/
  while (num_filters > 0)
  {
    fltr_len = 0;

    if(ps_flow_get_rx_fltr_by_handle(flow_ptr,
                                  IP_V6,
                                  handle,
                                  modify_flag,
                                  &filter,
                                  &precedence,
                                  &new_handle) == FALSE)
    {
      DS_3GPP_MSG1_HIGH("Could not get RX filter by handle for phys link %d",
                         flow_ptr);
      return FALSE;
    }

    handle = new_handle;

    ip6_prefix_len = filter->ip_hdr.v6.src.prefix_len;

    ASSERT ( filter->ip_vsn == IP_V6 );

    /*-----------------------------------------------------------------------
      The packet filter identifier field is used to identify each packet 
      filter in a TFT.
      Set the Packet Filter Identifier Mask 
    -----------------------------------------------------------------------*/
    tft_params_ptr->filter_list[filter_count].identifier = filter_id+1;
    
    /*-----------------------------------------------------------------------
      The packet filter evaluation precedence field is used to specify the 
      precedence for the packet filter among all packet filters in all TFTs 
      associated with this PDP address. We currently set the precedence
      of FIFO manner.
    -----------------------------------------------------------------------*/
    tft_params_ptr->filter_list[filter_count].eval_precedence = 
                                                                precedence;

    /*----------------------------------------------------------------------- 
      Set the Packet Filter IPV6 Source Address and Mask 
    -----------------------------------------------------------------------*/

    if ( filter->ip_hdr.v6.field_mask & (int)IPFLTR_MASK_IP6_SRC_ADDR )
    { 
     tft_params_ptr->filter_list[filter_count].\
     filter_contents[fltr_len++] = (int)DS_UMTS_TFT_IPV6_ADDR;
     /*------------------------------------------------------------------- 
       Set the Packet Filter Identifier Mask : IPV6 Addr 
      ------------------------------------------------------------------*/
     for(loop = 0; loop < ip6_addr_len; loop++)    
     {

      tft_params_ptr->filter_list[filter_count].\
      filter_contents[fltr_len++] = 
         filter->ip_hdr.v6.src.addr.ps_s6_addr[loop];

     }

      /*------------------------------------------------------------------ 
        Set the Packet Filter Address Mask 
      ------------------------------------------------------------------*/
     for(loop = 0; loop < ip6_prefix_len /8; loop++)    
     {
        tft_params_ptr->filter_list[filter_count].\
          filter_contents[fltr_len++] = 0xFF;
     }
      
     for(; loop < ip6_addr_len; loop++)    
     {
        tft_params_ptr->filter_list[filter_count].\
          filter_contents[fltr_len++] = 0x00;
     }
     ip6_prefix_len -= loop*8;
  
     for(loop = 0; loop < ip6_prefix_len ; loop++)    
     {
        tft_params_ptr->filter_list[filter_count].\
          filter_contents[ip6_prefix_len /8] |= 0x01 << (8-loop);
     }
      
     DS_3GPP_MSG2_HIGH ( " Filter %d : SRC ADDR : %d ",
                   filter_count, filter->ip_hdr.v4.src.addr.ps_s_addr);
     DS_3GPP_MSG2_HIGH ("Filter %d : SNET MSK : %d",
                         filter_count, 
                         filter->ip_hdr.v4.src.subnet_mask.ps_s_addr); 
    }

    /*-----------------------------------------------------------------------
      Set the Packet Filter next header protocol 
    -----------------------------------------------------------------------*/
    if ( filter->ip_hdr.v6.field_mask & (int)IPFLTR_MASK_IP6_NEXT_HDR_PROT )
    {   
      /*--------------------------------------------------------------------- 
        Set the Packet Filter Identifier Mask : Next Header Type 
      ---------------------------------------------------------------------*/
      tft_params_ptr->filter_list[filter_count].\
         filter_contents[fltr_len++] = (int)DS_UMTS_TFT_NEXT_HDR_TYPE;
     
      tft_params_ptr->filter_list[filter_count].
        filter_contents[fltr_len++] = filter->ip_hdr.v6.next_hdr_prot;
      DS_3GPP_MSG2_HIGH ("Filter %d : NXT HDR PROTOCOL :%d ",
                          filter_count, 
                          filter->ip_hdr.v6.next_hdr_prot);

      /*--------------------------------------------------------------------- 
        Set the Packet Filter destination and Source port range 
      ---------------------------------------------------------------------*/
      if ( filter->ip_hdr.v6.next_hdr_prot == (int)PS_IPPROTO_TCP )
      {     
        /*------------------------------------------------------------------- 
          TCP destination Port has been set 
        -------------------------------------------------------------------*/
        if ( filter->next_prot_hdr.tcp.field_mask & (int)IPFLTR_MASK_TCP_DST_PORT )
        {
          if ( filter->next_prot_hdr.tcp.dst.range == 0 )
          {
            temp_portnum =0;

            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Single Dest Port 
            ---------------------------------------------------------------*/
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SINGLE_DEST_PORT;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.tcp.dst.port );

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8;
          
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
          
            DS_3GPP_MSG2_HIGH("Filter %d : SINGLE TCP DEST PORT: %d",
                               filter_count, temp_portnum);
          }
          else
          {
            /*---------------------------------------------------------------
              Set the Packet Filter Identifier Mask : Dest Port Range Type 
            ---------------------------------------------------------------*/
            temp_portnum = 0;
            temp_portrng = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_DEST_PORT_RANGE;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.tcp.dst.port );
            temp_portrng = filter->next_prot_hdr.tcp.dst.range;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
          
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
          
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =   
              ((temp_portnum + temp_portrng) & 0xFF00) >> 8;
          
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =   
              ( ( temp_portnum + temp_portrng ) & 0x00FF);
          
            DS_3GPP_MSG3_HIGH ( " Filter %d : TCP DEST PORT RANGE: %d, %d ",
                       filter_count, temp_portnum, temp_portnum+temp_portrng);
          }
        } /* if (DST PORT) */

        /*------------------------------------------------------------------- 
          TCP source Port has been set 
        -------------------------------------------------------------------*/
        if ( filter->next_prot_hdr.tcp.field_mask & (int)IPFLTR_MASK_TCP_SRC_PORT )
        {
          if ( filter->next_prot_hdr.tcp.src.range == 0 )
          {
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Single Source Port 
            ---------------------------------------------------------------*/
            temp_portnum = 0;

            tft_params_ptr->filter_list[filter_count].\
                filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SINGLE_SRC_PORT;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.tcp.src.port );
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
      
            DS_3GPP_MSG2_HIGH("Filter %d: SINGLE TCP SRC PORT: %d",
                               filter_count, temp_portnum);
          }
          else
          {
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Source Port Range 
            ---------------------------------------------------------------*/
            temp_portnum = 0;
            temp_portrng = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SRC_PORT_RANGE;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.tcp.src.port );
            temp_portrng = filter->next_prot_hdr.tcp.src.range;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =   
              ( ( temp_portnum + temp_portrng) & 0xFF00) >> 8;
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =   
              ( ( temp_portnum + temp_portrng) & 0x00FF);
      
            DS_3GPP_MSG3_HIGH ( " Filter %d : TCP SRC PORT RANGE: %d, %d ",
                       filter_count, temp_portnum, temp_portnum+temp_portrng);
          }
        }
      }
      else if ( filter->ip_hdr.v6.next_hdr_prot == (int)PS_IPPROTO_UDP )
      {       
        /*------------------------------------------------------------------- 
          UDP destination Port has been set 
        -------------------------------------------------------------------*/
        if ( filter->next_prot_hdr.udp.field_mask & (int)IPFLTR_MASK_UDP_DST_PORT )
        {
          if ( filter->next_prot_hdr.udp.dst.range == 0 )
          {   
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Single Dest Port 
            ---------------------------------------------------------------*/
            temp_portnum = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SINGLE_DEST_PORT;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.udp.dst.port );
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
          
            DS_3GPP_MSG2_HIGH("Filter %d: SINGLE UDP DEST PORT: %d",
                               filter_count, temp_portnum);
          }
          else
          {
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Dest Port Range Type 
            ---------------------------------------------------------------*/
            temp_portnum = 0;
            temp_portrng = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_DEST_PORT_RANGE;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.udp.dst.port );
            temp_portrng = filter->next_prot_hdr.udp.dst.range;


            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =   
              ( ( temp_portnum + temp_portrng) & 0xFF00) >> 8;
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =   
              ( ( temp_portnum + temp_portrng) & 0x00FF);
      
            DS_3GPP_MSG3_HIGH ( " Filter %d : UDP DEST PORT RANGE: %d, %d ",
                       filter_count, temp_portnum, temp_portnum+temp_portrng);
          }
        }

        /*------------------------------------------------------------------- 
          UDP Source Port has been set 
        -------------------------------------------------------------------*/
        if ( filter->next_prot_hdr.udp.field_mask & (int)IPFLTR_MASK_UDP_SRC_PORT )
        {
          if ( filter->next_prot_hdr.udp.src.range == 0 )
          {   
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Single Source Port 
            ---------------------------------------------------------------*/
            temp_portnum = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SINGLE_SRC_PORT;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.udp.src.port );
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
      
            DS_3GPP_MSG2_HIGH("Filter %d: SINGLE UDP SRC PORT: %d",
                               filter_count, temp_portnum);
          }
          else
          {
            /*--------------------------------------------------------------- 
              Set the Packet Filter Identifier Mask : Source Port Range Type 
            ---------------------------------------------------------------*/
            temp_portnum = 0;
            temp_portrng = 0;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (int)DS_UMTS_TFT_SRC_PORT_RANGE;

            temp_portnum = ps_ntohs( filter->next_prot_hdr.udp.src.port );
            temp_portrng = filter->next_prot_hdr.udp.src.range;

            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =   
              ((temp_portnum + temp_portrng) & 0xFF00) >> 8;
      
            tft_params_ptr->filter_list[filter_count].\
              filter_contents[fltr_len++] =   
              ((temp_portnum + temp_portrng) & 0x00FF);
      
             DS_3GPP_MSG3_HIGH ( " Filter %d : UDP SRC PORT RANGE: %d, %d ",
                       filter_count, temp_portnum, temp_portnum+temp_portrng);
          }
        }
      } /* if(PS_IPPROTO_UDP) */
    } /* IF (IPFLTR_MASK_IP4_NEXT_HDR_PROT) */

    /*----------------------------------------------------------------------- 
      Set the Packet Filter Security Parameter Index (Supported in IPV6) 
    -----------------------------------------------------------------------*/

    /*----------------------------------------------------------------------- 
      Set the Packet Filter type of service: value and mask
    -----------------------------------------------------------------------*/
    if ( filter->ip_hdr.v6.field_mask & (int)IPFLTR_MASK_IP6_TRAFFIC_CLASS )
    {
      /*--------------------------------------------------------------------- 
        Set the Packet Filter Identifier Mask : Type of Service 
      ---------------------------------------------------------------------*/
      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = (int)DS_UMTS_TFT_IP6_TRAFFIC_CLASS;

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = filter->ip_hdr.v6.trf_cls.val;

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = filter->ip_hdr.v6.trf_cls.mask;

      DS_3GPP_MSG3_HIGH ( " Filter %d : TOS VAL: %d, TOS MASK: %d ",
                 filter_count, filter->ip_hdr.v4.tos.val, 
                 filter->ip_hdr.v4.tos.mask);
    }

    /*----------------------------------------------------------------------- 
      Set the Packet Filter Flow Label (Supported in IPV6) 
    -----------------------------------------------------------------------*/
    
    if ( filter->ip_hdr.v6.field_mask & (int)IPFLTR_MASK_IP6_FLOW_LABEL )
    {
      /*--------------------------------------------------------------------- 
        Set the Packet Filter Identifier Mask : Type of Service 
      ---------------------------------------------------------------------*/
      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = (int)DS_UMTS_TFT_FLOW_LABEL;

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = 
            (byte) (( filter->ip_hdr.v6.flow_label & 0x000F0000) >> 16);

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = 
            (byte) (( filter->ip_hdr.v6.flow_label & 0x0000FF00) >> 8);

      tft_params_ptr->filter_list[filter_count].\
        filter_contents[fltr_len++] = 
            (byte) (( filter->ip_hdr.v6.flow_label & 0x000000FF));

      DS_3GPP_MSG3_HIGH ( " Filter %d : Flow Label %d ",
                     filter->ip_hdr.v6.flow_label & 0x000FFFFF,0,0);
    }

    if ( fltr_len == 0 )
    {
      DS_3GPP_MSG1_ERROR("Invalid filter Components, Filter Length: %d",
                          fltr_len);
      return FALSE;
    }
    else
    {
      DS_3GPP_MSG1_HIGH("Filter Component Length: %d ",fltr_len);
    }

    tft_params_ptr->filter_list[filter_count].content_length = fltr_len;

    ASSERT( fltr_len < MAX_FILTER_CONTENT_LENGTH );

    tft_len += fltr_len;

    /*------------------------------------------------------------------------- 
      Increment the filter count
    -------------------------------------------------------------------------*/
    filter_count++;

    if ( filter_count == MAX_PACKET_FILTERS_IPV6 )
    {
      /*---------------------------------------------------------------------
        We can send only 4 IPV6 filters during the origination request. If the 
        application specifies more than 4 filters we would have to split them 
        into two: send the first 4 filters in the current origination request 
        and send the rest in the context modification procedure later. In the 
        current impl we will ignore the rest of the filters and just send 4
        in origination request. Modification added later (enhance) 
      ---------------------------------------------------------------------*/
      break;
    }
    /*------------------------------------------------------------------------- 
      Increment the filter identifier
    -------------------------------------------------------------------------*/
    filter_id++;

    /*------------------------------------------------------------------------- 
      Decrement to get the number of filters left to process.
    -------------------------------------------------------------------------*/
    num_filters--;
  }

  /*------------------------------------------------------------------------- 
    Update the num_filters field with the filter count.
  -------------------------------------------------------------------------*/
  tft_params_ptr->num_filters         = filter_count;

  /*------------------------------------------------------------------------- 
    The length of the packet filter contents field contains the binary coded
    representation of the length of the packet filter contents field of a 
    packet filter.     
    Set the total TFT length : Header + Length of total number of filters 
  -------------------------------------------------------------------------*/
  tft_params_ptr->tft_total_length = tft_len;

  DS_3GPP_MSG1_HIGH("Total Filter Length: %d",tft_len);

  return TRUE;
} /* dsumtsps_fill_v6_tft_params_for_sec() */

#endif




/*===========================================================================
FUNCTION DSUMTSPS_SEC_QOS_MODIFY_TFT_DELETE

DESCRIPTION
  This function deletes an existing TFT for a IPv4 or IPv6 secondary context 
  during MO context modification.
  For the "delete existing TFT" operation, the packet filter list shall be 
  empty and the number of packet filters shall be coded as 0. 
    
PARAMETERS
  tft_params_ptr    - Pointer to the TFT to populate
  call_instance     - Index into the call table.
  
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
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    Set the operation code to DELETE_TFT and number of filters to 0.
  -------------------------------------------------------------------------*/
  tft_params_ptr->valid               = TRUE;
  tft_params_ptr->operation_code      = (int)DS_UMTS_TFT_OPCODE_DEL_TFT;
  tft_params_ptr->num_filters         = 0;

  return TRUE;

} /* dsumtsps_sec_qos_modify_tft_delete() */


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
)
{
  uint8 filter_cnt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  tft_params_ptr->valid               = TRUE;
  tft_params_ptr->operation_code      = (int)DS_UMTS_TFT_OPCODE_DEL_FILTER;

  /*-----------------------------------------------------------------------
    Delete num_filters starting at the offset
  -----------------------------------------------------------------------*/
  for (filter_cnt = 0; filter_cnt < num_filters; filter_cnt++) 
  {
    filter_offset++;
    tft_params_ptr->filter_list[filter_cnt].identifier = 
                                                filter_offset;
  }

  /*-----------------------------------------------------------------------
    Populate the number of packet filters to be deleted.
  -----------------------------------------------------------------------*/
  tft_params_ptr->num_filters = num_filters;

  return TRUE;

} /* dsumtsps_sec_qos_modify_tft_delete_fltr() */


#endif

