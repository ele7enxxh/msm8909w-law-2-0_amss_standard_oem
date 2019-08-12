#ifndef __QSEE_LEGACY_H
#define __QSEE_LEGACY_H

#include "lktypes.h"
#include "lkthread.h"
#include "map.h"

/*Structure of the command passed to QSEE. It is used for 
 *1. App start
 *1. App shutdown
 */
typedef enum
{
  QSEE_LEGACY_APP_START_COMMAND = 0xFF01,
  QSEE_LEGACY_APP_SHUTDOWN_COMMAND,
  QSEE_LEGACY_APP_LOOKUP_COMMAND      /*only app lookup command of legacy is going to be used.*/
}qsee_legacy_command_type;


typedef struct qsee_legacy_app_lookup_command_info_type {
  char app_name[THREAD_NAME_LEN]; /*App Name*/
} qsee_legacy_app_lookup_command_info_t;

typedef struct qsee_legacy_command_info_type {
  qsee_legacy_command_type command;
  void *data;
} qsee_legacy_command_info_t;

/*Structure of the command response from QSEE. It is used for 
 *1. App start
 *1. App shutdown
 */
typedef enum
{
  QSEE_LEGACY_APP_ID = 0xEE01,
}qsee_legacy_command_resp_type;

typedef struct qsee_legacy_command_resp_info_type {
  uint32_t result; 
  qsee_legacy_command_resp_type resp_type;
  void *data;
} qsee_legacy_command_resp_info_t;


int qsee_legacy_app_smc_handler(uint32_t app_id, void* req, uint32_t reqlen, void* rsp, uint32_t rsplen);

int qsee_legacy_smc_handler(void* req, uint32_t reqlen, void* rsp, uint32_t rsplen);

void qsee_wait(void);

#endif
