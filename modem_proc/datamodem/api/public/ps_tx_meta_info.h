#ifndef PS_TX_META_INFO_H
#define PS_TX_META_INFO_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                         P S _ T X _ M E T A _ I N F O . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*======*/

/**
   @file ps_tx_meta_info.h
   @brief This is a header file that contains the definition of the transmit (Tx) 
  meta information that is passed down from the sockets layer all the way
  down to the interface.
 */

/*
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
*/
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
/*=============================================================================
Copyright (c) 2008-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/PS_TX_META_info.h_v   1.2
  12 Feb 2003 20:35:38   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_tx_meta_info.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ------------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
12/01/10    sd     (Tech Pubs) Edited Doxygen markup and comments.
12/01/10    sa     Modified the comments according to doxygen specific 
                   semantics.
08/26/10    pp     Added PS_EXT_TX_META_INFO_GET_ALL.
06/21/10    pp     PS_RT_META_INVALIDATE_PKT_INFO typo correction.
01/05/10    am     Removed rt_cache copy from Rx -> Tx metainfo copy.
12/14/08    pp     Common Modem Interface: Public/Private API split.
10/03/08    pp     Macro PS_TX_META_INFO_GET_ALL introduced.
03/06/08    pp     Created file.
=============================================================================*/
/* group: ipUtilities */

