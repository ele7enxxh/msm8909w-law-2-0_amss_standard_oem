/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Logging Services

General Description
  Services to support data logging.

Initializing and Sequencing Requirements 
  'log_init()' must be called once during initialization prior to use.

Copyright (c) 1990-2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                              Edit History 

$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diaglog.c#1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/07/16   ph      Diag log packet support for ULog 
08/04/15   ph      Time Sync between processors within the chip-Diag changes.
10/31/14   rs      Replaced EUIP_ID_MAX with LOG_EQUIP_ID_MAX
07/30/14   ph      Avoid additional mask check while draining.
07/06/14   ph      Fixed KW errors.
04/11/14   ph      Compiler warning fixes.
04/01/14   rs      Added support for build mask centralization. 
12/03/13   ph      Update log_composite_mask algorithm to avoid possible concurrency issues.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.  
11/04/13   sr      Diag support for log-on-demand extended 
10/14/13   sr      Added support for extended event, log and F3 pkts   
08/28/13   rh      Calls to log_update_composite_mask() use correct params now
09/20/13   tbg     Added support for Diag over STM 
06/03/13   sa      Fixed issues responding to V1 and V2 Diag Test Commands.
05/15/13   sr      Fixed Klockwork warnings 
04/05/13   sr      Fixed the incorrect status in log_on_demand_unreg_int()
04/03/13   sr      Replace memcpy() and memmove() with memscpy() and memsmove() 
03/28/13   sr      Fixed Klockwork warnings 
01/21/13   is      Support DIAG_EXT_LOGMASK_F with preset masks
12/19/12   rh      Fixed Klockwork-detected bugs
12/19/12   rh      Fixed Klockwork-detected bugs
12/07/12   is      Warnings fixes
12/05/12   sr      Freed the log buffer that was allocated to SSM  
11/16/12   ph      Klocwork warning fixes
11/15/12   is      Support for preset masks
01/10/12   sr      Filtering of logs/events based on ssm permission file
09/22/12   sr      Support for mask rtvl command
09/17/12   ra      Fixed the incorrect status in Log On Demand response
08/15/12   vk/ra   Modifications for Log On Demand algorithm change 
05/03/12   sa      Mainlined DIAG_CENTRAL_ROUTING
04/26/12   rh      Added composite (aggregated) log mask optimization  
03/26/12   hvm     Extended Event/Log Listener APIs added  
03/07/12   rh      Fixed Klockwork errors
11/07/11   is      Fixed compiler warnings
10/25/11   is      Fixed passing in proper stream_id in log_config_mask()
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
03/18/11   hm      Diag sleep vote bug fix 
03/09/11   hm      Dual mask bug fix 
12/20/10   mad     Changes for dual mask
12/21/10   hm      Dual mask changes 
08/10/10   sg      Moved log_config_mask to diaglogi.h
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
02/20/10   sg      Merging back DCM WM files.Moved api 
                   functionality to log_api.c on AMSS
05/06/10   is      Removed obsolete LOG_CDMA_TIME_FORMAT
02/12/10   vs      Ported diag_time_get from depot
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
11/04/09   cahn    DIAG Health
09/09/09   JV      Changed signature of osal mutex calls.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/10/09   as      Decoupled code under FEATURE_DIAG_NON_STREAMING
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   vk      changed includes from external to internal headers
04/22/09   JV      Cleaned up featurization for timestamps.
10/03/08   vg      Updated code to use PACK() vs. PACKED 
                   Removed OR of Frame number into WCDMA packets
12/15/06   as      Fixed compiler warnings.
07/13/06   as      Added RPC support for mobileview
05/19/06   as      Added Multi processor modem only support
10/31/05   as      Fixed lint errors.
10/10/05   as      Changed implementation to process configuration files 
                   without sending responses.
09/29/05   as      Added check for NULL pointer after diagbuf allocation.
03/09/05   as      Fixed log_on_demand_register to avoid logcode to become 
                   double registered.
02/10/05   as      Added FEATURE_DEBUG_DIAG_HEAP to debug memory corruption.
03/02/04   as      Corrected the boundary in Log Set Mask Response.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
07/30/02   as      Updated log_get_pkt to support FEATRUE_DIAG_NON_STREAMING.
07/30/02   lad     Added default values for last items for each ID in case
                   they are not defined in diagtune.h.
03/18/02   gr      Modified log_set_timestamp to call log_set_timestamp_ex.
03/15/02   gr      Added code to stuff frame number bits into the timestamp
                   field of the log header for WCDMA.
01/18/02   as      Added implementation for Log On Demand 
01/17/02   lad     Fixed array size for log_mask_offset_tbl[] and
                   log_nice_offset_tbl[].
10/24/01   jal     Cleaned up some compiler warnings.
09/12/01   lad     Fixed log_process_config_pkt to correctly set log masks.
06/27/01   lad     Overhauled logging service to support multiple equipment 
                   IDs and general cleanup.
                   Added FEATURE_LOG_EQUIP_ID_XLATE to allow old tools to
                   set log codes with equip ID != 1 using temp equip ID 1
                   codes.
04/17/01   lad     Added use of log_code_type instead of uint16.
04/06/01   lad     Changed use of FEATURE_DIAG_MSP_BUILD to FEATURE_ZREX_TIME.
02/23/01   lad     Updated for diag re-architecture.
12/05/00   lad     Added new logging API.
11/10/00   jq      Added support for analog logging.
11/02/00   lad     Makeover + API enhancements.
08/31/00   lad     Optimized logging API funcs.
                   Added logmask processing and logmask diag pkt functions.
07/31/00   lad     Added MSM5000_IRAM_FWD for log_submit().
05/22/00   lad     Added valid mask support for extended logmask.
05/15/00   lad     Changed log_nice default to 0.
05/10/00   lad     Added load balancing support.  Updated diagbuf API.
04/17/00   lad     FEATURE_DIAG_V7_LOG_COMPATIBILITY to use this diag with
                   old DMSS targets.
04/11/00   lad     Moved log_status to log.h as INLINE function.
02/28/00   lad     Made changes to use diagbuf allocation manager.
11/05/98   lad     Made necessary changes for ARM port.
05/03/95   rdh     Added log_status function.
08/31/92   gb      DMSS ver.
03/05/91   rdb     Created module. 

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#ifndef FEATURE_WINCE
#endif
#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagdiag_v.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "log.h"
#include "msg.h"
#include "qw.h"
#include "osal.h"
#include "diagcomm_v.h"
#include "diag_cfg.h"
#include "diagcomm_ctrl.h" /* For DIAG_CTRL_MSG_EQUIP_LOG_MASK */
#include "stringl.h"
#include "ULogFront.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif

#include "feature.h" /* for DIAG_IS_INT_FEATURE_BIT_SET */

#include <string.h>
#include <stdlib.h> 

#include "diaglogi.h"
#include "log_arrays_i.h"
#ifdef FEATURE_WINCE
#error code not present
#endif 
osal_mutex_arg_t         log_on_demand_mutex;
#ifndef FEATURE_WINCE
extern void log_listener_notify (uint32 seq_num, void *log, diag_searchlist_node_type *node);
extern boolean log_searchlist_ext(uint16 log_code, void *log);
#endif

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */

/* -------------------------------------------------------------------------
 * Definitions and Declarations
 * ------------------------------------------------------------------------- */

extern uint8 diag_cur_preset_id; /* from diag.c */
 
/* The log_on_demand list holds the entries for log_code and 
   function pointer. The size of both log on demand and log on demand ext array
   must be LOG_ON_DEMAND_SIZE*/
log_on_demand_list_item_type log_on_demand_list[LOG_ON_DEMAND_SIZE];
log_on_demand_list_item_type log_on_demand_ext_list[LOG_ON_DEMAND_SIZE];

/* This array contains the log masks for all equipment IDs. */
/*lint -save  -e572 */
byte * log_mask[DIAG_MAX_STREAM_ID];
byte diag_log_preset_mask[DIAG_MAX_PRESET_ID][LOG_MASK_SIZE];    /* Stream 1 QXDM masks. */
byte diag_log_dci_mask[LOG_MASK_SIZE];                           /* Stream 2 DCI mask. */
byte log_composite_mask[LOG_MASK_SIZE];
byte log_listener_mask[LOG_MASK_SIZE];
byte log_listener_ext_mask[LOG_MASK_SIZE];

typedef struct{
  uint8 cmd_code; /* cmd code for extended pkts */
  uint8 version;  /* version of the extended pkt; currently we support version 1 */
  uint16 proc_id; /* processor ID where the msg is being generated */
  uint32 id;      /* ID passed in by user */
}log_ext_hdr_type;

typedef struct{
  uint8 cmd_code; /* cmd code for extended pkts */
  uint8 version;  /* version of the extended pkt; currently we support version 1 */
  uint16 proc_id; /* processor ID where the msg is being generated */
  uint8 name_len; /*  Length of Source DiagBuffer  */
  uint8 time_offset_type; /*Time offset type for DiagBuffer Content */
  uint64 time_offset; /*Time offset, based on TIME_OFFSET_TYPE*/
  uint8 ulog_name[ULOG_MAX_NAME_SIZE + 1]; /*Null Terminated String with Name of DiagBuffer*/
}log_ext_hdr_type_v2;

/*the log mask for ssm*/
#ifdef FEATURE_SSM
byte diag_log_mask_ssm[LOG_MASK_SIZE];
extern diag_mask_ssm_buffer_type *diag_log_mask_range;
extern osal_mutex_arg_t diag_log_mask_ssm_mutex;
extern uint32 num_log_code_ssm;
#endif

#if defined(DIAG_MP_MASTER)
/* Defines and variables for saving to master log mask */
#define MSTR_LOG_MASK_SIZE 500
#define MSTR_LOG_MAX_LAST_ITEM (MSTR_LOG_MASK_SIZE << 3)

typedef struct
{
  uint8 status;
  byte mask[LOG_EQUIP_ID_MAX+1][MSTR_LOG_MASK_SIZE]; /* LOG_EQUIP_ID_MAX is max log equip id, but it starts at 0, so need to add 1 for size */
} diag_log_mask_type;

diag_log_mask_type * diag_log_mask_mstr[DIAG_MAX_STREAM_ID];
diag_log_mask_type diag_log_mask_mstr_preset[DIAG_MAX_PRESET_ID];    /* Stream 1 QXDM Master masks. */
diag_log_mask_type diag_log_mask_mstr_dci[DIAG_MAX_DCI_ID];          /* Stream 2 DCI Master mask. */

uint16 diag_log_last_item_mstr_tbl[LOG_EQUIP_ID_MAX+1];
#endif

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
static diag_ctrl_info_fwd_status bt_log_range_fwd_status;
#endif

static const byte log_valid_mask[LOG_MASK_SIZE];
/*lint -restore */

/* The content of this table are computed in log_init().  'uint16' is used
 * instead of 'int' to reduce data bus cycles to access an element of this
 * array.  The last entry is the offset to the byte following the array.  It
 * is used for range checking. */
uint16 log_mask_offset_tbl[LOG_EQUIP_ID_LAST+1];


log_state_enum_type log_state = LOG_INIT_STATE;
void *log_commit_last = NULL; /* Many writers, 1 reader (DIAG) */
void *log_flush_last = NULL; /* 1 writer, 1 reader (both DIAG) */

#ifndef FEATURE_WINCE
extern diag_searchlist_type log_listeners;
#endif 

#if defined(DIAG_MP_MASTER)
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */
#endif

/* There are two purposes of this variable 
   1. To know if there is any client registred for log on demand
   2. To know how many clients have registred for log on demand 
      hence type int
*/
uint8 log_on_demand_reg_count;
uint8 log_on_demand_ext_reg_count;

/* Time Get Function Pointer from diag.c */
extern uint8 (*diag_time_get)(qword time);

/* -------------------------------------------------------------------------
 * Internal (aka private) function prototypes
 * ------------------------------------------------------------------------- */
boolean log_status_mask (log_code_type code, byte stream_id);
static boolean log_search_mask (unsigned int id, unsigned int item, byte stream_id);
void log_update_composite_mask(uint32 start, uint32 end);
boolean log_mask_enabled (const byte *xx_mask_ptr, unsigned int xx_id,
                                 unsigned int xx_item);
static log_code_type log_set_equip_id (unsigned int code, 
                                       unsigned int equip_id);

static boolean log_sleep_vote (void);
static void log_service_outbound (diag_send_desc_type *desc,byte stream_id);

PACK(void *) diaglog_process_ext_config_req (PACK(void *) req_pkt_ptr, uint16 pkt_len);

/* -------------------------------------------------------------------------
 * External (aka public) Function Implementations
 * ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION LOG_ON_DEMAND_LIST_INIT

DESCRIPTION
  Initialize log on demand list.

===========================================================================*/
static void log_on_demand_list_init (void)
{
  int i;

  // Optimization variable to see if some clients have registered for log on demand
  log_on_demand_reg_count = 0;
  log_on_demand_ext_reg_count = 0;

  for (i = 0; i < LOG_ON_DEMAND_SIZE; i++)
  {
    log_on_demand_list[i].log_code = 0;
    log_on_demand_list[i].log_on_demand_funct_ptr.ptr = NULL;
    log_on_demand_ext_list[i].log_code = 0;
    log_on_demand_ext_list[i].log_on_demand_funct_ptr.ext_ptr = NULL;
  }
}


