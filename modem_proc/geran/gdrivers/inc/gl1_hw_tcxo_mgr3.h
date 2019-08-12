#ifndef GL1_HW_TCXO_MGR3_H
#define GL1_HW_TCXO_MGR3_H
/*===========================================================================

                    L 1  TCXO  M A N A G E R  
                    
DESCRIPTION
  This file contains global declarations and external references
  for the new multi RAT TCXO Manager.

EXTERNALIZED FUNCTIONS
  gl1_hw_tcxo_client_vco_cb


INTERNALIZED FUNCTIONS
  gl1_hw_request_tcxo_mgr_3
  gl1_hw_tcxo_frozen
  gl1_hw_tcxo_client_grant_cb
  gl1_hw_tcxo_client_change_cb
  gl1_hw_tcxo_client_release_cb
  gl1_hw_tcxo_client_deny_cb
  gl1_hw_tcxo_client_notify_cb

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_hw_tcxo_mgr3.h#1 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/15   am      CR828682 Enable Rpush update from all G clients
16/09/13   cs      CR503042 Updated XO RGS handling for Triton
28/05/09   cs      Updates to avoid spurious restriction update when TCXO released
10/07/08   ip      Added protype gl1_hw_tcxo_change_source
11/06/07   cs      Fix for FTM TCXO release in sweep test
08/06/07   cs      Tidy FEU debug and remove conditional compile
16/05/07   cs      Improvements to manage state change better for feu
04/05/07   cs      Updates for TCXO restriction support and rotator push
20/03/07   cs      Update to support restriction management
12/02/07   agv     Added TCXO Mgr 3.0 Rotator Push for GPS.
04/12/06   cs      Modified tcxo manager
05/10/06   agv     Initial Version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "comdef.h"
#include "tcxomgr.h"

#include "nas_exp.h"

#include "geran_dual_sim.h"
#include "geran_multi_sim.h"

/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION     GL1_HW_REQUEST_TCXO_MGR_3

DESCRIPTION
  This function requests TCXO Manager 3 before GSM can 
  attempt to acquire.

DEPENDENCIES
  None.

PARAMETERS
  ( NOTE : Active task can be GSM of FTM )
  rex_tcb_type  *                   - Active task tcb
  void         (*task_wait_handler) - Active task handler
  rex_sigs_type                     - Active task wait signal
  rex_sigs_type                     - Active task complete sig

RETURN VALUE
  None needed for XO.

SIDE EFFECTS
  None.
===========================================================================*/                                         
void gl1_hw_request_tcxo_mgr_3( rex_tcb_type  *task_ptr,
                                   void (*task_wait_handler)(rex_sigs_type, gas_id_t),
                                   rex_sigs_type task_wait_sig,
                                   rex_sigs_type task_done_sig,
                                   gas_id_t gas_id ); 

/*===========================================================================
FUNCTION     GL1_HW_MAP_GAS_ID_TO_TCXO_ASID

DESCRIPTION
  Set the correct TCXO asid for the chosen gas_id.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TCXO asid

SIDE EFFECTS
  None.
===========================================================================*/
tcxomgr_as_id_enum_type gl1_hw_map_gas_id_to_tcxo_asid( gas_id_t gas_id );

