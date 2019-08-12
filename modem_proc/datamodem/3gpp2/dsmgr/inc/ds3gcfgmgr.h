#ifndef DS3GCFGMGR_H
#define DS3GCFGMGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             3G  DATA  SERVICES  CONFIGURATION  MANAGER

GENERAL DESCRIPTION
  This software unit manages the configuration for DS. It provides APIs for
  nv/efs configuration related properties.

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  USING EFS ITEMS
    1. DS_CREATE_EFS_CONFIG_FILE() should be called in a powerup function.
       Only need to call this once per task. Each task shall have a
       different config file. DS task calls this function in dstask.c
    2. DS_CREATE_EFS_ITEM_FILE() should be called in component specific
       init function. This function will append an efs item file path into
       the config file.
       For DS task, use "/nv/item_files/conf/data_config_info.conf" for config
       file location
    3. When the EFS item wants to be read, DS_READ_EFS_NV() is called.
       The item file path is used to locate the item file in the EFS.
       A data structure/data type (passed by reference) will be filled with
       the value of the EFS item.
    4. If the read call returns with a ENOENT (file does not exist), then
       user will need to call DS_WRITE_EFS_NV() with default values passed in.
 
Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gcfgmgr.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/11/14   vm      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "nv.h"

#include "ds3gsubsmgr.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#ifndef NUM_FIELDS
#define NUM_FIELDS 10
#endif

#ifndef MAX_FIELD_LEN
#define MAX_FIELD_LEN 64
#endif

/*-----------------------------------------------------------------------------
  Structure used to parse the efs file
-----------------------------------------------------------------------------*/
typedef struct
{
    int read_buffer_size;       /*  For efs_read, value is DS_EFS_READ_BUFFER_SZ
                                    For efs_get, value is got from efs_stat  */
    char seperator;             /*  Seperator(;) to be parsed for            */
    char *buffer;               /*  Buffer to read the file into             */
    char *curr;                 /*  pointer to the current location          */
    char *end_pos;              /*  ponter to the end of the buffer          */
    char *efs_file_path;        /*  Use this when doing an efs_get           */
    boolean eof;                /*  used to indicate the end of file         */
    boolean skip_line;          /*  identifies  comments in the file         */
    boolean eol;                /*  used to indicate end of line             */
    boolean bol;                /*  used to indicate begining of the line    */
    boolean efs_get_performed;  /*  Variable to know if an efs_get has been 
                                    performed                                */
  sys_modem_as_id_e_type subs_id;  /*  Subscription ID                         */
}ds3gcfgmgr_efs_token_type;

/*-----------------------------------------------------------------------------
  Enum to specify the various return values 
  SUCCESS : Success
  EOL     : End of line is reached => record end
  EOF     : End of file is reached => file end => feof
  FAILURE : Failed 
-----------------------------------------------------------------------------*/
typedef enum 
{
  DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS  = 0,
  DS3GCFGMGR_EFS_TOKEN_PARSE_EOL      = 1,
  DS3GCFGMGR_EFS_TOKEN_PARSE_EOF      = 2,
  DS3GCFGMGR_EFS_TOKEN_PARSE_FAILURE  = 3,
  DS3GCFGMGR_EFS_TOKEN_PARSE_MAX
}ds3gcfgmgr_efs_token_parse_status_type;

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GCFGMGR_INIT

DESCRIPTION   This function initializes the DS3G Cfg Mgr module. This would 
              be invoked from ds3gi_cm_init() which is the initialization
              routine for DSMgr.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcfgmgr_init( void );

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_LEGACY_NV

DESCRIPTION   This function reads legacy NV item for Subscription ID 1.
              This API can be used to read device specific legacy NV.

DEPENDENCIES  NV task is expected to be available.  Must run in the 
              DS task context.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_read_legacy_nv
