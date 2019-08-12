#ifndef _SNS_USMR_PRIV_H
#define _SNS_USMR_PRIV_H
/*============================================================================
  @file sns_usmr_priv.h

  @brief Contains type definitions and function declarations that are only used
  internally by the uImage SMR.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/
#include "sns_usmr.h"
#include "sns_em.h"
#include "sns_queue.h"
#include "sns_memmgr.h"
#include "sns_uimg_utils.h"
#include "sns_profiling.h"

/*===========================================================================
  MACROS
  ===========================================================================*/

/* Maximum number of signal bits available for SMR use (Max: 24). */
#define SMR_MAX_SERVICES_SIG 14
/* Maximum number of services that will share a single signal bit.
 * Theoretically, higher values will make SMR perform less efficiently. */
#define SMR_MAX_SERVICES_PER_SIGNAL 2
/* Maximum number of services that may be registered with SMR */
#define SMR_MAX_SERVICES SMR_MAX_SERVICES_SIG * SMR_MAX_SERVICES_PER_SIGNAL
#define SMR_ALL_SERVICES_SIGS    0x00FFFFFF

#ifdef SNS_USES_ISLAND
# define SNS_SMR_UIMAGE_CODE __attribute__((section (".text.uSMR")))
# define SNS_SMR_UIMAGE_DATA __attribute__((section (".data.uSMR")))
# define SMR_LEAVE_ISLAND() \
    sns_utils_place_uimge_hold(SNS_UIMG_SMR)
# define SMR_ENTER_ISLAND() \
    sns_utils_remove_uimage_hold(SNS_UIMG_SMR)
#else
# define SNS_SMR_UIMAGE_CODE
# define SNS_SMR_UIMAGE_DATA
# define SMR_LEAVE_ISLAND()
# define SMR_ENTER_ISLAND()
#endif /* USES_ISLAND */

/* The number of maximum services supported is currently restricted by the
   number of signal bits, each service requires a unique signal bit. We are
   reserving 24 bits for services, leaving 8 for SMR + expansion.
   The lowest order bits shall be used for services
*/
#define SMR_QMI_CLIENT_SIG       0x80000000
#define SMR_SVC_CHECK_RESULT_SIG 0x40000000
#define SMR_ALL_SIGS             0xFFFFFFFF

/*===========================================================================
  TYPE DEFINITIONS
  ===========================================================================*/

/**
 * Represents a particular client-service connection.  To be used by clients
 * to send requests over an open connection, and for services to send
 * indication messages.  Every connection, regardless of whether it uses QMI,
 * will have an associated client handle object.  Each client handle will be
 * included on the client_q list.
 */
struct smr_client_hndl_s
{
  /* Next client on the client_q; or NULL if last */
  struct smr_client_hndl_s    *next;
  /* Unique identifier of this client */
  uint32_t                    client_id;

  /* Only one of the following fields will be in-use for any given object */
  union {
    /* Where this structure is used to represent a connection made by a client
    * to some service (through QMI), this field serves as an unique QMI
    * identifier for this client<->service connection. */
    qmi_client_type             qmi_client;
    /* Similar to qmi_client. Where this structure is used to represent an
    * incoming connection to a SMR-hosted service, this field is an unique QMI
    * identifier to this client<->service connection. */
    qmi_client_handle           qmi_client_conn_handle;
  };
  /* Indication callback function pointer. */
  smr_client_ind_cb           ind_cb;
  /* Callback data, as specified by the client, to be returned through ind_cb. */
  void                        *ind_cb_data;
  /* Signalling data used by QMI */
  qmi_client_os_params        os_params;
  /* Sensor service-provided callback data; to be used in conjunction with
   * qmi_client_conn_handle. */
  void                        *connection_handle;
  /* When a client has requested that their connection be terminated, they
   * specify this cllback function, to be used when SMR can guarantee that no
   * further indication callback will be received. */
  smr_client_release_cb       release_cb;
  /* Client-specific callback to be used in conjunction with release_cb */
  void                        *release_cb_data;
  /* Outstanding request messages; only used for SMR-bypass clients */
  struct smr_req_handle_s     *req_q;
  /* QMI Service object associated with this connected service. */
  qmi_idl_service_object_type service_obj;
  /* If the service is hosted by SMR, the index into smr_services.services[] */
  int8_t                      service_index;
  /* This client is busy; buffer indications until we get the all-clear. */
  bool                        busy;
};

