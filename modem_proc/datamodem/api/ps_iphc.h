#ifndef _PS_IPHC_H
#define _PS_IPHC_H
/*===========================================================================
                           P S _ I P H C . H

DESCRIPTION
  Header file for the IP header compression (IPHC) protocol (RFC 2507)

Copyright (c) 2004-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/ps_iphc.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/07/09    pp     CMI Phase-4: SU Level API Effort
10/24/08    pp     Modified iphc_get_ctrl_blk prototype to fix compiler 
                   warnings.
08/31/06    mjp    Added IPHC and ROHC Logging
11/01/05    ifk    Ported to UE.
09/02/04    spr    Created module.
===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "ps_logging_defs.h"

/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/
#define IPHC_MAX_INSTANCE_COUNT           16

/*---------------------------------------------------------------------------
  Structure used to pass IPHC configuration options to an IPHC instance.
  This is the minimum set of options currently negotiated with a PPP NCP and
  defined in RFC2509.  It can be extended to include other parameters which
  the client of IPHC needs to set.
---------------------------------------------------------------------------*/
typedef struct iphc_config_options_s_type
{
  uint16 tcp_space;    /* Max value of context identifier (CID) for TCP */
  uint16 non_tcp_space;/* Max value of CID for non-TCP                  */
  uint16 f_max_period; /* Max compressed non-tcp pkts between full hdrs */
  uint16 f_max_time;   /* Max time between full headers in seconds      */
  uint16 max_header;   /* Max header size that can be compressed        */
  uint8  rtp_enabled;  /* Set if IP/UDP/RTP compression is enabled      */
  uint8  expect_reordering; /* If packets can be received out of order  */
} iphc_config_options_s_type;

/*---------------------------------------------------------------------------
  The IPHC packet types.
  The first five are defined in RFC2507 while the remaining compressed types
  are defined in RFC 2508.
---------------------------------------------------------------------------*/
typedef enum iphc_packet_type_e_type
{
  IPHC_PACKET_TYPE_NORMAL_IPV4            = 0,
  IPHC_PACKET_TYPE_NORMAL_IPV6            = 1,
  IPHC_PACKET_TYPE_NORMAL_IP_MAX          = 2, /* Max IP version suported  */
  IPHC_PACKET_TYPE_FULL_HEADER            = 2,
  IPHC_PACKET_TYPE_COMPRESSED_TCP         = 3,
  IPHC_PACKET_TYPE_COMPRESSED_TCP_NODELTA = 4,
  IPHC_PACKET_TYPE_COMPRESSED_NON_TCP     = 5,
  IPHC_PACKET_TYPE_CONTEXT_STATE          = 6, 
  IPHC_PACKET_TYPE_COMPRESSED_UDP_8       = 7, 
  IPHC_PACKET_TYPE_COMPRESSED_UDP_16      = 8,
  IPHC_PACKET_TYPE_COMPRESSED_RTP_8       = 9,
  IPHC_PACKET_TYPE_COMPRESSED_RTP_16      = 10,
  IPHC_PACKET_TYPE_MAX
} iphc_packet_type_e_type;

/*---------------------------------------------------------------------------
  IPHC ioctls
---------------------------------------------------------------------------*/
typedef enum iphc_ioctl_e_type
{
  IPHC_IOCTL_COMPRESSOR_V4_SET_OPTIONS           =  0,
  IPHC_IOCTL_COMPRESSOR_V4_GET_OPTIONS           =  1,
  IPHC_IOCTL_COMPRESSOR_V6_SET_OPTIONS           =  2,
  IPHC_IOCTL_COMPRESSOR_V6_GET_OPTIONS           =  3,
  IPHC_IOCTL_COMPRESSOR_V4_GET_DEFAULT_OPTIONS   =  4,
  IPHC_IOCTL_COMPRESSOR_V6_GET_DEFAULT_OPTIONS   =  5,
  IPHC_IOCTL_DECOMPRESSOR_V4_SET_OPTIONS         =  6,
  IPHC_IOCTL_DECOMPRESSOR_V4_GET_OPTIONS         =  7,
  IPHC_IOCTL_DECOMPRESSOR_V6_SET_OPTIONS         =  8,
  IPHC_IOCTL_DECOMPRESSOR_V6_GET_OPTIONS         =  9,
  IPHC_IOCTL_DECOMPRESSOR_V4_GET_DEFAULT_OPTIONS = 10,
  IPHC_IOCTL_DECOMPRESSOR_V6_GET_DEFAULT_OPTIONS = 11,
  IPHC_IOCTL_COMPRESSOR_V4_INVALIDATE_CONTEXTS   = 12,
  IPHC_IOCTL_DECOMPRESSOR_V4_INVALIDATE_CONTEXTS = 13,
  IPHC_IOCTL_COMPRESSOR_V6_INVALIDATE_CONTEXTS   = 14,
  IPHC_IOCTL_DECOMPRESSOR_V6_INVALIDATE_CONTEXTS = 15,
  IPHC_IOCTL_MAX
} iphc_ioctl_e_type;

