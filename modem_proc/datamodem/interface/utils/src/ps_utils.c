/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ U T I L S . C

GENERAL DESCRIPTION
  Collection of utility functions being used by various modules in PS.
  Most of these functions assume that the caller is in PS task context.

Copyright (c) 1995-2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_utils.c_v   1.0   08 Aug 2002 11:19:58   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_utils.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
10/26/12    bb     Added ps_efs_init and functionality to handle nv item 
                    file paths in protocols 
05/20/11    am     Dyn buffer for extra \n in EFS create.
10/26/10    op     Added functions to use EFS items.
11/21/08    pp     Lint fixes.
09/07/07    scb    Added REX signal ext code under FEATURE_REX_SIGS_EXT
11/02/06    mct    Added 64 bit random number generator function.
02/22/06    msr    Using single critical section
10/31/04   msr/ks  Added memdump().
04/30/04    mct    Fixed some lint errors.
08/05/02    usb    Moved lcsum() from this file to ps_iputil.c
08/05/02    usb    Fixed get/set nv functions, moved them out of FEATURE_MIP
07/31/02    usb    Renamed the file from psmisc.c
06/14/02    usb    Removed byte manipulation functions.  Use dsbyte.h now.
04/17/02    rc     Wrapped code in !FEATURE_DATA_WCDMA_PS
12/21/01    dwp    Wrap get16 in !FEATURE_DATA_PS as it is defined else where
                   in MSM5200 archive.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
05/24/00    hcg    Added TARGET_OS_SOLARIS to compile with Solaris DSPE.
04/21/00    mvl    Fixed a #define so compiles properly under COMET
01/09/99    jjw    Changed to generic Browser interface
10/27/98    ldg    For T_ARM included C version of TCP checksum routine.
06/16/98    na     Converted the routine that calculates the TCP checksum
                   into 186 assembly.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
07/22/95    jjw    Created Initial Version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "ps_crit_sect.h"
#include "ps.h"
#include "rcinit_rex.h"

#ifndef FEATURE_WINCE
#include "task.h"
#endif /* FEATURE_WINCE */

#include "ps_utils.h"
#include "ps_utils_init.h"

#include "ran.h"
#include "qw.h"
#include "time_svc.h"

#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"
#include "ps_utils_defs.h"

#include <stringl/stringl.h>
#include "secapi.h"

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/
/*---------------------------------------------------------------------------
  Macro to prevent lint warning 818 'Symbol could be declared as pointing to
   const'
---------------------------------------------------------------------------*/
#ifndef PS_ARG_NOT_CONST
#define PS_ARG_NOT_CONST(arg) /*lint -save -e717 -e506 -e774 */ (arg)=(arg);/*lint -restore*/
#endif

/*===========================================================================

                      LOCAL DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  Command item to NV.
---------------------------------------------------------------------------*/
static nv_cmd_type  ps_nv_cmd_buf;
static nv_cmd_ext_type ps_nv_cmd_ext_buf;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*------------------------------------------------------------------------
  Table contain local Registered function for MCFS NV Refresh event.
-------------------------------------------------------------------------*/
ps_utils_refresh_reg_table   ps_utils_reg_clients;

#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

/*===========================================================================

                      DEFINITIONS FOR MODULE

===========================================================================*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
extern rex_tcb_type * rex_ps_tcb;
#endif
/*===========================================================================

FUNCTION PS_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged
  if status is other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other PS task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.

===========================================================================*/

nv_stat_enum_type ps_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ASSERT( rex_self() == rex_ps_tcb );
#else
  ASSERT( rex_self() == &ps_tcb );
#endif
  /*-------------------------------------------------------------------------
    Prepare command buffer to get the item from NV.
  -------------------------------------------------------------------------*/
  ps_nv_cmd_buf.cmd        = NV_READ_F;             /* Read request        */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ps_nv_cmd_buf.tcb_ptr    = rex_ps_tcb;            /* Notify this task    */
#else
  ps_nv_cmd_buf.tcb_ptr    = &ps_tcb;               /* Notify this task    */
#endif
  ps_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)PS_NV_CMD_SIGNAL; /* With this signal    */
  ps_nv_cmd_buf.done_q_ptr = NULL;             /* Do not enqueue when done */
  ps_nv_cmd_buf.item       = item_code;             /* Item to get         */
  ps_nv_cmd_buf.data_ptr   = data_ptr;              /* Where to return it  */

  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  PS_CLR_SIGNAL( PS_NV_CMD_SIGNAL );               /* Clear signal for NV  */
  nv_cmd( &ps_nv_cmd_buf );                        /* Issue the request    */
  (void)ps_wait((rex_sigs_type) 1 <<
                       (rex_sigs_type)PS_NV_CMD_SIGNAL ); /* Wait for completion*/

  if( ps_nv_cmd_buf.status != NV_DONE_S &&
      ps_nv_cmd_buf.status != NV_NOTACTIVE_S )
  {
    LOG_MSG_ERROR_2("ps_get_nv_item(): "
                    "NV Read Failed Item %d Code %d",
                    ps_nv_cmd_buf.item, ps_nv_cmd_buf.status );
  }
  return( ps_nv_cmd_buf.status );
} /* ps_get_nv_item() */


/*===========================================================================
FUNCTION PS_PUT_NV_ITEM

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged 
  if status is other than:
    NV_DONE_S       - request done

DEPENDENCIES
  This function can only be called from PS task.  Also it is not
  reentrant. Shouldn't be a problem, as it doesn't exit till we're done, and
  it's only called from the PS task.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_ERROR is logged 
  if status is other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other PS task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.

===========================================================================*/
nv_stat_enum_type ps_put_nv_item(
  nv_items_enum_type item_code,                              /* which item */
  nv_item_type *data_ptr                       /* pointer to data for item */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ASSERT( rex_self() == rex_ps_tcb );
#else
  ASSERT( rex_self() == &ps_tcb );
#endif

  /*-------------------------------------------------------------------------
    Prepare command buffer to write the item to NV.
    TODO:NV team to be notified to change the nv_cmd_buf.sigs to an array
  -------------------------------------------------------------------------*/
  ps_nv_cmd_buf.cmd        = NV_WRITE_F;            /* Write request       */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ps_nv_cmd_buf.tcb_ptr    = rex_ps_tcb;            /* Notify this task    */
#else
  ps_nv_cmd_buf.tcb_ptr    = &ps_tcb;               /* Notify this task    */
#endif
  ps_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)PS_NV_CMD_SIGNAL; /* With this signal    */
  ps_nv_cmd_buf.done_q_ptr = NULL;             /* Do not enqueue when done */
  ps_nv_cmd_buf.item       = item_code;             /* Item to put         */
  ps_nv_cmd_buf.data_ptr   = data_ptr;              /* Data to write       */


  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  PS_CLR_SIGNAL( PS_NV_CMD_SIGNAL );
  nv_cmd( &ps_nv_cmd_buf );
  (void)ps_wait( (rex_sigs_type) 1 <<
                        (rex_sigs_type)PS_NV_CMD_SIGNAL ); /* Wait for completion*/

  if( ps_nv_cmd_buf.status != NV_DONE_S )
  {
    LOG_MSG_ERROR_2("ps_put_nv_item(): "
                    "NV Write Failed Item %d Code %d",
                    ps_nv_cmd_buf.item, ps_nv_cmd_buf.status );
  }

  return( ps_nv_cmd_buf.status );
} /* ps_put_nv_item() */

/*=========================================================================== 
  FUNCTION  PS_UTILS_CONVERT_SUBS_PS_TO_SYS

  DESCRIPTION
    This function converts PS subs id to CM/System subs id.
  
  DEPENDENCIES
    None

  RETURN VALUE
    CM/SYS Subs ID

  SIDE EFFECTS
    None
===========================================================================*/
sys_modem_as_id_e_type ps_utils_convert_subs_ps_to_sys
(
  ps_sys_subscription_enum_type ps_subs_id
)
{
  sys_modem_as_id_e_type cm_subs_id = SYS_MODEM_AS_ID_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (ps_subs_id)
  {
    case PS_SYS_PRIMARY_SUBS:
      cm_subs_id = SYS_MODEM_AS_ID_1;
      break;
      
    case PS_SYS_SECONDARY_SUBS:
      cm_subs_id = SYS_MODEM_AS_ID_2;
      break;
      
    case PS_SYS_TERTIARY_SUBS:  
      cm_subs_id = SYS_MODEM_AS_ID_3;
      break;                  
      
    default:
      break;
  }
  LOG_MSG_INFO2_2("ps_utils_convert_subs_id(): "
    "PS subs id %d CM subs ID %d",ps_subs_id,cm_subs_id);
  
  return cm_subs_id;
}/*ps_utils_convert_subs_ps_to_sys*/

/*===========================================================================
  FUNCTION  PS_UTILS_CONVERT_SUBS_PS_TO_MCFG

  DESCRIPTION
    This function converts PS subs id to MCFG subs id.
  
  DEPENDENCIES
    None

  RETURN VALUE
    MCFG Subs ID

  SIDE EFFECTS
    None
===========================================================================*/
mcfg_fs_sub_id_e_type ps_utils_convert_subs_ps_to_mcfg
(
  ps_sys_subscription_enum_type ps_subs_id
)
{
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (ps_subs_id)
  {
    case PS_SYS_PRIMARY_SUBS:
      mcfg_subs_id = MCFG_FS_SUBID_0;
      break;
      
    case PS_SYS_SECONDARY_SUBS:
      mcfg_subs_id = MCFG_FS_SUBID_1;
      break;
      
    case PS_SYS_TERTIARY_SUBS:  
      mcfg_subs_id = MCFG_FS_SUBID_2;
      break;                  
      
    default:
      break;
  }
  
  return mcfg_subs_id;
}/*ps_utils_convert_subs_ps_to_sys*/

