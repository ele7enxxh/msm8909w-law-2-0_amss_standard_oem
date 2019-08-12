/******************************************************************************
  @file    io-p.c
  @brief   Implementation of performance server module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define ATRACE_TAG ATRACE_TAG_ALWAYS

#include "io-p.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include <private/android_filesystem_config.h>
#include <cutils/properties.h>
#include <cutils/trace.h>
#include <io-prefetch/list_capture.h>
#include <sys/socket.h>
#include <cutils/sockets.h>

#ifdef SERVER
#include <dlfcn.h>
#endif

#define LOG_TAG           "ANDR-IOP"
#include <cutils/log.h>
#if defined(ANDROID_JELLYBEAN)
#include "common_log.h"
#endif

#define BILLION             1000000000L

#if QC_DEBUG
#define QLOGE(...)    ALOGE(__VA_ARGS__)
#define QLOGW(...)    ALOGW(__VA_ARGS__)
#define QLOGI(...)    ALOGI(__VA_ARGS__)
#define QLOGV(...)    ALOGV(__VA_ARGS__)
#else
#define QLOGE(...)
#define QLOGW(...)
#define QLOGI(...)
#define QLOGV(...)
#endif

#define IOP_VERSION      1
#define IOP_SOCKET       "/dev/socket/iop"

#define IOP_MAGIC        0x4d502d43 /* "MP-C" */
#define SOCID_8939        239
#define SOCID_8994        207
#define SOCID_8992        251
#define SOCID_8092        252
#define SOCID_8996        246
#define SOCID_8996PRO     305
#define SOCID_8096PRO     312
#define SOCID_8096        291
#define SOCID_8998        292

#define MIN_FREQ_REQ      0
#define MAX_FREQ_REQ      1
#define PKG_LEN           1024

typedef struct _iop_msg_t {
    uint32_t magic;
    uint8_t  version;
    uint8_t  seq;
    uint16_t data;
    uint32_t pl_handle;
    int profile;
    int pl_time;
    int pid;
    char pkg_name[PKG_LEN];
    union {
        uint32_t msg;
        uint8_t  m[4];
    };
    int cmd;
    pid_t    client_pid;
    pid_t    client_tid;
} iop_msg_t;

#ifdef SERVER

#define IOP_NO_RECENT_APP 7
char recent_list[IOP_NO_RECENT_APP][PKG_LEN];
int recent_list_cnt = 0;

static pthread_t iop_server_thread;
static iop_msg_t msg;
static int conn_socket;

extern int create_database();
extern void start_playback(char *pkg_name);
extern void stop_playback();

static int comsoc = -1;
static struct sockaddr_un addr;

struct timespec m_accept;
struct timespec connected;
struct timespec recvd;

uint64_t time_delta;

struct request {
    timer_t timer;
    pid_t pid;
    pid_t tid;
    int duration;
    int num_args;
};

static int get_soc_id()
{
  int fd;
  int soc = -1;
  if (!access("/sys/devices/soc0/soc_id", F_OK)) {
       fd = open("/sys/devices/soc0/soc_id", O_RDONLY);
  } else {
       fd = open("/sys/devices/system/soc/soc0/id", O_RDONLY);
  }
  if (fd != -1)
  {
      char raw_buf[5];
      read(fd, raw_buf,4);
      raw_buf[4] = 0;
      soc = atoi(raw_buf);
      close(fd);
  }
  else
      close(fd);
  return soc;
}

static int soc_id;


/*=========================================================================================================================*/

