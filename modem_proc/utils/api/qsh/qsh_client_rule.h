/*!
  @file
  qsh_client_rule.h

  @brief
  QSH  header file for handling rule functionality.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/qsh/qsh_client_rule.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/29/15   ca      CR:935478: cmapi report, mini dump state collet,
                   Re-arch of rule management validation.  
10/11/15   ca      CR:920884, Initial check in. QSH rule man phase 1, 2 and 3.            
==============================================================================*/

#ifndef QSH_CLIENT_RULE_H
#define QSH_CLIENT_RULE_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include <comdef.h>
#include <sys.h>
#include <stdlib.h>
#include <qsh_types.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! A macro to validate cfg / notify rule client handler */
#define QSH_CLIENT_RULE_VALIDATE_CLIENT_H(client)           \
  (client >= QSH_CLT_MAX) || (client == QSH_CLT_NONE) ||    \
  (client == QSH_CLT_ALL)  ||  (client == QSH_CLT_OTHER)
  
/*! A macro to validate assert rule client handler */
#define QSH_CLIENT_RULE_ASSERT_VALIDATE_CLIENT_H(client)    \
  (client >= QSH_CLT_MAX) || (client == QSH_CLT_ALL)  ||    \
  (client == QSH_CLT_OTHER)
  
/*! A macro to validate analysis rule client handler */
#define QSH_CLIENT_RULE_ANALYSIS_VALIDATE_CLIENT_H(client)    \
  (client >= QSH_CLT_MAX) || (client == QSH_CLT_NONE) ||      \
  (client == QSH_CLT_OTHER)
    
  
/*! A macro to validate add/delete rule client generator */
#define QSH_CLIENT_RULE_ADD_DEL_VALIDATE_CLIENT_G(client)    \
  (client >= QSH_CLT_MAX)|| (client == QSH_CLT_OTHER)||      \
  (client == QSH_CLT_ALL)
  
/*! A macro to validate help rule client generator */
#define QSH_CLIENT_RULE_HELP_VALIDATE_CLIENT_G(client)    \
  (client >= QSH_CLT_MAX)|| (client == QSH_CLT_OTHER)||   \
  (client == QSH_CLT_NONE)

#define QSH_CLIENT_RULE_TIME_TO_ACTION_0_US 0

#define QSH_CLIENT_RULE_SET_EXIST_ENUM(val, mask) \
	
#define QSH_CLIENT_RULE_CLEAR_EXIST_ENUM(val, mask) \
  
/*! Macro to define modwrap intrinsic function for Q6 and the regular mod func
    for off target */
#ifdef  FEATURE_QSH_ON_TARGET  
  #define QSH_CLIENT_RULE_MODWRAP(val, range) (val % range)
#else
  #define QSH_CLIENT_RULE_MODWRAP(val, range) (val % range)        
#endif /* FEATURE_QSH_ON_TARGET */
	
/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
/*==============================================================================
	
											INTERNAL DEFINITIONS AND TYPES
	
==============================================================================*/

/*! @brief struct for maintaing client, event and sub id related info.	*/
typedef struct
{
  /*! name of the client */
  qsh_clt_e               client;

  /*! event id */
  qsh_event_id_t          event_id;

  /*! subscriber id */
  sys_modem_as_id_e_type  subs_id;
} qsh_client_event_subs_id_s;

/*! type of rule */
typedef enum
{
  QSH_CLIENT_RULE_TYPE_ANALYSIS    = 0x0,
  QSH_CLIENT_RULE_TYPE_NOTIFY      = 0x1,
  QSH_CLIENT_RULE_TYPE_FLUSH       = 0x2,
  QSH_CLIENT_RULE_TYPE_ASSERT      = 0x3,
  QSH_CLIENT_RULE_TYPE_MAX         = 0x4,
  QSH_CLIENT_RULE_TYPE_DEFAULT     = 0xFFFFFFFF,
} qsh_client_rule_type;

