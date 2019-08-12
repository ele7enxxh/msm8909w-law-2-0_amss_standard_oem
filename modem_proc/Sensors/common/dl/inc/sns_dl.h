/*==============================================================================
  @file sns_dl.h

  @brief
  Header file describing the SNS Dynamic Loading service (DL svc) required for
  reading the list of boot critical shared object based SNS DDF based drivers
  and SAM based SNS algorithms from the registry.

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/
#ifndef SNS_DL_H
#define SNS_DL_H

#include <stdint.h>
#include <stdbool.h>
#include "sns_common.h"
#include "sns_dl_reg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
  @brief Handle returned to clients when registering with the DL service
  (via sns_dl_svc_create).
*/
typedef void* sns_dl_svc_handle_t;

/**< -- A static SNS DL svc handle  */
#define SNS_DL_HANDLE_INITIALIZER        (NULL)
/**< -- Macro defining a number used for checking init state of data structs */
#define SNS_DL_INIT_MAGIC           0xF1F1F1F1
/**< -- Size of client name using the DL svc */
#define SNS_DL_CLIENT_NAME_SZ       16

/**
  @brief Identifier to uniquely describe a SNS driver or algorithm
*/
typedef enum
{
  SNS_DL_ID_TYPE_UUID = 0,  /**<-- Used to uniquely identify a SNS driver */
  SNS_DL_ID_TYPE_SUID,      /**<-- Used to uniquely identify a SNS algorithm */
  SNS_DL_ID_TYPE_INVALID    /**<-- Invalid identifier */
} sns_dl_identifier_t;

/**
  @brief Function definition for clients to supply their own dynamic memory
  allocator

  @detail
  Semantics are similar to C standard API malloc().
*/
typedef void* (*pfn_sns_dl_malloc)(size_t num_bytes);

/**
  @brief Function definition for clients to supply their own dynamic memory
  de-allocator.

  @detail
  Semantics are similar to C standard API free().
*/
typedef void  (*pfn_sns_dl_free)(void *p);

/**
  @brief Callback function definition for clients to be called indicating
  whether or not the DL svc is initialized or not for a supplied handle.

  @param[out] -- Indicates whether or not DL svc has fully initialized
  @param[out] -- Error code returned when determining the initialization status
*/
typedef void  (*pfn_is_initialized_cb)(void *param,
                                       bool is_initialized,
                                       sns_err_code_e err_code);

/**
  @brief DL SVC attribute data structure used to when creating a handle
*/
typedef struct
{
  /**<--  Field denoting whether this structure was initialized or not */
  uint32_t              init_magic;
  /**<--  Client name when using creating a handle to DL SVC */
  char                  name[SNS_DL_CLIENT_NAME_SZ];
  /**<--  Allocator API to be used for any memory allocations required
          for DL SVC APIs. It is expected that this function pointer stays
          valid until the process that this library is used in terminates */
  pfn_sns_dl_malloc     pfn_malloc;
  /**<--  Deallocator API to be used for any memory de-allocations required
          for DL SVC APIs. It is expected that this function pointer stays
          valid until the process that this library is used in terminates */
  pfn_sns_dl_free       pfn_free;
  /**<--  Data field describing a caller's preference to block or not block
          until all DL SVC dependencies have been met */
  bool                  is_blocking;
  /**<--  Time out value in microseconds a caller is prepared to block until
          all DL SVC dependencies have been fully initialized */
  uint32_t              timeout_us;
  /**<--  Pointer to a callback function indicating initialization state  */
  pfn_is_initialized_cb pfn_init_cb;
  /**<--  Parameter to be passed in along with the callback function */
  void                  *init_cb_param;
  /**<--  Flag to indicate whether DL config should be read from the
          SNS registry or be read from a static config table at the
          local processor */
  bool                  use_local_registry;
} sns_dl_attr_t;

/**
  @brief API to initialize the DL SVC structure
  @param[in] p_attr     -- Valid pointer to an attribute object

  @return
    SNS_SUCCESS - Call successful
    SNS_ERR_BAD_PTR - Invalid pointer
*/
sns_err_code_e sns_dl_attr_init(sns_dl_attr_t *p_attr);

/**
  @brief API to de-initialize the DL SVC structure

  @param[in] p_attr     -- Valid pointer to an attribute object
  @return
    SNS_SUCCESS - Call successful
    Failure otherwise
*/
sns_err_code_e sns_dl_attr_deinit(sns_dl_attr_t *p_attr);

