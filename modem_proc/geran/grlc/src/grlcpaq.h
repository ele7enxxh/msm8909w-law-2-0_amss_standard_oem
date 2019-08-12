/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK PENDING ACKNOWLEDGEMENT QUEUE
***
***
*** DESCRIPTION
***
***  Definitions of functions provided by the PAQ module
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcpaq.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/24/03    hv     Created
*** 08/03/04    hv     Added EGPRS code
***
*****************************************************************************/

#ifndef INC_GRLCPAQ_H
#define INC_GRLCPAQ_H

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
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */
#include "grlcllc.h"
#include "grlcllcif.h"
#include "rr_public_store.h"
#include "grlculproc.h"
#include "grlculpdu.h"
#include "geran_multi_sim.h"


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_UL_INIT_PAQ
===
===  DESCRIPTION
===
===    Init the Pending-acknowledgement-queue
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_ul_init_paq ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRLC_UL_PUT_PAQ
===
===  DESCRIPTION
===
===    Put the pointer of the LLC PDU message just loaded into the PAQ
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_ul_put_paq (gas_id_t gas_id, gprs_grr_ul_mes_t *msg_ptr);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_PAQ_MAKE_PSN_CURRENT
===
===  DESCRIPTION
===
===     Make the PSN the current PDU with the given offset. Expect the given
===     PDU is present in the PAQ
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_ul_paq_make_psn_current (gas_id_t gas_id, uint16 psn, uint16 offset);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_UNPUT_PAQ
===
===  DESCRIPTION
===
===    Restore and unput all PDUs in the PAQ into the main LLC UL Q.
===    next_pdu_ptr is unput as it is not held in the PAQ.
===    - unput next_pdu_ptr
===    - restore (if required) and unput PDUs in the PAQ in reverse order
===    - clear the PAQ
===    - clear cur_pdu_ptr if it points to a PDU among the paq queue
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_ul_unput_paq ( gas_id_t gas_id );

/*===========================================================================
===
===  FUNCTION      GRLC_UL_PAQ_DOES_PSN_EXIST
===
===  DESCRIPTION
===
===     Search and return TRUE if PSN exists in the PAQ
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     boolean
===
===  SIDE EFFECTS
===
===========================================================================*/
extern boolean grlc_ul_paq_does_psn_exist (gas_id_t gas_id, uint16 psn);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DEL_FREE_PAQ_UPTO_PSN
===
===  DESCRIPTION
===
===     Free PDUs and messages in PAQ upto and including the LL PDU with the
===     given PSN value. If cur_pdu_ptr points to a PDU to be freed then
===     cur_pdu_ptr is set to NULL as that PDU is freed.
===
===     When b_ul_psn_txed = TRUE, call gllc_grr_ul_transmit_ind() to indicate
===     that UL PDU transmission is completed.
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_ul_del_free_paq_upto_psn (gas_id_t gas_id, uint16 psn,boolean b_ul_psn_txed);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_PAQ_SEEK_PSN_CV0
===
===  DESCRIPTION
===
===    Seek the PSN of the last PDU that was acknowledged as received by the
===    NW when FA=1. Since we can't use the bitmap value when FA=1 is received.
===    We need to find the last block of the TBF (when CV=0) to identify the
===    PSN of the PDU
===
===  PARAMS
===
===    bsn: indicates the BSN where BSN-1 is the starts of the search for the
===         block with CV=0 -- backwards
===
===    Expect 'ul.vs' to be passed in in all cases except when PUAN is received
===    in GPRS mode EXT_UTBF where the passed in value should be VA; following
===    an update of the RRB/SSN
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
extern uint16 grlc_ul_paq_seek_psn_cv0 (gas_id_t gas_id, uint16 bsn);


#endif /* INC_GRLCPAQ_H */

/*** EOF: don't remove! ***/
