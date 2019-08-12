#ifndef PS_UTILS_H
#define PS_UTILS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ U T I L S . H

GENERAL DESCRIPTION
  Collection of utility functions being used by various modules in PS.
  Most of these functions assume that the caller is in PS task context.

Copyright (c) 1995-2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_utils.h_v   1.0   08 Aug 2002 11:19:58   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/api/ps_utils.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when      who     what, where, why
-------   ---     ---------------------------------------------------------- 
10/26/12  bb      Added NV item file paths 
10/14/11  scb     Added macro def. for PS_EFS_NV_CONF_FILE_NAME
10/26/10  op      Added functions to use EFS items.
01/08/09  ar      Added C++ wrapper.
12/17/08  pp      CMI: Public/Private API split.
09/08/06  mct     Added 64 bit random number generator function.
02/22/06  rt      Added macros for rex critical sections (FEATURE_DATA_PS_L4)
02/06/06  msr/ssh Added macros for rex critical sections (FEATURE_DATA_PS_L4)
09/09/04  msr/ks  Added memdump().
08/05/02  usb     Moved lcsum() from this file to ps_iputil.c
08/05/02  usb     Moved inclusion of nv.h out of FEATURE_MIP
07/31/02  usb     Renamed the file from psmisc.h
06/14/02  usb     Removed byte manipulation functions, use dsbyte.h.
04/17/02  rc      Wrapped include of rlcdl.h in FEATURE_DATA_WCDMA_PS
03/04/02  dwp     Add include of rlcdl.h.
12/21/01  dwp     Wrap get16 in !FEATURE_DATA_PS as it is defined else where
                  in MSM5200 archive.
11/12/01  dwp     Add "|| FEATURE_DATA_PS" to whole module.
08/05/99  rc      Added support for UDP debug statistics for Sockets.
03/04/99  hcg     Changed browser interface feature to FEATURE_DS_BROWSER_INTERFACE.
06/16/97  fkm     FEATURE_xxx Updates (via Automatic Scripts)
06/12/97  ldg     Added new FEATURE_DS_PSTATS
04/11/95  jjw     Initial version of file


===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ps_svc.h"
#include "nv.h"
#include "amssassert.h"
#include "fs_public.h"
#include "fs_sys_types.h"
#include "fs_fcntl.h"
#include "fs_errno.h"
#include "ps_sys.h"
#include "sys.h"
#include "dsm_item.h"
#include "mcfg_fs.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/** -------------------------------------------------------------------------
  Buffer to read the file into
---------------------------------------------------------------------------*/
#define PS_EFS_READ_BUFFER_SZ 128

/*------------------------------------------------------------------------
   Allowed internal registration for NV refresh
-------------------------------------------------------------------------*/
#define PS_UTILS_MAX_ALLOWED_NV_REFRESH_REG  2

/*-----------------------------------------------------------------------------
  Structure used to parse the efs file
-----------------------------------------------------------------------------*/
typedef struct
{
    int fd;                          /*  File descriptor                     */
    char buffer[PS_EFS_READ_BUFFER_SZ];/*  Buffer to read the file into   */
    char *curr;                     /*  pointer to the current location      */
    char *end_pos;                  /*  ponter to the end of the buffer      */
    char seperator;                  /*  Seperator(;) to be parsed for       */
    boolean eof;                    /*  used to indicate the end of file     */
    boolean skip_line;              /*  identifies  comments in the file     */
    boolean eol;                    /*  used to indicate end of line         */
    boolean bol;                    /*  used to indicate begining of the line*/
}ps_efs_token_type;


/*-----------------------------------------------------------------------------
  Enum to specify the various return values 
  SUCCESS : Success
  EOL     : End of line is reached => record end
  EOF     : End of file is reached => file end => feof
  FAILURE : Failed 
-----------------------------------------------------------------------------*/
typedef enum 
{
  PS_EFS_TOKEN_PARSE_SUCCESS  = 0,
  PS_EFS_TOKEN_PARSE_EOL      = 1,
  PS_EFS_TOKEN_PARSE_EOF      = 2,
  PS_EFS_TOKEN_PARSE_FAILURE  = 3,
  PS_EFS_TOKEN_PARSE_MAX      = 0xFF
}ps_efs_token_parse_status_enum_type;


typedef enum
{
  PS_UTILS_REFRESH_TYPE_MIN  = 0,
  PS_UTILS_REFRESH_TYPE_SLOT = PS_UTILS_REFRESH_TYPE_MIN,  /**< refresh type for slot specific items */
  PS_UTILS_REFRESH_TYPE_SUBS,                              /**< refresh type for subscription specific items */
  PS_UTILS_REFRESH_TYPE_SLOT_N_SUBS,                       /**< refresh type for slot & sub specific items */
  PS_UTILS_REFRESH_TYPE_MAX  = 0XFF                        /**< refresh type invalid */
} ps_utils_refresh_type_e_type;  

