/*
@file app_main.c
@brief App main entry point.

*/
/*===========================================================================
   Copyright (c) 2011-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/sampleapp/src/app_main.c#2 $
  $DateTime: 2016/11/17 01:02:11 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 10/06/16   dpk     Removed BLSP driver from QSEE tests.
# 04/25/16   dpk     Added BLSP driver to QSEE tests
# 12/19/13    wt     Added commands and logic for Content_Protection_Copy API
===========================================================================*/
#include <comdef.h>
//#include <rt_misc.h>
#include <string.h>
#include "qsee_log.h"
#include "qsee_services.h"
#include "qsee_timer.h"
#include "qsee_heap.h"
#include "qsee_fuse.h"
#include "app_content_protection.h"

/* commands supported from sample client */
#define CLIENT_CMD0_GET_VERSION             0
#define CLIENT_CMD1_BASIC_DATA              1
#define CLIENT_CMD2_REGISTER_SB_PTR         2
#define CLIENT_CMD3_RUN_CRYTPO_TEST         3
#define CLIENT_CMD4_RUN_CRYTPO_PERF         4
#define CLIENT_CMD5_RUN_SFS_TEST            5
#define CLIENT_CMD6_RUN_FS_TEST             6
#define CLIENT_CMD7_RUN_RSA_TEST            7
#define CLIENT_CMD8_RUN_RSA_PERF_TEST       8             
#define CLIENT_CMD9_RUN_CMNLIB_TEST         9                  
#define CLIENT_CMD10_RUN_CORE_TEST          10   
#define CLIENT_CMD11_RUN_SECURECHANNEL_TEST 11  
#define CLIENT_CMD12_RUN_MESSAGE_PASSING    12
#define CLIENT_CMD13_RUN_MISC_TEST          13     //14 and 15 are used for RPMB operations in the sample client.       
#define CLIENT_CMD16_RUN_BUSES_TEST         16     
#define CLIENT_CMD17_RUN_STOR_TEST          17  
#define CLIENT_CMD18_RUN_FUSE_TEST          18 
#define CLIENT_CMD19_RUN_BUSES_SPI_TEST     19    
#define CLIENT_CMD20_RUN_HANDLE_TEST        20          
#define CLIENT_CMD21_RUN_CRYPTO_COPY        21
#define CLIENT_CMD25_RUN_SAFE_TEST          25
#define CLIENT_CMD27_RUN_GP_PO_TEST         27

#define __64KB__ 0x10000
#define __32KB__ 0x8000
#define __16KB__ 0x4000
#define __8KB__ 0x2000
#define __4KB__ 0x1000
#define __2KB__ 0x800
#define __1KB__ 0x400

/* Error code: status sent as response to command from sample client*/
								  									  
#define SUCCESS 0
#define FAILED -1
#define FAIL_REGISTER_SB    -2
#define FAIL_DEREGISTER_SB  -3
#define FAIL_PREP_SB_NON_SECURE_READ -4
#define FAIL_PREP_SB_SECURE_READ     -5
#define FAIL_CMD_BUF_TOO_SMALL_FOR_CMD_ID     -6
#define FAIL_CMD_RSP_BUFS_TOO_SMALL           -7
#define FAIL_SHARED_BUFF_TOO_SMALL            -8

#define SAMPLE_APP_VERSION_MAJOR 0x1
#define SAMPLE_APP_VERSION_MINOR 0x2

#define SHARED_BUF_PATTERN_LEN 16

extern int tz_app_crypto_test(uint32 src, uint32 dst, uint32 length);
extern int tz_app_crypto_func_test(void);
extern int tz_app_crypto_perf_test(void);
extern int tz_app_sfs(void);
extern int tz_app_crypto_rsa_test(void);
extern int run_core_test(void* cmd, uint32 cmdlen, void* rsp, uint32 rsplen, void* data, uint32 len);
extern int run_misc_test(void);
extern int run_securechannel_test(void); 
extern int tz_app_cmnlib_test(void);
extern int run_message_passing_test(void);
extern int run_buses_test(void);
extern int run_fuse_test(void);
extern int run_sw_fuse_test(void);
extern int run_stor_test(void);
extern int run_buses_spi_test(void);
extern int tz_app_counter(void);
extern int run_handle_test(void);
extern int security_state_test(void);
extern int tz_app_gpPersistObj(void);