/*===========================================================================

FUNCTION LOG_INIT

DESCRIPTION
  Initialize log masks.

RETURN VALUE
  None
  
===========================================================================*/
void log_init (void)
{
  int id, return_val = 0;
  
#if defined(DIAG_MP_MASTER)
  uint8 port_num = 0;
#endif

  ASSERT(sizeof(log_header_type) == sizeof(log_hdr_type));
  ASSERT(LOG_EQUIP_ID_LAST <= LOG_EQUIP_ID_MAX);
  
  log_mask_offset_tbl[0] = 0;

  for (id = 0; id < LOG_EQUIP_ID_LAST; id++) 
  {
    log_mask_offset_tbl[id+1] = 1 + log_mask_offset_tbl[id] +
      LOG_MASK_ARRAY_INDEX(log_last_item_tbl[id]);
        
#if defined(DIAG_MP_MASTER)
    diag_log_last_item_mstr_tbl[id] = 0;
#endif
  }

  /* Point local masks to initial preset masks. */
  log_mask[DIAG_STREAM_1-1] = &diag_log_preset_mask[diag_cur_preset_id-1][0];
  log_mask[DIAG_STREAM_2-1] = &diag_log_dci_mask[0];
  
#if defined(DIAG_MP_MASTER)
  /* Point Master masks to initial Master preset masks. */
  diag_log_mask_mstr[DIAG_STREAM_1-1] = &diag_log_mask_mstr_preset[diag_cur_preset_id-1];
  diag_log_mask_mstr[DIAG_STREAM_2-1] = &diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1];
  
  for( port_num = 0; port_num < NUM_SMD_PORTS; port_num++ )
  {
    /* Point log mask update array to intial preset arrays. */
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_update[DIAG_STREAM_1-1] = 
        diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_preset_update[diag_cur_preset_id-1];
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_update[DIAG_STREAM_2-1] = 
        diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_dci_update[DIAG_DCI_MASK_1-1];
      
    for( id = 0; id <= LOG_EQUIP_ID_MAX; id++ )
    {
      diag_log_mask_mstr_preset[DIAG_PRESET_MASK_1-1].status = DIAG_CTRL_MASK_INVALID;
      diag_log_mask_mstr_preset[DIAG_PRESET_MASK_2-1].status = DIAG_CTRL_MASK_INVALID;
      diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1].status = DIAG_CTRL_MASK_INVALID;
      
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_preset_update[DIAG_PRESET_MASK_1-1][id] = FALSE;
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_preset_update[DIAG_PRESET_MASK_2-1][id] = FALSE;
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_dci_update[DIAG_DCI_MASK_1-1][id] = FALSE;
    }
  }
#endif /* DIAG_MP_MASTER*/

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
  bt_log_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT;
#endif

  /* These masks are cleared with memset because the listeners do not have a
     config function like log_config_mask to do so, nor is there a listener init. */
  memset(log_composite_mask, 0, sizeof(log_composite_mask));
  memset(log_listener_mask, 0, sizeof(log_listener_mask));
  memset(log_listener_ext_mask, 0, sizeof(log_listener_ext_mask));

    #ifdef FEATURE_SSM
  #ifdef DIAG_SSM_LOG_EVENT_WHITELIST
  /*If Whitelisting approach-by default all the log_codes are not allowed */
  memset(diag_log_mask_ssm, 0, sizeof(diag_log_mask_ssm));
  #else
  /*ssm permission mask array initialized to 0xFF- by default all the log_codes are allowed*/
  memset(diag_log_mask_ssm, 0xFF, sizeof(diag_log_mask_ssm));
  #endif
  #endif

#if !defined(T_WINNT)
  (void) log_config_mask (-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  log_config_mstr_mask (-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  diag_sleep_vote (DIAG_SLEEP_LOG, TRUE);
#else
  #error code not present
#endif

  log_on_demand_list_init();

  (void) diagbuf_register_outbound_pkt_handler (DIAG_LOG_F, log_service_outbound);
 
  log_on_demand_mutex.name = "MUTEX_LOG_ON_DEMAND";
  return_val = osal_init_mutex(&log_on_demand_mutex);
  ASSERT(OSAL_SUCCESS == return_val);
  
  /* Now that init is complete, enable normal operation */
  log_state = LOG_NORMAL_STATE;

} /* log_init */

 
/*===========================================================================

FUNCTION LOG_CONFIG_MASK

DESCRIPTION
  This function selects which log codes are enabled for logging.

PARAMETERS
  code - Specifies which log code is being referenced. Should be one of
         the enumerated log codes defined in log_codes.h.  
           -1 means entire mask.
  flag - Specifies whether to enable (TRUE) or disable (FALSE) logging
         for the specified log code.
  stream_id - Specifies which stream to enable/disable.
               Valid stream_id range: DIAG_MIN_STREAM_ID to DIAG_MAX_STREAM_ID
               Set all streams: DIAG_STREAM_ALL
  preset_id - Specifies preset_id for Stream 1 log mask. 
              Valid preset_id range: DIAG_MIN_PRESET_ID to DIAG_MAX_PRESET_ID
              Set all presets: DIAG_PRESET_MASK_ALL
              
RETURN VALUE
  TRUE if the operations was succesful.
  FALSE if an invalid code was specified.

WARNING
  Please update log_config_mstr_mask() if log_config_mask() is updated.
  
===========================================================================*/
boolean log_config_mask (int code, boolean  flag, byte stream_id, uint8 preset_id)
{
  boolean return_status = FALSE;
  uint8 preset_index = 0;
  unsigned int id, item;
  unsigned int index, start, end;
  byte mask = (flag) ? 0xFF : 0;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This will update entire composite log mask range */
  start = 0;
  end = LOG_MASK_SIZE -1;
  
  if (code == -1) //sets entire mask
  {    
    if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
    {
      if( preset_id == DIAG_PRESET_MASK_ALL )
      {
        for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
        {
          for( index = 0; index < LOG_MASK_SIZE; index++ )
            diag_log_preset_mask[preset_index][index] = mask;
        }
      }
      else
      {
        for( index = 0; index < LOG_MASK_SIZE; index++ )
          diag_log_preset_mask[preset_id-1][index] = mask;
      }
    } //end set stream 1 all
    
    if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
    {
      for( index = 0; index < LOG_MASK_SIZE; index++ )
        diag_log_dci_mask[index] = mask;
    } //end set stream 2 all
    
    return_status = diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
  }
  else if( code > 0 )
  {
    /* Get ID and item from log code */
    id = LOG_GET_EQUIP_ID(code);
    item = LOG_GET_ITEM_NUM(code);

    /* Check if code is valid */
    if ((id <= LOG_EQUIP_ID_LAST) && (item <= log_last_item_tbl[id]))
    {
      index = log_mask_offset_tbl[id] + LOG_MASK_ARRAY_INDEX(item);
      mask = LOG_MASK_BIT_MASK(item);
    
      if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
      {
        if( preset_id == DIAG_PRESET_MASK_ALL )
        {
          for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
          {
            if( flag )
              diag_log_preset_mask[preset_index][index] |= mask;
            else
              diag_log_preset_mask[preset_index][index] &= ~mask;
          }
        }
        else
        {
          if( flag )
            diag_log_preset_mask[preset_id-1][index] |= mask;
          else
            diag_log_preset_mask[preset_id-1][index] &= ~mask;
        }
      } //end set stream 1 all
    
      if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
      {
        if( flag )
          diag_log_dci_mask[index] |= mask;
        else
          diag_log_dci_mask[index] &= ~mask;
      } //end set stream 2 all
      
      /* This will update composite log mask for specified range */
      start = index;
      end = index;
    }
    else  
    {
      return_status = FALSE;
    }
  }
  
  log_update_composite_mask(start, end);
  return return_status;

} /* log_config_mask */


#ifdef FEATURE_SSM
/*===========================================================================
FUNCTION LOG_MASK_SSM

DESCRIPTION
  This function sets/clears the diag_log_mask_ssm array elements based on the
  perm file provided by SSM
  
===========================================================================*/
void log_mask_ssm(
   /*diag_log_mask_range has to be passed in for this field*/
   diag_mask_ssm_buffer_type* code_range, 
   uint32 size)
{
  int i=0;
  unsigned int start = 0;
  int check_perm_value; 
  unsigned int code_start, code_end;
  unsigned int end = LOG_MASK_SIZE -1;
  unsigned int index;
  int j=0;
  byte mask;

   if ((code_range == NULL) || (size == 0)) 
   {
     return;
   }
  
  
  #ifdef DIAG_SSM_LOG_EVENT_WHITELIST
  /*If Whitelisting approach */
   memset(diag_log_mask_ssm, 0, sizeof(diag_log_mask_ssm));
   /*If white listing approach is followed then we check for permission value 1 (not allowed); 
    the mask corresponding to these log codes in permission mask array are set to 0. */
   check_perm_value = 0;
  #else
  /*ssm permission mask array initialized to 0xFF- by default all the log_codes are allowed*/
   memset(diag_log_mask_ssm, 0xFF, sizeof(diag_log_mask_ssm));
   /*If black listing approach is followed then we check for permission value 1 (not allowed); 
   the mask corresponding to these log codes in permission mask array are set to 0. */
   check_perm_value = 1;
  #endif     

   osal_lock_mutex(&diag_log_mask_ssm_mutex);
  
  for(i=0;i<size;i++) 
  {
    if(code_range->permission == check_perm_value)
   {
/* Get ID and item from the log code start*/
      code_start = (log_code_type) code_range->start;
    code_end = (log_code_type) code_range->end;
	
    for (j=code_start; j<=code_end; j++) 
    {
      if ((LOG_GET_EQUIP_ID(j) > LOG_EQUIP_ID_LAST) || (LOG_GET_ITEM_NUM(j) > log_last_item_tbl[LOG_GET_EQUIP_ID(j)]))
    {
      break;
    }
      index = log_mask_offset_tbl[LOG_GET_EQUIP_ID(j)] + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(j));
      mask = LOG_MASK_BIT_MASK(LOG_GET_ITEM_NUM(j));
      #ifdef DIAG_SSM_LOG_EVENT_WHITELIST
      diag_log_mask_ssm[index] |= mask;
      #else
      diag_log_mask_ssm[index] &= ~mask;
      #endif
      }
   }
    code_range++;   
  }
   free (diag_log_mask_range);
  diag_log_mask_range = NULL;
   num_log_code_ssm = 0;
  osal_unlock_mutex(&diag_log_mask_ssm_mutex);
  log_update_composite_mask(start, end);
  return;
}

#endif

/*===========================================================================

FUNCTION LOG_CONFIG_MSTR_MASK

DESCRIPTION
  This function saves the master log mask for sending to slaves.

PARAMETERS
  code - Specifies which log code is being referenced. Should be one of
         the enumerated log codes defined in log_codes.h.  
           -1 means entire mask.
  flag - Specifies whether to enable (TRUE) or disable (FALSE) logging
         for the specified log code.
  stream_id - Specifies which stream to enable/disable.
               Valid stream_id range: DIAG_MIN_STREAM_ID to DIAG_MAX_STREAM_ID
               Set all streams: DIAG_STREAM_ALL
  preset_id - Specifies preset_id for Stream 1 log mask. 
              Valid preset_id range: DIAG_MIN_PRESET_ID to DIAG_MAX_PRESET_ID
              Set all presets: DIAG_PRESET_MASK_ALL
  
RETURN VALUE
  None

WARNING
  Please update this function if log_config_mask() is updated.
  
===========================================================================*/
void log_config_mstr_mask ( int code, boolean flag, byte stream_id, uint8 preset_id )
{
#if defined(DIAG_MP_MASTER)
  unsigned int id, item;
  unsigned int index;
  byte mask = (flag) ? 0xFF : 0;
  uint8 port_num, preset_index = 0;
  uint8 status;
    
  ASSERT( stream_id <= DIAG_MAX_STREAM_ID );
  ASSERT( preset_id <= DIAG_MAX_PRESET_ID );
  
  if( code == -1 ) //Sets entire mask
  {
    status = (mask) ? DIAG_CTRL_MASK_ALL_ENABLED : DIAG_CTRL_MASK_ALL_DISABLED;
    
    if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
    {
      if( preset_id == DIAG_PRESET_MASK_ALL )
      {
        for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
        {
          for( id = 0; id <= LOG_EQUIP_ID_MAX; id++ )
          {
            for( index = 0; index < MSTR_LOG_MASK_SIZE; index++ ) 
            {
              diag_log_mask_mstr_preset[preset_index].mask[id][index] = mask;
            }
          
          /* Master needs to keep track of updated log masks to send masks to Slaves */
          for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
          {
            diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_preset_update[preset_index][id] = TRUE;
          }
        }
          diag_log_mask_mstr_preset[preset_index].status = status;
        }
      }
      else
      {
        for( id = 0; id <= LOG_EQUIP_ID_MAX; id++ )
        {
          for( index = 0; index < MSTR_LOG_MASK_SIZE; index++ ) 
          {
            diag_log_mask_mstr_preset[preset_id-1].mask[id][index] = mask;
          }
             
        /* Master needs to keep track of updated log masks to send masks to Slaves */
        for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
        {
          diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_preset_update[preset_id-1][id] = TRUE;
        }
      }
        diag_log_mask_mstr_preset[preset_id-1].status = status;
      }
    } //end set stream 1 all
    
    if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
    {
      for( id = 0; id <= LOG_EQUIP_ID_MAX; id++ )
      {
        for (index = 0; index < MSTR_LOG_MASK_SIZE; index++) 
        {
          diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1].mask[id][index] = mask;
        }
             
      /* Master needs to keep track of updated log masks to send masks to Slaves */
      for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
      {
        diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_dci_update[DIAG_DCI_MASK_1-1][id] = TRUE;
      }
      }
      diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1].status = status;
    } //end set stream 2 all
  }
  else if( code > 0 )
  {
    diag_log_mask_type * log_mask_mstr_ptr = NULL;
    
    ASSERT( stream_id !=0 );
    ASSERT( preset_id !=0 );
    
    if( stream_id == DIAG_STREAM_1 )
      log_mask_mstr_ptr = &diag_log_mask_mstr_preset[preset_id-1];
    else if( stream_id == DIAG_STREAM_2 )
      log_mask_mstr_ptr = &diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1];
    
    /* Get ID and item from log code */
    id = LOG_GET_EQUIP_ID(code);
    item = LOG_GET_ITEM_NUM(code);

    if( (id <= LOG_EQUIP_ID_MAX ) && (item < MSTR_LOG_MAX_LAST_ITEM) )
    {
      log_mask_mstr_ptr->status = DIAG_CTRL_MASK_VALID;
      index = LOG_MASK_ARRAY_INDEX(item);
      mask = LOG_MASK_BIT_MASK(item);
      
      if( flag )
        log_mask_mstr_ptr->mask[id][index] |= mask;
      else
        log_mask_mstr_ptr->mask[id][index] &= ~mask;
        
      if( diag_log_last_item_mstr_tbl[id] < item )
        diag_log_last_item_mstr_tbl[id] = item;
        
      /* Master needs to keep track of updated log masks to send masks to Slaves */
      for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
      {
        if( stream_id == DIAG_STREAM_1 )
          diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_preset_update[preset_id-1][id] = TRUE;
        else if( stream_id == DIAG_STREAM_2 )
          diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].log_mask_dci_update[DIAG_DCI_MASK_1-1][id] = TRUE;
      }
    }
  }
