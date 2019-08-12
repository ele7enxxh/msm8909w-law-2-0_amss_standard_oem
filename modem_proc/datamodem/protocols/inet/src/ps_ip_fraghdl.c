/*===========================================================================

                        P S _ I P _ F R A G H D L . C

DESCRIPTION
  IP fragment handling routines to support IP filtering on fragments and
  IP reassembly.

Copyright (c) 2004-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_ip_fraghdl.c#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/15   cx      Fix the memory leak when free fragment handle 
12/23/14   ss      Add limit on maximum entries in frag queue
11/22/13   fn      Remove excessive logging for fragment processing
11/11/13   rk      Removed feature mobileap.
07/08/13   cx      Replace memcpy and memmove with memscpy and memsmove
02/25/13   op      Added function to set/get iface from fraghdl
02/20/13   ss      Reducing logging.
12/14/12   ssb     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR
05/23/12   sb      Fixed KW Error
04/19/12   vpk     Fix the LRU algorithm for fragment handlers
12/21/11   ss      IP hdr dsm offset retrieved from fraghdl for each frag.
10/18/11   ss      Reducing highly recurring F3.
05/30/10   ss      Fixed Metainfo leak/fragment leak in missing frag scenario
03/07/11   pp      ip_fraghdl_free: is_local flag must be set to FALSE.
01/20/11   mct     IPSec bug fixes.
06/08/10   pp      ip6_fraghdli_extract_xport_proto_info: Fixed pkt_offset
                   to extract transport layer info from the packet.
06/02/10   pp      Do one time q_init on Frag-Qs.
04/06/10   am      Making sure queues are zeroed out before being init again.
08/16/09   pp      Re-organized IP frag code.
07/27/09   sp      Fixed IP Fragmentation issues.
10/13/08   pp      Metainfo fixes.
09/12/08   pp      Metainfo optimizations.
03/16/07   scb     Fixed High LINT errors
08/17/06   mct     Added function to init meta info in fraghdls for sending
                   icmp errors.
08/14/06   mct     Should not data abort if meta info is not properly set.
04/19/06   rt      Modified IP v6 Header names.
08/12/05   kr      Added support for stats update
08/15/05   vp      Fixed bug in ip6_fraghdli_extract_xport_proto_info
05/25/05   vp      Fixed bug in processing of routing header in
                   frgahdli_extract_proto_info().
05/24/05   vp      Added xport_proto_info to the fraghdl_s_type. It got
                   removed during the lint changes.
05/11/05   sv      Lint changes.
05/10/05   vp      Fix to use meta_info_ptr->pkt_info_ptr->if_ptr for ICMP.
04/20/05   sv      Modified user data to meta info pointer.
03/25/05   ifk     Added support to read v6 transport proto information from
                   fragment.
03/21/05   ssh     Changes pursuant to the new typedef ps_timer_handle_type
03/18/05   vp      Fix such that we dont send ICMP errors for fragmented ICMP
                   errors.
03/09/05   ssh     Merged vp's changes to get_initial_cont_bytes()
03/02/05   vp      Support for ICMP(v6) error geenration.
02/02/05   ifk     Changed allocation of reassembly structures to LRU.
01/10/05   ifk     Added user_data_valid field, get_initial_cont_bytes()
                   function.  Modified get_user_data, renamed
                   get_cont_start_bytes() to get_initial_cont_byte_count()
10/14/04   ifk     Added module
===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"

#include "dsm.h"
#include "ps_crit_sect.h"
#include "ps_ip.h"
#include "ps_iputil.h"
#include "ps_ip4_hdr.h"
#include "ps_ip_fraghdl.h"
#include "ps_icmp.h"
#include "ps_icmp_error.h"
#include "pstimer.h"
#include "ps_tx_meta_info.h"
#include "ps_metai_info.h"
#include "ps_stat_ipv4.h"
#include "ps_iface.h"
#ifdef FEATURE_DATA_RM_NET
#include "ps_ifacei.h"
#endif /* FEATURE_DATA_RM_NET */

#include "ps_ifacei_utils.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_stat_ipv6.h"
#include "ps_ip6i_hdr.h"
#endif /* FEATURE_DATA_PS_IPV6 */
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

                            DEFINITIONS AND TYPES

===========================================================================*/
/*---------------------------------------------------------------------------
  ip_fraghdl_s_type is used to cache fragments belonging to a v4 or v6
  datagram
---------------------------------------------------------------------------*/
typedef struct ip_fraghdl_s_type
{
  ip_version_enum_type  ip_vsn;  /* IP version of the datagram */
  union
  {
    struct ip    v4;
#ifdef FEATURE_DATA_PS_IPV6
    ip6_hdr_type v6;
#endif /* FEATURE_DATA_PS_IPV6 */
  } ip_hdr;                      /* IPv4 header or IPv6 fragment header    */
  void    *meta_info_ptr;        /* Meta info ptr                          */
  ip_fraghdl_meta_info_kind_enum_type
           meta_info_kind;        /* TX / RX which ever is set             */
  boolean  meta_info_valid;      /* Metainfo validity flag               */
  q_type   frag_queue;           /* Queue of fragments                     */
  uint16   datagram_len;         /* Total length of complete datagram      */
  uint16   cont_start_bytes;     /* Contiguous bytes from beginning rcvd   */
  uint16   offset;               /* DSM offset to v4 or v6 frag header     */
  ps_timer_handle_type frag_timer;
                                 /* Timer for the fragment queue           */
  boolean  in_use;               /* Is the structure in use?               */
  ps_ip_protocol_enum_type
           xport_proto;          /* V6 transport layer protocol            */
  boolean xport_proto_info_valid;/* Is transport layer info valid?         */
#ifdef FEATURE_DATA_PS_IPV6
#ifdef FEATURE_SEC_IPSEC
  char xport_proto_info[12];      /* Transport layer info needed by filter  */
#else
  char xport_proto_info[6];       /* Transport layer info needed by filter  */
#endif /* FEATURE_SEC_IPSEC */
#endif /* FEATURE_DATA_PS_IPV6 */
  void *iface_ptr;               /* Iface ptr, using which ICMP error messages
                                    will be sent in V6 embedded calls */
  void                             *device_ptr; /* Iface or PPP Device info */
  ip_fraghdl_bridge_type_enum_type  device_type; /* Type of device_ptr */
  ps_ip_fraghdl_tx_cmd_f_ptr_type   frag_tx_cmd_f_ptr; /* Transmit function */
  boolean  is_local;              /* Bool to determine if the frag is meant
                     embedded stack */
  uint32 last_rcvd_frag_num;      /* Last received fragment number */
} ip_fraghdl_s_type;


static uint32 rcvd_fragments_num = 0;

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Array of IP fraghdl structures
---------------------------------------------------------------------------*/
static ip_fraghdl_s_type ip_fraghdl_array[NUM_FRAGHDL];


