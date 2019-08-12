/******************************************************************************

                        QMI_IP.H

******************************************************************************/

/******************************************************************************

  @file    qmi_ip.h
  @brief   QMI over IP header

  DESCRIPTION


  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
05/19/14   sb         Initial version

******************************************************************************/
/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#ifndef __QMI_IP_H__
#define __QMI_IP_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <linux/filter.h>
#include <linux/ioctl.h>
#include <linux/ip.h>

#include "xmllib_common.h"
#include "xmllib_parser.h"
#include "qmi_client.h"
#include "qmi_i.h"
#include "qmi_idl_lib.h"
#include "wireless_data_service_v01.h"
#include "network_access_service_v01.h"
#include "qmi_ip_service_v01.h"
#include "ds_util.h"
#include "common_v01.h"
#include "qmi_client_instance_defs.h"
#include "qmi_port_defs.h"
#include "netmgr.h"
#include "ds_string.h"
#include "data_common_v01.h"
#include "data_port_mapper_v01.h"

/*===========================================================================
                              VARIABLE DECLARATIONS
===========================================================================*/

#define QMI_TIMEOUT 90000
/* Total message size numbers */
#define QMI_MAX_STD_MSG_SIZE         (512)
#define QMI_REQ_CTRL_FLAGS                0x00
#define QMI_RESP_CTRL_FLAGS               0x02
#define QMI_IND_CTRL_FLAGS                0x04
#define QMI_WDS_SERVICE_TYPE  0x01
#define QMI_NAS_SERVICE_TYPE  0x03
#define QMI_IP_SERVICE_TYPE   0x00
#define MAX_LAN_CLIENTS       16
#define SUCCESS               0
#define FAILURE               -1
#define MAX_QMI_IP_PKT_SIZE   4096
#define MAX_FDS               10
#define MAX_TMGI_INSTANCES    32
#define MAX_TMGI_CLIENT_INST  MAX_TMGI_INSTANCES/MAX_LAN_CLIENTS
#define QMI_IP_IND_HDLR_FILE        "/etc/qmi_ip_ind_hdlr_file"
#define QMI_IP_TUNNEL_MODULE_FILE   "/etc/embms_tm_control_file"
#define QMI_IP_FPOP_FILE            "/etc/qmi_ip_fpop_file"
#define QMI_IP_CONTROL_FILE         "/etc/qmi_ip_control_file"
#define QMI_IP_MAX_INTERFACE_NAME  20
#define QMI_RESP_TLV_LEN           7
#define QMI_IP_ACTIVE              1
#define QMI_IP_INACTIVE            0
#define QMI_IP_FPOP_GRACE_PERIOD   5
#define QMI_IP_READ_BUF_SIZE       1500
#define QMI_IP_FPOP_PKT_SIZE       5
#define QMI_SAMPLE_TX_ID           0x0001
#define SRC_CLIENT_IP_TYPE_OFFSET  0
#define SRC_CLIENT_IP_START        1
#define SRC_CLIENT_IP_END          4
#define QMI_IP_CLIENT_ACTIVE       0
#define QMI_IP_CLIENT_INACTIVE     1
#define IND_REQ_NOT_REQUIRED       2
#define IND_ENABLE                 1
#define IND_DISABLE                0
#define SEND_TO_MODEM              1
#define REPLY_LOCALLY              0
#define MAX_ADDR_LEN               20
#define IP_V4                      0x00
#define QMI_IP_MAJOR_VERSION       1
#define QMI_IP_MINOR_VERSION       0
#define MAX_QMI_EMBD_INSTANCES     7
#define WDS_DISCONNECTED           0x01
#define WDS_IP_FAMILY_PREF_IPV4  0x04
#define WDS_IP_FAMILY_PREF_IPV6  0x06
#define RMNET_INTERFACE            "rmnet_data"
#define DEVICE_RMNET_IPA           "rmnet_ipa0"
#define DEVICE_RMNET_DATA          "rmnet0"
#define BRIDGE_IFACE               "bridge0"