#endif /* DIAG_MP_MASTER */

  return;

} /* log_config_mstr_mask */


/*===========================================================================

FUNCTION LOG_FLUSH

DESCRIPTION
  Prepares the logging service for a flush.  The latest log that was 
  committed is marked to be sent.  All others are discarded.

===========================================================================*/
void 
log_flush (void)
{
  if (log_state == LOG_NORMAL_STATE)
  {
    /* log_commit_last has multiple writers and is written prior to signalling
       the DIAG task.  This routine, however, is called from DIAG task only.  
       Therefore, DIAG is the only writer for log_flush_last which is
       captured here to mark the last log to send before declaring a flush
       complete.  Once sent, it is sent to NULL by log_service_outbound,
       also by the DIAG task. */
    log_flush_last = log_commit_last;

    if (log_flush_last)
    {
      log_state = LOG_FLUSH_STATE;
    }
  }
}

/*===========================================================================

FUNCTION LOG_PANIC

DESCRIPTION
  Prepares the logging service for panic mode.  All logs are configured off.
  All listeners are deleted.

===========================================================================*/
void 
log_panic (void)
{
  if (log_state != LOG_INIT_STATE)
  {
    /* log_commit_last has multiple writers and is written prior to signalling
       the DIAG task.  This routine, however, is called from DIAG task only.  
       Therefore, DIAG is the only writer for log_flush_last which is
       captured here to mark the last log to send before declaring a flush
       complete.  Once sent, it is sent to NULL by log_service_outbound,
       also by the DIAG task. */
    log_flush_last = log_commit_last;

    if (log_flush_last)
    {
      log_state = LOG_PANIC_STATE;
    }
    else
    {
      log_state = LOG_INIT_STATE; /* Disable */
    }
  }
}


 
/*===========================================================================

FUNCTION LOG_STATUS_MASK

DESCRIPTION
  This routine is a wrapper for log_search_mask().  It is used to look up
  the given code in the log mask for the given stream.
  
RETURN VALUE
  A boolean indicating if the specified log is enabled.
    
===========================================================================*/
boolean
log_status_mask (log_code_type code, byte stream_id)
{
  unsigned int id, item;
  boolean status = FALSE;
  
  id = LOG_GET_EQUIP_ID (code);
  item = LOG_GET_ITEM_NUM (code);

  if ( stream_id & DIAG_STREAM_1) {

   status = log_search_mask (id, item, DIAG_STREAM_1);
  }
  if ( stream_id & DIAG_STREAM_2) {

   status |= log_search_mask (id, item, DIAG_STREAM_2);
  }
 

  return status;
}



