#ifndef __QSEE_SMC_H
#define __QSEE_SMC_H

#include "lktypes.h"
#include "tzbsp_log.h"
#include "qsee_errno.h"
#include <limits.h>


typedef enum
{
  QSEE_APP_START_COMMAND = 0x01,
  QSEE_APP_SHUTDOWN_COMMAND,
  QSEE_APP_LOOKUP_COMMAND,
  QSEE_REGISTER_LISTENER,
  QSEE_DEREGISTER_LISTENER,
  QSEE_CLIENT_SEND_DATA_COMMAND,
  QSEE_LISTENER_DATA_RSP_COMMAND,
  QSEE_LOAD_EXTERNAL_ELF_COMMAND,
  QSEE_UNLOAD_EXTERNAL_ELF_COMMAND,
  QSEE_GET_APP_STATE_COMMAND,
  QSEE_LOAD_SERV_IMAGE_COMMAND,
  QSEE_UNLOAD_SERV_IMAGE_COMMAND,
  QSEE_APP_REGION_NOTIFICATION,
  QSEE_REGISTER_LOG_BUF_COMMAND,
  QSEE_RPMB_PROVISION_KEY_COMMAND,
  QSEE_RPMB_ERASE_COMMAND,
  QSEE_KS_GEN_KEY_COMMAND = 17,
  QSEE_KS_DEL_KEY_COMMAND = 18,
  QSEE_KS_GET_MAX_KEYS_COMMAND = 19,
  QSEE_KS_SET_PIPE_KEY_COMMAND      = 20,
  QSEE_KS_UPDATE_KEY_COMMAND        = 21
}qsee_smc_command_type;

/**
   Client must differentiate between a return to request a listener
   service and a return from the original request.  Clients now depend
   on calls returning corresponding enumerated response types for the
   non-listener request case.
 */
typedef enum
{
  QSEE_APP_ID      = 0x0000EE01, /**< QSEE response type when the response
                                  data is the application ID. */
  QSEE_LISTENER_ID = 0x0000EE02, /**< QSEE response type when the response
                                  data is the listener ID. */
  QSEE_APP_STATE   = 0x0000EE03, /**< QSEE response type when the response
                                  data is the app state. */
  QSEE_NONE        = 0x0000EE04,
  QSEE_GENERAL     = 0x0000EE05, /**< QSEE response type when the response
                                      data is specific to the request */
  QSEE_RESERVED    = INT_MAX /**< Required to make the enum 4 bytes. */
} qsee_command_resp_type;

typedef enum
{
  QSEE_APP_STATE_SUSPENDED = 0x01,
  QSEE_APP_STATE_ACTIVE  //"Active" includes all other states than "suspended" 
}qsee_app_state;

typedef struct qsee_command_resp_info_type
{
  uint32_t result;
  qsee_command_resp_type resp_type;
  unsigned int data;
} qsee_command_resp_info_t;

/*mdt must start with elf and program headers.*/
typedef struct img_info_type{
  uint32_t mdt_len;
  uint32_t img_len;
  uint32_t pa;
  char    app_name[32];
}img_info_t;

typedef struct qsee_common_request_type{
  uint32_t command_id;
} qsee_common_request_t;

typedef struct qsee_apps_region_notificaton_type{
  uint32_t applications_region_addr;
  uint32_t applications_region_size;  
}qsee_apps_region_notification_t;

typedef struct qsee_rpmb_provision_key_req_type{
  uint32_t key_type;
}qsee_rpmb_provision_key_req_t;

typedef struct qsee_client_send_data_req_type {
  uint32_t app_id;
  void * req_ptr;
  uint32_t req_len;
  void * rsp_ptr;   /* First 4 bytes should always be the return status */   
  uint32_t rsp_len;
}qsee_client_send_data_req_t;

typedef struct qsee_register_listener_type {
  uint32_t listener_id;
  void * req_ptr;
  uint32_t req_len;
}qsee_register_listener_t;

typedef struct qsee_deregister_listener_type {
  uint32_t listener_id;
}qsee_deregister_listener_t;

typedef struct qsee_get_app_state_type {
  uint32_t app_id;
}qsee_get_app_state_t;

typedef struct qsee_listener_rsp_type
{
  uint32_t listener_id;    
  uint32_t status;
}qsee_listener_rsp_t;

typedef struct qsee_listener_legacy_rsp_s
{
  uint32  listener_id;  /**< Listener service ID. */
} qsee_listener_legacy_rsp_t;

/* register log buffer */
typedef struct qsee_reg_log_buf_req_type {
   void *      phy_addr;    /* physical address of log buffer area */
   uint32_t    len;         /* Length of buffer */
}qsee_reg_log_buf_req_t;
typedef struct qsee_load_serv_image_type
{
  uint32_t mdt_len;
  uint32_t img_len;
  uint32_t pa; 
}qsee_load_serv_image_t;



int qsee_process_app_start_smc(void* req, uint32_t reqlen, void* rsp, uint32_t rsplen);

int qsee_smc_handler(void* req, uint32_t reqlen, void* rsp, uint32_t rsplen);

unsigned int qsee_get_app_blocked_on_listener(unsigned int listener_id);

//int qsee_validate_image_seg_range(uint32_t addr, uint32_t size);

int qsee_load_services_image(void* req, uint32_t reqlen, void* rsp, uint32_t rsplen);

int qsee_unload_services_image(void);

int qsee_remap_SD_MGR_error_codes(int input);
#endif