/** 
 * Sample app name 
 * Modify the app name to your specific app name  
 */
char TZ_APP_NAME[] = {"sampleapp"};


/**
  @brief 
    Add any app specific initialization code here
    QSEE will call this function after secure app is loaded and
    authenticated
*/
void tz_app_init(void)
{
  /*  App specific initialization code*/  
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "SAMPLEApp Init ");
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "Version: %x.%x", SAMPLE_APP_VERSION_MAJOR, SAMPLE_APP_VERSION_MINOR);
 
}

/**
  @brief 
    Data structure 

  @param[in]   cmd_id      Requested command
  @param[in]   data        information (could be data or a pointer to the memory that holds the data
  @param[in]   len         if data is ptr to some buffer, len indicates length of the buffer
  @param[in]   test_buf_size  When running crypto test, this indicates the test packet size      
*/
typedef struct send_cmd{
     uint32 cmd_id;
     uint32 data;
     uint32 data2;
     uint32 len;
     uint32 start_pkt;
     uint32 end_pkt;
	 uint32 test_buf_size;
 } send_cmd_t;

typedef struct send_cmd_rsp{
   uint32 data;
   int32 status;
 } send_cmd_rsp_t;

/*Timing vars used for crypto performance test*/
unsigned long long b4 = 0;
unsigned long long after = 0;
unsigned long long time_diff = 0;
unsigned long long average_time_diff = 0;