/*! type of error */
typedef enum
{
  QSH_CLIENT_RULE_ERROR_IDX_NONE                  = 0x0,
  QSH_CLIENT_RULE_ERROR_IDX_INSUFFICIENT_ARGS     = 0x1,
  QSH_CLIENT_RULE_ERROR_IDX_INVALID_ACTION        = 0x2,
  QSH_CLIENT_RULE_ERROR_IDX_INVALID_RULE_TYPE     = 0x3,
  QSH_CLIENT_RULE_ERROR_IDX_INVALID_REF_COUNT     = 0x4,
  QSH_CLIENT_RULE_ERROR_IDX_INVALID_CLIENT_G      = 0x5, 
  QSH_CLIENT_RULE_ERROR_IDX_INVALID_SUB_ID_G      = 0x6,
  QSH_CLIENT_RULE_ERROR_IDX_INVALID_TIME_TO_ACTION = 0x7,
  QSH_CLIENT_RULE_ERROR_IDX_INVALID_CLIENT_H      = 0x8,
  QSH_CLIENT_RULE_ERROR_IDX_INVALID_SUB_ID_H      = 0x9,
  QSH_CLIENT_RULE_ERROR_IDX_DUP_RULE              = 0xA,
  QSH_CLIENT_RULE_ERROR_IDX_RULE_NOT_FOUND        = 0xB,  
  QSH_CLIENT_RULE_ERROR_IDX_ACTION_ADD_DISABLED    =0xC,
  QSH_CLIENT_RULE_ERROR_IDX_ACTION_DELETE_DISABLED =0xD,
  QSH_CLIENT_RULE_ERROR_IDX_ACTION_CFG_DISABLED    =0xE,
  QSH_CLIENT_RULE_ERROR_IDX_ACTION_HELP_DISABLED   =0xF,
  QSH_CLIENT_RULE_ERROR_IDX_EXCEED_MAX_RULES       =0x10,
  QSH_CLIENT_RULE_ERROR_IDX_INVALID_EVENT_DATA_TIME_TO_ACT       =0x11,
  QSH_CLIENT_RULE_ERROR_IDX_INVALID_FLUSH_MODULE    =0x12,
  QSH_CLIENT_RULE_ERROR_IDX_DEFAULT               = 0xFFFFFFFF,
} qsh_client_rule_error_idx_e;

/*! ANALYSIS_CMD: 
  rule_ref_count: No. of times this rule is in affect. The value 0xFFFFFFFF implies the
                  rule is effective forever. Min value for ref count is 1.
  client_g      : Generator client
  subs_id_g     : Generator subscription id
  event_g       : Event from the generator client
  time_to_notify_us :  Time interval in usec after which the analysis needs to be invoked on the client below.
                         0xFFFFFFFF implies data coming from generator client.
  client        : Client on which the analysis is invoked
  category_mask : The category on which the analysis has to be performed. 
                  Use enum qsh_cat_e values to define bit mask Example: QSH_CAT_CFG | QSH_CAT_PERF ... 
  reset_flag    : Flag to indicate if a client needs to reset stats after analysis. 
  periodicty_us  : Periodicity of invoking analysis on client after the first invoke (Not supported)
  periodicty_count : Count for the above periodicity (Not supported)
 
  QSHRequest add analysis <rule_ref_count> <client_g> <event_g> <subs_id_g> <time_to_analysis_us> <client> <category_mask> <reset_flag> <periodicty_us> <periodicty_count>
  QSHRequest delete analysis <client_g> <event_g> <subs_id_g> <time_to_analysis_us> <client> <category_mask> <reset_flag> <periodicty_us> <periodicty_count>
*/
typedef struct
{
  /*! name of the client on which the analysis needs to be run */
  qsh_clt_e                     client;

  /*! 
    category mask: use enum qsh_cat_e values to define bit mask
    Example: QSH_CAT_CFG | QSH_CAT_PERF ... 
      
    @note
    If category_mask == 0, client is expected to not perform analysis and
    reset stats.
  */
  uint32                        category_mask;
  
  /*!
    whether or not to reset stats after analysis. Client should act on this
    flag after processing category_mask.
  */
  boolean                       reset_flag;

  /*! periodicity in msec */
  uint32                        periodicty_us;

  /*! periodicity count*/
  uint32                        periodicty_count;

} qsh_client_rule_analysis_s;