(
  nv_items_enum_type  item,
  nv_item_type       *data_ptr
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_LEGACY_NV_EX

DESCRIPTION   This function reads legacy NV item per subscription.

DEPENDENCIES  NV task is expected to be available.  Must run in the 
              DS task context.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_read_legacy_nv_ex
(
  nv_items_enum_type      item,
  nv_item_type           *data_ptr,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_WRITE_LEGACY_NV

DESCRIPTION   This function writes legacy NV item for Subscription ID 1.
              This API can be used to write device specific legacy NV.

DEPENDENCIES  NV task is expected to be available.  Must run in the 
              DS task context.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_write_legacy_nv
(
  nv_items_enum_type   item,
  nv_item_type        *data_ptr
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_WRITE_LEGACY_NV_EX

DESCRIPTION   This function writes legacy NV item per subscription.

DEPENDENCIES  NV task is expected to be available.  Must run in the 
              DS task context.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_write_legacy_nv_ex
(
  nv_items_enum_type      item,
  nv_item_type           *data_ptr,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_EFS_NV

DESCRIPTION   This function reads EFS NV item for Subscription ID 1.
              This API can be used to read device specific EFS NV.
              data_ptr should be correctly by initailized by clients.

DEPENDENCIES  None.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_read_efs_nv
(
  const char  *item_path,
  void        *data_ptr,
  uint32       item_size
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_EFS_NV_EX

DESCRIPTION   This function reads EFS NV item per subscription. 
              data_ptr should be correctly by initailized by clients.

DEPENDENCIES  None.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_read_efs_nv_ex
(
  const char             *item_path,
  void                   *data_ptr,
  uint32                  item_size,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_WRITE_EFS_NV

DESCRIPTION   This function writes the EFS NV item for Subscription ID 1.
              This API can be used to write device specific EFS NV.

DEPENDENCIES  None

RETURN VALUE  NV return code.

SIDE EFFECTS  None
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_write_efs_nv
(
  const char  *item_path,
  void        *data_ptr,
  uint32       item_size
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_WRITE_EFS_NV_EX

DESCRIPTION   This function writes the EFS NV item per subscription.

DEPENDENCIES  None

RETURN VALUE  NV return code.

SIDE EFFECTS  None
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_write_efs_nv_ex
(
  const char             *item_path,
  void                   *data_ptr,
  uint32                  item_size,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_EFS_FILE_INIT

DESCRIPTION   The function intializes the state machine and 
              also opens the file.

DEPENDENCIES  None.

RETURN VALUE  0  : SUCCESS: The file is good, readable, 
                            State Machine Initialized.
              -1 : FAILURE: The file cannot be opened/ readable. 

SIDE EFFECTS  None.
===========================================================================*/
int ds3gcfgmgr_efs_file_init
(
  const char                 *file_path,
  ds3gcfgmgr_efs_token_type  *sm
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_EFS_FILE_INIT_EX

DESCRIPTION   The function intializes the state machine and 
              also opens the file.

DEPENDENCIES  None.

RETURN VALUE  0  : SUCCESS: The file is good, readable, 
                            State Machine Initialized.
              -1 : FAILURE: The file cannot be opened/ readable. 

SIDE EFFECTS  None.
===========================================================================*/
int ds3gcfgmgr_efs_file_init_ex
(
  const char                 *file_path,
  sys_modem_as_id_e_type      subs_id,
  ds3gcfgmgr_efs_token_type  *sm
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_EFS_FILE_CLOSE

DESCRIPTION   The function closes file and releases the state machine 

DEPENDENCIES  The file should have opened already.

RETURN VALUE  NONE

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcfgmgr_efs_file_close
(
  ds3gcfgmgr_efs_token_type  *sm
);

/*===========================================================================
FUNCTION      DS_EFS_TOKENIZER

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
ds3gcfgmgr_efs_token_parse_status_type ds3gcfgmgr_efs_tokenizer
(
  ds3gcfgmgr_efs_token_type   *sm,
  char                       **begin,
  char                       **end
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_EFSIF_READ_A_RECORD

DESCRIPTION   Read a record including number of fields

DEPENDENCIES  None.

RETURN VALUE  Status to indicate if failed or Success or others

SIDE EFFECTS  None.
===========================================================================*/
ds3gcfgmgr_efs_token_parse_status_type ds3gcfgmgr_efsif_read_a_record
( 
  ds3gcfgmgr_efs_token_type  *sm,
  char                        rec[][MAX_FIELD_LEN],
  uint16                      num_fields,
  uint16                     *valid_fields 
);

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_FAST_DORMANCY_INFO_FROM_EFS

DESCRIPTION   This function will read the efs file all the necessary 
              data & fill the timer values in FD config structure.

              EFS File Format - Param_Name:Param_Val;
              For example     - TIMER_1:0;             

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gcfgmgr_read_fast_dormancy_info_from_efs (void);

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_FAST_DORMANCY_ENABLED_FLAG_FROM_EFS

DESCRIPTION   This function will read the efs file and gets if Fast 
              Dormancy is enabled or not. If file doesnt exist, by 
              deafult cosiders as FD is disabled

              EFS File Format - Param_Val
              For example     - 1/0

DEPENDENCIES  None

RETURN VALUE  
None

SIDE EFFECTS  None
===========================================================================*/
void ds3gcfgmgr_read_fast_dormancy_enabled_flag_from_efs( void );

#endif /* DS3GCFGMGR_H */

