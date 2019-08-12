/*===========================================================================

                   N V   I T E M    M A N A G E R  T A S K
DESCRIPTION
  All the declarations and definitions necessary to support the table  
  lookup of mapping between an NV item and the Flash File System files
  which simulate storage in an EEPROM.  External tasks queue requests for
  this task for NV EEPROM read and write operations.  This tasks then 
  performs the necessary internal operations to read, write
  and control access to the Flash File System files which simulate storage
  in an EEPROM.   
  This task includes the following files:
    nv.h                   -  NVM Item Manager task services
    nvim.h,     nvim.c     -  NVM Item Manager item services
    nvio.h,     nvio.c     -  NVM Item Manager read/write services
    nvi.h                  -  Common definitions

EXTERNALIZED FUNCTIONS
    nv_write_err_log
      Write an error log without requiring any tasking services.
    nv_read_err_log
      Read an error log without requiring any tasking services.
    nv_cmd
      This is the applications read and write interface to this task.
    nv_task
      This is the nv task initialization entry, started up with rex_def_task.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  This task should be started up with rex_def_task.  Caution must be
  taken when invoking error services from this task.  These services invoke
  this task again (to log errors) and could cause an infinite loop.
  

Copyright (c) 2005-2016 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/services/nv/src/nvim.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/06/14   sri     ESN/MEID over write support via SSM
02/28/14   sri     Max trials of SPC unlock over diag handling...
09/02/13   rks     Replaced memcpy with memscpy
07/10/13   rks     NV SSM White listing support
05/09/13   ph      De-coupled NV-RUIM and NV-EFS accesses.
03/21/13   sri     Using DOG Heart Beat to report to Dog. 
02/07/13   rks     Cleanup of TMC TASK Globals, Remove References to TASK.H
09/14/12   sri     Added support for new err codes added by SSM 
08/02/12   din     Added missing check for perm when NV SSM perm file is missing.
07/27/12   sri     Added support for SSM based NV access 
04/02/2012 rks     Removed False Dependency to TMC.H in CoreBSP-Services
08/02/11   ab      removed intlocks and used mutexs in place of them
09/03/10   er      Added code to nv_front_op to check for efs_(read/write) errors. 
10/12/09   as      Moved nvdiag_init call in to NVIM task 
09/25/09   pc      Dual SIM/Dual Standby support for LCU
07/23/09   sri     UMTS build support for remoted functions: nv_rtre_control,
                   nv_rtre_polling_for_nam and nv_rtre_polling_control
07/08/09   sri     Moved function implementations from nv.h and removed inline
05/27/09   sri     Adding "default" case in nvim_remove_item and the function
                   nv_cmd_remote() modified to protect some NV items remote
                   read/write access under FEATURE_NV_ACCESS_FILTER
03/18/09   sri     NV task priority is kept unchanged.
01/08/08   pc      Ensured mutex initialization for synchronous calls.
12/13/07   pc      Introduced kxmutexes around NV access.
06/08/07   pc      Added support for mdn based nai
12/15/06   ck      Include the header file fs_diag_access.h
06/05/05   pc      Created NV-2.
===========================================================================*/

/* ======================================================================

                       INCLUDE FILES FOR MODULE

========================================================================*/

#include "comdef.h"
#include <stringl/stringl.h>

#ifdef FEATURE_NV_ITEM_MGR

#include "nvim.h"
#include "msg.h"
#include "nvio.h"
#include "err.h"
#if defined(FEATURE_RCINIT)
#include "rcevt.h"
#include "rcinit.h"
#endif
#include "dog_hb_rex.h"
#include "crc.h"
#include "memory.h"
#include "fs_public.h"
#include "nv_items.h"
#include "fs_diag_access.h"
#include "diag.h"
#include "rex.h"
#include "osal.h"
#include "ssm.h"

#ifdef FEATURE_CDMA1X
  #include "mc.h"
#endif

#ifdef FEATURE_TREX
#pragma pack(1)
#endif
#include "nvim_items.h"
#ifdef FEATURE_TREX
#pragma pack()
#endif


#ifdef FEATURE_NV_CNV
#include "fs_public.h"
#include "nv_cnv.h"
#endif

#ifdef FEATURE_MDN_BASED_NAI
#define PS_NAI_DFLT_DOMAIN "@default.com"
#include "dsmip.h"
#endif /* FEATURE_MDN_BASED_NAI */

#ifdef FEATURE_WINCE
#error code not present
#endif

#include <stdlib.h> 
#include "nvdiag.h"

#define NV_DOG_HB_REPORT_SIG NV_RPT_TIMER_SIG

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                       CONST DEFINITIONS / MACROS                        */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Signal used in case of remote access clients */
#define NV_CMD_REMOTE_Q_SIG 0x0400

/* defines for NV SSM */
/* Number of NV attributes used in the perms file, caller + NV item */
#ifdef NV_SSM_ATTRIBS_2
#define NV_SSM_ATTR_CNT 2
#else
#define NV_SSM_ATTR_CNT 1 
#endif

/* Number of arguments sent to SSM API */
#define NV_SSM_ARG_C (2 + NV_SSM_ATTR_CNT) 

/* conversion for callers */
#define NV_SSM_CALLER_CONV(x)  (x+1)

/* Generally when a nvssm_<perm>.ssm file(s) is/are provisioned 
 * and if the access permission for any item(s) are not exclusively 
 * mentioned, the default return would be ACCESS_DENIED_PERM */
#define ACCESS_DENIED_PERM ( ((NV_SSM_ACCESS_DENIED_S)      | \
                              (NV_SSM_ACCESS_DENIED_S<<8)   | \
                              (NV_SSM_ACCESS_DENIED_S<<16)) & \
                              (0x00FFFFFF) )

/* Generally when a nvssm_<perm>.ssm file(s) is/are provisioned 
 * and if the access permission for any item(s) are not exclusively 
 * mentioned, and if you want the default return to not have any 
 * specific permission, then use DEFAULT_ACCESS_PERM. This will 
 * allow users to access other NV items like how they were able 
 * to access with out nvssm, this is also employed if SSM returns error */
#define DEFAULT_ACCESS_PERM ( ((NV_SSM_DEFAULT_PERM_S)      | \
		               (NV_SSM_DEFAULT_PERM_S<<8)   | \
			       (NV_SSM_DEFAULT_PERM_S<<16)) & \
                               (0x00FFFFFF) )  

/* OEMS using SSM must change this fallback string to make it as 
 * restrictive ( permission file for default mode ) as possible excluding 
 * factory commands.
 * Max NV# is NV_MAX_I which is much lesser than 8000
 * 4210752 -> 0x00404040 -> DEFAULT_ACCESS_PERM */
#define NV_DEFAULT_WHITELIST "0|8000|4210752|"

 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         DATA DEFINITIONS                                */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* static KxMutex nv_access_op_cs; */
static rex_crit_sect_type nv_access_op_cs;

/* Dog report Id for NV, other dependent modules may extern [TODO] */
dog_report_type nv_dog_rpt_id=0; 


extern rex_crit_sect_type nv_crit_sect;
extern const nvim_item_info_type nvim_item_info_table[];

#ifdef FEATURE_NV_CNV
/* File to back up the data into */
const char   *cnv_file = "/nvm/nv_cnv.bin";
#endif

boolean  nvi_task_running=FALSE;

/* Contains the roaming list "valid" indicator */
boolean  nv_prl_valid_data[NV_MAX_NAMS];

/* Contains the roaming list "prl_version" field */
word     nv_prl_version_data[NV_MAX_NAMS];

/* Buffer holding the union of all internal NVM storage types.  Use as    */
/* a working buffer when reading or writing data from or to NVM and the   */
/* data requires translation between internal format and external format. */
nvi_item_type  nvi_item;

/* Data buffer for individual writes/reads issued internally. */  
nv_item_type  local_item;  

/* Command buffer for individual writes/reads issued internally. */
nv_cmd_type   local_cmd; 

#ifdef NV_FEATURE_PRL_ITEMS
/* Globally used buffer to contain the roaming list read-from  */ 
/* or written-to NV memory.                                    */
nv_roaming_list_type   nv_pr_list; 
#endif

#ifdef NV_FEATURE_RENTAL_ITEMS
/* Exact copy of the rental timer item (internal format) saved as */
/* state data and accessed regionally within the NV task.         */
nvi_rental_timer_type  nv_rental_item_image;
/* Exact copy of the rental count item (internal format) saved as */
/* state data and accessed regionally within the NV task.         */
nvi_rental_cnt_type    nv_rental_cnt_image;
#endif

/* Dynamic pool available in EFS */
word                   nvi_memavail_data=0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                      REX Timers and Queues                              */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

LOCAL q_type           nv_cmd_q;         /* NV task command queue. */
LOCAL q_type           nv_cmd_remote_q;         /* NV task command queue. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Local Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Flag to determine if EEPROM initialization has occurred    */
static boolean                 nvi_initialized = FALSE;

/* Flag to determine if kxmutex initialization has occured */
static boolean                 nv_cs_init_flag = FALSE;

/* The total factory data block cache for peek/poke access */
byte fact_data[NVIM_FACTORY_DATA_SIZE];

static rex_tcb_type *nv_tcb_ptr_efserrno;

typedef enum
{
NV_VALIDATION_DEFAULT,
NV_VALIDATION_ALLOWED,
NV_VAIDATION_WRITE_ONCE,
NV_VALIDATION_FAIL
}nv_validate_type;

static boolean          nverr_init( void );
static void             nverr_update_log( void );
static boolean          nv_validate_caller(uint32 caller);
static nv_validate_type nv_validate_cmd(nv_cmd_ext_type  *cmd_ext_ptr,
                                        uint32 cmd_req_from);
static nv_stat_enum_type nv_verify_access_ruim(nv_cmd_type* cmd_ptr);
static void nv_process_rtre_cmd(nv_cmd_type* cmd_ptr);
static nv_stat_enum_type nv_process_write_once_cmd (nv_cmd_type* cmd_ptr);

static uint32 nvim_get_perm(uint32 cmd_req_from, nv_cmd_type *cmd_ptr);

static boolean nv_ruim_init_cb = FALSE;

extern void nvdiag_init (void);

/* Structure used to essentially double buffer the error log  */
struct 
{
  nv_err_log_type  external[NV_MAX_ERR_LOG];
  nv_err_log_type  internal[NV_MAX_ERR_LOG];
  boolean          update_required[NV_MAX_ERR_LOG];
  boolean          update_in_progress[NV_MAX_ERR_LOG];
  boolean          processing_required;
} nverr_log;


#include "fs_diag.h"

static const char *nv_access_dirs[] = {
  "nvm"
};

const char nv_ssm[]="nvssm";


/* Init status returned from SSM API */
static ssm_err_t nv_ssm_init_status=0;

/* client ID returned by SSM during init */
static uint32 nv_ssm_handle=SSM_CLIENT_INVALID_ID;

/* Callback that will called to report the status of NV */
static void nv_ssm_cb
(
  /** Error type : Depicts status of ssm _client 
    * after initialization is complete 
    */
  ssm_err_t         err
);

static void nvimnv_task_start ( void );


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Debug Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*function protect the critical section of nv code*/
static void nvim_mutex_init(void);
static int nvim_mutex_initialised = 0;
static boolean nv_check_remote_access (char *path, uint32 op_type);

/* To register NV RUIM callback function */
static nv_ruim_register_cb_type nv_ruim_callback;

/*This command is solely for consumption within nvim.c */
#define NV_WRITE_ONCE_F (nv_func_enum_type) (NV_FUNC_ENUM_PAD + (1))

/* Data buffer for individual writes/reads used in NV_WRITE_ONCE_F command */
nv_item_type local_ruim_item;

/*For ARM9*/
#ifndef FEATURE_QDSP6
static rex_crit_sect_type nvim_crit_sect={0};

#define NVIM_LOCK()  \
  if(0 == nvim_mutex_initialised)  \
     nvim_mutex_init();  \
   rex_enter_crit_sect(&nvim_crit_sect); \
   
#define NVIM_FREE()   rex_leave_crit_sect(&nvim_crit_sect)

#else /*else for QDSP6*/
/* Pointer to the mutex object for protecting critical-section of nvim 
   code*/
static qmutex_t nvim_qemu_mutex=0;
/*MACRO for locking/unlocking the qmutex*/
#define NVIM_LOCK()  \
  if (0==nvim_mutex_initialised)       \
    nvim_mutex_init();         \
  qmutex_lock(nvim_qemu_mutex); \
  
#define NVIM_FREE()  \
  qmutex_unlock(nvim_qemu_mutex); \
  
#endif /*FEATURE_QDSP6*/

/*=============================================================================

FUNCTION NVIM_MUTEX_INIT

DESCRIPTION
  This function initializes the qmutex that will be used for protecting
  the crit-section portion of nvim code.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void nvim_mutex_init(void)
{
#ifdef FEATURE_QDSP6
  qmutex_attr_t mutex_attr;
  int intErr=0;

  mutex_attr.type = QMUTEX_LOCAL;
  intErr = qmutex_create ( &nvim_qemu_mutex, &mutex_attr );
  if(EOK != intErr)
  {
    ERR_FATAL("Failed to create nvim_qemu_mutex",0,0,0);
  }
#else /* FEATURE_QDSP6 */
  rex_init_crit_sect(&nvim_crit_sect);
#endif /* FEATURE_QDSP6 */
  nvim_mutex_initialised = 1;
}


/*==================================================================
Function: NV_CHECK_REMOTE_ACCESS

Description: This functions returns access permissions for 
             directories specified by input parameter "path".
==================================================================*/
static boolean
nv_check_remote_access (char *path, uint32 op_type)
{
  (void) path;
  (void) op_type;
  return FALSE;
}

/*====================================================================
Function: NV_REGISTER_REMOTE_ACCESS_CHECK

Description: Registers callback function nv_check_remote_access()
             with directory listed in nv_access_dirs. On any access to
             nv_access_dirs directories, nv_check_remote_access will 
             be called to determine access permissions.
=====================================================================*/
void
nv_register_remote_access_check (void)
{
  diag_fs_register_access_check (nv_access_dirs, 1, nv_check_remote_access);
}


/*===========================================================================

FUNCTION NV_READ_ERR_LOG

DESCRIPTION
  This function reads an error log from NVM.

DEPENDENCIES
  The function MUST NOT USE ANY TASK SERVICES, so that it can be
  called when tasks are not initialized.  The function MUST NOT
  USE ERROR SERVICES.  It returns status to the caller if there
  is a failure.

RETURN VALUE
  NV_DONE_S         if it worked
  NV_NOT_ACTIVE_S   if item was not active
  NV_FAIL_S         if item could not be read

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nv_read_err_log (
  nv_err_log_type  *nv_err_log_ptr    /* Pointer where to return read data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!nvi_initialized) {
    return NV_DONE_S; /* can not read errors before nv built */
  }

  /* check and reject illegal request. */
  if (nv_err_log_ptr->address >= NV_MAX_ERR_LOG) {
    return NV_FAIL_S;
  }

  /* copy the shadow copy of the error log */
  NVIM_LOCK();
  nv_err_log_ptr->err_count = nverr_log.external[nv_err_log_ptr->address].err_count;
  strlcpy((char *)nv_err_log_ptr->file_name,
              (char *)nverr_log.external[nv_err_log_ptr->address].file_name,
              sizeof(nv_err_log_ptr->file_name));
  nv_err_log_ptr->line_num = nverr_log.external[nv_err_log_ptr->address].line_num;
  nv_err_log_ptr->fatal = nverr_log.external[nv_err_log_ptr->address].fatal;
  NVIM_FREE();

  return NV_DONE_S;
} /* nv_read_err_log */


/*===========================================================================

FUNCTION NV_WRITE_ERR_LOG

DESCRIPTION
  This function writes the boot error log to NVM.

DEPENDENCIES
  The function MUST NOT USE ANY TASK SERVICES, so that it can be
  called when tasks are not initialized.  The function MUST NOT
  USE ERROR SERVICES.  It returns status to the caller if there
  is a failure.

RETURN VALUE
  NV_DONE_S - if it worked
  NV_FAIL_S - if item could not be written

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nv_write_err_log(
  nv_err_log_type  *nv_err_log_ptr    /* Pointer where to get write data */
)
{
   static boolean nv_err_fataling = FALSE;
   nv_stat_enum_type status;       /* NV read status */
   
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   if (nv_err_fataling) return NV_FAIL_S;

   if (!nvi_initialized) {
      return NV_DONE_S; /* can not write errors before nv built */
   }

   /* Check if file system is error fataling  */
   if ((rex_self() == &fs_tcb) && (nv_err_log_ptr->fatal)) {
      nv_err_fataling = TRUE; /* NV is not usable once FS err fatals */
      return NV_DONE_S; /* can not write this error */
   }
   
   /* check and reject illegal request. */

   if (nv_err_log_ptr->address >= NV_MAX_ERR_LOG) {
      return NV_FAIL_S;
   }
   /* If the task is running we queue the write to the nv part as to not
    * interrupt a write in progress. This is a somewhat device dependend
    * concept in that we assume a write can not be safely interrupted during
    * a write, which is not true of all nv devices. 
    * Note that error_fatals always take the direct route.
    */
   if (nvi_task_running && !(nv_err_log_ptr->fatal)) {
      /* put error into shadow copy of nv error record */
      NVIM_LOCK();
      nverr_log.processing_required = TRUE;
      (void) memscpy((void *)&(nverr_log.external[nv_err_log_ptr->address]),
                    sizeof(nv_err_log_type),
                    (void *)nv_err_log_ptr,
                    sizeof(nv_err_log_type));          /* data buffer size */
      nverr_log.update_required[nv_err_log_ptr->address] = TRUE;
      NVIM_FREE();

      /* Signal the nv task that there is a buffer to process */
      (void) rex_set_sigs(&nv_tcb, NV_ERR_LOG_SIG);
      return NV_DONE_S;
   }
   else {
      if ((nv_err_log_ptr->fatal)) {
         nv_err_fataling = TRUE;
         /* allow lower levels to run with ints disabled */
         nvi_task_running = FALSE; 
      }

      status = nvio_write_item
               ( NV_ERR_LOG_I, 
                 nv_err_log_ptr->address, /* Item index within its array */
                 (void *)&nv_err_log_ptr->err_count,         /* data buffer */ 
                 nvim_op_get_size(NV_ERR_LOG_I),
                 0 /* context=0 implies no context is defined */
               );

      NVIM_LOCK();
      if (!(nv_err_log_ptr->fatal)) {
         (void) memscpy((void *)&(nverr_log.external[nv_err_log_ptr->address]),
                        sizeof(nv_err_log_type),
                       (void *)nv_err_log_ptr,
                       sizeof(nv_err_log_type));          /* data buffer size */
      }
      NVIM_FREE();

      return status;
   }
} /* nv_write_err_log */

/*===========================================================================

FUNCTION NVIM_OP_GET_PRESENCE

DESCRIPTION
  This function determines whether an entry is present.

DEPENDENCIES
  None.

RETURN VALUE
  0 : NVIM_NOT_PRESENT : Not present
  1 : NVIM_PRESENT : Present
  2 : NVIM_WRITE_ONCE : Present and write once only

SIDE EFFECTS

===========================================================================*/
boolean 
nvim_op_get_presence (
  nv_items_enum_type item   /* Item ID */
)
{
  if (item < NVIM_ITEM_INFO_TABLE_SIZE)
  {
    return nvim_item_info_table[item].is_present;
  }
  else
  {
    ERR("Item code %d out of range", item, 0, 0);
    return 0;
  }

} /* nvim_op_get_presence */