/*===========================================================================
  FUNCTION PS_GET_NV_ITEM_EXT 

  DESCRIPTION
    This function retrieves the specified item from NV. caller should call api with valid PS subs id

  DEPENDENCIES
    The NV task has been started and is running.
    This functions is Non-reentrant.

  RETURN VALUE
    Status returned from the NV read request.  An LOG_MSG_FATAL_ERROR is logged 
    if status is other than:
      NV_DONE_S       - request done
      NV_NOTACTIVE_S  - item was not active

  SIDE EFFECTS
    None

===========================================================================*/
extern nv_stat_enum_type ps_get_nv_item_ext 
(
  nv_items_enum_type             item_code,           /* Which item */
  nv_item_type                  *data_ptr,       /* Pointer to space for item */
  ps_sys_subscription_enum_type  ps_subs_id      /*PS Subscription ID's*/
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ASSERT( rex_self() == rex_ps_tcb );
#else
  ASSERT( rex_self() == &ps_tcb );
#endif
  /*-------------------------------------------------------------------------
        Prepare command buffer to get the item from NV.
     -------------------------------------------------------------------------*/

  ps_nv_cmd_buf.cmd        = NV_READ_F;             /* Read request        */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ps_nv_cmd_buf.tcb_ptr    = rex_ps_tcb;            /* Notify this task    */
#else
  ps_nv_cmd_buf.tcb_ptr    = &ps_tcb;               /* Notify this task    */
#endif
  ps_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)PS_NV_CMD_SIGNAL; /* With this signal    */
  ps_nv_cmd_buf.done_q_ptr = NULL;                  /* Do not enqueue when done */
  ps_nv_cmd_buf.item       = item_code;                  /* Item to get         */
  ps_nv_cmd_buf.data_ptr   = data_ptr;              /* Where to return it  */
    
  ps_nv_cmd_ext_buf.nvcmd   = &ps_nv_cmd_buf;

  ps_nv_cmd_ext_buf.context = ps_utils_convert_subs_ps_to_sys(ps_subs_id);
                                                    /* Index (Subs ID)return it  */
  
  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  
  PS_CLR_SIGNAL( PS_NV_CMD_SIGNAL );                      /* Clear signal for NV  */
  nv_cmd_ext(&ps_nv_cmd_ext_buf);                         /* Issue the request    */
  (void)ps_wait((rex_sigs_type) 1 <<
                       (rex_sigs_type)PS_NV_CMD_SIGNAL ); /* Wait for completion*/

  if( ps_nv_cmd_buf.status != NV_DONE_S &&
      ps_nv_cmd_buf.status != NV_NOTACTIVE_S )
  {
    LOG_MSG_ERROR_2("ps_get_nv_item(): "
                    "NV Read Failed Item %d Code %d",
                    ps_nv_cmd_buf.item, ps_nv_cmd_buf.status );
  }
  return( ps_nv_cmd_buf.status );
}/*ps_get_nv_item_ext*/

/*===========================================================================
  FUNCTION PS_PUT_NV_ITEM_EXT 

  DESCRIPTION
    Write an item to NV memory.  Wait until write is completed. caller should call api with valid PS subs id

  DEPENDENCIES
    This function can only be called from PS task.  Also it is not
    reentrant. Shouldn't be a problem, as it doesn't exit till we're done, and
    it's only called from the PS task.

  RETURN VALUE
    Status returned from the NV read request.  An LOG_MSG_FATAL_ERROR is logged 
    if status is other than:
      NV_DONE_S       - request done
      NV_NOTACTIVE_S  - item was not active

  SIDE EFFECTS
    None

===========================================================================*/
extern nv_stat_enum_type ps_put_nv_item_ext 
(
  nv_items_enum_type             item_code,           /* Which item */
  nv_item_type                  *data_ptr,       /* Pointer to space for item */
  ps_sys_subscription_enum_type  ps_subs_id      /*PS Subscription ID's*/
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ASSERT( rex_self() == rex_ps_tcb );
#else
  ASSERT( rex_self() == &ps_tcb );
#endif

  /*-------------------------------------------------------------------------
    Prepare command buffer to write the item to NV.
    TODO:NV team to be notified to change the nv_cmd_buf.sigs to an array
  -------------------------------------------------------------------------*/
  ps_nv_cmd_buf.cmd        = NV_WRITE_F;            /* Write request       */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ps_nv_cmd_buf.tcb_ptr    = rex_ps_tcb;            /* Notify this task    */
#else
  ps_nv_cmd_buf.tcb_ptr    = &ps_tcb;               /* Notify this task    */
#endif
  ps_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)PS_NV_CMD_SIGNAL; /* With this signal    */
  ps_nv_cmd_buf.done_q_ptr = NULL;                  /* Do not enqueue when done */
  ps_nv_cmd_buf.item       = item_code;             /* Item to put         */
  ps_nv_cmd_buf.data_ptr   = data_ptr;              /* Data to write       */
  
  ps_nv_cmd_ext_buf.nvcmd   = &ps_nv_cmd_buf;
  ps_nv_cmd_ext_buf.context =  ps_utils_convert_subs_ps_to_sys(ps_subs_id);
                                                    /* index (Subs ID) return it  */
  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  PS_CLR_SIGNAL( PS_NV_CMD_SIGNAL );
  nv_cmd_ext(&ps_nv_cmd_ext_buf);
  (void)ps_wait( (rex_sigs_type) 1 <<
                        (rex_sigs_type)PS_NV_CMD_SIGNAL ); /* Wait for completion*/

  if( ps_nv_cmd_buf.status != NV_DONE_S )
  {
    LOG_MSG_ERROR_2("ps_put_nv_item(): "
                    "NV Write Failed Item %d Code %d",
                    ps_nv_cmd_buf.item, ps_nv_cmd_buf.status );
  }
  
  return( ps_nv_cmd_buf.status );
}/*ps_put_nv_item_ext*/

/*===========================================================================

FUNCTION msclock

DESCRIPTION
  This function will return the time in milliseconds since ....

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

dword msclock( void)
{
  qword qw_time;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  time_get_ms( qw_time);

  return( qw_time[0]);

} /* msclock() */


/*===========================================================================
FUNCTION MEMDUMP()

DESCRIPTION
  Debug routine to dump memory to DM

PARAMETERS
  data_ptr -> address of memory to dump
  len      -> number of bytes to dump

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Memory access may go up to 7 bytes beyond last dumped byte, which could
  cause a data abort if dumping the last bytes of RAM.

===========================================================================*/
void memdump
(
  void * data_ptr,
  int    len
)
{
  char * data = (char *) data_ptr;
  int    i;                                                /* current byte */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2 ("memdump(): "
                   "Dumping %d bytes @ %x", len, data);

  for (i=0; i < len; i+= 8)
  {
    MSG_8( MSG_SSID_DFLT,
           MSG_LEGACY_MED,
           "%02x %02x %02x %02x %02x %02x %02x %02x",
           data[i], data[i+1], data[i+2], data[i+3],
           data[i+4], data[i+5], data[i+6], data[i+7]);
  }

} /* memdump() */

void ps_utils_generate_rand_num
(
  void    *rand_num,
  uint16  rand_len
)
{
  secerrno_enum_type  sec_errno = E_FAILURE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  sec_errno = secapi_get_random( SECAPI_PKCS_RANDOM,
                                 (uint8 *)rand_num,
                                 rand_len);
  if (sec_errno != E_SUCCESS)
  {
    ERR_FATAL("ps_utils_generate_rand_num(): "
              "secapi_get_random failed error: %d",
               sec_errno, 0, 0 );
  }

  return;
} /* ps_utils_generate_rand_num */

word ps_utils_generate_16bit_rand_num_by_range
(
  word  lo_val,
  word  hi_val
)
{
  dword random_num;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------ 
    Generate a 32 bit random number
  -------------------------------------------------------------------------*/
  ps_utils_generate_rand_num(&random_num, sizeof(uint32));

  return (ran_dist(random_num, lo_val, hi_val));
} /* ps_utils_generate_16bit_rand_num_by_range */

/*===========================================================================
FUNCTION PS_UTILS_GENERATE_RAND_64BIT_NUM()

DESCRIPTION
  This function generates a random 64 bit number.

PARAMETERS
  *random_num - Pointer to the 64 bit number to be returned by this function.

RETURN VALUE
  None

DEPENDENCIES
  ps_utils_generate_rand_num

SIDE EFFECTS
  None
===========================================================================*/
void ps_utils_generate_rand_64bit_num
(
  uint64 *rand_num                 /* Pointer to the 64bit num be returned */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ps_utils_generate_rand_num(rand_num, sizeof(uint64));

} /* ps_utils_generate_rand_64bit_num */
/*===========================================================================
FUNCTION PS_CREATE_EFS_CONFIG_FILE

DESCRIPTION
  Create a config file in EFS which stores the path of EFS item files.

DEPENDENCIES
  None

PARAMETERS
  conf_file_path - File path of config file

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  mcfg_fs_errno( file_type ) - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_create_efs_config_file
(
  const char *conf_file_path
)
{
  ps_sys_subscription_enum_type ps_subs_id = PS_SYS_DEFAULT_SUBS;
  return ps_create_efs_config_file_ext(conf_file_path,ps_subs_id);
} /* ps_create_efs_config_file() */

