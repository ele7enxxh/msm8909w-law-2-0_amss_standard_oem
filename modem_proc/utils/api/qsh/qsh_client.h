/*!
  @file
  qsh_client.h

  @brief
  Interface for QSH internal clients.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/qsh/qsh_client.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/11/15   ca      CR 920884: QSH rule management feature.
02/16/15   mm      Updated interface for qsh_client_metric_log_done()
02/16/15   mm      Interface changes
02/13/15   mm      Added union to pass struct per action back to QSH in 
                   qsh_client_cb_done()
02/12/15   mm      Added qsh_client_dump_tag_hdr_init()
01/22/15   mm      Initial check-in              
==============================================================================*/

#ifndef QSH_CLIENT_H
#define QSH_CLIENT_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <qsh_types.h>
#include <qsh_ext_types.h>
#include <qsh_util.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/* ----------------------------------------------------------------------------- 

  Action: QSH_ACTION_ANALYSIS
  
----------------------------------------------------------------------------- */

/*! 
  @brief
  Logging with up to 10 arguments. Max size is 1000 bytes. 
    1st arg: Enum for client
    2nd arg: Enum for category
    3rd arg: Enum for message type
    4th arg: Summary for the logging
    >= 5th arg: Arguments for the logging (optional)
*/
#define QSH_LOG(client, category, msg_type, summary, ...) \
  QSHI_LOG(client, category, msg_type, summary, ##__VA_ARGS__)

/*! 
  @brief
  Macro to handle simple "if" checks. This will maintain a running 
  count for the summary at the end of the analysis.
    1st arg: Expression (will write log if this evaluates to TRUE)
    2nd arg  Enum for client
    3rd arg: Enum for category
    4th arg: Enum for message type
    5th arg: Summary for the logging
    >= 6th arg: Arguments for the logging (optional)
*/  
#define QSH_CHECK_LOG(expr, client, category, msg_type, summary, ...) \
  QSHI_CHECK_LOG(expr, client, category, msg_type, summary, ##__VA_ARGS__)

/*!
  @brief
  Configures action.
*/
typedef struct
{
  /*! 
    Category mask: use enum qsh_cat_e values to define bit mask
    Example: QSH_CAT_CFG | QSH_CAT_PERF ... 
      
    @note
    If category_mask == 0, client is expected to not perform analysis and
    reset stats.
  */
  uint32    category_mask;
  
  /*!
    Whether or not to reset stats after analysis. Client should act on this
    flag after processing category_mask.
  */
  boolean   reset_flag;
} qsh_client_analysis_s;

/* ----------------------------------------------------------------------------- 

  Action: QSH_ACTION_DUMP_COLLECT
  
----------------------------------------------------------------------------- */

/*!
  @brief
  Configures action.
*/
typedef qsh_ext_dump_collect_s qsh_client_dump_collect_s;

/*!
  @brief
  Values passed back to QSH.
*/
typedef struct
{
  /*! total size written in bytes, or SIZE_MAX if not enough space given */
  size_t  size_written_bytes;
} qsh_client_dump_collect_done_s;

/* ----------------------------------------------------------------------------- 

  Action: QSH_ACTION_METRIC_TIMER_EXPIRY
  
----------------------------------------------------------------------------- */

typedef struct
{
  /*! metric id */
  qsh_metric_id_t         metric_id;
  
  /*! subscriber id */
  sys_modem_as_id_e_type  subs_id;

  /*! time since last expiry, or time since metric start if first call */
  uint16                  time_elapsed_ms;
} qsh_client_metric_timer_expiry_params_s;

/*!
  @brief
  Configures action.
*/
#define QSH_CLIENT_METRIC_TIMER_EXPIRY_ARR_MAX   16
typedef struct
{
  /*! number of metrics being configured 
    (must be 0 <= n < QSH_CLIENT_METRIC_TIMER_EXPIRY_ARR_MAX) */
  uint32                                    metric_id_count;
  
  /*! array of param structs */
  qsh_client_metric_timer_expiry_params_s   params[
    QSH_CLIENT_METRIC_TIMER_EXPIRY_ARR_MAX];
} qsh_client_metric_timer_expiry_s;

/* ----------------------------------------------------------------------------- 

  Action: QSH_ACTION_METRIC_CFG
  
----------------------------------------------------------------------------- */

/*!
  @brief
  Type used to identify metric config from ext clients.
*/
typedef void * qsh_client_metric_context_id_t;

/*!
  @brief
  Enum used to indicate the cause of the metric log.
*/
typedef enum
{
  /*! timer expiry */
  QSH_CLIENT_METRIC_LOG_REASON_TIMER_EXPIRY,
  
  /*! event internal to client */
  QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL,
  
  /*! event external to client (for future use) */
  QSH_CLIENT_METRIC_LOG_REASON_EVENT_EXTERNAL
} qsh_client_metric_log_reason_e;

/*!
  @brief
  Struct used to indicate that a metric was logged.
*/
typedef struct 
{
  /*! client */
  qsh_clt_e                       client;
  
  /*! metric_id */
  qsh_metric_id_t                 metric_id;
  
  /*! this comes from cb params for action QSH_ACTION_METRIC_CFG */
  qsh_client_metric_context_id_t  metric_context_id;

  /*! reason for log: set ONLY if reason is other than timer expiry */
  qsh_client_metric_log_reason_e  log_reason;
  
  /*! QSH internal use only. Set within qsh_client_metric_log_done_init().
      Client shall not change this field. */
  uint32                          cookie_qsh_internal_use; 
} qsh_client_metric_log_done_s;

/*!
  @brief
  Configures action.
*/
typedef struct
{
  /*! metric id */
  qsh_metric_id_t                 id;

  /*! action */
  qsh_metric_action_e             action;
  
  /*! period in ms */
  uint16                          sampling_period_ms;
  
  /*! subscriber id */
  sys_modem_as_id_e_type          subs_id;
  
  /*! pointer to configuration specific to this metric */
  void *                          cfg_data_ptr;

  /*! start address for metric */
  uint8 *                         start_addr;
  
  /*! metric size in bytes */
  uint16                          size_bytes;

  /*! metric context id that the client needs to pass into 
      qsh_client_metric_log_done(...) after writing a metric */
  qsh_client_metric_context_id_t  metric_context_id;
} qsh_client_metric_cfg_s;

/* ----------------------------------------------------------------------------- 

  Action: QSH_ACTION_EVENT_CFG
  
----------------------------------------------------------------------------- */

/*! 
  @brief
  Struct for data passed back to external client upon an event.
*/
typedef qsh_ext_event_cb_params_s qsh_client_event_notify_params_s;

/*!
  @brief
  Configures action.
*/
typedef qsh_ext_event_cfg_s qsh_client_event_cfg_s;
   
/*!
  @brief  
  Parameters passed to QSH during rule configuration from QXDM.
*/
typedef struct
{
  uint32   cmd_code;
  uint32   cfg_data_1;
  uint32   cfg_data_2;
  uint32   cfg_data_3;
  uint32   cfg_data_4;
  uint32   cfg_data_5;
  uint32   cfg_data_6;
  uint32   cfg_data_7;
  uint32   cfg_data_8;
} qsh_client_cfg_s;
   
/* ----------------------------------------------------------------------------- 

  Callback registration/params
  
----------------------------------------------------------------------------- */
typedef qsh_client_event_notify_params_s qsh_client_event_notify_s;
/*!
  @brief
  Represents configuration per action, if the corresponding bitmask is set
  in callback params.    
*/
typedef union
{
  /*! config for QSH_ACTION_ANALYSIS */
  qsh_client_analysis_s             analysis;
  
  /*! config for QSH_ACTION_DUMP_COLLECT */
  qsh_client_dump_collect_s         dump_collect;
  
  /*! config for QSH_ACTION_METRIC_TIMER_EXPIRY */
  qsh_client_metric_timer_expiry_s  metric_timer_expiry;
  
  /*! config for QSH_ACTION_METRIC_CFG */
  qsh_client_metric_cfg_s           metric_cfg;
  
  /*! config for QSH_ACTION_EVENT_CFG */
  qsh_client_event_cfg_s            event_cfg;

  /*! config for QSH_ACTION_EVENT_NOTIFY */
  qsh_client_event_notify_s         event_notify;

  /*! config for QSH_ACTION_CFG. */
  qsh_client_cfg_s                  cfg;
} qsh_client_action_params_u;

/*!
  @brief
  Parameters passed into the callback functions registered with QSH using 
  the function qsh_client_reg(...). 
*/
typedef struct 
{
  /*! client */
  qsh_clt_e                   client;

  /*! action indicating whether to run analysis, dump, metric cfg ... */   
  qsh_action_e                action;
  
  /*! configuration for the action indicated in the above field */
  qsh_client_action_params_u  action_params;
  
  /*! whether the registered cb can run in sync/async mode */
  qsh_action_mode_e           action_mode;

  /*! unique value for each run - should be later passed in 
      qsh_client_cb_done() after processing indicated action */
  qsh_context_id_t            context_id;
} qsh_client_cb_params_s;

/*!
  @brief Callback function for client to use to handle QSH action(s).
 
  @return
  None
*/
typedef void (*qsh_client_cb_t) (qsh_client_cb_params_s *);

/*!
  @brief 
  Default metric info from the client..
*/
typedef struct 
{
  /*! array of metric_cfg structs. memory for the array of
    'qsh_ext_metric_cfg_s' shall be preserved by client from power up.
    @note: Client SHALL init qsh_ext_metric_cfg_s array using
    qsh_client_metric_cfg_init(...)and then SHALL set only id, subs_id
    sampling_period_ms, fifo.element_size_bytes and fifo.element_count_total
    in the struct qsh_ext_metric_cfg_s */
  qsh_ext_metric_cfg_s *  metric_cfg_arr_ptr;
  
  /*! num of valid elements in the array */
  uint32                  metric_cfg_count;
} qsh_client_metric_info_s;

/*!
  @brief 
  Event info from the client..
*/
typedef struct
{
  /*! bitmask of supported event ids */
  uint32  support_mask;
} qsh_client_event_info_s;

/*!
  @brief 
  Dump info from the client..
*/
typedef struct
{
  /*! total size for all dump tags in bytes */
  uint32  max_size_bytes;
} qsh_client_dump_info_s;

/*!
  @brief 
  Parameters for client callback registration.
*/
typedef struct
{
  /*! client registering for the callback */
  qsh_clt_e                         client;
  
  /*! callback for client */
  qsh_client_cb_t                   client_cb_ptr;
  
  /*! major version for client CB (must be non-zero) */
  uint8                             major_ver; 
  
  /*! minor version for client CB */
  uint8                             minor_ver;
  
  /*! Client should set any action bitmasks for which it expects its callback
      to be invoked for a indicated action(s).
      OR qsh_action_e values to set mask if multiple actions are supported like
      QSH_ACTION_ANALYIS | QSH_ACTION_DUMP | ... */
  qsh_action_mask_t                 cb_action_support_mask;

  /*! get the default metric info from the client. */
  qsh_client_metric_info_s          metric_info;
  
  /*! get the event info from the client. */
  qsh_client_event_info_s           event_info;

  /*! get the dump info from the client. */
  qsh_client_dump_info_s            dump_info;

  /*! QSH internal use only. Set within qsh_client_reg_init().
      Client shall not change this field. */
  uint32                            cookie_qsh_internal_use; 
} qsh_client_reg_s;

/* ----------------------------------------------------------------------------- 

  Callback done indication
  
----------------------------------------------------------------------------- */

/*!
  @brief
  Union to contain any values per action that need to be passed back to QSH.
*/
typedef union
{ 
  /*! return values for QSH_ACTION_DUMP_COLLECT */
  qsh_client_dump_collect_done_s  dump_collect;
} qsh_client_action_done_params_u;

/*!
  @brief  
  Parameters passed to QSH when client has finished processing its callback.
*/
typedef struct
{
  /*! pointer to same params with which the client's CB was invoked
     for one of the supported actions */
  qsh_client_cb_params_s *          cb_params_ptr;
  
  /*! return values per action, if applicable for action 
      set in cb_params_ptr */
  qsh_client_action_done_params_u   params;
  
  /*! whether cb happened in SYNC or ASYNC mode */
  qsh_action_mode_done_e            action_mode_done;
  
  /*! QSH internal use only. Set within qsh_client_cb_done_init().
      Client shall not change this field. */
  uint32                            cookie_qsh_internal_use;
} qsh_client_action_done_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTIONS:  qsh_client_reg_init
              qsh_client_reg

==============================================================================*/
/*!
  @brief
  Initializes client struct and registers client set by user. 
  
  @note
  If either of these functions is not called in the correct order, QSH 
  behavior shall be undefined.
 
  @return
  None.
*/
/*============================================================================*/
void qsh_client_reg_init
(
  qsh_client_reg_s *  client_reg_ptr
);

void qsh_client_reg
(
  qsh_client_reg_s *  client_reg_ptr
);

/*==============================================================================

  FUNCTION:  qsh_client_dump_tag_hdr_init

==============================================================================*/
/*!
  @brief 
  Initializes the dump tag header.
  
  @note
  If client does not call this function on the dump tag, QSH behavior shall be
  undefined.
*/
/*============================================================================*/
static inline void qsh_client_dump_tag_hdr_init
(
  /*! pointer to dump tag header */
  qsh_dump_tag_hdr_s *  dump_tag_hdr_ptr,
  
  /*! dump tag enum */
  qsh_dump_tag_t        dump_tag,
  
  /*! size of dump struct in bytes (provide using sizeof()) */
  size_t                dump_struct_size_bytes
)
{
  dump_tag_hdr_ptr->dump_tag = (uint16) dump_tag;
  dump_tag_hdr_ptr->dump_struct_size_bytes = (uint16) dump_struct_size_bytes;
}

/*==============================================================================

  FUNCTION:  qsh_client_metric_cfg_init

==============================================================================*/
/*!
  @brief
  Initializes an array of metric cfg structs.
*/
/*============================================================================*/
void qsh_client_metric_cfg_init
(
  /*! array of metric cfg structs */
  qsh_ext_metric_cfg_s  metric_cfg_arr[],
  
  /*! size of array */
  uint32                metric_cfg_count
);

/*==============================================================================

  FUNCTIONS:  qsh_client_metric_log_done_init
              qsh_client_metric_log_done

==============================================================================*/
/*!
  @brief
  Function to log the given client/metric and maintain FIFO.
 
  @note
  If init() is not called first, QSH behavior shall be undefined.
 
  @return
  Address where the next metric needs to be logged. 
*/
/*============================================================================*/
void qsh_client_metric_log_done_init
(
  /*! pointer to metric_log_done struct */
  qsh_client_metric_log_done_s *  log_done_ptr
);

uint8 * qsh_client_metric_log_done
(
  /*! pointer to metric_log_done struct */
  qsh_client_metric_log_done_s *  log_done_ptr
);

/*==============================================================================

  FUNCTIONS:  qsh_client_action_done_init
              qsh_client_action_done

==============================================================================*/
/*!
  @brief
  Initializes cb_done params and provides interface for client to indicate 
  when its callback is complete for a given action. 
  
  @note
  If either of these functions is not called in the correct order, QSH 
  behavior shall be undefined.
 
  @return
  None.
*/
/*============================================================================*/
void qsh_client_action_done_init
(
  /*! action done ptr */
  qsh_client_action_done_s *  action_done_ptr
);

void qsh_client_action_done
(
  /*! action done ptr */
  qsh_client_action_done_s *  action_done_ptr
);

/*==============================================================================

  FUNCTION: qsh_client_event_notify_init
            qsh_client_event_notify

==============================================================================*/
/*!
  @brief
  Indicates to QSH to to notify external client of this client/event.
  
  @note
  If either of these functions is not called in the correct order, QSH 
  behavior shall be undefined.
 
  @return
  None.
*/
/*============================================================================*/
void qsh_client_event_notify_init
(
  qsh_client_event_notify_params_s *  event_notify_params_ptr
);

void qsh_client_event_notify
(
  qsh_client_event_notify_params_s *  event_notify_params_ptr
);

/*==============================================================================

  FUNCTIONS:  qsh_client_cfg_init

==============================================================================*/
/*!
  @brief
   
  
  @note
  If either of these functions is not called in the correct order, QSH 
  behavior shall be undefined.
 
  @return
  Error Status.
*/
/*============================================================================*/
qsh_client_rule_error_idx_e qsh_client_cfg_init
(
  qsh_client_cfg_s *client_cfg
);

/*==============================================================================

  FUNCTIONS:  qsh_client_cfg

==============================================================================*/
/*!
  @brief
   
  
  @note
  If either of these functions is not called in the correct order, QSH 
  behavior shall be undefined.

	DIAG_CMD: 
		client: Name of the client that needs to be configured
		cmd_code: The client should identify the type of cfg using cmd_code.
                          The cmd_code for different cfgs are defined by client. 
		client_cfg_data.*: Data that needs to be passed in to the client 
		for configuration. 
		QSHRequest cfg <client> <cmd_code> <client_cfg_data_1> 
		<client_cfg_data_2> <client_cfg_data_3> <client_cfg_data_4> 
		<client_cfg_data_5> <client_cfg_data_6> <client_cfg_data_7> 
		<client_cfg_data_8>
		Note: Clients can use to configure conditions/parameters based on which 
		the generator needs to raise event 
 
  @return
  Error Status.
*/
/*============================================================================*/
qsh_client_rule_error_idx_e qsh_client_cfg
(
   /*! client which needs to be configured */
  qsh_clt_e              client,
  qsh_client_cfg_s      *client_cfg
);

#endif /* QSH_CLIENT_H */