/*===========================================================================

FUNCTION NVIM_OP_GET_ARRAY_SIZE

DESCRIPTION
  This function returns the array size of an item, given its ID (item code).

DEPENDENCIES
  None.

RETURN VALUE
  Array size.

SIDE EFFECTS
  Only a meaningful return value if the entry is valid which is determined
  by calling nv_op_is_valid_entry

===========================================================================*/
uint16 
nvim_op_get_array_size (
  nv_items_enum_type   item   /* Item ID */
)
{
  if (item < NVIM_ITEM_INFO_TABLE_SIZE) {
    return nvim_item_info_table[item].array_size;
  }
  else {
    ERR("Item %d out of range", item, 0, 0);
    return 0;
  }
} /* nvim_op_get_array_size */



/*===========================================================================

FUNCTION NVIM_OP_GET_SIZE

DESCRIPTION
  This function returns the size of an item, given its ID (item code).

DEPENDENCIES
  None.

RETURN VALUE
  Item size.

SIDE EFFECTS
  Only a meaningful return value if the entry is valid which is determined
  by calling nv_op_is_valid_entry

===========================================================================*/
uint32 
nvim_op_get_size (
  nv_items_enum_type   item   /* Item ID */
)
{
  if (item < NVIM_ITEM_INFO_TABLE_SIZE)
  {
    return (nvim_item_info_table[item].item_size);
  }
  else
  {
    ERR("Item %d out of range", item, 0, 0);
    return 0;
  }
} /* nvim_op_get_size */



/*===========================================================================

FUNCTION NV_DETERMINE_ESN_VALIDITY

DESCRIPTION
  This function is determines if the current ESN has been programmed with the
  status of this check returned.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the current ESN is non-zero with proper checksums, else FALSE

SIDE EFFECTS

===========================================================================*/

boolean 
nv_determine_esn_validity( void)
{
  /* This is gross, but there is not enough stack space
   *  for these next two items so we declare them as the appear in the enum
   */
  dword             nv_item1_esn;        /* tmp buffer for esn */
  dword             nv_item2_esn_chksum; /* tmp buffer for esn checksum */
  nv_stat_enum_type status;              /* to get status of NV operations */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start by reading the ESN and returning FALSE if it is now -1        */

  status = nvio_read_item(NV_ESN_I,/* file handle */
                          0,       /* file position */
                          &nv_item1_esn,          /* data ptr */
                          sizeof(nv_item1_esn), /* data count */
                          0); /* context=0 implies no context is defined */

  if( (status != NV_DONE_S) ||
      (nv_item1_esn == 0xFFFFFFFF) )  {
    /* ------------------------- */
    /* The ESN has not been set, */
    /* return FALSE              */
    /* ------------------------- */
    ERR( "ESN has not been set", 0, 0, 0);
    return FALSE;
  }

  if( nv_item1_esn == 0 )  {
    NV_MSG_HIGH("ESN is zero for factory test");
  }

  /* --------------------------------------------------------------------- */
  /* To get here the ESN has been set. Determine if the checksums are      */
  /* valid. Do this be comparing calculated checksums against stored sums  */
  /* --------------------------------------------------------------------- */
 
  status = nvio_read_item(NV_ESN_CHKSUM_I,/* file handle */
                          0,       /* file position */
                          &nv_item2_esn_chksum,          /* data ptr */
                          sizeof(nv_item2_esn_chksum), /* data count */
                          0); /* context=0 implies no context is defined */

  /* --------------------------------------------------------------------- */
  /* To get here the ESN is valid and this is not a pre-Version 5.      */
  /* Check for proper checksums and return TRUE if so, else return FALSE   */
  /* --------------------------------------------------------------------- */

  if( crc_30_calc( (byte *)&nv_item1_esn, sizeof(nv_item1_esn) * 8) !=
      nv_item2_esn_chksum)  {
    /* ------------------------- */
    /* The ESN checksum is not   */
    /* valid, return FALSE       */
    /* ------------------------- */
    ERR( "Stored ESN CHKSUM bad: %u", nv_item2_esn_chksum, 0, 0);
    return FALSE;
  } else {
    return TRUE;
  }
}


#ifdef FEATURE_NV_CNV
/*======================================================================
FUNCTION NVI_CNV_CHECK

DESCRIPTION
  This function checks for the presence of a CNV file in EFS.
  If it is found, the items are inserted into the current nv.

DEPENDENCIES
  EFS must be initialized.  The item manager must also have completed
  nv_init.  This routine expects to be called at the end of NV_INIT.

RETURN VALUE
  None

SIDE EFFECTS
  NV items could be updated if a valid CNV fileis present.

======================================================================*/
void nvim_cnv_check(void)
{
  int       file_handle = NULL;
  struct    fs_stat sbuf;
  fs_off_t  offset;
  fs_size_t size;
  word      crc16;
  static    byte buffer[32];
  uint32    count;
  uint32    size_left;
  uint8     pad_size;


  cnv_item_hdr_type item_head;
  static nv_item_type      item;
  nv_cmd_type       cmd;
  nv_cmd_ext_type   cmd_ext;

  /* Open the file */
  file_handle = efs_open(cnv_file, O_RDWR);
  if (file_handle < 0)
  {
    NV_MSG_HIGH ("Cannot open CNV file");
    return;
  }

  /* Stat to get the file size */
  if (efs_fstat(file_handle, &sbuf) < 0)
  {
    NV_MSG_HIGH ("Error on fstat of CNV file");
    return;
  }

  /* Cycle thru the contents to check CRC */
  crc16 = (word) CRC_16_STEP_SEED;

  size_left = sbuf.st_size;
  while (size_left > 0) {
    count = size_left;
    if (count > sizeof (buffer))
      count = sizeof (buffer);

    size = efs_read(file_handle, buffer, count);
    if (size != count)
    {
      NV_MSG_MED ("Unable to read CNV data");
      return;
    }

    crc16 = crc_16_step (crc16, buffer, count);
    size_left -= count;
  }

  if (crc16 != CRC_16_OK) {
    NV_MSG_HIGH ("Invalid CRC for CNV data");
    return;
  }

  NV_MSG_MED ("Valid CNV file detected");

  /* Rewind the file back to the first item and ignore the header
     for now */
  offset = efs_lseek (file_handle, sizeof(cnv_header_type), SEEK_SET);

  if (offset < 0)
  {
    NV_MSG_HIGH ("Cannot seek to the beginning of the file");
    return;
  }

  size_left = sbuf.st_size - (sizeof(cnv_header_type) + sizeof(crc16)
                                          + sizeof(cnv_item_hdr_type));

  while (size_left > 0)
  {
    /* Read in an item. */
    size = efs_read (file_handle, (void *) &item_head, sizeof (item_head));
    if (size != sizeof (item_head))
    {
      NV_MSG_MED ("Unable to read handover item header");
      return;
    }

    size_left -= sizeof (item_head);

    NV_MSG_MED_2 ("CNV item: %d, size %d", item_head.nv_item_number,
               item_head.nv_item_size);

    /* Read in the raw data. */
    size = efs_read (file_handle, (void *) &item, item_head.nv_item_size);
    if (size != item_head.nv_item_size)
    {
      NV_MSG_MED ("Unable to read CNV item");
      return;
    }

    /* Write the item to NV. */
    cmd.item     = (nv_items_enum_type) item_head.nv_item_number;
    cmd.cmd      = NV_WRITE_F;
    cmd.data_ptr = &item;

    cmd_ext.nvcmd = &cmd;
    cmd_ext.context  = 0;

    if (nvio_write (&cmd_ext, NV_CMD_REQ_FROM_MODEM,NV_SSM_DEFAULT_PERM_S) != NV_DONE_S)
    {
      NV_MSG_HIGH_2 ("Unable to write handover item: %d, size %d",
                 item_head.nv_item_number, item_head.nv_item_size);
    }

    pad_size = 0;
    if ((item_head.nv_item_size % 4) != 0)
    {
      pad_size = 4 - (item_head.nv_item_size % 4);

      offset = efs_lseek(file_handle, pad_size, SEEK_CUR);
      if (offset < 0)
      {
        NV_MSG_HIGH ("Cannot seek forward to the next item");
        return;
      }
    }
    size_left -= (item_head.nv_item_size + pad_size);
  }

  /* Close the file */
  efs_close (file_handle);

  /* Remove the file since we are done updating the NV items */
  if (efs_unlink (cnv_file) < 0)
  {
    NV_MSG_HIGH ("Unable to remove CNV file");
  }
} /* nvi_cnv_check */

#endif /*FEATURE_NV_CNV*/


/*===========================================================================

FUNCTION NVIM_BUILD_SEC

DESCRIPTION
  This function builds all NVM variables.

DEPENDENCIES

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if a variable could not be initialized.

SIDE EFFECTS

===========================================================================*/

static nv_stat_enum_type 
nvim_build_sec(void)
{
   nv_stat_enum_type  status;    /* Status to return to calling procedure */
   word               cnt;       /* Counter for attempts to write variable */
   nv_cmd_ext_type    local_cmd_ext;
   
   /* Initialize the lock code to the default value (0000)   */
   for( cnt=0; cnt<NV_LOCK_CODE_SIZE; cnt++)  {
     local_item.lock_code.digits[ cnt] = '0';
   }
   
   local_cmd.item       = NV_LOCK_CODE_I;
   local_cmd.tcb_ptr    = NULL;
   local_cmd.sigs       = 0;
   local_cmd.done_q_ptr = NULL;
   local_cmd.cmd        = NV_WRITE_F;
   local_cmd.data_ptr   = &local_item;
   
   local_cmd_ext.nvcmd = &local_cmd;
   local_cmd_ext.context = 0; /* No context is defined */

   status = nvio_write(&local_cmd_ext, NV_CMD_REQ_FROM_MODEM,NV_SSM_DEFAULT_PERM_S);
   if(status != NV_DONE_S)
   {
      return status;
   }
   
   /* Initialize the sec code to the default value (0000)   */
   for( cnt=0; cnt<NV_SEC_CODE_SIZE; cnt++)  {
     local_item.sec_code.digits[ cnt] = '0';
   }
   local_cmd.item       = NV_SEC_CODE_I;
   local_cmd.tcb_ptr    = NULL;
   local_cmd.sigs       = 0;
   local_cmd.done_q_ptr = NULL;
   local_cmd.cmd        = NV_WRITE_F;
   local_cmd.data_ptr   = &local_item;
   
   local_cmd_ext.nvcmd = &local_cmd;
   local_cmd_ext.context = 0; /* No context is defined */

   status = nvio_write(&local_cmd_ext, NV_CMD_REQ_FROM_MODEM,NV_SSM_DEFAULT_PERM_S);

   return status;
}


/*===========================================================================

FUNCTION NVIM_CLEAR_CRCS

DESCRIPTION
  This function is called to clear CRCs in NV when the NV is built. The function 
  clears the NAM checksums, the MIN checksums within each NAM, and, if 
  the ESN and IMEI have not already been written (assigned), the ESN, the IMEI,
  the ESN checksum, the IMEI checksum.  This action lets higher level tasks 
  know that these parameters are not valid and that service programming is 
  required.

DEPENDENCIES
  NVM must have been built.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  NV parameters identified above are zeroed.

===========================================================================*/

LOCAL nv_stat_enum_type  
nvim_clear_crcs (void)
{
  byte                nam_idx;       /* NAM counter */
  byte                min_idx;       /* MIN counter */
  nv_cmd_ext_type    local_cmd_ext;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Setup command buffer parameters. */

  local_cmd.cmd = NV_WRITE_F;
  local_cmd.tcb_ptr = NULL;
  local_cmd.sigs = 0;
  local_cmd.done_q_ptr = NULL;
  local_cmd.data_ptr = &local_item;

  /* Clear all NAMs checksums and the MIN checksums for each NAM. */
  
  for (nam_idx = 0; nam_idx < NV_MAX_NAMS; nam_idx++) { 
    local_item.nam_chksum.nam = nam_idx;
    local_item.nam_chksum.chksum = 0;
    local_cmd.item = NV_NAM_CHKSUM_I;

    local_cmd_ext.nvcmd = &local_cmd;
    local_cmd_ext.context = 0; /* No context is defined */

    local_cmd.status = nvio_write(&local_cmd_ext, NV_CMD_REQ_FROM_MODEM,NV_SSM_DEFAULT_PERM_S);

    NV_MSG_LOW_2("NAM %d NAM checksum zero status %d", nam_idx, local_cmd.status);
    if (local_cmd.status != NV_DONE_S) {
      return (local_cmd.status);
    }
 
    local_item.min_chksum.nam = nam_idx;
    for (min_idx = 0; min_idx < NV_MAX_MINS; min_idx++) {
      local_item.min_chksum.chksum[min_idx] = 0;
    }
    local_cmd.item = NV_MIN_CHKSUM_I;

    local_cmd_ext.nvcmd = &local_cmd;
    local_cmd_ext.context = 0; /* No context is defined */

    local_cmd.status = nvio_write(&local_cmd_ext, NV_CMD_REQ_FROM_MODEM,NV_SSM_DEFAULT_PERM_S);

    NV_MSG_LOW_2("NAM %d MIN checksum zero status %d", nam_idx, local_cmd.status);
    if (local_cmd.status != NV_DONE_S) {
      return (local_cmd.status);
    }
  } /* for */

  /* --------------------------------------------------------------------- */
  /* Next we determine if the ESN is now proper and if not then clear the  */
  /* ESN,the ESN checksum, and the ESN valid flag.                         */
  /* --------------------------------------------------------------------- */
  if( nv_determine_esn_validity() == FALSE) {
    /* ------------------------------------------------------------------- */
    /* The ESN has not yet been assigned/written. Zero associated items    */
    /* Note that to get here status == NV_DONE_S                           */
    /* ------------------------------------------------------------------- */
    local_item.esn.esn = 0;
    local_cmd.item = NV_ESN_I;

    local_cmd_ext.nvcmd = &local_cmd;
    local_cmd_ext.context = 0; /* No context is defined */

    local_cmd.status = nvio_write(&local_cmd_ext, NV_CMD_REQ_FROM_MODEM,NV_SSM_DEFAULT_PERM_S);
    
    NV_MSG_LOW_1("ESN zero status %d", local_cmd.status);
    if (local_cmd.status != NV_DONE_S) {
      return local_cmd.status;
    }
  }
  return local_cmd.status;
} /* nvim_clear_crcs */


/*===========================================================================

FUNCTION NVIM_INIT_RENTAL_TIMER

DESCRIPTION
  This function reads the rental timer item structure out of NVRAM and
  initializes the "nv_rental_item_image" state data item.  It also
  validates the data structure and recovers from a previous bad powerdown.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type  
nvim_init_rental_timer (void) 
{

#ifdef NV_FEATURE_RENTAL_ITEMS
  nv_stat_enum_type   status;
  status = nvio_read_item(NV_RENTAL_TIMER_I,   /* file handle */
                          0,             /* file position */
                          &nv_rental_item_image,     /* data ptr */
                          sizeof(nvi_rental_timer_type),/* data count */
                          0); /* context=0 implies no context is defined */
  if (status != NV_DONE_S) {
    ERR("Couldn't read rental timer",0,0,0);
    return status;
  }
#endif
  return NV_DONE_S;
}


/*===========================================================================

FUNCTION NVIM_INIT_RENTAL_CNT

DESCRIPTION
  This function reads the rental count item from NV and initializes 
  the "nv_rental_cnt_image" state data item.  
  
DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type  
nvim_init_rental_cnt (void) {
#ifdef NV_FEATURE_RENTAL_ITEMS
  nv_stat_enum_type   status;
  status = nvio_read_item(NV_RENTAL_CNT_I,   /* file handle */
                          0,             /* file position */
                          &nv_rental_cnt_image,     /* data ptr */
                          sizeof(nvi_rental_cnt_type)/* data count */
                          0); /* context=0  implies no context is defined */

  if (status != NV_DONE_S) {
    ERR("Couldn't read rental count",0,0,0);
  }
  return status;
#else
  return NV_DONE_S;
#endif
}


/*===========================================================================

FUNCTION NVIM_INIT_PRL_DATA

DESCRIPTION
  This function initializes the "prl_valid_data" and "prl_version_data"
  state data items.  These are maintained as NV state data so the functions
  "nv_prl_is_valid()" and "nvim_prl_version()" can return an immediate 
  response.  

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if failed

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void
nvim_init_prl_data (void) 
{
  byte                         nam;
  nv_roaming_list_header_type  prl_header;
  nv_stat_enum_type            status;

  prl_header.nam =  FALSE;
  prl_header.prl_version = 0;
  prl_header.valid = FALSE;

  for (nam=0; nam<NV_MAX_NAMS; nam++) {
    /* For each nam, read the prl header */
    status = nvio_read_prl_item(nam,
                                (void *)&prl_header,               /* data ptr */
                                NV_ROAMING_LIST_HEADER_SIZE);/* data count */
    /* Set state data items -- note that the item's active flag */
    /* occupies the "nam" field in the external type.           */
    if (status == NV_DONE_S) {
      nv_prl_version_data[nam] = prl_header.prl_version;
      nv_prl_valid_data[nam] = prl_header.valid;
    }
    else {
      nv_prl_version_data[nam] = NV_PRL_VERSION_INVALID;
      nv_prl_valid_data[nam] = FALSE;
    }
  }
  return;
}


/*===========================================================================

FUNCTION NVIM_REMOVE_ITEM

DESCRIPTION
  This procedure processes NVM remove requests for a particular item.

DEPENDENCIES
  None.

RETURN VALUE
  Status of read operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nvim_remove_item (
  nv_cmd_ext_type  *cmd_ext_ptr           /* Pointer to Command block */
)
{
  //Filename size is incremented from 20 to 28 to account for context
  char file_name[28];
  nv_stat_enum_type status;

  if ((cmd_ext_ptr->nvcmd->item != NV_SMS_I) && (cmd_ext_ptr->nvcmd->item != NV_SMS_GW_I)) 
    return NV_BADPARM_S;

  status = NV_DONE_S;

  if (cmd_ext_ptr->nvcmd->item >= NV_MAX_I) {
   status = NV_BADPARM_S;
  }
  else {
    switch(cmd_ext_ptr->nvcmd->item) {
      case NV_SMS_I:
      case NV_SMS_DM_I:
        if(cmd_ext_ptr->nvcmd->data_ptr->sms.address >= NVI_MAX_SMS_ADDR)
          status = NV_BADPARM_S;
        else{
          //If context is not defined, use the old path
          if (cmd_ext_ptr->context == 0)
          (void) snprintf(file_name, sizeof(file_name), "/nvm/sms_%05d",
                                            cmd_ext_ptr->nvcmd->data_ptr->sms.address);
          else //Else, context is defined, use context for path
            (void) snprintf(file_name, sizeof(file_name), "/nvm/context%d/sms_%05d",
                                            cmd_ext_ptr->context,cmd_ext_ptr->nvcmd->data_ptr->sms.address);
          efs_unlink(file_name);
        }
        break;
      case NV_SMS_GW_I:
        if(cmd_ext_ptr->nvcmd->data_ptr->sms_gw.address >= NVI_MAX_SMS_ADDR) 
          status = NV_BADPARM_S;
        else {
          //If context is not defined, use the old path
          if (cmd_ext_ptr->context == 0)
          (void) snprintf(file_name, sizeof(file_name), "/nvm/sms_gw_%05d",
                                          cmd_ext_ptr->nvcmd->data_ptr->sms_gw.address);
          else //Else, context is defined, use context for path
            (void) snprintf(file_name, sizeof(file_name), "/nvm/context%d/sms_gw_%05d",
                                          cmd_ext_ptr->context,cmd_ext_ptr->nvcmd->data_ptr->sms_gw.address);
          efs_unlink(file_name);
        }
        break;
      default:
        status = NV_BADPARM_S;
    }/*switch*/
  }/* else */
  return status;
}/*nvim_remove_item*/


