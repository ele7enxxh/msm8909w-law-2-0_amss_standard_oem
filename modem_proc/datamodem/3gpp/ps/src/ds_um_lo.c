/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          U M T S  L O O P B A C K  M E C H A N I S M

GENERAL DESCRIPTION
  This file contains functions necessary for the implementation of the loopback 
  test mechanism. 

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

                                                                   
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_um_lo.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/12   kv      Replaced FEATURE_DATA_TEST_LOOPBACK_HDLR with 
                   FEATURE_DATA_TEST_LOOPBACK.
04/03/10   vk      Added support for MSG2.0 based logging
05/08/09   mal     Initial version.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#include "ds_um_lo.h"
#include "ds_loopback_hdlr.h"

#if (defined   (FEATURE_DATA_TEST_LOOPBACK) || \
     defined (FEATURE_DATA_TEST_LOOPBACK_PDCPSIM) \
    )
    
#include "ds_eps_bearer_context.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_context.h"
#include "cm.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "ds3gmgr.h"
#include "dsm.h"
#include "ps_svc.h"
#include "task.h"
#include "dsumtsps.h"       
#include "dstaski.h"
#include "ds3gmshif.h"
#include "ps_in.h"
#include "ps_iface_logging.h"
#include "ds_flow_control.h"
#include "ps_ip6_hdr.h"
#include "ps_iputil.h"
#include "ps_ip6_hdr.h"
#include "ps_pkt_info.h"
#include "ps_icmp6_msg.h"
#endif

#if defined(FEATURE_DATA_TEST_LOOPBACK) || defined(FEATURE_DATA_TEST_LOOPBACK_PDCPSIM)

extern void ds_bearer_cntxt_ps_to_L2_flow_enable_cb 
  (dsm_watermark_type *wm, void* callback_data);

extern void ds_bearer_cntxt_ps_to_L2_flow_disable_cb
  (dsm_watermark_type *wm, void* callback_data);

static dsumtsps_loopback_data_hdlr      dsumtsps_loopback_data_hdrl_ptr = NULL;
static dsumtsps_loopback_ip_addr_type   dsumtsps_loopback_ip_addr = {0, FALSE};
static boolean                          dsumtsps_loopback_active = FALSE;
static uint32                           dsumtsps_loopback_a2_port = 0;
static boolean                          dsumtsps_loopback_udp_chksum_verif = FALSE;

#endif 

#if (defined   (FEATURE_DATA_TEST_LOOPBACK) || \
     defined (FEATURE_DATA_TEST_LOOPBACK_PDCPSIM) )
/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_COMPUTE_CHECKSUM

DESCRIPTION
  Helper function to compute checksum     


PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
unsigned short int
dsumtsps_loopback_compute_checksum(unsigned char * pkt, long lpkt, long offset)
{
    int i, j;
    int pad = 0;
    unsigned long int csum = 0;

    if (!pkt || (lpkt == 0) || (offset > lpkt)) {
      DS_3GPP_MSG0_HIGH("dsumtsps_compute_checksum sanity check fail");
      return 0;
    }

    pkt += offset;

    i = (int)((lpkt-offset) >> 1);

    if ((i << 1) < (int)(lpkt-offset)) {
        pad = 1;
    }

    for (j = 0; j < i; ++j) {
        csum += (((short)*(pkt++)) << 8);
        csum += (short)*(pkt++);
    }

    if (pad) {
        csum += (((short)*(pkt)) << 8);
    }

    while (csum & 0xffff0000) {
        csum = (csum & 0x0000ffff) + ((csum & 0xffff0000) >> 16);
    }

    csum = ~csum;

    return (short int)csum;
}

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_CNTXT_PDP_IP_RX_UM_DATA_CB

DESCRIPTION
  This function forwards pkts received on Um interface to ps_iface for
  pdp-ip call.
  
PARAMETERS  
  PS SIG ENUM :
  user_data_p : Bearer Context
 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if Failure occurs PS Task will reset the signal ,
  FALSE PS Task does not reset signal and tries to pull data.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean dsumtsps_loopback_hdlr_cntxt_pdp_ip_rx_um_data_cb
(
  ps_sig_enum_type sig,
  void             *user_data_p
)
{

  dsm_item_type          *item_ptr=NULL;          /* Pointer to DSM item         */
  ds_bearer_context_wm_s *rx_watermark_ptr=NULL;
  ds_bearer_context_s    *bearer_context_p=NULL;  /* Pointer to bearer context   */
  boolean                result = TRUE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the bearer context from the call back data
  -------------------------------------------------------------------------*/
  bearer_context_p = (ds_bearer_context_s*)user_data_p;
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return result;
  }


  /*-------------------------------------------------------------------------
  RM_FLOW_ENABLED flag is set to TRUE always for Embedded calls.
  For Laptop calls, this flag is set based on the flow events happening on
  the Rm Phys link.
  -------------------------------------------------------------------------*/
  if(bearer_context_p->rm_flow_enabled == TRUE)
  {
    /*-----------------------------------------------------------------------
      Get PDCP state information for the call.
    -----------------------------------------------------------------------*/
    rx_watermark_ptr = &(bearer_context_p->rx_wm);

    /*-----------------------------------------------------------------------
      The PDCP wm item has IP packets queued up. De-queue and send it on the
      correct physlink.
    -----------------------------------------------------------------------*/
    item_ptr = (dsm_item_type *)dsm_dequeue(&(rx_watermark_ptr->wm_item));      
    if (item_ptr != NULL)
    {
      
      /*---------------------------------------------------------------------
        Data ready for PPP processing. Invoke function to process data
        stream.  
	    Call ps_phys_link_input with the dsm chain. This routes the packet to
        the correct iface (v4 or v6) and bridges it to the RM iface if need be 
      ---------------------------------------------------------------------*/
      (void)ps_phys_link_input(&(bearer_context_p->phys_link),
                               &item_ptr,NULL);
      result = FALSE;
    }    
  }
  return result;

} /* dsumtsps_loopback_cntxt_pdp_ip_rx_um_data_cb() */


