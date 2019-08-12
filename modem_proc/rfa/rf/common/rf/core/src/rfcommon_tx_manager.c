/*!
  @file
  rfcommon_tx_manager.c

  @brief
  This module contains interface and implementation of common Tx 
  resource manager.
  
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_tx_manager.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/30/14   sar     Fixed compiler warning in tx_mgr_stm_debug_hook. 
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
04/26/13   aro     F3 message update to match TxMgr log parser
04/17/13   aro     Cleaned up unused time profiler code
04/04/13   aro     Added ERR_FATAL for STM2 error
04/03/13   aro     Converted STM2 error handler function to static
04/03/13   aro     Handler function to catch STM2 error
04/03/13   aro     Added Debug Message
03/26/13   aro     Renamed interface to indicate xpt_resource
03/26/13   aro     MC Callflow illustration
03/11/13   sar     Updated file for APQ, GNSS only target.
02/21/13   aro     Implemented LM allocation and deallocation for slave resource
02/04/13   aro     Migrated the variable to enable/disable slave resource
                   to RF common data
02/04/13   aro     Added variable to skip Tx Manager
01/23/13   aro     Warning fix
01/23/13   aro     Debug interface to enable and disable slave resource
01/11/13   aro     Fixed the release of critical section
01/03/13   aro     Expanded debug logging to store the waiting client data
01/03/13   aro     Interface time profiling
01/03/13   aro     Error checking for tx manager initialization
12/28/12   aro     Added cleanup() call during SLAVE to OFF transition
12/27/12   aro     Documentation Update
12/27/12   aro     F3 update
12/27/12   aro     Added off-target helper function to query STM state
12/27/12   aro     Fixed invalid check in tx manager init
12/27/12   aro     Compiler warning fix
12/27/12   aro     Doxygen update
12/27/12   aro     Fixed reseting of waiting data during
                   tx_mgr_stm_master_pslave_proc_s_release
12/26/12   aro     Debug hook to log STM transition trace
12/26/12   aro     Added logging of error upon entry to ERROR state
12/26/12   aro     Support for forced ERROR state entry
12/26/12   aro     Update in interface function to support multiple instance of
                   state machine
12/26/12   aro     Implemented per Tx resource STM instance to have easy future
                   STM expansion
12/21/12   aro     Interface to release slave tx resource
12/21/12   aro     Interface to request slave tx resource
12/21/12   aro     Interface to release tx resource
12/21/12   aro     Transition function implemenmtation for SLAVE_RELEASE
                   input
12/21/12   aro     Transition function implemenmtation for SLAVE_ACQUIRE
                   input
12/21/12   aro     Added entry function for STM error state
12/21/12   aro     Transition function implemenmtation for MASTER_RELEASE
                   input
12/21/12   aro     Added common internal helper functions for data copy
                   and reset
12/21/12   aro     Added unrecoverable ERROR state
12/21/12   aro     Transition function implemenmtation for MASTER_ACQUIRE
                   input
12/21/12   aro     Implemented compressed state machine
12/21/12   aro     Added Tx resource data
12/19/12   aro     Documentation update
12/19/12   aro     Interface to request tx resource
12/18/12   aro     Fixed child STM initialization
12/18/12   aro     Added transition functions for all valid states and inputs
12/18/12   aro     Transition function for res0_X_res1_X state upon the receipt 
                   of RES0_M_ACQUIRE input
12/18/12   aro     Added critical section initialization
12/18/12   aro     Interface to initialize Tx resource manager
12/18/12   aro     Added debug hook function for STM2
12/18/12   aro     Added MCS STM2 compatible data structure for Tx manager
                   state machine
12/17/12   aro     Initial version

==============================================================================*/

#include "stringl.h" /* for memscpy */
#include "comdef.h"
#include "rfcommon_tx_manager.h"
#include "rfcommon_msg.h"
#include "rfcommon_locks.h"
#include "rfcommon_time_profile.h"
#include "modem_mem.h"
#include "stm2.h"

/*============================================================================*/
/*!
  @addtogroup RF_COMMON_TX_MGR_STM
 
  @brief
  This module includes the state machine implementaion of Tx resource manager
  based on MCS STM2 design. Multiple State machine instances are created
  per tx resource to be managed. These multiple state machine instances are
  allowed run in parallel.
 
  @image html rfcommon_tx_manager_stm2_callflow.jpg
  @image html rfcommon_tx_manager_stm_compressed.jpg
  @image html rfcommon_tx_manager_stm.jpg
  @{
*/

/*----------------------------------------------------------------------------*/
/*! Number of state mahcine instances. Each instance is created for a Tx 
resource */
#define TX_MGR_STM_NUM_INSTANCES 2

/*----------------------------------------------------------------------------*/
/*! List of valid state that Tx Manager State Machine can be in */
typedef enum
{
  TX_STATE_OFF = 0,
  /*!< Tx resource is not owned by any client */

  TX_STATE_MASTER = 1, 
  /*!< Tx resource is owned a as MASTER device by the client */

  TX_STATE_MASTER_PSLAVE = 2,
  /*!< Tx resource is owned a as MASTER device by the client while the 
  same resource has a pending slave request from other client */

  TX_STATE_SLAVE = 3,
  /*!< Tx resource is owned a as SALVE device by the client */

  TX_STATE_ERROR = 4,
  /*!< State indicating that error is encountered. The STM will be stuck in 
  this state forever. */

  TX_STATE_NUM
  /*!< Enumeration to indicate maximum number of states */
  
} tx_mgr_stm_states_type;

/*----------------------------------------------------------------------------*/
/*! Valid state machine inputs for Tx manager state machine */
typedef enum
{

  TX_INPUT_M_ACQUIRE, /*!< Acquire Resource as master resource */

  TX_INPUT_M_RELEASE, /*!< Release Resource as master resource */

  TX_INPUT_S_ACQUIRE, /*!< Acquire Resource as slave resource */

  TX_INPUT_S_RELEASE, /*!< Release Resource as slave resource */

  TX_INPUT_ERROR, /*!< Move the state machine to error state */

  TX_INPUT_NUM /*!< Enumeration to indicate maximum number of inputs */

} tx_mgr_stm_inputs_type;

/*============================================================================*/
/*!
  @name Tx Resource Manager Data container

  @brief
  This section contains definitions of Tx manager data container.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! Structure to keep track of the data received from Client */
typedef struct
{
  rfm_device_enum_type device;
  /*!< RFM device associated with the client data  */

  rfm_mode_enum_type mode;
  /*!< RFM Mode associated with the client data  */

  boolean slave_txlm_allocated;
  /*!< Flag indicating, if the LM Handle is allocated */

  lm_handle_type slave_txlm_handle;
  /*!< Opaque handle type for link manager buffers */

  rf_tx_resource_mgr_config_data_type res_config;
  /*!< Structure holding the data passed by clients during slave tx resource 
  request  */

} tx_mgr_client_data_type;

/*----------------------------------------------------------------------------*/
/*! List of enumeration to indicate the reason for ERROR state */
typedef enum
{
  TX_MGR_ERROR_NO_ERROR = 0,
  /*!< Enumeration indicating that there is no Error */

  TX_MGR_ERROR_MISMATCH_TX_RESOUCE,
  /*!< Enumeration inidcating that the STM Tx resource does not match the 
  TxLM Modem Chain. This means that STM may need to expand to have support 
  for new TxLM Modem Chain */ 

} tx_mgr_error_reason_enum;

/*----------------------------------------------------------------------------*/
/*! Structure to STM dump data when the state machine enters the 
unrecoverable ERROR state  */
typedef struct
{
  tx_mgr_error_reason_enum err_reason;
  /*!< Reason indicating the reason for error */

  tx_mgr_stm_states_type last_state;
  /*!< Last state before transition to ERROR state */

} tx_mgr_error_dump_type;

#define TX_MGR_TRAN_LOG_BUFF_LEN 15

/*----------------------------------------------------------------------------*/
/*! Structrure to track the state transition log */
typedef struct
{
  uint8 log_index;
  /*!< Variable to keep track of the index of latest entry in the log 
  table */

  struct
  {
    rf_time_tick_type time_tick;
    /*!< RF time snapshot at which the state transition is logged */

    rf_time_type elapsed_time_ms;
    /*!< Time elapsed in milliseconds since last state transition */

    rfm_device_enum_type device;
    /*!< RFM device pertaining to the state machine */

    rfm_mode_enum_type mode;
    /*!< RFM mode pertaining to the state machine */

    tx_mgr_stm_states_type s_state;
    /*!< Starting STM state */

    tx_mgr_stm_inputs_type tran_input;
    /*!< Input which triggered the state transition */

    tx_mgr_stm_states_type e_state;
    /*!< Ending STM state */

    rfm_mode_enum_type w_device;
    /*!< RFM device for the waiting client */

    rfm_mode_enum_type w_mode;
    /*!< RFM mode for the waiting client */

  } trace[ TX_MGR_TRAN_LOG_BUFF_LEN ];
  /*!< Circular buffer to store the state transition. Latest entry in this 
  buffer is indicated by log_index */

} tx_mgr_state_tran_log_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold STM debug data  */
typedef struct
{
  tx_mgr_error_dump_type error_dump;
  /*!< Structure to STM dump data when the state machine enters the 
  unrecoverable ERROR state */

  tx_mgr_state_tran_log_type tran_log;
  /*!< Structrure to track the state transition log */

} tx_mgr_debug_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data for each Tx resource  */
typedef struct
{

  tx_mgr_client_data_type active_client;
  /*!< Resource Information pertaining to the active client */

  tx_mgr_client_data_type waiting_client;
  /*!< Resource Information pertaining to the waiting client. This will 
  used when the waiting client is promoted to active */

  tx_mgr_debug_data_type debug_data;
  /*!< Structure to hold STM debug data */

} rf_tx_resource_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure holding the input parameter. This is basically the param 
sent by client */
typedef struct 
{

  rfm_device_enum_type device;
  /*!< RFM device associated with the client data  */
  
  rfm_mode_enum_type mode;
  /*!< RFM Mode associated with the client data  */

  txlm_chain_type modem_chain;
  /*!< Modem Chain */
 
  rf_tx_resource_mgr_config_data_type res_config;
  /*!< Structure holding the data passed by clients during slave tx resource 
  request */

  boolean enable_f3;
  /*!< Flag to indicate if the logging is enabled to print F3 */

  tx_mgr_error_reason_enum error_reason;
  /*!< If the STM is to be forced to go to ERROR state, then this field 
  is used to push the reason for error. */

} rf_tx_resource_input_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure to pack the data to be passed into the STM */
typedef struct
{
  rf_tx_resource_input_param_type input_parameter;
  /*!< Structure holding the input parameter. This is basically the param 
  sent by client and internal data to be pushed to STM */

  rf_tx_resource_data_type *resource_data;
  /*!< Pointer to the internal data structure holding the  */

} rf_tx_resource_stm_payload_type;

