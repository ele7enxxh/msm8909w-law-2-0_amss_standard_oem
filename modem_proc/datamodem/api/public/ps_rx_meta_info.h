#ifndef PS_RX_META_INFO_H
#define PS_RX_META_INFO_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ R X _ M E T A _ I N F O . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
   @file ps_rx_meta_info.h
   @brief This is a header file that contains the definition of the Rx meta information
  that is passed up from the Mode handler/PPP layer to the IP layer.
 */

/*
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
*/
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
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_rx_meta_info.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/06/11    sd     (Tech Pubs) Edited comments.
12/01/10    sd     (Tech Pubs) Edited Doxygen markup and comments.
12/01/10    sa     Modified the comments according to doxygen specific 
                   semantics.
03/26/09    pp     CMI De-featurization.
12/14/08    pp     Common Modem Interface: Public/Private API split.
12/28/07    pp     Created file.
===========================================================================*/

/* group: ipUtilities */

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "time_svc.h"
#include "qw.h"
#include "msg.h"

#include "ps_mem_ext.h"
#include "ps_iface_ipfltr.h"
#include "ps_acl.h"
#include "ps_in.h"
#include "ps_pkt_info.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            DEFINES AND TYPEDEFS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/** @addtogroup macros_ipUtilities
  @{
*/

/**
  Aids in internal debugging. Current level: MSG LOW.
*/
#define RX_META_INFO_DEBUG( sf, a)                                \
                       MSG_1( MSG_SSID_DS, MSG_LEGACY_LOW, sf, a)
/** @} */ /* end_addtogroup macros_ipUtilities */

/** @addtogroup datatypes
  @{
*/
/*---------------------------------------------------------------------------
TYPEDEF PS_RX_META_INFO_TYPE
---------------------------------------------------------------------------*/
/**
  @brief Passes per-packet meta information through the stack. This structure 
  moves with the DSM item carrying the actual IP packet in the Rx 
  path.
*/
typedef struct
{
  ps_ipsec_info_type                ipsec_info;
    /**< IP security information. */
  ip_pkt_info_type                  pkt_info;
    /**< IP packet information containing header and miscellaneous information. */
  uint32                            fi_mask;
    /**< Indicates whether a particular client filter is already executed. */
  ps_iface_ipfltr_result_type       fi_result[IP_FLTR_CLIENT_INPUT_MAX];
    /**< Filter result for each client. */
  void                            * routing_cache;
    /**< Cached routing result. */
  uint8                             is_pkt_logged;
    /**< Is packet already logged by HW*/
} ps_rx_meta_info_type;
/** @} */
/** @} */ /* end_addtogroup datatypes */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            RX META INFO MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO PS_RX_META_INFO_FREE()
===========================================================================*/
/**
  Frees the PS memory buffer used for the Rx meta information.

  @param[in] rx_mi_ptr_ptr  Pointer to ps_rx_meta_info_type.

  @return
  The rx_mi_ptr_ptr pointer is set to Null.

  @dependencies
  None.
*/
#define PS_RX_META_INFO_FREE ps_rx_meta_info_free
#ifdef __cplusplus
extern "C" {
#endif
/** @} */ /* end_addtogroup macros_ipUtilities */

/*===========================================================================
PS_RX_META_INFO_FREE()
===========================================================================*/
/** @ingroup ps_rx_meta_info_free
  Frees the Rx meta information pointer.

  @param[in] rx_mi_ptr_ptr  RX meta information pointer to be freed.
    
  @return
  None.

  @dependencies
  None.
*/
INLINE void ps_rx_meta_info_free
(
  ps_rx_meta_info_type ** rx_mi_ptr_ptr
)
{
  if (rx_mi_ptr_ptr != NULL && *rx_mi_ptr_ptr != NULL)
  {
    RX_META_INFO_DEBUG( "RX_META FREE ptr 0x%p", *rx_mi_ptr_ptr);
    PS_MEM_FREE(*rx_mi_ptr_ptr);
  }
}
#ifdef __cplusplus
}
#endif

/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO PS_RX_META_INFO_DUP()
===========================================================================*/
/**
  Duplicates the passed PS memory buffer used for the Rx meta
  information.

  @param[in] rx_mi_ptr     Pointer to ps_rx_meta_info_type.
  @param[out] dup_ptr_ptr  Duplicate pointer to the Rx meta information type
                           pointer.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  Allocates new PS memory buffers for Rx meta information and packet 
  information when present.
*/
#define PS_RX_META_INFO_DUP ps_rx_meta_info_dup
#ifdef __cplusplus
extern "C" {
#endif
/** @} */ /* end_addtogroup macros_ipUtilities */
/*===========================================================================
PS_RX_META_INFO_DUP()
===========================================================================*/
/** @ingroup ps_rx_meta_info_dup
  Duplicates the Rx meta information pointer.

  @param[in] rx_mi_ptr     Rx meta information pointer to be freed.
  @param[out] dup_ptr_ptr  Duplicate pointer to the Rx meta information type
                           pointer.
    
  @return
  None.

  @dependencies
  None.
*/
INLINE void ps_rx_meta_info_dup
(
  ps_rx_meta_info_type * rx_mi_ptr,
  ps_rx_meta_info_type ** dup_ptr_ptr
)
{
  if (rx_mi_ptr != NULL && dup_ptr_ptr != NULL)
  {
    RX_META_INFO_DEBUG( "RX_META DUP ptr 0x%p", rx_mi_ptr);
    *dup_ptr_ptr = (ps_rx_meta_info_type *) ps_mem_dup(rx_mi_ptr);
  }
}
#ifdef __cplusplus
}
#endif
/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO PS_RX_META_GET_PKT_INFO()
===========================================================================*/
/**
  Gets the packet information stored inside the Rx meta information.

  @param[in] rx_mi_ptr  Pointer to ps_rx_meta_info_type.

  @return
  None.

  @dependencies
  The meta information point must be valid.
*/
#define PS_RX_META_GET_PKT_INFO(rx_mi_ptr)  (rx_mi_ptr)->pkt_info

/*===========================================================================
MACRO PS_RX_META_SET_PKT_INFO()
===========================================================================*/
/**
  Copies the packet information to the meta information passed.

  @param[in] rx_mi_ptr    Pointer to ps_rx_meta_info_type.
  @param[in] mi_pkt_info  Packet information.

  @return
  None.

  @dependencies
  The meta information point must be valid.
*/
#define PS_RX_META_SET_PKT_INFO(rx_mi_ptr, mi_pkt_info)           \
  memcpy(&(rx_mi_ptr)->pkt_info, &(mi_pkt_info), sizeof(mi_pkt_info));

/*===========================================================================
MACRO PS_RX_META_IS_PKT_INFO_VALID()
===========================================================================*/
/**
  Returns the packet information validity.

  @param[in] rx_mi_ptr  Pointer to ps_rx_meta_info_type.

  @return
  Returns the pkt_info_valid flag.

  @dependencies
  The Rx meta information pointer must be valid.
*/
#define PS_RX_META_IS_PKT_INFO_VALID(rx_mi_ptr)                    \
  (rx_mi_ptr)->pkt_info.is_pkt_info_valid


/*===========================================================================
MACRO PS_RX_META_INVALIDATE_PKT_INFO()
===========================================================================*/
/**
  Invalidates the packet information.

  @param[in] rx_mi_ptr  Pointer to ps_rx_meta_info_type.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RX_META_INVALIDATE_PKT_INFO(rx_mi_ptr)                    \
  (rx_mi_ptr)->pkt_info.is_pkt_info_valid = FALSE

/*===========================================================================
MACRO PS_RX_META_IS_FILTER_MASK_SET()
===========================================================================*/
/**
  Checks whether the filter mask associated with a particular client ID is set.

  @param[in] rx_mi_ptr  Pointer to ps_rx_meta_info_type.
  @param[in] client_id  IP filter client ID.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RX_META_IS_FILTER_MASK_SET(rx_mi_ptr, client_id)         \
  ((rx_mi_ptr)->fi_mask & ((uint32) 1 << (uint32)(client_id)))

/*===========================================================================
MACRO PS_RX_META_GET_FILTER_MASK()
===========================================================================*/
/**
  Gets the entire filter mask associated with the meta information.

  @param[in] rx_mi_ptr  Pointer to ps_rx_meta_info_type.

  @return
  Filter mask.

  @dependencies
  The rx_mi_ptr pointer must be valid.
*/
#define PS_RX_META_GET_FILTER_MASK(rx_mi_ptr) (rx_mi_ptr)->fi_mask

/*===========================================================================
MACRO PS_RX_META_GET_FILTER_RESULT()
===========================================================================*/
/**
  Returns the filter result associated with a particular client ID. The result
  is NULL when the particular filter has not been executed or failed the match 
  on the packet.

  @param[in] rx_mi_ptr  Pointer to ps_rx_meta_info_type.
  @param[in] client_id  IP filter client ID.

  @return
  Filter result.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RX_META_GET_FILTER_RESULT(rx_mi_ptr, client_id)              \
  (((rx_mi_ptr)->fi_mask & ((uint32) 1 << (client_id)))                     \
     ? (rx_mi_ptr)->fi_result[client_id]                                    \
     : PS_IFACE_IPFLTR_NOMATCH)

/*===========================================================================
MACRO PS_RX_META_SET_FILTER_RESULT()
===========================================================================*/
/**
  Sets the filter result associated with a particular client ID. The 
  corresponding filter mask bit is also set to indicate there is no need to 
  execute filters for the client ID since a resulting fi_result is already
  available.

  @param[in] rx_mi_ptr     Pointer to ps_rx_meta_info_type.
  @param[in] client_id     IP filter client ID.
  @param[in] in_fi_result  Filter result to set.

  @return
  Filter result.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RX_META_SET_FILTER_RESULT(rx_mi_ptr, client_id, in_fi_result)       \
  (rx_mi_ptr)->fi_result[(client_id)] =                                     \
    (ps_iface_ipfltr_result_type)(in_fi_result);                         \
  (rx_mi_ptr)->fi_mask |= ((uint32) 1 << (uint32)(client_id));

/*===========================================================================
MACRO PS_RX_META_RESET_FILTER_RESULT()
===========================================================================*/
/**
  Resets the filter result associated with a particular client ID. The 
  corresponding filter mask bit is also reset.

  @param[in] rx_mi_ptr   Pointer to ps_rx_meta_info_type.
  @param[in] client_id   IP filter client ID.

  @return
  Filter result.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RX_META_RESET_FILTER_RESULT(rx_mi_ptr, client_id)                   \
  (rx_mi_ptr)->fi_result[(client_id)] = PS_IFACE_IPFLTR_NOMATCH;            \
  (rx_mi_ptr)->fi_mask &= ~((uint32) 1 << (client_id));

/*===========================================================================
MACRO PS_RX_META_GET_IPSEC_INFO()
===========================================================================*/
/**
  Returns IP security information associated with the given Rx meta 
  information.

  @param[in] rx_mi_ptr  Pointer to ps_rx_meta_info_type.

  @return
  IPSec information pointer.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RX_META_GET_IPSEC_INFO(rx_mi_ptr)  (rx_mi_ptr)->ipsec_info

/*===========================================================================
MACRO PS_RX_META_GET_ROUTING_CACHE()
===========================================================================*/
/**
  Gets the routing cache associated with the given Rx meta information.

  @param[in] rx_mi_ptr  Pointer to ps_tx_meta_info_type.

  @return
  Routing cache.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RX_META_GET_ROUTING_CACHE(rx_mi_ptr) (rx_mi_ptr)->routing_cache

/*===========================================================================
MACRO PS_META_SET_ROUTING_CACHE()
===========================================================================*/
/**
  Sets the routing for the given Rx meta information.

  @param[in] rx_mi_ptr         Pointer to ps_rx_meta_info_type.
  @param[in] mi_routing_cache  Subset ID to be set.

  @return
  Meta information routing cache.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_RX_META_SET_ROUTING_CACHE(rx_mi_ptr, mi_routing_cache)             \
  (rx_mi_ptr)->routing_cache = (mi_routing_cache);

/** @} */ /* end_addtogroup macros_ipUtilities */

#endif /* PS_RX_META_INFO_H */
