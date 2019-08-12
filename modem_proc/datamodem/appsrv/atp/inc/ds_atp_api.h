/******************************************************************************
  @file    ds_atp_api.h
  @brief   API header file for ATP(Application Traffic Paring)

  DESCRIPTION
  Definitions for ATP API functions

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/appsrv/atp/inc/ds_atp_api.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/13   jz      Created
===========================================================================*/
#ifndef DS_ATP_API_H
#define DS_ATP_API_H

// #include "dstask_v.h"
#include "ps_in.h"

#define DS_ATP_MAX_PACKAGE_NAME_LEN 256
#define DS_ATP_MAX_NUM_HASH_VALUE   64
#define DS_ATP_MAX_NUM_FRAMEWORK_UID   64
#define DS_ATP_MAX_NUM_PACKAGE   100
#define DS_ATP_MAX_NUM_SERVICE_ID   256
#define DS_ATP_MAX_APN_NAME_LEN     255
#define DS_ATP_MAX_NUM_APN          8
#define DS_ATP_IPV6_ADDR_LEN 16
#define DS_ATP_IPV4_ADDR_LEN  4

typedef enum
{
  DS_ATP_PROXY_EVENT_MIN,

  /*ATP active status indication, sent upon bootup or
    active status changes by NW through OTA*/
  DS_ATP_PROXY_EVENT_ACTIVE_STATUS_IND = DS_ATP_PROXY_EVENT_MIN,

  /*ATP policy update indication, sent when policy file is fetched*/
  DS_ATP_PROXY_EVENT_POLICY_UPDATED_IND,

  /*ATP filter report ack indication, sent when modem finishes
    sending a filter report*/
  DS_ATP_PROXY_EVENT_FILTER_REPORT_ACK_IND,

  DS_ATP_PROXY_EVENT_MAX
} ds_atp_proxy_event_enum_type;

typedef enum
{
  DS_ATP_PROXY_SERVICE_REQ_MIN,

  /*Request for querying active status*/
  DS_ATP_PROXY_SERVICE_ACTIVE_STATUS_QUERY_REQ =
                                    DS_ATP_PROXY_SERVICE_REQ_MIN,

  /*Request for querying ATP policy*/
  DS_ATP_PROXY_SERVICE_POLICY_QUERY_REQ,

  /*Request for sending filter report to the NW*/
  DS_ATP_PROXY_SERVICE_FILTER_REPORT_REQ,

  DS_ATP_PROXY_SERVICE_REQ_MAX
} ds_atp_proxy_service_req_enum_type;

typedef enum
{
  /*Success*/
  DS_ATP_PROXY_ERR_NONE,

  /*Event not supported*/
  DS_ATP_PROXY_ERR_EVENT_NOT_SUPP,

  /*General failure*/
  DS_ATP_PROXY_ERR_FAIL
} ds_atp_proxy_err_enum_type;

typedef struct
{
  /*1: activated; 0: deactivated*/
  uint8 active_status;
} ds_atp_active_status_type;

typedef struct
{
  /* package name string */
  char    package_name[DS_ATP_MAX_PACKAGE_NAME_LEN+1];

  /* number of hash values */
  uint8   hash_num;

  /*number of framework uids*/
  uint8   framework_uid_num;

  /*hash value array*/
  uint32  hash_values[DS_ATP_MAX_NUM_HASH_VALUE];

  /*framework uids*/
  uint32  framework_uid[DS_ATP_MAX_NUM_FRAMEWORK_UID];

  /* timeout value for each filter report, if no ack is received from
    NW in the duration, socket call shall be returned*/
  uint32  max_ack_timeout;

  /*name of the APN the package belongs to*/
  char    apn[DS_ATP_MAX_APN_NAME_LEN+1];

  /*service ID the package belongs to*/
  uint32  service_id;
} ds_atp_policy_pkg_info_type;

typedef struct
{
  /*number of packages in total*/
  uint32                          num_package;

  /*policy info for each package*/
  ds_atp_policy_pkg_info_type     package_arr[DS_ATP_MAX_NUM_PACKAGE];
} ds_atp_policy_info_type;


typedef enum
{
  DS_ATP_FILTER_ACTION_MIN,
  DS_ATP_FILTER_ACTION_ADD =  1,
  DS_ATP_FILTER_ACTION_REMOVE,
  DS_ATP_FILTER_ACTION_MAX
} ds_atp_filter_action_enum_type;

typedef enum
{
  DS_ATP_FILTER_PROTOCOL_MIN,
  DS_ATP_FILTER_PROTOCOL_UDP = DS_ATP_FILTER_PROTOCOL_MIN,
  DS_ATP_FILTER_PROTOCOL_TCP,
  DS_ATP_FILTER_PROTOCOL_MAX
} ds_atp_filter_protocol_enum_type;