#define MAJOR_NUM                  100
#define TUNNELING_ON               1
#define TUNNELING_OFF              0
#define EMBMS_DEVICE_FILE       "/dev/embms_tm_device"

#define GET_FIRST_BYTE(r)     (r >> 24)
#define GET_SECOND_BYTE(r)    ((r << 8) >> 24)
#define GET_THIRD_BYTE(r)     ((r << 16) >> 24)
#define GET_FOURTH_BYTE(r)    ((r << 24) >> 24)
/*============================================================
  Log Message Macros
=============================================================*/
#define LOG_MSG_INFO1_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO2_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO3_LEVEL           MSG_LEGACY_LOW
#define LOG_MSG_ERROR_LEVEL           MSG_LEGACY_ERROR
#define PRINT_MSG( level, fmtString, x, y, z)                         \
        MSG_SPRINTF_4( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,      \
                       __FUNCTION__, x, y, z);
#define PRINT_MSG_6( level, fmtString, a, b, c, d, e, f)              \
  MSG_SPRINTF_7( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,            \
                 __FUNCTION__, a, b, c, d, e, f);

#define LOG_MSG_INFO1( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO1_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO2( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO2_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO3( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO3_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO1_6( fmtString, a, b, c, d, e, f)                 \
{                                                                     \
  PRINT_MSG_6 ( LOG_MSG_INFO1_LEVEL, fmtString, a, b, c, d, e, f);    \
}
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_ERROR_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_ERROR_6( fmtString, a, b, c, d, e, f)                 \
{                                                                     \
  PRINT_MSG_6( LOG_MSG_ERROR_LEVEL, fmtString, a, b, c, d, e, f);     \
}

#define IPV4_ADDR_MSG(ip_addr) MSG_4(MSG_SSID_DS, \
                                     MSG_LEGACY_HIGH, \
                                     "IPV4 Address is %d.%d.%d.%d", \
                                     (unsigned char)(ip_addr), \
                                     (unsigned char)(ip_addr >> 8), \
                                     (unsigned char)(ip_addr >> 16) , \
                                     (unsigned char)(ip_addr >> 24))

#define QMI_IP_QMUX_SDU_MESSAGE_HDR_LEN_BYTES \
  (sizeof(qmi_ip_qmux_if_type_t) + \
   sizeof(qmi_ip_qmux_hdr_s) + \
   sizeof(qmi_ip_qmux_sdu_hdr_s)+ \
   sizeof(qmi_ip_qmux_qmi_msg_hdr_s))

typedef int (* qmi_ip_read_data) (int fd);
typedef int (* qmi_ip_write_data) (int fd, void* data, int len);

/*---------------------------------------------------------------------------
  Enums
----------------------------------------------------------------------------*/
typedef enum
{
  QMI_IP_WDS_PKT_SRVC_STATUS_IND,
  QMI_IP_WDS_EMBMS_TMGI_ACTIVATE_IND,
  QMI_IP_WDS_EMBMS_TMGI_DEACTIVATE_IND,
  QMI_IP_WDS_EMBMS_TMGI_LIST_IND,
  QMI_IP_WDS_EMBMS_TMGI_ACT_DEACT_IND,
  QMI_IP_NAS_EMBMS_STATUS_IND,
  QMI_IP_NAS_SYS_INFO_IND,
  QMI_IP_MAX_IND
}qmi_ip_ind_t;

typedef enum
{
  QMI_IP_TUNN_INST_DISABLED,
  QMI_IP_TUNN_INST_ENABLING,
  QMI_IP_TUNN_INST_ENABLED,
  QMI_IP_TUNN_INST_DISABLING
}qmi_ip_tun_inst_status_t;