/*===========================================================================

FUNCTION LOG_SERVICE_OUTBOUND

DESCRIPTION
  This routine is called by diagbuf when it starts a new DIAG_LOG_F packet.
  This routine handles log listeners as well as sending to the comm layer.
  
  This routine also handles various 'log_state's that are used for 
  preserving listener state while flushing logs to the comm layer.
  
===========================================================================*/
static void
log_service_outbound (diag_send_desc_type *desc,byte stream_id)
{
  const diag_log_rsp_type *log;
  log_code_type code;
  diag_send_state_enum_type state;
  unsigned int id, item;
  log_ext_hdr_type *log_hdr = NULL;
  log_ext_hdr_type_v2 *log_hdr2 = NULL;
  diagbuf_header_type *diagbuf_hdr1 = NULL;
  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_START, FALSE};
  static boolean log_ext_hdr_sent = FALSE;

  if (desc)
  {
    state = desc->state;
    
    if (state == DIAG_SEND_STATE_START)
    {
      if (log_state == LOG_INIT_STATE)
      {
        /* Skip immediately */
        state = DIAG_SEND_STATE_COMPLETE;
      }
      else
      {
        diagbuf_hdr1 = (diagbuf_header_type *) desc->pkt;

        /*point to the diagbuf header*/
        diagbuf_hdr1 -= 1;

        /* Processing for chained diagbuf entries (extended logs) */
        if((diagbuf_hdr1->version == DIAGBUF_VERSION_1 ) && (diagbuf_hdr1->chain == DIAGBUF_FIRST_PKT_CHAIN))
        {
          if( !( stream_id&DIAG_STREAM_1 ) )
          {
            /* Drop extended header if STREAM_1 is not active.
               Not currently supported by listeners or by DCI */
            desc->pkt = (void *)((byte *)desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            return;
          }

          /* Send out extended packet as needed to STREAM_1 only */
          log_hdr = (log_ext_hdr_type *) desc->pkt;
          log_hdr->cmd_code = DIAG_CMD_EXT_F;
          log_hdr->version = ((log_ext_hdr_type *)desc->pkt)->version;
          log_hdr->proc_id = ((log_ext_hdr_type *)desc->pkt)->proc_id;
          log_hdr->id = ((log_ext_hdr_type *)desc->pkt)->id;
          send.pkt = (void *) &log_hdr->cmd_code;
          send.last = (void *)( (uint32)send.pkt + sizeof(log_ext_hdr_type)-1);
          send.terminate = FALSE;
          send.state = DIAG_SEND_STATE_START;

          /* Send extended packet header, but mask out DCI STREAM until it supports extended packets */
          diagbuf_send_pkt(&send, (stream_id & ~DIAG_STREAM_2));
          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            desc->pkt = (void *)((byte *)desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            /* Set flag indicating first portion of extended log packet sent */
            log_ext_hdr_sent = TRUE;
            send.pkt = NULL;
          } 
          return;
        }

        if((diagbuf_hdr1->version == DIAGBUF_VERSION_2 ) && (diagbuf_hdr1->chain == DIAGBUF_FIRST_PKT_CHAIN))
        {
          if( !( stream_id&DIAG_STREAM_1 ) )
          {
            /* Drop extended header if STREAM_1 is not active.
               Not currently supported by listeners or by DCI */
            desc->pkt = (void *)((byte *)desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            return;
          }

          /* Send out extended packet as needed to STREAM_1 only */
          log_hdr2 = (log_ext_hdr_type_v2 *) desc->pkt;
          log_hdr2->cmd_code = DIAG_CMD_EXT_F;

          log_hdr2->name_len = ((log_ext_hdr_type_v2 *)desc->pkt)->name_len;


          send.pkt = (void *) &log_hdr2->cmd_code;
          send.last = (void *)( (uint32)send.pkt + FPOS(diag_cmd_ext_v2_type, ulog_name) + log_hdr2->name_len  - 1 );
          send.terminate = FALSE;
          send.state = DIAG_SEND_STATE_START;

          /* Send extended packet header, but mask out DCI STREAM until it supports extended packets */
          diagbuf_send_pkt(&send, (stream_id & ~DIAG_STREAM_2));
          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            desc->pkt = (void *)((byte *)desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
          /* Set flag indicating first portion of extended log packet sent */
          log_ext_hdr_sent = TRUE;
            send.pkt = NULL;
          }

          return;
        }


        log = (diag_log_rsp_type *) desc->pkt;
        code = log_get_code ((void *) log->log);

#ifndef FEATURE_WINCE
        /* Check listener masks before searching listener lists */
        id = LOG_GET_EQUIP_ID (code);
        item = LOG_GET_ITEM_NUM (code);

        if (log_mask_enabled(log_listener_mask, id, item)) {
          (void) diag_searchlist_search_all (code, 
                                             &log_listeners, 
                                             (void *) log->log, 
                                             log_listener_notify);
        }
        if (log_mask_enabled(log_listener_ext_mask, id, item)) {
          (void) log_searchlist_ext (code, (void *) log->log);
        }
#endif 

        /* Check if the stream is not either QXDM or DCI. */
        if ( !(stream_id & (DIAG_STREAM_1|DIAG_STREAM_2)))
        {
          /* Mark complete - don't send to comm layer. */
          state = DIAG_SEND_STATE_COMPLETE;
        }

        /* log_commit_last has multiple writers.  log_commit_last is always 
           captured before committing to the queue.  Therefore, it is always
           ahead of log->log in the queue.  Therefore, one can assume that if 
           this match succeeds, log_commit_last is no longer pointing to a 
           valid item in the queue, and must be cleared. */
        if ((void *) log->log == log_commit_last)
        {
          /* In the off chance DIAG is preempted by a writer of 
             'log_commit_last' between the above check and writing NULL here,
         the side effect is only that the log may be ignored by a flush
             operation when it copies it to log_flush_last.  DIAG is the 
             only reader of this variable, so there is no risk of capturing 
             a corrupt value for log_flush_last when captured from this 
             variable.  This is a tolerable side effect.  The only loss is 
             that any log generated from a preemption here will eb lost if a 
             flush begins before that log is sent.  The tradeoff would be to 
             require INTLOCK around this 'if' block. */
          log_commit_last = NULL;
        }

        if (log_flush_last)
        {
          log_flush_last = NULL;

          /* Flush or panic mode will always disable comm bound logs.
             Panic moves to 'init' state to halt the svc when complete. */
          if (log_state == LOG_FLUSH_STATE)
          {
            (void) log_config_mask (-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
            log_config_mstr_mask (-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
            log_state = LOG_NORMAL_STATE;
          }
          else if (log_state == LOG_PANIC_STATE)
          {
            (void) log_config_mask (-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
            log_config_mstr_mask (-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
            log_state = LOG_INIT_STATE; /* disable logging */
          }
          else
          {
            MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"flush_last set in wrong state");
          } 
        }
      }
    }

    if (state == DIAG_SEND_STATE_COMPLETE)
    {
      /* Skip this packet - no tool asked for it. */
      desc->pkt = (void *) (((uint8 *) desc->last) + 1);
      desc->state = state;
    }
    else
    {
      /* This routine will transition state even if there if no sending can be 
         done.  Therefore, this routine can assume that state == "start" will
         only happen once. */
      if( log_ext_hdr_sent )
      {
        desc->state = DIAG_SEND_STATE_BUSY;
        log_ext_hdr_sent = FALSE;
      }
      (void) diagbuf_send_pkt (desc,stream_id);
    }
  }

  return;
}

/* -------------------------------------------------------------------------
 * Internal to diag (aka protected) function prototypes
 * ------------------------------------------------------------------------- */
/* Begin packet processing routines */
/*===========================================================================

FUNCTION LOG_PROCESS_CONFIG_PKT

DESCRIPTION
  This procedure processes a request to set the log mask.

  Once again the log service config packet has evolved.  This time it has
  evolved to be able to handle multiple equipment IDs.  In the past, the MSM
  always assumed the equipment ID is 1.  Multi-mode products, different
  technologies can log with the same service using equipment ID to specify the
  technology.  This means we have 16 masks, one for each equipment ID.

  The extended logmask packet was designed using a very odd bit ordering scheme
  that created a lot of confusion.  To alleviate this confusion, the logmask is
  now a byte array, with each bit being ordered LSB to MSB.  Macros are defined
  to perform these operations.

PARAMETERS
  req_pkt_ptr - Pointer to request packet
  pkt_len - Length of request packet
  
RETURN VALUE
  Pointer to response packet.

  This command effects what type of entries are logged.  
===========================================================================*/
PACK(void *) log_process_config_pkt (
  PACK(void *) req_pkt_ptr, 
  uint16 pkt_len
)
{
  /* For cmd DIAG_LOG_CONFIG_F 115 - Supports Stream 1 */
  DIAG_LOG_CONFIG_F_req_type *req_ptr = (DIAG_LOG_CONFIG_F_req_type *) req_pkt_ptr;
  DIAG_LOG_CONFIG_F_rsp_type *rsp_ptr = NULL;
  /* For Diag subsys cmd DIAG_EXT_LOG_CONFIG 75 18 98 - Supports all streams */
  log_ext_config_req_type *req_dm_ptr = NULL;
  log_ext_config_rsp_type *rsp_dm_ptr = NULL;

  /* Pointers to unions for operation specific data (For non-dual-mask Packets)*/
  log_config_op_req_type *req_op_ptr = NULL;
  log_config_op_rsp_type *rsp_op_ptr = NULL;

  /* Local working variables */
  unsigned int rsp_len, operation, last_item, equip_id = 0;
  uint16 i;
  byte stream_id = 1;
  int dual_mask;
  byte *mask_rd_ptr = NULL;
  byte *mask_wr_ptr = NULL;
  boolean send_rsp = TRUE;
  log_config_status_enum_type status = LOG_CONFIG_SUCCESS_S;

  /* Sanity checks */
  if( (req_pkt_ptr == NULL) || (pkt_len == 0) )
  {
    return NULL;
  }
  
  dual_mask = (req_ptr->xx_header.opaque_header == DIAG_LOG_CONFIG_F)? 0:1;
  
  if( dual_mask ) 
  {
    req_dm_ptr = (log_ext_config_req_type *) req_pkt_ptr;
    
    if( req_dm_ptr->header.cmd_version == 2 )
    {
      return diaglog_process_ext_config_req( req_pkt_ptr, pkt_len ); //ToDo: separate function for now
    }
    else if( req_dm_ptr->header.cmd_version != 1 || req_dm_ptr->header.opcode == LOG_CONFIG_RETRIEVE_VALID_MASK_OP )
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
    }
  }
  else
  {
    req_op_ptr = (log_config_op_req_type *) req_ptr->op_data;
  }

  rsp_len = FPOS(DIAG_LOG_CONFIG_F_rsp_type, op_data);
  operation = (dual_mask)? req_dm_ptr->header.opcode : req_ptr->operation;

  /* First, determine the length of the response.
   * Second, allocate the response.
   * Last, fill in the response. */
  
  /* This switch statement computes rsp_len for the operation */
  switch (operation) 
  {
    case LOG_CONFIG_DISABLE_OP:
      rsp_len = dual_mask ? sizeof(log_ext_config_disable_op_rsp): rsp_len;
      if(dual_mask) {
        // Stream ID 0 is acceptable in this case (to disable all the streams). 
          if(req_dm_ptr->disable_op.id.stream_id > DIAG_MAX_STREAM_ID) { 
          status = LOG_CONFIG_INVALID_STREAM_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
          }
        }
      /* No response op data */
      break;
      
    case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
      rsp_len = dual_mask ? sizeof(log_ext_config_id_ranges_rsp) : (rsp_len + sizeof(log_config_ranges_rsp_type));
      break;

    case LOG_CONFIG_RETRIEVE_VALID_MASK_OP:
      equip_id = req_op_ptr->valid_mask.equip_id;
      if (equip_id > LOG_EQUIP_ID_MAX) {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      rsp_len += FPOS(log_config_valid_mask_rsp_type, mask) +
        (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);
      break;

    case LOG_CONFIG_SET_MASK_OP:
        equip_id = dual_mask? req_dm_ptr->set_mask.code_range.equip_id : req_op_ptr->set_mask.code_range.equip_id;
      if (equip_id > LOG_EQUIP_ID_MAX) {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
        }

        if(dual_mask) {
          if(req_dm_ptr->set_mask.id.stream_id < 1 || req_dm_ptr->set_mask.id.stream_id > DIAG_MAX_STREAM_ID) {
          status = LOG_CONFIG_INVALID_STREAM_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
          }
      }
      rsp_len =  (dual_mask ? FPOS(log_ext_config_set_mask_rsp, mask) : (rsp_len + FPOS(log_config_valid_mask_rsp_type, mask))) +
        (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);
      
      break;

    case LOG_CONFIG_GET_LOGMASK_OP:
      equip_id = dual_mask ? req_dm_ptr->get_mask.equip_id : req_op_ptr->valid_mask.equip_id;
      if (equip_id > LOG_EQUIP_ID_MAX) {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
      }
      if(dual_mask) {
        if(req_dm_ptr->set_mask.id.stream_id < 1 || req_dm_ptr->set_mask.id.stream_id > DIAG_MAX_STREAM_ID) {
          status = LOG_CONFIG_INVALID_STREAM_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }
      }
      rsp_len = (dual_mask ? FPOS(log_ext_config_get_mask_rsp, mask):(rsp_len + FPOS(log_config_valid_mask_rsp_type, mask))) +
         (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);
      
      break;

    default:
      return diagpkt_err_rsp(DIAG_BAD_PARM_F,  req_pkt_ptr, pkt_len);
  } /* end switch for rsp_len computation */
  
  
  rsp_ptr = dual_mask ? rsp_ptr : (DIAG_LOG_CONFIG_F_rsp_type *) diagpkt_alloc(DIAG_LOG_CONFIG_F, rsp_len);
  rsp_dm_ptr = dual_mask ? (log_ext_config_rsp_type *) diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_LOG_CONFIG, rsp_len) : rsp_dm_ptr;

  
  /* For cmd DIAG_LOG_CONFIG_F 115 - Supports Stream 1 */
  if( !dual_mask ) {
    if(rsp_ptr == NULL)
    {
      return rsp_ptr;
    }

    rsp_ptr->pad[0] = 0;
    rsp_ptr->pad[1] = 0;
    rsp_ptr->pad[2] = 0;

    rsp_ptr->operation = operation;

    rsp_op_ptr = (log_config_op_rsp_type *) rsp_ptr->op_data;

  /* This switch statement actually performs the operation */
  switch (operation) {
    case LOG_CONFIG_DISABLE_OP:
           (void) log_config_mask(-1, FALSE, DIAG_STREAM_1, diag_cur_preset_id);
          log_config_mstr_mask(-1, FALSE, DIAG_STREAM_1, diag_cur_preset_id);
      break;

      case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
        for (i = 0; i <= LOG_EQUIP_ID_MAX; i++) {
          rsp_op_ptr->ranges.last_item[i] = (uint32) log_last_item_tbl[i];
        }
        break;

    case LOG_CONFIG_RETRIEVE_VALID_MASK_OP:
      rsp_op_ptr->valid_mask.code_range.equip_id = equip_id;
      rsp_op_ptr->valid_mask.code_range.last_item = log_last_item_tbl[equip_id];

      if (equip_id > LOG_EQUIP_ID_LAST_DEFAULT)
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }

      mask_rd_ptr = (byte *) &log_valid_mask[log_mask_offset_tbl[equip_id]];
      mask_wr_ptr = (byte *) rsp_op_ptr->valid_mask.mask;

      for (i = 0; i <= (unsigned int)
          LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++) {

          *mask_wr_ptr = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;
        }
        break;

    case LOG_CONFIG_SET_MASK_OP:
      rsp_op_ptr->set_mask.code_range.equip_id = equip_id;
      rsp_op_ptr->set_mask.code_range.last_item = log_last_item_tbl[equip_id];
      
      if (equip_id > LOG_EQUIP_ID_LAST_DEFAULT)
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }

#if defined(DIAG_MP_MASTER)
      /* Save master copy of log mask */
      mask_rd_ptr = (byte *) req_op_ptr->set_mask.mask;
      mask_wr_ptr = diag_log_mask_mstr[DIAG_STREAM_1-1]->mask[equip_id]; //&log_mask_mstr[DIAG_STREAM_1-1][equip_id][0];
      last_item = MIN(MSTR_LOG_MAX_LAST_ITEM, req_op_ptr->set_mask.code_range.last_item);
      diag_log_last_item_mstr_tbl[equip_id] = last_item;
      diag_log_mask_mstr[DIAG_STREAM_1-1]->status = DIAG_CTRL_MASK_VALID;
      
      for (i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++) {
        *mask_wr_ptr = *mask_rd_ptr;
        mask_wr_ptr++;
        mask_rd_ptr++;
      }
      /* Zero all bits that are after the last item in the set mask */
      mask_wr_ptr--;
      i--;
      
      /* (The next boundary of 8) - 1 - (last_item) gives the number of bits 
       * at the end of the last byte that are not specified. 
       * This line 0's out the bits that remain in the mask that are not
       * included in last_item. */
/*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
/*lint -save -e817 */
      *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
      
      /* Master needs to keep track to send log masks to Slaves */
      for( i=0; i < NUM_SMD_PORTS; i++ )
      {
        diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][i].log_mask_update[0][equip_id] = TRUE;
      }
#endif
            
      /* Set the log mask */
      mask_rd_ptr = (byte *) req_op_ptr->set_mask.mask;
      mask_wr_ptr = &log_mask[0][log_mask_offset_tbl[equip_id]];
      last_item = MIN(log_last_item_tbl[equip_id], req_op_ptr->set_mask.code_range.last_item);

      for (i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++) {
        *mask_wr_ptr = *mask_rd_ptr;
         rsp_op_ptr->set_mask.mask[i] = *mask_rd_ptr;
        mask_wr_ptr++;
        mask_rd_ptr++;
      }
        
        /* Zero all bits that are after the last item in the set mask */
        mask_wr_ptr--;
        i--;

      /* (The next boundary of 8) - 1 - (last_item) gives the number of bits 
       * at the end of the last byte that are not specified. 
       * This line 0's out the bits that remain in the mask that are not
       * included in last_item. */
/*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
/*lint -save -e817 */
        *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
        rsp_op_ptr->set_mask.mask[i] = *mask_wr_ptr;

        /* Zero out all items that were not specified in the request. */
        for (i = last_item + 1; i <= log_last_item_tbl[equip_id]; i++) {
              (void) log_config_mask(log_set_equip_id(i, equip_id), FALSE, DIAG_STREAM_1, diag_cur_preset_id);
        }

      for( i = LOG_MASK_ARRAY_INDEX(last_item) + 1; i <= 
           LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++)
      {
         rsp_op_ptr->set_mask.mask[i] = 0;
      }
      
      /* Update composite mask */
      log_update_composite_mask(log_mask_offset_tbl[equip_id],
                                log_mask_offset_tbl[equip_id] + LOG_MASK_ARRAY_INDEX(last_item));
      break;

  case LOG_CONFIG_GET_LOGMASK_OP:
      rsp_op_ptr->get_mask.code_range.equip_id = equip_id;
      rsp_op_ptr->get_mask.code_range.last_item = log_last_item_tbl[equip_id];

      if (equip_id > LOG_EQUIP_ID_LAST_DEFAULT)
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      mask_rd_ptr = (byte *) &log_mask[0][log_mask_offset_tbl[equip_id]];
      mask_wr_ptr = (byte *) rsp_op_ptr->valid_mask.mask;

      for (i = 0; i <= (unsigned int)
          LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++) {

          *mask_wr_ptr = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;

        }
        break;
    default:
        break;

    } /* end switch */

    
    if((LOG_CONFIG_SUCCESS_S == status) && 
      (LOG_CONFIG_SET_MASK_OP == operation))
    {
       (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
    }

    /* Config may have changed.  Run sleep vote routine. */
    (void) log_sleep_vote ();
    
    rsp_ptr->status = status;
     
    /* if master processor on multi processor build, don't send response */
    #if !defined(DIAG_RSP_SEND)
     if (!diagcomm_apps_only_config)
      send_rsp = FALSE;
    #endif

    #if defined(DIAG_MP_MASTER)
    if( (operation == LOG_CONFIG_SET_MASK_OP) || (operation == LOG_CONFIG_DISABLE_OP) )
    {
      /* MP Master now responds to "set mask" requests */
      send_rsp = TRUE;
    }
	  else if( (operation == LOG_CONFIG_RETRIEVE_ID_RANGES_OP) ||
	         (operation == LOG_CONFIG_RETRIEVE_VALID_MASK_OP) ||
	         (operation == LOG_CONFIG_GET_LOGMASK_OP) )
      {
	    /* If we're in buffering mode, Master needs to send out the responses.
         If Modem is not up, Master needs to send out responses as well. */
        send_rsp = TRUE;
      }
    #endif
    
  /* if diag is not connected, don't send response */
    if (!diagcomm_status(DIAGCOMM_PORT_1))
    {
      send_rsp = FALSE;
    }

  /* return response */
    if (!send_rsp)
    {
      diagpkt_free(rsp_ptr);
      return NULL;
    }
    else
    {
      return ((void *) rsp_ptr);
    }
  }
  else {
    /* Dual mask */
    if(req_dm_ptr == NULL || rsp_dm_ptr == NULL) {
      return NULL;
    }
    rsp_dm_ptr->header = req_dm_ptr->header;
    switch (operation) {
        case LOG_CONFIG_DISABLE_OP:
          rsp_dm_ptr->disable_op.status = status;
          if ((rsp_dm_ptr->disable_op.id.stream_id = req_dm_ptr->disable_op.id.stream_id) == 0) {
            // Disable all streams
            (void) log_config_mask(-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
            log_config_mstr_mask(-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
          }
          else {
                stream_id = req_dm_ptr->disable_op.id.stream_id;
                // Disable a specific stream given by stream_id
                (void) log_config_mask (-1, FALSE, stream_id, diag_cur_preset_id);
                log_config_mstr_mask(-1, FALSE, stream_id, diag_cur_preset_id);
          }
          break;
    
        case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
          rsp_dm_ptr->id_ranges.status = status;
          rsp_dm_ptr->id_ranges.rsvd = 0;
          for (i = 0; i <= LOG_EQUIP_ID_MAX; i++) {
            rsp_dm_ptr->id_ranges.last_item[i] = (uint32) log_last_item_tbl[i];
          }
          break;
    
        case LOG_CONFIG_SET_MASK_OP:
          rsp_dm_ptr->set_mask.id.stream_id = stream_id = req_dm_ptr->set_mask.id.stream_id;
          rsp_dm_ptr->set_mask.status = status;
          rsp_dm_ptr->set_mask.code_range.equip_id = equip_id;
          rsp_dm_ptr->set_mask.code_range.last_item = log_last_item_tbl[equip_id];
    
          if (equip_id > LOG_EQUIP_ID_LAST_DEFAULT)
          {
            return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
          }
    
          mask_rd_ptr = (byte *) req_dm_ptr->set_mask.mask;
          mask_wr_ptr = &log_mask[req_dm_ptr->set_mask.id.stream_id - 1][log_mask_offset_tbl[equip_id]];
          last_item = MIN(log_last_item_tbl[equip_id], req_dm_ptr->set_mask.code_range.last_item);
    
          for (i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++) {
            *mask_wr_ptr = *mask_rd_ptr;
            rsp_dm_ptr->set_mask.mask[i] = *mask_rd_ptr;
    
            mask_wr_ptr++;
            mask_rd_ptr++;
          }
          /* Zero all bits that are after the last item in the set mask */
          mask_wr_ptr--;
          i--;

          /* (The next boundary of 8) - 1 - (last_item) gives the number of bits 
           * at the end of the last byte that are not specified. 
           * This line 0's out the bits that remain in the mask that are not
           * included in last_item. */
    /*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
    /*lint -save -e817 */
          *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
          rsp_dm_ptr->set_mask.mask[i] = *mask_wr_ptr;
    
          /* Zero out all items that were not spefcified in the request. */
          for (i = last_item + 1; i <= log_last_item_tbl[equip_id]; i++) {
            (void) log_config_mask(log_set_equip_id(i, equip_id), FALSE, stream_id, diag_cur_preset_id);
          }

          for( i = LOG_MASK_ARRAY_INDEX(last_item) + 1; i <= 
               LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++)
          {
             rsp_dm_ptr->set_mask.mask[i] = 0;
          }

          /* Update composite mask */
          log_update_composite_mask(log_mask_offset_tbl[equip_id],
                                    log_mask_offset_tbl[equip_id] + LOG_MASK_ARRAY_INDEX(last_item));
          break;
    
        case LOG_CONFIG_GET_LOGMASK_OP:
          rsp_dm_ptr->get_mask.id.stream_id = req_dm_ptr->get_mask.id.stream_id;
          rsp_dm_ptr->get_mask.status = status;
          rsp_dm_ptr->get_mask.code_range.equip_id = equip_id;
          rsp_dm_ptr->get_mask.code_range.last_item = log_last_item_tbl[equip_id];
    
          if (equip_id > LOG_EQUIP_ID_LAST_DEFAULT)
          {
            return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
          }
          mask_rd_ptr = (byte *) &log_mask[req_dm_ptr->get_mask.id.stream_id - 1][log_mask_offset_tbl[equip_id]];
          mask_wr_ptr = (byte *) rsp_dm_ptr->get_mask.mask;
    
          for (i = 0; i <= (unsigned int)
              LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++) {
    
            *mask_wr_ptr = *mask_rd_ptr;
            mask_wr_ptr++;
            mask_rd_ptr++;
    
          }
          break;
      default:
          break;
    
      } /* end switch */

    if((LOG_CONFIG_SUCCESS_S == status) && 
      (LOG_CONFIG_SET_MASK_OP == operation))
    {
       (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
    }
  
    /* Config may have changed.  Run sleep vote routine. */
    (void) log_sleep_vote ();

    /* if master processor on multi processor build, don't send response */
    #if !defined(DIAG_RSP_SEND)
     if (!diagcomm_apps_only_config)
      send_rsp = FALSE;
    #endif
  
  /* if diag is not connected, don't send response */
    if (!diagcomm_status(DIAGCOMM_PORT_1))
    {
      send_rsp = FALSE;
    }
  
  /* return response */
    if (!send_rsp)
    {
      diagpkt_free(rsp_dm_ptr);
      return NULL;
    }
    else
    {
      return ((void *) rsp_dm_ptr);
    }
  }
} /* log_process_config_pkt */

/*===========================================================================

FUNCTION LOG_ON_DEMAND_REG_INT

DESCRIPTION
  This is an internal function called to register log on demand clients 

===========================================================================*/
boolean log_on_demand_reg_int ( 
  log_code_type log_code,
  PACK(void *) log_on_demand_ptr, 
  log_on_demand_list_item_type* log_on_demand_array, log_on_demand_type type)
{
  int i = 0;
  ASSERT(log_on_demand_array != NULL);
  boolean status = FALSE;

  if (!diag_services_initialized)
    return FALSE;

  osal_lock_mutex(&log_on_demand_mutex); 

  while( i<LOG_ON_DEMAND_SIZE )
  {
    if(log_on_demand_array->log_code == log_code)
    {
      status = TRUE;
      break;
    }
    if(( log_on_demand_array->log_code == 0 ) && \
       (log_on_demand_array->log_on_demand_funct_ptr.ptr == NULL))
     {
      log_on_demand_array->log_code = log_code;
          if (type == LOG_ON_DEMAND)
          {
        log_on_demand_array->log_on_demand_funct_ptr.ptr = log_on_demand_ptr;
      // Optimization variable to see if someone is registered for log on demand
        log_on_demand_reg_count++; 
          }
          else if (type == LOG_ON_DEMAND_EXT)
          {
        log_on_demand_array->log_on_demand_funct_ptr.ext_ptr = log_on_demand_ptr;
      // Optimization variable to see if someone is registered for log on demand ext
        log_on_demand_ext_reg_count++; 
          }
          status = TRUE;
          break;
     } 
    log_on_demand_array++;
    i++;
    }
  osal_unlock_mutex(&log_on_demand_mutex); 
  return status;
}

/*===========================================================================

FUNCTION LOG_ON_DEMAND_UNREG_INT

DESCRIPTION
  This is an internal function called to unregister log on demand clients 

===========================================================================*/
boolean log_on_demand_unreg_int ( 
  log_code_type log_code, log_on_demand_list_item_type* log_on_demand_array, log_on_demand_type type)
{
  int i = 0;
  ASSERT(log_on_demand_array != NULL);
  boolean status = FALSE;

  if (!diag_services_initialized)
    return FALSE;

  osal_lock_mutex(&log_on_demand_mutex); 

  while( i<LOG_ON_DEMAND_SIZE )
  {
    if(log_on_demand_array->log_code == log_code)
     {
          log_on_demand_array->log_code = 0;
          if (type == LOG_ON_DEMAND)
          {
            log_on_demand_array->log_on_demand_funct_ptr.ptr = NULL;
            log_on_demand_reg_count--;
            status = TRUE;
          }
          else if (type == LOG_ON_DEMAND_EXT)
          {
            log_on_demand_array->log_on_demand_funct_ptr.ext_ptr = NULL;
            log_on_demand_ext_reg_count--;
            status = TRUE;
          }
     } 

    log_on_demand_array++;
    i++;
    }
  osal_unlock_mutex(&log_on_demand_mutex); 
  return status;
}
/*===========================================================================

FUNCTION LOG_PROCESS_EXT_LOGMASK

DESCRIPTION
  This function sets the logmask from the extended logmask bit field specified 
  in the diag_ext_logmask_req_type packet.

  This command effects what type of entries are logged.  
===========================================================================*/
void log_process_ext_logmask (
  PACK(uint8 *) logmask_ptr,
  unsigned int  num_bits
)
{
  unsigned int i;
  log_code_type code;
  boolean code_enabled; /* Used to determine if a code is enabled. */

  /*----------------------------------------------------------------------
    For each bit allowed in the logging mask, configure that log condition
    active if the bit is 1 and configure that log condition inactive if
    the bit is 0.
  ----------------------------------------------------------------------*/
  /* Tell lint it is OK to have a constant used as a boolean here */
  /*lint -e506 */
  for(i = 0; i < MIN(((uint32)log_last_item_tbl[1]) + 1, num_bits); i++) {
  /*lint +e506 */
    code_enabled = (logmask_ptr[i >> 3] & (0x80 >> (i & 7))) 
                   ? TRUE : FALSE;

    code = log_set_equip_id(i, 1);

    (void) log_config_mask(code, code_enabled, DIAG_STREAM_1, diag_cur_preset_id);
  }
  
  /*--------------------------------------------------------------------------
    If this DMSS supports more bits than are given, we should set the rest 
    to 0 to make sure they don't get unsuspected logs.
  --------------------------------------------------------------------------*/
  for (; i < (uint32) (log_last_item_tbl[1] + 1); i++) {
    code = log_set_equip_id(i, 1);

    (void) log_config_mask(code, FALSE, DIAG_STREAM_1, diag_cur_preset_id);
  }
  
#if defined(DIAG_MP_MASTER)
  /* Save to master log mask */
  for( i = 0; i < MIN(MSTR_LOG_MAX_LAST_ITEM, num_bits); i++ ) 
  {
    code_enabled = (logmask_ptr[i >> 3] & (0x80 >> (i & 7))) ? TRUE : FALSE;
    code = log_set_equip_id(i, 1);
    log_config_mstr_mask(code, code_enabled, DIAG_STREAM_1, diag_cur_preset_id);
  }
#endif

  /* Vote on sleep for the logging service. */
  (void) log_sleep_vote ();
  
} /* log_process_ext_logmask */



/*===========================================================================

FUNCTION LOG_PROCESS_EXT_LOGMASK_PKT

DESCRIPTION
  This procedure processes a request to change the logmask.

RETURN VALUE
  Pointer to response packet.

WARNING
  This function support Obsolete cmd 93, which is replaced by cmd 115.
  However, support for 93 is left in place to support Legacy tools

  When num_bits == 0 in the request, the command queries the log valid
  mask. However, looking at the code below, log_valid_mask[] is never set.
  Therefore the "get log mask" request for cmd 93 is likely broken.
  
===========================================================================*/
PACK(void *) log_process_ext_logmask_pkt (
  PACK(void *) req_pkt_ptr, /* pointer to request packet */
  uint16 pkt_len            /* length of request packet  */
)
{
  DIAG_EXT_LOGMASK_F_req_type *req_ptr = (DIAG_EXT_LOGMASK_F_req_type *) req_pkt_ptr;
  DIAG_EXT_LOGMASK_F_rsp_type *rsp_ptr = NULL;
  const unsigned int rsp_len = sizeof(DIAG_EXT_LOGMASK_F_rsp_type);

  uint32 i;
  const byte *valid_mask_ptr = NULL;
  boolean enabled;
  boolean send_rsp = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Quick sanity check */
  if (pkt_len != FPOS(DIAG_EXT_LOGMASK_F_req_type, mask) +  
                 (req_ptr->num_bits + 7) / 8) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  if (req_ptr->num_bits == 0) {
    /*------------------------------------------------------- 
      Report the number of valid bits as well as the mask of 
      valid bits for this build
    -------------------------------------------------------*/
    rsp_ptr = (DIAG_EXT_LOGMASK_F_rsp_type *) diagpkt_alloc(DIAG_EXT_LOGMASK_F, rsp_len);
	if(rsp_ptr){
    rsp_ptr->num_valid_bits = log_last_item_tbl[1] + 1;
    
    valid_mask_ptr = &log_valid_mask[log_mask_offset_tbl[1]];

    for (i = 0; i < DIAG_EXT_LOGMASK_NUM_BYTES; i++) {
      rsp_ptr->valid_mask[i] = 0;
    }

    for (i = 0; i < DIAG_EXT_LOGMASK_NUM_BITS; i++) {
      enabled = (log_mask_enabled(log_valid_mask, 1, i)) ? TRUE : FALSE;

      if (enabled) {
        rsp_ptr->valid_mask[i >> 3] |= 0x80 >> (i & 7);
      }
    }
   }
  } else {

   (void) valid_mask_ptr; /* to supress lint (Warning) 550 */

    /* Process the logmask */
    log_process_ext_logmask(&req_ptr->mask[0], req_ptr->num_bits);

    /* Only return a command code */
    rsp_ptr = (DIAG_EXT_LOGMASK_F_rsp_type *) diagpkt_alloc(DIAG_EXT_LOGMASK_F, 
      FPOS(DIAG_EXT_LOGMASK_F_rsp_type, num_valid_bits));
  }
  
   
  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if(!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif

  #if defined(DIAG_MP_MASTER)
    /* MP Master now responds to "set mask" requests.
       If we're in buffering mode, Master needs to send out the "get mask" responses.
       If Modem is not up, Master needs to send out responses as well.
    */
    send_rsp = TRUE;
  #endif
  
/* if diag is not connected, don't send response */
  if (!diagcomm_status(DIAGCOMM_PORT_1))
  {
    send_rsp = FALSE;
  }
  
/* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp_ptr);
    return NULL;
  }
  else
  {
    return ((void *) rsp_ptr);
  }
} /* log_process_ext_logmask_pkt */


/*===========================================================================

FUNCTION LOG PKT GET LOCAL EQUIPID RANGE

DESCRIPTION
  This function will return the number of log codes in each equipment id.
  This function queries the local Diag log mask table, as opposed to log_process_config_pkt(),
  which queries the target master log mask table

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACK(void *) log_pkt_get_local_equipid_range (
  PACK(void *) req_pkt_ptr, /* pointer to request packet */
  uint16 pkt_len            /* length of request packet  */
)
{
   log_get_local_equipid_range_req_type *req_ptr = (log_get_local_equipid_range_req_type *) req_pkt_ptr;
   log_get_local_equipid_range_rsp_type *rsp = NULL;
   int i = 0;
   diagpkt_subsys_cmd_code_type cmd_code;

   if(req_ptr == NULL) 
   {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"log_pkt_get_local_equipid_range(): Request packet pointer is NULL \n",1);
     return NULL;
   }
 
   cmd_code = diagpkt_subsys_get_cmd_code (req_ptr);
   if ((pkt_len != sizeof(log_get_local_equipid_range_req_type)) || 
       ((cmd_code != DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_MODEM) && (cmd_code != DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_APP)
         && (cmd_code != DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_Q6) && (cmd_code != DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE_RIVA)))
    {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Value of cmd code: %d\n",cmd_code);
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len));
    }
 
   if(diagcomm_status(DIAGCOMM_PORT_1)) 
   {
      rsp = (log_get_local_equipid_range_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
                                              cmd_code, sizeof(log_get_local_equipid_range_rsp_type));

      if (rsp) 
     {
       rsp->status=SUCCESSFUL;
  
       for (i = 0; i <= LOG_EQUIP_ID_MAX; i++) 
       {
           rsp->equipid_ranges[i] = (uint32) log_last_item_tbl[i];      
       }
     }

   }
   
   return ((void *) rsp);

}

/*===========================================================================

FUNCTION LOG PKT GET LOCAL MASKS

DESCRIPTION
  This function will return the number of log mask values
  for a particular equipment id specified in the request packet.
  This function queries the local Diag log mask table, as opposed to log_process_config_pkt(),
  which queries the target master log mask table

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACK(void *) log_pkt_get_local_masks (
  PACK(void *) req_pkt_ptr, /* pointer to request packet */
  uint16 pkt_len            /* length of request packet  */
)
{
   log_get_local_mask_req_type *req_ptr = (log_get_local_mask_req_type *) req_pkt_ptr;
   log_get_local_mask_rsp_type *rsp = NULL;
   unsigned int rsp_len = 0;
   int i = 0;
   byte *mask_rd_ptr = NULL;
   byte *mask_wr_ptr = NULL;
   diagpkt_subsys_cmd_code_type cmd_code;

   if(req_ptr == NULL) 
  {
   MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "log_pkt_get_local_masks(): Request pointer is NULL \n",1);
   return NULL;
  }   
     
   cmd_code = diagpkt_subsys_get_cmd_code (req_ptr); 

   if ((pkt_len != sizeof(log_get_local_mask_req_type)) || 
       ((cmd_code != DIAGDIAG_LOG_MASK_RETRIEVAL_MODEM) && (cmd_code != DIAGDIAG_LOG_MASK_RETRIEVAL_APP)
         && (cmd_code != DIAGDIAG_LOG_MASK_RETRIEVAL_Q6) && (cmd_code != DIAGDIAG_LOG_MASK_RETRIEVAL_RIVA)) || 
       (req_ptr->stream_id > DIAG_MAX_STREAM_ID) || (req_ptr->stream_id < DIAG_MIN_STREAM_ID) || 
       (req_ptr->equip_id > LOG_EQUIP_ID_MAX))
    {
     MSG_3(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "MSG_3 cmd_code %d stream id %d equipment id %d \n",cmd_code,req_ptr->stream_id,
           req_ptr->equip_id );
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len));
    }

   if(diagcomm_status(DIAGCOMM_PORT_1)) 
   {
       rsp_len = FPOS (log_get_local_mask_rsp_type, log_mask) +
                                ((LOG_MASK_ARRAY_INDEX(log_last_item_tbl[req_ptr->equip_id]) + 1) * sizeof(byte));
       rsp = (log_get_local_mask_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
                                              cmd_code, rsp_len);

   if (rsp) 
   {
      rsp->equip_id = req_ptr->equip_id;
      rsp->status = SUCCESSFUL;
      rsp->stream_id = req_ptr->stream_id;
      if(req_ptr->equip_id >LOG_EQUIP_ID_LAST && req_ptr->equip_id <=LOG_EQUIP_ID_MAX)
      {
        rsp->status =  LOG_CONFIG_NO_VALID_MASK_S; 
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_LOW, "unsupported equipment ID. cmd_code %d stream id %d equipment id %d \n",cmd_code,req_ptr->stream_id,
           req_ptr->equip_id );
        rsp->log_mask[0] = 0; //Since log_last_item_tbl[euip_id] for these will be 0, so rsp->log_mask array will be of size 1.		    
		    return ((void *) rsp);
      }
      
      mask_rd_ptr = (byte *) &log_mask[(req_ptr->stream_id) - 1][log_mask_offset_tbl[req_ptr->equip_id]];
      mask_wr_ptr = (byte *) rsp->log_mask;
    
      for (i = 0; i <= (unsigned int)
                LOG_MASK_ARRAY_INDEX(log_last_item_tbl[req_ptr->equip_id]); i++) 
      {
        *mask_wr_ptr = *mask_rd_ptr;
        mask_wr_ptr++;
        mask_rd_ptr++;    
      }
    }
  }
   return ((void *) rsp);
}
/*===========================================================================

FUNCTION DIAGLOG_PROCESS_EXT_CONFIG_REQ

DESCRIPTION
  This procedure processes a request to set the log mask for specific 
  preset id masks for Stream 1.
  
PARAMETERS
  req_pkt_ptr - Pointer to request packet
  pkt_len - Length of request packet
  
RETURN VALUE
  Pointer to response packet.

  This command effects what type of entries are logged.  
===========================================================================*/
PACK(void *) diaglog_process_ext_config_req (
  PACK(void *) req_pkt_ptr, 
  uint16 pkt_len
)
{
  /* For Diag subsys cmd DIAG_EXT_LOG_CONFIG 75 18 98 - Supports all streams */
  log_ext_config_req_type *req_dm_ptr = NULL;
  log_ext_config_rsp_type *rsp_dm_ptr = NULL;
  boolean send_rsp = TRUE;
  
  /* Local working variables */
  unsigned int rsp_len, operation, last_item, equip_id = 0;
  uint16 i = 0;
  uint8 preset_id;
  byte *mask_rd_ptr = NULL;
  byte *mask_wr_ptr = NULL;
  log_config_status_enum_type status = LOG_CONFIG_SUCCESS_S;
  
  /* Sanity checks */
  if( (req_pkt_ptr == NULL) || (pkt_len == 0) )
  {
    return NULL;
  }
  
  req_dm_ptr = (log_ext_config_req_type *) req_pkt_ptr;
  operation = req_dm_ptr->header.opcode;
  
   /* if master processor on multi processor build, don't send response */
    #if !defined(DIAG_RSP_SEND)
     if (!diagcomm_apps_only_config)
      send_rsp = FALSE;
    #endif

	#if defined(DIAG_MP_MASTER)
    if( (operation == LOG_CONFIG_SET_MASK_OP) ||
			(operation == LOG_CONFIG_DISABLE_OP) ||
			(operation == LOG_CONFIG_RETRIEVE_ID_RANGES_OP) ||
	         (operation == LOG_CONFIG_RETRIEVE_VALID_MASK_OP) ||
	         (operation == LOG_CONFIG_GET_LOGMASK_OP) )
    {
		/* MP Master now responds to "set mask" requests.
		 If we're in buffering mode, Master needs to send out the responses
	     for all common requests.
         If Modem is not up, Master needs to send out responses as well. */
		send_rsp = TRUE;
    }
	#endif
	
    
  /* if diag is not connected, don't send response */
    if (!diagcomm_status(DIAGCOMM_PORT_1))
    {
      send_rsp = FALSE;
    }


  
  if(send_rsp == TRUE)
  {
  switch( operation )
  {
    case LOG_CONFIG_DISABLE_OP:
    {
      preset_id = req_dm_ptr->disable_op.id.preset_id;
      rsp_len = sizeof(log_ext_config_disable_op_rsp);
      
      // Preset ID 0 is acceptable in this case (to disable all the preset masks). 
      if( preset_id > DIAG_MAX_PRESET_ID ) 
      {
        status = LOG_CONFIG_INVALID_PRESET_ID;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
      }
      break;
    }
      
    case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
    {
      /* To satisfy compiler warning - Error: C3017W: preset_id may be used before being set */
      preset_id = diag_cur_preset_id;
      rsp_len = sizeof(log_ext_config_id_ranges_rsp);
      break;
    }
    
    case LOG_CONFIG_SET_MASK_OP:
    {
      equip_id = req_dm_ptr->set_mask.code_range.equip_id;
      preset_id = req_dm_ptr->set_mask.id.preset_id;
      
      if( equip_id > LOG_EQUIP_ID_MAX ) 
      {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
      }

      if( (preset_id == 0) || (preset_id > DIAG_MAX_PRESET_ID) )
      {
        status = LOG_CONFIG_INVALID_PRESET_ID;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
      }
      rsp_len = FPOS(log_ext_config_set_mask_rsp, mask) + (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);
      break;
    }
      
    case LOG_CONFIG_GET_LOGMASK_OP:
    {
      equip_id = req_dm_ptr->get_mask.equip_id;
      preset_id = req_dm_ptr->set_mask.id.preset_id;
      
      if( equip_id > LOG_EQUIP_ID_MAX ) 
      {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
      }
      
      if( req_dm_ptr->set_mask.id.preset_id == 0 || preset_id > DIAG_MAX_PRESET_ID) 
      {
        status = LOG_CONFIG_INVALID_PRESET_ID;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
      }
      rsp_len = FPOS(log_ext_config_get_mask_rsp, mask) + (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);
      break;
    }
      
    default:
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
      
  } /* end switch for rps_len computation */
  
  rsp_dm_ptr = (log_ext_config_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_LOG_CONFIG, rsp_len);
  if( rsp_dm_ptr == NULL)
    return NULL;

  rsp_dm_ptr->header = req_dm_ptr->header;
  switch( operation )
  {
    case LOG_CONFIG_DISABLE_OP:
    {
      rsp_dm_ptr->disable_op.status = status;
      rsp_dm_ptr->disable_op.id.preset_id = preset_id;
      if( preset_id == 0) 
      {
        (void) log_config_mask(-1, FALSE, DIAG_STREAM_1, DIAG_PRESET_MASK_ALL);
        
#if defined(DIAG_MP_MASTER)
        log_config_mstr_mask(-1, FALSE, DIAG_STREAM_1, DIAG_PRESET_MASK_ALL);
        /* Propagate masks to Slaves. */
        diagcomm_mask_update( DIAG_UPDATE_MASK_LOGS, DIAG_STREAM_1, DIAG_PRESET_MASK_ALL, TRUE );
#endif
      }
      else
      {
        (void) log_config_mask(-1, FALSE, DIAG_STREAM_1, preset_id);
        
#if defined(DIAG_MP_MASTER)
        log_config_mstr_mask(-1, FALSE, DIAG_STREAM_1, preset_id);
        /* Propagate masks to Slaves. */
        diagcomm_mask_update( DIAG_UPDATE_MASK_LOGS, DIAG_STREAM_1, preset_id, TRUE );
#endif
      }      
      break;
    }
      
    case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
    {
      rsp_dm_ptr->id_ranges.status = status;
      rsp_dm_ptr->id_ranges.rsvd = 0;
      for (i = 0; i <= LOG_EQUIP_ID_MAX; i++) 
      {
        rsp_dm_ptr->id_ranges.last_item[i] = (uint32)log_last_item_tbl[i];
      }
      break;
    }
    
    case LOG_CONFIG_SET_MASK_OP:
    {
      rsp_dm_ptr->set_mask.id.preset_id = preset_id;
      rsp_dm_ptr->set_mask.status = status;
      rsp_dm_ptr->set_mask.code_range.equip_id = equip_id;
      rsp_dm_ptr->set_mask.code_range.last_item = log_last_item_tbl[equip_id];
    
      if( equip_id > LOG_EQUIP_ID_LAST_DEFAULT )
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
      }
   
      mask_rd_ptr = (byte *) req_dm_ptr->set_mask.mask;
      mask_wr_ptr = &diag_log_preset_mask[preset_id - 1][log_mask_offset_tbl[equip_id]];
      last_item = MIN(log_last_item_tbl[equip_id], req_dm_ptr->set_mask.code_range.last_item);
    
      for( i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++ ) 
      {
        *mask_wr_ptr = *mask_rd_ptr;
        rsp_dm_ptr->set_mask.mask[i] = *mask_rd_ptr;
    
        mask_wr_ptr++;
        mask_rd_ptr++;
      }
      /* Zero all bits that are after the last item in the set mask */
      mask_wr_ptr--;
      i--;

      /* (The next boundary of 8) - 1 - (last_item) gives the number of bits 
       * at the end of the last byte that are not specified. 
       * This line 0's out the bits that remain in the mask that are not
       * included in last_item. */
      /*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
      /*lint -save -e817 */
      *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
      rsp_dm_ptr->set_mask.mask[i] = *mask_wr_ptr;
    
      /* Zero out all items that were not spefcified in the request. */
      for( i = last_item + 1; i <= log_last_item_tbl[equip_id]; i++) 
      {
        (void)log_config_mask(log_set_equip_id(i, equip_id), FALSE, DIAG_STREAM_1, preset_id);
      }

      for( i = LOG_MASK_ARRAY_INDEX(last_item) + 1; i <= 
               LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++)
      {
         rsp_dm_ptr->set_mask.mask[i] = 0;
      }
      
#if defined(DIAG_MP_MASTER)
      /* Save master copy of log mask */
      mask_rd_ptr = (byte *)req_dm_ptr->set_mask.mask;
      mask_wr_ptr = (byte *)diag_log_mask_mstr_preset[preset_id-1].mask[equip_id];
      last_item = MIN(MSTR_LOG_MAX_LAST_ITEM, req_dm_ptr->set_mask.code_range.last_item);
      diag_log_last_item_mstr_tbl[equip_id] = last_item;
      diag_log_mask_mstr_preset[preset_id-1].status = DIAG_CTRL_MASK_VALID;
      
      for( i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++ ) 
      {
        *mask_wr_ptr = *mask_rd_ptr;
        mask_wr_ptr++;
        mask_rd_ptr++;
      }
      /* Zero all bits that are after the last item in the set mask */
      mask_wr_ptr--;
      i--;
      
      /* (The next boundary of 8) - 1 - (last_item) gives the number of bits 
       * at the end of the last byte that are not specified. 
       * This line 0's out the bits that remain in the mask that are not
       * included in last_item. */
/*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
/*lint -save -e817 */
      *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
      
      /* Master needs to keep track to send log masks to Slaves */
      for( i=0; i < NUM_SMD_PORTS; i++ )
      {
        diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][i].log_mask_preset_update[preset_id-1][equip_id] = TRUE;
      }
      
      /* Propagate masks to Slaves. */
      diagcomm_mask_update( DIAG_UPDATE_MASK_PARTIAL_LOGS, DIAG_STREAM_1, preset_id, TRUE );
#endif
      break;
    }
      
    case LOG_CONFIG_GET_LOGMASK_OP:
    {
      rsp_dm_ptr->get_mask.id.preset_id = preset_id;
      rsp_dm_ptr->get_mask.status = status;
      rsp_dm_ptr->get_mask.code_range.equip_id = equip_id;
      rsp_dm_ptr->get_mask.code_range.last_item = log_last_item_tbl[equip_id];
    
      if( equip_id > LOG_EQUIP_ID_LAST_DEFAULT )
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
      }
      mask_rd_ptr = (byte *)&diag_log_preset_mask[preset_id - 1][log_mask_offset_tbl[equip_id]];
      mask_wr_ptr = (byte *)rsp_dm_ptr->get_mask.mask;
    
      for( i = 0; i <= (unsigned int)LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++ ) 
      {
        *mask_wr_ptr = *mask_rd_ptr;
        mask_wr_ptr++;
        mask_rd_ptr++;
      }
      break;
    }
    
  } /* end switch */
 }
  
  return rsp_dm_ptr;
  
} /* diaglog_process_ext_config_req */


