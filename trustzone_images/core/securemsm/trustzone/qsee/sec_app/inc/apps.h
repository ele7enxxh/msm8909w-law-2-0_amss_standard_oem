#ifndef __QSEE_CORE_APPS_H
#define __QSEE_CORE_APPS_H

#include "lktypes.h"
#include "lkthread.h"
#include "lkmutex.h"
#include "lktimer.h"
#include "tzos_exec.h"
#include "map.h"

#define QSEE_VERION_MAJOR 01
#define QSEE_VERION_MINOR 02

#define QSEE_RETURN 1
#define LEGACY_RETURN 2

#define MAX_UINT32 0xffffffff

extern unsigned int syscall_return_path;
extern unsigned int syscall_command_id;
extern unsigned int global_app_id;

typedef struct apps_request_resp_info
{
  uint32_t command;
  const void* req;
  uint32_t reqlen;
  const void* rsp;
  uint32_t rsplen;
}apps_request_resp_info_t;

typedef struct apps_listener_resp_info
{
  void* rsp;
  unsigned int rsplen;
}apps_listener_resp_info_t;

int qsee_process_app_smc(uint32_t app_id, const void* req, uint32_t reqlen, const void* rsp, uint32_t rsplen);

/**
 * Called to notify QSEE client that warmboot has occurred
 */
void qsee_warmboot_notifier_func(void);

/**
 * Called to notify QSEE client that power collapse has occurred
 */
void qsee_power_collapse_notifier_func(void);

void qsee_app_entry_return(unsigned int init_result, void *entry, unsigned int stack_size, void *stack_base, char *name);
void qsee_app_init_return(unsigned int sp);
unsigned int qsee_create_app_thread(unsigned int init_result, void *entry, unsigned int stack_size, void *stack_base, char *name);
int qsee_app_entry(unsigned int entry);
int qsee_app_init(uint8_t major, uint8_t minor, unsigned int entry, unsigned int rw_base, unsigned int *sp);
int qsee_app_kill(int app_id);

void qsee_int_handler(unsigned int);


uint32 qsee_get_listener_buffer_size(unsigned int listener_id);
void* qsee_get_and_lock_listener_buffer(unsigned int listener_id);
void qsee_unlock_listener_buffer(unsigned int listener_id);

int qsee_request_service(unsigned int listener_id, void *req, unsigned int req_len, void *rsp, unsigned int rsplen);

int qsee_register_shared_buffer(void *address, unsigned int size);
int qsee_deregister_shared_buffer(void *address);
int qsee_prepare_shared_buf_for_nosecure_read(void * address, unsigned int size);
int qsee_prepare_shared_buf_for_secure_read(void *address, unsigned int size);

void qsee_app_shutdown(thread_t *pthread);

void qsee_set_global_flag(unsigned int flag);
unsigned int qsee_get_global_flag(void);

/*Structure of the command passed to QSEE. It is used for 
 *1. App start
 *1. App shutdown
 */
typedef enum
{
  QSEE_START_CMD_TO_APP = 0xFF01,
  QSEE_SHUTDOWN_CMD_TO_APP,
  QSEE_FLUSH_CMD_TO_APP,
  QSEE_INVAL_CMD_TO_APP,
}qsee_command_type;

typedef struct qsee_app_start_command_info_type {
  uint32_t elf_header_ptr; /*Elf header pointer*/
  uint32_t image_base_addr; /*Image base pointer*/
} qsee_app_start_command_info_t;

typedef struct qsee_app_shutdown_command_info_type {
  uint32_t app_id; /*App ID*/
} qsee_app_shutdown_command_info_t;

typedef struct qsee_app_lookup_command_info_type {
  char app_name[THREAD_NAME_LEN]; /*App Name*/
} qsee_app_lookup_command_info_t;

typedef struct qsee_command_info_type {
  qsee_command_type command;
  void *data;
} qsee_command_info_t;

typedef struct listener_info_type{
  uint32_t listener_id;
  void*    req_ptr;
  uint32_t req_len;
  uint32_t blocked_app_id; 
  uint32_t status;
}listener_info_t;

void qsee_initialize_listener_table(void);
int qsee_register_listener(uint32_t listener_id, void * req_ptr, uint32_t req_len);
int qsee_deregister_listener(uint32_t listener_id);
//int qsee_process_listener_response(uint32_t listener_id, void * rsp_ptr, uint32_t rsp_len);
int qsee_process_listener_response(uint32_t listener_id, uint32_t status);

typedef struct qsee_app_reqrsp_protection_type {
  uint32_t app_id;
  uint32_t addr;
  uint32_t  len;
}qsee_app_reqrsp_protection_t;

int qsee_protect_reqrsp_region(uint32_t app_id, uint32_t addr, uint32_t len);

void qsee_unprotect_reqrsp_region(void);

void qsee_app_faulted(void);
void qsee_set_app_faulted(void);

void qsee_return_smc_handler(void);

int qsee_validate_ptrs_belong_to_app(uint32 address, uint32 size, thread_t *t, boolean commonlib, uint32* bytes_left);
int qsee_is_range_in_app_segments(uint32 address, uint32 size, segments *s, uint32* bytes_left);

/*Checks if [addr,size-1] is mapped by ANY application*/
int qsee_is_mapped_by_application(uint32_t address, uint32_t size);

uint32 qsee_read_register(uint32 register_addr);

uint32 qsee_get_app_cert_id(uint8 * app_cert_id, uint32 id_buf_len);
uint32 qsee_get_app_img_hash(uint8 * hash_buf, uint32 hash_buf_len);

#endif
