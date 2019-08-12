/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                               T M   L O G
                           TCXO Manager Logging data

GENERAL DESCRIPTION

  This module handles the logging of data from TCXOMGR.

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/src/tcxomgr_log.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ---------------------------------------------------------
05/22/15   tl       Suppress logging RGS retrieval if log is unchanged
12/11/12   mn       New log packet for Retrieve RGS.

============================================================================*/
#include "mcs_variation.h"
#include "customer.h"
#include "tcxomgr_log_i.h"
#include "tcxomgr_diag.h"

/*============================================================================

                   Defines

============================================================================*/

/*============================================================================

                   Globals

============================================================================*/
typedef struct
{
  tcxomgr_log_rgs_struct_type                log_rgs_data;
  int32                                      rgs_log_id;
  
  tcxomgr_log_rgs_retrieval_struct_type      log_rgs_retrieval_data;
  int32                                      rgs_retrieval_log_id;
  
  tcxomgr_log_aged_rgs_retrieval_struct_type log_aged_rgs_retrieval_data;
  int32                                      aged_rgs_retrieval_log_id;
} tcxomgr_log_rgs_global_data_type;


static  tcxomgr_log_rgs_global_data_type  tcxomgr_rgs_log;

static tcxomgr_log_rgs_retrieval_data_struct_type last_log_rgs;

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_RGS_RETRIEVAL_IS_NEW

DESCRIPTION
  Determines if the RGS retrieval log entry is different from the last log
  entry, to avoid sending log messages to the Tcxomgr task for identical log
  entries. The log entry is considered different if the temperature, rotator,
  VCO type, or API type is different, or the timestamp differs by more than a
  second.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the log is different from the last log; FALSE otherwise

SIDE EFFECTS
  Acquires and releases the Tcxomgr critical section. Updates the static
  variable last_log_rgs holding the last RGS retrieval log sent to the Tcxomgr
  task.

============================================================================*/
static boolean tcxomgr_rgs_retrieval_is_new
(
  const tcxomgr_log_rgs_retrieval_data_struct_type *log_rgs
)
{
  boolean rgs_is_new = FALSE;

  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  if(((log_rgs->timestamp_ms - last_log_rgs.timestamp_ms) >= 1000) ||
      (log_rgs->temp_xo != last_log_rgs.temp_xo) ||
      (log_rgs->temp_pmic != last_log_rgs.temp_pmic) ||
      (log_rgs->rot_value != last_log_rgs.rot_value) ||
      (log_rgs->vco_type != last_log_rgs.vco_type) ||
      (log_rgs->api_type != last_log_rgs.api_type))
  {
    rgs_is_new = TRUE;
    last_log_rgs = *log_rgs;
  }

  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  return rgs_is_new;
}

/*============================================================================

FUNCTION TCXOMGR_RESET_LOGGING_DATA

DESCRIPTION
  Resets the global logging data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_reset_logging_data
(
  void
)
{
  /* Initialize the logging data */
  memset(&tcxomgr_rgs_log, 0, sizeof(tcxomgr_rgs_log));
}

