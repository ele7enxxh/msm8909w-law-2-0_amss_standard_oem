#ifndef TDSRRCDATA_H
#define TDSRRCDATA_H
/*===========================================================================
              R R C D A T A  H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  RRC Data
  
Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
Export of this technology or software is regulated by the U.S. Government. 
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rrc/inc/tdsrrcdata_v.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/14/12   as      Added code to support feature NB/WB handover support
10/20/11   dhf    Clean up redundant include files
06/16/11   bj     Added new definition about REL9.
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/10/11   ysh     Merged with VU_MODEM_WCDMA_RRC_OSYS.01.93.53
03/02/11   ad      Fixed compilation warnings on 3.0 PL
03/14/11   su      Added code changes to give UL/DL UE supported CAT and 
                   max. data rate possible to DS.
03/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.92.50
02/28/11   ad      Fixed compilation warnings on 3.0 PL
02/24/11   ad      Moved rrc_return_lc_info_for_rab protoype to rrcdata_v.h
02/19/11   sk      Created RRC_NV_REL_INDICATOR_MINIMUM_REL7 for GPS team. 
02/19/11   sk      Moved rrc_nv_rel_indicator to internal file. 
02/15/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.91.50
02/11/11   su      Added code to support setting UE HSDPA category 
                   through QXDM command.
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/03/11   ad      Added changes to refeaturize the API files
01/31/11   ad      Fixed compilation warnings
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/27/11   ad      Moving RRC_NV_REL_INDICATOR_REL7 to API folder
01/28/11   rl      Fixed compilation error
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/18/11   su      Fixed compilation error.
01/12/11   ad      Added changes for RRC SW decoupling
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50 
11/18/10   ysh     Save Prach info Transport Format Set. 3GPP-8.6.5.1(3.2.32)
                               Add some rom table declare.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/08/10   ysh     Add Uplink Physical Channel Control. 3Gpp-8.2.10(3.2.6)
11/04/10    yzh    (3GPP 10.3.6.21) modify Downlink DPCH info for each RL from FDD to TDD.
11/01/10    yzh    (3GPP 8.6.6.6) modify Uplink DPCH info from FDD to TDD
10/26/10   zwj     Added rel4 define. 
===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/
#include "tdscdma_variation.h"
#ifdef FEATURE_MVS
#include <mvsamr.h>
#endif /* FEATURE_MVS */

#include <mvs.h>
#include "modem_mem.h"
/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

//reserves
#define TDSNUM_OF_BITS_FOR_DL_DCCH_MSG_TYPE 0x05/* Number of bits for DL DCCH
                                                 Message type  */
    
#define TDSNUM_OF_BITS_FOR_UL_DCCH_MSG_TYPE 0x05/* Number of bits for DL DCCH
                                                 Message type  */
    
#define TDSNUM_OF_BITS_FOR_UL_CCCH_MSG_TYPE 0x02/* Number of bits for DL CCCH
                                                 Message type  */

    /* Minimum RSCP value conveyed to UI/CM during RL FAILURE */
#ifdef FEATURE_TDSCDMA_DATA_FOR_UI_BAR_DISPLAY
    
#define TDSMIN_RSCP_RL_FAILURE_OOS 121
#define TDSMIN_RSSI_RL_FAILURE_OOS 106
#define TDSMIN_ECIO_RL_FAILURE_OOS 24
    
#else
#define TDSMIN_RSCP_RL_FAILURE_OOS 240
#endif
    
    /*constant definition for number of default configs. specified in
      13.7 of 25.331*/
      /* TDSNUM_DEFAULT_CONFIGS has to be 13 for Rel-3 if all configurations are supported */
    /* For release-6 default configs upto 31 has been introduced */

#define TDSNUM_DEFAULT_CONFIGS 32

//end reserves

/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/


#ifdef FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA
/*This data type keeps track of whether CSoHSPA is active or not at RRC.*/
typedef enum
{
  TDSCSOHS_INACTIVE,
  TDSCSOHS_ACTIVE,
  TDSCSOHS_SUSPEND
} tdsrrc_cs_over_hs_status_e_type;
#endif /*FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA*/

#ifdef FEATURE_TDS_DC_HSDPA
/* DC-HSDPA Status Variable */
typedef enum
{
  TDSDCHSDPA_INACTIVE,
  TDSDCHSDPA_ACTIVE
} tdsrrc_dchsdpa_status_enum_type;
#endif/*FEATURE_TDS_DC_HSDPA*/