/*===========================================================================

                    PRIVATE FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IP_FRAGHDL_CLEAN_FRAG_Q()

DESCRIPTION
  Function called when timer associated with a fraghdl entry expires or
  Frag handle is reclaimed during LRU. This dequeues fragments pending in
  FragQ and sent it to bridged device if set. For Embedded ICMP error
  code is sent to network.

  time_out = TRUE  [Frag handle getting reclaimed by timer callback]
           = FALSE [Frag handle getting reclaimed by LRU]
                   {see ip_fraghdl_timer_cb & ip_fraghdl_alloc}
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

NOTE
  On Error cases - fraghdl is not reclaimed. The caller should reclaim
  the handle.
===========================================================================*/
static void ip_fraghdl_clean_frag_q
(
  ip_fraghdl_s_type *fraghdl_ptr,
  boolean            time_out
)
{
  ps_iface_type         * iface_ptr = NULL;
  dsm_item_type         * item_ptr;
  ps_iface_type         * bridge_iface_ptr = NULL;
  ps_tx_meta_info_type  * tmp_meta_info_ptr = NULL;
  ps_tx_meta_info_type  * frag_meta_info_ptr = NULL;
#ifdef FEATURE_DATA_RM_NET
  errno_enum_type         ps_errno;
#endif /* FEATURE_DATA_RM_NET */
  ip_pkt_info_type        pkt_info;
  uint8                   type;
  ip_pkt_info_type      * meta_pkt_info_ptr;
  boolean                 do_free = FALSE;

  /* Sanity */
  if( NULL == fraghdl_ptr )
  {
    return;
  }

  /*----------------------------------------------------------------------
    Laptop call cases
  ----------------------------------------------------------------------*/
  if( NULL != fraghdl_ptr->device_ptr )
  {
    item_ptr = ip_fraghdl_get_fragment(fraghdl_ptr);
    if ( (NULL == item_ptr) || (NULL == fraghdl_ptr->frag_tx_cmd_f_ptr) )
    {
      LOG_MSG_INFO2_0("Frag Q is empty!!");
      return;
    }

    if ( IP_FRAGHDL_BRIDGE_TYPE_IFACE_BRIDGE == fraghdl_ptr->device_type )
    {
      bridge_iface_ptr = (ps_iface_type *)fraghdl_ptr->device_ptr;
      LOG_MSG_INFO1_2("Iface Bridging - Sending all the queued fragments onto "
               "Iface 0x%x:%d", bridge_iface_ptr->name,
               bridge_iface_ptr->instance);

      /*-----------------------------------------------------------------------
        Without metainfo - packets cannot reside in FragQ in a bridge scenario.
        Something wrong happened...return immediately and free frag handle.
      ----------------------------------------------------------------------*/
      if(fraghdl_ptr->meta_info_ptr == NULL)
      {
        LOG_MSG_ERROR_0("Meta_info is NULL!");
        dsm_free_packet(&item_ptr);
        return;
      }

      if (fraghdl_ptr->meta_info_kind == IP_FRAGHDL_RX_META_INFO)
      {
        /*------------------------------------------------------------------
          This case requires an explicit TX Metainfo creation, as we have 
          RX Metainfo ptr in the FragQ.
        -------------------------------------------------------------------*/
        PS_TX_META_INFO_AND_RT_META_INFO_GET(frag_meta_info_ptr);

        /* Check for Memory allocation failures! */
        if ((frag_meta_info_ptr == NULL) ||
           (PS_TX_META_GET_RT_META_INFO_PTR(frag_meta_info_ptr) == NULL))
        {
          LOG_MSG_ERROR_0("Metainfo creation failed..drop the fragment!!");
          dsm_free_packet(&item_ptr);
          if(frag_meta_info_ptr != NULL)
          {
            PS_TX_META_INFO_FREE(&frag_meta_info_ptr);
          }
          return;
        }

        /*--------------------------------------------------------------------
          Copy the packetinfo into newly created TX metainfo
          (Don't generate Pkt info again and Dont make this fragment to get
          Queued again!)
        -------------------------------------------------------------------*/
        PS_TX_META_SET_PKT_INFO(frag_meta_info_ptr,
           (((ps_rx_meta_info_type *)fraghdl_ptr->meta_info_ptr)->pkt_info));
        do_free = TRUE;
      }
      else if (fraghdl_ptr->meta_info_kind == IP_FRAGHDL_TX_META_INFO)
      {
        /* Re-use the TX metainfo exists in Frag handle */
        frag_meta_info_ptr = (ps_tx_meta_info_type *)fraghdl_ptr->meta_info_ptr;
      }
      else
      {
        LOG_MSG_ERROR_0("Meta_info sanity corrupted..drop the packet!");
        dsm_free_packet(&item_ptr);
        return;
      }

      /*----------------------------------------------------------------------
        Set the _pkt_info_valid flag to TRUE to avoid hitting frag logic, and
        avoding queuing this packet
      ----------------------------------------------------------------------*/
      PS_TX_META_GET_PKT_INFO(frag_meta_info_ptr).is_pkt_info_valid = TRUE;

#ifdef FEATURE_DATA_RM_NET
      /*------------------------------------------------------------------
        If bridge interface is broadcast[RmNet case], create the meta_info
        and set the next hop IP addr, IP addr scope in the meta_info,
        which will be used by the lan LLC module
      -------------------------------------------------------------------*/
      if( PS_IFACE_GET_IS_BCAST_IFACE(bridge_iface_ptr) == TRUE )
      {
        /*--------------------------------------------------------------------
          Set the next hop address and the IP address scope for the forwarded
          packet.
        --------------------------------------------------------------------*/
        if (-1 == ps_iface_fill_next_hop_addr(bridge_iface_ptr,
                                              frag_meta_info_ptr,
                                              &ps_errno))
        {
          LOG_MSG_ERROR_1("Unable to get next_hop_addr %d!", ps_errno);
          dsm_free_packet(&item_ptr);
          PS_TX_META_INFO_FREE(&frag_meta_info_ptr);
          return;
        }
      }
#endif /* FEATURE_DATA_RM_NET */
      /*------------------------------------------------------------------
        Iface Bridging - Send out all the queued fragments on the
        bridged Iface
      ------------------------------------------------------------------*/
      do
      {
        PS_TX_META_INFO_DUP(frag_meta_info_ptr,
                            &tmp_meta_info_ptr);
        (void)fraghdl_ptr->frag_tx_cmd_f_ptr(bridge_iface_ptr,
                                             &item_ptr,
                                             tmp_meta_info_ptr);
        item_ptr = ip_fraghdl_get_fragment(fraghdl_ptr);
      } while( NULL != item_ptr );/* End of while() */

      if(do_free == TRUE)
      {
        PS_TX_META_INFO_FREE(&frag_meta_info_ptr);
      }
    } /* IP_FRAGHDL_BRIDGE_TYPE_IFACE_BRIDGE */
    else if( IP_FRAGHDL_BRIDGE_TYPE_PPP_BRIDGE == fraghdl_ptr->device_type )
    {
      LOG_MSG_INFO1_0("PPP Bridging - Sending all the queued fragments onto "
               "PPP Dev");
      /*------------------------------------------------------------------
        PPP Bridging - Send out all the queued fragments on this PPP Dev
      ------------------------------------------------------------------*/
      do
      {
        (void)fraghdl_ptr->frag_tx_cmd_f_ptr(fraghdl_ptr->device_ptr,
                                             &item_ptr,
                                              NULL);
        item_ptr = ip_fraghdl_get_fragment(fraghdl_ptr);
      } while( NULL != item_ptr );/* End of while() */
    } /* IP_FRAGHDL_BRIDGE_TYPE_PPP_BRIDGE */
    else
    {
      /*------------------------------------------------------------------
        No Device setup, drop the packet.
      ------------------------------------------------------------------*/
      LOG_MSG_ERROR_0("No Next Device setup for this frag, drop it!");
      dsm_free_packet(&item_ptr);
      return;
    }
  } /* End of if( NULL != fraghdl_ptr->device ) */
  else
  {
    /*-----------------------------------------------------------------------
      Embedded call case. Send ICMP(v6) Time Exceeded message, if its timeout!
    -----------------------------------------------------------------------*/
    if (fraghdl_ptr->ip_vsn == IP_V4
#ifdef FEATURE_DATA_PS_IPV6
      || fraghdl_ptr->ip_vsn == IP_V6
#endif /* FEATURE_DATA_PS_IPV6 */
       )
    {
      /*-----------------------------------------------------------------------
       Without metainfo packets cannot reside in FragQ [in case of V4].
       Something wrong happened...return immediately and free frag handle.
      ----------------------------------------------------------------------*/
      if(fraghdl_ptr->meta_info_ptr == NULL)
      {
        LOG_MSG_ERROR_0("Meta_info is NULL!");
        return;
      }

      if (fraghdl_ptr->meta_info_kind == IP_FRAGHDL_TX_META_INFO)
      {
        meta_pkt_info_ptr = &PS_TX_META_GET_PKT_INFO
                             ((ps_tx_meta_info_type *)fraghdl_ptr->meta_info_ptr);
      }
      else if (fraghdl_ptr->meta_info_kind == IP_FRAGHDL_RX_META_INFO)
      {
        meta_pkt_info_ptr = &PS_RX_META_GET_PKT_INFO
                             ((ps_rx_meta_info_type *)fraghdl_ptr->meta_info_ptr);
      }
      else
      {
        DATA_ERR_FATAL("Meta_info is invalid but NOT NULL!");
        return;
      }
      iface_ptr = (ps_iface_type *)meta_pkt_info_ptr->if_ptr;

#ifdef FEATURE_DATA_PATH_INFO
      if (NULL != iface_ptr)
      {
        LOG_MSG_INFO1_2("Meta_info's iface 0x%x:%d",iface_ptr->name, iface_ptr->instance);
      }
#endif
    }

    if( iface_ptr == NULL )
    {
      return;
    }

    item_ptr = q_get(&fraghdl_ptr->frag_queue);
    if( item_ptr == NULL )
    {
      return;
    }

    if( fraghdl_ptr->cont_start_bytes != 0 )
    {
      LOG_MSG_INFO1_0("Embedded case: Frag missing:: send ICMP Error");
      memset(&pkt_info, 0, sizeof(ip_pkt_info_type));
      pkt_info.if_ptr = iface_ptr;

      switch( fraghdl_ptr->ip_vsn )
      {
        case IP_V4:
          /*-------------------------------------------------------------------
            Increment IPv4 reasm_timeout stats for timeout.
          -------------------------------------------------------------------*/
          if(time_out)
          {
            IPV4_INC_STATS(reasm_timeout,1) ;
          }
          if(fraghdl_ptr->ip_hdr.v4.protocol == PS_IPPROTO_ICMP)
          {
            (void) dsm_peek_byte(item_ptr,
                                 (uint16)(fraghdl_ptr->offset +
                                          fraghdl_ptr->ip_hdr.v4.ihl *4),
                                 &type
                                 );
            if(type == ICMP_DEST_UNREACH ||
               type == ICMP_QUENCH       ||
               type == ICMP_REDIRECT     ||
               type == ICMP_TIME_EXCEED  ||
               type == ICMP_PARAM_PROB
              )
            {
              /*---------------------------------------------------------------
                Do not send ICMP error message in response to ICMP error.
              ---------------------------------------------------------------*/
              dsm_free_packet(&item_ptr);
              break;
            }
          }
          pkt_info.ip_vsn = IP_V4;
          pkt_info.ip_hdr.v4 = fraghdl_ptr->ip_hdr.v4;
          /*---------------------------------------------------------------
           ICMP TIME EXCEED or ICMP SOURCE QUENCH based on time_out flag
          ---------------------------------------------------------------*/
          if(time_out)
          {
            icmpv4_error(&item_ptr,
                         ICMP_TIME_EXCEED,
                         ICMP_FRAG_EXCEED,
                         &pkt_info);
          }
          else
          {
            icmpv4_error(&item_ptr,
                         ICMP_QUENCH,
                         0,
                         &pkt_info);
          }
          break;

#ifdef FEATURE_DATA_PS_IPV6
        case IP_V6:
          /*---------------------------------------------------------------
            For V6 only timeout is applicable, no source quench!
          ---------------------------------------------------------------*/
          if (time_out)
          {
            /*-------------------------------------------------------------------
              Increment IPv6 reasm_timeout stats :
            -------------------------------------------------------------------*/
            IPV6_INC_STATS(reasm_timeout,1);
            if(fraghdl_ptr->ip_hdr.v6.hdr_body.frag_hdr.next_hdr == PS_IPPROTO_ICMP6)
            {
              (void)dsm_peek_byte( item_ptr,
                                    (uint16)(fraghdl_ptr->offset +
                                    sizeof(ip6_frag_hdr_type)),
                                    &type
                                   );
              if(type < ICMP6_ECHO_REQ)
              {
                /*---------------------------------------------------------------
                  Do not send ICMPv6 error message in response to ICMPv6 error.
                ---------------------------------------------------------------*/
                dsm_free_packet(&item_ptr);
                break;
              }
            }
            pkt_info.ip_vsn = IP_V6;
            pkt_info.ip_hdr.v6.hdr_body.base_hdr =
                                      fraghdl_ptr->ip_hdr.v6.hdr_body.base_hdr;
            icmpv6_error(&item_ptr,
                         ICMP6_TIME_EXCEEDED,
                         ICMP6_FRAG_REASS_EXCEED,
                         &pkt_info);
          } /* time_out */
          else
          {
            LOG_MSG_ERROR_0("Free the IPv6 fragment in LRU frag queue!");
            dsm_free_packet(&item_ptr);
          }
          break;
#endif /* FEATURE_DATA_PS_IPV6 */
        default:
          break;
      } /* End of switch( fraghdl_ptr->ip_vsn ) */
    } /* End of if( fraghdl_ptr->cont_start_bytes != 0 ) */
    else
    {
      /*-------------------------------------------------------------------
        At this moment we dont know where to send the packet. 
        TODO: Is RouteACLs need to be executed here?
      -------------------------------------------------------------------*/
      LOG_MSG_INFO1_0("Out-Of-Order fragment, drop it!");
      dsm_free_packet(&item_ptr);
    }
  }
}/* ip_fraghdl_clean_frag_q */

/*===========================================================================
FUNCTION IP_FRAGHDL_TIMER_CB()

DESCRIPTION
  Function called when timer associated with a fraghdl entry expires.  Frees
  the fraghdl data structure along with any queued fragments.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Frees fraghdl for which the timer expired along with any queued fragments
===========================================================================*/
static void ip_fraghdl_timer_cb
(
  void *arg_ptr
)
{
  ip_fraghdl_s_type  * fraghdl_ptr = (ip_fraghdl_s_type *)arg_ptr;
  /*-------------------------------------------------------------------------
    Call ip_fraghdl_free() on the passed fraghdl argument
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_1("fraghdl timer expired for fraghdl 0x%x", fraghdl_ptr );

  if( fraghdl_ptr == NULL )
  {
    LOG_MSG_ERROR_0("fraghdl_ptr is NULL!");
    return;
  }

  /* Clean Frag Q */
  ip_fraghdl_clean_frag_q(fraghdl_ptr, TRUE);

  /* Free Frag handle */
  ip_fraghdl_free(fraghdl_ptr);
  return;
} /* ip_fraghdl_timer_cb() */