void tz_app_cmd_handler(void* cmd, uint32 cmdlen, 
                        void* rsp, uint32 rsplen)
{
  /* Request-response buffers are allocated by non-secure side*/
  /* They are MPU protected by QSEE kernel before reaching here*/
  /* Add code to process requests and set response (if any)*/
  int retval;
  int  j; 
  uint32 i = 0;
  uint32 cmd_id;
  uint32 expected_cmdlen;
  uint32 expected_rpslen;
  unsigned char *ptr;
  struct send_cmd *cmd_ptr = (struct send_cmd *)cmd;
  struct send_cmd_rsp *rsp_ptr = (struct send_cmd_rsp *)rsp;

  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "Yash TZ app handler"); 
  /*First we check if the response pointer is large enough to support a pass/fail response*/
  if(rsplen < sizeof(send_cmd_rsp_t)) 
  {
    QSEE_LOG(QSEE_LOG_MSG_FATAL, "Response buffer len insufficient, ERROR OUT");
    return; 
  }

  /*Determine the command id*/
  /*We check if the command buffer is large enough to support the uint32 read for cmd_id*/
  /*It is assumed that the first member of the command buffer is the cmd_id*/
  if(cmdlen < sizeof(uint32))
  {
    QSEE_LOG(QSEE_LOG_MSG_FATAL, "Command buffer len insufficient for reading cmd_id, ERROR OUT");
    rsp_ptr->status = FAIL_CMD_BUF_TOO_SMALL_FOR_CMD_ID;
    return;
  }
  cmd_id = cmd_ptr->cmd_id;
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "TZ App cmd handler, cmd_id = %d", cmd_id);

  /*Identify which command and response buffers we are using based on command ID*/
  if(cmd_id == CLIENT_CMD21_RUN_CRYPTO_COPY)
  {
    expected_cmdlen = sizeof(tz_sc_copy_req_t);
    expected_rpslen = sizeof(tz_sc_copy_rsp_t);
  }
  else
  {
    expected_cmdlen = sizeof(send_cmd_t);
    expected_rpslen = sizeof(send_cmd_rsp_t);
  }

  /*Validate the command buffer and response buffer are the correct size. 
    If not, the the MPU protection and ns_range checks done by QSEE kernel might be insufficient*/
  if(cmdlen < expected_cmdlen || rsplen < expected_rpslen)
  {
    QSEE_LOG(QSEE_LOG_MSG_FATAL, "Cmd/rsp buffer lens insufficient - %x, %x, ERROR OUT", cmdlen, rsplen);
    rsp_ptr->status = FAIL_CMD_RSP_BUFS_TOO_SMALL;
    return;
  }

  switch (cmd_id) {
  case CLIENT_CMD0_GET_VERSION :
	   /* Return major version of the sample test app*/
	  rsp_ptr->data = SAMPLE_APP_VERSION_MAJOR;
	  rsp_ptr->status = SUCCESS;	
  break;

  case CLIENT_CMD1_BASIC_DATA :
	  /*Modify response by 10 */
	  rsp_ptr->data = cmd_ptr->data * 10;
	  rsp_ptr->status = SUCCESS;
  break;

  case CLIENT_CMD2_REGISTER_SB_PTR :
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "****Begin Register Shared Buffer Test (CMD_ID 2) ****");
        if(cmd_ptr->len < SHARED_BUF_PATTERN_LEN)
        {
          rsp_ptr->status = FAIL_SHARED_BUFF_TOO_SMALL;
          QSEE_LOG(QSEE_LOG_MSG_DEBUG, "****Shared Buffer test FAILED because shared buffer len too small.");   
          return;
        }
	  ptr = (unsigned char *)(cmd_ptr->data);
	  retval = qsee_register_shared_buffer(ptr, cmd_ptr->len);
	  if (retval) {
		  rsp_ptr->status = FAIL_REGISTER_SB;
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "****Shared Buffer test FAILED at qsee_register_shared_buffer()");
		  return;
	  }
	  retval = qsee_prepare_shared_buf_for_secure_read(ptr, cmd_ptr->len);
	  if (retval) {
		  rsp_ptr->status = FAIL_PREP_SB_SECURE_READ;
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "****Shared Buffer test FAILED at qsee_prepare_shared_buf_for_secure_read()");
      retval = qsee_deregister_shared_buffer((void *)cmd_ptr->data);
		  return;
	  }
	  /*Check for the pattern and update that in shared buffer*/
      for (j = 0; j < SHARED_BUF_PATTERN_LEN; j++) {
		  i = *ptr;
		  i = i + 10;
		  *ptr = i;
		  ptr++;
	  }
	  retval = qsee_prepare_shared_buf_for_nosecure_read((void *)cmd_ptr->data, cmd_ptr->len);
	  if (retval) {
		  rsp_ptr->status = FAIL_PREP_SB_NON_SECURE_READ;
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "****Shared Buffer test FAILED at qsee_prepare_shared_buf_for_nosecure_read()");
      return;
	  }
	  retval = qsee_deregister_shared_buffer((void *)cmd_ptr->data);
	  if (retval) {
		  rsp_ptr->status = FAIL_DEREGISTER_SB;
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "****Shared Buffer test FAILED at qsee_deregister_shared_buffer()");
		  return;
	  }
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "****Shared Buffer test PASSED  (CMD_ID 2)");
  break;

  case CLIENT_CMD3_RUN_CRYTPO_TEST:
	  ptr = (unsigned char *)(cmd_ptr->data);
	  retval = qsee_register_shared_buffer(ptr, cmd_ptr->len);
	  if (retval) {
		  rsp_ptr->status = FAIL_REGISTER_SB;
		  break;
	  }

	  retval = qsee_prepare_shared_buf_for_secure_read(ptr, cmd_ptr->len);
	  if (retval) {
		  rsp_ptr->status = FAIL_PREP_SB_SECURE_READ;
		  break;
	  }

	  //Run crypto functional tests
	  retval = tz_app_crypto_func_test();
	  if(retval) 
	  {
	    rsp_ptr->status = FAILED;
	  }
	  else 
	  {
	    rsp_ptr->status = SUCCESS;	
	  }

	  retval = qsee_prepare_shared_buf_for_nosecure_read((void *)cmd_ptr->data, cmd_ptr->len);
	  if (retval) {
		  rsp_ptr->status = FAIL_PREP_SB_NON_SECURE_READ;
	  }
	  retval = qsee_deregister_shared_buffer((void *)cmd_ptr->data);
	  if (retval) {
		  rsp_ptr->status = FAIL_DEREGISTER_SB;
	  }
  break;

  case CLIENT_CMD4_RUN_CRYTPO_PERF:
	
	  ptr = (unsigned char *)(cmd_ptr->data);
	  retval = qsee_register_shared_buffer(ptr, cmd_ptr->len);
	  if (retval) {
		  rsp_ptr->status = FAIL_REGISTER_SB;
		  break;
	  }
	  retval = qsee_prepare_shared_buf_for_secure_read(ptr, cmd_ptr->len);
	  if (retval) {
		  rsp_ptr->status = FAIL_PREP_SB_SECURE_READ;
		  break;
	  }

	  //Run crypto performance tests
	  retval = tz_app_crypto_perf_test();
	  if(retval)	 
	  {
	    rsp_ptr->status = FAILED;
	  } 
	  else 
	  {
	    rsp_ptr->status = SUCCESS;	
	  }

	  if(retval)	 
	  {
	     rsp_ptr->status = FAILED;
	     QSEE_LOG(QSEE_LOG_MSG_DEBUG, "    crypto test failed for pkt_size = %d", i);
	     rsp_ptr->data= average_time_diff;   
	  } else
	  {
	  	rsp_ptr->status = SUCCESS;	
	    rsp_ptr->data= average_time_diff; 
	  }
	  retval = qsee_prepare_shared_buf_for_nosecure_read((void *)cmd_ptr->data, cmd_ptr->len);
	  if (retval) {
		  rsp_ptr->status = FAIL_PREP_SB_NON_SECURE_READ;
		  return;
	  }
	  retval = qsee_deregister_shared_buffer((void *)cmd_ptr->data);
	  if (retval) {
		  rsp_ptr->status = FAIL_DEREGISTER_SB;
	  }
  break;
	  
  case CLIENT_CMD5_RUN_SFS_TEST :
    retval = tz_app_sfs();
	  if (retval) {
		  rsp_ptr->status = FAILED;
	  } else {
		  rsp_ptr->status = SUCCESS;
	  }

    if (!retval) 
    {
    /* Must run tz_app_sfs first,
    tz counter root folder is set through qsee_sfs_init  */
    retval = tz_app_counter();
	  if (retval) {
		  rsp_ptr->status = FAILED;
	  } else {
		  rsp_ptr->status = SUCCESS;
	  }
    }
  break;
	  
  case CLIENT_CMD7_RUN_RSA_TEST :
	  retval = tz_app_crypto_rsa_test();
	  if (retval) {
		  rsp_ptr->status = FAILED;
	  } else {
		  rsp_ptr->status = SUCCESS;
	  }
  break;
      
      /*
  case CLIENT_CMD8_RUN_RSA_PERF_TEST :
	  retval = SUCCESS;
	  if (retval) {
		  rsp_ptr->status = FAILED;
	  } else {
		  rsp_ptr->status = SUCCESS;
	  }
  break;
	  */

  case CLIENT_CMD9_RUN_CMNLIB_TEST:
    retval = tz_app_cmnlib_test();
    if (retval) {
      rsp_ptr->status = FAILED;
    } else {
      rsp_ptr->status = SUCCESS;
    }
  break;

  case CLIENT_CMD10_RUN_CORE_TEST :
	  retval = run_core_test(cmd, cmdlen, rsp, rsplen, (void*)cmd_ptr->data, cmd_ptr->len);
	  if (retval) {
		  rsp_ptr->status = FAILED;
	  } else {
		  rsp_ptr->status = SUCCESS;
	  }
  break;

  /*
  case CLIENT_CMD11_RUN_SECURECHANNEL_TEST :
	  retval = run_securechannel_test();
	  if (retval) {
		  rsp_ptr->status = FAILED;
	  } else {
		  rsp_ptr->status = SUCCESS;
	  }
  break;  
	*/

  case CLIENT_CMD12_RUN_MESSAGE_PASSING:
    retval = run_message_passing_test();
	  if (retval) {
		  rsp_ptr->status = FAILED;
	  } else {
		  rsp_ptr->status = SUCCESS;
	  }
  break;


  case CLIENT_CMD13_RUN_MISC_TEST :
	  retval = run_misc_test();
	  if (retval) {
		  rsp_ptr->status = FAILED;
	  } else {
		  rsp_ptr->status = SUCCESS;
	  }
  break;
	  
