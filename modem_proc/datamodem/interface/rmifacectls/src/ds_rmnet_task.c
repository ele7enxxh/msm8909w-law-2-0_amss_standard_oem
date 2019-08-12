/*===========================================================================

                         D S _ R M N E T _ T A S K . C

DESCRIPTION

 The Data Services Rm Network Interface Task source file.

EXTERNALIZED FUNCTIONS

        
Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/src/ds_rmnet_task.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
03/01/11    kk     Removing DCC command buffer dependency from RmNet.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
10/01/07    mct    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ds_rmnet_sm_ext.h"
#include "ds_rmnet_smi.h"
#include "dcc_task_defs.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_qmi_svc.h"
#include "ps_system_heap.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

#define rmnet_smi_free_cmd_buf(buf_ptr) PS_SYSTEM_HEAP_MEM_FREE(buf_ptr)


/*===========================================================================
  FUNCTION RMNET_PROCESS_CMD()

  DESCRIPTION
    This function processes a RMNET cmd.

    It is called by the serving task main loop and will dispatch the
    approprate cmd handler function.

  PARAMETERS
    cmd          :  RmNet command to be processed
    user_data_ptr:  private data buffer containing the Rmnet command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    RMNET module must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_sm_process_cmd
(
  dcc_cmd_enum_type cmd,
  void              *user_data_ptr
)
{
  rmnet_smi_cmd_type * cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  cmd_ptr = (rmnet_smi_cmd_type *)user_data_ptr;
  ASSERT (cmd_ptr && (cmd == DCC_RMNET_SM_CMD) && (cmd_ptr->sm_id < RMNET_MAX_SM));

  switch (cmd_ptr->sm_id)
  {
    case RMNET_META_SM:
      rmnet_meta_sm_process_cmd(cmd, user_data_ptr); 
      break;
  
    case RMNET_V4_SM:
      rmnet_v4_sm_process_cmd(cmd, user_data_ptr);
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case RMNET_V6_SM:
      rmnet_v6_sm_process_cmd(cmd, user_data_ptr);
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      LOG_MSG_INFO1_1("Ignoring RMNET cmd for SM %d", cmd_ptr->sm_id);
      break;
  }

  rmnet_smi_free_cmd_buf(user_data_ptr);
} /* rmnet_process_cmd() */


/*===========================================================================
  FUNCTION RMNET_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM event cmd.

    It is called by the serving task main loop and will dispatch the
    appropriate event handler function.

  PARAMETERS
    cmd_ptr:  serving task command buffer containing the RmNet sm
              event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_sm_process_ev_cmd
(
  dcc_cmd_enum_type cmd,
  void              *user_data_ptr
)
{
  rmnet_smi_ev_cmd_type * cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_ptr = (rmnet_smi_ev_cmd_type *)user_data_ptr;
  ASSERT (cmd_ptr && (cmd == DCC_RMNET_SM_EV_CMD) && (cmd_ptr->sm_id < RMNET_MAX_SM));

  switch (cmd_ptr->sm_id)
  {
    case RMNET_META_SM:
      rmnet_meta_sm_process_ev_cmd(cmd, user_data_ptr); 
      break;
  
    case RMNET_V4_SM:
      rmnet_v4_sm_process_ev_cmd(cmd, user_data_ptr);
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case RMNET_V6_SM:
      rmnet_v6_sm_process_ev_cmd(cmd, user_data_ptr);
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      LOG_MSG_INFO1_1("Ignoring RMNET event cmd for SM %d", cmd_ptr->sm_id);
      break;
  }

  rmnet_smi_free_cmd_buf(user_data_ptr); 
} /* rmnet_v4_sm_process_ev_cmd() */

/*===========================================================================
  FUNCTION RMNET_SMI_GET_CMD_BUF()

  DESCRIPTION
    This function allocates a new RmNet SM command buffer

  PARAMETERS
    cmd_id : the command type i.e. DCC_RMNET_SM_CMD or DCC_RMNET_SM_EV_CMD
    filename: filename of caller.
    line_num: line number of caller.
    grceful: gracefull or graceless handling.

  RETURN VALUE
    cmd_ptr - pointer to the newly allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void *rmnet_smi_get_cmd_buf
(
  dcc_cmd_enum_type cmd_id,
  const char* filename,
  uint32      line_num,
  boolean     graceful
)
{
  rmnet_smi_cmd_type *cmd_ptr;
  rmnet_smi_ev_cmd_type *ev_cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  cmd_ptr = NULL;
  ev_cmd_ptr = NULL;

  MSG_SPRINTF_3(MSG_SSID_DS, MSG_LEGACY_MED,
			"allocate memory in file (%s) line (%d) for cmd_id (%d) ",
 	 	                   filename,line_num,cmd_id);
  switch(cmd_id)
  {
    case DCC_RMNET_SM_CMD:
    {
      PS_SYSTEM_HEAP_MEM_ALLOC(cmd_ptr, sizeof(rmnet_smi_cmd_type),
                               rmnet_smi_cmd_type*);
      if(cmd_ptr == NULL && graceful == FALSE)
      {
        ASSERT(0);
      }
      return ((void *)cmd_ptr);
    }
    case DCC_RMNET_SM_EV_CMD:
    {
      PS_SYSTEM_HEAP_MEM_ALLOC(ev_cmd_ptr, sizeof(rmnet_smi_ev_cmd_type),
                               rmnet_smi_ev_cmd_type*);
      if(ev_cmd_ptr == NULL && graceful == FALSE)
      {
        ASSERT(0);
      }
      return ((void *)ev_cmd_ptr);
    }

    default:
    {
      break;
    }
  }

  return NULL;
} /* rmnet_sm_get_cmd_buf() */