/*===========================================================================
FUNCTION     gl1_hw_tcxo_release

DESCRIPTION
  Releases control of the TCXO.

DEPENDENCIES
  None.

PARAMETERS
  Active client gas_id.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void gl1_hw_tcxo_release( gas_id_t gas_id );

/*===========================================================================
FUNCTION     GL1_TCXO_RPUSH_FLAG_CB

DESCRIPTION
  Invoked by TCXO Manager when the rotator push flag changes. For GSM the
  timing of the Push is implicit in the AFC Algorithm update, and all
  the Call Back is telling us is if we send the Raw Results to the
  TCXO Manager for the GPS.


DEPENDENCIES
  None.

PARAMETERS
  boolean gl1_tcxo_enable_push - Whether rotator push is being enabled or disabled.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void gl1_hw_tcxo_rpush_flag_cb( boolean gl1_tcxo_enable_push );

/*===========================================================================
FUNCTION     GL1_HW_UPDATE_RAW_FE_TCXO_MGR

DESCRIPTION
  This function passes the Raw Accumulated Freq Err and Number of samples
  for the Rotator Push.

DEPENDENCIES
  None.

PARAMETERS
  int32 - Accumulated FError.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void gl1_hw_update_raw_fe_tcxo_mgr( int32 freq_err, gas_id_t gas_id );

/*===========================================================================
FUNCTION     GL1_HW_SET_CGPS_FEU_STATE

DESCRIPTION
  If current state is idle then return TRUE, otherwise FALSE but may need
  changing as we also have the NO_INFO state to worry about after a data reset.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if IDLE/FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
void gl1_hw_set_cgps_feu_state( boolean idle_mode, gas_id_t gas_id );

/*===========================================================================
FUNCTION     GL1_HW_GET_CGPS_FEU_STATE

DESCRIPTION
  If current state is idle then return TRUE, otherwise FALSE but may need
  changing as we also have the NO_INFO state to worry about after a data reset.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if IDLE/FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean gl1_hw_get_cgps_feu_state( gas_id_t gas_id );

/*===========================================================================
FUNCTION     GL1_HW_CGPS_FEU_TICK

DESCRIPTION
  The frame tick handler for the feu updates.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void gl1_hw_cgps_feu_tick( gas_id_t gas_id );

/*===========================================================================
FUNCTION     GL1_HW_RESET_CGPS_FEU

DESCRIPTION
  This function resets the Freq Err control structure for GPS to use.

DEPENDENCIES
  None.

PARAMETERS
  boolean - reset_state, whether to clear state info as well.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void gl1_hw_reset_cgps_feu( boolean reset_state, gas_id_t gas_id );

/*===========================================================================
FUNCTION     GL1_HW_SET_CGPS_FEU_FORCE_RPUSH

DESCRIPTION
  Set the local rpush flag to force a send, should be used after page read
  for sleep case and DRX, when the state changes from idle->traffic or vice versa
  and at first transition to enabling rpush.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void gl1_hw_set_cgps_feu_force_rpush( gas_id_t gas_id );

/*===========================================================================
FUNCTION     GL1_HW_SET_CGPS_FEU_SAMPLE_THRESHOLD

DESCRIPTION
  Set the local sample threshold to force a send. Used to force a
  single sample at startup.

DEPENDENCIES
  None.

PARAMETERS
  int16 - How many samples to take before sending.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void gl1_hw_set_cgps_feu_sample_threshold( int16 rpush_frame_thresh, gas_id_t gas_id );

/*===========================================================================
FUNCTION gl1_hw_update_xo_rgs

DESCRIPTION
  This function sends the XO RGS value to XO manager.

DEPENDENCIES
  None.

PARAMETERS
  Active current active XO value, GL1 state info and gas_id.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void gl1_hw_update_xo_rgs( int16    xo_freq_error, tcxomgr_client_state_type state,
                           gas_id_t gas_id );


/*===========================================================================
FUNCTION gl1_hw_get_initial_rgs

DESCRIPTION
  This function sets the starting RGS estimate for the particular SUB
  based on whether Multi-SIM mode is active or not.

DEPENDENCIES
  None.

PARAMETERS
  Active gas_id.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
tcxomgr_vco_info_type gl1_hw_get_initial_rgs( gas_id_t gas_id );

/*===========================================================================
FUNCTION     GL1_HW_MAP_GAS_ID_TO_TCXO_ASID

DESCRIPTION
  Set the correct TCXO asid for the chosen gas_id.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TCXO asid

SIDE EFFECTS
  None.
===========================================================================*/
tcxomgr_as_id_enum_type gl1_hw_map_gas_id_to_tcxo_asid( gas_id_t gas_id );

/*===========================================================================
FUNCTION     GL1_HW_MAP_TCXO_ASID_TO_GAS_ID

DESCRIPTION
  Set the correct gas_id from the TCXO asid.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  gas_id

SIDE EFFECTS
  None.
===========================================================================*/
gas_id_t gl1_hw_map_tcxo_asid_to_gas_id( tcxomgr_as_id_enum_type tcxo_asid );

/*===========================================================================

                     MACROS

===========================================================================*/


#endif /* GL1_HW_TCXO_MGR3_H */