/*----------------------------------------------------------------------------*/
/*! Structure to store Tx resouce manager data */
typedef struct
{
  boolean init_done;
  /*!< Flag indicating if the Tx resource manager block is initialized */

  rf_lock_data_type critical_sect;
  /*!< Critical section protection to used during perform Tx resource 
  management */

  boolean enable_f3;
  /*!< Flag to indicate if F3 logging is to be enabled */

  rf_tx_resource_data_type resource_data[TX_MGR_STM_NUM_INSTANCES];
  /*!< Structure to hold data for each Tx resource  */

  boolean skip_tx_mgr;
  /*!< Variable to skip Tx Manager block. This is used for debugging purpose 
  only. By default, this will be set to FALSE */
      
} tx_mgr_data_type;

/*----------------------------------------------------------------------------*/
/*! Variable to keep track of Tx resource manager data */
static tx_mgr_data_type tx_mgr_data;

/*! @} */

/*============================================================================*/
/*!
  @name LM function

  @brief
  This section contains internal helper functions to be used for 
  handling Tx resource manager data
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Allocate LM
 
  @details
  This function is called the STM transition function to perform LM allcoation
 
  @param client_data
  Pointer to the client data for which LM allocation is to be done
 
  @return
  Status of LM allocation
*/
static boolean
tx_mgr_allocate_lm
(
  tx_mgr_client_data_type *client_data
)
{
  boolean ret_val = TRUE; /* Return value for the allocate function */
  rf_tx_resource_mgr_init_ret_type init_ret; /* Container to hold the return
  from init function */

  if ( client_data == NULL )
  {
    RF_MSG( RF_ERROR, "[TX_RESOURCE_MGR][LM][ALLOCATE]: NULL Client" );
    return FALSE;
  } /* if ( client_data == NULL ) */

  /* Init only if txlm is not already allocated */
  if ( client_data->slave_txlm_allocated == FALSE )
  {
    /* NULL pointer check */
    if ( client_data->res_config.func_ptrs.init_fp != NULL )
    {
      /* call function pointer to perform initialization */ 
      init_ret = client_data->res_config.func_ptrs.init_fp( 
                                  client_data->device,
                                  client_data->res_config.cb_data.data_ptr );
      ret_val &= init_ret.init_status;

      /* Based on init_status update data structure */
      if ( init_ret.init_status == TRUE )
      {
        client_data->slave_txlm_handle = init_ret.lm_handle;
        client_data->slave_txlm_allocated = TRUE;

        RF_MSG_3( RF_MED, "[TX_RESOURCE_MGR][LM][ALLOCATE]: Dev %d Mode %d "
                  "LM %d - Done", client_data->device, client_data->mode,
                  client_data->slave_txlm_handle );
      } /* if ( init_ret.init_status == TRUE ) */
      else
      {
        ret_val = FALSE;
        RF_MSG_3( RF_ERROR, "[TX_RESOURCE_MGR][LM][ALLOCATE]: Dev %d Mode %d - "
                  "[LM FAILURE]", client_data->device, client_data->mode,
                  client_data->slave_txlm_handle );
      } /* if ! ( init_ret.init_status == TRUE ) */

    } /* if ( client_data->res_config.init_fp != NULL ) */
    else
    {
      ret_val = FALSE;
      RF_MSG_2( RF_ERROR, "[TX_RESOURCE_MGR][LM][ALLOCATE]: Dev %d Mode %d - "
                "NULL Init FP", client_data->device, client_data->mode );
    } /* if ! ( client_data->res_config.init_fp != NULL ) */

  } /* if ( client_data->txlm_allocated == FALSE ) */
  else
  {
    RF_MSG_3( RF_MED, "[TX_RESOURCE_MGR][LM][ALLOCATE]: Dev %d Mode %d "
              "LM %d - [SKIP]", client_data->device, client_data->mode,
              client_data->slave_txlm_handle );
  } /* if ! ( client_data->txlm_allocated == FALSE ) */

  return ret_val;
} /* tx_mgr_allocate_lm */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Allocate LM
 
  @details
  This function is called the STM transition function to perform LM allcoation
 
  @param client_data
  Pointer to the client data for which LM allocation is to be done
 
  @return
  Status of LM allocation
*/
static boolean
tx_mgr_deallocate_lm
(
  tx_mgr_client_data_type *client_data
)
{
  boolean ret_val = TRUE; /* Return value for the allocate function */

  if ( client_data == NULL )
  {
    RF_MSG( RF_ERROR, "[TX_RESOURCE_MGR][LM][DEALLOCATE]: NULL Client" );
    return FALSE;
  } /* if ( client_data == NULL ) */

  /* DeInit only if txlm is  already allocated */
  if ( client_data->slave_txlm_allocated == TRUE )
  {
    /* NULL pointer check */
    if ( client_data->res_config.func_ptrs.deinit_fp != NULL )
    {
      /* call function pointer to perform initialization */ 
      ret_val &= client_data->res_config.func_ptrs.deinit_fp( 
                                   client_data->device, 
                                   client_data->slave_txlm_handle,
                                   client_data->res_config.cb_data.data_ptr );

      /* Based on init_status update data structure */
      if ( ret_val == TRUE )
      {
        client_data->slave_txlm_handle = (lm_handle_type)(-1);
        client_data->slave_txlm_allocated = FALSE;

        RF_MSG_2( RF_MED, "[TX_RESOURCE_MGR][LM][DEALLOCATE]: Dev %d Mode %d - "
                  "Done", client_data->device, client_data->mode );
      } /* if ( ret_val == TRUE ) */
      else
      {
        ret_val = FALSE;
        RF_MSG_3( RF_ERROR, "[TX_RESOURCE_MGR][LM][DEALLOCATE]: Dev %d Mode "
                  "%d - [LM FAILURE]", client_data->device, client_data->mode,
                  client_data->slave_txlm_handle );
      } /* if ! ( ret_val == TRUE ) */

    } /* if ( client_data->res_config.deinit_fp != NULL ) */
    else
    {
      ret_val = FALSE;
      RF_MSG_2( RF_ERROR, "[TX_RESOURCE_MGR][LM][DEALLOCATE]: Dev %d Mode %d - "
                "NULL Deinit FP", client_data->device, client_data->mode );
    } /* if ! ( client_data->res_config.deinit_fp != NULL ) */

  } /* if ( client_data->txlm_allocated == TRUE ) */
  else
  {
    RF_MSG_2( RF_MED, "[TX_RESOURCE_MGR][LM][DEALLOCATE]: Dev %d Mode %d - "
              "[SKIP]", client_data->device, client_data->mode );
  } /* if ! ( client_data->txlm_allocated == TRUE ) */

  return ret_val;
} /* tx_mgr_deallocate_lm */


/*! @} */

/*============================================================================*/
/*!
  @name Data handler function

  @brief
  This section contains internal helper functions for handling Tx resource 
  manager data
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Input Parameter Copy
 
  @details
  This helper function can be called to perform copy data from input param to
  resource config data container,
 
  @param copy_to
  Pointer to resource config data container where the input param is to be
  copied
 
  @param copy_from
  Pointer to input param which need to be copied to resource config data
  container
 
  @return
  Status of function execution
*/
static boolean
tx_mgr_store_input_param
(
  tx_mgr_client_data_type *copy_to,
  rf_tx_resource_input_param_type *copy_from
)
{
  boolean ret_val = TRUE;

  copy_to->device = copy_from->device;
  copy_to->mode = copy_from->mode;

  copy_to->res_config.func_ptrs = copy_from->res_config.func_ptrs;

  copy_to->res_config.cb_data.data_size = 
                copy_from->res_config.cb_data.data_size;

  /* Allocate only if the data size is greater than 0 */
  if ( copy_to->res_config.cb_data.data_size > 0 )
  {
    copy_to->res_config.cb_data.data_ptr = 
      modem_mem_alloc( copy_to->res_config.cb_data.data_size,
                       MODEM_MEM_CLIENT_RFA );

    /* Perform NULL pointer check */
    if ( copy_to->res_config.cb_data.data_ptr != NULL )
    {
      memscpy( copy_to->res_config.cb_data.data_ptr,
              copy_to->res_config.cb_data.data_size,
              copy_from->res_config.cb_data.data_ptr,
              copy_to->res_config.cb_data.data_size );
    } /* if ( copy_to->res_config.cb_data.data_ptr != NULL ) */
    else
    {
      ret_val = FALSE;
      RF_MSG( RF_ERROR, "[TX_RESOURCE_MGR][DATA]: Malloc failed" );
    } /* if ! ( copy_to->res_config.cb_data.data_ptr != NULL ) */

  } /* if ( copy_to->res_config.cb_data.data_size > 0 ) */
  else
  {
    copy_to->res_config.cb_data.data_ptr = NULL;
  } /* if ! ( copy_to->res_config.cb_data.data_size > 0 ) */

  return ret_val;
} /* tx_mgr_store_input_param */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Copy resource data
 
  @details
  This helper function can be called to perform copying of data between
  active resource and waiting resource.
 
  @param copy_to
  Pointer to address where the data is to be copied
 
  @param copy_from
  Pointer to address from where the  data is to be copied