/*=============================================================================

                          INCLUDE FILES FOR MODULE

=============================================================================*/
#include "amssassert.h"
#include "comdef.h"
#include "ps_pkt_meta_info.h"
#include "ps_rt_meta_info.h"
#include "ps_rx_meta_info.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            DEFINES AND TYPEDEFS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/** @addtogroup macros_ipUtilities
  @{
*/

/** 
  Aids in internal debugging. Current level: MSG_LOW.
*/
#define TX_META_INFO_DEBUG( sf, a)                                \
                      MSG_1( MSG_SSID_DS, MSG_LEGACY_LOW, sf, a) 
#define TX_META_INFO_DEBUG_2( sf, a, b )                                \
                      MSG_2( MSG_SSID_DS, MSG_LEGACY_LOW, sf, a, b) 
/** @} */ /* end_addtogroup macros_ipUtilities */

/** @addtogroup datatypes
  @{
*/
/*-----------------------------------------------------------------------------
TYPEDEF PS_TX_META_INFO_TYPE
-----------------------------------------------------------------------------*/
/**
  @brief Passes the Tx meta information through the stack. The structure 
  moves with the DSM item carrying the actual IP packet.
*/
typedef struct
{
  ps_pkt_meta_info_type           * pkt_meta_info_ptr;
    /**< Pointer to packet meta information. */
  ps_rt_meta_info_type            * rt_meta_info_ptr;
    /**< Pointer to routing meta information. */
} ps_tx_meta_info_type;
/** @} */ /* end_addtogroup datatypes */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            PS TX META INFO MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/** @addtogroup macros_ipUtilities
  @{
*/
/*=============================================================================
MACRO PS_TX_META_GET_RT_META_INFO_PTR()
=============================================================================*/
/**
  Gets the routing meta information pointer associated with the Tx meta 
  information.

  @param[in] tx_mi_ptr   Pointer to ps_tx_meta_info_type.

  @return
  #rt_meta_info_ptr

  @dependencies
  The tx_mi_ptr parameter value must be valid.
*/
#define PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr)                    \
  (tx_mi_ptr)->rt_meta_info_ptr

/*=============================================================================
MACRO PS_TX_META_GET_PKT_META_INFO_PTR()
=============================================================================*/
/**
  Gets the packet meta information pointer associated with the Tx 
  meta information.

  @param[in] tx_mi_ptr   Pointer to ps_tx_meta_info_type.

  @return
  #pkt_meta_info_ptr

  @dependencies
  The tx_mi_ptr parameter value must be valid.
*/
#define PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr)                   \
  (tx_mi_ptr)->pkt_meta_info_ptr

/*=============================================================================
MACRO PS_TX_META_INFO_FREE()
=============================================================================*/
/**
  Deallocates the PS memory buffer used for the Tx meta information.
  This first deallocates packet meta information, routes meta information 
  buffers inside the Tx meta information and then frees memory 
  associated with the Tx meta information buffer.

  @note1hang This is the single macro to free the entire meta information 
  buffer.

  @param[in] tx_mi_ptr  ps_tx_meta_info_type pointer that is freed.

  @return
  The tx_mi_ptr_ptr parameter value is set to NULL.

  @dependencies
  None.
*/
#define PS_TX_META_INFO_FREE ps_tx_meta_info_free
#ifdef __cplusplus
extern "C" {
#endif
/** @} */ /* end_addtogroup macros_ipUtilities */

/*===========================================================================
PS_TX_META_INFO_FREE()
===========================================================================*/
/** @ingroup ps_tx_meta_info_free
  Frees the Tx meta information pointer.

  @param[in] tx_mi_ptr_ptr   Pointer to be freed.
  
  @return
  None.

  @dependencies
  None.
*/
INLINE void ps_tx_meta_info_free
(
  ps_tx_meta_info_type ** tx_mi_ptr_ptr
)
{
  ps_pkt_meta_info_type * pkt_mi_ptr;
  ps_rt_meta_info_type  * rt_mi_ptr;

  if (tx_mi_ptr_ptr != NULL && *tx_mi_ptr_ptr != NULL)
  {
    pkt_mi_ptr = PS_TX_META_GET_PKT_META_INFO_PTR(*tx_mi_ptr_ptr);
    PS_PKT_META_INFO_FREE(&pkt_mi_ptr);

    rt_mi_ptr  = PS_TX_META_GET_RT_META_INFO_PTR(*tx_mi_ptr_ptr);
    PS_RT_META_INFO_FREE(&rt_mi_ptr);

    TX_META_INFO_DEBUG( "TX_META FREE ptr 0x%p", *tx_mi_ptr_ptr );
    PS_MEM_FREE(*tx_mi_ptr_ptr);
  }
}
#ifdef __cplusplus
}
#endif

/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO PS_TX_META_INFO_DUP()
===========================================================================*/
/**
  Duplicates the passed PS memory buffer used for the Tx meta
  information.

  @param[in] tx_mi_ptr     ps_tx_meta_info_type pointer to be duplicated.
  @param[out] dup_ptr_ptr  Pointer to ps_tx_meta_info_type.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  This macro duplicates routing, packet, and Tx meta information.
*/
#define PS_TX_META_INFO_DUP ps_tx_meta_info_dup
#ifdef __cplusplus
extern "C" {
#endif
/** @} */ /* end_addtogroup macros_ipUtilities */
/*===========================================================================
PS_TX_META_INFO_DUP()
===========================================================================*/
/** @ingroup ps_tx_meta_info_dup
  Duplicates the Tx meta information pointer.

  @param[in] tx_mi_ptr      Tx meta information pointer to be duped.
  @param[out] dup_ptr_ptr   Duplicate for Tx meta information pointer.
  
  @return
  None.

  @dependencies
  None.
*/
INLINE void ps_tx_meta_info_dup
(
  ps_tx_meta_info_type * tx_mi_ptr,
  ps_tx_meta_info_type ** dup_ptr_ptr
)
{
  if (tx_mi_ptr != NULL && dup_ptr_ptr != NULL)
  {
    TX_META_INFO_DEBUG( "PS_TX_META_INFO_DUP ptr 0x%p", tx_mi_ptr);

    *dup_ptr_ptr = (ps_tx_meta_info_type *) ps_mem_dup(tx_mi_ptr);
    PS_PKT_META_INFO_DUP(PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr),
                         &(PS_TX_META_GET_PKT_META_INFO_PTR(*dup_ptr_ptr)));
    PS_RT_META_INFO_DUP(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr),
                        &(PS_TX_META_GET_RT_META_INFO_PTR(*dup_ptr_ptr)));
  }
}
#ifdef __cplusplus
}
#endif

