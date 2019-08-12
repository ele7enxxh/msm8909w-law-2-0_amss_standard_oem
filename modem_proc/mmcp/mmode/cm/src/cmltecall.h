#ifndef CMLTECALL_H
#define CMLTECALL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   LTE   C A L L   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Call Object for LTE.

  The Call Object is responsible for:
  1. Processing clients' call commands.
  2. Processing Lower Layer replies for clients' call commands.
  3. Processing Lower Layer call related notifications.
  4. Notifying the client list of call events.


EXTERNALIZED FUNCTIONS

  cmltecall_info_get
    Copy the current call state information into a specified buffer.

  cmltecall_client_cmd_proc
    Process clients' call commands.

  cmltecall_rpt_proc
    Process Lower Layer call commands.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/01/11   gm      Thermal emergency support
01/12/11   fj      Change mapping functions to static functions.
07/01/10   fj      Revert temp solution in CL#1330955.
06/01/10   fj      Temp solution for handling DRB_SETUP_IND before SERVICE_IND
                   during W to L redirection.
05/17/10   fj      Add support for GW to LTE call transfer.
01/20/10   fj      Merged changelist 987678.
08/31/09   fj      Added prototypes cmltecall_copy_tft_type(),
                   cmltecall_rpt_info_get(), and cmltecall_reset().
07/27/09   fj      Added cmltecall_copy_protocol_config_options() prototype.
04/06/09   fj      Added cmltecall_client_cmd_proc() prototype.
01/05/09   fj      Initial version.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "cm.h"
#include "cmi.h"
#include "cmcall.h"

/*===========================================================================

FUNCTION cmltecall_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmltecall_info_get(

    cm_mm_call_info_s_type        *call_info_ptr,
        /* Copy call state info into this buffer */

    const cmcall_s_type           *call_ptr
        /* Pointer to the call object  */
);

/*===========================================================================

FUNCTION cmltecall_call_end

DESCRIPTION
  Clean up call object and ph object when LTE call ends.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmltecall_call_end(

  cmcall_s_type           *call_ptr
        /* Pointer to the call object  */
);

/*===========================================================================

FUNCTION cmltecall_rpt_proc

DESCRIPTION
  Process lower layer reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmltecall_rpt_proc(

  const cm_esm_cmd_u_type   *cm_rpt_ptr
    /* Pointer to LL report */
);

/*===========================================================================

FUNCTION cmltecall_client_cmd_proc

DESCRIPTION
  Process clients' call commands

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmltecall_client_cmd_proc(

    cm_call_cmd_s_type  *call_cmd_ptr
        /* pointer to a CM command */
);

/*===========================================================================

FUNCTION cmltecall_copy_protocol_config_options

DESCRIPTION
  Copy protocol_config_options from the source to the destination.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmltecall_copy_protocol_config_options(

    protocol_cfg_options_T                 *dest_cfg_options,
        /* Pointer to the destination cfg_options */

    const protocol_cfg_options_T           *src_cfg_options
        /* Pointer to the source cfg_options  */
);

/*===========================================================================

FUNCTION cmltecall_copy_tft_type

DESCRIPTION
  Copy the tft_type from source to destination structure.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
extern void cmltecall_copy_tft_type(

    tft_type_T                 *dest_tft,
        /* Pointer to the destination tft */

    const tft_type_T           *src_tft
        /* Pointer to the source tft  */
);

/*===========================================================================

FUNCTION cmltecall_rpt_info_get

DESCRIPTION
  Copy the report information into the call_info_ptr.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None.

SIDE EFFECTS
  none

===========================================================================*/
extern void cmltecall_rpt_info_get(

    const cm_esm_cmd_u_type *cm_rpt_ptr,
        /* Pointer to the report message */

    cm_call_event_e_type    call_event,
        /* notify client list of this call event */

    cm_mm_call_info_s_type     *call_info_ptr
        /* call info pointer to be copied to */
        
);


/*===========================================================================

FUNCTION cmltecall_reset

DESCRIPTION
  Reset LTE-specific call fields to default values reflecting
  a LTE call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmltecall_reset(

    lte_cmcall_type              *lte_mode_ptr
        /* pointer to the LTE mode info */
);


/*===========================================================================

FUNCTION cmltecall_irat_call_object_transfer

DESCRIPTION
  During LTE to GW or GW to LTE call transfer, map call object info.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmltecall_irat_call_object_transfer(

    sys_radio_access_tech_e_type              target_rat
        /* the target service system */
);

/*===========================================================================

FUNCTION cmltecall_irat_call_object_transfer_per_subs

DESCRIPTION
  During LTE to GW or GW to LTE call transfer, map call object info.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmltecall_irat_call_object_transfer_per_subs(

    sys_radio_access_tech_e_type              target_rat,
        /* the target service system */
    sys_modem_as_id_e_type             asubs_id
	    /* subs identifier */
);

#endif // CMLTECALL_H
