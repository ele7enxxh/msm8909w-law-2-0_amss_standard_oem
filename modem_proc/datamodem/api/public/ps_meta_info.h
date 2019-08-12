#ifndef PS_META_INFO_H
#define PS_META_INFO_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         P S _ M E T A _ I N F O . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
   @file ps_meta_info.h
   @brief This is a header file that contains the definition of the meta information
  that is passed down from the sockets layer all the way down to the
  interface.
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
Copyright (c) 2002-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_meta_info.h_v   1.2
  12 Feb 2003 20:35:38   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_meta_info.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/07/11    SCD    (Tech Pubs) Continued to edit Doxygen markup and comments.
12/01/10    SCD    (Tech Pubs) Edited Doxygen markup and comments.
12/01/10    sa     Modified the comments according to doxygen specific 
                   semantics.
03/26/09    pp     CMI De-featurization.
12/14/08    pp     Common Modem Interface: Public/Private API split.
09/12/08    pp     Metainfo optimizations. New ps_*_meta_info.h files created.
05/29/08    ssh    Added PS_META_INFO_COPY
03/16/07    scb    Fixed Klocwork High errors
12/15/06    msr    Using time_type for timestamping packets
11/17/06    msr    Added support for timestamping packets
07/31/06    rt     Route Scope modifications.
07/07/06    msr    Added PS_META_IS_FILTER_MASK_SET()
12/15/05    msr    Added PS_META_RESET_FILTER_RESULT()
09/02/05    msr    Allocating pkt_info in PS_META_INFO_GET()
05/11/05    sv     Lint changes.
05/10/05    vp     PS_META_INFO_DUP and PS_META_INFO_FREE to increment the
                   refcnt instead of allocating a new buffer.
05/02/05    msr    Including ps_iface_ipfltr.h instead of ps_iface_defs.h and
                   changed PS_META_FILTER_ID and PS_META_SET_FILTER_ID to
                   PS_META_SET_FILTER_RESULT and PS_META_SET_FILTER_RESULT,
04/18/05    vp     Addition of route_scope to meta_info.
04/17/05    msr    Added subset ID to support subset based filtering.
01/10/05    sv     Merged IPSEC changes.
12/23/04    lyr    Added next hop addr & packet scope (e.g. unicast) metainfo
10/01/04    ifk    Added meta_info_dup macro
06/15/04    sv     Added flow label to metainfo structure.
04/27/04    usb    Added support for filtering/pkt info in metainfo, removed
                   unused member ipsec_required, cleaned up existing macro
                   definitions, defined new macros.
03/10/04    aku    Call ps_mem_free() only if meta_info_ptr is not NULL.
02/02/04    mvl    Added #define for kind field - used in ps-ds interface.
01/31/04    usb    Removed policy info from meta info.
08/25/03    aku    ps_mem_free() takes pointer-to-pointer as arg.
08/15/03    aku    Added support to use PS memory allocation schemes instead
                   of DSM
08/01/03    ss     Removed ttl and sock_opts from meta info
06/06/03    om     Added ipsec_required flag.
04/24/03    mvl    Added some missing parentheses in macros.
02/12/03    om     Added policy-info-is-valid flag and UDP routing info.
09/24/02    om     Added socket options mask.
04/01/02    mvl    created file.
===========================================================================*/
/* group: ipUtilities */

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_tx_meta_info.h"
#include "ps_rx_meta_info.h"
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            DEFINES AND TYPEDEFS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/** @addtogroup datatypes
  @{
*/

/*---------------------------------------------------------------------------
TYPEDEF ps_meta_info_type
---------------------------------------------------------------------------*/
/**
   @deprecated
   Use #ps_tx_meta_info_type.
*/
typedef ps_tx_meta_info_type ps_meta_info_type;

/*---------------------------------------------------------------------------
TYPEDEF ps_meta_info_type_ex
---------------------------------------------------------------------------*/
/**
  @brief Passes both Tx and Rx meta information.
 */
typedef struct
{
  ps_tx_meta_info_type           * tx_meta_info_ptr;
    /**< Pointer to the Tx meta information. */
  ps_rx_meta_info_type           * rx_meta_info_ptr;
    /**< Pointer to the Rx meta information. */
} ps_meta_info_type_ex;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO PS_META_INFO_GET_EX()
===========================================================================*/
/**
  Allocates a PS memory buffer used for ps_meta_info_type_ex.

  @param[out] mi_ptr Pointer with ps_meta_info_ex memory assigned to it.

  @return
  The mi_ptr pointer is NULL on failure or points to the #ps_meta_info_type_ex buffer.

  @dependencies
  None.

  @sideeffects
  Initializes the allocated meta information block to all zeros.
*/
#define PS_META_INFO_GET_EX(mi_ptr)                                 \
  (mi_ptr) = (ps_meta_info_type_ex *)ps_ext_mem_get_buf(PS_EXT_MEM_META_INFO_TYPE_EX); \
  if ( (mi_ptr) != NULL )                                              \
  {                                                                     \
    memset( (mi_ptr), 0, sizeof(ps_meta_info_type_ex) );               \
  }

#define  PS_META_INFO_DUP  ps_meta_info_dup   
#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================
MACRO PS_META_INFO_DUP()

DESCRIPTION
  This macro duplicates the passed PS mem buffer that is used for the meta
  info.

PARAMETERS
  mi_ptr_ptr : pointer to a ps_meta_info_type pointer.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates new PS mem buffers for meta info and packet info if present