/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO PS_TX_META_INFO_COPY()
===========================================================================*/
/**
  Copies the meta information buffer details into passed copy pointer buffer.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type
  @param[out] copy_ptr  Pointer to ps_tx_meta_info_type (copy buffer).

  @return
  None.

  @dependencies
  Both tx_mi_ptr and copy_ptr must be valid.

  @sideeffects
  Copies the routing and packet meta information buffers from tx_mi_ptr to 
  copy_ptr buffer.
*/
#define PS_TX_META_INFO_COPY(tx_mi_ptr, copy_ptr)                   \
  {                                                                      \
    if(((tx_mi_ptr) != NULL) && ((copy_ptr) != NULL))                 \
    {                                                                    \
      TX_META_INFO_DEBUG_2( "TX_META COPY ptr f:0x%p t:0x%p ", tx_mi_ptr, copy_ptr);  \
      if((PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr) != NULL) &&            \
         (PS_TX_META_GET_RT_META_INFO_PTR(copy_ptr) != NULL))            \
      {                                                                  \
        memscpy(PS_TX_META_GET_RT_META_INFO_PTR(copy_ptr),             \
                sizeof(ps_rt_meta_info_type),                             \
                PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr),               \
                sizeof(ps_rt_meta_info_type));                           \
      }                                                                  \
      if((PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr) != NULL) &&          \
         (PS_TX_META_GET_PKT_META_INFO_PTR(copy_ptr) != NULL))          \
      {                                                                  \
        memscpy(PS_TX_META_GET_PKT_META_INFO_PTR(copy_ptr),             \
                sizeof(ps_pkt_meta_info_type),                             \
                PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr),               \
                sizeof(ps_pkt_meta_info_type));                           \
      }                                                                  \
    }                                                                    \
  }

/*===========================================================================
MACRO PS_TX_META_INFO_IS_NULL()
===========================================================================*/
/**
  NULL check macros.

  @param[in] tx_mi_ptr Pointer to ps_tx_meta_info_type.

  @return
  TRUE  -- Tx meta information pointer is NULL.\n
  FALSE -- Otherwise.

  @dependencies
  None.
*/
#define PS_TX_META_INFO_IS_NULL(tx_mi_ptr)                           \
   ((tx_mi_ptr == NULL))

/*===========================================================================
MACRO PS_TX_AND_RT_META_INFO_IS_NULL()
===========================================================================*/
/**
  NULL check macros.
  
  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type.

  @return
  TRUE  -- Tx and RT meta information pointers are NULL.\n
  FALSE -- Otherwise.

  @dependencies
  None.
*/
#define PS_TX_AND_RT_META_INFO_IS_NULL(tx_mi_ptr)                           \
      ((tx_mi_ptr == NULL) ||                                           \
    (PS_RT_META_INFO_IS_NULL(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))))

/*===========================================================================
MACRO PS_TX_META_INFO_ALL_IS_NULL()
===========================================================================*/
/**
  NULL check macros.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type.

  @return
  TRUE  -- Tx, RT, and packet meta information pointers are NULL.\n
  FALSE -- Otherwise.

  @dependencies
  None.
*/
#define PS_TX_META_INFO_ALL_IS_NULL(tx_mi_ptr)                           \
   ((tx_mi_ptr == NULL) ||                                           \
    (PS_RT_META_INFO_IS_NULL(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))) || \
    (PS_PKT_META_INFO_IS_NULL(PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr))))

