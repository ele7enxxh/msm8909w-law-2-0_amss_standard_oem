/*!
  @file
  qsh_types.h

  @brief
  Contains types common to client, invoke, and ext interfaces.
*/

/*==============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/qsh/qsh_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/11/15   ca      CR:920884, QSH rule man phase 1, 2 and 3.
05/15/15   vd      CR835881: Increase the size of the metric timestamp to uint32
                   and change the granularity to 1 msec
03/03/15   mm      Removed EVENT
02/19/15   mm      Added EVENT to qsh_msg_type_e
02/18/15   mm      Added qsh_dump_meta_hdr_s
02/17/15   mm      Updated documentation for qsh_metric_fifo_s
02/13/15   mm      Changed void * -> uint8 * for pointer addition
02/12/15   mm      Added qsh_dump_tag_hdr_s
01/22/15   mm      Interface changes for dumping data structures
11/21/14   mm      Added TRM client
07/14/14   mm      Changed client CFM -> CFCM
05/23/14   mm      Initial check-in
==============================================================================*/

#ifndef QSH_TYPES_H
#define QSH_TYPES_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <stddef.h>
#include <limits.h>
#include <qsh_types_clt.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/* -----------------------------------------------------------------------------

  Generic types.
  
------------------------------------------------------------------------------*/

/*! 
  @brief
  Represents a buffer of memory.
*/
typedef struct
{
  /*! pointer to memory; shall be 4 byte aligned address */
  uint8 *   addr;
  
  /*! size of memory in bytes
      shall be cumulative of QSH_EXT_DUMP_CLIENT_META_HDR_SIZE + size of all 
      the tags requested as per 'dump_tag_mask' */
  size_t    size_bytes;
} qsh_iovec_s;

/*!
  @brief
  Initializes an iovec type.
*/
static inline void qsh_iovec_init
(
  qsh_iovec_s *   qsh_iovec_ptr
)
{
  qsh_iovec_ptr->addr = NULL;
  qsh_iovec_ptr->size_bytes = 0;
}

/* -----------------------------------------------------------------------------

  Top-level QSH types.
  
------------------------------------------------------------------------------*/

/*!
  @brief
  Represents an ID per QSH invocation.
*/
typedef uint32 qsh_context_id_t;

/*!
  @brief
  Represents qsh_context_id_t reserved for NONE.
*/
#define QSH_CONTEXT_ID_NONE   ((qsh_context_id_t) 0)

/*!
  @brief
  Client enum, exported in <qsh_types_clt.h>.
*/
typedef enum qsh_clt_e qsh_clt_e;

/*!
  @brief
  For internal purposes only. Client should use qsh_action_e below. 

  Enum to represent action index. 
*/
typedef enum
{
  QSH_ACTION_RESET_IDX,
  QSH_ACTION_ANALYSIS_IDX,
  QSH_ACTION_DUMP_COLLECT_IDX,
  QSH_ACTION_METRIC_TIMER_EXPIRY_IDX,
  QSH_ACTION_METRIC_CFG_IDX,
  QSH_ACTION_EVENT_CFG_IDX,
  QSH_ACTION_EVENT_NOTIFY_IDX,
  QSH_ACTION_CFG_IDX,
  QSH_ACTION_MAX_IDX
} qsh_action_idx_e;

/*!
  @brief
  Enum used to identify the action. For bitmask of this enum, see 
  qsh_action_mask_t below.
*/
typedef enum
{
  /*! No action */
  QSH_ACTION_NONE                 = 0,
  
  /*! Reset (todo: remove) */
  QSH_ACTION_RESET                = 1 << QSH_ACTION_RESET_IDX,
  
  /*! Analyze */
  QSH_ACTION_ANALYSIS             = 1 << QSH_ACTION_ANALYSIS_IDX,
  
  /*! Collect dump tag */
  QSH_ACTION_DUMP_COLLECT         = 1 << QSH_ACTION_DUMP_COLLECT_IDX,
  
  /*! Metric timer expiry */
  QSH_ACTION_METRIC_TIMER_EXPIRY  = 1 << QSH_ACTION_METRIC_TIMER_EXPIRY_IDX,
  
  /*! Configure metric timer */
  QSH_ACTION_METRIC_CFG           = 1 << QSH_ACTION_METRIC_CFG_IDX,
  
  /*! Configure event(s) */
  QSH_ACTION_EVENT_CFG            = 1 << QSH_ACTION_EVENT_CFG_IDX,

  /*! Notify client with event(s) */
  QSH_ACTION_EVENT_NOTIFY         = 1 << QSH_ACTION_EVENT_NOTIFY_IDX,
  
  /*! Config client with param(s) */
  QSH_ACTION_CFG                  = 1 << QSH_ACTION_CFG_IDX
} qsh_action_e;