/*===========================================================================
FUNCTION dsumtsps_loopback_hdlr_non_empty_data_cb

DESCRIPTION
  This function sets the PS signal for the specific bearer that has data.
  
PARAMETERS 
  bearer_context : Bearer context  
      
DEPENDENCIES 
  None.

RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void dsumtsps_loopback_hdlr_non_empty_data_cb
(
 void*              bearer_cntxt
)
{
  ds_bearer_context_s       *bearer_context_p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Get call instance from the callbackdata passed in
  -------------------------------------------------------------------------*/
  bearer_context_p = (ds_bearer_context_s*) bearer_cntxt;

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Set Rx Signal.
  -------------------------------------------------------------------------*/

  PS_SET_SIGNAL(bearer_context_p->rx_sig);
  
} /* dsumtsps_loopback_hdlr_non_empty_data_cb() */

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_COMPUTE_ICMP_CHECKSUM

DESCRIPTION
  Computes ICMP packet checksum

PARAMETERS
  pkt - pointer to ICMP packet
  lpkt - length of the packet

DEPENDENCIES
  None.

RETURN VALUE
  int

SIDE EFFECTS
  None.

===========================================================================*/
unsigned short int
dsumtsps_loopback_compute_icmp_checksum(unsigned char * pkt, long lpkt)
{
    unsigned short int csum;

    if (!pkt || (lpkt < IP_HDR_LENGTH + ICMP_CHECKSUM_OFFSET + 2)) {
      DS_3GPP_MSG0_HIGH("dsumtsps_compute_icmp_checksum sanity check fail");
      return 0;
    }

    /* Set csum field to 0000 */

    *(pkt+IP_HDR_LENGTH+ICMP_CHECKSUM_OFFSET) = 0;
    *(pkt+IP_HDR_LENGTH+ICMP_CHECKSUM_OFFSET+1) = 0;

    csum = dsumtsps_loopback_compute_checksum(pkt, lpkt, IP_HDR_LENGTH);

    /* write csum */
    *(pkt+IP_HDR_LENGTH+ICMP_CHECKSUM_OFFSET) = (unsigned char)(csum >> 8);
    *(pkt+IP_HDR_LENGTH+ICMP_CHECKSUM_OFFSET+1) = (unsigned char)csum;

    return csum;
}


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_COMPUTE_UDP_CHECKSUM

DESCRIPTION
  Computes UDP packet checksum

PARAMETERS
  pkt - pointer to ICMP packet
  lpkt - length of the packet

DEPENDENCIES
  None.

RETURN VALUE
  int

SIDE EFFECTS
  None.

===========================================================================*/
unsigned short int
dsumtsps_loopback_compute_udp_checksum(unsigned char * pkt, long lpkt)
{
    unsigned short int csum;
    dsumtsps_udp_hdr   hdr;
	unsigned char *tmp_pkt = NULL;

    /*-------------------------------------------------------------- 
      Get Allocated memory for tmp_pkt, If tmp_pkt is NULL return
      -------------------------------------------------------------*/
    tmp_pkt = ds_loopback_hdlr_get_dyn_unsigned_char();
    if (tmp_pkt == NULL)
    {
      DS_3GPP_MSG0_ERROR("dsumtsps_loopback_compute_udp_checksum memory allocation failure");
      return 0;
    }

    if (!pkt || (lpkt < IP_HDR_LENGTH + ICMP_CHECKSUM_OFFSET + 2) || 
        ((size_t)lpkt > DS_LOOPBACK_HDLR_TMP_PKT_SIZE))
    {
      DS_3GPP_MSG0_HIGH("dsumtsps_compute_udp_checksum sanity check fail");
      return 0;
    }

    (void)memset(tmp_pkt,0,DS_LOOPBACK_HDLR_TMP_PKT_SIZE);

    /* Set csum field to 0000 */
    *(pkt+IP_HDR_LENGTH+UDP_CHECKSUM_OFFSET) = 0;
    *(pkt+IP_HDR_LENGTH+UDP_CHECKSUM_OFFSET+1) = 0;

    /* Create pseudo header */
    hdr.wSource = ((dsumtsps_ip_format *)pkt)->wSource;
    hdr.wDest   = ((dsumtsps_ip_format *)pkt)->wDest;
    hdr.hwProt[0]  = 0;
    hdr.hwProt[1]  = 17;  /* IP Protcol field set to 17 - UDP */
    hdr.hwLen[0]   = (unsigned char)((lpkt - IP_HDR_LENGTH) >> 8);
    hdr.hwLen[1]   = (unsigned char)((lpkt - IP_HDR_LENGTH) & 0xFF);

    /* Create a packet to compute checksum */
    memscpy(tmp_pkt,DS_LOOPBACK_HDLR_TMP_PKT_SIZE,&hdr,sizeof(hdr));
    memscpy(tmp_pkt+sizeof(hdr),(DS_LOOPBACK_HDLR_TMP_PKT_SIZE-sizeof(hdr)),
		   pkt + IP_HDR_LENGTH,lpkt - IP_HDR_LENGTH);

    /* Compute the checksum */
    csum = dsumtsps_loopback_compute_checksum(tmp_pkt, lpkt - IP_HDR_LENGTH + sizeof(hdr), 0);

    /* write csum */
    *(pkt+IP_HDR_LENGTH+UDP_CHECKSUM_OFFSET) = (unsigned char)(csum >> 8);
    *(pkt+IP_HDR_LENGTH+UDP_CHECKSUM_OFFSET+1) = (unsigned char)csum;
    
    return csum;
}

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_COMPUTE_ICMP6_CHECKSUM

