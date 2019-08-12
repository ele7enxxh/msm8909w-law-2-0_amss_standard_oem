#ifndef _EMBMS_TM_API_H_
#define _EMBMS_TM_API_H_

/******************************************************************************

                           EMBMS_TM_API.H

******************************************************************************/

/******************************************************************************

  @file    embms_tm_api.h
  @brief   eMBMs Tunneling Module Lib API

  DESCRIPTION
  Header file for eMBMs Tunneling Module Lib.

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/25/14   pm         Created file.
******************************************************************************/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stdbool.h>
#include "comdef.h"
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <linux/if_addr.h>
#include <netinet/ether.h>
#include <stdint.h>
#include <sys/stat.h>
#include "dsi_netctrl.h"
#include "ds_util.h"
#include "ds_list.h"

#define EMBMS_TM_CONTROL_FILE "/etc/embms_tm_control_file"
#define QMI_IP_CONTROL_FILE   "/etc/qmi_ip_control_file"
#define EMBMS_TM_MAX_IFACE__NAME    16
#define SOCKET_RCV_TIMEOUT_SEC      0
#define SOCKET_RCV_TIMEOUT_USEC     100000
#define MAX_CLIENTS                 5
#define DST_IP_START                16  //Byte number to be used to read dst addr from IPV4 header
#define DST_IP_END                  19
#define DST_PORT_START              22  //Byte number to be used to read dst port from UDP header
#define DST_PORT_END                23

typedef enum {
  EMBMS_TM_ADD_CLIENT_ENTRY = 0,
  EMBMS_TM_DELETE_CLIENT_ENTRY,
  EMBMS_TM_TMGI_DEACTIVATE,
  EMBMS_TM_CLIENT_ACTIVATE_ALL_TMGI,
  EMBMS_TM_CLIENT_DEACTIVATE_ALL_TMGI,
  EMBMS_TM_SESSION_DEACTIVATE,
  EMBMS_TM_SOCK_INFO
}embms_tm_qmi_ip_action_type;

typedef struct {
  uint32 multicast_addr;
  uint16 multicast_port;
  uint32 client_addr;
  uint16 client_port;
  uint16 data_port;
  embms_tm_qmi_ip_action_type action_type;
  char iface_name[EMBMS_TM_MAX_IFACE__NAME];
}embms_tmgi_to_client_info_update;

typedef struct embms_client_info_internal {
  uint32 addr;
  uint16 port;
  int packets_sent;
  embms_client_info_internal *next_client;
};

typedef struct embms_tmgi_to_client_info_internal {
  uint32 tmgi_multicast_addr;
  uint16 tmgi_port;
  embms_client_info_internal *client_info_head;
  embms_tmgi_to_client_info_internal *next_tmgi;
};



/*---------------------------------------------------------------------------
                    Return values indicating error status
---------------------------------------------------------------------------*/
#define EMBMS_TM_SUCCESS               0         /* Successful operation   */
#define EMBMS_TM_ERROR                -1         /* Unsuccessful operation */

/*---------------------------------------------------------------------------
           Error Condition Values
---------------------------------------------------------------------------*/
#define EMBMS_TM_ENOERROR              0        /* No error                */
#define EMBMS_TM_EWOULDBLOCK           1        /* Operation would block   */
#define EMBMS_TM_ENOMEM                2        /* Out of memory  */
#define EMBMS_TM_EBADPARAM             3        /* Incorrect parameters passed*/
#define EMBMS_TM_ENOEFFECT             4        /* No Effect*/


/*===========================================================================
LOG Msg Macros
=============================================================================*/
#define LOG_MSG_INFO1_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO2_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO3_LEVEL           MSG_LEGACY_LOW
#define LOG_MSG_ERROR_LEVEL           MSG_LEGACY_ERROR
#define PRINT_MSG( level, fmtString, x, y, z)                         \
        MSG_SPRINTF_4( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,      \
                       __FUNCTION__, x, y, z);
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
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_ERROR_LEVEL, fmtString, x, y, z);                \
}

#define IPV4_ADDR_MSG(ip_addr) MSG_4(MSG_SSID_DS, \
                        MSG_LEGACY_HIGH, \
                        "IPV4 Address is %d.%d.%d.%d", \
                        (unsigned char)(ip_addr), \
                        (unsigned char)(ip_addr >> 8), \
                        (unsigned char)(ip_addr >> 16) , \
                        (unsigned char)(ip_addr >> 24))

#endif
