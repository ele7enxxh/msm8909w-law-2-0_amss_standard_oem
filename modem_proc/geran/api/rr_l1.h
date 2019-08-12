#ifndef RR_L1_H
#define RR_L1_H
/*========================================================================
                 GSM GPRS RR to L1 DEFINITIONS FILE
DESCRIPTION
   This file contains definitions used by both the GSM/GPRS RR Layer and 
   L1 layer.

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_l1.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
01/10/13    sk      CR552259 CR sync from 1.0/1.1
11-02-16    tjw     API content split out from the original GERAN internal file
                    previously used by GERAN clients.
========================================================================== */
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "nas_exp.h"
#include "sys_type.h"
#include "geran_dual_sim.h"

typedef struct
{
   byte        PLMN_colour_code;
   byte        BS_colour_code;
} BSIC_T;

/* ******************************************************* *
 * Please do not renumber this list!!! -- used by logging  *
 * Obsolete messages that are removed by either marking    *
 * them RESERVED or not reusing their number. New messages *
 * get new numbers                                         *
 * ******************************************************* */
typedef enum
{ /* RR->L1 primitives */
/* MPH_ADD_CHANNEL_ASSIGN_REQ            = 0x00  */
/*,MPH_ALTERNATE_BCCH_MONITOR_REQ        = 0x01  */
   MPH_CHANGE_PAGE_MODE_REQ              = 0x02
  ,MPH_CHANNEL_ASSIGN_REQ                = 0x03
  ,MPH_CHANNEL_MODE_MODIFY_REQ           = 0x04
  ,MPH_CHANNEL_RELEASE_REQ               = 0x05
  ,MPH_DEDICATED_UPDATE_REQ              = 0x06
/*,MPH_FIND_BCCH_REQ                     = 0x07  */
  ,MPH_FREQUENCY_REDEFINITION_REQ        = 0x08
  ,MPH_HANDOVER_REQ                      = 0x09
  ,MPH_IDLE_UPDATE_REQ                   = 0x0a
  ,MPH_IMMED_ASSIGN_REQ                  = 0x0b
/*,MPH_PART_CHANNEL_RELEASE_REQ          = 0x0c  */
/*,MPH_READ_EXT_SYS_INFO_REQ             = 0x0d  */
  ,MPH_READ_SYS_INFO_REQ                 = 0x0e
  ,MPH_RECONNECT_CHANNEL_REQ             = 0x0f
/*,MPH_RESELECTION_REQ                   = 0x10  */
  ,MPH_SELECT_SPECIFIC_BCCH_REQ          = 0x11
  ,MPH_SET_CIPHERING_MODE_REQ            = 0x12
  ,MPH_START_IDLE_REQ                    = 0x13
  ,MPH_STOP_HANDOVER_ACCESS_REQ          = 0x14
/*,MPH_SURROUND_UPDATE_REQ               = 0x15  */
  ,MPH_ABORT_RA_REQ                      = 0x16
  ,MPH_EARLY_CAMPING_REQ                 = 0x17
  ,MPH_DECODE_BCCH_LIST_REQ              = 0x18
  ,MPH_DECODE_BCCH_LIST_ABORT_REQ        = 0x19
  ,MPH_POWER_SCAN_REQ                    = 0x1A
/*,MPH_STOP_CAMPING_REQ                  = 0x1B  */
  ,MPH_POWER_SCAN_ABORT_REQ              = 0x1C
  ,MPH_START_GSM_MODE_REQ                = 0x20
  ,MPH_STOP_GSM_MODE_REQ                 = 0x21
  ,MPH_UE_MODE_CHANGE_REQ                = 0x22
/*,MPH_STOP_READ_SYS_INFO_REQ            = 0x23  */
/*,MPH_READ_EXT_BCCH_REQ                 = 0x25  */
  ,MPH_WCDMA_CELL_UPDATE_LIST_REQ        = 0x26
  ,MPH_IGNORE_WCDMA_NEIGHBORS_REQ        = 0x27
  ,MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ = 0x29
  ,MPH_START_GPRS_IDLE_MODE_REQ          = 0x34
  ,MPH_GPRS_L1PARAM_UPDATE_REQ           = 0x35
  ,MPH_NC_MEASUREMENTS_REQ               = 0x36
  ,MPH_STOP_NC_MEASUREMENTS_REQ          = 0x37
/*,MPH_EXT_MEASUREMENTS_REQ              = 0x38 */
/*,MPH_STOP_EXT_MEASUREMENTS_REQ         = 0x39 */
  ,MPH_GPRS_SURROUND_UPDATE_REQ          = 0x3a
  ,MPH_GPRS_RESELECTION_REQ              = 0x3b
  ,MPH_START_MM_NON_DRX_IND              = 0x3c
  ,MPH_STOP_MM_NON_DRX_IND               = 0x3d
  ,MPH_INTERRAT_LIST_SRCH_TIME_REQ       = 0x3e
  ,MPH_SUSPEND_GSM_MODE_REQ              = 0x3f
  ,MPH_RESUME_GSM_MODE_REQ               = 0x40
  ,MPH_START_GSM_MODE_FOR_BPLMN_REQ      = 0x41
  ,MPH_RESUME_GSM_MODE_FOR_BPLMN_REQ     = 0x42
  ,MPH_RAT_CHANGE_IND                    = 0x43
  ,MPH_ENABLE_SLEEP_IND                  = 0x44
  ,MPH_DEVICE_MODE_IND                   = 0x45
  ,MPH_SET_PRIORITY_IND                  = 0x46
  ,MPH_DSC_THRESHOLD_IND                 = 0x47
  ,MPH_NV_REFRESH_REQ                    = 0x48
  ,MPH_SPECIFIC_IRAT_FREQ_UPDATE_REQ     = 0x49  
  ,MPH_DEEP_SLEEP_IND                    = 0x4A
  ,MPH_UPDATE_IDLE_DRX_IND               = 0x4B
  ,MPH_REDUCED_NCELL_MEAS_IND            = 0x4C

  ,MPH_RESET_REQ                         = 0x7f

  /* RR<-L1 primitives */
/*,MPH_ADD_CHANNEL_ASSIGN_CNF            = 0x80 */
/*,MPH_ALTERNATE_BCCH_MONITOR_CNF        = 0x81 */
  ,MPH_BLOCK_QUALITY_IND                 = 0x82
  ,MPH_CHANNEL_ASSIGN_CNF                = 0x83
  ,MPH_CHANNEL_MODE_MODIFY_CNF           = 0x84
  ,MPH_CHANNEL_RELEASE_CNF               = 0x85
/*,MPH_FIND_BCCH_CNF                     = 0x86 */
  ,MPH_FREQUENCY_REDEFINITION_CNF        = 0x87
  ,MPH_HANDOVER_CNF                      = 0x88
  ,MPH_HANDOVER_IND                      = 0x89
  ,MPH_IMMED_ASSIGN_CNF                  = 0x8a
/*,MPH_PART_CHANNEL_RELEASE_CNF          = 0x8b */
  ,MPH_RECONNECT_CHANNEL_CNF             = 0x8c
  ,MPH_SELECT_SPECIFIC_BCCH_CNF          = 0x8d
  ,MPH_SERVING_DED_MEAS_IND              = 0x8e
  ,MPH_SERVING_IDLE_MEAS_IND             = 0x8f
  ,MPH_SET_CIPHERING_CNF                 = 0x90
  ,MPH_STOP_HANDOVER_ACCESS_CNF          = 0x91
  ,MPH_SURROUND_MEAS_IND                 = 0x92
  ,MPH_SYS_INFO_1_ABSENT_IND             = 0x93
  ,MPH_RESELECTION_CNF                   = 0x94
  ,MPH_DECODE_BCCH_LIST_CNF              = 0x95
  ,MPH_DECODE_BCCH_LIST_FAILURE_IND      = 0x96
  ,MPH_POWER_SCAN_CNF                    = 0x97
  ,MPH_SURROUND_UPDATE_FAILURE_IND       = 0x98
  ,MPH_SPECIFIC_BCCH_DECODE_FAILURE_IND  = 0x99
  ,MPH_SURROUND_WCDMA_IDLE_MEAS_IND      = 0x9a
  ,MPH_SERVING_AUX_MEAS_IND              = 0x9b
  ,MPH_READ_SYS_INFO_FAILURE_IND         = 0x9c
  ,MPH_FCCH_SCH_DECODE_IND               = 0x9d
  ,MPH_FREQUENCY_REDEFINITION_IND        = 0x9e

  ,MPH_START_GSM_MODE_CNF                = 0xa0
  ,MPH_STOP_GSM_MODE_CNF                 = 0xa1
  ,MPH_NC_MEASUREMENT_IND                = 0xb1
/*,MPH_EXT_MEASUREMENT_IND               = 0xb2 */
  ,MPH_SERVING_MEAS_IND                  = 0xb4
  ,MPH_STOP_SURROUND_UPDATE_REQ          = 0xb5
  ,MPH_SURROUND_WCDMA_DEDICATED_MEAS_IND = 0xc0
  ,MPH_SURROUND_WCDMA_XFER_MEAS_IND      = 0xc1
  ,MPH_START_IDLE_MODE_CNF               = 0xd0
  ,MPH_INTERRAT_LIST_SRCH_TIME_IND       = 0xd1
  ,MPH_SUSPEND_GSM_MODE_CNF              = 0xd2
  ,MPH_RESUME_GSM_MODE_CNF               = 0xd3
  ,MPH_SUSPEND_GSM_MODE_IND              = 0xd4
  ,MPH_START_GSM_MODE_FOR_BPLMN_CNF      = 0xd5
  ,MPH_RESUME_GSM_MODE_FOR_BPLMN_CNF     = 0xd6

  ,MPH_DTM_CHANNEL_RELEASE_REQ           = 0xda
  ,MPH_DTM_CHANNEL_RELEASE_IND           = 0xdb
  ,MPH_DTM_CHANNEL_ASSIGN_CNF            = 0xdc
  ,MPH_DTM_L1PARAM_UPDATE_REQ            = 0xde

  ,MPH_SURROUND_IRAT_MEAS_IND            = 0xdf

  ,MPH_L1_TEST_MODE_START_IND            = 0xe0
  ,MPH_L1_TEST_MODE_STOP_IND             = 0xe1

  ,MPH_L1_DS_ABORT_IND                   = 0xe2
  ,MPH_ABORT_RA_CNF                      = 0xe3
  ,MPH_UE_MODE_CHANGE_CNF                = 0xe4
  ,MPH_COEX_MSC_CHANGE_IND               = 0xe5
  ,MPH_NV_REFRESH_CNF                    = 0xe6
  ,MPH_XMSI_UPDATE_IND                   = 0xe7

  ,MPH_RESET_IND                         = 0xfd
  ,MPH_RESET_CNF                         = 0xfe
  ,MPH_INVALID_MESSAGE_ID                = 0xff

}rr_l1_message_id_enum_T;