/*===========================================================================

FUNCTION LOG_PROCESS_LEGACY_LOGMASK

DESCRIPTION
  This procedure processes a request to change the logmask.
  Note: This is for the orignal 32-bit logmask that the DM used.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
 
===========================================================================*/
PACK(void *) log_process_legacy_logmask (
  PACK(void *) req_pkt_ptr,
  uint16 pkt_len
)
{
  uint32 i;                /* index to loop through logmask */
  boolean code_enabled; /* Used to determine if a code is enabled. */

  DIAG_LOGMASK_F_req_type *req_ptr = (DIAG_LOGMASK_F_req_type *) req_pkt_ptr;
  DIAG_LOGMASK_F_rsp_type *rsp_ptr = NULL;
  boolean send_rsp = TRUE;
  const unsigned int rsp_len = sizeof(DIAG_LOGMASK_F_rsp_type);
   
  unsigned int mask = req_ptr->mask;
  const uint32 numbits = log_last_item_tbl[1] + 1;

  (void) pkt_len; /* suppress compiler warning */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For each bit allowed in the logging mask, configure that log condition
   * active if the bit is 1 and configure that log condition inactive if
   * the bit is 0. */

  /* Tell lint it is OK to have a constant used as a boolean here */
  /*lint -e506 */
  for( i = 0; i < MIN(numbits, 32); i++ )
  {
  /*lint +e506 */

    code_enabled = (boolean) (mask & 0x01);

    /* Configure logmask */
    (void) log_config_mask(log_set_equip_id(i, 1), code_enabled, DIAG_STREAM_1, diag_cur_preset_id); //ToDo: save to master?

    mask >>= 1;
  }

  /* Set any codes above 32 to zero. */
  for(; i < numbits; i++)
  {
    (void) log_config_mask (log_set_equip_id(i, 1), FALSE, DIAG_STREAM_1, diag_cur_preset_id); //ToDo: save to master?
  }

  /* Vote on sleep for the logging service. */
  (void) log_sleep_vote ();

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if (!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif

/* if diag is not connected, don't send response */
  if (!diagcomm_status(DIAGCOMM_PORT_1))
  {
    send_rsp = FALSE;
  }
  if (send_rsp) 
    rsp_ptr = (DIAG_LOGMASK_F_rsp_type *) diagpkt_alloc (DIAG_LOGMASK_F, rsp_len);

  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
  return (rsp_ptr);

} /* log_process_legacy_logmask */

/* -------------------------------------------------------------------------
 * Internal (aka private) function prototypes
 * ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION LOG_SEARCH_MASK

DESCRIPTION
  This function returns a boolean indicating TRUE if the given ID and 'item'
  denotes a valid and enabled log code for the given stream.

===========================================================================*/
static boolean log_search_mask (
  unsigned int id, 
  unsigned int item,
  byte stream_id
)
{
  boolean return_val = FALSE;

  if(stream_id <= 0 || stream_id > DIAG_MAX_STREAM_ID) 
    return return_val;

  /* if valid code val */
  if (log_mask_enabled(log_mask[stream_id - 1], id, item)) {
    return_val = TRUE;
  }
  return return_val;

} /* log_search_mask */



/*===========================================================================

FUNCTION LOG_MASK_ENABLED
      
DESCRIPTION
  This function returns a boolean indicating if the specified code is enabled.

  The equipment ID and item code are passed in to avoid duplicating the
  calculation within this call.  It has to be done for most routines that call
  this anyways.
   
===========================================================================*/
boolean log_mask_enabled (
  const byte *mask_ptr, 
  unsigned int id, 
  unsigned int item
)
{
  unsigned int offset, index, mask;
  boolean enabled = FALSE;

  if (id <= LOG_EQUIP_ID_LAST && item <= log_last_item_tbl[id]) {

    offset = log_mask_offset_tbl[id];

    index = LOG_MASK_ARRAY_INDEX(item);

    mask = LOG_MASK_BIT_MASK(item);

    if (mask_ptr[offset + index] & mask) {
      enabled = TRUE;
    }
  } else {
    /* It is expected that this will be compiled out in a target build */
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Invalid code given 0x%x", (id << 12) | item);
  }
  return enabled;
} /* log_mask_enabled */



/*===========================================================================

FUNCTION LOG_SET_EQUIP_ID
      
DESCRIPTION
  This function returns the equipment ID of the specified 'code' 

===========================================================================*/
static log_code_type log_set_equip_id (
  unsigned int code,
  unsigned int equip_id
)
{
  ASSERT(equip_id <= LOG_EQUIP_ID_MAX);

  code &= 0x00000FFF;

  equip_id &= 0x0000000F;
    
  equip_id <<= 12;

  return (log_code_type) code | (log_code_type)equip_id;

} /* log_set_equip_id */



/*===========================================================================

FUNCTION LOG_SLEEP_VOTE

DESCRIPTION
  This function checks the configuration of the logging service to determine 
  if the system is allowed to sleep.  All log items must be disabled to 
  allow sleep.

===========================================================================*/
static boolean log_sleep_vote (void)
{
  unsigned int id = 0;
  uint16 item = 0;
  boolean ok2sleep = TRUE;

  /* This is not very efficient, but it works.  It only gets called
     when a config packet is sent, not from any clients.  Therefore,
     runtime efficiency is not a huge issue. */

  for (id = 0; ok2sleep && id <= LOG_EQUIP_ID_MAX; id++)
  {
    for (item = 0; ok2sleep && item <= log_last_item_tbl[id]; item++)
    {
      if (log_mask_enabled (log_mask[DIAG_STREAM_1-1], id, item))
      {
        ok2sleep = FALSE;
      }
    }
  }

  diag_sleep_vote (DIAG_SLEEP_LOG, ok2sleep);

  return ok2sleep;

}


/*===========================================================================

FUNCTION LOG_ON_DEMAND_PKT

DESCRIPTION
  This procedure processes a log on demand request. It scans in the 
  log_on_demand_list to match the log_code. If it find the log_code it
  processes the function pointer associated with it.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
 
===========================================================================*/
PACK(void *) log_on_demand_pkt (
  PACK(void *) req_pkt_ptr,
  uint16 pkt_len
)
{
  DIAG_LOG_ON_DEMAND_F_req_type *req_ptr = (DIAG_LOG_ON_DEMAND_F_req_type *) req_pkt_ptr;
  DIAG_LOG_ON_DEMAND_F_rsp_type *rsp_ptr = NULL;
  boolean send_rsp = TRUE;
  int i;
   const unsigned int rsp_len = sizeof( DIAG_LOG_ON_DEMAND_F_rsp_type );

  if ((req_ptr == NULL) || ((int)(req_ptr->xx_header.opaque_header) != DIAG_LOG_ON_DEMAND_F))
  {
    return NULL;
  }
   (void) pkt_len; // suppress compiler warning 
  rsp_ptr = (DIAG_LOG_ON_DEMAND_F_rsp_type *) diagpkt_alloc( DIAG_LOG_ON_DEMAND_F, rsp_len );
  
  if( rsp_ptr == NULL ) {
    // If we can't allocate, diagpkt_err_rsp() can't either. 
    return NULL;
  }

  rsp_ptr->log_code = req_ptr->log_code;
  rsp_ptr->status = LOG_ON_DEMAND_ACKNOWLEDGE_S;

  if (log_on_demand_reg_count != 0)
    {
     for( i=0; i<LOG_ON_DEMAND_SIZE; i++ ) 
     {
      if ( ( log_on_demand_list[i].log_on_demand_funct_ptr.ptr != NULL ) && (log_on_demand_list[i].log_code == req_ptr->log_code) ) 
        {
            rsp_ptr->status=(*log_on_demand_list[i].log_on_demand_funct_ptr.ptr)
              (log_on_demand_list[i].log_code);
            break;
        }
     }
    }

#if defined(DIAG_MP_MASTER)
  // Log on demand should be responded by Modem if feature mask bit (2 for log on demand)
  //   i.e. log on demand bit is not set by modem.
  //   Since send_rsp is init to TRUE above, following condition says that since the bit is set on modem APPS should respond
  
    if ( !DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask,(diag_feature_query_enum_type)F_DIAG_LOG_ON_DEMAND_RSP_ON_MASTER))
#else
  // This will execute on modem, or other peripherals. It will work for 'modem only' configuration as feature mask will not be set 
    if ( DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].feature_mask,(diag_feature_query_enum_type)F_DIAG_LOG_ON_DEMAND_RSP_ON_MASTER))