*/
static void
tx_mgr_copy_resource_data
(
  tx_mgr_client_data_type *copy_to,
  tx_mgr_client_data_type *copy_from
)
{
  memscpy( copy_to, sizeof(tx_mgr_client_data_type),copy_from, sizeof(tx_mgr_client_data_type) );

} /* tx_mgr_copy_resource_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reset resource data
 
  @details
  This helper function can be called to reset resource data
  active resource and waiting resource.
 
  @param res_data
  Pointer to address where the data is to be reset
 
  @param free_memory
  Flag to indicate if the memory is to be freed. Memory would not be freed
  for the case, when reset is done after move.
*/
static void
tx_mgr_reset_resource_data
(
  tx_mgr_client_data_type *res_data,
  boolean free_memory
)
{

  res_data->device = RFM_INVALID_DEVICE;
  res_data->mode = RFM_INVALID_MODE;
  res_data->slave_txlm_allocated = FALSE;
  res_data->slave_txlm_handle = -1;
  res_data->res_config.func_ptrs.init_fp = NULL;
  res_data->res_config.func_ptrs.deinit_fp = NULL;
  res_data->res_config.func_ptrs.prep_fp = NULL;
  res_data->res_config.func_ptrs.setup_fp = NULL;
  res_data->res_config.func_ptrs.cleanup_fp = NULL;
  res_data->res_config.cb_data.data_size = 0;
  if ( free_memory == TRUE )
  {
    if ( res_data->res_config.cb_data.data_ptr != NULL )
    {
      modem_mem_free( res_data->res_config.cb_data.data_ptr, 
                      MODEM_MEM_CLIENT_RFA );
      res_data->res_config.cb_data.data_ptr = NULL;
    } /* if ( res_data->res_config.cb_data.data_ptr != NULL ) */
  } /* if ( free_memory == TRUE ) */
  else
  {
    res_data->res_config.cb_data.data_ptr = NULL;
  } /* if ! ( free_memory == TRUE ) */

} /* tx_mgr_reset_resource_data */

/*! @} */

/*============================================================================*/
/*!
  @name State Machine - Entry

  @brief
  This section contains STM Entry related functions for Tx Resource manager
  block. The function in this block are internal to this module and are not
  exported to the Tx resource manager clients.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  STM Error entry funciton
 
  @details
  This function will be called form STM2 framework, if any error occurs
  during the input processing.
 
  @param error
  Error Type which resulted in this function execution
 
  @param filename
  Filename which encounted the error
 
  @param line
  Line number in the file, from where this function is executed due to
  error
 
  @param sm
  Pointer to the state machine instance
*/
static void
tx_mgr_stm_error
(
  stm_status_t error,
  const char *filename,
  uint32 line,
  struct stm_state_machine_s *sm
)
{

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  RF_STR_MSG_7( RF_FATAL, "[TX_RESOURCE_MGR][STM]{%d}: ***ERROR*** "
                "UID.0x%x.[State %d RecentInput %d] - STM2 Framework "
                "[ Error %d { Line %d @ (%s)} ]", 
                stm_get_instance_num(sm),
                stm_get_uid(sm),
                stm_get_state(sm),
                stm_get_last_input(sm),
                error,
                line,
                filename );

  ERR_FATAL( "[TX_RESOURCE_MGR][STM]{%d}: ***ERROR*** STM2 Framework "
             "[ Error %d { Line %d @ (%s)} ]", 
             error,
             line,
             filename );


} /* tx_mgr_stm_error */

/*! @} */

/*============================================================================*/
/*!
  @name State Machine - Debug

  @brief
  This section contains state machine related functions for Tx Resource manager
  block. The function in this block are internal to this module and are not
  exported to the Tx resource manager clients.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  State machine debug hook function
 
  @details
  This function will be called by MCS STM2 infrastructure during STM entry,
  exit and process input. Thus will print additional debug messages in order
  based on the debug flag.
 
  @param debug_event
  Parameter indicate for what reason it is being called
 
  @param sm
  Pointer to the data structure containing the instance of state machine
 
  @param state_info
  Variable indicating the current STM state
 
  @param payload
  Pointer opaque data payload
*/
static void
tx_mgr_stm_debug_hook
(
  stm_debug_event_t debug_event,
  struct stm_state_machine_s *sm,
  stm_state_t state_info,
  void *payload
)
{
  rf_tx_resource_stm_payload_type *sm_payload;
  uint8 buff_idx = 0;
  tx_mgr_state_tran_log_type *tran_log_ptr;
  static rf_time_tick_type last_timetick;
  static boolean first_log = TRUE;
  
  /* Perform logging only for STM_POST_TRANS_FN event */ 
  if ( debug_event != STM_POST_TRANS_FN )
  {
    return;
  } /* if ( debug_event != STM_POST_TRANS_FN ) */

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  /* Get STM payload */
  sm_payload = (rf_tx_resource_stm_payload_type*)payload;

  /* Perform NULL pointer check for Payload */
  if ( sm_payload != NULL )
  {
    /* Perform logging until the point STM enters the TX_STATE_ERROR (to
    avoid flushing of all previous data) */
    if ( state_info != TX_STATE_ERROR )
    {
      /* Get the Log pointer */
      tran_log_ptr = &(sm_payload->resource_data->debug_data.tran_log);

      /* Increment the log index */
      tran_log_ptr->log_index++;

      /* Get current buffer index */
      buff_idx = tran_log_ptr->log_index;

      /* Reset the index if the circular buffer is full */
      if ( buff_idx >= TX_MGR_TRAN_LOG_BUFF_LEN )
      {
        buff_idx = 0;
      } /* if ( buff_idx >= TX_MGR_TRAN_LOG_BUFF_LEN ) */

      /* Get the current time */
      last_timetick = rf_time_get_tick();
      tran_log_ptr->trace[buff_idx].time_tick = last_timetick;

      /* calculate the elpased time from second log only. coarse_timetick will
      will be zero only for the first time, thus this is used to detect the
      first logging */
      if ( first_log == TRUE )
      {
        tran_log_ptr->trace[buff_idx].elapsed_time_ms = 0;
        first_log = FALSE;
      } /* if ( first_log == TRUE ) */
      else
      {
        tran_log_ptr->trace[buff_idx].elapsed_time_ms = 
                                  rf_time_get_elapsed( last_timetick, RF_MSEC );
      } /* if ! ( first_log == TRUE ) */
      tran_log_ptr->trace[buff_idx].device = 
                            sm_payload->resource_data->active_client.device;
      tran_log_ptr->trace[buff_idx].mode = 
                            sm_payload->resource_data->active_client.mode;
      #ifndef FEATURE_GNSS_ONLY_NO_WWAN
      tran_log_ptr->trace[buff_idx].s_state = stm_get_state(sm);
      tran_log_ptr->trace[buff_idx].tran_input = stm_get_last_input(sm);
      #endif
      tran_log_ptr->trace[buff_idx].e_state = state_info;
      tran_log_ptr->trace[buff_idx].w_device = 
         (rfm_mode_enum_type) sm_payload->resource_data->waiting_client.device;
      tran_log_ptr->trace[buff_idx].w_mode = 
                            sm_payload->resource_data->waiting_client.mode;

      /* Print F3, only if enabled */
      if ( sm_payload->input_parameter.enable_f3 == TRUE )
      {
        #ifndef FEATURE_GNSS_ONLY_NO_WWAN
        RF_MSG_9( RF_MED, "[TX_RESOURCE_MGR][STM]{%d}: "
                  "Active[ D %d M %d {Event %d} i %d - {State %d -> %d} ] ::: "
                  "Waiting[ D %d M %d ]",
                  stm_get_instance_num(sm),
                  tran_log_ptr->trace[buff_idx].device,
                  tran_log_ptr->trace[buff_idx].mode,
                  debug_event,
                  tran_log_ptr->trace[buff_idx].tran_input,
                  tran_log_ptr->trace[buff_idx].s_state,
                  tran_log_ptr->trace[buff_idx].e_state,
                  tran_log_ptr->trace[buff_idx].w_device,
                  tran_log_ptr->trace[buff_idx].w_mode );

        #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
      } /* if ( sm_payload->input_parameter.enable_f3 == TRUE ) */
    } /* if ( state_info != TX_STATE_ERROR ) */
  } /* if ( sm_payload != NULL ) */

  return;

} /* tx_mgr_stm_debug_hook */

/*! @} */

/*============================================================================*/
/*!
  @name State Machine - State Transition

  @brief
  This section contains state machine related functions for Tx Resource manager
  block. The function in this block are internal to this module and are not
  exported to the Tx resource manager clients.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Transition function for state TX_STATE_OFF, upon receiving input
  TX_INPUT_M_ACQUIRE

  @details
  This handler function will be called upon receipt of input
  TX_INPUT_M_ACQUIRE, with optional user-passed payload pointer.
 
  @param sm
  State Machine instance pointer
 
  @param payload
  Pointer to opaque payload

  @return
  Returns the next state that the state machine should transition to
  upon receipt of the input.  This state must be a valid state for this
  state machine.
*/
static stm_state_t
tx_mgr_stm_off_proc_m_acquire
(
  stm_state_machine_t *sm,
  void *payload
)
{
  stm_state_t next_state = TX_STATE_ERROR;
  rf_tx_resource_stm_payload_type *sm_payload;
  boolean api_status = TRUE;

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  /* Get STM payload */
  sm_payload = (rf_tx_resource_stm_payload_type*)payload;

  /* Perform NULL pointer check for Payload */
  if ( sm_payload != NULL )
  {
    /* Copy the input parameter */
    api_status &= tx_mgr_store_input_param( 
                         &(sm_payload->resource_data->active_client),
                         &(sm_payload->input_parameter) );

    /* Update next state */
    if ( api_status == TRUE )
    {
      next_state = TX_STATE_MASTER;
    } /* if ( api_status == TRUE ) */
  } /* if ( sm_payload != NULL ) */
  else
  {

    #ifndef FEATURE_GNSS_ONLY_NO_WWAN
    RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [X] -> m.a -> [MASTER] : "
              "Failed - NULL payload", stm_get_instance_num(sm) );

    #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
  } /* if ! ( sm_payload != NULL ) */

  return next_state;

} /* tx_mgr_stm_off_proc_m_acquire */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Transition function for state TX_STATE_MASTER, upon receiving input
  TX_INPUT_M_RELEASE

  @details
  This handler function will be called upon receipt of input
  TX_INPUT_M_RELEASE, with optional user-passed payload pointer.
 
  @param sm
  State Machine instance pointer
 
  @param payload
  Pointer to opaque payload

  @return
  Returns the next state that the state machine should transition to
  upon receipt of the input.  This state must be a valid state for this
  state machine.
