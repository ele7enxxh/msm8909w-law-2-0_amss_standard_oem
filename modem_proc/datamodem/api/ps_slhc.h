#ifndef PS_SLHC_H
#define PS_SLHC_H

/*===========================================================================

                          P S  _ S L H C . H

DESCRIPTION
  Header file for the VJ header compression protocol (RFC 1144)
  
 Redistribution and use in source and binary forms are permitted
 provided that the above copyright notice and this paragraph are
 duplicated in all such forms and that any documentation,
 advertising materials, and other materials related to such
 distribution and use acknowledge that the software was developed
 by the University of California, Berkeley.  The name of the
 University may not be used to endorse or promote products derived
 from this software without specific prior written permission.
 THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 Compressed packet format:

 The first octet contains the packet type (top 3 bits), TCP
 'push' bit, and flags that indicate which of the 4 TCP sequence
 numbers have changed (bottom 5 bits).  The next octet is a
 conversation number that associates a saved IP/TCP header with
 the compressed packet.  The next two octets are the TCP checksum
 from the original datagram.  The next 0 to 15 octets are
 sequence number changes, one change per bit set in the header
 (there may be no changes and there are two special cases where
 the receiver implicitly knows what changed -- see below).

 There are 5 numbers which can change (they are always inserted
 in the following order): TCP urgent pointer, window,
 acknowlegement, sequence number and IP ID.  (The urgent pointer
 is different from the others in that its value is sent, not the
 change in value.)  Since typical use of SLIP links is biased
 toward small packets (see comments on MTU/MSS below), changes
 use a variable length coding with one octet for numbers in the
 range 1 - 255 and 3 octets (0, MSB, LSB) for numbers in the
 range 256 - 65535 or 0.  (If the change in sequence number or
 ack is more than 65535, an uncompressed packet is sent.)

 Packet types (must not conflict with IP protocol version)

 The top nibble of the first octet is the packet type.  There are
 three possible types: IP (not proto TCP or tcp with one of the
 control flags set); uncompressed TCP (a normal IP/TCP packet but
 with the 8-bit protocol field replaced by an 8-bit connection id --
 this type of packet syncs the sender & receiver); and compressed
 TCP (described above).

 LSB of 4-bit field is TCP "PUSH" bit (a worthless anachronism) and
 is logically part of the 4-bit "changes" field that follows.  Top
 three bits are actual packet type.  For backward compatibility
 and in the interest of conserving bits, numbers are chosen so the
 IP protocol version number (4) which normally appears in this nibble
 means "IP packet".

 Copyright (c) 1989 Regents of the University of California.
 All rights reserved.
 Redistribution and use in source and binary forms are permitted
 provided that the above copyright notice and this paragraph are
 duplicated in all such forms and that any documentation,
 advertising materials, and other materials related to such
 distribution and use acknowledge that the software was developed
 by the University of California, Berkeley.  The name of the
 University may not be used to endorse or promote products derived
 from this software without specific prior written permission.
 THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.

Copyright (c) 1995-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/ps_slhc.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $ 

when       who    what, where, why
--------   ---    ----------------------------------------------------------
05/06/09   ss     SLHC De-coupling effort, getting the interface in sync with 
                  existing HC modules.      
08/15/05   mct    Fixed naming causing some issues w/C++ compilers.
08/30/04   mct    Removed some function prototypes to cleanup unused code.
02/02/03   usb    Added device type to slhc_init().
07/25/02   mvl    Renamed file, file cleanup.
11/09/95   jjw    IS-99 Compliant Release
===========================================================================*/

#include "comdef.h"
#include "dsm.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  SLHC return codes.  
---------------------------------------------------------------------------*/
#define SLHC_SUCCESS  0    /* Operation successful */
#define SLHC_FAILED  -1    /* Operation failed */

/* Limits on number of slots to hold for TCP/IP headers */
#define SLHC_MIN_SLOTS 1
#define SLHC_MAX_SLOTS 16    

/* Handle that clients would use to refer to the slhc instance */
typedef void *slhc_handle_type;

/*---------------------------------------------------------------------------
  The SLHC packet types.  
---------------------------------------------------------------------------*/
typedef enum slhc_packet_type_e_type
{
  SLHC_PACKET_TYPE_ERROR			= -1,
  SLHC_PACKET_TYPE_IP               = 0,
  SLHC_PACKET_TYPE_UNCOMPRESSED_TCP = 1,
  SLHC_PACKET_TYPE_COMPRESSED_TCP   = 2,
  SLHC_PACKET_TYPE_MAX              = 0x7FFFFFFF
} slhc_packet_type_e_type;

/*---------------------------------------------------------------------------
  Structure used to pass SLHC configuration options to an SLHC instance.
---------------------------------------------------------------------------*/
typedef struct slhc_config_options_s_type
{
  uint32 rslots;   /* To get/set the number of receiver slots */
  uint32 tslots;   /* To get/set the number of transmitter slots */
} slhc_config_options_s_type;