/*===========================================================================

FUNCTION NVIM_REMOVE_ALL

DESCRIPTION
  This removes all files under the NVM directory

DEPENDENCIES
  None.

RETURN VALUE
  Status of read operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nvim_remove_all ( void )
{
  struct fs_dirent         *dirent;
  EFSDIR *dir;

  dir = efs_opendir("nvm/sms");
  while((dirent = efs_readdir(dir))!= NULL){
    efs_unlink( dirent->d_name );
  }
  efs_closedir (dir);
  efs_rmdir("/nvm/sms");

  dir = efs_opendir("nvm/prl");
  while((dirent = efs_readdir(dir))!= NULL){
    efs_unlink( dirent->d_name );
  }
  efs_closedir (dir);
  efs_rmdir("/nvm/prl");

  dir = efs_opendir("nvm/nvm");
  while((dirent = efs_readdir(dir))!= NULL){
    efs_unlink( dirent->d_name );
  }
  efs_closedir (dir);
  efs_rmdir("/nvm/nvm");

  dir = efs_opendir("nvm");
  while((dirent = efs_readdir(dir))!= NULL){
    efs_unlink( dirent->d_name );
  }
  efs_closedir (dir);

  return NV_DONE_S;
}/*nvim_remove_all*/


/*===========================================================================

FUNCTION NVIM_MIGRATION_NEEDED

DESCRIPTION
  This function will decide if there is a need to unlink the nvm directory 
  during a transition of older NV services to NV-2

DEPENDENCIES
  None.

RETURN VALUE
  0 if migration is not required
  1 if migratuion is required
  
SIDE EFFECTS
  None.

===========================================================================*/

LOCAL int 
nvim_migration_needed(void) 
{
  struct fs_stat temp_buf;
  /* Make sure that the change to efs_get is made so that it returns 
  correct value */
  if(efs_stat("nvm/num/0",&temp_buf) == -1)
    return 1;
  else 
    return 0;
}


/*===========================================================================

FUNCTION NV_INIT

DESCRIPTION
  This function checks the major version number of the the NV task
  as stored at edit time, against the major version number of the
  NV EEPROM as read from NVM.  If the numbers are different then the
  function completely rebuilds the NV EEPROM.  If the major version
  number has not changed then the function checks if the number of
  items defined in NVI.H has increased from the internal NVM stored
  items count.  If so then only those new items are built.  Once the
  NV EEPROM has been initialized the function updates the stored
  major version number to the one contained in NVI.H.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  Rebuilding the NVM takes a significant amount of time, measured in
  seconds on serial eeprom devices, if NVM is rebuilt. Startup time 
  will vary with the type of EEPROM.

===========================================================================*/

LOCAL nv_stat_enum_type  
nv_init (void)
{
  nv_stat_enum_type  status;          /* Status to return to caller */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!nv_cs_init_flag)
    nv_access_op_cs_init();

  NV_GLOBAL_LOCK_INIT();
  if (nvim_migration_needed()) {
    status = nvim_remove_all();
    if ((status = nvim_build_sec()) == NV_DONE_S) {
      status = nvim_clear_crcs();
    }
  } 
  else {
    status = NV_DONE_S;
  }

#ifdef FEATURE_NV_CNV
  /* If the file was dropped in as part of a CEFS image, then restore
     the NV items */
  nvim_cnv_check();

  /* Remove the file since we are done updating the NV items and just
     in case the file is left hanging around because of error conditions */
  if (efs_unlink (cnv_file) < 0) {
    NV_MSG_HIGH ("Unable to remove CNV file");
  }
#endif

  (void)nverr_init();      /* cannot init err logging until nv is built */
  (void)nvim_init_rental_timer();  /* validate the rental timer item */
  (void)nvim_init_rental_cnt();    /* validate the rental count item */

#if !defined( FEATURE_NV_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  nvim_init_prl_data();      /* initialize prl state data items */
#endif /* FEATURE_RUIM */

  nv_register_remote_access_check();

  nv_ssm_init_status = ssm_client_init_func( (char *)nv_ssm, NV_SSM_ATTR_CNT, 
                                        ACCESS_DENIED_PERM, SSM_UINT32,
                                        nv_ssm_cb, &nv_ssm_handle, 
                                        NV_DEFAULT_WHITELIST,
                                        strlen(NV_DEFAULT_WHITELIST));
  if((E_SSM_SUCCESS != nv_ssm_init_status) &&
     (E_SSM_CLIENT_INIT_IN_PROCESS != nv_ssm_init_status))
  {
    NV_MSG_ERROR_2("[nv_init]: SSM client init failed with status %d, client id %d", 
                   nv_ssm_init_status, nv_ssm_handle);
  }
#if defined(FEATURE_DIAG_SPC_TIMEOUT) && defined(FEATURE_DIAG_SPC_TTL)
  {
    uint8 diag_spc_count;
    status = nvio_read_item(NV_DIAG_SPC_UNLOCK_TTL_I,/* file handle */
                          0,       /* file position */
                          &diag_spc_count,          /* data ptr */
                          sizeof(diag_spc_count), /* data count */
                          0); /* context=0 implies no context is defined */

    if( status == NV_NOTACTIVE_S){
      diag_spc_count = DIAG_SPC_COUNT_MAX; /* use macro write default */
      status = nvio_write_item
               ( NV_DIAG_SPC_UNLOCK_TTL_I, 
                 0, /* Item index within its array */
                 (void *)&diag_spc_count,         /* data buffer */ 
                 nvim_op_get_size(NV_DIAG_SPC_UNLOCK_TTL_I),
                 0 /* context=0 implies no context is defined */
               );
      if(status == NV_DONE_S) {
      NV_MSG_HIGH ("Default SPC has been provisoned!!");
      } else
      {
      NV_MSG_HIGH ("Default SPC provisoning failed!!!");
      }
    } else 
    if(status == NV_DONE_S)
    {
      if(diag_spc_count > DIAG_SPC_COUNT_MAX) 
      {
         diag_spc_count = DIAG_SPC_COUNT_MAX; /* use macro write default */
         status = nvio_write_item
               ( NV_DIAG_SPC_UNLOCK_TTL_I, 
                 0, /* Item index within its array */
                 (void *)&diag_spc_count,         /* data buffer */ 
                 nvim_op_get_size(NV_DIAG_SPC_UNLOCK_TTL_I),
                 0 /* context=0 implies no context is defined */
               );
         if(status == NV_DONE_S) {
           NV_MSG_HIGH ("Default SPC has been provisoned!!");
         } else
         {
           NV_MSG_HIGH ("Default SPC provisoning failed!!!");
         }
      }
      NV_MSG_HIGH ("SPC has been programmed!!");
    }else
    {
      status = NV_FAIL_S;
    }
  }
#endif
  return status;
} /* nv_init */

/*===========================================================================

FUNCTION NV_SSM_CB

DESCRIPTION
  Functions that gets called after the SSM NV client init is done

DEPENDENCIES
  None.

RETURN VALUE
  Returns err code anything other than E_SSM_SUCCESS is a failure

SIDE EFFECTS
  None.

===========================================================================*/
static void nv_ssm_cb
(
  /** Error type : Depicts status of ssm _client 
    * after initialization is complete 
    */
  ssm_err_t         err
)
{
  /* update the global init status since init is Async */
  nv_ssm_init_status = err;

  if(E_SSM_SUCCESS != err)
  {
    NV_MSG_ERROR_2("[nv_init]: SSM client init failed with status %d, client id %d", 
                   err, nv_ssm_handle);
  } else
  {
    NV_MSG_ERROR_2("[nv_init]: SSM client init success with status %d, client id %d", 
                   err, nv_ssm_handle );
  }
}


#ifdef FEATURE_MDN_BASED_NAI

/*===========================================================================

FUNCTION NV_BCD_TO_CHAR

DESCRIPTION
  Functions to convert MDN (BCD form) into ASCII characters.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the characters for the numeric values between 1 - 9.
  Returns 0 for 10, * for 11 and # for 12.

SIDE EFFECTS
  None.

===========================================================================*/
char nv_bcd_to_char
(
  word num_to_convert
)
{
  switch(num_to_convert)
  {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      return '0' + num_to_convert;
    case 10:
      return '0';
    case 11:
      return '*';
    case 12:
      return '#';
    default:
      NV_MSG_ERROR_1 ("Invalid BCD digit (%d)", num_to_convert);
      return ' ';
  }
} /* nv_bcd_to_char */


/*===========================================================================

FUNCTION NV_NAI_FROM_MDN

DESCRIPTION
  Functions form NAI based on given MDN and domain .

DEPENDENCIES
  None.

RETURN VALUE
  TRUE    if it worked
  FALSE   Failure

SIDE EFFECTS
  None.

===========================================================================*/

boolean nv_nai_from_mdn
(
  char *                  nai_to_write,
  word *                  len,   //  len should be set to MAX_NAI_LENGTH
  nv_mob_dir_data_type *  mob_dir,
  char *                  domain  // must be null terminated
)
{
  int  i, cpylen;

  ASSERT (nai_to_write);
  ASSERT (len);
  ASSERT (mob_dir);
  ASSERT (domain);

  if (*len < mob_dir->n_digits + 1)
  {
    NV_MSG_ERROR ("Output nai buffer too small for MDN");
    return FALSE;
  }

  for (i = 0; i < mob_dir->n_digits; i++)
  {
    nai_to_write[i] = nv_bcd_to_char(mob_dir->digitn[i]);
    if (nai_to_write[i] == ' ')
    {
      NV_MSG_ERROR ("Invalid MDN when converting to NAI");
      return FALSE;  // item_to_be_written = FALSE;
    }
  }

  nai_to_write[i] = 0;
 
  cpylen = strlen(domain);
  if (cpylen > *len - i - 1)
  {
    NV_MSG_ERROR ("Output nai buffer too small for domain");
    return FALSE;  // item_to_be_written = FALSE;
  }

  strlcat(&(nai_to_write[i]), domain, *len - i - 1);
  nai_to_write[i+cpylen] = 0;  // NULL terminate
  *len = i + cpylen;
  return TRUE;
} /* nv_nai_from_mdn */

/*===========================================================================

FUNCTION NV_GET_DFLT_MIP_PROFILE

DESCRIPTION
  Sets the current profile with the parameters for the default MIP profile.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void nv_get_dflt_mip_profile
(
  nv_ds_mip_gen_user_prof_type * prof
)
{
  ASSERT (prof);

  prof->index             = MIP_DLFT_PROFILE_NUM;
  prof->nai_length        = 0;
  prof->nai[0]            = 0;
  prof->mn_ha_spi_set     = TRUE;
  prof->mn_ha_spi         = MIP_MD5_SPI;
  prof->mn_aaa_spi_set    = TRUE;
  prof->mn_aaa_spi        = MIP_CHAP_SPI;
  prof->rev_tun_pref      = FALSE;
  prof->home_addr         = DYNAMIC_HOME_ADDR;
  prof->primary_ha_addr   = DYNAMIC_HA_ADDR;
  prof->secondary_ha_addr = UNSET_HA_ADDR;
} /* nv_get_dflt_mip_profile */
#endif /* FEATURE_MDN_BASED_NAI */