/*===========================================================================
FUNCTION IP_FRAGLIST_USE()

DESCRIPTION
  Function called when a fraghdl is being used. Updates the received fragment
 for the frag handle.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ip_fraglist_use(ip_fraghdl_s_type *fraghdl_ptr)
{
   /*-------------------------------------------------------------------------
     When a fragment handle is used (i.e. a new fragment comes in) then
     increment the received number of fragments and update the last received
     fragment number.

     Being a uint32 there might be a integer overflow but then the fragment
     counts will be resets.

     Fragment handle with the lowest received fragment number is the oldest
     item.
   -------------------------------------------------------------------------*/
   rcvd_fragments_num++;
   fraghdl_ptr->last_rcvd_frag_num = rcvd_fragments_num ;

}

/*===========================================================================
FUNCTION IP_FRAGLIST_GET_OLDEST_ITEM()

DESCRIPTION
  Function returns fraghdl with the max age.

DEPENDENCIES
  None

RETURN VALUE
  fraghdl

SIDE EFFECTS
 None
===========================================================================*/

static ip_fraghdl_s_type* ip_fraglist_get_oldest_item(void)
{
   ip_fraghdl_s_type *oldest_item = &ip_fraghdl_array[0];
   ip_fraghdl_s_type *fraghdl_ptr = NULL;
   int i;
   uint32 oldest_rcvd_frag_num = oldest_item->last_rcvd_frag_num;
   uint32 curr_rcvd_frag_num = 0;

   /*-------------------------------------------------------------------------
     As the zeroth item is used for comparision start with element 1 for
     comparision.
   -------------------------------------------------------------------------*/

   for(i = 1; i < NUM_FRAGHDL ; i++ )
   {
      fraghdl_ptr = &ip_fraghdl_array[i];

      /*-------------------------------------------------------------------------
        This complex computation is requried for greater than comparision
        because of integer overflow. As the variable rcvd_fragments_num is
        increased evertime a fragment comes in and never reset, its possible
        that the integer overflow happens. The following logic ensures that
        comparision to be proper even when overflow happens.
        s1 is said to be greater than s2 if, and only if, s1 is not equal to
        s2, and

        (i1 < i2 and i2 - i1 > 2^(SERIAL_BITS - 1)) or
        (i1 > i2 and i1 - i2 < 2^(SERIAL_BITS - 1))
        From RFC 1982, for our use Serial bits is 32

        -------------------------------------------------------------------------*/
      curr_rcvd_frag_num = fraghdl_ptr->last_rcvd_frag_num;

      if (((oldest_rcvd_frag_num < curr_rcvd_frag_num ) &&
               ((curr_rcvd_frag_num - oldest_rcvd_frag_num )
                > 0x80000000L)) ||
            ((oldest_rcvd_frag_num > curr_rcvd_frag_num ) &&
             (oldest_rcvd_frag_num - curr_rcvd_frag_num <
              0x80000000L )))
      {
         oldest_item = fraghdl_ptr;
         oldest_rcvd_frag_num = oldest_item->last_rcvd_frag_num;
      }
   }

   return oldest_item;
}

/*===========================================================================
FUNCTION IP_FRAGLIST_CLEAR()

DESCRIPTION
  Function clears and frees fragment handle.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void ip_fraglist_clear(ip_fraghdl_s_type *fraghdl_ptr)
{
   /* Clean Frag Q */
   ip_fraghdl_clean_frag_q(fraghdl_ptr, FALSE); /* FALSE indicates LRU */

   /* reclaim the handle */
   ip_fraghdl_free( fraghdl_ptr );
}

/*===========================================================================
FUNCTION IP_FRAGLIST_GETNEW()

DESCRIPTION
  Function called to allocate a new fragment handler.

DEPENDENCIES
  None

RETURN VALUE
  Fragmnet Handle

SIDE EFFECTS
  Frees old fraghdl, if all the fragment handles are in use
===========================================================================*/
static ip_fraghdl_s_type *ip_fraglist_getnew(void)
{
   ip_fraghdl_s_type *fraghdl_ptr = NULL;
   int i = 0;

   /*-------------------------------------------------------------------------
     Loop through the fraghdl structures initializing them and allocating
     timers, initialize the queues etc.
     -------------------------------------------------------------------------*/
   for(i = 0; i < NUM_FRAGHDL ; i++ )
   {
      if (ip_fraghdl_array[i].in_use == FALSE) {
         fraghdl_ptr = &ip_fraghdl_array[i];
         break;
      }
   }

   if (fraghdl_ptr != NULL)
   {
      fraghdl_ptr->meta_info_ptr = NULL;
      fraghdl_ptr->meta_info_kind = IP_FRAGHDL_INVALID_META_INFO;
      fraghdl_ptr->meta_info_valid = FALSE;
      fraghdl_ptr->datagram_len = 0;
      fraghdl_ptr->cont_start_bytes = 0;
      fraghdl_ptr->device_ptr  = NULL;
      fraghdl_ptr->device_type = IP_FRAGHDL_BRIDGE_TYPE_NONE;
      fraghdl_ptr->frag_tx_cmd_f_ptr = NULL;
      fraghdl_ptr->is_local = FALSE;
#ifdef FEATURE_DATA_PS_IPV6
      fraghdl_ptr->iface_ptr = NULL;
#endif /* FEATURE_DATA_PS_IPV6 */
   }
   else
   {
       fraghdl_ptr = ip_fraglist_get_oldest_item();
       /*-------------------------------------------------------------------------
         Oldest item is the least recently used item.
         -------------------------------------------------------------------------*/

       ip_fraglist_clear(fraghdl_ptr);
   }

   fraghdl_ptr->last_rcvd_frag_num = 0;
   fraghdl_ptr->in_use = TRUE;

   return fraghdl_ptr;
}

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION IP6_FRAGHDLI_EXTRACT_XPORT_PROTO_INFO()

DESCRIPTION
  Called for the first IPv6 fragment.  Extracts transport layer information
  and stores into the fraghdl.

DEPENDENCIES
  Argument being non-NULL and handle being in use verified in caller.
  This should be called only on the first v6 fragment for a v6 datagram and
  after the relevant header information in the fraghdl is updated.

RETURN VALUE
  None

SIDE EFFECTS
  Copies transport layer information into the fraghdl.
===========================================================================*/
static void ip6_fraghdli_extract_xport_proto_info
(
  ip_fraghdl_s_type *fraghdl_ptr,
  dsm_item_type     *item_ptr
)
{
  ps_ip_protocol_enum_type next_hdr =
   (ps_ip_protocol_enum_type)fraghdl_ptr->ip_hdr.v6.hdr_body.frag_hdr.next_hdr;
  uint8  extract_buf[2];
  uint16 pkt_offset = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  fraghdl_ptr->xport_proto_info_valid = FALSE;

  /*-------------------------------------------------------------------------
    Initialize the pkt_offset with end of Frag offset in the packet.
  -------------------------------------------------------------------------*/
  pkt_offset = fraghdl_ptr->offset + sizeof(ip6_frag_hdr_type); 

  /*-------------------------------------------------------------------------
    Process all the extension headers until an upper layer header or the
    header NO_NEXT_HEADER is found.
    >>IPv6 nodes should parse all extension headers irrespective of order,
      Hence, this logic.
  -------------------------------------------------------------------------*/
  while (next_hdr !=PS_IPPROTO_TCP &&
         next_hdr !=PS_IPPROTO_UDP &&
         next_hdr !=PS_IPPROTO_ICMP6 &&
         next_hdr !=PS_IPPROTO_ESP &&
         next_hdr !=PS_IPPROTO_AH)
  {
    switch( next_hdr )
    {
      case PS_IPPROTO_ROUTING_HDR:
        if (sizeof(uint16) != dsm_extract(item_ptr,
                                          pkt_offset, 
                                          extract_buf, sizeof(uint16)))
        {
          LOG_MSG_ERROR_0("ip6_frag: unable to parse ROUTING_HDR len");
          return;
        }
        pkt_offset += sizeof(ip6_routing_hdr_type) + (extract_buf[1]<<3);
        next_hdr = (ps_ip_protocol_enum_type)extract_buf[0];
        break;

      case PS_IPPROTO_HOP_BY_HOP_OPT_HDR:
        if (sizeof(uint16) != dsm_extract(item_ptr, 
                                          pkt_offset,
                                          extract_buf, sizeof(uint16)))
        {
          LOG_MSG_ERROR_0("ip6_frag: unable to get HopByHop hdr len");
          return;
        }
        pkt_offset += sizeof(ip6_hopbyhop_hdr_type) + (extract_buf[1]<<3);
        next_hdr = (ps_ip_protocol_enum_type)extract_buf[0];
        break;

      case PS_IPPROTO_FRAG_HDR:
        LOG_MSG_ERROR_0("ip6 frag hdr inside frag hdr, cannot filter");
        return;

      /*--------------------------------------------------------------------
        Below headers are not supported with Frag logic
      --------------------------------------------------------------------*/
      case PS_NO_NEXT_HDR:
      default:
        LOG_MSG_INFO1_1("%d Header Not Supported",next_hdr);
        return;
    }
  }

  /*-------------------------------------------------------------------------
    Extract the transport header information and mark the boolean as valid
  -------------------------------------------------------------------------*/
  if (0 == dsm_extract(item_ptr,
                       pkt_offset,
                       fraghdl_ptr->xport_proto_info,
                       sizeof(fraghdl_ptr->xport_proto_info)))
  {
    LOG_MSG_ERROR_0("unable to get transport layer info for IP filtering");
    return;
  }

  fraghdl_ptr->xport_proto_info_valid = TRUE;
  fraghdl_ptr->xport_proto = next_hdr;

  return;
} /* ip6_fraghdli_extract_xport_proto_info() */
#endif /* FEATURE_DATA_PS_IPV6 */


/*===========================================================================
FUNCTION IP4_FRAGHDLI_LOOKUP()

DESCRIPTION
  This function returns a handle to the IP fragment handling structure
  corresponding to passed fragment header if already allocated or NULL
  otherwise.

DEPENDENCIES
  Argument being non-NULL and handle being in use verified in
  ip_fraghdl_lookup()

RETURN VALUE
  Handle to allocated IP fragment handling structure corresponding to the
  passed fragment header.

SIDE EFFECTS
  None
===========================================================================*/
static void *ip4_fraghdli_lookup
(
  struct ip *v4hdr
)
{
  int                i;
  void              *ret = NULL;
  ip_fraghdl_s_type *fraghdl_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Loop through the fraghdl array and match passed header's id, source
    address, destination address and protocol fields with respective fields
    of the entries in the array.
  -------------------------------------------------------------------------*/
  for(i = 0; i < NUM_FRAGHDL ; i++ )
  {
    /*-----------------------------------------------------------------------
      Get the i'th ip frghdl entry.  Continue to next entry if either the
      i'th entry isn't in use or the IP version doesn't match
    -----------------------------------------------------------------------*/
    fraghdl_ptr = &ip_fraghdl_array[i];
    if( FALSE == fraghdl_ptr->in_use || fraghdl_ptr->ip_vsn != IP_V4 )
    {
      continue;
    }

    /*-----------------------------------------------------------------------
      Perform a match based on the identification, protocol, source and
      destination addresses
    -----------------------------------------------------------------------*/
    if(v4hdr->id == fraghdl_ptr->ip_hdr.v4.id &&
       v4hdr->source.ps_s_addr == fraghdl_ptr->ip_hdr.v4.source.ps_s_addr &&
       v4hdr->dest.ps_s_addr == fraghdl_ptr->ip_hdr.v4.dest.ps_s_addr &&
       v4hdr->protocol == fraghdl_ptr->ip_hdr.v4.protocol )
    {
      ret = (void *)fraghdl_ptr;
      break;
    }
  }

  return ret;
} /* ip4_fraghdli_lookup() */