/*===========================================================================
MACRO PS_TX_META_GET_PKT_INFO()
===========================================================================*/
/**
  Gets the pointer to the packet information stored inside the Tx 
  meta information.

  @param[in] tx_mi_ptr     Pointer to ps_tx_meta_info_type.

  @return
  #pkt_info

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_GET_PKT_INFO(tx_mi_ptr)                                    \
  PS_RT_META_GET_PKT_INFO(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_SET_PKT_INFO()
===========================================================================*/
/**
  Sets the packet information to the packet information stored inside the 
  Tx meta information.

  @param[in] tx_mi_ptr    Pointer to ps_tx_meta_info_type.
  @param[in] mi_pkt_info  Packet information to be copied.

  @return
  None.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_SET_PKT_INFO(tx_mi_ptr, mi_pkt_info)                      \
  PS_RT_META_SET_PKT_INFO(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr), mi_pkt_info)

/*===========================================================================
MACRO PS_TX_META_IS_PKT_INFO_VALID()
===========================================================================*/
/**
  Indicates whether the packet information inside the Tx meta 
  information is valid. 

  @param[in] tx_mi_ptr Pointer to ps_tx_meta_info_type.

  @return
  #is_pkt_info_valid

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_IS_PKT_INFO_VALID(tx_mi_ptr)                      \
  PS_RT_META_IS_PKT_INFO_VALID(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_INVALIDATE_PKT_INFO()
===========================================================================*/
/**
  Invalidates the packet information inside the Tx meta information. 
  This is used to regenate pkt_info using ps_pkt_info utility functions.

  @param[in] tx_mi_ptr Pointer to ps_tx_meta_info_type.
  @param[out] is_pkt_info_valid flag.

  @return
  None.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_INVALIDATE_PKT_INFO(tx_mi_ptr)                      \
  PS_RT_META_INVALIDATE_PKT_INFO(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_GET_FILTER_MASK()
===========================================================================*/
/**
  Gets the entire filter mask associated with the meta info.

  @param[in] tx_mi_ptr Pointer to ps_tx_meta_info_type.

  @return
  #fi_result_mask

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_GET_FILTER_MASK(tx_mi_ptr)                          \
  PS_RT_META_GET_FILTER_MASK(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_IS_FILTER_MASK_SET()
===========================================================================*/
/**
  Checks whether the filter mask associated with a particular client ID is set.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type.
  @param[in] client_id  IP filter client ID.

  @return
  TRUE  -- Filter mask set.\n
  FALSE -- Otherwise.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_IS_FILTER_MASK_SET(tx_mi_ptr, client_id)                    \
  PS_RT_META_IS_FILTER_MASK_SET(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr), \
                                    client_id)

/*===========================================================================
MACRO PS_TX_META_GET_FILTER_RESULT()
===========================================================================*/
/**
  Gets the filter result associated with a particular client ID. The result is 
  NULL when the particular filter has not been executed or failed the match on 
  the packet.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type.
  @param[in] client_id  IP filter client ID.

  @return
  #fi_result

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_GET_FILTER_RESULT(tx_mi_ptr, client_id)                     \
  PS_RT_META_GET_FILTER_RESULT(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr), \
                                   client_id)

/*===========================================================================
MACRO PS_TX_META_SET_FILTER_RESULT()
===========================================================================*/
/**
  Sets the filter result associated with a particular client ID. The 
  corresponding filter mask bit is also set to indicate there is no need
  to execute filters for the client ID because a resulting fi_result is already
  available.

  @param[in] tx_mi_ptr  Pointer to a ps_tx_meta_info_type.
  @param[in] client_id  IP filter client ID.
  @param[in] fi_result  Filter result to set. See #ps_iface_ipfltr_result_type.

  @return
  None.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_SET_FILTER_RESULT(tx_mi_ptr, client_id, in_fi_result)    \
  PS_RT_META_SET_FILTER_RESULT(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr), \
                                   client_id, in_fi_result)

/*===========================================================================
MACRO PS_TX_META_RESET_FILTER_RESULT()
===========================================================================*/
/**
  Resets the filter result associated with a particular client ID. The 
  corresponding filter mask bit is also reset.

  @param[in] tx_mi_ptr  Pointer to #ps_tx_meta_info_type.
  @param[in] client_id  IP filter client ID.

  @return
  None.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_RESET_FILTER_RESULT(tx_mi_ptr, client_id)                   \
  PS_RT_META_RESET_FILTER_RESULT(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr), \
                                     client_id)

/*===========================================================================
MACRO PS_TX_META_GET_SUBSET_ID()
===========================================================================*/
/**
  Gets the subset ID associated with the given Tx meta information.

  @param[in] tx_mi_ptr  Pointer to #ps_tx_meta_info_type.

  @return
  The subset ID associated with the Tx meta information.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_GET_SUBSET_ID(tx_mi_ptr)                                   \
  PS_RT_META_GET_SUBSET_ID(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_SET_SUBSET_ID()
===========================================================================*/
/**
  Sets the subset ID for the given Tx meta information.

  @param[in] tx_mi_ptr     Pointer to #ps_tx_meta_info_type.
  @param[in] mi_subset_id  Subset ID to be set.

  @return
  None.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_SET_SUBSET_ID( tx_mi_ptr, mi_subset_id)                   \
  PS_RT_META_SET_SUBSET_ID(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr),     \
                              mi_subset_id)

/*===========================================================================
MACRO PS_TX_META_GET_ROUTING_CACHE()
===========================================================================*/
/**
  Gets the routing cache associated with the given Tx meta information.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type.

  @return
  The routing cache associated with the given Tx meta information.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_GET_ROUTING_CACHE(tx_mi_ptr)                          \
  PS_RT_META_GET_ROUTING_CACHE(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_SET_ROUTING_CACHE()
===========================================================================*/
/**
  Sets the routing cache for the given Tx meta information.

  @param[in] tx_mi_ptr          Pointer to ps_tx_meta_info_type.
  @param[in] mi_routing_cache   Routing cache to be set.

  @return
  None.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_SET_ROUTING_CACHE(tx_mi_ptr, mi_routing_cache)             \
  PS_RT_META_SET_ROUTING_CACHE(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr),\
                                  mi_routing_cache)

/*===========================================================================
MACRO PS_TX_META_GET_IPSEC_INFO()
===========================================================================*/
/**
  Gets the IP security information associated with the given Tx meta 
  information.

  @param[in] tx_mi_ptr Pointer to ps_tx_meta_info_type.

  @return
  ipsec_info

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_GET_IPSEC_INFO(tx_mi_ptr)                                 \
  PS_RT_META_GET_IPSEC_INFO(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_SET_IPSEC_INFO()
===========================================================================*/
/**
  Sets the IP security information for the given Tx meta information.

  @param[in] tx_mi_ptr      Pointer to ps_tx_meta_info_type.
  @param[in] mi_ipsec_info  IP security information to be set.

  @return
  None.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_SET_IPSEC_INFO( tx_mi_ptr, mi_ipsec_info)                \
  PS_RT_META_SET_IPSEC_INFO(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr),   \
                               (mi_ipsec_info));

/*===========================================================================
MACRO PS_TX_META_GET_IP_ADDR_SCOPE()
===========================================================================*/
/**
  Gets the IP address scope associated with the given Tx meta information.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type.

  @return
  ip_addr_scope

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_GET_IP_ADDR_SCOPE(tx_mi_ptr)                               \
  PS_RT_META_GET_IP_ADDR_SCOPE(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_SET_IP_ADDR_SCOPE()
===========================================================================*/
/**
  Sets the IP address scope for the given Tx meta information.

  @param[in] tx_mi_ptr         Pointer to ps_tx_meta_info_type.
  @param[in] mi_ip_addr_scope  IP address scope to be set.

  @return
  None.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_SET_IP_ADDR_SCOPE(tx_mi_ptr, mi_ip_addr_scope)             \
  PS_RT_META_SET_IP_ADDR_SCOPE(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr),\
                                  mi_ip_addr_scope)

/*===========================================================================
MACRO PS_TX_META_GET_NEXT_HOP_ADDR()
===========================================================================*/
/**
  Gets the next hop address associated with the given Tx meta 
  information.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type.

  @return
  next_hop_addr

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_GET_NEXT_HOP_ADDR(tx_mi_ptr)                             \
  PS_RT_META_GET_NEXT_HOP_ADDR(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_SET_NEXT_HOP_ADDR()
===========================================================================*/
/**
  Sets the next hop address for the given Tx meta information.

  @param[in] tx_mi_ptr         Pointer to ps_tx_meta_info_type.
  @param[in] mi_next_hop_addr  Next hop address to be set.

  @return
  None.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_SET_NEXT_HOP_ADDR(tx_mi_ptr, mi_next_hop_addr)             \
  PS_RT_META_SET_NEXT_HOP_ADDR(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr),\
                                  mi_next_hop_addr)

/*===========================================================================
MACRO PS_TX_META_GET_TX_FLAGS()
===========================================================================*/
/**
  Gets the Tx flags associated with the given Tx meta 
  information.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type.

  @return
  tx_flags

  @dependencies
  tx_mi_ptr and tx_mi_ptr->pkt_meta_info_ptr must be valid.
*/
#define PS_TX_META_GET_TX_FLAGS(tx_mi_ptr)                                \
  PS_PKT_META_GET_TX_FLAGS(PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_SET_TX_FLAGS()
===========================================================================*/
/**
  Sets the Tx flags for the given Tx meta information.

  @param[in] tx_mi_ptr    Pointer to a ps_tx_meta_info_type.
  @param[in] mi_tx_flags  Tx flags to be set.

  @return
  None.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->pkt_meta_info_ptr must be valid.
*/
#define PS_TX_META_SET_TX_FLAGS(tx_mi_ptr, mi_tx_flags)                       \
  PS_PKT_META_SET_TX_FLAGS(PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr),    \
                              mi_tx_flags);

