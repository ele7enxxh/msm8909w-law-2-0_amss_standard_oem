#ifndef PS_RT_META_INFO_H
#define PS_RT_META_INFO_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ R T _ M E T A _ I N F O . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
   @file ps_rt_meta_info.h
   @brief This is a header file that contains the definition of the ROUTING META
  INFORMATION that is passed down from the sockets layer all the way down
  to the interface.
 */

/*===========================================================================
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
===========================================================================*/
/*===========================================================================
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
Copyright (c) 2007-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_meta_info.h_v   1.2
  12 Feb 2003 20:35:38   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_rt_meta_info.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/06/11    sd     (Tech Pubs) Edited comments.
12/01/10    sd     (Tech Pubs) Edited Doxygen markup and comments.
12/01/10    sa     Modified the comments according to doxygen specific 
                   semantics.
08/26/10    pp     Added PS_EXT_RT_META_INFO_GET API.
03/26/09    pp     CMI De-featurization.
12/14/08    pp     Common Modem Interface: Public/Private API split.
10/08/08    pp     Metainfo fixes.
12/28/07    pp     Created file.
===========================================================================*/
/* group: ipUtilities */

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "msg.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "ps_pkt_info.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifndef __cplusplus
#include <stringl.h>
#endif /* __cplusplus */


