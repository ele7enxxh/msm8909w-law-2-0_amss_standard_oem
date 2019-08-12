#ifndef PS_LOGGING_HELPER_H
#define PS_LOGGING_HELPER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     P S _ L O G G I N G _ H E L P E R . H

GENERAL DESCRIPTION

  Contains the helper routines to log data.

SUBSYSTEM INTERNAL FUNCTIONS
  DPLI_LOG_FULL_PACKET()
    Used to log full packet

Copyright (c) 2004-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ps_logging_helper.h#1 $
 $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when          who    what, where, why
--------      ---    -------------------------------------------------------- 
11/30/11      su     Adding log code support for DPL logging 
07/09/10      sy     Fixed type-punned compiler warnings.
07/23/09      pp     Removed diagtune.h from inclusions.
11/13/07      pp     Lint medium/low - fixes.
08/31/06      mjp    Added IPHC and ROHC Logging
06/05/06      mp     Added DPL PPP Partial Logging
05/12/05      ks     fixed lint errors
01/27/05      ssh    Changed include file name dsbyte.h to ps_byte.h
01/12/05      msr    Added code review comments.
01/10/05      ks     Fixed DPLI_LOG_LINK_FULL_PACKET macro.
01/10/05      msr    Added support for flow and link logging.
11/16/04    msr/ks   Fixed the offset_ptr parameter passed to
                     dsm_seek_extract() in DPLI_BUILD_LOG_PACKET macro.
11/09/04    msr/ks   Fixed DPLI_BUILD_LOG_PACKET to take values rather than
                     pointers.
10/31/04    msr/ks   Major Clean up. Also created ps_logging_helper.c file to
                     contain the code for dpli_log_frames() which is defined
                     here.
09/13/04    msr/ks   Created the module.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "customer.h"

#include "log.h"
#include "err.h"
#include "log_codes.h"
#include "dsm.h"
#include "diagdiag.h"
#include "ps_byte.h"
#include "ps_logging_defs.h"
#include "ps_phys_link.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_dpm_hw.h"



/*===========================================================================

                               CONSTANTS

===========================================================================*/
/*---------------------------------------------------------------------------
  Size of header added by DIAG subsytem to each log packet
---------------------------------------------------------------------------*/
#define LOG_HDR_SIZE  sizeof(log_hdr_type)

/*---------------------------------------------------------------------------
  Size of header added by PS to each log packet
---------------------------------------------------------------------------*/
#define DPL_LOG_HDR_SIZE  8

/*---------------------------------------------------------------------------
  Total size of all headers, visible to PS, that are included in log packet
---------------------------------------------------------------------------*/
#define DPL_TOTAL_HDR_SIZE  (LOG_HDR_SIZE + DPL_LOG_HDR_SIZE)

/*---------------------------------------------------------------------------
  Maximum data bytes from the packet to be logged that can be carried
  in each log packet
---------------------------------------------------------------------------*/
#define MAX_PAYLOAD  256


/*===========================================================================

                          HELPER FUNCTIONS

===========================================================================*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION    DPLI_LOG_FULL_PACKET

DESCRIPTION
  Logs a full packet, potentially splitting it to multiple segments

PARAMETERS
  item_ptr      : pointer to the dsm chain to be logged
  len           : length of the packet to be logged
  iid           : IID of the data log point
  frame_seq_num : sequence number for the packet being logged

RETURN VALUE
  TRUE  : packet was logged
  FALSE : packet was not logged

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dpli_log_full_packet
(
  dsm_item_type  * item_ptr,
  uint32           len,
  dpl_iid_type     iid,
  uint16           frame_seq_num
);

/*===========================================================================
FUNCTION    DPLI_LOG_PPP_PARTIAL_PACKET

DESCRIPTION
  Logs a packet potentially only partially and possibly by splitting it
  to multiple segments

PARAMETERS
  item_ptr          : pointer to the dsm chain to be logged
  log_flag_link_cb  : pointer to the ppp link layer control block

RETURN VALUE
  TRUE  : partial packet was logged
  FALSE : partial packet was not logged

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dpli_log_ppp_partial_packet
(
  const dsm_item_type   *  item_ptr,
  dpl_iid_type             dpl_id,
  dpl_link_dir_cb_type  *  log_flag_link_cb
);

#ifdef FEATURE_DATA_IPA
/*===========================================================================
FUNCTION      PS_IPA_DPL_INIT

DESCRIPTION
  This function initialize IPA DPL module. It needs to be called at power up.

PARAMETERS
  none.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void ps_ipa_dpl_init
(
  void
);

/*===========================================================================
FUNCTION      PS_IPA_DPL_DE_INIT

DESCRIPTION
  This function flushes all packets from the A2 DPL watermark.

PARAMETERS
  none.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/

void ps_ipa_dpl_de_init
(
  void
);
#endif /* FEATURE_DATA_IPA */

