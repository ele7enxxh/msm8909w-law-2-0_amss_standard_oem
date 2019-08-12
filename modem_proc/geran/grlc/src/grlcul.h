/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK MODULE INTERFACE DEFINITION
***
***
*** DESCRIPTION
***
***  Interface definition of RLC uplink to the L2 task control module.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcul.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/29/01    hv     Created
*** 09/19/12    hv     Added dual data space functionality
***
*****************************************************************************/

#ifndef INC_GRLCUL_H
#define INC_GRLCUL_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "gmutex.h"
#include "geran_multi_sim.h"
#include "grlcmaccmn.h"
#include "grlci.h"
#include "queue.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/


/*-----------------------------------------------------------------
** Critical sections to protect GRLC UL message and variable access
**-----------------------------------------------------------------
*/

extern rex_crit_sect_type grlc_ul_msg_crit_sec[NUM_GERAN_DATA_SPACES];
extern rex_crit_sect_type grlc_ul_no_data_crit_sec[NUM_GERAN_DATA_SPACES];
extern rex_crit_sect_type grlc_ul_tx_vars_crit_sec[NUM_GERAN_DATA_SPACES];


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/
#ifdef  DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*===========================================================================
===
===  MACRO         xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/




/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      RLC_UL_HANDLE_PL1_SIG
===
===  DESCRIPTION
===
===    Wrapper function to pass on L1 event to the RLC UL state machine
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    alter module var event
===
===========================================================================*/
void grlc_ul_handle_pl1_sig (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      RLC_UL_HANDLE_MAC_SIG
===
===  DESCRIPTION
===
===    Wrapper function to pass on MAC event to the RLC UL state machine
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    alter module var event
===
===========================================================================*/
void grlc_ul_handle_mac_sig (gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_LLC_TMP_PDU_SIG
===
===  DESCRIPTION
===
===    Wrapper function to loop and copy LLC PDUs from the temporary queue
===    and put into RLC main queue. This is to allow PDUs to be put into RLC's
===    PDU queue in RLC context which means the exsiting function enqueue_pdu()
===    can be invoked without changes except for calls to TASKLOCK/FREE().
===
===    The problem was enqueue_pdu() was called in LLC'c context, so TASKLOCK/FREE
===    was added to lock out RLC from gaining context and operated on the same
===    PDU queue and grouping information
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    ul.event,
===
===========================================================================*/
void grlc_ul_handle_llc_tmp_pdu_sig (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      RLC_UL_HANDLE_LLC_PDU_SIG
===
===  DESCRIPTION
===
===    Wrapper function to pass on LLC PDU event to the RLC UL state machine
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    alter module var event
===
===========================================================================*/
void grlc_ul_handle_llc_pdu_sig (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANLDE_INTERNAL_SIG()
===
===  DESCRIPTION
===
===    Handler function to send PRR Type 2. Because the conditions for sending
===    the PRR is triggered from ISR context in post_tx_actions(). Sending the
===    PRR involves allocating memory is now deferred from happening inside the ISR,
===    by setting the GRLC_UL_ITN_SIG mask, which later causes the PRR to be sent
===    in task time
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    ul.event,
===
===========================================================================*/
void grlc_ul_handle_internal_sig (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      RLC_UL_HANDLE_LLC_MSG_SIG
===
===  DESCRIPTION
===
===    Wrapper function to process on LLC MSG events
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
void grlc_ul_handle_llc_msg_sig (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      RLC_UL_HANDLE_TMR_SIG
===
===  DESCRIPTION
===
===    Wrapper function to pass on TIMER event to the RLC UL state machine
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    alter module var event
===
===========================================================================*/
void grlc_ul_handle_tmr_sig (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_ONE_TIME_INIT
===
===  DESCRIPTION
===
===    Power up init of rlc downlink operation.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_ul_one_time_init (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      RLC_UPLINK_INIT
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    return value written to module var cv.
===
===========================================================================*/
void grlc_ul_init (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_NO_PS_ACCESS()
===
===  DESCRIPTION
===
===    Set sub-state to no access and delete PDU as per indicated by the flag.
===    If RLC is in user suspended state then delete all PDUs. Otherwise, delete
===    the current PDU if DEL_ONE is indicated, or delete no PDU if DEL_NONE is
===    indicated
===
===  DEPENDENCIES
===
===  PARAM
===    delete_pdu_flag: DEL_NONE or DEL_ONE
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_handle_no_ps_access (gas_id_t gas_id, delete_pdu_cause_t flag);

/*===========================================================================
===
===  FUNCTION      INDICATE_UL_RELEASE()
===
===  DESCRIPTION
===
===    Send an uplink release indication to MAC with a release cause.
===
===  DEPENDENCIES
===
===    None
===
===  PARAMS
===
===    release cause of type rm_tbf_release_t
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
============================================================================*/
void indicate_ul_release ( gas_id_t gas_id, rm_tbf_release_t rel_cause );

extern void grlc_ul_handle_log_sig(const gas_id_t gas_id);

#endif /* INC_GRLCUL_H */

/*** EOF: don't remove! ***/