#ifdef FEATURE_TDSCDMA_VOC_AMR_WB
/* Define the types of codes receives in NAS-Syncronisation indicator in OTA msg */
typedef enum
{
  TDSRRC_GSM_FR=0,
  TDSRRC_GSM_HR=1,
  TDSRRC_GSM_EFR=2,
  TDSRRC_FR_AMR=3,
  TDSRRC_HR_AMR=4,
  TDSRRC_UMTS_AMR=5,
  TDSRRC_UMTS_AMR_2=6,
  TDSRRC_TDMA_EFR=7,
  TDSRRC_PDC_EFR=8,
  TDSRRC_FR_AMR_WB=9,
  TDSRRC_UMTS_AMR_WB=10,
  TDSRRC_OHR_AMR=11,
  TDSRRC_OFR_AMR_WB=12,
  TDSRRC_OHR_AMR_WB=13,
  TDSRRC_RESERVED=0xFF
}tdsrrc_codec_types;

#endif /*FEATURE_TDSCDMA_VOC_AMR_WB*/

typedef enum
{
  TDSRRC_MSG_VALID,
  TDSRRC_MSG_UNSUPPORTED_CONFIG, /* Message has un-supported configuration  */
  TDSRRC_MSG_PROTOCOL_ERROR,     /* Message has protocol error              */
  TDSRRC_MSG_INVALID_CONFIG      /* Message has invalid configuration       */
} tdsrrcllc_msg_failure_status_e_type;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* following structure is to keep track of the pdcp receive number.  This
   is to used when it needs to be included in a responce msg to UTRAN */
typedef struct 
{
  uint32 rb_id;     /* Specifies the rb_id */
  uint32 pdcp_sn;   /* Pdcp Sequence number of the rb */
}tdsrrc_rb_with_pdcp_info_type;


#ifdef FEATURE_UMTS_PDCP
/* Variable pdcp_sn_info as defined in spec */
typedef struct
{
  tdsrrc_rb_with_pdcp_info_type rrc_rb_with_pdcp_info[MAX_RAB_TO_SETUP];
}tdsrrc_pdcp_sn_info_type;
#endif /* FEATURE_UMTS_PDCP */

/*type to hold the byte array and the number of bytes in each of the byte
  array*/
typedef struct
{
  uint8 * default_config_ptr;                        /*ptr to default config
                                                      byte array          */    
#ifdef FEATURE_TDSCDMA_SRB5_SUPPORT
  uint32   default_config_len;                        /*number of bytes in
                                                      each of the arrays  */
#else
  uint8   default_config_len;                        /*number of bytes in
                                                      each of the arrays  */
#endif /* FEATURE_TDSCDMA_SRB5_SUPPORT */
}tdsrrc_default_config_info_type;

/* Data Structure to store RACH Transmission RRC Retry related statistics */
typedef struct
{
  /* Count which captures number of times RRC is successful in receiving RRC 
     procedure level response message from N/W for any RRC message sent on RACH. */
  uint32 succful_rrc_tran_retry_cnt ;
  /* Count which captures number of times RRC Retry Failed due L2 ACK IND 
     with cause failure in sending RRC message on RACH transport channel */
  uint32 rrc_retry_fail_due_to_cmac_status_ind_cnt;
  /* Counter is incremented when RRC exhausts with max time re-transmission
     before receiving RRC procedure level response message on RACH */
  uint32 rrc_retry_fail_due_to_others_cnt;
}tdsrrc_debug_rach_transmission_stats_type;

/* Data Structure to store the OOS related statistics */
typedef struct
{
  /* Number of times OOS is triggered in idle mode since last power up */
  uint32 oos_idle_cnt;
  /* Number of times OOS is triggered in connected mode since last power up */
  uint32 oos_connected_cnt;
  uint32 oos_connected_cnt_cell_fach;
  uint32 oos_connected_cnt_cell_dch;
  uint32 oos_connected_cnt_cell_pch;
  uint32 oos_connected_cnt_ura_pch;
}tdsrrc_debug_oos_stats_type;