DESCRIPTION
  Computes UDP packet checksum

PARAMETERS
  pkt - pointer to ICMP packet
  lpkt - length of the packet

DEPENDENCIES
  None.

RETURN VALUE
  int

SIDE EFFECTS
  None.

===========================================================================*/
unsigned short int
dsumtsps_loopback_compute_icmp6_checksum(unsigned char * pkt, long lpkt)
{
    unsigned short int csum;
    pseudo_header_type ph;
    dsm_item_type *dsm_item_ptr;
    

    if (!pkt || (lpkt < IPV6_HDR_LENGTH + ICMP_CHECKSUM_OFFSET + 2)) {
      DS_3GPP_MSG0_HIGH("dsumtsps_compute_icmp6_checksum sanity check fail");
      return 0;
    }

    /* Create pseudo header */
    memscpy(ph.source.ps_s6_addr, sizeof(ph.source), 
            ((ip6_base_hdr_type*)pkt)->src_addr.ps_s6_addr, sizeof(ph.source));
    memscpy(ph.dest.ps_s6_addr, sizeof(ph.dest),
            ((ip6_base_hdr_type*)pkt)->dst_addr.ps_s6_addr, sizeof(ph.dest));
    /* Filling in Payload length for checksum */
    ph.length = ps_htons(((ip6_base_hdr_type*)pkt)->payload_len);
    DS_3GPP_MSG3_ERROR("Payload Length %d", ph.length,0,0);
    ph.protocol = (uint8)PS_IPPROTO_ICMP6;

    dsm_item_ptr = dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL);
    if(dsm_item_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Ran out of DSM buffers, cannot compute icmpv6 checksum");
      return 0;
    }
    (void)dsm_pushdown_tail(&dsm_item_ptr, pkt+IPV6_HDR_LENGTH, (uint16)lpkt - IPV6_HDR_LENGTH, DSM_DS_SMALL_ITEM_POOL);

    csum = cksum(&ph, dsm_item_ptr, (uint16)lpkt - IPV6_HDR_LENGTH, 0);

    dsm_free_packet(&dsm_item_ptr);

    return csum;
}

unsigned short int dsumtsps_loopback_compute_v6udp_checksum(unsigned char * pkt, long lpkt)
{
  unsigned short int csum;
  pseudo_header_type ph;
  dsm_item_type *dsm_item_ptr;
 
  if (!pkt || (lpkt < IPV6_HDR_LENGTH + UDP_CHECKSUM_OFFSET + 2)) 
  {
    DS_3GPP_MSG0_HIGH("dsumtsps_compute_icmp6_checksum sanity check fail");
    return 0;
  }

  /* Create pseudo header */
  memscpy(ph.source.ps_s6_addr, sizeof(ph.source), 
            ((ip6_base_hdr_type*)pkt)->src_addr.ps_s6_addr, sizeof(ph.source));
  memscpy(ph.dest.ps_s6_addr, sizeof(ph.dest),
            ((ip6_base_hdr_type*)pkt)->dst_addr.ps_s6_addr, sizeof(ph.dest));
  ph.length = ps_htons(((ip6_base_hdr_type*)pkt)->payload_len);
  DS_3GPP_MSG3_ERROR("Payload Length %d", ph.length,0,0);
  ph.protocol = (uint8)PS_IPPROTO_UDP;

  dsm_item_ptr = dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL);
  if(dsm_item_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Ran out of DSM buffers, cannot compute icmpv6 checksum",0,0,0);
    return 0;
  }
  (void)dsm_pushdown_tail(&dsm_item_ptr, pkt+IPV6_HDR_LENGTH, (uint16)lpkt - IPV6_HDR_LENGTH, DSM_DS_SMALL_ITEM_POOL);

  csum = cksum(&ph, dsm_item_ptr, (uint16)lpkt - IPV6_HDR_LENGTH, 0);

  dsm_free_packet(&dsm_item_ptr);

  return csum;
}


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_PROCESS_V6_PACKET

DESCRIPTION
  This function processes V6 packet

PARAMETERS
  data_ptr - pointer to the packet in a byte array
  data_len - length of the packet

DEPENDENCIES
  None.

RETURN VALUE
  int

SIDE EFFECTS
  None.

