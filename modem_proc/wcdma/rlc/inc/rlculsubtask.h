#ifndef RLCULSUBTASK_H
#define RLCULSUBTASK_H
/*===========================================================================

          U P L I N K   R L C : S U B T A S K   H E A D E R   F I L E 

                
GENERAL DESCRIPTION
  
  This module contains the declaration of the functions and data types
  defined in downlink RLC subtask file.
  
Copyright (c) 1992-2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2008-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/inc/rlculsubtask.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/24/12   grk     Fixed compiler warnings    
05/11/09   ssg     Updated Copyright Information
05/21/01   rc      Cleaned up code.
11/22/00   rc      Created file.

===========================================================================*/

/*===========================================================================

FUNCTION PROC_UL_RLC_COMMANDS

DESCRIPTION
  Processes Uplink RLC Commands.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void proc_ul_rlc_commands 
(   
  l2_ul_cmd_enum_type     cmd_id,             /* Command ID                */  
  l2_ul_cmd_data_type     *cmd_data_ptr       /* Pointer to command data   */    
);

#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
/*===========================================================================

FUNCTION PROC_WCDMA_UL_RLC_COMMANDS

DESCRIPTION
  Processes Uplink RLC Commands.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void proc_wcdma_ul_rlc_commands 
(   
  wcdma_l2_ul_cmd_enum_type     cmd_id,             /* Command ID                */  
  wcdma_l2_ul_cmd_data_type     *cmd_data_ptr       /* Pointer to command data   */    
);
#endif
#endif  /* RLCULSUBTASK_H */


