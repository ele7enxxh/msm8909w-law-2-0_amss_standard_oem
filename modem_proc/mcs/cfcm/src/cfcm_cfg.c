/*!
  @file
  cfcm_cfg.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

  @author
  rohitj

*/

/*==============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/cfcm_cfg.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/29/15   rj      Increase LTE UL threshold same as DL
05/15/15   rj      Adding support for PDCP COMPRESSION feature for LTE
04/22/15   rj      New Client for GPS
02/06/15   rj      Increase LTE UL thresholds to 93,90,90
11/20/14   rj      Log Packet support added in CFCM
11/19/14   rj      Seperate LTE DL and UL flow control trigger tresholds
11/14/14   rj      Updating default CPU thresholds to 97,95,95 for LTE and UMTS clients
10/27/14   rj      Adding QTF support for CFCM
10/09/14   rj      CFCM changes to Process monitor's input in its task context
09/22/14   rj      Added support to have default CPU thresholds
09/06/14   rj      Changes to send OFF command after UP due to CPU
07/28/14   rj      Updating F3s to debugging
07/03/14   rj      BUS BW changes
04/07/14   rj      initial version
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <customer.h>
#include <msg.h>
#include "fs_sys_types.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "fs_lib.h"

#include "cfcm_cfg.h"
#include "cfcm_dbg.h"
#include "cfcmi.h"
#include "cfcm_log.h"

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                         LOCAL VARIABLES

==============================================================================*/

cfcm_cfg_s cfcm_cfg;


/* Default CPU Threshold array */
static const cfcm_config_cpu_thrshld_type_s	cpu_thrshld_g[CFCM_CLIENT_MAX]= 
{ 
  /* Sr 0: CFCM_CLIENT_LTE_UL */
  {TRUE,97,95,95},
  /* Sr 1: CFCM_CLIENT_LTE_RLC_DL */
  {TRUE,97,95,95},
  /* Sr 2: CFCM_CLIENT_A2_UL_PER/CFCM_CLIENT_UL_PER */
  {TRUE,97,95,95},
  /* Sr 3: CFCM_CLIENT_LTE_ML1 */
  {TRUE,97,95,95},
  /* Sr 4: CFCM_CLIENT_LTE_PDCP_DL */
  {TRUE,97,95,95},
  /* Sr 5: CFCM_CLIENT_LTE_PDCP_COMP */
  {TRUE,85,75,75},
  /* Sr 6: CFCM_CLIENT_CM */
  {FALSE,99,98,97},
  /* Sr 7: CFCM_CLIENT_LTE_ML1_CPU */
  {TRUE,97,95,95},
  /* Sr 8: CFCM_CLIENT_HDR_DS */
  {TRUE,90,80,80},
  /* Sr 9: CFCM_CLIENT_WCDMA_RLC_DL */
  {TRUE,97,95,95},
  /* Sr 10: CFCM_CLIENT_WCDMA_RLC_UL */
  {TRUE,97,95,95},
  /* Sr 11: CFCM_CLIENT_GSM */
  {TRUE,90,80,80},
  /* Sr 12: CFCM_CLIENT_TDSCDMA_RLC_DL */
  {TRUE,90,80,80},
  /* Sr 13: CFCM_CLIENT_TDSCDMA_RLC_UL */
  {TRUE,90,80,80},
  /* Sr 14: CFCM_CLIENT_PM */
  {FALSE,90,80,80},
  /* Sr 15: CFCM_CLIENT_DS */
  {FALSE,90,80,80},
  /* Sr 16: CFCM_CLIENT_LTE_RLC_LEVEL1_DL */
  {FALSE,90,80,80},
  /* Sr 17: CFCM_CLIENT_GPS_MC */
  {FALSE,90,80,80}
};

/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/


/*==============================================================================

                                FUNCTIONS

==============================================================================*/

