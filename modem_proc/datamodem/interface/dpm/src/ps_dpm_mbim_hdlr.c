/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    P S _ D P M _ MBIM _ HDLR . C

GENERAL DESCRIPTION
  This is the implementation of the Data Path Manager module wrt processing
  of mbim packets

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpm_mbim_hdlr.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/19/13    bvd    created file.
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "ps_dpmi.h"
#include "ds_rmnet_xport.h"
#include "ps_dpm_mbim.h"
#include "ps_iface_mbim_defs.h"
#include "ps_dpm_opt_defs.h"
#include "ds_qmi_svc.h"
#include "ps_rm_svc.h"

#ifdef FEATURE_DATA_MBIM
#define PS_OPT_PATH_MBIM_IPS_MASK        0x000000FF

static uint8 * ndp_buf  = NULL;


void ps_dpm_mbim_phys_port_format_set
(
  void             * phys_xport,
  uint32             ul_data_agg_max_size
)
{	   
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_2("ps_dpm_mbim_phys_port_format_set "
                  "phys_xport 0x%x agg_size %d ", 
                  phys_xport, ul_data_agg_max_size);

  (void)ps_rm_set_sig_handler( PS_RM_MBIM_RX_DATA_Q_SIGNAL,
                              (ps_rm_sig_handler_type)ps_dpm_mbim_rx_sig_hdlr,
                              phys_xport);
  ps_rm_enable_sig(PS_RM_MBIM_RX_DATA_Q_SIGNAL); 
  
  if (NULL != ndp_buf)
  {
    PS_SYSTEM_HEAP_MEM_FREE (ndp_buf);
  }
  
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(ndp_buf, ul_data_agg_max_size, uint8*);
  if (NULL == ndp_buf)
  {
    return;
  }
  
  memset(ndp_buf, 0, ul_data_agg_max_size);
} /* ps_dpm_mbim_phys_port_format_set */

/*===========================================================================
FUNCTION PS_DPM_MBIM_HDLR_NDP_PROCESS

DESCRIPTION
  This function processes the NDP in MBIM packet and enqueues ip data
  to logical rx wmk

PARAMETERS
  rx_wmk              :  Pointer to logical rx wmk
  ndp_datagram_offset :  IP/Eth offset in NDP
  ntb_dsm_item        :  MBIM pkt
  rx_pkt              :  IP/ETH pkt
  ndp_buf             :  ndp buffer

RETURN VALUE
  TRUE  :
  FALSE :

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint8 ps_dpm_mbim_hdlr_ndp_process
(
  dsm_watermark_type           * rx_wmk,
  uint16                         ndp_datagram_offset,
  dsm_item_type                * ntb_dsm_item,
  dsm_item_type               ** rx_pkt,
  uint8                        * ndp_buf,
  ps_iface_mbim_ntb_nth_hdr      nth_hdr,
  uint16                         ndp_offset
)
{
  ps_iface_mbim_ntb_ndp_ip_datagram_info * ndp_ip_datagram_info;
  ps_iface_mbim_ntb_ndp_hdr              * ndp_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_0 ("ps_dpm_mbim_hdlr_ndp_process");

  ndp_ip_datagram_info = (ps_iface_mbim_ntb_ndp_ip_datagram_info *)
                           (ndp_buf + PS_OPT_PATH_NDP_HDR_LEN) ;
  ndp_hdr              = (ps_iface_mbim_ntb_ndp_hdr *)ndp_buf;

  while ( ndp_datagram_offset < ndp_hdr->len )
  {
    LOG_MSG_INFO3_1("ps_dpm_mbim_hdlr_ndp_process : Start to "
	                "process ndp_dg_offset %d", ndp_datagram_offset);
					
    /*---------------------------------------------------------------------
      Validate datagram Index and length and perform IP processing
      dgindex == 0 -> Terminate
    ---------------------------------------------------------------------*/
    if ( ndp_ip_datagram_info->index == 0)
    {
      LOG_MSG_INFO3_1("ps_dpm_mbim_hdlr_ndp_process : ndp datagram %d "
	                  "terminated with zero", ndp_datagram_offset);

      break; /*break and continue with next NDP*/
    }
    else  /*Valid IP packet to process*/
    {
      LOG_MSG_INFO3_2("ps_dpm_mbim_hdlr_ndp_process : Process d/g "
	                  "nth_datagram_index: %d, ndp_datagram_offset:%d",
                      ndp_ip_datagram_info->index, ndp_datagram_offset);

      LOG_MSG_INFO3_1 ("ps_dpm_mbim_hdlr_ndp_process : dsm_dup of bytes:%d", 
	                   ndp_ip_datagram_info->len);

      /* Dup dsm item and start processing */
      /* Dup dsm item and start processing */
     if ( dsm_dup_packet( rx_pkt, ntb_dsm_item,
                           ndp_ip_datagram_info->index,
                           ndp_ip_datagram_info->len) !=
                           ndp_ip_datagram_info->len
         )
      {
        dsm_free_packet( rx_pkt );
      }
      else
      {
        /*-------------------------------------------------------------------
          Enqueue IP datagram to particular logical port rx watermakr
        -------------------------------------------------------------------*/
        dsm_enqueue(rx_wmk,rx_pkt);
      }

    } /*Else: Valid IP packet processing*/

    ndp_datagram_offset += PS_OPT_PATH_NDP_DG_OFFSET;
    ndp_ip_datagram_info ++;

  } /* while() for datagram processing in a NDP */

  return FALSE;
}/*ps_dpm_mbim_hdlr_ndp_process()*/


