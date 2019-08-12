#ifndef _PS_IFACE_MBIM_DEFS_H
#define _PS_IFACE_MBIM_DEFS_H
/*===========================================================================

                     PS_IFACE_MBIM_DEFS.H
DESCRIPTION
Header file describing MBIM Header specific structures.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_iface_mbim_defs.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/28/2011  pgm    Removed NDP Signature mask and changed NDP signature to Spec                    0.3.
09/15/2011  pgm    Created File.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*---------------------------------------------------------------------------
  Definition for NCM Transfer Block (NTB)
---------------------------------------------------------------------------*/
#define PS_IFACE_MBIM_NTH16_SIGNATURE         0x484D434E    /* NCMH for NTH16 */
#define PS_IFACE_MBIM_NTH16_HEAD_LEN          0x000C

#define PS_IFACE_MBIM_NDP16_SIGNATURE         0x344D434E

typedef PACKED struct PACKED_POST
{
  uint32              signature;
  uint16              hdr_len;
  uint16              seq_num;
  uint16              mbim_block_len;
  uint16              first_ndp_index;
} ps_iface_mbim_ntb_nth_hdr;

typedef PACKED struct PACKED_POST
{
  uint32              signature;
  uint16              len;
  uint16              next_ndp_index;
} ps_iface_mbim_ntb_ndp_hdr;

typedef PACKED struct PACKED_POST
{
  uint16              index;
  uint16              len;
} ps_iface_mbim_ntb_ndp_ip_datagram_info;

#endif /* _PS_IFACE_MBIM_DEFS_H */

