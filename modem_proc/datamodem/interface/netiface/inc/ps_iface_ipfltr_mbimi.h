#ifndef PS_IFACE_IPFLTR_MBIMI_H
#define PS_IFACE_IPFLTR_MBIMI_H
/*===========================================================================
  @file ps_iface_ipfltr_mbimi.h

  TODO

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_iface_ipfltr_mbimi.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ps_mem.h"

/** @ps_iface_ipfltr_mbimi_free

  This function free packet filter rule

  @dataypes
  filter_node : pointer to the filter node

  @return
  TRUE : success
  FALSE : otherwise

  @dependencies
  None.
  
*/
void ps_iface_ipfltr_mbimi_free
(
  ip_filter_mbim_type        * mbim_filter_ptr
);


/** @ps_iface_ipfltr_mbimi_alloc

  This function allocates memory for mbim filter

  @dataypes
  alloc_len : Amount of memory to be allocated
  
  @return
  NULL if Memory Allocation failed, otherwise mbim filter ptr with allocated
  memory

  @dependencies
  None.
  
*/
ip_filter_mbim_type * ps_iface_ipfltr_mbimi_alloc
(
  uint8      alloc_len
);

#endif /* PS_IFACE_IPFLTR_MBIMI_H */
