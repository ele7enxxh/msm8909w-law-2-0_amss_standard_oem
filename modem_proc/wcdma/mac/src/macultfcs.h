#ifndef MACULTFCS_H
#define MACULTFCS_H
/*===============================================================================================

                      U P L I N K   M A C  T F C S   H E A D E R   F I L E

GENERAL DESCRIPTION

  This module contains the declarations for UL TFCS functions

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2004-2005, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

================================================================================================*/

/*===============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macultfcs.h_v   1.5   18 Jun 2001 10:00:34   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macultfcs.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/03/13   kc       Remove CM interleaving workaround
01/10/13   kc      Compilation Errors Fixed when audio is disabled
07/24/12   geg      Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/24/12   grk     Fixed compiler warnings
02/10/12   kc     Added bitmap to capture TFC power state during UL ISR
01/02/12   grk    Feature cleanup
10/22/11   sk     NikeL UL CM workaround under feature FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
09/16/10   mrg    Moved auido(MVS) dependency under the feature flag FEATURE_MVS
06/16/10   mrg    Lint warnings are fixed
05/07/09   ssg     Updated Copyright Information
04/29/09   av       Fixed compiler warnings in L2 
03/06/08   ssg     Added prototype for mac_ul_assign_pdu_size().
07/18/05   sk      Added prototype for mac_initialize_pwr_state_for_tfcs,
                   mac_compute_minimum_set_of_tfcs
12/30/04   sk      Added amr_trch_mapping_info_type
06/18/01   ps      Added declaration for "oc_ul_tfcs_ptr"

================================================================================================*/
#include "wcdma_variation.h"
#ifdef FEATURE_MVS
#include "mvs.h"
#endif
#include "l1ulcfg.h"

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN
#define TFC_PWR_STATE_BITMAP_SIZE (L1_UL_TFC_MAX/32) 

extern uint32 tfc_pwr_state_bitmap[TFC_PWR_STATE_BITMAP_SIZE];
#endif

/* Pointer to ordered TFCS data */
extern  mac_ul_tfcs_type    *oc_ul_tfcs_ptr;

#ifdef FEATURE_MVS
typedef struct
{
  uint8               amr_a_dch_idx;
  uint8               amr_b_dch_idx;
  uint8               amr_c_dch_idx;
  uint8               num_amr_trch;
  uint16              amr_a_tfi;
  uint16              amr_b_tfi;
  uint16              amr_c_tfi;
  mvs_amr_mode_type   amr_mode;

}amr_trch_mapping_info_type;
#endif

/*================================================================================================

 FUNCTION NAME  update_ul_tfcs_info_table

 PURPOSE        Updates the uplink TFCS information table

 DESCRIPTION
================================================================================================*/
void    proc_ul_tfcs_info
(
    /* UL TFCS Configuration Pointer */
    mac_ul_tfcs_type        *tfcs_ptr
);

/*================================================================================================
 FUNCTION NAME  select_tfc

 DESCRIPTION
    Selects the appropriate Transport Format Combination based on :
    - TTI (10 or 20 or 40 or 80 msec)
    - Buffer occupancy and priority of uplink logical channels
    - Available Transport Format Combination Set

 RETURNS
    UL RLC Frame Format (i.e. # of PDUS/PDU Size of each logical channel)

 CONTEXT
    Layer 2 Task context

================================================================================================*/
void        select_tfc
(
    /* TTI boundary of the frame */
    l1_tti_enum_type            tti,

    /* Uplink Buffer Status Pointer */
    rlc_ul_buf_status_type      *ul_buf_status_ptr,

    /* Pointer to UL RLC Frame Format information */
    rlc_ul_frame_format_type   *rlc_frame_fmt_ptr

);

/*================================================================================================
 FUNCTION NAME  mac_compute_minimum_set_of_tfcs()

 PURPOSE        This function computes the minimum set.

 DESCRIPTION    The minum set would be computed every time MAC is configured with a new
                TFCS table.
================================================================================================*/
void mac_compute_minimum_set_of_tfcs
(
#ifdef FEATURE_WCDMA_MULTIRATE_AMR
  mac_amr_mode_info_type  *mac_amr_mode_info_ptr
#else
  void
#endif
);

/*================================================================================================

 FUNCTION NAME  mac_initialize_pwr_state_for_tfcs

 PURPOSE        Initializes the power state information after reconfig procedure.

 DESCRIPTION

================================================================================================*/
void    mac_initialize_pwr_state_for_tfcs
(
  const boolean *tfc_excess_pwr_state_ptr
);

/*================================================================================================

 FUNCTION NAME  mac_ul_assign_pdu_size

 PURPOSE        Assigns the pdu_size using TB_size for enhanced buffer status 
                computations during re-configuration of MAC parameters.

================================================================================================*/
void mac_ul_assign_pdu_size( void );