/*=======================================================================
FUNCTION CFCM_UPDATE_CONF_FILE

DESCRIPTION
  Update cfcm_conf file with efs file names

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static boolean cfcm_update_conf_file(void)
{
  int32 fd;
  int8  result;
  size_t         data_size;
  struct fs_stat conf_file_info;
  boolean is_updated = FALSE;
 
  /* buffer for the list of file paths to be be written to the conf file */
  char	  *str = CFCM_UPDATE_CONF_FILE_WITH_EFS_NAMES;

  result = efs_stat(CFCM_CONF_EFS_FILE, &conf_file_info);
  data_size = (size_t)strlen(str);
 
  if ( result != 0 || data_size != conf_file_info.st_size )
  {
    /* Open the CFCM CONF EFS file. If its not present then create it. */
    fd = efs_open(CFCM_CONF_EFS_FILE, O_CREAT|O_AUTODIR|O_WRONLY|O_TRUNC, ALLPERMS);
  
     CFCM_MSG_1(HIGH, "OPEN MCS conf EFS file to read %d", fd);
    if (fd < 0)
    {
      CFCM_MSG_1(ERROR, "Error opening EFS file %d", fd);
      return is_updated;
    }

    /* Write the conf file */
    (void)efs_write(fd, str, data_size);

    efs_close(fd);
    is_updated = TRUE;
  }
  return is_updated;
}


/*=======================================================================
FUNCTION CFCM_EFS_CREATE_FOLDERS

DESCRIPTION
  Creates all the folders required CFCM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static void cfcm_efs_create_folders(void)
{
  int ret;

  /* Create mcs folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs", 0x777);
  if (ret !=0 && efs_errno != EEXIST)
  {
    CFCM_MSG_1(ERROR, "Error %d creating mcs folder in efs", efs_errno);
    return ;
  }

  /* Create CFCM folder if it doesnt already exist */
  ret = efs_mkdir("/nv/item_files/mcs/cfcm", 0x777);
  if (ret !=0 && efs_errno != EEXIST)
  {
    CFCM_MSG_1(ERROR, "Error %d creating CFCM folder in efs", efs_errno);
    return ;
  }

}



/*=======================================================================
FUNCTION CFCM_CONFIG_VALIDATE_CPU_THRSHLD

DESCRIPTION
  Validates a CPU Thrshld EFS record 

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
========================================================================*/
static boolean cfcm_config_validate_cpu_thrshld 
(
  cfcm_config_cpu_thrshld_type* cpu_thrshld
)
{

  if (cpu_thrshld == NULL)
  {
    return FALSE;
  }

  /* If this entry is not valid no need to check further so return FALSE */
  if ( cpu_thrshld->is_valid != TRUE )
  {
    return FALSE;
  }

  /* CFCM Tech ID should be valid */
  if (!CFCM_IS_CLIENT_VALID (cpu_thrshld->client_id) )
  {
    return FALSE;
  }

  /* CPU Threshold Limit should be between 0 and 100 */
  if (!(CFCM_IS_CPU_THRSHLD_VALID(cpu_thrshld->tier_high)&&
      CFCM_IS_CPU_THRSHLD_VALID(cpu_thrshld->tier_medium)&&
      CFCM_IS_CPU_THRSHLD_VALID(cpu_thrshld->tier_low)))
  {
    return FALSE;
  }  

  /* CPU Threshold Limit high >= low & high >= medium */
  if ((cpu_thrshld->tier_high < cpu_thrshld->tier_medium)||
      (cpu_thrshld->tier_high < cpu_thrshld->tier_low)||
      (cpu_thrshld->tier_medium < cpu_thrshld->tier_low))
  {
    return FALSE;
  }  

  return TRUE;
}