boolean ps_dpm_mbim_rx_sig_hdlr
(
  ps_rm_sig_enum_type      rx_sig,
  void                 * user_data
)
{
  ps_iface_mbim_ntb_ndp_hdr    * ndp_hdr;
  ps_iface_mbim_ntb_nth_hdr      nth_hdr;

  dsm_item_type  * ntb_dsm_item = NULL;   /*MBIM pkt*/
  dsm_item_type  * rx_pkt       = NULL;  /*IP pkt*/

  uint16 ndp_datagram_offset = 0;
  uint32 ndp_signature;
  uint16 ndp_length          = 0;
  uint16 ndp_offset          = 0;
  uint8  ips_id              = 0;
  uint8  rval                = FALSE;
  rmnet_xport_type           * logical_xport_ptr = NULL;
  rmnet_phys_xport_type      * phys_xport = NULL;
  dsm_watermark_type         * rx_wmk_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_0("In ps_dpm_mbim_rx_sig_hdlr");

  ASSERT(user_data);
  
  /*-------------------------------------------------------------------------
     Retrieve physical port information. MBIM packet is actually dequeued
     from physical port.
  -------------------------------------------------------------------------*/  
  phys_xport = (rmnet_phys_xport_type *)user_data;

  /*-------------------------------------------------------------------------
     Physical port is flow disabled if any one of its logical port is
     flow disabled. So if flow mask is set, return and donot dequeue
     any more. This would result in clearing of the signal PS_RM_MBIM_RX_DATA_Q_SIGNAL
  -------------------------------------------------------------------------*/    
  if (phys_xport->flow_mask != 0)
  {
    return TRUE;
  }
  
  rx_wmk_ptr = &(phys_xport->rx_wmk);
  
  ndp_signature = 0;  
  /*-------------------------------------------------------------------------
    Retrieve MBIM pkt from watermark
  -------------------------------------------------------------------------*/
  ntb_dsm_item = dsm_dequeue(rx_wmk_ptr);
  if (NULL == ntb_dsm_item)
  {
    return TRUE;
  }
  
  /*-------------------------------------------------------------------------
      Process NCM transfer Block (NTB)
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
     Retrieve complete NTH common header at once (12 bytes)
     1.Signature
     2.HeaderLength
     3.Sequence
     4.wBlockLength
     5.NDP Index
  -------------------------------------------------------------------------*/
  if ( (PS_OPT_PATH_NTHC_LEN + PS_OPT_PATH_NTHS_LEN) !=
                           dsm_extract( ntb_dsm_item,
                                        0,
                                        &nth_hdr,
                                        PS_OPT_PATH_NTHC_LEN +
                                        PS_OPT_PATH_NTHS_LEN
                                      )
     )
  {
    LOG_MSG_ERROR_0("Unable to retrieve NTH heder");
    dsm_free_packet(&ntb_dsm_item);
    return FALSE;
  }

  /*Error checking for all MBIM NTH header fields*/

  if ((nth_hdr.signature != RMNET_MBIM_HDR_NTH16_SIG)  ||
     (nth_hdr.hdr_len != PS_IFACE_MBIM_NTH16_HEAD_LEN) ||
     //((nth_hdr.mbim_block_len & 0x0003) != 0)  ||
     (nth_hdr.first_ndp_index < PS_IFACE_MBIM_NTH16_HEAD_LEN )
    )
  {
    LOG_MSG_ERROR_2("ps_dpm_mbim_rx_sig_hdlr : Unrecognized MBIM NTH Header"
                    ": signature 0x%8X or length %d",
                    nth_hdr.signature, nth_hdr.hdr_len);
    dsm_free_packet(&ntb_dsm_item);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
       Start processing NDPs one by one from NDP[0]
  -------------------------------------------------------------------------*/
  ndp_offset = nth_hdr.first_ndp_index;

  LOG_MSG_INFO3_1("ps_dpm_mbim_rx_sig_hdlr : First NDP Index at: %d", 
                  ndp_offset);
  
  while ( (ndp_offset != 0) && (ndp_offset < nth_hdr.mbim_block_len) )
  {
    /*----------------------------------------------------------------------
      Extract the NDP header length. Do a dsm_extract of length bytes to get
      complete NDP buf.
    ----------------------------------------------------------------------*/
    if ( PS_OPT_PATH_NDP_HDR_FIELD_LEN !=
            dsm_extract (
                          ntb_dsm_item,
                          (uint16)ndp_offset +
                          PS_OPT_PATH_NDP_HDR_FIELD_OFFSET,
                          &ndp_length,
                          PS_OPT_PATH_NDP_HDR_FIELD_LEN
                        )
        )
    {
      LOG_MSG_ERROR_0("ps_dpm_mbim_rx_sig_hdlr : Unable to retrieve "
	                  "NDP Header length field");
      dsm_free_packet(&ntb_dsm_item);
      return FALSE;
    }

    LOG_MSG_INFO3_1("ps_dpm_mbim_rx_sig_hdlr NDP length from NDP header" 
	                "len field : %d", ndp_length );

    if (ndp_buf == NULL)
    {
      LOG_MSG_ERROR_0("ps_dpm_mbim_rx_sig_hdlr : NDP buf not allocated");
      dsm_free_packet(&ntb_dsm_item);
      return FALSE;
    }

    if ( ndp_length != dsm_extract( ntb_dsm_item,
                                    (uint16)ndp_offset,
                                    ndp_buf,
                                    ndp_length)
       )
    {
      LOG_MSG_ERROR_0("ps_dpm_mbim_rx_sig_hdlr : Unable to retrieve full "
	                  "NDP buffer");
      PS_SYSTEM_HEAP_MEM_FREE (ndp_buf);
      dsm_free_packet(&ntb_dsm_item);
      return FALSE;
    }
 
    /*NDP Common header*/
    ndp_hdr = (ps_iface_mbim_ntb_ndp_hdr *)ndp_buf;

    ndp_signature = ndp_hdr->signature;

    /*-----------------------------------------------------------------------
       Retrieve ips_id from ndp sig and associated rmnet info.
    -----------------------------------------------------------------------*/
    ips_id    = (uint8)((ps_ntohl(ndp_signature)) &
                         PS_OPT_PATH_MBIM_IPS_MASK);

    LOG_MSG_INFO3_2("ps_dpm_mbim_rx_sig_hdlr ndp sig : 0x%08X , ips_id : %d", 
	                 ndp_signature, ips_id);

    /*-----------------------------------------------------------------------
       Start processing IP datagrams offsets and length in the NDP.
       We have all IP offsets and length in ndp_buf.
    -----------------------------------------------------------------------*/

    /*Start with first IP index/len*/

    ndp_datagram_offset  = PS_OPT_PATH_NDP_HDR_LEN;
    
    logical_xport_ptr = rmnet_logical_xport_from_phys_port
                        (
                          phys_xport,
                          ips_id
                        );

    if (TRUE == ps_dpm_mbim_hdlr_ndp_process( &(logical_xport_ptr->rx_wmk),
                                               ndp_datagram_offset,
                                               ntb_dsm_item,
                                               &rx_pkt,
                                               ndp_buf,
                                               nth_hdr,                                                       
                                               ndp_offset ))
    {
      return TRUE;
    }


    /*---------------------------------------------------------------------
       Free NDP buff if allocated. Here ndp_buf could be pointing to our
      static short_ndp_buf. Need to check this to free. Cant use Null check
       here since it is never NULL. So check for length and then free.

      But first get the next NDP offset and then free the curr ndp buff.
    ---------------------------------------------------------------------*/
    ndp_offset = ndp_hdr->next_ndp_index; /* Next ndp offset */

    LOG_MSG_INFO3_1("Next NDP offset:%d", ndp_offset);
  } /*while() for NDP processing*/

  dsm_free_packet(&ntb_dsm_item);
  rx_pkt = NULL;

  LOG_MSG_INFO3_1("ps_dpm_mbim_rx_sig_hdlr : Returning rval at end : %d", 
                  rval  );

  return rval; /*TODO Usually we will be returning FALSE here??*/

} /* ps_dpm_mbim_rx_sig_hdlr() */

void ps_dpm_mbim_enable_data_flow
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Set Rx Signal. It will be called either when there is data in 
    physical port rx watermark or when all logical ports are flow enabled
    and physical port goes from flow disbaled to flow enabled state. Check
    out rmnet_xport_set_phys_port_flow
  -------------------------------------------------------------------------*/  
  PS_RM_SET_SIGNAL(PS_RM_MBIM_RX_DATA_Q_SIGNAL);
} /* ps_dpm_mbim_enable_data_flow */

void ps_dpm_mbim_phys_rx_wmk_data_cb
(
 dsm_watermark_type  * wm,
 void                * cb_data
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Enable data flow
  -------------------------------------------------------------------------*/
  ps_dpm_mbim_enable_data_flow();
  
} /* ps_dpm_mbim_phys_rx_wmk_data_cb() */

#else

boolean ps_dpm_mbim_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  LOG_MSG_ERROR_0("ps_dpm_mbim_rx_sig_hdlr : MBIM not supported");
  return TRUE;
} /* ps_dpm_mbim_rx_sig_hdlr */

void ps_dpm_mbim_phys_rx_wmk_data_cb
(
 dsm_watermark_type  * wm,
 void                * cb_data
)
{
} /* ps_dpm_mbim_phys_rx_wmk_data_cb() */

void ps_dpm_mbim_phys_port_format_set
(
  void             * phys_xport,
  uint32             ul_data_agg_max_size;
)
{
} /* ps_dpm_mbim_phys_port_format_set */

void ps_dpm_mbim_enable_data_flow
(
  void
)
{
} /* ps_dpm_mbim_enable_data_flow */

#endif /* FEATURE_DATA_MBIM */
