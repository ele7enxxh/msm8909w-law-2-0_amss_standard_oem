/*!
  @file
  qsh_str.h

  @brief
  Contains string lookup table declarations.
*/

/*==============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/qsh/backend/qsh_str.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/29/15   ca      CR:935478: cmapi report, mini dump state collet,
                   Re-arch of rule management validation. 
08/28/15   ca      CR 898746: QSH rule management feature.
06/20/14   ar      Moved qsh_crit_sect_enter() and qsh_crit_sect_leave() to 
                   qsh.c file
05/23/14   mm      Added standardized analysis summary
03/12/14   vd      CR 630063: initial version                   
==============================================================================*/

#ifndef QSH_STR_H
#define QSH_STR_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <qsh_types.h>
#include <qsh_client_rule.h>

/*==============================================================================

                         EXTERNAL VARIABLES

==============================================================================*/

/*! Client names as strings
*/
extern const char * qsh_client_str[QSH_CLT_MAX];

/*! Client names padded to maximum length (crashscope won't pad with %-*s)
*/
extern const char * qsh_client_str_pad[QSH_CLT_MAX];

/*! Category names padded to maximum length
*/
extern const char * qsh_cat_str_pad[QSH_CAT_MAX_IDX];

/*! Messages types padded to maximum length
*/
extern const char * qsh_msg_type_str_pad[QSH_MSG_TYPE_MAX];

/*! Action type as strings */
extern const char * qsh_action_str[QSH_ACTION_MAX_IDX];

/*! Callback mode as strings 
*/
extern const char * qsh_action_mode_str[QSH_ACTION_MODE_MAX];

/*! Callback mode done as strings 
*/
extern const char * qsh_action_mode_done_str[QSH_ACTION_MODE_DONE_MAX];

/*! Client rule type as strings */
extern const char * qsh_client_rule_type_str[QSH_CLIENT_RULE_TYPE_MAX];

/*! DIAG aaction type as strings */
extern const char * qsh_client_rule_action_type_str[QSH_CLIENT_RULE_ACTION_MAX_IDX + 1];

#endif /* QSH_STR_H */