/*===========================================================================
FUNCTION PS_CREATE_EFS_CONFIG_FILE_EXT

DESCRIPTION
  Create a config file in EFS which stores the path of EFS item files.

DEPENDENCIES
  None

PARAMETERS
  conf_file_path - File path of config file
  ps_subs_id     - Subscription ID on which the
                   operation needs to be performed  

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  mcfg_fs_errno( file_type ) - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_create_efs_config_file_ext
(
  const char *conf_file_path,
  ps_sys_subscription_enum_type ps_subs_id
)
{
  int32                 config_fd, result;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == conf_file_path )
  {
    LOG_MSG_ERROR_0("ps_create_efs_config_file(): "
                    "conf_file_path is NULL");
    return -1;
  }
  
  mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);
  
  /*-------------------------------------------------------------------------
    Create common directories if needed.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_0("ps_create_efs_config_file(): "
                  "EFS: Creating conf file if necessary");
  result = ps_path_is_directory_ext("/nv", ps_subs_id);
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    LOG_MSG_INFO1_0("ps_create_efs_config_file(): "
                    "Create /nv dir in EFS");
    result = mcfg_mkdir( "/nv", S_IREAD|S_IWRITE|S_IEXEC, file_type);
    if ( -1 == result )
    {
      LOG_MSG_ERROR_1("ps_create_efs_config_file(): "
                      "Create EFS Dir Failed: error %d", mcfg_fs_errno( file_type ));
      return mcfg_fs_errno( file_type );
    } 
  }

  result = ps_path_is_directory_ext("/nv/item_files", ps_subs_id);
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    LOG_MSG_INFO1_0("ps_create_efs_config_file(): "
                    "Create /nv/item_files dir in EFS");
    result = mcfg_mkdir( "/nv/item_files", S_IREAD|S_IWRITE|S_IEXEC, file_type);
    if ( -1 == result )
    {
      LOG_MSG_ERROR_1("ps_create_efs_config_file(): "
                      "Create EFS Dir Failed: error %d", mcfg_fs_errno( file_type ));
      return mcfg_fs_errno( file_type );
    }
  }

  result = ps_path_is_directory_ext("/nv/item_files/conf", ps_subs_id);
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    LOG_MSG_INFO1_0("ps_create_efs_config_file(): "
                    "Create /nv/item_file/conf dir in EFS");
    result = mcfg_mkdir( "/nv/item_files/conf", S_IREAD|S_IWRITE|S_IEXEC, file_type);
    if ( -1 == result )
    {
      LOG_MSG_ERROR_1("ps_create_efs_config_file(): "
                      "Create EFS Dir Failed: error %d", mcfg_fs_errno( file_type ));
      return mcfg_fs_errno( file_type );
    }
  }

  /*-------------------------------------------------------------------------
    Open conf file. Create conf file if does not exist.
    Resulting file is truncated to zero bytes.
  -------------------------------------------------------------------------*/  
  config_fd = mcfg_fopen (conf_file_path, 
                          MCFG_FS_O_WRONLY|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC, 
                          MCFG_FS_ALLPERMS, file_type, 
                          mcfg_subs_id);
  if ( 0 > config_fd )
  {
    LOG_MSG_ERROR_1("ps_create_efs_config_file(): "
                    "Error creating config file, error %d", mcfg_fs_errno( file_type ));
    return mcfg_fs_errno( file_type );
  }

  result = mcfg_fclose (config_fd, file_type);
  if ( 0 != result )
  {
    LOG_MSG_ERROR_1("ps_create_efs_config_file(): "
                    "Error closing config file, error %d", mcfg_fs_errno( file_type ));
    return mcfg_fs_errno( file_type );
  }
  return 0;

} /* ps_create_efs_config_file_ext() */

/*===========================================================================
FUNCTION PS_CREATE_EFS_ITEM_FILE

DESCRIPTION
  Put(append and add newline) item_file_path into conf_file_path

DEPENDENCIES
  None

PARAMETERS
  conf_file_path - File path to a specific conf file
  item_file_path - File path to item in NV
  item_file_path_size - Size of item file path

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  mcfg_fs_errno( file_type ) - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_create_efs_item_file
(
  const char   *conf_file_path,
  char         *item_file_path, 
  fs_size_t     item_file_path_size
)
{
  ps_sys_subscription_enum_type ps_subs_id = PS_SYS_DEFAULT_SUBS;
  return ps_create_efs_item_file_ext(conf_file_path,
                                     item_file_path,
                                     item_file_path_size,
                                     ps_subs_id);
} /* ps_create_efs_item_file() */

/*===========================================================================
FUNCTION PS_CREATE_EFS_ITEM_FILE_EXT

DESCRIPTION
  Put(append and add newline) item_file_path into conf_file_path

DEPENDENCIES
  None

PARAMETERS
  conf_file_path - File path to a specific conf file
  item_file_path - File path to item in NV
  item_file_path_size - Size of item file path

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  mcfg_fs_errno( file_type ) - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_create_efs_item_file_ext
(
  const char   *conf_file_path,
  char         *item_file_path, 
  fs_size_t     item_file_path_size,
  ps_sys_subscription_enum_type ps_subs_id
)
{
  int32              config_fd, result;
  char              *file_loc;
  int32              ret_val = 0;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  if ( NULL == conf_file_path || NULL == item_file_path )
  {
    LOG_MSG_ERROR_0("ps_create_efs_item_file(): "
                    "conf_file_path or item_file_path is NULL");
    return -1;
  }

  mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);
  /*-------------------------------------------------------------------------
    Open conf file. If it doesn't exist create it and reopen it.
  -------------------------------------------------------------------------*/  
  config_fd = mcfg_fopen (conf_file_path, 
                          MCFG_FS_O_RDWR|MCFG_FS_O_APPEND, 
                          MCFG_FS_ALLPERMS,
                          file_type,
                          mcfg_subs_id);
  if ( 0 > config_fd )
  {
    if ( ENOENT == mcfg_fs_errno( file_type ) ) /* Conf file does not exist */
    {
      LOG_MSG_ERROR_0("ps_create_efs_item_file(): "
                      "EFS: Config file not present");
      LOG_MSG_INFO1_0("ps_create_efs_item_file(): "
                      "EFS: Creating config file");
      result = ps_create_efs_config_file_ext(conf_file_path, ps_subs_id);
      if ( 0 != result )
      {
        LOG_MSG_ERROR_1("ps_create_efs_item_file(): "
                        "EFS: Error creating conf file, error %d", mcfg_fs_errno( file_type ));
        return mcfg_fs_errno( file_type );
      }

      config_fd = mcfg_fopen (conf_file_path, 
                              MCFG_FS_O_RDWR|MCFG_FS_O_APPEND,
                              MCFG_FS_ALLPERMS,
                              file_type,
                              mcfg_subs_id);
      if ( 0 > config_fd )
      {
        LOG_MSG_ERROR_1("ps_create_efs_item_file(): "
                        "EFS: Error opening config file, error %d", mcfg_fs_errno( file_type ));
        return mcfg_fs_errno( file_type );
      }
    }
    else /* Could not open conf file for some other reason */
    {
      LOG_MSG_ERROR_1("ps_create_efs_item_file(): "
                      "Error opening config file, error %d", mcfg_fs_errno( file_type ));
      return mcfg_fs_errno( file_type );
    }
  }

  PS_SYSTEM_HEAP_MEM_ALLOC(file_loc, item_file_path_size + 1, char*);
  if (NULL == file_loc)
  {
    LOG_MSG_ERROR_0("ps_create_efs_item_file(): "
                    "Out of mem, can't create file");
    ret_val = -1;
    goto bail;
  }

  (void)strlcpy(file_loc, item_file_path, item_file_path_size + 1);
  file_loc[item_file_path_size] = '\n';
  result = mcfg_fwrite (config_fd, file_loc, (item_file_path_size + 1), file_type);
  if ( (item_file_path_size + 1) != result )
  {
    LOG_MSG_ERROR_1("ps_create_efs_item_file(): "
                    "Error writing into config file, error %d", mcfg_fs_errno( file_type ));
    ret_val = mcfg_fs_errno( file_type );
    goto bail;
  }

bail:  
  if (file_loc)
  {
    PS_SYSTEM_HEAP_MEM_FREE(file_loc);
  }

  result = mcfg_fclose(config_fd, file_type);
  if ( 0 != result )
  {
    LOG_MSG_ERROR_1("ps_create_efs_item_file(): "
                    "Error closing config file, error %d", mcfg_fs_errno( file_type ));
    return mcfg_fs_errno( file_type );
  }

  return ret_val;
} /* ps_create_efs_item_file_ext() */

/*===========================================================================
FUNCTION PS_READ_EFS_NV

DESCRIPTION
  This function reads the EFS item from the item file

DEPENDENCIES
  None

PARAMETERS
  item_file_path - File path to item in NV
  nv_info - Struct for NV item(s)
  nv_info_size - Size of NV item structure

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  mcfg_fs_errno( file_type ) - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_read_efs_nv
(
  const char   *item_file_path, 
  void         *nv_info_ptr, 
  fs_size_t    nv_info_size
)
{
  ps_sys_subscription_enum_type ps_subs_id = PS_SYS_DEFAULT_SUBS;
  return ps_read_efs_nv_ext(item_file_path,nv_info_ptr,nv_info_size,ps_subs_id);
} /* ps_read_efs_nv() */

