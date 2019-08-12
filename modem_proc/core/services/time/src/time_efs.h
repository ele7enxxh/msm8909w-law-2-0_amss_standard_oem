#ifndef TIME_EFS_V_H
#define TIME_EFS_V_H
/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements functionality to save offsets into efs item file.

EXTERNALIZED FUNCTIONS
  time_efs_rw 
    Local function that provides IO access for generic offsets in EFS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2009 - 2015 by QUALCOMM Technologies Incorporated.  All Rights Reserved.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_efs.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/15   abh     Changes for correctly handling ATS_PRIVATE bases.
04/19/10   din     Prevented Multiple efs_writes for same file. 
03/11/10   din     Moved efs write to Time IPC task.
17/10/09   din     File created. 

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_genoff.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/* Enum for return status */

typedef enum
{
TIME_EFS_RW_STAT_OK,
TIME_EFS_RW_STAT_RD_ERR,
TIME_EFS_RW_STAT_WR_ERR,
TIME_EFS_RW_STAT_IO_ERR,
TIME_EFS_RW_STAT_INVALID
} time_efs_rw_stat_enum_type;

/* Enum for command */
typedef enum
{
TIME_EFS_RW_CMD_RD,
TIME_EFS_RW_CMD_WR,
TIME_EFS_RW_CMD_INVALID
} time_efs_rw_cmd_enum_type;

/* define different client entries for efs */
#define TIME_TOD_EFS      ATS_MAX
#define TIME_SECURE_EFS   TIME_TOD_EFS + 1
#define TIME_USER_EFS     TIME_TOD_EFS + 2
#define TIME_LOCAL_EFS    TIME_TOD_EFS + 3

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIME_EFS_INIT

DESCRIPTION
  Initializes the efs read/write queues.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_efs_init ( void );



/*=============================================================================

FUNCTION TIME_EFS_RW

DESCRIPTION
  Local function that provides IO access for generic offsets in EFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  For writes: Expect a overwrite of the entire value of what ever size

=============================================================================*/
time_efs_rw_stat_enum_type time_efs_rw 
(
  /* Name of the file to be written "absolute path" */
  const char                 * f_name,

  /* Pointer to data to be read/written */
  void                       * data_ptr,

  /* Read/write command */
  time_efs_rw_cmd_enum_type rd_wr
);



/*=============================================================================

FUNCTION TIME_EFS_WRITE

DESCRIPTION
  Called from Time IPC task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  For writes: Expect a overwrite of the entire value of what ever size

=============================================================================*/

void time_efs_write(void);



/*=============================================================================

FUNCTION TIME_EFS_DEL_FILE

DESCRIPTION
  Called to delete EFS file node or a directory

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Post deletion, reading the efs file item will return error

=============================================================================*/

time_efs_rw_stat_enum_type time_efs_del_file
(
  /* Name of the file or directory to be deleted */
  const char                   * file_name
); /* time_efs_del_file */


#endif /* TIME_EFS_V_H */