/*!
  @brief
  Type to represent bitmask of actions.
    
  Example:
  qsh_action_mask_t mask = QSH_ACTION_ANALYSIS | QSH_ACTION_DUMP_COLLECT;
*/
typedef uint32 qsh_action_mask_t;

/*!
  @brief
  Macro to check if action is enabled in the bitmask.
*/
#define QSH_ACTION_ENABLED(action_mask, action) \
  ((action_mask & action) != 0)

/*!
  @brief
  Enum to represent whether or not sync is required.
*/
typedef enum
{
  QSH_ACTION_MODE_SYNC_REQUIRED,
  QSH_ACTION_MODE_SYNC_OPTIONAL,
  QSH_ACTION_MODE_ASYNC_REQUIRED,
  QSH_ACTION_MODE_MAX
} qsh_action_mode_e;

/*!
  @brief
  Enum to represent whether action was done in sync/async mode.
*/
typedef enum
{
  QSH_ACTION_MODE_DONE_ASYNC,
  QSH_ACTION_MODE_DONE_SYNC,
  QSH_ACTION_MODE_DONE_MAX
} qsh_action_mode_done_e;

/*!
  @brief
  Struct for passing test configuration to clients.
*/
typedef struct
{
  uint32  metric_mask;
  uint32  event_mask;
} qsh_test_cfg_s;

/* ----------------------------------------------------------------------------- 

  Types for action: QSH_ACTION_ANALYSIS
  
----------------------------------------------------------------------------- */

/*!
  @brief
  For internal purposes only. Client should use qsh_cat_e below.
            
  Enum to represent category index.
*/
typedef enum
{
  QSH_CAT_TPUT_IDX,
  QSH_CAT_CFG_IDX,
  QSH_CAT_DSM_IDX,
  QSH_CAT_PERF_IDX,
  QSH_CAT_OTHER_IDX,
  QSH_CAT_EVENT_IDX,
  QSH_CAT_MAX_IDX
} qsh_cat_idx_e;

/*!
  @brief  
  Enum used to identify the category. Should be used as a bitmask.
*/
typedef enum
{
  /*! Throughput */
  QSH_CAT_TPUT    = 1 << QSH_CAT_TPUT_IDX,
  
  /*! Configuration */
  QSH_CAT_CFG     = 1 << QSH_CAT_CFG_IDX,
  
  /*! DSM */
  QSH_CAT_DSM     = 1 << QSH_CAT_DSM_IDX,
  
  /*! Performance */
  QSH_CAT_PERF    = 1 << QSH_CAT_PERF_IDX,
  
  /*! Other */
  QSH_CAT_OTHER   = 1 << QSH_CAT_OTHER_IDX,
  
  /*! event */
  QSH_CAT_EVENT   = 1 << QSH_CAT_EVENT_IDX,
  
  /*! DO NOT USE. Add new enums before this entry and update qsh_cat_idx_e */
  QSH_CAT_MAX     = 1 << QSH_CAT_MAX_IDX,
  
  /*! All categories. DO NOT USE for logging */
  QSH_CAT_ALL     = 0xFFFFFFFF
} qsh_cat_e;

/*!
  @brief
  For internal purposes only.
            
  Enum to represent client rule action.
*/
typedef enum
{
  QSH_CLIENT_RULE_ACTION_ADD_IDX     = 0x0,
  QSH_CLIENT_RULE_ACTION_DELETE_IDX  = 0x1,
  QSH_CLIENT_RULE_ACTION_CFG_IDX     = 0x2,
  QSH_CLIENT_RULE_ACTION_HELP_IDX    = 0x3,
  QSH_CLIENT_RULE_ACTION_MAX_IDX     = 0x4,
} qsh_client_rule_action_idx_e;