===========================================================================*/
void dsumtsps_loopback_process_v6_packet
(
  byte*   data_ptr,
  uint16* data_len
)
{
  uint64    router_iid = 0x090A0B0C0D0E0F00LL; //corresponds to 000F:0E0D:0C0B:0A09
  dsumtsps_icmp6_ro_ad_format* ro_ad_ptr;
  dsumtsps_icmp6_prefix_info_format* prefix_info_ptr;
  uint16 new_chksum = 0;
  struct ps_in6_addr src_addr;
  struct ps_in6_addr dest_addr;                    
  /*------------------------------------------------------------------------------*/

  /* if this is a ICMPv6 msg */
  if (((ip6_base_hdr_type*)data_ptr)->next_hdr == 58)
  {
    /* if it is a ro sol msg, compose a rol ad msg in return */
    if (((dsumtsps_icmp6_gen_format*)(data_ptr + sizeof(ip6_base_hdr_type)))->type 
                                                                      == 133)
    {
      DS_3GPP_MSG0_HIGH("RoSol msg received, composing Ro Ad.");

      ((ip6_base_hdr_type*)data_ptr)->src_addr.ps_s6_addr64[1] = router_iid;

      /*-------------------------------------------------------------------
       Router Advertisement:
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |     Type      |     Code      |          Checksum             |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       | Cur Hop Limit |M|O|  Reserved |       Router Lifetime         |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                         Reachable Time                        |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                          Retrans Timer                        |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      total: 16 bytes
      --------------------------------------------------------------------*/

      ((ip6_base_hdr_type*)data_ptr)->payload_len = ps_htons(48);  // payload length is 16+32 octets
      ro_ad_ptr = (dsumtsps_icmp6_ro_ad_format*)(data_ptr + sizeof(ip6_base_hdr_type));
      ro_ad_ptr->type = 134;
      ro_ad_ptr->code = 0;
      ro_ad_ptr->curHopLimit = 0;
      ro_ad_ptr->MOFlag = 0;
      ro_ad_ptr->roLifetime = 0xFFFF;
      ro_ad_ptr->reachableTime = 0;
      ro_ad_ptr->retransTimer = 0;
      *data_len += 8;   // RS is 8 bytes
      
    /*-------------------------------------------------------------------
       Prefix Information option:
 
         0                   1                   2                   3
         0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |     Type      |    Length     | Prefix Length |L|A| Reserved1 |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |                         Valid Lifetime                        |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |                       Preferred Lifetime                      |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |                           Reserved2                           |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |                                                               |
        +                                                               +
        |                                                               |
        +                            Prefix                             +
        |                                                               |
        +                                                               +
        |                                                               |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        total: 32 bytes
    --------------------------------------------------------------------*/
      prefix_info_ptr = (dsumtsps_icmp6_prefix_info_format*)
          (data_ptr + sizeof(ip6_base_hdr_type) + sizeof(dsumtsps_icmp6_ro_ad_format));
      prefix_info_ptr->type = 3;  // prefix type value
      prefix_info_ptr->length = 4;
      prefix_info_ptr->prefixLen = 64;
      prefix_info_ptr->LAFlag = ICMP6_RA_AUTO_ADDR_CFG_FLAG_MASK;
      prefix_info_ptr->validLifetime = 0xFFFFFFFF;
      prefix_info_ptr->prefLifetime = 0xFFFFFFFF;
      prefix_info_ptr->reserved = 0;
      prefix_info_ptr->prefixAddr.ps_s6_addr64[0] = 0x1122334455667788LL;
      prefix_info_ptr->prefixAddr.ps_s6_addr64[1] = 0x0LL;
      *data_len += 32;

      // set the checksum to 0
      ro_ad_ptr->checkSum = 0;

      new_chksum = dsumtsps_loopback_compute_icmp6_checksum(data_ptr, *data_len);
      ro_ad_ptr->checkSum = ps_htons(new_chksum);
    }
    else
    {
      /* Type is Echo request; change to echo reply */
      if (((dsumtsps_icmp6_gen_format*)(data_ptr + sizeof(ip6_base_hdr_type)))->type 
                                                                     == 128)
      {
        ((dsumtsps_icmp6_gen_format*)(data_ptr + sizeof(ip6_base_hdr_type)))->type = 129;

        memscpy(&src_addr,
                 sizeof(struct ps_in6_addr),
                 &(((ip6_base_hdr_type*)data_ptr)->dst_addr),
                 sizeof(struct ps_in6_addr) );
        memscpy(&dest_addr,
                 sizeof(struct ps_in6_addr),
                 &(((ip6_base_hdr_type*)data_ptr)->src_addr),
                 sizeof(struct ps_in6_addr) );
        memscpy(&(((ip6_base_hdr_type*)data_ptr)->dst_addr),
                 sizeof(struct ps_in6_addr),
                 &dest_addr,
                 sizeof(struct ps_in6_addr) );
        memscpy(&(((ip6_base_hdr_type*)data_ptr)->src_addr),
                 sizeof(struct ps_in6_addr),
                 &src_addr,
                 sizeof(struct ps_in6_addr) );

        ((dsumtsps_icmp6_gen_format*)(data_ptr + sizeof(ip6_base_hdr_type)))->checkSum = 0;

         new_chksum = dsumtsps_loopback_compute_icmp6_checksum(data_ptr, *data_len);
         ((dsumtsps_icmp6_gen_format*)(data_ptr + sizeof(ip6_base_hdr_type)))->checkSum = ps_htons(new_chksum);

       }
     }
  }
  else
  {
    /* Protocol is Udp */
    if(((ip6_base_hdr_type*)data_ptr)->next_hdr == 17 )
    {
      memscpy(&src_addr,
              sizeof(struct ps_in6_addr),
              &(((ip6_base_hdr_type*)data_ptr)->dst_addr),
              sizeof(struct ps_in6_addr) );
      memscpy(&dest_addr,
              sizeof(struct ps_in6_addr),
              &(((ip6_base_hdr_type*)data_ptr)->src_addr),
              sizeof(struct ps_in6_addr) );
      memscpy(&(((ip6_base_hdr_type*)data_ptr)->dst_addr),
              sizeof(struct ps_in6_addr),
              &dest_addr,
              sizeof(struct ps_in6_addr) );
      memscpy(&(((ip6_base_hdr_type*)data_ptr)->src_addr),
              sizeof(struct ps_in6_addr),
              &src_addr,
              sizeof(struct ps_in6_addr) );
      ((dsumtsps_udp6_gen_format*)(data_ptr + sizeof(ip6_base_hdr_type)))->checksum = 0;
      new_chksum = dsumtsps_loopback_compute_v6udp_checksum(data_ptr, *data_len);
      ((dsumtsps_udp6_gen_format*)(data_ptr + sizeof(ip6_base_hdr_type)))->checksum= ps_htons(new_chksum);

    }
  }

}
/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_PS_LOW_WATERMARK_CB

