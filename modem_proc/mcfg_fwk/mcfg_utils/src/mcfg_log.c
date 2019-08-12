/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

	MODEM_CFG Logging support

GENERAL DESCRIPTION

  This file makes up the config processing component of the MODEM_CFG
  
  EXTERNALIZED FUNCTIONS
    mcfg_save_log_entry                    
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
 
Copyright (c) 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE/

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_log.c#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $ 
 
when       who     what, where, why
--------   ---    ----------------------------------------------------------
01/18/12   sbt     Initial Creation.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "fs_public.h"                          /* For handling EFS files */
#include "fs_lib.h"
#include "fs_sys_types.h"
#include <stringl/stringl.h>

#include "mcfg_feature_config.h"
#include "mcfg_log.h"

/*==========================================================================

  FUNCTION mcfg_save_log_entry

  DESCRIPTION
    Writes string to the EFS log for MCFG debugging.

  DEPENDENCIES
    None.

  PARAMETERS
    line [in] string to write to EFS log

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void mcfg_save_log_entry(
  const char *line
  )
{
  int fd;
  int line_len;
  int bytes_written;
  int efs_ret;

  struct fs_stat f_stat;
  static boolean rotated = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* First see if we need to rotate the log - only do this check once */
  if (!rotated)
  {
    efs_ret = efs_stat(MCFG_LOG_PATH, &f_stat);
    if (efs_ret == 0)
    {
      if (f_stat.st_size >= MCFG_LOG_MAX_SIZE)
      {
        /* Delete any existing old log file. This might fail if the file
         * doesn't exist, so ignore the return value. */
        (void) efs_unlink(MCFG_OLD_LOG_PATH);
  
        efs_ret = efs_rename(MCFG_LOG_PATH, MCFG_OLD_LOG_PATH);
        if (efs_ret != 0)
        {
          /* Rename failed. Try deleting the log file. */
          efs_ret = efs_unlink(MCFG_LOG_PATH);
          if (efs_ret != 0)
          {
            /* This is very bad. Guess we won't be saving a log entry. */
            return;
          }
        }
      }
      rotated = TRUE;
    }
  }

  fd = efs_open(MCFG_LOG_PATH, O_CREAT | O_WRONLY | O_APPEND, 0644);
  /* Ignore the open failed case since this is a purely diagnostic log */
  if (fd >= 0)
  {
    line_len = strlen(line);
    bytes_written = 0;
    do
    {
      efs_ret = efs_write(fd, (void *) (line + bytes_written), 
                          (line_len - bytes_written));
      bytes_written += efs_ret;
    } while (efs_ret > 0 && bytes_written < line_len);

    efs_close(fd);
  }
} /* mcfg_save_log_entry() */