*/
static stm_state_t
tx_mgr_stm_master_proc_m_release
(
  stm_state_machine_t *sm,
  void *payload
)
{
  stm_state_t next_state = TX_STATE_ERROR;
  rf_tx_resource_stm_payload_type *sm_payload;

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  /* Get STM payload */
  sm_payload = (rf_tx_resource_stm_payload_type*)payload;

  /* Perform NULL pointer check for Payload */
  if ( sm_payload != NULL )
  {
    /* Reset the Resource Data */
    tx_mgr_reset_resource_data( &(sm_payload->resource_data->active_client),
                                TRUE );

    next_state = TX_STATE_OFF;
  } /* if ( sm_payload != NULL ) */
  else
  {
    #ifndef FEATURE_GNSS_ONLY_NO_WWAN
    RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [MASTER] -> m.r -> [X] : "
              "Failed - NULL payload", stm_get_instance_num(sm) );

    #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
  } /* if ! ( sm_payload != NULL ) */

  return next_state;

} /* tx_mgr_stm_master_proc_m_release */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Transition function for state TX_STATE_MASTER, upon receiving input
  TX_INPUT_S_ACQUIRE

  @details
  This handler function will be called upon receipt of input
  TX_INPUT_S_ACQUIRE, with optional user-passed payload pointer.
 
  @param sm
  State Machine instance pointer
 
  @param payload
  Pointer to opaque payload

  @return
  Returns the next state that the state machine should transition to
  upon receipt of the input.  This state must be a valid state for this
  state machine.
*/
static stm_state_t
tx_mgr_stm_master_proc_s_acquire
(
  stm_state_machine_t *sm,
  void *payload
)
{
  stm_state_t next_state = TX_STATE_ERROR;
  rf_tx_resource_stm_payload_type *sm_payload;
  rf_tx_resource_mgr_prep_fp_type prep_fp;
  boolean api_status = TRUE;
  tx_mgr_client_data_type *waiting_client;

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  /* Get STM payload */
  sm_payload = (rf_tx_resource_stm_payload_type*)payload;

  /* Perform NULL pointer check for Payload */
  if ( sm_payload != NULL )
  {
    /* Copy the input parameter */
    api_status &= tx_mgr_store_input_param( 
                              &(sm_payload->resource_data->waiting_client),
                              &(sm_payload->input_parameter) );

    /* Check for API status */
    if ( api_status == TRUE )
    {
      /* Shorten waiting client pointer */
      waiting_client = &(sm_payload->resource_data->waiting_client);

      /* Get the Prep function */
      prep_fp = waiting_client->res_config.func_ptrs.prep_fp;

      /* Cleanup the resource */
      if ( prep_fp != NULL )
      {
        api_status &= prep_fp( waiting_client->device,
                               waiting_client->res_config.cb_data.data_ptr );
      } /* if ( prep_fp != NULL ) */
      
      /* Update the state only for successful prep */
      if ( api_status == TRUE )
      {
        next_state = TX_STATE_MASTER_PSLAVE;
      } /* if ( api_status == TRUE ) */
    } /* if ( api_status == TRUE ) */
  } /* if ( sm_payload != NULL ) */
  else
  {
    #ifndef FEATURE_GNSS_ONLY_NO_WWAN
    RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [MASTER] -> s.a -> "
              "[MASTER_PSLAVE] : Failed - NULL payload", 
              stm_get_instance_num(sm) );

    #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
  } /* if ! ( sm_payload != NULL ) */

  return next_state;

} /* tx_mgr_stm_master_proc_s_acquire */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Transition function for state TX_STATE_MASTER_PSLAVE, upon receiving
  input TX_INPUT_S_RELEASE

  @details
  This handler function will be called upon receipt of input
  TX_INPUT_S_RELEASE, with optional user-passed payload pointer.
 
  @param sm
  State Machine instance pointer
 
  @param payload
  Pointer to opaque payload

  @return
  Returns the next state that the state machine should transition to
  upon receipt of the input. This state must be a valid state for this
  state machine.
*/
static stm_state_t
tx_mgr_stm_master_pslave_proc_s_release
(
  stm_state_machine_t *sm,
  void *payload
)
{
  stm_state_t next_state = TX_STATE_ERROR;
  rf_tx_resource_stm_payload_type *sm_payload;
  boolean dealloc_ret = TRUE;

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  /* Get STM payload */
  sm_payload = (rf_tx_resource_stm_payload_type*)payload;

  /* Perform NULL pointer check for Payload */
  if ( sm_payload != NULL )
  {
    /* TXLMbuff : de-allocate for waiting client (conditional) */
    dealloc_ret = tx_mgr_deallocate_lm(
                         &(sm_payload->resource_data->waiting_client));
    
    if ( dealloc_ret == TRUE )
    {
      /* Copy the input parameter */
      tx_mgr_reset_resource_data( 
                   &(sm_payload->resource_data->waiting_client), TRUE );

      next_state = TX_STATE_MASTER;
    } /* if ( alloc_ret == TRUE ) */
    else
    {
      #ifndef FEATURE_GNSS_ONLY_NO_WWAN
      RF_MSG_2( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [MASTER_PSLAVE] -> "
                "s.r -> [MASTER] : Failed - [dealloc %d]",
                stm_get_instance_num(sm), dealloc_ret );
      #endif
    } /* if ! ( alloc_ret == TRUE ) */
  } /* if ( sm_payload != NULL ) */
  else
  {
    #ifndef FEATURE_GNSS_ONLY_NO_WWAN
    RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [MASTER_PSLAVE] -> s.r -> "
              "[MASTER] : Failed - NULL payload", stm_get_instance_num(sm) );
    #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
  } /* if ! ( sm_payload != NULL ) */

  return next_state;
} /* tx_mgr_stm_master_pslave_proc_s_release */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Transition function for state TX_STATE_MASTER_PSLAVE, upon receiving
  input TX_INPUT_M_RELEASE

  @details
  This handler function will be called upon receipt of input
  TX_INPUT_M_RELEASE, with optional user-passed payload pointer.
 
  @param sm
  State Machine instance pointer
 
  @param payload
  Pointer to opaque payload

  @return
  Returns the next state that the state machine should transition to
  upon receipt of the input.  This state must be a valid state for this
  state machine.
*/
static stm_state_t
tx_mgr_stm_master_pslave_proc_m_release
(
  stm_state_machine_t *sm,
  void *payload
)
{
  stm_state_t next_state = TX_STATE_ERROR;
  rf_tx_resource_stm_payload_type *sm_payload;
  rf_tx_resource_mgr_setup_fp_type setup_fp;
  tx_mgr_client_data_type *active_client;
  boolean api_status = TRUE;

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  /* Get STM payload */
  sm_payload = (rf_tx_resource_stm_payload_type*)payload;

  /* Perform NULL pointer check for Payload */
  if ( sm_payload != NULL )
  {

    /* TXLMbuff : allocate for waiting client (conditional) */
    api_status = tx_mgr_allocate_lm(
                         &(sm_payload->resource_data->waiting_client));

    /* Copy the resource data from waiting to active indicating that there
    no longer any waiting client */
    tx_mgr_copy_resource_data( &(sm_payload->resource_data->active_client),
                               &(sm_payload->resource_data->waiting_client));

    /* Reset the wating Resource Data */
    tx_mgr_reset_resource_data( &(sm_payload->resource_data->waiting_client),
                                FALSE );
    
    /* Shorten active client pointer */
    active_client = &(sm_payload->resource_data->active_client);

    /* Get Setup fp */
    setup_fp = active_client->res_config.func_ptrs.setup_fp;

    /* Call function to setup ET resource */ 
    if ( ( setup_fp != NULL ) && ( api_status == TRUE ) )
    {
      api_status &= setup_fp( active_client->device,
                          active_client->slave_txlm_handle,
                          active_client->res_config.cb_data.data_ptr );
    } /* if ( setup_fp != NULL ) */
    else
    {
      api_status = FALSE;

      #ifndef FEATURE_GNSS_ONLY_NO_WWAN
      RF_MSG_2( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [MASTER_PSLAVE] "
                "-> m.r -> [SLAVE] : Failed - setup_fp [alloc %d]", 
                stm_get_instance_num(sm), api_status );
      #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
    } /* if ! ( ( setup_fp != NULL ) && ( alloc_ret == TRUE ) ) */

    /* Change the state only if the setup is successful */
    if ( api_status == TRUE )
    {
      next_state = TX_STATE_SLAVE;
    } /* if ( api_status == TRUE ) */

  } /* if ( sm_payload != NULL ) */
  else
  {
    #ifndef FEATURE_GNSS_ONLY_NO_WWAN
    RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [MASTER_PSLAVE] -> m.r -> "
              "[SLAVE] : Failed - NULL payload", stm_get_instance_num(sm) );
    #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
  } /* if ! ( sm_payload != NULL ) */

  return next_state;

} /* tx_mgr_stm_master_pslave_proc_m_release */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Transition function for state TX_STATE_SLAVE, upon receiving
  input TX_INPUT_M_ACQUIRE

  @details
  This handler function will be called upon receipt of input
  TX_INPUT_M_ACQUIRE, with optional user-passed payload pointer.
 
  @param sm
  State Machine instance pointer
 
  @param payload
  Pointer to opaque payload

  @return
  Returns the next state that the state machine should transition to
  upon receipt of the input.  This state must be a valid state for this
  state machine.
