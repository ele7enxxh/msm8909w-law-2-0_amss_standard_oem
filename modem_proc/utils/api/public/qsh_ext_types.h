/*!
  @file
  qsh_ext_types.h
  
  Types for external QSH usage. Common between external and internal clients.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/public/qsh_ext_types.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/17/15   mm      Updated qsh_ext_event_cfg_s
02/13/15   mm      Changed void * -> uint8 * for pointer addition
01/30/15   mm      Initial check-in                  
==============================================================================*/

#ifndef QSH_EXT_TYPES_H
#define QSH_EXT_TYPES_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <sys.h>
#include <qsh_types.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! header size that shall be considered for providing dump size for a given
    dump collection */
#define QSH_EXT_DUMP_CLIENT_META_HDR_SIZE sizeof(qsh_dump_hdr_s)

/* -----------------------------------------------------------------------------

  Dump collection
  
----------------------------------------------------------------------------- */

/*! 
  @brief
  Type to configure a dump collection.
*/
typedef struct
{
  /*! 
    @brief
    Bitmask of all tags for which dump is requested for the client.
   
    @note
    If the value is QSH_DUMP_TAG_MASK_ANY then client is free 
    to make a decision what to log and dump size shall at least be
    cumulative size of all tags supported by client.
 
    @note
    Supported max # of tags per client is QSH_DUMP_TAG_MAX_COUNT.
  */
  qsh_dump_tag_mask_t   dump_tag_mask;

  /*! 
    @brief
    Storage for requested dump tags (internally partitioned by QSH).
   
    @note
    The iovec shall at least hold cumulative size of tags requested in 
    the dump_tag_mask.
    'addr' shall be 4 byte aligned.
  */
  qsh_iovec_s           dump_iovec;
  
  /*! QSH internal use only. Client shall not change this field. */
  uint32                qsh_internal_use; 
} qsh_ext_dump_collect_s;

/* -----------------------------------------------------------------------------

  Metric configuration
  
----------------------------------------------------------------------------- */

/*!
  @brief
  Type to represent top-level metric configuration.
*/
typedef struct
{
  /*! metric id */
  qsh_metric_id_t         id;

  /*! action */
  qsh_metric_action_e     action;
  
  /*! subscriber id */
  sys_modem_as_id_e_type  subs_id;
  
  /*! flag to indicate when FIFO wraparound happens: if flag is FALSE, the
      first element of FIFO should be ignored */
  boolean                 wrap_around_flag;
  
  /*! QSH internal use only. Client shall not change this field; reserved for
      client. */
  uint8                   qsh_internal_use_0;
  
  /*! period in ms */
  uint16                  sampling_period_ms;
  
  /*! FIFO for elements */
  qsh_metric_fifo_s       fifo;

  /*! pointer to configuration specific to this metric */
  void *                  cfg_data_ptr;

  /*! QSH internal use only. Client/ext module shall not change this field. */
  uint32                  qsh_internal_use_1; 
  
  /*! QSH internal use only. Client/ext module shall not change this field. */
  uint32                  qsh_internal_use_2; 
} qsh_ext_metric_cfg_s;

/*! 
  @brief
  Callback type for external client to receive notification of metric actions.
*/
typedef void (*qsh_ext_metric_stop_cb_t)(qsh_ext_metric_cfg_s *);

/* -----------------------------------------------------------------------------

  Event configuration
  
----------------------------------------------------------------------------- */

/*! 
  @brief
  Type to represent event cfg.
*/
typedef struct
{
  /*! event_id */
  qsh_event_id_t      id;

  /*! whether or not to enable notification of the event */
  qsh_event_action_e  action;

  /*! event specific cfg ptr */
  void *              event_data;
  
  /*! QSH internal use only. Client shall not change this field. */
  uint32              qsh_internal_use; 
} qsh_ext_event_cfg_s;

/*! 
  @brief  
  Struct passed to event callback function registered by external client.
*/
typedef struct
{
  /*! client id */
  qsh_clt_e               client;
  
  /*! event id */
  qsh_event_id_t          id;
  
  /*! subs id */
  sys_modem_as_id_e_type  subs_id;
  
  /*! event specific data */
  void *                  event_data;
  
  /*! QSH internal use only. Client shall not change this field. */
  uint32                  qsh_internal_use; 
} qsh_ext_event_cb_params_s;

/*! 
  @brief
  Callback type for external client to receive notification of events.
*/
typedef void (*qsh_ext_event_cb_t)(qsh_ext_event_cb_params_s *);

#endif /* QSH_EXT_TYPES_H */
