#ifndef PS_PKT_META_INFO_H
#define PS_PKT_META_INFO_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ P K T _ M E T A _ I N F O . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
   @file ps_pkt_meta_info.h
   @brief This is a header file that contains the definition of the per packet meta
  information that is passed down from the sockets layer all the way down to the
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
Copyright (c) 2008-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_meta_info.h_v   1.2
  12 Feb 2003 20:35:38   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_pkt_meta_info.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/06/11    SCD    (Tech Pubs) Continued to edit Doxygen markup and comments.
08/26/10    pp     Added PS_EXT_PKT_META_INFO_GET API.
12/01/10    SCD    (Tech Pubs) Edited Doxygen markup and comments.
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

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "time_svc.h"
#include "qw.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "msg.h"

#include "ps_mem_ext.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            DEFINES AND TYPEDEFS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/** @addtogroup macros_ipUtilities
  @{
*/

/**
  Aids in internal debugging. Current level: MSG LOW.
*/
#define PKT_META_INFO_DEBUG( sf, a)                               \
                     MSG_1( MSG_SSID_DS, MSG_LEGACY_LOW, sf, a)
/** @} */ /* end_addtogroup macros_ipUtilities */

/** @addtogroup datatypes
  @{
*/ 
/*-----------------------------------------------------------------------------
TYPEDEF PS_PKT_META_INFO_TYPE
-----------------------------------------------------------------------------*/
/**
  @brief Passes per-packet meta information through the stack. This structure 
  moves with the DSM item carrying the actual IP packet.

  This structure is used in the Tx path of the IP packet.
*/
typedef struct
{
  time_type                         timestamp;
    /**< Time when this packet is sent by the application. */
  uint32                            tx_flags;
    /**< Flags associated with each IP packet as defined in dssocket.h. */
  int32                             dos_ack_handle;
    /**< Socket that generated this packet. */
} ps_pkt_meta_info_type;
/** @} */ /* end_addtogroup datatypes */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         PACKET META INFO MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/** @addtogroup macros_ipUtilities
  @{
*/

/*===========================================================================
MACRO PS_PKT_META_INFO_FREE()
===========================================================================*/
/**
  Frees the PS memory buffer used for the packet meta information.

  @param[in] pkt_mi_ptr_ptr Pointer to ps_pkt_meta_info_type.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  The pkt_mi_ptr pointer is set to NULL.
*/
#define PS_PKT_META_INFO_FREE ps_pkt_meta_info_free
#ifdef __cplusplus
extern "C" {
#endif
/** @} */ /* end_addtogroup macros_ipUtilities */

/*===========================================================================
PS_PKT_META_INFO_FREE()
===========================================================================*/
/** @ingroup ps_pkt_meta_info_free
  Frees the meta information pointer.

  @datatypes
  #ps_pkt_meta_info_type

  @vertspace
  @param[in] pkt_mi_ptr_ptr   Pointer to be freed.
  
  @return
  None.

  @dependencies
  None.
*/
INLINE void ps_pkt_meta_info_free
(
  ps_pkt_meta_info_type ** pkt_mi_ptr_ptr
)
{
  if (pkt_mi_ptr_ptr != NULL && *pkt_mi_ptr_ptr != NULL)
  {
    PKT_META_INFO_DEBUG( "PKT_META FREE ptr 0x%p", *pkt_mi_ptr_ptr);
    PS_MEM_FREE(*pkt_mi_ptr_ptr);
  }
}
#ifdef __cplusplus
}
#endif

/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO PS_PKT_META_INFO_DUP()
===========================================================================*/
/**
  Duplicates the passed PS memory buffer used for the packet meta information.

  @param[in] pkt_mi_ptr    Pointer to ps_pkt_meta_info_type.
  @param[out] dup_ptr_ptr  Duplicate double pointer to ps_pkt_meta_info_type.

  @return
  None.

  @dependencies
  None.
*/
#define PS_PKT_META_INFO_DUP ps_pkt_meta_info_dup
#ifdef __cplusplus
extern "C" {
#endif
/** @} */ /* end_addtogroup macros_ipUtilities */

/*===========================================================================
PS_PKT_META_INFO_DUP()
===========================================================================*/
/** @ingroup ps_pkt_meta_info_dup
  Duplicates the meta information pointer.

  @datatypes
  #ps_pkt_meta_info_type

  @vertspace
  @param[in] pkt_mi_ptr    Packet meta information pointer.
  @param[out] dup_ptr_ptr  Duplicate pointer for the meta information
                           pointer.
  
  @return
  None.

  @dependencies
  None.
*/
INLINE void ps_pkt_meta_info_dup
(
  ps_pkt_meta_info_type * pkt_mi_ptr,
  ps_pkt_meta_info_type ** dup_ptr_ptr
)
{
  if  (pkt_mi_ptr != NULL && dup_ptr_ptr != NULL)
  {
    PKT_META_INFO_DEBUG( "PKT_META DUP ptr 0x%p", pkt_mi_ptr);
    *(dup_ptr_ptr) = (ps_pkt_meta_info_type *)ps_mem_dup(pkt_mi_ptr);
  }
}
#ifdef __cplusplus
}
#endif
/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO PS_PKT_META_INFO_IS_NULL()
===========================================================================*/
/**
  Checks whether meta information is NULL.

  @param[in] pkt_mi_ptr Pointer to ps_pkt_meta_info_type.

  @return 
  TRUE  -- Value is NULL.\n
  FALSE -- Value is not NULL.

  @dependencies
  None.
*/
#define PS_PKT_META_INFO_IS_NULL(pkt_mi_ptr) ((pkt_mi_ptr) == NULL)

/*===========================================================================
MACRO PS_PKT_META_GET_TX_FLAGS()
===========================================================================*/
/**
  Gets the Tx flags associated with the given packet meta information.

  @param[in] pkt_mi_ptr Pointer to ps_pkt_meta_info_type.

  @return
  The Tx flags associated with the given packet meta information.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_PKT_META_GET_TX_FLAGS(pkt_mi_ptr)  (pkt_mi_ptr)->tx_flags

/*===========================================================================
MACRO PS_PKT_META_SET_TX_FLAGS()
===========================================================================*/
/**
  Sets the Tx flags for the given packet meta information.

  @param[in] pkt_mi_ptr     Pointer to ps_pkt_meta_info_type.
  @param[in] mi_tx_flags    Tx flags to be set.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_PKT_META_SET_TX_FLAGS(pkt_mi_ptr, mi_tx_flags)                       \
  (pkt_mi_ptr)->tx_flags = (mi_tx_flags);

/*===========================================================================
MACRO PS_PKT_META_GET_DOS_ACK_HANDLE()
===========================================================================*/
/**
  Gets the dos_ack_handle value associated with the given packet meta 
  information.

  @param[in] pkt_mi_ptr   Pointer to ps_pkt_meta_info_type.

  @return
  Data Over Signaling Acknowledgement handle.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_PKT_META_GET_DOS_ACK_HANDLE(pkt_mi_ptr)                  \
  (pkt_mi_ptr)->dos_ack_handle

/*===========================================================================
MACRO PS_PKT_META_SET_DOS_ACK_HANDLE()
===========================================================================*/
/**
  Sets the dos_ack_handle value for the given packet meta information.

  @param[in] pkt_mi_ptr         Pointer to ps_pkt_meta_info_type.
  @param[in] mi_dos_ack_handle  Handle to be set.

  @return
  Meta information DOS acknowledgement handle.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_PKT_META_SET_DOS_ACK_HANDLE(pkt_mi_ptr, mi_dos_ack_handle) \
  (pkt_mi_ptr)->dos_ack_handle = (mi_dos_ack_handle);

/*===========================================================================
MACRO PS_PKT_META_GET_TIMESTAMP()
===========================================================================*/
/**
  Gets the timestamp associated with the given packet meta information.

  @param[in] pkt_mi_ptr   Pointer to ps_pkt_meta_info_type.

  @return
  Time stamp.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_PKT_META_GET_TIMESTAMP(pkt_mi_ptr)  (pkt_mi_ptr)->timestamp

/*===========================================================================
MACRO PS_PKT_META_SET_TIMESTAMP()
===========================================================================*/
/**
  Sets the timestamp for the given packet meta information.

  @param[in] pkt_mi_ptr    Pointer to ps_tx_meta_info_type.
  @param[in] mi_timestamp  Timestamp to be set.

  @return
  None.

  @dependencies
  The meta information pointer must be valid.
*/
#define PS_PKT_META_SET_TIMESTAMP(pkt_mi_ptr, mi_timestamp)          \
  qw_equ( (pkt_mi_ptr)->timestamp, mi_timestamp);

/*===========================================================================
MACRO PS_EXT_PKT_META_INFO_GET()
===========================================================================*/
/**
  Allocates a PS memory buffer used for the packet meta information.
  This function is used by external clients only.

  @param[in,out] pkt_mi_ptr Pointer that has the ps_pkt_meta_info_type 
                            memory assigned to it.

  @return
  The pkt_mi_ptr value is NULL on failure or points to the 
  ps_pkt_meta_info_type data.

  @dependencies
  None.

  @sideeffects
  Initializes the allocated meta information block to all zeros.
*/
#define PS_EXT_PKT_META_INFO_GET(pkt_mi_ptr)                                   \
  (pkt_mi_ptr) = (ps_pkt_meta_info_type *)ps_ext_mem_get_buf(PS_EXT_MEM_PKT_META_INFO_TYPE); \
  if ((pkt_mi_ptr) != NULL)                                                    \
  {                                                                            \
    memset((pkt_mi_ptr), 0, sizeof(ps_pkt_meta_info_type));                    \
  }

/** @} */  /* end_addtogroup macros_ipUtilities */

#endif /* PS_PKT_META_INFO_H */
