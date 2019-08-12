/*==========================================================================
Description
  Wcnss_filter provides mux/demux of Bluetooth and ANT data sent over the same
  UART channel

# Copyright (c) 2013-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*
 * wcnss_filter code has three processing threads,

 * Main/Transport_read_thread: This thread would open the UART channel and select
 * for the data/events coming over UART port. Whenever there is a data available
 * from the controller, this thread would read the protocol byte and route the data
 * to available client handles (either Bluetooth or ANT client sockets)

 * Bluetooth client thread: This thread create server socket (UNIX domain socket)
 * for Bluetooth client and wait for the incoming connection from Bluetooth stack.
 * Upon valid client connection, the thread would start selecting on Bluetooth
 * client socket,so that commands/data coming from Bluetooth stack/Host would be
 * read and passed to UART transport. These threads have the logic of closing of
 * their ends as part of client closure and start waiting for the connection for
 * next client connection

 * ANT client thread: This thread create server socket (UNIX domain socket) for ANT
 * client and wait for the incoming connection from ANT stack. Upon valid client
 * connection, the thread would start selecting on ANT client socket, so that
 * commands/data coming from ANT stack/Host would be read and passed to UART
 * transport.  These threads have the logic of closing of their ends as part of
 * client closure and start waiting for the connection for next client connection.

 * Writes to UART port is guarded with a mutex so that writes from different
 * clients would be synchronized as required.
**/

#include <cutils/log.h>
#include <sys/socket.h>
#include <cutils/sockets.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/un.h>
#include <cutils/properties.h>
#include "wcnss_ibs.h"
#include "bt_qxdmlog.h"
#include "private/android_filesystem_config.h"

#define LOG_TAG "WCNSS_FILTER"

#define BT_SOCK "bt_sock"
#define ANT_SOCK "ant_sock"
#define BT_HS_UART_DEVICE "/dev/ttyHS0"
#define PROC_PANIC_PATH     "/proc/sysrq-trigger"
#define UART_IPC_LOG_PATH     "/sys/module/msm_serial_hs/parameters/debug_mask"

#define BT_SSR_TRIGGERED 0xee

#define BT_CMD_PACKET_TYPE 0x01
#define BT_ACL_PACKET_TYPE 0x02
#define BT_SCO_PACKET_TYPE 0x03
#define BT_EVT_PACKET_TYPE 0x04
#define ANT_CTL_PACKET_TYPE 0x0c
#define ANT_DATA_PACKET_TYPE 0x0e

#define MAX_BT_HDR_SIZE 4

#define BT_ACL_HDR_SIZE 4
#define BT_SCO_HDR_SIZE 3
#define BT_EVT_HDR_SIZE 2
#define BT_CMD_HDR_SIZE 3

#define BT_ACL_HDR_LEN_OFFSET 2
#define BT_SCO_HDR_LEN_OFFSET 2
#define BT_EVT_HDR_LEN_OFFSET 1
#define BT_CMD_HDR_LEN_OFFSET 2

#define ANT_CMD_HDR_SIZE 2
#define ANT_HDR_OFFSET_LEN 1

#ifndef BLUETOOTH_UID
#define BLUETOOTH_UID 1002
#endif
#ifndef SYSTEM_UID
#define SYSTEM_UID 1000
#endif

#ifndef ROOT_UID
#define ROOT_UID 0
#endif

#ifndef AID_USER
#define AID_USER 100000
#endif

#ifndef AID_APP
#define AID_APP 10000
#endif

#define HOST_TO_SOC 0
#define SOC_TO_HOST 1

pthread_mutex_t signal_mutex;

int remote_bt_fd;
int remote_ant_fd;
int fd_transport;
#ifdef PANIC_ON_SOC_CRASH
int fd_sysrq = -1;
#endif//PANIC_ON_SOC_CRASH
static pthread_t bt_mon_thread;
static pthread_t ant_mon_thread;
int copy_bt_data_to_channel(int src_fd, int dest_fd, unsigned char protocol_byte,int dir);
int copy_ant_host_data_to_soc(int src_fd, int dest_fd, unsigned char protocol_byte);
void ibs_recv_ibs_cmd(uint8_t *data);
int do_write(int fd, unsigned char *buf,int len);
bool enable_diag = false ;
static void handle_cleanup();
static void cleanup_and_suicide();
extern int bt_ssr_level ();
bool send_special_bytes = false ;
unsigned char reset_cmpl[] = {0x04, 0x0e, 0x04, 0x01,0x03, 0x0c, 0x00};

RING_BUF g_out_ringbuf;
RING_BUF g_out_ringbuf_swap;
RING_BUF g_in_ringbuf;
RING_BUF g_in_ringbuf_swap;

bool g_in_ringbuf_enabled = false;
bool g_out_ringbuf_enabled = false;
bool g_force_special_byte_enabled = false;


void set_debug_properties() {
    int ret =0;
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    bool enabled = false;

    ret = property_get("wc_transport.force_special_byte", value, NULL);
    ALOGV("%s: property_get: wc_transport.force_special_byte: %d", __func__, ret);

    if (ret) {
        enabled = (strcmp(value, "false") ==0) ? false : true;
        ALOGV("%s: wc_transport.force_special_byte: %s, enabled: %d ",
            __func__, value, enabled);
    }
    g_force_special_byte_enabled = enabled;

    enabled = false;
    ret = property_get("wc_transport.out_ringbuf_log", value, NULL);
    ALOGV("%s: property_get: wc_transport.ringbuf: %d", __func__, ret);

    if (ret) {
        enabled = (strcmp(value, "false") ==0) ? false : true;
        ALOGV("%s: wc_transport.out_ringbuf_log: %s, enabled: %d ",
            __func__, value, enabled);
    }

    g_out_ringbuf_enabled = enabled;

    enabled = false;
    ret = property_get("wc_transport.in_ringbuf_log", value, NULL);
    ALOGV("%s: property_get: wc_transport.ringbuf: %d", __func__, ret);

    if (ret) {
        enabled = (strcmp(value, "false") ==0) ? false : true;
        ALOGV("%s: wc_transport.in_ringbuf_log: %s, enabled: %d ",
            __func__, value, enabled);
    }

    g_in_ringbuf_enabled = enabled;
}

bool is_debug_force_special_bytes() {
    return g_force_special_byte_enabled;
}