/* Data Structure to store the TDSTOG reselection statistics */
typedef struct
{
  /* TDSTOG cell reselection count since last power up */
  uint32 tdstog_resel_cnt;
  /* TDSTOG cell reselection failure count since last power up */
  uint32 tdstog_resel_failure_cnt;
  uint32 tdstog_resel_failure_cnt_NBCCH_SETUP_FAILURE;
  uint32 tdstog_resel_failure_cnt_SIB_WAIT_TIMEOUT;
  uint32 tdstog_resel_failure_cnt_ACQ_FAILURE;
  uint32 tdstog_resel_failure_cnt_OTHER_CAUSE;
}tdsrrc_debug_td2g_resel_stats_type;

/* Data Structure to store the TDSTOG reselection statistics */
typedef struct
{
  /* TDSTOG cell reselection count since last power up */
  uint32 tdstol_resel_cnt;
  /* TDSTOG cell reselection failure count since last power up */
  uint32 tdstol_resel_failure_cnt;
  uint32 tdstol_resel_failure_cnt_NBCCH_SETUP_FAILURE;
  uint32 tdstol_resel_failure_cnt_SIB_WAIT_TIMEOUT;
  uint32 tdstol_resel_failure_cnt_ACQ_FAILURE;
  uint32 tdstol_resel_failure_cnt_OTHER_CAUSE;
}tdsrrc_debug_td2l_resel_stats_type;

typedef struct
{
    uint32 g2td_resel_cnt;
    uint32 g2td_resel_failure_cnt;
    uint32 g2td_resel_failure_cnt_NBCCH_SETUP_FAILURE;
    uint32 g2td_resel_failure_cnt_SIB_WAIT_TIMEOUT;
    uint32 g2td_resel_failure_cnt_ACQ_FAILURE;
    uint32 g2td_resel_failue_cnt_OTHER_CAUSE;
}tdsrrc_debug_g2td_resel_stats_type;

typedef struct
{
    uint32 l2td_resel_cnt;
    uint32 l2td_resel_failure_cnt;
    uint32 l2td_resel_failure_cnt_NBCCH_SETUP_FAILURE;
    uint32 l2td_resel_failure_cnt_SIB_WAIT_TIMEOUT;
    uint32 l2td_resel_failure_cnt_ACQ_FAILURE;
    uint32 l2td_resel_failue_cnt_OTHER_CAUSE;
}tdsrrc_debug_l2td_resel_stats_type;

typedef struct
{
    uint32 td_acq_attemp_cnt;
    uint32 td_acq_attemp_succeed_cnt;
    uint32 td_acq_attemp_failed_cnt;
}tdsrrc_debug_acq_stats_type;

typedef struct
{
    uint32 td2td_bplmn_cnt;
    uint32 td2g_bplmn_cnt;
    uint32 td2l_bplmn_cnt;
    uint32 l2td_bplmn_cnt;
    uint32 g2td_bplmn_cnt;
}tdsrrc_debug_bplmn_stats_type;

typedef struct
{
    uint32 tdsrrc_rce_attemp_cnt;  
    uint32 tdsrrc_rce_succeed_cnt;    
    uint32 tdsrrc_rce_failed_cnt;
}tdsrrc_debug_rce_stats_type;


/* Data Structure to store the intra frequency reselection statistics */
typedef struct
{
  /* Intra frequency reselection success count since last power up */
  uint32 intra_freq_resel_success_cnt;
  /* Intra frequency reselection failure count due to N-BCCH set up failue since last power up */
  uint32 intra_freq_nbcch_setup_failure_cnt;
  /* Intra frequency reselection failure count due to SIB wait time out since last power up */
  uint32 intra_freq_sib_wait_timeout_cnt;
  /* Intra frequency reselection failure count due to transition failures since last power up */
  uint32 intra_freq_transition_fail_cnt;
  /* Intra frequency reselection failure count due to other causes since last power up */
  uint32 intra_freq_failed_other_causes_cnt;
}tdsrrc_debug_intra_freq_resel_stats_type;

/* Data Structure to store the inter frequency reselection statistics */
typedef struct
{
  /* Inter frequency reselection success count since last power up */
  uint32 inter_freq_resel_success_cnt;
  /* Inter frequency reselection failure count due to N-BCCH set up failue since last power up */
  uint32 inter_freq_nbcch_setup_failure_cnt;
  /* Inter frequency reselection failure count due to SIB wait time out since last power up */
  uint32 inter_freq_sib_wait_timeout_cnt;
  /* Inter frequency reselection failure count due to transition failures since last power up */
  uint32 inter_freq_transition_fail_cnt;
  /* Inter frequency reselection failure count due to other causes since last power up */
  uint32 inter_freq_failed_other_causes_cnt;
}tdsrrc_debug_inter_freq_resel_stats_type;