/*===========================================================================
MACRO PS_TX_META_GET_DOS_ACK_HANDLE()
===========================================================================*/
/**
  Gets dos_ack_handle associated with the given Tx meta information.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type.

  @return
  dos_ack_handle

  @dependencies
  tx_mi_ptr and tx_mi_ptr->pkt_meta_info_ptr must be valid.
*/
#define PS_TX_META_GET_DOS_ACK_HANDLE(tx_mi_ptr)                            \
  PS_PKT_META_GET_DOS_ACK_HANDLE(PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_SET_DOS_ACK_HANDLE()
===========================================================================*/
/**
  Sets the dos ack handle for the given Tx meta information.

  @param[in] tx_mi_ptr          Pointer to a ps_tx_meta_info_type.
  @param[in] mi_dos_ack_handle  dos ack handle to be set.

  @return
  None.

  @dependencies
  tx_mi_ptr and tx_mi_ptr->pkt_meta_info_ptr must be valid.
*/
#define PS_TX_META_SET_DOS_ACK_HANDLE(tx_mi_ptr, mi_dos_ack_handle)           \
  PS_PKT_META_SET_DOS_ACK_HANDLE(PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr), \
                                 mi_dos_ack_handle);

/*===========================================================================
MACRO PS_TX_META_GET_TIMESTAMP()
===========================================================================*/
/**
  Gets the timestamp associated with the Tx meta information.

  @param[in] tx_mi_ptr Pointer to ps_tx_meta_info_type.

  @return
  timestamp

  @dependencies
  tx_mi_ptr and tx_mi_ptr->pkt_meta_info_ptr must be valid.
*/
#define PS_TX_META_GET_TIMESTAMP(tx_mi_ptr)                              \
  PS_PKT_META_GET_TIMESTAMP(PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr))