bool is_debug_out_ring_buf_enabled() {
    return g_out_ringbuf_enabled;
}

bool is_debug_in_ring_buf_enabled() {
    return g_in_ringbuf_enabled;
}
void reset_ring_buffer (RING_BUF *rb) {
    ALOGV(">%s:", __func__);
    if (rb == NULL) {
        ALOGE("%s: invalid input bufs",__func__);
        return;
    }

    memset (rb->ring_buf, '0', MAX_RING_BUF_SIZE);
    rb->wPtr = &rb->ring_buf[0];
    rb->size = 0;
    ALOGV("%s: Done", __func__);
}

void update_ring_buffer (RING_BUF *rb, unsigned char* buf, int len, RING_BUF *swapb)
{
    ALOGV(">%s: mainbuf: %x, swapb: %x", __func__, rb, swapb);
    if (rb == NULL || buf == NULL) {
        ALOGE("%s: invalid input bufs");
        return;
    }

    if ((rb->size + len) >= MAX_RING_BUF_SIZE) {
        //backup main buffer
        memcpy(swapb->ring_buf, rb->ring_buf, rb->size);
        swapb->size = rb->size;
        reset_ring_buffer(rb);
    }

    ALOGV("%s: b4: buf: %x wPtr: %x size: %d, len : %d", __func__, rb, rb->wPtr, rb->size, len);
    ALOGV("dumping the bytes");
    memcpy (rb->wPtr, buf, len);
    rb->size += len;
    rb->wPtr = &rb->ring_buf[rb->size];

    ALOGV("%s: buf: %x, wPtr: %x size: %d, len : %d", __func__, rb, rb->wPtr, rb->size, len);

    ALOGV("<%s:", __func__);
}

void commit_out_ring_buffer () {
    unsigned char dpath[255];
    unsigned char swap_dpath[255];
    int dfd, ret;
    int swap_dfd;
    int inx_scan;
    ALOGV(">%s", __func__);
    /* Scan for uart ipc dump file */
    for (inx_scan= 0; inx_scan<255;inx_scan++)
    {
       snprintf(dpath, 255, "/data/misc/bluetooth/out_ringbuf_logs-%.02d.txt",
                                inx_scan);
       ALOGI("%s: try to open file : %s", __func__, dpath);
       dfd = open(dpath, O_RDONLY);
       if(dfd < 0 && (errno == ENOENT) ) {
           ALOGI("%s: file( %s) can be used for out_ringbuf ipc", __func__, dpath);
            break;
       }
       close(dfd);
    }

    ALOGI("%s: Dumping out ring buffer in %s", __func__, dpath);

    dfd = open(dpath, O_CREAT| O_SYNC | O_WRONLY,  S_IRUSR |S_IWUSR |S_IRGRP );
    if (dfd < 0) {
       ALOGE("%s: Error opening destination file %d (%s)", __func__, dfd, strerror(errno));
       return;
    }

    ret = write (dfd, g_out_ringbuf_swap.ring_buf, g_out_ringbuf_swap.size);
    if (ret <= 0) {
         ALOGE("%s: Error writing swap buffer to d file: %d (%s)", __func__, ret, strerror(errno));
    }

    ALOGV("%s: Success writing swap buffer to file: %d (%s)", __func__, ret, strerror(errno));
    ret = write (dfd, g_out_ringbuf.ring_buf, g_out_ringbuf.size);
    if (ret <= 0) {
        ALOGE("%s: Error writing main buffer to d file: %d (%s)", __func__, ret, strerror(errno));
    }
    ALOGV("%s: Success writing main buffer to dfile: %d (%s)", __func__, ret, strerror(errno));

    close (dfd);

    //Reset buffers
    reset_ring_buffer(&g_out_ringbuf);
    reset_ring_buffer(&g_out_ringbuf_swap);
    ALOGV("<%s", __func__);
}

void commit_in_ring_buffer () {
    unsigned char dpath[255];
    int dfd, ret;
    int inx_scan;
    ALOGV(">%s", __func__);
    /* Scan for uart ipc dump file */
    for (inx_scan= 0; inx_scan<255;inx_scan++)
    {
         snprintf(dpath, 255, "/data/misc/bluetooth/in_ringbuf_logs-%.02d.txt",
                                inx_scan);
         ALOGV("%s: try to open file : %s", __func__, dpath);
         dfd = open(dpath, O_RDONLY);
         if(dfd < 0 && (errno == ENOENT) ) {
             ALOGV("%s: file( %s) can be used for in_ringbuf ipc", __func__, dpath);
             break;
         }
         close(dfd);
    }

    ALOGI("%s: Dumping in ring buffer in %s", __func__, dpath);

    dfd = open(dpath, O_CREAT| O_SYNC | O_WRONLY,  S_IRUSR |S_IWUSR |S_IRGRP );
    if (dfd < 0) {
        ALOGE("%s: Error opening destination file %d (%s)", __func__, dfd, strerror(errno) );
        return;
    }

    ret = write (dfd, g_in_ringbuf_swap.ring_buf, g_in_ringbuf_swap.size);
    if (ret <= 0) {
         ALOGE("%s: Error writing swap buffer to file: %d (%s)", __func__, ret, strerror(errno));
    }
    ALOGV("%s: Success writing swap buffer to file: %d (%s)", __func__, ret, strerror(errno));

    ret = write (dfd, g_in_ringbuf.ring_buf, g_in_ringbuf.size);
    if (ret <= 0) {
        ALOGE("%s: Error writing main buffer to file: %d (%s)", __func__, ret, strerror(errno));
    }
    ALOGV("%s: Success writing main buffer to file: %d (%s)", __func__, ret, strerror(errno));

    close (dfd);

    //Reset buffers
    reset_ring_buffer(&g_in_ringbuf);
    reset_ring_buffer(&g_in_ringbuf_swap);
    ALOGV("<%s", __func__);
}

static int extract_uid(int uuid)
{
    int userid;
    int appid;

    appid = userid =  uuid % AID_USER;
    if (userid > BLUETOOTH_UID)
    {
        appid = userid % AID_APP;
    }
    ALOGD("%s appid = %d",__func__,appid);
    return appid;
}

