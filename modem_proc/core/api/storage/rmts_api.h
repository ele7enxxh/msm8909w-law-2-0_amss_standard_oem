#ifndef RMTS_API_H
#define RMTS_API_H

/*=======================================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/storage/rmts_api.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

YYYY-MM-DD       who     what, where, why
--------------   ---     ----------------------------------------------------------
2013-01-15       ak      add ssr rmts status, add osparams in get buffer 
2012-05-24       vk      Initial version based on legacy API
=========================================================================================*/
/** \mainpage Remote Storage Client Public API
 *
 *   This header file contains the Remote Storage Client (RSC) API definitions. RSC provides a
 *  handle to its clients to access data on storage devices controlled by a remote processor.
 */

#include "comdef.h"
/**
 * While we've made the Public Interface OS agnostic, this module relies on event signalling
 * for synchronous operations. The following OS-specific include is therefore necessary
 */
#include "rex.h"

/**
 * rmts_status lists the possible return statuses. A non-zero return status indicates an error.
 * Detailed error descriptions follow.
 */
enum rmts_status
{
  RMTS_STATUS_SUCCESS = 0,           /**< No errors */
  RMTS_STATUS_ERROR_PARAM,           /**< Unexpected Input Argument */
  RMTS_STATUS_ERROR_SERVER_DOWN,     /**< Remote Storage Server is not ready to handle the requested service */
  RMTS_STATUS_ERROR_NOT_SUPPORTED,   /**< The service requested cannot be performed by the server */
  RMTS_STATUS_ERROR_WRITE_PROTECTED, /**< The write was requested to a write-protected area of storage */
  RMTS_STATUS_ERROR_TIMEOUT,         /**< The requested operation could not be completed in a timely manner */
  RMTS_STATUS_ERROR_QMI,             /**< A failure happened during QMI message communication*/
  RMTS_STATUS_ERROR_TRANSPORT,       /**< Data transfer failure (e.g. SDIO, USB, etc) */
  RMTS_STATUS_ERROR_NOMEM,           /**< The requested memory could not be allocated */
  RMTS_STATUS_ERROR_SYSTEM,          /**< System failure (OS / timers / memory etc) */
  RMTS_STATUS_ERROR_SERVER_RESTART   /**< Remote Storage Server on APPS restarted */
};

/*************************************************************************//**
 * Definition of a Globally Unique Identifier (GUID)
 ****************************************************************************/
PACKED struct rmts_guid
{
  uint32 data1;
  uint16 data2;
  uint16 data3;
  uint8  data4[8];
} PACKED_POST;

/** \details
 * Clients of Remote Storage call rmts_get_handle to reserve a handle for future transactions. The client
 * identifies itself with input parameter client_id. A client may hold only one open handle at a time. The
 * same handle can be used for multiple read and write operations. A valid handle is merely a local token -
 * it does not indicate the condition of the remote server.
 *
 * @param[in] client_id
 *    A string identifier for the client. e.g "EFS"
 *
 * @param[out] client_handle
 *    If rmts_get_handle() succeeds, a valid handle for future transactions.
 *
 * @return enum rmts_status
 *     Success or error code. Valid error codes for this function are RMTS_STATUS_ERROR_PARAM,
 *     RMTS_STATUS_ERROR_SYSTEM
 */
enum rmts_status rmts_get_handle (char *client_id, void **client_handle);

/**
 * iovec structure type to be used in conjunction with iovec read and write calls to
 * remote storage. Relative sector addressing is used - 0 being the start of the given
 * partition.
 */
typedef struct
{
   uint32 sector_addr;    /**< Relative sector address on the storage device*/
   uint32 data_phy_addr;  /**< Memory address */
   uint32 num_sector;     /**< Number of sectors to read or write*/
} rmts_iovec_t;

/**
 * Type to encapsulate OS specific signaling / event synchronization parameters.
*/
typedef struct
{
   rex_sigs_type sig;    /**< Signal to use for QMI */
   uint32 timeout_in_ms; /**< Timeout for the operation */
} rmts_os_params_t;