/*============================================================================

FUNCTION TCXOMGR_LOG_RGS_DATA

DESCRIPTION
    Helper function to copy the factory cal data to the log structure. 

DEPENDENCIES
    None     

RETURN VALUE
    None
 
SIDE EFFECTS
    None

============================================================================*/
void tcxomgr_log_rgs_data
(
  tcxomgr_rgs_logging_enum_type rgs_log_type,
  void* log_rgs
)
{
  tcxomgr_cmd_type *cmd_buffer; 
  uint8 log_data = FALSE;

  /* Get a tcxomgr command buffer and send it away */
  cmd_buffer = tcxomgr_get_cmd_buf();

  /* Make sure we got a buffer */
  if (cmd_buffer != NULL) 
  {
    cmd_buffer->cmd_id = TCXOMGR_CMD_RGS_LOG;
    
    switch ( rgs_log_type )
    {
      case TCXOMGR_RGS_UPDATE_LOG_CMD:
        {
          /* Log rgs only if the it is enabled, this way we will prevent unnecessary 
             task switches */
          if ( log_status(LOG_TCXOMGR_RGS_C) != FALSE)
          {
            cmd_buffer->cmd.log_rgs_data.type = TCXOMGR_RGS_UPDATE_LOG_CMD; 
            cmd_buffer->cmd.log_rgs_data.data.rgs_data = \
                    *((tcxomgr_log_rgs_data_struct_type *)log_rgs);
            log_data = TRUE;
          }
          break;
        }
      
      case TCXOMGR_RGS_RETRIEVAL_LOG_CMD:
        {
          /* Log rgs only if the it is enabled, this way we will prevent unnecessary 
             task switches */
#ifdef LOG_TCXOMGR_RGS_RETRIEVAL_C
          if ( log_status(LOG_TCXOMGR_RGS_RETRIEVAL_C) != FALSE &&
              tcxomgr_rgs_retrieval_is_new(log_rgs))
          {
            cmd_buffer->cmd.log_rgs_data.type = TCXOMGR_RGS_RETRIEVAL_LOG_CMD; 
            cmd_buffer->cmd.log_rgs_data.data.rgs_retrieval_data = \
                    *((tcxomgr_log_rgs_retrieval_data_struct_type *)log_rgs);
            log_data = TRUE;
          }
#endif /* LOG_TCXOMGR_RGS_RETRIEVAL_C */
          break;
        }
        
      case TCXOMGR_AGED_RGS_RETRIEVAL_LOG_CMD:
        {
          /* Log rgs only if the it is enabled, this way we will prevent unnecessary 
             task switches */
#ifdef LOG_TCXOMGR_RGS_RETRIEVAL_C
          if ( log_status(LOG_TCXOMGR_RGS_RETRIEVAL_C) != FALSE)
          {
            cmd_buffer->cmd.log_rgs_data.type = TCXOMGR_AGED_RGS_RETRIEVAL_LOG_CMD; 
            cmd_buffer->cmd.log_rgs_data.data.aged_rgs_retrieval_data = \
                  *((tcxomgr_log_aged_rgs_retrieval_data_struct_type *)log_rgs);
            log_data = TRUE;
          }
#endif /* LOG_TCXOMGR_RGS_RETRIEVAL_C */
          break;
        }
      
      default:
        DBG_1(ERROR,"Invalid RGS log command type: %d", rgs_log_type);
        break;
    }
    
    if ( log_data != FALSE )
    {
      tcxomgr_send_cmd(cmd_buffer);
    }
    else
    {
      tcxomgr_free_cmd_buf(cmd_buffer);
      cmd_buffer = NULL;
    }
  }
  else
  {
    /* queue is full, just drop the log packet */
    DBG_0(ERROR,"XO Field Cal Logging: Command Buffer is full");
  }
}

/*============================================================================

FUNCTION TCXOMGR_LOG_RGS_TO_DIAG_FLUSH

DESCRIPTION
  This function is called by 
  1. tcxomgr_task when it has recieved 10 RGS log packets
  2. tcxomgr_task when field calibration is performed

DEPENDENCIES
    None     

RETURN VALUE
    None
 
SIDE EFFECTS
    None

============================================================================*/
void tcxomgr_log_rgs_to_diag_flush(void)
{
  /* If we have atleast one packet to log, flush it */
  if ( tcxomgr_rgs_log.log_rgs_data.num_packets >= TCXOMGR_LOG_RGS_PACKETS ) 
  {
    /* send the data to diag */
    tcxomgr_diag_send_rgs_log( &tcxomgr_rgs_log.log_rgs_data,
            tcxomgr_rgs_log.rgs_log_id++);
            
    /* clear out the memory  */
    memset(&tcxomgr_rgs_log.log_rgs_data, 0, 
            sizeof(tcxomgr_rgs_log.log_rgs_data) );
  }
  
  /* If we have atleast one packet to log, flush it */
  if ( tcxomgr_rgs_log.log_rgs_retrieval_data.num_packets >= TCXOMGR_LOG_RGS_PACKETS ) 
  {
    /* send the data to diag */
    tcxomgr_diag_send_rgs_retrieval_log( &tcxomgr_rgs_log.log_rgs_retrieval_data,
            tcxomgr_rgs_log.rgs_retrieval_log_id++);
            
    /* clear out the memory  */
    memset(&tcxomgr_rgs_log.log_rgs_retrieval_data, 0, 
            sizeof(tcxomgr_rgs_log.log_rgs_retrieval_data) );
  }
  
  /* If we have atleast one packet to log, flush it */
  if ( tcxomgr_rgs_log.log_aged_rgs_retrieval_data.num_packets >= TCXOMGR_LOG_RGS_PACKETS ) 
  {
    /* send the data to diag */
    tcxomgr_diag_send_aged_rgs_retrieval_log( &tcxomgr_rgs_log.log_aged_rgs_retrieval_data,
            tcxomgr_rgs_log.aged_rgs_retrieval_log_id++);
            
    /* clear out the memory  */
    memset(&tcxomgr_rgs_log.log_aged_rgs_retrieval_data, 0, 
            sizeof(tcxomgr_rgs_log.log_aged_rgs_retrieval_data) );
  }
}