static int establish_remote_socket(char *name)
{
    int fd = -1;
    struct sockaddr_un client_address;
    socklen_t clen;
    int sock_id, ret;
    struct ucred creds;
    int c_uid;
    ALOGV("%s(%s) Entry  ", __func__, name);

    sock_id = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sock_id < 0) {
        ALOGE("%s: server Socket creation failure", __func__);
        return fd;
    }

    ALOGV("convert name to android abstract name:%s %d", name, sock_id);
    if (socket_local_server_bind(sock_id,
        name, ANDROID_SOCKET_NAMESPACE_ABSTRACT) >= 0) {
        if (listen(sock_id, 5) == 0) {
            ALOGV("listen to local socket:%s, fd:%d", name, sock_id);
        } else {
            ALOGE("listen to local socket:failed");
            close(sock_id);
            return fd;
        }
    } else {
        close(sock_id);
        ALOGE("%s: server bind failed for socket : %s", __func__, name);
        return fd;
    }

    clen = sizeof(client_address);
    ALOGV("%s: before accept_server_socket", name);
    fd = accept(sock_id, (struct sockaddr *)&client_address, &clen);
    if (fd > 0) {
        ALOGV("%s accepted fd:%d for server fd:%d", name, fd, sock_id);
        close(sock_id);

        memset(&creds, 0, sizeof(creds));
        socklen_t szCreds = sizeof(creds);
        ret = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &creds, &szCreds);
        if (ret < 0) {
            ALOGE("%s: error getting remote socket creds: %d\n", __func__, ret);
            close(fd);
            return -1;
        }
        c_uid = creds.uid;
        if (c_uid > BLUETOOTH_UID)
            c_uid = extract_uid(creds.uid);
        if (c_uid != BLUETOOTH_UID && c_uid != SYSTEM_UID
                && c_uid != ROOT_UID) {
            ALOGE("%s: client doesn't have required credentials", __func__);
            ALOGE("<%s req> client uid: %d", name, creds.uid);
            close(fd);
            return -1;
        }

        ALOGV("%s: Remote socket credentials: %d\n", __func__, creds.uid);
        return fd;
    } else {
        ALOGE("BTC accept failed fd:%d sock d:%d error %s", fd, sock_id, strerror(errno));
        close(sock_id);
        return fd;
    }

    close(sock_id);
    return fd;
}

void dump_buf(unsigned char* buf, int len) {
    int i;
    for (i=0; i<len; i++)
        ALOGV("%x", buf[i]);
}

#ifdef DEBUG_MIMIC_CMD_TOUT
/* This mimics the CMD TOUT by dropping the
 * "set Local name" command at filter
 * user can repro the cmd tout by setting the name
 * couple of times from UI
 */
int command_is_change_lname(unsigned char* buf, int len) {
    int val =1, i;
    unsigned char lname[] = {0x01, 0x13, 0x0c};
    ALOGE("%s:", __func__);
    for (i=0; i<3; i++) {
       if(buf[i] != lname[i]) val = 0;
    }
    return val;
}
#endif //DEBUG_MIMIC_CMD_TOUT

#ifdef IGNORE_HCI_RESET
/**
  * this is to ignore HCI_RESET coming from stack.
  * as performing HCI_RESET when ANT is on is not recomended.
  * HCI_RESET only executed once from libbt-vendor as part of
  * chip and NVM initialization
  */
int command_is_reset(unsigned char* buf, int len) {
    int val = 1, i;
    unsigned char reset[] = {0x01, 0x03, 0x0c, 0x00};
    ALOGE("%s:", __func__);
    for (i=0; i<len; i++) {
       if(buf[i] != reset[i]) val = 0;
    }
    return val;
}
#endif//IGNORE_HCI_RESET


timer_t g_soc_crash_wait_timer;
int g_soc_crash_wait_timer_state = TIMER_NOT_CREATED;
bool  g_is_clock_on = false;

static void soc_crash_wait_timer_start()
{
    int status;
    struct itimerspec ts;
    struct sigevent se;
    uint32_t timeout_ms;
    ALOGV("%s",__FUNCTION__);
    if(g_soc_crash_wait_timer_state == TIMER_NOT_CREATED)
    {
        se.sigev_notify_function = report_soc_failure;
        se.sigev_notify = SIGEV_THREAD;
        se.sigev_value.sival_ptr = &g_soc_crash_wait_timer;
        se.sigev_notify_attributes = NULL;

        status = timer_create(CLOCK_MONOTONIC, &se, &g_soc_crash_wait_timer);
        if (status == 0)
        {
            ALOGV("%s: SoC Crash wait timer created",__FUNCTION__);
            g_soc_crash_wait_timer_state = TIMER_CREATED;
        } else {
            ALOGE("%s: Error creating timer %d\n", __func__, status);
        }
    }
    if(g_soc_crash_wait_timer_state == TIMER_CREATED)
    {
        timeout_ms = 100;
        ts.it_value.tv_sec = timeout_ms/1000;
        ts.it_value.tv_nsec = 1000000*(timeout_ms%1000);
        ts.it_interval.tv_sec = 0;
        ts.it_interval.tv_nsec = 0;

        status = timer_settime(g_soc_crash_wait_timer, 0, &ts, 0);
        if (status < 0)
            ALOGE("%s:Failed to set soc Crash wait timer: %d",__FUNCTION__, status);
        else {
            ALOGV("%s: time started", __func__);
            g_soc_crash_wait_timer_state = TIMER_ACTIVE;
        }
    }
}

bool soc_crash_wait_timer_stop()
{
    int status;
    struct itimerspec ts;
    struct sigevent se;
    uint32_t timeout_ms;
    bool was_active = false;
    ALOGV("%s",__FUNCTION__);

    if(g_soc_crash_wait_timer_state == TIMER_ACTIVE)
    {
        ts.it_value.tv_sec = 0;
        ts.it_value.tv_nsec = 0;
        ts.it_interval.tv_sec = 0;
        ts.it_interval.tv_nsec = 0;
        was_active = true;
        status = timer_settime(g_soc_crash_wait_timer, 0, &ts, 0);
        if (status == -1)
            ALOGE("%s:Failed to stop soc Crash wait timer",__FUNCTION__);
        else if (status == 0) {
             ALOGV("%s: Soc crash timer Stopped",__FUNCTION__);
             g_soc_crash_wait_timer_state = TIMER_CREATED;
        }
    }
    return was_active;
}