/* If this test runs concurrently with a touch or power event, an xPU violation
   will be triggered.   
*/
#if 0
  case CLIENT_CMD16_RUN_BUSES_TEST:
    retval = run_buses_test();
    if (retval) {
      rsp_ptr->status = FAILED;
    } else {
      rsp_ptr->status = SUCCESS;
    }
    break;
#endif

  case CLIENT_CMD17_RUN_STOR_TEST :
    retval = run_stor_test();
    if (retval) {
      rsp_ptr->status = FAILED;
    } else {
      rsp_ptr->status = SUCCESS;
    }
    break;

  case CLIENT_CMD18_RUN_FUSE_TEST:
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "----- BEGIN HARD FUSE TESTS -------");
    retval = run_fuse_test();
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "----- END HARD FUSE TESTS -------");

    QSEE_LOG(QSEE_LOG_MSG_ERROR, "--- BEGIN SW-FUSE TESTS -------");
    retval |= run_sw_fuse_test();
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "--- END SW-FUSE TESTS -------");

    if (retval) {
      rsp_ptr->status = FAILED;
    } else {
      rsp_ptr->status = SUCCESS;
    }
  break;

	case CLIENT_CMD19_RUN_BUSES_SPI_TEST:
    retval = run_buses_spi_test();
    if (retval) {
      rsp_ptr->status = FAILED;
    } else {
      rsp_ptr->status = SUCCESS;
    }
    break;

  case CLIENT_CMD20_RUN_HANDLE_TEST:
    retval = run_handle_test(); 
    if (retval) {
      rsp_ptr->status = FAILED;
    } else {
      rsp_ptr->status = SUCCESS;
    }
    break;

    
  case CLIENT_CMD21_RUN_CRYPTO_COPY: 
       // copyDir = 1 means Secure to Non-secure
       if((uint32)(((tz_sc_copy_req_t*)cmd)->copyDir))
       {
#ifdef USES_NO_CP
          retval = Content_Protection_Copy_SecureToNonsecure((uint8*)(((tz_sc_copy_req_t*)cmd)->nonSecBuffer),
                  (uint32)(((tz_sc_copy_req_t*)cmd)->nonSecBufferLength),
                  (tz_mem_array_s_t*)(&(((tz_sc_copy_req_t*)cmd)->secBufferHandle)),
                  (uint32)(((tz_sc_copy_req_t*)cmd)->secBufferOffset),
                  (uint32*)&((tz_sc_copy_rsp_t*)rsp)->secBufferLength);
#else
          retval = FAILED;
          QSEE_LOG(QSEE_LOG_MSG_ERROR, "This feature is not supported on external builds...");
#endif
          ((tz_sc_copy_rsp_t *)rsp)->ret = retval;
       }
       // copyDir = 0 means Non-secure to secure
       else
       {
          retval = Content_Protection_Copy_NonsecureToSecure((uint8*)(((tz_sc_copy_req_t*)cmd)->nonSecBuffer),
                   (uint32)(((tz_sc_copy_req_t*)cmd)->nonSecBufferLength),
                   (tz_mem_array_s_t*)(&(((tz_sc_copy_req_t*)cmd)->secBufferHandle)),
                   (uint32)(((tz_sc_copy_req_t*)cmd)->secBufferOffset),
                   (uint32*)(&((tz_sc_copy_rsp_t*)rsp)->secBufferLength));
          ((tz_sc_copy_rsp_t*)rsp)->ret = retval;
       }
       break;

  case CLIENT_CMD25_RUN_SAFE_TEST:
     retval = security_state_test();
     rsp_ptr->status = retval;
     break;

  case CLIENT_CMD27_RUN_GP_PO_TEST:
     rsp_ptr->status = (0 == tz_app_gpPersistObj()) ? SUCCESS : FAILED;
     break;
	 
  default :
	 rsp_ptr->status = SUCCESS;
	 QSEE_LOG(QSEE_LOG_MSG_DEBUG, "Unsupported command: %d.  Test not yet implemented or commnented out.", cmd_ptr->cmd_id);
  break;
  }
}


/**
  @brief 
    App specific shutdown
    App will be given a chance to shutdown gracefully
*/
void tz_app_shutdown(void)
{
  /* app specific shutdown code*/
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "SAMPLE App shutdown");
  return;
}
