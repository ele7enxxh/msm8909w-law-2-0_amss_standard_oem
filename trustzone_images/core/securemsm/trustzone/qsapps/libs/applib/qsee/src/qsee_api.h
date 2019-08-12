#ifndef __TZAPPS_APPS_H
#define __TZAPPS_APPS_H

typedef  unsigned char  boolean;

typedef struct apps_request_resp_info {
  unsigned int command;
  void* req;
  unsigned int reqlen;
  void* rsp;
  unsigned int rsplen;
}apps_request_resp_info_t;


typedef enum {
  QSEE_APP_START_COMMAND = 0xFF01,
  QSEE_APP_SHUTDOWN_COMMAND,
  QSEE_APP_FLUSH_COMMAND,
  QSEE_APP_INVAL_COMMAND
}qsee_command_type;

#endif