void soc_crash_wait_timer_cleanup() {
   if (g_soc_crash_wait_timer_state == TIMER_ACTIVE)
       soc_crash_wait_timer_stop();
   if (g_soc_crash_wait_timer_state == TIMER_CREATED) {
        timer_delete(g_soc_crash_wait_timer);
        g_soc_crash_wait_timer_state = TIMER_NOT_CREATED;
        g_soc_crash_wait_timer = NULL;
   }
}

void send_special_buffer() {
    unsigned char buf[1100];
    int ret;
    int retval;

    ALOGI(">%s: ", __func__);
    if (is_debug_force_special_bytes()) {
        memset (buf, 0xfb, 1100);
#ifdef WCNSS_IBS_ENABLED
        ALOGV("%s: Assert BT SOC",__FUNCTION__);
        retval = wcnss_wake_assert();
        if(retval == -1)
        {
            ALOGE("%s:Failed to assert SoC",__func__);
            wcnss_vote_on_clk();
            g_is_clock_on = true;
        }
#endif
        ret = do_write(fd_transport, buf, 1100);
        if (ret < 0) {
            ALOGE("%s: Write Err : %d (%s)", ret, __func__, ret, strerror(errno));
            wcnss_vote_off_clk();
            g_is_clock_on = false;
            report_soc_failure();
            return;
        } else if (ret < 1024) {
            ALOGE("%s: able to write only %x special bytes", __func__, ret);
            send_special_bytes = true ;
        }

        soc_crash_wait_timer_start();
    }
    ALOGV("<%s: ", __func__);
}

int handle_command_writes(int fd) {
    ALOGV("%s: ", __func__);
    unsigned char first_byte;
    int retval;
    int ssrlevel;

    retval = read (fd, &first_byte, 1);
    if (retval < 0) {
        ALOGE("%s:read returns err: %d\n", __func__,retval);
        return -1;
    }

    if (retval == 0) {
        ALOGE("%s: This indicates the close of other end", __func__);
        return -99;
    }

    ALOGV("%s: Protocol_byte: %x", __func__, first_byte);
    switch(first_byte) {
        case ANT_CTL_PACKET_TYPE:
        case ANT_DATA_PACKET_TYPE:
            ALOGV("%s: Ant data", __func__);
            retval = copy_ant_host_data_to_soc(fd, fd_transport, first_byte);
            ALOGV("%s: copy_ant_data_to_channel returns %d", __func__, retval);
            break;
        case BT_EVT_PACKET_TYPE:
        case BT_ACL_PACKET_TYPE:
        case BT_CMD_PACKET_TYPE:
            ALOGV("%s: BT data", __func__);
            retval = copy_bt_data_to_channel(fd, fd_transport, first_byte,HOST_TO_SOC);
            break;
        case BT_SSR_TRIGGERED:
            ALOGV("It is SSR triggered from command tout");
            send_special_buffer();
            dump_uart_ipc_logs();
            ssrlevel = bt_ssr_level();
            if (ssrlevel == 1 || ssrlevel == 2) {
            //    bt_kernel_panic();
            }
            break;
        default:
            ALOGE("%s: Unexpected data format!!",__func__);
            retval = -1;
    }

    ALOGV("%s: retval %d", __func__, 0);
    return 0;
}

static int bt_thread() {
    fd_set client_fds;
    int retval, n;

    ALOGV("%s: Entry ", __func__);
    do {
        remote_bt_fd = establish_remote_socket(BT_SOCK);

        if (remote_bt_fd < 0) {
            ALOGE("%s: invalid remote socket", __func__);
            return -1;
        }

        FD_ZERO(&client_fds);
        FD_SET(remote_bt_fd, &client_fds);

        do {
            ALOGV("%s: Back in BT select loop", __func__);
            n = select(remote_bt_fd+1, &client_fds, NULL, NULL, NULL);
            if(n < 0){
                ALOGE("Select: failed: %s", strerror(errno));
                break;
            }
            ALOGV("%s: select came out\n", __func__);
            if (FD_ISSET(remote_bt_fd, &client_fds)) {
                retval = handle_command_writes(remote_bt_fd);
                ALOGV("%s: handle_command_writes . %d", __func__, retval);
#ifdef WCNSS_IBS_ENABLED
                ALOGV("%s: tx complete",__FUNCTION__);
                wcnss_tx_done(true);
#endif
               if(retval < 0) {
                   if (retval == -99) {
                       ALOGV("%s:End of wait loop", __func__);
                   }
                   ALOGV("%s: handle_command_writes returns: %d: ", __func__, retval);
                   break;
                }
            }
        } while(1);

        ALOGV("%s: Bluetooth turned off", __func__);
        close(remote_bt_fd);
        remote_bt_fd = 0;
        if(!send_special_bytes)
            handle_cleanup();
    } while(1);

    pthread_exit(NULL);
    return 0;
}

static int ant_thread() {
    fd_set client_fds;
    int retval, n;

    ALOGV("%s: Entry ", __func__);
    do {
        remote_ant_fd = establish_remote_socket(ANT_SOCK);
        if (remote_ant_fd < 0) {
            ALOGE("%s: invalid remote socket", __func__);
            return -1;
        }

        FD_ZERO(&client_fds);
        FD_SET(remote_ant_fd, &client_fds);
        do {
            ALOGV("%s: Back in ANT select loop", __func__);
            n = select(remote_ant_fd+1, &client_fds, &client_fds, NULL, NULL);
            if(n < 0){
                ALOGE("Select: failed: %s", strerror(errno));
                break;
            }
            ALOGV("%s: select cameout out: (%s)\n", __func__, strerror(errno));
            if (FD_ISSET(remote_ant_fd, &client_fds)) {
                retval = handle_command_writes(remote_ant_fd);
                if(retval < 0) {
                   if (retval == -99) {
                       ALOGV("%s:End of wait loop", __func__);
                   }
                   ALOGV("%s: handle_command_writes returns: %d: ", __func__, retval);
                   break;
                }
            }
            ALOGV("%s: moving back to Select loop", __func__);
#ifdef WCNSS_IBS_ENABLED
            ALOGV("%s: tx complete",__FUNCTION__);
            wcnss_tx_done(true);
#endif

        } while(1);

        ALOGV("%s: ANT turned off", __func__);
        close(remote_ant_fd);
        remote_ant_fd = 0;
        if(!send_special_bytes)
            handle_cleanup();
    } while(1);

    pthread_exit(NULL);
    return 0;
}

