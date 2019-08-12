/*===========================================================================

                        P S _ I P 4 _ F R A G . C

DESCRIPTION
  Contains routines to perform IPv4 fragmentation and reassembly

Copyright (c) 2005-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_ip4_frag.c#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/23/14   ss      Add limit on maximum entries in frag queue
08/08/14   cx      Free the oringinal packet if fragmentation fails in 
                   ip4_fragment_optimized()
04/28/14   cx      Implement ip4_fragment_optimized().
07/08/13   cx      Replace memcpy and memmove with memscpy and memsmove
12/12/12   ds      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR. 
02/24/11   cp      Fixed compiler warnings.
01/07/11   cp      ip4_fragment() API is changed to accept 
                   PATH MTU as input param.
11/14/10   am      Update ip hdr lenght in ip_pkt_info in ip4_fragment().
11/06/09   ba      set the fraghdl in the pkt_info to NULL after reassembling
09/24/09   ss      Fixed KW warnings.
09/30/09   ss      IP hdr init before parsing in ip4_reassemble().
08/21/09   sp      Fix for recalculating IP Header checksum after reassembling
                   the IP fragments.
03/13/08   pp      Metainfo optimizations.
10/24/05   ifk     Removed check for NULL meta info in ip4_fragment()
08/12/05   kr      Added code for updating statistics
05/11/05   sv      Lint changes.
05/04/05   ifk     Fixed bug in reassembling duplicate fragments.
04/20/05   sv      Modified ip4_reassmebly to get fraghdl from pkt info.
01/06/05   msr     Freeing meta info and DSM item in ip4_fragment() in all
                   error cases also.
08/28/04   ifk     Created module.
===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "msg.h"
#include "amssassert.h"

#include "dsm.h"
#include "ps_iface.h"
#include "ps_ip4_hdr.h"
#include "ps_ip4_frag.h"

#include "err.h"
#include "ps_ip.h"
#include "ps_tcp.h"
#include "ps_iputil.h"
#include "ps_ip_fraghdl.h"
#include "ps_stat_ipv4.h"
#include "ds_Utils_DebugMsg.h"

/*---------------------------------------------------------------------------
  Macro for calcuating the header checksum based on RF1642:
  HC' = HC - ~m - m 
  HC : old checksum in header
  HC': new checksum in header
  m  : old value of a 16-bit field
  m' : new value of a 16-bit field    
----------------------------------------------------------------------------*/
#define GET_CARRY_FLAG(value_1, value_2) ((value_1)>=(uint16)(value_2)?0:1)
#define SUB_WITH_CARRY(value_1, value_2) \
        ((value_1) - (value_2) - GET_CARRY_FLAG(value_1,value_2))
#define UPDATE_CHECKSUM(cksum,old_value, new_value)\
  ((cksum) = SUB_WITH_CARRY(SUB_WITH_CARRY((cksum),(~(old_value))),(new_value)))