===========================================================================*/
INLINE void ps_meta_info_dup
(
  ps_meta_info_type_ex * mi_ptr,
  ps_meta_info_type_ex ** dup_ptr_ptr
)
{
  if((mi_ptr) && (dup_ptr_ptr))                                      
  {                                                                   
    *(dup_ptr_ptr) = (ps_meta_info_type_ex *)ps_mem_dup((void *)mi_ptr);                              
    if((mi_ptr)->tx_meta_info_ptr != NULL && *(dup_ptr_ptr) != NULL)      
    {                                                                 
      ps_tx_meta_info_dup(mi_ptr->tx_meta_info_ptr, &(*(dup_ptr_ptr))->tx_meta_info_ptr);
    }                                                                 
    if((mi_ptr)->rx_meta_info_ptr != NULL && *(dup_ptr_ptr) != NULL)      
    {                                                                 
      ps_rx_meta_info_dup(mi_ptr->rx_meta_info_ptr, &(*(dup_ptr_ptr))->rx_meta_info_ptr);
    }                                                                       
  }                                                                   
}  

#ifdef __cplusplus
}
#endif                                                  
/** @} */ /* end_addtogroup macros_ipUtilities */
/** @cond
*/
/*===========================================================================
MACRO PS_META_INFO_FREE_EX()
===========================================================================*/
/**
  Deallocates the PS memory buffer used for ps_meta_info_type_ex.

  @param[in] mi_ptr  #ps_meta_info_type_ex pointer that is freed.

  @return
  The mi_ptr_ptr parameter is set to NULL.

  @dependencies
  None.
*/
#define PS_META_INFO_FREE_EX ps_meta_info_free_ex
#ifdef __cplusplus
extern "C" {
#endif
/** @endcond */ /* shortcut */

/*===========================================================================
PS_META_INFO_FREE_EX()
===========================================================================*/
/** @ingroup ps_meta_info_free_ex
  Frees the meta information pointer.

  @datatypes
  #ps_meta_info_type_ex

  @vertspace
  @param[in] mi_ptr_ptr   Pointer to be freed.
  
  @return
  None.

  @dependencies
  None.
*/
INLINE void ps_meta_info_free_ex
(
  ps_meta_info_type_ex ** mi_ptr_ptr
)
{
  if(mi_ptr_ptr != NULL && *mi_ptr_ptr != NULL)
  {
    PS_MEM_FREE(*mi_ptr_ptr);
  }
}
#ifdef __cplusplus
}
#endif

/** @cond
*/
/*---------------------------------------------------------------------------
                             META INFO/TX META INFO

  Following definitions are kept to NOT to impact mode handlers!
---------------------------------------------------------------------------*/
/*===========================================================================
MACRO PS_META_INFO_IS_NULL()
===========================================================================*/
/**
  Checks for NULL. Alias: #PS_TX_META_INFO_IS_NULL
 */
#define PS_META_INFO_IS_NULL PS_TX_META_INFO_IS_NULL

/*===========================================================================
MACRO PS_META_INFO_FREE()
===========================================================================*/
/**
  Frees the PS memory buffer used for the meta information.
  Alias: #PS_TX_META_INFO_FREE
 */
#define PS_META_INFO_FREE PS_TX_META_INFO_FREE

/*===========================================================================
MACRO PS_META_GET_IPSEC_INFO()
===========================================================================*/
/**
  Gets the IP security information associated with the given meta 
  information. Alias: #PS_TX_META_GET_IPSEC_INFO
 */
#define PS_META_GET_IPSEC_INFO PS_TX_META_GET_IPSEC_INFO

/*===========================================================================
MACRO PS_META_GET_TX_FLAGS()
===========================================================================*/
/**
  Gets the Tx flags associated with the given meta information.
  Alias: #PS_TX_META_GET_TX_FLAGS
 */
#define PS_META_GET_TX_FLAGS PS_TX_META_GET_TX_FLAGS

/*===========================================================================
MACRO PS_META_GET_DOS_ACK_HANDLE()
===========================================================================*/
/**
  Gets dos_ack_handle associated with the given meta information.
  Alias: #PS_TX_META_GET_DOS_ACK_HANDLE
 */
#define PS_META_GET_DOS_ACK_HANDLE PS_TX_META_GET_DOS_ACK_HANDLE

/*===========================================================================
MACRO PS_META_GET_ROUTING_CACHE ()
===========================================================================*/
/**
  Gets the routing cache associated with the given Tx meta information.
  Alias: #PS_TX_META_GET_ROUTING_CACHE
 */
#define PS_META_GET_ROUTING_CACHE PS_TX_META_GET_ROUTING_CACHE

/*===========================================================================
MACRO PS_META_GET_TIMESTAMP()
===========================================================================*/
/**
  Gets the timestamp associated with the given meta information.
  Alias: #PS_TX_META_GET_TIMESTAMP
 */
#define PS_META_GET_TIMESTAMP PS_TX_META_GET_TIMESTAMP

/*===========================================================================
  PS_META* version of MACROs to access TX_META fields
===========================================================================*/
/*===========================================================================
MACRO PS_META_GET_RT_META_INFO_PTR()
===========================================================================*/
/**
  Gets the routing meta information pointer associated with the Tx 
  meta information.
  Alias: #PS_TX_META_GET_RT_META_INFO_PTR
 */
#define PS_META_GET_RT_META_INFO_PTR PS_TX_META_GET_RT_META_INFO_PTR

/*===========================================================================
MACRO PS_META_GET_PKT_META_INFO_PTR()
===========================================================================*/
/**
  Gets the packet meta information pointer associated with the Tx 
  meta information.
  Alias: #PS_TX_META_GET_PKT_META_INFO_PTR
 */
#define PS_META_GET_PKT_META_INFO_PTR PS_TX_META_GET_PKT_META_INFO_PTR
/** @endcond */

#endif /* PS_META_INFO_H */