#include "ps_mem_ext.h"
#include "ps_iface_ipfltr.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            DEFINES AND TYPEDEFS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/** @addtogroup macros_ipUtilities
  @{
*/

/**
  Aids in internal debugging. Current level: MSG LOW.
*/
#define RT_META_INFO_DEBUG( sf, a)                                \
                      MSG_1( MSG_SSID_DS, MSG_LEGACY_LOW, sf, a)
/** @} */ /* end_addtogroup macros_ipUtilities */

/** @addtogroup datatypes
  @{
*/  
/*---------------------------------------------------------------------------
TYPEDEF IP_ADDR_SCOPE_ENUM_TYPE
---------------------------------------------------------------------------*/
/**
  Defines the scope of a given IP address.
*/
typedef enum
{
  IP_ADDR_UNICAST      = 0, /**< Unicast scope. */
  IP_ADDR_MULTICAST    = 1, /**< Multicast scope. */
  IP_ADDR_BROADCAST    = 2, /**< Broadcast scope. */
  /** @cond
  */
  IP_ADDR_INVALID_SCOPE     /**< Internal use only. */
  /** @endcond */
} ip_addr_scope_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF ps_rt_meta_info_type
---------------------------------------------------------------------------*/
/**
  @brief Passes routing meta information through the stack. This structure 
  moves with the DSM item carrying the actual IP packet.

  This meta information is used in the Tx path.
*/
typedef struct
{
  ps_ipsec_info_type                ipsec_info;
    /**< IP security related information. */
  ip_pkt_info_type                  pkt_info;
    /**< IP packet information containing header and miscellaneous 
         information. */
  uint32                            fi_mask;
    /**< Maximum number of output filter clients supported. */
#define IP_FLTR_CLIENT_OUTPUT_MAX ((uint32)IP_FLTR_CLIENT_MAX - (uint32)IP_FLTR_CLIENT_INPUT_MAX)
    /**< @relates ps_rt_meta_info_type
         Indicates whether a particular filter client has already been executed. */
  ps_iface_ipfltr_result_type       fi_result[IP_FLTR_CLIENT_OUTPUT_MAX];
    /**< Filter result for each output client. */
  ps_iface_ipfltr_subset_id_type    subset_id;
    /**< Unique ID for each filter set. */
  ps_ip_addr_type                   next_hop_addr;
    /**< IP address of the next hop. */
  ip_addr_scope_enum_type           ip_addr_scope;
    /**< Unicast, multicast, or broadcast scope. */
  void                            * routing_cache;
    /**< Pointer to the cached routing result. */
} ps_rt_meta_info_type;

/** @} */ /* end_addtogroup datatypes */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            ROUTING META INFO MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO PS_RT_META_INFO_FREE()
===========================================================================*/
/**
  Frees the PS memory buffer used for the routing meta information.

  @param[in] rt_mi_ptr_ptr  Pointer to the #ps_rt_meta_info_type pointer.

  @return
  The rt_mi_ptr_ptr pointer is set to NULL.

  @dependencies
  None.
*/
#define PS_RT_META_INFO_FREE ps_rt_meta_info_free
#ifdef __cplusplus
extern "C" {
#endif
/** @} */ /* end_addtogroup macros_ipUtilities */
/*===========================================================================
PS_RT_META_INFO_FREE()
===========================================================================*/
/** @ingroup ps_rt_meta_info_free
  Frees the RT meta information pointer.

  @param[in] rt_mi_ptr_ptr   Pointer to be freed.
  
  @return
  None.

  @dependencies
  None.
*/
INLINE void ps_rt_meta_info_free
(
  ps_rt_meta_info_type ** rt_mi_ptr_ptr
)
{
  if(rt_mi_ptr_ptr != NULL && *rt_mi_ptr_ptr != NULL)
  {
    RT_META_INFO_DEBUG( "RT_META_INFO_FREE ptr 0x%p", *rt_mi_ptr_ptr);
    PS_MEM_FREE(*rt_mi_ptr_ptr);
  }
}
#ifdef __cplusplus
}
#endif
/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO PS_RT_META_INFO_DUP()
===========================================================================*/
/**
  Duplicates the passed PS memory buffer used for the routing meta information.

  @param[in] rt_mi_ptr_ptr   Pointer to #ps_rt_meta_info_type.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  Allocates new PS memory buffers for routing meta information.
*/
#define PS_RT_META_INFO_DUP ps_rt_meta_info_dup
#ifdef __cplusplus
extern "C" {
#endif
/** @} */ /* end_addtogroup macros_ipUtilities */
/*===========================================================================
PS_RT_META_INFO_DUP()
===========================================================================*/
/** @ingroup ps_rt_meta_info_dup
  Duplicates the retransmit meta information pointer.

  @param[in] rt_mi_ptr    Retransmit meta information pointer.
  @param[out] dup_ptr_ptr Duplicate for RT meta information pointer.
  
  @return
  None.

  @dependencies
  None.
*/
INLINE void ps_rt_meta_info_dup
(
  ps_rt_meta_info_type * rt_mi_ptr,
  ps_rt_meta_info_type ** dup_ptr_ptr
)
{
  if(rt_mi_ptr != NULL && dup_ptr_ptr != NULL)
  {
    RT_META_INFO_DEBUG( "RT_META_INFO_DUP ptr 0x%p", rt_mi_ptr);
    *dup_ptr_ptr = (ps_rt_meta_info_type *) ps_mem_dup(rt_mi_ptr);
  }
}
#ifdef __cplusplus
}
#endif

/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO PS_RT_META_INFO_IS_NULL()
===========================================================================*/
/**
  Checks the routing meta information for a NULL value.

  @param[in] rt_mi_ptr Pointer to ps_rt_meta_info_type.

  @return 
  TRUE  -- Value is NULL.\n
  FALSE -- Value is not NULL.

  @dependencies
  None.
*/
#define PS_RT_META_INFO_IS_NULL(rt_mi_ptr) ((rt_mi_ptr) == NULL)

/*===========================================================================
MACRO PS_RT_META_GET_PKT_INFO()
===========================================================================*/
/**
  Gets the pointer to the packet information stored inside the routing meta 
  information.

  @param[in] rt_mi_ptr   Pointer to ps_rt_meta_info_type.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_GET_PKT_INFO(rt_mi_ptr)  (rt_mi_ptr)->pkt_info

/*===========================================================================
MACRO PS_RT_META_SET_PKT_INFO()
===========================================================================*/
/**
  Sets the packet information inside the routing meta information to 
  mi_pkt_info.

  @param[in] rt_mi_ptr    Pointer to ps_rt_meta_info_type.
  @param[in] mi_pkt_info  Meta packet information pointer.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_SET_PKT_INFO(rt_mi_ptr,mi_pkt_info)                    \
  memscpy(&((rt_mi_ptr)->pkt_info), sizeof(ip_pkt_info_type),             \
          &(mi_pkt_info), sizeof(mi_pkt_info))

/*===========================================================================
MACRO PS_RT_META_IS_PKT_INFO_VALID()
===========================================================================*/
/**
  Indicates whether the packet information inside the routing meta information 
  is valid.

  @param[in] rt_mi_ptr   Pointer to ps_rt_meta_info_type.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_IS_PKT_INFO_VALID(rt_mi_ptr)                    \
  (rt_mi_ptr)->pkt_info.is_pkt_info_valid
  

/*===========================================================================
MACRO PS_RT_META_INVALIDATE_PKT_INFO()
===========================================================================*/
/**
  Invalidates the packet information inside the routing meta information.

  @param[in] rt_mi_ptr   Pointer to ps_rt_meta_info_type.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_INVALIDATE_PKT_INFO(rt_mi_ptr)                    \
  (rt_mi_ptr)->pkt_info.is_pkt_info_valid = FALSE

/*===========================================================================
MACRO GET_INDEX_FROM_CLIENT_ID()
===========================================================================*/
/**
  Gets the fi_result array index from the client ID passed. This macro is used 
  to access a fi_result array and set the bit in the filter mask.

  @note1hang\n
  Output client IDs are defined between IP_FLTRR_CLIENT_INPUT_MAX 
  and IP_FLTR_CLIENT_MAX in #ps_iface_ipfltr_client_id_enum_type.\n
  To get the correct index from the client ID, the following rule 
  is applied:\n
  @code (index) = (client_id) - IP_FLTR_CLIENT_INPUT_MAX @endcode

  @param[in] client_id   Client ID for the IP output filter.

  @return
  None.

  @dependencies
  The client ID must be valid, and it must be the output client ID.
*/
#define GET_INDEX_FROM_CLIENT_ID(client_id)                         \
  ((uint8)(client_id) - (uint8)IP_FLTR_CLIENT_INPUT_MAX)

/*===========================================================================
MACRO PS_RT_META_IS_FILTER_MASK_SET()
===========================================================================*/
/**
  Checks whether the filter mask associated with a particular client ID is set.

  @param[in] rt_mi_ptr  Pointer to ps_rt_meta_info_type.
  @param[in] client_id  IP filter client ID.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_IS_FILTER_MASK_SET(rt_mi_ptr, client_id)                    \
  ((rt_mi_ptr)->fi_mask & ((uint32) 1 << (GET_INDEX_FROM_CLIENT_ID(client_id))))

/*===========================================================================
MACRO PS_RT_META_GET_FILTER_MASK()
===========================================================================*/
/**
  Gets the entire filter mask associated with the meta information.

  @param[in] rt_mi_ptr   Pointer to ps_rt_meta_info_type.

  @return
  Filter Mask.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_GET_FILTER_MASK(rt_mi_ptr) (rt_mi_ptr)->fi_mask

/*===========================================================================
MACRO PS_RT_META_GET_FILTER_RESULT()
===========================================================================*/
/**
  Gets the filter result associated with a particular client ID. The result is
  NULL when the particular filter has not been executed or failed the match on 
  the packet.

  @param[in] rt_mi_ptr  Pointer to a ps_rt_meta_info_type pointer.
  @param[in] client_id  IP filter client ID.

  @return
  Filter Result.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_GET_FILTER_RESULT(rt_mi_ptr, client_id)            \
  (((rt_mi_ptr)->fi_mask & ((uint32) 1 << (GET_INDEX_FROM_CLIENT_ID(client_id)))) \
     ? (rt_mi_ptr)->fi_result[(GET_INDEX_FROM_CLIENT_ID(client_id))]  \
     : PS_IFACE_IPFLTR_NOMATCH)

/*===========================================================================
MACRO PS_RT_META_SET_FILTER_RESULT()
===========================================================================*/
/**
  Sets the filter result associated with a particular client ID. The 
  corresponding filter mask bit is also set to indicate there is no need
  to execute filters for the client ID because a resulting fi_result is already
  available.

  @param[in] rt_mi_ptr    Pointer to a #ps_rt_meta_info_type pointer.
  @param[in] client_id    IP filter client ID.
  @param[in] in_fi_result Filter result to set (of the ps_iface_ipfltr_result_type type).

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_SET_FILTER_RESULT(rt_mi_ptr, client_id, in_fi_result) \
  (rt_mi_ptr)->fi_result[(GET_INDEX_FROM_CLIENT_ID(client_id))] =    \
    (ps_iface_ipfltr_result_type)(in_fi_result);                         \
  (rt_mi_ptr)->fi_mask |= ((uint32) 1 << (GET_INDEX_FROM_CLIENT_ID(client_id)));

/*===========================================================================
MACRO PS_RT_META_RESET_FILTER_RESULT()
===========================================================================*/
/**
  Resets the filter result associated with a particular client ID. The 
  corresponding filter mask bit is also reset.

  @param[in] rt_mi_ptr  Pointer to ps_rt_meta_info_type.
  @param[in] client_id  IP filter client ID.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_RESET_FILTER_RESULT(rt_mi_ptr, client_id)       \
  (rt_mi_ptr)->fi_result[(GET_INDEX_FROM_CLIENT_ID(client_id))] =   \
                                               PS_IFACE_IPFLTR_NOMATCH;   \
  (rt_mi_ptr)->fi_mask &= ~((uint32) 1 << (GET_INDEX_FROM_CLIENT_ID(client_id)));

/*===========================================================================
MACRO PS_RT_META_GET_SUBSET_ID()
===========================================================================*/
/**
  Gets the subset ID associated with the given routing meta information.

  @param[in] rt_mi_ptr   Pointer to ps_rt_meta_info_type.

  @return
  Subset ID.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_GET_SUBSET_ID(rt_mi_ptr)  (rt_mi_ptr)->subset_id

/*===========================================================================
MACRO PS_RT_META_SET_SUBSET_ID()
===========================================================================*/
/**
  Sets the subset ID for the given meta information.

  @param[in] rt_mi_ptr      Pointer to ps_rt_meta_info_type.
  @param[in] mi_subset_id   Subset ID to be set.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_SET_SUBSET_ID(rt_mi_ptr, mi_subset_id)                     \
  (rt_mi_ptr)->subset_id = (mi_subset_id);

/*===========================================================================
MACRO PS_RT_META_GET_IPSEC_INFO()
===========================================================================*/
/**
  Gets the IP security information associated with the given routing meta 
  information.

  @param[in] rt_mi_ptr   Pointer to ps_rt_meta_info_type.

  @return
  IPS security information.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_GET_IPSEC_INFO(rt_mi_ptr)  (rt_mi_ptr)->ipsec_info

/*===========================================================================
MACRO PS_RT_META_SET_IPSEC_INFO()
===========================================================================*/
/**
  Assigns IP security information to the given routing meta information.

  @param[in] rt_mi_ptr      Pointer to ps_rt_meta_info_type.
  @param[in] mi_ipsec_info  Pointer to the meta information for the IPSec
                            information.

  @return
  Meta information for the IPSec information.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_SET_IPSEC_INFO(rt_mi_ptr, mi_ipsec_info)              \
  memscpy(&((rt_mi_ptr)->ipsec_info), sizeof(ps_ipsec_info_type),        \
          &(mi_ipsec_info), sizeof(mi_ipsec_info))

/*===========================================================================
MACRO PS_RT_META_GET_ROUTING_CACHE()
===========================================================================*/
/**
  Gets the routing cache for the given routing meta information.

  @param[in] rt_mi_ptr   Pointer to ps_rt_meta_info_type.

  @return
  Routing cache.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_GET_ROUTING_CACHE(rt_mi_ptr)  (rt_mi_ptr)->routing_cache

/*===========================================================================
MACRO PS_RT_META_SET_ROUTING_CACHE()
===========================================================================*/
/**
  Sets the routing cache for the given meta information.

  @param[in] rt_mi_ptr         Pointer to ps_rt_meta_info_type.
  @param[in] mi_routing_cache  Routing cache to be set.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_SET_ROUTING_CACHE(rt_mi_ptr, mi_routing_cache)    \
  (rt_mi_ptr)->routing_cache = (mi_routing_cache);

/*===========================================================================
MACRO PS_RT_META_GET_IP_ADDR_SCOPE()
===========================================================================*/
/**
  Gets the IP address scope associated with the given routing meta information.

  @param[in] rt_mi_ptr   Pointer to ps_rt_meta_info_type.

  @return
  IP Address scope.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_GET_IP_ADDR_SCOPE(rt_mi_ptr) (rt_mi_ptr)->ip_addr_scope

/*===========================================================================
MACRO PS_RT_META_SET_IP_ADDR_SCOPE()
===========================================================================*/
/**
  Sets IP address scope for a given routing meta information.

  @param[in] rt_mi_ptr         Pointer to ps_rt_meta_info_type.
  @param[in] mi_ip_addr_scope  IP address scope to be set.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_SET_IP_ADDR_SCOPE(rt_mi_ptr, mi_ip_addr_scope)    \
  (rt_mi_ptr)->ip_addr_scope = (mi_ip_addr_scope);

/*===========================================================================
MACRO PS_RT_META_GET_NEXT_HOP_ADDR()
===========================================================================*/
/**
  Gets the next hop address associated with the given routing meta information.

  @param[in] rt_mi_ptr   Pointer to ps_rt_meta_info_type.

  @return
  Next hop address.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_GET_NEXT_HOP_ADDR(rt_mi_ptr)  (rt_mi_ptr)->next_hop_addr

/*===========================================================================
MACRO PS_RT_META_SET_NEXT_HOP_ADDR()
===========================================================================*/
/**
  Sets the next hop address for the given routing meta information.

  @param[in] rt_mi_ptr         Pointer to ps_tx_meta_info_type.
  @param[in] mi_next_hop_addr  Next hop address to be set.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RT_META_SET_NEXT_HOP_ADDR(rt_mi_ptr, mi_next_hop_addr)       \
  (rt_mi_ptr)->next_hop_addr = (mi_next_hop_addr);

/*===========================================================================
MACRO PS_EXT_RT_META_INFO_GET()
===========================================================================*/
/**
  Allocates a PS memory buffer used for the routing meta information.
  This macro is used by external clients only.

  @param[in] rt_mi_ptr      Pointer to ps_tx_meta_info_type that is to have
                            the ps_rt_meta_info_type memory assigned to it.

  @return
  The rt_mi_ptr pointer is NULL on failure or points to the 
  #ps_rt_meta_info_type data.

  @dependencies
  None.
*/
#define PS_EXT_RT_META_INFO_GET(rt_mi_ptr)                                  \
  (rt_mi_ptr) = (ps_rt_meta_info_type *) ps_ext_mem_get_buf(PS_EXT_MEM_RT_META_INFO_TYPE); \
  if ((rt_mi_ptr) != NULL)                                                 \
  {                                                                        \
    memset((rt_mi_ptr), 0, sizeof(ps_rt_meta_info_type));                  \
  }

/** @} */  /* end_addtogroup macros_ipUtilities */

#endif /* PS_RT_META_INFO_H */