/*============================================================================

FUNCTION TCXOMGR_LOG_RGS_TO_DIAG

DESCRIPTION
    Helper function to copy the factory cal data to the log structure. 

DEPENDENCIES
    None     

RETURN VALUE
    None
 
SIDE EFFECTS
    None

============================================================================*/
void tcxomgr_log_rgs_to_diag
(
  tcxomgr_log_rgs_cmd_type* log_rgs
)
{
  tcxomgr_log_rgs_struct_type*                 log_rgs_data_ptr = \
          &tcxomgr_rgs_log.log_rgs_data;
  tcxomgr_log_rgs_retrieval_struct_type*       log_rgs_retrieval_data_ptr = \
          &tcxomgr_rgs_log.log_rgs_retrieval_data;
  tcxomgr_log_aged_rgs_retrieval_struct_type*  log_aged_data_ptr = \
          &tcxomgr_rgs_log.log_aged_rgs_retrieval_data;          

  if ( log_rgs != NULL )
  {
    /* Store the RGS data to database */
    switch (log_rgs->type)
    {
      case TCXOMGR_RGS_UPDATE_LOG_CMD:
        {
	        log_rgs_data_ptr->rgs_data[log_rgs_data_ptr->num_packets++] = log_rgs->data.rgs_data;
	        break;
        }
      
      case TCXOMGR_RGS_RETRIEVAL_LOG_CMD:
        {
	        log_rgs_retrieval_data_ptr->rgs_data[log_rgs_retrieval_data_ptr->num_packets++] = \
									log_rgs->data.rgs_retrieval_data;
          break;
        }
        
      case TCXOMGR_AGED_RGS_RETRIEVAL_LOG_CMD:
        {
	        log_aged_data_ptr->rgs_data[log_aged_data_ptr->num_packets++] = \
									log_rgs->data.aged_rgs_retrieval_data;
          break;
        }

			default:
      {
        DBG_1(ERROR,"Invalid RGS log command type: %d", log_rgs->type);
        break;
      }
    }
  }
  
  /* Flush log packets if required */
  tcxomgr_log_rgs_to_diag_flush();
}

/*============================================================================

FUNCTION TCXOMGR_SCHEDULE_RGS_UPDATE

DESCRIPTION
    Helper function to schedule rgs update function in tcxomgr task queue 

DEPENDENCIES
    None     

RETURN VALUE
    None
 
SIDE EFFECTS
    None

============================================================================*/
void tcxomgr_schedule_rgs_update
(
  tcxomgr_update_rgs_struct_type *rgs_data
)
{
  tcxomgr_cmd_type *cmd_buffer; 

  /* Get a tcxomgr command buffer and send it away */
  cmd_buffer = tcxomgr_get_cmd_buf();

  /* Make sure we got a buffer */
  if (cmd_buffer != NULL) 
  {
    cmd_buffer->cmd_id = TCXOMGR_CMD_RGS_UPDATE;
    cmd_buffer->cmd.rgs_data.data_to_update = *rgs_data;
    tcxomgr_send_cmd(cmd_buffer);
  }
  else
  {
    /* queue is full*/
    DBG_0(ERROR,"XO rgs data update: Command Buffer is full");
  }
}