#ifdef FEATURE_DATA_A2
/*===========================================================================
FUNCTION      PS_A2_DPL_INIT

DESCRIPTION   Function to initialize the module. This function needs to be
              called once during ps init. All one time initializations
              will happen in this module.

PARAMETERS    none.

DEPENDENCIES  none.

RETURN VALUE  none.

SIDE EFFECTS  none.
===========================================================================*/

void ps_a2_dpl_init
(
  void
);

/*===========================================================================
FUNCTION      PS_A2_DPL_DE_INIT

DESCRIPTION
  This function flushes all packets from the A2 DPL watermark.

PARAMETERS
  none.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/

void ps_a2_dpl_de_init
(
  void
);
#endif /* FEATURE_DATA_A2 */
/*===========================================================================

                          HELPER MACROS

===========================================================================*/
/*===========================================================================
MACRO    DPLI_BUILD_LOG_PACKET

DESCRIPTION
  Constructs the log packet by filling in header and data information.

PARAMETERS
  item_ptr_ptr  : pointer to the dsm chain to extract data from
  log_ptr       : Pointer to the log packet
  len_ptr       : Number of bytes to be extracted
  iid           : IID associated with this log packet
  frame_seq_num : Sequence number for this log packet
  seg_num       : Segment number for this log packet
  offset_ptr    : Number of bytes to offset into packet

RETURN VALUE
  NONE
===========================================================================*/
#define DPLI_BUILD_LOG_PACKET(item_ptr_ptr, log_ptr, offset_ptr,         \
                              iid, len_ptr, frame_seq_num, seg_num)      \
  /*lint -save -e740 IID is a 4 byte value but represented in a struct */\
  /*casting to void* to take care of type-punned compiler warning      */\
  log_ptr = put32(log_ptr, *(uint32 *)((void *) &(iid)));                \
  /*lint -restore */                                                     \
  log_ptr = hput16(log_ptr, frame_seq_num);                              \
  log_ptr = hput16(log_ptr, seg_num);                                    \
                                                                         \
  (void) dsm_seek_extract(item_ptr_ptr, offset_ptr, log_ptr, len_ptr);   \
  *offset_ptr += *len_ptr;

/*===========================================================================
MACRO  DPLI_LOG_GET_PACKET_SNAPLEN

DESCRIPTION
  returns length of network protocol packet 

PARAMETERS
  log_flag_net_cb : pointer to logging control block
  proto                : network protocol of packet to be logged
  
RETURN VALUE
  integer length of packet
  Will return 0 when logging is not enabled 
  and logging for protocol is not set
===========================================================================*/
#define DPLI_LOG_GET_PACKET_SNAPLEN(log_flag_net_cb, proto)   \
  ((TRUE == log_flag_net_cb.is_logging_enabled &&           \
     0 != (log_flag_net_cb.mask & (0x1 << (proto - 1))))    \
     ? log_flag_net_cb.snaplen[proto]                     \
     : 0)     
