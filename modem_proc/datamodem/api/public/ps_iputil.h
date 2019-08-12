#ifndef PS_IPUTIL_H
#define PS_IPUTIL_H
/**
   @file ps_iputil.h
   @brief contains declarations for utility functions used by the IP module. 
 */

/*===========================================================================

                   I P   I N T E R N A L   U T I L I T Y

                           F U N C T I O N S

DESCRIPTION
  This file contains declarations for internal utility functions
  used by the IP module.

Copyright (c) 2002-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the ipUtilities group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iputil.h_v   1.1   31 Jan 2003 18:20:00   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_iputil.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/14   xc      Added cksum_update
08/28/11   sd      (Tech Pubs) Applied new Doxygen grouping markup.
07/20/11   gs      Removed inclusion of <target.h>
12/14/08   pp      Common Modem Interface: Public/Private API split.
10/13/04   vp      Removed inclusion of ps_iphdr.h and included ps_ip4_hdr.h.
                   Removed ntohip() and htonip() functions. Added declaration
                   of buffer_checksum() function.
11/11/03   rc/aku  Added parameter 'offset' to the cksum() function.
01/31/03   usb     Added an extra arg to ntohip() to support hdr copying only
07/25/02   om      Created module from ip.h

===========================================================================*/
/* group: ipUtilities */
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "ps_pkt_info.h"


/*=============================================================================
FUNCTION BUFFER_CKSUM()
=============================================================================*/
/** @ingroup buffer_cksum
  Compute the cksum of the passed buffer of data.

  @param[in] buf            Pointer to consecutive buffer to be checksummed.
  @param[in] count          Count of number of bytes in the buffer.

  @return
  16-bit checksum.\n 

  @dependencies
  None.
  @newpage
*/
uint16 buffer_cksum
(
  uint16 *buf,
  uint16 count
);

/*=============================================================================
FUNCTION CKSUM()
=============================================================================*/
/** @ingroup cksum
  This function calculates the IP checksum.

  @param[in] ph       Pseudo header.
  @param[in] m        IP packet (DSM Pointer)
  @param[in] len      IP header length
  @param[in] offset   DSM offset
  
  @return
  16-bit checksum.\n 

  @dependencies
  None.
  @newpage
*/
uint16 cksum
(
  pseudo_header_type  *ph,
  struct dsm_item_s    *m,
  uint16               len,
  uint16               offset
);

/*=============================================================================
FUNCTION CKSUM_UPDATE()
=============================================================================*/
/** @ingroup cksum
  This function calculates the new IP checksum given an old checksum and 
  old and new pseudo headers

  @param[in] old_ph   Old pseudo header
  @param[in] new_ph   New pseudo header
  @param[in] old_cksum  Old checksum
  
  @return
  16-bit checksum.\n 

  @dependencies
  None.
  @newpage
*/
uint16 cksum_update
(
  pseudo_header_type  *old_ph,  /* old pseudo header */
  pseudo_header_type   *new_ph,  /* old pseudo header */
  uint16                old_cksum /* old cksum */
);

/*=============================================================================
FUNCTION LCSUM()
=============================================================================*/
/** @ingroup lcsum
  This function computes the 1's-complement sum of the given data buffer.
  This uses a inline assembly routine of the processor which reduces
  CPU utilization greatly. The inline assembly uses an unwound loop method 
  to reduce iterations of loop.

  @param[in] buf            Pointer to consecutive buffer to be checksummed.
  @param[in] count          Count of number of bytes in the buffer.
  
  @return
  The ones complement sum of the bytes in the buffer.\n 

  @dependencies
  None.
  @newpage
*/
uint16 lcsum
(
  uint16 *buf,
  uint16 cnt
);

#endif  /* PS_IPUTIL_H */
