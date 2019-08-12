/*
 * Copyright (c) 2015-16 Qualcomm Atheros, Inc.
 *
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*
 * Copyright (c) 2014-2015 The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This file was originally distributed by Qualcomm Atheros, Inc.
 * under proprietary terms before Copyright ownership was assigned
 * to the Linux Foundation.
 */

#ifndef _CLD_DIAG_PARSER_H
#define _CLD_DIAG_PARSER_H

#define FEATURE_LOG_EXPOSED_HEADER

#include <stdint.h>
#ifndef __KERNEL__
#include <sys/socket.h>
#include <netinet/in.h>
#endif // __KERNEL__
#include "event.h"
#include "msg.h"
#include "log.h"
#include "diag_lsm.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "diag.h"
#ifdef USE_GLIB
#include <glib.h>
#define strlcat g_strlcat
#define strlcpy g_strlcpy
#endif
#include <athdefs.h>
#include <a_types.h>
#include "dbglog_common.h"
#include <inttypes.h>

#define INIT_WITH_SLEEP            10
#define CLD_NETLINK_USER           17

#define LOGFILE_FLAG               0x01
#define CONSOLE_FLAG               0x02
#define QXDM_FLAG                  0x04
#define SILENT_FLAG                0x08
#define DEBUG_FLAG                 0x10
#define BUFFER_SIZE_FLAG           0x20
#define HDRLEN                     16

#define RECLEN (HDRLEN + ATH6KL_FWLOG_PAYLOAD_SIZE)

#define DEFAULT_LOG_BUFFER_LIMIT             (32 * 1024)
#define WRITE_TO_FILE_DISABLED   0
#define WRITE_TO_INTERNAL_SDCARD 1
#define WRITE_TO_EXTERNAL_SDCARD 2
#define FLAG_VALUE_OFFSET        3
#define MAX_RETRY_COUNT          40
#define MAX_FILE_SIZE            ((30) * (1024) * (1024))
#define MAX_FILE_INDEX           2
#define MAX_FILENAME_SIZE        100
#define MAX_SIZE                 512
#define TIME_DATA_SIZE           100
#define PKT_BUF_SIZE             1024
#define i_isspace(ch)  (((ch) >= 0x09 && (ch) <= 0x0d) || (ch) == ' ')
#define HEXDUMP_PKT8(p, pkt) \
          snprintf(p, 25, "%02x %02x %02x %02x %02x %02x %02x %02x ", \
                          pkt[0], pkt[1], pkt[2], pkt[3], \
                          pkt[4], pkt[5], pkt[6], pkt[7]);
#define UNUSED(x)	(void)(x)

extern int max_file_size;
extern int optionflag;

static inline unsigned int get_32(const unsigned char *pos)
{
	return pos[0] | (pos[1] << 8) | (pos[2] << 16) | (pos[3] << 24);
}

/* General purpose MACROS to handle the WNI Netlink msgs */
#define ANI_NL_MASK        3

/*
 * The following enum defines the target kernel module for which the netlink
 * message is intended for. Each kernel module along with its counterpart
 * in the user space, will then define a set of messages they recognize.
 * Each of this message will have an header of type tAniHdr define below.
 * Each Netlink message to/from a kernel module will contain only one
 * message which is preceded by a tAniHdr.
 *
 *         +------------+-------+-------+----------+
 *         |Netlink hdr | Align |tAniHdr| msg body |
 *         +------------+-------+-------|----------+
 */
#define ANI_NL_MSG_BASE     0x10    /* Some arbitrary base */
typedef enum eAniNlModuleTypes {
	ANI_NL_MSG_NETSIM = ANI_NL_MSG_BASE,// NetSim Messages (to the server)
	ANI_NL_MSG_PUMAC,       // Messages for/from the Upper MAC driver
	ANI_NL_MSG_WNS,         // Messages for the Wireless Networking
	//  Services module(s)
	ANI_NL_MSG_MACSW,       // Messages from MAC
	ANI_NL_MSG_ES,          // Messages from ES
	ANI_NL_MSG_WSM,         // Message from the WSM in user space
	ANI_NL_MSG_DVT,         // Message from the DVT application
	ANI_NL_MSG_PTT,         // Message from the PTT application
	ANI_NL_MSG_MAC_CLONE,     //Message from the Mac clone App
	ANI_NL_MSG_LOG = ANI_NL_MSG_BASE + 0x0C, // Message for WLAN logging
	ANI_NL_MSG_MAX
} tAniNlModTypes;

enum fileType {
	HOST_LOG_FILE,
	FW_LOG_FILE
};


//All Netlink messages must contain this header
typedef struct sAniHdr {
	unsigned short type;
	unsigned short length;
} tAniHdr, tAniMsgHdr;

/*
 * This msg hdr will always follow tAniHdr in all the messages exchanged
 * between the Applications in userspace the Pseudo Driver, in either
 * direction.
 */
typedef struct sAniNlMsg {
	struct  nlmsghdr nlh;   // Netlink Header
	int radio;          // unit number of the radio
	tAniHdr wmsg;       // Airgo Message Header
} tAniNlHdr;

typedef struct sAniAppRegReq {
	tAniNlModTypes type;    /* The module id that the application is
			           registering for */
	int pid;            /* Pid returned in the nl_sockaddr structure
			    in the call getsockbyname after the
			    application opens and binds a netlink
			    socket */
} tAniNlAppRegReq;