/*---------------------------------------------------------------------------
  SLHC ioctls
  
  SLHC_IOCTL_CONTEXT_REINITIALIZATION
    Reset the existing context list.
    
  SLHC_IOCTL_CHANNEL_REINITIALIZATION
    Reallocate memory for all contexts.
    rslots, tslots value can be changed.

  SLHC_IOCTL_SET_TOSS
    Set the toss flag.
    All further packets with compressed cid are tossed.
---------------------------------------------------------------------------*/
typedef enum slhc_ioctl_e_type
{
  SLHC_IOCTL_CONTEXT_REINITIALIZATION = 0, 
  SLHC_IOCTL_CHANNEL_REINITIALIZATION = 1,
  SLHC_IOCTL_SET_TOSS                 = 2, 
  SLHC_IOCTL_MAX                      = 0x7FFFFFFF
} slhc_ioctl_e_type;

/*---------------------------------------------------------------------------
  SLHC ioctl data
---------------------------------------------------------------------------*/
typedef union slhc_ioctl_u_type
{
  /* Holds the SLHC configuration options */
  slhc_config_options_s_type options;  
} slhc_ioctl_u_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION SLHC_ALLOC

DESCRIPTION
  Allocates and returns a handle to the SLHC instance, or NULL on error.

DEPENDENCIES
  slot values must lie between SLHC_MIN_SLOTS and SLHC_MAX_SLOTS

PARAMETERS
  int rslots - The number of connection slots to allocate for the receiver.
  int tslots - The number of conneciton slots to allocate for the transmitter.

RETURN VALUE
  non-NULL handle on success
  NULL on failure
===========================================================================*/
slhc_handle_type slhc_alloc
(
  uint32 rslots, 
  uint32 tslots
);

/*===========================================================================
FUNCTION SLHC_FREE

DESCRIPTION
  Frees the SLHC instance.  The handle should not be used after a call to
  slhc_free().

DEPENDENCIES
  A valid handle must be passed

PARAMETERS
  slhc_handle_type handle - SLHC handle.

RETURN VALUE
  None
===========================================================================*/
void slhc_free
(
  slhc_handle_type handle
);

/*===========================================================================
FUNCTION SLHC_COMPRESS

DESCRIPTION
  Takes the SLHC instance handle and the packet to compress. Returns the
  compressed packet in bpp.

DEPENDENCIES
  A valid handle must be passed.

PARAMETERS
  slhc_handle_type handle - SLHC handle.
  struct dsm_item_s **bpp - Packet to be compressed.
  int compress_cid - Flag which enables or disables connection id compression.
  slhc_packet_type_e_type *pkt_type - Packet type after compression.

RETURN VALUE
  -1 in case of failure.
  0 in case of success.
===========================================================================*/
int32 slhc_compress
(
  slhc_handle_type         handle,
  struct dsm_item_s      **bpp, 
  int32                    compress_cid,
  slhc_packet_type_e_type *pkt_type
);

/*===========================================================================
FUNCTION SLHC_DECOMPRESS

DESCRIPTION
  This function accepts the SLHC handle and the compressed packet and returns 
  the compressed packet in bpp. 

DEPENDENCIES
  A valid handle must be passed.
  A valid packet type must be passed.
  Non null packet should be sent in all cases except when the packet type is 
  SLHC_PACKET_TYPE_ERROR

PARAMETERS
  slhc_handle_type handle - SLHC handle.
  struct dsm_item_s **bpp - Packet to be decompressed.
  slhc_packet_type_e_type *pkt_type - Packet type.

RETURN VALUE
  -1 in case of error (e.g., invalid arguments, pkt_type equals 
  SLHC_PACKET_TYPE_ERROR or decompression failed).
  0 if the packet is returned as it is.
  Length of the decompressed packet if decompression was successful
===========================================================================*/
int32 slhc_decompress
(
  slhc_handle_type         handle,
  struct dsm_item_s      **bpp,
  slhc_packet_type_e_type  pkt_type
);

/*===========================================================================
FUNCTION SLHC_IOCTL

DESCRIPTION
  Sets or gets SLHC parameters.

DEPENDENCIES
  A valid handle and ioctl_type must be passed.

PARAMETERS
  slhc_handle_type handle - SLHC handle.
  slhc_ioctl_e_type ioctl_type - IOCTL type.
  slhc_ioctl_u_type *ioctl_data - Depending on the ioctl type this is used to 
                                  return information or retrieve information.

RETURN VALUE
  -1 in case of error (e.g., invalid arguments)
   0 in case of success
===========================================================================*/
int32 slhc_ioctl
(
  slhc_handle_type         handle,
  slhc_ioctl_e_type        ioctl_type,
  const slhc_ioctl_u_type *ioctl_data
);
#endif  /* PS_SLHC_H */