/*! NOTIFY_CMD: 
  rule_ref_count: No. of times this rule is in affect.The value 0xFFFFFFFF implies the
                  rule is effective forever. Min ref ount is 1. 
  client_g      : Generator client
  subs_id_g     : Generator subscription id
  event_g       : Event from the generator client
  time_to_notify_us : Time interval in usec after which the handler client needs to be notified.
                      0xFFFFFFFF implies data coming from generator client.
  client_h      : Handler client
  subs_id_h     : Handler subscription id
  event_h       : Event raised on the handler client
  event_data_h  : Handler event data. 0xFFFFFFFF implies data coming from generator client
  QSHRequest add notify <rule_ref_count> <client_g> <event_g> <subs_id_g> <time_to_notify_us> <client_h> <event_id_h> <subs_id_h> <event_data_h> 
  QSHRequest delete notify <client_g> <event_g> <subs_id_g> <time_to_notify_us> <client_h> <event_id_h> <subs_id_h> <event_data_h> 
*/
typedef struct
{
  /*! handler client event info based on which the event is raised on the 
    handler client */
  qsh_client_event_subs_id_s  client_event_info_handler;
  /*! Handler event data */
  uint32                      event_data;
} qsh_client_rule_notify_s;

/*! DIAG_CMD: 
  rule_ref_count: No. of times this rule is in affect. The value 0xFFFFFFFF implies the
                  rule is effective forever. Min ref count is 1.
  client_g      : Generator client
  subs_id_g     : Generator subscription id
  event_g       : Event from the generator client
  time_to_flush_us : Time interval in usec after which the DIAG needs to be flushed if it is
                     in circuilar mode.
                     1) If the diag is in streaming mode then no op.
                     2) if time is 0 and diag mode is circuilar, then flush the diag.
                     3) if time is !0 and diag is in circuilar mode,then start the timer.
                     4) After the timer expiry, if the diag mode is circuuilar mode, flush the diag.   
                     6) 0xFFFFFFFF implies data coming from generator client.
  flush_module  : The module to be flushed (diag/ulog/All ...)
  flush_time_us :
   This is the time after which QSH configures DIAG from streaming to circuilar mode.
    1) If the value is 0, then no op
    2) If the value is non zero, then QSH sets the diag to streaming mode and then starts a timer.
    3) After the timer expiry, qsh sets the diag mode to circuilar mode.
    4) Max limit is 5 mins
    5) Its a no-op for ULOG
  QSHRequest add flush <rule_ref_count> <client_g> <event_g> <subs_id_g> <time_to_flush_us> <flush_module> <flush_time_us>
  QSHRequest delete flush <client_g> <event_g> <subs_id_g> <time_to_flush_us> <flush_module> <flush_time_us>
*/

/*! 
  enum to describe the modules to be flushed.
  Note: use this as bit mask. all the values should be power of 2.
*/
typedef enum
{
  /*! Flush all modules */
  QSH_CLIENT_RULE_FLUSH_MODULE_ALL  = 0x0,
  /*! Flush the diag module */
  QSH_CLIENT_RULE_FLUSH_MODULE_DIAG = 0x1,
  /*! Flush ulog module */
  QSH_CLIENT_RULE_FLUSH_MODULE_ULOG = 0x2,
  /* Max module */
  QSH_CLIENT_RULE_FLUSH_MODULE_MAX = 0x3,
}qsh_client_rule_flush_module_e;

/*! A struct to hold info about flush params */
typedef struct
{
  /*! The module to be flushed */
  qsh_client_rule_flush_module_e  module;

  /*! Flush time in usec. 
    1) This is the time when QSH configures DIAG from circular to streaming mode.
    2) If the value is 0 its just a one time flush
    3) Max limit is 5 mins
    4) Its a no-op for DIAG is its already in streaming mode.
    5) Its a no-op for ULOG
    */
  uint32                          flush_time_us;
} qsh_client_rule_flush_s;

/*!
ASSERT_CMD: 
  client_g      : Generator client
  subs_id_g     : Generator subscription id
  event_g       : Event from the generator client
  time_to_action_us :  time interval in usec after which the assert is invoked on the client below.
                         0xFFFFFFFF implies data coming from generator client.
  client        : Client on which the assert needs to be invoked. All clients implies error fatal.
  time_to_assert_us: Not supported.

  QSHRequest add assert <rule_ref_count> <client_g> <event_g> <subs_id_g> <time_to_action_us> <client> <time_to_assert_us> 
  QSHRequest delete assert <client_g> <event_g> <subs_id_g> <time_to_action_us> <client> <time_to_assert_us>
  
  todo: support DELETE ALL 
*/
typedef struct
{
  /*! Name of the client which needs to be stopped. If the client is QSH_CLT_ALL then 
    its equivalent to error fatal */
  qsh_clt_e                 client;

  uint32                    time_to_assert_us;
} qsh_client_rule_assert_s;

