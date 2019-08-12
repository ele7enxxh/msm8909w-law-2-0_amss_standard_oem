/*============================================================================
@file dbgmgr.h

Public interface for the power debug scenario callback framework.

Copyright (c) 2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.mpss/3.5.c12.3/api/power/dbgmgr.h#1 $
============================================================================*/
#ifndef DBGMGR_H
#define DBGMGR_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_SCENARIO_STRING_SIZE    64          /**< -- Maximum # of bytes needed to describe a debug scenario  */
#define DM_SCENARIO_FLAG_DISABLE    0           /**< -- Flag to disable a debug scenario  */
#define DM_SCENARIO_FLAG_ENABLE     1           /**< -- Flag to enable a debug scenario  */

#define DM_EVENT_LOG_START          (1 << 0)    /**< -- Event describing that tracing has started on all enabled scenarios  */
#define DM_EVENT_LOG_STOP           (1 << 1)    /**< -- Event describing that tracing has stopped on all enabled scenarios  */
#define DM_EVENT_LOG_WRAP           (1 << 2)    /**< -- Event describing that a wrap around has occurred in the trace log file or buffer. All enabled scenarios will be notified of this event */
#define DM_EVENT_LOG_MASK_ALL       ((1 << 3)-1)/**< -- Event mask taking all events into account */ 

#define DM_SUCCESS                   0          /**< -- Success return code  */
#define DM_ERROR                    -1          /**< -- Error code describing a general failure */
#define DM_NOMEM                    -2          /**< -- Error code describing a memory allocation failure */
#define DM_INVALID                  -3          /**< -- Error code describing an invalid operation */
#define DM_BADPARAM                 -4          /**< -- Error code describing invalid parameters passed into an API */
#define DM_NOTFOUND                 -5          /**< -- Error code describing an item/resource was not found */
#define DM_UNSUPPORTED              -6          /**< -- Error code describing an unsupported operation */
#define DM_UNINITIALIZED            -7          /**< -- Error code describing an uninitialized operation */

/**
  @brief Handle returned to clients when registering a debug scenario.

  @detail
  A handle is returned to the client when API register_scenario_callback
  is called.
 */
typedef void*                         dm_scenario_handle_t;
#define INVALID_DM_SCENARIO_HANDLE_T  (NULL)    /**< -- An invalid handle  */

/**
  @brief Client callback function prototype to be registered with 
  the scenario manager.

  @detail
  This prototype accepts a scenario name and 
  the DM_EVENT_* bit mask of interest.
 */
typedef int(*pfnscncb)(const char* scenario_name, uint32_t evt_mask);

/**
  @brief
  API for clients to register a callback.

  @param p_handle         - [OUT] Valid pointer to a DM Scenario handle
  @param scenario_name    - [IN]  NULL terminated scenario string of interest
  @param pfn              - [IN]  Client callback function pointer
  @param evt_mask         - [IN]  DM_EVENT_* bit mask the client would like 
                                  to be notified about

  @note 
  At most MAX_SCENARIO_STRING_SIZE bytes of scenario_name will be accepted.
  
  @return   
  DM_SUCCESS     -- Success, client callback registered along with event mask.
  DM_BADPARAM    -- Bad parameters were passed into API.
  DM_UNSUPPORTED -- API not supported on the platform.
  DM_NOMEM       -- Memory allocation failure.
  DM_ERROR       -- All other errors.
  
  @sa  dbgmgr_unregister_scenario_callback
  @sa  dbgmgr_update_scenario_event_mask
*/
int dbgmgr_register_scenario_callback
(
  dm_scenario_handle_t *p_handle,
  const char *scenario_name,
  pfnscncb pfn,
  uint32_t evt_mask
);

/**
  @brief
  API for clients to register a callback to recieve notifications of 
  DM_EVENT_* without specifying a scenario of interest. This would amount
  to receiving any change in the state of the logger.

  @param p_handle         - [OUT] Valid pointer to a DM Scenario handle
  @param pfn              - [IN]  Client callback function pointer
  @param evt_mask         - [IN]  DM_EVENT_* bit mask the client would like
  to be notified about

  @return
  DM_SUCCESS     -- Success, client callback registered along with event mask.
  DM_BADPARAM    -- Bad parameters were passed into API.
  DM_UNSUPPORTED -- API not supported on the platform.
  DM_NOMEM       -- Memory allocation failure.
  DM_ERROR       -- All other errors.

  @sa  dbgmgr_unregister_scenario_callback
  @sa  dbgmgr_update_scenario_event_mask
*/
int dbgmgr_register_any_scenario_callback
(
  dm_scenario_handle_t *p_handle,
  pfnscncb pfn,
  uint32_t evt_mask
);

/**
  @brief
  API for clients to un-register their callback. A valid handle returned 
  from API dbgmgr_register_scenario_callback must be provided.

  @param handle        - [IN]  Valid DM Scenario handle

  @return   
  DM_SUCCESS     -- Success, client callback un-registered.
  DM_BADPARAM    -- Bad parameters were passed into API.
  DM_UNSUPPORTED -- API not supported on the platform.
  DM_ERROR       -- All other errors.
  
  @sa  dbgmgr_register_scenario_callback
  @sa  dbgmgr_update_scenario_event_mask
*/
int dbgmgr_unregister_scenario_callback(dm_scenario_handle_t handle);

/**
  @brief
  API for clients to modify event mask at runtime. A valid handle returned 
  from API dbgmgr_register_scenario_callback must be provided.

  @param handle        - [IN]  Valid DM Scenario handle
  @param evt_mask      - [IN]  DM_EVENT_* bit mask the client would like 
                               to be notified about

  @return
  DM_SUCCESS     -- Success, client event bit mask was updated.
  DM_BADPARAM    -- Bad parameters were passed into API.
  DM_UNSUPPORTED -- API not supported on the platform.
  DM_ERROR       -- All other errors.
  
  @sa  dbgmgr_register_scenario_callback
  @sa  dbgmgr_unregister_scenario_callback
*/
int dbgmgr_update_scenario_event_mask
(
  dm_scenario_handle_t handle, 
  uint32_t event
);

#ifdef __cplusplus
}
#endif

#endif //DBGMGR_H