/*==============================================================================

  FUNCTION:  cfcm_cfg_update_default_cpu_thrshlds

==============================================================================*/
/*!
    @brief
    this function updates default CPU threshold values for clients for whom value is not updated in EFS

    @return
    None
*/
/*============================================================================*/
static void cfcm_cfg_update_default_cpu_thrshlds ( void )
{
  cfcm_client_e index;
  cfcm_config_efs_type_s* client_efs; /*!<  Specific threshold values for each clients */

  /*--------------------------------------------------------------------------*/
  for(index = CFCM_CLIENT_LTE_UL; index < CFCM_CLIENT_MAX; index++)
  {
    client_efs = &cfcm_cfg.config_efs[index];

    /* Whether Data is NOT read from EFS and default value is valid */
    if ((!client_efs->cpu_thrshld.is_valid) && cpu_thrshld_g[index].is_valid)
    {
      client_efs->cpu_thrshld.is_valid      = cpu_thrshld_g[index].is_valid;
      client_efs->cpu_thrshld.tier_high     = cpu_thrshld_g[index].tier_high;
      client_efs->cpu_thrshld.tier_medium   = cpu_thrshld_g[index].tier_medium;
      client_efs->cpu_thrshld.tier_low      = cpu_thrshld_g[index].tier_low;
    }

  }
  /*--------------------------------------------------------------------------*/
} /* cfcm_cfg_update_default_cpu_thrshlds() */


/*==============================================================================

  FUNCTION:  cfcm_cfg_update_cpu_thrshlds_to_log

==============================================================================*/
/*!
    @brief
    this function updates CPU threshold values for Logging purpose

    @return
    None
*/
/*============================================================================*/
static void cfcm_cfg_update_cpu_thrshlds_to_log ( void )
{
  cfcm_client_e index;
  cfcm_config_efs_type_s* client_efs; /*!<  Specific threshold values for each clients */
  cfcm_log_cpu_thrshld_type thrshld[CFCM_CLIENT_MAX];

  /*--------------------------------------------------------------------------*/
  for(index = CFCM_CLIENT_LTE_UL; index < CFCM_CLIENT_MAX; index++)
  {
    client_efs = &cfcm_cfg.config_efs[index];
    thrshld[index].is_valid      = client_efs->cpu_thrshld.is_valid;
    thrshld[index].tier_high     = client_efs->cpu_thrshld.tier_high;
    thrshld[index].tier_medium   = client_efs->cpu_thrshld.tier_medium;
    thrshld[index].tier_low      = client_efs->cpu_thrshld.tier_low;
  }

  cfcm_log_update_cpu_thrshlds(thrshld);

} /* cfcm_cfg_update_cpu_thrshlds_to_log() */

/*=======================================================================
FUNCTION CFCM_CONFIG_READ_CPU_FC_THRSHLD

DESCRIPTION
  Initialize and Read CFCM's CPU Flow Control Thresholds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static void cfcm_config_read_cpu_fc_thrshlds (void)
{
  int32 fd_enable;
  int read_success;
  cfcm_config_cpu_thrshld_type cpu_thrshld;
  cfcm_client_e  client_id;
     
  /*--------------------------------------------------------------------------*/
     
  /* Open the EFS file. If its not present then create it. */
  fd_enable = efs_open(CFCM_CONFIG_CPU_BASED_FC_THRESHOLDS, O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd_enable < 0)
  {
    CFCM_MSG_2(ERROR, "Error opening EFS file %d %d", fd_enable, efs_errno);
    return;
  }

  while (1)
  {
    /* Read EFS file */
    read_success = efs_read (fd_enable, (void *)&cpu_thrshld, 
                                    sizeof(cpu_thrshld));
  
    /* Check to see if the read failed. If it did, then deterime
        * why it failed. */
    if (read_success <= 0)
    {
      CFCM_MSG_1(HIGH, "EFS error %d while reading CPU_BASED_FC_THRESHOLDS", efs_errno);
      break;
    }
  
    /* If read passed, check if we read the expected number of bytes */
    if (read_success != sizeof(cfcm_config_cpu_thrshld_type))
    {
      CFCM_MSG_1(MED,"Could not read a complete record of size %d", 
                     sizeof(cfcm_config_cpu_thrshld_type));
      break; 
    }

    /* Validate data read and update internal Data structure */
    if (cfcm_config_validate_cpu_thrshld(&cpu_thrshld))
    {
      client_id = cpu_thrshld.client_id;
      cfcm_cfg.config_efs[client_id].cpu_thrshld.is_valid    = cpu_thrshld.is_valid;
      cfcm_cfg.config_efs[client_id].cpu_thrshld.tier_high   = cpu_thrshld.tier_high;
      cfcm_cfg.config_efs[client_id].cpu_thrshld.tier_medium = cpu_thrshld.tier_medium;
      cfcm_cfg.config_efs[client_id].cpu_thrshld.tier_low    = cpu_thrshld.tier_low;
    }
  }

  /* Close the EFS file */
  (void)efs_close(fd_enable);

  /* Update CPU Threshold values for all Clients with Default value */
  cfcm_cfg_update_default_cpu_thrshlds();

  /* Update CPU Threshold values for Logging */
  cfcm_cfg_update_cpu_thrshlds_to_log();

}


