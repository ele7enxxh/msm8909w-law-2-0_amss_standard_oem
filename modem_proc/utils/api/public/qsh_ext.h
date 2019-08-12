/*!
  @file
  qsh_ext.h
  
  Interface for external QSH usage.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/public/qsh_ext.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/28/16   ca      CR:968035: Adding event ID assert.
10/25/15   ca      CR:922311: Enabling cmapi metric collection.
02/13/15   mm      Changed interfaces for metric/event configuration
01/30/15   mm      Initial check-in                  
==============================================================================*/

#ifndef QSH_EXT_H
#define QSH_EXT_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <qsh_ext_types.h>
#include <qsh_types.h>

/*==============================================================================

                    EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/* Maximum number of events that can be configured. */
#define QSH_EXT_MAX_EVENT_ID 64

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/* -----------------------------------------------------------------------------

  Dump collection
  
----------------------------------------------------------------------------- */

/*==============================================================================

  FUNCTIONS:  qsh_ext_dump_collect_init
              qsh_ext_dump_collect

==============================================================================*/
/*!
  @brief
  Collects dump(s) for a client in sync mode (in error fatal context).
  
  @note
  qsh_ext_dump_collect_init() must be called on cfg struct array prior to usage
  or QSH behavior shall be undefined. Caller shall set the structure parameters
  after calling initialize.
 
  @note
  Client QSH_CLT_IMAGE *MUST* be called as first client per set of dump 
  collection.

  @note
  If "dump_tag_mask" is set to QSH_DUMP_TAG_MASK_ANY then client is free 
  to make a decision what to log and dump size shall at least be
  cumulative size of all tags supported by client + 
  QSH_EXT_DUMP_CLIENT_META_HDR_SIZE.

  @note
  'dump_iovec.addr' shall be 4 byte aligned address;
  'dump_iovec.size' shall be cumulative of QSH_EXT_DUMP_CLIENT_META_HDR_SIZE + 
   size of all the tags requested as per 'dump_tag_mask'

  @init
  qsh_ext_dump_collect_init()initialize dump_tag_mask/iovec size/iovec addr to 0
 
  @return
  qsh_ext_dump_collect_init:
    None
  qsh_ext_dump_collect:
    == SIZE_MAX   : insufficient buffer size
    < SIZE_MAX    : actual size written to the buffer for all the cfg tags
  
  For definition of SIZE_MAX, see <stdint.h>.
*/
/*============================================================================*/
void qsh_ext_dump_collect_init
(
  /*! cfg struct for dump collection for multiple tags */
  qsh_ext_dump_collect_s *  dump_collect_ptr
);

size_t qsh_ext_dump_collect
(
  /*! client */
  qsh_clt_e                 client,
  
  /*! cfg struct for dump collection for multiple tags */
  qsh_ext_dump_collect_s *  dump_collect_ptr
);

/* -----------------------------------------------------------------------------

  Metric configuration
  
----------------------------------------------------------------------------- */

/*==============================================================================

  FUNCTIONS:  qsh_ext_metric_cfg_init
              qsh_ext_metric_cfg

==============================================================================*/
/*!
  @brief
  Configures metrics per client.
  
  @note
  qsh_ext_metric_cfg_init() must be called on cfg struct array prior 
  to usage or QSH behavior shall be undefined. Caller shall set the  structure
  parameters after calling initialize.
  
  @note
  External client must manually configure all fields in each metric_cfg struct
  prior to usage or QSH behavior shall be undefined.
  To modify sampling period for a given metric, external module shall STOP 
  the metric first and then configure new one with needed parameters.
  Any reconfig (without STOP) of a given metric is NOT supported.

  @note
  memory pointed by 'qsh_ext_metric_cfg_s *' shall be preserved by external 
  module until metric is stopped and call back with metric action STOP is called
  for a given metric.

  @note
  External module is expected to use same metric_cfg ptr for START and STOP.
   
  @return
  qsh_ext_metric_cfg_init:
    None
  qsh_ext_metric_cfg
    None
*/
/*============================================================================*/
void qsh_ext_metric_cfg_init
(
  /*! array of pointers to metric_cfg structs */
  qsh_ext_metric_cfg_s *  metric_cfg_ptr_arr[],
  
  /*! array size */
  uint32                  metric_cfg_count
);

void qsh_ext_metric_cfg
(
  /*! client enum */
  qsh_clt_e               client,
  
  /*! array of pointers to metric_cfg structs */
  qsh_ext_metric_cfg_s *  metric_cfg_ptr_arr[],
  
  /*! array size */
  uint32                  metric_cfg_count
);

/*==============================================================================

  FUNCTION:  qsh_ext_metric_stop_register_cb

==============================================================================*/
/*!
  @brief
  Registers a callback for metric action STOP.

  @return
  None
*/
/*============================================================================*/
void qsh_ext_metric_stop_register_cb
(
  /*! metric callback */
  qsh_ext_metric_stop_cb_t   metric_cb
);

/*==============================================================================

  FUNCTIONS:  qsh_ext_metric_get_last_valid_entry

==============================================================================*/
/*!
  @brief
  Gets the last valid metric addr given the metric fifo info.
  
  @note
  This API SHALL only be called after atleast one metric has
  been successfully logged.
   
  @return
  void* : Address of the last valid metric entry
*/
/*============================================================================*/
void* qsh_ext_metric_last_valid_entry_get
(
   qsh_metric_fifo_s *metric_fifo
);

/* -----------------------------------------------------------------------------

  Event configuration
  
----------------------------------------------------------------------------- */

/*==============================================================================

  FUNCTIONS:  qsh_ext_event_cfg_init
              qsh_ext_event_cfg

==============================================================================*/
/*!
  @brief
  Configures events per client.
  
  @note
  qsh_ext_event_cfg_init() must be called on event_cfg_arr[] prior to usage 
  or QSH behavior shall be undefined.

  @note
  Event configuration is persistent, i.e. new calls to event_cfg() will not
  overwrite state from previous calls for different values of evenet_id.

  This implies the following:
  - events will need to be explicitly disabled by setting event_enable to 
    DISABLE
  - events already configured will be unchanged
  - if event being configured has already been configured, previous 
    configuration will be overwritten
 
  @return
  qsh_ext_event_cfg_init:
    None
  qsh_ext_event_cfg
    None
*/
/*============================================================================*/
void qsh_ext_event_cfg_init
(
  /*! array of pointers to event_cfg structs */
  qsh_ext_event_cfg_s *   event_cfg_ptr_arr[],
  
  /*! number of event_cfg structs */
  uint32                  event_cfg_count
);  

void qsh_ext_event_cfg
(
  /*! client enum */
  qsh_clt_e               client,
  
  /*! array of pointers to event_cfg structs */
  qsh_ext_event_cfg_s *   event_cfg_ptr_arr[],
  
  /*! number of event_cfg structs */
  uint32                  event_cfg_count
);

/*==============================================================================

  FUNCTION:  qsh_ext_event_register_cb

==============================================================================*/
/*!
  @brief
  Registers callback for all events from all clients.

  @note
  External module is expected to post a signal or command to its task for 
  processing a given event.
 
  @return
  None.
*/
/*============================================================================*/
void qsh_ext_event_register_cb
(
  /*! event callback */
  qsh_ext_event_cb_t  event_cb
); 

#endif /* QSH_EXT_H */
