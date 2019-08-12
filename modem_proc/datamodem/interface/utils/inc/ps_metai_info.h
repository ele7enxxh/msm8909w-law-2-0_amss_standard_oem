#ifndef PS_METAI_INFO_H
#define PS_METAI_INFO_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         P S _ M E T A  I  _ I N F O  . H

GENERAL DESCRIPTION
  This is a header file that contains the definition of the METAINFO macros
  used internally by Common Data modules.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ps_metai_info.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/26/10    pp     Created file [moved GET/VALID macros here].
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ps_tx_meta_info.h"
#include "ps_rx_meta_info.h"
#include "ps_rt_meta_info.h"
#include "ps_pkt_meta_info.h"
#include "ps_mem.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            RT METAINFO GET & IS_VALID MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
MACRO PS_RT_META_INFO_GET()

DESCRIPTION
  This macro allocates a PS mem buffer that is used for the ROUTING meta info.

PARAMETERS
  rt_mi_ptr : pointer that will have the ps_rt_meta_info_type memory assigned
              to it.

RETURN VALUE
  rt_mi_ptr will be NULL on failure, or point to the ps_rt_meta_info_type data

DEPENDENCIES
  None

SIDE EFFECTS
  Initializes the allocated meta info block to all 0s.
===========================================================================*/
#ifndef PS_RT_META_INFO_GET
#define PS_RT_META_INFO_GET(rt_mi_ptr)                                  \
  (rt_mi_ptr) = (ps_rt_meta_info_type *) ps_mem_get_buf(PS_MEM_RT_META_INFO_TYPE); \
  RT_META_INFO_DEBUG( "RT_META_INFO_GET ptr 0x%p", rt_mi_ptr );     \
  if ((rt_mi_ptr) != NULL)                                                        \
  {                                                                        \
    memset((rt_mi_ptr), 0, sizeof(ps_rt_meta_info_type));                \
  }

/*===========================================================================
MACRO PS_RT_META_INFO_IS_VALID()

DESCRIPTION
  This macro validates that this is a valid ROUTING meta info item.

PARAMETERS
  rt_mi_ptr : pointer to the ROUTING meta info item being checked

RETURN VALUE
  TRUE if the item is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_RT_META_INFO_IS_VALID(rt_mi_ptr)                          \
  (ps_mem_is_valid((rt_mi_ptr), PS_MEM_RT_META_INFO_TYPE))

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            PKT METAINFO GET & IS_VALID MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
MACRO PS_PKT_META_INFO_GET()

DESCRIPTION
  This macro allocates a PS mem buffer that is used for the PACKET meta info.

PARAMETERS
  pkt_mi_ptr : pointer that will have the ps_pkt_meta_info_type memory assigned to it.

RETURN VALUE
  pkt_mi_ptr will be NULL on failure, or point to the ps_pkt_meta_info_type data

DEPENDENCIES
  None

SIDE EFFECTS
  Initializes the allocated meta info block to all 0s.
===========================================================================*/
#define PS_PKT_META_INFO_GET(pkt_mi_ptr)                                     \
  (pkt_mi_ptr) = (ps_pkt_meta_info_type *)ps_mem_get_buf(PS_MEM_PKT_META_INFO_TYPE); \
  if ((pkt_mi_ptr) != NULL)                                                          \
  {                                                                      \
    PKT_META_INFO_DEBUG( "PKT_META GET ptr 0x%p", (pkt_mi_ptr));    \
    memset((pkt_mi_ptr), 0, sizeof(ps_pkt_meta_info_type));                      \
  }

/*===========================================================================
MACRO PS_PKT_META_INFO_IS_VALID()

DESCRIPTION
  This macro validates that this is a valid PACKET meta info item.

PARAMETERS
  pkt_mi_ptr : pointer to the ps_pkt_meta_info_type item being checked

RETURN VALUE
  TRUE if the item is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_PKT_META_INFO_IS_VALID(pkt_mi_ptr)                                    \
  (ps_mem_is_valid((pkt_mi_ptr), PS_MEM_PKT_META_INFO_TYPE))

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            TX METAINFO GET & IS_VALID MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
MACRO PS_TX_META_INFO_GET()

DESCRIPTION
  This macro allocates a PS mem buffer that is used for the TX meta info.

PARAMETERS
  tx_mi_ptr : pointer that will be have the ps_tx_meta_info memory assigned to it.

RETURN VALUE
  tx_mi_ptr will be NULL on failure, or point to the ps_tx_meta_info_type buffer.

DEPENDENCIES
  None

SIDE EFFECTS
  Initializes the allocated meta info block to all 0s.

NOTE:
  If you are looking to get TX meta info along with Routing Meta info buffer,
  use PS_TX_META_INFO_AND_RT_META_INFO_GET.
===========================================================================*/
#define PS_TX_META_INFO_GET(tx_mi_ptr)                                 \
  (tx_mi_ptr) = (ps_tx_meta_info_type *)ps_mem_get_buf(PS_MEM_TX_META_INFO_TYPE); \
  if ( (tx_mi_ptr) != NULL )                                              \
  {                                                                         \
    TX_META_INFO_DEBUG( "TX_META GET ptr 0x%p",tx_mi_ptr);    \
    memset( (tx_mi_ptr), 0, sizeof(ps_tx_meta_info_type) );               \
  }