/*---------------------------------------------------------------------------
  IPHC ioctls data
---------------------------------------------------------------------------*/
typedef union iphc_ioctl_u_type
{
  iphc_config_options_s_type options;
} iphc_ioctl_u_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IPHC_POWERUP_INIT

DESCRIPTION
  Initializes the IPHC layer at powerup.  Sets up the IPHC memory heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes IPHC state variables
===========================================================================*/
void iphc_powerup_init
(
  void
);


/*===========================================================================
FUNCTION IPHC_ALLOC

DESCRIPTION
  Allocates and returns a handle to the IPHC instance, or NULL on error.  The
  handle is passed to all the other IPHC functions and is freed with a call
  to iphc_free().
  
  The default options are used for any set of options passed as NULL.

DEPENDENCIES
  None

RETURN VALUE
  non-NULL handle on success
  NULL on failure

SIDE EFFECTS
  Allocates an IPHC instance
===========================================================================*/
void *iphc_alloc
(
  iphc_config_options_s_type *ipv4_compressor_opt,
  iphc_config_options_s_type *ipv4_decompressor_opt,
  iphc_config_options_s_type *ipv6_compressor_opt,
  iphc_config_options_s_type *ipv6_decompressor_opt
);

/*===========================================================================
FUNCTION IPHC_SET_LOGGING

DESCRIPTION
  Sets up IPHC logging if called.

DEPENDENCIES
  The IPHC memheap must have been initialized and iphc_alloc called prior to
  calling iphc_set_logging(). MUST USE iphc_free_logging() before clearing 
  the IPHC instance.

PARAMETERS
  void             *handle - pointer to the iphc handle
  dpl_link_cb_type *dpl_cb - pointer to the dpl cb of associated phys link

RETURN VALUE
   0   if successful.
  -1   in case of null arguments.

SIDE EFFECTS
  Sets the IPHC logging control block address variable
===========================================================================*/
int iphc_set_logging
(
  void                 *handle,
  dpl_link_cb_type     *dpl_cb
);

/*===========================================================================
FUNCTION IPHC_FREE_LOGGING

DESCRIPTION
  Frees up logging so it is no longer done.

DEPENDENCIES
  The IPHC memheap must have been initialized and iphc_alloc called prior to
  calling iphc_free_logging()

PARAMETERS
  void *handle - pointer to the iphc handle

RETURN VALUE
   0   if successful.
  -1   in case of null handle.

SIDE EFFECTS
  Sets the IPHC logging control block address variable to null
===========================================================================*/
int iphc_free_logging
(
  void  *handle
);


/*===========================================================================
FUNCTION IPHC_IOCTL

DESCRIPTION
  Sets or gets IPHC parameters

  Takes an IPHC instance, the type of ioctl and associated data.
  Depending on the IOCTL the data may be used to return information or used
  to pass information.

DEPENDENCIES
  A valid handle must be passed unless performing a GET_DEFAULT_OPTIONS
  ioctl.  Also, ioctl_data must be non-NULL unless using an INVALIDATE
  ioctl.

RETURN VALUE
  -1 in case of error (e.g., invalid arguments)
   0 in case of success

SIDE EFFECTS
  The context associated with the side may be reinitialized if ioctl sets
  options.
===========================================================================*/
int iphc_ioctl
(
  void              *handle,           /* Instance of IPHC                 */
  iphc_ioctl_e_type  ioctl_type,       /* The IOCTL                        */
  iphc_ioctl_u_type *ioctl_data        /* Associated ioctl information     */
);


