/*!
  @file
  cfcm_log.c

  @brief
  This file implements the Log related data for CFCM task.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/cfcm_log.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/03/14   rj      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "cfcmi.h"
#include "cfcm_log.h"
#include "cfcm_client.h"
/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
cfcm_log_cpu_thrshld_type cpu_thrshld[CFCM_CLIENT_MAX];

#define CFCM_LOG_MONITOR_INFO_VERSION 1 /* v1 */
#define CFCM_LOG_COMMAND_INFO_VERSION 1 /* v1  */

/*=============================================================================

                                FUNCTIONS

=============================================================================*/
/*=============================================================================

  FUNCTION:  cfcm_log_monitor_input

=============================================================================*/
/*!
    @brief
    Logs the power and frequency information for a particular tech
 
    @return
    None
*/
/*===========================================================================*/
void cfcm_log_monitor_input
(
  cfcm_monitor_e monitor_id,
  cfcm_log_monitor_type* monitor_data
)
{
#ifdef LOG_MCS_CFCM_MONITOR_INFO_C
  cfcm_log_monitor_info_type *log_ptr=NULL;
  uint32 log_size = 0;

    /* Check if tech_id is valid */
  if (!CFCM_IS_MONITOR_ID_VALID(monitor_id))
  {
    CFCM_MSG_1(ERROR, "Invalid monitor ID %d sent for logging", monitor_id);
    return;
  }

  /* Find out how much size we need to allocate */
  log_size = sizeof(cfcm_log_monitor_info_type);

  log_ptr = (cfcm_log_monitor_info_type *)log_alloc_ex( 
                                       (log_code_type) LOG_MCS_CFCM_MONITOR_INFO_C, log_size );

  if ( log_ptr != NULL )
  {
    log_ptr->version = CFCM_LOG_MONITOR_INFO_VERSION;
    /* Update monitor data to log packet */
    log_ptr->monitor_id = (uint8)monitor_id;
    memscpy(&log_ptr->monitor, sizeof(cfcm_log_monitor_type),
              monitor_data, sizeof(cfcm_log_monitor_type));

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
  else
  {
    CFCM_MSG_1(MED, "Could not allocate cfcm_log_monitor_input pkt of size %d",
                          log_size );
  }
#else
  CFCM_UNUSED(monitor_data);
#endif /* LOG_MCS_CFCM_MONITOR_INFO_C */
}

/*=============================================================================

  FUNCTION:  cfcm_log_command_output

=============================================================================*/
/*!
    @brief
    Logs all CFCM Output Command
 
    @return
    None
*/
/*===========================================================================*/
void cfcm_log_command_output
( 
  uint32* hist_idx,
  cfcm_monitor_e monitor_id,
  cfcm_log_monitor_type* monitor_data
)
{
#ifdef LOG_MCS_CFCM_COMMAND_INFO_C
  cfcm_log_cmd_info_output_type *log_ptr=NULL;
  uint32 index = 0, latest_hist_idx = 0;
  uint32 log_size = 0;
  cfcm_client_info_s*   client_ptr;
  boolean is_any_client_updated = FALSE;
  
  if (hist_idx == NULL)
  {
    CFCM_MSG_0(ERROR, "INVALID arguments to cfcm_log_command_output");
    return ;
  }

  /* Find out how much size we need to allocate */
  log_size = FPOS(cfcm_log_cmd_info_output_type, output.client_cmd);
  log_size += (CFCM_CLIENT_MAX * sizeof(cfcm_log_client_cmd_data_type));

  log_ptr = (cfcm_log_cmd_info_output_type *)log_alloc_ex( 
                                       (log_code_type) LOG_MCS_CFCM_COMMAND_INFO_C,
                                               log_size );

  if ( log_ptr != NULL )
  {
    log_ptr->version = CFCM_LOG_COMMAND_INFO_VERSION;

    /* Update monitor data to log packet */
    log_ptr->output.monitor_id = (uint8)monitor_id;
    memscpy(&log_ptr->output.monitor, sizeof(cfcm_log_monitor_type),
              monitor_data, sizeof(cfcm_log_monitor_type));

    for (index=0; index < CFCM_CLIENT_MAX; index++)
    {
      client_ptr = &cfcm_client.clients[index];
      if (hist_idx[index] != client_ptr->latest_cmd_hist_idx)
      {
        latest_hist_idx = client_ptr->latest_cmd_hist_idx;
        log_ptr->output.client_cmd[index].client_id = (uint8)client_ptr->cmd_hist[latest_hist_idx].client_id;
        log_ptr->output.client_cmd[index].cmd = (uint32)client_ptr->cmd_hist[latest_hist_idx].cmd;
        log_ptr->output.client_cmd[index].step_timer = client_ptr->cmd_hist[latest_hist_idx].step_timer;
        log_ptr->output.client_cmd[index].monitors_active = client_ptr->cmd_hist[latest_hist_idx].monitors_active;
        log_ptr->output.client_cmd[index].data_rate = client_ptr->cmd_hist[latest_hist_idx].data_rate;
		is_any_client_updated = TRUE;
        if (CFCM_MONITOR_ID_PRESENT_IN_MASK(client_ptr->cmd_hist[latest_hist_idx].monitors_active, CFCM_MONITOR_CPU))
        {
          log_ptr->output.client_cmd[index].cpu_thrshld = cpu_thrshld[index];
        }

      }
	  else
      {
        log_ptr->output.client_cmd[index].client_id = CFCM_CLIENT_MAX;
      }
    }

    if (is_any_client_updated == TRUE)
    {
      /* submit the log to DIAG */
      log_commit( log_ptr );
    }
    else
    {
      /* Free the log  */
      log_free( log_ptr );
    }
  }
  else
  {
    CFCM_MSG_1(MED, "Could not allocate log pkt of size %d", log_size);
  }
#else
  CFCM_UNUSED(hist_idx);
#endif /* LOG_MCS_CFCM_COMMAND_INFO_C */
}

/*=============================================================================

  FUNCTION:  cfcm_log_update_cpu_thrshlds

=============================================================================*/
/*!
    @brief
    Updates CPU thresholds for logging purpose
 
    @return
    None
*/
/*===========================================================================*/
void cfcm_log_update_cpu_thrshlds
(
  cfcm_log_cpu_thrshld_type* thrshld
)
{
  uint32 size = 0; 
  size = (CFCM_CLIENT_MAX * sizeof(cfcm_log_cpu_thrshld_type));
  if (thrshld != NULL)
  {
    memscpy(cpu_thrshld, size, thrshld, size);
  }
}