/*===========================================================================
MACRO PS_TX_META_INFO_COPY_FROM_RX_META_INFO()
===========================================================================*/
/**
  Copies the contents from the Rx meta information buffer into the 
  Tx meta information buffer. The main fields copied are ipsec_info, 
  subset_id, routing_cache, and pkt_info.

  @param[in] rx_mi_ptr  Pointer to ps_rx_meta_info_type.
  @param[out] tx_mi_ptr  Pointer to ps_tx_meta_info_type.

  @return
  The tx_mi_ptr pointer is NULL on failure or points to the meta_info data.

  @dependencies
  rx_tx_mi_ptr, tx_mi_ptr, and tx_mi_ptr->rt_meta_info_ptr must be valid.
*/
#define PS_TX_META_INFO_COPY_FROM_RX_META_INFO(rx_mi_ptr, tx_mi_ptr ) \
  if (((tx_mi_ptr) != NULL) &&                                             \
       (PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr) != NULL) && \
      (rx_mi_ptr) != NULL)                                         \
  {                                                                 \
    ps_rt_meta_info_type *rt_mi_ptr = PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr); \
    TX_META_INFO_DEBUG( "TX_META COPY ptr 0x%p", tx_mi_ptr );\
    /* Copy the Rx meta information fields to the Tx meta information here. */              \
    PS_RT_META_SET_IPSEC_INFO(rt_mi_ptr,                          \
                                 PS_RX_META_GET_IPSEC_INFO(rx_mi_ptr)); \
    PS_RT_META_SET_PKT_INFO(rt_mi_ptr,                             \
                               PS_RX_META_GET_PKT_INFO(rx_mi_ptr)); \
  }

