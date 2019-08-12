/*============================================================================
@file dbgmgr_fwk.h

Restricted interface for the power debug scenario callback framework.

Copyright (c) 2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.mpss/3.5.c12.3/power/dbgmgr/inc/dbgmgr_fwk.h#1 $
============================================================================*/
#ifndef DBGMGR_FWK_H
#define DBGMGR_FWK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
  @brief
  API to initialize the scenario manager.

  @note This is not a client API. It is expected that the logging framework 
  using the scenario manager will invoke this API.

  @note In the case of an error an exception will be generated.

  @return
  None.
*/
void dbgmgr_scenario_init(void);

/**
  @brief
  API to notify the scenario manager of an enable or disable request of a 
  particular scenario.

  @note This is not a client API. It is expected that the logging framework 
  using the scenario manager will invoke this API.

  @note 
  At most MAX_SCENARIO_STRING_SIZE bytes of scenario_name will be accepted.

  @param scenario_name    - [IN]  NULL terminated scenario string of interest.
  @param scenario_name_sz - [IN]  Size of the scenario_name buffer passed in.
  @param flag             - [IN]  PSCM_SCENARIO_FLAG* bit mask the scenario
                                  manager should be notified about.
  @return   
  DM_SUCCESS     -- Success, client callback registered along with event mask.
  DM_BADPARAM    -- Bad parameters were passed into API.
  DM_UNSUPPORTED -- API not supported on the platform.
  DM_ERROR       -- All other errors.
*/
int dbgmgr_scenario_control
(
  const char *scenario_name,
  size_t scenario_name_sz,
  uint32_t flag
);

/**
  @brief
  API to notify the scenario manager of a PSCM_EVENT_* that has occurred. 
  It is expected that when logging starts, DM_EVENT_LOG_START be passed in.
  It is expected that when logging stops, PSCM_EVENT_LOG_STOP be passed in.
  It is expected that when logging wraps, PSCM_EVENT_LOG_WRAP be passed in.
  All clients of all enabled scenarios will be notified of the event.

  @note This is not a client API. It is expected that the logging framework 
  using the scenario manager will invoke this API.

  @param evt_mask      - [IN]   PSCM_EVENT_* bit mask the scenario manager
                                should be notified about.

  @return
  DM_SUCCESS     -- Success, client event bit mask was updated.
  DM_UNSUPPORTED -- API not supported on the platform.
  DM_ERROR       -- All other errors.
*/
int dbgmgr_scenario_handle_event(uint32_t event);

typedef enum
{
  JSON_TYPE_STRING = 0,   /**< JSON data type specification for char string */
  JSON_TYPE_CHARACTER,    /**< JSON data type specification for a character  */
  JSON_TYPE_INTEGER,      /**< JSON data type specification for an integer */
  JSON_TYPE_BOOLEAN,      /**< JSON data type specification for an boolean */
  JSON_TYPE_INVALID,      /**< JSON data type invalid */
} json_data_t;

/**
  @brief API to parse a string containing an array specified in JSON.
  It is expected to be called in a loop until no more elements are to be read.
  
  @param msg        [in]  Valid string pointer to the start of the array
  @param msg_size   [in]  Size of the string
  @param data_type  [in]  Data type expected to be contained in the array
  @param p_idx      [in]  Valid pointer to an index currently being processed 
                          in the string
  @param p_dest     [out] Buffer to contain an element read from the string
  @param dest_sz    [in]  Size of the buffer in bytes
  @param p_num_bytes_written  [out]  Optional. If not null, 
                              the number of bytes written into p_dest will be
                              update.

  @return
  DAL_SUCCESS                 -- Success, p_dest will contain the element.
                                 If p_num_bytes_written is valid, it will be 
                                 updated.
  DAL_ERROR_INVALID_PARAMETER -- Bad parameters passed in.
  DAL_ERROR_INVALID_DATA      -- Incorrectly formatted JSON string passed in
  DAL_ERROR_NOT_SUPPORTED     -- API not supported on the platform
  DAL_ERROR_NOT_FOUND         -- No more elements found
  DAL_ERROR                   -- All other errors.  
*/
int parse_json_array
(
  const char  *msg,
  size_t      msg_size,
  json_data_t data_type,
  size_t      *p_idx,
  void        *p_dest,
  size_t      dest_sz,
  size_t      *p_num_bytes_written
);

#ifdef __cplusplus
}
#endif

#endif  //DBGMGR_FWK_H
