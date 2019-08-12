/*==============================================================================

                  Remote Storage Client Interface

DESCRIPTION
  This file contains implementation of the QMI based Remote Storage Client.

  Copyright (c) 2012-14 by Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/
/*==============================================================================
               EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/remotefs/src/rmts_api.c#1 $  $DateTime: 2016/12/13 07:59:23 $  $Author: mplcsds1 $

YYYY-MM-DD      who    what, where, why
--------------   ---     -----------------------------------------------------
2014-09-10      dks    Fix client handle validation for external clients.
2014-07-22      dks    Cleanup messaging and configurations.
2014-07-16      dks    Dont mem map shared RAM to virtual address for MDMs.
2014-06-19      dks    Support reconnection with server if it restarted.
2014-06-04      dks    Store external client buffer info for validation.
2014-05-27      dks    Featurize compilation for old DAL and RFSA XPU.
2014-05-23      dks    Do not RFSA xpu-lock for MDM.
2014-04-04      dks    Dynamically allocate EFS shared RAM address.
2013-10-03      dks    Fix assignment type to remove hexagone warning.
2013-10-03      ak     Merge support for SFP2 part of multiclient
2013-09-10      bb     Klocwork fixes
2013-09-05      bb     Added XPU protection for EFS buffer in Fusion
                       configuration
2013-02-21      bb     Klocwork Fixes
2013-01-04      ak     update read, get buffer, write for fusion cases
                       (because of efs & hsic deadlock issue)
2013-01-10      ak     Add multiclient/ SFP support
2012-11-07      ak     Initial version based on rmts_api.h for standalone
==============================================================================*/

/** \mainpage Remote Storage Client
 *
 *   This header file contains the Remote Storage Client (RSC) API
 *   implementation. RSC provides a handle to its clients to access data on
 *   storage devices controlled by a remote processor.
 */
#include <string.h>
#include <stdlib.h>
#include "qmi_cci_target_ext.h"
#include "qmi_client.h"
#include "mba_ac.h"
#include "remote_storage_v01.h"
#include "remote_filesystem_access_v01.h"
#include "rmts_api.h"
#include "rmts_msg.h"
#include "rmts_config_i.h"
#include "err.h"
#include "DALStdDef.h"
#include "DALSysTypes.h"
#include "DALSys.h"
#include "assert.h"
#include "stringl/stringl.h"

// add critical sections for SFP
#define RMTS_MAX_INTERNAL_CLIENTS 1
#define RMTS_MAX_CLIENTS (RMTS_MAX_INTERNAL_CLIENTS + RMTS_MAX_EXTERNAL_CLIENTS)
#define NUM_RMTS_SERVICES 1
#define RMTS_OP_COMPLETE_SIG 0x80000000
#define RMTS_OP_TIMEOUT_SIG  0x20000000
#define MAX_PARTITIONS 4
/* SMEM Kernel driver requires a unique Client ID to be passed. RMTS is ID 1 */
#define RMTS_CLIENT_ID_FOR_SMEM_DRIVER 1
/* SMEM Driver has two instances. Instance 0 is User space and instance 1
   is kernel driver */
#define RMTS_SECTOR_SIZE 512
#define RMTS_SMEM_SERVER_INSTANCE 1
#define RMTS_INVALID_HANDLE 0xFFFFFFFF

static const char *efs_file_names[MAX_PARTITIONS] = {
#ifdef SCL_GOLDEN_EFS_RAM_BASE
  "m9kefs1",
  "m9kefs2",
  "m9kefs3",
  "m9kefsc"
#else
  "/boot/modem_fs1",
  "/boot/modem_fs2",
  "/boot/modem_fsg",
  "/boot/modem_fsc"
#endif
};

// Global data for rmts client module
struct rmts_info_t
{
  // Client Init done flag
  boolean               client_init_done;

  // Object of the server to connect
  qmi_idl_service_object_type service_object;

  // Object of the kernel driver to connect
  qmi_idl_service_object_type smem_kernel_service_object;

  int smem_kernel_service_found;

  // Signal on which to wait on
  qmi_cci_os_signal_type os_params;

  // Client notifier
  qmi_client_type       notifier;

  // Number of clients that have called in to get a handle
  uint32                num_clients_registered;

  // Number of Services
  uint32                num_services;

  // Number of entries in service info
  uint32                num_entries;

  // Service info
  qmi_service_info      service_info[NUM_RMTS_SERVICES];

  // Client object used for talking to SMEM Kernel Driver
  qmi_client_type       smem_client;

  // Critical section to protect the data structures
  rex_crit_sect_type    lock_globals;

  boolean               is_fusion;

  /* These variables store details about the memory to be parceled out to
     non EFS clients. Currently these will point to a single memory block
     If needed in the future to point to multiple memory blocks, one could
     make an array of these */
  uint64                ext_client_mem_alloc_base;
  uint32                ext_client_mem_alloc_size;
  uint32                ext_client_mem_alloc_used;

  uint64                efs_client_shared_ram_base;
  uint32                efs_client_shared_ram_size;
};


// Client specific data
struct client_info
{
  // Client ID - used as an index into the client table
  uint32            id;

  // Stores handle(s) to open file(s)
  uint32            handles[MAX_PARTITIONS];

  // Stores the client name
  char              client_name[RMTS_MAX_CLIENT_NAME_LENGTH];

  // Flag to indicate that file handle has been opened
  boolean           client_handles_opened;

  // Since EFS is treated different/special
  boolean           isEFSClient;

  // if guids are for fusion, so we fail the reads
  boolean           is_fusion_guid;

  // Physical Shared RAM address
  uint64            shared_phy_ram_addr;

  // Virtual mapping of physical shared RAM address
  uint64            shared_ram_addr;

  // Shared RAM size
  uint32            shared_ram_size;

  // Flag indicates client has been given a buffer assigned from the server
  boolean           alloc_done;

  // Signal on which to wait on
  qmi_cci_os_signal_type  os_params;

  // client object returned by server
  qmi_client_type   client;

  // Signal on which to wait on
  qmi_cci_os_signal_type  write_os_params;

  // client object returned by server
  qmi_client_type   write_client;

  // Signal on which to wait on
  qmi_cci_os_signal_type  read_os_params;

  // client object returned by server
  qmi_client_type       read_client;

};

static struct rmts_guid partition_guids[MAX_PARTITIONS] =
{
  {0xEBBEADAF, 0x22C9, 0xE33B,
    {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}},
  {0x0A288B1F, 0x22C9, 0xE33B,
    {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}},
  {0x638FF8E2, 0x22C9, 0xE33B,
    {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}},
  {0x57B90A16, 0x22C9, 0xE33B,
    {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}}
};

static struct rmts_guid fusion_partition_guids[MAX_PARTITIONS] =
{
  {0x2290BE64, 0x22C9, 0xE33B,
    {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}},
  {0x346C26D1, 0x22C9, 0xE33B,
    {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}},
  {0xBF64FB9C, 0x22C9, 0xE33B,
    {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}},
  {0x5CB43A64, 0x22C9, 0xE33B,
    {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}}
};

static struct client_info client_data[RMTS_MAX_CLIENTS];
static struct rmts_info_t rmts_info = {0};