#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION IP6_FRAGHDLI_LOOKUP()

DESCRIPTION
  This function returns a handle to the IP fragment handling structure
  corresponding to passed v6 fragment header if already allocated or NULL
  otherwise.

DEPENDENCIES
  Argument being non-NULL and handle being in use verified in
  ip_fraghdl_lookup()

RETURN VALUE
  Handle to allocated IP fragment handling structure corresponding to the
  passed v6 header.

SIDE EFFECTS
  None
===========================================================================*/
static void *ip6_fraghdli_lookup
(
  ip6_hdr_type *v6hdr
)
{
  int                i;
  void              *ret = NULL;
  ip_fraghdl_s_type *fraghdl_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Loop through the fraghdl array and match passed header's id, source
    address and destination address fields with respective fields of the
    entries in the array.
  -------------------------------------------------------------------------*/
  for(i = 0; i < NUM_FRAGHDL ; i++ )
  {
    /*-----------------------------------------------------------------------
      Get the i'th ip frghdl entry.  Continue to next entry if either the
      i'th entry isn't in use or the IP version isn't v6
    -----------------------------------------------------------------------*/
    fraghdl_ptr = &ip_fraghdl_array[i];
    if( FALSE == fraghdl_ptr->in_use || fraghdl_ptr->ip_vsn != IP_V6 )
    {
      continue;
    }

    /*-----------------------------------------------------------------------
      Perform a match based on the identification, protocol source and
      destination addresses
    -----------------------------------------------------------------------*/
    if(v6hdr->hdr_body.frag_hdr.id ==
         fraghdl_ptr->ip_hdr.v6.hdr_body.frag_hdr.id &&
       PS_IN6_ARE_ADDR_EQUAL(&v6hdr->hdr_body.base_hdr.src_addr,
                          &fraghdl_ptr->ip_hdr.v6.hdr_body.base_hdr.
                            src_addr) &&
       PS_IN6_ARE_ADDR_EQUAL(&v6hdr->hdr_body.base_hdr.dst_addr,
                          &fraghdl_ptr->ip_hdr.v6.hdr_body.base_hdr.
                            dst_addr) )
    {
      ret = (void *)fraghdl_ptr;
      break;
    }
  }

  return ret;
} /* ip6_fraghdli_lookup() */
#endif /* FEATURE_DATA_PS_IPV6 */


/*===========================================================================
FUNCTION IP4_FRAGHDLI_PUT()

DESCRIPTION
  This function is called to put the passed v4 fragment onto the fragment
  queue corresponding to passed IP fraghdl handle.

DEPENDENCIES
  Arguments being non-NULL and handle being in use verified in
  ip_fraghdl_put()

RETURN VALUE
  None

SIDE EFFECTS
  The passed datagram is placed on the queue associated with passed handle
  in its sorted position according to fragment offset
===========================================================================*/
static void ip4_fraghdli_put
(
  void          *handle,
  dsm_item_type *pkt_ptr,
  struct ip     *v4hdr
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type*)handle;
  dsm_item_type     *curr_frag = NULL;
  struct ip          currhdr;
  uint16             offset;
  uint16             start_offset;
  uint16             end_offset;
  uint16             curr_start_offset;
  uint16             curr_end_offset;
  uint16             new_cont_start_bytes;
  dsm_item_type     *next_frag = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If this is the only fragment in the datagram, free the DSM item and
    return
  -------------------------------------------------------------------------*/
  if( 0 == v4hdr->mf && 0 == v4hdr->off )
  {
    LOG_MSG_ERROR_0( "ip4_fraghdli_put() called on non-fragment");
    dsm_free_packet( &pkt_ptr );
    return;
  }
  /*-------------------------------------------------------------------------
    Calculate offset space occupied by new fragment
  -------------------------------------------------------------------------*/
  start_offset = (uint16)v4hdr->off * 8;
  end_offset = start_offset + ((uint16)v4hdr->length) - ((uint16)(v4hdr->ihl * 4));

  /*-------------------------------------------------------------------------
    Calculate updated value of cont_start_bytes due to new fragment
  -------------------------------------------------------------------------*/
  new_cont_start_bytes = fraghdl_ptr->cont_start_bytes;
  if( start_offset <= new_cont_start_bytes &&
      end_offset > new_cont_start_bytes )
  {
    new_cont_start_bytes = end_offset;
  }

  /*-------------------------------------------------------------------------
    Find the place at which the passed fragment needs to be queued.
  -------------------------------------------------------------------------*/
  curr_frag = q_check(&fraghdl_ptr->frag_queue);

  /*-------------------------------------------------------------------------
    Loop through all fragments.  This loop looks for the fragment coming
    after the new fragment in this datagram's offset space.  This loop
    also updates the value of cont_start_bytes that may have changed due
    to the new fragment
  -------------------------------------------------------------------------*/
  while( NULL != curr_frag )
  {
    offset = fraghdl_ptr->offset;

    /*-----------------------------------------------------------------------
      Parse the fragment
    -----------------------------------------------------------------------*/
    if( FALSE == ps_ip4_hdr_parse(curr_frag, &offset, &currhdr) )
    {
      /*---------------------------------------------------------------------
        A malformed fragment.  This case should not occur.
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_0( "ps_ip4_hdr_parse() returned FALSE" );
      dsm_free_packet( &pkt_ptr );
      return;
    }

    /*-----------------------------------------------------------------------
      Calculate payload covered by current fragment
    -----------------------------------------------------------------------*/
    curr_start_offset = (uint16)currhdr.off * 8;
    curr_end_offset = curr_start_offset + ((uint16)currhdr.length) -
                      ((uint16)(currhdr.ihl * 4));

    /*-----------------------------------------------------------------------
      Go into this if statement only if we haven't discovered the fragment
      coming after the new fragment
    -----------------------------------------------------------------------*/
    if( NULL == next_frag )
    {
      /*---------------------------------------------------------------------
        If current fragment offset is greater then passed fragment then the
        passed fragment should be queued before the current fragment.
      ---------------------------------------------------------------------*/
      if(currhdr.off > v4hdr->off)
      {
        next_frag = curr_frag;
      }
    }

    /*-----------------------------------------------------------------------
      Calculate updated value of cont_start_bytes due to curr fragment
    -----------------------------------------------------------------------*/
    if( curr_start_offset <= new_cont_start_bytes &&
        curr_end_offset > new_cont_start_bytes )
    {
      new_cont_start_bytes = curr_end_offset;
    }

    /*-----------------------------------------------------------------------
      Break out of outer while loop if the fragmet coming after the new
      fragment is found and the value of cont_start_bytes is updated
    -----------------------------------------------------------------------*/
    if( NULL != next_frag && curr_start_offset > new_cont_start_bytes )
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Move to next fragment
    -----------------------------------------------------------------------*/
    curr_frag = q_next(&fraghdl_ptr->frag_queue, &curr_frag->link);
  } /* while */

  /*-------------------------------------------------------------------------
    If passed fragment is the last fragment, compute and set the datagram
    length
  -------------------------------------------------------------------------*/
  if( 0 == v4hdr->mf )
  {
    fraghdl_ptr->datagram_len = (uint16)((v4hdr->off * 8) + v4hdr->length -
                                  (v4hdr->ihl * 4));
  }

  /*-------------------------------------------------------------------------
    If passed fragment is the first fragment, save its header
  -------------------------------------------------------------------------*/
  if( 0 == v4hdr->off )
  {
    fraghdl_ptr->ip_hdr.v4 = *v4hdr;
  }

  /*-------------------------------------------------------------------------
    Update cont_start_bytes
  -------------------------------------------------------------------------*/
  fraghdl_ptr->cont_start_bytes = new_cont_start_bytes;

  /*-------------------------------------------------------------------------
    Queue fragment
  -------------------------------------------------------------------------*/
  if(next_frag == NULL)
  {
    q_put( &fraghdl_ptr->frag_queue, &pkt_ptr->link );
  }
  else
  {
    q_insert(
#ifdef FEATURE_Q_NO_SELF_QPTR
              &fraghdl_ptr->frag_queue,
#endif
              &pkt_ptr->link, &next_frag->link );
  }

  return;
} /* ip4_fraghdli_put() */


#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION IP6_FRAGHDLI_PUT()

DESCRIPTION
  This function is called to put the passed v6 fragment onto the fragment
  queue corresponding to passed IP fraghdl handle.

DEPENDENCIES
  Arguments being non-NULL and handle being in use verified in
  ip_fraghdl_put()

RETURN VALUE
  None

SIDE EFFECTS
  The passed datagram is placed on the queue associated with passed handle
  in its sorted position according to fragment offset