/*===========================================================================
FUNCTION PS_READ_EFS_NV_EXT

DESCRIPTION
  This function reads the EFS item from the item file

DEPENDENCIES
  None

PARAMETERS
  item_file_path - File path to item in NV
  nv_info - Struct for NV item(s)
  nv_info_size - Size of NV item structure

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  mcfg_fs_errno( file_type ) - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_read_efs_nv_ext
(
  const char   *item_file_path,
  void         *nv_info_ptr,
  fs_size_t    nv_info_size,
  ps_sys_subscription_enum_type ps_subs_id
)
{
  int32                 retval = 0;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_0("ps_read_efs_nv_ext(): "
                  "Reading from EFS");

  if ( NULL == item_file_path || NULL == nv_info_ptr )
  {
    LOG_MSG_ERROR_0("ps_read_efs_nv(): "
                    "item_file_path is NULL or nv_info_ptr is NULL");
    return -1;
  }

  mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);

  memset (nv_info_ptr, 0, nv_info_size);

  /*-------------------------------------------------------------------------
    Read the item file from EFS into nv_info_ptr
  -------------------------------------------------------------------------*/
  retval = mcfg_fs_read( item_file_path, 
                    (void *)nv_info_ptr, 
                         nv_info_size,
                         file_type,
                         mcfg_subs_id); 

  if(MCFG_FS_STATUS_OK != retval ) /* Error or 0 bytes read */
  {
    LOG_MSG_ERROR_1("ps_read_efs_nv(): "
                    "Unable to read EFS item, error %d ", mcfg_fs_errno( file_type ));
    return mcfg_fs_errno( file_type );
  }

  return 0;
} /* ps_read_efs_nv_ext() */

/*===========================================================================
FUNCTION PS_WRITE_EFS_NV

DESCRIPTION
  This function writes the EFS item to the item file

DEPENDENCIES
  None

PARAMETERS
  item_file_path - File path to item in NV
  nv_info - Pointer to NV item
  nv_info_size - Size of NV item

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  mcfg_fs_errno( file_type ) - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_write_efs_nv
(
  const char   *item_file_path, 
  void         *nv_info_ptr, 
  fs_size_t    nv_info_size
)
{
  ps_sys_subscription_enum_type ps_subs_id = PS_SYS_DEFAULT_SUBS;
  return ps_write_efs_nv_ext(item_file_path,nv_info_ptr,nv_info_size,ps_subs_id);
} /* ps_write_efs_nv() */

/*===========================================================================
FUNCTION PS_WRITE_EFS_NV_EXT

DESCRIPTION
  This function writes the EFS item to the item file

DEPENDENCIES
  None

PARAMETERS
  item_file_path - File path to item in NV
  nv_info - Pointer to NV item
  nv_info_size - Size of NV item
  ps_subs_id - Ps Subs id on which efs operation
               needs to be performed.

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  mcfg_fs_errno( file_type ) - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_write_efs_nv_ext
(
  const char   *item_file_path, 
  void         *nv_info_ptr, 
  fs_size_t    nv_info_size,
  ps_sys_subscription_enum_type ps_subs_id
)
{
  int32                 result = 0;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_0("ps_write_efs_nv(): "
                  "Writing to EFS");

  if ( NULL == item_file_path || NULL == nv_info_ptr )
  {
    LOG_MSG_ERROR_0("ps_write_efs_nv(): "
                    "item_file_path is NULL or nv_info_ptr is NULL");
    return -1;
  }

  mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);

  /*-------------------------------------------------------------------------
    Write the item into the item file. If file or file path does not exist,
    create the file path and file
  -------------------------------------------------------------------------*/
  result = mcfg_fs_write( item_file_path,
                    (void *)nv_info_ptr, 
                    nv_info_size,
                          MCFG_FS_O_CREAT|O_AUTODIR, 
                          MCFG_FS_ALLPERMS,
                          file_type,
                          mcfg_subs_id);
  if( MCFG_FS_STATUS_OK != result )
  {
    LOG_MSG_ERROR_1("ps_write_efs_nv(): "
                    "Unable to write EFS item, error %d ", mcfg_fs_errno( file_type ));
    return mcfg_fs_errno( file_type );
  }

  return 0;
} /* ps_write_efs_nv_ext() */

/*===========================================================================
FUNCTION PS_PATH_IS_DIRECTORY

DESCRIPTION
  To check if the EFS directory exists

DEPENDENCIES
  None

PARAMETERS
  dirname - Directory path

RETURN VALUE
   0         - success
   mcfg_fs_errno( file_type ) - EFS error
   -1        - Other error

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_path_is_directory
(
  const char *dirname
)
{
  ps_sys_subscription_enum_type ps_subs_id = PS_SYS_DEFAULT_SUBS;
  return ps_path_is_directory_ext(dirname,ps_subs_id);
} /* ps_path_is_directory */

/*===========================================================================
FUNCTION PS_PATH_IS_DIRECTORY_EXT

DESCRIPTION
  To check if the EFS directory exists

DEPENDENCIES
  None

PARAMETERS
  dirname - Directory path

RETURN VALUE
   0         - success
   mcfg_fs_errno( file_type ) - EFS error
   -1        - Other error

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_path_is_directory_ext
(
  const char *dirname,
  ps_sys_subscription_enum_type ps_subs_id
)
{
  int                           rsp;
  struct fs_stat                stat_info;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);
  rsp = mcfg_fs_stat( dirname, &stat_info, file_type, mcfg_subs_id);
  if( MCFG_FS_STATUS_OK != rsp )
  {
    rsp = mcfg_fs_errno( file_type );
    if( ENOENT != rsp )
    {
      LOG_MSG_ERROR_1("ps_path_is_directory(): "
                      "efs_lstat error %d", rsp);
      return rsp;
    }
  }
  else if( S_ISDIR (stat_info.st_mode))
  {
    return 0;
  }

  return -1;
} /* ps_path_is_directory_ext */

/*===========================================================================
FUNCTION      PS_EFS_FILE_INIT

DESCRIPTION   The function intializes the state machine and 
              also opens the file

DEPENDENCIES  None.

RETURN VALUE   0  : SUCCESS: The file is good, readable, 
                             State Machine Initialized.
              -1 : FAILURE: The file cannot be opened/ readable. 

SIDE EFFECTS  None.
===========================================================================*/
int ps_efs_file_init
( 
  const char *file_path, 
  ps_efs_token_type *sm
)
{
  ps_sys_subscription_enum_type ps_subs_id = PS_SYS_DEFAULT_SUBS;
  return ps_efs_file_init_ext(file_path,sm,ps_subs_id);
} /* ps_efs_file_init() */

/*===========================================================================
FUNCTION      PS_EFS_FILE_INIT

DESCRIPTION   The function intializes the state machine and 
              also opens the file

DEPENDENCIES  None.

RETURN VALUE   0  : SUCCESS: The file is good, readable, 
                             State Machine Initialized.
              -1 : FAILURE: The file cannot be opened/ readable. 

SIDE EFFECTS  None.
===========================================================================*/
int ps_efs_file_init_ext
( 
  const char *file_path, 
  ps_efs_token_type *sm,
  ps_sys_subscription_enum_type ps_subs_id
)
{
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Sanity checks on input parameters
  -------------------------------------------------------------------------*/
  if((NULL == file_path) || (NULL == sm))
  {
    LOG_MSG_INFO1_0("ps_efs_file_init(): "
                    "Input parameters are NULL!");
    return -1;
  }
  
  mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);

  /*-------------------------------------------------------------------------
    Initialize the structure variables and open the file in read mode.
  -------------------------------------------------------------------------*/    
  sm->seperator   = ';';
  sm->curr        = sm->buffer;
  sm->end_pos     = sm->buffer;
  
  sm->eof         = FALSE;
  sm->eol         = FALSE;
  sm->skip_line   = FALSE;
  sm->bol         = TRUE;

  sm->fd = mcfg_fopen( file_path, 
                       MCFG_FS_O_RDONLY, 
                       MCFG_FS_ALLPERMS,
                       file_type,
                       mcfg_subs_id);
  if(sm->fd != -1) 
  {
    return 0;
  }
  else 
  {
    LOG_MSG_INFO1_0("ps_efs_file_init(): "
                    "Cannot open file");
    return -1;
  }
} /* ps_efs_file_init_ext() */

/*===========================================================================
FUNCTION      PS_EFS_FILE_CLOSE

DESCRIPTION   The function closes file and releases the state machine 

DEPENDENCIES  The file should have opened already.

RETURN VALUE  NONE

SIDE EFFECTS  None.
===========================================================================*/
void ps_efs_file_close
( 
  ps_efs_token_type *sm
)
{
  ps_sys_subscription_enum_type ps_subs_id = PS_SYS_DEFAULT_SUBS;
  ps_efs_file_close_ext(sm,ps_subs_id);
  return;
}

/*===========================================================================
FUNCTION      PS_EFS_FILE_CLOSE

DESCRIPTION   The function closes file and releases the state machine 

DEPENDENCIES  The file should have opened already.

RETURN VALUE  NONE

SIDE EFFECTS  None.
===========================================================================*/
void ps_efs_file_close_ext
( 
  ps_efs_token_type *sm,
  ps_sys_subscription_enum_type ps_subs_id
)
{
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(NULL == sm)
  {
    LOG_MSG_INFO1_0("ps_efs_file_close_ext(): "
                    "Input parameters are NULL!");
    return;
  }

  /* lint fix */
  PS_ARG_NOT_CONST(sm);
  (void) mcfg_fclose( sm->fd, file_type);
  return;
}