/*===========================================================================

FUNCTION NV_OTASP_COMMIT

DESCRIPTION
  This function performs the OTASP "commit" operation, which updates
  a number of OTASP-related nv items at once.  

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  Others       Failure for internal call

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type 
nv_otasp_commit (
  nv_cmd_ext_type  *cmd_ext_ptr,  /* Command block */
  uint32           cmd_req_from   /* cmd requested from local/remote proc */
)
{

#ifdef FEATURE_NV_OTASP
  nv_cmd_ext_type local_cmd_ext;
/* Define constants for the switch statement below */
#define NV_COMMIT_DIR_NUM                      0
#define NV_COMMIT_IMSI_ADDR                    1
#define NV_COMMIT_IMSI_MCC                     2
#define NV_COMMIT_IMSI_11_12                   3
#define NV_COMMIT_MIN1                         4
#define NV_COMMIT_MIN2                         5
#define NV_COMMIT_ACCOLC                       6
#define NV_COMMIT_MOB_HOME                     7
#define NV_COMMIT_MOB_SID                      8
#define NV_COMMIT_MOB_NID                      9
#define NV_COMMIT_SID_NID                     10
#define NV_COMMIT_FIRSTCHP                    11
#define NV_COMMIT_HOME_SID                    12
#define NV_COMMIT_ROAM_LIST                   13
#define NV_COMMIT_SPC                         14
#define NV_COMMIT_IMSI_T_S1                   15
#define NV_COMMIT_IMSI_T_S2                   16
#define NV_COMMIT_IMSI_T_11_12                17
#define NV_COMMIT_IMSI_T_MCC                  18
#define NV_COMMIT_IMSI_T_ADDR_NUM             19
#define NV_COMMIT_NAM_LOCK                    20
#define NV_COMMIT_PRIMARY_MIP_NAI_AFTER_MDN   21
#define NV_COMMIT_PRIMARY_SIP_NAI_AFTER_MDN   22
#define NV_COMMIT_AN_NAI_AFTER_MDN            23

#define NV_COMMIT_DONE            255

  boolean      item_to_be_written; /* Flag controls whether write occurs */
  nv_stat_enum_type  status;
  byte         i, commit_state=0;  /* Index variables */

#ifdef FEATURE_MDN_BASED_NAI

/* use the largest of the source nai fields (MIP, SIP, AN) */
#define MAX_NAI_LENGTH NV_MAX_PPP_USER_ID_LENGTH

  char    nai_to_write[MAX_NAI_LENGTH]; // max for MIP, check SIP
  char    domain[MAX_NAI_LENGTH + 1];
  char *  domain_ptr;
  word    nai_length;
#endif /* FEATURE_MDN_BASED_NAI */

  NV_MSG_HIGH ("NV Starting OTASP commit");

  /* Set up generic command buffer parameters */
  local_cmd.cmd         = NV_WRITE_F;
  local_cmd.tcb_ptr     = NULL;
  local_cmd.sigs        = 0;
  local_cmd.done_q_ptr  = NULL;
  local_cmd.data_ptr    = &local_item;
  local_cmd.status      = NV_DONE_S;

  while (commit_state != NV_COMMIT_DONE) { 

    item_to_be_written = FALSE;
    switch (commit_state) {
  
      case NV_COMMIT_DIR_NUM:
    
        /* Read control flag to determine if item should be written */
        if (cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->commit_mob_dir) {

          /* Write NV_DIR_NUMBER_PCS_I */
          item_to_be_written = TRUE;
          local_item.mob_dir_number.nam = 
              cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
          local_item.mob_dir_number.n_digits = 
              cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->mob_dir.n_digits;
          for (i=0; i < NV_DIR_NUMB_PCS_SIZ; ++i) {
            local_item.mob_dir_number.digitn[i] = 
                cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->mob_dir.digitn[i];
          }      
          local_cmd.item = NV_DIR_NUMBER_PCS_I;
        }

        /* Set next state */
#ifdef FEATURE_MDN_BASED_NAI
        if (item_to_be_written) 
        {
          commit_state = NV_COMMIT_PRIMARY_MIP_NAI_AFTER_MDN;
          break;
        }
#endif /* FEATURE_MDN_BASED_NAI */
        commit_state = NV_COMMIT_IMSI_ADDR;
        break;

      case NV_COMMIT_IMSI_ADDR:

        /* Read control flag to determine if items should be written */
        if (cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->commit_ftc_cdma) {
        
          /* Write NV_IMSI_ADDR_NUM_I */
          item_to_be_written = TRUE;
          local_item.imsi_addr_num.nam = 
              cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
          local_item.imsi_addr_num.num = 
              cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.imsi_addr_num;
          local_cmd.item = NV_IMSI_ADDR_NUM_I;
          /* Set commit_state to write the additional items of this group */
          commit_state = NV_COMMIT_IMSI_MCC;
        }
        else {
          /* Control flag indicates no items from this group should */
          /* be written, so set commit_state to bypass them all     */
          commit_state = NV_COMMIT_FIRSTCHP;
        }
        break;

      case NV_COMMIT_IMSI_MCC:

        /* Write NV_IMSI_MCC_I */
        item_to_be_written = TRUE;
        local_item.imsi_mcc.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.imsi_mcc.imsi_mcc = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.mcc;
        local_cmd.item = NV_IMSI_MCC_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_IMSI_11_12:

        /* Write NV_IMSI_11_12_I */
        item_to_be_written = TRUE;
        local_item.imsi_11_12.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.imsi_11_12.imsi_11_12 = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.imsi_11_12;
        local_cmd.item = NV_IMSI_11_12_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_MIN1:

        /* Write NV_MIN1_I */
        item_to_be_written = TRUE;
        local_item.min1.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.min1.min1[NV_CDMA_MIN_INDEX] = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.imsi_s1;
        /* Set both the CDMA min and the analog min to this same value */
        local_item.min1.min1[NV_ANALOG_MIN_INDEX] = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.imsi_s1;
        local_cmd.item = NV_MIN1_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_MIN2:

        /* Write NV_MIN2_I */
        item_to_be_written = TRUE;
        local_item.min2.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.min2.min2[NV_CDMA_MIN_INDEX] = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.imsi_s2;
        /* Set both the CDMA min and the analog min to this same value */
        local_item.min2.min2[NV_ANALOG_MIN_INDEX] = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.imsi_s2;
        local_cmd.item = NV_MIN2_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_ACCOLC:

        /* Write NV_ACCOLC_I */
        item_to_be_written = TRUE;
        local_item.accolc.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX] = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.accolc;
        local_cmd.item = NV_ACCOLC_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_MOB_HOME:

        /* Write NV_MOB_TERM_HOME_I */
        item_to_be_written = TRUE;
        local_item.mob_term_home.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.mob_term_home.enabled[NV_CDMA_MIN_INDEX] = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.mob_term_home;
        local_cmd.item = NV_MOB_TERM_HOME_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_MOB_SID:

        /* Write NV_MOB_TERM_FOR_SID_I */
        item_to_be_written = TRUE;
        local_item.mob_term_for_sid.nam = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.mob_term_for_sid.enabled[NV_CDMA_MIN_INDEX] = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.mob_term_for_sid;
        local_cmd.item = NV_MOB_TERM_FOR_SID_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_MOB_NID:

        /* Write NV_MOB_TERM_FOR_NID_I */
        item_to_be_written = TRUE;
        local_item.mob_term_for_nid.nam = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.mob_term_for_nid.enabled[NV_CDMA_MIN_INDEX] = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.mob_term_for_nid;
        local_cmd.item = NV_MOB_TERM_FOR_NID_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_SID_NID:

        /* Write NV_SID_NID_I */
        item_to_be_written = TRUE;
        /* All the targets which have SSPR Enhancements use NV item HOME SID-NID */
#if (defined (NV_FEATURE_SSPR_ENHANCEMENTS) || defined (NV_FEATURE_TRIMODE_ITEMS))
        local_item.home_sid_nid.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        for(i=0;i<NV_MAX_HOME_SID_NID;i++)
        {
          local_item.home_sid_nid.pair[i].sid = 
              cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.pair[i].sid;
          local_item.home_sid_nid.pair[i].nid = 
              cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.pair[i].nid;
        } 
        local_cmd.item = NV_HOME_SID_NID_I;
#else
        local_item.sid_nid.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        for(i=0;i<NV_MAX_SID_NID;i++)
        {
          local_item.sid_nid.pair[NV_CDMA_MIN_INDEX][i].sid = 
              cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.pair[i].sid;
          local_item.sid_nid.pair[NV_CDMA_MIN_INDEX][i].nid = 
              cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_cdma.pair[i].nid;
        }
        local_cmd.item = NV_SID_NID_I;
#endif
        /* Set next state */
        commit_state = NV_COMMIT_FIRSTCHP;
        break;

      case NV_COMMIT_FIRSTCHP:

        /* Read control flag to determine if items should be written */
        if (cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->commit_ftc_amps) {

          /* Write NV_ANALOG_FIRSTCHP_I */
          item_to_be_written = TRUE;
          local_item.analog_firstchp.nam = 
              cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
          local_item.analog_firstchp.channel = 
              cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_amps.firstchp;
          local_cmd.item = NV_ANALOG_FIRSTCHP_I;
          /* Set commit_state to write the next item of this group */
          commit_state = NV_COMMIT_HOME_SID;
        }
        else {
          /* Control flag indicates no items from this group should */
          /* be written, so set commit_state to bypass them all     */
          commit_state = NV_COMMIT_SPC;
        }
        break;

      case NV_COMMIT_HOME_SID:

        /* Write NV_ANALOG_HOME_SID_I */
        item_to_be_written = TRUE;
        local_item.analog_home_sid.nam = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.analog_home_sid.sid = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->ftc_amps.home_sid;
        local_cmd.item = NV_ANALOG_HOME_SID_I;
        /* Set next state */
        commit_state = NV_COMMIT_SPC;
        break;

      case NV_COMMIT_SPC:

        /* Read control flag to determine if item should be written */
        if (cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->commit_spc) {

          /* Write NV_SEC_CODE_I */
          item_to_be_written = TRUE;
          local_item.sec_code = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->spc;
          local_cmd.item = NV_SEC_CODE_I;
        }
        /* Set next state */
        commit_state = NV_COMMIT_IMSI_T_S1;
        break;

      case NV_COMMIT_IMSI_T_S1:

        /* Read control flag to determine if items should be written */
        if (cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->commit_imsi_t) {

          /* Write NV_IMSI_T_S1 */
          item_to_be_written = TRUE;
          local_item.imsi_t_s1.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
          local_item.imsi_t_s1.min1[NV_CDMA_MIN_INDEX] = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->imsi_t.s1;
          local_cmd.item = NV_IMSI_T_S1_I;
          /* Set commit_state to write the next item of this group */
          commit_state++;
        }
        else
        {
          /* Control flag indicates no items from this group should */
          /* be written, so set commit_state to bypass them all     */
          commit_state = NV_COMMIT_ROAM_LIST;
        }
        break;

      case NV_COMMIT_IMSI_T_S2:

        /* Write NV_IMSI_T_S2_I */
        item_to_be_written = TRUE;
        local_item.imsi_t_s2.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.imsi_t_s2.min2[NV_CDMA_MIN_INDEX] = 
          cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->imsi_t.s2;
        local_cmd.item = NV_IMSI_T_S2_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_IMSI_T_11_12:

        /* Write NV_IMSI_T_11_12_I */
        item_to_be_written = TRUE;
        local_item.imsi_t_11_12.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.imsi_t_11_12.imsi_11_12 = 
          cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->imsi_t.imsi_t_11_12;
        local_cmd.item = NV_IMSI_T_11_12_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;
      
      case NV_COMMIT_IMSI_T_MCC:

        /* Write NV_IMSI_T_MCC_I */
        item_to_be_written = TRUE;
        local_item.imsi_t_mcc.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.imsi_t_mcc.imsi_mcc = 
          cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->imsi_t.mcc;
        local_cmd.item = NV_IMSI_T_MCC_I;
        /* Set commit_state to write the next item of this group */
        commit_state++;
        break;

      case NV_COMMIT_IMSI_T_ADDR_NUM:

        /* Write NV_IMSI_T_MCC_I */
        item_to_be_written = TRUE;
        local_item.imsi_t_addr_num.nam = 
          cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
        local_item.imsi_t_addr_num.num = 
          cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->imsi_t.addr_num;
        local_cmd.item = NV_IMSI_T_ADDR_NUM_I;
        /* Set next state */
        commit_state = NV_COMMIT_ROAM_LIST;
        break;

      case NV_COMMIT_ROAM_LIST:

        /* Commit to the Roaming List MUST be the last commit to be performed  */
        /* This is, because the local_cmd.data_ptr gets assigned to pointer to */
        /* nv_pr_list during this sequence of operations. If one must add any  */
        /* other NV item in the commit switch after Roaming List at all, then  */
        /* (s)he should make sure to reassign this to local_item               */ 

        /* Read control flag to determine if item should be written */
        if (cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->commit_roam_list) {

          /* Write NV_ROAMING_LIST_I */
          item_to_be_written = TRUE;
          local_cmd.data_ptr = (nv_item_type *)&nv_pr_list;
          #ifdef NV_FEATURE_IS683A_PRL
            local_cmd.item = NV_ROAMING_LIST_683_I;
          #else
            local_cmd.item = NV_ROAMING_LIST_I;
          #endif
        }
        /* Set next state */
        commit_state = NV_COMMIT_NAM_LOCK;
        break;

      case NV_COMMIT_NAM_LOCK:
        /* Read control flag to determine if items should be written */
        if (cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->commit_nam_lock) 
        {
        #ifdef FEATURE_OTASP_OTAPA
          /* Write NV_NAM_LOCK_I */
          item_to_be_written = TRUE;
          local_item.nam_lock.nam = cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam;
          local_item.nam_lock.enabled = 
            cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->nam_lock.enabled;
          local_cmd.item = NV_NAM_LOCK_I;
        #else
          ERR("Attempted to write NV_NAM_LOCK_I when FEATURE_OTASP_OTAPA not on",
              0, 0, 0);
        #endif
        }
        /* Set commit_state to signify we've finished committing */
        commit_state = NV_COMMIT_DONE;
      
        break;

#ifdef FEATURE_MDN_BASED_NAI
      case NV_COMMIT_PRIMARY_MIP_NAI_AFTER_MDN:

        /*-------------------------------------------------------------------
          Note: this defaults to read/write profile at index 0.
                Customer should use whichever index they assign
                to the active MIP profile.
        -------------------------------------------------------------------*/
        local_item.ds_mip_gen_user_prof.index = MIP_DLFT_PROFILE_NUM;
        local_cmd.cmd                         = NV_READ_F;
        local_cmd.item                        = NV_DS_MIP_GEN_USER_PROF_I;

        local_cmd_ext.nvcmd = &local_cmd;
        local_cmd_ext.context = 0; /* No context is defined */

        /*Since we initialized the local_cmd.tcb_ptr to NULL, this is a
        sychronous call to RUIM and we get the status populated in
        local_cmd.status if item is supported in RUIM*/

        if (( status = nv_verify_access_ruim(local_cmd_ext.nvcmd) ) == NV_DONE_S )
        {
            local_cmd.status = local_cmd_ext.nvcmd->status;
        }
        else if ( status == NV_RUIM_NOT_SUPPORTED_S)
        {
        local_cmd.status                      = nvio_read(&local_cmd_ext, cmd_req_from);
        }
        else
        {
            /*if status is NV_BADPARM_S */
            NV_MSG_ERROR_1 ("Error %d reading NV",local_cmd.status);
            local_cmd.status = NV_FAIL_S;
        }

        if (local_cmd.status == NV_NOTACTIVE_S)
        {
          nv_get_dflt_mip_profile( &(local_item.ds_mip_gen_user_prof) );
        }
        else if (local_cmd.status != NV_DONE_S)
        {
          NV_MSG_ERROR_1 ("Error %d reading NV",local_cmd.status);
          commit_state = NV_COMMIT_PRIMARY_SIP_NAI_AFTER_MDN;
          break;
        }

        if(local_item.ds_mip_gen_user_prof.nai_length < NV_MAX_NAI_LENGTH)
          local_item.ds_mip_gen_user_prof.nai[
            local_item.ds_mip_gen_user_prof.nai_length] = 0;
        domain_ptr =
          strchr( (char*) local_item.ds_mip_gen_user_prof.nai, '@' );
        if (domain_ptr == NULL)
        {
          NV_MSG_HIGH("Using default domain");
          domain_ptr = PS_NAI_DFLT_DOMAIN;
        }
        strlcpy(domain,domain_ptr,NV_MAX_NAI_LENGTH);
        domain[NV_MAX_NAI_LENGTH] = 0;  // NULL terminate

        nai_length = NV_MAX_NAI_LENGTH;  // max bytes to write
        if( !nv_nai_from_mdn( nai_to_write,
                           &nai_length,
                           &(cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->mob_dir),
                           domain ) )
        {
          ERR("Couldn't convert MDN and domain to NAI!",0,0,0);
          commit_state = NV_COMMIT_PRIMARY_SIP_NAI_AFTER_MDN;
          break;
        }

        item_to_be_written = TRUE;
        local_cmd.cmd = NV_WRITE_F;
        local_item.ds_mip_gen_user_prof.nai_length = nai_length;
        memscpy( (char*) local_item.ds_mip_gen_user_prof.nai,
                NV_MAX_NAI_LENGTH,
                nai_to_write,
                nai_length );
        commit_state = NV_COMMIT_PRIMARY_SIP_NAI_AFTER_MDN;
        break;

      case NV_COMMIT_PRIMARY_SIP_NAI_AFTER_MDN:

        local_cmd.cmd    = NV_READ_F;
        local_cmd.item   = NV_PPP_USER_ID_I;

        local_cmd_ext.nvcmd = &local_cmd;
        local_cmd_ext.context = 0; /* No context is defined */

        /*Since we initialized the local_cmd.tcb_ptr to NULL, this is a
        sychronous call to RUIM and we get the status populated in
        local_cmd.status if item is supported in RUIM*/

        if ((status = nv_verify_access_ruim(local_cmd_ext.nvcmd) ) == NV_DONE_S)
        {
            local_cmd.status = local_cmd_ext.nvcmd->status;
        }
        else if (status == NV_RUIM_NOT_SUPPORTED_S)
        {
        local_cmd.status = nvio_read(&local_cmd_ext, cmd_req_from);
        }
        else
        {
            /*if status is NV_BADPARM_S */
            NV_MSG_ERROR_1 ("Error %d reading NV",local_cmd.status);
            local_cmd.status = NV_FAIL_S;
        }

        if (local_cmd.status == NV_NOTACTIVE_S)
        {
          local_item.ppp_user_id.user_id_len = 0;
        }
        else if (local_cmd.status != NV_DONE_S)
        {
          NV_MSG_ERROR_1 ("Error %d reading NV",local_cmd.status);
          commit_state = NV_COMMIT_AN_NAI_AFTER_MDN;
          break;
        }

        // copy the old domain from the user profile, or the default domain
        local_item.ppp_user_id.user_id[local_item.ppp_user_id.user_id_len] = 0;
        domain_ptr = strchr( (char*) local_item.ppp_user_id.user_id, '@' );
        if (domain_ptr == NULL)
        {
          NV_MSG_HIGH("Using default domain for SIP nai");
          domain_ptr = PS_NAI_DFLT_DOMAIN;
        }
        strlcpy(domain,domain_ptr,NV_MAX_PPP_USER_ID_LENGTH);
        domain[NV_MAX_PPP_USER_ID_LENGTH] = 0;  // NULL terminate

        nai_length = NV_MAX_PPP_USER_ID_LENGTH;  // max bytes to write
        if( !nv_nai_from_mdn( nai_to_write,
                           &nai_length,
                           &(cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->mob_dir),
                           domain ) )
        {
          ERR("Couldn't convert MDN and domain to NAI!",0,0,0);
          commit_state = NV_COMMIT_AN_NAI_AFTER_MDN;
          break;
        }

        item_to_be_written = TRUE;
        local_cmd.cmd      = NV_WRITE_F;
        local_cmd.item     = NV_PPP_USER_ID_I;
        local_item.ppp_user_id.user_id_len = nai_length;
        memscpy( (char*) local_item.ppp_user_id.user_id,
                NV_MAX_PPP_USER_ID_LENGTH,
                nai_to_write,
                nai_length );
        commit_state = NV_COMMIT_AN_NAI_AFTER_MDN;
        break;

      case NV_COMMIT_AN_NAI_AFTER_MDN:
        
        local_cmd.cmd    = NV_READ_F;
        local_cmd.item   = NV_HDR_AN_AUTH_USER_ID_LONG_I;

        local_cmd_ext.nvcmd = &local_cmd;
        local_cmd_ext.context = 0; /* No context is defined */

        /*Since we initialized the local_cmd.tcb_ptr to NULL, this is a
        sychronous call to RUIM and we get the status populated in
        local_cmd.status if item is supported in RUIM*/

        if (( status = nv_verify_access_ruim(local_cmd_ext.nvcmd)) == NV_DONE_S)
        {
            local_cmd.status = local_cmd_ext.nvcmd->status;
        }
        else if (status == NV_RUIM_NOT_SUPPORTED_S)
        {
        local_cmd.status = nvio_read(&local_cmd_ext, cmd_req_from);
        }
        else
        {
            /*if status is NV_BADPARM_S */
            NV_MSG_ERROR_1 ("Error %d reading NV",local_cmd.status);
            local_cmd.status = NV_FAIL_S;
        }

        if (local_cmd.status == NV_NOTACTIVE_S)
        {
          local_item.hdr_an_ppp_user_id.user_id_len = 0;
        }
        else if (local_cmd.status != NV_DONE_S)
        {
          NV_MSG_ERROR_1 ("Error %d reading NV",local_cmd.status);
          commit_state = NV_COMMIT_IMSI_ADDR;
          break;
        }
 
        // copy the old domain from the user profile, or the default domain
        local_item.hdr_an_ppp_user_id.user_id[
          local_item.hdr_an_ppp_user_id.user_id_len] = 0;
        domain_ptr =
          strchr( (char*) local_item.hdr_an_ppp_user_id.user_id, '@' );
        if (domain_ptr == NULL)
        {
          NV_MSG_HIGH("Using default domain for AN nai");
          domain_ptr = PS_NAI_DFLT_DOMAIN;
        }
        strlcpy(domain,domain_ptr,NV_MAX_AN_PPP_USER_ID_LENGTH);
        domain[NV_MAX_AN_PPP_USER_ID_LENGTH] = 0;  // NULL terminate
 
        nai_length = NV_MAX_AN_PPP_USER_ID_LENGTH;  // max bytes to write
        if( !nv_nai_from_mdn( nai_to_write,
                           &nai_length,
                           &(cmd_ext_ptr->nvcmd->data_ptr->otasp_commit->mob_dir),
                           domain ) )
        {
          ERR("Couldn't convert MDN and domain to NAI!",0,0,0);
          commit_state = NV_COMMIT_IMSI_ADDR;
          break;
        }
 
        item_to_be_written = TRUE;
        local_cmd.cmd      = NV_WRITE_F;
        local_cmd.item     = NV_HDR_AN_AUTH_USER_ID_LONG_I;
        local_item.hdr_an_ppp_user_id.user_id_len = nai_length;
        memscpy( (char*) local_item.hdr_an_ppp_user_id.user_id,
                NV_MAX_AN_PPP_USER_ID_LENGTH,
                nai_to_write,
                nai_length );
        commit_state = NV_COMMIT_IMSI_ADDR;
        break;
#endif /* FEATURE_MDN_BASED_NAI */ 


      default:
        return(cmd_ext_ptr->nvcmd->status = NV_BADCMD_S);
      
    } /* switch */

    if (item_to_be_written == TRUE) {
      local_cmd_ext.nvcmd = &local_cmd;
      local_cmd_ext.context = 0; /* No context is defined */

      /*Since we initialized the local_cmd.tcb_ptr to NULL, this is a
      sychronous call to RUIM and we get the status populated in
      local_cmd.status if item is supported in RUIM*/

      if (( status = nv_verify_access_ruim( local_cmd_ext.nvcmd) ) == NV_DONE_S)
      {
        local_cmd.status = local_cmd_ext.nvcmd->status;
      }
      else if (status == NV_RUIM_NOT_SUPPORTED_S)
      {
         local_cmd.status = nvio_write(&local_cmd_ext, cmd_req_from,NV_SSM_DEFAULT_PERM_S);
      }
      else
      {
        /*if status is NV_BADPARM_S */
        NV_MSG_ERROR_1 ("Error %d Writing NV",local_cmd.status);
        local_cmd.status = NV_FAIL_S;
      }

      if (local_cmd.status != NV_DONE_S) {
        return (local_cmd.status);
      }
    }

    /* Reset data_ptr for next item */
    local_cmd.data_ptr = &local_item;
  } /* while */  


  return (local_cmd.status);

#else
  return NV_BADTG_S;

#endif /* FEATURE_NV_OTASP */
} /* nv_otasp_commit */


/*===========================================================================

FUNCTION NV_PRL_IS_VALID

DESCRIPTION
  This function returns the "valid" indicator from the specified NAM's
  roaming list.  

DEPENDENCIES
  None.

RETURN VALUE
  TRUE         if the roaming list "valid" field is TRUE 
  FALSE        if the roaming list "valid" field is FALSE or if the
                 specified NAM is invalid or if the roaming list
                 is currently NOT_ACTIVE

SIDE EFFECTS
  None.  
  
===========================================================================*/

boolean  nv_prl_is_valid
(
  byte   nam          /* Which NAM the request is for */
)
{
  /* Check for illegal NAM */
  if (nam >= NV_MAX_NAMS) return FALSE;

  /* Note that valid field is set FALSE if prl is NOT_ACTIVE */
  return nv_prl_valid_data[nam];
}


/*===========================================================================

FUNCTION NV_PRL_VERSION

DESCRIPTION
  This function returns the "prl_version" indicator from the specified NAM's
  roaming list.  

DEPENDENCIES
  None.

RETURN VALUE
  prl_version             if the roaming list for the specified NAM is
                            currently ACTIVE 
  NV_PRL_VERSION_INVALID  if the specified NAM is invalid or if the 
                            roaming list for the specified NAM is 
                            currently NOT_ACTIVE

SIDE EFFECTS
  None.

===========================================================================*/

word  nv_prl_version
(
  byte   nam          /* Which NAM the request is for */
)
{
  /* Check for illegal NAM */
  if (nam >= NV_MAX_NAMS) return NV_PRL_VERSION_INVALID;

  /* Note that prl_version field is set to NV_PRL_VERSION_INVALID if   */
  /* the roaming list is NOT_ACTIVE                                    */
  return nv_prl_version_data[nam];
}


/*===========================================================================

FUNCTION NVIMNV_TASK_START

DESCRIPTION
  The function first handshakes' with RCINIT framework, then performs NV 
  initialization and then does registers a command table with Diag

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The calling task stays here until it receives the START signal.

===========================================================================*/

static void nvimnv_task_start ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rcinit_handshake_startup();                                                   /* Handshake allows startup to progress */

  /* Initialize the EFS files. */
  if (nv_init() != NV_DONE_S)
  {
     ERR_FATAL("Failed NVIM EFS initialization", 0, 0, 0);
  }

  NV_MSG_MED("NVIM initialization successfull");
  nvi_initialized = TRUE;

  nvdiag_init();


} /* nvimnv_task_start */