#endif
  {
	  send_rsp = FALSE; // if this execute it says the bit is not set on modem, modem should respond, APPS should not respond.
  }

  // if diag is not connected, don't send response 
  if (!diagcomm_status(DIAGCOMM_PORT_1))
  {
    if(!diagcomm_apps_only_config)
    send_rsp = FALSE;
  }

  // return response 
  if (!send_rsp)
  {
    diagpkt_free(rsp_ptr);
    return NULL;
  }
  else
  {
    return ((void *) rsp_ptr);
  }

}  /*log_on_demand_pkt */

/*===========================================================================

FUNCTION LOG_ON_DEMAND_EXT_PKT

DESCRIPTION
  This procedure processes a log on demand extended request. It scans in the 
  log_on_demand_ext_list to match the log_code. If it find the log_code it
  processes the function pointer associated with it.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
 
===========================================================================*/
PACK(void *) log_on_demand_ext_pkt (
  PACK(void *) req_pkt_ptr,
  uint16 pkt_len
)
{
  DIAG_LOG_ON_DEMAND_EXT_F_req_type *req_ptr = (DIAG_LOG_ON_DEMAND_EXT_F_req_type *) req_pkt_ptr;
  DIAG_LOG_ON_DEMAND_F_rsp_type *rsp_ptr = NULL;
  boolean send_rsp = TRUE;
  int i;
   const unsigned int rsp_len = sizeof( DIAG_LOG_ON_DEMAND_F_rsp_type );

  if ((req_ptr == NULL) || ((int)(req_ptr->xx_header.opaque_header) != DIAG_LOG_ON_DEMAND_EXT_F))
  {
    return NULL;
  }
   (void) pkt_len; // suppress compiler warning 
    rsp_ptr = (DIAG_LOG_ON_DEMAND_F_rsp_type *) diagpkt_alloc( DIAG_LOG_ON_DEMAND_EXT_F, rsp_len);
  
  if( rsp_ptr == NULL ) {
    // If we can't allocate, diagpkt_err_rsp() can't either. 
    return NULL;
  }

  rsp_ptr->log_code = req_ptr->log_code;
  rsp_ptr->status = LOG_ON_DEMAND_ACKNOWLEDGE_S;
  
  if (log_on_demand_ext_reg_count != 0)
  {
    for( i=0; i<LOG_ON_DEMAND_SIZE; i++ ) 
    {
      if ( ( log_on_demand_ext_list[i].log_on_demand_funct_ptr.ext_ptr != NULL ) && (log_on_demand_ext_list[i].log_code == req_ptr->log_code) ) 
        {
            rsp_ptr->status=(*log_on_demand_ext_list[i].log_on_demand_funct_ptr.ext_ptr)
              (log_on_demand_ext_list[i].log_code, req_ptr->log_param_type, req_ptr->log_param_value);
            break;
      }
    }
  }

#if defined(DIAG_MP_MASTER)
  // Log on demand should be responded by Modem if feature mask bit (2 for log on demand)
  //   i.e. log on demand bit is not set by modem.
  //   Since send_rsp is init to TRUE above, following condition says that since the bit is set on modem APPS should respond
  
    if ( !DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].feature_mask,(diag_feature_query_enum_type)F_DIAG_LOG_ON_DEMAND_EXT_RSP_ON_MASTER))