/*===========================================================================
MACRO   DPLI_LOG_NETWORK_PACKET

DESCRIPTION
  Log a network protocol packet (Common for network and flow logging)

PARAMETERS
  log_flag_net_cb : pointer to logging control block
  packet          : packet to be logged
  protocol        : network protocol of packet to be logged

RETURN VALUE
  TRUE on success
  FALSE otherwise
===========================================================================*/
#define DPLI_LOG_NETWORK_PACKET(log_flag_net_cb, dpl_id, packet, proto)  \
  if ((FALSE == PS_DPM_HW_IS_DPL_ENABLED())        &&                      \
       TRUE  == log_flag_net_cb.is_logging_enabled &&                     \
     0 != (log_flag_net_cb.mask & (0x1 << (proto - 1))))                 \
  {                                                                      \
    /*-------------------------------------------------------------------\
      Only allocate IID variable if logging is turned on, to impact      \
      less on data path in the common case (logging disabled).           \
    -------------------------------------------------------------------*/\
    dpl_iid_type net_dpl_id;                                             \
                                                                         \
    if (NULL == packet)                                                  \
    {                                                                    \
      LOG_MSG_ERROR_0("NULL item ptr");                                  \
    }                                                                    \
    else                                                                 \
    {                                                                    \
      /*-----------------------------------------------------------------\
        IID is passed at beginning of each DPL log packet. Only protocol \
        field needs to be filled here. Rest of the IID is populated when \
        ps_iface is created in ps_iface_create()                         \
      -----------------------------------------------------------------*/\
      net_dpl_id          = dpl_id;                                      \
      net_dpl_id.protocol = proto;                                       \
                                                                         \
      /*-----------------------------------------------------------------\
        If snaplen is SNAPLEN_ALL log whole packet else log snaplen      \
        number of bytes. By doing this partial logging is supported      \
      -----------------------------------------------------------------*/\
      if (SNAPLEN_ALL == log_flag_net_cb.snaplen[proto])                 \
      {                                                                  \
        dpli_log_full_packet(packet,                                     \
                             dsm_length_packet(packet),                  \
                             net_dpl_id,                                 \
                             log_flag_net_cb.seq_num[proto]++);          \
      }                                                                  \
      else if (0 == log_flag_net_cb.snaplen[proto])                      \
      {                                                                  \
        dpli_log_full_packet(packet,                                     \
                             dsm_length_packet(packet),                  \
                             net_dpl_id,                                 \
                             log_flag_net_cb.seq_num[proto]++);          \
      }                                                                  \
	  else                                                               \
      {                                                                  \
        uint32  pkt_len = dsm_length_packet(packet);                     \
        pkt_len = MIN(log_flag_net_cb.snaplen[proto], pkt_len);          \
        dpli_log_full_packet(packet,                                     \
                             pkt_len,                                    \
                             net_dpl_id,                                 \
                             log_flag_net_cb.seq_num[proto]++);          \
      }                                                                  \
    }                                                                    \
  }



/*===========================================================================
MACRO   DPLI_LOG_LINK_FULL_PACKET

DESCRIPTION
  Log a full link protocol packet

PARAMETERS
  log_flag_link_cb : pointer to logging control block
  packet           : packet to be logged
  protocol         : link protocol of packet to be logged

RETURN VALUE
  TRUE on success
  FALSE otherwise
===========================================================================*/
#define DPLI_LOG_LINK_FULL_PACKET(log_flag_link_cb, dpl_id, packet,      \
                                  proto)                                 \
  if (TRUE == log_status(LOG_DATA_PROTOCOL_LOGGING_C) &&                 \
      TRUE == log_flag_link_cb.is_logged)                                \
  {                                                                      \
    /*-------------------------------------------------------------------\
      Only allocate IID variable if logging is turned on, to impact      \
      less on data path in the common case (logging disabled).           \
    -------------------------------------------------------------------*/\
    dpl_iid_type link_dpl_id;                                            \
    uint32       dsm_pkt_len;                                            \
                                                                         \
    if (NULL == packet)                                                  \
    {                                                                    \
      LOG_MSG_ERROR_0("NULL item ptr");                                  \
    }                                                                    \
    else                                                                 \
    {                                                                    \
      /*-----------------------------------------------------------------\
        IID is passed at beginning of each DPL log packet. Only protocol \
        field needs to be filled here. Rest of the IID is populated when \
        ps_iface is created in ps_iface_create()                         \
      -----------------------------------------------------------------*/\
      link_dpl_id               = dpl_id;                                \
      link_dpl_id.protocol      = proto;                                 \
                                                                         \
      /*-----------------------------------------------------------------\
        If snaplen is SNAPLEN_ALL log whole packet else log snaplen      \
        number of bytes. By doing this partial logging is supported      \
      -----------------------------------------------------------------*/\
      if (SNAPLEN_ALL == log_flag_link_cb.snaplen                        \
	        || 0 == log_flag_link_cb.snaplen)                            \
      {                                                                  \
        dpli_log_full_packet(packet,                                     \
                             dsm_length_packet(packet),                  \
                             link_dpl_id,                                \
                             log_flag_link_cb.seq_num[proto]++);         \
      }                                                                  \
      else                                                               \
      {                                                                  \
        dsm_pkt_len = dsm_length_packet(packet);                         \
        dpli_log_full_packet(packet,                                     \
                             MIN(log_flag_link_cb.snaplen, dsm_pkt_len), \
                             link_dpl_id,                                \
                             log_flag_link_cb.seq_num[proto]++);         \
      }                                                                  \
    }                                                                    \
  }