/** \details
 * rmts_get_buffer is used by the client to determine the shared RAM address where data to be synced is held.
 * Depending on the target architecture, this memory may be allocated either by the remote processor or by
 * the local processor. If the memory is assigned locally, the return is immediate. If the memory is assigned
 * remotely, the function call will block until the remote system has finished handling the request.
 *
 * @param[in] client_handle
 *    client_handle returned by rmts_get_handle
 *
 * @param[in] size
 *    size of the shared memory - the client specifies this.
 *
 * @param[out] buff
 *    Pointer to the shared memory location if allocation was succesful.
 * @param[in] os_params
 *   OS parameter required by the QMI infrastructure to preserve context
 *
 * @return enum rmts_status
 *     Success or error code. Valid error codes for this function are RMTS_STATUS_ERROR_PARAM,
 *     RMTS_STATUS_ERROR_NOT_SUPPORTED, RMTS_STATUS_ERROR_TIMEOUT, RMTS_STATUS_ERROR_QMI,
 *     RMTS_STATUS_ERROR_NOMEM and RMTS_STATUS_ERROR_SYSTEM. In all error situations buff will be
 *     explicitly set to NULL.
 *     If ERROR_NOT_SUPPORTED is returned, the client may use other information (static or build time
 *     data) to determine the memory to use for remote storage.
 */
enum rmts_status rmts_get_buffer (void *client_handle, uint8 **buff, uint32 size, rmts_os_params_t *os_params);

/** \details
 * rmts_read_iovec synchronously reads data from eMMC sectors specified in the iovec into the
 * memory region specified by the iovec.
 *
 * @param[in] client_handle
 *    client_handle returned by rmts_get_handle
 *
 * @param[in] partition_id
 *    Partition identifier (16 byte GUID). If GUIDs are not used, the partititon name.
 *
 * @param[in] iovec
 *    An iovec array describing source and destination locations.
 *
 * @param[in] size
 *    Size of the iovec array
 *
 * @param[in] os_params
 *   OS parameter required by the QMI infrastructure to preserve context
 *
 * @return enum rmts_status
 *     Success or error code as described above. Valid errors for this operation are
 * RMTS_STATUS_ERROR_PARAM, RMTS_STATUS_ERROR_SERVER_DOWN, RMTS_STATUS_ERROR_NOT_SUPPORTED,
 * RMTS_STATUS_ERROR_TIMEOUT, RMTS_STATUS_ERROR_QMI, RMTS_STATUS_ERROR_TRANSPORT and
 * RMTS_STATUS_ERROR_SYSTEM
 */
enum rmts_status rmts_read_iovec (void *client_handle,
                                  struct rmts_guid *guid,
                                  rmts_iovec_t *iovec,
                                  uint32 size,
                                  rmts_os_params_t *os_params);

/** \details
 * rmts_write_iovec synchronously writes data into the eMMC sectors specified in the iovec
 * from the memory specified by the iovec.
 *
 * @param[in] client_handle
 *    client_handle returned by rmts_get_handle
 *
 * @param[in] partition_id
 *    Partition identifier (16 byte GUID). If GUIDs are not used, the partititon name.
 *
 * @param[in] iovec
 *    An iovec array describing source and destination locations.
 *
 * @param[in] size
 *    Size of the iovec array
 *
 * @param[in] os_params
 *   OS parameter required by the QMI infrastructure to preserve context
 *
 * @return enum rmts_status
 *     Success or error code as described above. Valid errors for this operation are
 * RMTS_STATUS_ERROR_PARAM, RMTS_STATUS_ERROR_SERVER_DOWN, RMTS_STATUS_ERROR_WRITE_PROTECTED,
 * RMTS_STATUS_ERROR_TIMEOUT, RMTS_STATUS_ERROR_QMI, RMTS_STATUS_ERROR_TRANSPORT and
 * RMTS_STATUS_ERROR_SYSTEM
 */
enum rmts_status rmts_write_iovec (void *client_handle,
                                   struct rmts_guid *guid,
                                   rmts_iovec_t *iovec,
                                   uint32 size,
                                   rmts_os_params_t *os_params);

/** \details
 * rmts_close deregisters the handle assigned in rmts_get_handle. This is used to close the
 * remote storage session opened by the client
 *
 * @param[in] client_handle
 *    client_handle returned by rmts_get_handle
 *
 * @return enum rmts_status
 *     Status - RMTS_STATUS_SUCCESS or RMTS_STATUS_ERROR_PARAM
 */
enum rmts_status rmts_close (uint32 *client_handle);

#endif /* RMTS_API_H */