typedef struct
{
    uint32 fast_return_trigger_cnt;
    uint32 fast_return_succeed_cnt;
}tdsrrc_debug_gtot_fast_return_stats_type;

/*data structure to hold all rrc debug info*/
typedef struct
{
    /* Intra frequency cell reselection statistics */
    tdsrrc_debug_intra_freq_resel_stats_type tdsrrc_debug_intra_freq_resel_stats;
    /* Inter frequency cell reselection statistics */
    tdsrrc_debug_inter_freq_resel_stats_type tdsrrc_debug_inter_freq_resel_stats;

    /* TDSTOG cell reselection statistics */
    tdsrrc_debug_td2g_resel_stats_type tdsrrc_debug_td2g_resel_stats;

    /* OOS related statistics */
    tdsrrc_debug_oos_stats_type tdsrrc_debug_oos_stats;

    /* TDSTOl cell reselection statistics */
    tdsrrc_debug_td2l_resel_stats_type tdsrrc_debug_td2l_resel_stats;

    tdsrrc_debug_g2td_resel_stats_type tdsrrc_debug_g2td_resel_stats;
    tdsrrc_debug_l2td_resel_stats_type tdsrrc_debug_l2td_resel_stats;
    tdsrrc_debug_acq_stats_type tdsrrc_debug_acq_stats;
    tdsrrc_debug_bplmn_stats_type tdsrrc_debug_bplmn_stats;
    tdsrrc_debug_rce_stats_type tdsrrc_debug_rce_stats;
    /* RACH transmission statistics, This is currently TBD. Its values are set to Zero for now */
    tdsrrc_debug_rach_transmission_stats_type tdsrrc_debug_rach_transmission_stats_stats;
    tdsrrc_debug_gtot_fast_return_stats_type tdsrrc_debug_gtot_fast_return_stats;
}tdsrrc_debug_global_var_type;

      /* Given a MIB of a cell, finds out whether the Cell is shared or not.*/
#define TDSRRC_IS_NETWORK_SHARED(mib_ptr)     \
    (    \
      (mib_ptr->m.v690NonCriticalExtensionsPresent) &&    \
        (mib_ptr->v690NonCriticalExtensions.masterInformationBlock_v690ext.m.multiplePLMN_ListPresent)    \
    )

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

extern tdsrrcllc_msg_failure_status_e_type tdsrrc_msg_failure_cause;

extern boolean tdsrrc_skip_dynamic_power_class;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */



/*===========================================================================

FUNCTION tdsrrc_log_protocol_error

DESCRIPTION
  This function logs the protocol error


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrc_log_protocol_error
(
  uint8 rrc_state,                /* The RRC state */
  uint8 rrc_procedure,            /* The RRC procedure */
  uint8 failure_cause,            /* The failure cause */
  uint8 protocol_error_cause      /* The protocol error cause */  
);

/*===========================================================================

FUNCTION tdsrrc_log_acq_info

DESCRIPTION
  This function logs the acq_info


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrc_log_acq_info
(
  int16                    freq_index,
  uint8                    num_freqs,
  uint8                    acq_mode
);
#ifdef FEATURE_TDSCDMA_DSDS_TUNEAWAY
/*===========================================================================

FUNCTION tdsrrc_log_dsds_lta_info

DESCRIPTION
  This function logs the dsds lta info


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrc_log_dsds_lta_info
(
  void
);
#endif

#if defined (FEATURE_TDSCDMA_DSDS_TUNEAWAY) || defined (FEATURE_TDSCDMA_DSDS_QTA)
/*===========================================================================

FUNCTION  tdsrrc_event_log_dsds_ta_block

DESCRIPTION

  This show the event that LTA/QTA is block

DEPENDENCIES
  None.
 
RETURN VALUE

  Lock status.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_event_log_dsds_ta_block
(
  void
);

#endif
/*===========================================================================

FUNCTION tdsrrc_log_map_protocol_cause

DESCRIPTION
  This function maps the asn1 cause to an internal macros

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 tdsrrc_log_map_protocol_cause
(
  tdsrrc_ProtocolErrorCause prot_err_cause
);

/*===========================================================================

FUNCTION tdsrrc_log_map_procedure_name

DESCRIPTION
  This function maps RRC procedure name to Apex procedure name

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 tdsrrc_log_map_procedure_name
(
  uint8 rrc_procedure            /* The RRC procedure */
);

