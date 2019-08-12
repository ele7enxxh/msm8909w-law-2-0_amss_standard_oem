#ifndef CMSTATS_H
#define CMSTATS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  C A L L   M A N A G E R   S T A T S  M O D U L E

GENERAL DESCRIPTION

  This module contains the Call Manager Modem Statistics module.

  The Statistics object is responsible for:
  1. Processing client's required statistics information
  2. Processing modem statistics information received from protocol
     and passing it to clients

EXTERNALIZED FUNCTIONS

  

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2004 - 2009 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmstats.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/11   rm      LTE Modem statistics
05/24/10   rm      Correcting featurisation of FEATURE_MODEM_STATISTICS
03/17/10   rm      Adding Modem Statistics changes for 1x/HDR
10/09/09   rm      Initial release
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"       /* system wide common types */
#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"
#ifdef CM_GW_SUPPORTED
#include "cmwll.h"
#endif

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/


/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Macros
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/
#ifdef CM_FTD_DEBUG
#error code not present
#endif

/*===========================================================================

FUNCTION cmstats_process_stats_signal

DESCRIPTION
  Process stats signal set for respective protocol module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmstats_process_stats_signal(
  sys_modem_module_e_type,
  sys_modem_as_id_e_type asubs_id);  


/**--------------------------------------------------------------------------
** Functions - external
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmstats_client_cmd_proc

DESCRIPTION
  Process clients' Statistics commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmstats_client_cmd_proc(

  cm_stats_cmd_s_type  *stats_cmd_ptr
    /* pointer to a CM command
    */
);

/*===========================================================================

FUNCTION cmstats_ptr

DESCRIPTION
  Return a pointer to the one and only stats object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern sys_modem_stats_info_s_type  *cmstats_ptr( void );

/*===========================================================================

FUNCTION cmstats_update_stats_and_post_event

DESCRIPTION
  Updates the stats structure with changed field and posts STATS event 
  to clients.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmstats_update_stats_and_post_event (

   uint32 bit_mask_category
   /* Bit mask modified */
);

/*===========================================================================

FUNCTION cmstats_map_cm_prl_pref_to_sys_prl_pref

DESCRIPTION
  Maps CM prl pref to sys prl pref

DEPENDENCIES
  None

RETURN VALUE
  sys_stats_prl_pref_e_type

SIDE EFFECTS
  None

===========================================================================*/
sys_stats_prl_pref_e_type cmstats_map_cm_prl_pref_to_sys_prl_pref (
  
  cm_prl_pref_e_type  cm_prl_pref
  /* CM PRL preference */
);

/*===========================================================================

FUNCTION cmstats_increment_send_key_press_count

DESCRIPTION
  Increment the number of times the user had originated a call
  
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmstats_increment_send_key_press_count ( void );

#ifdef FEATURE_CM_LTE
/*===========================================================================

FUNCTION cmstats_get_lte_bit_mask

DESCRIPTION
  Returns the bit mask set for LTE

DEPENDENCIES
  none

RETURN VALUE
  uint64

SIDE EFFECTS
  none

===========================================================================*/
uint64 cmstats_get_lte_bit_mask( void );

/*===========================================================================

FUNCTION cmstats_process_lte_stats_info

DESCRIPTION
  Process the LTE stats command

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmstats_process_lte_stats_info( void );
#endif

#ifdef CM_GW_SUPPORTED
/*===========================================================================

FUNCTION cmstats_process_nas_stats_per_subs_stack_info

DESCRIPTION
  Read updated stats info from NAS and send teh data to clients.
  Report CM_STATS_EVENT_MODEM_INFO to clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cmstats_process_nas_stats_per_subs_stack_info(

  sys_modem_as_id_e_type    asubs_id,
  sys_modem_stack_id_e_type stack_id
);
#endif
#endif /* CMSTATS_H */