/*===========================================================================

FUNCTION NV_FRONT_OP

DESCRIPTION
  This is the function called by both nv_cmd and nv_task. This is required
  in order to use kxmutexes for both synchronous and asynchronous accesses via
  NV.

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_block must be already set.

RETURN VALUE
  None directly.  The status variable of the nv_cmd_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  The nv_cmd_type is placed on the NV command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/


void nv_front_op(
  nv_cmd_ext_type  *cmd_ext_ptr,   /* Pointer to NV command buffer */
  uint32           cmd_req_from   /* cmd requested from local/remote proc */
  )
{
  nv_cmd_type *cmd_ptr = cmd_ext_ptr->nvcmd;
  uint16 context = cmd_ext_ptr->context;
  nv_tcb_ptr_efserrno = rex_self();
  nv_stat_enum_type nv_status =NV_DONE_S;
  
  if (context > MAX_NV_CONTEXT) {
    cmd_ptr->status = NV_FAIL_S;
    NV_MSG_ERROR("NV context out of bounds");
  }
  else {

  /* (void)KxMutex_Lock(&nv_access_op_cs);    */
  (void)osal_enter_crit_sect(&nv_access_op_cs);

  switch ((uint32)(cmd_ptr->cmd)) {
    case NV_READ_F:
        cmd_ptr->status = nvio_read(cmd_ext_ptr, cmd_req_from);
    break;

    case NV_WRITE_F:
        cmd_ptr->status = nvio_write(cmd_ext_ptr, cmd_req_from, nvim_get_perm(cmd_req_from, cmd_ptr));
    break;

    case NV_PEEK_F:
      cmd_ptr->status = NV_FAIL_S;
    break;

    case NV_POKE_F:
      cmd_ptr->status = NV_FAIL_S;
    break;

	case NV_FREE_F:
          cmd_ptr->status = nvim_remove_item(cmd_ext_ptr);
    break;

    case NV_CHKPNT_ENA_F:
      break;

    case NV_CHKPNT_DIS_F:
      break;

	case NV_OTASP_COMMIT_F:
        cmd_ptr->status = nv_otasp_commit(cmd_ext_ptr, cmd_req_from);
    break;

	case NV_REPLACE_F:
        cmd_ptr->status = nvio_write(cmd_ext_ptr, cmd_req_from, nvim_get_perm(cmd_req_from, cmd_ptr));
    break;

    case NV_INCREMENT_F:
        cmd_ptr->status = nvim_increment(cmd_ext_ptr, cmd_req_from);
    break;

    case NV_WRITE_ONCE_F:
    {
        /*update to Write i.e. original client command */
        cmd_ext_ptr->nvcmd->cmd = NV_WRITE_F;
        nv_status = nvio_read(cmd_ext_ptr, cmd_req_from);

        if(nv_status == NV_NOTACTIVE_S)
      {
            cmd_ptr->status = nvio_write(cmd_ext_ptr, cmd_req_from, nvim_get_perm(cmd_req_from, cmd_ptr));
          }
		  else
		  {
             cmd_ptr->status = NV_FAIL_S;
            NV_MSG_ERROR_3("NV item=%d write once only for caller=%d err code %d",
                cmd_ptr->item, cmd_req_from, cmd_ptr->status);
      }
    }
    break;

    case NV_RTRE_OP_CONFIG_F:
    default:
      cmd_ptr->status = NV_BADCMD_S;
        NV_MSG_ERROR_3("NV operation failed: item %d command %d status %d",
                            cmd_ptr->item, cmd_ptr->cmd , cmd_ptr->status);

    break;
  }/*switch*/

  if (cmd_ptr->status == NV_FAIL_S ||
      cmd_ptr->status == NV_READONLY_S) {
    NV_MSG_HIGH_1("NV operation failed with efs_errno: %d", efs_errno);
  }

  /* (void)KxMutex_Unlock(&nv_access_op_cs); */
  (void)osal_exit_crit_sect(&nv_access_op_cs);
}

if (cmd_ptr->cmd == NV_WRITE_F) /*log all the write instances for debugging purppose*/
{
   NV_MSG_HIGH_9("NV item = %d, cmd = %d,context = %d, status = %d, Byte1= %d, Byte2= %d, Byte3= %d, Byte4= %d, Byte5= %d",
    cmd_ptr->item, cmd_ptr->cmd, context, cmd_ptr->status,*((char*)(cmd_ptr->data_ptr)), 
    *(((char*)(cmd_ptr->data_ptr))+1), *(((char*)(cmd_ptr->data_ptr))+2),
    *(((char*)(cmd_ptr->data_ptr))+3),*(((char*)(cmd_ptr->data_ptr))+4));
}

  /* Free memory allocated for cmd_ext_ptr */
  free(cmd_ext_ptr);

} /* nv_front_op */


/*===========================================================================
FUNCTION NV_VALIDATE_CALLER

DESCRIPTION
  Validates the caller.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE is the caller is known. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean nv_validate_caller(uint32 caller)
{
  
  if( (caller == NV_CMD_REQ_FROM_MODEM) ||
      (caller == NV_CMD_REQ_FROM_DIAG ) ||
      (caller == NV_CMD_REQ_FROM_APPS ) )
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION NVIM_GET_PERM

DESCRIPTION
  Gets the permission from SSM.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns the 8bit permission per access point if its not default.

SIDE EFFECTS
  None.

===========================================================================*/
static uint32 nvim_get_perm(uint32 cmd_req_from, nv_cmd_type *cmd_ptr)
{
  ssm_err_t nv_ssm_status=0;     /* status returned from SSM API */
  uint32 perm = DEFAULT_ACCESS_PERM;

  /* Get permission info from NV SSM */
  if(E_SSM_SUCCESS == nv_ssm_init_status)
  {
 #ifdef NV_SSM_ATTRIBS_2
      nv_ssm_status = ssm_get_perm(&perm, NV_SSM_ARG_C, nv_ssm_handle,
                         NV_SSM_CALLER_CONV(cmd_req_from), cmd_ptr->item);
 #else
      nv_ssm_status = ssm_get_perm(&perm, NV_SSM_ARG_C, nv_ssm_handle,
                                                           cmd_ptr->item);
 #endif
     if ( (nv_ssm_status != E_SSM_SUCCESS) || (perm == SSM_GET_PERM_ERR) )
     {
        NV_MSG_LOW_3("[nvim_get_perm]: NV ssm status =%d perm=%d perm new=0x%x",
                                    nv_ssm_status, perm, DEFAULT_ACCESS_PERM);
        perm = DEFAULT_ACCESS_PERM;
     }
     if(perm != DEFAULT_ACCESS_PERM)
     {
        NV_MSG_LOW_3("[nvim_get_perm]: NV item=%d cmd=%d perm 0x%x",
                                    cmd_ptr->item, cmd_ptr->cmd, perm);
     } else
     {
        NV_MSG_LOW_2("[nvim_get_perm]: NV item=%d cmd=%d perm default!",
                                        cmd_ptr->item, cmd_ptr->cmd);
     }
    #ifndef NV_SSM_ATTRIBS_2
    if(perm)
    {
        perm = (uint32)((perm >> (8*cmd_req_from))& 0x000000FF);
        NV_MSG_LOW_3("[nvim_get_perm]: NV item=%d caller=%d perm 0x%x",
                            cmd_ptr->item, cmd_req_from, perm);
    }
    /*
    {
       perm = 0x0; // all access points RW allowed
                   // no extraction is needed since it is 0x0 
    }
    */
    #endif
    NV_MSG_LOW_3("[nvim_get_perm]: NV item=%d caller=%d SSM perm 0x%x",
                            cmd_ptr->item, cmd_req_from, perm);
  }
  else
  {
    /* reset the perm value to avoid stray bitmask match */
    perm = NV_SSM_DEFAULT_PERM_S;

    /* SSM inactive, fall back to old model that
       already has known access protection methods*/
    //MSG_ERROR("[nv_validate_cmd]: NV SSM not active, perm default=0x%x",
    //                                                        perm, 0, 0);
    NV_MSG_LOW_3("[nvim_get_perm]: NV item=%d caller=%d DEFAULT perm 0x%x",
                            cmd_ptr->item, cmd_req_from, perm);
  }

  return (perm);

}

/*===========================================================================

FUNCTION NV_TASK

DESCRIPTION
  This is the NV task, created via rex_def_task from the Main Control task.
  The NV task sets MC_ACK_SIG when it is done with initialization and it
  then waits for NV_START_SIG before starting its normal operations.  The
  NV task then enters a main loop, awaiting for commands received on its
  input queue.  Each command received is processed and the loop continues.
  While in the main loop the task kicks the watchdog periodically.

  *MODIFIED to handle new parameter nv_cmd_ext_type

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  nv_task
( 
  dword   parm          /* from REX - ignored */
)
{
  nv_cmd_ext_type     *cmd_ext_ptr;   /* Pointer to command received on nv_cmd_q */
  nv_cmd_type     *cmd_ptr;
  rex_sigs_type   rex_sigs;   /* REX signals word */
  rex_tcb_type    *task_ptr;  /* Task to signal back on request completion. */
  rex_sigs_type   task_sigs;  /* Signals to set back to requesting task */

/*-------------------------------------------------------------------------*/

/* Keep Lint happy */

#ifdef _lint
  parm = parm;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the NV command queue. */
  (void) q_init(&nv_cmd_q);

  /* Initialize the NV remote command queue. */
  (void) q_init(&nv_cmd_remote_q);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Wait for start signal. */

  nvimnv_task_start ( );

  /* Register for Dog Heart Beat */
  nv_dog_rpt_id = dog_hb_register_rex ( NV_DOG_HB_REPORT_SIG );


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* signal nv err log operations to queue writes to nv */
  nvi_task_running = TRUE;   

  for (;;) { /*  while (TRUE) and Lint happy */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Set REX wait with mask of all possible input signals. */

    rex_sigs = rex_wait(  NV_DOG_HB_REPORT_SIG  /* Watchdog HB sig */
                        | NV_CMD_Q_SIG          /* Input of command queue */
                        | NV_CMD_REMOTE_Q_SIG   /* Input of command queue 
                                                   from remote processor */
                        | NV_ERR_LOG_SIG);        /* Process the nverr record */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         COMMAND QUEUE SIGNAL                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/

    if ((rex_sigs & NV_DOG_HB_REPORT_SIG) != 0) {
      NV_MSG_LOW("NV_DOG_HB_REPORT_SIG received, reply to dog HB");
      (void) rex_clr_sigs(&nv_tcb, NV_DOG_HB_REPORT_SIG);
      dog_hb_report( nv_dog_rpt_id );
      /* write out any old ERR() if required */
      nverr_update_log();
    }

    if ((rex_sigs & NV_ERR_LOG_SIG) != 0) {
      NV_MSG_LOW("NV_ERR_LOG_SIG received");
      (void) rex_clr_sigs(&nv_tcb, NV_ERR_LOG_SIG);
      /* write out any old ERR() if required */
      nverr_update_log();
    }
    /* Check if command queue signal was set. */
    /* If set then clear signal and proceeed. */

    if ((rex_sigs & NV_CMD_Q_SIG) != 0) {
      NV_MSG_LOW("NV_CMD_Q_SIG received");
      (void) rex_clr_sigs(&nv_tcb, NV_CMD_Q_SIG);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Enter a loop where the command queue is checked and */
      while ((cmd_ext_ptr = (nv_cmd_ext_type *) q_get(&nv_cmd_q)) != NULL) 
      {
        cmd_ptr = cmd_ext_ptr->nvcmd;

        nv_front_op(cmd_ext_ptr, NV_CMD_REQ_FROM_MODEM);  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Save for a moment the requesting task TCB and signal to set. */
  
        task_ptr = cmd_ptr->tcb_ptr;
        task_sigs = cmd_ptr->sigs;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        
        /* If specified in request then return buffer to specified queue. */

        if (cmd_ptr->done_q_ptr != NULL)
          q_put(cmd_ptr->done_q_ptr, &cmd_ptr->link);
  
        /* If specified in request then signal the requesting task. */
  
        if (task_ptr != NULL)
          (void) rex_set_sigs(task_ptr, task_sigs);
  
      } /* while */
    } /* if command queue signal */


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                  REMOTE COMMAND QUEUE SIGNAL                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/

    if ((rex_sigs & NV_CMD_REMOTE_Q_SIG) != 0) {
      NV_MSG_LOW("NV_CMD_REMOTE_Q_SIG received");
      (void) rex_clr_sigs(&nv_tcb, NV_CMD_REMOTE_Q_SIG);
      while ((cmd_ext_ptr = (nv_cmd_ext_type *) q_get(&nv_cmd_remote_q)) != NULL) 
      {
        cmd_ptr = cmd_ext_ptr->nvcmd;

        nv_front_op(cmd_ext_ptr, NV_CMD_REQ_FROM_APPS);  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Save for a moment the requesting task TCB and signal to set. */
  
        task_ptr = cmd_ptr->tcb_ptr;
        task_sigs = cmd_ptr->sigs;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        
        /* If specified in request then return buffer to specified queue. */

        if (cmd_ptr->done_q_ptr != NULL)
          q_put(cmd_ptr->done_q_ptr, &cmd_ptr->link);
  
        /* If specified in request then signal the requesting task. */
  
        if (task_ptr != NULL)
          (void) rex_set_sigs(task_ptr, task_sigs);
  
      } /* while */
    } /* if command queue signal */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                     WATCHDOG REPORT TIMER SIGNAL                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* PC debug only.                                                          */

#ifdef FEATURE_NV_UNIT_TEST
    if ((rex_sigs & NV_RPT_TIMER_SIG) != 0) {
      (void) rex_clr_sigs(&nv_tcb, NV_RPT_TIMER_SIG);
      NV_MSG_LOW("NV_RPT_TIMER_SIG received");
    }
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* for (;;) */
} /* nv_task */



/*===========================================================================
FUNCTION NVERR_INIT

DESCRIPTION
  Initialize nv error queue for external tasks
  
DEPENDENCIES
  Called once, by NV.C to init buffers

RETURN VALUE
  False if init conflict.

SIDE EFFECTS
  None.

===========================================================================*/
boolean 
nverr_init( void ) {
  int i;
  static boolean nv_error_initialized = FALSE;
  static boolean nv_error_init_in_progress = FALSE;

  /* We have allready visited this routine */
  if(nv_error_initialized) {
    nv_error_init_in_progress = FALSE;
    return TRUE;
  }

  /* Flag incase multiple paths initing nv at same time. */
  if(nv_error_init_in_progress) return FALSE;
  nv_error_init_in_progress = TRUE;

  /* read up shadow copy of error log */
  for(i=0; i< NV_MAX_ERR_LOG; i++) {
    (void) nvio_read_item (
        NV_ERR_LOG_I,
        (byte) i,                          /* Item index within its array */
        (byte*)&(nverr_log.external[i].err_count),          /* data buffer */
        nvim_op_get_size(NV_ERR_LOG_I),
        0 /* context=0 implies no context is defined */
    );
    nverr_log.update_required[i] = FALSE;
  }
  nverr_log.processing_required = FALSE;

  nv_error_initialized = TRUE;
  nv_error_init_in_progress = FALSE;
  return TRUE;
}


/*===========================================================================
FUNCTION NVERR_UPDATE_LOG

DESCRIPTION
  write dirty error log items out to nv.
  
DEPENDENCIES
  nverr_init must have been called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void 
nverr_update_log( void ) {
  int  i;      /* loop counter */

  /* If no errors waiting to be saved, get out of here */
  if(!nverr_log.processing_required) return;

  /* update internal copy of err_log to write out from */
  NVIM_LOCK();
  (void) memscpy((void *) nverr_log.internal,
         sizeof(nverr_log.internal),
         (void *) nverr_log.external, 
         sizeof(nverr_log.external));   /* data buffer size */

  (void) memscpy((void *) nverr_log.update_in_progress,
         sizeof(nverr_log.update_in_progress),
         (void *) nverr_log.update_required, 
         sizeof(nverr_log.update_required));   /* data buffer size */

  for(i=0; i< NV_MAX_ERR_LOG; i++) {
    nverr_log.update_required[i] = FALSE;
  }
  nverr_log.processing_required = FALSE;
  NVIM_FREE();

  /* write out internal shadow copy of error log */
  for(i=0; i< NV_MAX_ERR_LOG; i++) {
    if(nverr_log.update_in_progress[i]) {
      NV_MSG_MED_1("nverr_log entry %d update in progress ", i);
      nvio_write_item
             (
                 NV_ERR_LOG_I,
                 (byte) i,                          /* Item index within its array */
                 (byte *)&(nverr_log.internal[i].err_count),    /* data buffer */ 
                 nvim_op_get_size(NV_ERR_LOG_I),
                 0 /* context=0 implies no context is defined */
             );
    }
  }
}


/*===========================================================================

FUNCTION NV_MAX_SIZE_OF_ROAMING_LIST

DESCRIPTION
  This function returns the maximum number of bytes available for
  storage of a roaming list (per NAM).  The value returned depends 
  on the total amount of NVRAM installed in the phone and the total 
  number of NAMs defined.  

DEPENDENCIES
  The function cannot be called until NV is initialized.  A
  premature call will raise err_fatal.

RETURN VALUE
  The maximum number of bytes available for storage of a roaming list
  (per NAM).

SIDE EFFECTS
  None.

===========================================================================*/

word  
nv_max_size_of_roaming_list( void ) {
  if (!nvi_initialized)
  {
    ERR_FATAL("Illegal task start-up order", 0, 0, 0);
  }

  if (nv_ruim_callback != NULL)
  {
    word size;
    if ((nv_ruim_callback(NV_RUIM_CMD_ROAMING_LIST_MAX_SIZE_IOCTL,
                            NULL, (void *) &size)) == NV_DONE_S)
    {
        return size;
    }
    }
    return (word)0;

} /* nv_max_size_of_roaming_list */


/*===========================================================================

FUNCTION NV_MAX_SIZE_OF_WEB_CACHE

DESCRIPTION
  This function returns the maximum number of bytes currently available 
  for storage of the Unwired Planet cache.  At this time, the cache
  size is always zero (it is not currently used).
  
DEPENDENCIES
  The function cannot be called until NV is initialized.  A
  premature call will raise err_fatal.

RETURN VALUE
  The maximum number of bytes currently available for storage of the
  Unwired Planet cache.

SIDE EFFECTS
  None.

===========================================================================*/

word  
nv_max_size_of_web_cache( void ) {
  if (!nvi_initialized) {
    ERR_FATAL("Illegal task start-up order", 0, 0, 0);
  }
  return (word)0;
} /* nv_max_size_of_web_cache */


/*===========================================================================

FUNCTION NV_BUILT

DESCRIPTION
  This function will return TRUE once the files are available that simulate
  NV storage.

DEPENDENCIES
  This is a special use function, normally called by error services
  to allow early access to NV, and before the NV task has been started.
  The NV Item Manager allows access once file initialization is complete.

RETURN VALUE
  TRUE      - The NV has been built and direct read/write is allowed
  FALSE     - The NV has not been built and access is not allowed

SIDE EFFECTS
  None.

===========================================================================*/

boolean nv_built (void)

{
  return nvi_initialized;
  
} /* nv_built */