typedef enum
{
  ADD_ENTRY,
  DELETE_ENTRY,
  TMGI_DEACTIVATE,
  CLIENT_ACTIVATE_ALL_TMGI,
  CLIENT_DEACTIVATE_ALL_TMGI,
  SESSION_DEACTIVATE,
  OPEN_SEND_SOCKET
}qmi_ip_tunnel_iface_msg_t;

typedef enum
{
  INACTIVE_MODE      = 0,
  SINGLE_CLIENT_MODE = 1,
  MULTI_CLIENT_MODE  = 2,
}qmi_ip_mode_t;

/*---------------------------------------------------------------------------
  Structures
----------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Socket read and write functions
----------------------------------------------------------------------------*/
typedef struct
{
  int socket_fd;
  qmi_ip_read_data read_ex;
  qmi_ip_write_data write_ex;
} socket_conf_t;

/*---------------------------------------------------------------------------
  Structure to hold sockets maintained by QMI IP
----------------------------------------------------------------------------*/
typedef struct
{
  socket_conf_t sk_conf[MAX_FDS];
  int num_of_fds;
  int max_fd;
  fd_set fdset;
  int fpop_write_socket;
}qmi_ip_socket_info_t;

/*---------------------------------------------------------------------------
  QMI info maintained by QMI IP
----------------------------------------------------------------------------*/
typedef struct
{
  qmi_client_type      qmi_ip_wds_v4_handle;
  uint32               qmi_ip_wds_v4_call_handle;
  qmi_client_type      qmi_ip_nas_handle;
  netmgr_client_hdl_t  netmgr_hndl;
  int                  net_iface_status;
  char                 net_iface_name[QMI_IP_MAX_INTERFACE_NAME];
  int                  sni_count;
  int                  embms_list_ind_count;
  int                  nce_count;
  int                  nas_sys_info_ind_count;
  int                  embms_status_ind_count;
  int                  embms_sai_list_ind_count;

}qmi_info_t;

/*---------------------------------------------------------------------------
  TMGI structure
----------------------------------------------------------------------------*/
typedef struct
{
  uint8 tmgi[6];
}qmi_ip_embms_tmgi_t;


/*---------------------------------------------------------------------------
  Data structure per client to maintain client status
----------------------------------------------------------------------------*/
typedef struct
{
  int                 socket_fd;
  struct in_addr      client_ip;
  struct in_addr      private_ip;
  uint16              udp_data_port;
  timer_t             timerid;
  boolean             init_done;
  boolean             is_active;
  boolean             is_sni_active;
  boolean             is_embms_list_ind_active;
  boolean             is_nce_active;
  boolean             is_nas_sys_info_ind_active;
  boolean             is_embms_status_ind_active;
  boolean             is_embms_sai_list_ind_active;
}qmi_ip_client_ctrl_table_t;

/*---------------------------------------------------------------------------
  Data structure to hold TMGI table/data
----------------------------------------------------------------------------*/
typedef struct
{
  uint32                      multicast_addr;
  uint16                      multicast_port;
  qmi_ip_embms_tmgi_t         client_tmgi;
  qmi_ip_tun_inst_status_t    tmgi_status;
  int                         ref_count;
  int                         client_fds[MAX_LAN_CLIENTS];
}qmi_ip_tmgi_ctrl_t;

/*---------------------------------------------------------------------------
  Data structure to handle act_deact messages
----------------------------------------------------------------------------*/
typedef struct
{
  int                   fd;
  qmi_ip_embms_tmgi_t   act_tmgi;
  boolean               act_tmgi_ind_status;
  qmi_ip_embms_tmgi_t   deact_tmgi;
  boolean               deact_tmgi_ind_status;
}qmi_ip_act_deact_ind_t;

