#ifndef L1_SC_LTE_IRAT_H
#define L1_SC_LTE_IRAT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              l1_sc_lte_irat.h

GENERAL DESCRIPTION
   This module contains I/F declarations and prototypes related with LTE.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L1/vcs/l1_sc.h_v   1.9   03 Jul 2002 15:44:46   gwatkins  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_sc_lte_irat.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   --------  ----------------------------------------------------------
12/12/14   cgc       CR769137 Add l1_sc_g2l_init() for G2L initialisation.
26/08/14   pa        CR715657 Band deregistration on finishing IRAT activity.
22/08/14   cgc       CR713634 obsoleted G2L code and featurisations FEATURE_GSM_G2L_TIMED_SRCH and FEATURE_CM_G2L removed
29/05/14   cgc       CR669540 Add l1_sc_start_G2L_timed_search() to GTOW_SRCH_STATE_MULTI_SIM_FW_WAIT 
08/16/01   mk        Initial version.  Minor Cleanup and Header Addition.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Alphabetical Order */
#include "geran_variation.h"
#include "comdef.h"
#include "l1_sc_drv.h"
#include "l1_sc_int.h"
#include "gl1_msg.h"
#include "gl1_msg_g.h"
#include "gl1_msg_wcdma_meas.h"
#include "l1i.h" /* for running_avg_struct */
#include "gl1_sys_algo.h"
#include "gl1_sys_params.h"
#include "rex.h"

#ifdef  FEATURE_GSM_TO_LTE

#include "lte_as.h"
#include "lte_l1_types.h"
#include "lte_cphy_irat_meas_msg.h"
#include "lte_ml1_irat_ext_api.h"


/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

void   g2l_schedule_idle_lte_search(gas_id_t gas_id);

typedef enum
{
   G2L_CM_INACTIVE     =0x00,
   G2L_CM_ACTIVE       =0x01, /* Next state after G2L_CM_START*/
   G2L_CM_INIT_CNF     =0x02,
   G2L_CM_CNF_PENDING  =0x04,
   G2L_CM_STOP         =0x08,
   G2L_CM_START        =0x40,

   G2L_CM_CHANGE_MASK  = (G2L_CM_ACTIVE | G2L_CM_INIT_CNF                      | G2L_CM_STOP ),
   G2L_CM_ACTIVE_MASK  = (G2L_CM_ACTIVE | G2L_CM_INIT_CNF | G2L_CM_CNF_PENDING | G2L_CM_STOP ),
   G2L_CM_PENDING_MASK = (G2L_CM_ACTIVE | G2L_CM_INIT_CNF | G2L_CM_CNF_PENDING ),
   G2L_CM_READY        = (G2L_CM_ACTIVE | G2L_CM_INIT_CNF),
   
   G2L_CM_END
} g2l_cm_mode_states_e;

typedef enum
{
    LTE_GAP_NULL,
    LTE_GAP_INIT, /* Bring CM out from G2L_CM_START */
    LTE_GAP_START_REQ,
    LTE_GAP_NEXT_REQ,
    LTE_GAP_STATUS_REQ,
    LTE_GAP_CNF,
    LTE_GAP_ABORT,
    LTE_GAP_INIT_CNF,
    LTE_GAP_TRIGGER_MDSP,
    LTE_GAP_LEAVE_CM_REQ,
    LTE_GAP_FINISH
} g2l_cm_events_e;


typedef enum
{
    LTE_IDLE_GAP_NULL,
    LTE_IDLE_GAP_WAIT,
    LTE_IDLE_GAP_SCHEDULE,
    LTE_IDLE_GAP_TRIGGER_MDSP,

    LTE_IDLE_GAP_START_REQ,
    LTE_IDLE_GAP_NEXT_REQ,
    LTE_IDLE_GAP_STATUS_REQ,
    LTE_IDLE_GAP_CNF,
    LTE_IDLE_GAP_STATUS,
    LTE_IDLE_GAP_DEINIT_REQ,
    LTE_IDLE_GAP_DEINIT_CNF,
    LTE_IDLE_GAP_INIT_CNF
} g2l_idle_events_e;

extern lte_ml1_irat_gap_schedule_gap_state_s     ml1_gap_state;                    /*  {0}; */

/* populate the content of the timed_srch_meas_req from rr_params */
void    l1_sc_populate_g2l( uint16 freq_idx, lte_cphy_irat_meas_timed_srch_meas_req_s *msg, gas_id_t gas_id, boolean gap_start_asap);
uint16  l1_sc_cm_g2l_state( g2l_cm_events_e gapEvent , uint16 freq_idx , boolean gapBool, gas_id_t gas_id);

void    g2l_init_cm(gas_id_t gas_id);
void    g2l_set_cm_inactive(gas_id_t gas_id);

void    l1_sc_idle_g2l_state( g2l_idle_events_e gapEvent, gas_id_t gas_id);
void    l1_isr_G2L_idle_trigger(gas_id_t gas_id);
void    l1_sci_g2l_idle_aborted( void* dummy, gas_id_t gas_id);
void    l1_sc_set_ml1_init(boolean in_ml1_init, gas_id_t gas_id);
void    l1_sc_start_G2L_timed_search(gas_id_t gas_id);
void    l1_sc_g2l_init(gas_id_t gas_id );

#endif /*FEATURE_GSM_TO_LTE*/
#endif /*L1_SC_LTE_IRAT_H*/