*/
static stm_state_t
tx_mgr_stm_slave_proc_m_acquire
(
  stm_state_machine_t *sm,
  void *payload
)
{
  stm_state_t next_state = TX_STATE_ERROR;
  rf_tx_resource_stm_payload_type *sm_payload;
  rf_tx_resource_mgr_cleanup_fp_type cleanup_fp;
  boolean api_status = TRUE;
  tx_mgr_client_data_type *active_client;

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  /* Get STM payload */
  sm_payload = (rf_tx_resource_stm_payload_type*)payload;

  /* Perform NULL pointer check for Payload */
  if ( sm_payload != NULL )
  {
    /* Shorten Active client pointer */
    active_client = &(sm_payload->resource_data->active_client);

    /* Get cleanup function pointer */
    cleanup_fp = active_client->res_config.func_ptrs.cleanup_fp;

    /* Cleanup the resource */
    if ( cleanup_fp != NULL )
    {
      api_status &= cleanup_fp( active_client->device,
                                active_client->slave_txlm_handle,
                                active_client->res_config.cb_data.data_ptr);
    } /* if ( cleanup_fp != NULL ) */

    /* Continue if the cleanup is successful */
    if ( api_status == TRUE )
    {
      /* Copy the resource data from active to waiting indicating that the
      cleaned up resource has one waiting client */
      tx_mgr_copy_resource_data( &(sm_payload->resource_data->waiting_client),
                                 &(sm_payload->resource_data->active_client));

      /* Reset the Active client data */
      tx_mgr_reset_resource_data( &(sm_payload->resource_data->active_client),
                                  FALSE );

      /* Copy the input parameter */
      api_status &= tx_mgr_store_input_param( 
                                &(sm_payload->resource_data->active_client),
                                &(sm_payload->input_parameter) );

      if ( api_status == TRUE )
      {
        /* Update next state */
        next_state = TX_STATE_MASTER_PSLAVE;
      } /* if ( api_status == TRUE ) */
    } /* if ( api_status == TRUE ) */
    else
    {
      #ifndef FEATURE_GNSS_ONLY_NO_WWAN
      RF_MSG_3( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [SLAVE] -> m.a -> "
                "[MASTER_PSLAVE] : Failed - CleanUp of Dev %d Mode %d failed",
                stm_get_instance_num(sm),
                sm_payload->resource_data->active_client.device,
                sm_payload->resource_data->active_client.mode );
      #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
    } /* if ! ( api_status == TRUE ) */

  } /* if ( sm_payload != NULL ) */
  else
  {
    #ifndef FEATURE_GNSS_ONLY_NO_WWAN
    RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [SLAVE] -> m.a -> "
              "[MASTER_PSLAVE] : Failed - NULL payload", 
              stm_get_instance_num(sm) );
    #endif /* FEATURE_CDMA1X */
  } /* if ! ( sm_payload != NULL ) */

  return next_state;

} /* tx_mgr_stm_slave_proc_m_acquire */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Transition function for state TX_STATE_SLAVE, upon receiving
  input TX_INPUT_S_RELEASE

  @details
  This handler function will be called upon receipt of input
  TX_INPUT_S_RELEASE, with optional user-passed payload pointer.
 
  @param sm
  State Machine instance pointer
 
  @param payload
  Pointer to opaque payload

  @return
  Returns the next state that the state machine should transition to
  upon receipt of the input.  This state must be a valid state for this
  state machine.
*/
static stm_state_t
tx_mgr_stm_slave_proc_s_release
(
  stm_state_machine_t *sm,
  void *payload
)
{
  stm_state_t next_state = TX_STATE_ERROR;
  rf_tx_resource_stm_payload_type *sm_payload;
  rf_tx_resource_mgr_cleanup_fp_type cleanup_fp;
  tx_mgr_client_data_type *active_client;
  boolean api_status = TRUE;

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  /* Get STM payload */
  sm_payload = (rf_tx_resource_stm_payload_type*)payload;

  /* Perform NULL pointer check for Payload */
  if ( sm_payload != NULL )
  {
    /* Shorten Active client pointer */
    active_client = &(sm_payload->resource_data->active_client);

    /* Get cleanup function pointer */
    cleanup_fp = active_client->res_config.func_ptrs.cleanup_fp;

    /* Cleanup the resource */
    if ( cleanup_fp != NULL )
    {
      api_status &= cleanup_fp( active_client->device,
                            active_client->slave_txlm_handle,
                            active_client->res_config.cb_data.data_ptr);

      /* Check cleanup status */
      if ( api_status == TRUE )
      {
        /* TXLMbuff : de-allocate for active client */
        api_status &= tx_mgr_deallocate_lm( active_client );
      } /* if ( api_status == TRUE ) */

    } /* if ( cleanup_fp != NULL ) */

    /* Continue if the cleanup is successful */
    if ( api_status == TRUE )
    {
      /* Copy the input parameter */
      tx_mgr_reset_resource_data( &(sm_payload->resource_data->active_client),
                                  TRUE );

      next_state = TX_STATE_OFF;
    } /* if ( api_status == TRUE ) */
    else
    {
      #ifndef FEATURE_GNSS_ONLY_NO_WWAN
      RF_MSG_2( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [SLAVE] -> s.r -> [X] : "
                "Failed - cleanup [dealloc %d]", stm_get_instance_num(sm),
                api_status );
      #endif
    } /* if ! ( api_status == TRUE )  */

  } /* if ( sm_payload != NULL ) */
  else
  {

    #ifndef FEATURE_GNSS_ONLY_NO_WWAN
    RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [SLAVE] -> s.r -> [X] : "
              "Failed - NULL payload", stm_get_instance_num(sm) );
    #endif
  } /* if ! ( sm_payload != NULL ) */

  return next_state;

} /* tx_mgr_stm_slave_proc_s_release */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Transition function for state TX_STATE_OFF, upon receiving
  input TX_INPUT_S_ACQUIRE

  @details
  This handler function will be called upon receipt of input
  TX_INPUT_S_ACQUIRE, with optional user-passed payload pointer.
 
  @param sm
  State Machine instance pointer
 
  @param payload
  Pointer to opaque payload

  @return
  Returns the next state that the state machine should transition to
  upon receipt of the input.  This state must be a valid state for this
  state machine.
*/
static stm_state_t
tx_mgr_stm_off_proc_s_acquire
(
  stm_state_machine_t *sm,
  void *payload
)
{
  stm_state_t next_state = TX_STATE_ERROR;
  rf_tx_resource_stm_payload_type *sm_payload;
  rf_tx_resource_mgr_setup_fp_type setup_fp;
  boolean api_status = TRUE;
  tx_mgr_client_data_type *active_client;

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  /* Get STM payload */
  sm_payload = (rf_tx_resource_stm_payload_type*)payload;

  /* Perform NULL pointer check for Payload */
  if ( sm_payload != NULL )
  {
    /* Copy the input parameter */
    api_status &= tx_mgr_store_input_param(
                              &(sm_payload->resource_data->active_client),
                              &(sm_payload->input_parameter) );

    if ( api_status == TRUE )
    {
      /* Shorten active client pointer */
      active_client = &(sm_payload->resource_data->active_client);

      /* Get the setup function */
      setup_fp = active_client->res_config.func_ptrs.setup_fp;

      /* TXLMbuff : allocate for active client */
      api_status &= tx_mgr_allocate_lm( active_client );

      /* Call function to setup ET resource */ 
      if ( ( setup_fp != NULL ) && ( api_status == TRUE ) )
      {
        /* Setup Client */
        api_status &= setup_fp( active_client->device,
                                active_client->slave_txlm_handle,
                                active_client->res_config.cb_data.data_ptr );
      } /* if ( ( setup_fp != NULL ) && ( api_status == TRUE ) ) */
      else
      {
        api_status = FALSE;

        #ifndef FEATURE_GNSS_ONLY_NO_WWAN
        RF_MSG_2( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [X] -> s.a -> [SLAVE] : "
                  "Falied - setup_fp [alloc %d]", stm_get_instance_num(sm),
                  api_status );
        #endif
      } /* if ! ( ( setup_fp != NULL ) && ( api_status == TRUE ) ) */

      /* Update state for successful setup */
      if ( api_status == TRUE )
      {
        next_state = TX_STATE_SLAVE;
      } /* if ( api_status == TRUE ) */
    } /* if ( api_status == TRUE ) */
  } /* if ( sm_payload != NULL ) */
  else
  {

    #ifndef FEATURE_GNSS_ONLY_NO_WWAN
    RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][STM]{%d}: [X] -> s.a -> [SLAVE] : "
              "Failed - NULL payload", stm_get_instance_num(sm) );
    #endif
  } /* if ! ( sm_payload != NULL ) */

  return next_state;

} /* tx_mgr_stm_off_proc_s_acquire */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Transition function for state any state, upon receiving
  input TX_INPUT_ERROR

  @details
  This handler function will be called upon receipt of input
  TX_INPUT_ERROR, with optional user-passed payload pointer.
 
  @param sm
  State Machine instance pointer
 
  @param payload
  Pointer to opaque payload

  @return
  Returns the next state that the state machine should transition to
  upon receipt of the input.  This state must be a valid state for this
  state machine.
*/
static stm_state_t
tx_mgr_stm_any_state_proc_error
(
  stm_state_machine_t *sm,
  void *payload
)
{
  stm_state_t next_state = TX_STATE_ERROR;

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  next_state = TX_STATE_ERROR;

  return next_state;

} /* tx_mgr_stm_any_state_proc_error */

/*! @} */

/*============================================================================*/
/*!
  @name State Machine - State Entry

  @brief
  This section contains state machine related functions for Tx Resource manager
  block. The function in this block are internal to this module and are not
  exported to the Tx resource manager clients.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Entry function for TX_STATE_ERROR state
 
  @details
  This function will be called when the state machine enter the TX_STATE_ERROR
  state. This state is unrecoverable state; thus error fatal will be
  done as this state is entered.
 
  @param sm
  State Machine instance pointer
 
  @param previous_state
  Previous State of state machine
 
  @param payload
  Pointer to opaque payload
*/
static void
tx_mgr_stm_error_state_entry
(
  stm_state_machine_t *sm,
  stm_state_t previous_state,
  void *payload
)
{
  rf_tx_resource_stm_payload_type *sm_payload;

  /* Ensure that the state machine instance pointer passed is valid */
  STM_NULL_CHECK( sm );

  /* Get STM payload */
  sm_payload = (rf_tx_resource_stm_payload_type*)payload;

  /* Perform NULL pointer check for Payload */
  if ( sm_payload != NULL )
  {
    /* Save the Error Dump */
    sm_payload->resource_data->debug_data.error_dump.err_reason = 
      sm_payload->input_parameter.error_reason;
    sm_payload->resource_data->debug_data.error_dump.last_state = 
      previous_state;
  } /* if ( sm_payload != NULL ) */

  /* Since this is unrecoverable state, perform error fatal */

  #ifndef FEATURE_GNSS_ONLY_NO_WWAN
  #ifdef T_WINNT
  #error code not present
#else
  ERR_FATAL( "[TX_RESOURCE_MGR][STM]{%d}: ***ERROR*** Unrecoverable error "
             "in State %d", stm_get_instance_num(sm), previous_state, 0 );
  #endif /* T_WINNT */
  #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
} /* tx_mgr_stm_error_state_entry */

/*! @} */