/*===========================================================================

                    PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IP4_FRAGMENT

DESCRIPTION
  This function is called with an IPv4 datagram, meta info associated with
  the datagram and the PS iface to send the datagram over.  It fragments
  and transmits the datagram.

  NOTE: Meta Info and DSM items are freed in error cases also.

DEPENDENCIES
    None

RETURN VALUE
  -1 in case of error
  0 in case of success
 
SIDE EFFECTS
  The datagram is transmitted as fragments in case of success.
===========================================================================*/
int ip4_fragment
(
  dsm_item_type       **ip_pkt_ptr,
  struct ip            *pkt_hdr,
  ps_iface_type        *rt_if_ptr,
  ps_tx_meta_info_type *meta_info_ptr,
  uint32                path_mtu
)
{
  int                retval = 0;
  struct ip          hdr;
  uint16             i = 0;
  uint16             fragment_offset;
  uint16             fragment_payload;
  dsm_item_type     *fragment_ptr = NULL;
  int                opt_size;
  uint16             copy_bytes;
  ps_tx_meta_info_type *local_meta_info_ptr;
  int                mf_bit;
  uint16             hlen = 0;
  uint16             payload_length;
  ip_pkt_info_type   *pkt_info_ptr=NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( NULL == ip_pkt_ptr || NULL == *ip_pkt_ptr )
  {
    PS_TX_META_INFO_FREE(&meta_info_ptr);
  }

  if( NULL == rt_if_ptr || NULL == pkt_hdr )
  {
    dsm_free_packet(ip_pkt_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
  }
  
  if( NULL == ip_pkt_ptr || NULL == *ip_pkt_ptr || NULL == rt_if_ptr || NULL == pkt_hdr )
  {
    LOG_MSG_ERROR_2("NULL DSM item passed or NULL arg: rt_if_ptr %p, pkt_hdr %p",
                    rt_if_ptr, pkt_hdr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Copy the IP header
  -------------------------------------------------------------------------*/
  memscpy( &hdr, sizeof(hdr), pkt_hdr, sizeof(struct ip) );
  hlen = (uint16)(hdr.ihl * 4);

  /*-------------------------------------------------------------------------
    Pull off header
  -------------------------------------------------------------------------*/
  if( hlen != dsm_pullup( ip_pkt_ptr, NULL, hlen) )
  {
    dsm_free_packet(ip_pkt_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    LOG_MSG_ERROR_0( "error from dsm_pullup while getting IP header" );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Calculate payload length (including offset of original datagram), mf bit
    value set in original datagram, fragment offset and fragment payload.
    Set mf bit in header to 1.
  -------------------------------------------------------------------------*/
  mf_bit           = hdr.mf;
  fragment_offset  = (uint16)hdr.off * 8;
  payload_length   = ((uint16)hdr.length) - hlen + fragment_offset;
  hdr.mf           = 1;
  fragment_payload = ((uint16)path_mtu - hlen) & ~0x7;
  hdr.optlen       = hlen - IPLEN;

  /*-------------------------------------------------------------------------
    Save local copy of meta info
  -------------------------------------------------------------------------*/
  local_meta_info_ptr = meta_info_ptr;
  if (NULL != PS_TX_META_GET_RT_META_INFO_PTR(meta_info_ptr))
  {
    pkt_info_ptr = &PS_TX_META_GET_PKT_INFO(meta_info_ptr);
  }
  else
  {
    LOG_MSG_ERROR_1("Found NULL RT for tx meta info 0x%p", meta_info_ptr);
    dsm_free_packet(ip_pkt_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Form and TX fragments
  -------------------------------------------------------------------------*/
  while( fragment_offset < payload_length )
  {
    /*-----------------------------------------------------------------------
      Calculate payload of this fragment.  Dup the passed DSM item to form
      this fragment
    -----------------------------------------------------------------------*/
    copy_bytes = MIN(fragment_payload, (payload_length - fragment_offset));
    (void) dsm_dup_packet( &fragment_ptr, *ip_pkt_ptr, 0, copy_bytes );
    if( copy_bytes != dsm_pullup( ip_pkt_ptr, NULL, copy_bytes ) )
    {
      retval = -1;
      dsm_free_packet( &fragment_ptr );
      break;
    }

    /*-----------------------------------------------------------------------
      Form the IP header for the fragment by updating the total_length,
      flags and fragment offset fields.
      In setting the MF bit, if original datagram was a fragment then don't
      clear the MF bit even for the last fragment
    -----------------------------------------------------------------------*/
    hdr.length = copy_bytes + hlen;
    if( (fragment_offset + copy_bytes) >= payload_length && mf_bit != 0x1 )
    {
      hdr.mf = 0;
    }
    hdr.offset = fragment_offset;
    hdr.off = (fragment_offset >> 3);

    if( FALSE == ps_ip4_hdr_create( &fragment_ptr, &hdr, FALSE ) )
    {
      retval = -1;
      dsm_free_packet( &fragment_ptr );
      break;
    }

    /* Modify the ip pkt info's ip4 hdr params, 
     * aware we are modifying the dup mem here
     */
    pkt_info_ptr->ip_hdr.v4.length = copy_bytes + hlen;
    pkt_info_ptr->ip_hdr.v4.offset = fragment_offset;
    pkt_info_ptr->ip_hdr.v4.off =  (fragment_offset >> 3);

    /*-----------------------------------------------------------------------
      Allocate a meta_info_ptr for the fragment and copy the original
      datagram's meta_info into it.
    -----------------------------------------------------------------------*/
    PS_TX_META_INFO_DUP( local_meta_info_ptr, &meta_info_ptr );
    if( NULL != local_meta_info_ptr && NULL == meta_info_ptr )
    {
      retval = -1;
      dsm_free_packet( &fragment_ptr );
      break;
    }

    /*-----------------------------------------------------------------------
      TX the fragment.  In case of error, tx_cmd is responsible for freeing
      the fragment and meta info
    -----------------------------------------------------------------------*/
    retval = ps_iface_tx_cmd(rt_if_ptr, &fragment_ptr, meta_info_ptr );
    if( -1 == retval )
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Stats collection: count the number of successfully tx fragments
    -----------------------------------------------------------------------*/
    IPV4_INC_STATS(frag_create,1) ;

    /*-----------------------------------------------------------------------
      Handle options for subsequent fragments by copying from first fragment
    -----------------------------------------------------------------------*/
    if( 0 == fragment_offset )
    {
      /*---------------------------------------------------------------------
        Copy only those options which go into a fragment.
      ---------------------------------------------------------------------*/
      while( i < (hlen - IPLEN) )
      {
        /*-------------------------------------------------------------------
          Handle the one byte options
        -------------------------------------------------------------------*/
        if( hdr.options[i] == IP_EOL )
        {
          if( hlen - IPLEN != i )
          {
            hdr.optlen = i;
            hlen       = i + IPLEN;
            hdr.ihl    = (hlen>>2);
          }
          break;
        }
        if( hdr.options[i] == IP_NOOP )
        {
          i++;
          continue;
        }
        /*-------------------------------------------------------------------
          Handle the type-length-value options.  Copy only if the OPT_COPIED
          flag is set.  Otherwise set the option to NO-OP
        -------------------------------------------------------------------*/
        if( (i+1) >= IP_MAXOPT )
        {
          LOG_MSG_ERROR_0("Header options index exceeds limit.");
          retval = -1;
          break;
        }
        if( OPT_COPIED & hdr.options[i] )
        {
          i += hdr.options[i+1];
        }
        else
        {
          opt_size = hdr.options[i+1];
          for(; i<(i+opt_size) && i<IP_MAXOPT; i++ )
          {
            hdr.options[i] = IP_NOOP;
          }
        }
      } /* while */
    }

    /*-----------------------------------------------------------------------
      Update the fragment offset and the offset into the original DSM item
      by the payload bytes in the fragment just sent
    -----------------------------------------------------------------------*/
    fragment_offset += copy_bytes;
  } /* while */

  if (NULL != *ip_pkt_ptr)
  {
    dsm_free_packet(ip_pkt_ptr);
  }
  PS_TX_META_INFO_FREE( &local_meta_info_ptr );

  return retval;
} /* ip4_fragment() */

/*===========================================================================
FUNCTION IP4_FRAGMENT_OPTIMIZED

DESCRIPTION
  This function is called to fragment one packet into several fragments based 
  on passed in MTU value.All the fragments will be stored in the passed in 
  buffer

DEPENDENCIES
  PS_IP4_HDR_PARSE_OPTIMIZED() should be called to parse the IPv4 header 
  before this function is called

RETURN VALUE
  -1 in case of error
  0 in case of success
 
SIDE EFFECTS
  In good case, the fragments are stored in the queue, the oringinal packet 
  will be freed.
  In error case, all the fragments are freed, oringinal packet will NOT be
  freed.
===========================================================================*/
int ip4_fragment_optimized
(
  dsm_item_type       **ip_pkt_ptr,
  struct ip            *hdr,
  uint32                path_mtu,
  dsm_item_type       **frag_buf, 
  uint16                frag_buf_size
)
{
  int                retval = 0;  
  uint16             frag_num = 0;
  uint16             original_packet_len;
  uint16             original_offset;
  int                original_df;
  int                original_congest;
  int                original_mf;
  uint16             fragment_max_len;
  uint16             offset = 0;
  uint16             fragment_len=0;
  dsm_item_type     *fragment_dsm_ptr = NULL;
  uint16             old_flag_offset;
  uint16             new_flag_offset;
  
  /*------------------------------------------------------------------------
    Parameters Check 
  -------------------------------------------------------------------------*/
  if(NULL == ip_pkt_ptr || 
     NULL == frag_buf || 
     0 == path_mtu)
  {
    LOG_MSG_ERROR_0("ip4_fragment_ex(): Invalid input arguments!");
    return -1;
  }
  
  /*------------------------------------------------------------------------
    Check buffer size is big enough to hold all the fragments
  -------------------------------------------------------------------------*/
  original_packet_len = ntohs(hdr->length);
  original_offset     = (uint16)hdr->off * 8;
  original_df         = hdr->df;
  original_congest    = hdr->congest;
  original_mf         = hdr->mf;
  old_flag_offset     = hdr->flags_offset;
  hdr->mf             = 1;
  fragment_max_len    = (((uint16)path_mtu - IPLEN) & ~0x7) + IPLEN;

  /*------------------------------------------------------------------------
    Check buffer size is big enough to hold all the fragments
  -------------------------------------------------------------------------*/
  if((original_packet_len - IPLEN) > frag_buf_size*((uint16)path_mtu-IPLEN) )
  {
    LOG_MSG_ERROR_1( "ip4_fragment_ex():"
                     "fragment number exceed the buffer size! "
                     "packet ptr: 0x%x",
                      ip_pkt_ptr);
    dsm_free_packet(ip_pkt_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Form fragments and push it into buffer
  -------------------------------------------------------------------------*/
  while( offset < (original_packet_len - IPLEN))
  {  
    /*-----------------------------------------------------------------------
      Calculate payload of this fragment.  Dup the passed DSM item to form
      this fragment
    -----------------------------------------------------------------------*/
    fragment_len = MIN(fragment_max_len, (original_packet_len - offset));
    (void) dsm_dup_packet( &fragment_dsm_ptr,
                           *ip_pkt_ptr, 
                           (offset + IPLEN), 
                           (fragment_len - IPLEN) );    

    /*-----------------------------------------------------------------------
      Flags_offset always changes. So update the flags,offset and checksum
      for each fragments
    -----------------------------------------------------------------------*/
    if(fragment_len + offset >= original_packet_len &&  
       original_mf != 1)
    {
      hdr->mf = 0;
    }   
    hdr->offset     = original_offset + offset;
    hdr->congest    = original_congest;
    hdr->df         = original_df;

    new_flag_offset = (((uint16)hdr->congest) << 15) |
                           (((uint16)hdr->df) << 14) |
                           (((uint16)hdr->mf) << 13) |
                        ((uint16)(hdr->offset >> 3));     
    
    ((uint16*)hdr)[3] = ps_htons(new_flag_offset);
    
    hdr->checksum = ps_htons( hdr->checksum);

    UPDATE_CHECKSUM( hdr->checksum,
                     old_flag_offset,
                     new_flag_offset);

    /*-----------------------------------------------------------------------
      Cache the old flag_offset for next fragment usage
    -----------------------------------------------------------------------*/
    old_flag_offset = new_flag_offset;
     
    /*-----------------------------------------------------------------------
      If packet length changed, update the checksum and length in header
    -----------------------------------------------------------------------*/
    if(hdr->length != ps_htons(fragment_len))
    {  
      UPDATE_CHECKSUM( hdr->checksum,
                       ps_ntohs(hdr->length),
                       fragment_len);
      hdr->length = ps_htons(fragment_len);
    }

    hdr->checksum = ps_htons(hdr->checksum);
  
    /*-----------------------------------------------------------------------
       Push the header into fragment packet
    -----------------------------------------------------------------------*/
    if ((dsm_pushdown_packed(&fragment_dsm_ptr, 
                            ((uint8*)hdr), IPLEN,
                            DSM_DS_SMALL_ITEM_POOL))< IPLEN)
    {
      LOG_MSG_ERROR_0("Error in dsm_pushdown_packed()"
                      " in ip4_frag_optimized()");
      retval = -1;
      break;
    }
    
    /*-----------------------------------------------------------------------
      Store the fragment pointer in buffer
    -----------------------------------------------------------------------*/
    frag_buf[frag_num] = fragment_dsm_ptr;  
    
    /*-----------------------------------------------------------------------
      Increase the number of fragments
    -----------------------------------------------------------------------*/
    frag_num++;
    
    /*-----------------------------------------------------------------------
      Stats collection: count the number of successfully tx fragments
    -----------------------------------------------------------------------*/
    IPV4_INC_STATS(frag_create,1);
    
    /*-----------------------------------------------------------------------
      Update the offset
    -----------------------------------------------------------------------*/
    offset += (fragment_len-IPLEN);
  } /* offset < (original_packet_len -20) */

  /*-------------------------------------------------------------------------
    Free all the fragments in error case;
    Free the oringinal packet in good case;
  -------------------------------------------------------------------------*/
  if( -1 == retval )
  {
    while( NULL != frag_buf[frag_num] &&
           0 != frag_num)
    {
      frag_num--;
      dsm_free_packet( &frag_buf[frag_num] );
      frag_buf[frag_num] = NULL;
    };  
  }
  
  /*-------------------------------------------------------------------------
    Free the oringinal packet
  -------------------------------------------------------------------------*/
  dsm_free_packet(ip_pkt_ptr);
  
  return retval;
}


/*===========================================================================
FUNCTION IP4_REASSEMBLE()

DESCRIPTION
  Function is passed a fragment and associated packet info for the fragment
  along with offset to payload of fragment.  If enough fragments have been
  collected to complete the datagram return value is pointer to DSM item
  containing the datagram with the header returned in the pkt_info argument
  and offset to datagram payload in offset argument.  Otherwise the fragment
  is queued and a NULL returned.

DEPENDENCIES
  None

RETURN VALUE
  If datagram is complete, returns DSM chain containing datagram else
  returns NULL.

SIDE EFFECTS
  Passed fragment may be queued and NULL returned in which case no
  further action should be taken on this fragment by the caller
===========================================================================*/
struct dsm_item_s *ip4_reassemble
(
  ip_pkt_info_type  *pkt_info,/* IP packet information                     */
  struct dsm_item_s *pkt_ptr, /* The fragment itself including IP hdr      */
  uint16            *offset   /* IP pkt offset                             */
)
{
  void          *handle;
  uint16         datagram_len;
  uint16         frag_offset = 0;
  dsm_item_type *reas_ptr = NULL;
  dsm_item_type *curr_ptr;
  struct ip      currhdr;
  uint16         payload_offset = 0;
  uint16         local_offset = 0;
  uint16         curr_offset;
  uint16         curr_len;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( NULL == pkt_ptr || NULL == pkt_info || NULL == offset )
  {
    LOG_MSG_ERROR_3("NULL arg: pkt_ptr 0x%x, pkt_info 0x%x, offset 0x%x",
        (uint32)pkt_ptr, (uint32)pkt_info, (uint32)offset);
    dsm_free_packet(&pkt_ptr);
    ASSERT( 0 );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Verify fraghdl
  -------------------------------------------------------------------------*/
  if (NULL == (handle = pkt_info->fraghdl))
  {
    dsm_free_packet(&pkt_ptr);
    DATA_ERR_FATAL("NULL fraghdl passed");
    return NULL;
  }

  //pkt_info->is_local_frag = TRUE;
  ps_ip_fraghdl_set_local(handle);

  /*-------------------------------------------------------------------------
    Initialization.
  -------------------------------------------------------------------------*/
  memset((void *)&currhdr, 0, sizeof(currhdr));

  /*-------------------------------------------------------------------------
    Parse the current fragment.
  -------------------------------------------------------------------------*/
  if(ps_ip4_hdr_parse(pkt_ptr, offset, &(pkt_info->ip_hdr.v4)) == FALSE)
  {
    LOG_MSG_ERROR_0("Failed to parse IP packet");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Set local offset to offset to v4 header
  -------------------------------------------------------------------------*/
  local_offset = *offset - ((uint16)(pkt_info->ip_hdr.v4.ihl *4));

  if(ip_fraghdl_put( handle, pkt_ptr, pkt_info ) != 0)
  {
    LOG_MSG_ERROR_0("Failed to put frag to queue, freeing packet");
  
    /* Free the packet */
    dsm_free_packet( &pkt_ptr );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Check if all fragments of fragmented datagram are present.  If so
    reassemble
  -------------------------------------------------------------------------*/
  if( ip_fraghdl_get_initial_cont_bytes_count( handle ) ==
      (datagram_len = ip_fraghdl_get_datagram_len( handle )) &&
      0 != datagram_len )
  {
    /*-----------------------------------------------------------------------
      Go through all the fragments until have a complete datagram
    -----------------------------------------------------------------------*/
    while( frag_offset < datagram_len )
    {
      /*---------------------------------------------------------------------
        Get next fragment.  In case the fragment pointer is NULL free the
        datagram under reassembly as we don't have the complete datagram yet
      ---------------------------------------------------------------------*/
      curr_ptr = ip_fraghdl_get_fragment( handle );
      if( NULL == curr_ptr )
      {
        LOG_MSG_ERROR_0("Got NULL fragment unexpectedly while reassembling");
        IPV4_INC_STATS(reasm_fails,1) ;
        dsm_free_packet( &reas_ptr );
        break;
      }

      /*---------------------------------------------------------------------
        Set payload_offset to offset to v4 header for each fragment
      ---------------------------------------------------------------------*/
      payload_offset = local_offset;

      /*---------------------------------------------------------------------
        Parse the fragment header for the current fragment
      ---------------------------------------------------------------------*/
      if( FALSE == ps_ip4_hdr_parse( curr_ptr, &payload_offset, &currhdr ) )
      {
        LOG_MSG_ERROR_0("ps_ip4_hdr_parse() returned FALSE, dropping pkt");
        IPV4_INC_STATS(reasm_fails,1) ;
        dsm_free_packet( &curr_ptr );
        continue;
      }

      /*---------------------------------------------------------------------
        If the fragment offset doesn't match the counter of fragment offset
        free the datagram being reassembled and return
      ---------------------------------------------------------------------*/
      if( (curr_offset = ((uint16)currhdr.off*8)) != frag_offset )
      {
        LOG_MSG_INFO2_2("expecting fragment offset %d, got %d.",
                        frag_offset, (currhdr.off*8));
        curr_len = ((uint16)currhdr.length) - ((uint16)currhdr.ihl*4);
        if ((curr_offset + curr_len) <= frag_offset)
        {
          dsm_free_packet( &curr_ptr );
          continue;
        }
        else if (curr_offset < frag_offset)
        {
          /* Partial overlap with reassembled datagram so far */
          payload_offset += frag_offset - curr_offset;
        }
        else
        {
          /* This point should not have been reached */
          LOG_MSG_ERROR_0( "In reassembly with hole in data");
          IPV4_INC_STATS(reasm_fails,1) ;
          dsm_free_packet( &reas_ptr );
          dsm_free_packet( &curr_ptr );
          break;
        }
      }

      /*---------------------------------------------------------------------
        Remove header from fragment
      ---------------------------------------------------------------------*/
      if(payload_offset != dsm_pullup(&curr_ptr, NULL, payload_offset))
      {
        LOG_MSG_INFO2_1("unable to pull %d bytes from fragment.  Dropping fragment",
                        *offset);
        IPV4_INC_STATS(reasm_fails,1) ;
        dsm_free_packet(&curr_ptr);
        continue;
      }

      /*---------------------------------------------------------------------
        Set reassembly pointer to current fragment for the first fragment
        otherwise append the dsm item to the reassembly pointer.  Update
        fragment offset counter by the length of payload in fragment.
        Reconstitute header for datagram from first fragment
      ---------------------------------------------------------------------*/
      if(0 == frag_offset)
      {
        reas_ptr = curr_ptr;
        frag_offset = (uint16)dsm_length_packet( reas_ptr );
        currhdr.mf = 0;
        currhdr.length = datagram_len + (currhdr.ihl*4);
        if(FALSE == ps_ip4_hdr_create( &reas_ptr, &currhdr, FALSE ))
        {
          IPV4_INC_STATS(reasm_fails,1) ;
          LOG_MSG_ERROR_0("ps_ip4_hdr_create() returned FALSE during reassembly");
          dsm_free_packet( &reas_ptr );
          break;
        }
        pkt_info->ip_hdr.v4 = currhdr;
        *offset = (uint16)(currhdr.ihl*4);
      }
      else
      {
       frag_offset += (uint16)dsm_length_packet( curr_ptr);
        dsm_append(&reas_ptr, &curr_ptr);
      }
    } /* while */

    /*-----------------------------------------------------------------------
      Free the fraghdl structure
    -----------------------------------------------------------------------*/
    ip_fraghdl_free( handle );
    pkt_info->fraghdl = NULL;
//    pkt_info->is_local_frag = FALSE;
  }

  /*-------------------------------------------------------------------------
    Return NULL or reconstituted datagram
  -------------------------------------------------------------------------*/
  return reas_ptr;
} /* ip4_reassemble() */

