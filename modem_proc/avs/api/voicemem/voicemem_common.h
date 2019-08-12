#ifndef __VOICEMEM_COMMON_H__
#define __VOICEMEM_COMMON_H__

/**
  @file voicemem_common.h
  @brief This file contains the VOICEMEM common APIs.
*/

/*===========================================================================
Copyright (C) 2013 QUALCOMM Technologies, Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/voicemem/voicemem_common.h#1 $
  $Author: mplcsds1 $

===========================================================================*/


/****************************************************************************
 * INITIALIZATION/DEINITIALIZATION APIS                                     *
 ****************************************************************************/

/**
  Initializes VOICEMEM.
 
  @par Payload
  None.

  @return
  #VSD_EOK in case of success.

  @dependencies
  None.

  @comments
  None.
*/
#define VOICEMEM_CMD_INIT ( 0x00012A6F )


/**
  Deinitializes VOICEMEM.
 
  @par Payload
  None.

  @return
  #VSD_EOK in case of success.

  @dependencies
  None.

  @comments
  None.
*/
#define VOICEMEM_CMD_DEINIT ( 0x00012A72 )


/****************************************************************************
 * COMMON APIS                                                              *
 ****************************************************************************/

/**
  Allocates a memory region of default type.
 
  @par Payload
  #voicemem_cmd_alloc_default_t

  @return
  #VSD_EOK in case of success.

  @dependencies
  None.

  @comments
  The allocated memory region must be treated by the client as cached.
*/
#define VOICEMEM_CMD_ALLOC_DEFAULT ( 0x00012EE7 )

/** @brief Type definition for #voicemem_cmd_alloc_default_t.
*/
typedef struct voicemem_cmd_alloc_default_t voicemem_cmd_alloc_default_t;

/** @brief Payload structure for the #VOICEMEM_CMD_ALLOC_DEFAULT command.
*/
struct voicemem_cmd_alloc_default_t
{
  uint32_t size;
    /**< Size of the memory to be allocated in bytes. */
  uint32_t* ret_voicemem_handle;
    /**< Returned VOICEMEM handle for the allocated memory region. */
  void** ret_virt_addr;
    /**< Returned virtual start address of the allocated memory region. */
  uint8_t client_name[31];
    /**< Null-terminated client name string. The client must provide 
         a name when allocating memory. This string cannot be zero-length. */
};


/**
  Invalidates cache in an allocted memory region at the specified offset and
  size.
 
  @par Payload
  #voicemem_cmd_cache_invalidate_t.

  @return
  #VSD_EOK in case of success.

  @dependencies
  Memory region must first be allocated via the #VOICEMEM_CMD_ALLOC_DEFAULT
  command.

  @comments
  None.
*/
#define VOICEMEM_CMD_CACHE_INVALIDATE ( 0x00012EE8 )

/** @brief Type definition for #voicemem_cmd_cache_invalidate_t.
*/
typedef struct voicemem_cmd_cache_invalidate_t voicemem_cmd_cache_invalidate_t;

/** @brief Payload structure for the #VOICEMEM_CMD_CACHE_INVALIDATE command.
*/
struct voicemem_cmd_cache_invalidate_t
{
  uint32_t voicemem_handle;
    /**< VOICEMEM memory region handle. */
  void* virt_addr;
    /**< Address at which to invalidate. */
  uint32_t size;
    /**< Size to invalidate. */
};


/**
  Flushes cache in an allocted memory region at the specified offset and
  size.
 
  @par Payload
  #voicemem_cmd_cache_flush_t.

  @return
  #VSD_EOK in case of success.

  @dependencies
  Memory region must first be allocated via the #VOICEMEM_CMD_ALLOC_DEFAULT
  command.

  @comments
  None.
*/
#define VOICEMEM_CMD_CACHE_FLUSH ( 0x00012EE9 )

/** @brief Type definition for #voicemem_cmd_cache_flush_t.
*/
typedef struct voicemem_cmd_cache_flush_t voicemem_cmd_cache_flush_t;

/** @brief Payload structure for the #VOICEMEM_CMD_CACHE_FLUSH command.
*/
struct voicemem_cmd_cache_flush_t
{
  uint32_t voicemem_handle;
    /**< VOICEMEM memory region handle. */
  void* virt_addr;
    /**< Address at which to flush. */
  uint32_t size;
    /**< Size to flush. */
};


/**
  Frees an allocated memory region.
 
  @par Payload
  #voicemem_cmd_free_t.

  @return
  #VSD_EOK in case of success.

  @dependencies
  None.

  @comments
  None.
*/
#define VOICEMEM_CMD_FREE ( 0x00012EEA )

/** @brief Type definition for #voicemem_cmd_free_t.
*/
typedef struct voicemem_cmd_free_t voicemem_cmd_free_t;

/** @brief Payload structure for the #VOICEMEM_CMD_FREE command.
*/
struct voicemem_cmd_free_t
{
  uint32_t voicemem_handle;
    /**< VOICEMEM memory region handle. */
};


/****************************************************************************
 * CALL API                                                                 *
 ****************************************************************************/

/**
  Issues a command to VOICEMEM.
 
  @param[in] cmd_id       Command ID.
  @param[in,out] params   Command payload.
  @param[in] size         Size of the command payload.
   
  @return
  VSD_EOK when successful.
 
  @dependencies
  None.

  @comments
  None.
*/
VSD_PUBLIC vsd_status_t voicemem_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);


#endif /* __VOICEMEM_COMMON_H__ */
