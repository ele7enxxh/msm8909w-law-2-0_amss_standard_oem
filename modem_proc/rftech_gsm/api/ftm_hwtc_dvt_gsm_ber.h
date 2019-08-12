/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       F T M  H W T C  D V T  S T R I N G  C O M M A N D S  M O D U L E

GENERAL DESCRIPTION
  This is the FTM HWTC DVT string commands module, contains core command 
  and it also serves as an example for how to add commands.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2003 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/ftm_hwtc_dvt_gsm_ber.h#1 $ 
  $DateTime: 2016/12/13 07:59:51 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/21/12   tws     Remove dependency on legacy GERAN API.
01/05/12   sr      geran api changes for non-signaling.
11/23/11   sr      removed GSDI related include files.
12-17-03   dp      Created
===========================================================================*/
#ifndef FTM_HWTC_DVT_GSM_BER_H
#define FTM_HWTC_DVT_GSM_BER_H

/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "queue.h"


#include "rr_l1.h"
#include "l2_l1.h"
#include "gl1_defs.h"
#include "sys_type.h"

#ifdef FEATURE_FTM_HWTC
#ifdef FEATURE_FTM_GSM_BER

/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
 
                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

typedef enum
{
    FTM_HWTC_DVT_GSM_BER_STATE_GSM_INACTIVE      = 0,
    FTM_HWTC_DVT_GSM_BER_STATE_GSM_NULL,
    FTM_HWTC_DVT_GSM_BER_STATE_ACQ,
    FTM_HWTC_DVT_GSM_BER_STATE_BCCH,
    FTM_HWTC_DVT_GSM_BER_STATE_IDLE,
    FTM_HWTC_DVT_GSM_BER_STATE_TCH,
    FTM_HWTC_DVT_GSM_BER_INVALID_STATE

} ftm_hwtc_dvt_gsm_ber_state_type;

typedef enum
{
    FTM_TCH_SLOT0 = 0,
    FTM_TCH_SLOT1,
    FTM_TCH_SLOT2,
    FTM_TCH_SLOT3,
    FTM_TCH_SLOT4,
    FTM_TCH_SLOT5,
    FTM_TCH_SLOT6,
    FTM_TCH_SLOT7,
    FTM_TCH_SLOT8,
    FTM_TCH_SLOT_INVALID
} ftm_hwtc_dvt_gsm_ber_tch_slot_type;

typedef enum
{
    FTM_GSM_LOOPBACK_TYPE_NULL    = GL1_DEFS_NO_LOOPBACK,
    FTM_GSM_LOOPBACK_TYPE_A       = GL1_DEFS_LOOPBACK_TYPE_A,
    FTM_GSM_LOOPBACK_TYPE_B       = GL1_DEFS_LOOPBACK_TYPE_B,
    FTM_GSM_LOOPBACK_TYPE_C       = GL1_DEFS_LOOPBACK_TYPE_C, 
#ifdef FEATURE_GSM_HALF_RATE
                           
    FTM_GSM_LOOPBACK_TYPE_D       = GL1_DEFS_LOOPBACK_TYPE_D,
    FTM_GSM_LOOPBACK_TYPE_E       = GL1_DEFS_LOOPBACK_TYPE_E,
    FTM_GSM_LOOPBACK_TYPE_F       = GL1_DEFS_LOOPBACK_TYPE_F, 
#endif
    FTM_GSM_NUM_LOOPBACK_TYPES
} ftm_hwtc_dvt_gsm_ber_loopback_type;

typedef ARFCN_T ftm_hwtc_dvt_gsm_ber_arfcn_type;

typedef struct
{

    uint32            numFrames;
    uint32            numGoodFrames;
} ftm_hwtc_dvt_gsm_bler_type;

typedef struct
{
    
    BSIC_T                               bsic;
} ftm_hwtc_dvt_gsm_ber_network_parms_type;

typedef struct
{

    ftm_hwtc_dvt_gsm_ber_state_type          state;
    ftm_hwtc_dvt_gsm_ber_arfcn_type          arfcn; 
    ftm_hwtc_dvt_gsm_ber_tch_slot_type       tch_slot;
    ftm_hwtc_dvt_gsm_ber_loopback_type       loopback_type;
    ftm_hwtc_dvt_gsm_bler_type               bler_data[8];
    ftm_hwtc_dvt_gsm_ber_network_parms_type  network_parms;

} ftm_hwtc_dvt_gsm_ber_status_type;



