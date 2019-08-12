#ifndef CMDBG_QSH_EXT_H
#define CMDBG_QSH_EXT_H
/*===========================================================================

     C A L L   M A N A G E R   D E B U G   H E A D E R   F I L E

DESCRIPTION
  This header file contains debug macros and definitions necessary to
  interface with cmdbg_qsh.c


Copyright (c) 1991 - 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmdbg_qsh.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/09/15   KC      Initial release.

===========================================================================*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "qsh.h"
#include "cmcall.h"


#ifndef CM_DEBUG
  /* keeps the API in uncompressed section in th elf
   ** this will be used by QSH dump collection ,
   ** since after the crash happened uncompressed functions
   ** compressed functions wont be accesible */
  #define CM_API_IN_UNCOMPRESSED_IN_ELF __attribute__((section(".uncompressible.text")))
#else
  #error code not present
#endif


typedef enum cm_qsh_metrics_chg_type_e
{
  CM_QSH_METRICS_CHG_TYPE_NONE = -1,
  CM_QSH_METRICS_CHG_TYPE_SS_EVENT,
  CM_QSH_METRICS_CHG_TYPE_PH_EVENT,
  CM_QSH_METRICS_CHG_TYPE_SEND_INITIAL_METRIC,
  CM_QSH_METRICS_CHG_TYPE_MAX
}cm_qsh_metrics_chg_type;


/*===========================================================================

FUNCTION cmdbg_qsh_chk_and_send_srv_evts

DESCRIPTION
  Sends srv change events to QSH

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void             cmdbg_qsh_chk_and_send_srv_evts
                 (sys_modem_as_id_e_type as_id);


/*===========================================================================

FUNCTION cmdbg_qsh_send_call_evts

DESCRIPTION
  Sends call start/end/drop/setup failed events to QSH

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void                                 cmdbg_qsh_send_call_evts
(

    cm_call_event_e_type    call_event,
        /* notify client list of this call event */

    cmcall_s_type    *call_ptr
        /* pointer to a call object. */


);


/*===========================================================================

FUNCTION cmdbg_qsh_send_volte_call_start_end_evts

DESCRIPTION
  Sends VOLTE call start/end events to QSH

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void                                 cmdbg_qsh_send_volte_call_start_end_evts
(

    const cm_ext_volte_call_state_ind_s_type    *volte_state
        /* VOLTE state indication */

);


/*===========================================================================

FUNCTION cmdbg_qsh_send_call_end_evts

DESCRIPTION
  Send call end events to QSH

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void                                 cmdbg_qsh_send_call_end_evts
(

    cmcall_s_type          *call_ptr,
        /* pointer to a call object. */

    uint8                  *start_addr
        /* start address of metric */

);


/*===========================================================================

FUNCTION cmdbg_chk_and_update_srv_metrics

DESCRIPTION
  Updates the metrics buffer and sends to QSH

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void  cmdbg_chk_and_update_srv_metrics
                (sys_modem_as_id_e_type as_id,
                 /* AS id */
                 cm_qsh_metrics_chg_type chg_type
                 /* densotes which metric changed */
                 );


/*===========================================================================

FUNCTION cmdbg_chk_and_update_serving_cell_metrics

DESCRIPTION
  Updates the serving cell metrics buffer and sends to QSH

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void  cmdbg_chk_and_update_serving_cell_metrics(

  sys_modem_as_id_e_type as_id,
      /* AS id */

  cm_ss_e_type           cm_ss
      /* ss where the info changed */

);


/*===========================================================================

FUNCTION cmdbg_chk_and_update_call_end_info_metrics

DESCRIPTION
  Updates the call end metrics buffer and sends to QSH

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void  cmdbg_chk_and_update_call_end_info_metrics(

    cm_call_event_e_type    call_event,
      /* the call event */

    cmcall_s_type    *call_ptr
        /* pointer to a call object. */

);  

/*===========================================================================

FUNCTION cmdbg_qsh_determine_3gpp_call_end_evt_type

DESCRIPTION
  determine the call end event from either cause or rej type. Only applicable for 3GPP calls.
  To determine the event:
    call helper functions to determine if event is NORMAL, RX_DISC, TX_DISC, RX_RELEASE
    if reject type is present and value is AS_REJECT:
      map it to event RX_RRC_RELEASE
    if neither cause or rej type can be mapped, map to event OTHERS

DEPENDENCIES
  none

RETURN VALUE
  the call end event, determined from either cause or rej type

SIDE EFFECTS
  none

===========================================================================*/
 sys_call_end_event_e_type cmdbg_qsh_determine_3gpp_call_end_evt_type(

  cmcall_s_type    *call_ptr

);

/*===========================================================================

FUNCTION cmdbg_qsh_determine_3gpp2_call_end_evt_type

DESCRIPTION
  Determine the call end event for CDMA CS calls. ( this API does not support HDR )
  The orig failures reported by 1xcp are converted to call end status in CM.
  This call end status can be classified into 3 categories using this API
  a) RLF_OR_WEAK_SIGNAL
  b) NORMAL
  c) OTHERS
  

DEPENDENCIES
  none

RETURN VALUE
  the call end event type, determined from call_ptr->end_status
 
SIDE EFFECTS
  none

===========================================================================*/
sys_call_end_event_e_type cmdbg_qsh_determine_3gpp2_call_end_evt_type(

    cmcall_s_type    *call_ptr
    /* pointer to call object */

);
/*===========================================================================

FUNCTION cmdbg_qsh_generic_rpt_proc

DESCRIPTION
  Process Generic reports in CMSS


DEPENDENCIES
  cm qsh object must have already been initialized with
  cmdbg_qsh_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_qsh_generic_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to generic reports */
);

/*===========================================================================

FUNCTION cmdbg_qsh_init

DESCRIPTION
  Initilize CM QSH interface.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmdbg_qsh_init();

#endif /* CM_QSH_EXT_H */