/*===========================================================================

FUNCTION          RRC_CONVERT_IMEI_INTO_INT_VALUE


DESCRIPTION       
  This function translates a IMEI internal type  into  an interger value.

DEPENDENCIES
  None

RETURN VALUE
                  
  uint64: returns an integer value of the IMEI

SIDE EFFECTS
                  
  None

===========================================================================*/
uint64 tdsrrc_convert_imei_into_int_value 
(
  rrc_imei_gsm_map_type  int_imei    /* Local variable internal IMEI type */

);

/*===========================================================================

FUNCTION          RRC_CONVERT_TMSI_MAP_INTO_INT_VALUE


DESCRIPTION       

  This function translates a TMSI internal type  into
  an interger value type used by RRC LLC. 
  
  FROM 25.304 
  For UE-Id types "TMSI and LAI (GSM-MAP)" or "P-TMSI and RAI (GSM-MAP)", only 
  the TMSI or P-TMSI parts given as Bitstring(32) shall be used, and "Initial 
  UE Identity" shall be interpreted as a binary representation of an integer 
  number, where the first bit in the Bitstring represents the highest order bit.  

DEPENDENCIES
  None

RETURN VALUE
                  
  uint64: returns an integer value of the TMSI

SIDE EFFECTS
                  
  None

===========================================================================*/
uint64 tdsrrc_convert_tmsi_map_into_int_value 
(
  rrc_tmsi_gsm_map_type  int_tmsi    /* Pointer to internal TMSI type */
);

/*===========================================================================

FUNCTION          RRC_CONVERT_PTMSI_MAP_INTO_INT_VALUE


DESCRIPTION

  This function translates a PTMSI internal type  into
  an interger value type used by RRC LLC.

  FROM 25.304
  For UE-Id types "TMSI and LAI (GSM-MAP)" or "P-TMSI and RAI (GSM-MAP)", only
  the TMSI or P-TMSI parts given as Bitstring(32) shall be used, and "Initial
  UE Identity" shall be interpreted as a binary representation of an integer
  number, where the first bit in the Bitstring represents the highest order bit.

DEPENDENCIES
  None

RETURN VALUE

  uint64: returns an integer value of the PTMSI

SIDE EFFECTS

  None

===========================================================================*/
uint64 tdsrrc_convert_ptmsi_map_into_int_value
(
  rrc_ptmsi_gsm_map_type  int_ptmsi    /* Pointer to internal PTMSI type */
);

#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif

#ifdef FEATURE_TDS_DC_HSDPA
/*===========================================================================
FUNCTION tdsrrc_get_hsdpa_plus_status

DESCRIPTION
  This function returns DC HSDPA Status
  
DEPENDENCIES

RETURN VALUE
  DC HSDPA status of type tdsrrc_dchsdpa_status_enum_type

SIDE EFFECTS
  None
===========================================================================*/
tdsrrc_dchsdpa_status_enum_type tdsrrc_get_dchsdpa_status
(
  void
);
#endif/*FEATURE_TDS_DC_HSDPA*/

//controversial
/*====================================================================
FUNCTION: rrc_get_uplink_rab_rate

DESCRIPTION:
  This function returns the uplink rate of the RAB passed in

DEPENDENCIES:
  None

RETURN VALUE:
  uint32

SIDE EFFECTS:
====================================================================*/
extern uint32 tdsrrc_get_uplink_rb_rate(uint8 lc_id);

/*====================================================================
FUNCTION: rrc_get_downlink_rab_rate

DESCRIPTION:
  This function returns the downlink rate of the RAB passed in

DEPENDENCIES:
  None

RETURN VALUE:
  uint32

SIDE EFFECTS:
====================================================================*/
extern uint32 tdsrrc_get_downlink_rb_rate(uint8 lc_id);


/*===========================================================================
FUNCTION rrc_register_with_msgr_for_client_and_register_umids

DESCRIPTION
  This function rregisters RRC with MSGR and registers for UMIDs it receives
  
DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrc_register_with_msgr_for_client_and_register_umids
(
  void
);

/*===========================================================================
FUNCTION tdsrrc_deregister_with_msgr

DESCRIPTION
  This function delete the registered MSGR client
  
DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrc_deregister_with_msgr(void);

/*============================================================================
FUNCTION: tdsrrcllc_get_64QAM_status()

DESCRIPTION:
This function checks whether 64QAM is configured or not.

DEPENDENCIES:
  None.

RETURN VALUE:
If 64QAM is configured it returns TRUE else it returns false.

SIDE EFFECTS:
=============================================================================*/