/*===========================================================================
FUNCTION      PS_EFS_TOKENIZER

DESCRIPTION   The is the function that reads data from the opened file.
              The data read is looked for tokens 
              1. Each token is seperated by ';'
              2. Successive ';' means empty token
              3. Line begining with '#' is comment
              4. '\n' is the end of token and record
              5. Empty line is ignored
              6. Insufficient tokens is a record is considered bad record
              
DEPENDENCIES  File should have already been opened.

RETURN VALUE   
              SUCCESS : Success => Found Token.
                        *begin points to the begining of token.
                        *end points to the end of token.
              EOL     : End of line is reached => record end 
                        => no token extracted
              END     : End of file is reached => file end => feof
                        => no token extracted
              FAILURE : Failed 

SIDE EFFECTS  None.
===========================================================================*/
ps_efs_token_parse_status_enum_type ps_efs_tokenizer
(
  ps_efs_token_type *sm,
  char **begin,
  char **end
)
{
  ps_sys_subscription_enum_type ps_subs_id = PS_SYS_DEFAULT_SUBS;
  return ps_efs_tokenizer_ext(sm,begin,end,ps_subs_id);
}/* ps_efs_tokenizer() */

/*===========================================================================
FUNCTION      PS_EFS_TOKENIZER_EXT

DESCRIPTION   The is the function that reads data from the opened file.
              The data read is looked for tokens 
              1. Each token is seperated by ';'
              2. Successive ';' means empty token
              3. Line begining with '#' is comment
              4. '\n' is the end of token and record
              5. Empty line is ignored
              6. Insufficient tokens is a record is considered bad record
              
DEPENDENCIES  File should have already been opened.

RETURN VALUE   
              SUCCESS : Success => Found Token.
                        *begin points to the begining of token.
                        *end points to the end of token.
              EOL     : End of line is reached => record end 
                        => no token extracted
              END     : End of file is reached => file end => feof
                        => no token extracted
              FAILURE : Failed 

SIDE EFFECTS  None.
===========================================================================*/
ps_efs_token_parse_status_enum_type ps_efs_tokenizer_ext
(
  ps_efs_token_type *sm,
  char **begin,
  char **end,
  ps_sys_subscription_enum_type ps_subs_id
)
{
  int bytes_read = 0;
  char *dummy;
  ps_efs_token_parse_status_enum_type retval;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((NULL == sm) || (NULL == begin) || (NULL == end))
  {
    LOG_MSG_ERROR_0("ps_efs_tokenizer(): "
                    "Input paramters are NULL");
    return PS_EFS_TOKEN_PARSE_FAILURE;
  }
 
  *begin = 0;
  *end   = 0; 
  /*------------------------------------------------------------x------------
     Traversed to end of file => return 
  ---------------------------------------------------------------------------*/
  if( sm->eof ) 
  {
    return PS_EFS_TOKEN_PARSE_EOF;
  }
   
  /*------------------------------------------------------------------------
     Have some bytes to read from the buffer
  ---------------------------------------------------------------------------*/
  while( sm->curr < sm->end_pos ) 
  {

    /*----------------------------------------------------------------------
      Skip over all carriage return characters (\r) added if file was
      editted using a windows machine
    -----------------------------------------------------------------------*/

    if (*sm->curr == '\r')         
    {
      sm->skip_line = FALSE;
      sm->curr++;
      continue;
    }

    /*-----------------------------------------------------------------------
       Lines begining the record with # are comments. Continue to read 
       until we reach the end of file.
    -----------------------------------------------------------------------*/
    if( sm->bol && *sm->curr ==  '#' ) 
    {
      sm->skip_line = TRUE;
      sm->bol = FALSE;
      sm->curr++;
      continue;
    } 

    if( sm->skip_line )                 /* reading a comment */
    {
      if( *sm->curr == '\n' )           /* End of comment */
      {  
        sm->skip_line = FALSE;
        sm->eol = TRUE;
        sm->bol = TRUE;
      }
      sm->curr++;
      continue;                         /*Continue to read until the end of line */
    }
    
    /*--------------------------------------------------------------------------
      Look for the token. If ';' found at the begining then it is 
      an empty token.
      There could be a  case where we hit '\n' while we are looking for a token
      so skip over all the new lines.
    ----------------------------------------------------------------------------*/
    if( *begin == 0 )                   /* Find the beginning of token */
    {                          
      if( *sm->curr == sm->seperator )  /* an empty token */
      {                             

        if( sm->bol == TRUE ) 
        {
          sm->bol = FALSE;
        }
        
        *begin = sm->curr;
        *end   = sm->curr;
        sm->curr++;
        return PS_EFS_TOKEN_PARSE_SUCCESS;
      }

      if( *sm->curr == '\n' )           /* Possibly an empty token */
      {    
        if( sm->eol )                   /* Skip over any successive new lines */
        {     
          sm->curr++;
          continue;
        }
        *begin  = sm->curr;
        *end    = sm->curr;
        sm->eol = TRUE;
        sm->bol = TRUE;
        sm->curr++;
        return PS_EFS_TOKEN_PARSE_SUCCESS;
      }

      /*-------------------------------------------------------------------------
       Before beginning a new token, return an end of record for previous record. 
      --------------------------------------------------------------------------*/
      if( sm->eol ) 
      {                             
        sm->eol = FALSE;
        return PS_EFS_TOKEN_PARSE_EOL;
      }

      *begin = sm->curr;                /* Initialize to beginning of token */
    }
    else if( *sm->curr == sm->seperator || *sm->curr == '\n' )
    {
      *end = sm->curr++;                /* Found end of token */
      
      /*--------------------------------------------------------------------------
         This is a end of line. Save the state and send 
         end of line event when a next token is requested .
      --------------------------------------------------------------------------*/
      if( **end == '\n' ) 
      {       
        sm->eol = TRUE;
        sm->bol = TRUE;
      }
      return PS_EFS_TOKEN_PARSE_SUCCESS;
    }
    
    sm->curr++;
  }/* while */

  /*-------------------------------------------------------------------------- 
    In the middle of token and we ran out characters in the buffer 
  --------------------------------------------------------------------------*/
  if( *begin ) 
  {      
    
    if( *begin != sm->buffer )
    {
      /*---------------------------------------------------------------------- 
        Move the partial token over to beginning of buffer 
      -----------------------------------------------------------------------*/
      /*lint -e732 */
      memscpy( sm->buffer, PS_EFS_READ_BUFFER_SZ*sizeof(char),
               *begin, (sm->curr - *begin) );
      /*lint +e732 */
      sm->curr = sm->buffer + (sm->curr - *begin);
      *begin = sm->buffer;
    }
    else 
    {
      LOG_MSG_INFO1_0("ps_efs_tokenizer(): "
                      "Token is larger than PS_EFS_READ_BUFFER_SZ");
      return PS_EFS_TOKEN_PARSE_FAILURE;
    }
  }
  else 
  {
    /*--------------------------------------------------------------------
      No token or data exists in the buffer 
    ---------------------------------------------------------------------*/
    sm->curr = sm->buffer;
  }
  
  /*----------------------------------------------------------------------
      Read data from the efs file.
  -----------------------------------------------------------------------*/
  {
    /*lint -e732 */
    bytes_read = mcfg_fread( sm->fd, sm->curr, 
                             PS_EFS_READ_BUFFER_SZ - (sm->curr - sm->buffer),
                             file_type);
    /*lint +e732 */
    
    if( bytes_read > 0 ) 
    {
      sm->end_pos = sm->curr + bytes_read;
      sm->eof    = FALSE;
      
      if(*begin != 0)
      {
        retval= ps_efs_tokenizer_ext( sm, &dummy, end, ps_subs_id ); /* Call the function 
                                              again because you could be in the
                                              middle of reading a token */
      }
      else
      {
        retval = ps_efs_tokenizer_ext( sm, begin, end, ps_subs_id);
      }

      return retval;
    }
    else 
    {

      /*
        No bytes read => reached the end of file.
      */
      if(*begin == 0) 
      {
        sm->eof = 1;
        return PS_EFS_TOKEN_PARSE_EOL;
      }
      else
      {

        /*------------------------------------------------------------------
          If a token was found return the token and 
          when next token is requested send EOF 
        --------------------------------------------------------------------*/
        *end = sm->curr;
        if(bytes_read == 0)
        {
          /*---------------------------------------------------------------
           If the EOF character is missing in the file and the bytes read
           are zero all the time then we are trying to bail out of this.
           
           NOTE: We might have to revisit this later again if different 
           modules
           
          ----------------------------------------------------------------*/
          sm->eof = 1;
          return PS_EFS_TOKEN_PARSE_EOL;
        }
        return PS_EFS_TOKEN_PARSE_SUCCESS;
      }      
    }
  }/* End of bytes read*/
}/* ps_efs_tokenizer_ext() */