static int init_transport() {
    struct termios   term;
    uint32_t baud = B3000000;
    uint8_t stop_bits = 0;

    ALOGV("%s: Entry ", __func__);

    if ((fd_transport = open(BT_HS_UART_DEVICE, O_RDWR)) == -1)
    {
        ALOGE("%s: Unable to open %s: %d (%s)", __func__, BT_HS_UART_DEVICE,
           fd_transport, strerror(errno));
        return -1;
    }

    if (tcflush(fd_transport, TCIOFLUSH) < 0)
    {
        ALOGE("issue while tcflush %s", BT_HS_UART_DEVICE);
        close(fd_transport);
        return -1;
    }

    if (tcgetattr(fd_transport, &term) < 0)
    {
        ALOGE("issue while tcgetattr %s", BT_HS_UART_DEVICE);
        close(fd_transport);
        return -1;
    }

    cfmakeraw(&term);
    /* Set RTS/CTS HW Flow Control*/
    term.c_cflag |= (CRTSCTS | stop_bits);

    if (tcsetattr(fd_transport, TCSANOW, &term) < 0)
    {
       ALOGE("issue while tcsetattr %s", BT_HS_UART_DEVICE);
       close(fd_transport);
       return -1;
    }

    if (tcflush(fd_transport, TCIOFLUSH) < 0)
    {
        ALOGE("after enabling flags issue while tcflush %s", BT_HS_UART_DEVICE);
        close(fd_transport);
        return -1;
    }

    if (tcsetattr(fd_transport, TCSANOW, &term) < 0)
    {
       ALOGE("issue while tcsetattr %s", BT_HS_UART_DEVICE);
       close(fd_transport);
       return -1;
    }

    if (tcflush(fd_transport, TCIOFLUSH) < 0)
    {
        ALOGE("after enabling flags issue while tcflush %s", BT_HS_UART_DEVICE);
        close(fd_transport);
        return -1;
    }

    /* set input/output baudrate */
    cfsetospeed(&term, baud);
    cfsetispeed(&term, baud);
    tcsetattr(fd_transport, TCSANOW, &term);
#ifdef WCNSS_IBS_ENABLED
    wcnss_ibs_init(fd_transport);
#endif
    ALOGV("%s returns fd: %d", __func__, fd_transport);
    return fd_transport;
}

bool isIBSByte(unsigned char byte) {
    ALOGV("%s>", __func__);
    if (byte == IBS_WAKE_ACK || byte == IBS_WAKE_IND || byte == IBS_SLEEP_IND) {
        return true;
    }
    return false;
}

int do_read(int fd, unsigned char* buf, size_t len) {
   int bytes_left, bytes_read = 0, read_offset;

   bytes_left = len;
   read_offset = 0;

   do {
       bytes_read = read(fd, buf+read_offset, bytes_left);
       if (bytes_read < 0) {
           ALOGE("%s: Read error: %d (%s)", __func__, bytes_left, strerror(errno));
           return -1;
       } else if (bytes_read == 0) {
            ALOGE("%s: read returned 0, err = %s, read bytes: %d, expected: %d",
                              __func__, strerror(errno), (len-bytes_left), len);
            return (len-bytes_left);
       }
       else {
           if (bytes_read < bytes_left) {
              ALOGV("Still there are %d bytes to read", bytes_left-bytes_read);
              bytes_left = bytes_left-bytes_read;
              read_offset = read_offset+bytes_read;
           } else {
              ALOGV("%s: done with read",__func__);
              break;
           }
       }
   }while(1);
   return len;
}

int do_write(int fd, unsigned char *buf,int len)
{
    int ret = 0;
    int write_offset = 0;
    int write_len = len;
    do {
        ret = write(fd, buf+write_offset, write_len);
        if (ret < 0)
        {
            ALOGE("%s: write failed ret = %d err = %s",__func__,ret,strerror(errno));
            return -1;

        } else if (ret == 0) {
            ALOGE("%s: Write returned 0, err = %s, Written bytes: %d, expected: %d",
                        __func__, strerror(errno), (len-write_len), len);
            return (len-write_len);

        } else {
            if (ret < write_len)
            {
               ALOGD("%s, Write pending,do write ret = %d err = %s",__func__,ret,
                       strerror(errno));
               write_len = write_len - ret;
               write_offset = ret;
            } else if (ret > write_len) {
               ALOGE("%s: FATAL wrote more than expected: written bytes: %d expected: %d",
                      __func__, write_len, ret);
               break;
            } else {
               ALOGV("Write successful");
               break;
            }
        }
    } while(1);
    return len;
}