/*==============================================================================

  FUNCTION:  cfcm_cfg_update_default_step_timer_values

==============================================================================*/
/*!
    @brief
    this function updates step timer values for all clients for whom value is not updated in EFS

    @return
    None
*/
/*============================================================================*/
static void cfcm_cfg_update_default_step_timer_values ( void )
{
  cfcm_client_e index;
  cfcm_config_efs_type_s* client_efs; /*!<  Specific threshold values for each clients */

  /*--------------------------------------------------------------------------*/
  for(index = CFCM_CLIENT_LTE_UL; index < CFCM_CLIENT_MAX; index++)
  {
    client_efs = &cfcm_cfg.config_efs[index];

    /* Validate data read and update internal Data structure */
    if (!client_efs->step_timer.is_valid)
    {
      client_efs->step_timer.is_valid           = TRUE;
      client_efs->step_timer.cpu_step_timer     = CFCM_CPU_BASED_FC_STEP_TIMER;
      client_efs->step_timer.dsm_step_timer     = CFCM_MONITOR_DSM_POOL_STEP_TIMER;
      client_efs->step_timer.thermal_step_timer = CFCM_MONITOR_THERMAL_STEP_TIMER;
      client_efs->step_timer.bw_step_timer      = CFCM_BW_BASED_FC_STEP_TIMER;
    }

  }
  /*--------------------------------------------------------------------------*/
} /* cfcm_cfg_update_default_step_timer_values() */


/*=======================================================================
FUNCTION CFCM_CONFIG_VALIDATE_STEP_TIMER_EFS

DESCRIPTION
  Validates a Step Timer EFS record 

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
========================================================================*/
static boolean cfcm_config_validate_step_timer_efs 
(
  cfcm_config_step_timer_type* step_timer
)
{

  if (step_timer == NULL)
  {
    return FALSE;
  }

  /* If this entry is not valid no need to check further so return FALSE */
  if ( step_timer->is_valid != TRUE )
  {
    return FALSE;
  }

  /* CFCM Tech ID should be valid */
  if (!CFCM_IS_CLIENT_VALID (step_timer->client_id) )
  {
    return FALSE;
  }

  return TRUE;
}


/*=======================================================================
FUNCTION CFCM_CONFIG_READ_STEP_TIMER_EFS

DESCRIPTION
  Initialize and Read CFCM's Step Timer EFS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static void cfcm_config_read_step_timer_efs (void)
{
  int32 fd_enable;
  int read_success;
  cfcm_config_step_timer_type step_timer;
  cfcm_client_e  client_id;
     
  /*--------------------------------------------------------------------------*/
     
  /* Open the EFS file. If its not present then create it. */
  fd_enable = efs_open(CFCM_CFG_STEP_TIMER_EFS_FILE, O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd_enable < 0)
  {
    CFCM_MSG_2(ERROR, "Error opening EFS file %d %d", fd_enable, efs_errno);
    return;
  }

  while (1)
  {
    /* Read EFS file */
    read_success = efs_read (fd_enable, (void *)&step_timer, 
                                    sizeof(step_timer));
  
    /* Check to see if the read failed. If it did, then deterime
        * why it failed. */
    if (read_success <= 0)
    {
      CFCM_MSG_1(HIGH, "EFS error %d while reading STEP_TIMER_EFS_FILE", efs_errno);
      break;
    }
  
    /* If read passed, check if we read the expected number of bytes */
    if (read_success != sizeof(cfcm_config_step_timer_type))
    {
      CFCM_MSG_1(MED,"Could not read a complete record of size %d", 
                     sizeof(cfcm_config_step_timer_type));
      break; 
    }

    /* Validate data read and update internal Data structure */
    if (cfcm_config_validate_step_timer_efs(&step_timer))
    {
      client_id = step_timer.client_id;
	  cfcm_cfg.config_efs[client_id].step_timer.is_valid           = step_timer.is_valid;
	  cfcm_cfg.config_efs[client_id].step_timer.cpu_step_timer     = step_timer.cpu_step_timer;
	  cfcm_cfg.config_efs[client_id].step_timer.dsm_step_timer     = step_timer.dsm_step_timer;
	  cfcm_cfg.config_efs[client_id].step_timer.thermal_step_timer = step_timer.thermal_step_timer;
	  cfcm_cfg.config_efs[client_id].step_timer.bw_step_timer      = step_timer.bw_step_timer;
    }
  }

  /* Close the EFS file */
  (void)efs_close(fd_enable);

  /* Update Step timer values for all Clients with Default value */
  cfcm_cfg_update_default_step_timer_values();

}

