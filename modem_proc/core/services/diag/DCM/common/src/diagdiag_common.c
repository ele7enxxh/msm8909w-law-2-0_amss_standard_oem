/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Common Routines

General Description
  Core diagnostic packet processing routines that are common to all targets.

Copyright (c) 2000-2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagdiag_common.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/03/16   gn      Added support for 12k log packet
01/07/16   ph      Diag log packet support for ULog
06/18/15   sa      Added support for time sync feature.
07/20/15   ph      Fixed compiler warnings.
02/18/15   sa      Fixed KW Warnings.
11/29/14   sa      Delete the sleep timer when it expires.
11/25/14   ph      Removed test case DIAGDIAG_STRESS_TEST_ERR_FATAL_ISR.
12/09/14   sa      Added the missing break in 4k extended log packet handling
06/19/14   sa      Added support to send an event to debug diag drops
07/16/14   xy      Update Diag delayed response command to take parameter  
04/24/14   tbg     Added stress tests for log_submit and log_submit_ext APIs 
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
12/20/13   rs      Ported LLVM warning fix.
11/01/13   sr      Diag support for log-on-demand extended 
10/14/13   sr      Added stress tests for extended APIs  
07/25/12   sr      Support max req pkt len commands   
09/20/13   tbg     Added support for FEATURE_DIAG_STRESS_PROFILE
04/03/13   sr      Replace memcpy() and memmove() with memscpy() and memsmove()
02/08/13   sr      Fixed compiler  and Klockwork warnings 
12/19/12   rh      Fixed Klockwork-detected bugs
12/14/12   is      Use correct define for diagbuf_commit_threshold
12/07/12   is      Include preset_id in set preset_id response
11/15/12   is      Support for preset masks
09/13/12   sa      Added MSG_SPRINTF_* F3 message generations.
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes 
08/24/12   ra      Support for querying Command Registration Tables     
08/22/12   sr      Support for mask retrieval command 
08/10/12   sa      Replaced DIAG_REQ_FWD with DIAG_MP_MASTER
07/20/12   sr      Added processor id to msg stress test   
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap. 
03/26/12   hvm     Fix compiler warnings for diagdiag_ext_listener_test
03/26/12   hvm     additional F3 msgs for event and log callback funcs
03/26/12   hvm     Extended Event/Log Listener APIs added    
11/07/11   is      Fixed compiler warning
10/14/11   hm      Diag consumer API - modem side changes 
09/28/11   is      Support dynamic sleep voting and non-deferrable Diag timers   
08/03/11   hm      QDSP6 specific change commit threshold command     
07/19/11   hm      Non-aggregation feature for events
06/14/11   hm      Migrating to REX opaque TCB APIs
06/13/11   sg      Diag Stress test votes against power collapse
                   on lpass when stress test is running
03/04/11   is      Support for RIVA Health and stress test commands      
03/01/11   is      Migrate to diag_strlcpy()
02/15/11   vs      Changed registration for qdsp6 for 9k
02/15/11   hm      QPST Cookie Changes decoupling USB header file    
02/01/11   hm      Dual Mask Changes     
01/24/11   hm      Added Query/Save QPST Cookie Command handler
01/24/11   is      Define SLEEP_PRI when build does not have sleep feature (decoupling)
12/10/10   vs      strncpy for q6 instead of strlcpy
12/06/10   hm      Added Timestamp to Subsystem loopback command response     
12/03/10   hm      Added unique identifier field to fix BVT stress test failures
11/22/10   mad     Initialized name for diagdiag_memop_tbl_mutex 
07/27/10   vs      Added diag command to switch encoding protocols
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
08/25/10   sg      Fixed compiler warnings
07/15/10   sg      Moved declaration of diagdiag_memop_tbl_mutex to here 
                   from diagdiag.c
07/10/10   vs      Added support for diagpeek/poke registration API
07/10/10   vs      Added check for bad length in command handlers
06/22/10   sg      DIAGDIAG_START_STRESS_TEST_F is defined to the appropriate
                   command id on the processor
06/10/10   is      Core image feature flag cleanup
06/04/10   sg      Fix to provide the correct delayed response drop count 
                   when diag health commands for delayed responses are sent 
04/05/10   JV      Added test cases for QSR messages in the stress test
03/02/10   JV      Reduced stack size for stress test to 1K. Use system heap 
                   instead of diag heap to allocate for stress test stack.
03/02/10   JV      New command to enable/disable data aggregation
02/25/10   JV      New command to flush diagbuf.
02/23/10   JV      Fixed bug in event_stress_test_completed
02/18/10   JV      Fix klocwork errors
02/02/10   JV      Relative priority for the ADSP
01/22/10   mad     Stress-test-task sleeps for 1 s before sending out 
                   EVENT_DIAG_STRESS_TEST_COMPLETED. This is to ensure that 
                   the event gets allocated, and APS does not miss the event.
01/20/10   sg      Relative Priority Changes For Win Mobile
01/18/10   mad     Sending out EVENT_DIAG_STRESS_TEST_COMPLETED before marking
                   stress-test-task for clean-up.
12/17/09   JV      Mainlining the diagdiag stress test and other functioanlity
                   under the DEBUG_DIAG_TEST feature.
12/17/09   JV      Switching over to BLAST APIs in OSAL
12/16/09   cahn    Corrected EVENT_DIAG_STRESS_TEST_COMPLETED event generation
                   logic in support of test automation.
12/09/09   cahn    Fixed stress test on QDSP6 processor. Removed mutexes and
                   resolved compilation error.
12/07/09   cahn    Added Delayed Response to DIAG Health.
12/02/09   cahn    Fixed bug in DIAGDIAG_STRESS_TEST_MSG_1.
12/02/09   JV      Fix for warning that is treated as an error in WM.
11/12/09   cahn    Added variable length log types for DIAG stress test.
11/09/09   cahn    Defined DIAG_DEBUG_TEST feature. Made necessary code changes
                   to avoid compilation errors.
11/04/09   cahn    DIAG Health
09/25/09   sg      Cleaned up diagdiag.c and moved the functions specific to modem
                   to diagdiag_brew.c and removed dip switches
10/22/09   JV      Include queue.h
10/06/09   cahn    Added EVENT_DIAG_STRESS_TEST_COMPLETED for automation support.
10/06/09   vs      Added payload to LOG_STRESS_TEST_C_type
09/15/09   cahn    Added loopback on individual processors.
09/09/09   JV      Changed signature of osal mutex calls.
08/11/09   JV      Set the malloced memory to 0 in the stress test.
08/07/09   vs      Added DIAG_SINGLE_PROC for targets that don't define
                    IMAGE_MODEM_PROC, but are single proc targets.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
08/05/09   JV      task.h is now included in osal.
07/31/09   JV      Removed all the KxMutex calls
07/31/09   JV      Merged Q6 diag code back to mainline
07/10/09    as     Mainlined code under FEATURE_DIAG_HW_ADDR_CHECK
05/12/09    JV     Introduced the OS abstraction layer for rex.
05/07/09   vk      changed includes from external to internal headers
01/15/09    as     Decoupled Err services packets.
10/03/08   vg      Updated code to use PACK() vs. PACKED
11/19/07    pc     Use dword transfers for PEEKD/POKED if address is aligned.
11/19/07    pc     Use word transfers for PEEKW/POKEW if address is aligned.
12/15/06    as     Fixed compiler warnings.
10/31/05    as     Fixed lint errors.
05/19/05    as     Fixed argument name in DIAG POKE DWORD 
22/03/05    as     Added type cast in memory poke operations
06/11/04    gr     Added support for event masks.
05/18/04    as     Added security check to diagdiag_get_property and 
                   diagdiag_put_property. Removed support for DIAG_USER_CMD_F 
                   & DIAG_PERM_USER_CMD_F
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
04/07/03   jct     Added featurization to dissallow certain operations that
                   can access memory (FEATURE_DIAG_DISALLOW_MEM_OPS):
                      DIAG_PEEKB_F
                      DIAG_PEEKW_F
                      DIAG_PEEKD_F
                      DIAG_POKEB_F
                      DIAG_POKEW_F
                      DIAG_POKED_F
                      DIAG_OUTP_F
                      DIAG_OUTPW_F
                      DIAG_INP_F
                      DIAG_INPW_F
01/07/03   djm     add FEATURE_DIAG_RPC support for WCDMA_PLT use
08/20/02   lad     Moved diagpkt_error() to diagpkt.c.
                   Moved diagpkt_mode_chg() to diagcomm_sio.c as
                   diagcomm_mode_chg_pkt().
                   Removed msg_nice[] and log_nice[] references pending a
                   redesign of the load balancing feature.
                   Moved diagpkt_dload() to diagdload.c.
                   Featurized outp/inp routines for off target removal.
06/07/02   lad     Added DIAG_PROTOCOL_LOOPBACK_F.
11/01/01   jal     Support for DIAG_SERIAL_CHG_F (switch to data mode)
09/18/01   jal     Support for DIAG_CONTROL_F (mode reset/offline)
08/20/01   jal     Support for Diag packet: DIAG_TS_F (timestamp),
                   DIAG_SPC_F (service programming code), DIAG_DLOAD_F
                   (start downloader), DIAG_OUTP_F/DIAG_OUTPW_F (IO
                   port byte/word output), DIAG_INP_F/DIAG_INPW_F (IO
                   port byte/word input) 
06/27/01   lad     Moved diagpkt_err_rsp() to diagpkt.c.
                   Updated diagpkt_stream_config() for logging service.
                   diagpkt_stream_config() can now return DIAGBUF_SIZ.
04/17/01   lad     Added #include of diagtune.h.
04/06/01   lad     Introduced typedefs for command codes, etc.
                   Updated DIAGPKT_SUBSYS_REQ_DEFINE macros.
                   Added diagpkt_subsys_alloc().
                   Removed diagpkt_process_request since it is not part of the
02/23/01   lad     Created file.

===========================================================================*/
#include "core_variation.h"
#if defined __cplusplus
  extern "C" {
#endif
#ifndef FEATURE_WINCE
#endif
#include "eventi.h"
#include "diag.h"
#include "diaglogi.h"
#include "msgi.h"
#include "comdef.h"
#include "diag_v.h"
#include "diagcmd.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagdiag_v.h"
#include "diagtarget.h"
#include "event.h"
#include "err.h"
#include "diagcomm_v.h"
#include "diagbuf_v.h"
#include "diag_cfg.h"
#include "stringl.h"

#if defined (DIAG_DYNAMIC_SLEEP)
  #include "npa.h" /* For npa_client_handle, etc */
#endif

#if defined(DIAG_QDSP6_APPS_PROC)
#include "diagstub.h" /* For sleep Priority*/
#include "diagMmpm.h"
#endif

#include "assert.h"
#include <stdlib.h> 

#include "msg.h"
#include "qw.h"
#include "queue.h"

#include "osal.h"
#include "ULog.h"
#include "ULogFront.h"

/* DLOAD QPST COOKIE CHANGES */
#ifdef DIAG_FEATURE_QPST_COOKIE
  #include "msm.h"
  #define QPST_COOKIE_FILE_NAME "/qpst_cookie.cke"
#endif

#if defined (DIAG_FEATURE_EFS2)
  #include "fs_public.h"
#endif

#ifdef FEATURE_DIAG_STRESS_PROFILE
  #include "qurt_cycles.h"
  #include "memheap.h"
  extern mem_heap_type amss_mem_heap;
  extern uint64 diag_sigs_pcycle_total,diag_hdlc_pcycles_total, diag_tracer_pcycles_total;
  #include "diag_stm.h"
#endif /* FEATURE_DIAG_STRESS_PROFILE */

/* Internal APIs to handle file system access */
boolean diagdiag_file_write(char *file_name, void *buf, int length);
boolean diagdiag_file_read(char *file_name, void *buf, int length, int offset);

/* Tell lint that we are OK with the fact that pkt_len and req are not
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req) */

osal_thread_attr_t diagdiag_stress_attr;

osal_thread_attr_t diag_cmd_req_attr;
int thread_exit_status = 0;
osal_mutex_arg_t diagdiag_memop_tbl_mutex;

#if defined (DIAG_DYNAMIC_SLEEP)
extern npa_client_handle diagtest_npa_handle;
#endif

extern uint8 diag_max_per_drop_threshold;
extern uint8 diag_max_per_step;

/* Time Get Function Pointer from diag.c */
extern uint8 (*diag_time_get)(qword time);


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

/* stack size for the task created */
#define STACK_SIZE 1024

/* Stress-task sleeps for 1s before sending out EVENT_DIAG_STRESS_TEST_COMPLETED */
#define STRESS_TASK_SLEEP_TO_COMPLETE   1000 

#if (defined (DIAG_IMAGE_MODEM_PROC) || defined(DIAG_SINGLE_PROC)) && !defined (DIAG_IMAGE_QDSP6_PROC)
  /* The ARM9 modem and NOT QDSP6 modem */
  #define DIAGDIAG_START_STRESS_TEST_F DIAGDIAG_START_STRESS_TEST_MODEM_F
#elif defined (DIAG_IMAGE_APPS_PROC) && !defined (DIAG_IMAGE_QDSP6_PROC)
  /* Brew apps and WM apps */
  #define DIAGDIAG_START_STRESS_TEST_F DIAGDIAG_START_STRESS_TEST_APPS_F
#elif defined(DIAG_IMAGE_QDSP6_PROC)
  /* QDSP6 modem */
  #define DIAGDIAG_START_STRESS_TEST_F DIAGDIAG_START_STRESS_TEST_QDSP_F
#elif defined(DIAG_IMAGE_RIVA_PROC)
  /* RIVA */
  #define DIAGDIAG_START_STRESS_TEST_F DIAGDIAG_START_STRESS_TEST_RIVA_F
#endif

/* sleep signals */
#define DIAG_STRESS_TEST_SLEEP_SIG     0x100000

/*
  These are defined for Querying the Apps processor for registration
  tables.
 
  The slave table is identified by the port number. Apps stores the
  registration tables of the other processors in the slave table and
  differentiates them by the port number 
*/

#if defined (DIAG_IMAGE_APPS_PROC)
  #define DIAGDIAG_MODEM_PORT     0x00  /* For quering the Modem Registration Table on the Apps */
  #define DIAGDIAG_QDSP6_PORT     0x01  /* For quering the QDSP6 Registration Table on the Apps */
  #define DIAGDIAG_RIVA_PORT      0x02  /* For quering the RIVA Registration Table on the Apps */
#endif  /* #if defined (DIAG_IMAGE_APPS_PROC) */

#define DIAGDIAG_NO_PORT        0x00  /* This is a dummy value. We don't need port number for querying Master Table */

/* Variables for Registration Table Retrieval commands */
diagpkt_subsys_cmd_code_type reg_tbl_cmd_code;  /*  Command Code */
diagpkt_subsys_delayed_rsp_id_type reg_tbl_delay_rsp_id = 0; /* Retrieval Delayed Response ID */
boolean reg_tbl_is_master = TRUE; /* Flag for master processor */
uint8 reg_tbl_port_id = DIAGDIAG_NO_PORT; /* Port ID to identify processor in Slave Table */
uint8 reg_tbl_proc_id = 0;

typedef enum {
  DIAG_TASK_INITIALIZED = 0, /* Task is initialized */
  DIAG_TASK_RUNNING = 1,     /* Task is running */
  DIAG_TASK_DONE = 2         /* Task is done, and can be removed from the Q*/
} diag_stress_test_status_enum_type;


/* Stores the info of the task, tcb, status and stack */
typedef struct{
  diag_stress_test_status_enum_type status; /* refer to diag_stress_test_status_enum_type*/
  diag_per_task_test_info task_info;        /* Contains info of the task */
  osal_tcb_t tcb;                         /* used to define a task */
  unsigned long long stack[STACK_SIZE];    /* stack needed by the task */
  int unique_id;                          /* Unique identifier to help differentiate tasks */
} diag_tcb_stack_type;

/* Queue for diag stress test */
q_type diag_stress_test_task_q;

/* Queue item for diag stress test */
typedef struct
{
  q_link_type link;   /* Command queue link */
  diag_tcb_stack_type data; /* data block */
}diag_stress_test_task_q_item_type;

/* Stores info, wheather Q is initialised or not */
boolean diag_stress_test_task_q_init = FALSE;

/* Predetermined sequence */
byte pseudo_array[] = {6,2,4,5,3,0,1,2,5,0,3,1,6,4};

/* for repeatabilty use pseudo random numbers */
int pseudo_random(int i)
{
  i=i%(sizeof(pseudo_array)/sizeof(byte));
  if(i<14)
    return pseudo_array[i];
  else
    return 0;
}

/* Structures for stress test logs */
typedef struct {
  log_hdr_type  header;  /* log code header */
  int iteration;         /* stores which iteration */
  int task_priority;     /* stores the priority of the current task */
  int req_iterations;    /* stores num of iterations requested */
  uint32 payload[125];     /* Total log size: 12 + 125 * 4 (payload) = 512 */
}LOG_STRESS_TEST_C_type;

typedef struct {
  log_hdr_type  header;  /* log code header */
  int iteration;         /* stores which iteration */
  int task_priority;     /* stores the priority of the current task */
  int req_iterations;    /* stores num of iterations requested */
  uint32 payload[13];    /* Total log size: 12 + 13 * 4 (payload) = 64 */
}LOG_STRESS_TEST_64_type;

typedef struct {
  log_hdr_type  header;  /* log code header */
  int iteration;         /* stores which iteration */
  int task_priority;     /* stores the priority of the current task */
  int req_iterations;    /* stores num of iterations requested */
  uint32 payload[29];    /* Total log size: 12 + 29 * 4 (payload) = 128 */
}LOG_STRESS_TEST_128_type;

typedef struct {
  log_hdr_type  header;  /* log code header */
  int iteration;         /* stores which iteration */
  int task_priority;     /* stores the priority of the current task */
  int req_iterations;    /* stores num of iterations requested */
  uint32 payload[61];    /* Total log size: 12 + 61 * 4 (payload) = 256 */
}LOG_STRESS_TEST_256_type;

typedef struct {
  log_hdr_type  header;  /* log code header */
  int iteration;         /* stores which iteration */
  int task_priority;     /* stores the priority of the current task */
  int req_iterations;    /* stores num of iterations requested */
  uint32 payload[253];   /* Total log size: 12 + 253 * 4 (payload) = 1024 */
}LOG_STRESS_TEST_1K_type;

typedef struct {
  log_hdr_type  header;  /* log code header */
  int iteration;         /* stores which iteration */
  int task_priority;     /* stores the priority of the current task */
  int req_iterations;    /* stores num of iterations requested */
  uint32 payload[509];    /* Total log size: 12 + 509 * 4 (payload) = 2048 */
}LOG_STRESS_TEST_2K_type;

typedef struct {
  log_hdr_type  header;  /* log code header */
  int iteration;         /* stores which iteration */
  int task_priority;     /* stores the priority of the current task */
  int req_iterations;    /* stores num of iterations requested */
  uint32 payload[1021];    /* Total log size: 12 + 1021 * 4 (payload) = 4096 */
}LOG_STRESS_TEST_4K_type;

typedef struct{
  log_hdr_type  header;  /* log code header */
  int iteration;         /* stores which iteration */
  int task_priority;     /* stores the priority of the current task */
  int req_iterations;    /* stores num of iterations requested */
  uint32 payload[1533];  /* Total log size: 12 + 1533 * 4 (payload) = 6144 */
}LOG_STRESS_TEST_6K_type;

typedef struct{
  log_hdr_type  header;  /* log code header */
  int iteration;         /* stores which iteration */
  int task_priority;     /* stores the priority of the current task */
  int req_iterations;    /* stores num of iterations requested */
  uint32 payload[2173];  /* Total log size: 12 + 2173 * 4 (payload) = 8704 */
}LOG_STRESS_TEST_8K5_type;

typedef struct{
  log_hdr_type  header;  /* log code header */
  int iteration;         /* stores which iteration */
  int task_priority;     /* stores the priority of the current task */
  int req_iterations;    /* stores num of iterations requested */
  uint32 payload[3069];  /* Total log size: 12 + 3069 * 4 (payload) = 12288 */
}LOG_STRESS_TEST_12k_type;

typedef struct{
  int iteration;         /* stores which iteration */
  int task_priority;     /* stores the priority of the current task */
  int req_iterations;    /* stores num of iterations requested */
}EVENT_PAYLOAD_INFO_type;

extern int diagbuf_commit_threshold;
extern unsigned int event_report_pkt_size;
extern ULogHandle diag_err_ulog_handle;

/*===========================================================================     
     
FUNCTION DIAGDIAG_FILE_WRITE     
     
DESCRIPTION     
    This procedure writes the data in "buf" of length "length" to the 
    file "file_name". New file will be created if the file is not present. 
    Overwritten if the file is present.
     
RETURN VALUE     
    Returns TRUE on success and FALSE on failure  
=============================================================================*/
boolean diagdiag_file_write(char *file_name, void *buf, int length)
{
  boolean return_val = FALSE;
  #ifdef DIAG_FEATURE_EFS2
  int file_descriptor, wr_bytes;
  if(buf == NULL)
  {
    return return_val;
  }
  file_descriptor = efs_open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR);
  if(file_descriptor == -1) /* Error opening file */
  {
    return return_val;
  }
  wr_bytes = efs_write(file_descriptor, buf, length);
  if(wr_bytes == -1) /* Error Writing to file */
  {
    (void)efs_close(file_descriptor);
    return return_val;
  }
  (void)efs_close(file_descriptor);
  return (return_val = TRUE);
  #endif

  return return_val;
} /* diagdiag_file_write */