DESCRIPTION
  This function is called when the Loopback to PS watermark reaches the low
  watermark. 
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : Call instance registered with the watermark callback
   
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  
===========================================================================*/
void dsumtsps_loopback_ps_low_watermark_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
)
{
  DS_3GPP_MSG0_HIGH("Low watermark hit");
  ds_bearer_cntxt_ps_to_L2_flow_enable_cb(wm,callback_data);
}


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_PS_HIGH_WATERMARK_CB

DESCRIPTION
  This function is called when the Loopback to PS watermark reaches the low
  watermark. 
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : Call instance registered with the watermark callback
   
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  
===========================================================================*/
void dsumtsps_loopback_ps_high_watermark_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
)
{
  DS_3GPP_MSG0_HIGH("High watermark hit");
  ds_bearer_cntxt_ps_to_L2_flow_disable_cb(wm,callback_data);
}

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_CONVERT_PACKET

DESCRIPTION
  This function converts ICMP ping request packets to ping responces, and
  swaps addresses in UDP iperf traffic
  In process, it will verify checksum (first on the packet before conversion,
  then on the packet after coversion)
  
PARAMETERS
   item_ptr - pointer to the DSM item with the packet
   pkt_len  - will be filled in with the length of resulting pakcet 
              (can be passed in as NULL if not needed)
   
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE  if conversion was successful
  FALSE if packet was corrupted (note, FALSE only indicates checksum failure.
  packet may still be converted even if FALSE was returned)
  
SIDE EFFECTS
  Caller must check item_ptr for NULL after calling this function
  
===========================================================================*/
boolean dsumtsps_loopback_convert_packet
(
  dsm_item_type     **item_ptr,
  uint16            * pkt_len
)
{
  uint32    wSource;
  uint32    wDest;
  uint16    orig_chksum = 0, new_chksum = 0, recvd_chksum = 0;
  uint16    data_len = 0;
  boolean   bRet = TRUE;
  byte 	   *data_ptr = NULL;

 /*--------------------------------------------------------------------------
    Get Allocated memory for data_ptr, If data_ptr is NULL return FAILURE
  --------------------------------------------------------------------------*/
  data_ptr = ds_loopback_hdlr_get_dyn_byte();
  if (data_ptr == NULL)
  {
     DS_3GPP_MSG0_ERROR("dsumtsps_loopback_convert_packet memory allocation failed");  
     bRet = FALSE;
     return bRet;
  }
  // If UDP checksum is disabled, extract header only
  if (dsumtsps_loopback_udp_chksum_verif == FALSE) {
    data_len = sizeof(dsumtsps_ip_format);
  } else {
    data_len = DS_LOOPBACK_HDLR_DATA_PTR_SIZE ;
  }
  
  /*-------------------------------------------------------------------------
    Extract packet out of the DSM item
  -------------------------------------------------------------------------*/
  (void)memset(data_ptr,0,DS_LOOPBACK_HDLR_DATA_PTR_SIZE );
  data_len = dsm_pullup(item_ptr,data_ptr,data_len);

  // Extract the rest of the packet if this is a ICMP packet since checksum
  // must be recomputed, also need to extract the entire packet if it is v6
  if ( (dsumtsps_loopback_udp_chksum_verif == FALSE) &&
       ((((dsumtsps_ip_format*)data_ptr)->bProt == 1) ||
       ((((byte) *data_ptr) >> 4) == 6))
     ) 
  {
    data_len = dsm_pullup( item_ptr, ((byte*)data_ptr + data_len), 
                           DS_LOOPBACK_HDLR_DATA_PTR_SIZE - data_len );
    data_len += sizeof(dsumtsps_ip_format);
  }    

  /*-------------------------------------------------------------------------
    Modify the packet and put it back into a DSM item to loop it back:
     - Swap source and Dest addresses
     - If this is an ICMP pkt, assume that it's a ping request and change 
       it to ping response 
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("ip tx loopback um data cb ");    
  /*-------------------------------------------------------------------------
    V4 packet processing
  -------------------------------------------------------------------------*/
  if ((((byte)*data_ptr) >> 4) == 4)
  {
  if ( (((dsumtsps_ip_format*)data_ptr)->bProt == 17) &&
       (dsumtsps_loopback_udp_chksum_verif == TRUE) ) {

    recvd_chksum = (*(data_ptr+IP_HDR_LENGTH+UDP_CHECKSUM_OFFSET)) << 8;
    recvd_chksum += *(data_ptr+IP_HDR_LENGTH+UDP_CHECKSUM_OFFSET + 1);    
    orig_chksum = dsumtsps_loopback_compute_udp_checksum(data_ptr, (long)data_len);    
    
    if (orig_chksum != recvd_chksum)
    {      
      DS_3GPP_MSG0_HIGH("UDP checksum verification failed!!pkt corrupted");
      bRet = FALSE;
    }
  }

  if (((dsumtsps_ip_format*)data_ptr)->bProt == 1) {    
    recvd_chksum = ((dsumtsps_icmp_format*)(data_ptr + sizeof(dsumtsps_ip_format)))->hwCheckSum;
    if(ps_ntohs(recvd_chksum )!= dsumtsps_loopback_compute_icmp_checksum(data_ptr, (long)data_len))
    {
      DS_3GPP_MSG0_HIGH("ICMP checksum verification failed!!pkt corrupted");
      bRet = FALSE;
    }
  } 
  
  wSource = ((dsumtsps_ip_format*)data_ptr)->wSource;
  wDest = ((dsumtsps_ip_format*)data_ptr)->wDest;
  ((dsumtsps_ip_format*)data_ptr)->wSource = wDest;
  ((dsumtsps_ip_format*)data_ptr)->wDest = wSource;

  /* Compute new checksum */
  if (((dsumtsps_ip_format*)data_ptr)->bProt == 1) {
    DS_3GPP_MSG0_LOW("ICMP pkt recvd, looping back as ping rsp");
    ((dsumtsps_icmp_format*)(data_ptr + sizeof(dsumtsps_ip_format)))->bType = 0x00;
    (void)dsumtsps_loopback_compute_icmp_checksum(data_ptr, (long)data_len);
  } else if ( (((dsumtsps_ip_format*)data_ptr)->bProt == 17) &&
              (dsumtsps_loopback_udp_chksum_verif == TRUE) ) {
    new_chksum = dsumtsps_loopback_compute_udp_checksum(data_ptr, (long)data_len);
  }

  /* Compare checksums */
  if ((((dsumtsps_ip_format*)data_ptr)->bProt == 17) &&
      (orig_chksum != new_chksum) &&
      (dsumtsps_loopback_udp_chksum_verif == TRUE) ) {
    DS_3GPP_MSG3_HIGH("UDP checksum verification failed!! orig=%d new=%d",orig_chksum,
             new_chksum,0);
    bRet = FALSE;
  }

  if (pkt_len != NULL) {
    *pkt_len = ((dsumtsps_ip_format*)data_ptr)->hwTotalLen;
  }
  }
  else if ((((byte) *data_ptr) >> 4) == 6)
  {
    dsumtsps_loopback_process_v6_packet(data_ptr, &data_len);

    if (pkt_len != NULL) 
    {
      // we are not counting v6 packet length right now
      *pkt_len = 0;
    }
  }
  /* Pushdown the modified packet */
  (void)dsm_pushdown ( item_ptr,
                        data_ptr,
                        data_len,
                        DSM_DS_LARGE_ITEM_POOL );

  return bRet;
} /* dsumtsps_loopback_convert_packet */