/*=============================================================================

  FUNCTION:  cfcm_read_fc_mask_efs

=============================================================================*/
/*!
    @brief
    Reads the Disable FC EFS file and return the value stored in it.
 
    @return
    None
*/
/*===========================================================================*/
static uint32 cfcm_read_fc_mask_efs(void)
{
  int32      fd;
  int        result;

  /* Disable mask stored in the EFS... By default consider it to be 0 
     i.e assume sw mitigation is required */
  cfcm_config_fc_mask_type     efs_content;
	
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(CFCM_CONFIG_CFCM_FC_MASK, 
                     O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    CFCM_MSG_1(ERROR, "Error opening EFS file %d", fd);
    return 0;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&efs_content, 
                       sizeof(cfcm_config_fc_mask_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      CFCM_MSG_1(ERROR,"EFS error %d while reading fc mask efs record",efs_errno);
      efs_content.fc_mask = 0;
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(cfcm_config_fc_mask_type))
    {
      CFCM_MSG_1(MED,"Could not read a complete record of size %d", 
                    sizeof(cfcm_config_fc_mask_type));
      efs_content.fc_mask = 0;
	  break; 
    }
    
    /* Read was succesful... */
    CFCM_MSG_1(MED, "Mitigation EFS file read passed with value 0x%x", 
                        efs_content.fc_mask );
    break;
  }

  (void)efs_close(fd);

  return (efs_content.fc_mask);
}

/*=============================================================================

  FUNCTION:  cfcm_read_cpu_dne_efs

=============================================================================*/
/*!
    @brief
    Reads the CPU DNE EFS file and return the value stored in it.
 
    @return
    None
*/
/*===========================================================================*/
cfcm_config_cpu_dne_type cfcm_read_cpu_dne_efs
(
 void
)
{
  int32      fd;
  int        result;

  /* Disable mask stored in the EFS... By default consider it to be 0 
     i.e assume sw mitigation is required */
  cfcm_config_cpu_dne_type     efs_content;

  /* Initialize efs_content */
  efs_content.version                     = 0;
  efs_content.cpu_dne_limit               = 0;
  efs_content.cpu_max_successive_dne_hits = 0;
	  
  /* Open the EFS file. If its not present then create it. */
  fd = efs_open(CFCM_CONFIG_CPU_DNE_EFS_FILE, 
                     O_CREAT|O_AUTODIR|O_RDONLY, ALLPERMS);

  if (fd < 0)
  {
    CFCM_MSG_1(ERROR, "Error opening EFS file %d", fd);
    efs_content.cpu_dne_limit = 0;
    efs_content.cpu_max_successive_dne_hits = 0;
    return efs_content;
  }

  while (1)
  {
    /* Read a record from EFS */
    result = efs_read (fd, (void *)&efs_content, 
                       sizeof(cfcm_config_cpu_dne_type));

    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */
    if (result <= 0)
    {
      CFCM_MSG_1(ERROR,"EFS error %d while reading cpu dne efs record",efs_errno);  
      efs_content.cpu_dne_limit = 0;
      efs_content.cpu_max_successive_dne_hits = 0;   
      break ;
    }

    /* If read passed, check if we read the expected number of bytes */
    if (result != sizeof(cfcm_config_cpu_dne_type))
    {
      CFCM_MSG_1(MED,"Could not read a complete record of size %d", 
                    sizeof(cfcm_config_cpu_dne_type));      
      efs_content.cpu_dne_limit = 0;
      efs_content.cpu_max_successive_dne_hits = 0;
	  break; 
    }
    
    /* Read was succesful... */
    CFCM_MSG_2(MED, "CPU DNE EFS file read passed with value 0x%x", 
                        efs_content.cpu_dne_limit, efs_content.cpu_max_successive_dne_hits );
    break;
  }

  (void)efs_close(fd);

  return (efs_content);
}