/*============================================================================*/
/*!
  @name State Machine Static Variable

  @brief
  This section contains all static variable pertaining to tx resource
  manage state machine
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! Structure to hold the state machine mapping. This maps a state to its entry 
and exit functions, along with its string name.  An array of these structures is 
used to form a sequential list of all possible states for a state machine.  The
state's value itself is represented by its index into this array  */
static const stm_state_map_t 
  tx_mgr_stm_state_mapping[ TX_STATE_NUM ] =
{

  { 
    "TX_STATE_OFF",
    NULL, /* Entry Function */
    NULL, /* Exit Function */
    NULL /* Child STM */
  } /* TX_STATE_OFF */,
  {
    "TX_STATE_MASTER",
    NULL, /* Entry Function */
    NULL, /* Exit Function */
    NULL /* Child STM */
  } /* TX_STATE_MASTER */,
  {
    "TX_STATE_MASTER_PSLAVE",
    NULL, /* Entry Function */
    NULL, /* Exit Function */
    NULL /* Child STM */
  } /* TX_STATE_MASTER_PSLAVE */,
  {
    "TX_STATE_SLAVE",
    NULL, /* Entry Function */
    NULL, /* Exit Function */
    NULL /* Child STM */
  } /* TX_STATE_SLAVE */,
  {
    "TX_STATE_ERROR",
    tx_mgr_stm_error_state_entry, /* Entry Function */
    NULL, /* Exit Function */
    NULL /* Child STM */
  } /* TX_STATE_ERROR */,

}; /* tx_mgr_stm_state_mapping */

/*----------------------------------------------------------------------------*/
/*! Structure to hold the stm input mapping. This maps state machine inputs 
to their names. An array of these structures is used to map all possible inputs 
for a state machine into an index for subsequent use in looking up the 
corresponding transition function in a non-sparse table  */
static const stm_input_map_t
  tx_mgr_stm_input_mapping[ TX_INPUT_NUM ] =
{

  { "TX_INPUT_M_ACQUIRE", (stm_input_t)TX_INPUT_M_ACQUIRE },
  { "TX_INPUT_M_RELEASE", (stm_input_t)TX_INPUT_M_RELEASE },
  { "TX_INPUT_S_ACQUIRE", (stm_input_t)TX_INPUT_S_ACQUIRE },
  { "TX_INPUT_S_RELEASE", (stm_input_t)TX_INPUT_S_RELEASE },
  { "TX_INPUT_ERROR"    , (stm_input_t)TX_INPUT_ERROR },

}; /* tx_mgr_stm_input_mapping */ 

/*----------------------------------------------------------------------------*/
/*! Structure holding the state transition functions. Transition functions are 
called in accordance with the current state the recipient state machine. */
static const stm_transition_fn_t
  tx_mgr_stm_transitions[ TX_STATE_NUM * TX_INPUT_NUM ] =
{

  /*--------------------------------------------------------------------------*/
  /* Transition functions for state TX_STATE_OFF */
  /*--------------------------------------------------------------------------*/
  tx_mgr_stm_off_proc_m_acquire, /* TX_INPUT_M_ACQUIRE */
  NULL, /* TX_INPUT_M_RELEASE */
  tx_mgr_stm_off_proc_s_acquire, /* TX_INPUT_S_ACQUIRE */
  NULL, /* TX_INPUT_S_RELEASE */
  tx_mgr_stm_any_state_proc_error, /* TX_INPUT_ERROR */

  /*--------------------------------------------------------------------------*/
  /* Transition functions for state TX_STATE_MASTER */
  /*--------------------------------------------------------------------------*/
  NULL, /* TX_INPUT_M_ACQUIRE */
  tx_mgr_stm_master_proc_m_release, /* TX_INPUT_M_RELEASE */
  tx_mgr_stm_master_proc_s_acquire, /* TX_INPUT_S_ACQUIRE */
  NULL, /* TX_INPUT_S_RELEASE */
  tx_mgr_stm_any_state_proc_error, /* TX_INPUT_ERROR */

  /*--------------------------------------------------------------------------*/
  /* Transition functions for state TX_STATE_MASTER_PSLAVE */
  /*--------------------------------------------------------------------------*/
  NULL, /* TX_INPUT_M_ACQUIRE */
  tx_mgr_stm_master_pslave_proc_m_release, /* TX_INPUT_M_RELEASE */
  NULL, /* TX_INPUT_S_ACQUIRE */
  tx_mgr_stm_master_pslave_proc_s_release, /* TX_INPUT_S_RELEASE */
  tx_mgr_stm_any_state_proc_error, /* TX_INPUT_ERROR */

  /*--------------------------------------------------------------------------*/
  /* Transition functions for state TX_STATE_SLAVE */
  /*--------------------------------------------------------------------------*/
  tx_mgr_stm_slave_proc_m_acquire, /* TX_INPUT_M_ACQUIRE */
  NULL, /* TX_INPUT_M_RELEASE */
  NULL, /* TX_INPUT_S_ACQUIRE */
  tx_mgr_stm_slave_proc_s_release, /* TX_INPUT_S_RELEASE */
  tx_mgr_stm_any_state_proc_error, /* TX_INPUT_ERROR */

  /*--------------------------------------------------------------------------*/
  /* Transition functions for state TX_STATE_ERROR */
  /*--------------------------------------------------------------------------*/
  NULL, /* TX_INPUT_M_ACQUIRE */
  NULL, /* TX_INPUT_M_RELEASE */
  NULL, /* TX_INPUT_S_ACQUIRE */
  NULL, /* TX_INPUT_S_RELEASE */
  NULL, /* TX_INPUT_ERROR */

}; /* tx_mgr_stm_transitions */ 

/*----------------------------------------------------------------------------*/
/*! Structure representing internal constant data of a state machine that is 
applicable to all instances */
static const stm_state_machine_constdata_t 
  tx_mgr_stm_constdata =
{

  TX_MGR_STM_NUM_INSTANCES, /* number of state machine instances */
  TX_STATE_NUM, /* number of states */
  tx_mgr_stm_state_mapping, /* array of state mappings */
  TX_INPUT_NUM, /* number of inputs */
  tx_mgr_stm_input_mapping, /* array of input mappings */
  tx_mgr_stm_transitions, /* array of transition function mappings */
  NULL, /* stm entry function */
  NULL, /* stm exit function */
  tx_mgr_stm_error, /* stm error func called upon any internal STM error */
  tx_mgr_stm_debug_hook, /* stm debug hook function */
  TX_STATE_OFF /* state machine initial state */

}; /* tx_mgr_stm_constdata*/

/*----------------------------------------------------------------------------*/
/*! Structure representing internal constant data of a state machine 
that is unique to each instance. */
static const stm_state_machine_perinst_constdata_t
  tx_mgr_stm_perinst_constdata[ TX_MGR_STM_NUM_INSTANCES ] =
{
  {
    &tx_mgr_stm_constdata, /*!< state machine constant data */
    "tx_mgr_stm_constdata[0]", /*!< state machine name */
    0x4b491150, /*!< stm ID (md5("tx_mgr_stm_constdata[0]") & 0xFFFFFFFF) */
    0 /*!< this state machine instance */
  },
  {
    &tx_mgr_stm_constdata, /*!< state machine constant data */
    "tx_mgr_stm_constdata[1]", /*!< state machine name */
    0x055c9626, /*!< stm ID (md5("tx_mgr_stm_constdata[1]") & 0xFFFFFFFF) */
    1 /*!< this state machine instance */
  }
}; /* stm_state_machine_perinst_constdata_t */

/*----------------------------------------------------------------------------*/
/* State machine instance array definition */
static stm_state_machine_t
  TX_MGR_STM[ TX_MGR_STM_NUM_INSTANCES ] =
{
  {
    &tx_mgr_stm_perinst_constdata[0], /*!< per instance constant data */
    STM_DEACTIVATED_STATE, /*!< current state */
    -1, /*!< current input index */
    NULL, /*!< calling SM instance */
    FALSE, /*!< propagate input to parent */
    FALSE, /*!< locked flag */
    NULL, /*!< user defined per-instance data */
    0, /*!< user defined debug mask */
  },
  {
    &tx_mgr_stm_perinst_constdata[1], /*!< per instance constant data */
    STM_DEACTIVATED_STATE, /*!< current state */
    -1, /*!< current input index */
    NULL, /*!< calling SM instance */
    FALSE, /*!< propagate input to parent */
    FALSE, /*!< locked flag */
    NULL, /*!< user defined per-instance data */
    0, /*!< user defined debug mask */
  }
}; /* TX_MGR_STM */

/*! @} */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_COMMON_TX_MGR_INTERFACE
 
  @brief
  This module exports the interfaces to client (MC layer) to perform
  Tx resource management such as requesting and releasing of several Tx 
  resources as master and slave. Clients are expected to use the exported
  interface to acquire the resource before performing any action.
  Similarly, the resources are expected to be released once there is no usage
  of the resource by the client.
 
  @image html rfcommon_tx_manager_interface.jpg
  @image html rfcommon_tx_manager_mc_callflow.jpg
 
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Tx resource manager
 
  @details
  This function performs initialization of Tx resource manager. This function
  initialized STM infrastructure along with initialization of housekeeping data.
  This initialization is required to be done only once during RFM 
  initialization.
 
  @return
  Flag indicating the status of tx resource manager initialization