#endif /* ..LOOPBACK or ..LOOPBACK_PDCPSIM */

#ifdef FEATURE_DATA_TEST_LOOPBACK

/*===========================================================================
FUNCTION DSEPSBEARERCNTXT_LOOPBACK_PDP_IP_TX_UM_DATA_CB

DESCRIPTION
  This function modifies and loops ping and iperf pkts received back to
  RX WM
  This function is registered with ps_iface for a PDP-IP call 
  
PARAMETERS  
  *iface_ptr       - Interface ptr for the call.
  **item_ptr       - Payload
  *meta_info_ptr   - Not used.
  *pkt_instance    - Call instance.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0 - Success
  
SIDE EFFECTS 
  None.

===========================================================================*/
void dsumtsps_loopback_pdp_ip_tx_um_data_cb
(
  ps_phys_link_type *phys_link_ptr,
  dsm_item_type     **item_ptr,
  ps_meta_info_type *meta_info_ptr,
  void              *tx_info_ptr
  /*lint -esym(715,pkt_instance) */
  /*lint -esym(818,pkt_instance) */
)
{
  ds_bearer_context_s     *bearer_context_p;  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bearer_context_p = (ds_bearer_context_s*)tx_info_ptr;

  if ((bearer_context_p == NULL) ||
    (!ds_bearer_cntx_validate_bearer_context(bearer_context_p)))
  {
    return;
  }

  (void)dsumtsps_loopback_convert_packet(item_ptr, NULL);
  /*-------------------------------------------------------------------------
    Enqueue the packet on RX watermark and set a signal to PS to pick it up
    or send it to a registered handler
  -------------------------------------------------------------------------*/
  if (*item_ptr == NULL)
  {
    DS_3GPP_MSG0_HIGH("NULL DSM item from dsm_pushdown");
    /*lint -e506 -e774*/
    PS_META_INFO_FREE(&meta_info_ptr);
    /*lint +e506 +e774*/
    return;
  }
  
  if (dsumtsps_loopback_data_hdrl_ptr != NULL) {
    dsumtsps_loopback_data_hdrl_ptr(item_ptr, dsumtsps_loopback_a2_port);
  } else {
    dsm_enqueue(&(bearer_context_p->rx_wm.wm_item),item_ptr);
  }

  /*lint -e506 -e774*/
  PS_META_INFO_FREE(&meta_info_ptr);
  /*lint +e506 +e774*/

  return;
} /* dsumtsps_loopback_pdp_ip_tx_um_data_cb() */