/*===========================================================================
FUNCTION IPHC_COMPRESS

DESCRIPTION
  Takes the IPHC instance handle and the packet to compress.  Returns the
  compressed packet in pkt and the compressed packet type in pkt_type.

  In the case of a recoverable error (e.g., an uncompressible packet), this
  function may return success (0) with a pkt_type of
  IPHC_PACKET_TYPE_NORMAL_IPV4 or IPHC_PACKET_TYPE_NORMAL_IPV6 (that is, the
  packet was not modified). 

DEPENDENCIES
  A valid handle must be passed.

RETURN VALUE
  -1 in case of error (e.g., invalid arguments)
   0 if successful.
   Compressed packet in pkt, compressed packet type in pkt_type.

SIDE EFFECTS
  May compress the packet returning it in pkt with the packet type returned
  in pkt_type.
===========================================================================*/
int iphc_compress
(
  void                     *handle,    /* IPHC instance                    */
  dsm_item_type           **pkt,       /* Packet to compress               */
  iphc_packet_type_e_type  *pkt_type   /* Packet type after compression    */
);


/*===========================================================================
FUNCTION IPHC_DECOMPRESS

DESCRIPTION
  Function to decompress an IPHC compressed packet.  The decompressed packet
  is returned in pkt, and the type (IPHC_PACKET_TYPE_NORMAL_IPV4 or
  IPHC_PACKET_TYPE_NORMAL_IPV6) is returned in pkt_type.  If the sk_buff must
  be expanded to accomodate the decompressed header, the old buffer is freed,
  and *pkt is set to point to the new buffer.  Regardless of how the function
  returns, the caller is responsible for freeing *pkt.

  If a CONTEXT_STATE packet is passed to decompress, it will be processed,
  but because this processing produces no resultant packet, -1 will be
  returned.  Attempting to decompress a regular, uncompressed IP packet
  will result in an error (-1 return value).
  
  If a CONTEXT_STATE packet is to be sent to the remote compressor, an
  sk_buff containing the packet will be allocated, and *snd_pkt will
  be set to point to the packet.  If no CONTEXT_STATE packet is
  to be sent, *snd_pkt will remain unchanged (NULL).  To prevent
  CONTEXT_STATE packets from being generated, pass NULL for snd_pkt;
  if snd_pkt is non-NULL, *snd_pkt should be NULL.  Note that if a
  CONTEXT_STATE packet is generated, freeing the allocated sk_buff 
  (with kfree_skb()) is the caller's responsibility.
  
  Note also that the return value relates to whether or not decompression was
  successful; CONTEXT_STATE packets, however, are generated because
  decompression failed, thus even if this function returns -1, the caller
  must still check for generated CONTEXT_STATE packets.

DEPENDENCIES
  A valid handle must be passed; snd_pkt, if non-NULL, should point to
  a NULL sk_buff; pkt_type must point to the type of the packet to be
  decompressed.  

RETURN VALUE
  -1 in case of error (e.g., invalid arguments, or decompression failed)
   0 if decompression was successful
  
  The decompressed packet type is returned in pkt_type.

SIDE EFFECTS
  Decompresses the received packet.  May generate an sk_buff containing a
  CONTEXT_STATE packet.  Note that the caller is responsible for freeing this
  buffer with kfree_skb().  *pkt may be reallocated.  If an error occurs,
  *pkt will still be valid (and must be freed by the caller) but its contents
  are undefined.  pkt_type may also be modified even if an error occurs.
===========================================================================*/
int iphc_decompress
(
  void                     *handle,    /* Instance of IPHC                 */
  dsm_item_type           **pkt,       /* Packet to decompress             */
  iphc_packet_type_e_type  *pkt_type,  /* Packet type of pkt               */
  dsm_item_type           **snd_pkt    /* May contain CONTEXT_STATE request*/
);


/*===========================================================================
FUNCTION IPHC_FREE

DESCRIPTION
  Frees the IPHC instance.  The handle should not be used after a call to
  iphc_free().

DEPENDENCIES
  A valid handle must be passed

RETURN VALUE
  None

SIDE EFFECTS
  Frees the IPHC instance
===========================================================================*/
void iphc_free
(
  void                     *handle     /* IPHC instance                    */
);

/*===========================================================================
FUNCTION IPHC_GET_CTRL_BLK_INDEX

DESCRIPTION
  Given a control block pointer, returns the index of the control block

DEPENDENCIES
  None

RETURN VALUE
  Index of the control block

SIDE EFFECTS
  None
===========================================================================*/
uint8 iphc_get_ctrl_blk_index
(
  void * instp
);

/*===========================================================================
FUNCTION IPHC_GET_CTRL_BLK

DESCRIPTION
  Given a control block index, returns the pointer to the control block

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the control block
  NULL if invalid index provided

SIDE EFFECTS
  None
===========================================================================*/
void *iphc_get_ctrl_blk
(
  int32 index
);

#endif /* _PS_IPHC_H */