#ifdef FEATURE_MVS
/*================================================================================================
 FUNCTION NAME  mac_get_amr_trch_mapping_info()

 PURPOSE        This function computes the minimum set.

 DESCRIPTION    The minum set would be computed every time MAC is configured with a new
                TFCS table.
================================================================================================*/
void mac_get_amr_trch_mapping_info
(
  amr_trch_mapping_info_type *amr_trch_mapping
);
#endif

#ifdef FEATURE_TC_CONSTANT_DELAY
/*================================================================================================
 FUNCTION NAME  mac_calculate_constant_delay

 PURPOSE        This function calculates the constant delay based on the highest tti type
                configured for the transport channels

 DESCRIPTION    For constant delay in the case of multiple DTCH with different TTI, it has to
                be atleast maxTTI. Adding an additional delay of 5 ( for the corresponding
                data to be transmitted on the uplink). Now this delay should be a multiple
                of the maxTTI since data can be transmitted only at the end of the TTI.
================================================================================================*/
uint8 mac_calculate_constant_delay(l1_tti_enum_type tti_type);
#endif

/*================================================================================================
 FUNCTION NAME  eliminate_tfc_based_on_tti()

 PURPOSE        Eliminate TFCs based on TTI constraint

 DESCRIPTION    Once TF has been selected for a given transport channel, it cannot change
                until next TTI boundary for that transport channel. Hence, in the meantime, it is
                only possible to select TFCs that have the same TF for the transport channel.
                Since TTIs are aligned for all transport channels, a TTI boundary for one TTI
                length is also a boundary for all channels that have equal or shorter TTI length.
================================================================================================*/
void    eliminate_tfc_based_on_tti
(
/* Current TTI boundary */
 l1_tti_enum_type    cur_tti,
 rlc_ul_buf_status_type  *ul_buf_status_ptr
);

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN
/*====================================================================================================
 FUNCTION NAME      eliminate_tfc_based_on_ue_tx_pwr()

 DESCRIPTION        This function eliminates TFCs based on the UE Tx power
====================================================================================================*/

void eliminate_tfc_based_on_ue_tx_pwr(void);
#endif

#ifdef FEATURE_TFCS_CM_BY_HLS
/*====================================================================================================
 FUNCTION NAME      eliminate_tfc_based_on_cm()

 DESCRIPTION        This function eliminates TFCs based on Compressed Mode by Higher Layer Scheduling
 n  Layer 1 provides a call back function pointer to MAC at the time of build frame indication. MAC
 when needed uses the call back function to get an updated list of TFCI's from Layer 1. MAC provides
 Number of TFCs that are allowed after eliminating the TFC's based on TTI,TF constraints and UE Tx Power.
 L1 returns a list of TFC's which are restricted and allowed.
====================================================================================================*/
void eliminate_tfc_based_on_cm( void );
#endif

/*===================================================================================================
 FUNCTION NAME  eliminate_tfc_based_on_bo ()

 DESCRIPTION    This function eliminates Transport Format Combination based on Logical
                channel buffer constraints
====================================================================================================*/
void  eliminate_tfc_based_on_bo
(
/* Uplink Buffer Status Pointer */
rlc_ul_buf_status_type  *ul_buf_status_ptr
);

/*================================================================================================
 FUNCTION NAME      compute_tfc_bo ()

 DESCRIPTION        This function computes buffer Occupancy of each Transport Format
                    Combination
================================================================================================*/
void    compute_tfc_bo
(
void
);

/*================================================================================================
 FUNCTION NAME      compute_rlc_frame_format_based_on_priorities

 DESCRIPTION
================================================================================================*/
boolean compute_rlc_frame_format_based_on_priorities
(
/* Pointer to UL RLC Frame Format information */
rlc_ul_frame_format_type   *rlc_frame_fmt_ptr
);

/*================================================================================================
 FUNCTION NAME      pick_tfc_with_highest_bo

 DESCRIPTION        This function picks the TFC with highest buffer occupancy
================================================================================================*/
boolean     pick_tfc_with_highest_bo (void);

/*================================================================================================
 FUNCTION NAME  eliminate_tfc_based_on_active_trchs

 PURPOSE        This function populates a table which contains a list of transport channels
                and a flag indicating if they are currently active or not.
================================================================================================*/
uint8 eliminate_tfc_based_on_active_trchs (void);

/*====================================================================================================
 FUNCTION NAME      eliminate_tfc_based_on_tfi()

 DESCRIPTION        This function eliminates TFCs based on TFI constraint
====================================================================================================*/
void    eliminate_tfc_based_on_tfi
(
/* Transport channel index */
tr_ch_id_type   tr_index,

/* Non-allowed TFI */
uint16          non_allowed_tfi
);
#endif