===========================================================================*/
static void ip6_fraghdli_put
(
  void          *handle,
  dsm_item_type *pkt_ptr,
  ip6_hdr_type  *v6hdr
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type*)handle;
  dsm_item_type     *curr_frag = NULL;
  ip6_hdr_type       currhdr;
  uint16             offset;
  uint16             start_offset;
  uint16             end_offset;
  uint16             curr_start_offset;
  uint16             curr_end_offset;
  uint16             new_cont_start_bytes;
  dsm_item_type     *next_frag = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If this is the only fragment in the datagram, free the DSM item and
    return
  -------------------------------------------------------------------------*/
  if( 0 == (v6hdr->hdr_body.frag_hdr.offset_flags & 0x1) &&
      0 == (v6hdr->hdr_body.frag_hdr.offset_flags & ~0x7) )
  {
    LOG_MSG_ERROR_0( "ip6_fraghdli_put() called on non-fragment");
    dsm_free_packet( &pkt_ptr );
    return;
  }

  /*-------------------------------------------------------------------------
    Calculate offset space occupied by new fragment
  -------------------------------------------------------------------------*/
  start_offset = v6hdr->hdr_body.frag_hdr.offset_flags & ~0x7;
  end_offset = start_offset + v6hdr->hdr_body.base_hdr.payload_len -
               fraghdl_ptr->offset + sizeof(ip6_base_hdr_type) -
               sizeof(ip6_frag_hdr_type);

  /*-------------------------------------------------------------------------
    Calculate updated value of cont_start_bytes due to new fragment
  -------------------------------------------------------------------------*/
  new_cont_start_bytes = fraghdl_ptr->cont_start_bytes;
  if( start_offset <= new_cont_start_bytes &&
      end_offset > new_cont_start_bytes )
  {
    new_cont_start_bytes = end_offset;
  }

  /*-------------------------------------------------------------------------
    Find the place at which the passed fragment needs to be queued.
  -------------------------------------------------------------------------*/
  curr_frag = q_check(&fraghdl_ptr->frag_queue);

  /*-------------------------------------------------------------------------
    Loop through all fragments.  This loop looks for the fragment coming
    after the new fragment in this datagram's offset space.  This loop
    also updates the value of cont_start_bytes that may have changed due
    to the new fragment
  -------------------------------------------------------------------------*/
  while( NULL != curr_frag )
  {
    offset = fraghdl_ptr->offset;

    /*-----------------------------------------------------------------------
      Parse the fragment
    -----------------------------------------------------------------------*/
    memset (&currhdr, 0, sizeof(ip6_hdr_type));
    if( FALSE == ps_ip6_frag_hdr_parse(curr_frag, &offset, &currhdr) )
    {
      /*---------------------------------------------------------------------
        A malformed fragment.  This case should not occur.
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_0( "ps_ip6_frag_hdr_parse() returned FALSE" );
      dsm_free_packet( &pkt_ptr );
      return;
    }

    /*-----------------------------------------------------------------------
      Calculate payload covered by current fragment
    -----------------------------------------------------------------------*/
    curr_start_offset = currhdr.hdr_body.frag_hdr.offset_flags & ~0x7;
    curr_end_offset = curr_start_offset + ((uint16)dsm_length_packet(curr_frag)) -
                      offset;

    /*-----------------------------------------------------------------------
      Go into this if statement only if we haven't discovered the fragment
      coming after the new fragment
    -----------------------------------------------------------------------*/
    if( NULL == next_frag )
    {
      /*---------------------------------------------------------------------
        If current fragment offset is greater then passed fragment then the
        passed fragment should be queued before the current fragment.
      ---------------------------------------------------------------------*/
      if((currhdr.hdr_body.frag_hdr.offset_flags & ~0x7) >
         (v6hdr->hdr_body.frag_hdr.offset_flags & ~0x7))
      {
        next_frag = curr_frag;
      }
    }

    /*-----------------------------------------------------------------------
      Calculate updated value of cont_start_bytes due to curr fragment
    -----------------------------------------------------------------------*/
    if( curr_start_offset <= new_cont_start_bytes &&
        curr_end_offset > new_cont_start_bytes )
    {
      new_cont_start_bytes = curr_end_offset;
    }

    /*-----------------------------------------------------------------------
      Break out of outer while loop if the fragmet coming after the new
      fragment is found and the value of cont_start_bytes is updated
    -----------------------------------------------------------------------*/
    if( NULL != next_frag && curr_start_offset > new_cont_start_bytes )
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Move to next fragment
    -----------------------------------------------------------------------*/
    curr_frag = q_next(&fraghdl_ptr->frag_queue, &curr_frag->link);
  } /* while */

  /*-------------------------------------------------------------------------
    If passed fragment is the last fragment, compute and set the datagram
    length
  -------------------------------------------------------------------------*/
  if( 0 == (v6hdr->hdr_body.frag_hdr.offset_flags & 0x1) )
  {
    fraghdl_ptr->datagram_len =
      (v6hdr->hdr_body.frag_hdr.offset_flags & ~0x7) +
      v6hdr->hdr_body.base_hdr.payload_len - fraghdl_ptr->offset +
      sizeof(ip6_base_hdr_type) - sizeof(ip6_frag_hdr_type);
  }

  /*-------------------------------------------------------------------------
    If passed fragment is the first fragment, save its header
  -------------------------------------------------------------------------*/
  if( 0 == (v6hdr->hdr_body.frag_hdr.offset_flags & ~0x7) )
  {
    fraghdl_ptr->ip_hdr.v6.hdr_body.base_hdr = v6hdr->hdr_body.base_hdr;
    fraghdl_ptr->ip_hdr.v6.hdr_body.frag_hdr = v6hdr->hdr_body.frag_hdr;
    ip6_fraghdli_extract_xport_proto_info( fraghdl_ptr, pkt_ptr );
  }

  /*-------------------------------------------------------------------------
    Update cont_start_bytes
  -------------------------------------------------------------------------*/
  fraghdl_ptr->cont_start_bytes = new_cont_start_bytes;

  /*-------------------------------------------------------------------------
    Queue fragment
  -------------------------------------------------------------------------*/
  if(next_frag == NULL)
  {
    q_put( &fraghdl_ptr->frag_queue, &pkt_ptr->link );
  }
  else
  {
    q_insert(
#ifdef FEATURE_Q_NO_SELF_QPTR
              &fraghdl_ptr->frag_queue,
#endif
              &pkt_ptr->link, &next_frag->link );
  }

  return;
} /* ip6_fraghdli_put() */
#endif /* FEATURE_DATA_PS_IPV6 */


/*===========================================================================
FUNCTION IP4_FRAGHDLI_DELETE_FRAGMENT()

DESCRIPTION
  Deletes the passed fragment from the fragment queue, updating any relevant
  statistics.

DEPENDENCIES
  Arguments being non-NULL and handle being in use verified in
  ip_fraghdl_delete()

RETURN VALUE
  None

SIDE EFFECTS
  The passed fragment is removed from the fragment queue.  For overlapping
  fragments cont_start_bytes may be invalid after a delete()
===========================================================================*/
static void ip4_fraghdli_delete_fragment
(
  void          *handle,
  dsm_item_type *item_ptr,
  struct ip     *v4hdr
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type*)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Verify that the version of the IP fraghdl is v4
  -------------------------------------------------------------------------*/
  if( IP_V4 != fraghdl_ptr->ip_vsn )
  {
    DATA_ERR_FATAL("v4 header passed with v6 handle");
    return;
  }

  /*-------------------------------------------------------------------------
    Update the cont_start_bytes field if needed
  -------------------------------------------------------------------------*/
  if( (v4hdr->off*8) <= fraghdl_ptr->cont_start_bytes )
  {
    fraghdl_ptr->cont_start_bytes = (uint16)(v4hdr->off*8);
  }

  /*-------------------------------------------------------------------------
    Delete fragment
  -------------------------------------------------------------------------*/
  q_delete(
#ifdef FEATURE_Q_NO_SELF_QPTR
            &fraghdl_ptr->frag_queue,
#endif /* FEATURE_Q_NO_SELF_QPTR */
            &item_ptr->link );

  return;
} /* ip4_fraghdli_delete_fragment() */


#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION IP6_FRAGHDLI_DELETE_FRAGMENT()

DESCRIPTION
  Deletes the passed fragment from the fragment queue, updating any relevant
  statistics.

DEPENDENCIES
  Arguments being non-NULL and handle being in use verified in
  ip_fraghdl_delete()

RETURN VALUE
  None

SIDE EFFECTS
  The passed fragment is removed from the fragment queue.  For overlapping
  fragments cont_start_bytes may be invalid after a delete()
===========================================================================*/
static void ip6_fraghdli_delete_fragment
(
  void          *handle,
  dsm_item_type *item_ptr,
  ip6_hdr_type  *v6hdr
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type*)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Verify that version of the IP fraghdl is v6
  -------------------------------------------------------------------------*/
  if( IP_V6 != fraghdl_ptr->ip_vsn )
  {
    DATA_ERR_FATAL("v6 header passed with v4 handle");
    return;
  }

  /*-------------------------------------------------------------------------
    Update the cont_start_bytes field if needed
  -------------------------------------------------------------------------*/
  if( (v6hdr->hdr_body.frag_hdr.offset_flags & ~0x7) <=
        fraghdl_ptr->cont_start_bytes )
  {
    fraghdl_ptr->cont_start_bytes =
      v6hdr->hdr_body.frag_hdr.offset_flags & ~0x7;
  }

  /*-------------------------------------------------------------------------
    Delete fragment
  -------------------------------------------------------------------------*/
  q_delete(
#ifdef FEATURE_Q_NO_SELF_QPTR
            &fraghdl_ptr->frag_queue,
#endif /* FEATURE_Q_NO_SELF_QPTR */
            &item_ptr->link );

  return;
} /* ip6_fraghdli_delete_fragment() */
#endif /* FEATURE_DATA_PS_IPV6 */


