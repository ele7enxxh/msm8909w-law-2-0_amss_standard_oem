#ifndef PS_IFACE_IPFLTR_MBIM_H
#define PS_IFACE_IPFLTR_MBIM_H

/*===========================================================================

                P S _ I F A C E _ I P F L T R _ M B I M. H

DESCRIPTION
  Header file for declaration of functions to use mbim IP filters for inbound
  iface specific processing of IP packets received on that iface.

Copyright (c) 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_iface_ipfltr_mbim.h#1 $
  $DateTime: 2016/12/13 08:00:03 $
 
  when        who    what, where, why
  --------    ---    -------------------------------------------------------- 
===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "ps_iface_defs.h"
#include "ps_ipfltr_defs.h"
#include "ps_pkt_info.h"
#include "list.h"
#include "ps_in.h"

#define PS_IFACE_IPFLTR_MBIM_PACKET_FILTER_MAX_SIZE  (192)

#ifdef __cplusplus
extern "C" {
#endif

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                          REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/** @brief : Filter type associated with mbim filters. Client provides 
             the pattern and mask to be applied to IP packet from offset 
             zero. For example if pattern = 69, mask = 0xFF would be applied
             to first byte of IP packet (i.e it would compare IP version)
*/
typedef struct 
{
  uint8                          len;
  uint8                        * pattern;
  uint8                        * mask;
} ip_filter_mbim_type;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNAL FUNCTION DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/** @ps_iface_ipfltr_mbim_execute

  This function executes all the mbim filters on the specified iface.
  Currently filters are added and executed on rm iface only.

  @dataypes
  iface_ptr:          Iface to pick filters from for execution
  client_id:          Filtering client id    
  dsm_item_ptr:       Ptr to IP pkt to apply filter on

  @return
  PS_IFACE_IPFLTR_NOMATCH : Filter dont match

  @dependencies
  None.
*/
uint32 ps_iface_ipfltr_mbim_execute
(
  ps_iface_type                       * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type   client_id,
  dsm_item_type                       * dsm_item_ptr
);


/** @ps_iface_ipfltr_mbim_get_filter

  This function get filter rule associated with the passed handle

  @dataypes
  iface_ptr : iface ptr
  handle   : rule handle
  p_buf    : pointer to bytes array in sequence of len/pattern/len/mask

  @return
  NULL : No filter found
  ip_filter_mbim_type : mbim filter found

  @dependencies
  None.
*/
ip_filter_mbim_type * ps_iface_ipfltr_mbim_get_filter
(
  ps_iface_type       * iface_ptr,
  uint32                handle
);

#ifdef __cplusplus
}
#endif

#endif /* PS_IFACE_IPFLTR_MBIM_H */