static inline unsigned int aniNlAlign(unsigned int len)
{
	return ((len + ANI_NL_MASK) & ~ANI_NL_MASK);
}

/*
 * Determines the aligned length of the WNI MSG including the hdr
 * for a given payload of length 'len'.
 */
static inline unsigned int aniNlLen(unsigned int len)
{
	return  (aniNlAlign(sizeof(tAniHdr)) + len);
}

/* KERNEL DEFS END */

#ifdef CONFIG_ANDROID_LOG
#include <android/log.h>

#define FWDEBUG_LOG_NAME        "CNSS"
#define FWDEBUG_NAME            "CNSS_DEBUG"
#define android_printf(...) \
    __android_log_print(ANDROID_LOG_INFO, FWDEBUG_LOG_NAME, __VA_ARGS__);

#define debug_printf(...) do {                         \
	if (optionflag & DEBUG_FLAG)                  \
		__android_log_print(ANDROID_LOG_INFO,  \
			FWDEBUG_NAME, __VA_ARGS__);    \
} while(0)
#else
#define android_printf printf
#define debug_printf(...) do {} while(0);
#endif

#define WLAN_NL_MSG_CNSS_DIAG   27 /* Msg type between user space/wlan driver */
#define WLAN_NL_MSG_CNSS_HOST_MSG    28
#define WLAN_NL_MSG_CNSS_HOST_EVENT_LOG    17
#define CNSS_WLAN_DIAG          0x07
#define CNSS_WLAN_SSR_TYPE      0x01
#define CNSS_WLAN_LEVEL_TYPE    0x02
/* NL messgage Carries actual Logs from Driver */
#define ANI_NL_MSG_LOG_HOST_MSG_TYPE 89
#define ANI_NL_MSG_LOG_MGMT_MSG_TYPE 0x5B
#define ANI_NL_MSG_LOG_FW_MSG_TYPE 0x5C
#define ANI_NL_MSG_LOG_HOST_EVENT_LOG_TYPE 0x5050
/* NL message Registration Req/Response to and from Driver */
#define ANI_NL_MSG_LOG_REG_TYPE  0x0001
#define MAX_MSG_SIZE 50000
/* PKT SIZE for intf */
#define MAX_PKT_SIZE 8192

/* SPECIAL DIAG HANDLING */
#define DIAG_WLAN_MODULE_STA_PWRSAVE  34292
#define DIAG_WLAN_MODULE_WAL          42996
#define DIAG_NAN_MODULE_ID            56820
#define DIAG_WLAN_MODULE_IBSS_PWRSAVE 57332

#define RESTART_LEVEL     \
    "echo related > /sys/bus/msm_subsys/devices/subsys%d/restart_level"
#define DB_1_FILE_1_PATH        "/firmware/image/Data.msc"
#define DB_1_FILE_2_PATH        "/vendor/firmware/Data.msc"
#define BUF_SIZ  256
#define NUM_OF_DRIVERS  2
#define DB_2_FILE_1_PATH        "/lib/firmware/qca9377/Data.msc"

#define WLAN_LOG_TO_DIAG(xx_ss_id, xx_ss_mask, xx_fmt) \
do { \
	if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
		msg_const_type xx_msg = { \
		{__LINE__, (xx_ss_id), (xx_ss_mask)}, (NULL), msg_file}; \
		xx_msg.fmt = xx_fmt; \
		msg_send (&xx_msg); \
	} \
} while  (0); \

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(a)         (sizeof(a) / sizeof((a)[0]))
#endif

void *cnss_wlan_handle(void *req_pkt, uint16_t pkt_len);

static const diagpkt_user_table_entry_type cnss_wlan_tbl[] =
{ /* susbsys_cmd_code lo = 7 , susbsys_cmd_code hi = 7, call back function */
	{CNSS_WLAN_DIAG, CNSS_WLAN_DIAG,cnss_wlan_handle},
};

int32_t parser_init();

int32_t
dbglog_parse_debug_logs(u_int8_t *datap, u_int16_t len, u_int16_t dropped, uint32_t radio_id);

void
diag_initialize(int sock_fd, uint32_t optionflag);

void
process_diaghost_msg(uint8_t *datap, uint16_t len);

uint32_t
process_diagfw_msg(uint8_t *datap, uint16_t len, uint32_t optionflag,
    FILE *fw_log_fp, int32_t version, int sock_fd, uint32_t radio_id);

int
diag_msg_handler(uint32_t id, char *payload,  uint16_t vdevid, uint64_t timestamp, uint32_t radio_id);

int
cnssdiag_register_kernel_logging(int sock_fd, struct nlmsghdr *nlh);

void process_cnss_host_message(tAniNlHdr *wnl, int32_t optionflag,
    FILE *host_log_fp);
void process_pronto_firmware_logs(tAniNlHdr *wnl, int32_t optionflag,
    FILE *fw_log_fp);

void process_cnss_host_diag_events_log(char *pData, int32_t optionflag);
void backup_file(enum fileType);
void readDir(const char *dirName);
void cnss_open_log_file(int max_size_reached);
void printLocalTime(FILE *fp, const char *prefix, const char *suffix);
void appendLocalTime(char* tmbuf);
#endif