int copy_bt_data_to_channel(int src_fd, int dest_fd, unsigned char protocol_byte,int direction) {
    unsigned char len;
    unsigned short acl_len;
    unsigned char* buf;
    unsigned char hdr[MAX_BT_HDR_SIZE];
    bool no_valid_client = false;
    int retval, i;

    ALOGV("%s: Entry.. proto byte : %d\n", __func__, protocol_byte);
    if (dest_fd == 0) {
        ALOGE("%s: No valid BT client connection", __func__);
        /*Discard the packet and keep the read loop alive*/
        no_valid_client = true;
    }

    switch(protocol_byte) {
       case BT_ACL_PACKET_TYPE:
           retval = do_read(src_fd, hdr, BT_ACL_HDR_SIZE);
           if (retval < 0) {
               ALOGE("%s:error in reading hdr: %d",__func__, retval);
               return -1;
           }

           /*ACL data len in two bytes in length*/
           acl_len = *((unsigned short*)&hdr[BT_ACL_HDR_LEN_OFFSET]);
           ALOGV("acl_len: %d\n", acl_len);
           buf = (unsigned char*)calloc(acl_len+BT_ACL_HDR_SIZE+1, sizeof(char));
           if (buf == NULL) {
               ALOGE("%s:alloc error", __func__);
               return -2;
           }
           buf[0] = protocol_byte;
           memcpy(buf+1, hdr, BT_ACL_HDR_SIZE);

           retval = do_read(src_fd, buf+1+BT_ACL_HDR_SIZE, acl_len);
           if (retval < 0) {
               ALOGE("%s:error in reading buf: %d", __func__, retval);
               retval = -1;
               free(buf);
               return retval;
           }
           acl_len = acl_len+BT_ACL_HDR_SIZE+1;
           break;

       case BT_SCO_PACKET_TYPE:
       case BT_CMD_PACKET_TYPE:
           retval = do_read(src_fd, hdr, BT_SCO_HDR_SIZE);
           if (retval < 0) {
               ALOGE("%s:error in reading hdr: %d",__func__, retval);
               return -1;
           }

           len = hdr[BT_SCO_HDR_LEN_OFFSET];
           ALOGV("len: %d\n", len);
           buf = (unsigned char*)calloc(len+BT_SCO_HDR_SIZE+1, sizeof(char));
           if (buf == NULL) {
               ALOGE("%s:alloc error", __func__);
               return -2;
           }

           buf[0] = protocol_byte;
           memcpy(buf+1, hdr, BT_SCO_HDR_SIZE);
           retval = do_read(src_fd, buf+BT_SCO_HDR_SIZE+1, len);
           if (retval < 0) {
               ALOGE("%s:error in reading buf: %d", __func__, retval);
               retval = -1;
               free(buf);
               return retval;
           }
           acl_len = len+BT_SCO_HDR_SIZE+1;
           break;

       case BT_EVT_PACKET_TYPE:
           retval = do_read(src_fd, hdr, BT_EVT_HDR_SIZE);
           if (retval < 0) {
               ALOGE("%s:error in reading hdr: %d", __func__, retval);
               return -1;
           }
#ifdef WCNSS_IBS_ENABLED
            if(hdr[0] == 0x13)
            {
                ALOGV("completed packet event: device can sleep\n");
//                wcnss_device_can_sleep();
            }
#endif
           len = hdr[BT_EVT_HDR_LEN_OFFSET];
           ALOGV("len: %d\n", len);
           buf = (unsigned char*)calloc(len+BT_EVT_HDR_SIZE+1, sizeof(char));
           if (buf == NULL) {
               ALOGE("%s:alloc error" ,__func__);
               return -2;
           }

           buf[0] = protocol_byte;
           memcpy(buf+1, hdr, BT_EVT_HDR_SIZE);
           retval = do_read(src_fd, buf+BT_EVT_HDR_SIZE+1, len);
           if (retval < 0) {
               ALOGE("%s:error in reading buf: %d", __func__, retval);
               retval = -1;
               free(buf);
               return retval;
           }
           acl_len = len+BT_EVT_HDR_SIZE+1;
           break;
       default:
          ALOGE("%s: packet type error", __func__);
          return -3;
     }
#ifdef DIAG_ENABLED
#ifdef LOG_BT_ENABLE
     if(enable_diag && ((buf[0] == LOG_BT_EVT_PACKET_TYPE && buf[1] == LOG_BT_EVT_VENDOR_SPECIFIC &&
        buf[3] == LOG_BT_CONTROLLER_LOG) || is_snoop_log_enabled())) {
        send_btlog_pkt(buf, acl_len, direction);
     }
#endif
#endif//DIAG_ENABLED

     if (no_valid_client || remote_bt_fd == 0) {
          /*Discard the packet and keep the read loop alive*/
          ALOGE("BT is turned off in b/w, keep back in loop");
          free(buf);
          return 0;
     }
#ifdef DEBUG_MIMIC_CMD_TOUT
     if ( command_is_change_lname(buf, acl_len) ) {
         ALOGE("Drop the change local name cmd");
         free(buf);
         return 0;
     }
#endif //DEBUG_MIMIC_CMD_TOUT
#ifdef IGNORE_HCI_RESET
      if (acl_len == 4 && command_is_reset(buf, acl_len))
      {
         ALOGV("It is an HCI_RESET Command ");
         //Dont write it controller rather mimmc success event
         retval = write (src_fd, reset_cmpl, 7);
         if (retval < 0) {
              ALOGE("%s: error while writing hci_reset_cmp", __func__);
         }
         free(buf);
         return retval;
      }
#endif//IGNORE_HCI_RESET
#ifdef WCNSS_IBS_ENABLED
     if(direction == HOST_TO_SOC)
     {
         ALOGV("%s: Assert BT SOC",__FUNCTION__);
         retval = wcnss_wake_assert();
         if(retval == -1)
          {
            ALOGE("%s:Failed to assert SoC",__func__);
            free(buf);
            return 0;
          }
     }
#endif
     if (buf[0] == LOG_BT_EVT_PACKET_TYPE && buf[1] == LOG_BT_EVT_VENDOR_SPECIFIC && buf[3] == LOG_BT_CONTROLLER_LOG)
     {
        if (is_debug_in_ring_buf_enabled() && buf[4] != LOG_BT_MESSAGE_TYPE_MEM_DUMP) {
            update_ring_buffer (&g_in_ringbuf, buf, acl_len, &g_in_ringbuf_swap);
        }

        // Log event from controller. Should not be sent to the stack. Free it here.
        free(buf);
        return 0;
     }
     pthread_mutex_lock(&signal_mutex);
     retval = do_write(dest_fd, buf, acl_len);
     pthread_mutex_unlock(&signal_mutex);
     if (retval < 0) {
         ALOGE("%s:error in writing buf: %d: %s", __func__, retval, strerror(errno));
         if (errno == EPIPE) {
             ALOGV("%s: BT has closed of the other end", __func__);
             /*return 0, so that read loop continues*/
             retval = 0;
         } else {
             retval = -1;
         }
         free(buf);
         return retval;
     }

     ALOGI("Direction(%d): bytes: %d : bytes_written: %d", direction, acl_len, retval);
     for (i =0; i<acl_len; i++) {
         ALOGV("%x-", buf[i]);
     }
     ALOGV("*done");

     if ( direction == HOST_TO_SOC) {
         if (is_debug_out_ring_buf_enabled())
             update_ring_buffer (&g_out_ringbuf, buf, retval, &g_out_ringbuf_swap);
     }
     else {
         if (is_debug_in_ring_buf_enabled())
             update_ring_buffer (&g_in_ringbuf, buf, retval, &g_in_ringbuf_swap);
     }
     ALOGV("%s: copied bt data/cmd (of len %d) succesfully\n", __func__, acl_len);

     free(buf);
     return retval;
}