static enum rmts_status
rmts_map_qmi_to_rmts_error (int qmi_error)
{
  switch (qmi_error)
  {
    case QMI_NO_ERR:
      return RMTS_STATUS_SUCCESS;

    case QMI_SERVICE_ERR:
      return RMTS_STATUS_ERROR_SERVER_DOWN;

    case QMI_TIMEOUT_ERR:
      return RMTS_STATUS_ERROR_TIMEOUT;

    case QMI_INTERNAL_ERR:
    default:
      return RMTS_STATUS_ERROR_QMI;
  }
}

static enum rmts_status
rmts_map_service_to_rmts_error (qmi_error_type_v01 qmi_err)
{
  switch (qmi_err)
  {
  case QMI_ERR_NONE_V01:
    return RMTS_STATUS_SUCCESS;

  case QMI_ERR_INVALID_CLIENT_ID_V01:
    return RMTS_STATUS_ERROR_SERVER_DOWN;

  default:
    return RMTS_STATUS_ERROR_QMI;
  }
}

void rmts_rc_init(void)
{
  memset(&rmts_info, 0, sizeof(struct rmts_info_t));
  rex_init_crit_sect(&(rmts_info.lock_globals));

#ifdef SCL_GOLDEN_EFS_RAM_BASE
  #ifdef SCL_SHARED_EFS_RAM_BASE
      rmts_info.efs_client_shared_ram_base = SCL_SHARED_EFS_RAM_BASE;
      rmts_info.efs_client_shared_ram_size = SCL_SHARED_EFS_RAM_SIZE;
  #else
   #error "Config error. SCL_GOLDEN_EFS_RAM defined but not SCL_SHARED_EFS_RAM"
  #endif

  rmts_info.is_fusion = TRUE;
  rmts_info.ext_client_mem_alloc_base = SCL_GOLDEN_EFS_RAM_BASE;
  rmts_info.ext_client_mem_alloc_size = SCL_GOLDEN_EFS_RAM_SIZE;
  rmts_info.ext_client_mem_alloc_used = 0;
#endif
}

enum rmts_status rmts_init
(
  struct rmts_info_t *rmts_init_data,
  rex_tcb_type *tcb,
  rex_sigs_type sigs,
  rex_sigs_type timer_sig
)
{
  qmi_client_error_type rc;
  enum rmts_status status;

  rex_enter_crit_sect(&rmts_init_data->lock_globals);

  if (rmts_init_data->client_init_done)
  {
    status = RMTS_STATUS_SUCCESS;
    goto end;
  }

  /* Verify that service_object did not return NULL */
  rmts_init_data->service_object = rmtfs_get_service_object_v01();
  if (!rmts_init_data->service_object)
  {
    status = RMTS_STATUS_ERROR_QMI;
    goto end;
  }

  rmts_init_data->os_params.sig = sigs;
  rmts_init_data->os_params.tcb = tcb;
  rmts_init_data->os_params.timed_out = 0;
  rmts_init_data->os_params.timer_sig = timer_sig;

  rc = qmi_client_notifier_init(rmts_init_data->service_object,
                                &rmts_init_data->os_params,
                                &rmts_init_data->notifier);
  status = rmts_map_qmi_to_rmts_error (rc);
  if(rc != QMI_NO_ERR)
  {
    goto end;
  }

  /* Check if the service is up, if not wait on a signal */
  while(1)
  {
    rc = qmi_client_get_service_list(rmts_init_data->service_object,
                          NULL, NULL,
                          (unsigned int *)&rmts_init_data->num_services);

    if(rc == QMI_NO_ERR)
    {
      break;
    }
    // wait for server to come up
    //these macros give warning inside them, qmi teams
    QMI_CCI_OS_SIGNAL_WAIT(&rmts_init_data->os_params, 0);
  };

  QMI_CCI_OS_SIGNAL_CLEAR(&rmts_init_data->os_params);

  rmts_init_data->num_entries = NUM_RMTS_SERVICES;
  /* The server has come up, store the information in info variable */
  rc = qmi_client_get_service_list(rmts_init_data->service_object,
                               rmts_init_data->service_info,
                              (unsigned int *)&rmts_init_data->num_entries,
                              (unsigned int *)&rmts_init_data->num_services);

  status = rmts_map_qmi_to_rmts_error (rc);

  if(rc != QMI_NO_ERR)
  {
    goto end;
  }

  ASSERT (status == RMTS_STATUS_SUCCESS);
  ASSERT (rmts_init_data->num_services >= 1);
  rmts_init_data->client_init_done = TRUE;

end:
  rex_leave_crit_sect(&rmts_init_data->lock_globals);
  RMTS_MSG_HIGH1 ("RMTS INIT : status = %d", status);
  return status;
}


/** \details
 * Clients of Remote Storage call rmts_get_handle to reserve a handle for
 * future transactions. The client identifies itself with input parameter
 * client_id. A client may hold only one open handle at a time. The same handle
 * can be used for multiple read and write operations. A valid handle is merely
 * a local token - it does not indicate the condition of the remote server.
 *
 * @param[in] client_id
 *    A string identifier for the client. e.g "EFS"
 *
 * @param[out] client_handle
 *    If rmts_get_handle() succeeds, a valid handle for future transactions.
 *
 * @return enum rmts_status
 *     Success or error code. Valid error codes for this function are
 *     RMTS_STATUS_ERROR_PARAM,
 *     RMTS_STATUS_ERROR_SYSTEM
 */
enum rmts_status rmts_get_handle (char *client_id, void **client_handle)
{
  enum rmts_status status;
  uint32 client_index;
  struct rmts_info_t *prmts_info = &rmts_info;

  if (NULL == client_id || NULL == client_handle || '\0' == client_id[0])
  {
    RMTS_MSG_ERR2 ("rmts_get_handle: Invalid prams %d, %d", (uint32) client_id,
                  (uint32) client_handle);
    return RMTS_STATUS_ERROR_PARAM;
  }

  if (strlen (client_id) >= sizeof (client_data[client_index].client_name))
  {
    RMTS_MSG_ERR2 ("rmts_get_handle: Path too long %d, %d", strlen (client_id),
                   sizeof (client_data[client_index].client_name));
    return RMTS_STATUS_ERROR_PARAM;
  }

  rex_enter_crit_sect(&(prmts_info->lock_globals));

  if (prmts_info->num_clients_registered >= RMTS_MAX_CLIENTS)
  {
    RMTS_MSG_ERR2 ("rmts_get_handle: Max clients registered %d , %d",
                   rmts_info.num_clients_registered, RMTS_MAX_CLIENTS);
    *client_handle = NULL;
    status = RMTS_STATUS_ERROR_NOMEM;
    goto end;
  }

  client_index = prmts_info->num_clients_registered++;
  client_data[client_index].id = client_index + 1;

  // Since client handle is defined as a pointer, and should ideally store
  // an address, let it store the address of the id field, so that we can
  // later dereference this and get the index
  // example: client_index = (*((uint32 *) client_handle)) - 1
  *client_handle = (void *) (&(client_data[client_index]));

  // Store the client name because for non-EFS clients this name will be
  // used as the filename
  (void)strlcpy (client_data[client_index].client_name,
                 client_id,
                 RMTS_MAX_CLIENT_NAME_LENGTH);

  if ( 0 == strcmp("EFS" , client_data[client_index].client_name))
  {
    RMTS_MSG_MED1 ("rmts_get_handle: EFS Client assigned handle %d",
                  client_index);
    client_data[client_index].isEFSClient = TRUE;
  }
  else
  {
    RMTS_MSG_MED1 ("rmts_get_handle: Non-EFS Client assigned handle %d",
                   client_index);
  }