boolean tdsrrcllc_get_64QAM_status
(
  void
);


/*============================================================================
FUNCTION: tdsrrc_mvssup_wcdma_set_ul_channel()

DESCRIPTION:
This function sets the ul channel parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void tdsrrc_mvssup_wcdma_set_ul_channel
(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class
);
/*============================================================================
FUNCTION: tdsrrc_mvssup_wcdma_set_dl_channel()

DESCRIPTION:
This function sets the dl channel parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void tdsrrc_mvssup_wcdma_set_dl_channel
(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
  , mvssup_wcdma_blk_size_cb_set_type *blk_size_cb_set
#endif  /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

);

#if defined(FEATURE_TDSCDMA_VOC_AMR_WB) || defined(FEATURE_TDSCDMA_RRC_INDICATE_CODEC_TO_CM)
/*===========================================================================

FUNCTION tdsrrc_get_current_codec_mode

DESCRIPTION
  This  function returns that the Current codec mode type that is set 

DEPENDENCIES
  None.

RETURN VALUE
  mvs_mode_type: rteurns the  Current codec mode type that is set 

===========================================================================*/
mvs_mode_type tdsrrc_get_current_codec_mode
(
  void
);

/*===========================================================================

FUNCTION tdsrrc_set_current_codec_mode

DESCRIPTION
  This  function sets the current codec mode with value configured in MVS
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void tdsrrc_set_current_codec_mode
(
  mvs_mode_type new_codec_mode
);

/*===========================================================================

FUNCTION  tdsrrc_translate_nas_sync_indicator

DESCRIPTION This function translates a Nas-Syncronisation-indicator defined by
 ASN.1 into the internal type used by RRC. 

DEPENDENCIES

  The algorithm used assumes that the processor is Little Endian.
  
RETURN VALUE
                  
  Nas-Syncronisation-indicator converted to the internal type rrc_codec_types


===========================================================================*/
mvs_mode_type tdsrrc_translate_nas_sync_indicator
(
  ASN1BitStr32  *nas_sync_indicator         /* Pointer to external PLMN type */
);

/*===========================================================================

FUNCTION  tdsrrc_set_ordered_codec_mode

DESCRIPTION This function sets the codec mode ordered in the ota messages

DEPENDENCIES
  None
  
RETURN VALUE          
  None

===========================================================================*/
void tdsrrc_set_ordered_codec_mode
(
  mvs_mode_type nas_sync_ind
);

/*===========================================================================

FUNCTION  tdsrrc_get_ordered_codec_mode

DESCRIPTION This function sets the codec mode ordered in the ota messages

DEPENDENCIES
  None
  
RETURN VALUE          
  None

===========================================================================*/
mvs_mode_type tdsrrc_get_ordered_codec_mode
(
  void
);

#endif /*FEATURE_TDSCDMA_VOC_AMR_WB*/

#ifdef FEATURE_TDSCDMA_3GPP_FAST_DORMANCY_ENHANCEMENT
/*===========================================================================

FUNCTION   tdsrrc_check_psrab_data_is_present

DESCRIPTION
  This function check for whether the WM size corresponding
  any of the rb_id is zero or not.

DEPENDENCIES
  None.

RETURN VALUE
  FALSE, if WM_size for all the rb_id is 0.
  TRUE, if WM_size for all the rb_id is non-zero. 

SIDE EFFECTS
  None.
===========================================================================*/
boolean tdsrrc_check_psrab_data_is_present
(
  void
);
#endif

/*============================================================================
FUNCTION: tdsrrc_mvssup_get_ul_amr_mode()

DESCRIPTION:
This function gets the UL frame mode for AMR mode

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
mvs_amr_mode_type tdsrrc_mvssup_get_ul_amr_mode
(
  void 
);
/*============================================================================
FUNCTION: tdsrrc_mvssup_get_ul_amb_mode()

DESCRIPTION:
This function gets the UL frame mode for AMR WB mode

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
mvs_amr_mode_type tdsrrc_mvssup_get_ul_awb_mode
(
  void 
);
#endif