typedef enum
{

    FTM_START_GSM_MODE_REQ       = MPH_START_GSM_MODE_REQ,
    FTM_STOP_GSM_MODE_REQ        = MPH_STOP_GSM_MODE_REQ,
    FTM_SELECT_SPECIFIC_BCCH_REQ = MPH_SELECT_SPECIFIC_BCCH_REQ,
    FTM_START_IDLE_REQ           = MPH_START_IDLE_REQ,
    FTM_POWER_SCAN_REQ           = MPH_POWER_SCAN_REQ,
    FTM_CHANNEL_ASSIGN_REQ       = MPH_CHANNEL_ASSIGN_REQ,
    FTM_CHANNEL_RELEASE_REQ      = MPH_CHANNEL_RELEASE_REQ,
    FTM_DEDICATED_UPDATE_REQ     = MPH_DEDICATED_UPDATE_REQ,

#ifdef FEATURE_INTERRAT_NON_BLIND_HANDOVER_GTOW
    FTM_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ = MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ,
#endif /* FEATURE_INTERRAT_NON_BLIND_HANDOVER_GTOW */

    FTM_SELECT_SPECIFIC_BCCH_CNF = MPH_SELECT_SPECIFIC_BCCH_CNF,
    FTM_START_GSM_MODE_CNF       = MPH_START_GSM_MODE_CNF,
    FTM_STOP_GSM_MODE_CNF        = MPH_STOP_GSM_MODE_CNF,
    FTM_CHANNEL_ASSIGN_CNF       = MPH_CHANNEL_ASSIGN_CNF,
    FTM_POWER_SCAN_CNF           = MPH_POWER_SCAN_CNF,
    FTM_SERVING_DED_MEAS_IND     = MPH_SERVING_DED_MEAS_IND,
    FTM_SURROUND_MEAS_IND        = MPH_SURROUND_MEAS_IND
} ftm_gsm_l1_msg_id_type;



typedef enum
{
   FTM_PH_DATA_REQ                       = PH_DATA_REQ,
   FTM_PH_EMPTY_FRAME_REQ                = PH_EMPTY_FRAME_REQ,
   FTM_PH_RANDOM_ACCESS_REQ              = PH_RANDOM_ACCESS_REQ,
                                    
   FTM_PH_DATA_IND                       = PH_DATA_IND,
   FTM_PH_CONNECT_IND                    = PH_CONNECT_IND,
   FTM_PH_READY_TO_SEND_IND              = PH_READY_TO_SEND_IND,
   FTM_PH_RANDOM_ACCESS_CNF              = PH_RANDOM_ACCESS_CNF,
   FTM_PH_STOP_SACCH_MEASUREMENT_REPORT  = PH_STOP_SACCH_MEASUREMENT_REPORT
} ftm_gsm_l2_l1_msg_id_type;


typedef union
{

  IMH_T                   message_header;
  ph_data_ind_T           ph_data_ind;
  ph_connect_ind_T        ph_connect_ind;
  ph_ready_to_send_ind_T  ph_ready_to_send_ind;
  ph_random_access_cnf_T  ph_random_access_cnf;


} ftm_pseudo_l2_msg_type;

typedef union
{
  IMH_T                                  message_header;
  mph_serving_ded_meas_ind_T             mph_serving_ded_meas_ind;
  mph_serving_meas_ind_t                 mph_serving_meas_ind;
  mph_channel_assign_cnf_T               mph_channel_assign_cnf;
  mph_channel_release_cnf_T              mph_channel_release_cnf;
  mph_select_specific_bcch_cnf_T         mph_select_specific_bcch_cnf;
  /* mph_geran_to_ftm_padding_T is not used for messages but for adding to 
     this union to make it at least as big as the RR -> L1 union */
  mph_geran_to_ftm_padding_T             mph_geran_to_ftm_padding;

} ftm_pseudo_rr_msg_type;


typedef union
{
        ftm_pseudo_rr_msg_type       ftm_pseudo_rr_msg;
        ftm_pseudo_l2_msg_type       ftm_pseudo_l2_msg;
} ftm_gsm_ber_msg;

typedef struct
{
    q_link_type            link;
    ftm_gsm_ber_msg        msg;
} ftm_gsm_ber_queue_type;





#define MS_FTM_P_RR_L1   MS_RR_L1
#define MS_FTM_P_L2_L1   MS_L2_L1


typedef rr_l1_pscan_results_T   ftm_l1_pscan_results_T;

typedef mph_add_channel_assign_cnf_T               ftm_gsm_add_channel_assign_cnf_type;           
typedef mph_alternate_bcch_monitor_cnf_T           ftm_gsm_alternate_bcch_monitor_cnf_type;       


void ftm_hwtc_dvt_gsm_ber_init( void);

#endif /* FEATURE_FTM_GSM_BER */
#endif /* FEATURE_FTM_HWTC */
#endif /* FTM_HWTC_DVT_GSM_BER_H */