#define SCH_BLOCK_SIZE     4
#define MAX_MA_CHANNEL     64

typedef byte paging_mode_T;

#define NORMAL_PAGING           0
#define EXTENDED_PAGING         1
#define PAGING_REORGANIZATION   2
#define SAME_AS_BEFORE          3

#define RR_MAX_PSCAN_FREQ_SCAN_ARRAY_SIZE 971

typedef struct
{
  ARFCN_T arfcn;
  byte    RXLEV_average;
} rr_l1_freq_power_T;

typedef struct
{
  word                num_frequencies;
  rr_l1_freq_power_T  meas[RR_MAX_PSCAN_FREQ_SCAN_ARRAY_SIZE];
} rr_l1_pscan_results_T;

typedef struct
{
   byte                 no_of_items;
   ARFCN_T              channel[MAX_MA_CHANNEL];
} frequency_list_T;

/*  TSC Training Sequence Code RANGE 0 TO 7
 *  hopping_flag = TRUE,  MAIO and HSN are Valid
 *         = FALSE, the frequency list contains only one entry.
 *  MAIO  Mobile Allocation Index Offset RANGE 0 to 63
 *  HSN   Hopping Sequence Number        RANGE 0 to 53
 *  frequency list hopping frequencies
 */

typedef struct
{
   byte                 tsc;
   boolean              hopping_flag;
   byte                 maio;
   byte                 hsn;
   frequency_list_T     frequency_list;
} frequency_information_T;