#else
  // This will execute on modem, or other peripherals. It will work for 'modem only' configuration as feature mask will not be set 
    if ( DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].feature_mask,(diag_feature_query_enum_type)F_DIAG_LOG_ON_DEMAND_EXT_RSP_ON_MASTER))
#endif
  {
	  send_rsp = FALSE; // if this execute it says the bit is not set on modem, modem should respond, APPS should not respond.
  }

  // if diag is not connected, don't send response 
  if (!diagcomm_status(DIAGCOMM_PORT_1))
  {
    if(!diagcomm_apps_only_config)
    send_rsp = FALSE;
  }

  // return response 
  if (!send_rsp)
  {
    diagpkt_free(rsp_ptr);
    return NULL;
  }
  else
  {
    return ((void *) rsp_ptr);
  }

}  /*log_on_demand_ext_pkt */
/*===========================================================================

FUNCTION LOG_UPDATE_COMPOSITE_MASK

DESCRIPTION
  (re)compiles the composite log mask within the specified range
  This takes the log listener mask, the log extended listener mask, and
  all the diag stream masks and ORs together their bits.
  The end result is a mask which has its bits turned on if any of the masks
  show a request for that particular log code.

PARAMETERS 
  start - first mask index to be updated
  end - last mask index to be updated (inclusive)
  
RETURN VALUE
  
  
===========================================================================*/
void log_update_composite_mask(uint32 start, uint32 end)
{
  int mask_index, stream_index;
  byte temp_mask=0;

  if(end >= LOG_MASK_SIZE) {
    end = LOG_MASK_SIZE - 1;
  }

  if(start > end) {
    return;
  }

  for(mask_index = start; mask_index <= end; mask_index++)
  {
    temp_mask  = log_listener_mask[mask_index];
    temp_mask |= log_listener_ext_mask[mask_index];

  if(log_state == LOG_NORMAL_STATE) {
    for(stream_index = 0; stream_index < DIAG_MAX_STREAM_ID; stream_index += 1) {
        temp_mask |= log_mask[stream_index][mask_index];
    }
  }
   /*This is an ANDing operation and has to be done in the end */
  #ifdef FEATURE_SSM
    temp_mask &= diag_log_mask_ssm[mask_index];
  #endif
    log_composite_mask[mask_index] =temp_mask;
  }
}

/*===========================================================================

FUNCTION LOG_STATUS_ADV

DESCRIPTION
   Checks if log code is enabled in composite mask array first.
  If its enabled, then it checks for both streams.If the log code is disabled
  in composite mask array then this function returns 0 as stream id

PARAMETERS
  
RETURN VALUE
  Returns appropriate stream_id.
  
===========================================================================*/
byte log_status_adv (log_code_type code)
{
  byte stream_id = 0;
  boolean status = FALSE;
  boolean status1 = FALSE;
  boolean status2 = FALSE;
   #ifdef FEATURE_SSM
  boolean status_composite_mask = FALSE;
  #endif
  unsigned int id, item;
  
  if (log_state == LOG_NORMAL_STATE || log_state == LOG_FLUSH_STATE)
  {
    id = LOG_GET_EQUIP_ID (code);
    item = LOG_GET_ITEM_NUM (code);

    
    #ifdef FEATURE_SSM 
     status_composite_mask = log_mask_enabled(log_composite_mask, id, item);

     if (!status_composite_mask) 
     {
       return stream_id;
     }
     #endif

    status1 = log_mask_enabled(log_listener_mask, id, item);
    status2 = log_mask_enabled(log_listener_ext_mask, id, item);

    status = status1 || status2;

    if(status)
    {
      /* Set DIAG_STREAM_LISTENER bit if listeners active */
      stream_id |= DIAG_STREAM_LISTENER;
    }

    if(log_state == LOG_NORMAL_STATE)
    {
      if(log_status_mask (code, DIAG_STREAM_1))
        stream_id |= DIAG_STREAM_1;
      if(log_status_mask (code, DIAG_STREAM_2))
        stream_id |= DIAG_STREAM_2;
    }
  }

  return stream_id;
} /* log_status_adv */