/*===========================================================================
MACRO   DPLI_LOG_LINK_FULL_COMP_PACKET

DESCRIPTION
  Log a full link compressed protocol packet. This is needed to increment pkt
  length by one when snaplength is non zero.

PARAMETERS
  log_flag_link_cb : pointer to logging control block
  packet           : packet to be logged
  protocol         : link protocol of packet to be logged
  type             : type of packet being logged

RETURN VALUE
  TRUE on success
  FALSE otherwise
===========================================================================*/
#define DPLI_LOG_LINK_FULL_COMP_PACKET(log_flag_link_cb,                 \
                                       dpl_id, packet,                   \
                                       proto)                            \
  if (TRUE == log_status(LOG_DATA_PROTOCOL_LOGGING_C) &&                 \
      TRUE == log_flag_link_cb.is_logged)                                \
  {                                                                      \
    /*-------------------------------------------------------------------\
      Only allocate IID variable if logging is turned on, to impact      \
      less on data path in the common case (logging disabled).           \
    -------------------------------------------------------------------*/\
    dpl_iid_type link_dpl_id;                                            \
                                                                         \
    if (NULL == packet)                                                  \
    {                                                                    \
      LOG_MSG_ERROR_0("NULL item ptr");                                  \
    }                                                                    \
    else                                                                 \
    {                                                                    \
      /*-----------------------------------------------------------------\
        IID is passed at beginning of each DPL log packet. Only protocol \
        field needs to be filled here. Rest of the IID is populated when \
        ps_iface is created in ps_iface_create()                         \
      -----------------------------------------------------------------*/\
      link_dpl_id               = dpl_id;                                \
      link_dpl_id.protocol      = proto;                                 \
                                                                         \
      /*-----------------------------------------------------------------\
        If snaplen is SNAPLEN_ALL log whole packet else log snaplen      \
        number of bytes. By doing this partial logging is supported.     \
        For compressed packet types this is incremented by one so that   \
        pkt                                                              \
        type takes no space                                              \
      -----------------------------------------------------------------*/\
      if (SNAPLEN_ALL == log_flag_link_cb.snaplen)                       \
      {                                                                  \
        dpli_log_full_packet(packet,                                     \
                             dsm_length_packet(packet),                  \
                             link_dpl_id,                                \
                             log_flag_link_cb.seq_num[proto]++);         \
      }                                                                  \
	  else if (0 == log_flag_link_cb.snaplen)                            \
	  {                                                                  \
        dpli_log_full_packet(packet,                                     \
                             dsm_length_packet(packet),                  \
                             link_dpl_id,                                \
                             log_flag_link_cb.seq_num[proto]++);         \
      }                                                                  \
      else                                                               \
      {                                                                  \
        /*---------------------------------------------------------------\
          Note that compressed type has already been added as part of    \
          the DSM chain. snaplen must also consider this extra byte.     \
          note hack re. 65533 - assumes sizeof(dpl_comp_hder) is 3       \
        ---------------------------------------------------------------*/\
        uint32 pkt_len = dsm_length_packet(packet);                      \
        if(log_flag_link_cb.snaplen < 65533)                             \
        {                                                                \
          pkt_len = MIN(log_flag_link_cb.snaplen +                       \
                                 sizeof(dpl_comp_header_type), pkt_len); \
        }                                                                \
        else                                                             \
        {                                                                \
          pkt_len = MIN(log_flag_link_cb.snaplen, pkt_len);              \
        }                                                                \
                                                                         \
        dpli_log_full_packet(packet,                                     \
                             pkt_len,                                    \
                             link_dpl_id,                                \
                             log_flag_link_cb.seq_num[proto]++);         \
      }                                                                  \
    }                                                                    \
  }

/*===========================================================================
MACRO   DPLI_LOG_PPP_PARTIAL_PACKET

DESCRIPTION
  Partially log a ppp protocol packet

PARAMETERS
  log_flag_link_cb : pointer to logging control block
  dpl_id           : pointer to the DPL ID
  packet           : packet to be logged

RETURN VALUE
  TRUE on success
  FALSE otherwise
===========================================================================*/
#define DPLI_LOG_PPP_PARTIAL_PACKET(log_flag_link_cb, dpl_id, packet)    \
  if (TRUE == log_status(LOG_DATA_PROTOCOL_LOGGING_C) &&                 \
      TRUE == log_flag_link_cb.is_logged)                                \
  {                                                                      \
    /*-------------------------------------------------------------------\
      IID is stored in the Link Logging Control Block in the ps_iface    \
      structure. The IID is populated when ps_iface is created in        \
      ps_iface_create(). The dpl_id.protocol is automatically set to     \
      ppp-hdlc since this may only be called when using ppp.             \
    -------------------------------------------------------------------*/\
                                                                         \
    dpl_id.protocol  =  (unsigned int)DPL_IID_LINKPROT_PPP_IN_HDLC;      \
                                                                         \
    dpli_log_ppp_partial_packet(packet, dpl_id, &log_flag_link_cb);     \
  }

#endif /* PS_LOGGING_HELPER_H */