/* ------------------------------------------------------------------------------*/

/* The following types are referenced by FTM but have no supported functionality */

typedef struct
{
   IMH_T                 message_header;
   byte                  error_code;
} mph_add_channel_assign_cnf_T;

typedef struct
{
   IMH_T             message_header;
   ARFCN_T           non_serving_BCCH_carrier;
   byte              SCH_block[SCH_BLOCK_SIZE];
   byte              RXLEV_average;
   byte              error_code;
} mph_alternate_bcch_monitor_cnf_T;

/* ------------------------------------------------------------------------------*/

/* types required by ftm_gsm_ber.h */

/**********************************************************************/
/* Additional structure/enum components to support GPRS L1 primitives */
/**********************************************************************/

/***************************************************************
 * MPH_CHANNEL_RELEASE_CONFIRM primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * This primitive is sent as a confirmation of the
 * request of the same name. The Layer 1 just
 * released the main physical channel and now back to
 * idle mode
 ***************************************************************/

typedef struct
{
   IMH_T                   message_header;
   byte                    error_code;
//ifdef F EATURE_DUAL_SIM
   gas_id_t                gas_id;
//endif /* F EATURE_DUAL_SIM */
} mph_channel_release_cnf_T;

/******************************************************************************
 * MPH_SELECT_SPECIFIC_BCCH_CONF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 *
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   boolean           BCCH_found;
   byte              SCH_block[SCH_BLOCK_SIZE];
//ifdef F EATURE_DUAL_SIM
   gas_id_t                gas_id;
//endif /* F EATURE_DUAL_SIM */
} mph_select_specific_bcch_cnf_T;


