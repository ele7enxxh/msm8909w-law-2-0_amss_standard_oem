#ifndef PS_IFACE_IPFLTR_HW_H
#define PS_IFACE_IPFLTR_HW_H

/*===========================================================================

                  P S _ I F A C E _ I P F L T R _ H W. H

DESCRIPTION
  Header file for declaration of functions to add/delete fitlers to 
  Hardware

Copyright (c) 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_iface_ipfltr_hw.h#1 $
  $DateTime: 2016/12/13 08:00:03 $
 
  when        who    what, where, why
  --------    ---    -------------------------------------------------------- 
===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "ps_iface_defs.h"
#include "ps_iface_ipfltr.h"
#include "ps_ipfltr_defs.h"



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                          REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


#ifdef __cplusplus
extern "C" {
#endif
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNAL FUNCTION DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/** @ps_iface_ipfltr_hw_calculate_v6_addr_mask

  This function calculates v6 address mask given the prefix length

  @dataypes
  ipa_mask       : Pointer to array of mask
  prefix_len     : Prefix length of ip address
  
  @return

  @dependencies
  None.
*/
#define PS_IFACE_IPFLTR_HW_CALCULATE_V6_ADDR_MASK  ps_iface_ipfltr_hw_calculate_v6_addr_mask
INLINE void ps_iface_ipfltr_hw_calculate_v6_addr_mask
(
  uint8                     ipa_mask[16],
  uint8                     prefix_len
)
{
  uint8                   offset = 0;
  uint8                   remainder = 0;
  uint8                   num = 0;
  uint8                   index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------
    Reset the last bits/bytes based of prefix length provided.
    1. Check if prefix length is provided
    2. Reset bytes based on prefix length & remainder. Eg if prefix length 
       is 100 then 100/8 = 12. So reset bytes 13, 14, 15 to 0
    3. For the byte that requires only few bit reset, rest the last
       x bits. For eg 100/8 = 12, So byte 12 needs few bits to be 
       reset. 100%8 = 4. So reset last four bits of 12th byte.
  -----------------------------------------------------------------*/ 
  offset = prefix_len/8;
  remainder = prefix_len % 8;

  for (index = (remainder?(offset+1):offset); index < 16; index++)
  {
    ipa_mask[index] = 0;
  }
  
  if (remainder)
  {
    while (num < (8 - remainder))
    {
      ipa_mask[offset] &= ~(1 << num);
      num++;
    }
  }
} /* ps_iface_ipfltr_hw_calculate_v6_addr_mask() */

/** @ps_iface_ipfltr_hw_add

  This function installs filters to hardware

  @dataypes
  iface_ptr       : Pointer to iface
  client_id       : Client id of the filter
  filter_buf_ptr  : Pointer to filter buffer
  
  @return
   0            : on success
   -1           : on failure

  @dependencies
  None.
*/
int32 ps_iface_ipfltr_hw_add
(
  ps_iface_type                         * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type     client_id,
  ps_iface_ipfilteri_type               * filter_buf_ptr
);

/** @ps_iface_ipfltr_hw_delete

  This function deletes filters from hardware

  @dataypes
  hw_rule_handle  : Hardware handle
  client_id       : Client id of the filter  
  ip_vsn          : IP version of the filter
  
  @return
   None

  @dependencies
  None.
*/
void ps_iface_ipfltr_hw_delete
(
  ps_iface_ipfltr_hw_rule_handle_type    hw_rule_handle,
  ps_iface_ipfltr_client_id_enum_type    client_id,
  ip_version_enum_type                   ip_vsn  
);


/** @ps_iface_ipfltr_hw_add_all_iface_filters

  This function adds all filters from software to hardware

  @dataypes
  iface_ptr       : Pointer to iface
  client_id       : Client id of the filter  
  
  @return
  None

  @dependencies
  None.
*/
void ps_iface_ipfltr_hw_add_all_iface_filters
(
  ps_iface_type                         * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type     client_id
);