/*===========================================================================
MACRO PS_TX_META_SET_PKT_META_INFO_PTR()
===========================================================================*/
/**
  Attaches a packet meta information pointer to the given Tx meta 
  information.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type.
  @param[in] pkt_mi_ptr Packet meta information pointer to set.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  tx_mi_ptr->pkt_meta_info_ptr is updated.
*/
#define PS_TX_META_SET_PKT_META_INFO_PTR(tx_mi_ptr, pkt_mi_ptr)  \
  (tx_mi_ptr)->pkt_meta_info_ptr = (pkt_mi_ptr);

/*===========================================================================
MACRO PS_TX_META_SET_RT_META_INFO_PTR()
===========================================================================*/
/**
  Attaches a routing meta information pointer to the given Tx meta 
  information.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type.
  @param[in] rt_mi_ptr  Routing meta information pointer to set.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  tx_mi_ptr->pkt_meta_info_ptr is updated.
*/
#define PS_TX_META_SET_RT_META_INFO_PTR(tx_mi_ptr, rt_mi_ptr)  \
  (tx_mi_ptr)->rt_meta_info_ptr = (rt_mi_ptr);

/*===========================================================================
MACRO PS_EXT_TX_META_INFO_GET_ALL()
===========================================================================*/
/**
  Allocates a PS memory buffer used for the Tx meta information.
  This macro is used by external clients. This macro also allocates the routing
  and packet meta information buffers within the Tx meta information.

  @param[in] tx_mi_ptr  Pointer to ps_tx_meta_info_type that is be have
                        the ps_tx_meta_info memory assigned to it.

  @return
  The tx_mi_ptr pointer is NULL on failure or points to the 
  #ps_tx_meta_info_type buffer.\n
  tx_mi_ptr->rt_meta_info_ptr is NULL on failure or points to the
  rt_meta_info_type buffer.\n
  tx_mi_ptr->pkt_meta_info_ptr is NULL on failure or points to the
  #pkt_meta_info_type buffer.

  @dependencies
  None.

  @sideeffects
  Initializes the allocated meta information block to all zeros. This creates
  all the meta information buffers within the Tx meta information.
*/
#define PS_EXT_TX_META_INFO_GET_ALL(tx_mi_ptr)               \
  (tx_mi_ptr) = (ps_tx_meta_info_type *)ps_ext_mem_get_buf(PS_EXT_MEM_TX_META_INFO_TYPE); \
  if ( (tx_mi_ptr) != NULL )                                              \
  {                                                                         \
    TX_META_INFO_DEBUG( "TX_META EXT GET ptr 0x%p", tx_mi_ptr);   \
    memset( (tx_mi_ptr), 0, sizeof(ps_tx_meta_info_type) );               \
    /* Create ROUTING meta information */                                  \
    PS_EXT_RT_META_INFO_GET(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr)); \
    /* Create PACKET meta information */                                  \
    PS_EXT_PKT_META_INFO_GET(PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr)); \
  }

/** @} */  /* end_addtogroup macros_ipUtilities */

#endif /* PS_TX_META_INFO_H */
