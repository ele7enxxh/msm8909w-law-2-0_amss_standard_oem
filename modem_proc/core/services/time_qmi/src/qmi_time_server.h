/*=============================================================================
  Copyright (c) 2003-2013,2015 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/**
@file qmi_time_server.c
@brief

Implements time server for QMI remoting.

INITIALIZATION AND SEQUENCING REQUIREMENTS

time server should be started from tmc.c

*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time_qmi/src/qmi_time_server.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/23/15   dks     Replaced qmi core framework with qcsi calls

=============================================================================*/


#ifndef _QMI_TIME_SERVER_H_
#define _QMI_TIME_SERVER_H_

/* Private part of the server */
#include <stdint.h>
#include "qmi_csi_common.h"
#include "time_service_v01.h"
#include "time_service_impl_v01.h"

#include "time_genoff.h"

#if (!defined (FEATURE_NO_DB) && !defined (FEATURE_GSM_PLT))
#include "db.h"
#endif

#define QMI_TIME_SVC_WAIT_SIG 0x00020000
#define QMI_TIME_SVC_STOP_SIG 0x00002000
#define TIME_MAX_NUM_CLIENTS 10
#define TIME_MAX_CLIENT_SIG_STR_THRESHOLDS                (5)

/*This struct keeps track of the clients that connects through connect callback*/
typedef  struct time_server_client_data  {
    uint8_t                       active_flag;
    qmi_client_handle             client;
}time_server_client_data_type;


/* This is the server object */
typedef struct time_server_class {
    /* Client data */
    time_server_client_data_type       client_data[TIME_MAX_NUM_CLIENTS];
    uint32_t                           client_index;/* Index into the client data array */
    uint32_t                           client_counter;/*Counts number of active clients */
    /* Rest of the server data goes here */
    uint32_t                           event_signal;
    qmi_csi_service_handle             service_handle;
    uint8_t                            service_registered;
}time_server_class_type;



/* Private part ends here */


/* Public part begins here */

typedef time_server_class_type time_server;


typedef  enum {
    //Inherits qmi server error codes
    TIME_SERVER_NO_ERROR = QMI_CSI_CB_NO_ERR,
    TIME_SERVER_INTERNAL_ERR = QMI_CSI_CB_INTERNAL_ERR,
    TIME_SERVER_NOT_INITIALISED,
    TIME_SERVER_INVALID_OBJECT
}time_server_error;

time_server_error
time_server_init (time_server **handle, qmi_csi_os_params *os_params);

time_server_error
time_server_register(time_server *me, qmi_csi_os_params *os_params);

time_server_error
time_server_unregister(time_server *me);


/* FIXME:Send to a particular client*/
time_server_error
time_server_send_ind( time_server *me,
                     int32_t     msg_id,
                     uint16_t    base,
                     uint64_t    offset);

/* Public part ends here */

/*this is test code, this has to go*/
void time_server_call_ind(uint16 base, uint64 offset);

qmi_csi_os_params* time_server_get_os_params
(
  time_server  *me
);

void time_server_handle_event
(
  time_server  *me,
  rex_sigs_type        *event
);

/*
void time_server_call_ind1(void);
*/

#endif
