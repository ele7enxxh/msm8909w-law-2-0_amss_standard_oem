/*==========================================================================

                     FTM WLAN Header File

Description
  The header file includes enums, struct definitions for WLAN FTM packets

# Copyright (c) 2010-2011, 2014 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================

                         Edit History


when       who       what, where, why
--------   ---       ----------------------------------------------------------
07/11/11   karthikm  Created header file to include enums, struct for WLAN FTM
                     for Atheros support
========================================================================*/

#ifndef  FTM_WLAN_H_
#define  FTM_WLAN_H_

#ifdef CONFIG_FTM_WLAN

#include "diagpkt.h"
#include <sys/types.h>

#define FTM_WLAN_CMD_CODE 22

extern char g_ifname[];

/* Various ERROR CODES supported by the FTM WLAN module*/
typedef enum {
    FTM_ERR_CODE_PASS = 0,
    FTM_ERR_CODE_IOCTL_FAIL,
    FTM_ERR_CODE_SOCK_FAIL,
    FTM_ERR_CODE_UNRECOG_FTM
}FTM_WLAN_LOAD_ERROR_CODES;


#define CONFIG_HOST_TCMD_SUPPORT  1
#define AR6000_IOCTL_SUPPORTED    1

#define ATH_MAC_LEN               6

typedef enum {
    FTM_WLAN_COMMON_OP,
    FTM_WLAN_BDF_GET_MAX_TRANSFER_SIZE,
    FTM_WLAN_BDF_READ,
    FTM_WLAN_BDF_WRITE,
    FTM_WLAN_BDF_GET_FNAMEPATH,
    FTM_WLAN_BDF_SET_FNAMEPATH
}FTM_WLAN_CMD;

typedef enum {
    WLAN_BDF_READ_SUCCESS,
    WLAN_BDF_READ_FAILED,
    WLAN_BDF_WRITE_SUCCESS,
    WLAN_BDF_WRITE_FAILED,
    WLAN_BDF_INVALID_SIZE = 5,
    WLAN_BDF_BAD_OFFSET,
    WLAN_BDF_FILE_OPEN_FAIL,
    WLAN_BDF_FILE_SEEK_FAIL,
    WLAN_BDF_FILE_STAT_FAIL,
    WLAN_BDF_PATH_GET_SUCCESS,
    WLAN_BDF_PATH_GET_FAILED,
    WLAN_BDF_PATH_SET_SUCCESS,
    WLAN_BDF_PATH_SET_FAILED
}FTM_WLAN_ERROR_CODES;

/*FTM WLAN request type*/
typedef PACKED struct
{
    diagpkt_cmd_code_type              cmd_code;
    diagpkt_subsys_id_type             subsys_id;
    diagpkt_subsys_cmd_code_type       subsys_cmd_code;
    uint16                             cmd_id; /* command id (required) */
    uint16                             cmd_data_len;
    uint16                             cmd_rsp_pkt_size;
    union {
        struct {
             uint16                    rsvd;
             byte                      rsvd1;
             byte                      rsvd2;
             byte                      rsvd3;
             byte                      wlandeviceno;
             byte                      data[0];
        }__attribute__((packed))common_ops;
        struct {
             byte                       rsvd[6];
             byte                       data[0];
        }__attribute__((packed))get_max_transfer_size;
        struct {
            uint32                     offset;
            byte                       rsvd[2];
            byte                       data[0];
        }__attribute__((packed))read_file;
        struct {
            uint16                     size;
            uint8                      append_flag;
            byte                       rsvd[3];
            byte                       data[0];
        }__attribute__((packed))write_file;
        struct {
            byte                       rsvd[6];
            byte                       data[0];
        }__attribute__((packed))get_fname;
        struct {
            byte                       rsvd[6];
            byte                       data[0];
        }__attribute__((packed))set_fname;
    }cmd;
}__attribute__((packed))ftm_wlan_req_pkt_type;

/*FTM WLAM response type */
typedef PACKED struct
{
    struct {
        diagpkt_subsys_header_type         header; /*diag header*/
        uint16                             cmd_id; /* command id (required) */
        uint16                             cmd_data_len;
        uint16                             cmd_rsp_pkt_size;
    }__attribute__((packed))common_header;
    union {
        struct {
            uint16                             rsvd;
            uint32                             result ;/* error_code */
            union {
                struct {
                    byte                              data[0]; /*rxReport*/
                }rxReport;
                struct {
                    byte                              data[0];  /*ThermValReport*/
                }thermval_report;
            }rx_and_therm;
        }__attribute__((packed))common_ops;
        struct {
            uint16                             result; /*error_code*/
            byte                               rsvd[4];
            uint16                             max_size;
        }__attribute__((packed))get_max_transfer_size;
        struct {
            byte                               result; /*error_code*/
            uint16                             size;
            byte                               bytes_remaining[3];
            byte                               data[0];
        }__attribute__((packed))read_file;
        struct {
            byte                               result;
            byte                               rsvd[5];
            byte                               data[0];
        }__attribute__((packed))write_file;
        struct {
            byte                              result;
            byte                              rsvd[5];
            byte                              data[0];
        }__attribute__((packed))get_fname;
        struct {
            byte                              result;
            byte                              rsvd[5];
            byte                              data[0];
        }__attribute__((packed))set_fname;
    }cmd;
}__attribute__((packed))ftm_wlan_rsp_pkt_type;


void* ftm_wlan_dispatch(ftm_wlan_req_pkt_type *wlan_ftm_pkt, int pkt_len);
#endif /* CONFIG_FTM_WLAN */
#endif /* FTM_WLAN_H_ */