/*!
  @brief  
  Enum used to identify the client rule action. Should be used as a bitmask.
*/
typedef enum
{
  /*! No Action */
  QSH_CLIENT_RULE_ACTION_NONE      = 0x0,
  /*! Add */
  QSH_CLIENT_RULE_ACTION_ADD       = 1 << QSH_CLIENT_RULE_ACTION_ADD_IDX,

  /*! delete */
  QSH_CLIENT_RULE_ACTION_DELETE    = 1 << QSH_CLIENT_RULE_ACTION_DELETE_IDX,

  /*! cfg */
  QSH_CLIENT_RULE_ACTION_CFG       = 1 << QSH_CLIENT_RULE_ACTION_CFG_IDX,
  
  /*! help */
  QSH_CLIENT_RULE_ACTION_HELP      = 1 << QSH_CLIENT_RULE_ACTION_HELP_IDX,
  
  /*! DO NOT USE. Add new enums before this entry and update qsh_cat_idx_e */
  QSH_CLIENT_RULE_ACTION_MAX       = 1 << QSH_CLIENT_RULE_ACTION_MAX_IDX,
  
  /*! All categories. DO NOT USE for logging */
  QSH_CLIENT_RULE_ACTION_ALL       = 0xFFFFFFFF
} qsh_client_rule_action_e;

/*!
  @brief
  Type to represent bitmask of categories.
*/
typedef uint32 qsh_cat_mask_t;

/*!
  @brief
  Macro to check if category is enabled in the bitmask.
*/
#define QSH_CAT_ENABLED(cat_mask, cat) \
  ((cat_mask & cat) != 0)

/*!
  @brief
  Enum used to identify the message type. 
*/
typedef enum
{
  /*! High */
  QSH_MSG_TYPE_HIGH,
  
  /*! Medium */
  QSH_MSG_TYPE_MEDIUM,
  
  /*! For information */
  QSH_MSG_TYPE_INFO,
  
  /*! For debugging */
  QSH_MSG_TYPE_DEBUG,
  
  /*! DO NOT USE */
  QSH_MSG_TYPE_MAX
} qsh_msg_type_e;

/* -----------------------------------------------------------------------------

  Action: QSH_ACTION_DUMP_COLLECT
  
----------------------------------------------------------------------------- */

/*!
  @brief
  Type to represent dump tag enum.
*/
typedef uint32 qsh_dump_tag_t;

/*!
  @brief
  Type to represent bitmask of qsh_dump_tag_t.
*/
typedef uint32 qsh_dump_tag_mask_t;

/*!
  @brief
  Gives whether or not the dump tag is set in the tag mask.
*/
#define QSH_DUMP_TAG_ENABLED(tag_mask, tag_enum) \
  ((tag_mask & (1 << tag_enum)) != 0)

/*! 
  @brief
  Max # of tags that can be defined by client: currently 31 due to 
  reserved bit for ANY.
*/
#define QSH_DUMP_TAG_MAX_COUNT \
  ((CHAR_BIT * sizeof(qsh_dump_tag_mask_t)) - 1)
  
/*!
  @brief
  Macro to get dump_tag mask from enum.
    
  Example: 
    qsh_dump_tag_mask_t mask = 
      QSH_DUMP_TAG_MASK_GET(A2_QSH_DUMP_TAG_UL_PHY) |
      QSH_DUMP_TAG_MASK_GET(A2_QSH_DUMP_TAG_DL_PHY);
*/
#define QSH_DUMP_TAG_MASK_GET(dump_tag) \
  ((qsh_dump_tag_mask_t) (1 << ((uint32) dump_tag)))

/*!
  @brief
  Evaluates to qsh_dump_tag_mask_t with all bits set.

  @note
  This will include the reserved ANY bit, meaning clients are free 
  to choose which dumps tags to collect, up to and including all.
*/
#define QSH_DUMP_TAG_MASK_ANY \
  ((qsh_dump_tag_mask_t) (~((qsh_dump_tag_mask_t) 0)))
  
/*! 
  @brief
  Evaluates to qsh_dump_tag_mask_t with all bits except ANY bit
  set, indicating that all dump tags are required.
*/  
#define QSH_DUMP_TAG_MASK_ALL \
  ((qsh_dump_tag_mask_t) (QSH_DUMP_TAG_MASK_ANY & \
    (~QSH_DUMP_TAG_MASK_ANY_BIT)))
    