/*===========================================================================     
     
FUNCTION DIAGDIAG_FILE_READ    
     
DESCRIPTION     
    This procedure reads the data of length "length" from the file 
    "file_name" to the buffer "buf". "offset" indicates the offset 
    from the begining of the file to read the data from. 
         
RETURN VALUE     
    Returns TRUE on success and FALSE on failure  
=============================================================================*/
boolean diagdiag_file_read(char *file_name, void *buf, int length, int offset)
{
  boolean return_val = FALSE;
  #ifdef DIAG_FEATURE_EFS2
  int file_descriptor, rd_bytes;
  if(buf == NULL)
  {
    return return_val;
  }
  file_descriptor = efs_open(file_name, O_RDONLY);
  if(file_descriptor == -1)
  {
    return return_val;
  }
  if(offset != 0)
  {
    rd_bytes = efs_lseek(file_descriptor, offset, SEEK_SET);
    if(rd_bytes != offset)
    {
      (void)efs_close(file_descriptor);
      return return_val;
    }
  }
  rd_bytes = efs_read(file_descriptor, buf, length);
  if(rd_bytes == -1 )
  {
    (void)efs_close(file_descriptor);
    return return_val;
  }
  (void)efs_close(file_descriptor);
  return (return_val = TRUE);
  #endif

  return return_val;
} /* diagdiag_file_read */

/*lint -save -e729 */
/* Symbol 'diag_prop_table' not explicitly initialized */
/* Also there is no reference to filling this table.  */

/*===========================================================================

FUNCTION DIAGDIAG_LOOPBACK

DESCRIPTION
  This procedure echos the request in the response.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_loopback (
  void * req_pkt,
  uint16 pkt_len
)
{
  void * rsp = NULL;
  
  /* Allocate the same length as the request. */
  rsp = diagpkt_alloc (DIAG_PROTOCOL_LOOPBACK_F, pkt_len);

  if (rsp != NULL) {
    memscpy ((void *) rsp, pkt_len, (void *) req_pkt, pkt_len);
  }

  return (rsp);
}


#ifdef FEATURE_WINCE
#error code not present
#endif /* FEATURE_WINCE */

/*===========================================================================

FUNCTION DIAGDIAG_SUBSYS_LOOPBACK

DESCRIPTION
  This procedure echos the request in the response. This function has equivalent
  functionality to DIAGDIAG_LOOPBACK, except it is used for subsystem commands
  with subsystem dispatch (i.e. cmd code 75).

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_subsys_loopback (
  void * req_pkt,
  uint16 pkt_len
)
{
  /*=========================================================================
   *		RESPONSE PACKET STRUCTURE (Length in Bytes)
   *
   *	   CMD_CODE SUBSYS_ID SUBSYS_CMD_CODE          PAYLOAD
   *	  									                  TIMESTAMP	 ^	  STRING
   *	  -------------------------------------------------------------------	
   *    |		     |		     |		        |				    |				      |
   *    |	 1	   |	  1	   |	   2	    |		  8		  |   VARIABLE	|
   *    -------------------------------------------------------------------
   *
   *========================================================================*/    
	
  

  diag_subsys_loopback_rsp_type *rsp = NULL;
  byte * p = NULL;
  byte * req_pkt_byte = NULL;
  
  #if defined(FEATURE_WINCE) || defined(DIAG_QDSP6_APPS_PROC)
  diagpkt_subsys_header_type *req = (diagpkt_subsys_header_type *) req_pkt;
  #else
  diag_log_event_listener_req_type *req = 
    (diag_log_event_listener_req_type *) req_pkt;
  #endif
  diagpkt_subsys_cmd_code_type cmd_code = diagpkt_subsys_get_cmd_code (req);

  /* Allocate the same length as the request. */
  pkt_len = pkt_len + 1 + 8;  // Timestamp needs 8 bytes
  rsp = (diag_subsys_loopback_rsp_type *)diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, cmd_code, pkt_len);
  
  if (rsp != NULL) {
      
    rsp->header.opaque_header[0] = *((byte *)req_pkt);  // Copy the CMD_CODE (1 Byte)
    /*=========================================================================== 
     * We don't have to copy the Subsys Cmd_code and Subsys_ID. They are already
     * copied to the response packet in diagpkt_subsys_alloc function. Get the
     * timestamp directly copied into the response packet.
     *==========================================================================*/
    diag_time_get((unsigned long *)(rsp->ts));

    /*===========================================================================   
     * Copying the string which is 4 + 8 bytes away from the base in the reponse 
     * packet, 4 bytes away from the base in the request packet as there is no
     * timestamp in the request.
     *==========================================================================*/
    
    p = (byte *)rsp;
    req_pkt_byte = (byte *)req_pkt;	
    memscpy ((void *) (p+12), (pkt_len-1)-12, (void *) (req_pkt_byte+4), (pkt_len-1)-12);
    *(p+(pkt_len-1)) = '\0';
  }

  return (void *) rsp;
}
/*===========================================================================

FUNCTION DIAGDIAG_DELAYED_RSP_TEST

DESCRIPTION
  This procedure tests the delayed response functionality. It sends an   
  immediate response followed by multiple delayed responses. The number
  of delayed rsp generated is passed in by cmd.
   

RETURN VALUE
  None.

============================================================================*/
PACK(void *) diagdiag_delayed_rsp_test (
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  diagdiag_delayed_rsp_test_req_type *req = NULL;
  uint32 delayed_rsp_id = 0;
  uint16 num = 0;
  int i = 0;
  diagdiag_delayed_rsp_test_rsp_type *rsp = NULL;

  if (pkt_len < sizeof(diagdiag_delayed_rsp_test_req_type)) 
  {
     return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  req = (diagdiag_delayed_rsp_test_req_type *)req_pkt;
  
  if (req != NULL) 
  {
     num = req -> num_rsp;

     if (num == 0 || num > 0x0FFF) 
     {
       return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
     }

     rsp = (diagdiag_delayed_rsp_test_rsp_type *)
            diagpkt_subsys_alloc_v2(DIAG_SUBSYS_DIAG_SERV, DIAGDIAG_STRESS_TEST_DELAYED_RSP, 
                                                 sizeof(diagdiag_delayed_rsp_test_rsp_type));
     if(rsp)
     {
       delayed_rsp_id = rsp->delayed_rsp_id;
       /*According to ICD, rsp count always starts at 0x8000*/
       rsp->response_cnt = 0x8000;
       diagpkt_commit(rsp);
     }
     else
     {
       MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Out of memory- Cannot send the response");
     }

     for (i = 0;i < num; i++) {

        rsp = NULL;       
        rsp = (diagdiag_delayed_rsp_test_rsp_type *)diagpkt_subsys_alloc_v2_delay(DIAG_SUBSYS_DIAG_SERV, DIAGDIAG_STRESS_TEST_DELAYED_RSP, 
                                                                                  delayed_rsp_id, sizeof(diagdiag_delayed_rsp_test_rsp_type));
        if(rsp)
        {
           if (i == (num -1)) 
           {
             /*If the packet is the last delayed response, set the response_cnt to be x0nnn*/
             rsp->response_cnt = i+1;
           }
           else
           {
             rsp->response_cnt = i+1+0x8000;
           }
           diagpkt_delay_commit(rsp);
        }
        else
        {
          MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Out of memory- Cannot send the delayed response");
        }

     }
  }
  return NULL;
}
/*===========================================================================

FUNCTION DIAGDIAG_HEALTH_COMMAND

DESCRIPTION
  This procedure performs operations on the global variables used for diag
  health based on the command code.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACK (void *)diagdiag_health_command(
  void * req_pkt,
  uint16 pkt_len
)
{
  /* Initialization */
  diagpkt_subsys_cmd_code_type cmd_code;
  diag_health_response_type *rsp = NULL;

  cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);
  rsp = (diag_health_response_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
                                              cmd_code, sizeof(diag_health_response_type));
                                              
  if ( rsp != NULL ) 
  {
    switch ( cmd_code ) 
    {
      case DIAGDIAG_RESET_DROP_COUNT_LOG_APP:
      case DIAGDIAG_RESET_DROP_COUNT_LOG_MODEM: 
      case DIAGDIAG_RESET_DROP_COUNT_LOG_Q6:
      case DIAGDIAG_RESET_DROP_COUNT_LOG_RIVA:
          diagbuf_reset_drop_count_log();
          rsp->count.drop_count = diagbuf_get_drop_count_log();
          break;
      case DIAGDIAG_GET_DROP_COUNT_LOG_APP:
      case DIAGDIAG_GET_DROP_COUNT_LOG_MODEM:
      case DIAGDIAG_GET_DROP_COUNT_LOG_Q6:
      case DIAGDIAG_GET_DROP_COUNT_LOG_RIVA:
          rsp->count.drop_count = diagbuf_get_drop_count_log();
          break;
      case DIAGDIAG_RESET_DROP_COUNT_EVENT_APP:
      case DIAGDIAG_RESET_DROP_COUNT_EVENT_MODEM: 
      case DIAGDIAG_RESET_DROP_COUNT_EVENT_Q6:
      case DIAGDIAG_RESET_DROP_COUNT_EVENT_RIVA:
          event_reset_drop_count_event();
          rsp->count.drop_count = event_get_drop_count_event();
          break;
      case DIAGDIAG_GET_DROP_COUNT_EVENT_APP:
      case DIAGDIAG_GET_DROP_COUNT_EVENT_MODEM:
      case DIAGDIAG_GET_DROP_COUNT_EVENT_Q6:
      case DIAGDIAG_GET_DROP_COUNT_EVENT_RIVA:
          rsp->count.drop_count = event_get_drop_count_event();
          break;
      case DIAGDIAG_RESET_DROP_COUNT_F3_APP:
      case DIAGDIAG_RESET_DROP_COUNT_F3_MODEM: 
      case DIAGDIAG_RESET_DROP_COUNT_F3_Q6:
      case DIAGDIAG_RESET_DROP_COUNT_F3_RIVA:
          diagbuf_reset_drop_count_f3();
          rsp->count.drop_count = diagbuf_get_drop_count_f3();
          break;
      case DIAGDIAG_GET_DROP_COUNT_F3_APP:
      case DIAGDIAG_GET_DROP_COUNT_F3_MODEM:
      case DIAGDIAG_GET_DROP_COUNT_F3_Q6:
      case DIAGDIAG_GET_DROP_COUNT_F3_RIVA:
          rsp->count.drop_count = diagbuf_get_drop_count_f3();
          break;
      case DIAGDIAG_RESET_DROP_COUNT_DELAY_APP:
      case DIAGDIAG_RESET_DROP_COUNT_DELAY_MODEM: 
      case DIAGDIAG_RESET_DROP_COUNT_DELAY_Q6:
      case DIAGDIAG_RESET_DROP_COUNT_DELAY_RIVA:
          diagpkt_reset_drop_count_delay();
          rsp->count.drop_count = diagpkt_get_drop_count_delay();
          break;
      case DIAGDIAG_GET_DROP_COUNT_DELAY_APP:
      case DIAGDIAG_GET_DROP_COUNT_DELAY_MODEM:
      case DIAGDIAG_GET_DROP_COUNT_DELAY_Q6:
      case DIAGDIAG_GET_DROP_COUNT_DELAY_RIVA:
          rsp->count.drop_count = diagpkt_get_drop_count_delay();
          break;
      case DIAGDIAG_GET_ALLOC_COUNT_LOG_APP:
      case DIAGDIAG_GET_ALLOC_COUNT_LOG_MODEM:
      case DIAGDIAG_GET_ALLOC_COUNT_LOG_Q6:
      case DIAGDIAG_GET_ALLOC_COUNT_LOG_RIVA:
          rsp->count.drop_count = diagbuf_get_alloc_count_log();
          break;
      case DIAGDIAG_GET_ALLOC_COUNT_EVENT_APP:
      case DIAGDIAG_GET_ALLOC_COUNT_EVENT_MODEM:
      case DIAGDIAG_GET_ALLOC_COUNT_EVENT_Q6:
      case DIAGDIAG_GET_ALLOC_COUNT_EVENT_RIVA:
          rsp->count.drop_count = event_get_alloc_count_event();
          break;
      case DIAGDIAG_GET_ALLOC_COUNT_F3_APP:
      case DIAGDIAG_GET_ALLOC_COUNT_F3_MODEM:
      case DIAGDIAG_GET_ALLOC_COUNT_F3_Q6:
      case DIAGDIAG_GET_ALLOC_COUNT_F3_RIVA:
          rsp->count.drop_count = diagbuf_get_alloc_count_f3();
          break;
      case DIAGDIAG_GET_ALLOC_COUNT_DELAY_APP:
      case DIAGDIAG_GET_ALLOC_COUNT_DELAY_MODEM:
      case DIAGDIAG_GET_ALLOC_COUNT_DELAY_Q6:
      case DIAGDIAG_GET_ALLOC_COUNT_DELAY_RIVA:
          rsp->count.drop_count = diagpkt_get_alloc_count_delay();
          break;
      case DIAGDIAG_GET_DROP_PER_APP:
      case DIAGDIAG_GET_DROP_PER_MODEM:
      case DIAGDIAG_GET_DROP_PER_Q6:
      case DIAGDIAG_GET_DROP_PER_RIVA:
          rsp->count.per_drop_count = diagbuf_get_drop_percentage();
          break;
    }
  }
  return ((void*)rsp);
  
} /* diagdiag_health_command */