/** @ps_iface_ipfltr_hw_delete_all_iface_filters

  This function deletes all filters from software to hardware

  @dataypes
  iface_ptr       : Pointer to iface
  client_id       : Client id of the filter  
  
  @return
  None

  @dependencies
  None.
*/
void ps_iface_ipfltr_hw_delete_all_iface_filters
(
  ps_iface_type                         * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type     client_id
);

#ifdef FEATURE_DATA_PS_464XLAT
/** @ps_iface_ipfltr_hw_add_clat_v6_dst_fltr

  This function adds v6 clat destination filter to hardware

  @dataypes
  iface_ptr       : Pointer to iface
  v6_addr_ptr     : IPv6 address pointer 
  prefix_len      : Prefix length
  
  @return
  Filter handle : Success
  NULL          : Failure

  @dependencies
  None.
*/
ps_iface_ipfltr_hw_rule_handle_type ps_iface_ipfltr_hw_add_clat_v6_dst_fltr
(
  ps_iface_type          * iface_ptr,
  struct ps_in6_addr     * v6_addr_ptr,
  uint8                    prefix_len
);
#endif /* FEATURE_DATA_PS_464XLAT */

/** @ps_iface_ipfltr_add_v6_dst_fltr

  This function adds v6 destination filter to hardware

  @dataypes
  iface_ptr       : Pointer to iface
  v6_addr_ptr     : IPv6 address pointer 
  
  @return
  Filter handle : Success
  NULL          : Failure

  @dependencies
  None.
*/
ps_iface_ipfltr_hw_rule_handle_type ps_iface_ipfltr_hw_add_v6_dst_fltr
(
  ps_iface_type          * iface_ptr,
  struct ps_in6_addr     * v6_addr_ptr
);


/** @ps_iface_ipfltr_hw_delete_v6_dst_fltr

  This function deletes v6 destination filter to hardware

  @dataypes
  v6_dst_fltr_handle_ptr       : Filter handle
  
  @return
  None
  
  @dependencies
  None.
*/
void ps_iface_ipfltr_hw_delete_v6_dst_fltr
(
  ps_iface_ipfltr_hw_rule_handle_type  v6_dst_fltr_handle_ptr
);

/** @ps_iface_ipfltr_hw_add_v6_frag_filter

  This function adds v6 fragmentation filter to hardware

  @dataypes
  v6_addr_ptr     : IPv6 address pointer 
  iface_ptr       : Pointer to iface
  
  @return
  Filter handle : Success
  NULL          : Failure

  @dependencies
  None.
*/
ps_iface_ipfltr_hw_rule_handle_type  ps_iface_ipfltr_hw_add_v6_frag_filter
(
  struct ps_in6_addr  * v6_addr_ptr,
  ps_iface_type          * iface_ptr
);

/** @ps_iface_ipfltr_hw_init

  This function installs filter when UE is loaded

  @dataypes
  None
  
  @return
  None

  @dependencies
  None.
*/
void ps_iface_ipfltr_hw_init
(
  void
);

/** @ps_iface_ipfltr_hw_add_ul_prefix_comp_fltr

  This function deletes v6 prefix rule handle from the hardware

  @dataypes
  iface_ptr       : Hardware rule handle
  stream_id       : Stream Id
  hw_fltr_result  : Hardware filter result
  
  @return
  Hardware filter handle on success
  NULL                   on failure

  @dependencies
  None.
*/
ps_iface_ipfltr_hw_rule_handle_type ps_iface_ipfltr_hw_add_ul_prefix_comp_fltr
(
  ps_iface_type     * iface_ptr,
  uint32              stream_id,
  uint32              hw_fltr_result
);

/** @ps_iface_ipfltr_hw_delete_v6_prefix_hndl

  This function deletes v6 prefix rule handle from the hardware

  @dataypes
  hw_rule_handle : Hardware rule handle
  
  @return
  None

  @dependencies
  None.
*/
void ps_iface_ipfltr_hw_delete_v6_prefix_hndl
(
  ps_iface_ipfltr_hw_rule_handle_type  hw_rule_handle
);

#ifdef __cplusplus
}
#endif

#endif /* PS_IFACE_IPFLTR_HW_H */