  status = RMTS_STATUS_SUCCESS;

end:
  rex_leave_crit_sect(&(prmts_info->lock_globals));
  return status;
}

static enum rmts_status
open_partitions_on_server (struct rmts_info_t *prmts_info,
                           struct client_info *pclient_data, const char *path,
                           uint32 *ret_fs_handle, rmts_os_params_t *os_params)
{
  rmtfs_open_req_msg_v01  rmtfs_req;
  rmtfs_open_resp_msg_v01 rmtfs_resp;
  qmi_client_error_type rc;
  enum rmts_status status;

  *ret_fs_handle = -1;

  pclient_data->os_params.sig = os_params->sig;
  pclient_data->os_params.tcb = rex_self ();
  pclient_data->os_params.timer_sig = RMTS_OP_TIMEOUT_SIG;
  pclient_data->os_params.timed_out = 0;

  rc = qmi_client_init(&prmts_info->service_info[0],
                        prmts_info->service_object,
                        NULL,
                        NULL,
                        &pclient_data->os_params,
                        &pclient_data->client);
  status = rmts_map_qmi_to_rmts_error (rc);

  RMTS_MSG_MED1 ("Open parti qmi_client_init : client = %d",
                  pclient_data->client);
  RMTS_MSG_HIGH2 ("Open parti qmi_client_init : rc = %d, staus = %d",
                   rc, status);

  if(rc != QMI_NO_ERR)
  {
    return status;
  }

  memset(&rmtfs_req, 0, sizeof(rmtfs_open_req_msg_v01));
  memset(&rmtfs_resp, 0, sizeof(rmtfs_open_resp_msg_v01));

  if (strlcpy(rmtfs_req.path, path, RMTFS_MAX_FILE_PATH_V01)
      >= RMTFS_MAX_FILE_PATH_V01)
  {
    RMTS_MSG_ERR2 ("rmts: Path too long %d  > %d", strlen (rmtfs_req.path),
                  strlen(path));
    status = RMTS_STATUS_ERROR_PARAM;
    goto end;
  }

  rc = qmi_client_send_msg_sync(pclient_data->client,
                                QMI_RMTFS_OPEN_REQ_V01,
                                (void *)&rmtfs_req,
                                sizeof(rmtfs_open_req_msg_v01),
                                (void *)&rmtfs_resp,
                                sizeof(rmtfs_open_resp_msg_v01),
                                0);
  status = rmts_map_qmi_to_rmts_error (rc);

  RMTS_MSG_MED2 ("Open parti send_msg_sync : rc = %d, staus = %d",
                  rc, status);

  if (rc != QMI_NO_ERR)
  {
    goto end;
  }

  status = rmts_map_service_to_rmts_error (rmtfs_resp.resp.error);
  if (rmtfs_resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    goto end;
  }

  ASSERT (status == RMTS_STATUS_SUCCESS);

  if (rmtfs_resp.caller_id_valid)
  {
    *ret_fs_handle = rmtfs_resp.caller_id;
  }
  else
  {
    ERR_FATAL ("RMTS Open status success but caller id invalid", 0, 0, 0);
  }

end:
  (void) qmi_client_release(pclient_data->client);
  return status;
}

static enum rmts_status
rmts_get_buffer_from_smem_server (struct client_info *pclient_data,
                                  uint32 size, rmts_os_params_t *os_params)
{
  rfsa_get_buff_addr_req_msg_v01 smem_req;
  rfsa_get_buff_addr_resp_msg_v01 smem_resp;
  enum rmts_status status;
  qmi_client_error_type rc;
  struct rmts_info_t *prmts_info = &rmts_info;
  uint32 tout;

  /* If SMEM Kernel Service is present, it should have come up by now. So need
     wait for the shortest duration possible */
  tout = 1;

  pclient_data->os_params.sig = os_params->sig;
  pclient_data->os_params.tcb = rex_self ();
  pclient_data->os_params.timer_sig = RMTS_OP_TIMEOUT_SIG;
  pclient_data->os_params.timed_out = 0;

  prmts_info->smem_kernel_service_object = rfsa_get_service_object_v01 ();
  rc = qmi_client_init_instance (prmts_info->smem_kernel_service_object,
                                 RMTS_SMEM_SERVER_INSTANCE, NULL, NULL,
                                 &pclient_data->os_params, tout,
                                 &rmts_info.smem_client);
  status = rmts_map_qmi_to_rmts_error (rc);

  RMTS_MSG_MED2 ("Smem_get_buffer init client : rc = %d, staus = %d",
                  rc, status);

  if (rc != QMI_NO_ERR)
  {
    RMTS_MSG_HIGH2 ("SMEM Kernel Service not found %d %d", rc, status);
    return status;
  }

  RMTS_MSG_HIGH ("SMEM Kernel Service found");
  prmts_info->smem_kernel_service_found = 1;

  memset(&smem_req, 0, sizeof (rfsa_get_buff_addr_req_msg_v01));
  memset(&smem_resp, 0, sizeof (rfsa_get_buff_addr_resp_msg_v01));
  smem_req.client_id = RMTS_CLIENT_ID_FOR_SMEM_DRIVER;
  smem_req.size = size;

  rc = qmi_client_send_msg_sync (rmts_info.smem_client,
                                 QMI_RFSA_GET_BUFF_ADDR_REQ_MSG_V01,
                                 (void *)&smem_req, sizeof(smem_req),
                                 (void *)&smem_resp, sizeof(smem_resp), 0);
  status = rmts_map_qmi_to_rmts_error (rc);

  RMTS_MSG_MED2 ("Smem_get_buffer send_msg : rc = %d, staus = %d",
                 rc, status);

  if (rc != QMI_NO_ERR)
  {
    ERR_FATAL ("qmi_send_msg failed on Kernel Service. %d %d", rc, status, 0);
  }

  status = rmts_map_service_to_rmts_error (smem_resp.resp.error);
  if (smem_resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    ERR_FATAL ("qmi_send_msg on Kernel Service returned failure %d %d", rc,
                status, 0);
  }

  if (!smem_resp.address_valid)
  {
    ERR_FATAL ("qmi_send_msg on Kernel Service success but addr invalid", 0,
                0, 0);
  }

  RMTS_MSG_HIGH1 ("SMEM Kernel Service Get_buffer succeeded %u",
                   smem_resp.address);

  ASSERT (status == RMTS_STATUS_SUCCESS);
  pclient_data->shared_ram_size = size;
  pclient_data->shared_phy_ram_addr = smem_resp.address;

  (void) qmi_client_release (rmts_info.smem_client);

  return status;
}