int copy_ant_host_data_to_soc(int src_fd, int dest_fd, unsigned char protocol_byte) {
    unsigned char hdr[ANT_CMD_HDR_SIZE];
    int len;
    unsigned char *ant_pl;
    int i, retval;
    bool no_valid_client = false;

    ALOGV("%s: entry", __func__);
    if (dest_fd == 0) {
        ALOGE("%s: No valid ANT client connection", __func__);
        /*Discard the packet and keep the read loop alive*/
        no_valid_client = true;
    }

    /*Read protocol byte*/
    retval = do_read(src_fd, &(hdr[ANT_HDR_OFFSET_LEN]), ANT_HDR_OFFSET_LEN);
    if (retval < 0) {
        ALOGE("%s: read length returns err: %d\n", __func__,retval);
        return -1;
    }
    len =  hdr[ANT_HDR_OFFSET_LEN];
    hdr[0] = protocol_byte;
    ALOGV("%s: size of the data is: %d\n", __func__, len);
    ant_pl = (unsigned char*)malloc(len+ANT_CMD_HDR_SIZE);
    if (ant_pl == NULL) {
        ALOGE("Malloc error\n");
        return -1;
    }

    retval = do_read(src_fd, ant_pl+ANT_CMD_HDR_SIZE, (int)len);
    if (retval < 0) {
        ALOGE("read returns err: %d\n", retval);
        retval = -1;
        free(ant_pl);
        return retval;
    }

    memcpy(ant_pl, hdr, ANT_CMD_HDR_SIZE);
#ifdef WCNSS_IBS_ENABLED
    ALOGV("%s: Assert BT SOC for ANT DATA",__FUNCTION__);
    wcnss_wake_assert();
#endif
#ifdef DIAG_ENABLED
#ifdef LOG_BT_ENABLE
    if(enable_diag && is_snoop_log_enabled()) {
        send_btlog_pkt(ant_pl, len+ANT_CMD_HDR_SIZE, HOST_TO_SOC);
    }
#endif
#endif//DIAG_ENABLED
    pthread_mutex_lock(&signal_mutex);
    retval = do_write(dest_fd, ant_pl, len+ANT_CMD_HDR_SIZE);
    pthread_mutex_unlock(&signal_mutex);
    if (retval < 0) {
        ALOGE("write returns err: file_desc: %d %d(%s)\n", dest_fd, retval,strerror(errno));
        retval = -1;
        free(ant_pl);
        return retval;
    }

    ALOGV("ANT host bytes sent*");
    for (i =0; i<retval; i++) {
         ALOGV("%x-", ant_pl[i]);
    }

    free(ant_pl);
    return retval;
}

int copy_ant_data_to_channel(int src_fd, int dest_fd, unsigned char protocol_byte)
{
    unsigned char len;
    unsigned char* ant_pl;
    int retval = 0,i;

    ALOGV("%s: Entry ", __func__);

    int ret = read (src_fd, &len, 1);
    if (ret < 0) {
        ALOGE("%s: read length returns err: %d\n", __func__,ret);
        return -1;
    }

    ALOGV("%s: size of the data is: %d\n", __func__, len);
    ant_pl = (unsigned char*)malloc(len+2);

    if (ant_pl == NULL) {
        ALOGE("Malloc error\n");
        return -1;
    }

    ret = do_read(src_fd, ant_pl+2, (int)len);
    if (ret < 0) {
        ALOGE("read returns err: %d\n", ret);
        retval = -1;
        free(ant_pl);
        return retval;
    }

    if (remote_ant_fd == 0) {
        /*Discard the packet and keep the read loop alive*/
        free(ant_pl);
        return 0;
    }

    if (ret < len)
        ALOGV("%s: expected %d bytes, recieved only %d", __func__, len, ret);

    ant_pl[0] = protocol_byte;
    ant_pl[1] = len;
#ifdef DIAG_ENABLED
#ifdef LOG_BT_ENABLE
    if(enable_diag && is_snoop_log_enabled()) {
        send_btlog_pkt(ant_pl, len+2, SOC_TO_HOST);
    }
#endif
#endif//DIAG_ENABLED
    pthread_mutex_lock(&signal_mutex);
    ret = do_write(dest_fd, ant_pl+1, ret+1);
    pthread_mutex_unlock(&signal_mutex);

    if (ret < 0) {
        ALOGE("write returns err: file_desc: %d %d(%s)\n", dest_fd, ret,strerror(errno));
        if (errno == EPIPE) {
            ALOGV("%s: ANT has closed of the other end", __func__);
             /*return 0, so that read loop continues*/
             retval = 0;
        } else {
             retval = -1;
        }
        free(ant_pl);
        return retval;
    }

    ALOGV("ANT event bytes sent*");
    for (i =0; i<ret; i++) {
         ALOGV("%x-", ant_pl[i]);
    }
    ALOGV("*done");

    ALOGV("%s: copied ant data(of len %d) succesfully\n", __func__,len);

    free(ant_pl);
    ALOGV("%s: retval: %d\n", __func__,retval);
    return retval;
}

int handle_soc_events(int fd_transport) {
    unsigned char first_byte;
    int retval;
    ALOGV("%s: Entry ", __func__);

    retval = read (fd_transport, &first_byte, 1);
    if (retval < 0) {
        ALOGE("%s:read returns err: %d\n", __func__,retval);
        return -1;
    }

    ALOGV("%s: protocol_byte: %x", __func__, first_byte);
    if (isIBSByte(first_byte)) {
        ALOGV("%s: sleep byte", __func__);
#ifdef WCNSS_IBS_ENABLED
        ibs_recv_ibs_cmd(&first_byte);
#endif
        if (is_debug_in_ring_buf_enabled()) {
           update_ring_buffer (&g_in_ringbuf, &first_byte, 1, &g_in_ringbuf_swap);
        }
        return 0;
    }

    switch(first_byte) {
        case ANT_CTL_PACKET_TYPE:
        case ANT_DATA_PACKET_TYPE:
            ALOGV("%s: Ant data", __func__);
            retval = copy_ant_data_to_channel(fd_transport, remote_ant_fd, first_byte);
            ALOGV("%s: copy_ant_data_to_channel returns %d", __func__, retval);
            break;
        case BT_EVT_PACKET_TYPE:
        case BT_ACL_PACKET_TYPE:
            ALOGV("%s: BT data", __func__);
            retval = copy_bt_data_to_channel(fd_transport, remote_bt_fd, first_byte,SOC_TO_HOST);
            break;
        default:
            ALOGE("%s: Unexpected data format!!:%x - Ignore the Packet ",__func__,first_byte);
            //retval = -1;
            tcflush(fd_transport, TCIFLUSH);
            retval = 0 ;
    }

    ALOGV("%s: Exit %d", __func__, retval);
    return retval;
}