/*===========================================================================

                    PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IP_FRAGHDL_LOOKUP()

DESCRIPTION
  Returns a handle to the IP fragment handling structure corresponding to
  passed fragment header if already allocated or NULL otherwise.

DEPENDENCIES
  None

RETURN VALUE
  Handle to allocated IP fragment handling structure corresponding to the
  passed fragment.

SIDE EFFECTS
  None
===========================================================================*/
void *ip_fraghdl_lookup
(
  ip_pkt_info_type *pkt_info
)
{
  void *ret = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify argument
  -------------------------------------------------------------------------*/
  if( NULL == pkt_info )
  {
    DATA_ERR_FATAL("NULL pkt_info passed");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Call appropriate handler function
  -------------------------------------------------------------------------*/
  switch( pkt_info->ip_vsn )
  {
    case IP_V4:
      ret = ip4_fraghdli_lookup( &pkt_info->ip_hdr.v4 );
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case IP_V6:
      ret = ip6_fraghdli_lookup( &pkt_info->ip_hdr.v6 );
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      LOG_MSG_ERROR_1( "Invalid protocol version %d passed", 
                      pkt_info->ip_vsn);
      ASSERT( 0 );
      break;
  }

  return ret;
} /* ip_fraghdl_lookup() */


/*===========================================================================
FUNCTION IP_FRAGHDL_ALLOC()

DESCRIPTION
  Allocates an IP fraghdl structure for the passed fragment header.

DEPENDENCIES
  Only one IP header should be passed

RETURN VALUE
  Handle to allocated IP fragment handling structure corresponding to the
  passed fragment.

SIDE EFFECTS
  Allocates a fraghdl structure along with a PS timer which is started
===========================================================================*/
void *ip_fraghdl_alloc
(
  dsm_item_type      *item_ptr,
  uint16              offset,
  ip_pkt_info_type   *pkt_info
)
{
  ip_fraghdl_s_type *fraghdl_ptr = NULL;
  int64              timeout;
  int                mf;
  uint16             start_offset;
  uint16             end_offset;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify passed DSM item chain pointer is non-NULL
  -------------------------------------------------------------------------*/
  if( NULL == item_ptr )
  {
    DATA_ERR_FATAL("NULL dsm pointer passed in ip_fraghdl_lookup()");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Verify pkt_info argument
  -------------------------------------------------------------------------*/
  if( NULL == pkt_info )
  {
    DATA_ERR_FATAL("NULL pkt_info passed");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Find values of mf bit and start and end offset based on protocol version
  -------------------------------------------------------------------------*/
  switch( pkt_info->ip_vsn )
  {
    case IP_V4:
      mf = pkt_info->ip_hdr.v4.mf;
      start_offset = ((uint16)pkt_info->ip_hdr.v4.off * 8);
      end_offset = start_offset + ((uint16)(pkt_info->ip_hdr.v4.length)) -
                   ((uint16)(pkt_info->ip_hdr.v4.ihl*4));
      timeout = IP4_FRAGHDL_TIMEOUT;
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case IP_V6:
      mf = pkt_info->ip_hdr.v6.hdr_body.frag_hdr.offset_flags & 0x1;
      start_offset =
        pkt_info->ip_hdr.v6.hdr_body.frag_hdr.offset_flags & ~0x7;
      end_offset = start_offset +
                     pkt_info->ip_hdr.v6.hdr_body.base_hdr.payload_len -
                     offset + sizeof(ip6_base_hdr_type) -
                     sizeof(ip6_frag_hdr_type);
      timeout = IP6_FRAGHDL_TIMEOUT;
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      LOG_MSG_ERROR_1( "Invalid protocol version %d passed", 
                      pkt_info->ip_vsn);
      ASSERT( 0 );
      return NULL;
  }

  /*-------------------------------------------------------------------------
    Return if this is not a fragment
  -------------------------------------------------------------------------*/
  if( 0 == mf && 0 == start_offset )
  {
    LOG_MSG_ERROR_0( "non-fragment passed to ip_fraghdl_alloc()" );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Find empty fraghdl entry
  -------------------------------------------------------------------------*/
  fraghdl_ptr = ip_fraglist_getnew();

  if( NULL != fraghdl_ptr )
  {

    /*-----------------------------------------------------------------------
      Copy IP headers
    -----------------------------------------------------------------------*/
    switch( pkt_info->ip_vsn )
    {
      case IP_V4:
        fraghdl_ptr->ip_vsn = IP_V4;
        fraghdl_ptr->ip_hdr.v4 = pkt_info->ip_hdr.v4;
        break;

#ifdef FEATURE_DATA_PS_IPV6
      case IP_V6:
        fraghdl_ptr->ip_vsn = IP_V6;
        fraghdl_ptr->ip_hdr.v6.hdr_body.base_hdr =
          pkt_info->ip_hdr.v6.hdr_body.base_hdr;
        fraghdl_ptr->ip_hdr.v6.hdr_body.frag_hdr =
          pkt_info->ip_hdr.v6.hdr_body.frag_hdr;
        /* Set the iface_ptr, used to send out ICMP error messages. */
        fraghdl_ptr->iface_ptr = pkt_info->if_ptr;
        break;
#endif /* FEATURE_DATA_PS_IPV6 */

      default:
        LOG_MSG_ERROR_1( "Invalid protocol version %d passed", 
                        pkt_info->ip_vsn);
        ASSERT( 0 );
        fraghdl_ptr->in_use = FALSE;
        return NULL;
    }

    fraghdl_ptr->offset = offset;

    /*-----------------------------------------------------------------------
      If this is the last fragment set the value of datagram_len
    -----------------------------------------------------------------------*/
    if( 0 == mf )
    {
      fraghdl_ptr->datagram_len = end_offset;
    }

    /*-----------------------------------------------------------------------
      If this is the first fragment set the value of cont_start_bytes
    -----------------------------------------------------------------------*/
    if( 0 == start_offset )
    {
      fraghdl_ptr->cont_start_bytes = end_offset;
#ifdef FEATURE_DATA_PS_IPV6
      if (IP_V6 == pkt_info->ip_vsn)
      {
        ip6_fraghdli_extract_xport_proto_info( fraghdl_ptr, item_ptr );
      }
#endif /* FEATURE_DATA_PS_IPV6 */
    }

    /* Update the counts of fragments */
    ip_fraglist_use(fraghdl_ptr);

    /*-----------------------------------------------------------------------
      No need to do q_init on FragQ again, as we will re-use the mutex 
      allocated at init time.
    -----------------------------------------------------------------------*/
    (void) ps_timer_start(fraghdl_ptr->frag_timer, timeout);
    q_put( &fraghdl_ptr->frag_queue, &item_ptr->link );
  }

  return fraghdl_ptr;
} /* ip_fraghdl_alloc() */


/*===========================================================================
FUNCTION IP_FRAGHDL_PUT()

DESCRIPTION
  This function is called to put the passed fragment onto the fragment
  queue corresponding to passed IP fraghdl handle.

DEPENDENCIES
  None

RETURN VALUE
  0 for success, -1 for error

SIDE EFFECTS
  The passed datagram is placed on the queue associated with passed handle.
===========================================================================*/
int32 ip_fraghdl_put
(
  void             *handle,
  dsm_item_type    *item_ptr,
  ip_pkt_info_type *pkt_info
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type*)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr || NULL == item_ptr || NULL == pkt_info )
  {
    LOG_MSG_ERROR_3( "NULL arg: handle 0x%x, item_ptr 0x%x, pkt_info 0x%x",
         (uint32)handle, (uint32)item_ptr, (uint32)pkt_info );
    ASSERT( 0 );
    dsm_free_packet( &item_ptr );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Verify handle is in use
  -------------------------------------------------------------------------*/
  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle 0x%x not in use", (uint32)handle);
    ASSERT( 0 );
    dsm_free_packet( &item_ptr );
    return -1;
  }

  /*-----------------------------------------------------------------------
    Check if this fragment entry is holding maximum number of allowed 
    fragments. If so, clear this fragment entry.
    We have such check to prevent DSM exhaustion while some rogue peer sends
    a large number of fragments while skipping the first one.
    In such case, we will end up queueing all fragments and hence exhausting
    our DSM items unless below check is there.
  -------------------------------------------------------------------------*/
  if( q_cnt(&fraghdl_ptr->frag_queue) >= IP_FRAG_MAX_QUEUE_CAPACITY )
  {
    LOG_MSG_ERROR_2( "Fraghdl 0x%x queue capacity (%d) reached, "
                     "clearing queued fragments", fraghdl_ptr,
                     IP_FRAG_MAX_QUEUE_CAPACITY );
    ip_fraglist_clear(fraghdl_ptr);
    return -1;
  }

  /* Update the counts of fragments */
  ip_fraglist_use(fraghdl_ptr);
  /*-------------------------------------------------------------------------
    Call appropriate handler function
  -------------------------------------------------------------------------*/
  switch( pkt_info->ip_vsn )
  {
    case IP_V4:
      ip4_fraghdli_put( handle, item_ptr, &pkt_info->ip_hdr.v4 );
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case IP_V6:
      ip6_fraghdli_put( handle, item_ptr, &pkt_info->ip_hdr.v6 );
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      LOG_MSG_ERROR_1( "Invalid protocol version %d passed", 
                      pkt_info->ip_vsn);
      ASSERT( 0 );
      break;
  }

  return 0;
} /* ip_fraghdl_put() */


/*===========================================================================
FUNCTION IP_FRAGHDL_DELETE_FRAGMENT()

DESCRIPTION
  Deletes the passed fragment from the fragment queue, updating any relevant
  statistics.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  The passed fragment is removed from the fragment queue.
===========================================================================*/
void ip_fraghdl_delete_fragment
(
  void             *handle,
  dsm_item_type    *item_ptr,
  ip_pkt_info_type *pkt_info
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type*)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr || NULL == item_ptr || NULL == pkt_info )
  {
    LOG_MSG_ERROR_3( "NULL args: handle 0x%x, item_ptr 0x%x, pkt_info 0x%x",
         (uint32)handle, (uint32)item_ptr, (uint32)pkt_info );
    ASSERT( 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_delete() is not in use",
             (uint32)handle);
    ASSERT( 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  /*-------------------------------------------------------------------------
    Call the relevant delete function based on the protocol
  -------------------------------------------------------------------------*/
  switch( pkt_info->ip_vsn )
  {
    case IP_V4:
      ip4_fraghdli_delete_fragment( handle, item_ptr, &pkt_info->ip_hdr.v4 );
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case IP_V6:
      ip6_fraghdli_delete_fragment( handle, item_ptr, &pkt_info->ip_hdr.v6 );
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      LOG_MSG_ERROR_1( "Invalid protocol version %d passed", 
                      pkt_info->ip_vsn);
      ASSERT( 0 );
      break;
  }

  return;
} /* ip_fraghdl_delete_fragment() */


/*===========================================================================
FUNCTION IP_FRAGHDL_GET_FRAGMENT()

DESCRIPTION
  Dequeues and returns the first fragment in the queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The passed fragment is removed from the fragment queue.  For overlapping
  fragments cont_start_bytes may be invalid after a get()
===========================================================================*/
dsm_item_type *ip_fraghdl_get_fragment
(
  void *handle
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type*)handle;
  dsm_item_type     *pkt_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL handle passed");
    return NULL;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_delete() is not in use",
             (uint32)handle);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Get the first fragment in the queue.  Set cont_start_bytes to 0
  -------------------------------------------------------------------------*/
  pkt_ptr = q_get( &fraghdl_ptr->frag_queue );
  fraghdl_ptr->cont_start_bytes = 0;

  return pkt_ptr;
} /* ip_fraghdl_get_fragment() */


/*===========================================================================
FUNCTION IP_FRAGHDL_GET_FIRST_FRAGMENT()

DESCRIPTION
  This function is called with an IP fraghdl handle and returns the first
  fragment queued in the fraghdl.

  THE FRAGMENT RETURNED SHOULD NOT BE DEQUEUED OR OTHERWISE MOVED IN THE
  QUEUE

DEPENDENCIES
  None

RETURN VALUE
  First fragment queued in the IP fraghdl passed.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *ip_fraghdl_get_first_fragment
(
  void *handle
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL handle passed to ip_fraghdl_get_first_fragment()");
    return NULL;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_get_first_fragment() is "
         "not in use", (uint32)handle);
    ASSERT( 0 );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Return the first fragment in the queue
  -------------------------------------------------------------------------*/
  return q_check( &fraghdl_ptr->frag_queue );
} /* ip_fraghdl_get_first_fragment() */


/*===========================================================================
FUNCTION IP_FRAGHDL_GET_NEXT_FRAGMENT()

DESCRIPTION
  This function returns the fragment after the passed fragment in the
  IP fraghdl structure corresponding to the passed handle

  THE FRAGMENT RETURNED SHOULD NOT BE DEQUEUED OR OTHERWISE MOVED IN THE
  QUEUE

DEPENDENCIES
  None

RETURN VALUE
  Fragment after the passed fragment

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *ip_fraghdl_get_next_fragment
(
  void          *handle,
  dsm_item_type *item_ptr
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL handle passed to ip_fraghdl_set_user_data()");
    return NULL;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_set_user_data() is not in"
         " use", (uint32)handle);
    ASSERT( 0 );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Verify item_ptr
  -------------------------------------------------------------------------*/
  if( NULL == item_ptr )
  {
    DATA_ERR_FATAL("NULL DSM item passed to ip_fraghdl_set_user_data()");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Return fragment after the passed fragment
  -------------------------------------------------------------------------*/
  return q_next( &fraghdl_ptr->frag_queue, &item_ptr->link );
} /* ip_fraghdl_get_next_fragment() */