typedef enum
{
  DS_ATP_IPV4,
  DS_ATP_IPV6,
  DS_ATP_INVALID
} ds_atp_ip_type_enum_type;

typedef enum
{
  DS_ATP_FILTER_REPORT_ACK,
  DS_ATP_FILTER_REPORT_NAK
} ds_atp_filter_report_ack_enum_type;

typedef struct
{
  ds_atp_filter_report_ack_enum_type filter_ack;
  uint32                             report_id;
} ds_atp_filter_report_resp_type;

typedef struct
{
  /*ip type: v4 or v6*/
  ds_atp_ip_type_enum_type         ip_type;

  /*destination ip address, all 0 indicates not availiable*/
  union
  {
    /*destination IPv4 address in network order*/
    uint32                           dest_ipv4_addr;
    /*destination IPv6 address*/
    uint8                            dest_ipv6_addr[DS_ATP_IPV6_ADDR_LEN];
  } dest_ip;

  union
  {
    /*source IPv4 address in network order*/
    uint32                           src_ipv4_addr;
    /*source IPv6 address*/
    uint8                            src_ipv6_addr[DS_ATP_IPV6_ADDR_LEN];
  } src_ip;

  /*destination port, 0 indicates destination port not available*/
  uint16                           dest_port;

  /*source port, 0 indicates source port no available*/
  uint16                           src_port;

  /*protocol type*/
  ds_atp_filter_protocol_enum_type protocol;
}ds_atp_filter_report_tuple_type;

typedef struct
{
   /*filter action*/
  ds_atp_filter_action_enum_type    action;

  /*service id*/
  uint32                            service_id;

  /*package name length*/
  uint8                             package_name_len;

  /*package name*/
  char                              package_name[DS_ATP_MAX_PACKAGE_NAME_LEN+1];

  /*number of hash values*/
  uint8                             hash_num;

  /*hash value array*/
  uint32                            hash_values[DS_ATP_MAX_NUM_HASH_VALUE];

  /*tuple information*/
  ds_atp_filter_report_tuple_type   tuple_info;
}ds_atp_filter_report_content_type;

typedef struct
{
  /*filter report content*/
  ds_atp_filter_report_content_type   filter_report;

  /*a track ID generated by AP, will be returned in the corresponding ack indication */
  uint32                              report_id;
}ds_atp_filter_report_type;




#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================
FUNCTION DS_ATP_PROCY_EVENT_CALLBACK_F_PTR_TYPE

DESCRIPTION
  Callback function for ATP event handling.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
typedef void (*ds_atp_event_callback_f_ptr_type)(
                                          ds_atp_proxy_event_enum_type  event,
                                          void                          *event_info_ptr,
                                          void                          *user_data_ptr
                                          );

/*===========================================================================
FUNCTION DS_ATP_PROCY_EVENT_REG

DESCRIPTION
  Registration fucntion for clients to register any ATP events they are
  interested in. Clients provide the event ID and callback handling, ATP
  Proxy will callback to the registered clients when registered events
  need to be triggered.

DEPENDENCIES
  None

RETURN VALUE
  See ds_atp_proxy_err_enum_type definition

SIDE EFFECTS
  None
===========================================================================*/
ds_atp_proxy_err_enum_type ds_atp_proxy_event_reg(
                      ds_atp_proxy_event_enum_type      start_event,
                      ds_atp_proxy_event_enum_type      end_event,
                      ds_atp_event_callback_f_ptr_type  event_callback_f_ptr,
                      void                              *user_data_ptr
                       );

/*===========================================================================
FUNCTION DS_ATP_PROCY_EVENT_DEREG

DESCRIPTION
  Deregistration fucntion for clients to deregister any ATP events.

DEPENDENCIES
  None

RETURN VALUE
  See ds_atp_proxy_err_enum_type definition

SIDE EFFECTS
  None
===========================================================================*/
ds_atp_proxy_err_enum_type ds_atp_proxy_event_dereg(
                                    ds_atp_proxy_event_enum_type start_event,
                                    ds_atp_proxy_event_enum_type end_event,
                                    ds_atp_event_callback_f_ptr_type  event_callback_f_ptr
                                    );

/*===========================================================================
FUNCTION DS_ATP_PROCY_SERVICE_REQ

DESCRIPTION
  API for ATP service request.

DEPENDENCIES
  None

RETURN VALUE
  See ds_atp_proxy_err_enum_type definition

SIDE EFFECTS
  None
===========================================================================*/
ds_atp_proxy_err_enum_type  ds_atp_proxy_service_req(
                                   ds_atp_proxy_service_req_enum_type   request,
                                   void                                 *arg_val_ptr
                                   );

/*===========================================================================
FUNCTION DS_ATP_INIT

DESCRIPTION
  ATP initialization

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_atp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* DS_ATP_API_H */