/*===========================================================================

FUNCTION NV_CMD

DESCRIPTION
  This is the outside world's interface to the non volatile memory task
  this function takes an already filled out nv_cmd_type and place it on
  the nv queue.  The function returns to the caller after setting the
  status to busy indicaton.  This does _not_ mean that the command has
  been executed yet, just that it is waiting to be executed.

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_block must be already set.

RETURN VALUE
  None directly.  The status variable of the nv_cmd_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  The nv_cmd_type is placed on the NV command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/

void  
nv_cmd (
  nv_cmd_type  *cmd_ptr                   /* Pointer to NV command buffer */
)
{
  nv_validate_type nv_val;
  nv_stat_enum_type nv_status;
  if (cmd_ptr->cmd == NV_RTRE_OP_CONFIG_F)
  {
    nv_process_rtre_cmd(cmd_ptr);
    return;
  }
  nv_cmd_ext_type *cmd_ext_ptr = (nv_cmd_ext_type *) malloc (sizeof(nv_cmd_ext_type));

  if (cmd_ext_ptr != NULL)
  { 

    /* Converting nv_cmd_type to nv_cmd_ext_type by setting context to 0 */
    cmd_ext_ptr->nvcmd = cmd_ptr;
    cmd_ext_ptr->context = 0;
    
    /*verify whether the command is valid by checking its permission and
    perform appropriate action. */

    nv_val = nv_validate_cmd(cmd_ext_ptr, NV_CMD_REQ_FROM_MODEM);

    switch (nv_val) {
       case NV_VALIDATION_FAIL:
        cmd_ptr->status = NV_FAIL_S;
        NV_MSG_ERROR_2 ("NV item %d access denied by SSM, status %d",
                                        cmd_ptr->item, cmd_ptr->status );
        if(cmd_ptr->tcb_ptr != NULL)
        {
            (void) rex_set_sigs(cmd_ptr->tcb_ptr, cmd_ptr->sigs);
        }
        free(cmd_ext_ptr);
        return;

       case NV_VAIDATION_WRITE_ONCE:
       /* Verify with the RUIM whether this is supported by it and if inactive, process
        the write command*/
        if((nv_status = nv_process_write_once_cmd(cmd_ptr)) == NV_RUIM_NOT_SUPPORTED_S)
        {
        /*update the command to write once and it will be handled in nv_front_op()*/
            cmd_ptr->cmd = NV_WRITE_ONCE_F;
        }
        else if (nv_status == NV_DONE_S)
        {
            free(cmd_ext_ptr);
            if (cmd_ptr->cmd == NV_WRITE_F){
                NV_MSG_HIGH_3 ("NV item %d processed by RUIM command %d status %d",
                                 cmd_ptr->item, cmd_ptr->cmd, cmd_ptr->status );
            }
            return;
        }
        else
        {
            free(cmd_ext_ptr);
            NV_MSG_ERROR_1 ("Error %d input validation fail in RUIM",
                                                        nv_status);
            if(cmd_ptr->tcb_ptr != NULL)
            {
                (void) rex_set_sigs(cmd_ptr->tcb_ptr, cmd_ptr->sigs);
            }
            return;
        }
        break;

       case NV_VALIDATION_ALLOWED:
       case NV_VALIDATION_DEFAULT:
       /*handling to be re-visited if default perm is modified, TBD */
        if(( nv_status = nv_verify_access_ruim(cmd_ptr)) == NV_RUIM_NOT_SUPPORTED_S )
        {
            NV_MSG_LOW_1 ("NV item %d command yet to be processed by NV",
                                          cmd_ptr->item );
        }
        else if (nv_status == NV_DONE_S)
        {
            free(cmd_ext_ptr);
            if (cmd_ptr->cmd == NV_WRITE_F){
                NV_MSG_HIGH_3 ("NV item %d processed by RUIM command %d status %d",
                                 cmd_ptr->item, cmd_ptr->cmd, cmd_ptr->status );
            }
            return;
        }
        else
        {
            free(cmd_ext_ptr);
            cmd_ptr->status = NV_FAIL_S;
            NV_MSG_ERROR_1 ("Error %d input validation fail in RUIM",
                                                        nv_status);
            if(cmd_ptr->tcb_ptr != NULL)
            {
                (void) rex_set_sigs(cmd_ptr->tcb_ptr, cmd_ptr->sigs);
            }
            return;
        }
        break;

        default:
        /*this should not happen, since we shall return any of above 3 status
        from nv_validate_cmd, we just break and queue the command ??  */
        break;
    }

    if (cmd_ext_ptr->nvcmd->tcb_ptr != NULL)
    {
  /* Set the request status to busy. initialize and link the command  */
  /* onto the NV command queue, set a signal to the NV task and exit. */
      cmd_ext_ptr->nvcmd->status = NV_BUSY_S;
      (void) q_link(cmd_ext_ptr, &cmd_ext_ptr->link);
      q_put(&nv_cmd_q, &cmd_ext_ptr->link);
    (void) rex_set_sigs(&nv_tcb, NV_CMD_Q_SIG);
  }
  else
  {
    /* For synchronous calls, we need to ensure mutexes are initiliazed */
    if(!nv_cs_init_flag)
      nv_access_op_cs_init();
      cmd_ext_ptr->nvcmd->status = NV_BUSY_S;
      nv_front_op(cmd_ext_ptr, NV_CMD_REQ_FROM_MODEM);
  } /* if command has no tcb  */
  }
  else 
  {
    NV_MSG_ERROR("Could not allocate memory for variable of type nv_cmd_ext_type");
    cmd_ptr->status = NV_NOMEM_S;
    if(cmd_ptr->tcb_ptr != NULL) 
    {
      (void) rex_set_sigs(cmd_ptr->tcb_ptr, cmd_ptr->sigs);
    }
  }
} /* nv_cmd */


/*===========================================================================

FUNCTION NV_CMD_EXT

DESCRIPTION
  This is the outside world's interface to the non volatile memory task
  this function takes an already filled out nv_cmd_type and place it on
  the nv queue.  The function returns to the caller after setting the
  status to busy indicaton.  This does _not_ mean that the command has
  been executed yet, just that it is waiting to be executed.

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_block must be already set. This function allows items across
  multiple SIMs to be accessed.

RETURN VALUE
  None directly.  The status variable of the nv_cmd_ext_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  The nv_cmd_ext_type is placed on the NV command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/

void  
nv_cmd_ext (
  nv_cmd_ext_type  *cmd_ext_ptr                   /* Pointer to NV command buffer */
)
{
  nv_validate_type nv_val;
  nv_stat_enum_type nv_status;
  if (cmd_ext_ptr->nvcmd->cmd == NV_RTRE_OP_CONFIG_F)
  {
    nv_process_rtre_cmd(cmd_ext_ptr->nvcmd);
    return;
  }

  nv_cmd_ext_type *lcmd_ext_ptr = (nv_cmd_ext_type *) malloc (sizeof(nv_cmd_ext_type));

  if (lcmd_ext_ptr != NULL)
  { 
    /* copy the parameters into the allocated momory */
    lcmd_ext_ptr->nvcmd = cmd_ext_ptr->nvcmd;
    lcmd_ext_ptr->context = cmd_ext_ptr->context;
  } else {
    NV_MSG_ERROR("Could not allocate memory for variable of type nv_cmd_ext_type");
    cmd_ext_ptr->nvcmd->status = NV_NOMEM_S;
    if(cmd_ext_ptr->nvcmd->tcb_ptr != NULL) 
    {
      (void) rex_set_sigs(cmd_ext_ptr->nvcmd->tcb_ptr, cmd_ext_ptr->nvcmd->sigs);
    }
    return;
  }
    nv_val = nv_validate_cmd(lcmd_ext_ptr, NV_CMD_REQ_FROM_MODEM);

    switch (nv_val) {
       case NV_VALIDATION_FAIL:
        cmd_ext_ptr->nvcmd->status = NV_FAIL_S;
        NV_MSG_ERROR_2("NV item %d access denied by SSM, status %d",
                    cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status);
        if(cmd_ext_ptr->nvcmd->tcb_ptr != NULL)
        {
            (void) rex_set_sigs(cmd_ext_ptr->nvcmd->tcb_ptr, cmd_ext_ptr->nvcmd->sigs);
        }
        free(lcmd_ext_ptr);
        return;

       case NV_VAIDATION_WRITE_ONCE:
        /* Verify with the RUIM whether this is supported by it and if inactive, process
        the write command*/
        if((nv_status = nv_process_write_once_cmd(cmd_ext_ptr->nvcmd))
                                                == NV_RUIM_NOT_SUPPORTED_S)
        {
            /*update the command to write once and it will be handled in nv_front_op()*/
            cmd_ext_ptr->nvcmd->cmd = NV_WRITE_ONCE_F;
        }
        else if (nv_status == NV_DONE_S)
        {
            free(lcmd_ext_ptr);
            if (cmd_ext_ptr->nvcmd->cmd == NV_WRITE_F ){
                NV_MSG_HIGH_4 ("NV item %d processed by RUIM command %d status %d, context = %d",
                cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->cmd,
                cmd_ext_ptr->nvcmd->status, cmd_ext_ptr->context );
            }
            return;
        }
        else
        {
            free(lcmd_ext_ptr);
            cmd_ext_ptr->nvcmd->status = NV_FAIL_S;
            NV_MSG_ERROR_1 ("Error %d input validation fail in RUIM", nv_status);
            if(cmd_ext_ptr->nvcmd->tcb_ptr != NULL)
            {
                (void) rex_set_sigs(cmd_ext_ptr->nvcmd->tcb_ptr,
                                    cmd_ext_ptr->nvcmd->sigs);
            }
            return;
        }
        break;

       case NV_VALIDATION_ALLOWED:
       case NV_VALIDATION_DEFAULT:
        if((nv_status = nv_verify_access_ruim(cmd_ext_ptr->nvcmd)) == NV_RUIM_NOT_SUPPORTED_S )
        {
            NV_MSG_LOW_1 ("NV item %d command yet to be processed by NV", cmd_ext_ptr->nvcmd->item );
        }
        else if(nv_status == NV_DONE_S)
        {
            free(lcmd_ext_ptr);
            if (cmd_ext_ptr->nvcmd->cmd == NV_WRITE_F){
                NV_MSG_HIGH_3 ("NV item %d command processed by RUIM, status %d, context =%d",
                       cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status,  cmd_ext_ptr->context);
            }
            return;
        }
        else
        {
            free(lcmd_ext_ptr);
            cmd_ext_ptr->nvcmd->status = NV_FAIL_S;
            NV_MSG_ERROR_1 ("Error %d input validation fail in RUIM", nv_status);
            if(cmd_ext_ptr->nvcmd->tcb_ptr != NULL)
            {
                (void) rex_set_sigs(cmd_ext_ptr->nvcmd->tcb_ptr,
                                    cmd_ext_ptr->nvcmd->sigs);
            }
            return;
        }
        break;
    }

  /* Set the request status to busy. initialize and link the command  */
  /* onto the NV command queue, set a signal to the NV task and exit. */
  if (lcmd_ext_ptr->nvcmd->tcb_ptr != NULL) 
  {
    lcmd_ext_ptr->nvcmd->status = NV_BUSY_S;
    (void) q_link(lcmd_ext_ptr, &lcmd_ext_ptr->link);
    q_put(&nv_cmd_q, &lcmd_ext_ptr->link);
    (void) rex_set_sigs(&nv_tcb, NV_CMD_Q_SIG);
  }
  else
  {
    /* For synchronous calls, we need to ensure mutexes are initiliazed */
    if(!nv_cs_init_flag)
      nv_access_op_cs_init();
  
    lcmd_ext_ptr->nvcmd->status = NV_BUSY_S;
    nv_front_op(lcmd_ext_ptr, NV_CMD_REQ_FROM_MODEM);
    }
} /* nv_cmd_ext */


/*===========================================================================

FUNCTION NV_CMD_DIAG

DESCRIPTION
  This interface iso nly for Diag to use to access the non volatile memory.
  It will take an already filled out nv_cmd_ext_type param. The function 
  returns to the caller after processing the required command. If the function 
  returns then the command has been executed. 

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_block must be already set. This function allows items across
  multiple SIMs to be accessed.

RETURN VALUE
  None directly.  The status variable of the nv_cmd_ext_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  None

===========================================================================*/

void  
nv_cmd_diag (
  nv_cmd_ext_type  *cmd_ext_ptr  /* Pointer to NV command buffer */
)
{
  nv_validate_type nv_val;
  nv_stat_enum_type nv_status;
  uint32 cmd_req_from;
  nv_cmd_ext_type *lcmd_ext_ptr = (nv_cmd_ext_type *) malloc (sizeof(nv_cmd_ext_type));

  if (lcmd_ext_ptr != NULL)
  {
    /* copy the parameters into the allocated momory */
    lcmd_ext_ptr->nvcmd = cmd_ext_ptr->nvcmd;
    lcmd_ext_ptr->context = cmd_ext_ptr->context;
    /* For synchronous calls, we need to ensure mutexes are initiliazed */
    if(!nv_cs_init_flag)
      nv_access_op_cs_init();
  
    lcmd_ext_ptr->nvcmd->status = NV_BUSY_S;
    if(NV_SEC_CODE_I == cmd_ext_ptr->nvcmd->item)
    {
      cmd_req_from = NV_CMD_REQ_FROM_MODEM;
    }
    else
    {
      cmd_req_from = NV_CMD_REQ_FROM_DIAG;
    }

    nv_val = nv_validate_cmd(lcmd_ext_ptr, cmd_req_from);
    switch(nv_val)
    {
      case NV_VALIDATION_FAIL:
        cmd_ext_ptr->nvcmd->status = NV_FAIL_S;
        NV_MSG_HIGH_2 ("NV item %d access denied by SSM, status %d",
                    cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status );
        free(lcmd_ext_ptr);
        break;
      case NV_VAIDATION_WRITE_ONCE:
         /* Verify with the RUIM whether this is supported by it and if inactive, process
        the write command*/
        if((nv_status = nv_process_write_once_cmd(cmd_ext_ptr->nvcmd)) == NV_RUIM_NOT_SUPPORTED_S)
        {
    /*update the command to write once and it will be handled in nv_front_op()*/
            lcmd_ext_ptr->nvcmd->cmd = NV_WRITE_ONCE_F;
            nv_front_op(lcmd_ext_ptr, cmd_req_from);
        }
        else if (nv_status == NV_DONE_S)
        {
            if (cmd_ext_ptr->nvcmd->cmd == NV_WRITE_F){
                NV_MSG_HIGH_3 ("NV item %d command processed by RUIM, status %d, context =%d",
                      cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status, cmd_ext_ptr->context );
            }
            free(lcmd_ext_ptr);
        }
        else
        {
            NV_MSG_ERROR_2 ("NV item %d input validation failed by RUIM, status %d",
                      cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status);
            cmd_ext_ptr->nvcmd->status = NV_FAIL_S;
            free(lcmd_ext_ptr);
        }
        break;
      case NV_VALIDATION_ALLOWED:
      case NV_VALIDATION_DEFAULT:
        if((nv_status = nv_verify_access_ruim(cmd_ext_ptr->nvcmd)) == NV_RUIM_NOT_SUPPORTED_S )
        {
            nv_front_op(lcmd_ext_ptr, cmd_req_from);
        }
        else if(nv_status == NV_DONE_S)
        {
            if (cmd_ext_ptr->nvcmd->cmd == NV_WRITE_F){
                NV_MSG_HIGH_3 ("NV item %d command processed by RUIM, status %d, context = %d",
                      cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status ,cmd_ext_ptr->context);
            }
            free(lcmd_ext_ptr);
    }
    else
    {
            NV_MSG_ERROR_2 ("NV item %d input validation failed by RUIM, status %d",
                      cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status);
            cmd_ext_ptr->nvcmd->status = NV_FAIL_S;
            free(lcmd_ext_ptr);
        }
        break;
    }
  }
  else {
    NV_MSG_ERROR ("Could not allocate memory for nv_cmd_ext_type");
    cmd_ext_ptr->nvcmd->status = NV_NOMEM_S;
  }
} /* nv_cmd_diag */




/*===========================================================================

FUNCTION NV_CMD_R

DESCRIPTION
  This is same as nv_cmd but it waits till NV finishes its operation.

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_block must be already set.

RETURN VALUE
  None directly.  The status variable of the nv_cmd_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  None.

===========================================================================*/

void  
nv_cmd_r (
  nv_cmd_type  *cmd_ptr,                   /* Pointer to NV command buffer */
  nv_item_type *data_ptr /* Pointer to structure that contains item data */
)
{

#ifdef FEATURE_RPC
  nv_validate_type nv_val;
  nv_stat_enum_type nv_status;
  if (cmd_ptr->cmd == NV_RTRE_OP_CONFIG_F)
  {
    nv_process_rtre_cmd(cmd_ptr);
    return;
  }
  nv_cmd_ext_type *cmd_ext_ptr = (nv_cmd_ext_type *) malloc (sizeof(nv_cmd_ext_type));
  
  /* Save params */
  rex_tcb_type *client_tcb = cmd_ptr->tcb_ptr;
  nv_item_type *client_data = cmd_ptr->data_ptr;

  /* Set the request status to busy. initialize and link the command  */
  /* onto the NV command queue, set a signal to the NV task and exit. */

  cmd_ptr->tcb_ptr = rex_self();
  cmd_ptr->data_ptr = data_ptr;


  /* Converting nv_cmd_type to nv_cmd_ext_type by setting context to 0 */
  if (cmd_ext_ptr != NULL) 
  {
    cmd_ext_ptr->nvcmd = cmd_ptr;
    cmd_ext_ptr->context = 0;
    
        nv_val = nv_validate_cmd(cmd_ext_ptr, NV_CMD_REQ_FROM_MODEM)
        switch(nv_val)
        {
          case NV_VALIDATION_FAIL:
            cmd_ptr->status = NV_FAIL_S;
            NV_MSG_HIGH_2 ("NV item %d access denied by SSM, status %d",
                                    cmd_ptr->item, cmd_ptr->status );
            free(cmd_ext_ptr);
            return;

          case NV_VAIDATION_WRITE_ONCE:
            /* Verify with the RUIM whether this is supported by it and if inactive, process
            the write command*/
            if(( nv_status = nv_process_write_once_cmd(cmd_ptr)) ==
                                                NV_RUIM_NOT_SUPPORTED_S)
            {
    /*update the command to write once and it will be handled in nv_front_op()*/
                cmd_ptr->cmd = NV_WRITE_ONCE_F;
            }
            else if (nv_status == NV_DONE_S)
            {
                free(cmd_ext_ptr);
                NV_MSG_HIGH_2 ("NV item %d command in process by RUIM, status %d",
                      cmd_ptr->item, cmd_ptr->status);
                return;
            }
            else
            {
                cmd_ptr->status = NV_FAIL_S;
                NV_MSG_ERROR_2 ("NV item %d input validation failed in RUIM,status %d",
                                    cmd_ptr->item, cmd_ptr->status );
                free(cmd_ext_ptr);
                return;
            }
            break;
          case NV_VALIDATION_ALLOWED:
          case NV_VALIDATION_DEFAULT:
            if((nv_status = nv_verify_access_ruim(cmd_ptr)) ==
                                                NV_RUIM_NOT_SUPPORTED_S )
            {
                NV_MSG_LOW_1 ("NV item %d command yet to be processed by NV",
                                          cmd_ptr->item);
            }
            else if (nv_status == NV_DONE_S)
            {
                free(cmd_ext_ptr);
                NV_MSG_HIGH_2 ("NV item %d command in process by RUIM, status %d",
                      cmd_ptr->item, cmd_ptr->status );
                return;
            }
            else
            {
                cmd_ptr->status = NV_FAIL_S;
                NV_MSG_ERROR_2 ("NV item %d input validation failed in RUIM,status %d",
                                    cmd_ptr->item, cmd_ptr->status );
                free(cmd_ext_ptr);
                return;
            }
            break;
        }

    /* Putting nv_cmd_ext on queue */
        cmd_ext_ptr->nvcmd->status = NV_BUSY_S;
    (void) q_link(cmd_ext_ptr, &cmd_ext_ptr->link);
    q_put(&nv_cmd_q, &cmd_ext_ptr->link);
  (void) rex_set_sigs(&nv_tcb, NV_CMD_Q_SIG);
  rex_wait( cmd_ptr->sigs );
  rex_clr_sigs( cmd_ptr->tcb_ptr, cmd_ptr->sigs );

  }
  else
  {
        NV_MSG_HIGH ("Could not allocate memory for variable of type nv_cmd_ext_type");
    cmd_ptr->status = NV_NOMEM_S;
  }

  /* Restore params */

  cmd_ptr->tcb_ptr = client_tcb;
  cmd_ptr->data_ptr = client_data;
#else
  NV_MSG_HIGH("nv_cmd_r():FEATURE_RPC not defined");
#endif
} /* nv_cmd_r */