static enum rmts_status
rmts_get_buffer_from_rmts_server(struct client_info *pclient_data, uint32 size,
                                 rmts_os_params_t *os_params,
                                 int do_dummy_alloc)
{
  qmi_client_error_type rc;
  enum rmts_status status;
  rmtfs_alloc_buff_req_msg_v01  rmtfs_req;
  rmtfs_alloc_buff_resp_msg_v01 rmtfs_resp;

  pclient_data->os_params.sig = os_params->sig;
  pclient_data->os_params.tcb = rex_self ();
  pclient_data->os_params.timer_sig = RMTS_OP_TIMEOUT_SIG;
  pclient_data->os_params.timed_out = 0;

  rc = qmi_client_init (&rmts_info.service_info[0],
                        rmts_info.service_object,
                        NULL,
                        NULL,
                        &pclient_data->os_params,
                        &pclient_data->client);
  status = rmts_map_qmi_to_rmts_error (rc);

  RMTS_MSG_MED1 ("Get_buffer : qmi_client_init client = %d",
                 pclient_data->client);
  RMTS_MSG_HIGH2 ("Get_buffer : qmi_client_init rc = %d status = %d", rc,
                  status);

  if(rc != QMI_NO_ERR)
  {
    return status;
  }

  memset(&rmtfs_req, 0, sizeof(rmtfs_alloc_buff_req_msg_v01));
  memset(&rmtfs_resp, 0, sizeof(rmtfs_alloc_buff_resp_msg_v01));

  rmtfs_req.caller_id = pclient_data->handles[0];
  rmtfs_req.buff_size = size;

  RMTS_MSG_MED1 ("Requesting Shared buffer of size %d", size);

  rc = qmi_client_send_msg_sync(pclient_data->client,
                                QMI_RMTFS_ALLOC_BUFF_REQ_V01,
                                (void *)&rmtfs_req,
                                sizeof(rmtfs_alloc_buff_req_msg_v01),
                                (void *)&rmtfs_resp,
                                sizeof(rmtfs_alloc_buff_resp_msg_v01),
                                0);
  status = rmts_map_qmi_to_rmts_error (rc);
  RMTS_MSG_MED2 ("Get_buffer : send_msg_sync rc = %d status = %d", rc,
                 status);

  if (rc != QMI_NO_ERR)
  {
    goto end;
  }

  status = rmts_map_service_to_rmts_error (rmtfs_resp.resp.error);
  if (rmtfs_resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    ERR_FATAL ("get_buffer on RMTS returned failure %d %d",
                rmtfs_resp.resp.result, status, 0);
  }

  ASSERT (status == RMTS_STATUS_SUCCESS);

  if (!rmtfs_resp.buff_address_valid)
  {
    ERR_FATAL ("get_buffer on RMTS succeeded but addr invalid", 0, 0, 0);
  }

  if (!do_dummy_alloc)
  {
    pclient_data->shared_ram_size = size;
    pclient_data->shared_phy_ram_addr = rmtfs_resp.buff_address;
  }

end:
  (void) qmi_client_release (pclient_data->client);
  return status;
}


static void
rmts_xpu_lock (uint64 start_addr, uint64 end_addr)
{
  if (E_SUCCESS != mba_xpu_lock_region(start_addr,
                                       end_addr,
                                       MBA_XPU_PARTITION_MODEM_EFS))
  {
    ERR_FATAL ("xpu lock failed %d %d",
               (uint32) start_addr, (uint32) end_addr, 0);
  }

  RMTS_MSG_HIGH2 ("xpu-lock success %d %d", (uint32)start_addr,
                 (uint32) end_addr);
}

#ifdef  FEATURE_RMTS_COMPILE_WITH_OLD_DAL

static void
rmts_mmap_shared_ram (uint64 physical_addr, uint32 size,
                      uint64* virtual_addr)
{
  DALSYSMemHandle handle;
  DALSYSMemInfoEx mem_info;
  DALResult result;

  mem_info.PhysicalAddr = (DALSYSMemAddr64) physical_addr;
  mem_info.VirtualAddr = DALSYS_MEM_ADDR_NOT_SPECIFIED;
  mem_info.dwLen = size;
  mem_info.dwProps = (DALSYS_MEM_PROPS_UNCACHED | DALSYS_MEM_PROPS_PHYS_CONT);
  result = DALSYS_MemRegionAllocEx (&mem_info, &handle, NULL);
  if (result != DAL_SUCCESS || ((void *) mem_info.VirtualAddr) == NULL)
  {
    ERR_FATAL ("Failed to get virtual address for physical address %d %d %d",
               (uint32) physical_addr, mem_info.VirtualAddr, result);
  }

  RMTS_MSG_HIGH2 ("SharedMem Physical to virtual mapping successful %d %d",
                 (uint32) physical_addr, (uint32) mem_info.VirtualAddr);

  *virtual_addr = mem_info.VirtualAddr;
}

#else /* FEATURE_RMTS_COMPILE_WITH_OLD_DAL */

static void
rmts_mmap_shared_ram (uint64 physical_addr, uint32 size,
                      uint64* virtual_addr)
{
  DALResult result;
  DALSYSMemHandle mem_handle;
  DALSYSMemInfoEx mem_info;
  DALSYS_MEM_REQ_OBJECT(mem_req);

  mem_req.memInfo.physicalAddr = (DALSYSPhyAddr) physical_addr;
  mem_req.memInfo.size = size;

  result = DALSYS_MemRegionAllocEx (&mem_handle, &mem_req, &mem_info);
  if (result != DAL_SUCCESS || ((void *) mem_info.virtualAddr) == NULL)
  {
    ERR_FATAL ("Failed to get virtual address for physical address %d %d",
               result, (uint32) physical_addr, 0);
  }

  RMTS_MSG_HIGH2 ("Shared Memory Physical to virtual mapping successful %d %d",
                 (uint32) physical_addr,
                 (uint32) mem_info.virtualAddr);

  *virtual_addr = (uint64) mem_info.virtualAddr;
}

#endif /* FEATURE_RMTS_COMPILE_WITH_OLD_DAL */

static enum rmts_status
rmts_get_buffer_core (struct client_info *pclient_data, uint32 size,
                      rmts_os_params_t *os_params)
{
  enum rmts_status status;

  status = rmts_get_buffer_from_smem_server (pclient_data, size, os_params);
  if (status == RMTS_STATUS_SUCCESS)
  {
    /* Special request RMTS Server to indicate buffer has been
       allocated from Kernel driver*/
    (void) rmts_get_buffer_from_rmts_server (pclient_data, 0, os_params, 1);
  }
  else
  {
    status = rmts_get_buffer_from_rmts_server (pclient_data, size, os_params,
                                               0);
  }

  return status;
}

static enum rmts_status
do_rmts_init_sequence(struct client_info *pclient_data,
                      rmts_os_params_t *os_params)
{

  uint32 server_fs_handle = -1;
  struct rmts_info_t *prmts_info = &rmts_info;
  enum rmts_status status;
  uint32 i;

  status = rmts_init (prmts_info,
                      rex_self(),
                      RMTS_OP_COMPLETE_SIG,
                      RMTS_OP_TIMEOUT_SIG);
  if (RMTS_STATUS_SUCCESS != status)
  {
    RMTS_MSG_ERR1 ("rmts_init error %d", status);
    return status;
  }

  if (TRUE == pclient_data->client_handles_opened)
  {
    return RMTS_STATUS_SUCCESS;
  }

  if (TRUE == pclient_data->isEFSClient)
  {
      // If it's the EFS client, open up all the possible partitions
      for (i = 0; i < MAX_PARTITIONS; ++i)
      {
        server_fs_handle = -1;
        status = open_partitions_on_server(prmts_info, pclient_data,
                                           efs_file_names[i],
                                           &server_fs_handle, os_params);
        if (RMTS_STATUS_SUCCESS == status)
        {
           pclient_data->handles[i] = server_fs_handle;
           RMTS_MSG_MED1 ("EFS file %d opened", i);
        }
        else
        {
           RMTS_MSG_ERR1 ("ERROR opening EFS file %d", i);
           return status;
        }
      }
  }
  else
  {
      // otherwise open up the partitions based on the client names
      server_fs_handle = -1;
      status = open_partitions_on_server(prmts_info,
                                         pclient_data,
                                         pclient_data->client_name,
                                         &server_fs_handle, os_params);
      if (RMTS_STATUS_SUCCESS == status)
      {
        // Since non-EFS clients only use the one file, we're putting the
        // handle into the first index
        pclient_data->handles[0] = server_fs_handle;
        RMTS_MSG_MED ("File opened for non-EFS client");
      }
      else
      {
        RMTS_MSG_ERR ("ERROR opening file for non-EFS client");
        return status;
      }
  }

  pclient_data->client_handles_opened = TRUE;
  return RMTS_STATUS_SUCCESS;
}