/*===========================================================================

FUNCTION DIAGDIAG_GET_MAX_REQ_PKT_LEN

DESCRIPTION
  This procedure handles the command to query the maximum diag request
  packet length.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
#if defined(DIAG_MAX_REQ_LEN_RSP_SEND)
  PACK(void *) diagdiag_get_max_req_pkt_len(PACK(void *) req_pkt,uint16 pkt_len)
  {
     diag_get_max_req_pkt_len_rsp_type *rsp = NULL;

     rsp = (diag_get_max_req_pkt_len_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
         DIAGDIAG_GET_MAX_REQ_PKT_LEN, sizeof(diag_get_max_req_pkt_len_rsp_type));
                           
     if( rsp != NULL ) 
     {
       rsp->max_req_len = DIAG_MAX_RX_PKT_SIZ;
     }
  
     return rsp;
  
  } /* diagdiag_get_max_req_pkt_len */
#endif /* DIAG_MAX_REQ_LEN_RSP_SEND */

#ifdef DIAG_FEATURE_QPST_COOKIE
/*===========================================================================

FUNCTION DIAGDIAG_SAVE_QPST_COOKIE
     
DESCRIPTION
  This procedure handles the command to save qpst cookie.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_save_qpst_cookie(
  void * req_pkt,
  uint16 pkt_len
)
{
    diag_save_qpst_cookie_req_type *req = (diag_save_qpst_cookie_req_type *)req_pkt;
    diag_save_qpst_cookie_rsp_type *rsp = NULL;

    byte *temp;
    boolean flag;

    if(req->cmd_version != 1 || req->cookie_length > DLOAD_QPST_COOKIE_SIZE)
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
    }

    /* Check if the cookie_length and the actual length of the cookie given are the same  */
    if((pkt_len - FPOS(diag_save_qpst_cookie_req_type, cookie)) != req->cookie_length)
    {
      return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len);
    }
    rsp = (diag_save_qpst_cookie_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
           DIAG_SAVE_QPST_COOKIE, sizeof(diag_save_qpst_cookie_rsp_type));
    if(rsp == NULL)
    {
      return rsp;
    }
      rsp->header       = req->header;
      rsp->cmd_version  = req->cmd_version;

      /* Write to IRAM */
      temp  = (byte *)DLOAD_QPST_COOKIE_ADDR;
      *temp = req->cookie_length;
      temp++;
      memscpy((void *)temp, req->cookie_length, (void *)(req->cookie), req->cookie_length);

   #if !defined(DIAG_FEATURE_EFS2)
      rsp->error_code   = COOKIE_SAVE_TO_FS_ERROR;
      return rsp;
   #else
      flag = diagdiag_file_write(QPST_COOKIE_FILE_NAME, (void *)&(req->cookie_length),
                              req->cookie_length + FSIZ(diag_save_qpst_cookie_req_type, cookie_length));
      if(!flag)
      {
          rsp->error_code = COOKIE_SAVE_TO_FS_ERROR;
          return rsp;
      }
         
      rsp->error_code = COOKIE_SAVE_SUCCESS;
      return rsp;
   #endif
  
} /* diagdiag_save_qpst_cookie */

/*===========================================================================

FUNCTION DIAGDIAG_QUERY_QPST_COOKIE
     
DESCRIPTION
  This procedure handles the command to query qpst cookie.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_query_qpst_cookie(
  void * req_pkt,
  uint16 pkt_len
)
{
    diag_query_qpst_cookie_req_type *req = (diag_query_qpst_cookie_req_type *)req_pkt;
    diag_query_qpst_cookie_rsp_type *rsp = NULL;
    int rsp_len;
    byte *temp, cookie_length;
    boolean flag;
    
    if(req->cmd_version != 1)
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
    }

    /* If there is no file system, read the cookie from IRAM */
 #ifndef DIAG_FEATURE_EFS2 
    temp  = (byte *)DLOAD_QPST_COOKIE_ADDR;
    cookie_length = *temp;
    temp++;
    rsp_len = FPOS(diag_query_qpst_cookie_rsp_type, cookie) + cookie_length;
    rsp = (diag_query_qpst_cookie_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
             DIAG_QUERY_QPST_COOKIE, rsp_len);
    if(rsp == NULL)
    {
      return rsp;
    }
    memscpy((void *)(rsp->cookie), cookie_length,(void *)temp, cookie_length);
    rsp->header         = req->header;
    rsp->cmd_version    = req->cmd_version;
    rsp->error_code     = COOKIE_READ_SUCCESS;
    rsp->cookie_length  = cookie_length;
    return rsp;

 #else /* DIAG_FEATURE_EFS2 */

    flag = diagdiag_file_read(QPST_COOKIE_FILE_NAME, (void *)&cookie_length, FSIZ(diag_query_qpst_cookie_rsp_type, cookie_length), 0);
    if(!flag)
    {
      rsp_len = sizeof(diag_query_qpst_cookie_rsp_type);
      rsp = (diag_query_qpst_cookie_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
             DIAG_QUERY_QPST_COOKIE, rsp_len);
      if(rsp == NULL)
      {
        return rsp;
      }
      rsp->header         = req->header;
      rsp->cmd_version    = req->cmd_version;
      rsp->error_code     = COOKIE_READ_ERROR;
      rsp->cookie_length  = 0;
      rsp->cookie[0]      = 0; 
      return rsp;
    }

    /* Compute the reponse length based on the cookie length */
    rsp_len   = FPOS(diag_query_qpst_cookie_rsp_type, cookie) + cookie_length;
    rsp = (diag_query_qpst_cookie_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
             DIAG_QUERY_QPST_COOKIE, rsp_len);

    if(rsp == NULL)
      {
        return rsp;
      }
    rsp->header         = req->header;
    rsp->cmd_version    = req->cmd_version;
    rsp->cookie_length  = cookie_length;
    flag = diagdiag_file_read(QPST_COOKIE_FILE_NAME, (void *)(rsp->cookie), cookie_length,
                                                  FSIZ(diag_query_qpst_cookie_rsp_type, cookie_length));
    if(!flag)// File read error 
    {
      rsp->error_code     = COOKIE_READ_ERROR;
      return rsp;
    }
    rsp->error_code     = COOKIE_READ_SUCCESS;
    return rsp;

 #endif /* DIAG_FEATURE_EFS2 */

} /* diagdiag_query_qpst_cookie */

#endif /* DIAG_FEATURE_QPST_COOKIE */


/*===========================================================================

FUNCTION DIAGDIAG_HEALTH_FLOW_CTRL

DESCRIPTION
  This procedure performs operations on Diag health flow control count requests.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_health_flow_ctrl(
  void * req_pkt,
  uint16 pkt_len
)
{
  /* Initialization */
  diagpkt_subsys_cmd_code_type cmd_code;
  diag_health_get_flow_ctrl_rsp_type *rsp = NULL;

  cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);
  rsp = (diag_health_get_flow_ctrl_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
         cmd_code, sizeof(diag_health_get_flow_ctrl_rsp_type));
                           
  if( rsp != NULL ) 
  {
    switch ( cmd_code ) 
    {
      case DIAGDIAG_RESET_FLOW_CTRL_COUNT_APP:
      case DIAGDIAG_RESET_FLOW_CTRL_COUNT_MODEM: 
      case DIAGDIAG_RESET_FLOW_CTRL_COUNT_Q6:
          diagcomm_reset_flow_ctrl_count();
          rsp->version = DIAG_HEALTH_FLOW_CTRL_VER;
          rsp->count[DIAGCOMM_PORT_SMD] = diagcomm_get_flow_ctrl_count(DIAGCOMM_PORT_SMD);
          rsp->count[DIAGCOMM_PORT_SIO] = diagcomm_get_flow_ctrl_count(DIAGCOMM_PORT_SIO);
          break;
      case DIAGDIAG_GET_FLOW_CTRL_COUNT_APP:
      case DIAGDIAG_GET_FLOW_CTRL_COUNT_MODEM:
      case DIAGDIAG_GET_FLOW_CTRL_COUNT_Q6:
          rsp->version = DIAG_HEALTH_FLOW_CTRL_VER;
          rsp->count[DIAGCOMM_PORT_SMD] = diagcomm_get_flow_ctrl_count(DIAGCOMM_PORT_SMD);
          rsp->count[DIAGCOMM_PORT_SIO] = diagcomm_get_flow_ctrl_count(DIAGCOMM_PORT_SIO);
          break;
    }
  }
  
  return rsp;
  
} /* diagdiag_health_flow_ctrl */


/*===========================================================================

FUNCTION DIAGDIAG_HEALTH_VER_CMD

DESCRIPTION
  This procedure performs operations on Diag health requests with version field.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_health_ver_cmd(
  void * req_pkt,
  uint16 pkt_len
)
{
  /* Initialization */
  diagpkt_subsys_cmd_code_type cmd_code;
  diag_health_ver_rsp_type *rsp = NULL;

  cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);
  rsp = (diag_health_ver_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
         cmd_code, sizeof(diag_health_ver_rsp_type));
                           
  if( rsp != NULL ) 
  {
    switch ( cmd_code ) 
    {
      case DIAGDIAG_RESET_DSM_CHAINED_COUNT_APP:
      case DIAGDIAG_RESET_DSM_CHAINED_COUNT_MODEM: 
      case DIAGDIAG_RESET_DSM_CHAINED_COUNT_Q6:
      case DIAGDIAG_RESET_DSM_CHAINED_COUNT_RIVA:
          diagbuf_reset_dsm_chained_count();
          rsp->version = DIAG_HEALTH_DSM_CHAINED_VER;
          rsp->count = diagbuf_get_dsm_chained_count();
          rsp->tot_count = diagbuf_get_dsm_count();
          break;
      case DIAGDIAG_GET_DSM_CHAINED_COUNT_APP:
      case DIAGDIAG_GET_DSM_CHAINED_COUNT_MODEM:
      case DIAGDIAG_GET_DSM_CHAINED_COUNT_Q6:
      case DIAGDIAG_GET_DSM_CHAINED_COUNT_RIVA:
          rsp->version = DIAG_HEALTH_DSM_CHAINED_VER;
          rsp->count = diagbuf_get_dsm_chained_count();
          rsp->tot_count = diagbuf_get_dsm_count();
          break;
    }
  }
  
  return rsp;
  
} /* diagdiag_health_ver_cmd */


/*===========================================================================

FUNCTION DIAGDIAG_SET_PRESET_CMD

DESCRIPTION

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_set_preset_cmd( void * req_pkt, uint16 pkt_len )
{
  diagpkt_subsys_cmd_code_type cmd_code;
  diag_set_preset_id_rsp_type *rsp = NULL;
  diag_set_preset_id_req_type * req = (diag_set_preset_id_req_type *)req_pkt;
  
  if(pkt_len !=sizeof(diag_set_preset_id_req_type))
    return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len);

  if(req_pkt == NULL)
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);

  cmd_code = diagpkt_subsys_get_cmd_code(req_pkt);
  
  if(cmd_code != DIAGDIAG_SET_PRESET_ID_APP)
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
    

  rsp = (diag_set_preset_id_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
         cmd_code, sizeof(diag_set_preset_id_rsp_type));
  if( rsp )
  {
    rsp->preset_id = req->preset_id;
    rsp->error = diag_set_current_preset_mask_id( req->preset_id );
  }

  return rsp;
  
} /* diagdiag_set_preset_cmd */


/*===========================================================================

FUNCTION DIAGDIAG_GET_PRESET_CMD

DESCRIPTION

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_get_preset_cmd( void * req_pkt, uint16 pkt_len )
{
  diagpkt_subsys_cmd_code_type cmd_code;
  diag_get_preset_id_rsp_type *rsp = NULL;

  if(pkt_len !=sizeof(diag_get_preset_id_req_type))
    return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len);

  if(req_pkt == NULL)
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);

  cmd_code = diagpkt_subsys_get_cmd_code(req_pkt);
    
  if(cmd_code != DIAGDIAG_GET_PRESET_ID_APP)
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
  

  rsp = (diag_get_preset_id_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
         cmd_code, sizeof(diag_get_preset_id_rsp_type));
  if( rsp )
  {
    rsp->preset_id = diag_get_current_preset_mask_id();
  }

  return rsp;
  
} /* diagdiag_get_preset_cmd */


/*===========================================================================

FUNCTION DIAGDIAG_FLUSH_BUFFER_HANDLER

DESCRIPTION
  This procedure flushes diagbuf to send out TX traffic.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_flush_buffer_handler(
  void * req_pkt,
  uint16 pkt_len
)
{
  void * rsp = NULL;

  if (pkt_len != sizeof (diagpkt_subsys_header_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }
  /* Allocate the same length as the request. */
  rsp = diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_FLUSH_BUFFER, sizeof(diagpkt_subsys_header_type));

  if (rsp)
  {
	if(diagcomm_sio_flow_enabled())
	{
      diag_tx_notify();
	}
  }
  return (rsp);
}

/*===========================================================================

FUNCTION DIAGDIAG_CHANGE_THRESHOLD

DESCRIPTION
  This procedure changes draining thresholds for Diag buffer and event heap
  when data aggregation is enabled.  Also enables/disables data aggregation.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_change_threshold(
  void * req_pkt,
  uint16 pkt_len
)
{
  diagdiag_change_threshold_req_type *req = (diagdiag_change_threshold_req_type *) req_pkt;
  diagdiag_change_threshold_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof( diagdiag_change_threshold_rsp_type );
  static unsigned int event_rpt_size_temp = 0;

  event_rpt_size_temp = event_report_pkt_size;
  if (pkt_len != sizeof (diagdiag_change_threshold_req_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  rsp = (diagdiag_change_threshold_rsp_type *)diagpkt_subsys_alloc 
                            (DIAG_SUBSYS_DIAG_SERV, DIAG_CHANGE_THRESHOLD, rsp_len);
  if (rsp)
  {
    if (!(req->enable_aggregation))
    {
      diagbuf_commit_threshold = 0;
      event_rpt_size_temp = event_report_pkt_size;
      event_report_pkt_size = 0;
    }
    else
    {
      diagbuf_commit_threshold = DIAG_TX_SLEEP_THRESHOLD_DEFAULT /2;
      event_report_pkt_size = event_rpt_size_temp;
    }
  }
  return (rsp);
  
} /* diagdiag_change_threshold */