typedef enum
{
  PS_UTILS_REFRESH_SLOT_MIN  = 1,
  PS_UTILS_REFRESH_SLOT_1 = PS_UTILS_REFRESH_SLOT_MIN,  /**< refresh default (primary) slot */
  PS_UTILS_REFRESH_SLOT_2,                              /**< refresh secondary slot */
  PS_UTILS_REFRESH_SLOT_3,                              /**< refresh tertiary slot  */
  PS_UTILS_REFRESH_SLOT_MAX  = 0XFF                     /**< refresh invalid slot  */
} ps_utils_refresh_slot_e_type; 

typedef enum
{
  PS_UTILS_REG_SUCCESS                   = 0,           /**< Registration successful      */
  PS_UTILS_REG_FAILURE_NULL_CB           = 1,           /**< Failure: Call back is NULL   */
  PS_UTILS_REG_FAILURE_REREG             = 2,           /**< Failure: Call back is same   */
  PS_UTILS_REG_FAILURE_MAX_REG           = 4,           /**< Failure: Maximum registration */
  PS_UTILS_REG_FAILURE_INIT_NOT_COMPLETE = 8,           /**< Failure: Framework yet not initialized  */ 
  PS_UTILS_REG_FAILURE_OUTSIDE_PS_TASK   = 16,          /**< Failure:  function can only be called from PS task  */
  PS_UTILS_IND_SUCCESS                   = 32,          /**< Indication successful: refresh event reported successful */
  PS_UTILS_IND_FAILURE_NULL_USER_DATA    = 64,          /**< Indication Failure: refresh data is NULL   */
  PS_UTILS_IND_FAILURE_DONE_CNF          = 128,         /**< Indication Failure: refresh done failure   */
    
} ps_utils_status_e_type; 

/*-----------------------------------------------------------------------------
  Structure used to notify refresh information
-----------------------------------------------------------------------------*/

typedef struct
{
  ps_utils_refresh_type_e_type  refresh_type;     /** Refresh for slot or sub specific */
  ps_utils_refresh_slot_e_type  refresh_slot;     /** Slot index for this refresh */
  ps_sys_subscription_enum_type refresh_subs;     /** Subscription index for this refresh */
  void                          *user_data_ptr;   /**  User data  */
}ps_utils_refresh_notify_info;

/*------------------------------------------------------------------------
   Clients Register a callback function with PS utils for MCFS NV Refresh event.
-------------------------------------------------------------------------*/

typedef void (*ps_utils_refresh_notify_func_type)(
                   ps_utils_refresh_notify_info  refresh_event_info);

/*------------------------------------------------------------------------
   Clients registration information.
-------------------------------------------------------------------------*/
typedef struct
{
  ps_utils_refresh_notify_func_type  func_ptr;
  void                              *user_data_ptr;
}ps_utils_refresh_reg_info;

/*------------------------------------------------------------------------
  registered clients information.
-------------------------------------------------------------------------*/

typedef struct
{
  uint8                       is_initialized;
  uint8                       total_reg_count;
  ps_utils_refresh_reg_info   clients_reg_data[PS_UTILS_MAX_ALLOWED_NV_REFRESH_REG];
}ps_utils_refresh_reg_table;


#define PS_UTILS_DSM_PULLUP(pkt_head_ptr,buf,len) \
          ps_utils_dsm_pullup(pkt_head_ptr,buf,len,__FILENAME__,__LINE__,TRUE)
#define PS_UTILS_DSM_PULLUP_ASSERT(pkt_head_ptr,buf,len) \
          ps_utils_dsm_pullup(pkt_head_ptr,buf,len,__FILENAME__,__LINE__,FALSE)

#define PS_UTILS_DSM_PUSHDOWN_PACKED(pkt_head_ptr,buf,size,pool_id) \
          ps_utils_dsm_pushdown_packed(pkt_head_ptr,buf,size,pool_id,__FILENAME__,__LINE__,TRUE)
#define PS_UTILS_DSM_PUSHDOWN_PACKED_ASSERT(pkt_head_ptr,buf,size,pool_id) \
          ps_utils_dsm_pushdown_packed(pkt_head_ptr,buf,size,pool_id,__FILENAME__,__LINE__,FALSE)

#define PS_UTILS_DSM_DUP_PACKET(dup_ptr,src_ptr,offset,len) \
          ps_utils_dsm_dup_packet(dup_ptr,src_ptr,offset,len,__FILENAME__,__LINE__,TRUE)