/*===========================================================================
MACRO PS_TX_META_INFO_AND_RT_META_INFO_GET()

DESCRIPTION
  This macro allocates a PS mem buffer that is used for the TX meta info.
  - This also allocates ROUTING meta info buffer inside the TX meta info.
  - This does NOT allocate PACKET meta info buffer [as no use case exists]
    inside the TX meta info.

PARAMETERS
  tx_mi_ptr : pointer that will be have the ps_tx_meta_info memory assigned to it.

RETURN VALUE
  tx_mi_ptr will be NULL on failure, or point to the ps_tx_meta_info_type buffer.
  tx_mi_ptr->rt_meta_info_ptr will be NULL on failure, or point to the
                           rt_meta_info_type buffer.
  tx_mi_ptr->pkt_meta_info_ptr will be NULL.

DEPENDENCIES
  None

SIDE EFFECTS
  Initializes the allocated meta info block to all 0s.

NOTE:
  This always creates ROUTING meta info inside the TX meta info,
  pointed by rt_meta_info_ptr.
  This doesnot allocate memory for PACKET meta info.
===========================================================================*/
#define PS_TX_META_INFO_AND_RT_META_INFO_GET(tx_mi_ptr)               \
  (tx_mi_ptr) = (ps_tx_meta_info_type *)ps_mem_get_buf(PS_MEM_TX_META_INFO_TYPE); \
  if ( (tx_mi_ptr) != NULL )                                              \
  {                                                                         \
    TX_META_INFO_DEBUG( "TX_META GET ptr 0x%p", tx_mi_ptr );   \
    memset( (tx_mi_ptr), 0, sizeof(ps_tx_meta_info_type) );               \
    /* Create ROUTING meta info */                                  \
    PS_RT_META_INFO_GET(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr)); \
  }

/*===========================================================================
MACRO PS_TX_META_INFO_GET_ALL()

DESCRIPTION
  This macro allocates a PS mem buffer that is used for the TX meta info.
  - This also allocates ROUTING meta info buffer inside the TX meta info.
  - This also allocates PACKET meta info buffer inside the TX meta info.

PARAMETERS
  tx_mi_ptr : pointer that will be have the ps_tx_meta_info memory assigned to it.

RETURN VALUE
  tx_mi_ptr will be NULL on failure, or point to the ps_tx_meta_info_type buffer.
  tx_mi_ptr->rt_meta_info_ptr will be NULL on failure, or point to the
                           rt_meta_info_type buffer.
  tx_mi_ptr->pkt_meta_info_ptr will be NULL. on failure, or point to the
                           pkt_meta_info_type buffer.

DEPENDENCIES
  None

SIDE EFFECTS
  Initializes the allocated meta info block to all 0s.

NOTE:
  This always creates ALL meta info buffers inside the TX meta info,
===========================================================================*/
#define PS_TX_META_INFO_GET_ALL(tx_mi_ptr)               \
  (tx_mi_ptr) = (ps_tx_meta_info_type *)ps_mem_get_buf(PS_MEM_TX_META_INFO_TYPE); \
  if ( (tx_mi_ptr) != NULL )                                              \
  {                                                                         \
    TX_META_INFO_DEBUG( "TX_META GET ptr 0x%p", tx_mi_ptr);   \
    memset( (tx_mi_ptr), 0, sizeof(ps_tx_meta_info_type) );               \
    /* Create ROUTING meta info */                                  \
    PS_RT_META_INFO_GET(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr)); \
    /* Create PACKET meta info */                                  \
    PS_PKT_META_INFO_GET(PS_TX_META_GET_PKT_META_INFO_PTR(tx_mi_ptr)); \
  }

/*===========================================================================
MACRO PS_TX_META_INFO_IS_VALID()

DESCRIPTION
  This macro validates that this is a valid meta info item.

PARAMETERS
  tx_mi_ptr : pointer to the TX meta info item being checked

RETURN VALUE
  TRUE if the item is valid
  FALSE otherwise

DEPENDENCIES
  Meta info is valid if:
   - ps_tx_meta_info_type is valid and
   - ps_rt_meta_info_type is valid and

SIDE EFFECTS
  None

NOTE
  Current usage of this MACRO is only with PS_FLOW_GET_FLOW_FROM_META_INFO,
  hence we don;t need to verify the validity of ps_pkt_meta_info_type!
===========================================================================*/
#define PS_TX_META_INFO_IS_VALID(tx_mi_ptr)                                 \
  ((ps_mem_is_valid((tx_mi_ptr), PS_MEM_TX_META_INFO_TYPE)) &&                \
   (PS_RT_META_INFO_IS_VALID(PS_TX_META_GET_RT_META_INFO_PTR(tx_mi_ptr))))

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                        RX METAINFO GET & IS_VALID MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
MACRO PS_RX_META_INFO_GET()

DESCRIPTION
  This macro allocates a PS mem buffer that is used for the RX meta info.

PARAMETERS
  rx_mi_ptr : pointer that will have the ps_rx_meta_info_type buffer
              assigned to it.

RETURN VALUE
  rx_mi_ptr will be NULL on failure, or point to the ps_rx_meta_info_type buffer.

DEPENDENCIES
  None

SIDE EFFECTS
  Initializes the allocated meta info block to all 0s.
===========================================================================*/
#define PS_RX_META_INFO_GET(rx_mi_ptr)                                  \
  (rx_mi_ptr) = (ps_rx_meta_info_type *)ps_mem_get_buf(PS_MEM_RX_META_INFO_TYPE); \
  RX_META_INFO_DEBUG("RX_META GET ptr 0x%p",rx_mi_ptr);          \
  if ((rx_mi_ptr) != NULL)                                                \
  {                                                                        \
    memset((rx_mi_ptr), 0, sizeof(ps_rx_meta_info_type));                \
  }

#endif /* PS_RT_META_INFO_GET */
#endif /* PS_META_INFO_I_H */