/** ===========================================================================
FUNCTION PS_CFG_GET_FILE

DESCRIPTION
  Returns the file descriptor for the nv file path supplied
  
PARAMETERS  
  config_fd       - pointer to file descriptor
  config_path   -  pointer to buffer to that has the path name of efs file
  flag_new       - pointer to flag storing file status
  ps_subs_id     - subscription id on which the EFS
                   operations needs to be performed
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0 Success
  -1 / non zero on failure
  
SIDE EFFECTS 
  None.
===========================================================================*/
static int32 ps_conf_get_file
(
  int32    * config_fd,
  char     * config_path,
  boolean  * flag_new,
  ps_sys_subscription_enum_type ps_subs_id
)
{
  int32 ret_val = 0;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
  /*--------------------------------------------------------------------
    Create a file if file does not exist
   --------------------------------------------------------------------*/
  
  mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);

  *config_fd = mcfg_fopen (config_path, 
                           MCFG_FS_O_RDWR, 
                           MCFG_FS_ALLPERMS,
                           file_type,
                           mcfg_subs_id);

  if ( 0 > *config_fd )
  {
    if ( ENOENT == mcfg_fs_errno( file_type ) ) /* Conf file does not exist */
    {
      LOG_MSG_INFO2_0("ps_cfg_get_file_ext(): EFS: Config file not present");
      ret_val = ps_create_efs_config_file_ext(config_path, ps_subs_id);
      if ( 0 != ret_val )
      {
        LOG_MSG_INFO2_1("ps_cfg_get_file_ext(): EFS: Error creating config file "
                        "result %d", ret_val);
        return ret_val;
      }
      *flag_new = TRUE;

      *config_fd = mcfg_fopen (config_path,
                               MCFG_FS_O_RDWR, 
                               MCFG_FS_ALLPERMS,
                               file_type,
                               mcfg_subs_id);
      if ( 0 > *config_fd )
      {
        LOG_MSG_INFO2_1("ps_cfg_get_file_ext(): EFS: Error opening config file, "
                        "config_fd %d", config_fd);
        ret_val = *config_fd;
        return ret_val;
      }
    }
    else /* Could not open conf file for some other reason */
    {
      LOG_MSG_INFO2_0("ps_cfg_get_file_ext(): Error opening config file, ");
      return -1;
    }
  }
  
  return ret_val;
}

/** =========================================================================
FUNCTION PS_EFS_GET_CONF_STRING

DESCRIPTION
  Returns the file descriptor for the nv file path supplied.
  
PARAMETERS  
  ps_cfg_item_info - array of pointers to nv item file paths.
  file_path_buf    -  pointer to buffer to that will store the
  string.

DEPENDENCIES 
  None.
  
RETURN VALUE 
  int32 - number of bytes in config string
  
SIDE EFFECTS 
  None.
===========================================================================*/

static int32 ps_efs_get_conf_string
(
  const char * ps_nv_conf_item_info[],
  char * file_path_buf,
  int ps_efs_conf_items
)
{
  uint32 position = 0;
  int32 index;
  uint32 file_path_buf_len = 0;
  /* - - -- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( file_path_buf == NULL )
  {
    LOG_MSG_ERROR_1 ("ps_efs_get_conf_string() : Failed to allocate memory",
                     	 0);
    return -1;
  }
  
  file_path_buf[0] = '\0';
  file_path_buf_len = ps_efs_conf_items * PS_NV_CONF_MAX_PATH_SIZE;
  /*---------------------------------------------------------------------
    Update data_config_info.conf with the list of nv item files.
  ----------------------------------------------------------------------*/
  /* Browse through ps_cfg_item_info and copy all items in local
     buffer to be written to efs in one shot */
  for (index = 0; index < ps_efs_conf_items; index ++)
  {
    /* Copy current path to buffer and append \n */
    position += snprintf( file_path_buf + position, 
                          file_path_buf_len - position,
                          "%s\n", 
                          ps_nv_conf_item_info[index] );		
    /* Check for truncation */
    if( position >= file_path_buf_len)
    {
      LOG_MSG_ERROR_1 ("Position %d exceeds max length", file_path_buf_len);
      return -1;
    }							
  }

  return position;
}

/** =========================================================================
  FUNCTION PS_EFS_WRITE_ITEMS
  
  DESCRIPTION
    Puts all the required nv item file paths in the passed file.
    
  PARAMETERS  
    data_config_info_fd: data_config_info file descriptor.
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    0 for success.
    -1 for failure.
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/
static int32 ps_efs_write_items
(
  char * data_conf_path,
  const char * ps_nv_conf_item_info[],
  int ps_efs_conf_items,
  int max_path_size,
  ps_sys_subscription_enum_type ps_subs_id
)
{
  int32  conf_fd, conf_info_size = 0;
  fs_ssize_t bytes_read           = 0;
  boolean flag_new_file           = FALSE;
  int efs_ret                     = 0;
  char *file_read_buf             = NULL;
  char *file_path_buf             = NULL;
  int32 ret_val                   = 0;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (NULL == ps_nv_conf_item_info)
  {
    LOG_MSG_ERROR_0("ps_efs_write_items(): Failed to get ps_cfg_item_info");
    return -1; 
  }
  
  if (NULL == data_conf_path)
  {
    LOG_MSG_ERROR_0("ps_efs_write_items(): Failed to get file path");
    return -1; 
  }

  do {
    /*------------------------------------------------------------
      Get file descriptor for given nv file path
    -------------------------------------------------------------*/
  
    ret_val = ps_conf_get_file( &conf_fd, data_conf_path, &flag_new_file, ps_subs_id );
  
    if ( 0 != ret_val )
    {
      LOG_MSG_ERROR_1("ps_efs_write_items(): Failed to get file %d", ret_val);
      ret_val = -1;
      break;
    }

    /* Allocate memory for file_read_buf */
    PS_SYSTEM_HEAP_MEM_ALLOC(file_path_buf, ps_efs_conf_items*max_path_size,
                             char*);

    if( file_path_buf == NULL )
    {
      LOG_MSG_ERROR_0("ps_efs_write_items(): Failed to allocate"
                      "file_path_buf memory");
      ret_val = -1;
      break;
    }
  
    /* Get all cfg data to be written */
    conf_info_size = ps_efs_get_conf_string( ps_nv_conf_item_info, 
                                             file_path_buf, 
                                             ps_efs_conf_items);

    if( 0 == conf_info_size )
    {
      LOG_MSG_ERROR_0("ps_efs_write_items(): Failed to read cfg data");
      ret_val = -1;
      break;
    }

    /* Allocate memory for file_read_buf */
    PS_SYSTEM_HEAP_MEM_ALLOC(file_read_buf, conf_info_size, char*);

    if (file_read_buf == NULL )
    {
      LOG_MSG_ERROR_0("ps_efs_write_items(): Failed to allocate "
                      "file_read_buf memory");
      ret_val = -1;
      break;
    }

    /* Initialize buffers */
    memset(file_read_buf, 0, conf_info_size);

    /* Read file if already existed */

    if (FALSE == flag_new_file)
    {
      efs_ret = mcfg_fread(conf_fd, file_read_buf, conf_info_size, file_type);
      bytes_read = efs_ret;

      if( efs_ret < 0 )
      {
        LOG_MSG_ERROR_0("ps_efs_write_items(): Error reading file:"  );
      }  
    }

    /* Write to file if file existed and length is different or its a new file */

    if ((FALSE == flag_new_file && (bytes_read != conf_info_size))
        || TRUE == flag_new_file ) 
    {
      /* Write the local buffer in efs memory */
      if( (fs_size_t)conf_info_size != mcfg_fwrite(conf_fd,
                                                file_path_buf,
                                                   (fs_size_t)conf_info_size,
                                                   file_type))
      {
        LOG_MSG_ERROR_1("ps_efs_write_items(): mcfg_fwrite error, bytes: %d", conf_info_size);
          ret_val = -1;
      }
    }    
  } while (0);

  /* Free memory for file_paths_buf */
  PS_SYSTEM_HEAP_MEM_FREE( file_read_buf );
  PS_SYSTEM_HEAP_MEM_FREE( file_path_buf );
  
  /* EFS Close */
  efs_ret = mcfg_fclose (conf_fd, file_type);
  if ( 0 != efs_ret )
  {
    LOG_MSG_ERROR_1("ps_efs_write_items(): Error closing config file "
                    "result %d", ret_val);
    ret_val = efs_ret;
  }

  return ret_val;
}

/** ========================================================================
  FUNCTION PS_EFS_INIT
  
  DESCRIPTION
    Initialization function for creating NV item file paths in EFS under
    ps_config_info.conf. The function will put all the required 
    ps nv item file paths in efs. The required ps nv item file paths
    are specified in ps_utils_nv_conf_path_array and reference using the 
    indicies from ps_utils_nv_conf_enum_type
    
  PARAMETERS  
    None
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    None
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/
void ps_efs_init_ext
(
  ps_sys_subscription_enum_type ps_subs_id
)
{
  int ps_efs_conf_items = 0;
   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  LOG_MSG_INFO2_0("ps_efs_init(): Initializing EFS config item files");

  ps_efs_conf_items = sizeof(ps_nv_conf_efs_path_array ) / sizeof( *ps_nv_conf_efs_path_array );

  if (0 !=  ps_efs_write_items(PS_NV_CONF_EFS_FILE_NAME, 
                               ps_nv_conf_efs_path_array,
                               ps_efs_conf_items,
                               PS_NV_CONF_MAX_PATH_SIZE,
                               ps_subs_id))

  {
    LOG_MSG_ERROR_0("ps_efs_init(): Item files write failed.");
  }

}

/** ========================================================================
  FUNCTION PS_EFS_INIT
  
  DESCRIPTION
    Initialization function for creating NV item file paths in EFS under
    ps_config_info.conf. The function will put all the required 
    ps nv item file paths in efs. The required ps nv item file paths
    are specified in ps_utils_nv_conf_path_array and reference using the 
    indicies from ps_utils_nv_conf_enum_type
    
  PARAMETERS  
    None
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    None
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/
void ps_efs_init
(
  void
)
{
  ps_sys_subscription_enum_type ps_subs_id = PS_SYS_DEFAULT_SUBS;
  ps_efs_init_ext(ps_subs_id);
  return;
}

#ifdef FEATURE_MODEM_CONFIG_REFRESH

/*=========================================================================== 
  FUNCTION  PS_UTILS_CONVERT_SUBS_MCFG_TO_PS

  DESCRIPTION
    This function converts mcfg subs index to ps subs id.
  
  DEPENDENCIES
    None

  RETURN VALUE
    PS Subs ID

  SIDE EFFECTS
    None
===========================================================================*/
ps_sys_subscription_enum_type ps_utils_convert_subs_sys_to_ps
(
  mcfg_refresh_index_type subs_index
)
{
  ps_sys_subscription_enum_type subs_id = PS_SYS_SUBS_MAX;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (subs_index)
  {
/*SYS_MODEM_AS_ID_1*/
    case MCFG_REFRESH_INDEX_0:
      subs_id = PS_SYS_PRIMARY_SUBS;
      break;
      
/*SYS_MODEM_AS_ID_2*/
    case MCFG_REFRESH_INDEX_1:
      subs_id = PS_SYS_SECONDARY_SUBS;
      break;
      
/*SYS_MODEM_AS_ID_3*/
    case MCFG_REFRESH_INDEX_2:  
      subs_id = PS_SYS_TERTIARY_SUBS;
      break;                  
      
    default:
      break;
  }
  LOG_MSG_INFO2_2("ps_utils_convert_subs_id(): "
    "MCFG subs index %d PS subs ID %d",subs_index,subs_id);
  
  return subs_id;
}/*ps_utils_convert_subs_sys_to_ps*/