/*===========================================================================

FUNCTION NV_CMD_EXT_R

DESCRIPTION
  This is same as nv_cmd but it waits till NV finishes its operation.

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_block must be already set.

RETURN VALUE
  None directly.  The status variable of the nv_cmd_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  None.

===========================================================================*/

void  
nv_cmd_ext_r (
  nv_cmd_ext_type  *cmd_ext_ptr,                   /* Pointer to NV command buffer */
  nv_item_type *data_ptr /* Pointer to structure that contains item data */
)
{
#ifdef FEATURE_RPC
  nv_validate_type nv_val;
  nv_stat_enum_type nv_status;
  if (cmd_ext_ptr->nvcmd->cmd == NV_RTRE_OP_CONFIG_F)
  {
    nv_process_rtre_cmd(cmd_ext_ptr->nvcmd);
    return;
  }
  /* Save params */
  rex_tcb_type *client_tcb = cmd_ext_ptr->nvcmd->tcb_ptr;
  nv_item_type *client_data = cmd_ext_ptr->nvcmd->data_ptr;


  cmd_ext_ptr->nvcmd->tcb_ptr = rex_self();
  cmd_ext_ptr->nvcmd->data_ptr = data_ptr;


  nv_val = nv_validate_cmd(cmd_ext_ptr, NV_CMD_REQ_FROM_MODEM);

  switch (nv_val)
  {
       case NV_VALIDATION_FAIL:
       cmd_ext_ptr->nvcmd->status == NV_FAIL_S;
        NV_MSG_HIGH_2 ("NV item %d access denied by SSM, status %d",
                  cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status);
        break;

       case NV_VAIDATION_WRITE_ONCE:
        /* Verify with the RUIM whether this is supported by it and if inactive, process
        the write command*/
        if((nv_status = nv_process_write_once_cmd(cmd_ext_ptr->nvcmd)) ==
                                                    NV_RUIM_NOT_SUPPORTED_S)
        {
    /*update the command to write once and it will be handled in nv_front_op()*/
            cmd_ext_ptr->nvcmd->cmd = NV_WRITE_ONCE_F;
        }
        else if (nv_status == NV_DONE_S)
        {
            NV_MSG_HIGH_2 ("NV item %d command in process by RUIM, status %d",
                      cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status);
        }
        else
        {
            NV_MSG_ERROR_2("NV item %d input validation failed in  RUIM, status %d",
                  cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status );
        }
        break;

       case NV_VALIDATION_ALLOWED:
       case NV_VALIDATION_DEFAULT:

        if((nv_status = nv_verify_access_ruim(cmd_ext_ptr->nvcmd)) == NV_RUIM_NOT_SUPPORTED_S )
        {
            NV_MSG_LOW_1 ("NV item %d command yet to be processed by NV",
                                          cmd_ext_ptr->nvcmd->item);
        }
        else if (nv_status = NV_DONE_S)
        {
            NV_MSG_HIGH_2 ("NV item %d command in process by RUIM, status %d",
                      cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status );
        }
        else
        {
            NV_MSG_ERROR_2("NV item %d input validation failed in  RUIM, status %d",
                  cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status);
        }

        break;
  }

  /* Putting nv_cmd_ext on queue */
  (void) q_link(cmd_ext_ptr, &cmd_ext_ptr->link);
  q_put(&nv_cmd_q, &cmd_ext_ptr->link);
  (void) rex_set_sigs(&nv_tcb, NV_CMD_Q_SIG);
    rex_wait( cmd_ext_ptr->nvcmd->sigs );
    rex_clr_sigs( cmd_ext_ptr->nvcmd->tcb_ptr, cmd_ext_ptr->nvcmd->sigs );
  
  
  /* Restore params */
  cmd_ext_ptr->nvcmd->tcb_ptr = client_tcb;
  cmd_ext_ptr->nvcmd->data_ptr = client_data;
#else
  NV_MSG_HIGH("nv_cmd_ext_r():FEATURE_RPC not defined");
#endif
  
} /* nv_cmd_ext_r */


/*===========================================================================

FUNCTION NV_CMD_REMOTE

DESCRIPTION
  This is the outside world's interface to the non volatile memory task.

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_type must be already set.

RETURN VALUE

  NV_DONE_S,      -  Request completed okay
  NV_BUSY_S,      -  Request is queued
  NV_BADCMD_S,    -  Unrecognizable command field
  NV_FULL_S,      -  The NVM is full
  NV_FAIL_S,      -  Command failed, reason other than NVM was full
  NV_NOTACTIVE_S, -  Variable was not active
  NV_BADPARM_S,   -  Bad parameter in command block
  NV_READONLY_S,  -  Parameter is write-protected and thus read only
  NV_BADTG_S,     -  Item not valid for Target
  NV_NOMEM_S,     -  free memory exhausted
  NV_NOTALLOC_S   -  address is not a valid allocation

SIDE EFFECTS
  The nv_cmd_type is placed on the NV command queue.  It must not be
  modified until the command has been processed!

===========================================================================*/

nv_stat_enum_type 
nv_cmd_remote (
  nv_func_enum_type cmd,
  nv_items_enum_type item,
  nv_item_type *data_ptr
)
{
  nv_cmd_type nv_rpc_cmd;
#ifdef FEATURE_NV_RPC_SUPPORT

#ifdef FEATURE_NV_ACCESS_FILTER
  if(cmd == NV_READ_F)
  {
    if(unreadable_nv_item(item))
    {
      return NV_BADPARM_S;   
    }
  }
  else 
  { 
    if((cmd == NV_WRITE_F)||(cmd == NV_REPLACE_F))
    {
      if(unwritable_nv_item(item))
      {
        return NV_BADPARM_S;   
      }
    }
  }
#endif

  nv_validate_type nv_val;
  nv_stat_enum_type nv_status = NV_FAIL_S;

  nv_rpc_cmd.status = NV_BUSY_S;
  //(void) q_link(&nv_rpc_cmd, &nv_rpc_cmd.link);
  nv_rpc_cmd.tcb_ptr = rex_self();
  nv_rpc_cmd.sigs = NV_QIDS_RPC_SIG;
  nv_rpc_cmd.cmd = cmd;
  nv_rpc_cmd.item = item;
  nv_rpc_cmd.data_ptr = data_ptr;
  nv_rpc_cmd.done_q_ptr = NULL;

  // nv_cmd( &nv_rpc_cmd );
  {

    nv_cmd_ext_type *cmd_ext_ptr = (nv_cmd_ext_type *) malloc (sizeof(nv_cmd_ext_type));

  if (cmd_ext_ptr != NULL)
  { 

    /* Converting nv_cmd_type to nv_cmd_ext_type by setting context to 0 */
    cmd_ext_ptr->nvcmd = &nv_rpc_cmd;
    cmd_ext_ptr->context = 0;
    
  /* Set the request status to busy. initialize and link the command  */
  /* onto the NV command queue, set a signal to the NV task and exit. */

    nv_val = nv_validate_cmd(cmd_ext_ptr, NV_CMD_REQ_FROM_APPS);
    switch(nv_val)
    {
      case NV_VALIDATION_FAIL:

        nv_rpc_cmd.status =NV_FAIL_S;
        NV_MSG_HIGH_2 ("NV item %d access denied by SSM, status %d",
                     cmd_ext_ptr->nvcmd->item, cmd_ext_ptr->nvcmd->status );
        free(cmd_ext_ptr);
        return NV_FAIL_S;

      case NV_VAIDATION_WRITE_ONCE:
         /* Verify with the RUIM whether this is supported by it and if inactive, process
        the write command*/
        if((nv_status = nv_process_write_once_cmd(&nv_rpc_cmd)) ==
                                                    NV_RUIM_NOT_SUPPORTED_S)
        {
    /*update the command to write once and it will be handled in nv_front_op()*/
            cmd_ext_ptr->nvcmd->cmd = NV_WRITE_ONCE_F;
            NV_MSG_LOW_1 ("NV item %d command yet to be processed by NV",
                                    cmd_ext_ptr->nvcmd->item );
        }
        else if (nv_status == NV_DONE_S)
        {
            free(cmd_ext_ptr);
            NV_MSG_LOW_1 ("NV item %d passed to RUIM for processing",
                                    cmd_ext_ptr->nvcmd->item );
        }
        else
        {
            nv_rpc_cmd.status =NV_FAIL_S;
            NV_MSG_HIGH_2 ("NV item %d input validation failed in RUIM status %d",
                    cmd_ext_ptr->nvcmd->item, nv_status );
            free(cmd_ext_ptr);
            return NV_FAIL_S;
        }
        break;
       case NV_VALIDATION_ALLOWED:
       case NV_VALIDATION_DEFAULT:
        if((nv_status = nv_verify_access_ruim(&nv_rpc_cmd)) == NV_RUIM_NOT_SUPPORTED_S )
        {
            NV_MSG_LOW_1 ("NV item %d command yet to be processed by NV",
                                    cmd_ext_ptr->nvcmd->item);
        }
        else if (nv_status == NV_DONE_S)
        {
            free(cmd_ext_ptr);
            NV_MSG_LOW_1 ("NV item %d passed to RUIM for processing",
                                    cmd_ext_ptr->nvcmd->item);
        }
        else
        {
            nv_rpc_cmd.status =NV_FAIL_S;
            NV_MSG_HIGH_2 ("NV item %d input validation failed in RUIM status %d",
                    cmd_ext_ptr->nvcmd->item, nv_status);
            free(cmd_ext_ptr);
            return NV_FAIL_S;
        }
        break;
    }

    if(nv_status == NV_RUIM_NOT_SUPPORTED_S)
    {
        /* Set the request status to busy. initialize and link the command  */
        /* onto the NV command queue, set a signal to the NV task and exit. */
      cmd_ext_ptr->nvcmd->status = NV_BUSY_S;
      (void) q_link(cmd_ext_ptr, &cmd_ext_ptr->link);
      q_put(&nv_cmd_remote_q, &cmd_ext_ptr->link);
    (void) rex_set_sigs(&nv_tcb, NV_CMD_REMOTE_Q_SIG);
  }

     /* this signal needs to be reserved for all worker task*/
    rex_wait( NV_QIDS_RPC_SIG );
    rex_clr_sigs( nv_rpc_cmd.tcb_ptr, NV_QIDS_RPC_SIG );
  }
  else 
  {
    NV_MSG_HIGH ("Could not allocate memory for variable of type nv_cmd_ext_type");
    // (void) rex_set_sigs(nv_rpc_cmd.tcb_ptr, NV_QIDS_RPC_SIG);
    return NV_NOMEM_S;
  }
  }

#else
  NV_MSG_HIGH("nv_stat_enum_type(): FEATURE_NV_RPC_SUPPORT not defined");
  nv_rpc_cmd.status = NV_FAIL_S;
#endif
  return nv_rpc_cmd.status;

}

/*===========================================================================

FUNCTION NV_CMD_EXT_REMOTE

DESCRIPTION
  This is the outside world's interface to the non volatile memory task.

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_ext_type must be already set.

RETURN VALUE

  NV_DONE_S,      -  Request completed okay
  NV_BUSY_S,      -  Request is queued
  NV_BADCMD_S,    -  Unrecognizable command field
  NV_FULL_S,      -  The NVM is full
  NV_FAIL_S,      -  Command failed, reason other than NVM was full
  NV_NOTACTIVE_S, -  Variable was not active
  NV_BADPARM_S,   -  Bad parameter in command block
  NV_READONLY_S,  -  Parameter is write-protected and thus read only
  NV_BADTG_S,     -  Item not valid for Target
  NV_NOMEM_S,     -  free memory exhausted
  NV_NOTALLOC_S   -  address is not a valid allocation

SIDE EFFECTS
  The nv_cmd_ext_type is placed on the NV command queue.  It must not be
  modified until the command has been processed!

===========================================================================*/

nv_stat_enum_type 
nv_cmd_ext_remote (
  nv_func_enum_type cmd,
  nv_items_enum_type item,
  nv_item_type *data_ptr,
  uint16 context
)
{
  nv_cmd_type nv_rpc_cmd;
#ifdef FEATURE_NV_RPC_SUPPORT

#ifdef FEATURE_NV_ACCESS_FILTER
  if(cmd == NV_READ_F)
  {
    if(unreadable_nv_item(item))
    {
      return NV_BADPARM_S;   
    }
  }
  else 
  { 
    if((cmd == NV_WRITE_F)||(cmd == NV_REPLACE_F))
    {
      if(unwritable_nv_item(item))
      {
        return NV_BADPARM_S;   
      }
    }
  }
#endif
  nv_validate_type nv_val;
  nv_stat_enum_type nv_status = NV_FAIL_S;

  nv_rpc_cmd.status = NV_BUSY_S;
  (void) q_link(&nv_rpc_cmd, &nv_rpc_cmd.link);
  nv_rpc_cmd.tcb_ptr = rex_self();
  nv_rpc_cmd.sigs = NV_QIDS_RPC_SIG;
  nv_rpc_cmd.cmd = cmd;
  nv_rpc_cmd.item = item;
  nv_rpc_cmd.data_ptr = data_ptr;
  nv_rpc_cmd.done_q_ptr = NULL;
  
  {

  nv_cmd_ext_type *lcmd_ext_ptr = (nv_cmd_ext_type *) malloc (sizeof(nv_cmd_ext_type));

  if (lcmd_ext_ptr != NULL)
  { 
    /* copy the parameters into the allocated momory */
    lcmd_ext_ptr->nvcmd = &nv_rpc_cmd;
    lcmd_ext_ptr->context = context;
  } else {
    NV_MSG_HIGH ("Could not allocate memory for variable of type nv_cmd_ext_type");
    // (void) rex_set_sigs(nv_rpc_cmd.tcb_ptr, NV_QIDS_RPC_SIG);
    return NV_NOMEM_S;
  }

  nv_val = nv_validate_cmd(lcmd_ext_ptr, NV_CMD_REQ_FROM_APPS);
  switch(nv_val)
  {
      case NV_VALIDATION_FAIL:
        nv_rpc_cmd.status = NV_FAIL_S;
        NV_MSG_ERROR_2 ("NV item %d access denied by SSM, status %d",
                     lcmd_ext_ptr->nvcmd->item, lcmd_ext_ptr->nvcmd->status );
        free(lcmd_ext_ptr);
        return NV_FAIL_S;

      case NV_VAIDATION_WRITE_ONCE:
        /* Verify with the RUIM whether this is supported by it and if inactive, process
        the write command*/
        if((nv_status = nv_process_write_once_cmd(&nv_rpc_cmd)) ==
                                            NV_RUIM_NOT_SUPPORTED_S)
        {
            /*update the command to write once and it will be handled
            in nv_front_op()*/

            lcmd_ext_ptr->nvcmd->cmd = NV_WRITE_ONCE_F;
            NV_MSG_LOW_1 ("NV item %d command yet to be processed by NV",
                                    lcmd_ext_ptr->nvcmd->item);
        }
        else if (nv_status == NV_DONE_S)
        {
            free(lcmd_ext_ptr);
            NV_MSG_LOW_1 ("NV item %d passed to RUIM for processing",
                                    lcmd_ext_ptr->nvcmd->item);
        }
        else
        {
            nv_rpc_cmd.status = NV_FAIL_S;
            NV_MSG_ERROR_2 ("NV item %d input validation failed status %d",
                            lcmd_ext_ptr->nvcmd->item, nv_status );
            free(lcmd_ext_ptr);
            return NV_FAIL_S;
        }
        break;

      case NV_VALIDATION_ALLOWED:
      case NV_VALIDATION_DEFAULT:
        if((nv_status = nv_verify_access_ruim(&nv_rpc_cmd)) == NV_RUIM_NOT_SUPPORTED_S )
        {
            NV_MSG_LOW_1 ("NV item %d command yet to be processed by NV",
                                    lcmd_ext_ptr->nvcmd->item );
        }
        else if(nv_status == NV_DONE_S)
        {
            free(lcmd_ext_ptr);
            NV_MSG_LOW_1 ("NV item %d passed to RUIM for processing",
                                    lcmd_ext_ptr->nvcmd->item);
        }
        else
        {
            nv_rpc_cmd.status = NV_FAIL_S;
            NV_MSG_ERROR_2 ("NV item %d input validation failed status %d",
                            lcmd_ext_ptr->nvcmd->item, nv_status);
            free(lcmd_ext_ptr);
            return NV_FAIL_S;
        }
        break;
  }

  if(nv_status == NV_RUIM_NOT_SUPPORTED_S)
  {
  /* Set the request status to busy. initialize and link the command  */
  /* onto the NV command queue, set a signal to the NV task and exit. */
    lcmd_ext_ptr->nvcmd->status = NV_BUSY_S;
    (void) q_link(lcmd_ext_ptr, &lcmd_ext_ptr->link);
    q_put(&nv_cmd_remote_q, &lcmd_ext_ptr->link);
    (void) rex_set_sigs(&nv_tcb, NV_CMD_REMOTE_Q_SIG);
  }

    /* this signal needs to be reserved for all worker task*/
    rex_wait( NV_QIDS_RPC_SIG );
  rex_clr_sigs( nv_rpc_cmd.tcb_ptr, NV_QIDS_RPC_SIG );
  }

#else
  NV_MSG_HIGH("nv_cmd_ext_remote(): FEATURE_NV_RPC_SUPPORT not defined");
  nv_rpc_cmd.status = NV_FAIL_S;
#endif

  return nv_rpc_cmd.status;

}


/*===========================================================================

FUNCTION NVIM_SSM_STATUS

DESCRIPTION
  This procedure checks if NV SSM is active.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SSM active
  FALSE if not

SIDE EFFECTS
  None.

============================================================================*/
boolean nvim_ssm_status(void)
{
  if(nv_ssm_handle == SSM_CLIENT_INVALID_ID)
  {
     return FALSE;
  } else
  {
    return TRUE;
  }
}


/*===========================================================================

FUNCTION SP_NVIM_READ_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's a Service
  Programming item, which should not be read until the SPC has been
  entered, or security unlocked.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is a Service Programming item,
  FALSE if not

SIDE EFFECTS
  None.

============================================================================*/
boolean sp_nvim_read_item (
  nv_items_enum_type item
)
{
  switch (item) {
    case NV_SEC_CODE_I:
    case NV_FSC_I:
    case NV_FSC2_I:
    case NV_FSC2_CHKSUM_I:
    case NV_OTKSL_I:
    case NV_PAP_USER_ID_I:
    case NV_PPP_USER_ID_I:
    case NV_HDR_AN_AUTH_NAI_I:
    case NV_DEVICE_SERIAL_NO_I:

      return (TRUE);

    default:
      return (FALSE);
  }
} /* sp_nvim_read_item */


