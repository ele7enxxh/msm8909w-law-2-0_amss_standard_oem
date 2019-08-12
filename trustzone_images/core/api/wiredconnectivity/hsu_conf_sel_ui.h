#ifndef _HSU_CONF_SEL_UI_H_
#define _HSU_CONF_SEL_UI_H_

/*=============================================================================

     High Speed USB configuration Selector UI related
        H E A D E R   F I L E

=============================================================================*/
/**
@file hsu_conf_sel_ui.h

Functions that are related to High-Speed USB composition changes through the
user interface.

Externalized functions:
 - hsu_conf_sel_ui_dynamic_switch_is_allowed()
 - hsu_conf_sel_ui_rdm_done()
 - hsu_conf_sel_ui_comp_change_is_done()
 - reset_state_machine_and_data() (#ifdef FEATURE_HSU_TEST_COMPONENT)
 - hsu_conf_sel_ui_comp_change_state_completed()
   (#ifdef FEATURE_HSU_TEST_COMPONENT)
 - hsu_conf_sel_ui_update_nv_for_diag_if_needed()
   (#ifdef FEATURE_HSU_TEST_COMPONENT)

hsu_conf_sel_ui_comp_menu_init() should be called during power up, before the
user can perform any UI actions.
*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_conf_sel_ui.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  06/24/10 lo   (Tech Pubs) Edited/added Doxygen comments and markup.
  12/04/06 ke   Initial version
    
=============================================================================*/

/*=============================================================================
Copyright (c) 2006-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

#include "hsu_conf_sel_types.h"

/** @ingroup hsusb_qc_api
    @{
*/
/*=============================================================================
  Forward Declarations
=============================================================================*/

/*=============================================================================
  Constants
=============================================================================*/

#define HSU_MAX_UI_ITEMS     10
  /**< A limit from the UI code for the maximum number of
       compositions. */
#define HSU_UNDEFINED_MENU_INDEX  0xFF
 /**< Flag value to indicate that the current default is unknown. */


/*=============================================================================
  Macros
=============================================================================*/

/*=============================================================================
  Typedefs
=============================================================================*/

/*=============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_conf_sel_ui_comp_menu_init
=============================================================================*/
/**
Prepares the High-Speed USB Composition Selection Menu for operation.

@dependencies
Must be called before any other HS-USB Composition Menu Services are used.

@sideeffects
None.

@return
None.
*/
void hsu_conf_sel_ui_comp_menu_init(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_ui_dynamic_switch_is_allowed
=============================================================================*/
/**
Returns TRUE if it is allowed to dynamically switch to a new composition,
FALSE otherwise.\ This is determined according to the current composition.

@dependencies
Called from the UI task's context.

@sideeffects
None.

@return
TRUE  -- Dynamic composition switching is allowed.\n
FALSE --  Dynamic composition switching is not allowed.
*/
boolean hsu_conf_sel_ui_dynamic_switch_is_allowed(void);

/*=============================================================================
FUNCTION0    hsu_conf_sel_ui_rdm_done
=============================================================================*/
/**
Continues the process of application closing and opening according to the
current internal state.\ Called from UI context when a dialog receives an
EVT_RDM_DONE. 

@dependencies
Must be called after stack initialization, during a composition change process.

@sideeffects
None.

@return
TRUE  -- The RDM state transition was successful.\n
FALSE -- The RDM state transition was successful was not successful.
*/
boolean hsu_conf_sel_ui_rdm_done(void);

/*===========================================================================
FUNCTION    hsu_conf_sel_ui_rdm_event_received
==============================================================================*/
/**
Resets the internal state and data, to be ready for another composition
change.\ Called from the UI task's context, when an EVT_RDM_* other than
EVT_RDM_DONE is received.

@param rdm_event [IN] -- The event received from RDM.

@dependencies
Must be called after stack initialization, during a composition change process.

@sideeffects
None.

@return
None.
*/
void hsu_conf_sel_ui_rdm_event_received(uint16 rdm_event);

/*===========================================================================
FUNCTION    hsu_conf_sel_ui_comp_change_is_done
==============================================================================*/
/**
Checks if an EVT_RDM_DONE is the last one expected.\ Called from the UI context
when an EVT_RDM_DONE is received. 

@dependencies
Must be called after stack initialization, during a composition change process.

@sideeffects
None.

@return
TRUE  -- The composition change process is completed.\n
FALSE -- The composition change process is not completed.
*/
boolean hsu_conf_sel_ui_comp_change_is_done(void);

/* The following functions are used by HSU_TEST_COMPONENT */
/*===========================================================================
FUNCTION    reset_state_machine_and_data
==============================================================================*/
/**
Checks if an EVT_RDM_DONE is the last one expected.\ Called from the UI context 
when an EVT_RDM_DONE is received. 

@dependencies
Must be called after stack initialization, during a composition change process.

@sideeffects
None.

@return
TRUE  -- Composition change is completed when the function returns.\n
FALSE -- Composition change process is not yet completed.
*/
void reset_state_machine_and_data(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_ui_comp_change_state_completed
=============================================================================*/
/**
Checks if the composition state change is completed.

@dependencies
Must be called after stack initialization, during a composition change process.

@sideeffects
None.

@return
TRUE  -- The composition state change is completed
         (HSU_COMP_CHANGE_STATE_COMPLETED).\n
FALSE -- The composition state change is not completed.
*/
boolean hsu_conf_sel_ui_comp_change_state_completed(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_ui_update_nv_for_diag_if_needed
=============================================================================*/
/**
Writes the new composition to NV, if appropriate.

@dependencies
Must be called after stack initialization, during a composition change process.

@sideeffects
None.

@return
None.
*/
void hsu_conf_sel_ui_update_nv_for_diag_if_needed(void);

/** @} */ /* end_group hsusb_qc_api */

#endif /* _HSU_CONF_SEL_UI_H_ */