/*=========================================================================== 
  FUNCTION  PS_UTILS_CONVERT_SLOT_MCFG_TO_PS

  DESCRIPTION
    This function converts mcfg slot index to PS slot id.
  
  DEPENDENCIES
    None

  RETURN VALUE
    PS slot ID

  SIDE EFFECTS
    None
===========================================================================*/
ps_utils_refresh_slot_e_type ps_utils_convert_slot_mcfg_to_ps
(
  mcfg_refresh_index_type slot_index
)
{
  ps_utils_refresh_slot_e_type slot_id = PS_UTILS_REFRESH_SLOT_MAX;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (slot_index)
  {
/*MMGSDI_SLOT_1*/
    case MCFG_REFRESH_INDEX_0:
      slot_id = PS_UTILS_REFRESH_SLOT_1;
      break;
      
/*MMGSDI_SLOT_2*/
    case MCFG_REFRESH_INDEX_1:
      slot_id = PS_UTILS_REFRESH_SLOT_2;
      break;
      
/*MMGSDI_SLOT_3*/
    case MCFG_REFRESH_INDEX_2:  
      slot_id = PS_UTILS_REFRESH_SLOT_3;
      break;                  
      
    default:
      break;
  }
  
  LOG_MSG_INFO2_2("ps_utils_convert_slot_id(): "
    "MCFG slot index %d PS slot ID %d",slot_index,slot_id);
  
  return slot_id;
}/*ps_utils_convert_slot_mcfg_to_ps*/
/*=========================================================================== 
  FUNCTION  PS_UTILS_CONVERT_REFRESH_TYPE_MCFG_TO_PS

  DESCRIPTION
     This function converts mcfg refresh type to PS refresh type.
  
  DEPENDENCIES
    None

  RETURN VALUE
    PS refresh type

  SIDE EFFECTS
    None
===========================================================================*/
ps_utils_refresh_type_e_type ps_utils_convert_refresh_type_mcfg_to_ps
(
  mcfg_refresh_type_e_type mcfg_refresh_type
)
{
  ps_utils_refresh_type_e_type ps_refresh_type = PS_UTILS_REFRESH_TYPE_MAX;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (mcfg_refresh_type)
  {
    case MCFG_REFRESH_TYPE_SLOT:
      ps_refresh_type = PS_UTILS_REFRESH_TYPE_SLOT;
      break;
      
    case MCFG_REFRESH_TYPE_SUBS:
      ps_refresh_type = PS_UTILS_REFRESH_TYPE_SUBS;
      break;
      
    case MCFG_REFRESH_TYPE_SLOT_N_SUBS:  
      ps_refresh_type = PS_UTILS_REFRESH_TYPE_SLOT_N_SUBS;
      break;                  
      
    default:
      break;
  }
  
  LOG_MSG_INFO2_2("ps_utils_convert_refresh_type_mcfg_to_ps(): "
    "MCFG type %d PS type %d",mcfg_refresh_type,ps_refresh_type);
  
  return ps_refresh_type;
}/*ps_utils_convert_refresh_type_mcfg_to_ps*/