/*!
   @brief
    Union to hold rule parameters */
typedef union
{
  qsh_client_rule_analysis_s    analysis_params;
  qsh_client_rule_notify_s      notify_params; 
  qsh_client_rule_flush_s       flush_params; 
  qsh_client_rule_assert_s      assert_params; 
} qsh_client_rule_params_u;

typedef struct
{  
  /*! time interval in usec after which the action to be taken.
    0 implies instantaneously. Have a max value of 5 mins */
  uint32                      time_to_action_us;

  qsh_client_rule_type        type; 

  /*! generator client event info based on which the analysis needs to be run */
  qsh_client_event_subs_id_s  generator_info; 
  
  qsh_client_rule_params_u    params;

  /*! No. of times this rule is in affect. No-op for delete */
  uint32                      ref_count;  
  
  /*! QSH internal use only. Client shall not change this field. */
  uint32                      qsh_internal_use; 
 
} qsh_client_rule_s;

/*! An enum to describe the type of help requested by user */
typedef enum 
{
  /*! no help */
  QSH_CLIENT_HELP_NONE = 0x0,
  /*! display rules */
  QSH_CLIENT_HELP_DISPLAY = 0x1,
  /* Max number. Invalid arg. for internal use */
  QSH_CLIENT_HELP_MAX = 0xFFFFFFFF,
}qsh_client_help_e;

/*! A struct to gold the diplay help params */
typedef struct
{
  qsh_clt_e   client;
}qsh_client_help_display_params_s;

/* This is used to help the users about qsh rule management feature. 
   Help command is given from QXDM.
   QSHRequest help <type_of_help> <client> <arg1> <arg2> <arg3> <arg4> <arg5> 
   <arg6> <arg7>
   This displays all the rules installed or rules installed per client.

   Cmd: QSHRequest help <display> <client> <arg1> <arg2> <arg3> <arg4> <arg5> 
   <arg6> <arg7>

  E.g. QSHRequest help display dsm 0x0 0x0 0x0 0x0 0x0 0x0 0x0 
   - Displays all the rules installed for dsm client.*/

/*! A union to hold the help params */
typedef union
{
 qsh_client_help_display_params_s  display_params;
}qsh_client_help_params_u;

/*! A struct to hold the help info */
typedef struct
{
  /* Help type */
  qsh_client_help_e         help_type;

  /*Help params */
  qsh_client_help_params_u help_params;
  
}qsh_client_help_s;
/*==============================================================================

  FUNCTIONS:  qsh_client_rule_add_init

==============================================================================*/
/*!
  @brief: An api to initialize the client rule.
          Client rule is filled with default params.  
  @note
 
  @return
  error status.
*/
/*============================================================================*/
qsh_client_rule_error_idx_e qsh_client_rule_add_init
(
  qsh_client_rule_type  event_rule,
  qsh_client_rule_s     *client_rule_ptr
);

/*==============================================================================

  FUNCTIONS: qsh_client_rule_add

==============================================================================*/
/*!
  @brief: an api to add a rule. This is called from diag module.
  
  @note
 
  @return
  error status.
*/
/*============================================================================*/
qsh_client_rule_error_idx_e qsh_client_rule_add
(
  qsh_client_rule_s     *client_rule_ptr
);

/*==============================================================================

  FUNCTIONS: qsh_client_rule_delete

==============================================================================*/
/*!
  @brief: an api to delete a rule. This is called from diag module.
  
  @note
 
  @return
  Error Status.
*/
/*============================================================================*/
qsh_client_rule_error_idx_e qsh_client_rule_delete
(
  qsh_client_rule_s     *client_rule_ptr
);

/*==============================================================================

FUNCTIONS: qsh_client_rule_print

==============================================================================*/
/*!
  @brief
  Print the content of the clien_rule in ulog
  
  @note
  
  @return
  None.
*/
/*============================================================================*/
void qsh_client_rule_print
(
   qsh_client_rule_s* client_rule,
   const char* str
);

/*==============================================================================

  FUNCTIONS: qsh_client_rule_help

==============================================================================*/
/*!
  @brief: an api to execute different help options.

  @note
 
  @return
  error status.
*/
/*============================================================================*/
qsh_client_rule_error_idx_e qsh_client_rule_help
(
  qsh_client_help_s     *client_help_ptr
);

#endif /* QSHI_CLIENT_RULE_H */
