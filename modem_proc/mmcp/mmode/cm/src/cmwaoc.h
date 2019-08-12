#ifndef CMWAOC_H
#define CMWAOC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C A L L   M A N A G E R   A D V I C E   O F   C H A R G E

GENERAL DESCRIPTION
  This module performs the Advice of Charge functionality for the
  Call Manager.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmwaoc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/12   gm      Data synchronization for CM Call objects
12/23/10   sv      Integrate DSDS feature
09/01/09   sv      Removing un-necessary clk API hedear
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
12/03/07   rm      Adding RAT-balancing feature support.
                   Removed cm_refresh_aoc_init
08/16/06   ka/rp   Moving aoc_ready flag to PH object.
01/06/06   ic      Lint cleanup  
05/25/05   ic      Mainlined FEATURE_AOC
08/04/04   ic      Lint cleanup - removed unused argument from cmwaoc_save_acm()
02/06/04   ws      Initial jaguar/mainline merge.
05/01/03   jar     Added cm_refresh_aoc_init
01/21/03   ws      Updated copyright information for 2003
08/28/02   PK      Moved function prototype cmwaoc_init from cm.h to this
                   .h file.
04/22/02   AT      Updated with changes for Advice of Charge.
04/16/02   AT      Merged Advice of Charge with GSDI and UI.
01/30/02   AT      Fixed comments from code reviews.
12/13/01   AT      Added prototypes for new functions for GSDI integration.
11/15/01   AT      Made changes for AOC to lint cleanly (relatively).
11/01/01   AT      Added a function to reset the ACM.
10/17/01   AT      Initial release.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "queue.h"     /* Interface to queue services */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "customer.h"  /* Customer configuration file */
#include <string.h>    /* Interface to string library */
#include "task.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */
#include "rex.h"       /* Interface to rex services */
#include "cmph.h"      /* Interface to CM phone object */
#include "cmcall.h"        /* Get the call structures. */

#define CM_AOC_ACM_SIZE 3

/*===========================================================================

FUNCTION CMAOC_CCM_TO_ACM

DESCRIPTION
  This macro changes the CCM value to be an ACM value.

DEPENDENCIES
  The value of a CCM (in units of .001).

RETURN VALUE
  The value of CCM / 1000 rounded up.

SIDE EFFECTS
  None
===========================================================================*/
#define CMAOC_CCM_TO_ACM(ccm) (((ccm) + 999) / 1000)


/*===========================================================================

FUNCTION cmwaoc_init_using_mmgsdi

DESCRIPTION
  This function initializes the data structures necessary for Advice of
  Charge. This includes initializing the values of ACM and ACMmax from
  the SIM, as well as setting the SIM type variables.

DEPENDENCIES
  The initialization of the SIM must be complete, with the PIN verified.

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_init_using_mmgsdi(
      cm_ss_e_type ss
);



#ifdef CM_GW_SUPPORTED
/*===========================================================================

FUNCTION cmwaoc_setup_aoc_for_call

DESCRIPTION
  This function initializes the AOC data structure within the given.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  Boolean value indicating if we can support AOC for this call.

SIDE EFFECTS
  None
===========================================================================*/

#define MS_PER_TIMEUNIT 100

boolean cmwaoc_setup_aoc_for_call(cmcall_s_type *call_ptr, const cm_e_values_type *charge_info);
#endif