/**
 * An outstanding request, received by a service.  This handle shall be
 * used by the service to send the appropriate response message.  Note that
 * the response message will not be sent to the client, if the client has
 * already disconnected.
 */
struct smr_req_handle_s
{
  struct smr_req_handle_s *next;
  /* Response callback function, as specified by the client. */
  smr_client_resp_cb      resp_cb;
  /* Callback data to be returned during resp_cb. */
  void                    *resp_cb_data;
  /* Client ID, to be matched with smr_client_hndl_s.client_id. */
  uint32_t                client_id;
  /* If this request involves travel over QMI, the associated handle. */
  qmi_req_handle          req_handle;
  /* Buffer pre-allocated by the client to place response message. */
  void                    *resp_c_struct;
  /* If the service is hosted by SMR, the index into smr_services.services[] */
  int8_t                  service_index;
  /* Length of pre-allocated response buffer. */
  uint8_t                 resp_c_struct_len;
};

/**
 * A particular service registered with SMR.
 */
struct smr_service_hndl_s
{
  /* QMI service object for this SMR service */
  qmi_idl_service_object_type service_obj;
  /* Instance ID of this service. */
  qmi_service_instance        instance_id;
  /* QMI handle identifying this service. */
  qmi_csi_service_handle      service_provider;
  /* Signalling data used by QMI */
  qmi_csi_os_params           os_params;
  /* Connect callback function registered by the service. */
  smr_service_connect         connect_cb;
  /* Disconnect callback function registered by the service. */
  smr_service_disconnect      disconnect_cb;
  /* Resume callback function registered by the service. */
  smr_service_resume          resume_cb;
  /* Request message callback function registered by the service. */
  smr_process_req             process_req_cb;
  /* Per-service callback data. */
  void                        *service_cookie;
  /* Index of this service handle in smr_services.services[]. */
  int8_t                      service_index;
};

typedef struct
{
  //OS signals
  OS_FLAG_GRP      *smr_flag_grp;
  OS_FLAGS         smr_flags;
  /* Services registered with SMR.  For all other services, SMR will function
   * as a wrapper around QMI. */
  smr_service_hndl services[ SMR_MAX_SERVICES ];
}smr_services_info_s;

typedef struct
{
  sns_q_s   protected_q;
  OS_EVENT* mutex_ptr;
} sns_smr_protected_q_s;


/** Data structure used while determining if a specific service is available
 * via QMI.  Event is received in non-SMR thread, therefore this item will be
 * added to a SMR queue during the callback. */
typedef struct
{
  sns_q_link_s q_link;
  /* Callback for SMR once the service has been found; or timeout. */
  smr_client_init_ext_cb init_cb;
  /* QMI service object for the service we are attempting to find. */
  qmi_idl_service_object_type serviceObj;
  /* Instance ID we require of the service; or SMR_CLIENT_INSTANCE_ANY. */
  qmi_service_instance instanceID;
  /* QMI handle for the notification object. */
  qmi_client_type qmi_notifier_handle;
  /* EM timer if a timeout period was specified. */
  sns_em_timer_obj_t timer;
  /* QMI signalling object. */
  qmi_client_os_params os_params;
  /* Whether this item has already been added to smr_svc_check_cb_data_q.
   * Theoretically protects against race conditions when cancelling the timer. */
  bool addedToQueue;
  /* User parameter to be passed in as a parameter when invoking init_cb */
  void *smr_client_init_cb_param;
} sns_smr_service_check_cb_data;

#endif
