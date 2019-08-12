#ifndef LTE_DSM_UTIL_H
#define LTE_DSM_UTIL_H

/*!
  @file
  lte_dsm_util.h

  @brief
  This file includes DSM utility functions/apis tailored for LTE usage.
  These functions are expected to perform better than legacy DSM apis.
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/common/inc/lte_dsm_util.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/20/14   sb      CR506328: Remove unused code and file
01/04/11   ax      Separate out what is needed by FTM to API dir
02/16/10   ar      included dsm_lock.h as dsm_queue.h no longer exports 
                   dsm_lock() api
02/15/09   bqiu    add lte_dsm_queue_init
01/29/09   bqiu    add lte_dsm_check, lte_dsm_next
11/18/08   ar      Added new DSM API lte_dsm_get_wm_size() to get the wm size
11/05/08   sm      initial version
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/* Target-independent Include files */
#include <comdef.h>
#include <customer.h>
#include <lte_assert.h>
#include <lte_lockless_q.h>
#include <dsm_item.h>
#include <dsm_lock.h>
#include <dsm_queue.h>
#include <msg.h>


/*==============================================================================

                   MACRO DEFINITIONS

==============================================================================*/

/*!
    @brief
    Locks the given watermark.
*/
#define LTE_DSM_LOCK_WM(wm_ptr) DSM_LOCK(&((wm_ptr)->lock))

/*!
    @brief
    Unlocks the given watermark.
*/
#define LTE_DSM_UNLOCK_WM(wm_ptr) DSM_UNLOCK(&((wm_ptr)->lock))

/*==============================================================================

                   static inline FUNCTIONS

==============================================================================*/

/*==============================================================================

  FUNCTION:  lte_dsm_get_wm_item_count()

==============================================================================*/
/*!
    @brief
    This function returns the number of dsm packet chains enqueued to a
    given watermark

    @note
    parameter 'wm_ptr' must NOT be NULL.

    @return
    Number of packets enqueued to the dsm watermark.

*/
/*============================================================================*/
static inline uint16 lte_dsm_get_wm_item_count
(
  /*! Pointer to watermark item to get item from */
  dsm_watermark_type *wm_ptr
)
{
  return(lte_lockless_q_cnt(wm_ptr->q_ptr));
}

/*==============================================================================

  FUNCTION:  lte_dsm_check()

==============================================================================*/
/*!
    @brief
    This function return the first dsm item in the water mark
    @note

    @return
    dsm pointer to the first item in the water mark
*/
/*============================================================================*/
static inline dsm_item_type * lte_dsm_check
(
  /*! Pointer to Watermark item to check */
  dsm_watermark_type *wm_ptr
)
{
  return ((dsm_item_type *) lte_lockless_q_check(wm_ptr->q_ptr));
} /* lte_dsm_check */

/*==============================================================================

  FUNCTION:  lte_dsm_get_wm_size()

==============================================================================*/
/*!
    @brief
    This function returns the total size (in bytes) of the dsm items enqueued to
    the given watermark

    @note
    parameter 'wm_ptr' must NOT be NULL.

    @return
    Number of bytes enqueued to the given watermark.

*/
/*============================================================================*/
static inline uint32 lte_dsm_get_wm_size
(
  /*! Pointer to watermark item */
  dsm_watermark_type *wm_ptr
)
{
  return(wm_ptr->current_cnt);
}

/*==============================================================================

  FUNCTION:  lte_dsmi_extract_1byte()

==============================================================================*/
/*!
    @brief
    This function extracts/copies one byte from the 'offset' inside the DSM
    packet pointer and stores the same to the address 'buf' passed to this
    function.

    @return
    number of bytes extracted
*/
/*============================================================================*/
#define lte_dsm_extract_1byte(packet_ptr, offset, buf) \
  lte_dsmi_extract_1byte((packet_ptr), (offset), (buf))

uint16 lte_dsmi_extract_1byte
(
  /*! dsm pointer of the packet from which a byte needs to be extracted */
  dsm_item_type *packet_ptr,
  /*! offset inside the dsm packet from which a byte needs to be extracted */
  uint16 offset,
  /*! address where extracted byte needs to be stored */
  void *buf
);

/*==============================================================================

  FUNCTION:  lte_dsmi_extract_2byte()

==============================================================================*/
/*!
    @brief
    This function extracts/copies two bytes from the 'offset' inside the DSM
    packet pointer and stores the same to the address 'buf' passed to this
    function.

    @return
    number of bytes extracted
*/
/*============================================================================*/
#define lte_dsm_extract_2byte(packet_ptr, offset, buf) \
  lte_dsmi_extract_2byte((packet_ptr), (offset), (buf))

uint16 lte_dsmi_extract_2byte
(
  /*! dsm pointer of the packet from which 2 bytes need to be extracted */
  dsm_item_type *packet_ptr,
  /*! offset inside the dsm packet from which 2 bytes need to be extracted */
  uint16 offset,
  /*! address where extracted bytes needs to be stored */
  void *buf
);

/*==============================================================================

  FUNCTION:  lte_dsmi_extract_3byte()

==============================================================================*/
/*!
    @brief
    This function extracts/copies three bytes from the 'offset' inside the DSM
    packet pointer and stores the same to the address 'buf' passed to this
    function.

    @return
    number of bytes extracted
*/
/*============================================================================*/
#define lte_dsm_extract_3byte(dsm_packet_ptr, offset, buf) \
  lte_dsmi_extract_3byte((dsm_packet_ptr), (offset), (buf))


uint16 lte_dsmi_extract_3byte
(
  /*! dsm pointer of the packet from which 3 bytes need to be extracted */
  dsm_item_type *packet_ptr,
  /*! offset inside the dsm packet from which 3 bytes need to be extracted */
  uint16 offset,
  /*! address where extracted bytes needs to be stored */
  void *buf
);


/*==============================================================================

  FUNCTION:  lte_dsm_extract_4byte()

==============================================================================*/
/*!
    @brief
    This function extracts/copies four bytes from the 'offset' inside the DSM
    packet pointer and stores the same to the address 'buf' passed to this
    function.

    @return
    number of bytes extracted
*/
/*============================================================================*/
#define lte_dsm_extract_4byte(dsm_packet_ptr, offset, buf) \
  lte_dsmi_extract_4byte((dsm_packet_ptr), (offset), (buf))

uint16 lte_dsmi_extract_4byte
(
  /*! dsm pointer of the packet from which four bytes need to be extracted */
  dsm_item_type *packet_ptr,
  /*! offset inside the dsm packet from which four bytes need to be extracted */
  uint16 offset,
  /*! address where extracted bytes needs to be stored */
  void *buf
);

#endif /* LTE_DSM_UTIL_H */