/**
  @brief API to assign an allocator and deallocator API for the
  DL SVC operations.

  @param[in] p_attr     -- Valid pointer to an attribute object
  @param[in] pfn_malloc -- Valid pointer allocator API
  @param[in] pfn_free   -- Valid pointer de-allocator API

  @return
    SNS_SUCCESS - Call successful
    SNS_ERR_BAD_PARM - Bad parameter was passed in.
*/
sns_err_code_e sns_dl_set_attr_allocator
(
  sns_dl_attr_t     *p_attr,
  pfn_sns_dl_malloc pfn_malloc,
  pfn_sns_dl_free   pfn_free
);

/**
  @brief API to describe how a client could either block for a period
  specified by time out and/or issue a callback with the result of
  initialization of DL SVC handle.

  @note: The client is expected to block until the initialization dependencies
  are met or provide a callback which will be issued once the time out expires.
  If a callback is provided, it will always be invoked either on a successful
  initialization, a failure or time out.

  @param[in] p_attr       --  Valid pointer to an attribute object
  @param[in] should_block --  Boolean denoting weather the caller should be
                              blocked or not waiting on initialization to
                              complete.
  @param[in] timeout_us   --  Time out in usec to block the caller until all
                              DL SVC initialization dependencies are met.
  @param[in] pfn          --  Callback function pointer that is called on a
                              successful initialization, a failure or time out.
  @param[in] init_cb_param -- Caller supplied parameter to be passed in when
                              the callback is issued.

  @return
    SNS_SUCCESS - Call successful
    SNS_ERR_BAD_PARM - Bad parameter was passed in.
*/
sns_err_code_e sns_dl_set_attr_blocking_info
(
  sns_dl_attr_t *p_attr,
  bool          should_block,
  uint32_t      timeout_us,
  pfn_is_initialized_cb pfn,
  void          *init_cb_param
);

/**
  @brief API to associate a name with the handle which is supplied by
  the caller. This is primarily used for debug purposes.

  @param[in] p_attr   --  Valid pointer to an attribute object
  @param[in] name     --  NULL terminated string containing the name
  @param[in] name_sz  --  Size of the name buffer

  @return
    SNS_SUCCESS - Call successful
    SNS_ERR_BAD_PARM - Bad parameter was passed in.
*/
sns_err_code_e sns_dl_set_attr_name
(
  sns_dl_attr_t *p_attr,
  const char *name,
  size_t name_sz
);

/**
  @brief API to retrieve the name associated with a DL SVC handle.

  @param[in]  p_attr    --  Valid pointer to an attribute object
  @param[out] name      --  Buffer that will contain the name
  @param[in]  name_sz   --  Size of the name buffer

  @return
    SNS_SUCCESS - Call successful
    SNS_ERR_BAD_PARM - Bad parameter was passed in.
*/
sns_err_code_e sns_dl_get_attr_name
(
  sns_dl_attr_t *p_attr,
  char *name,
  size_t name_sz
);

/**
  @brief API to select whether the local or SNS registry should be consulted
  for dynamic loading of SNS algos and drivers.

  @param[in] p_attr     -- Valid pointer to an attribute object

  @return
    SNS_SUCCESS - Call successful
    Failure otherwise
*/
sns_err_code_e sns_dl_attr_use_local_registry(sns_dl_attr_t *p_attr);

/**
  @brief API to initialize the SNS DL SVC. This API is expected to be called
  before calls to any other APIs. Typically expected to be called around
  process init.
*/
sns_err_code_e sns_dl_init(void);

/**
  @brief API to de-initialize the SNS DL SVC. This API is expected to be called
  before calls to any other APIs. Typically expected to be called around
  process destroy.
*/
sns_err_code_e sns_dl_deinit(void);

/**
  @brief API to create a handle to DL SVC. Using this handle, callers can
  query or walk through the list of SNS driver and algorithms.

  @param[in]  p_handle  --  Valid pointer to a DL SVC handle
  @param[in]  p_attr    --  Valid pointer to an attribute object

  @return
    SNS_SUCCESS         - Call successful
    SNS_ERR_NOTALLOWED  - Operation is not allowed
    SNS_ERR_BAD_PARM    - Bad parameter was passed in
    SNS_ERR_NOMEM       - Out of memory error
    SNS_ERR_WOULDBLOCK  - Time out occurred when initializing the DL SVC
*/
sns_err_code_e sns_dl_svc_create
(
  sns_dl_svc_handle_t *p_handle,
  sns_dl_attr_t       *p_attr
);