/*===========================================================================

FUNCTION cmwaoc_update_ccm

DESCRIPTION
  This function is called when its time to update the current call
  meter in the phone pointer.  This occurs whenever a call updates
  its own AOC counters.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void cmwaoc_update_ccm(cmcall_s_type *call_ptr);

/*===========================================================================

FUNCTION cmwaoc_update_aoc

DESCRIPTION
  This function updates the AOC value for the given call.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void cmwaoc_update_aoc(cmcall_s_type *call_ptr);

/*===========================================================================

FUNCTION cmwaoc_end_call

DESCRIPTION
  This function is called when a call ends.  It accounts for any
  partial charges and makes sure the CCM and ACM are up to date.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void cmwaoc_end_call(cmcall_s_type *call_ptr);

/*===========================================================================

FUNCTION cmwaoc_ccm_in_30

DESCRIPTION
  This function will figure out the value that the current call meter will be
  in 30 seconds at the given rates being charged.  This rate represents a
  best guess at the time being called.  For each call with associated charges,
  any time related charges accrued in 30 seconds will be added.  Additionally,
  for data calls, any data related charges will also be accrued.  This
  algorithm first accounts for any parts of a segment that may already have
  been transmitted.  It then computes the number of additional segments which
  will accrue at the maximum rate for the next 30 seconds.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cmwaoc_ccm_in_30(sys_modem_as_id_e_type  asubs_id);


/*===========================================================================

FUNCTION cmwaoc_remove_call_from_list

DESCRIPTION
  This function will remove the given call pointer from the list of calls.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_remove_call_from_list(const cmcall_s_type *call_ptr);

/*===========================================================================

FUNCTION cmwaoc_add_to_chargelist

DESCRIPTION
  This function adds the current call pointer to the list of calls being
  charged.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_add_to_chargelist(cmcall_s_type *call_ptr);

/*===========================================================================

FUNCTION cmwaoc_get_list_head

DESCRIPTION
  This function returns the head of the list of calls being charged.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern cmcall_s_type *cmwaoc_get_list_head(sys_modem_as_id_e_type  asubs_id);

/*===========================================================================

FUNCTION cmwaoc_drop_all_calls_now

DESCRIPTION
  When we get a SIM error, this function will drop all calls with charges
  associated with them.

DEPENDENCIES
  None.

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_drop_all_calls_now (sys_modem_as_id_e_type  asubs_id);

/*===========================================================================

FUNCTION cmwaoc_handle_ccm_timeout();

DESCRIPTION
  This function handles the updates when the CCM timer times out.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_handle_ccm_timeout(sys_modem_as_id_e_type  asubs_id);

/*===========================================================================

FUNCTION cmwaoc_reset

DESCRIPTION
  This function resets Advice of charge parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_reset (void);

/*===========================================================================

FUNCTION cmwaoc_reset_acm_mmgsdi

DESCRIPTION
  This function sets the value of the ACM to 0.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void cmwaoc_reset_acm_mmgsdi(cm_ss_e_type ss);

/*===========================================================================

FUNCTION cmwaoc_get_ccm

DESCRIPTION
  This function returns the current value of the Current Call Meter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern unsigned long cmwaoc_get_ccm(cm_ss_e_type ss);


/*===========================================================================

FUNCTION cmwaoc_get_acm

DESCRIPTION
  This function returns the current value of the Accumulated Call Meter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern unsigned long cmwaoc_get_acm( sys_modem_as_id_e_type  asubs_id);


/*===========================================================================

FUNCTION cmwaoc_get_acmmax

DESCRIPTION
  This function returns the current value of the Accumulated Call Meter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern unsigned long cmwaoc_get_acmmax(

  sys_modem_as_id_e_type  asubs_id
    /* sub whose max acm is requested */
);

/*===========================================================================

FUNCTION cmwaoc_set_acmmax_mmgsdi

DESCRIPTION
  This function sets the maximum value Accumulated Call Meterof. The maximum 
  value of the Accumulated Call Meter can achieved before calls are killed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_set_acmmax_mmgsdi(

  cm_ss_e_type            ss,

  unsigned long           new_max

);


/*===========================================================================

FUNCTION cmwaoc_get_puct

DESCRIPTION
  This function returns the current value of the Accumulated Call Meter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern unsigned long cmwaoc_get_puct(void);


/*===========================================================================

FUNCTION cmwaoc_save_acm

DESCRIPTION
  This function saves the ACM to the SIM.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern void cmwaoc_save_acm(sys_modem_as_id_e_type  asubs_id, dword increment_by);


/*===========================================================================

FUNCTION cmaoc_send_call_end

DESCRIPTION
  Command CM to end a call - in origination, incoming or conversation state.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
void cmaoc_send_call_end(cmcall_s_type *call_ptr);
#endif /* CMWAOC_H */