/*===========================================================================

FUNCTION SP_NVIM_WRITE_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's a Service
  Programming item, which should not be written until the SPC
  has been entered, or security unlocked.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is a Service Programming item,
  FALSE if not

SIDE EFFECTS
  None.

============================================================================*/
boolean sp_nvim_write_item (
  nv_items_enum_type item
)
{
  switch (item) {
    case NV_SLOT_CYCLE_INDEX_I:
    case NV_CDMA_SID_LOCK_I:
    case NV_CDMA_SID_ACQ_I:
    case NV_ANALOG_SID_LOCK_I:
    case NV_ANALOG_SID_ACQ_I:
    case NV_ANALOG_FIRSTCHP_I:
    case NV_ANALOG_HOME_SID_I:
    case NV_ANALOG_REG_I:
    case NV_PCDMACH_I:
    case NV_SCDMACH_I:
    case NV_NAM_CHKSUM_I:
    case NV_MIN1_I:
    case NV_MIN2_I:
    case NV_MOB_TERM_HOME_I:
    case NV_MOB_TERM_FOR_SID_I:
    case NV_MOB_TERM_FOR_NID_I:
    case NV_ACCOLC_I:
    case NV_HOME_SID_NID_I:
    case NV_SID_NID_I:
    case NV_MIN_CHKSUM_I:
    case NV_SEC_CODE_I:
    case NV_IMSI_MCC_I:
    case NV_IMSI_11_12_I:
    case NV_DIR_NUMBER_I:
    case NV_FSC_I:
    case NV_WDC_I:
    case NV_FSC2_I:
    case NV_FSC2_CHKSUM_I:
    case NV_IMSI_ADDR_NUM_I:
    case NV_DIR_NUMBER_PCS_I:
    case NV_OTKSL_I:
    case NV_RTRE_CONFIG_I:
    case NV_UIM_PREF_PROTOCOL_I:
    case NV_PAP_USER_ID_I:
    case NV_PAP_PASSWORD_I:
    case NV_A_KEY_I:
    case NV_A_KEY_CHKSUM_I:
    case NV_SSD_A_I:
    case NV_SSD_B_I:
    case NV_UP_KEY_I:
    case NV_ROAMING_LIST_I:
    case NV_ROAMING_LIST_683_I:   
    case NV_HDR_AN_AUTH_PASSWORD_I:
    case NV_HDR_AN_AUTH_PASSWD_LONG_I:
    case NV_HDR_AN_PPP_PASSWORD_I:
    case NV_PPP_PASSWORD_I:
    case NV_DS_MIP_SS_USER_PROF_I:
    case NV_DS_MIP_DMU_MN_AUTH_I:
    case NV_DEVICE_SERIAL_NO_I:
    case NV_HDRSCMDB_SECURE_CONFIG_I:
    case NV_GPSONE_PASSWORD_I:
    case NV_GPS1_SEC_UPDATE_RATE_I:

      return (TRUE);

    default:
      return (FALSE);
  }
} /* sp_nvim_write_item */


/*===========================================================================

FUNCTION NV_RTRE_USE_RUIM_FOR_NAM

DESCRIPTION
  This function determines if the R-UIM is to be used (based on RTRE control)
  and the NAM is the R-UIM NAM.
  
DEPENDENCIES
  The return value has a default setting and is updated when NV receives
  a NV_RTRE_OP_CONFIG_F command.

RETURN VALUE
  The RTRE control indicates to use the R-UIM and the NAM matches the R-UIM 
  NAM.

SIDE EFFECTS
  This function uses the current value of the RTRE control.  It may
  change.

===========================================================================*/

boolean 
nv_rtre_use_ruim_for_nam (
  byte nam
)
{
    if(nv_ruim_callback != NULL)
    {
        boolean status;
        byte lnam = nam;
        if ((nv_ruim_callback(NV_RUIM_CMD_RTRE_NAM_IOCTL , (void *) &lnam,
                                            (void *) &status)) == NV_DONE_S )
        {
            return status;
        }
    }
    (void)nam;
    return FALSE;
}

/*==================================================================
Function: NV_ACCESS_OP_CS_INIT

Description: This functions initialises kxmutex.
==================================================================*/

void nv_access_op_cs_init(void)
{
  /*if (!(KxMutex_Init(&nv_access_op_cs,"KXMUTEX_NV_ACCESS_OP_CS"))) 
  {
    ERR_FATAL("nv_access_op_cs KxMutex_Alloc Failed",0,0,0);
  } */
  osal_init_crit_sect(&nv_access_op_cs);
  nv_cs_init_flag = TRUE;
}

/*===========================================================================

FUNCTION NV_RTRE_CONTROL

DESCRIPTION
  This function provides the current RTRE control.  The control indicates 
  whether the use the R-UIM in an operation or not.

DEPENDENCIES
  The return value has a default setting and is updated when NV receives
  a NV_RTRE_OP_CONFIG_F command.

RETURN VALUE
  The RTRE control

SIDE EFFECTS
  This function returns the current value of the RTRE control.  It may
  change.

===========================================================================*/

nv_rtre_control_type 
nv_rtre_control( void ) 
{
    if (nv_ruim_callback != NULL)
    {
        nv_rtre_control_type rtre_control_val;
        if ( (nv_ruim_callback(NV_RUIM_CMD_GET_RTRE_CONTROL_VALUE_IOCTL, NULL,
                            (void *) & rtre_control_val )) == NV_DONE_S )
        {
            return rtre_control_val;
        }
    }
  return NV_RTRE_CONTROL_NO_RUIM;
}

/*===========================================================================

FUNCTION NV_RTRE_POLLING_CONTROL

DESCRIPTION
  This function provides the current RTRE polling control.  The polling control
  indicates whether to poll the UIM card when originating a call or while on 
  the traffic channel.

DEPENDENCIES
  The return value has a default setting and is updated when NV receives
  a NV_RTRE_OP_CONFIG_F command.

RETURN VALUE
  The RTRE polling control

SIDE EFFECTS
  This function returns the current value of the RTRE polling control.  It may
  change.

===========================================================================*/

nv_rtre_polling_control_type 
nv_rtre_polling_control( void ) {

    if(nv_ruim_callback != NULL)
    {
        nv_rtre_polling_control_type polling_control_val;
        if ( (nv_ruim_callback(NV_RUIM_CMD_GET_RTRE_POLLING_CONTROL_VALUE_IOCTL,  NULL,
                                (void *) & polling_control_val ) == NV_DONE_S))
        {
            return polling_control_val;
        }
    }
  return NV_RTRE_POLLING_CONTROL_NO_POLL;
}

/*===========================================================================

FUNCTION NV_RTRE_POLLING_FOR_NAM

DESCRIPTION
  This function provides the current RTRE polling control when the NAM is the 
  R_UIM NAM.  The polling control indicates whether to poll the UIM card when 
  originating a call or while on the traffic channel.

DEPENDENCIES
  The return value has a default setting and is updated when NV receives
  a NV_RTRE_OP_CONFIG_F command.

RETURN VALUE
  The RTRE polling control indicates whether to poll and the NAM matches the
  R-UIM NAM.

SIDE EFFECTS
  This function returns the current value of the RTRE polling control.  It may
  change.

===========================================================================*/

boolean 
nv_rtre_polling_for_nam ( 
  byte nam
)
{
    if(nv_ruim_callback != NULL)
    {
        boolean status;
        byte lnam = nam;
        if((nv_ruim_callback(NV_RUIM_CMD_RTRE_POLLING_NAM_IOCTL, (void *) &lnam,
                                               (void *) &status )) == NV_DONE_S)
        {
            return status;
        }
    }
   (void)nam;
   return FALSE;
}

/*===========================================================================
  Allows RUIM to register its callback with NV.

@param[in] : function pointer to register with NV.

@return
  None

@dependencies
  None

@sideeffects
  None
===========================================================================*/
void nv_ruim_register(nv_ruim_register_cb_type register_fn)
{
    if(!nv_ruim_init_cb)
    {
        nv_ruim_init_cb = TRUE;
        nv_ruim_callback = register_fn;        
    }
}

/*===========================================================================
Verifies whether a command is valid or not by checking the permissions from SSM.
Reutrns appropriate enum based on permissions.

@param[in] : command pointer to nv_cmd_ext_type
             command request information from.

@return
 NV_VALIDATION_FAIL :         Command Validation fails from SSM.
 NV_VALIDATION_ALLOWED:     Command validation allowed.
 NV_VALIDATION_DEFAULT:     If permission set to default.
 NV_VALIDATION_WRITE_ONCE:    If permission is write once.

@dependencies
  None

@sideeffects
  None
===========================================================================*/
static nv_validate_type nv_validate_cmd(
  nv_cmd_ext_type  *cmd_ext_ptr,   /* Pointer to NV command buffer */
  uint32           cmd_req_from   /* cmd requested from local/remote proc */
  )
{
  nv_cmd_type *cmd_ptr = cmd_ext_ptr->nvcmd;
  uint16 context = cmd_ext_ptr->context;
  uint32 perm = NV_SSM_DEFAULT_PERM_S;
  nv_validate_type val_type = NV_VALIDATION_ALLOWED;
  nv_tcb_ptr_efserrno = rex_self();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Validate the caller */
  if(!(nv_validate_caller(cmd_req_from)))
  {
     NV_MSG_ERROR_1("[nv_validate_cmd]: NV unknown caller %d", cmd_req_from);
     cmd_ptr->status = NV_FAIL_S;
  }


  /* Get permission info from NV SSM */
  perm = nvim_get_perm(cmd_req_from, cmd_ptr);

  if( (( NV_SSM_READ_NOT_ALLOWED_S  == (perm & NV_SSM_READ_NOT_ALLOWED_S)  )  &&
       ( NV_SSM_WRITE_NOT_ALLOWED_S == (perm & NV_SSM_WRITE_NOT_ALLOWED_S) )) ||
      (NV_SSM_ACCESS_DENIED_S == (perm & NV_SSM_ACCESS_DENIED_S)) ) {
    /* NV item access denied as per the permissions */
    cmd_ptr->status = NV_FAIL_S;
    NV_MSG_ERROR_1("[nv_validate_cmd]: NV access denied for %d", cmd_ptr->item);
    val_type = NV_VALIDATION_FAIL;
  }
  else if (context > MAX_NV_CONTEXT) {
    cmd_ptr->status = NV_FAIL_S;
    NV_MSG_ERROR_2("[nv_validate_cmd] : NV context out of bounds 0x%x, err code 0x%x",
                                                context , cmd_ptr->status);
    val_type = NV_VALIDATION_FAIL;
  }

  else
  {
  switch (cmd_ptr->cmd)
  {
    case NV_READ_F:
    {
       if( NV_SSM_READ_NOT_ALLOWED_S == (perm & NV_SSM_READ_NOT_ALLOWED_S) )
       {
          /* do we need a different status, NV_ACCESS_DENIED_S */
          cmd_ptr->status = NV_FAIL_S;
          NV_MSG_ERROR_3("NV item=%d read access denied for caller=%d err code %d",
                                cmd_ptr->item, cmd_req_from, cmd_ptr->status);
          val_type = NV_VALIDATION_FAIL;
       }
       else if(NV_SSM_READ_SPC_PROTECTED_S == (perm & NV_SSM_READ_SPC_PROTECTED_S))
       {
/*-----------------------------------------------------------------------
          Next check to see if this item we're reading is a SP item, and if
          it is, make sure the SPC has been entered properly first, or that
          security is unlocked.  If it hasn't, we will return an error packet.
          Also make sure the security password is not unwritten. If the security
          password is unwritten, then we rely on the SPC state flag.
-----------------------------------------------------------------------*/
          if ( (diag_get_sp_state() == DIAG_SPC_LOCKED) &&
               ( (diag_get_security_state() == DIAG_SEC_LOCKED) ||
                 diag_check_password (NULL) == TRUE) )
          {
            cmd_ptr->status = NV_BADPARM_S;
            NV_MSG_ERROR_3("NV item=%d SPC needed for caller=%d err code %d",
                                cmd_ptr->item, cmd_req_from, cmd_ptr->status);
            val_type = NV_VALIDATION_FAIL;
          }
       }
    }
    break;

    case NV_PEEK_F:
      cmd_ptr->status = NV_FAIL_S;
      val_type = NV_VALIDATION_FAIL;
    break;

    case NV_POKE_F:
      cmd_ptr->status = NV_FAIL_S;
      val_type = NV_VALIDATION_FAIL;
    break;

    case NV_WRITE_F:
    case NV_FREE_F:
    case NV_INCREMENT_F:
    case NV_REPLACE_F:
    {
      if( NV_SSM_WRITE_NOT_ALLOWED_S == (perm & NV_SSM_WRITE_NOT_ALLOWED_S) )
      {
          /* to remove you need write access */
          cmd_ptr->status = NV_FAIL_S;
          NV_MSG_ERROR_3("NV item=%d modify access denied caller=%d err code %d",
                                cmd_ptr->item, cmd_req_from, cmd_ptr->status);
          val_type = NV_VALIDATION_FAIL;
      }
      else if( NV_SSM_WRITE_SPC_PROTECTED_S == (perm & NV_SSM_WRITE_SPC_PROTECTED_S))
      {
/*-----------------------------------------------------------------------
          Next check to see if this item we're writing is a SP item, and if
          it is, make sure the SPC has been entered properly first, or that
          security is unlocked.  If it hasn't, we will return an error packet.
          Also make sure the security password is not unwritten. If the security
          password is unwritten, then we rely on the SPC state flag.
-----------------------------------------------------------------------*/
          if ( (diag_get_sp_state() == DIAG_SPC_LOCKED ) &&
               ( (diag_get_security_state() == DIAG_SEC_LOCKED) ||
               diag_check_password (NULL) == TRUE) )
          {
            cmd_ptr->status = NV_FAIL_S;
            NV_MSG_ERROR_3("[nv_validate_cmd]: NV item=%d SPC needed for caller=%d err code %d",
                                    cmd_ptr->item, cmd_req_from, cmd_ptr->status);
            val_type = NV_VALIDATION_FAIL;
          }
      }
      else if( NV_SSM_WRITE_ONCE_S == (perm & NV_SSM_WRITE_ONCE_S) )
      {
        val_type = NV_VAIDATION_WRITE_ONCE;
      }
    }
    break;
    case NV_OTASP_COMMIT_F:
        val_type = NV_VALIDATION_ALLOWED;
        break;

    case NV_CHKPNT_ENA_F:
    case NV_CHKPNT_DIS_F:
        val_type = NV_VALIDATION_FAIL;
        break;

    default:
        cmd_ptr->status = NV_BADCMD_S;
        val_type = NV_VALIDATION_FAIL;
    break;

  }/*switch*/
  }
  return val_type;
} /* nv_validate_cmd */

/*===========================================================================
Queries RUIM callback and verifies whether a command is supported in RUIM or not.
If supported, RUIM performs the command operations and populates the status
varaible in nv_cmd.

@param[in] : command pointer to nv_cmd_type

@return
 NV_DONE_S:                 If RUIM performs the command access.
 NV_RUIM_NOT_SUPPORTED_S:     If item not supported in RUIM.

@dependencies
  None

@sideeffects
  None
===========================================================================*/

nv_stat_enum_type nv_verify_access_ruim(nv_cmd_type* cmd_ptr)
{
    if( nv_ruim_callback != NULL )
    {
        nv_stat_enum_type status;
        status = nv_ruim_callback(NV_RUIM_CMD_VERIFY_AND_ACCESS_IOCTL,
                                            (void *) cmd_ptr, NULL);
        return status;
    }
    return NV_RUIM_NOT_SUPPORTED_S;
}
/*===========================================================================
Queries RUIM callback and proceses the NV_RTRE_OP_CONFIG_F command if supported
in RUIM. Updates the status varaible in nv_cmd.

@param[in] : command pointer to nv_cmd_type

@return
 None

@dependencies
  None

@sideeffects
  None
===========================================================================*/

void nv_process_rtre_cmd(nv_cmd_type* cmd_ptr)
{
    if( nv_ruim_callback != NULL )
    {
        if( (nv_ruim_callback(NV_RUIM_CMD_PROCESS_RTRE_CONFIG_IOCTL,
                                (void *) cmd_ptr, NULL)) != NV_DONE_S)
        {
            /*If RUIM callback is present, it is expected that the caller
            is signalled by RUIM/MMGSDI */
            cmd_ptr->status = NV_BADCMD_S;
            if(cmd_ptr->tcb_ptr != NULL)
            {
                (void) rex_set_sigs(cmd_ptr->tcb_ptr, cmd_ptr->sigs);
            }
        }
        else
        {
            NV_MSG_LOW_1(" Sent RTRE command to RUIM for NV item=%d ",
                                        cmd_ptr->item);
        }
    }
    else
    {
        cmd_ptr->status = NV_BADCMD_S;
        if(cmd_ptr->tcb_ptr != NULL)
        {
            (void) rex_set_sigs(cmd_ptr->tcb_ptr, cmd_ptr->sigs);
        }
    }
}

/*===========================================================================
RUIM calls this API to indicate NV to update the prl version data.

@param[in] : command pointer to nv_cmd_type

@return
 NV_DONE_S: If PRL data is updated.
 NV_FAIL_S:    If item is invalid and prl data is not updated.

@dependencies
  None

@sideeffects
  None
===========================================================================*/

nv_stat_enum_type nv_update_prl_version_data(nv_cmd_type* cmd_ptr)
{
    nv_stat_enum_type status;
    if (cmd_ptr->item == NV_ROAMING_LIST_683_I)
    {
        if (cmd_ptr->status == NV_DONE_S) {
            nv_prl_version_data[cmd_ptr->data_ptr->roaming_list.nam] =
                                cmd_ptr->data_ptr->roaming_list.prl_version;
            nv_prl_valid_data[cmd_ptr->data_ptr->roaming_list.nam] =
                                cmd_ptr->data_ptr->roaming_list.valid;
            status = NV_DONE_S;
        }
        else {
            nv_prl_version_data[cmd_ptr->data_ptr->roaming_list.nam] =
                                                    NV_PRL_VERSION_INVALID;
            nv_prl_valid_data[cmd_ptr->data_ptr->roaming_list.nam] = FALSE;
            status = NV_DONE_S;
        }
    }
    else
    {
        status = NV_FAIL_S;
        NV_MSG_ERROR_1(" Invalid NV item received to update prl data NV item=%d ",
                                        cmd_ptr->item);
    }
    return status;
}

/*===========================================================================
Queries RUIM callback with a read command and proceses the command if supported.
Verifies if status in NV_NOTACTIVE_S and then attempts to write on RUIM.
Else return NV_RUIM_NOT_SUPPORTED_S.

@param[in] : command pointer to nv_cmd_type

@return
 NV_DONE_S:                If RUIM performs the write
 NV_RUIM_NOT_SUPPORTED: NV item not supported in RUIM.
 NV_FAIL_S:                NV item already active.

@dependencies
  None

@sideeffects
  None
===========================================================================*/

nv_stat_enum_type nv_process_write_once_cmd (nv_cmd_type* cmd_ptr)
{
    nv_cmd_type cmd_temp;
    nv_stat_enum_type nv_status;

    cmd_temp.cmd = NV_READ_F;
    cmd_temp.tcb_ptr = NULL;
    cmd_temp.done_q_ptr = NULL;
    cmd_temp.item = cmd_ptr->item;


    cmd_temp.data_ptr = &local_ruim_item;
    nv_status = nv_verify_access_ruim(&cmd_temp);


    if (nv_status == NV_DONE_S)
    {
        if( cmd_temp.status == NV_NOTACTIVE_S)
        {
            nv_status = nv_verify_access_ruim(cmd_ptr);
            if (nv_status == NV_DONE_S)
            {
                NV_MSG_HIGH_1("NV item=%d write handled in RUIM", cmd_ptr->item);
            }
            else
            {
                /*if status is NV_BADPARM_S */
                cmd_ptr->status = NV_FAIL_S;
                NV_MSG_ERROR_1 ("Error %d input validation fail in RUIM", nv_status);
            }
        }
        else
        {
            cmd_ptr->status = NV_FAIL_S;
            NV_MSG_ERROR_2("NV item=%d write once, err code %d",
                            cmd_ptr->item, cmd_ptr->status);
            return NV_FAIL_S;
        }
    }
    else if (nv_status == NV_RUIM_NOT_SUPPORTED_S)
    {
        return NV_RUIM_NOT_SUPPORTED_S;
    }
    else
    {
        /*if status is NV_BADPARM_S */
        cmd_ptr->status = NV_FAIL_S;
        NV_MSG_ERROR_1 ("Error %d input validation fail in RUIM",nv_status);
    }
    return nv_status;
}


#endif /* FEATURE_NV_ITEM_MGR */