static int start_reader_thread() {
    fd_set input;
    int n = 0, retval;
    ALOGV("%s: Entry ", __func__);

    if ((fd_transport = init_transport()) == -1)
    {
        ALOGE("unable to initialize transport %s", BT_HS_UART_DEVICE);
        return -1;
    }

    /*Indicate that, server is ready to accept*/
    property_set("wc_transport.hci_filter_status", "1");

    FD_ZERO(&input);
    FD_SET(fd_transport, &input);

    do {
        ALOGV("%s: Selecting on transport for events", __func__);
        n = select (fd_transport+1, &input, NULL, NULL, NULL);

        if(n < 0){
            ALOGE("Select failed: %s", strerror(errno));
            retval = -1;
            break;
        }

        ALOGV("%s: Select comes out\n", __func__);

        if (FD_ISSET(fd_transport, &input))        {
             retval = handle_soc_events(fd_transport);
             if(retval < 0) {
                 ALOGE("%s: handle_soc_events returns: %d: ", __func__, retval);
                 retval = -1;
                 break;
            }
        }
    } while(1);

    close(fd_transport);
    fd_transport = 0;
    ALOGV("%s: Exit %d", __func__, retval);
    return retval;
}

int cleanup_thread(pthread_t thread) {
    int status = 0;
    ALOGV("%s: Entry", __func__);

    if((status = pthread_kill(thread, SIGUSR1)) != 0) {
        ALOGE("Error cancelling thread %d, error = %d (%s)",
        (int)thread, status, strerror(status));
    }

    if((status = pthread_join(thread, NULL)) != 0) {
        ALOGE("Error joining thread %d, error = %d (%s)",
      (int)thread, status, strerror(status));
    }
#ifdef WCNSS_IBS_ENABLED
    wcnss_ibs_cleanup();
#endif
    ALOGV("%s: End", __func__);
    return status;
}

int main() {
    ALOGV("%s: Entry", __func__);
    signal(SIGPIPE, SIG_IGN);

#ifdef PANIC_ON_SOC_CRASH
    ALOGV("%s: open: %s", __func__, PROC_PANIC_PATH);
    fd_sysrq = open(PROC_PANIC_PATH, O_WRONLY);
    if(fd_sysrq < 0) {
        ALOGE("%s: open (%s) fail - %s (%d)", __func__,
            PROC_PANIC_PATH, strerror(errno), errno);
        //ignore if panic path can't be opened
        //and proceed
    }
#endif//PANIC_ON_SOC_CRASH

    set_debug_properties();

    if (is_debug_out_ring_buf_enabled()) {
        reset_ring_buffer(&g_out_ringbuf);
        reset_ring_buffer(&g_out_ringbuf_swap);
    }
    if (is_debug_in_ring_buf_enabled()) {
       reset_ring_buffer(&g_in_ringbuf);
       reset_ring_buffer(&g_in_ringbuf_swap);
    }

    pthread_mutex_init(&signal_mutex, NULL);
    if (pthread_create(&bt_mon_thread, NULL, (void *)bt_thread, NULL) != 0) {
        perror("pthread_create for bt_monitor");
        pthread_mutex_destroy(&signal_mutex);
        return -1;
    }

    if (pthread_create(&ant_mon_thread, NULL, (void *)ant_thread, NULL) != 0) {
        perror("pthread_create for ant_monitor");
        cleanup_thread(bt_mon_thread);
        pthread_mutex_destroy(&signal_mutex);
        return -1;
    }
#ifdef DIAG_ENABLED
#ifdef LOG_BT_ENABLE
    enable_diag = diag_init();
#endif
#endif//DIAG_ENABLED
    /*Main thread monitors on UART data/events*/
    int ret = start_reader_thread();

    if (ret < 0) {
        ALOGE("%s: start_reader_thread returns: %d", __func__, ret);
    }

    cleanup_thread(ant_mon_thread);
    cleanup_thread(bt_mon_thread);
    pthread_mutex_destroy(&signal_mutex);

#ifdef DIAG_ENABLED
#ifdef LOG_BT_ENABLE
    diag_deinit();
#endif
#endif//DIAG_ENABLED
    ALOGV("%s: Exit: %d", __func__, ret);
    return ret;
}

static void handle_cleanup()
{
    char ref_count[PROPERTY_VALUE_MAX];
    char cleanup[PROPERTY_VALUE_MAX];
    int ref_val,clean;

    ALOGE("wcnss_filter client is terminated");
    property_get("wc_transport.clean_up", cleanup, "0");
    clean = atoi(cleanup);
    ALOGE("clean Value =  %d",clean);
    property_get("wc_transport.ref_count", ref_count, "0");
    ref_val = atoi(ref_count);
    if(clean == 0) {
      if(ref_val > 0)
      {
         ref_val--;
         snprintf(ref_count, 3, "%d", ref_val);
         property_set("wc_transport.ref_count", ref_count);
      }
    }
    else {
      if(ref_val == 1) {
        ALOGE("VOTOFF Clk before shutdown");
        wcnss_vote_off_clk();
      }
    }
    if (!remote_bt_fd && !remote_ant_fd)
    {
        cleanup_and_suicide();
    }
}

static void cleanup_and_suicide()
{
    //Set REF count 1, so that next trial to reset the BT succeeds
    //property_set("wc_transport.ref_count", "1");
    send_special_bytes = false ;
    property_set("wc_transport.hci_filter_status", "0");
    property_set("wc_transport.start_hci",  "false");
    property_set("wc_transport.soc_initialized", "0");
    property_set("wc_transport.start_root", "false");
    ALOGE("Done with this Life!!!");
    exit(0);
}


void report_soc_failure(void)
{
   char eve_buf[] = {0x04,0x10,0x01,0x0f};
   int ret_val;
   ALOGD("%s",__func__);
   ret_val = do_write(remote_bt_fd,eve_buf,4);
   if(ret_val < 0)
    ALOGE("%s: Failed to report",__func__);

   cleanup_and_suicide();
   if (is_debug_force_special_bytes()) {
       soc_crash_wait_timer_cleanup();

       if (g_is_clock_on) {
           ALOGE("%s: VOTOFF Clk before soc failure", __func__);
           wcnss_vote_off_clk();
       }
   }
}
