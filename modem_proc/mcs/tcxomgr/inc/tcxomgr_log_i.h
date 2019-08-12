#ifndef TCXOMGR_LOG_I_H
#define TCXOMGR_LOG_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                           T C X O    M A N A G E R

                           TCXO Manager Header File

GENERAL DESCRIPTION

  This module maintains the internal definitions for the txcomgr subsystem

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/inc/tcxomgr_log_i.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
--------     ---     --------------------------------------------------------- 
12/11/12     mn       New log packet for Retrieve RGS.

============================================================================*/

#include "tcxomgr.h"
#include "tcxomgr_i.h"

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
);

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
);

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
);

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
void tcxomgr_log_rgs_to_diag_flush(void);

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
);

#endif /* !TCXOMGR_LOG_I_H */