/*==============================================================================

  FUNCTION:  cfcm_cfg_init

==============================================================================*/
/*!
    @brief
    initialize cfcm_cfg structure

    @return
    None
*/
/*============================================================================*/
void cfcm_cfg_init(void)
{
  cfcm_dbg_s* dbg_data_ptr = cfcm_dbg_get_data();

  if (cfcm_update_conf_file())
  {
    /* Create the necessary folders if they dont exist */
    cfcm_efs_create_folders();
  }

  /* MemSet CFCM Config structure to 0 */
  memset(&cfcm_cfg, 0, sizeof(cfcm_cfg_s));

  /* If read is not successful then take the default mask instead */
  if (efs_get(CFCM_CFG_CLIENT_FC_ENABLED_MASKS_EFS_FILE, 
              &cfcm_cfg.client_fc_enable_mask, 
              sizeof(cfcm_cfg.client_fc_enable_mask)) 
      != (int)sizeof(cfcm_cfg.client_fc_enable_mask))
  {
    cfcm_cfg.client_fc_enable_mask = dbg_data_ptr->client_fc_enable_mask;
  }

  CFCM_MSG_1(HIGH, "CFCM client FC enable mask = 0x%x",
            cfcm_cfg.client_fc_enable_mask);

  /* If read is not successful then take the default mask instead */
  if (efs_get(CFCM_CFG_MONITOR_FC_ENABLED_MASKS_EFS_FILE, 
              &cfcm_cfg.monitor_fc_enable_mask, 
              sizeof(cfcm_cfg.monitor_fc_enable_mask)) 
      != (int)sizeof(cfcm_cfg.monitor_fc_enable_mask))
  {
    cfcm_cfg.monitor_fc_enable_mask = dbg_data_ptr->monitor_fc_enable_mask;
  }

  CFCM_MSG_1(HIGH, "CFCM monitor FC enable mask = 0x%x",
            cfcm_cfg.monitor_fc_enable_mask);

  /* Read CPU FC Threshold limits */
  cfcm_config_read_cpu_fc_thrshlds();

  /* Read Step timer Threshold limits */
  cfcm_config_read_step_timer_efs();  

  /* Read FC Mask info */
  cfcm_cfg.cfcm_fc_mask = cfcm_read_fc_mask_efs();
	
} /* cfcm_cfg_init() */


/*==============================================================================

  FUNCTION:  cfcm_cfg_monitor_enabled

==============================================================================*/
/*!
    @brief
    Return whether or not the monitor is enabled for flow control.

    @return
    TRUE if flow control is enabled; FALSE otherwise
*/
/*============================================================================*/
EXTERN boolean cfcm_cfg_monitor_enabled
(
  cfcm_monitor_e          monitor_id  /*!< the monitor in question */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return ((cfcm_cfg.monitor_fc_enable_mask & (1 << (uint32)monitor_id)) != 0);

} /* cfcm_cfg_monitor_enabled() */

/*==============================================================================

  FUNCTION:  cfcm_cfg_client_enabled

==============================================================================*/
/*!
    @brief
    Return whether or not the client is enabled for flow control.

    @return
    TRUE if flow control is enabled; FALSE otherwise
*/
/*============================================================================*/
EXTERN boolean cfcm_cfg_client_enabled
(
  cfcm_client_e          client_id  /*!< the client in question */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return ((cfcm_cfg.client_fc_enable_mask & (1 << (uint32)client_id)) != 0);

} /* cfcm_cfg_client_enabled() */