/* Clear out intialization data to reattempt connect with restarted server */
static void
rmts_reset_init_data (void)
{
  struct client_info *cinfo;
  uint32 i;

  RMTS_MSG_HIGH ("Resetting init data");

  rmts_info.client_init_done = FALSE;

  /* Clear out all client init data */
  for (i = 0; i < rmts_info.num_clients_registered; i++)
  {
    cinfo = &client_data[i];
    if (cinfo->isEFSClient == TRUE)
    {
      cinfo->client_handles_opened = FALSE;
    }
  }
}

static enum rmts_status
rmts_get_buffer_locally (struct client_info *pclient_data, uint32 size)
{
  enum rmts_status status = RMTS_STATUS_ERROR_NOMEM;

  rex_enter_crit_sect(&rmts_info.lock_globals);

  ASSERT (rmts_info.is_fusion == TRUE);

  if (pclient_data->isEFSClient == TRUE)
  {
    pclient_data->shared_phy_ram_addr = rmts_info.efs_client_shared_ram_base;
    pclient_data->shared_ram_size = rmts_info.efs_client_shared_ram_base;
    status = RMTS_STATUS_SUCCESS;
  }
  else
  {
    if (size < (rmts_info.ext_client_mem_alloc_size -
                rmts_info.ext_client_mem_alloc_used))
    {
      pclient_data->shared_phy_ram_addr = rmts_info.ext_client_mem_alloc_base
                                       + rmts_info.ext_client_mem_alloc_used;
      pclient_data->shared_ram_size = size;
      rmts_info.ext_client_mem_alloc_used += size;

      status = RMTS_STATUS_SUCCESS;
    }
    else
    {
      RMTS_MSG_ERR3 ("rmts: Run out of buffer space to allocate [%u > %u-%u]",
                     size, rmts_info.ext_client_mem_alloc_size,
                     rmts_info.ext_client_mem_alloc_used);
    }
  }

  rex_leave_crit_sect(&rmts_info.lock_globals);

  return status;
}


/** \details
 * rmts_get_buffer is used by the client to determine the shared RAM address
 * where data to be synced is held.
 * Depending on the target architecture, this memory may be allocated either by
 * the remote processor or by the local processor. If the memory is assigned
 * locally, the return is immediate. If the memory is assigned remotely, the
 * function call will block until the remote system has finished handling the
 * request.
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
 *     Success or error code. Valid error codes for this function are
 *       RMTS_STATUS_ERROR_PARAM,
 *       RMTS_STATUS_ERROR_NOT_SUPPORTED,
 *       RMTS_STATUS_ERROR_TIMEOUT,
 *       RMTS_STATUS_ERROR_QMI,
 *       RMTS_STATUS_ERROR_NOMEM, and
 *       RMTS_STATUS_ERROR_SYSTEM.
 *     In all error situations buff will be explicitly set to NULL.
 *     If ERROR_NOT_SUPPORTED is returned, the client may use other information
 *     (static or build time data) to determine the memory to use for remote
 *     storage.
 */
enum rmts_status rmts_get_buffer (void *client_handle, uint8 **buff,
                                  uint32 size, rmts_os_params_t *os_params)
{
  enum rmts_status status;
  uint64 mpss_remote_address_start, mpss_remote_address_end;
  int i;
  struct client_info *pclient_data = (struct client_info *) client_handle;

  if (NULL == client_handle || NULL == buff || 0 == size)
  {
    RMTS_MSG_ERR3 ("rmts_get_buffer: Invalid prams %d,%d,%d",
                  (uint32) client_handle, (uint32)buff, size);
    return RMTS_STATUS_ERROR_PARAM;
  }

  RMTS_MSG_MED1 ("rmts: Client #%d needs buffer", pclient_data->id);

  if (pclient_data->alloc_done)
  {
    if (pclient_data->shared_ram_size != size)
    {
      RMTS_MSG_ERR2 ("Client trying to reallocate mem of different size, %d %d",
                      pclient_data->shared_ram_size, size);
      *buff = (uint8 *)NULL;
      return RMTS_STATUS_ERROR_NOMEM;
    }

    RMTS_MSG_MED ("rmts: Client buffer already allocated");
    *buff = (uint8 *)(unsigned long)pclient_data->shared_ram_addr;
    return RMTS_STATUS_SUCCESS;
  }

  /* Buffer not already allocated. */

  if (rmts_info.is_fusion == TRUE)
  {
    status = rmts_get_buffer_locally (pclient_data, size);
  }
  else
  {
    if (pclient_data->isEFSClient == FALSE)
    {
      RMTS_MSG_ERR ("rmts: No support for external clients on MSMs");
      status = RMTS_STATUS_ERROR_NOT_SUPPORTED;
    }
    else
    {
      for (i = 0; i < RMTS_MAX_RECONNECT_RETRIES + 1; i++)
      {
        if (i > 0)
        {
          RMTS_MSG_ERR1 ("Server restarted during buf alloc. Retry count: %d",
                         i);
          /* Server has restarted, reset the init data to reconnect */
          rmts_reset_init_data ();
        }
        status = do_rmts_init_sequence (pclient_data, os_params);
        if (status == RMTS_STATUS_SUCCESS)
        {
          status = rmts_get_buffer_core (pclient_data, size, os_params);
        }

        if (status != RMTS_STATUS_ERROR_SERVER_DOWN)
        {
          break; /* Success or unexpected failure. Break */
        }
      }
    }
  }

  /* If buffer allocated, then XPU lock and MMAP the memory for use */
  if (status == RMTS_STATUS_SUCCESS)
  {
    RMTS_MSG_HIGH ("rmts_get_buffer success");
    mpss_remote_address_start = pclient_data->shared_phy_ram_addr;
    mpss_remote_address_end = pclient_data->shared_phy_ram_addr +
                              pclient_data->shared_ram_size - 1;

    if (rmts_info.is_fusion == TRUE) /* no XPU locking and Virtualizing memory for MDM */
    {
      pclient_data->shared_ram_addr = pclient_data->shared_phy_ram_addr;
    }
    else
    {
      rmts_xpu_lock (mpss_remote_address_start, mpss_remote_address_end);
      rmts_mmap_shared_ram (pclient_data->shared_phy_ram_addr,
                            pclient_data->shared_ram_size,
                            &pclient_data->shared_ram_addr);
    }

    *buff = (uint8 *)(unsigned long)pclient_data->shared_ram_addr;
    pclient_data->alloc_done = TRUE;
  }
  else
  {
    RMTS_MSG_ERR1 ("rmts_get_buffer failed %d", status);
    pclient_data->shared_phy_ram_addr = 0;
    pclient_data->shared_ram_addr = 0;
    pclient_data->shared_ram_size = 0;
    pclient_data->alloc_done = FALSE;
    *buff = (uint8 *)NULL;
  }

  return status;
}