#define PS_UTILS_DSM_DUP_PACKET_ASSERT(dup_ptr,src_ptr,offset,len) \
          ps_utils_dsm_dup_packet(dup_ptr,src_ptr,offset,len,__FILENAME__,__LINE__,FALSE)

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

FUNCTION PS_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An LOG_MSG_FATAL_ERROR is logged 
  if status is other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other PS task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.

===========================================================================*/
extern nv_stat_enum_type ps_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
);

/*===========================================================================
FUNCTION PS_PUT_NV_ITEM

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
  While this function is running all other PS task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.

===========================================================================*/
extern nv_stat_enum_type ps_put_nv_item(
  nv_items_enum_type item_code,                              /* which item */
  nv_item_type *data_ptr                       /* pointer to data for item */
);

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
);

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
);

/*===========================================================================
FUNCTION PS_GET_NV_ITEM_EXT 

DESCRIPTION
  This function retrieves the specified item from NV.

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
  nv_items_enum_type             item,           /* Which item */
  nv_item_type                  *data_ptr,       /* Pointer to space for item */
  ps_sys_subscription_enum_type  ps_subs_id      /*PS Subscription ID's*/
);

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
  nv_items_enum_type             item,           /* Which item */
  nv_item_type                  *data_ptr,       /* Pointer to space for item */
  ps_sys_subscription_enum_type  ps_subs_id      /* PS Subscription ID's*/
);

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
extern dword msclock( void);

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
extern void memdump
(
  void * data_ptr,
  int    len
);

/*===========================================================================
FUNCTION PS_UTILS_GENERATE_RAND_64BIT_NUM()

DESCRIPTION
  This function generates a random 64 bit number.

PARAMETERS
  *random_num - Pointer to the 64 bit number to be returned by this function.

RETURN VALUE
  None

DEPENDENCIES
  As a pre-condition, this function assumes that ran_seed() has been called
  at least once. Currently, ran_seed() is called by the main control task
  as part of initialization.

SIDE EFFECTS
  None
===========================================================================*/
void ps_utils_generate_rand_64bit_num
(
  uint64 *rand_num                 /* Pointer to the 64bit num be returned */
);

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
 -1         - Non-EFS related failures
  efs_errno - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_create_efs_config_file
(
  const char *conf_file_path
);

