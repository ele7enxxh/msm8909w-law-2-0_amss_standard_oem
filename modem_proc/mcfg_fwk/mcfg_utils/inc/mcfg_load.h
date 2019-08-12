/*===========================================================================
                        mcfg_load.h

DESCRIPTION

   Internal header file for modem_cfg_task
 
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/inc/mcfg_load.h#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
1/18/12      sbt   Create

===========================================================================*/

#ifndef MCFG_LOAD_H
#define MCFG_LOAD_H

#include "mcfg_common.h"
/*------------------------------------------------------------------------------
  MCFG Files - Constants
------------------------------------------------------------------------------*/
/* Maximum size of an entire MCFG IMAGE file = 128KB,
   total zise occupied by a single type of MCFGs = 512KB */

//#define MCFG_MAX_FILE_SIZE              (128*1024)
//#define MCFG_MAX_TOTAL_STORAGE_SIZE     (512*1024)

/* Directory in EFS where MCFG files are stored. 
 * If you change this value, you must also change MCFG_FILE_MAX_PATH_LEN */
//#define MCFG_FILES_DIR       "/nvm/mcfg/"
//#define MCFG_FILENAME_MAX_LEN   (64)

/* Max MCFG Image File path includes space for null-terminator. 18 comes from the length
 * of MCFG_FILES_DIR + length of "cfg_X/" + 1 */
//#define MCFG_FILE_MAX_PATH_LEN  (18 + MCFG_FILENAME_MAX_LEN)

//#define MCFG_FILE_EFS_PERMS          (0644)

/* Function prototypes*/
mcfg_error_e_type modem_cfg_load_and_auth_mcfg(uint8 cfg_type, 
                                    mcfg_config_id_s_type* current_id_ptr, 
                                    mcfg_config_s_type* config_ptr, 
                                    boolean active, 
                                    mcfg_sub_id_type_e_type sub); 
uint8* mcfg_load_seg(mcfg_config_s_type* config_ptr, uint32* add_info_ptr);
uint8* mcfg_load_devcfg(mcfg_config_s_type* config_ptr);
boolean mcfg_get_hash_seg_info(mcfg_config_s_type* config_ptr, uint32* offset, uint32* data_size);
boolean mcfg_get_data_seg_info(mcfg_config_s_type* config_ptr, uint32* offset, uint32* data_size);

#endif /* MODEM_CFG_PROC_H */