uint32 get_partition_handle_for_guid
(
  struct client_info *pclient_data,
  struct rmts_guid *guid
)
{
  uint32 handle_for_guid = RMTS_INVALID_HANDLE;
   uint32 i;

   if (pclient_data->isEFSClient)
   {
      // Check regular GUIDs
      for (i = 0; i < MAX_PARTITIONS; ++i)
      {
        if (0 == memcmp(guid, &partition_guids[i], sizeof(struct rmts_guid)))
        {
           pclient_data->is_fusion_guid = FALSE;
           handle_for_guid = pclient_data->handles[i];
           goto done;
        }
      }

      // Check fusion GUIDs
      for (i = 0; i < MAX_PARTITIONS; ++i)
      {
        if (0 == memcmp(guid,
                        &fusion_partition_guids[i],
                        sizeof(struct rmts_guid)))
        {
           handle_for_guid = pclient_data->handles[i];
           pclient_data->is_fusion_guid = TRUE;
           goto done;
        }
      }
   }
   else
   {
      handle_for_guid = pclient_data->handles[0];
   }

done:
   return handle_for_guid;

}

static uint32
rmts_get_address_for_rw_iovec (struct client_info *pclient_data,
                               uint32 virtual_addr, int get_address_in_offset)
{
  uint64 offset;
  uint64 virtual_base_addr;
  uint64 physical_base_addr;

  virtual_base_addr = pclient_data->shared_ram_addr;
  physical_base_addr = pclient_data->shared_phy_ram_addr;

  offset = virtual_addr - virtual_base_addr;

  if (get_address_in_offset)
  {
    return (uint32) offset;
  }
  else
  {
    return (uint32) (physical_base_addr + offset);
  }
}

static int
rmts_is_iovec_address_valid (struct client_info *pclient_data,
                             rmts_iovec_t *iovec, uint32 size)
{
  uint32 start_addr, end_addr, allowed_start_addr, allowed_end_addr;
  uint32 i;

  allowed_start_addr = pclient_data->shared_ram_addr;
  allowed_end_addr = pclient_data->shared_ram_addr +
                     pclient_data->shared_ram_size;

  for (i = 0; i < size; i++)
  {
    uint32 max_sectors = pclient_data->shared_ram_size / RMTS_SECTOR_SIZE;
    if (iovec->num_sector > max_sectors)
    {
      RMTS_MSG_ERR2 ("rw_iovec supplied bad params, num_sectors %d > max %d",
                     iovec->num_sector, max_sectors);
      return 0;
    }

    if (iovec->sector_addr > max_sectors)
    {
      RMTS_MSG_ERR2 ("rw_iovec supplied bad params, num_sectors %d > max %d",
                     iovec->sector_addr, max_sectors);
      return 0;
    }

    if ((iovec->sector_addr + iovec->num_sector) > max_sectors)
    {
      RMTS_MSG_ERR2 ("rw_iovec supplied bad params, end sector %d > max %d",
                     iovec->num_sector + iovec->sector_addr, max_sectors);
      return 0;
    }

    start_addr = iovec->data_phy_addr;
    if (start_addr < allowed_start_addr)
    {
      RMTS_MSG_ERR2 ("rw_iovec supplied bad params, start_addr %d < allowed %d",
                     start_addr, allowed_start_addr);
      return 0;
    }

    if (start_addr > allowed_end_addr)
    {
      RMTS_MSG_ERR2 ("rw_iovec supplied bad params, start_addr %d > allowed %d",
                     start_addr, allowed_end_addr);
      return 0;
    }

    end_addr = start_addr + (RMTS_SECTOR_SIZE * iovec->num_sector);
    if (end_addr > allowed_end_addr)
    {
      RMTS_MSG_ERR2 ("rw_iovec supplied bad params, end_addr %d > allowed %d",
                     end_addr, allowed_end_addr);
      return 0;
    }
  }

  return 1;
}

static int
rmts_is_handle_valid (struct client_info *pclient_data, uint32 handle)
{
  int result = 0;

  if (handle == RMTS_INVALID_HANDLE)
  {
    return 0;
  }

  if (rmts_info.is_fusion == TRUE)
  {
    if (pclient_data->isEFSClient == FALSE)
    {
      result = 1;
    }
    else
    {
      if (pclient_data->is_fusion_guid == TRUE)
      {
        result = 1;
      }
    }
  }
  else
  {
    if (pclient_data->is_fusion_guid == FALSE)
    {
      result = 1;
    }
  }

  return result;
}