/*! 
  @brief
  Evaluates to qsh_dump_tag_mask_t with no bits set.
*/  
#define QSH_DUMP_TAG_MASK_NONE \
  ((qsh_dump_tag_mask_t) 0)
  
/*!
  @brief
  This should not be used directly.
  
  Bit reserved to indicate tag bitmask behavior is ANY. 
*/
#define QSH_DUMP_TAG_MASK_ANY_BIT \
  (QSH_DUMP_TAG_MASK_GET(QSH_DUMP_TAG_MAX_COUNT))

/*!
  @brief 
  Header for a single dump tag.
  
  @note
  To be included as the first field in each client dump tag struct.
*/
typedef PACK(struct)
{
  /*! set using dump_tag enum exported by client */
  uint16  dump_tag;
  
  /*! size of dump struct in bytes */
  uint16  dump_struct_size_bytes;
} qsh_dump_tag_hdr_s;

/*!
  @brief 
  Header for dump per client.
  
  @note
  This is written internally by QSH.
*/
#define QSH_DUMP_HDR_CLIENT_NAME_SIZE_BYTES   8
typedef struct
{
  /*! header cookie: QSH_COOKIE_32 */
  uint32  hdr_cookie;
  
  /*! client name string (including \0 terminator) */
  uint8   client_name[QSH_DUMP_HDR_CLIENT_NAME_SIZE_BYTES];
  
  /*! size of this struct */
  uint8   hdr_size_bytes;
  
  /*! major version */
  uint8   major_version;
  
  /*! minor version */
  uint8   minor_version;
  
  /*! reserved for cache alignment */
  uint8   reserved_0;
  
  /*! bytes allocated for client */
  uint32  allocated_bytes;
  
  /*! bytes written by client */
  uint32  written_bytes;

  /*! dump tag requested */
  uint32  tag_mask_requested;

  /*! reserved to make it 32 byte aligned */
  uint32  reserved_1;
} qsh_dump_hdr_s;


/* ----------------------------------------------------------------------------- 

  Types for action: QSH_ACTION_METRIC_CFG
  
----------------------------------------------------------------------------- */

/*! 
  @brief
  Type to represent metric_id. This should be set by metric enum 
  exported by client.
*/
typedef uint16 qsh_metric_id_t;

/*!
  @brief
  Type to represent a metric_id with no value.
*/
#define QSH_METRIC_ID_NONE  0xFFFF

/*!
  @brief  
  Enum for configuring action to perform.
    
  @note
  If there are duplicate or out-of-order START/STOP requests, QSH behavior 
  shall be undefined.
*/
typedef enum
{
  /*! start */
  QSH_METRIC_ACTION_START,
  
  /*! stop */
  QSH_METRIC_ACTION_STOP,
  
  /*! do not use */
  QSH_METRIC_ACTION_MAX
} qsh_metric_action_e;

typedef uint16 qsh_metric_size_t;

/*!
  @brief
  Represents a generic circular queue.
    
  @note
  All fields/structs are required to be manually initialized by client.
  If any fields are not explicitly set, QSH behavior shall be undefined.
    
  @note
  Current index represents next location to write to, i.e. oldest element.
*/
typedef struct
{
  /*! External module use only. QSH/clients shall not change this field. */
  uint16              ext_use;

  /*! size of each element in bytes */
  qsh_metric_size_t   element_size_bytes;
  
  /*! total number of elements */
  uint16              element_count_total;
  
  /*! current index into to the buffer in terms of elements 
      (wraps around at element_count_total) */
  uint16              element_cur_idx;
  
  /*! memory for metric FIFO (provided by ext client) */
  uint8 *             start_addr;
} qsh_metric_fifo_s;

/*!
  @brief
  Header for metric struct exported by client.
  To be included as the first field in each client metric struct.
*/
typedef struct
{
  uint32  timestamp;
} qsh_metric_hdr_s;

/* ----------------------------------------------------------------------------- 

  Action: QSH_ACTION_EVENT_CFG
  
----------------------------------------------------------------------------- */

/*!
  @brief  
  Type to represent event enum exported by client.
*/
typedef uint32 qsh_event_id_t;

/*!
  @brief
  Whether or not notification should be enabled for this event.
*/
typedef enum
{
  QSH_EVENT_ACTION_ENABLE,
  QSH_EVENT_ACTION_DISABLE
} qsh_event_action_e;

#endif /* QSH_TYPES_H */