/**
  @brief API to create a handle to DL SVC. This can used either to query or
  load

  @param[in]  handle    --  A valid handle

  @return
    SNS_SUCCESS         - Call successful
    SNS_ERR_NOTALLOWED  - Operation is not allowed
    SNS_ERR_BAD_PARM    - Bad parameter was passed in
*/
sns_err_code_e sns_dl_svc_destroy(sns_dl_svc_handle_t handle);

/**
  @brief API to check if the DL SVC is fully initialized.

  @param[in]  handle    --  A valid handle
  @param[in]  pfn       --  Function pointer invoked after determining if the DL
                            SVC if fully initialized.
  @param[in]  param     --  Parameter to be passed into the callback function
  @param[in]  timeout_us  --  Time out to block before returning back to the
                              caller. If the timeout is 0, the caller is
                              blocked until the service is fully initialized.

  @return
    SNS_SUCCESS         - Call successful
    SNS_ERR_NOTALLOWED  - Operation is not allowed
    SNS_ERR_BAD_PARM    - Bad parameter was passed in
    SNS_ERR_NOMEM       - Out of memory error
    SNS_ERR_WOULDBLOCK  - Time out occurred when initializing the DL SVC
*/
sns_err_code_e
sns_dl_svc_is_initialized(sns_dl_svc_handle_t svc_handle,
                            pfn_is_initialized_cb pfn,
                            void *param,
                            uint32_t timeout_us);

/**
  @brief Function type that will be invoked for every instance in a set of
  dynamic shared object based SNS drivers and algorithms looked up from the
  SNS registry. This to be specified as a parameter in the sns_dl_reg_for_each_*
  or sns_dl_reg_search_* APIs.

  @param[in]  p_cfg_param    --  Pointer to a valid DL cfg struct that will be
                                 filled up with information required to load
                                 a module dynamically
*/
typedef void (*pfn_sns_reg_for_each)(sns_reg_dyn_cfg_t *p_cfg_param);

/**
  @brief  API to iterate over all the boot critical SNS algorithms to be
          dynamically loaded at SNS init.

  @param[in]  handle    --  A valid handle
  @param[in]  pfn       --  Function pointer callback to be called for every
                            SNS algorithm
  @param[in]  p_cfg     --  Parameter to be passed into the callback function
  @param[in]  timeout_us  --  Time out to block before returning back to the
                              caller. If the timeout is 0, the caller is
                              blocked until the entire list of algorithms is
                              determined.

  @return
    SNS_SUCCESS         - Call successful
    SNS_ERR_BAD_PARM    - Bad parameter was passed in
    SNS_ERR_NOMEM       - Out of memory error
    SNS_ERR_WOULDBLOCK  - Time out occurred before the operation could complete
    SNS_ERR_NOTFOUND    - No algorithms found
    SNS_ERR_FAILED      - General failure
*/
sns_err_code_e sns_dl_reg_for_each_algorithm(sns_dl_svc_handle_t svc_handle,
                                              pfn_sns_reg_for_each pfn,
                                              sns_reg_dyn_cfg_t *p_cfg,
                                              uint32_t timeout_us);

/**
  @brief  API to search the DL registry for a specific SNS driver UUID and
          retrieve information to dynamically load a SNS driver.

  @param[in]  handle    --  A valid handle
  @param[in]  uuid      --  Function pointer callback to be called for every
                            SNS algorithm
  @param[in]  id_sz     --  Parameter to be passed into the callback function
  @param[in]  p_cfg     --  Parameter to be populated with the DL info.
  @param[in]  timeout_us  --  Time out to block before returning back to the
                              caller. If the timeout is 0, the caller is
                              blocked until the registry search operation
                              completes.

  @return
    SNS_SUCCESS         - Call successful
    SNS_ERR_BAD_PARM    - Bad parameter was passed in
    SNS_ERR_NOMEM       - Out of memory error
    SNS_ERR_WOULDBLOCK  - Time out occurred before the operation could complete
    SNS_ERR_NOTFOUND    - No drivers found
    SNS_ERR_FAILED      - General failure
*/
sns_err_code_e sns_dl_reg_search_uuid
(
  sns_dl_svc_handle_t svc_handle,
  void                *uuid,
  size_t              id_sz,
  sns_reg_dyn_cfg_t   *p_cfg,
  uint32_t            timeout_us
);

/**
  @brief  Print diagnostic information pertaining to the specified handle.

  @param[in]  handle    --  SVC handle
*/
void sns_dl_svc_print_state
(
  sns_dl_svc_handle_t svc_handle
);

#ifdef __cplusplus
}
#endif

#endif  //SNS_DL_H