/******************************************************************************
 * MPH_SERVING_DED_MEAS_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Serving cell measurement report in dedicated mode
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   byte              RXLEV_FULL_average;
   byte              RXLEV_SUB_average;
   byte              RXQUAL_FULL_average;
   byte              RXQUAL_SUB_average;
   boolean           DTX_used;
   word              trk_lo_adj;
   byte              RXLEV_VAL;
   byte              MEAN_BEP;
   byte              CV_BEP;
   byte              NBR_RCVD_BLOCKS;
//ifdef F EATURE_DUAL_SIM
   gas_id_t                gas_id;
//endif /* F EATURE_DUAL_SIM */
} mph_serving_ded_meas_ind_T;

/******************************************************************************
 * MPH_SERVING_MEAS_IND primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * Serving cell measurement report
 ******************************************************************************/
typedef struct
{
   IMH_T       message_header;
   byte        rxlev_average;
   boolean     i_level_valid;
   byte        i_level;
   boolean     SNR_is_bad;    //transfer SNR
   boolean     SNR_is_bad_valid;
   byte        rxqual;
//ifdef F EATURE_DUAL_SIM
   gas_id_t                gas_id;
//endif /* F EATURE_DUAL_SIM */
} mph_serving_meas_ind_t;

/**************************************************************************
 * MPH_CHANNEL_ASSIGN_CONF primitive
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION :
 * This primitive is used to confirm the request of the same name
 **************************************************************************/

typedef struct
{
   IMH_T                message_header;
   byte                 error_code;
//ifdef F EATURE_DUAL_SIM
   gas_id_t                gas_id;
//endif /* F EATURE_DUAL_SIM */
} mph_channel_assign_cnf_T;


/***************************************************************
 * MPH_CHANNEL_RELEASE_REQUEST primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * This primitive is used to order layer 1 to release the main
 * channels.
 ***************************************************************/

typedef struct
{
   IMH_T                   message_header;
   boolean                 valid_bcch_carrier;
   ARFCN_T                 serving_bcch_carrier;
   BSIC_T                  BSIC;
//ifdef F EATURE_DUAL_SIM
   gas_id_t                gas_id;
//endif /* F EATURE_DUAL_SIM */
} mph_channel_release_req_T;

/******************************************************************************
 * MPH_SELECT_SPECIFIC_BCCH_REQ primitive
 * FROM : RR
 * TO   : LAYER 1
 * DESCRIPTION :
 * RR wants Layer 1 to sit on a particular BCCH channel, about which it
 * already has system information
 ******************************************************************************/

typedef struct
{
   IMH_T             message_header;
   ARFCN_T           specific_channel_no;
   BSIC_T              bsic;
//ifdef F EATURE_DUAL_SIM
   gas_id_t                gas_id;
//endif /* F EATURE_DUAL_SIM */
} mph_select_specific_bcch_req_T;

/******************************************************************************
 * MPH_GERAN_TO_FTM_PADDING
 * FROM : LAYER 1
 * TO   : FTM
 * DESCRIPTION : Not used for messages but for adding to union to make it
 *               at least as big as the RR -> L1 union rr_l1_messages_u
 ******************************************************************************/
#define MPH_GERAN_TO_FTM_PADDING_SIZE 0xF80 /* linker reports rr_l1_messages_u is
                                               0xD0C bytes, allow some headroom */
typedef struct
{
  uint8 padding[MPH_GERAN_TO_FTM_PADDING_SIZE];
} mph_geran_to_ftm_padding_T;

/******************************************************************************
 * MPH_COEX_MSC_CHANGE_IND
 * FROM : LAYER 1
 * TO   : RR
 * DESCRIPTION : L1 to RR indication of max UL and DL slot restriction in COEX
 *               scenarios. If a value of zero is specified this should be
 *               interpreted as no restriction.
 ******************************************************************************/
typedef struct
{
  IMH_T             message_header;
  uint8             ul_slot_restriction;
  uint8             dl_slot_restriction;
  gas_id_t          gas_id;
} mph_coex_msc_change_ind_T;

#endif