/*===========================================================================

FUNCTION DIAG_SWITCH_LOG_PRESET_MASK

DESCRIPTION

PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_switch_log_preset_mask()
{
#if defined(DIAG_MP_MASTER)
  uint8 i, j = 0;
#endif

  log_mask[DIAG_STREAM_1-1] = &diag_log_preset_mask[diag_cur_preset_id-1][0];
  
#if defined(DIAG_MP_MASTER)
  diag_log_mask_mstr[DIAG_STREAM_1-1] = &diag_log_mask_mstr_preset[diag_cur_preset_id-1];
  
  for( i=0; i < NUM_SMD_PORT_TYPES; i++ )
  {
    for( j=0; j < NUM_SMD_PORTS; j++ )
      diagcomm_io_ctrl_conn[i][j].log_mask_update[DIAG_STREAM_1-1] = 
      diagcomm_io_ctrl_conn[i][j].log_mask_preset_update[diag_cur_preset_id-1];
  }
#endif

  /* Re-compose entire composite log mask. */
  log_update_composite_mask(0, LOG_MASK_SIZE-1);

  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);

  /* Config may have changed. Run sleep vote routine. */
  (void) log_sleep_vote ();
  
} /* diag_switch_log_preset_mask */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_SEND_LOG_MASK

DESCRIPTION
  Composes a CTRL msg to send a log mask update to specified Slave Diag.

PARAMETERS
  port_type - Port type (DIAGCOMM_PORT_SIO/DIAGCOMM_PORT_SMD)
  port_num - Port num (DIAGCOMM_PORT_1, DIAGCOMM_PORT_2, ...)
  stream_id - Log stream to send
  preset_id - 
  send_now - If True, send CTRL msg immediately. 
             If False, aggregate until DSM is full before sending.
  
RETURN VALUE
  None
  
===========================================================================*/
/* On MP Master only */
#if defined(DIAG_MP_MASTER)
void diagcomm_ctrl_send_log_mask( diagcomm_port_type port_type, 
                                  diagcomm_enum_port_type port_num, 
                                  uint8 stream_id,
                                  uint8 preset_id,
                                  boolean send_now )
{
  diag_ctrl_msg_type ctrl_log_mask;
  boolean is_sent, update_all = FALSE;
  uint8 equip_id = 0;
  boolean * mask_status_ptr = NULL;
  diag_log_mask_type * log_mask_mstr_ptr = NULL;
  diagcomm_io_conn_type * conn = NULL;

  conn = &diagcomm_io_ctrl_conn[port_type][port_num];
        
  /* Check if stream id is valid */
  if( (stream_id > DIAG_MAX_STREAM_ID) || (stream_id < DIAG_MIN_STREAM_ID) ||
      (preset_id > DIAG_MAX_PRESET_ID) || (preset_id < DIAG_MIN_PRESET_ID) )
    return;
  
  /* Check if port has been opened */
  if( !conn->connected || !conn->channel_connected )
    return;
    
  MSG_4(MSG_SSID_DIAG, MSG_LEGACY_LOW, "send_log_mask port_num=%d stream=%d preset_id=%d send=%d", 
        port_num, stream_id, preset_id, send_now);
        
  if( diagcomm_ctrl_mask_status(port_type, port_num, DIAG_UPDATE_MASK_LOGS, stream_id, preset_id) )
  {
    update_all = TRUE;
    diagcomm_ctrl_clear_mask_status( port_type, port_num, DIAG_UPDATE_MASK_LOGS, stream_id, preset_id );
    diagcomm_ctrl_clear_mask_status( port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_LOGS, stream_id, preset_id ); //ToDo: combine mask clearing
  }
  else if( diagcomm_ctrl_mask_status(port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_LOGS, stream_id, preset_id) )
  {
    diagcomm_ctrl_clear_mask_status( port_type, port_num, DIAG_UPDATE_MASK_PARTIAL_LOGS, stream_id, preset_id );
  }
  else
    return;
  
  /* Grab the right master log mask. */
  if( stream_id == DIAG_STREAM_1 )
  {
    log_mask_mstr_ptr = &diag_log_mask_mstr_preset[preset_id-1];
    mask_status_ptr = conn->log_mask_preset_update[preset_id-1];
  }
  else if(stream_id == DIAG_STREAM_2 )
  {
    log_mask_mstr_ptr = &diag_log_mask_mstr_dci[DIAG_DCI_MASK_1-1];
    mask_status_ptr = conn->log_mask_dci_update[DIAG_DCI_MASK_1-1];
  }
    
  /* Compose DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3 packet */
  ctrl_log_mask.cmd_type = DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3;
  ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.stream_id = stream_id;
  ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.preset_id = preset_id;
  ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.status = log_mask_mstr_ptr->status;

  if( (log_mask_mstr_ptr->status == DIAG_CTRL_MASK_ALL_DISABLED) ||
      (log_mask_mstr_ptr->status == DIAG_CTRL_MASK_ALL_ENABLED) )
  {
    ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.equip_id = 0;
    ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.last_item = 0;
    ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.log_mask_size = 0;
    ctrl_log_mask.data_len = sizeof(diag_ctrl_msg_equip_log_mask_v3_type);
      
    (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_log_mask,
                                       DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_v3_type), 
                                       NULL, 0,
                                       port_type, port_num, send_now, &is_sent );

    for( equip_id = 0; equip_id <= LOG_EQUIP_ID_MAX; equip_id++ )
    {
      mask_status_ptr[equip_id] = FALSE;
    }
  }
  else if( log_mask_mstr_ptr->status == DIAG_CTRL_MASK_VALID )
  {
    for( equip_id = 0; equip_id <= LOG_EQUIP_ID_MAX; equip_id++ )
    {
      if( update_all || (mask_status_ptr[equip_id] == TRUE) )
      {
        if( diag_log_last_item_mstr_tbl[equip_id] > 0 )
        {
          /* Send mask for only equip id's which have log codes */
          ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.equip_id = equip_id;
          ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.last_item = diag_log_last_item_mstr_tbl[equip_id];
          ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.log_mask_size = LOG_MASK_ARRAY_INDEX(diag_log_last_item_mstr_tbl[equip_id])+1;
          ctrl_log_mask.data_len = sizeof(diag_ctrl_msg_equip_log_mask_v3_type) + 
                                   ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.log_mask_size;
        
          (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_log_mask,
                                             DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_v3_type), 
                                             (PACK(void *))&log_mask_mstr_ptr->mask[equip_id][0],
                                             ctrl_log_mask.data.ctrl_msg_equip_log_mask_v3.log_mask_size,
                                             port_type, port_num, send_now, &is_sent );
        }
      }
      mask_status_ptr[equip_id] = FALSE;
    }
    
    /* No more msg mask tables to fwd; Send the current DSM item immediately and then quit */
    diagcomm_ctrl_send_mult_pkt( NULL, 0, NULL, 0, port_type, port_num, send_now, &is_sent );
  }

} /* diagcomm_ctrl_send_log_mask */
#endif /* DIAG_MP_MASTER */


/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_LOG_MASK

DESCRIPTION
  Given a new log mask, updates the local copy of the log mask with the
  new mask.

PARAMETERS
  ctrl_msg - Ptr to CTRL msg
  
RETURN VALUE
  None
  
===========================================================================*/
/* On MP Slave only */
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
void diag_ctrl_update_log_mask( diag_ctrl_msg_type * ctrl_msg )
{
  uint8 * mask_ptr = NULL;
  uint32 mask_len, mask_end, len = 0;
  uint8 stream_id, status, equip_id = 0;

  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_EQUIP_LOG_MASK) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask Sanity check failed");
    return;
  }
  
  mask_len = ctrl_msg->data.ctrl_msg_equip_log_mask.log_mask_size;
  stream_id = ctrl_msg->data.ctrl_msg_equip_log_mask.stream_id;
  status = ctrl_msg->data.ctrl_msg_equip_log_mask.status;
  equip_id = ctrl_msg->data.ctrl_msg_equip_log_mask.equip_id;
                                      
  if( (stream_id > DIAG_MAX_STREAM_ID) || (stream_id < DIAG_MIN_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask: Invalid stream_id %d", stream_id);
    return;
  }

  if(equip_id > LOG_EQUIP_ID_LAST)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask: Invalid equip_id %d", equip_id);
    return;
  }
  
  if( mask_len > (ctrl_msg->data_len - sizeof(diag_ctrl_msg_equip_log_mask_type)) )
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask: Invalid lengths %d, %d", 
          mask_len, ctrl_msg->data_len);
    return;
  }
  
  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {
    (void) log_config_mask( -1, FALSE, stream_id, diag_cur_preset_id );
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED )
  {
    (void) log_config_mask( -1, TRUE, stream_id, diag_cur_preset_id );
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_equip_log_mask_type) + mask_len) )
    {
      MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
             "diag_ctrl_update_log_mask: Invalid lengths %d != %d", 
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_equip_log_mask_type) + mask_len );
      return;
    }
    
    mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_type);
    len = MIN( mask_len, LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id])+1 );
    memscpy( (void*)&log_mask[stream_id-1][log_mask_offset_tbl[equip_id]], 
             LOG_MASK_SIZE-log_mask_offset_tbl[equip_id],mask_ptr, MIN(len, LOG_MASK_SIZE) );

    /* Update composite mask */
    mask_end = log_mask_offset_tbl[equip_id] + len - 1;
    log_update_composite_mask(log_mask_offset_tbl[equip_id], mask_end);
  }
 
  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);

  /* Config may have changed. Run sleep vote routine. */
  (void) log_sleep_vote ();
  
} /* diag_ctrl_update_log_mask */


/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_LOG_PRESET_MASK

DESCRIPTION
  Given a new log mask, updates the local copy of the log mask with the
  new mask.

PARAMETERS
  ctrl_msg - Ptr to CTRL msg
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_ctrl_update_log_preset_mask( diag_ctrl_msg_type * ctrl_msg )
{
  uint8 * ctrl_mask_ptr = NULL;
  byte * log_mask_ptr = NULL;
  uint32 mask_len, mask_end, len = 0;
  uint32 length = 0;
  uint8 stream_id, preset_id, status, equip_id = 0;

  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask Sanity check failed");
    return;
  }
  
  mask_len = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.log_mask_size;
  stream_id = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.stream_id;
  preset_id = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.preset_id;
  status = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.status;
  equip_id = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.equip_id;
  
  MSG_6(MSG_SSID_DIAG, MSG_LEGACY_LOW, "update_log_preset_mask procid=%d stream=%d preset=%d status=%d equip_id=%d len=%d", 
        DIAG_MY_PROC_ID, stream_id, preset_id, status, equip_id, mask_len);
  
  if( equip_id > LOG_EQUIP_ID_LAST )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask: Invalid equip_id %d", equip_id);
    return;
  }
  
  if( mask_len > (ctrl_msg->data_len - sizeof(diag_ctrl_msg_equip_log_mask_v3_type)) )
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask: Invalid lengths %d, %d", 
          mask_len, ctrl_msg->data_len);
    return;
  }
  
  if( stream_id == DIAG_STREAM_1 )
  {
    if( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) )
	{
      log_mask_ptr = &diag_log_preset_mask[preset_id-1][log_mask_offset_tbl[equip_id]];
      length = LOG_MASK_SIZE - log_mask_offset_tbl[equip_id];
	}
    else
      return;
  }
  else if( stream_id == DIAG_STREAM_2 )
  {
    log_mask_ptr = &diag_log_dci_mask[log_mask_offset_tbl[equip_id]];
    length = LOG_MASK_SIZE;
    preset_id = DIAG_DCI_MASK_1;
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask: Invalid stream_id %d", stream_id);
    return;
  }
  
  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {
    (void) log_config_mask( -1, FALSE, stream_id, preset_id );
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED ) 
  {
    (void) log_config_mask( -1, TRUE, stream_id, preset_id );
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_equip_log_mask_v3_type) + mask_len) )
    {
      MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
             "diag_ctrl_update_log_preset_mask: Invalid lengths %d != %d", 
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_equip_log_mask_v3_type) + mask_len );
      return;
    }
    
    ctrl_mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_v3_type);
    len = MIN( mask_len, LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id])+1 );
    memscpy( (void*)log_mask_ptr, length, ctrl_mask_ptr, MIN(len, LOG_MASK_SIZE) );
    
    /* Update composite mask */
    mask_end = log_mask_offset_tbl[equip_id] + len - 1;
    log_update_composite_mask(log_mask_offset_tbl[equip_id], mask_end);
  }
  
  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);

  /* Config may have changed. Run sleep vote routine. */
  (void) log_sleep_vote ();
  
} /* diag_ctrl_update_log_preset_mask */

/*===========================================================================

FUNCTION DIAG_CTRL_SEND_LOG_RANGE_CTRL_PKT

DESCRIPTION
  Sends Log range as well as last item supported by each equipment id to
  Master Diag for build mask centralization support.
  
PARAMETERS
  None
 
RETURN VALUE
  None
  
===========================================================================*/
void diag_ctrl_send_log_range_ctrl_pkt(void)
{
  diag_ctrl_msg_type ctrl_msg;
  uint8 id = 0;
  boolean is_sent = FALSE;
  uint32 bytes_written = 0;

  if(bt_log_range_fwd_status == DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT)
  {
		/* Compose DIAG_CTRL_LOG_RANGE packet */
		ctrl_msg.cmd_type = DIAG_CTRL_BT_LOG_RANGE;
		ctrl_msg.data_len = sizeof(diag_ctrl_log_range_type);
		ctrl_msg.data.ctrl_msg_bt_log_range.version = DIAG_CTRL_MSG_BT_MASK_VER;
		ctrl_msg.data.ctrl_msg_bt_log_range.log_last_equipid = LOG_EQUIP_ID_LAST;
		ctrl_msg.data.ctrl_msg_bt_log_range.num_equipid_ranges = LOG_EQUIP_ID_LAST + 1;

		for (id = 0; id <= LOG_EQUIP_ID_LAST; id++) 
		{
			 ctrl_msg.data.ctrl_msg_bt_log_range.ranges[id].equip_id = id;
			 ctrl_msg.data.ctrl_msg_bt_log_range.ranges[id].num_items = log_last_item_tbl[id];
		}

		bytes_written = diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_log_range_type), 
														NULL, 0, DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1, TRUE, &is_sent );

		if(bytes_written == 0)
			 return;
		else if(is_sent == TRUE)
			 bt_log_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_SENT;
		else /* bytes_written > 0 && is_sent == FALSE */
			 bt_log_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_QUEUED;
  }
}
#endif /* DIAG_MP && !DIAG_MP_MASTER */