/*---------------------------------------------------------------------------
  QMI IP main data structure: QMI IP config
----------------------------------------------------------------------------*/
typedef struct
{
  qmi_ip_mode_t               mode;
  uint16                      embms_rmnet_link;
  uint16                      fpop_udp_port;
  uint16                      tcp_server_ctrl_port;
  uint16                      single_client_udp_port;
  int                         fpop_timeout;
  int                         fpop_threshold;
  qmi_ip_client_ctrl_table_t  embms_client[MAX_LAN_CLIENTS];
  int                         lan_clients_allowed;
  int                         current_client_cnt;
  qmi_ip_tmgi_ctrl_t          tmgi_instance[MAX_TMGI_INSTANCES];
  int                         current_tmgi_inst_cnt;
  int                         tmgi_count_allowed;
  qmi_ip_socket_info_t        socket_info;
  qmi_info_t                  qmi_info;
  qmi_ip_act_deact_ind_t      act_deact_entry[MAX_TMGI_INSTANCES];
  int                         act_deact_entry_count;
  int                         embms_device_fd;
}qmi_ip_conf_t;

/*---------------------------------------------------------------------------
  Tunneling data sent from QMI IP to Tunneling module
----------------------------------------------------------------------------*/
typedef struct
{
  uint32                     tmgi_multicast_addr;
  uint16                     tmgi_multicast_port;
  uint32                     client_addr;
  uint16                     client_port;
  uint16                     source_data_port;
  qmi_ip_tunnel_iface_msg_t  action_type;
  char                       interface_name[QMI_IP_MAX_INTERFACE_NAME];
}qmi_ip_tun_mgmt_info_t;

/*---------------------------------------------------------------------------
  Indication data received from indication callback context to QMI IP thread.
----------------------------------------------------------------------------*/
typedef struct
{
  uint8   service_type;
  uint16  message_id;
  char    ind_buf[QMI_MAX_STD_MSG_SIZE];
  int     ind_buf_len;
  int     client_id;
}qmi_ip_ind_buf_t;

/*---------------------------------------------------------------------------
  FPOP data received from FPOP thread to QMI IP main thread
----------------------------------------------------------------------------*/
typedef struct
{
  uint32 client_ip;
  uint32 private_ip;
  uint16 private_port;
}qmi_ip_fpop_msg_t;

/*---------------------------------------------------------------------------
  QMUX message packet structure
----------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
  Control channel message preamble byte: Interface Type, which indicates the
  protocol used for the message
---------------------------------------------------------------------------*/
typedef uint8 qmi_ip_qmux_if_type_t;

/*--------------------------------------------------------------------------
  QMUX Header, follows the preamble
---------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  uint16 length;     /*!< Length of the QMUX message including the QMUX header,but not the I/F Type */
  uint8  ctl_flags;  /*!< QMUX Control Flags indicating the sender */
  uint8  svc_type;   /*!< QMI service type of the SDU */
  uint8  client_id;  /*!< Client ID pertaining to this message */
} qmi_ip_qmux_hdr_s;

/*--------------------------------------------------------------------------
  QMUX Service Data Unit Transaction Header for a non-QMI_CTL message
---------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  uint8  svc_ctl_flags;  /*!< QMUX SDU Control Flags indicating message type */
  uint16 txn_id;         /*!< Transaction ID (unique among control points) */
} qmi_ip_qmux_sdu_hdr_s;

/*--------------------------------------------------------------------------
  QMI_CTL Message Header (replaces QMUX SDU Transaction Header for QMI_CTL
    messages)
---------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  uint8 svc_ctl_flags;  /*!< QMI_CTL Control Flags indicating message type */
  uint8 txn_id;         /*!< Transaction ID (unique for every message) */
} qmi_ip_qmux_qmi_ctl_msg_hdr_s;

/*-------------------------------------------------------------------------
  Message ID and message length
--------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  uint16 msg_id;     /*!< QMI Service-specific message ID */
  uint16 msg_length; /*!< Length of TLV data to follow */
} qmi_ip_qmux_qmi_msg_hdr_s;