/*===========================================================================
FUNCTION DSPDNCNTX_LOOPBACK_IFACEUPCMDHDLR

DESCRIPTION
Called in DS TASK context in response to an iface up command
  on the UMTS iface.
  Brings up the iface requested after allocating a PDN context and a bearer
  context.  
  
PARAMETERS
  iface_p       - Pointer to the iface that is to be brought up.
  info_ptr      - Pointer to data passed in from the app.

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

  
===========================================================================*/
void dsumtsps_loopback_iface_up_cmd_hdlr
(
    ps_iface_type *iface_p,
    void          *info_ptr
)
{   
    ds_3gpp_iface_s                        *ds3gpp_iface_p=NULL;
    ds_bearer_context_s*                   bearer_context_p=NULL;
    ds_pdn_context_s                       *pdn_cntx_p=NULL;
    ps_ip_addr_type                        ip_addr;
    int                                    ret_val;
    ps_phys_link_link_protocol_handle_type protocol_handle;
    sys_modem_as_id_e_type                 subs_id = SYS_MODEM_AS_ID_NONE;
    ps_iface_down_mh_event_info_type       down_info;
    /*----------------------------------------------------------------------*/

    ASSERT (iface_p != NULL);  
    
    memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));
    
    /*-------------------------------------------------------------------------
     Get the DS Iface pointer from the PS Iface. Once this is done, get the
     PDN context pointer.
     -------------------------------------------------------------------------*/
    ds3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);

    pdn_cntx_p = (ds_pdn_context_s*)(ds3gpp_iface_p->client_data_ptr);
    
    if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id)== FALSE)
    {
      return;
    }

    down_info.down_reason = PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES;
    down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
    down_info.bearer_tech.rat_value =
      ds_3gpp_map_sys_mode_to_ds_sys_rat
        (ds3gpp_get_current_network_mode(subs_id));
    down_info.bearer_tech.so_mask = ds_3gpp_bearer_cntxt_get_so_mask(subs_id);

    /*-------------------------------------------------------------------------
     Check to see if the default bearer is already available. In attach cases,
     the default bearer is allocated when we get the bearer activate indication.
     -------------------------------------------------------------------------*/
    if(pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr == NULL)
    {
        /*----------------------------------------------------------------------
          Allocate a bearer context from the free pool of bearers. If no bearer 
          is available, we should fail the call.
          --------------------------------------------------------------------*/
        bearer_context_p = ds_bearer_cntxt_allocate_new_bearer(subs_id);

        if ((bearer_context_p == NULL) ||
            (!ds_bearer_cntx_validate_bearer_context(bearer_context_p)))
        {
          //Do Local clean up - unbind the DS ifaces and PDN context
         ps_iface_down_ind(iface_p, &down_info);;
          return;
        }

        if(ds_pdn_cntxt_store_bearer_context(pdn_cntx_p,(void *)bearer_context_p) != \
                                                    DS3G_SUCCESS)
        {
        /*---------------------------------------------------------------------
          We should not hit this case of running out of placeholders for bearer
          during iface bring up. If this were to happen, it is probably because
          the bearer pointers were not cleaned up from a prior use of the PDN
          context.
         ---------------------------------------------------------------------*/
          //Post iface down with reason.
          DS_3GPP_MSG0_HIGH("Could not store the allocated bearer");
          //Deallocate bearer
          return;
        } 

        ds_pdn_cntxt_set_default_bearer(pdn_cntx_p, bearer_context_p);

        /*---------------------------------------------------------------------
        Register this bearer with the PDN context. Set is_default as true since
        this is the default bearer for that PDN context.
        ----------------------------------------------------------------------*/
        ds_bearer_cntxt_register_pdn_context(pdn_cntx_p, bearer_context_p, TRUE);

        /*---------------------------------------------------------------------
        Bind the phys_link to flow
        ---------------------------------------------------------------------*/
        ret_val =ps_flow_bind_phys_link(iface_p->iface_private.flow.default_flow_ptr,
                          &(bearer_context_p->phys_link));
        if(ret_val != 0)
        {
            DS_3GPP_MSG1_ERROR("binding flow to phys link failed:error=%d",ret_val);
            ASSERT(0);
            return;
            /* Need to free the bearer and recover */
        }
    }
    else
    {
      if (ds_bearer_cntx_validate_bearer_context(pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr)) 
      {
        bearer_context_p = pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
      }
      else
      {
         return;
      }
    }
    
    /*-----------------------------------------------------------------------
     Change the state to COMING_UP for MO Calls.
     For MT calls, the state would be set to DS_PDN_CONTEXT_STATE_ANSWER 
     ------------------------------------------------------------------------*/
    if(pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_DOWN)
    {
        pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_COMING_UP;
    }

    DS_3GPP_MSG3_HIGH("iface up cmd hdlr PDN cntx Iface:0x%x :info:%lx",\
            iface_p,(uint32) info_ptr,0);

    /*----------------------------------------------------------------------
     Assume primary IP call and hardcode the address
     -----------------------------------------------------------------------*/ 
    ip_addr.type = IPV4_ADDR;

    if (dsumtsps_loopback_ip_addr.is_set) {
       ip_addr.addr.v4.ps_s_addr = dsumtsps_loopback_ip_addr.ip_addr;
    } 
    else 
    {
       ip_addr.addr.v4.ps_s_addr = 0xAB30A8C0; // 192.168.48.171
    }

    if(ps_iface_set_addr(iface_p, &ip_addr)!=0)
    {
       DS_3GPP_MSG1_HIGH("LPBK: Setting address failed for iface_ptr: %d",iface_p);
    }

    dsm_empty_queue(&bearer_context_p->tx_wm.wm_item);

    bearer_context_p->rx_wm.wm_item.total_rcvd_cnt = 0;
    bearer_context_p->rx_wm.wm_item.lowater_func_ptr = 
                            (wm_cb_type) dsumtsps_loopback_ps_low_watermark_cb;
    bearer_context_p->rx_wm.wm_item.lowater_func_data = 
                            bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr;

    bearer_context_p->rx_wm.wm_item.hiwater_func_ptr = 
                            (wm_cb_type) dsumtsps_loopback_ps_high_watermark_cb;

    bearer_context_p->rx_wm.wm_item.hiwater_func_data = 
                            bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr;

    /*------------------------------------------------------------------------
     Set RX watermarks for loopbacks hdlr
     -------------------------------------------------------------------------*/