*/
boolean
rf_tx_resource_mgr_init
(
  void
)
{
  boolean ret_val = TRUE;
  uint8 stm_idx = 0;
  rf_tx_resource_stm_payload_type stm_payload; /* STM payload */

  /* Perform init, only if it is not done */
  if ( tx_mgr_data.init_done == FALSE )
  {
    /* Initialize Critical section */
    ret_val &= rf_common_init_critical_section( &(tx_mgr_data.critical_sect) );

    /* Initialize Modem Chain Data */
    for ( stm_idx = 0; stm_idx < TX_MGR_STM_NUM_INSTANCES; stm_idx++ )
    {
      /* Activate STM for each resource */
      #ifndef FEATURE_GNSS_ONLY_NO_WWAN
      stm_activate( &TX_MGR_STM[stm_idx], NULL );
      #endif /* FEATURE_GNSS_ONLY_NO_WWAN */

      /* Disable F3 logging by default */
      tx_mgr_data.enable_f3 = TRUE;
      
      /* Enable Slave resource by default ==> ET mode capable */
      tx_mgr_data.skip_tx_mgr = FALSE;
      
      /* Initialize the Resouce Data for active and waiting client */
      tx_mgr_reset_resource_data(
         &(tx_mgr_data.resource_data[stm_idx].active_client), TRUE );
      tx_mgr_reset_resource_data(
         &(tx_mgr_data.resource_data[stm_idx].waiting_client), TRUE );

      /* Initialize the debug data */
      memset( &(tx_mgr_data.resource_data[stm_idx].debug_data), 0x0, 
              sizeof(tx_mgr_debug_data_type) );

      /* Move the STM to error state, if there is a mismatch in number
      of TxLM chains and STM instance */
      if ( TX_MGR_STM_NUM_INSTANCES > TXLM_CHAIN_NUM )
      {
        /* Populate STM payload */
        memset( &stm_payload, 0x0, sizeof(rf_tx_resource_stm_payload_type) );
        stm_payload.input_parameter.device = RFM_INVALID_DEVICE;
        stm_payload.input_parameter.mode = RFM_INVALID_MODE;
        stm_payload.input_parameter.modem_chain = stm_idx;
        stm_payload.input_parameter.error_reason = 
                                     TX_MGR_ERROR_MISMATCH_TX_RESOUCE;
        stm_payload.input_parameter.enable_f3 = tx_mgr_data.enable_f3;
        stm_payload.resource_data = &(tx_mgr_data.resource_data[stm_idx]);

        /* Process the Input in STM */
        stm_process_input( NULL,
                           &TX_MGR_STM[stm_idx],
                           TX_INPUT_ERROR,
                           &stm_payload );

        RF_MSG_2( RF_ERROR, "[TX_RESOURCE_MGR][Init]: TxLM Chain (%d) and"
                  "STM instance (%d) mismatch ", TXLM_CHAIN_NUM,
                  TX_MGR_STM_NUM_INSTANCES ); 
        ret_val = FALSE;
      } /* if ( TX_MGR_STM_NUM_INSTANCES >= TXLM_CHAIN_NUM ) */

    } /* if ( stm_idx = 0; stm_idx < TX_MGR_STM_NUM_INSTANCES; stm_idx++ ) */

    /* Indicate that initialization is done */
    tx_mgr_data.init_done = TRUE;

    RF_MSG( RF_MED, "[TX_RESOURCE_MGR][Init]: Initalized" );
  } /* if ( tx_mgr_data.init_done == FALSE ) */
  else
  {
    RF_MSG( RF_ERROR, "[TX_RESOURCE_MGR][Init]: Alreday Initalized" );
    ret_val = FALSE;

  } /* if ! ( tx_mgr_data.init_done == FALSE ) */

  return ret_val;
  
} /* rf_tx_resource_mgr_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Request master tx resource
 
  @details
  This function will grant tx resource as master on a given device for the
  desired WAN technology. This function will ensure that the requested resource,
  if already held as slave by other tech, is released before granting to new 
  tech.
 
  The functionality of this function is synchronous, i.e. the client are
  resource granted without any pending ownership of the resource by any other
  tech.
 
  Requesting a tx resource as a master, while it is already held as master
  resource by other tech is a error case; as this scenario should never happen.
  This arbitration should happen in RFM.MC layer.
 
  @param device
  RFM device for which the tx master resource is requested
 
  @param modem_chain
  Modem chain for which the resource is requested.
 
  @param mode
  RFM mode for which the tx master resource is requested
 
  @return
  Status indicating, if the reource was granted or not.
*/
boolean
rf_request_master_tx_resource
(
  rfm_device_enum_type device,
  txlm_chain_type modem_chain,
  rfm_mode_enum_type mode
)
{
  boolean ret_val = TRUE; /* API return value */
  stm_status_t stm_status = STM_SUCCESS; /* STM return status */
  rf_tx_resource_stm_payload_type stm_payload; /* STM payload */
  rf_time_tick_type prof_tick; /* Time profile tick */
  rf_time_type prof_time; /* Time profile time */

  /* Check if the Tx Manager is set to SKIP mode */
  if ( tx_mgr_data.skip_tx_mgr == TRUE )
  {
    RF_MSG_3( RF_HIGH, "[TX_RESOURCE_MGR][RequestMaster]: Dev %d, "
              "ModemChain %d, Mode %d **SKIPPED**", device, modem_chain, 
              mode ); 
    return TRUE;
  } /* if ( tx_mgr_data.skip_tx_mgr == TRUE ) */
  
  /* Param validation */
  if ( ( modem_chain >= TX_MGR_STM_NUM_INSTANCES ) ||
       ( modem_chain >= TXLM_CHAIN_MAX ) )
  {
    RF_MSG_3( RF_ERROR, "[TX_RESOURCE_MGR][RequestMaster]: Dev %d, "
              "Bad ModemChain %d, Mode %d, ", device, modem_chain, 
              mode ); 
    return FALSE;
  } /* if ( modem_chain >= TX_MGR_STM_NUM_INSTANCES ) */
  
  /* Enter Critical Section */
  rf_common_enter_critical_section( &(tx_mgr_data.critical_sect) );

  /* Start Profiling */
  prof_tick = rf_time_get_tick();

  RF_MSG_3( RF_MED, "[TX_RESOURCE_MGR][RequestMaster]: [START] Dev %d, "
                    "Mode %d, ModemChain %d", device, mode, modem_chain );
  
  /* Proceed only if the init is done */
  if ( tx_mgr_data.init_done == TRUE )
  {
    /* Populate STM payload */
    memset( &stm_payload, 0x0, sizeof(rf_tx_resource_stm_payload_type) );
    stm_payload.input_parameter.device = device;
    stm_payload.input_parameter.mode = mode;
    stm_payload.input_parameter.modem_chain = modem_chain;
    stm_payload.input_parameter.error_reason = TX_MGR_ERROR_NO_ERROR;
    stm_payload.input_parameter.enable_f3 = tx_mgr_data.enable_f3;
    stm_payload.resource_data = &(tx_mgr_data.resource_data[modem_chain]);

    /* Process the Input in STM */
    stm_status = stm_process_input( NULL,
                                    &TX_MGR_STM[modem_chain],
                                    TX_INPUT_M_ACQUIRE,
                                    &stm_payload );

    /* Update the return value based in STM return status */
    if ( stm_status != STM_SUCCESS )
    {
      RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][RequestMaster]: STM Process "
                          "failed with Error %d", stm_status ); 
      ret_val = FALSE;
    } /* if ( stm_status != STM_SUCCESS ) */
  } /* if ( tx_mgr_data.init_done == TRUE ) */
  else
  {
    RF_MSG_3( RF_ERROR, "[TX_RESOURCE_MGR][RequestMaster]: Dev %d, "
              "ModemChain %d, Mode %d - Init not done", 
              device, modem_chain, mode ); 

    ret_val = FALSE;
  } /* if ! ( tx_mgr_data.init_done == TRUE ) */

  /* End Profiling */
  prof_time = rf_time_get_elapsed( prof_tick, RF_USEC );
  
  RF_MSG_5( RF_MED, "[TX_RESOURCE_MGR][RequestMaster]: [Status %d] Dev %d, "
            "ModemChain %d, Mode %d ProfileTime %d us", 
            ret_val, device, modem_chain, mode, prof_time );

  /* Leave Critical Section */
  rf_common_leave_critical_section( &(tx_mgr_data.critical_sect) );

  return ret_val;

} /* rf_request_master_tx_resource */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Release master tx resource
 
  @details
  This function will release tx master resource on a given device for the
  desired WAN technology. After the master resource is released, this function 
  will ensure that, if there is a pending request, it is granted as slave
  device to other client.
 
  The functionality of this function is synchronous.
 
  @param device
  RFM device for which the tx master resource is to be released
 
  @param modem_chain
  Modem chain for which the resource is to be released.
 
  @param mode
  RFM mode for which the tx master resource to be released
 
  @return
  Status indicating, if the reource was released or not.
*/
boolean
rf_release_master_tx_resource
(
  rfm_device_enum_type device,
  txlm_chain_type modem_chain,
  rfm_mode_enum_type mode
)
{
  boolean ret_val = TRUE; /* API return value */
  stm_status_t stm_status = STM_SUCCESS; /* STM return status */
  rf_tx_resource_stm_payload_type stm_payload; /* STM payload */
  rf_time_tick_type prof_tick; /* Time profile tick */
  rf_time_type prof_time; /* Time profile time */
  
  /* Check if the Tx Manager is set to SKIP mode */
  if ( tx_mgr_data.skip_tx_mgr == TRUE )
  {
    RF_MSG_3( RF_HIGH, "[TX_RESOURCE_MGR][ReleaseMaster]: Dev %d, "
              "ModemChain %d, Mode %d **SKIPPED**", device, modem_chain, 
              mode ); 
    return TRUE;
  } /* if ( tx_mgr_data.skip_tx_mgr == TRUE ) */

  /* Param validation */
  if ( ( modem_chain >= TX_MGR_STM_NUM_INSTANCES ) ||
       ( modem_chain >= TXLM_CHAIN_MAX ) )
  {
    RF_MSG_3( RF_ERROR, "[TX_RESOURCE_MGR][ReleaseMaster]: Dev %d, "
              "Bad ModemChain %d, Mode %d", device, modem_chain, 
              mode ); 
    return FALSE;
  } /* if ( modem_chain >= TX_MGR_STM_NUM_INSTANCES ) */
  
  /* Enter Critical Section */
  rf_common_enter_critical_section( &(tx_mgr_data.critical_sect) );
    
  /* Start Profiling */
  prof_tick = rf_time_get_tick();

  RF_MSG_3( RF_MED, "[TX_RESOURCE_MGR][ReleaseMaster]: [START] Dev %d, "
                    "Mode %d, ModemChain %d", device, mode, modem_chain );

  /* Proceed only if the init is done */
  if ( tx_mgr_data.init_done == TRUE )
  {
    /* Populate STM payload */
    memset( &stm_payload, 0x0, sizeof(rf_tx_resource_stm_payload_type) );
    stm_payload.input_parameter.device = device;
    stm_payload.input_parameter.mode = mode;
    stm_payload.input_parameter.modem_chain = modem_chain;
    stm_payload.input_parameter.error_reason = TX_MGR_ERROR_NO_ERROR;
    stm_payload.input_parameter.enable_f3 = tx_mgr_data.enable_f3;
    stm_payload.resource_data = &(tx_mgr_data.resource_data[modem_chain]);

    /* Process the Input in STM */
    stm_status = stm_process_input( NULL,
                                    &TX_MGR_STM[modem_chain],
                                    TX_INPUT_M_RELEASE,
                                    &stm_payload );

    /* Update the return value based in STM return status */
    if ( stm_status != STM_SUCCESS )
    {
      RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][ReleaseMaster]: STM Process "
                          "failed with Error %d", stm_status ); 
      ret_val = FALSE;
    } /* if ( stm_status != STM_SUCCESS ) */
  } /* if ( tx_mgr_data.init_done == TRUE ) */
  else
  {
    RF_MSG_3( RF_ERROR, "[TX_RESOURCE_MGR][ReleaseMaster]: Dev %d, "
              "ModemChain %d, Mode %d - Init not done", 
              device, modem_chain, mode ); 

    ret_val = FALSE;
  } /* if ! ( tx_mgr_data.init_done == TRUE ) */
  
  /* End Profiling */
  prof_time = rf_time_get_elapsed( prof_tick, RF_USEC );

  RF_MSG_5( RF_MED, "[TX_RESOURCE_MGR][ReleaseMaster]: [Status %d] Dev %d, "
            "ModemChain %d, Mode %d ProfileTime %d us", 
            ret_val, device, modem_chain, mode, prof_time );

  /* Leave Critical Section */
  rf_common_leave_critical_section( &(tx_mgr_data.critical_sect) );

  return ret_val;

} /* rf_release_master_tx_resource */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Request slave tx resource
 
  @details
  This function will grant tx resource as slave on a given device for the
  desired WAN technology. This function will ensure that the requested resource
  is configured by to slave mode by calling the setup function passed as a
  part of config_data.
 
  The functionality of this function is synchronous.
 
  @param device
  RFM device for which the tx slave resource is requested
 
  @param modem_chain
  Modem chain for which the resource is requested.
 
  @param mode
  RFM mode for which the tx slave resource is requested
 
  @param config_data
  Configuration data for Tx manager containing the function pointers to be
  used to move between Tx modes.
 
  @return
  Status indicating, if the reource was granted or not.
