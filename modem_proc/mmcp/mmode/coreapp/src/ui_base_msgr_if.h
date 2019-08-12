#ifndef _UI_BASE_MSGR_IF_H
#define _UI_BASE_MSGR_IF_H
/*===========================================================================

                         UI_BASE_ MSGR _ I F . H

DESCRIPTION

 The Data Services Qualcomm Voice service Messenger interface header file.


EXTERNALIZED FUNCTIONS



Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/coreapp/src/ui_base_msgr_if.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/23/13    vs     Initial revision.
===========================================================================*/

#include "qmi_mmode_msgr_msg.h"
#include "ui_base_msgr_msg.h"
#include "cm_msgr_msg.h"
#include "queue.h"
#include "msg.h"
#include "assert.h"
#ifdef CM_UNIT_TEST
  #error code not present
#endif

q_type ui_base_msgr_q;

/*===========================================================================

                    RESPONSES

===========================================================================*/




/*===========================================================================

                    INDICATIONS

===========================================================================*/



/*===========================================================================

                   CALLBACKS

===========================================================================*/

// Messages QMI_VOICE is expecting
union ui_base_msgr_msg_u
{
  mmode_qmi_nas_csg_search_select_config_cmd_msg_type   csg_srch_sel_cfg_cmd;
  mmode_qmi_nas_csg_immediate_search_select_cmd_msg_type  csg_immed_search;
  cm_csg_srch_trigger_ind_s_type csg_srch_trigger;
};

/*===========================================================================
TYPEDEF ui_base_msgr_type
 
DESCRIPTION 
  TUI Task MSGR queue data type
===========================================================================*/
typedef struct
{
  q_link_type       link;           /* For linking onto msgr queue         */  

  union ui_base_msgr_cmd_u
  {
    msgr_hdr_s hdr;   /* Message router header */

    union  ui_base_msgr_msg_u tui_cmd; // contains list of msgs TUI is registered to
    // MSGR support for other Thin UI task go here

  } cmd;
} ui_base_msgr_type;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


void ui_base_msgr_if_msgr_handler( void *param );

void  ui_base_msgr_register( msgr_client_t *msgr_client );

void ui_base_msgr_if_process_msgr( void );


#endif /* #define _UI_BASE_MSGR_IF_H*/
