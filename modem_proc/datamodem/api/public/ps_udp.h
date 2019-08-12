#ifndef PS_UDP_HDR_H
#define PS_UDP_HDR_H
/*===========================================================================

                         P S _ U D P _ H D R . H

===========================================================================*/

/** 
  @file ps_udp.h 
  @brief 
  The Data Services UDP protocol interface header file. Contains UDP header 
  definition.
*/
 
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the tcpUdpIpIp6 group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (c) 2008, 2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_udp.h_v   1.0   08 Aug 2002 11:19:54   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_udp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/06/11    sd     (Tech Pubs) Edited/added Doxygen markup and comments.
12/14/08    pp     Created module as part of Common Modem Interface: 
                   Public/Private API split.
===========================================================================*/

/* group: tcpUdpIpIp6*/
/*===========================================================================

                        INCLUDE FILES FOR THE MODULE
                       
===========================================================================*/

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/** @addtogroup constants 
  @{
*/
/**
  Maximum UDP payload length. The default 1472-byte length includes a 20-byte 
  IP header and an 8-byte UDP header, which are added to the payload to create 
  a 1500-byte packet. This packet size is the maximum Ethernet payload size. 
  The IP header does not allow fragmentation (the Don't Fragment bit is set); 
  therefore, any payload greater than 1500 bytes is discarded by the Ethernet.  
*/
#define UDP_MAX_PAYLOAD_LEN 1472
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  @brief UDP header.
*/
typedef struct
{
  uint16 source;                                  /**< Source port. */
  uint16 dest;                                    /**< Destination port. */
  uint16 length;                                  /**< UDP packet length. */
  uint16 chksum;                                  /**< UDP packet checksum. */
} udp_hdr_type;

/** @} */ /* end_addtogroup datatypes */
#endif /* PS_UDP_HDR_H */