/*===========================================================================

  FUNCTION  PS_UTILS_MCFG_REFRESH_IND_CB

  DESCRIPTION
    This function is the callback to process mcfg refresh ind.
    CB returns TRUE- MCFG will wait for DONE api.
    CB returns FALSE- MCFG will not wait for DONE api
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

boolean ps_utils_mcfg_refresh_ind_cb
(
  mcfg_refresh_info_s_type *mcfg_refresh_info_ptr
)
{
  mcfg_refresh_info_s_type     *event_info = NULL;
   
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
      Return FLASE so MCFG will not wait for DONE 
  -------------------------------------------------------------------------*/
  if (NULL == mcfg_refresh_info_ptr)
  {
    LOG_MSG_ERROR_0("ps_utils_mcfg_refresh_ind_cb(): NULL refresh pointer");
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
     Allocate buffer for NV refresh event
    -----------------------------------------------------------------------*/
  event_info = ps_system_heap_mem_alloc( sizeof(mcfg_refresh_info_s_type) );

  if(NULL == event_info)
  {
    LOG_MSG_ERROR_0("ps_utils_mcfg_refresh_ind_cb(): Allocation failed");
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
    Copy the refresh info
  -----------------------------------------------------------------------*/
  (void) memscpy(event_info,
                 sizeof(mcfg_refresh_info_s_type),
                 mcfg_refresh_info_ptr,
                 sizeof(mcfg_refresh_info_s_type));
  
 /*-------------------------------------------------------------------------
    Post a command to PS task to handle this event
  -------------------------------------------------------------------------*/
  ps_send_cmd(PS_UTILS_NV_REFRESH_EVENT_CMD, event_info );

  return TRUE;
}/*ps_utils_mcfg_refresh_ind_cb*/

/*===========================================================================
  FUNCTION: 
  PS_UTILS_MCFG_REFRESH_IND_HANDLER()

  DESCRIPTION:
    This function cmd handler to process NV refresh events

  INPUT/OUTPUT PARAMETERS:
    cmd - PS command ID
    user_data_ptr - User data provided by the caller routine

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
static void ps_utils_mcfg_refresh_ind_handler
(
  ps_cmd_enum_type    cmd,
  void              * data_ptr
)
{
  uint8                         client_count      = 0;
  mcfg_refresh_info_s_type     *mcfg_event_info   = NULL;
  ps_utils_status_e_type        result            = PS_UTILS_IND_SUCCESS;
  ps_utils_refresh_notify_info  ps_refresh_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if (NULL == data_ptr)
    {
      result = PS_UTILS_IND_FAILURE_NULL_USER_DATA;
      break;
    }

    mcfg_event_info = (mcfg_refresh_info_s_type  *) data_ptr;
    
    memset(&ps_refresh_info ,0x0 ,sizeof(ps_utils_refresh_notify_info));
    
    /*-------------------------------------------------------------------------
        MCFG refresh info conversion
     -------------------------------------------------------------------------*/
    ps_refresh_info.refresh_type =
            ps_utils_convert_refresh_type_mcfg_to_ps(mcfg_event_info->type);
    ps_refresh_info.refresh_slot =
            ps_utils_convert_slot_mcfg_to_ps(mcfg_event_info->slot_index);
    ps_refresh_info.refresh_subs =
            ps_utils_convert_subs_sys_to_ps(mcfg_event_info->sub_index);
    
    /*-------------------------------------------------------------------------
        Internal Clients reporting
     -------------------------------------------------------------------------*/
    if (TRUE == ps_utils_reg_clients.is_initialized)
    {
      
      for (client_count = 0 ;client_count < PS_UTILS_MAX_ALLOWED_NV_REFRESH_REG &&
                   client_count < ps_utils_reg_clients.total_reg_count ;client_count++)
      {
        ps_refresh_info.user_data_ptr = 
          ps_utils_reg_clients.clients_reg_data[client_count].user_data_ptr;
        
        if (NULL != ps_utils_reg_clients.clients_reg_data[client_count].func_ptr)
        {
          ps_utils_reg_clients.clients_reg_data[client_count].func_ptr(ps_refresh_info);
          
          LOG_MSG_INFO2_1("ps_utils_mcfg_refresh_ind_handler() :"
                          "refresh reported to client 0x%x",
                           ps_utils_reg_clients.clients_reg_data[client_count].func_ptr);
          
        }
      }
    }
    /*-------------------------------------------------------------------------
      Refresh done reporting to MCFG
    -------------------------------------------------------------------------*/
    if (FALSE == mcfg_refresh_done(mcfg_event_info->reference_id))
    {
      result = PS_UTILS_IND_FAILURE_DONE_CNF;
    }
    /*-------------------------------------------------------------------------
      Freed MCFG refresh event memory allocated in event call back
    -------------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_FREE(data_ptr);
    
  }while(0);
  LOG_MSG_INFO2_1("ps_utils_mcfg_refresh_ind_handler():"
                  " result %d ",result);
    
  return;
}

/*===========================================================================

  FUNCTION  PS_UTILS_REFRESH_INIT

  DESCRIPTION
    MCFG Interface initialization function. This function does the following:
    - Initializes Internal client table and critical section.
    - Registers with the MCFG for refresh event.
  
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ps_utils_refresh_init ( void )
{

  mcfg_refresh_registration_s_type mcfg_refresh_reg;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Internal clients registration initialization
  -------------------------------------------------------------------------*/

  memset(&ps_utils_reg_clients, 0, sizeof(ps_utils_reg_clients));
  
 /*-------------------------------------------------------------------------
      PS Event registration
   -------------------------------------------------------------------------*/
  ps_set_cmd_handler(PS_UTILS_NV_REFRESH_EVENT_CMD,
                         ps_utils_mcfg_refresh_ind_handler);
    
  /*-------------------------------------------------------------------------
    Refresh registration initalization
   -------------------------------------------------------------------------*/
  memset(&mcfg_refresh_reg, 0, sizeof(mcfg_refresh_registration_s_type));
  
  /*-------------------------------------------------------------------------
    Register with normal(default) priority for all slots and subs
   -------------------------------------------------------------------------*/
  mcfg_refresh_reg.order       = MCFG_REFRESH_ORDER_50;
  mcfg_refresh_reg.slot_mask   = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_refresh_reg.sub_mask    = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_refresh_reg.cb          = ps_utils_mcfg_refresh_ind_cb;

  /*-------------------------------------------------------------------------
       Registers with the MCFG for refresh event
   -------------------------------------------------------------------------*/
  if (TRUE == mcfg_refresh_register(&mcfg_refresh_reg))
  {
    
    ps_utils_reg_clients.is_initialized = TRUE;
  }
  
  LOG_MSG_INFO2_1("ps_utils_refresh_init(): Init Complete is %d",
                   ps_utils_reg_clients.is_initialized );
  
}/*ps_utils_refresh_init*/
/*===========================================================================

  FUNCTION PS_UTILS_REFRESH_REG

  DESCRIPTION
    Internal clients (with in PS task only) register with Framework for refresh notification whenever SIM swap/Subs swap. 
    Internal clients should provide a valid CB function to Framework for notifications. 

  DEPENDENCIES
   None

  RETURN VALUE
    PS_UTIL_REG_FAILURE    - Registration failed
    PS_UTIL_REG_SUCCESS  - Registration done

  SIDE EFFECTS
    None.

===========================================================================*/
ps_utils_status_e_type ps_utils_refresh_reg
(
  ps_utils_refresh_reg_info        reg_info
)
{
  ps_utils_status_e_type ret_val              = PS_UTILS_REG_SUCCESS;
  uint8                  internal_clients     = 0;
  uint8                  client_count         = 0;
  uint8                  is_client_registered = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-------------------------------------------------------------------------
      This function can only be called from PS task
     -------------------------------------------------------------------------*/
#ifndef TEST_FRAMEWORK

#ifdef FEATURE_MODEM_RCINIT_PHASE2
    if(rex_self() != rex_ps_tcb)
    {
      ret_val  = PS_UTILS_REG_FAILURE_OUTSIDE_PS_TASK;
      break;
    }
#else
    if(rex_self() != &ps_tcb )
    {
      ret_val  = PS_UTILS_REG_FAILURE_OUTSIDE_PS_TASK;
      break;
    }
#endif /*FEATURE_MODEM_RCINIT_PHASE2*/

#endif /*TEST_FRAMEWORK*/
    /*-------------------------------------------------------------------------
      Framework should initialize before accepting registration from internal clients.
    -------------------------------------------------------------------------*/
    if (TRUE != ps_utils_reg_clients.is_initialized)
    {
      ret_val  = PS_UTILS_REG_FAILURE_INIT_NOT_COMPLETE;
      break;
    }
  
    /*-------------------------------------------------------------------------
      Internal clients should register with valid CB.
    -------------------------------------------------------------------------*/
   if (NULL == reg_info.func_ptr)
   {
      ret_val =  PS_UTILS_REG_FAILURE_NULL_CB;
      break;
   }
  
   if (ps_utils_reg_clients.total_reg_count < PS_UTILS_MAX_ALLOWED_NV_REFRESH_REG)
   {
      internal_clients = ps_utils_reg_clients.total_reg_count;
      /*-------------------------------------------------------------------------
          Validate re registration.
        -------------------------------------------------------------------------*/
      for (client_count = 0 ;client_count < internal_clients ;client_count++)
      {
        if (ps_utils_reg_clients.clients_reg_data[client_count].func_ptr == reg_info.func_ptr)
        {
          is_client_registered = TRUE;
        
          break;
        }
      }
      /*-------------------------------------------------------------------------
          Regsitered internal client.
        -------------------------------------------------------------------------*/
      if (FALSE == is_client_registered)
      {
        ps_utils_reg_clients.clients_reg_data[internal_clients].func_ptr 
                                                       = reg_info.func_ptr;
        ps_utils_reg_clients.clients_reg_data[internal_clients].user_data_ptr 
                                                       = reg_info.user_data_ptr;
        ps_utils_reg_clients.total_reg_count++;
      
      }
      else
      {
        ret_val  = PS_UTILS_REG_FAILURE_REREG;
        break;
      }
    }
    else
    {
      ret_val  = PS_UTILS_REG_FAILURE_MAX_REG;
      break;
    }
  }while(0);

  LOG_MSG_INFO2_3("ps_utils_refresh_reg(): "
                  "call back 0x%p data 0x%p ret_val %d",
                   reg_info.func_ptr,reg_info.user_data_ptr,ret_val);

  return ret_val;
}/*ps_utils_refresh_reg*/

#endif /*FEATURE_MODEM_CONFIG_REFRESH*/


/*==========================================================================
FUNCTION PS_UTILS_DSM_PULLUP

DESCRIPTION
  This is a wrapper function for DSM_PULLUP().
  This function also prints file name and line for tracking.

DEPENDENCIES
  The passed data item must not be on a queue.

PARAMETERS
  pkt_head_ptr - Pointer to pointer to head of the packet.
  buf - Destination pointer for the data form the packet.
  len - The byte count to pull from packet head. 
  file_name - indicates caller filename
  line - indicates caller line number
  is_graceful - boolean specifies to handle gracefully or not

RETURN VALUE
  The number of bytes pulled from the passed packet head ptr.

SIDE EFFECTS
  None
===========================================================================*/
uint16 ps_utils_dsm_pullup
(
  dsm_item_type ** pkt_head_ptr,
  void *   buf,
  uint16   len,
  const    char* file_name,
  uint32   line,
  boolean  is_graceful
)
{
  uint16 length = 0;
  length = dsmi_pullup(pkt_head_ptr,buf,len,file_name,line);
  if(len != length)
  {
     MSG_SPRINTF_3(MSG_SSID_DS, MSG_LEGACY_MED,
                   "dsm_pullup failed in file (%s) line (%d) length (%d)",
                   file_name,line,length);
     ASSERT(is_graceful);
  }
  return length;
}

/*==========================================================================
FUNCTION PS_UTILS_DSM_PUSHDOWN_PACKED

DESCRIPTION
  This is a wrapper function for DSM_PUSHDOWN_PACKED().
  This function also prints file name and line for tracking.

PARAMETERS
  pkt_head_ptr - Pointer to pointer to packet to pushdown on.
  buf - Pointer to buffer of data to pushdown on packet
  size - Size of data to pushdown 
  pool_id - Pool id from which new buffers should be allocated.
  file_name - indicates caller filename
  line - indicates caller line number
  is_graceful - boolean specifies to handle gracefully or not

RETURN VALUE
   The number of bytes that were pushed on to the head of the packet. 
  
DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
uint16 ps_utils_dsm_pushdown_packed
(
  dsm_item_type        **pkt_head_ptr,
  void                 *buf,
  uint16               size,
  dsm_mempool_id_type  pool_id,
  const char*          file_name,
  uint32               line,
  boolean              is_graceful
)
{
  uint16 retval=0;
  retval = dsmi_pushdown_packed(pkt_head_ptr,buf,size,pool_id,file_name,line);
  if(retval != size)
  {
    MSG_SPRINTF_3(MSG_SSID_DS, MSG_LEGACY_MED,
                  "dsm_pushdown_packed failed in file (%s) line (%d) retval (%d)",
                  file_name,line,retval);
    ASSERT(is_graceful);
  }
  return retval;
}

/*==========================================================================
FUNCTION PS_UTILS_DSM_DUP_PACKET

DESCRIPTION
  This is a wrapper function for DSM_DUP_PACKET().
  This function also prints file name and line for tracking.

PARAMETERS
  dup_ptr - Pointer to pointer where new packet will be saved.
  src_ptr - Pointer to the packet to be duped.
  offset - Offset into src_ptr where duping should start
  len - Number of bytes from offset that should be duped
  file_name - indicates caller filename
  line - indicates caller line number
  is_graceful - boolean specifies to handle gracefully or not

RETURN VALUE
  The total size in bytes of the duplicated packet.
  
DEPENDENCIES
  none

SIDE EFFECTS
  None
===========================================================================*/
uint16 ps_utils_dsm_dup_packet
(
  dsm_item_type **   dup_ptr,
  dsm_item_type *    src_ptr,
  uint16             offset,
  uint16             len,
  const char*        file_name,
  uint32             line,
  boolean            is_graceful
)
{
  uint16 length = 0;
  length = dsmi_dup_packet( dup_ptr,DSM_DUP_ITEM_POOL,src_ptr,
                             offset,len,file_name,line);
  if(length != len)
  {
    MSG_SPRINTF_3(MSG_SSID_DS, MSG_LEGACY_MED,
                  "dsmi_dup_packet failed in file (%s) line (%d) length (%d)",
                  file_name,line,length);
    ASSERT(is_graceful);
  }
  return length;
}


