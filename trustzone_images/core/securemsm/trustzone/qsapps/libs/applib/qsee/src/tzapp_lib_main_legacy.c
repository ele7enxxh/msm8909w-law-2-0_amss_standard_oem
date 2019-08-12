
/////////////////////////////////////////////////////////
// main image entry point
/////////////////////////////////////////////////////////

#include <stdlib.h>
#include "qsee_api.h"
#include "qsee_heap.h"
#include "qsee_version.h"
#include "qsee_log.h"
#include "syscall_wrapper.h"
#include "syscall.h"
#include "memheap_lite.h"
#include "qsee_message.h"

extern char Image$$TZ_APP_STACK$$ZI$$Base;
extern unsigned int Image$$TZ_APP_STACK$$ZI$$Limit;

extern char Image$$TZ_APP_HEAP$$ZI$$Base;
extern unsigned int Image$$TZ_APP_HEAP$$ZI$$Limit;

extern void __cpp_initialize__aeabi_(void);
extern char TZ_APP_NAME[];

extern void qsee_dcache_flush_region(void *addr, uint32  len);
extern void qsee_dcache_inval_region(void *addr, unsigned int length);

extern void tz_app_init(void);


/*Needed for stack protection.  Set initial value:*/
unsigned int __stack_chk_guard[] = {0xDEADDEAD};

void qsee_err_fatal(void)
{
  qsee_generic_syscall(SYSNUM(qsee_app_fatal_error), 0,0,0,0,0,0);
}

void
__stack_chk_fail(void)
{
  qsee_err_fatal();
}

/*There may be a compiler bug because of which 
 *if there is no const string that needs to be
 *patched as part of __cpp_initialize__aeabi_ 
 *init array still have one entry which is invalid 
 *which causes issue. so force create at leave one 
 *valid entry with this dummy variable. 
 */
const char *dummy_str[] =
{
   "A",
};

apps_request_resp_info_t apps_request_resp;

void tzlib_app_entry(void);

/*Needs to be overridden. called by rvct*/
char *
_sys_command_string(char *cmd, int len)
{
  return "[stub]";
}

unsigned int get_stack_base(void)
{
  return (unsigned int)&Image$$TZ_APP_STACK$$ZI$$Base;
}
unsigned int get_stack_size(void)
{
  //return (unsigned int)&Image$$TZ_APP_STACK$$ZI$$Length;
  //return TZ_APP_STACK_SIZE;
  return (unsigned int)((unsigned int)&Image$$TZ_APP_STACK$$ZI$$Limit - (unsigned int)&Image$$TZ_APP_STACK$$ZI$$Base);
}

char * get_app_name(void)
{
  return TZ_APP_NAME;
}

typedef void (*tzlib_app_entry_t)(void);
tzlib_app_entry_t get_app_entry(void)
{
  return tzlib_app_entry;
}

unsigned int* get_app_req_resp(void)
{
  return (unsigned int *)&apps_request_resp;
}

extern void tz_app_cmd_handler(void* req, unsigned int reqlen, void* rsp, unsigned int rsplen);
extern void tz_app_shutdown(void);

extern void qsee_wait_syscall(void);
extern void qsee_wait_default_signal_syscall(void);

void wait_for_next_request(void)
{
  /*Wait for the default event (request from HLOS) */
  qsee_wait_default_signal_syscall();
    
  switch (apps_request_resp.command) {
    case QSEE_APP_SHUTDOWN_COMMAND:
      tz_app_shutdown();
      break;
    default:
      qsee_dcache_inval_region(apps_request_resp.req, apps_request_resp.reqlen);
      qsee_dcache_inval_region(apps_request_resp.rsp, apps_request_resp.rsplen);
      tz_app_cmd_handler(apps_request_resp.req, apps_request_resp.reqlen, apps_request_resp.rsp, apps_request_resp.rsplen);
      qsee_dcache_flush_region(apps_request_resp.req, apps_request_resp.reqlen );
      qsee_dcache_flush_region(apps_request_resp.rsp, apps_request_resp.rsplen );
      break;
  }
}

unsigned int qsee_read_register(unsigned int addr)
{
  return qsee_generic_syscall(SYSNUM(qsee_read_register), addr,0,0,0,0,0);
}

int qsee_request_service(unsigned int listener_id, void *req, unsigned int req_len, void *rsp, unsigned int rsplen)
{
  return qsee_generic_syscall(SYSNUM(qsee_request_service), listener_id, (unsigned int)req, req_len, (unsigned int)rsp, rsplen, 0);
}

void qsee_wait(void)
{
  qsee_generic_syscall(SYSNUM(qsee_wait_hlos), 0,0,0,0,0,0);
}