/*==============================================================================

  FUNCTION:  cfcm_cfg_set_client_fc_enable_mask

==============================================================================*/
/*!
    @brief
    Sets the client flow control enabled mask.
*/
/*============================================================================*/
EXTERN void cfcm_cfg_set_client_fc_enable_mask
(
  uint32 fc_enable_mask  /*!< new fc enable mask */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  cfcm_cfg.client_fc_enable_mask = fc_enable_mask;

} /* cfcm_cfg_set_client_fc_enable_mask() */

/*==============================================================================

  FUNCTION:  cfcm_cfg_set_monitor_fc_enable_mask

==============================================================================*/
/*!
    @brief
    Sets the monitor flow control enabled mask.
*/
/*============================================================================*/
EXTERN void cfcm_cfg_set_monitor_fc_enable_mask
(
  uint32 fc_enable_mask  /*!< new fc enable mask */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  cfcm_cfg.monitor_fc_enable_mask = fc_enable_mask;

} /* cfcm_cfg_set_monitor_fc_enable_mask() */

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_client_step_timers

==============================================================================*/
/*!
    @brief
    this function returns the ALL step timer values for a client 

    @return
    
*/
/*============================================================================*/
boolean cfcm_cfg_get_client_step_timers
( 
  cfcm_client_e                   client_id,
  cfcm_default_step_timer_type_s*  timers
)
{
  cfcm_config_efs_type_s* client_efs; /*!<  Specific threshold values for each clients */

  if(!(CFCM_IS_CLIENT_VALID(client_id)) || timers == NULL)
  {
    CFCM_MSG_1(ERROR, "cfcm_cfg_get_client_step_timers Error %d ", client_id);
    return FALSE;
  }

  client_efs = &cfcm_cfg.config_efs[client_id];
  if (client_efs->step_timer.is_valid)
  {
    timers->cpu_step_timer = client_efs->step_timer.cpu_step_timer;
    timers->dsm_step_timer = client_efs->step_timer.dsm_step_timer;
    timers->thermal_step_timer = client_efs->step_timer.thermal_step_timer;
    timers->bw_step_timer = client_efs->step_timer.bw_step_timer;
    return TRUE;
  }
  return FALSE;
} /* cfcm_cfg_get_client_step_timers() */


/*==============================================================================

  FUNCTION:  cfcm_cfg_get_thermal_step_timer

==============================================================================*/
/*!
    @brief
    this function returns the thermal step timer value in unit of ms.

    @return
    thermal step timer in unit of ms
*/
/*============================================================================*/
EXTERN uint32 cfcm_cfg_get_thermal_step_timer
( 
  cfcm_client_e          client_id  /*!< the client in question */
)
{
  uint32 step_timer;
  /*--------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------*/
  step_timer = cfcm_cfg.config_efs[client_id].step_timer.thermal_step_timer * 1000;
  return step_timer;
} /* cfcm_cfg_get_thermal_step_timer() */

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_dsm_step_timer

==============================================================================*/
/*!
    @brief
    this function returns the DSM step timer value in unit of ms.

    @return
    DSM step timer in unit of ms
*/
/*============================================================================*/
uint32 cfcm_cfg_get_dsm_step_timer
( 
  cfcm_client_e          client_id  /*!< the client in question */
)
{
  /*--------------------------------------------------------------------------*/
   return cfcm_cfg.config_efs[client_id].step_timer.dsm_step_timer;
  /*--------------------------------------------------------------------------*/
} /* cfcm_cfg_get_dsm_step_timer() */

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_cpu_step_timer