static void *iop_server(void *data)
{
    int rc, cmd;
    socklen_t len;

    if (comsoc < 0)
        return NULL;

    (void)data;

    /* Main loop */
    for (;;) {
        len = sizeof(struct sockaddr_un);

        clock_gettime(CLOCK_MONOTONIC, &m_accept);
        time_delta = (BILLION * (m_accept.tv_sec - connected.tv_sec)) +
                                (m_accept.tv_nsec - connected.tv_nsec);
        QLOGI("time taken from connected to accept: %llu nanoseconds",
                                (long long unsigned int)time_delta);

        conn_socket = accept(comsoc, (struct sockaddr *) &addr, &len);
        if (conn_socket == -1) {
            QLOGE("PERFLOCK iop server %s: accept failed, errno=%d (%s)"
                                    , __func__, errno, strerror(errno));
            goto close_conn;
        }

        clock_gettime(CLOCK_MONOTONIC, &connected);

        rc = recv(conn_socket, &msg, sizeof(iop_msg_t), 0);
        if (rc < 0) {
            QLOGE("PERFLOCK iop server: failed to receive message, errno=%d (%s)"
                                    , errno, strerror(errno));
            goto close_conn;
        }

        clock_gettime(CLOCK_MONOTONIC, &recvd);

        /* Descramble client id */
        msg.client_pid ^= msg.msg;
        msg.client_tid ^= msg.msg;

        QLOGV("Received len=%d, m=%u, v=%u, c=%u, s=%u, m=%u (0x%08x) d=%u",
              rc, msg.magic, msg.version, msg.client_pid, msg.seq, msg.msg, msg.msg, msg.data);

        if (rc != sizeof(iop_msg_t)) {
            QLOGE("Bad size");
        }
        else if (msg.magic != IOP_MAGIC) {
            QLOGE("Bad magic");
            close(conn_socket);
            continue;
        }
        else if (msg.version != IOP_VERSION) {
            QLOGE("Version mismatch");
            close(conn_socket);
            continue;
        }

        //cmd = CMD_DECODE(msg);
        cmd = msg.cmd;

        switch (cmd) {
            case IOP_CMD_PERFLOCK_IOPREFETCH_START:
            {
                static bool is_in_recent_list = false;
                char property[PROPERTY_VALUE_MAX];
                int enable_prefetcher = 0;

                property_get("enable_prefetch", property, "1");
                enable_prefetcher = atoi(property);

                if(!enable_prefetcher)
                {
                    QLOGE("io prefetch is disabled");
                    break;
                }
                // if PID < 0 consider it as playback operation
                if(msg.pid < 0)
                {
                    int ittr = 0;
                    is_in_recent_list = false;
                    //Check app is in recent list
                    for(ittr = 0; ittr < IOP_NO_RECENT_APP; ittr++)
                    {
                        if(0 == strcmp(msg.pkg_name,recent_list[ittr]))
                        {
                            is_in_recent_list = true;
                            QLOGE("is_in_recent_list is TRUE");
                            break;
                        }
                    }
                    // IF Application is in recent list, return
                    if(true == is_in_recent_list)
                    {
                        QLOGE("io prefetch is deactivate");
                        break;
                    }

                    if(recent_list_cnt == IOP_NO_RECENT_APP)
                        recent_list_cnt = 0;

                    //Copy the package name to recent list
                    strlcpy(recent_list[recent_list_cnt],msg.pkg_name,PKG_LEN);
                    recent_list_cnt++;

                    stop_capture();
                    stop_playback();
                    start_playback(msg.pkg_name);
                }
                // if PID > 0 then consider as capture operation
                if(msg.pid > 0)
                {
                    if(true == is_in_recent_list)
                    {
                        QLOGE("io prefetch Capture is deactivated ");
                        break;
                    }
                    stop_capture();
                    start_capture(msg.pid,msg.pkg_name);
                }

                break;
            }

            case IOP_CMD_PERFLOCK_IOPREFETCH_STOP:
            {
                stop_capture();
                break;
            }

            default:
                QLOGE("Unknown command %d", cmd);
        }
close_conn:
    close(conn_socket);
    }

    QLOGI("IOP server thread exit due to rc=%d", rc);
    return NULL;
}