int qsee_register_shared_buffer(void *address, unsigned int size)
{
  return qsee_generic_syscall(SYSNUM(qsee_reg_sb), (unsigned int)address, size, 0,0,0,0);
}
int qsee_deregister_shared_buffer(void *address)
{
  return qsee_generic_syscall(SYSNUM(qsee_dereg_sb), (unsigned int)address, 0,0,0,0,0);
}
int qsee_prepare_shared_buf_for_nosecure_read(void * address, unsigned int size)
{
  return qsee_generic_syscall(SYSNUM(qsee_prep_sb_for_ns_read), (unsigned int)address, size, 0,0,0,0);
}
int qsee_prepare_shared_buf_for_secure_read(void *address, unsigned int size)
{
  return qsee_generic_syscall(SYSNUM(qsee_prep_sb_for_s_read), (unsigned int)address, size, 0, 0,0,0);
}

void qsee_set_global_flag(unsigned int flag)
{
  qsee_generic_syscall(SYSNUM(qsee_set_global_flag), flag, 0,0,0,0,0);
}
unsigned int qsee_get_global_flag(void)
{
  return qsee_generic_syscall(SYSNUM(qsee_get_global_flag), 0,0,0,0,0,0);
}
unsigned int qsee_oem_process_cmd(unsigned char *input, unsigned int input_len, unsigned char *output, unsigned int output_len)
{
  return qsee_generic_syscall(SYSNUM(qsee_oem_process_cmd), (unsigned int)input, input_len, (unsigned int)output, output_len, 0,0);
}

//QSEE message passing:
int qsee_encapsulate_inter_app_message(char* dest_app_name, uint8* msg_buf, uint32 msg_len, uint8* out_buf, uint32* out_len)
{
  return qsee_generic_syscall(SYSNUM(qsee_encapsulate_inter_app_message), (unsigned int)dest_app_name, (unsigned int)msg_buf, (unsigned int)msg_len, (unsigned int)out_buf, (unsigned int)out_len, 0);
}
int qsee_decapsulate_inter_app_message(char* source_app_name, uint8* msg_buf, uint32 msg_len, uint8* out_buf, uint32* out_len)
{
  return qsee_generic_syscall(SYSNUM(qsee_decapsulate_inter_app_message), (unsigned int)source_app_name, (unsigned int)msg_buf, (unsigned int)msg_len, (unsigned int)out_buf, (unsigned int)out_len, 0);
}

int qsee_get_secure_state(qsee_secctrl_secure_status_t* status)
{
  return qsee_generic_syscall(SYSNUM(qsee_get_secure_state), (unsigned int)status,0,0,0,0,0);
}

//DALHeap application_heap;
mem_heap_type application_heap;
/*Memheap magic number arrays which needs to be passed in to all the memheap functions*/
static uint32 magic_num[MAX_HEAP_INIT] = {(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1};
static uint16 magic_num_index_array[MAX_HEAP_INIT] = {0,1,2,3,4};
static mem_magic_number_struct mem_magic_number;

void* qsee_malloc(size_t size)
{
  return mem_malloc(&application_heap, &mem_magic_number,size);
}

void qsee_free(void *ptr)
{
  mem_free(&application_heap,&mem_magic_number, ptr);
}

void tzlib_app_entry(void)
{
  __cpp_initialize__aeabi_();

  mem_magic_number.magic_num=&magic_num[0];
  mem_magic_number.magic_num_index_array=&magic_num_index_array[0];
  mem_magic_number.magic_num_index=0;
  //DALHeap_Init( &application_heap, (void*)&Image$$TZ_APP_HEAP$$ZI$$Base, (unsigned int)&Image$$TZ_APP_HEAP$$ZI$$Length);
  mem_init_heap( &application_heap, &mem_magic_number,(void*)&Image$$TZ_APP_HEAP$$ZI$$Base, (unsigned int)&Image$$TZ_APP_HEAP$$ZI$$Limit - (unsigned int)&Image$$TZ_APP_HEAP$$ZI$$Base);
  //DALHeap_Init( &application_heap, (void*)&Image$$TZ_APP_HEAP$$ZI$$Base, TZ_APP_HEAP_SIZE);
  //DALHeap_Init( &application_heap, TZ_APP_HEAP, TZ_APP_HEAP_SIZE);

  tz_app_init();
  /*Once the next request comes in, it will be serviced in tz_app_cmd_handler*/
  while (1) {
    /*Generate random canary for stack protection on every incoming command*/
    qsee_prng_getdata((uint8*)__stack_chk_guard, 4);
    wait_for_next_request();
  }
}