/*===========================================================================
FUNCTION PS_CREATE_EFS_CONFIG_FILE_EXT

DESCRIPTION
  Create a config file in EFS which stores the path of EFS item files.

DEPENDENCIES
  None

PARAMETERS
  conf_file_path - File path of config file
  ps_subs_id     - Subscription Id for which the efs
                   operation has to be performed on.

RETURN VALUE
  0         - Success
 -1         - Non-EFS related failures
  efs_errno - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_create_efs_config_file_ext
(
  const char *conf_file_path,
  ps_sys_subscription_enum_type  ps_subs_id 
);

/*===========================================================================
FUNCTION PS_CREATE_EFS_ITEM_FILE

DESCRIPTION
  Put(append and add newline) item file path into config file

DEPENDENCIES
  None

PARAMETERS
  conf_file_path      - File path to a specific conf file
  item_file_path      - File path to item in NV
  item_file_path_size - Size of item file path

RETURN VALUE
  0         - Success
 -1         - Non-EFS related failures
  efs_errno - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_create_efs_item_file
(
  const char   *conf_file_path,
  char         *item_file_path, 
  fs_size_t     item_file_path_size
);

/*===========================================================================
FUNCTION PS_CREATE_EFS_ITEM_FILE_EXT

DESCRIPTION
  Put(append and add newline) item file path into config file

DEPENDENCIES
  None

PARAMETERS
  conf_file_path      - File path to a specific conf file
  item_file_path      - File path to item in NV
  item_file_path_size - Size of item file path
  ps_subs_id          - Subscription Id for which efs item
                        file operation will be performed.

RETURN VALUE
  0         - Success
 -1         - Non-EFS related failures
  efs_errno - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_create_efs_item_file_ext
(
  const char   *conf_file_path,
  char         *item_file_path, 
  fs_size_t     item_file_path_size,
  ps_sys_subscription_enum_type  ps_subs_id 
);

/*===========================================================================
FUNCTION PS_READ_EFS_NV

DESCRIPTION
  This function reads the EFS item from the item file

DEPENDENCIES
  None

PARAMETERS
  item_file_path - File path to item in NV
  nv_info        - Pointer to data type for NV item(s)
  nv_info_size   - Size of NV item data type

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  efs_errno - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_read_efs_nv
(
  const char   *item_file_path, 
  void         *nv_info_ptr, 
  fs_size_t    nv_info_size
);

/*===========================================================================
FUNCTION PS_READ_EFS_NV_EXT

DESCRIPTION
  This function reads the EFS item from the item file

DEPENDENCIES
  None

PARAMETERS
  item_file_path - File path to item in NV
  nv_info        - Pointer to data type for NV item(s)
  nv_info_size   - Size of NV item data type
  ps_subs_id     - Subscription Id for which efs item
                   file operation will be performed.

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  efs_errno - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_read_efs_nv_ext
(
  const char   *item_file_path, 
  void         *nv_info_ptr, 
  fs_size_t    nv_info_size,
  ps_sys_subscription_enum_type  ps_subs_id 
);

/*===========================================================================
FUNCTION PS_WRITE_EFS_NV

DESCRIPTION
  This function writes the EFS item to the item file

DEPENDENCIES
  None

PARAMETERS
  item_file_path - File path to item in NV
  nv_info        - Pointer to data type for NV item(s)
  nv_info_size   - Size of NV item data type

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  efs_errno - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_write_efs_nv
(
  const char   *item_file_path, 
  void         *nv_info_ptr, 
  fs_size_t    nv_info_size
);

/*===========================================================================
FUNCTION PS_WRITE_EFS_NV_EXT

DESCRIPTION
  This function writes the EFS item to the item file

DEPENDENCIES
  None

PARAMETERS
  item_file_path - File path to item in NV
  nv_info        - Pointer to data type for NV item(s)
  nv_info_size   - Size of NV item data type
  ps_subs_id     - Subscription Id for which efs item
                   file operation will be performed.

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  efs_errno - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_write_efs_nv_ext
(
  const char   *item_file_path, 
  void         *nv_info_ptr, 
  fs_size_t    nv_info_size,
  ps_sys_subscription_enum_type  ps_subs_id
);

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
   efs_errno - EFS error
   -1        - Other error

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_path_is_directory
(
  const char *dirname
);

/*===========================================================================
FUNCTION PS_PATH_IS_DIRECTORY_EXT

DESCRIPTION
  To check if the EFS directory exists

DEPENDENCIES
  None

PARAMETERS
  dirname        - Directory path
  ps_subs_id     - Subscription Id for which efs item
                   file operation will be performed.

RETURN VALUE
   0         - success
   efs_errno - EFS error
   -1        - Other error

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_path_is_directory_ext
(
  const char *dirname,
  ps_sys_subscription_enum_type  ps_subs_id
);

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
);

/*===========================================================================
FUNCTION      PS_EFS_FILE_INIT_EXT

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
);

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
);

/*===========================================================================
FUNCTION      PS_EFS_FILE_CLOSE_EXT

DESCRIPTION   The function closes file and releases the state machine 

DEPENDENCIES  The file should have opened already.

RETURN VALUE  NONE

SIDE EFFECTS  None.
===========================================================================*/
void ps_efs_file_close_ext
( 
  ps_efs_token_type *sm,
  ps_sys_subscription_enum_type ps_subs_id
);

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
);

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
);


/** @ps_utils_generate_16bit_rand_num_by_range
  This function calculates a random integer within a given range using
  the formula:

  result = ran_num * ( hi_val - lo_val ) + lo_val

  @dataypes
  lo_val    - Lower limit for the random number
  hi_val    - Upper limit for the random number

  @return
  a random number within a range

  @secapi_get_random, ran_dist
  None.
*/
word ps_utils_generate_16bit_rand_num_by_range
(
  word  lo_val,
  word  hi_val
);

/** @ps_utils_generate_rand_num
  This function generates a random number of length specified by the caller

  @dataypes
  rand_num  - Pointer to the number to be returned by this function.
  rand_len  - Length of the random number requested

  @return
  None

  @secapi_get_random
  None.
*/
void ps_utils_generate_rand_num
(
  void    *rand_num,
  uint16  rand_len
);

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
void ps_utils_refresh_init ( void );

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
);


/*==========================================================================
FUNCTION PS_UTILS_DSM_PULLUP

DESCRIPTION
  This is a wrapper function for DSM_PULLUP().
  This function also prints file name and line for tracking.

DEPENDENCIES
  The passed data item must not be on a queue.

PARAMETERS
  pkt_head_ptr - Pointer to pointer to head of the packet.
  buf - Destination pointer for the data from the packet.
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
  dsm_item_type  **pkt_head_ptr,
  void           *buf,
  uint16          len,
  const char*     file_name,
  uint32          line,
  boolean         is_graceful
);

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
  dsm_item_type       **pkt_head_ptr,
  void                *buf,
  uint16               size,
  dsm_mempool_id_type  pool_id,
  const char*          file_name,
  uint32               line,
  boolean              is_graceful
);

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
);

#ifdef __cplusplus
}
#endif

#endif /* PS_UTILS_H */