/*============================================================================================

FUNCTION DIAGDIAG_DROP_THRESHOLD_CONFIG_CMD

DESCRIPTION
  This function sets the threshold percentage level drops at which an event (EVENT_DIAG_DROP) 
  will be reported and the step percetange drop as well to periodically send the event.
  
RETURN VALUE
  Pointer to response packet.

===============================================================================================*/
PACK(void* )diagdiag_drop_threshold_config_cmd( void* req_pkt, uint16 pkt_len)
{
  diag_drop_event_param_req_type *req = (diag_drop_event_param_req_type*) req_pkt;
  diag_drop_event_param_rsp_type *rsp = NULL;
  const uint32 rsp_len = sizeof(diag_drop_event_param_rsp_type);
  
  /* Extract the Command Code from the Request Packet */
  diagpkt_subsys_cmd_code_type cmd_code = diagpkt_subsys_get_cmd_code (req);
  
  if (pkt_len != sizeof(diag_drop_event_param_req_type))
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }
  
  rsp = (diag_drop_event_param_rsp_type*)diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, cmd_code, rsp_len);
  
  if (rsp)
  {
    /* If the threshold percentage drop to be set is less than the MAX value defined, 
       set the threshold percentage drop to the Max Value */
    if(req->per_threshold_drop < DIAG_DROP_PER_THRESHOLD_MAX)
    {
      rsp->per_threshold_drop = DIAG_DROP_PER_THRESHOLD_MAX;
    }
    else
    {
      rsp->per_threshold_drop = req->per_threshold_drop;
    }
    
    /* If the step percentage drop to be set is less than the MAX value defined, 
           set the step percentage drop to the Max Value */    
    if(req->per_step_drop < DIAG_DROP_PER_STEP_MAX )
    {
      rsp->per_step_drop = DIAG_DROP_PER_STEP_MAX;
    }
    else
    {
      rsp->per_step_drop = req->per_step_drop;
    }
    
    /* Update the global threshold percentage drop and the step percentage drop*/
    diag_max_per_drop_threshold = rsp->per_threshold_drop;      
    diag_max_per_step = rsp->per_step_drop; 
  }
  return (rsp);
}


/*===========================================================================

FUNCTION DIAGDIAG_CHANGE_ENC_PROTOCOL

DESCRIPTION
  This procedure changes the encryption protocol that diag uses for TX traffic.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_change_enc_protocol(
  void * req_pkt,
  uint16 pkt_len
)
{
  diagdiag_change_enc_protocol_req_type *req = (diagdiag_change_enc_protocol_req_type *) req_pkt;
  diagdiag_change_enc_protocol_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof( diagdiag_change_threshold_rsp_type );

  if (pkt_len != sizeof (diagdiag_change_enc_protocol_req_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  rsp = (diagdiag_change_enc_protocol_rsp_type *)diagpkt_subsys_alloc 
	                        (DIAG_SUBSYS_DIAG_SERV, DIAG_CHANGE_ENC_PROTOCOL, rsp_len);
  if (rsp)
  {
    if (req->disable_hdlc)
	{
      diag_hdlc_protocol = FALSE;
	}
	else
	{
	  diag_hdlc_protocol = TRUE;
	}
	rsp->disable_hdlc = req->disable_hdlc;
  }
  return (rsp);
}

extern uint32 diag_ballot_mask;
extern uint32 diag_cx_state;

/*===========================================================================

FUNCTION DIAGDIAG_QUERY_ENABLE
     
DESCRIPTION
  This procedure handles the command to test the Diag enabled status function.   
  It also returns the current sleep ballot.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_query_enable(
  void * req_pkt,
  uint16 pkt_len
)
{
  diagdiag_query_enable_req_type *req = (diagdiag_query_enable_req_type *) req_pkt;
  diagdiag_query_enable_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof( diagdiag_query_enable_rsp_type );
  diagpkt_subsys_cmd_code_type cmd_code;

  if (req)
  {
    cmd_code = diagpkt_subsys_get_cmd_code(req);
  }
  else
    return NULL;

  if (pkt_len != sizeof (diagdiag_query_enable_req_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  rsp = (diagdiag_query_enable_rsp_type *)diagpkt_subsys_alloc 
	                        (DIAG_SUBSYS_DIAG_SERV, cmd_code, rsp_len);
  if (rsp)
  {
    rsp->ballot_mask = diag_ballot_mask;
    rsp->cx_state = diag_cx_state;
    rsp->enable_report = diag_masks_enabled();
  }
  return (rsp);
}

/*============================================================================*/
/* Queue item for diag stress test */
typedef struct
{
  q_link_type link;
  unsigned int seq_num;
  diag_stress_test_status_enum_type status; /* refer to diag_stress_test_status_enum_type*/

  osal_tcb_t tcb;                         /* used to define a task */
  osal_stack_word_type stack[STACK_SIZE];    /* stack needed by the task */

  diagdiag_cmd_req_type req;
}
diag_cmd_req_q_item_type;

/* Queue for diag stress test */
q_type diag_cmd_req_q;

/*===========================================================================

FUNCTION DIAG_CMD_RSP_HANDLER

DESCRIPTION The event has been removed from listening
============================================================================*/
void diagdiag_cmd_rsp_handler (const byte * rsp, 
                               unsigned int length, 
                               void *param)
{
  MSG_2 (MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Received rsp:0x%X num:%d", 
            (rsp) ? *((uint32 *) rsp) : 0, (unsigned int) param);
}

/*===========================================================================

FUNCTION DIAG_CMD_REQ_TASK_STARTUP

DESCRIPTION This procedure calls the diag cmd req
============================================================================*/
void diag_cmd_req_task_startup (uint32 params)
{
  diag_cmd_req_q_item_type *req = (diag_cmd_req_q_item_type *) params;
  boolean success = FALSE;

  MSG_2 (MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Injecting req:0x%08X num:%d", 
         *(req->req.req), req->seq_num);

  success = diag_cmd_req (req->req.req, req->req.length, 
                         diagdiag_cmd_rsp_handler, (void *) req->seq_num);

  MSG_3 (MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Injected req:0x%08X num:%d success:%d", 
           *(req->req.req), req->seq_num, success);
  
  if (osal_thread_get_pri () <= GET_DIAG_PRI())
  {
    /* This is done to make sure that the task gets killed before the
     cleanup function, running in diag context, removes the data from the Q */
     (void) osal_thread_set_pri (GET_DIAG_PRI()+1);
  }
  
  req->status = DIAG_TASK_DONE;
}

/*===========================================================================

FUNCTION DIAG_CMD_REQ_CLEANUP_CB

DESCRIPTION This procedure is a cleanup function. It goes through the
  diag_cmd_req_q, removes the entry from the Q and frees the memory.

============================================================================*/
void 
diag_cmd_req_cleanup_cb (void *param)
{
  diag_cmd_req_q_item_type *q_cur_ptr = NULL;
  diag_cmd_req_q_item_type *q_next_ptr = NULL;

  q_cur_ptr = (diag_cmd_req_q_item_type *) q_check (&diag_cmd_req_q);
  
  while (q_cur_ptr != NULL)
  {
    if (q_cur_ptr->status == DIAG_TASK_DONE)
    {
      /* store the next link */
      q_next_ptr = (diag_cmd_req_q_item_type *)
        q_next (&diag_cmd_req_q, &q_cur_ptr->link);

      /* Remove the data block from the Q */
      #ifdef FEATURE_Q_NO_SELF_QPTR     
      q_delete (&diag_cmd_req_q, &q_cur_ptr->link);
      #else
	  q_delete(&q_cur_ptr->link );
      #endif

      /* Free the memory */
      diag_free (q_cur_ptr);

      if (q_cur_ptr != NULL)
      {
        q_cur_ptr = NULL;
      }

      q_cur_ptr = q_next_ptr;
    }
    else
    {
      /* store the next link */
      q_cur_ptr = (diag_cmd_req_q_item_type *)
        q_next (&diag_cmd_req_q, &q_cur_ptr->link);
    }
  }

  /* Deregister's when the count is 0(Q is empty) */
  if (diag_cmd_req_q.cnt == 0)
  {
    (void) diag_idle_processing_unregister (diag_cmd_req_cleanup_cb, &diag_cmd_req_q);
  }

  return;
}