/*-------------------------------------------------------------------------
  SDU structure specifically for QMI_CTL messages
--------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  qmi_ip_qmux_qmi_ctl_msg_hdr_s hdr;
  qmi_ip_qmux_qmi_msg_hdr_s     msg;
} qmi_ip_qmux_qmi_ctl_sdu_s;

/*-------------------------------------------------------------------------
  SDU structure for non-QMI_CTL messages
--------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  qmi_ip_qmux_sdu_hdr_s     hdr;
  qmi_ip_qmux_qmi_msg_hdr_s msg;
} qmi_ip_qmux_sdu_s;

/*-------------------------------------------------------------------------
  QMUX Message including everything for a single QMI message in the
  transaction except for the TLV data
  @note Because of the union for QMI_CTL/regular SDUs, the size of this struct
  is not accurate to the size of the QMI_CTL header data. Use the constant
  QTI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES to get an offset to the start of TLV data
  for a QMI_CTL message
--------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  qmi_ip_qmux_if_type_t if_type;
  qmi_ip_qmux_hdr_s     qmux_hdr;
  union __attribute__((packed)) {
    qmi_ip_qmux_qmi_ctl_sdu_s qmi_ctl;
    qmi_ip_qmux_sdu_s         qmux;
  } sdu;

  /*note Followed by msg_length bytes of TLV data, and potentially additional
    msg_id/msg_length/tlv_data sets of data */
} qmi_ip_qmux_msg_s;


/*IOCTL to communicate with embms tunneling device in kernel*/

#define ADD_EMBMS_TUNNEL_FOR_CLIENT _IOW(MAJOR_NUM, 0, qmi_ip_tun_mgmt_info_t)

#define DELETE_EMBMS_TUNNEL_FOR_CLIENT _IOW(MAJOR_NUM, 1, qmi_ip_tun_mgmt_info_t)

#define TMGI_DEACTIVATE _IOW(MAJOR_NUM, 2, qmi_ip_tun_mgmt_info_t)

#define CLIENT_DEACTIVATE _IOW(MAJOR_NUM, 3, qmi_ip_tun_mgmt_info_t)

#define GET_EMBMS_TUNNELING_STATUS _IO(MAJOR_NUM, 4)

#define START_EMBMS_TUNNELING _IOW(MAJOR_NUM, 5, qmi_ip_tun_mgmt_info_t)

#define STOP_EMBMS_TUNNELING _IO(MAJOR_NUM, 6)


/*-----------------------------------------------------------------------------
  Function definitions
-----------------------------------------------------------------------------*/
/*=============================================================================

FUNCTION QMI_IP_INIT_MASTER_CONN_SERVER()

DESCRIPTION
  This function initializes the master connection server which listens to the
  conenctions from LAN clients.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/
int qmi_ip_init_master_conn_server
(
  qmi_ip_conf_t *qmi_ip_cfg
);

/*=============================================================================

FUNCTION QMI_IP_MSG_HDLR_INIT()

DESCRIPTION
  This function initializes the QMI message handler

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/

int qmi_ip_msg_hdlr_init
(
  qmi_ip_conf_t *qmi_ip_config
);


/*===========================================================================

FUNCTION QMI_IP_PROCESS_CTRL_MSG()

DESCRIPTION

  This function processes the QMI messages received from the LAN clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE


SIDE EFFECTS
  None

=========================================================================*/

int qmi_ip_process_ctrl_msg
(
  int     active_fd,
  void    *data,
  int     data_len
);


/*===========================================================================

FUNCTION PRINT_BUFFER()

DESCRIPTION

  This function prints the QMI buffer. Used for debugging purposes.

DEPENDENCIES
  None.

RETURN VALUE
  none


SIDE EFFECTS
  None

=========================================================================*/
void print_buffer
(
  char    *buf,
  int      size
);