/*===========================================================================
FUNCTION IP_FRAGHDL_GET_INITIAL_CONT_BYTES()

DESCRIPTION
  This function returns specified number of bytes from the start of the
  fragmented datagram.

DEPENDENCIES
  None

RETURN VALUE
  -1 in case of error
   0 in case of success

SIDE EFFECTS
  None
===========================================================================*/
int ip_fraghdl_get_initial_cont_bytes
(
  void          *handle,
  char          *buffer,
  uint16         buf_size,
  uint16         byte_cnt
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  dsm_item_type     *frag_ptr    = NULL;
  struct ip          iphdr;
  uint16             copy_byte;
  uint16             start_offset;
  uint16             end_offset;
  uint16             dsm_offset;
  uint16             iphdr_offset = 0;
  uint16             offset_copied = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL handle passed to ip_fraghdl_get_initial_cont_bytes()");
    return -1;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_get_initial_cont_bytes() is "
         "not in use", (uint32)handle);
    ASSERT( 0 );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Verify buffer and byte_cnt
  -------------------------------------------------------------------------*/
  if( NULL == buffer || 0 == byte_cnt )
  {
    LOG_MSG_ERROR_2( "Invalid arg: buffer 0x%x, byte_cnt %d",
         (uint32)buffer, byte_cnt);
    ASSERT( 0 );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Verify that the fragment chain has sufficient bytes from the start to
    fulfill request
  -------------------------------------------------------------------------*/
  if( fraghdl_ptr->cont_start_bytes < byte_cnt )
  {
    LOG_MSG_ERROR_2( "Insufficient bytes from start: requested %d, present %d",
         byte_cnt, fraghdl_ptr->cont_start_bytes);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Copy transport layer information from xport_proto_info buffer.  This
    is currently for V6 only.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
  if (IP_V6 == fraghdl_ptr->ip_vsn)
  {
    if (byte_cnt <= sizeof(fraghdl_ptr->xport_proto_info) &&
        TRUE == fraghdl_ptr->xport_proto_info_valid)
    {
      memscpy( buffer, buf_size, fraghdl_ptr->xport_proto_info, byte_cnt);
      return 0;
    }
    else
    {
      return -1;
    }
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  /*-------------------------------------------------------------------------
    The fragment queue is sorted on offset in the fragmented datagram with
    possible duplicate fragments.
    Traverse the queue copying byte_cnt contiguous bytes from the fragmented
    datagram.
  -------------------------------------------------------------------------*/
  frag_ptr = q_check( &fraghdl_ptr->frag_queue );
  while( NULL != frag_ptr && byte_cnt > offset_copied )
  {
    /* Set IP hdr offset value as specified in fraghdl */
    iphdr_offset = fraghdl_ptr->offset;

    switch( fraghdl_ptr->ip_vsn )
    {
    case IP_V4:
      if( FALSE == ps_ip4_hdr_parse(frag_ptr, &iphdr_offset, &iphdr) )
      {
        /*-------------------------------------------------------------------
          A malformed fragment.  This case should not occur.
        -------------------------------------------------------------------*/
        LOG_MSG_ERROR_0( "ps_ip4_hdr_parse() returned FALSE");
        return -1;
      }
      /*---------------------------------------------------------------------
        Calculate the offset space contained by fragment.
      ---------------------------------------------------------------------*/
      start_offset = (uint16)iphdr.off * 8;
      end_offset = start_offset + ((uint16)iphdr.length) - ((uint16)(iphdr.ihl * 4));

      /*---------------------------------------------------------------------
        If offset space of fragment contains bytes not already copied (i.e.
        offset_copied lies inside the offset space) calculate offset in DSM
        item and bytes to copy.  Copy the bytes.
      ---------------------------------------------------------------------*/
      if( offset_copied >= start_offset && offset_copied <= end_offset )
      {
        dsm_offset = ((uint16)(iphdr.ihl * 4)) + offset_copied - start_offset;

        /*-------------------------------------------------------------------
          Copy bytes to the passed buffer
        -------------------------------------------------------------------*/
        copy_byte = MIN((byte_cnt - offset_copied),
                        (end_offset - offset_copied));
        (void) dsm_extract(frag_ptr, dsm_offset, buffer, copy_byte );
        buffer += copy_byte;
        offset_copied += copy_byte;
      }
      break;

    default:
      LOG_MSG_ERROR_2( "invalid IP version %d in fraghdl_ptr %p",
           fraghdl_ptr->ip_vsn, fraghdl_ptr);
      return -1;
    }

    /*-----------------------------------------------------------------------
      Go to next fragment.
    -----------------------------------------------------------------------*/
    frag_ptr = q_next( &fraghdl_ptr->frag_queue, &frag_ptr->link );
  } /* while */

  /*-------------------------------------------------------------------------
    Return success if bytes requested were successfully copied
  -------------------------------------------------------------------------*/
  return (byte_cnt == offset_copied) ? 0 : -1;
} /* ip_fraghdl_get_initial_cont_bytes() */


/*===========================================================================
FUNCTION IP_FRAGHDL_GET_DATAGRAM_LEN()

DESCRIPTION
  Returns the datagram of the datagram being held by the passed IP fraghdl
  structure

DEPENDENCIES
  None

RETURN VALUE
  Datagram length of the datagram associated with the passed IP fraghdl
  0 if datagram length is unknown or an error occurs

SIDE EFFECTS
  None
===========================================================================*/
uint16 ip_fraghdl_get_datagram_len
(
  void *handle
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL handle passed to ip_fraghdl_get_datagram_len()");
    return 0;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_get_datagram_len() is not "
         "in use", (uint32)handle);
    ASSERT( 0 );
    return 0;
  }

  /*-------------------------------------------------------------------------
    Return the value of the datagram length
  -------------------------------------------------------------------------*/
  return fraghdl_ptr->datagram_len;
} /* ip_fraghdl_get_datagram_len() */


/*===========================================================================
FUNCTION IP_FRAGHDL_GET_INITIAL_CONT_BYTES_COUNT()

DESCRIPTION
  Called with a handle to an IP fraghdl structure to return the contiguous
  bytes from the start of packet received so far

DEPENDENCIES
  None

RETURN VALUE
  Contiguous bytes from the start of packet received

SIDE EFFECTS
  None
===========================================================================*/
uint16 ip_fraghdl_get_initial_cont_bytes_count
(
  void *handle
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL handle passed to ip_fraghdl_get_initial_cont_bytes_count()");
    return 0;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_get_initial_cont_bytes_count() "
         "is not in use", (uint32)handle );
    ASSERT( 0 );
    return 0;
  }

  /*-------------------------------------------------------------------------
    Return cont_start_bytes
  -------------------------------------------------------------------------*/
  return fraghdl_ptr->cont_start_bytes;
} /* ip_fraghdl_get_initial_cont_bytes_count() */

/*===========================================================================
FUNCTION IP_FRAGHDL_SET_INIT_META_INFO_CACHE()

DESCRIPTION
  Called with a handle to an IP fraghdl structure and a the meta info to
  be initialized in the fragment handle structure. This is needed in case
  the first bytes of a packet never arrive. Having meta info will allow the
  icmp error messages to be properly sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ip_fraghdl_set_init_meta_info_cache
(
  void                               * handle,
  void                               * meta_info_ptr,
  ip_fraghdl_meta_info_kind_enum_type  meta_info_kind

)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL handle passed to ip_fraghdl_set_user_data()");
    return;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_set_user_data() is not in"
         " use", (uint32)handle);
    ASSERT( 0 );
    return;
  }

  /*-------------------------------------------------------------------------
    Set value of user_data
  -------------------------------------------------------------------------*/
  fraghdl_ptr->meta_info_ptr       = meta_info_ptr;
  fraghdl_ptr->meta_info_kind      = meta_info_kind;

  return;
} /* ip_fraghdl_set_init_meta_info_cache() */

/*===========================================================================
FUNCTION IP_FRAGHDL_SET_RX_META_INFO_CACHE()

DESCRIPTION
  Called with a handle to an IP fraghdl structure and a value of meta_info_ptr
  to be registered with the passed handle

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the passed meta_info_ptr value in the passed handle
===========================================================================*/
void ip_fraghdl_set_rx_meta_info_cache
(
  void *handle,
  ps_rx_meta_info_type *meta_info_ptr
)
{
  ip_fraghdl_s_type    *  fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  ps_rx_meta_info_type **  tmp_rx_meta_info_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL handle passed to ip_fraghdl_set_user_data()");
    return;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_set_user_data() is not in"
         " use", (uint32)handle);
    ASSERT( 0 );
    return;
  }

  /*-------------------------------------------------------------------------
    Meta info was initialized but is not valid. Free the meta info and set
    the valid meta info.
  -------------------------------------------------------------------------*/
  if( fraghdl_ptr->meta_info_ptr != NULL &&
      fraghdl_ptr->meta_info_valid == FALSE )
  {
    tmp_rx_meta_info_ptr =
      (ps_rx_meta_info_type **) &(fraghdl_ptr->meta_info_ptr);
    PS_RX_META_INFO_FREE( tmp_rx_meta_info_ptr);
  }

  fraghdl_ptr->meta_info_ptr       = (void *)meta_info_ptr;
  fraghdl_ptr->meta_info_kind      = IP_FRAGHDL_RX_META_INFO;
  fraghdl_ptr->meta_info_valid     = TRUE;

  return;
} /* ip_fraghdl_set_rx_meta_info_cache() */

/*===========================================================================
FUNCTION IP_FRAGHDL_SET_TX_META_INFO_CACHE()

DESCRIPTION
  Called with a handle to an IP fraghdl structure and a value of meta_info_ptr
  to be registered with the passed handle

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the passed meta_info_ptr value in the passed handle
===========================================================================*/
void ip_fraghdl_set_tx_meta_info_cache
(
  void *handle,
  ps_tx_meta_info_type *meta_info_ptr
)
{
  ip_fraghdl_s_type    *  fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  ps_tx_meta_info_type **  tmp_tx_meta_info_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL handle passed to ip_fraghdl_set_user_data()");
    return;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_set_user_data() is not in"
         " use", (uint32)handle);
    ASSERT( 0 );
    return;
  }

  /*-------------------------------------------------------------------------
    Meta info was initialized but is not valid. Free the meta info and set
    the valid meta info.
  -------------------------------------------------------------------------*/
  if( fraghdl_ptr->meta_info_ptr != NULL &&
      fraghdl_ptr->meta_info_valid == FALSE)
  {
    tmp_tx_meta_info_ptr =
      (ps_tx_meta_info_type **) &(fraghdl_ptr->meta_info_ptr);
    PS_TX_META_INFO_FREE( tmp_tx_meta_info_ptr);
  }

  fraghdl_ptr->meta_info_ptr      = (void *)meta_info_ptr;
  fraghdl_ptr->meta_info_kind     = IP_FRAGHDL_TX_META_INFO;
  fraghdl_ptr->meta_info_valid    = TRUE;

  return;
} /* ip_fraghdl_set_tx_meta_info_cache() */

/*===========================================================================
FUNCTION IP_FRAGHDL_GET_CACHED_RX_META_INFO()

DESCRIPTION
  Called with a handle to an IP fraghdl to get the meta_info registered with
  it.

DEPENDENCIES
  None

RETURN VALUE
  0 if a meta_info_ref_ptr value is set, -1 otherwise
  Value of meta_info_ptr field corresponding to the passed IP fraghdl handle
  returned in meta_info_ref_ptr argument if return value is 0

SIDE EFFECTS
  None
===========================================================================*/
int ip_fraghdl_get_cached_rx_meta_info
(
  void  *handle,
  ps_rx_meta_info_type **meta_info_ref_ptr
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL frag handle!");
    return -1;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x)NOT in use", (uint32)handle );
    ASSERT( 0 );
    return -1;
  }

  if( NULL == meta_info_ref_ptr )
  {
    LOG_MSG_ERROR_0( "NULL meta_info_ref" );
    ASSERT( 0 );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Set value of user_data if valid flag set
  -------------------------------------------------------------------------*/
  if( ( TRUE == fraghdl_ptr->meta_info_valid ) &&
      ( IP_FRAGHDL_RX_META_INFO == fraghdl_ptr->meta_info_kind ) )
  {
    *(meta_info_ref_ptr) = (ps_rx_meta_info_type *)fraghdl_ptr->meta_info_ptr;
    return 0;
  }

  return -1;

} /* ip_fraghdl_get_cached_rx_meta_info() */