static enum rmts_status
rmts_read_iovec_core (struct client_info *pclient_data,
                      struct rmts_guid *guid, rmts_iovec_t *iovec,
                      uint32 size, rmts_os_params_t *os_params)
{
  enum rmts_status status;
  qmi_client_error_type rc;
  uint32 server_fs_handle = -1, i = 0;
  rmtfs_rw_iovec_req_msg_v01  *rmtfs_req = NULL;
  rmtfs_rw_iovec_resp_msg_v01 *rmtfs_resp = NULL;
  unsigned int time_out;
  int get_addr_in_offset = 0;

  time_out = (unsigned int) os_params->timeout_in_ms;

  server_fs_handle = get_partition_handle_for_guid(pclient_data, guid);
  if (rmts_is_handle_valid (pclient_data, server_fs_handle) != 1)
  {
    RMTS_MSG_ERR ("rmts_read_iovec : Invalid handle");
    return RMTS_STATUS_ERROR_PARAM;
  }

  RMTS_MSG_MED ("rmts: read iovec partition found");

  rmtfs_req = (rmtfs_rw_iovec_req_msg_v01 *)
            malloc(sizeof(rmtfs_rw_iovec_req_msg_v01));
  if (rmtfs_req == NULL)
  {
    status = RMTS_STATUS_ERROR_NOMEM;
    goto done;
  }

  rmtfs_resp = (rmtfs_rw_iovec_resp_msg_v01 *)
            malloc(sizeof(rmtfs_rw_iovec_resp_msg_v01));
  if (rmtfs_resp == NULL)
  {
    status = RMTS_STATUS_ERROR_NOMEM;
    goto done;
  }

  memset(rmtfs_resp, 0, sizeof(rmtfs_rw_iovec_resp_msg_v01));
  memset(rmtfs_req, 0, sizeof(rmtfs_rw_iovec_req_msg_v01));

  rmtfs_req->caller_id = server_fs_handle;
  rmtfs_req->direction = RMTFS_DIRECTION_READ_V01;
  rmtfs_req->iovec_struct_len = size;

  for (i = 0; i < size; i++)
  {
    rmtfs_req->iovec_struct[i].sector_addr = iovec[i].sector_addr;
    rmtfs_req->iovec_struct[i].num_sector = iovec[i].num_sector;
  }

  /* If address was obtained from kernel instance, we are talking to new
     user space service that that is unaware of physical address and requires
     offset. If talking to older rmts service, provide physical address */
  get_addr_in_offset = (rmts_info.smem_kernel_service_found == 1) ? 1 : 0;
  for (i = 0; i < size; i++)
  {
    rmtfs_req->iovec_struct[i].data_phy_addr_offset =
        rmts_get_address_for_rw_iovec (pclient_data, iovec[i].data_phy_addr,
                                       get_addr_in_offset);

    RMTS_MSG_MED3 ("rmts: sector_addr = %d buffer = %d num sec = %d",
                   rmtfs_req->iovec_struct[i].sector_addr,
                   rmtfs_req->iovec_struct[i].data_phy_addr_offset,
                   rmtfs_req->iovec_struct[i].num_sector);
  }

  pclient_data->read_os_params.sig = os_params->sig;
  pclient_data->read_os_params.timer_sig = RMTS_OP_TIMEOUT_SIG;
  pclient_data->read_os_params.timed_out = 0; //no timeout
  pclient_data->read_os_params.tcb = rex_self();

  rc = qmi_client_init(&rmts_info.service_info[0],
                       rmts_info.service_object,
                       NULL,
                       NULL,
                       &pclient_data->read_os_params,
                       &pclient_data->read_client);
  status = rmts_map_qmi_to_rmts_error (rc);

  RMTS_MSG_MED1 ("QMI read pclient_data->write_client = %d",
                 pclient_data->read_client);

  RMTS_MSG_HIGH2 ("rmts_read_iovec: qmi_client_init rc = %d, status = %d",
                 rc, status);

  if (rc != QMI_NO_ERR)
  {
    goto done;
  }

  rc = qmi_client_send_msg_sync(pclient_data->read_client,
                                QMI_RMTFS_RW_IOVEC_REQ_V01,
                                (void *)rmtfs_req,
                                sizeof(rmtfs_rw_iovec_req_msg_v01),
                                (void *)rmtfs_resp,
                                sizeof(rmtfs_rw_iovec_resp_msg_v01),
                                time_out);
  status = rmts_map_qmi_to_rmts_error (rc);
  RMTS_MSG_MED2 ("rmts_read_iovec: qmi_send_msg_sync rc = %d, status = %d",
                 rc, status);

  if (rc != QMI_NO_ERR)
  {
    goto done;
  }

  status = rmts_map_service_to_rmts_error (rmtfs_resp->resp.error);
  if (rmtfs_resp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    goto done;
  }

  ASSERT (status == RMTS_STATUS_SUCCESS);

done:
  if ( NULL != rmtfs_req )
    free(rmtfs_req);
  if ( NULL != rmtfs_resp )
    free(rmtfs_resp);

  if (pclient_data->read_client != NULL)
  {
    (void) qmi_client_release (pclient_data->read_client);
  }

  RMTS_MSG_MED("Read done");

  return status;
}

/** \details
 * rmts_read_iovec synchronously reads data from eMMC sectors specified in the
 * iovec into the memory region specified by the iovec.
 *
 * @param[in] client_handle
 *    client_handle returned by rmts_get_handle
 *
 * @param[in] partition_id
 *    Partition identifier (16 byte GUID). If GUIDs are not used, the partititon
 *    name.
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
 *     Success or error code as described above. Valid errors for this operation
 *     are
 *       RMTS_STATUS_ERROR_PARAM,
 *       RMTS_STATUS_ERROR_SERVER_DOWN,
 *       RMTS_STATUS_ERROR_NOT_SUPPORTED,
 *       RMTS_STATUS_ERROR_TIMEOUT,
 *       RMTS_STATUS_ERROR_QMI,
 *       RMTS_STATUS_ERROR_TRANSPORT, and
 *       RMTS_STATUS_ERROR_SYSTEM
 */
enum rmts_status rmts_read_iovec (void *client_handle,
                                  struct rmts_guid *guid,
                                  rmts_iovec_t *iovec,
                                  uint32 size,
                                  rmts_os_params_t *os_params)
{
  enum rmts_status status;
  int i;
  struct client_info *pclient_data = (struct client_info *) client_handle;

  if (NULL == client_handle || NULL == iovec || NULL == os_params || 0 == size)
  {
    RMTS_MSG_ERR3 ("rmts_read_iovec: Invalid prams %d, %d, %d",
                  (uint32) client_handle, (uint32) iovec, (uint32)os_params);
    return RMTS_STATUS_ERROR_PARAM;
  }

  if (!rmts_is_iovec_address_valid (pclient_data, iovec, size))
  {
    RMTS_MSG_ERR2 ("Invalid read iovec address params: %d, %d",
                  (uint32) iovec, (uint32) size);
    return RMTS_STATUS_ERROR_PARAM;
  }

  if (TRUE == pclient_data->isEFSClient && NULL == guid)
  {
    RMTS_MSG_ERR1 ("Invalid read iovec GUID: %d", (uint32) guid);
    return RMTS_STATUS_ERROR_PARAM;
  }

  RMTS_MSG_MED1 ("rmts: Client %d read", pclient_data->id);

  if ((rmts_info.is_fusion == TRUE) && (pclient_data->isEFSClient == TRUE))
  {
    return RMTS_STATUS_ERROR_NOT_SUPPORTED;
  }

  for (i = 0; i < RMTS_MAX_RECONNECT_RETRIES + 1; i++)
  {
    if (i > 0)
    {
      RMTS_MSG_ERR1 ("Server restarted during read. Retry count: %d", i);

      /* Server has restarted, reset the init data to reconnect */
      rmts_reset_init_data ();
    }
    status = do_rmts_init_sequence (pclient_data, os_params);
    if (status == RMTS_STATUS_SUCCESS)
    {
      status = rmts_read_iovec_core (pclient_data, guid, iovec, size,
                                     os_params);
      if (status != RMTS_STATUS_ERROR_SERVER_DOWN)
      {
        break; /* Success or unexpected failure. Break */
      }
    }
  }

  return status;
}

/* Core of rmts_write_iovec which does the actual connet and write to server
   side */
