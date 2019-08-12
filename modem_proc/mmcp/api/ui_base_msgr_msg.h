#ifndef UI_BASE_MSGR_MSG_H
#define UI_BASE_MSGR_MSG_H
/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/ui_base_msgr_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/14/12   ndanturi      Provided config request support to CMSDS
==============================================================================*/


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include <msgr.h>
#include <msgr_umid.h>


/*==============================================================================
                           Module definitions
==============================================================================*/
#define MSGR_MODULE_TUI    0x0A
#define MSGR_MM_TUI       MSGR_TECH_MODULE( MSGR_TECH_MM, MSGR_MODULE_TUI )

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{

  UI_BASE_SYS_CSG_SELECTION_CONFIG_NONE =  -1,  /*FOR INTERNAL USE ONLY!*/

  UI_BASE_SYS_CSG_SELECTION_CONFIG_1,

  UI_BASE_SYS_CSG_SELECTION_CONFIG_MAX          /*FOR INTERNAL USE ONLY!*/

}ui_base_csg_selection_config_e_type;


typedef struct
{
  uint8 asubs_id;
  ui_base_csg_selection_config_e_type  select_config;
} ui_base_msgr_msg_csg_select_config_s_type;


typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  ui_base_msgr_msg_csg_select_config_s_type csg_sel; 

} ui_base_msgr_msg_csg_select_config_cmd_msg_type;



/*==============================================================================
                           Msg UMID definitions
==============================================================================*/

/*! @brief UMIDs of the messages sent by the QMI to other modules
*/
enum
{
  MM_TUI_IND_FIRST = MSGR_UMID_BASE(MSGR_MM_TUI, MSGR_TYPE_IND),

  /* MM_TUI_CSG_SELECTION_CMD  - Interface to pass conference participants info */
  MSGR_DEFINE_UMID(MM, TUI, CMD, CSG_SELECTION, 0x01, ui_base_msgr_msg_csg_select_config_cmd_msg_type),

  MM_TUI_IND_MAX,
  MM_TUI_IND_LAST = MM_TUI_IND_MAX - 1
};

#endif /* QMI_MMODE_MSGR_MSG_H */


