/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK PDU
***
***
*** DESCRIPTION
***
***  This module defines the PDU processing functions, at exceptions, such as
***  loading, un-loading, shortening, truncation, restoring and freeing etc..
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlculpdu.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10/04/13    hv     CR464591 - reset caused by IARs and Contention-res failures
*** 05/07/11    hv     Added promoting sig PDUs to the front of uplink queue
*** 08/03/04    hv     Added EGPRS code
*** 11/24/03    hv     Created
***
*****************************************************************************/

#ifndef INC_GRLCULPDU_H
#define INC_GRLCULPDU_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlcultypes.h"
#include "grlcutil.h"
#include "grlcdiag.h"
#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
#include "gllc_v.h"
#include "gllc_v_g.h"
#endif /* ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST  */
#include "grlcllc.h"
#include "grlcllcif.h"
#include "rr_public_store.h"
#include "grlcpaq.h"
#include "grlculproc.h"   // ul and stuff ..
#include "geran_multi_sim.h"



/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/* Inform NAS of GMM Signalling PDU deletion 
*/
#define NAS_INDICATION    1 
#define NO_NAS_INDICATION 0

typedef struct
{
  grr_data_req_cause_t  cause;            /* PDU origination cause ind      */
  uint8                 transaction_id;   /* Transaction id of a NAS Message*/
}indicate_failure_t;

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DEL_SIGNALLING_PDUS()
===
===  DESCRIPTION
===
===    Delete all SIGNALLING PDUs from the LLC PDU main queue.
===    First all PDUs in the PAQ queue and current and next pdu pointers
===    are returned to the main queue. The PDU is then read out one by one,
===    and deleted if a mtach occurs, otherwise it is put in the PAQ queue.
===    Finally the PAQ queue is unput in reverse order
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_del_signalling_pdus ( gas_id_t gas_id, uint8 nas_indication );

/*===========================================================================
===
===  FUNCTION      LOAD_CUR_PDU()
===
===  DESCRIPTION
===
===    Attempt to load the current PDU and the next pdu with PDUs from the queue.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    cur_pdu_ptr is returned either NULL or pointing to a valid PDU
===
===  SIDE EFFECTS
===
===========================================================================*/
extern gprs_grr_ul_mes_t *load_cur_pdu ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION       UNPUT_PDU
===
===  DESCRIPTION
===
===    Put back LLC PDU message in msg_ptr back on the LLC UL Q
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void unput_pdu ( gas_id_t gas_id, gprs_grr_ul_mes_t *msg_ptr);

/*===========================================================================
===
===  FUNCTION      UNPUT_NEXT_PDU()
===
===  DESCRIPTION
===
===    Puts back the next PDU on the queue if it is loaded.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    next_pdu_ptr, PDU queue
===
============================================================================*/
extern void unput_next_pdu ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      SHORTEN_CUR_PDU
===
===  DESCRIPTION
===
===     Shorten and restore the current PDU:
===     - free the remaining of the working copy of the payload (dup_pdu_ptr)
===     - shorten the original payload by stripping 'offset' octets off the start
===     - dup the shortened payload and use it as a new working copy of the payload
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void shorten_cur_pdu ( gas_id_t gas_id, uint16 offset);

/*===========================================================================
===
===  FUNCTION      DELINK_DATA_PDUS
===
===  DESCRIPTION
===
===    Moves all PDUs from the ULQ to grlc_ul_old_llc_q
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul_q
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void delink_data_pdus ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      RELINK_DATA_PDUS
===
===  DESCRIPTION
===
===    Moves all PDUs in the Reserve queue back the main ULQ
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul_q
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void relink_data_pdus ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      ENQUEUE_PDU
===
===  DESCRIPTION
===
===    Queue up the incoming LLC PDU in FIFO order. All PDUs are subjected to Flow
===    Control checking except for the GMM-procedure related PDUs which come in
===    during RLC Suspend/Resume operation.
===
===    Added FEATURE_RLC_PDU_REORDERING to push a PDU of higher radio priority and
===    different RLC mode compared to the current PDU, to just behind the current
===    PDU. If the current PDU contains a group of other PDUs behind it then the
===    PDU is re-processed and re-grouped accordingly
===
===  PARAMS
===
===    grr_ul_msg_ptr points to LLC message containing info about the LLC PDU
===    and a pointer to the PDU in DSM pool. It also has space for info internal
===    to RLC when RLC processes it.
===
===  DEPENDENCIES
===
===    ul_q
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    CTS_ON/OFF status
===
===========================================================================*/
extern void enqueue_pdu ( gas_id_t gas_id, gprs_grr_ul_mes_t *grr_ul_msg_ptr );

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DETECT_AND_FLUSH_BACKLOG_PDUS
===
===  DESCRIPTION
===    If Commercial Recovery NV is NOT enabled then if the maximum signalling
===    PDU threshold is about to be reached then delete all PDUs except one or
===    newest PDUs
===
===  PARAMS
===
===    ptr points to the rlc ul pdu struct
===
===  DEPENDENCIES
===
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    CTS_ON/OFF status
===
===========================================================================*/
extern void grlc_ul_detect_and_flush_backlog_pdus (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      DEQUEUE_PDU
===
===  DESCRIPTION
===
===    Takes an llc pdu at the front of the queue and return its address if
===    one exists, otherwise return NULL.
===
===    This function checks the RLC UL data queue capacity and resumes
===    LLC data flow if near empty.
===
===  PARAMS
===
===    ptr points to the rlc ul pdu struct
===
===  DEPENDENCIES
===
===    ul_q
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    CTS_ON/OFF status
===
===========================================================================*/
extern void dequeue_pdu (gas_id_t gas_id, gprs_grr_ul_mes_t **pptr);

/*===========================================================================
===
===  FUNCTION      FREE_LLC_PDU_AND_MSG
===
===  DESCRIPTION
===
===     Free the PDU payload, dup payload and the memory carrying the PDU (msg)
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void free_llc_pdu_and_msg(gprs_grr_ul_mes_t *msg_ptr);

#ifdef  FEATURE_GPRS_PS_HANDOVER

/*==========================================================================
===
===  FUNCTION      GRLC_UL_PROMOTE_SIG_PDUS
===
===  DESCRIPTION
===
===    Sort the temporary LLC PDU and the main LLC PDU queues to place the
===    RAUs and Cell-Update at the front of the queue
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul, ULQ, ULTQ
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_promote_sig_pdus ( gas_id_t gas_id );

#endif /* FEATURE_GPRS_PS_HANDOVER  */

/*==========================================================================
===
===  FUNCTION      GRLC_UL_REORDER_PDUS
===
===  DESCRIPTION
===
===    Sort the main uplink Q and Temp Q, such that the signalling PDUs are
===    at the front of the ULQ , everytime there is a release.
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul, ULQ, ULTQ
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_ul_reorder_pdus ( gas_id_t gas_id );

#endif /* INC_GRLCULPDU_H */

/*** EOF: don't remove! ***/