/*===========================================================================

FUNCTION DIAGDIAG_CMD_REQUEST_HANDLER

DESCRIPTION   
  This procedure sends a response to acknowledge that the test has been started.
  It then defines task with a requested packet. The task startup functions calls 
  the diag_cmd_req function with the packet sent.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void *
diagdiag_cmd_request_handler (void * req_pkt, uint16 pkt_len)
{
  diagdiag_cmd_rsp_type *rsp;
  diagdiag_cmd_req_type *req = (diagdiag_cmd_req_type *) req_pkt;

  const int rsp_len = pkt_len;

  /*  used to store and retrive data block from the Q */
  diag_cmd_req_q_item_type *q_item = NULL;
  static unsigned int seq_num = 0;
  static boolean diag_cmd_req_q_init = FALSE;

  if (pkt_len != sizeof (diagdiag_cmd_req_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  /* Allocate the same length as the request */
  rsp = (diagdiag_cmd_rsp_type *) diagpkt_subsys_alloc (
    DIAG_SUBSYS_DIAG_SERV,
    DIAGDIAG_CMD_REQUEST_F,
    rsp_len);

  /* Send the response acknowledging that the packet has been started */
  if (rsp != NULL)
  {
    memscpy ((void *) rsp, rsp_len, (void *) req, rsp_len);
    rsp->length = pkt_len - FPOS (diagdiag_cmd_rsp_type, req);

    if (diag_cmd_req_q_init == FALSE)
    {
      (void) q_init (&diag_cmd_req_q);
      diag_cmd_req_q_init = TRUE;
    }
    
    /* Get memory from the diag heap */
    q_item = (diag_cmd_req_q_item_type *) 
      diag_malloc (sizeof (diag_cmd_req_q_item_type));
       
    if (q_item != NULL)
    {
      /* Fill the data */
      memscpy ((void *) &(q_item->req), sizeof(diagdiag_cmd_req_type),
              (void *) req, pkt_len);
      q_item->status = DIAG_TASK_INITIALIZED;
      q_item->seq_num = seq_num;
      seq_num++;
      
      /* Place q_item on the diag_cmd_req_q */
      q_put (&diag_cmd_req_q, q_link (q_item, &(q_item->link)));

      /* We no longer own the memory at q_item. */
      q_item = NULL;
    }

    /* Commit the response before spawning the test task. */
    diagpkt_commit (rsp);

    rsp = NULL;
  }

  /* Get the head of the Q */
  q_item = (diag_cmd_req_q_item_type *) q_check (&diag_cmd_req_q);

  /* Loop until the end is reached */
  while (q_item != NULL)
  {
    if (q_item->status == DIAG_TASK_INITIALIZED)
    {
	  int diag_cmd_req_pri_array[32] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
      if (q_item->req.priority.pri_type == RELATIVE_PRI)
      {
        if (GET_DIAG_PRI() + q_item->req.priority.pri > GET_SLEEP_PRI())
        {
          q_item->req.priority.pri += GET_DIAG_PRI();
        }
        else
        {
          q_item->req.priority.pri = GET_SLEEP_PRI() + 1;
        }
      }
      /* Start the task */
	  diag_cmd_req_attr.stack_size = STACK_SIZE;
	  diag_cmd_req_attr.start_func = diag_cmd_req_task_startup;
      diag_cmd_req_attr.arg =(void *) (q_item);
	  diag_cmd_req_attr.name = "diag_cmd_req_task_startup";
	  diag_cmd_req_attr.priority = q_item->req.priority.pri;
	  diag_cmd_req_attr.stack_address = q_item->stack;
	  diag_cmd_req_attr.p_tskname = "DIAG_CMD";
	  diag_cmd_req_attr.suspended = FALSE;
	  diag_cmd_req_attr.dog_report_val = 0;

 	  if ((osal_create_thread(&q_item->tcb, &diag_cmd_req_attr, diag_cmd_req_pri_array)) != 0)
 	  {
		  return NULL;
 	  }

      /* Mark the task is running */
        /* Mark the task is running */
      if (q_item->status == DIAG_TASK_INITIALIZED)
      {
        q_item->status = DIAG_TASK_RUNNING;
      }
      
      /* go to the next Q item */
      q_item = q_next (&diag_cmd_req_q, &q_item->link);
    }
  }

  /* Register that the Q has items, and needs to be cleaned */
  (void) diag_idle_processing_register (diag_cmd_req_cleanup_cb, &diag_cmd_req_q);

  return NULL;
} /* diagdiag_cmd_request_handler */
/*===========================================================================

FUNCTION DIAG_STRESSTEST_CLEANUP_CB

DESCRIPTION
  This procedure is a cleanup function. It goes through the
  diag_stress_test_task_q, checks if any data block is ready to cleaned.
  If yes it removes the entry from the Q and frees the memory.

============================================================================*/
void  diag_stresstest_cleanup_cb (void *param)
{
  diag_stress_test_task_q_item_type *q_cur_ptr = NULL;
  diag_stress_test_task_q_item_type *q_next_ptr = NULL;
  int status;

  q_cur_ptr = (diag_stress_test_task_q_item_type *)q_check (&diag_stress_test_task_q);
  while( q_cur_ptr  != NULL )
  {
    /* If the element in the queue is ready to be deleted */
    if( q_cur_ptr->data.status == DIAG_TASK_DONE )
    {
      /* mapped to no-ops for all OSes except BLAST */
      osal_thread_join(&(q_cur_ptr->data.tcb), &status);

      /* cleanup OSAL related memory, queues, channel etc */
      osal_delete_thread(&(q_cur_ptr->data.tcb));

      /* store the next link */
      q_next_ptr =(diag_stress_test_task_q_item_type *)q_next
        ( &diag_stress_test_task_q, &q_cur_ptr->link );

      /* Remove the data block from the Q */
      #ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete( &diag_stress_test_task_q, &q_cur_ptr->link );
      #else
      q_delete  (&q_cur_ptr->link);
      #endif


      /* Free the memory */
      free( q_cur_ptr );

      if( q_cur_ptr != NULL) {
        q_cur_ptr = NULL;
      }
      q_cur_ptr = q_next_ptr;
    }
    else
    {
      /* store the next link */
      q_cur_ptr =(diag_stress_test_task_q_item_type *)q_next
        ( &diag_stress_test_task_q, &q_cur_ptr->link );
    }
  }

  /* Deregister's when the count is 0(Q is empty) */
  if( diag_stress_test_task_q.cnt == 0)
  {
    (void) diag_idle_processing_unregister (&diag_stresstest_cleanup_cb,
      &diag_stress_test_task_q);
  }

  return;
}

/*===========================================================================
FUNCTION DIAGDIAG_IS_VALID_REG_TBL_CMD

DESCRIPTION
  This procedure is used to validate the incoming command (and parameters)
  for retreiving the registration tables.
 
  Also assigns values to is_mater ( denotes whether it is a master proc )
  and the port_id (for querying slave table)

============================================================================*/
boolean
diagdiag_is_valid_reg_tbl_cmd()
{
  boolean is_valid = FALSE;

  reg_tbl_is_master = TRUE;
  reg_tbl_port_id = DIAGDIAG_NO_PORT;

#if defined (DIAG_IMAGE_APPS_PROC)
  /* Checking if the opcodes and the ports are valid */
  if(reg_tbl_cmd_code == DIAGDIAG_GET_CMD_REG_TBL_APPS)
  {
    switch(reg_tbl_proc_id)
    {
       case DIAG_APP_PROC:
         reg_tbl_is_master = TRUE;
         is_valid = TRUE;
         break;

       case DIAG_MODEM_PROC:
         if(NUM_SMD_PORT_TYPES > DIAGDIAG_MODEM_PORT)
         {
           reg_tbl_is_master = FALSE;
           is_valid = TRUE;
           reg_tbl_port_id = DIAGDIAG_MODEM_PORT;
         }
         break;

       case DIAG_QDSP6_PROC:
         if(NUM_SMD_PORT_TYPES > DIAGDIAG_QDSP6_PORT)
         {
           reg_tbl_is_master = FALSE;
           is_valid = TRUE;
           reg_tbl_port_id = DIAGDIAG_QDSP6_PORT;
         }
         break;

       case DIAG_RIVA_PROC:
         if(NUM_SMD_PORT_TYPES > DIAGDIAG_RIVA_PORT)
         {
           reg_tbl_is_master = FALSE;
           is_valid = TRUE;
           reg_tbl_port_id = DIAGDIAG_RIVA_PORT;
         }
         break;
    }
  }
#endif

#if defined (DIAG_IMAGE_MODEM_PROC)
  if(reg_tbl_cmd_code == DIAGDIAG_GET_CMD_REG_TBL_MODEM)
  {
    is_valid = TRUE;
  }
#endif

#if defined (DIAG_IMAGE_QDSP6_PROC)
  if(reg_tbl_cmd_code == DIAGDIAG_GET_CMD_REG_TBL_Q6)
  {
    is_valid = TRUE;
  }
#endif

#if defined (DIAG_IMAGE_RIVA_PROC)
  if(reg_tbl_cmd_code == DIAGDIAG_GET_CMD_REG_TBL_RIVA)
  {
    is_valid = TRUE;
  }
#endif

  return is_valid;
}

/*===========================================================================
FUNCTION DIAGDIAG_GET_REG_TABLE

DESCRIPTION 
  This procedure returns the commands that are registered with a given   
  processor based on the request. It calls diagpkt_get_registration_table()   
  to get the corresponding command registration table. It sends more than
  one response - An immediate response that contains the number of commands
  that are registered and one or more delayed response(s) containg the
  command registration information. This is because the response packet
  may exceed the total permissible length for a packet

============================================================================*/
void * diagdiag_get_reg_table (
  void * req_pkt,
  uint16 pkt_len
)
{
  diagdiag_get_reg_tbl_req_type *req = NULL;  /* The request packet */
  diagdiag_get_reg_tbl_imm_rsp_type *imm_rsp = NULL; /* The immediate reponse packet */
  
  req = (diagdiag_get_reg_tbl_req_type *)req_pkt;  

  /* Basic Sanity Check */
  if(req == NULL)
  {
    return ((void *)diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }

  /* If the packet length doesn't match */
  if(pkt_len != sizeof(diagdiag_get_reg_tbl_req_type))
  {
    return ((void *)diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  reg_tbl_cmd_code = diagpkt_subsys_get_cmd_code(req);
  reg_tbl_proc_id = req->proc_id;

  /* Check if the command code is valid or not */
  if( !diagdiag_is_valid_reg_tbl_cmd() )
  {
    return ((void *)diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }

  /* Create an immeditate reponse */
  imm_rsp = (diagdiag_get_reg_tbl_imm_rsp_type *)diagpkt_subsys_alloc_v2(DIAG_SUBSYS_DIAG_SERV, reg_tbl_cmd_code, sizeof(diagdiag_get_reg_tbl_imm_rsp_type));

  if(imm_rsp != NULL)
  {
    imm_rsp->cmd_count = diagpkt_get_reg_cmd_count();  /* The number of commands registered */
    imm_rsp->proc_id = reg_tbl_proc_id; /* Copying the processor id to the immediate response*/
    reg_tbl_delay_rsp_id = diagpkt_subsys_get_delayed_rsp_id(imm_rsp); /* Get the delayed response id */
    diagpkt_commit((void *)imm_rsp);  /* Commit the immediate reponse */

     /* Setting the internal signal to start sending Delayed Response(s) containing the registration table */
    (void)diag_set_internal_sigs( DIAG_INT_REG_TBL_RSP_SIG );
  }

  return NULL;
}

#ifdef FEATURE_DIAG_STRESS_PROFILE 

static npa_query_handle timer_test_CpuQueryHandle = NULL;
static npa_query_type qres;
/*this function will return you the CPU freq in KHz*/
uint32 qurt_timer_test_get_cpu_freq(void)
{
  uint32 cpu_freq = -1;
  if( NULL == timer_test_CpuQueryHandle )
  {
    timer_test_CpuQueryHandle = npa_create_query_handle("/clk/cpu");
  }

  /* Get the current CPU frequency. */
  if( NPA_QUERY_SUCCESS == npa_query( timer_test_CpuQueryHandle,
                                      NPA_QUERY_CURRENT_STATE,
                                      &qres ) );
  cpu_freq = qres.data.value;
  
  return cpu_freq;
}
#endif /* FEATURE_DIAG_STRESS_PROFILE */

/*===========================================================================

FUNCTION DIAGDIAG_TEST_TASK

DESCRIPTION
  This procedure is a startup function. Floods the link with the requested
  msg type packets.

============================================================================*/
void diagdiag_test_task (
  uint32 params
)
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  osal_timer_t sleep_timer;
  diag_tcb_stack_type *local_data = (diag_tcb_stack_type *)params;

  /* Pointers to stress test logs */
  LOG_STRESS_TEST_C_type * log_ptr;
  LOG_STRESS_TEST_64_type * log_ptr_64;
  LOG_STRESS_TEST_128_type * log_ptr_128;
  LOG_STRESS_TEST_256_type * log_ptr_256;
  LOG_STRESS_TEST_1K_type * log_ptr_1K;
  LOG_STRESS_TEST_2K_type * log_ptr_2K;
  LOG_STRESS_TEST_4K_type * log_ptr_4K;
  LOG_STRESS_TEST_6K_type * log_ptr_6K;
  LOG_STRESS_TEST_8K5_type * log_ptr_8K;
  LOG_STRESS_TEST_12k_type * log_ptr_12K;
  LOG_DIAG_ULOG_type * log_ptr_ulog;

  uint32 version_id = 10;           /*Used to generate extended packets*/
  uint16 log_index = 0;             /* Index used to fill char array */
  EVENT_PAYLOAD_INFO_type payload;  /* Store the payload info */
  uint32 payload_stress_test_complete = 0; /* payload for stress test complete event */
  boolean timer_init = FALSE;       /* timer initialized or not */
  int test_type = 0;   /* Stores the test type info requested */
  int num_iter_bef_sleep = 0; /* Keeps track of number of iterations before sleep */
  int i = 1;
  int N = local_data->task_info.num_iterations; /* Total number of iterations */
  int unique_id = local_data->unique_id; /* Get the task ID */
  boolean ext_flag = FALSE; /* Flag for extended API usage */
  char str[50] = "MSG_SPRINTF_FMT_VAR Iter %d uid %d num_iter %d\n";
  char str_ext[70] = "MSG_SPRINTF_FMT_VAR_EXT Iter %d uid %d num_iter %d\n";

#ifdef FEATURE_DIAG_STRESS_PROFILE
  uint64 pcycle_total=0, pcycle_start, pcycle_end;
  uint32 ms_time=0;
  uint32 cpu_freq=0;
#endif /* FEATURE_DIAG_STRESS_PROFILE */


#if defined(DIAG_QDSP6_APPS_PROC) && defined (DIAG_MMPM_SUPPORT) 

  diag_mmpm_register();
  /* Vote against power collapse on LPass when the test starts */
  diag_mmpm_config(DIAG_VOTE_AGAINST_POWER_COLLAPSE);
#endif
 
  
/* Vote against sleep for stress test */
#if defined( DIAG_DYNAMIC_SLEEP )    
  if( diagtest_npa_handle )
  {
    npa_issue_required_request( diagtest_npa_handle, 1);
  }
  
#elif defined(DIAG_QDSP6_APPS_PROC) && defined (DIAG_MMPM_SUPPORT)
  diag_mmpm_register();
  /* Vote against power collapse on LPass when the test starts */
  diag_mmpm_config(DIAG_VOTE_AGAINST_POWER_COLLAPSE);
#endif
 

  for(i=1; i <= N;i++) 
  {

    /* Special Case, message type is generated using random */
    if(local_data->task_info.priority.test_type == DIAGDIAG_STRESS_TEST_MSG_PSEUDO_RANDOM)
    {
      /* Pick the random msg type from the array */
      test_type = pseudo_random(i);
    }
    else {
      test_type = local_data->task_info.priority.test_type;
    }

#ifdef FEATURE_DIAG_STRESS_PROFILE
    cpu_freq = qurt_timer_test_get_cpu_freq();
    pcycle_start = qurt_get_core_pcycles();
#endif /* FEATURE_DIAG_STRESS_PROFILE */

    /**********************************************************************************
     *
     * In all the following cases, the priority field is replaced by unique id field 
     * to help the automation scripts differentiate between test tasks while parsing 
     * the test logs.
     *
     **********************************************************************************/    
    /* Reset ext_flag each time */
    ext_flag = FALSE;
    switch (test_type) {
      case DIAGDIAG_STRESS_TEST_MSG:
        MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Test MSG with no arg ");
        break;
      case DIAGDIAG_STRESS_TEST_MSG_1:
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_1 Iter %d\n",i);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_2:
        MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_2 Iter %d uid %d \n",i,unique_id);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_3:
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_3 Iter %d uid %d num_iter %d\n",i, unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_4:
        MSG_4(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_4 Iter %d uid %d num_iter %d procid %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_5:
        MSG_5(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_5 Iter %d uid %d num_iter %d procid %d %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID,5);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_6:
        MSG_6(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_6 Iter %d uid %d num_iter %d procid %d %d %d\n",
          i,unique_id,N,DIAG_MY_PROC_ID,5,6);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_STR:
        //TODO
        break;
      case DIAGDIAG_STRESS_TEST_MSG_LOW:
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_LOW, "MSG_LOW Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_MED:
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_MED, "MSG_MED Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_HIGH:
#if defined (DIAG_MP_MASTER)
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Master MSG_HIGH Iter %d uid %d num_iter %d \n",i,unique_id,N);
#else
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Client MSG_HIGH Iter %d uid %d num_iter %d \n",i,unique_id,N);
#endif
        break;
      case DIAGDIAG_STRESS_TEST_MSG_ERROR:
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"MSG_ERROR Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_FATAL:
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_FATAL, "MSG_FATAL Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_ERR:
        ERR( "ERR Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_ERR_FATAL:
        {
          int j;

          /* The ERR_FATAL test needs to make sure panic mode works.  
             Generate a few messages and see if they make it out. */
          for (j = 0; j < 10; j++)
          {
            MSG_2 (MSG_SSID_DIAG, MSG_LEGACY_FATAL,
                    "Panic mode test %d uid %d", j, unique_id);
          }
        }
        ERR_FATAL("ERR_FATAL Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_LOG:
        log_ptr = (LOG_STRESS_TEST_C_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
            sizeof(LOG_STRESS_TEST_C_type));
        if (log_ptr != NULL) {
          log_ptr->iteration = i;
          log_ptr->task_priority = unique_id;
          log_ptr->req_iterations = N;
          log_commit(log_ptr);
        }
        break;
      case DIAGDIAG_STRESS_TEST_EVENT_NO_PAYLOAD:
        event_report(EVENT_DIAG_STRESS_TEST_NO_PAYLOAD);
        break;
      case DIAGDIAG_STRESS_TEST_EVENT_WITH_PAYLOAD:
        payload.iteration = i;
        payload.task_priority = unique_id;
        payload.req_iterations = N;
        event_report_payload(EVENT_DIAG_STRESS_TEST_WITH_PAYLOAD,
          sizeof(EVENT_PAYLOAD_INFO_type),(void *) &payload);
        break;
      case DIAGDIAG_STRESS_TEST_LOG_64: 
        log_ptr_64 = (LOG_STRESS_TEST_64_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_64_type));
        if (log_ptr_64 != NULL) {
          log_ptr_64->iteration = i;
          log_ptr_64->task_priority =unique_id;
          log_ptr_64->req_iterations = N;
          for ( log_index = 0; log_index < 13; log_index++ ) {
              log_ptr_64->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_64);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_128:
        log_ptr_128 = (LOG_STRESS_TEST_128_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                             sizeof(LOG_STRESS_TEST_128_type));
        if (log_ptr_128 != NULL) {
          log_ptr_128->iteration = i;
          log_ptr_128->task_priority =unique_id;
          log_ptr_128->req_iterations = N;
          for ( log_index = 0; log_index < 29; log_index++ ) {
              log_ptr_128->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_128);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_256:
        log_ptr_256 = (LOG_STRESS_TEST_256_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                             sizeof(LOG_STRESS_TEST_256_type));
        if (log_ptr_256 != NULL) {
          log_ptr_256->iteration = i;
          log_ptr_256->task_priority =unique_id;
          log_ptr_256->req_iterations = N;
          for ( log_index = 0; log_index < 61; log_index++ ) {
              log_ptr_256->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_256);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_1K:
        log_ptr_1K = (LOG_STRESS_TEST_1K_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_1K_type));
        if (log_ptr_1K != NULL) {
          log_ptr_1K->iteration = i;
          log_ptr_1K->task_priority =unique_id;
          log_ptr_1K->req_iterations = N;
          for ( log_index = 0; log_index < 253; log_index++ ) {
              log_ptr_1K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_1K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_2K:
        log_ptr_2K = (LOG_STRESS_TEST_2K_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
            sizeof(LOG_STRESS_TEST_2K_type));
        if (log_ptr_2K != NULL) {
          log_ptr_2K->iteration = i;
          log_ptr_2K->task_priority =unique_id;
          log_ptr_2K->req_iterations = N;
          for ( log_index = 0; log_index < 509; log_index++ ) {
              log_ptr_2K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_2K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_4K:
        log_ptr_4K = (LOG_STRESS_TEST_4K_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_4K_type));
        if (log_ptr_4K != NULL) {
          log_ptr_4K->iteration = i;
          log_ptr_4K->task_priority =unique_id;
          log_ptr_4K->req_iterations = N;
          for ( log_index = 0; log_index < 1021; log_index++ ) {
              log_ptr_4K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_4K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_6K:
        log_ptr_6K = (LOG_STRESS_TEST_6K_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_6K_type));
        if (log_ptr_6K != NULL) {
          log_ptr_6K->iteration = i;
          log_ptr_6K->task_priority =unique_id;
          log_ptr_6K->req_iterations = N;
          for ( log_index = 0; log_index < 1533; log_index++ ) {
              log_ptr_6K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_6K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_8K5:
        log_ptr_8K = (LOG_STRESS_TEST_8K5_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_8K5_type));
        if (log_ptr_8K != NULL) {
          log_ptr_8K->iteration = i;
          log_ptr_8K->task_priority =unique_id;
          log_ptr_8K->req_iterations = N;
          for ( log_index = 0; log_index < (sizeof(log_ptr_8K->payload)/sizeof(log_ptr_8K->payload[0])); log_index++ ) {
              log_ptr_8K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_8K);
        }
        break;
	  case DIAGDIAG_STRESS_TEST_LOG_12k:
        log_ptr_12K = (LOG_STRESS_TEST_12k_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_12k_type));
        if (log_ptr_12K != NULL) {
          log_ptr_12K->iteration = i;
          log_ptr_12K->task_priority =unique_id;
          log_ptr_12K->req_iterations = N;
          for ( log_index = 0; log_index < (sizeof(log_ptr_12K->payload)/sizeof(log_ptr_12K->payload[0])); log_index++ ) {
              log_ptr_12K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_12K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_MSG_EXT:
        MSG_EXT(MSG_SSID_DIAG, MSG_LEGACY_HIGH, 1 , &version_id, "Test MSG_EXT with no arg ");
        break;
      case DIAGDIAG_STRESS_TEST_MSG_1_EXT:
        MSG_1_EXT(MSG_SSID_DIAG, MSG_LEGACY_HIGH, 1 , &version_id, "MSG_1_EXT Iter %d\n",i);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_2_EXT:
        MSG_2_EXT(MSG_SSID_DIAG, MSG_LEGACY_HIGH, 1, &version_id, "MSG_2_EXT Iter %d uid %d \n",i,unique_id);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_3_EXT:
        MSG_3_EXT(MSG_SSID_DIAG, MSG_LEGACY_HIGH, 1, &version_id, "MSG_3_EXT Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_4_EXT:
        MSG_4_EXT(MSG_SSID_DIAG, MSG_LEGACY_HIGH, 1,&version_id,"MSG_4_EXT Iter %d uid %d num_iter %d procid %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_9_EXT:
        MSG_9_EXT(MSG_SSID_DIAG, MSG_LEGACY_HIGH,1,&version_id, "MSG_9_EXT Iter %d uid %d num_iter %d procid %d %d %d\n",
          i,unique_id,N,DIAG_MY_PROC_ID,5,6,7,8,9);
        break;
      case DIAGDIAG_STRESS_TEST_LOG_EXT:
        log_ptr = (LOG_STRESS_TEST_C_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
            sizeof(LOG_STRESS_TEST_C_type), 1, &version_id);
        if (log_ptr != NULL) 
        {
          log_ptr->iteration = i;
          log_ptr->task_priority = unique_id;
          log_ptr->req_iterations = N;
          log_commit(log_ptr);
        }
        break;
	  case DIAGDIAG_STRESS_TEST_LOG_64_EXT:
        log_ptr_64 = (LOG_STRESS_TEST_64_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_64_type), 1, &version_id);
        if (log_ptr_64 != NULL) 
        {
          log_ptr_64->iteration = i;
          log_ptr_64->task_priority =unique_id;
          log_ptr_64->req_iterations = N;
          for ( log_index = 0; log_index < 13; log_index++ ) 
          {
              log_ptr_64->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_64);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_128_EXT:
        log_ptr_128 = (LOG_STRESS_TEST_128_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                             sizeof(LOG_STRESS_TEST_128_type), 1, &version_id);
        if (log_ptr_128 != NULL) 
        {
          log_ptr_128->iteration = i;
          log_ptr_128->task_priority =unique_id;
          log_ptr_128->req_iterations = N;
          for ( log_index = 0; log_index < 29; log_index++ ) 
          {
              log_ptr_128->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_128);
        }
         
        break;
      case DIAGDIAG_STRESS_TEST_LOG_256_EXT:
        log_ptr_256 = (LOG_STRESS_TEST_256_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                             sizeof(LOG_STRESS_TEST_256_type), 1, &version_id);
        if (log_ptr_256 != NULL) 
        {
          log_ptr_256->iteration = i;
          log_ptr_256->task_priority =unique_id;
          log_ptr_256->req_iterations = N;
          for ( log_index = 0; log_index < 61; log_index++ ) 
          {
              log_ptr_256->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_256);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_1K_EXT:
        log_ptr_1K = (LOG_STRESS_TEST_1K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_1K_type), 1, &version_id);
        if (log_ptr_1K != NULL) 
        {
          log_ptr_1K->iteration = i;
          log_ptr_1K->task_priority =unique_id;
          log_ptr_1K->req_iterations = N;
          for ( log_index = 0; log_index < 253; log_index++ ) 
          {
              log_ptr_1K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_1K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_2K_EXT:
        log_ptr_2K = (LOG_STRESS_TEST_2K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
            sizeof(LOG_STRESS_TEST_2K_type), 1, &version_id);
        if (log_ptr_2K != NULL) 
        {
          log_ptr_2K->iteration = i;
          log_ptr_2K->task_priority =unique_id;
          log_ptr_2K->req_iterations = N;
          for ( log_index = 0; log_index < 509; log_index++ ) 
          {
              log_ptr_2K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_2K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_1:        
        MSG_SPRINTF_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"MSG_SPRINTF_1 Iter %d\n", i);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_2:        
        MSG_SPRINTF_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"MSG_SPRINTF_2 Iter %d uid %d\n", i, unique_id);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_3:        
        MSG_SPRINTF_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"MSG_SPRINTF_3 Iter %d uid %d num_iter %d \n", i, unique_id, N);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_4:        
        MSG_SPRINTF_4(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"MSG_SPRINTF_4 Iter %d uid %d num_iter %d %s \n", i, unique_id, N, "4");
        break;
      case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_5:        
        MSG_SPRINTF_5(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"MSG_SPRINTF_5 Iter %d uid %d num_iter %d %s %s \n",i, unique_id,N,"4", "5");
        break;
      case DIAGDIAG_STRESS_TEST_LOG_4K_EXT:        
        log_ptr_4K = (LOG_STRESS_TEST_4K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_4K_type), 1, &version_id);
        if (log_ptr_4K != NULL) 
        {
          log_ptr_4K->iteration = i;
          log_ptr_4K->task_priority =unique_id;
          log_ptr_4K->req_iterations = N;
          for ( log_index = 0; log_index < 1021; log_index++ ) 
          {
              log_ptr_4K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_4K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_6K_EXT:        
        log_ptr_6K = (LOG_STRESS_TEST_6K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_6K_type), 1, &version_id);
        if (log_ptr_6K != NULL) 
        {
          log_ptr_6K->iteration = i;
          log_ptr_6K->task_priority =unique_id;
          log_ptr_6K->req_iterations = N;
          for ( log_index = 0; log_index < 1533; log_index++ ) 
          {
              log_ptr_6K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_6K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_6_EXT:        
        MSG_SPRINTF_6_EXT(MSG_SSID_DIAG, MSG_LEGACY_HIGH,1,&version_id, "MSG_SPRINTF_6_EXT Iter %d uid %d num_iter %d %s %s \n",i, unique_id,N,"4", "5", "6");
        break;
      case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_7_EXT:        
        MSG_SPRINTF_7_EXT(MSG_SSID_DIAG, MSG_LEGACY_HIGH,1,&version_id, "MSG_SPRINTF_7_EXT Iter %d uid %d num_iter %d %s %s \n",i, unique_id,N,"4", "5", "6","7");
        break;
      case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_8_EXT:        
        MSG_SPRINTF_8_EXT(MSG_SSID_DIAG, MSG_LEGACY_HIGH,1,&version_id, "MSG_SPRINTF_8_EXT Iter %d uid %d num_iter %d %s %s \n",i, unique_id,N,"4", "5", "6","7","8");
        break;
      case DIAGDIAG_STRESS_TEST_EVENT_EXT_NO_PAYLOAD:        
        event_report_ext(EVENT_DIAG_STRESS_TEST_NO_PAYLOAD, 1, &version_id);
        break;
      case DIAGDIAG_STRESS_TEST_EVENT_EXT_WITH_PAYLOAD:        
        payload.iteration = i;
        payload.task_priority = unique_id;
        payload.req_iterations = N;
        event_report_payload_ext(EVENT_DIAG_STRESS_TEST_WITH_PAYLOAD,
          sizeof(EVENT_PAYLOAD_INFO_type),(void *) &payload, 1, &version_id);
        break;

       case DIAGDIAG_STRESS_TEST_LOG_SUBMIT_EXT_64:
         ext_flag = TRUE;
         /* Continue into next case statement with ext_flag set */
       case DIAGDIAG_STRESS_TEST_LOG_SUBMIT_64:
         {
           uint8* log_pkt;

           log_pkt = (uint8*)malloc(sizeof(LOG_STRESS_TEST_64_type) + sizeof(log_header_type));
           
           if(log_pkt != NULL)
           {
              log_ptr_64 = (LOG_STRESS_TEST_64_type *)(log_pkt + sizeof(log_header_type));
              log_set_length(log_pkt,sizeof(LOG_STRESS_TEST_64_type));
              log_set_code(log_pkt,LOG_DIAG_STRESS_TEST_C);
              log_set_timestamp(log_pkt);
              
              log_ptr_64->iteration = i;
              log_ptr_64->task_priority =unique_id;
              log_ptr_64->req_iterations = N;
              for ( log_index = 0; log_index < 13; log_index++ ) {
                log_ptr_64->payload[log_index] = 0x7EABBA7E;
              }

              if(!ext_flag)
              {
                log_submit(log_pkt);
              }
              else
              {
                log_submit_ext(log_pkt,1,&version_id);
              }

              free(log_pkt);
           }
         }
         break;
       case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_FMT_VAR:
         {
           MSG_SPRINTF_FMT_VAR_3 (MSG_SSID_DIAG, MSG_LEGACY_HIGH, str, i, unique_id,N);
         }
         break;
       case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_FMT_VAR_EXT:
         {
           MSG_SPRINTF_FMT_VAR_3_EXT (MSG_SSID_DIAG, MSG_LEGACY_HIGH, 1,&version_id, str_ext, i, unique_id,N);
         }
         break;
       case DIAGDIAG_STRESS_TEST_ULOG_DIAG_LOG_EXT:
       {
            unsigned long int outputFormat;
            uint8* payload_buf=NULL;
            unsigned long int bytes_written=0;
            ULogResult status;
            uint32 pkt_len=FPOS(LOG_DIAG_ULOG_type, buf );

            ULogFront_RealTimePrintf(diag_err_ulog_handle, 2, "Ulog stress test iter %d max Iter %d", i, N );

            if(i == N)
            {
                payload_buf= (uint8 *)malloc(DIAG_ULOG_MAX_PAYLOAD_SIZE);

                if(payload_buf)
                {
                    status= ULogDiagFormatPayload(diag_err_ulog_handle, 0, &outputFormat, DIAG_ULOG_MAX_PAYLOAD_SIZE, (char *)payload_buf, &bytes_written);

                    if((DAL_SUCCESS == status) &&(bytes_written >0))
                    {
                        pkt_len+=bytes_written;

                        log_ptr_ulog = (LOG_DIAG_ULOG_type *) log_alloc_ext (LOG_DIAG_ULOG_PAYLOAD,
                                                                    pkt_len , 2, diag_err_ulog_handle );
                        if(log_ptr_ulog)
                        {
                            log_ptr_ulog->output_format=outputFormat;
                            log_ptr_ulog->output_length=bytes_written;
                            memscpy(&log_ptr_ulog->buf[0], DIAG_ULOG_MAX_PAYLOAD_SIZE, payload_buf, bytes_written );
                            log_commit_ext(log_ptr_ulog);
                        }
                    }
                    else
                    {
                        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Unable to format the ulogs");
                    }
                    free(payload_buf);
                }
            }
       }
       break;
       case DIAGDIAG_STRESS_TEST_ULOG_DIAG_LOG_SUBMIT_EXT:
        {
            uint8* log_pkt;
            unsigned long int outputFormat;
            char payload_buf[ULOG_MAX_MSG_SIZE];
            unsigned long int bytes_written=0;
            ULogResult status;
            uint32 pkt_len=FPOS(LOG_DIAG_ULOG_type, buf );

            ULogFront_RealTimePrintf(diag_err_ulog_handle, 2, "log_submit:Ulog stress test iter %d max Iter %d", i, N );

            if(i == N)
            {
                status = ULogDiagFormatPayload(diag_err_ulog_handle, 0, &outputFormat, sizeof(payload_buf), payload_buf, &bytes_written);

                if(DAL_SUCCESS == status)
                {
                    pkt_len+=bytes_written;

                    log_pkt = (uint8*)malloc( pkt_len + sizeof(log_hdr_type));

                    if(log_pkt != NULL)
                    {
                      log_ptr_ulog = (LOG_DIAG_ULOG_type *)(log_pkt + sizeof(log_hdr_type));
                      log_set_length(log_pkt,pkt_len);
                      log_set_code(log_pkt,LOG_DIAG_ULOG_PAYLOAD);
                      log_set_timestamp(log_pkt);

                      log_ptr_ulog->output_format=outputFormat;
                      log_ptr_ulog->output_length=bytes_written;
                      memscpy(&log_ptr_ulog->buf[0], sizeof(payload_buf), payload_buf, bytes_written );
                      log_submit_ext(log_pkt, 2, diag_err_ulog_handle);
                      free(log_pkt);
                    }
                }
                else
                {
                    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Unable to format the ulogs");
                }
            }
        }
        break;
      default: 
        break;
     }

#ifdef FEATURE_DIAG_STRESS_PROFILE
    /* To-do: Account for drops and remove those totals from time */
    pcycle_end = qurt_get_core_pcycles();
    pcycle_total += pcycle_end - pcycle_start;
    ms_time += ((pcycle_end-pcycle_start)/cpu_freq);
#endif /* FEATURE_DIAG_STRESS_PROFILE */

   num_iter_bef_sleep++;

   if( (local_data->task_info.num_iterations_before_sleep > 0)
     && (num_iter_bef_sleep == local_data->task_info.num_iterations_before_sleep) ) {

      if(timer_init == FALSE) {
        /* Defines a timer */
        osal_create_diag_timer (&sleep_timer, osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG);
        timer_init = TRUE;
      }

      /* Wait for sleep_duration. This blocks the current task, and
         different task with which is ready to run starts executing */
      (void) osal_timed_wait(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &sleep_timer,
        local_data->task_info.sleep_duration);

      /* Clear the signals. The block on the current task is cleared */
      return_val = osal_reset_sigs(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);

      num_iter_bef_sleep = 0;
    }
  } /* end of for loop */
  
  /* Sleep before sending out EVENT_DIAG_STRESS_TEST_COMPLETED,
   to make sure the event can be allocated and is received by APS.
   In some test-cases, APS misses the event because it is sent out too 
   early. */
  if(timer_init == FALSE)
  {
     /* Defines a timer */
     osal_create_diag_timer (&sleep_timer, osal_thread_self(),DIAG_STRESS_TEST_SLEEP_SIG);
     timer_init = TRUE;
  }
   (void) osal_timed_wait(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &sleep_timer, STRESS_TASK_SLEEP_TO_COMPLETE);

    /* Clear the signals. The block on the current task is cleared */
  return_val = osal_reset_sigs(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);

  if(osal_thread_get_pri() <= GET_DIAG_PRI())
  {
    /* This is done to make sure that the task gets killed before the
     cleanup function, running in diag context, removes the data from the Q */
    (void) osal_thread_set_pri(GET_DIAG_PRI()+1);
  }

  /* Notify that a stress test task has completed for automation */
  event_report_payload( EVENT_DIAG_STRESS_TEST_COMPLETED , sizeof(int32), &payload_stress_test_complete );

#ifdef FEATURE_DIAG_STRESS_PROFILE
  {
    /* To-do: Report drop counts directly based on type of test */
    uint64 diag_sigs_pcycle_count = diag_sigs_pcycle_total;
    uint64 diag_hdlc_pcycles_count = diag_hdlc_pcycles_total;
    uint64 diag_tracer_pcycles_count = diag_tracer_pcycles_total;
        
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Stress test complete. %d iterations\n",N);
    {
      uint64 pcycle_avg = pcycle_total/N;
      uint32 ms_avg = ms_time/N;
      
      if(pcycle_avg > (uint64)0x00000000FFFFFFFF)
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Pcycle average overflow\n",unique_id);
      }
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Pcycle average per iteration is: %d\n",unique_id,(uint32)pcycle_avg);
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d MS average per iteration is: %d\n",unique_id,ms_avg);
      
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag Task Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_sigs_pcycle_count/N));

      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag HDLC Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_hdlc_pcycles_count/N));

      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag Tracer Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_tracer_pcycles_count/N));

    }
    
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d CPU frequency is: %d\n",unique_id,cpu_freq);
    
    MSG_5(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d amss heap stats: Total: %d, Used: %d, Max used: %d. Max request: %d",unique_id,amss_mem_heap.total_bytes,amss_mem_heap.used_bytes,amss_mem_heap.max_used,amss_mem_heap.max_request);
    
#ifdef FEATURE_DIAG_STM
    /* Temp assist for STM testing; print to USB for quick results */
    if(diag_stm_enabled)
    {
      diag_stm_enabled=0;
      
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Stress test complete. %d iterations\n",N);
      {
        uint64 pcycle_avg = pcycle_total/N;
        uint32 ms_avg = ms_time/N;
        
        if(pcycle_avg > (uint64)0x00000000FFFFFFFF)
        {
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Pcycle average overflow\n",unique_id);
        }
        MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Pcycle average per iteration is: %d\n",unique_id,(uint32)pcycle_avg);
        MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d MS average per iteration is: %d\n",unique_id,ms_avg);

      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag Task Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_sigs_pcycle_count/N));

      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag HDLC Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_hdlc_pcycles_count/N));

      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag Tracer Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_tracer_pcycles_count/N));
      }
      
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d CPU frequency is: %d\n",unique_id,cpu_freq);
      
      MSG_5(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d amss heap stats: Total: %d, Used: %d, Max used: %d. Max request: %d",unique_id,amss_mem_heap.total_bytes,amss_mem_heap.used_bytes,amss_mem_heap.max_used,amss_mem_heap.max_request);
      
      diag_stm_enabled=1;
    }
#endif /* FEATURE_DIAG_STM */
  }

#endif /* FEATURE_DIAG_STRESS_PROFILE */



#if defined(DIAG_QDSP6_APPS_PROC)&& defined(DIAG_MMPM_SUPPORT )
/*Sleep for few seconds before voting for power collpase so that diag can drain the 
event stress test completed on lpass*/
(void) osal_timed_wait(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &sleep_timer, 10000);

    /* Clear the signals. The block on the current task is cleared */
  return_val = osal_reset_sigs(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Vote for power collapse on LPass when the test completes */
  diag_mmpm_release(DIAG_VOTE_FOR_POWER_COLLAPSE);
  diag_mmpm_deregister();
#endif

  
/* Vote for sleep for diag stress test */
#if defined( DIAG_DYNAMIC_SLEEP )
  if( diagtest_npa_handle )
  {
    npa_cancel_request( diagtest_npa_handle );
  }
 
#elif defined(DIAG_QDSP6_APPS_PROC)&& defined(DIAG_MMPM_SUPPORT )
  /*Sleep for few seconds before voting for power collpase so that diag can drain the 
  event stress test completed on lpass*/
  (void) osal_timed_wait(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &sleep_timer, 10000);

    /* Clear the signals. The block on the current task is cleared */
  return_val = osal_reset_sigs(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Vote for power collapse on LPass when the test completes */
  diag_mmpm_release(DIAG_VOTE_FOR_POWER_COLLAPSE);
  diag_mmpm_deregister();
#endif

    /* delete the timer to free the memory allocated when the timer was created */
  if(timer_init == TRUE)
  {
     return_val = osal_delete_timer(&sleep_timer);
     ASSERT(OSAL_SUCCESS == return_val);
  }
  
  /* mark the task to be cleaned */
  local_data->status = DIAG_TASK_DONE;

  /* mapped to no-ops for all OSes except BLAST */
  osal_thread_exit(thread_exit_status);
} /* stress_test_start_up */


/*===========================================================================

FUNCTION DIAGDIAG_STRESS_TEST

DESCRIPTION
  This procedure sends a response to acknowledge that the test has been started.
  It then defines "num_tasks" tasks, with a requested priorities.
  The task startup functions tests flooding the link using the requested
  parameters.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void *
diagdiag_stress_test (void * req_pkt, uint16 pkt_len)
{

  DIAGDIAG_STRESS_TEST_rsp_type *rsp;
  DIAGDIAG_STRESS_TEST_req_type *req = (DIAGDIAG_STRESS_TEST_req_type *) req_pkt;

  const int rsp_len = FPOS(DIAGDIAG_STRESS_TEST_rsp_type,test) +
                        sizeof(diag_per_task_test_info) * (req->num_tasks);

  /*  used to store and retrive data block from the Q */
  diag_stress_test_task_q_item_type *q_item = NULL;
  int i=0;

  if (pkt_len != rsp_len)
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  rsp = (DIAGDIAG_STRESS_TEST_rsp_type *) diagpkt_subsys_alloc (
  DIAG_SUBSYS_DIAG_SERV,
  DIAGDIAG_START_STRESS_TEST_F,
  rsp_len
  );

  /* Send the response acknowledging that the packet has been started */
  if (rsp != NULL)
  {
    memscpy ((void *) rsp, rsp_len, (void *) req, rsp_len);

    /* Initialize the queue */
    if(diag_stress_test_task_q_init == FALSE)
    {
      (void) q_init(&diag_stress_test_task_q);
      diag_stress_test_task_q_init = TRUE;
    }
#ifdef FEATURE_DIAG_STRESS_PROFILE
    // Reset diag pcycles counters
    diag_sigs_pcycle_total = 0;
    diag_hdlc_pcycles_total = 0; 
    diag_tracer_pcycles_total = 0;
#endif
    for(i=0; i<req->num_tasks; i++)
    {
      /* Get memory from the system heap */
      q_item =(diag_stress_test_task_q_item_type *) malloc(
        sizeof(diag_stress_test_task_q_item_type));

      if(q_item != NULL)
      {
         memset(q_item,0x0,sizeof(diag_stress_test_task_q_item_type));

        /* Initialize the link field */
        (void) q_link(q_item, &(q_item->link) );

        /* Fill the data */
        q_item->data.task_info = req->test[i];
        q_item->data.status = DIAG_TASK_INITIALIZED;
	q_item->data.unique_id = i;   /* Task IDs range from 0 to num_tasks-1 */       

        /* Place q_item on the diag_stress_test_task_q */
        q_put (&diag_stress_test_task_q, &(q_item->link));
      } else { /* Failed  system malloc, handle this error */
          diagpkt_commit(rsp);
          rsp = NULL;
          MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Failed to allocate q_item for stress test.\n");
          return NULL;
      }
    }

    /* Commit the response before spawning the test task. */
    diagpkt_commit(rsp);

    rsp = NULL;
  }

  /* Get the head of the Q */
  q_item = (diag_stress_test_task_q_item_type *)q_check (&diag_stress_test_task_q);

  /* Loop until the end is reached */
  while( q_item != NULL )
  {
    /* if the task is not yet started */
    if( q_item->data.status == DIAG_TASK_INITIALIZED )
    {
	  int diagdiag_stress_pri_array[32] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

      if(q_item->data.task_info.priority.pri_type == RELATIVE_PRI)
      {
#if defined(FEATURE_WINCE) || defined(DIAG_QDSP6_APPS_PROC)
        if (GET_DIAG_PRI() + q_item->data.task_info.priority.pri < GET_SLEEP_PRI())
        {
          q_item->data.task_info.priority.pri += GET_DIAG_PRI();
        }
        else
        {
          q_item->data.task_info.priority.pri = GET_SLEEP_PRI();
        }
#else   
        if( GET_DIAG_PRI() + q_item->data.task_info.priority.pri > GET_SLEEP_PRI() )
        {
          q_item->data.task_info.priority.pri += GET_DIAG_PRI();
        } 
        else
        {
          q_item->data.task_info.priority.pri = GET_SLEEP_PRI() + 1;
        }
#endif
      }

      /* Start the task */
      /* stack is of type unsigned long long' */
      diagdiag_stress_attr.stack_size = (STACK_SIZE * sizeof(unsigned long long) );
      diagdiag_stress_attr.start_func = diagdiag_test_task;
      diagdiag_stress_attr.arg = (void *) (&(q_item->data));
      diagdiag_stress_attr.name = "diagdiag_test_task";
      diagdiag_stress_attr.priority = q_item->data.task_info.priority.pri;
      diagdiag_stress_attr.stack_address = q_item->data.stack;
      diagdiag_stress_attr.p_tskname = "DIAG_STRESS";
      diagdiag_stress_attr.suspended = FALSE;
      diagdiag_stress_attr.dog_report_val = 0;

      /* Mark the task is running BEFORE creating the thread for multi-threaded case */
      if( q_item->data.status == DIAG_TASK_INITIALIZED )
      {
        q_item->data.status = DIAG_TASK_RUNNING;
      }

	  if ((osal_create_thread(&(q_item->data.tcb), &diagdiag_stress_attr, diagdiag_stress_pri_array)) != 0)
	  {   
           return NULL;
          
	  }

    }
    /* go to the next Q item */
    q_item = q_next (&diag_stress_test_task_q, &q_item->link);
  }

  /* Register that the Q has items,  and needs to be cleaned */
  (void) diag_idle_processing_register (diag_stresstest_cleanup_cb, &diag_stress_test_task_q);

  return NULL;

}  /* diagdiag_stress_test */

#ifdef DIAGDIAG_EXT_LISTENER_TEST
static boolean log_ext_listener_test_status = FALSE;
static boolean event_ext_listener_test_status = FALSE;
static uint32 events_listened = 0;
static uint32 events_listened_2 = 0;
static uint32 incorrect_event_count = 0;
static uint32 logs_listened = 0;
uint32 logs_listened_2 = 0;
uint8  data_array_1[] = {0,1,2,3,4};
uint8  data_array_2[] = {5,6,7,8,9};

void log_ext_listener_cb(const byte *log, unsigned int length, void *param)
{
  log_header_type *hdr = (log_header_type *)log;
  uint8* data_arr = (uint8 *)param;

  if(hdr->code == LOG_DIAG_STRESS_TEST_C)
  {
    logs_listened++;
    MSG_7(MSG_SSID_DIAG,MSG_LEGACY_HIGH, "Log code %d, Params (%d %d %d %d %d), Logs listened %d\n", 
     hdr->code, data_arr[0],data_arr[1],data_arr[2],data_arr[3],data_arr[4], logs_listened);
  }
}

void log_ext_listener_cb_2(const byte *log, unsigned int length, void *param)
{
  log_header_type *hdr = (log_header_type *)log;
  uint8* data_arr = (uint8 *)param;

  if(hdr->code == LOG_DIAG_STRESS_TEST_C)
  {
    logs_listened_2++;
    MSG_7(MSG_SSID_DIAG,MSG_LEGACY_HIGH, "Log code %d, Params (%d %d %d %d %d), Logs listened %d\n", 
     hdr->code, data_arr[0],data_arr[1],data_arr[2],data_arr[3],data_arr[4], logs_listened_2);
    
  }
}

void event_ext_listener_cb(const diag_event_type *event, void *param)
{
  uint8* data_arr = (uint8 *)param;

  switch(event->event_id)
  {
    case EVENT_DIAG_STRESS_TEST_NO_PAYLOAD:
      events_listened++;
      MSG_7(MSG_SSID_DIAG,MSG_LEGACY_HIGH, "Event Id %d, Params (%d %d %d %d %d), Events listened %d\n",
       event->event_id, data_arr[0],data_arr[1],data_arr[2],data_arr[3],data_arr[4], events_listened);
      break;
    case EVENT_DIAG_STRESS_TEST_WITH_PAYLOAD:
      events_listened++;
      MSG_7(MSG_SSID_DIAG,MSG_LEGACY_HIGH, "Event Id %d, Params (%d %d %d %d %d), Events listened %d\n", 
       event->event_id, data_arr[0],data_arr[1],data_arr[2],data_arr[3],data_arr[4], events_listened);
      break;
    /*case EVENT_DIAG_STRESS_TEST_COMPLETED:
      events_listened++; */
   /*   diag_remove_event_listener_ext(event_ext_listener_cb);
      diag_remove_log_listener_ext(log_ext_listener_cb); */
      break;
    default:
      incorrect_event_count++;
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Incorrect event received");
  }
}

void event_ext_listener_cb_2(const diag_event_type *event, void *param)
{
  uint8* data_arr = (uint8 *)param;

  switch(event->event_id)
  {
      case EVENT_DIAG_STRESS_TEST_COMPLETED:
      events_listened_2++; 
      MSG_7(MSG_SSID_DIAG,MSG_LEGACY_HIGH, "Event Id %d, Params (%d %d %d %d %d), Events listened %d\n", 
        event->event_id, data_arr[0],data_arr[1],data_arr[2],data_arr[3],data_arr[4], events_listened_2);
      
      /*   diag_remove_event_listener_ext(event_ext_listener_cb);
           diag_remove_log_listener_ext(log_ext_listener_cb); */
      break;
    default:
      incorrect_event_count++;
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Incorrect event received");
  }
}

void diagdiag_ext_listener_test(void)
{
  uint16 log_array[] = {LOG_DIAG_STRESS_TEST_C};
  uint16 log_array_2[]= {LOG_DIAG_STRESS_TEST_C};
  uint16 event_array[] = {EVENT_DIAG_STRESS_TEST_NO_PAYLOAD, EVENT_DIAG_STRESS_TEST_WITH_PAYLOAD /*, EVENT_DIAG_STRESS_TEST_COMPLETED*/};
  uint16 event_array_2[] = {EVENT_DIAG_STRESS_TEST_COMPLETED};
 
  event_ext_listener_test_status = diag_add_event_listener_ext(event_array, 2, event_ext_listener_cb, data_array_1);
  if(!event_ext_listener_test_status)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Unable to register as event ext listener");
  }
  
  event_ext_listener_test_status = diag_add_event_listener_ext(event_array_2, 1, event_ext_listener_cb_2, data_array_2);

  if(!event_ext_listener_test_status)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Unable to register as event ext listener");
  }

  log_ext_listener_test_status   = diag_add_log_listener_ext(log_array, 1, log_ext_listener_cb, data_array_1);

  if(!log_ext_listener_test_status)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Unable to register as log ext listener");
  }

  log_ext_listener_test_status   = diag_add_log_listener_ext(log_array_2, 1, log_ext_listener_cb_2, data_array_2);

  if(!log_ext_listener_test_status)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Unable to register as log ext listener");
  }
  
}
#endif



/* Tell lint that we are OK with the fact that pkt_len and req are not
** used in some of the packets, as this is dictated by the awesome design */
/*lint +esym(715,pkt_len,req) */

/* Define and register the dispatch table for common diagnostic packets */
#define DISPATCH_DECLARE(func) \
extern void * func(void * req_pkt, word len)

DISPATCH_DECLARE (event_inbound_pkt);

DISPATCH_DECLARE (log_process_legacy_logmask);
DISPATCH_DECLARE (log_process_ext_logmask_pkt);
DISPATCH_DECLARE (log_process_config_pkt);
DISPATCH_DECLARE (log_on_demand_pkt);
DISPATCH_DECLARE (log_on_demand_ext_pkt);

DISPATCH_DECLARE (msg_pkt_process_config);
DISPATCH_DECLARE (msg_pkt_legacy_req);
DISPATCH_DECLARE (ext_msg_pkt_config_adv);
DISPATCH_DECLARE (diagdiag_ext_event_report_ctrl);

DISPATCH_DECLARE (event_mask_get_handler);
DISPATCH_DECLARE (event_mask_set_handler);


static const diagpkt_user_table_entry_type diagdiag_common_tbl[] =
{
  /* Place these at the top of the list for faster search. */
  {DIAG_MSG_F, DIAG_MSG_F, msg_pkt_legacy_req},

  {DIAG_EXT_MSG_CONFIG_F, DIAG_EXT_MSG_CONFIG_F, msg_pkt_process_config},
  {DIAG_LOG_CONFIG_F, DIAG_LOG_CONFIG_F, log_process_config_pkt},
  {DIAG_LOG_ON_DEMAND_F, DIAG_LOG_ON_DEMAND_F, log_on_demand_pkt},
  {DIAG_LOG_ON_DEMAND_EXT_F, DIAG_LOG_ON_DEMAND_EXT_F, log_on_demand_ext_pkt},
  {DIAG_EXT_LOGMASK_F, DIAG_EXT_LOGMASK_F, log_process_ext_logmask_pkt},
  {DIAG_LOGMASK_F, DIAG_LOGMASK_F, log_process_legacy_logmask},
  {DIAG_EVENT_REPORT_F, DIAG_EVENT_REPORT_F, event_inbound_pkt},
  {DIAG_EVENT_MASK_GET_F, DIAG_EVENT_MASK_GET_F, event_mask_get_handler },
  {DIAG_EVENT_MASK_SET_F, DIAG_EVENT_MASK_SET_F, event_mask_set_handler },

};

static const diagpkt_user_table_entry_type diag_subsys_common_tbl[] =
{
   {DIAG_FLUSH_BUFFER, DIAG_FLUSH_BUFFER, diagdiag_flush_buffer_handler},
   {DIAG_CHANGE_ENC_PROTOCOL, DIAG_CHANGE_ENC_PROTOCOL, diagdiag_change_enc_protocol},
   {DIAG_EXT_EVENT_REPORT_CTRL, DIAG_EXT_EVENT_REPORT_CTRL, diagdiag_ext_event_report_ctrl},
   {DIAG_EXT_LOG_CONFIG, DIAG_EXT_LOG_CONFIG, log_process_config_pkt},
   {DIAG_EXT_MSG_CONFIG_ADV, DIAG_EXT_MSG_CONFIG_ADV, ext_msg_pkt_config_adv}
};


/* The ARM9 modem */
#if (defined (DIAG_IMAGE_MODEM_PROC) || defined(DIAG_SINGLE_PROC)) \
   && !defined (DIAG_IMAGE_QDSP6_PROC)
static const diagpkt_user_table_entry_type diagdiag_subsys_tbl[] =
{
  {DIAGDIAG_START_STRESS_TEST_F, DIAGDIAG_START_STRESS_TEST_F, diagdiag_stress_test},
  {DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_MODEM, DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_MODEM, diagdiag_subsys_loopback},
  {DIAGDIAG_CMD_REQUEST_F, DIAGDIAG_CMD_REQUEST_F, diagdiag_cmd_request_handler},
  {DIAGDIAG_RESET_DROP_COUNT_LOG_MODEM, DIAGDIAG_GET_ALLOC_COUNT_F3_MODEM, diagdiag_health_command},
  {DIAGDIAG_RESET_DROP_COUNT_DELAY_MODEM, DIAGDIAG_GET_ALLOC_COUNT_DELAY_MODEM, diagdiag_health_command},
  {DIAGDIAG_GET_FLOW_CTRL_COUNT_MODEM, DIAGDIAG_RESET_FLOW_CTRL_COUNT_MODEM, diagdiag_health_flow_ctrl},
   {DIAGDIAG_LOG_MASK_RETRIEVAL_MODEM, DIAGDIAG_LOG_MASK_RETRIEVAL_MODEM, log_pkt_get_local_masks},
   {DIAGDIAG_MSG_MASK_RETRIEVAL_MODEM, DIAGDIAG_MSG_MASK_RETRIEVAL_MODEM, msg_pkt_get_local_masks},
  {DIAGDIAG_EVENT_MASK_RETRIEVAL_MODEM, DIAGDIAG_EVENT_MASK_RETRIEVAL_MODEM, event_pkt_get_local_masks},
  {DIAGDIAG_RETRIEVE_SSID_RANGE_MODEM, DIAGDIAG_RETRIEVE_SSID_RANGE_MODEM, msg_pkt_get_local_ssid_range},
  {DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_MODEM, DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_MODEM,log_pkt_get_local_equipid_range},
  {DIAGDIAG_GET_DSM_CHAINED_COUNT_MODEM, DIAGDIAG_RESET_DSM_CHAINED_COUNT_MODEM, diagdiag_health_ver_cmd},
  {DIAGDIAG_DROP_THRESHOLD_CONFIG_MODEM, DIAGDIAG_DROP_THRESHOLD_CONFIG_MODEM, diagdiag_drop_threshold_config_cmd},
  {DIAGDIAG_GET_DROP_PER_MODEM, DIAGDIAG_GET_DROP_PER_MODEM, diagdiag_health_command},
  #ifdef DIAG_FEATURE_QPST_COOKIE
  {DIAG_SAVE_QPST_COOKIE, DIAG_SAVE_QPST_COOKIE, diagdiag_save_qpst_cookie},
  {DIAG_QUERY_QPST_COOKIE, DIAG_QUERY_QPST_COOKIE, diagdiag_query_qpst_cookie},
  #endif
  {DIAGDIAG_QUERY_ENABLE_MODEM, DIAGDIAG_QUERY_ENABLE_MODEM, diagdiag_query_enable},
  {DIAGDIAG_GET_TIME_API_MODEM, DIAGDIAG_GET_TIME_API_MODEM, diag_get_time_api_request}
};
#endif

/* Brew apps and WM apps */
#if defined (DIAG_IMAGE_APPS_PROC) && !defined (DIAG_IMAGE_QDSP6_PROC)
static const diagpkt_user_table_entry_type diagdiag_subsys_tbl_app[] =
{
  {DIAGDIAG_START_STRESS_TEST_F,DIAGDIAG_START_STRESS_TEST_F,diagdiag_stress_test},
  {DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_APP, DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_APP, diagdiag_subsys_loopback},
  {DIAGDIAG_RESET_DROP_COUNT_LOG_APP, DIAGDIAG_GET_ALLOC_COUNT_F3_APP, diagdiag_health_command},
  {DIAGDIAG_RESET_DROP_COUNT_DELAY_APP, DIAGDIAG_GET_ALLOC_COUNT_DELAY_APP, diagdiag_health_command},
  {DIAGDIAG_LOG_MASK_RETRIEVAL_APP, DIAGDIAG_LOG_MASK_RETRIEVAL_APP, log_pkt_get_local_masks},
   {DIAGDIAG_MSG_MASK_RETRIEVAL_APP, DIAGDIAG_MSG_MASK_RETRIEVAL_APP, msg_pkt_get_local_masks},
  {DIAGDIAG_EVENT_MASK_RETRIEVAL_APP, DIAGDIAG_EVENT_MASK_RETRIEVAL_APP, event_pkt_get_local_masks},
  {DIAGDIAG_RETRIEVE_SSID_RANGE_APP, DIAGDIAG_RETRIEVE_SSID_RANGE_APP,msg_pkt_get_local_ssid_range},
   {DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_APP, DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_APP,log_pkt_get_local_equipid_range},
  {DIAGDIAG_GET_FLOW_CTRL_COUNT_APP, DIAGDIAG_RESET_FLOW_CTRL_COUNT_APP, diagdiag_health_flow_ctrl},
  {DIAGDIAG_GET_DSM_CHAINED_COUNT_APP, DIAGDIAG_RESET_DSM_CHAINED_COUNT_APP, diagdiag_health_ver_cmd},
  {DIAGDIAG_SET_PRESET_ID_APP, DIAGDIAG_SET_PRESET_ID_APP, diagdiag_set_preset_cmd},
  {DIAGDIAG_GET_PRESET_ID_APP, DIAGDIAG_GET_PRESET_ID_APP, diagdiag_get_preset_cmd}
  {DIAG_CHANGE_THRESHOLD_APP, DIAG_CHANGE_THRESHOLD_APP, diagdiag_change_threshold}
  {DIAGDIAG_QUERY_ENABLE_APP, DIAGDIAG_QUERY_ENABLE_APP, diagdiag_query_enable}
  {DIAGDIAG_DROP_THRESHOLD_CONFIG_APP,DIAGDIAG_DROP_THRESHOLD_CONFIG_APP, diagdiag_drop_threshold_config_cmd},
  {DIAGDIAG_GET_DROP_PER_APP,DIAGDIAG_GET_DROP_PER_APP, diagdiag_health_command},
};
#endif

/* The QDSP6 on the 8650 and the MDM8200 */
#if defined(DIAG_IMAGE_QDSP6_PROC)
static const diagpkt_user_table_entry_type diagdiag_subsys_tbl_qdsp[] =
{
  {DIAGDIAG_START_STRESS_TEST_F, DIAGDIAG_START_STRESS_TEST_F, diagdiag_stress_test},
  {DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_Q6, DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_Q6, diagdiag_subsys_loopback},
  {DIAGDIAG_RESET_DROP_COUNT_LOG_Q6, DIAGDIAG_GET_ALLOC_COUNT_F3_Q6, diagdiag_health_command},
  {DIAGDIAG_RESET_DROP_COUNT_DELAY_Q6, DIAGDIAG_GET_ALLOC_COUNT_DELAY_Q6, diagdiag_health_command},
  {DIAGDIAG_GET_FLOW_CTRL_COUNT_Q6, DIAGDIAG_RESET_FLOW_CTRL_COUNT_Q6, diagdiag_health_flow_ctrl},
  {DIAGDIAG_LOG_MASK_RETRIEVAL_Q6, DIAGDIAG_LOG_MASK_RETRIEVAL_Q6, log_pkt_get_local_masks},
  {DIAGDIAG_MSG_MASK_RETRIEVAL_Q6, DIAGDIAG_MSG_MASK_RETRIEVAL_Q6, msg_pkt_get_local_masks},
  {DIAGDIAG_EVENT_MASK_RETRIEVAL_Q6, DIAGDIAG_EVENT_MASK_RETRIEVAL_Q6,event_pkt_get_local_masks},
  {DIAGDIAG_RETRIEVE_SSID_RANGE_Q6, DIAGDIAG_RETRIEVE_SSID_RANGE_Q6,msg_pkt_get_local_ssid_range},
   {DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_Q6, DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_Q6,log_pkt_get_local_equipid_range},
  {DIAGDIAG_GET_DSM_CHAINED_COUNT_Q6, DIAGDIAG_RESET_DSM_CHAINED_COUNT_Q6, diagdiag_health_command},
  {DIAG_CHANGE_TRHESHOLD_Q6, DIAG_CHANGE_TRHESHOLD_Q6, diagdiag_change_threshold},
  {DIAGDIAG_DROP_THRESHOLD_CONFIG_Q6, DIAGDIAG_DROP_THRESHOLD_CONFIG_Q6, diagdiag_drop_threshold_config_cmd},
  {DIAGDIAG_GET_DROP_PER_Q6, DIAGDIAG_GET_DROP_PER_Q6, diagdiag_health_command},
  {DIAGDIAG_GET_TIME_API_Q6, DIAGDIAG_GET_TIME_API_Q6, diag_get_time_api_request}
};
#endif

/* RIVA */
/* No flow control for RIVA */
#if defined(DIAG_IMAGE_RIVA_PROC)
static const diagpkt_user_table_entry_type diagdiag_subsys_tbl_riva[] =
{
  {DIAGDIAG_START_STRESS_TEST_F, DIAGDIAG_START_STRESS_TEST_F, diagdiag_stress_test},
  {DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_RIVA, DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK_RIVA, diagdiag_subsys_loopback},
  {DIAGDIAG_RESET_DROP_COUNT_LOG_RIVA, DIAGDIAG_GET_ALLOC_COUNT_F3_RIVA, diagdiag_health_command},
  {DIAGDIAG_RESET_DROP_COUNT_DELAY_RIVA, DIAGDIAG_GET_ALLOC_COUNT_DELAY_RIVA, diagdiag_health_command},
  {DIAGDIAG_GET_DSM_CHAINED_COUNT_RIVA, DIAGDIAG_RESET_DSM_CHAINED_COUNT_RIVA, diagdiag_health_command},
  {DIAGDIAG_LOG_MASK_RETRIEVAL_RIVA, DIAGDIAG_LOG_MASK_RETRIEVAL_RIVA, log_pkt_get_local_masks},
  {DIAGDIAG_MSG_MASK_RETRIEVAL_RIVA, DIAGDIAG_MSG_MASK_RETRIEVAL_RIVA, msg_pkt_get_local_masks},
  {DIAGDIAG_EVENT_MASK_RETRIEVAL_RIVA, DIAGDIAG_EVENT_MASK_RETRIEVAL_RIVA,event_pkt_get_local_masks},
  {DIAGDIAG_RETRIEVE_SSID_RANGE_RIVA, DIAGDIAG_RETRIEVE_SSID_RANGE_RIVA,msg_pkt_get_local_ssid_range},
  {DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_RIVA, DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_RIVA,log_pkt_get_local_equipid_range},
  {DIAG_CHANGE_THRESHOLD_RIVA, DIAG_CHANGE_THRESHOLD_RIVA, diagdiag_change_threshold},
  {DIAGDIAG_DROP_THRESHOLD_CONFIG_RIVA DIAGDIAG_DROP_THRESHOLD_CONFIG_RIVA, diagdiag_drop_threshold_config_cmd},
  {DIAGDIAG_GET_DROP_PER_RIVA, DIAGDIAG_GET_DROP_PER_RIVA, diagdiag_health_command},
};
#endif

#if defined(DIAG_MAX_REQ_LEN_RSP_SEND)
static const diagpkt_user_table_entry_type diagdiag_subsys_req_len_tbl[] =
{
   {DIAGDIAG_GET_MAX_REQ_PKT_LEN, DIAGDIAG_GET_MAX_REQ_PKT_LEN, diagdiag_get_max_req_pkt_len},
};
#endif /* DIAG_MAX_REQ_LEN_RSP_SEND */

#if (defined (DIAG_IMAGE_MODEM_PROC) || defined(DIAG_SINGLE_PROC)) \
   && !defined (DIAG_IMAGE_QDSP6_PROC)
static const diagpkt_user_table_entry_type diagdiag_tbl[] =
{
   {DIAG_PROTOCOL_LOOPBACK_F, DIAG_PROTOCOL_LOOPBACK_F, diagdiag_loopback},
};
#endif
/* Delayed Response Table */
static const diagpkt_user_table_entry_type diagdiag_delayed_rsp_tbl[] = 
{
  {DIAGDIAG_STRESS_TEST_DELAYED_RSP, DIAGDIAG_STRESS_TEST_DELAYED_RSP, diagdiag_delayed_rsp_test}
};

/* Define DIAG_PSEUDO_MODEM on 9K ARM9 so that DIAG_CHANGE_THRESHOLD is registered
   only on 9K Q6 Modem. ARM9 modem registers for the command DIAG_CHANGE_THRESHOLD_APP  */

#if defined(DIAG_IMAGE_MODEM_PROC) && !defined(DIAG_PSEUDO_MODEM)

static const diagpkt_user_table_entry_type diag_subsys_config_tbl[] =
{
  {DIAG_CHANGE_THRESHOLD, DIAG_CHANGE_THRESHOLD, diagdiag_change_threshold}
};
#endif

#if defined(DIAG_PSEUDO_MODEM)
static const diagpkt_user_table_entry_type diag_subsys_config_tbl[] =
{
  {DIAG_CHANGE_THRESHOLD_APP, DIAG_CHANGE_THRESHOLD_APP, diagdiag_change_threshold}
};
#endif

/* Delayed Response Table for Registration Table Retrieval */
static const diagpkt_user_table_entry_type diagdiag_get_reg_tbl_delay[] = 
{
  #if defined(DIAG_IMAGE_APPS_PROC)
  {DIAGDIAG_GET_CMD_REG_TBL_APPS, DIAGDIAG_GET_CMD_REG_TBL_APPS, diagdiag_get_reg_table}
  #endif
  #if defined(DIAG_IMAGE_MODEM_PROC)
  {DIAGDIAG_GET_CMD_REG_TBL_MODEM, DIAGDIAG_GET_CMD_REG_TBL_MODEM, diagdiag_get_reg_table}
  #endif
  #if defined(DIAG_IMAGE_QDSP6_PROC)
  {DIAGDIAG_GET_CMD_REG_TBL_Q6, DIAGDIAG_GET_CMD_REG_TBL_Q6, diagdiag_get_reg_table}
  #endif
  #if defined(DIAG_IMAGE_RIVA_PROC)
  {DIAGDIAG_GET_CMD_REG_TBL_RIVA, DIAGDIAG_GET_CMD_REG_TBL_RIVA, diagdiag_get_reg_table}
  #endif
};

void diagdiag_init (void)
  {

  int return_val =  0;

  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_COMMON_PROC, DIAGPKT_NO_SUBSYS_ID, 
        diagdiag_common_tbl);

  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_COMMON_PROC, DIAG_SUBSYS_DIAG_SERV, 
        diag_subsys_common_tbl);

#if (defined (DIAG_IMAGE_MODEM_PROC) || defined(DIAG_SINGLE_PROC)) \
	  && !defined (DIAG_IMAGE_QDSP6_PROC)

  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, diagdiag_tbl);
#endif

/* The ARM9 modem */
#if (defined (DIAG_IMAGE_MODEM_PROC) || defined(DIAG_SINGLE_PROC)) \
	  && !defined (DIAG_IMAGE_QDSP6_PROC)
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diagdiag_subsys_tbl);
#endif

/* Brew apps and WM apps */
#if defined (DIAG_IMAGE_APPS_PROC) && !defined (DIAG_IMAGE_QDSP6_PROC)
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_APP_PROC, DIAG_SUBSYS_DIAG_SERV,  
        diagdiag_subsys_tbl_app);
#endif

/* The QDSP6 on the 8650 */
#if defined(DIAG_IMAGE_QDSP6_PROC) && !defined(DIAG_IMAGE_MODEM_PROC)
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_QDSP6_PROC, DIAG_SUBSYS_DIAG_SERV,  
        diagdiag_subsys_tbl_qdsp);
