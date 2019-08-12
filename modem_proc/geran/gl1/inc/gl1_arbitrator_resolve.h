#ifndef GARB_RESOLVE_H
#define GARB_RESOLVE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                   G L 1  A R B I T R A T O R  R E S O L V E

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gl1_arbitrator_resolve.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   --------  ----------------------------------------------------------
27/11/13   pjr       CR.579214 : COEX : Prioritise PDTCH over TCH during 2 phase access
09/11/13   mc       CR.572746 : COEX : DSDA G+G VBATT power back-offs for TxTx collisions are not correctly applied
15/08/13   sk        CR524039 GBTA bringup changes
29/07/13   sk        CR519663 Partial QBTA bringup changes
13/03/13   mc        CR.459487 : COEX Support - GERAN arbitrator framework
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gl1_arbitrator_interface.h"
#include "gl1_arbitrator_types.h"


/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION      garb_resolve_none

DESCRIPTION   Resolver for NULL conflict

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_none(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_low_tier

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_low_tier(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_high_tier

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_high_tier(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_tch_pdtch
 
DESCRIPTION   should resolve high priority for tch except when 
              transfer is in L1_SINGLE_BLOCK_ALLOC 
 
PARAMS
 
RETURN VALUE  None

===========================================================================*/
void garb_resolve_tch_pdtch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================
 
FUNCTION      garb_resolve_pdtch_tch
 
DESCRIPTION   should resolve low priority for pdtch except 
              when transfer is in L1_SINGLE_BLOCK_ALLOC
 
PARAMS
 
RETURN VALUE  None
 
===========================================================================*/
void garb_resolve_pdtch_tch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_pdtch_ccch

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_pdtch_ccch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_pdtch_pdtch
 
DESCRIPTION   should resolve low priority for pdtch except 
              when transfer is in L1_SINGLE_BLOCK_ALLOC
 
PARAMS
 
RETURN VALUE  None
 
===========================================================================*/
void garb_resolve_pdtch_pdtch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);


/*===========================================================================

FUNCTION      garb_resolve_ccch_pdtch

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ccch_pdtch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);
/*===========================================================================

FUNCTION      garb_resolve_ccch_rrbph

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ccch_rrbph(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ccch_rrbpl

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ccch_rrbpl(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_rrbph_ccch

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_rrbph_ccch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_rrbpl_ccch

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_rrbpl_ccch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ptcch_ccch

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ptcch_ccch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ccch_ptcch

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ccch_ptcch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ptcch_ncellh

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ptcch_ncellh(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ptcch_ncellh

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ptcch_ncelll(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_pdtch_ncellh

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_pdtch_ncellh(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_pdtch_ncelll

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_pdtch_ncelll(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ccch_ncellh

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ccch_ncellh(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_rrbph_ncellh

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_rrbph_ncellh(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_rrbph_ncelll

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_rrbph_ncelll(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_rrbpl_ncellh

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_rrbpl_ncellh(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_rrbpl_ncelll

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_rrbpl_ncelll(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ncellh_ccch

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ncellh_ccch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ncellh_ncelll

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ncellh_ncelll(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ncelll_ptcch

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ncelll_ptcch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ncelll_paach

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ncelll_paach(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ptcch_ncellh

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ncelll_rrbpl(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ncelll_ncellh

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ncelll_ncellh(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ncelll_pdtch

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ncelll_pdtch(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

/*===========================================================================

FUNCTION      garb_resolve_ncelll_rrbph

DESCRIPTION

PARAMS

RETURN VALUE  None

===========================================================================*/
void garb_resolve_ncelll_rrbph(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t gas_id);

#endif /*GARB_RESOLVE_H*/