==============================================================================*/
/*!
    @brief
    this function returns the CPU step timer value in unit of ms.

    @return
    CPU step timer in unit of ms
*/
/*============================================================================*/
uint32 cfcm_cfg_get_cpu_step_timer
( 
  cfcm_client_e          client_id  /*!< the client in question */
)
{
  /*--------------------------------------------------------------------------*/
   return cfcm_cfg.config_efs[client_id].step_timer.cpu_step_timer;
  /*--------------------------------------------------------------------------*/
} /* cfcm_cfg_get_cpu_step_timer() */

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_bw_step_timer

==============================================================================*/
/*!
    @brief
    this function returns the Bus BW step timer value in unit of ms.

    @return
    Bus BW step timer in unit of ms
*/
/*============================================================================*/
uint32 cfcm_cfg_get_bw_step_timer
( 
  cfcm_client_e          client_id  /*!< the client in question */
)
{
  /*--------------------------------------------------------------------------*/
   return cfcm_cfg.config_efs[client_id].step_timer.bw_step_timer;
  /*--------------------------------------------------------------------------*/
} /* cfcm_cfg_get_bw_step_timer() */


/*==============================================================================

  FUNCTION:  cfcm_cfg_get_cfcm_disabled

==============================================================================*/
/*!
    @brief
    this function returns CFCM is disabled based on the EFS/NV read

    @return
    CPU Threshold values
*/
/*============================================================================*/
boolean cfcm_cfg_get_cfcm_disabled( void )
{
  /*--------------------------------------------------------------------------*/
  return (cfcm_cfg.cfcm_fc_mask & CFCM_DISABLE_MASK);
} /* cfcm_cfg_get_cpu_fc_thrshld() */

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_diag_test_enable_mask

==============================================================================*/
/*!
    @brief
    this function returns CFCM testing via Diag command enabled based on the EFS/NV read

    @return
    CPU Threshold values
*/
/*============================================================================*/
boolean cfcm_cfg_get_diag_test_enable_mask( void )
{
  /*--------------------------------------------------------------------------*/
  /* This retruns 1st Bit of FC mask */
  return ((cfcm_cfg.cfcm_fc_mask & CFCM_DIAG_TEST_ENABLE_MASK)>>1);
} /* cfcm_cfg_get_diag_test_enable_mask() */


/*==============================================================================

  FUNCTION:  cfcm_cfg_cpu_monitor_client_cmd_status

==============================================================================*/
/*!
    @brief
    This function updated CFCM command status based on the new CPU busy percentage 

    @return
    None
*/
/*============================================================================*/
boolean cfcm_cfg_cpu_monitor_client_cmd_status
( 
  cfcm_client_e client_id,
  uint32 idlest_pct,
  cfcm_cpu_monitor_client_cmd_s*   status
)
{
  cfcm_config_efs_type_s* client_efs; /*!<  Specific threshold values for each clients */

  client_efs = &cfcm_cfg.config_efs[client_id];

  if(!client_efs->cpu_thrshld.is_valid || status == NULL)
  {
    return FALSE;
  }

  memset( status, 0, sizeof(cfcm_cpu_monitor_client_cmd_s));

  if(idlest_pct > client_efs->cpu_thrshld.tier_high)
  {
    status->cmd = CFCM_CMD_DOWN;
    status->state = CFCM_CPU_MONITOR_STATE_OVERLOAD;
  }
  else if((idlest_pct <= client_efs->cpu_thrshld.tier_high) &&
  	(idlest_pct >= client_efs->cpu_thrshld.tier_medium))
  {
    status->cmd = CFCM_CMD_FREEZE;
    status->state = CFCM_CPU_MONITOR_STATE_NORMAL;
  }
  else if((idlest_pct < client_efs->cpu_thrshld.tier_medium) &&
  	(idlest_pct >= client_efs->cpu_thrshld.tier_low))
  {
    status->cmd = CFCM_CMD_UP;
    status->state = CFCM_CPU_MONITOR_STATE_UNDERLOAD;
  }
  else
  {
    status->cmd = CFCM_CMD_FC_OFF;
    status->state = CFCM_CPU_MONITOR_STATE_NOLOAD;
  }

return TRUE;
} /* cfcm_cfg_cpu_monitor_client_cmd_status() */


/*==============================================================================

                                UNIT TEST

==============================================================================*/