#endif

/* QDSP6 for modem */
#if defined(DIAG_IMAGE_QDSP6_PROC) && defined(DIAG_IMAGE_MODEM_PROC)
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_MODEM_PROC, DIAG_SUBSYS_DIAG_SERV,  
        diagdiag_subsys_tbl_qdsp);
#endif

/* RIVA */
#if defined(DIAG_IMAGE_RIVA_PROC)
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_RIVA_PROC, DIAG_SUBSYS_DIAG_SERV,  
        diagdiag_subsys_tbl_riva);
#endif

#if defined(DIAG_MAX_REQ_LEN_RSP_SEND)
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_MY_PROC_ID, DIAG_SUBSYS_DIAG_SERV,  
        diagdiag_subsys_req_len_tbl);
#endif

/* Modem (Moving forward we should have only one modem ). On 9k, Q6 and Arm9 are both defined
   as modem processors but to avoid any confusion, we treat only q6 as the real modem and
   the feature DIAG_PSEUDO_MODEM should be used to help differentiate this. */
#if defined(DIAG_IMAGE_MODEM_PROC) && !defined(DIAG_PSEUDO_MODEM)
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_MODEM_PROC, DIAG_SUBSYS_DIAG_SERV,  
        diag_subsys_config_tbl);
#endif

#if defined(DIAG_PSEUDO_MODEM)
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_MODEM_PROC, DIAG_SUBSYS_DIAG_SERV,  
        diag_subsys_config_tbl);
#endif

  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(DIAG_SUBSYS_CMD_VER_2_F,
                                           DIAG_SUBSYS_DIAG_SERV,
                                           diagdiag_get_reg_tbl_delay);
  /*register table for delayed response*/
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY_PROC(DIAG_MY_PROC_ID, DIAG_SUBSYS_CMD_VER_2_F, DIAG_SUBSYS_DIAG_SERV,
        diagdiag_delayed_rsp_tbl);

  diagdiag_memop_tbl_mutex.name = "MUTEX_DIAGDIAG_MEMOP_CS"; 
	return_val = osal_init_mutex(&diagdiag_memop_tbl_mutex);

    ASSERT(OSAL_SUCCESS == return_val);

  
  }

#if defined __cplusplus
  }
#endif