int iop_server_init()
{
    int rc = 0, stage = 0;

    QLOGI("IOP server starting");

    soc_id = get_soc_id();
    switch(soc_id)
    {
         case SOCID_8994:
         case SOCID_8092:
         case SOCID_8992:
         case SOCID_8996:
         case SOCID_8996PRO:
         case SOCID_8096PRO:
         case SOCID_8096:
         case SOCID_8998:
         case SOCID_8939:
             break;
         default:
             QLOGI("Fail to init IOP Server");
             return 0;
    }

    create_database();

    comsoc = android_get_control_socket("iop");
    if (comsoc < 0) {
        stage = 1;
        goto error;
    }

    rc = listen(comsoc, SOMAXCONN);
    if (rc != 0) {
        stage = 2;
        goto error;
    }

    rc = pthread_create(&iop_server_thread, NULL, iop_server, NULL);
    if (rc != 0) {
        stage = 3;
        goto error;
    }
    return 1;

error:
    QLOGE("Unable to create control service (stage=%d, rc=%d)", stage, rc);
    if (comsoc != -1) {
        close(comsoc);
        comsoc = -1;
    }
    return 0;
}

void iop_server_exit()
{
    pthread_join(iop_server_thread, NULL);
    if (comsoc != -1) {
        close(comsoc);
        comsoc = -1;
    }
}

#endif /* SERVER */

#ifdef CLIENT
static volatile int seq_num = 0;

static void iop_client_exit(int client_comsoc)
{
    if (client_comsoc >= 0) {
        close(client_comsoc);
    }
}

int iop_send(int control, ...)
{
    int rc, len;
    iop_msg_t msg;
    int client_comsoc = -1;
    pid_t client_pid = 0;
    pid_t client_tid = 0;
    struct sockaddr_un client_addr;
    struct timeval tv;

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    client_comsoc = socket(PF_UNIX, SOCK_SEQPACKET, 0);

    if (client_comsoc < 0) {
        QLOGE("Failed to initialize control channel");
        return 0;
    }

    fcntl(client_comsoc, F_SETFL, O_NONBLOCK);

    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    client_addr.sun_family = AF_UNIX;
    snprintf(client_addr.sun_path, UNIX_PATH_MAX, IOP_SOCKET);

    /* Prepare message for sending */
    msg.magic   = IOP_MAGIC;
    msg.version = IOP_VERSION;
    msg.seq     = seq_num++;
    msg.data    = 0;

    switch (control)
    {
        case IOP_CMD_PERFLOCK_IOPREFETCH_START:
        {
            char *pnew_pkg_name;
            va_list vargs;
            va_start(vargs, control);
            msg.pid = va_arg(vargs, int);
            pnew_pkg_name = va_arg(vargs, char*);
            strlcpy(msg.pkg_name,pnew_pkg_name,PKG_LEN);
            va_end(vargs);
            QLOGI("IOP pkg_name = %s PID = %d",msg.pkg_name, msg.pid);
            break;
        }

        case IOP_CMD_PERFLOCK_IOPREFETCH_STOP:
        {
            // No argument need to pass
            break;
        }

        default:
            QLOGW("Bad parameter to iop_send %d", control);
            return 0;
    }


    QLOGI("IOP CMD_ENCODE send -4 %d", control);

    msg.cmd = control;

    client_pid = getpid();
    client_tid = gettid();

    msg.client_pid  = client_pid ^ msg.msg;
    msg.client_tid  = client_tid ^ msg.msg;

    len = sizeof(struct sockaddr_un);
    rc = connect(client_comsoc, (struct sockaddr *) &client_addr, len);

    if (rc == -1) {
        QLOGE("IOP failed to connect to socket: errno=%d (%s)", errno, strerror(errno));
        iop_client_exit(client_comsoc);
        return rc;
    }

    rc = send(client_comsoc, &msg, sizeof(iop_msg_t), 0);

    if (rc == -1) {
        QLOGE("IOP failed to send message to client: errno=%d (%s)", errno, strerror(errno));
        iop_client_exit(client_comsoc);
        return rc;
    }

    iop_client_exit(client_comsoc);
    return rc;
}
#endif /* CLIENT */