static enum rmts_status
rmts_write_iovec_core (struct client_info *pclient_data,
                       struct rmts_guid *guid, rmts_iovec_t *iovec,
                       uint32 size, rmts_os_params_t *os_params)
{
  enum rmts_status status;
  qmi_client_error_type rc;
  uint32 server_fs_handle = -1, i = 0;
  rmtfs_rw_iovec_req_msg_v01  *rmtfs_req = NULL;
  rmtfs_rw_iovec_resp_msg_v01 *rmtfs_resp = NULL;
  unsigned int time_out;
  int get_addr_in_offset = 0;

  RMTS_MSG_MED1 ("rmts: Client #%d write", pclient_data->id);

  time_out = (unsigned int) os_params->timeout_in_ms;

  server_fs_handle = get_partition_handle_for_guid(pclient_data, guid);
  if (rmts_is_handle_valid (pclient_data, server_fs_handle) != 1)
  {
    RMTS_MSG_ERR ("rmts_write_iovec : Invalid handle");
    return RMTS_STATUS_ERROR_PARAM;
  }

  RMTS_MSG_MED ("rmts: write iovec partition found");

  rmtfs_req = (rmtfs_rw_iovec_req_msg_v01 *)
               malloc(sizeof(rmtfs_rw_iovec_req_msg_v01));
  if (rmtfs_req == NULL)
  {
    status = RMTS_STATUS_ERROR_NOMEM;
    goto done;
  }

  rmtfs_resp = (rmtfs_rw_iovec_resp_msg_v01 *)
                malloc(sizeof(rmtfs_rw_iovec_resp_msg_v01));
  if (rmtfs_resp == NULL)
  {
    status = RMTS_STATUS_ERROR_NOMEM;
    goto done;
  }

  memset(rmtfs_resp, 0, sizeof(rmtfs_rw_iovec_resp_msg_v01));
  memset(rmtfs_req, 0, sizeof(rmtfs_rw_iovec_req_msg_v01));

  rmtfs_req->caller_id = server_fs_handle;
  rmtfs_req->direction = RMTFS_DIRECTION_WRITE_V01;
  rmtfs_req->iovec_struct_len = size;

  for ( i = 0; i < size; i++)
  {
    rmtfs_req->iovec_struct[i].sector_addr = iovec[i].sector_addr;
    rmtfs_req->iovec_struct[i].num_sector = iovec[i].num_sector;
  }

  /* If address was obtained from kernel instance, we are talking to new
     user space service that that is unaware of physical address and requires
     offset. If talking to older rmts service, provide physical address */
  get_addr_in_offset = (rmts_info.smem_kernel_service_found == 1) ? 1 : 0;
  for (i = 0; i < size; i++)
  {
    rmtfs_req->iovec_struct[i].data_phy_addr_offset =
        rmts_get_address_for_rw_iovec (pclient_data, iovec[i].data_phy_addr,
                                       get_addr_in_offset);

    RMTS_MSG_MED3 ("rmts: sector_addr = %d buffer = %d num sec = %d",
                   rmtfs_req->iovec_struct[i].sector_addr,
                   rmtfs_req->iovec_struct[i].data_phy_addr_offset,
                   rmtfs_req->iovec_struct[i].num_sector);
  }

  pclient_data->write_os_params.sig = os_params->sig;
  pclient_data->write_os_params.timer_sig = RMTS_OP_TIMEOUT_SIG;
  pclient_data->write_os_params.timed_out = 0; //no timeout
  pclient_data->write_os_params.tcb = rex_self();

  rc = qmi_client_init (&rmts_info.service_info[0],
                        rmts_info.service_object,
                        NULL,
                        NULL,
                        &pclient_data->write_os_params,
                        &pclient_data->write_client);
  status = rmts_map_qmi_to_rmts_error (rc);

  RMTS_MSG_MED1 ("QMI WRITE pclient_data->write_client = %d",
                 pclient_data->write_client);

  RMTS_MSG_HIGH2 ("rmts_write_iovec: qmi_client_init rc = %d, status = %d",
                  rc, status);

  if(rc != QMI_NO_ERR)
  {
    goto done;
  }

  RMTS_MSG_LOW ("rmts_write_iovec: start qmi");
  rc = qmi_client_send_msg_sync(pclient_data->write_client,
                                QMI_RMTFS_RW_IOVEC_REQ_V01,
                                (void *)rmtfs_req,
                                sizeof(rmtfs_rw_iovec_req_msg_v01),
                                (void *)rmtfs_resp,
                                sizeof(rmtfs_rw_iovec_resp_msg_v01),
                                time_out);
  status = rmts_map_qmi_to_rmts_error (rc);
  RMTS_MSG_HIGH2 ("rmts_write_iovec: qmi_send_msg_sync rc = %d, status = %d",
                   rc, status);

  if (rc != QMI_NO_ERR)
  {
    goto done;
  }

  status = rmts_map_service_to_rmts_error (rmtfs_resp->resp.error);
  if (rmtfs_resp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    goto done;
  }

  ASSERT (status == RMTS_STATUS_SUCCESS);

done:
  if ( NULL != rmtfs_req )
    free(rmtfs_req);
  if ( NULL != rmtfs_resp )
    free(rmtfs_resp);

  if (pclient_data->write_client != NULL)
  {
    (void) qmi_client_release (pclient_data->write_client);
  }

  RMTS_MSG_MED ("Write done");
  return status;
}


/** \details
 * rmts_write_iovec synchronously writes data into the eMMC sectors specified
 * in the iovec from the memory specified by the iovec.
 *
 * @param[in] client_handle
 *    client_handle returned by rmts_get_handle
 *
 * @param[in] partition_id
 *    Partition identifier (16 byte GUID). If GUIDs are not used, the
 *    partititon name.
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
 *     Success or error code as described above. Valid errors for this
 *     operation are
 *       RMTS_STATUS_ERROR_PARAM,
 *       RMTS_STATUS_ERROR_SERVER_DOWN,
 *       RMTS_STATUS_ERROR_WRITE_PROTECTED,
 *       RMTS_STATUS_ERROR_TIMEOUT,
 *       RMTS_STATUS_ERROR_QMI,
 *       RMTS_STATUS_ERROR_TRANSPORT, and
 *       RMTS_STATUS_ERROR_SYSTEM
 */
enum rmts_status rmts_write_iovec (void *client_handle,
                                   struct rmts_guid *guid,
                                   rmts_iovec_t *iovec,
                                   uint32 size,
                                   rmts_os_params_t *os_params)
{
  enum rmts_status status;
  int i;
  struct client_info *pclient_data = (struct client_info *) client_handle;

  if (NULL == client_handle || NULL == iovec || NULL == os_params || 0 == size)
  {
    RMTS_MSG_ERR3 ("rmts_write_iovec: Invalid prams %d, %d, %d",
                  (uint32) client_handle, (uint32) iovec, (uint32)os_params);
    return RMTS_STATUS_ERROR_PARAM;
  }

  if (!rmts_is_iovec_address_valid (pclient_data, iovec, size))
  {
    RMTS_MSG_ERR2 ("Invalid write iovec address params: %d, %d",
                  (uint32) iovec, (uint32) size);
    return RMTS_STATUS_ERROR_PARAM;
  }

  if (TRUE == pclient_data->isEFSClient && NULL == guid)
  {
    RMTS_MSG_ERR1 ("Invalid write iovec GUID: %d", (uint32) guid);
    return RMTS_STATUS_ERROR_PARAM;
  }

  for (i = 0; i < RMTS_MAX_RECONNECT_RETRIES + 1; i++)
  {
    if (i > 0)
    {
      RMTS_MSG_ERR1 ("Server restarted during write. Retry count: %d", i);
      /* Server has restarted, reset the init data to reconnect */
      rmts_reset_init_data ();
    }
    status = do_rmts_init_sequence (pclient_data, os_params);
    if (status == RMTS_STATUS_SUCCESS)
    {
      status = rmts_write_iovec_core (client_handle, guid, iovec, size,
                                      os_params);
      if (status != RMTS_STATUS_ERROR_SERVER_DOWN)
      {
        break; /* Success or unexpected failure. Break */
      }

      if (rmts_info.is_fusion == TRUE)
      {
        break; /* Server restart not expected or handled for fusion */
      }
    }
  }

  return status;
}


/** \details
 * rmts_close deregisters the handle assigned in rmts_get_handle. This is used
 * to close the
 * remote storage session opened by the client
 *
 * @param[in] client_handle
 *    client_handle returned by rmts_get_handle
 *
 * @return enum rmts_status
 *     Status - RMTS_STATUS_SUCCESS or RMTS_STATUS_ERROR_PARAM
 */
enum rmts_status
rmts_close (uint32 *client_handle)
{
  (void) client_handle;
   return RMTS_STATUS_SUCCESS;
}