/*===========================================================================

FUNCTION QMI_IP_IND_HDLR_INIT()

DESCRIPTION

  This function initializes the socket used for handling indications.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=========================================================================*/
int qmi_ip_ind_hdlr_init();

/*==========================================================================

FUNCTION QMI_IP_TUN_MGMT_INIT()

DESCRIPTION

  This function initializes the tunneling management module of QMI_IP.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_tun_mgmt_init
(
  qmi_ip_conf_t *qmi_ip_cfg
);

/*==========================================================================

FUNCTION QMI_IP_GET_CLIENT_ID()

DESCRIPTION
  This function returns the client ID based on socket descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_get_client_id
(
  int sock_fd
);


/*==========================================================================

FUNCTION QMI_IP_ACT_TMGI_INST()

DESCRIPTION

  This function initializes the TMGI value in QMI IP TMGI DB.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_activate_tmgi_inst
(
  int                     sock_fd,
  uint32                  multicast_addr,
  uint16                  multicast_port,
  qmi_ip_embms_tmgi_t     *embms_tmgi
);

/*==========================================================================

FUNCTION QMI_IP_ADD_TMGI_INST_TUNNEL()

DESCRIPTION

  This function enables the TMGI instance in QMI IP TMGI DB.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_tunnel_addition
(
  int                      sock_fd,
  qmi_ip_embms_tmgi_t      *tmgi
);


/*==========================================================================

FUNCTION QMI_IP_DEACT_TMGI_INST()

DESCRIPTION

  This function marks the TMGI to be deactivated.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_deactivate_tmgi_inst
(
  int                     sock_fd,
  qmi_ip_embms_tmgi_t     *embms_tmgi
);

/*==========================================================================

FUNCTION QMI_IP_DEL_TMGI_INST_TUNNEL()

DESCRIPTION

  This function deletes the TMGI instance from QMI IP DB

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_tunnel_deletion
(
  int                      sock_fd,
  qmi_ip_embms_tmgi_t      *tmgi
);

/*===========================================================================

FUNCTION QMI_IP_XML_INIT()

DESCRIPTION

  This function
  - initializes config from XML

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/

int qmi_ip_xml_init
(
  qmi_ip_conf_t   * qmi_ip_cfg,
  char            * xml_file
);

/*===========================================================================

FUNCTION QMI_IP_QMI_CLIENT_INIT()

DESCRIPTION

  Initialize QMI clients

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE


SIDE EFFECTS
  None

=========================================================================*/
int qmi_ip_qmi_client_init();

/*===========================================================================

FUNCTION QMI_IP_QMI_CLIENT_RELEASE()

DESCRIPTION

  This function releases the QMI specific handles obtained by QMI_IP

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE


SIDE EFFECTS
  None

=========================================================================*/
int qmi_ip_qmi_client_release();

/*==========================================================================

FUNCTION QMI_IP_SEND_MSG_TUNNEL_MODULE()

DESCRIPTION

  Send message to tunneling module

DEPENDENCIES
  None.

RETURN VALUE
  0 on succes
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/

int qmi_ip_send_msg_tunnel_module
(
  qmi_ip_tun_mgmt_info_t  *tun_inst
);

/*==========================================================================

FUNCTION QMI_IP_NETMGR_RELEASE()

DESCRIPTION

  Releases the netmgr handle.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_netmgr_release();


/*==========================================================================

FUNCTION QMI_IP_GET_CLIENT_ID_BY_IP()

DESCRIPTION
  This function gets the client index given the client IP.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

=============================================================================*/
int qmi_ip_get_client_id_by_ip
(
  uint32    ip_addr
);

/*==========================================================================

FUNCTION QMI_IP_DELETE_CLIENT_TUNNEL()

DESCRIPTION

  Deletes the tunnels associated with the client.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure


SIDE EFFECTS
  None

==========================================================================*/
int qmi_ip_delete_client_tunnel
(
  int  sock_fd
);

#endif