/*===========================================================================
FUNCTION IP_FRAGHDL_GET_CACHED_TX_META_INFO()

DESCRIPTION
  Called with a handle to an IP fraghdl to get the meta_info registered with
  it.

DEPENDENCIES
  None

RETURN VALUE
  0 if a meta_info_ref_ptr value is set, -1 otherwise
  Value of meta_info_ptr field corresponding to the passed IP fraghdl handle
  returned in meta_info_ref_ptr argument if return value is 0

SIDE EFFECTS
  None
===========================================================================*/
int ip_fraghdl_get_cached_tx_meta_info
(
  void  *handle,
  ps_tx_meta_info_type **meta_info_ref_ptr
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL handle passed to ip_fraghdl_get_user_data()");
    return -1;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_get_user_data() isn't in"
         " use", (uint32)handle );
    ASSERT( 0 );
    return -1;
  }

  if( NULL == meta_info_ref_ptr )
  {
    DATA_ERR_FATAL("NULL user_data passed to ip_fraghdl_get_user_data()");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Set value of meta_info_ref_ptr if it is valid
  -------------------------------------------------------------------------*/
  if( ( TRUE == fraghdl_ptr->meta_info_valid ) &&
      ( IP_FRAGHDL_TX_META_INFO == fraghdl_ptr->meta_info_kind ) )
  {
    *(meta_info_ref_ptr) = (ps_tx_meta_info_type *)fraghdl_ptr->meta_info_ptr;
    return 0;
  }

  return -1;

} /* ip_fraghdl_get_cached_tx_meta_info() */

/*===========================================================================
FUNCTION IP_FRAGHDL_GET_XPORT_PROTO()

DESCRIPTION
  Called with a handle to an IP fraghdl to get the transport layer protocol

DEPENDENCIES
  None

RETURN VALUE
  Value of xport_proto field corresponding to the passed IP fraghdl handle

SIDE EFFECTS
  None
===========================================================================*/
ps_ip_protocol_enum_type ip_fraghdl_get_xport_proto
(
  void  *handle
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    DATA_ERR_FATAL("NULL handle passed to ip_fraghdl_get_xport_proto()");
    return PS_NO_NEXT_HDR;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_ERROR_1( "handle (0x%x) passed to ip_fraghdl_get_xport_proto() isn't in"
         " use", (uint32)handle);
    ASSERT( 0 );
    return PS_NO_NEXT_HDR;
  }

  return fraghdl_ptr->xport_proto;
} /* ip_fraghdl_get_xport_proto() */


/*===========================================================================
FUNCTION IP_FRAGHDL_FREE()

DESCRIPTION
  Called with a handle to an IP fraghdl struct to free.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Frees the IP fraghdl data structure corresponding to the passed handle
===========================================================================*/
void ip_fraghdl_free
(
  void *handle
)
{
  ip_fraghdl_s_type    *  fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  ps_tx_meta_info_type **  tmp_tx_meta_info_ptr;
  ps_rx_meta_info_type **  tmp_rx_meta_info_ptr;
  dsm_item_type        *  item_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify passed handle is valid and is in use
  -------------------------------------------------------------------------*/
  if( NULL == fraghdl_ptr )
  {
    LOG_MSG_INFO2_0( "NULL handle passed to ip_fraghdl_free()" );
    return;
  }

  if( FALSE == fraghdl_ptr->in_use )
  {
    LOG_MSG_INFO2_0( "handle passed to ip_fraghdl_free() is not in use");
    return;
  }

  /*-------------------------------------------------------------------------
    Remove any fragments currently in the queue
  -------------------------------------------------------------------------*/
  while( NULL != (item_ptr = q_get(&fraghdl_ptr->frag_queue)) )
  {
    dsm_free_packet( &item_ptr );
  }

  /*-------------------------------------------------------------------------
    Cancel the fragment timer
  -------------------------------------------------------------------------*/
  (void)ps_timer_cancel( fraghdl_ptr->frag_timer );

  /*-------------------------------------------------------------------------
    Set in use and user_data_valid fields to FALSE
  -------------------------------------------------------------------------*/
  fraghdl_ptr->in_use = FALSE;
  fraghdl_ptr->datagram_len = 0;
  fraghdl_ptr->cont_start_bytes = 0;
  fraghdl_ptr->xport_proto_info_valid = FALSE;
  fraghdl_ptr->device_ptr  = NULL;
  fraghdl_ptr->device_type = IP_FRAGHDL_BRIDGE_TYPE_NONE;
  fraghdl_ptr->frag_tx_cmd_f_ptr = NULL;
#ifdef FEATURE_DATA_PS_IPV6
  fraghdl_ptr->iface_ptr = NULL;
#endif /* FEATURE_DATA_PS_IPV6 */

  if(fraghdl_ptr->meta_info_ptr != NULL)
  {
    if(fraghdl_ptr->meta_info_kind == IP_FRAGHDL_RX_META_INFO)
    {
      tmp_rx_meta_info_ptr =
        (ps_rx_meta_info_type **) &(fraghdl_ptr->meta_info_ptr);
      PS_RX_META_INFO_FREE(tmp_rx_meta_info_ptr);
    }
    else if(fraghdl_ptr->meta_info_kind == IP_FRAGHDL_TX_META_INFO)
    {
      tmp_tx_meta_info_ptr =
        (ps_tx_meta_info_type **) &(fraghdl_ptr->meta_info_ptr);
      PS_TX_META_INFO_FREE(tmp_tx_meta_info_ptr);
    }
    else
    {
      DATA_ERR_FATAL("Meta info KIND Invalid but Not NULL");
    }
  }
  fraghdl_ptr->meta_info_ptr  = NULL;
  fraghdl_ptr->meta_info_kind = IP_FRAGHDL_INVALID_META_INFO;
  fraghdl_ptr->meta_info_valid = FALSE;
  fraghdl_ptr->is_local = FALSE;

  return;
} /* ip_fraghdl_free() */


/*===========================================================================
FUNCTION IP_FRAGHDL_INIT()

DESCRIPTION
  Initializes the fraghdl layer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes fraghdl structures, allocates PS timers etc.
===========================================================================*/
void ip_fraghdl_init
(
  void
)
{
  ip_fraghdl_s_type *fraghdl_ptr = NULL;
  int                i;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Loop through the fraghdl structures initializing them and allocating
    timers, initialize the queues etc.
  -------------------------------------------------------------------------*/
  for(i = 0; i < NUM_FRAGHDL ; i++ )
  {
    fraghdl_ptr = &ip_fraghdl_array[i];
    memset( fraghdl_ptr, 0, sizeof(ip_fraghdl_s_type) );
    fraghdl_ptr->frag_timer =
      ps_timer_alloc( ip_fraghdl_timer_cb, fraghdl_ptr );

    if( PS_TIMER_INVALID_HANDLE == fraghdl_ptr->frag_timer )
    {
      LOG_MSG_ERROR_0( "Failed to allocate a PS timer");
    }

    /*-------------------------------------------------------------------------
      This being a global queue, a one-time init is sufficient to avoid
      Q mutex [de]alloc multiple times as well as avoding q_destroy API usage.
      Caveat is: We reserve NUM_FRAGHDL mutexes for all FragQs.
    -------------------------------------------------------------------------*/
    (void) q_init(&fraghdl_ptr->frag_queue);
  }

  return;
} /* ip_fraghdl_init() */

/*===========================================================================
FUNCTION PS_IP_FRAGHDL_SET_BRIDGE_DEVICE()

DESCRIPTION
  Sets the passed Bridge info [Iface or PPP dev] in the Fraghdl.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ps_ip_fraghdl_set_bridge_device
(
  void * handle,
  void * bridge_ptr,
  ip_fraghdl_bridge_type_enum_type bridge_type,
  ps_ip_fraghdl_tx_cmd_f_ptr_type  frag_tx_cmd_f_ptr
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (NULL == fraghdl_ptr) || (NULL == bridge_ptr) )
  {
    LOG_MSG_ERROR_0( "NULL handle/device passed to ps_ip_fraghdl_set_bridge_device()");
    return;
  }
  /*-------------------------------------------------------------------------
    Set the passed bridged device Info
  -------------------------------------------------------------------------*/
  LOG_MSG_DATA_PATH_INFO_2("ps_ip_fraghdl_set_bridge_device: Setting bridge device for queue (%x) to iface %x.", 
                           fraghdl_ptr,
                           bridge_ptr);
  fraghdl_ptr->device_type = bridge_type;
  fraghdl_ptr->device_ptr  = bridge_ptr;
  fraghdl_ptr->frag_tx_cmd_f_ptr = frag_tx_cmd_f_ptr;
  return;
} /* ps_ip_fraghdl_set_bridge_device() */

/*===========================================================================
FUNCTION PS_IP_FRAGHDL_SET_LOCAL()

DESCRIPTION
  Sets the is_local flag in the Fraghdl. Thus tags the frag for embedded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ps_ip_fraghdl_set_local
(
  void     *handle
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( NULL == fraghdl_ptr )
  {
    LOG_MSG_ERROR_0( "NULL handle passed");
    return;
  }

  fraghdl_ptr->is_local = TRUE;

} /* ps_ip_fraghdl_set_local */

/*===========================================================================
FUNCTION PS_IP_FRAGHDL_IS_LOCAL()

DESCRIPTION
  Checks the is_local flag in the Fraghdl. Thus tags the frag for embedded.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if the fraghdl is local
  FALSE - otherwise

SIDE EFFECTS
===========================================================================*/
boolean ps_ip_fraghdl_is_local
(
  void * handle
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( NULL == fraghdl_ptr )
  {
    LOG_MSG_ERROR_0( "NULL handle passed");
    return FALSE;
  }

  return (fraghdl_ptr->is_local);

} /* ps_ip_fraghdl_is_local */

/*===========================================================================
FUNCTION PS_IP_FRAGHDL_INVALIDATE_RX_MI_CACHE()

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ps_ip_fraghdl_invalidate_rx_mi_cache
(
  void * handle
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( NULL == fraghdl_ptr )
  {
    LOG_MSG_ERROR_0( "NULL handle passed");
    return;
  }

  fraghdl_ptr->meta_info_valid = FALSE;

} /* ps_ip_fraghdl_invalidate_rx_mi_cache */

/*===========================================================================
FUNCTION PS_IP_FRAGHDL_SET_IFACE()

DESCRIPTION
  Sets the iface in the Fraghdl.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ps_ip_fraghdl_set_iface
(
  void          *handle,
  ps_iface_type *iface_ptr
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( NULL == fraghdl_ptr )
  {
    LOG_MSG_ERROR_0( "NULL handle passed");
    return;
  }

  fraghdl_ptr->iface_ptr = iface_ptr;

} /* ps_ip_fraghdl_set_iface */

/*===========================================================================
FUNCTION PS_IP_FRAGHDL_GET_IFACE()

DESCRIPTION
  Returns the iface in the Fraghdl.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
ps_iface_type* ps_ip_fraghdl_get_iface
(
  void     *handle
)
{
  ip_fraghdl_s_type *fraghdl_ptr = (ip_fraghdl_s_type *)handle;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( NULL == fraghdl_ptr )
  {
    return NULL;
  }

  return fraghdl_ptr->iface_ptr;

} /* ps_ip_fraghdl_get_iface */