// Loopback specific Water mark for Non LTE Loopback RAT
    dsm_set_low_wm(&bearer_context_p->rx_wm.wm_item, DS_LOOPBACK_HDLR_UL_WM_LO);
    dsm_set_hi_wm (&bearer_context_p->rx_wm.wm_item, DS_LOOPBACK_HDLR_UL_WM_HI);
    dsm_set_dne(&bearer_context_p->rx_wm.wm_item, DS_LOOPBACK_HDLR_UL_WM_DNE);

    bearer_context_p->rx_wm.wm_item.non_empty_func_ptr = 
                                    (wm_cb_type)dsumtsps_loopback_hdlr_non_empty_data_cb;
    bearer_context_p->rx_wm.wm_item.non_empty_func_data
	                                       = (void*) bearer_context_p;
    /*-------------------------------------------------------------------------
     Set the Tx function 
     -------------------------------------------------------------------------*/
    ps_phys_link_set_tx_function(&(bearer_context_p->phys_link),
                               dsumtsps_loopback_pdp_ip_tx_um_data_cb,
                               (void*)bearer_context_p);

    /*---------------------------------------------------------------------------
     * We are filling the protocol handle structure with the iface pointers
    ---------------------------------------------------------------------------*/

    protocol_handle.none_handle.handle.ip_proto_handle.v4_iface_ptr = \
          &(((ds_pdn_context_s*)(bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr))->\
            ds_pdn_context_dyn_p->\
                  ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]->ps_iface);

    protocol_handle.none_handle.high_protocol =
                                      PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IP;

    /*---------------------------------------------------------------------------
      Set the link layer protocol to NONE and pass the protocol handle into the
      function that will do this. The protocol handle has just been populated
      with the higher layer protocol and the pointers to the ifaces that might
      communicate over this physlink.
    ---------------------------------------------------------------------------*/
    (void) ps_phys_link_set_link_protocol(&(bearer_context_p->phys_link),
                                          PS_PHYS_LINK_LINK_PROTOCOL_NONE,
                                          protocol_handle);

    /*-------------------------------------------------------------------------
     Set the handler for the downlink signal for the call.
     -------------------------------------------------------------------------*/
    (void)ps_set_sig_handler(
                  bearer_context_p->rx_sig,
                  dsumtsps_loopback_hdlr_cntxt_pdp_ip_rx_um_data_cb, 
                  (void *)bearer_context_p);
    /*-------------------------------------------------------------------------  
     Enable it.
     -------------------------------------------------------------------------*/
    ps_enable_sig(bearer_context_p->rx_sig);

    /*----------------------------------------------------------------------
     Send Physical link UP indicaton.
     -----------------------------------------------------------------------*/
    ps_flow_activate_ind(PS_IFACE_GET_DEFAULT_FLOW(iface_p),
                        PS_EIC_NOT_SPECIFIED);

    ps_phys_link_up_ind(&(bearer_context_p->phys_link));

    /*----------------------------------------------------------------------
     Connected call is Primary. Enable flow on the primary phys link
     -----------------------------------------------------------------------*/
    ps_phys_link_enable_flow(&(bearer_context_p->phys_link),
                                    DS_FLOW_LTE_PDCP_MASK|DS_FLOW_UMTS_MASK);
    ps_iface_up_ind(iface_p);  
}

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TEST_PC_REGISTER_DATA_HDLR

DESCRIPTION
  API to register a callback which will be called every time a packet is 
  recieved on tx path. Packet will be modified prior to delivering it to 
  the callback 
  
PARAMETERS  
    pfn_hdlr - pointer to the handler to register
    a2_port  - currently ignored
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.

===========================================================================*/
void dsumtsps_loopback_test_pc_register_data_hdlr 
( 
  dsumtsps_loopback_data_hdlr pfn_hdlr,
  uint32                      a2_port
) 
{
  dsumtsps_loopback_data_hdrl_ptr = pfn_hdlr;
  dsumtsps_loopback_a2_port      = a2_port;
}  


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TEST_PC_SET_IP_ADDR

DESCRIPTION
  This funciton allows to set the IP address of the loopback iface 
  
PARAMETERS  
  ip_addr  - address of the loopback adapter
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.

SIDE EFFECTS 
  None.

===========================================================================*/
void dsumtsps_loopback_test_pc_set_ip_addr
(
  uint32  ip_addr
)
{
  dsumtsps_loopback_ip_addr.ip_addr = ip_addr;
  dsumtsps_loopback_ip_addr.is_set = TRUE;
}


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TEST_PC_ACTIVATE

DESCRIPTION
  Activates looback mechanism
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None. 
  
SIDE EFFECTS 
  None.

===========================================================================*/
void dsumtsps_loopback_test_pc_activate (void) {
  dsumtsps_loopback_active = TRUE;
}


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TEST_PC_IS_ACTIVE

DESCRIPTION
  Checks if looback mechanism is active
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE - active
  FALSE - inactive
  
SIDE EFFECTS 
  None.

===========================================================================*/
boolean dsumtsps_loopback_test_pc_is_active (void) {
  return dsumtsps_loopback_active;
}


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TEST_PC_DEACTIVATE

DESCRIPTION
  De-activates loopback mechanism
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.

===========================================================================*/
void dsumtsps_loopback_test_pc_deactivate(void) {
  dsumtsps_loopback_active = FALSE;
}

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_SET_UDP_CHECK_VERIF

DESCRIPTION
  De-activates loopback mechanism
  
PARAMETERS
  bEnable - TRUE to enable checksum verification
            FALSE to disable checksum verification  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.

===========================================================================*/
void dsumtsps_loopback_set_udp_checksum_verif(boolean bEnable) {
  dsumtsps_loopback_udp_chksum_verif = bEnable;
}


#endif 

