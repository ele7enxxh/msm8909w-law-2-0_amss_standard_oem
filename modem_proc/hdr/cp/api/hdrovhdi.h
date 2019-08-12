#ifndef HDROVHDI_H
#define HDROVHDI_H

/*===========================================================================

          O V E R H E A D   M E S S A G E S   P R O T O C O L
       I N T E R N A L   D E F I N T I O N S   H E A D E R   F I L E

DESCRIPTION
  This contains the declarations for the Overheade Messages Protocol (OVHD).

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrovhdi.h_v   1.15   05 Sep 2001 09:14:38   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrovhdi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
       
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/14   cnx     Support NV refresh without reset.
10/10/11   cnx     Added hdrovhd_reset_tunnel_info().
07/27/11   cnx     Merged Optimized Handoff changes.
06/10/10   pba     IRAT Measurement feature.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/24/09   wsh     CMI: updated included headers due to header split
02/01/09   wsh     Merged BCMCS 2.0 on to 1H09
12/23/08   pba     Support for Quick Idle Protocol
03/03/08   wsh     DCM optimization: not closing session unnecessarily when
                   moving across subnet
11/03/06   wsh     Clean up the way timers are handled in ovhd protocol
07/06/06   mpa     Added hdrovhd_clear_timers().
08/28/05   mpa     (for dna) Added HDROVHD_REPARSE_BC_MSG_CMD
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
05/09/01   vas     Re-naming for consistency
04/19/01   vas     DDR Changes. Added macros for capsule_length_max in AP msg.
                   Featurized under FEATURE_IS856_NO_DDR
03/21/01   vas     Changed supervision timeouts for IS856 from 4.8 to 5.12 secs
03/21/01   dna     Fixes for QC and SP message unpacking.
09/07/00   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "hdrovhd.h"
#include "hdraddr_v.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#ifdef FEATURE_LTE_DEPRIORITIZATION
#include "hdrcp_msg.h"
#endif /* FEATURE_LTE_DEPRIORITIZATION */

/* <EJECT> */
/*=============================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*---------------------------------------------------------------------------
                           OVHD PROTOCOL COMMANDS

   The overhead protocol can receive the following commands.
---------------------------------------------------------------------------*/


/********** Command types **********/

/* 
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

typedef enum
{
   HDROVHD_ACTIVATE_CMD          = 0,  /* Activates the overhead protocol         */
   HDROVHD_DEACTIVATE_CMD        = 1,  /* Deactivates overhead protocol           */
   HDROVHD_WAKEUP_CMD            = 2,  /* Wakes Up OVHD from sleep state          */
   HDROVHD_SLEEP_CMD             = 3,  /* Puts OVHD in Sleep state                */
   HDROVHD_RETURN_TO_IDLE_CMD    = 4,  /* ALMP returning to idle                  */
   HDROVHD_REPARSE_BC_MSG_CMD    = 5,  /* Re-Parse the BroadcastOverhead message  */
   HDROVHD_BOM_CACHING_SETUP_CMD = 6,  /* Setup BOM Caching rules             */
   HDROVHD_AN_REDIRECTED_CMD     = 7,  /* Process AN redirected                  */
   HDROVHD_CHECK_CONFIG_CMD      = 8,  /* Check overhead cache information          */
   HDROVHD_TUNNEL_DEACTIVATE_CMD = 9   /* Exit tunnel state */
} hdrovhd_cmd_name_enum_type;


/********** Common command structure **********/
typedef struct
{
  hdrsrch_link_id_type  link_id;
  sys_link_type link;
  uint8 config_change;

} hdrovhd_config_change_cmd_type;

typedef struct
{
  sys_bcmcs_bom_caching_e_type setup;
  uint8  		stale_factor;
} hdrovhd_bom_caching_params_type;

typedef union
{
  hdrovhd_bom_caching_params_type bom_caching;
  hdrovhd_config_change_cmd_type config_change; /* ConfigurationChange field in QuickPage */
} hdrovhd_cmd_params_union_type;

typedef struct
{
  hdrovhd_cmd_name_enum_type name;       /* Name of the command      */
  hdrhai_protocol_name_enum_type sender; /* protocol giving cmd, for debug */
  hdrovhd_cmd_params_union_type params; /* Common command fields */
} hdrovhd_cmd_type;

/* <EJECT> */

/*=============================================================================

                  LOCAL FUNCTION DECLARATIONS FOR MODULE

=============================================================================*/

/*===========================================================================

FUNCTION HDROVHD_PROCESS_CMD

DESCRIPTION
  This function processes commands given to OVHD.

DEPENDENCIES
  None

PARAMETERS
  ovhd_cmd_ptr - Pointer to the command for OVHD to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrovhd_process_cmd
( 
  hdrovhd_cmd_type *ovhd_cmd_ptr 
);

/*===========================================================================

FUNCTION HDROVHD_PROCESS_IND

DESCRIPTION
  This function processes indications given to OVHD.

DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Name of the indication for OVHD to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrovhd_process_ind
(
  hdrind_ind_name_enum_type   ind_name, 
  hdrind_ind_data_union_type *ind_data_ptr 
);

/*===========================================================================

FUNCTION HDROVHD_PROCESS_MSG

DESCRIPTION
  This function processes messages for OVHD.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrovhd_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);

/*===========================================================================
FUNCTION HDROVHD_RESTART_TIMERS

DESCRIPTION
  This function resets & starts the QuickConfig & SectorParameters 
  supervision timers. It resets & restarts the AccessParameters timer only
  if it were already running
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
extern void hdrovhd_restart_timers( void );

/*===========================================================================
FUNCTION HDROVHD_STOP_TIMERS

DESCRIPTION
  This function stops all OVHD timers ( QC, SP & AP ) and clears the
  associated signals.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
extern void hdrovhd_stop_timers( void );

/*===========================================================================
FUNCTION HDROVHD_CLEAR_TIMERS

DESCRIPTION
  This function resets all saved supervision times ( QC, SP ) and stops all
  the supervision timers.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
extern void hdrovhd_clear_timers( void );

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
/* <EJECT> */
/*=============================================================================
FUNCTION HDROVHD_DCM_REACQ_DIFF_SUBNET

DESCRIPTION
  This function informs OVHD that AT has reacq-ed on a different
  subnet during DCM mode.

DEPENDENCIES
  None.
  
PARAMETERS
  None
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrovhd_dcm_reacq_diff_subnet( void );
#endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */


/*=============================================================================
FUNCTION HDROVHD_RESET_INFO_STATE

DESCRIPTION
  This function resets OVHD INFO state to NOT_UPDATED.

DEPENDENCIES
  None.
  
PARAMETERS
  None
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrovhd_reset_info_state( void );


/*=============================================================================
FUNCTION HDROVHD_RESET_TUNNEL_INFO

DESCRIPTION
  This function processes reset hdrovhd.tunnel_info.

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrovhd_reset_tunnel_info( void );

#ifdef FEATURE_LTE_DEPRIORITIZATION
/*=============================================================================

FUNCTION HDROVHD_UPDATE_LTE_DEPRI_FREQ

DESCRIPTION
  This function updates LTE deprioritized frequencies. 

DEPENDENCIES
  None.

PARAMETERS
  depri_req - pointer to deprioritize request structure
  is_online - whether HDR is online.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrovhd_update_lte_depri_freq 
( 
  hdrcp_lte_depri_freq_req_t *depri_req,
  boolean is_online
);
#endif /* FEATURE_LTE_DEPRIORITIZATION */

#endif /* HDROVHDI_H */