*/
boolean
rf_request_xpt_tx_resource
(
  rfm_device_enum_type device,
  txlm_chain_type modem_chain,
  rfm_mode_enum_type mode,
  rf_tx_resource_mgr_config_data_type config_data
)
{
  boolean ret_val = TRUE; /* API return value */
  stm_status_t stm_status = STM_SUCCESS; /* STM return status */
  rf_tx_resource_stm_payload_type stm_payload; /* STM payload */
  rf_time_tick_type prof_tick; /* Time profile tick */
  rf_time_type prof_time; /* Time profile time */

  /* Check if the Tx Manager is set to SKIP mode */
  if ( tx_mgr_data.skip_tx_mgr == TRUE )
  {
    RF_MSG_3( RF_HIGH, "[TX_RESOURCE_MGR][RequestXPT]: Dev %d, "
              "ModemChain %d, Mode %d **SKIPPED**", device, modem_chain, 
              mode ); 
    return TRUE;
  } /* if ( tx_mgr_data.skip_tx_mgr == TRUE ) */
  
  /* Param validation */
  if ( ( modem_chain >= TX_MGR_STM_NUM_INSTANCES ) ||
       ( modem_chain >= TXLM_CHAIN_MAX ) )
  {
    RF_MSG_3( RF_ERROR, "[TX_RESOURCE_MGR][RequestXPT]: Dev %d, "
              "Bad ModemChain %d, Mode %d", device, modem_chain, 
              mode ); 
    return FALSE;
  } /* if ( modem_chain >= TX_MGR_STM_NUM_INSTANCES ) */
  
  /* Enter Critical Section */
  rf_common_enter_critical_section( &(tx_mgr_data.critical_sect) );
 
  /* Start Profiling */
  prof_tick = rf_time_get_tick();

  RF_MSG_3( RF_MED, "[TX_RESOURCE_MGR][RequestXPT]: [START] Dev %d, "
                    "Mode %d, ModemChain %d", device, mode, modem_chain );

  /* Proceed only if the init is done */
  if ( tx_mgr_data.init_done == TRUE )
  {
    /* Populate STM payload */
    memset( &stm_payload, 0x0, sizeof(rf_tx_resource_stm_payload_type) );
    stm_payload.input_parameter.device = device;
    stm_payload.input_parameter.mode = mode;
    stm_payload.input_parameter.modem_chain = modem_chain;
    stm_payload.input_parameter.error_reason = TX_MGR_ERROR_NO_ERROR;
    stm_payload.input_parameter.res_config = config_data;
    stm_payload.input_parameter.enable_f3 = tx_mgr_data.enable_f3;
    stm_payload.resource_data = &(tx_mgr_data.resource_data[modem_chain]);

    /* Process the Input in STM */
    stm_status = stm_process_input( NULL,
                                    &TX_MGR_STM[modem_chain],
                                    TX_INPUT_S_ACQUIRE,
                                    &stm_payload );

    /* Update the return value based in STM return status */
    if ( stm_status != STM_SUCCESS )
    {
      RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][RequestXPT]: STM Process "
                          "failed with Error %d", stm_status ); 
      ret_val = FALSE;
    } /* if ( stm_status != STM_SUCCESS ) */
  } /* if ( tx_mgr_data.init_done == TRUE ) */
  else
  {
    RF_MSG_3( RF_ERROR, "[TX_RESOURCE_MGR][RequestXPT]: Dev %d, "
              "ModemChain %d, Mode %d - Init not done", 
              device, modem_chain, mode ); 

    ret_val = FALSE;
  } /* if ! ( tx_mgr_data.init_done == TRUE ) */
  
  /* End Profiling */
  prof_time = rf_time_get_elapsed( prof_tick, RF_USEC );

  RF_MSG_5( RF_MED, "[TX_RESOURCE_MGR][RequestXPT]: [Status %d] Dev %d, "
            "ModemChain %d, Mode %d ProfileTime %d us", 
            ret_val, device, modem_chain, mode, prof_time );

  /* Leave Critical Section */
  rf_common_leave_critical_section( &(tx_mgr_data.critical_sect) );

  return ret_val;

} /* rf_request_xpt_tx_resource */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Release slave tx resource
 
  @details
  This function will release tx slave resource on a given device for the
  desired WAN technology. This function will ensure that the requested resource
  is de-configured from slave mode by calling the cleanup function passed as a
  part of config_data.
 
  The functionality of this function is synchronous.
 
  @param device
  RFM device for which the tx slave resource is to be released
 
  @param modem_chain
  Modem chain for which the resource is to be released.
 
  @param mode
  RFM mode for which the tx slave resource is to be released
 
  @return
  Status indicating, if the reource was released or not.
*/
boolean
rf_release_xpt_tx_resource
(
  rfm_device_enum_type device,
  txlm_chain_type modem_chain,
  rfm_mode_enum_type mode
)
{
  boolean ret_val = TRUE; /* API return value */
  stm_status_t stm_status = STM_SUCCESS; /* STM return status */
  rf_tx_resource_stm_payload_type stm_payload; /* STM payload */
  rf_time_tick_type prof_tick; /* Time profile tick */
  rf_time_type prof_time; /* Time profile time */
  
  /* Check if the Tx Manager is set to SKIP mode */
  if ( tx_mgr_data.skip_tx_mgr == TRUE )
  {
    RF_MSG_3( RF_HIGH, "[TX_RESOURCE_MGR][ReleaseXPT]: Dev %d, "
              "ModemChain %d, Mode %d **SKIPPED**", device, modem_chain, 
              mode ); 
    return TRUE;
  } /* if ( tx_mgr_data.skip_tx_mgr == TRUE ) */

  /* Param validation */
  if ( ( modem_chain >= TX_MGR_STM_NUM_INSTANCES ) ||
       ( modem_chain >= TXLM_CHAIN_MAX ) )
  {
    RF_MSG_3( RF_ERROR, "[TX_RESOURCE_MGR][ReleaseXPT]: Dev %d, "
              "Bad ModemChain %d, Mode %d", device, modem_chain, 
              mode ); 
    return FALSE;
  } /* if ( modem_chain >= TX_MGR_STM_NUM_INSTANCES ) */
  
  /* Enter Critical Section */
  rf_common_enter_critical_section( &(tx_mgr_data.critical_sect) );
    
  /* Start Profiling */
  prof_tick = rf_time_get_tick();

  RF_MSG_3( RF_MED, "[TX_RESOURCE_MGR][ReleaseXPT]: [START] Dev %d, "
                    "Mode %d, ModemChain %d", device, mode, modem_chain );
  
  /* Proceed only if the init is done */
  if ( tx_mgr_data.init_done == TRUE )
  {
    /* Populate STM payload */
    memset( &stm_payload, 0x0, sizeof(rf_tx_resource_stm_payload_type) );
    stm_payload.input_parameter.device = device;
    stm_payload.input_parameter.mode = mode;
    stm_payload.input_parameter.modem_chain = modem_chain;
    stm_payload.input_parameter.error_reason = TX_MGR_ERROR_NO_ERROR;
    stm_payload.input_parameter.enable_f3 = tx_mgr_data.enable_f3;
    stm_payload.resource_data = &(tx_mgr_data.resource_data[modem_chain]);

    /* Process the Input in STM */
    stm_status = stm_process_input( NULL,
                                    &TX_MGR_STM[modem_chain],
                                    TX_INPUT_S_RELEASE,
                                    &stm_payload );

    /* Update the return value based in STM return status */
    if ( stm_status != STM_SUCCESS )
    {
      RF_MSG_1( RF_ERROR, "[TX_RESOURCE_MGR][ReleaseXPT]: STM Process "
                          "failed with Error %d", stm_status ); 
      ret_val = FALSE;
    } /* if ( stm_status != STM_SUCCESS ) */
  } /* if ( tx_mgr_data.init_done == TRUE ) */
  else
  {
    RF_MSG_3( RF_ERROR, "[TX_RESOURCE_MGR][ReleaseXPT]: Dev %d, "
              "ModemChain %d, Mode %d - Init not done", 
              device, modem_chain, mode ); 

    ret_val = FALSE;
  } /* if ! ( tx_mgr_data.init_done == TRUE ) */
  
  /* End Profiling */
  prof_time = rf_time_get_elapsed( prof_tick, RF_USEC );

  RF_MSG_5( RF_MED, "[TX_RESOURCE_MGR][ReleaseXPT]: [Status %d] Dev %d, "
            "ModemChain %d, Mode %d ProfileTime %d us", 
            ret_val, device, modem_chain, mode, prof_time );
  
  /* Leave Critical Section */
  rf_common_leave_critical_section( &(tx_mgr_data.critical_sect) );

  return ret_val;

} /* rf_release_xpt_tx_resource */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_COMMON_TX_MGR_OFT_INTF
 
  @brief
  This module exports interfaces used by off-target framework. <b>These
  interfaces will be compiled only for off-target builds</b>.
  @{
*/

#ifdef T_WINNT
#error code not present
#endif /* #ifdef T_WINNT */

/*! @} */


