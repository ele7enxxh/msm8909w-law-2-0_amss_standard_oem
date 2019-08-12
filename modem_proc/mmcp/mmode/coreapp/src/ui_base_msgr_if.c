/*===========================================================================

                         D S _ Q M I _ V O I C E _MSGR_IF . C

DESCRIPTION

 The Data Services Qualcomm Voice service Messenger interface source file.

EXTERNALIZED FUNCTIONS     

Copyright (c) 2004-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/coreapp/src/ui_base_msgr_if.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/2313         Created Module.
===========================================================================*/

/*===========================================================================
 
  INCLUDE FILES FOR MODULE
 
===========================================================================*/
#include "ui_base_msgr_if.h"
#include "msgr_rex.h"
#include "csg_app_cm_if.h"

/*===========================================================================
            
                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*========================================================================
  FUNCTION qmi_voice_msgr_handler

  DESCRIPTION
    process messages received from msgr

  PARAMETERS
    msg pointer in *void. cast to qmi_mmodei_msgr_type before use.
    *void type is to avoid circular reference of headers

  RETURN VALUE
    None
===========================================================================*/
void ui_base_msgr_if_msgr_handler( void *param )
{

  ui_base_msgr_type       *msg;

  ASSERT( param );

  msg = (ui_base_msgr_type*)param;
  TUI_MSG_HIGH_1("qmi_voice_msgr_handler cmd id = %d", msg->cmd.hdr.id);

  switch ( msg->cmd.hdr.id )
  {
    case QMI_NAS_CSG_SEARCH_SELECTION_CONFIG_CMD:
      csg_app_search_select_config_handler( &msg->cmd.tui_cmd.csg_srch_sel_cfg_cmd);
      break;

    case QMI_NAS_CSG_IMMEDIATE_SEARCH_SELECTION_CMD:
      csg_app_immediate_search_handler( &msg->cmd.tui_cmd.csg_immed_search.csg_immediate_search_select_req);
      break;

    case MM_CM_CSG_SEARCH_TRIGGER_IND:
      csg_app_search_trigger_handler( &msg->cmd.tui_cmd.csg_srch_trigger);
      break;

    default:
      break;
    }
  }

/*========================================================================
  FUNCTION qmi_voice_msgr_register

  DESCRIPTION
    register to msgr messages

  PARAMETERS
    msgr client object pointer

  RETURN VALUE
    None
===========================================================================*/
void ui_base_msgr_register( msgr_client_t *msgr_client )
{
  errno_enum_type err;

  err = msgr_register( MSGR_MM_TUI, msgr_client, MSGR_ID_REX, QMI_NAS_CSG_SEARCH_SELECTION_CONFIG_CMD );
  if(err != E_SUCCESS)
  {
    TUI_MSG_HIGH_1("TUI : QMI_NAS_CSG_SEARCH_SELECTION_CONFIG_CMD register ret %d", err);
  }

  err = msgr_register( MSGR_MM_TUI, msgr_client, MSGR_ID_REX, QMI_NAS_CSG_IMMEDIATE_SEARCH_SELECTION_CMD );
  if(err != E_SUCCESS)
  {
    TUI_MSG_HIGH_1("TUI : QMI_NAS_CSG_IMMEDIATE_SEARCH_SELECTION_CMD register ret %d", err);
  }

  err = msgr_register( MSGR_MM_TUI, msgr_client, MSGR_ID_REX, MM_CM_CSG_SEARCH_TRIGGER_IND);
  if(err != E_SUCCESS)
  {
    TUI_MSG_HIGH_1("TUI : MM_CM_CSG_SEARCH_TRIGGER_IND register ret %d", err);
  }

  ASSERT( err == E_SUCCESS );
}


/*===========================================================================

FUNCTION QMI_MMODEI_PROCESS_MSGR()

DESCRIPTION
  This function is called in response to QMI_MMODE_TASK_MSGR_SIGNAL. It 
  processes the commands from the DCC msgr queue.

DEPENDENCIES
  An item must be on the QMI_MMODE msgr queue.

RETURN VALUE
  none

SIDE EFFECTS
  None.

===========================================================================*/
void ui_base_msgr_if_process_msgr
(
  void
)
{
  ui_base_msgr_type  * msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while ((msg = q_get(&ui_base_msgr_q)) != NULL)
  {
    /*-----------------------------------------------------------------------
      Do not manipulate msg contents. Other services may handle same message.
      So do not change the message. And do not free the item back to free q.
      Freeing q should be done at the end of this section. Again, this is due
      to possibility of same message being handled by multiple services.
    -----------------------------------------------------------------------*/
    ui_base_msgr_if_msgr_handler( (void*)msg );

    msgr_rex_free_msg_buf(&(msg->link));
  }
} /* dcci_process_msgr() */


