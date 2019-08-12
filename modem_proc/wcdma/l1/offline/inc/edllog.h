#ifndef EDLLOG_H
#define EDLLOG_H

/*============================================================================
              EDLLOG.H     Logging support for the DL channels for enhanced uplink

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
the logging related code for the DL channels for EUL support.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006, 2008 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/edllog.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
08/18/14    tsk     Jolokia 1.0: Fixes for EUL, DC, 16QAM feature bringup.
07/28/14    pkg     Code Check-in for 16QAM feature.
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
06/12/13    ymu     Fix KW errors, deadlock crash and 435B crash. 
05/22/13    oh      Adding DC-HSUPA framework support
05/23/13    geg     WSW decoupling (except for component of QCHAT)
02/01/12    gnk     Nikel Feature cleanup.
09/22/09    sup     Removed log code assignments and placed them in 
                    log_codes_wcdma_int_v.h
02/06/09    sa      Fix for EDL channel log packet crash and double buffering changes
10/03/08    hk      Bringing in 7k Mailine fixes
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
05/21/07    gv      Changes to support the 4302 log packet re-design.
10/03/06    gv      Moved the TTI information in the 4301 log packet to the
                    common header.
09/25/06    gv      Added support for the dynamic headers for 4301 log packet.
08/14/06    yiz     Mainline HSUPA code
05/23/06    gv      file created.
                    
===========================================================================*/

/* <EJECT> */

/*===========================================================================*/

/*===========================================================================

                     DEFINE/UNDEFINE LOCAL FEATURES  

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "msm.h"
#include "log.h"
#include "log_codes.h"
#include "log_codes_wcdma_int.h"
#include "edlcfg.h"

/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/

/* For log packet: EUL DL Channel Configuration Log Packet: 
** This log packet should be logged every time there is an EUL start stop or reconfig.
** This log packet is only for DL channels. This log packet shows for every active EUL RL
** the different DL channels that are active.*/

/*==================WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET ==================*/
/* Defining macros for setting different bit fields in the log packet information. Each byte in the
** log packet  has multiple fields*/

#define WCDMA_EDL_CHAN_RL_INFO_CELL_IDX_BMSK 0x07
#define WCDMA_EDL_CHAN_RL_INFO_CELL_IDX_SHIFT 0

#define WCDMA_EDL_CHAN_RL_INFO_RLS_IDX_BMSK 0x38
#define WCDMA_EDL_CHAN_RL_INFO_RLS_IDX_SHIFT 3

#define WCDMA_EDL_CHAN_RL_INFO_SERV_CELL_BMSK 0x40
#define WCDMA_EDL_CHAN_RL_INFO_SERV_CELL_SHIFT 6

#define WCDMA_EDL_CHAN_AGCH_ERNTI_INFO_AGCH_ACTION_BMSK 0x07
#define WCDMA_EDL_CHAN_AGCH_ERNTI_INFO_AGCH_ACTION_SHIFT 0

#define WCDMA_EDL_CHAN_AGCH_ERNTI_INFO_AGCH_PRESENT_BMSK 0x08
#define WCDMA_EDL_CHAN_AGCH_ERNTI_INFO_AGCH_PRESENT_SHIFT 3

#define WCDMA_EDL_CHAN_AGCH_ERNTI_INFO_P_ERNTI_PRESENT_BMSK 0x10
#define WCDMA_EDL_CHAN_AGCH_ERNTI_INFO_P_ERNTI_PRESENT_SHIFT 4

#define WCDMA_EDL_CHAN_AGCH_ERNTI_INFO_S_ERNTI_PRESENT_BMSK 0x20
#define WCDMA_EDL_CHAN_AGCH_ERNTI_INFO_S_ERNTI_PRESENT_SHIFT 5

#define WCDMA_EDL_CHAN_AGCH_ERNTI_INFO_S_CELL_IDX_BMSK 0xC0
#define WCDMA_EDL_CHAN_AGCH_ERNTI_INFO_S_CELL_IDX_SHIFT 6

#define WCDMA_EDL_CHAN_STTD_SG_TTI_INFO_INIT_SG_BMSK 0x1F
#define WCDMA_EDL_CHAN_STTD_SG_TTI_INFO_INIT_SG_SHIFT 0

#define WCDMA_EDL_CHAN_STTD_SG_TTI_INFO_SG_SEL_BMSK 0x20
#define WCDMA_EDL_CHAN_STTD_SG_TTI_INFO_SG_SEL_SHIFT 5

#define WCDMA_EDL_CHAN_STTD_SG_TTI_INFO_STTD_AGCH_BMSK 0x40
#define WCDMA_EDL_CHAN_STTD_SG_TTI_INFO_STTD_AGCH_SHIFT 6

#define WCDMA_EDL_CHAN_STTD_SG_TTI_INFO_TTI_BMSK 0x80
#define WCDMA_EDL_CHAN_STTD_SG_TTI_INFO_TTI_SHIFT 7

#define WCDMA_EDL_CHAN_RGCH_TPC_IDX_INFO_RGCH_ACTION_BMSK 0x70
#define WCDMA_EDL_CHAN_RGCH_TPC_IDX_INFO_RGCH_ACTION_SHIFT 4

#define WCDMA_EDL_CHAN_RGCH_TPC_IDX_INFO_RGCH_PRESENT_BMSK 0x80
#define WCDMA_EDL_CHAN_RGCH_TPC_IDX_INFO_RGCH_PRESENT_SHIFT 7

#define WCDMA_EDL_CHAN_RGCH_TPC_IDX_INFO_TPC_IDX_BMSK 0x07
#define WCDMA_EDL_CHAN_RGCH_TPC_IDX_INFO_TPC_IDX_SHIFT 0

#define WCDMA_EDL_CHAN_DPCH_HICH_PSC_INFO_HICH_ACTION_BMSK 0x07
#define WCDMA_EDL_CHAN_DPCH_HICH_PSC_INFO_HICH_ACTION_SHIFT 0

#define WCDMA_EDL_CHAN_DPCH_HICH_PSC_INFO_HICH_PRESENT_BMSK 0x08
#define WCDMA_EDL_CHAN_DPCH_HICH_PSC_INFO_HICH_PRESENT_SHIFT 3

#define WCDMA_EDL_CHAN_DPCH_HICH_PSC_INFO_DPCH_CHAN_BMSK 0x70
#define WCDMA_EDL_CHAN_DPCH_HICH_PSC_INFO_DPCH_CHAN_SHIFT 4

#define WCDMA_EDL_CHAN_DPCH_HICH_PSC_INFO_PSC_LSB_BMSK 0x80
#define WCDMA_EDL_CHAN_DPCH_HICH_PSC_INFO_PSC_LSB_SHIFT 7

/* The bit mask and shift to fill the byte consisting of the last half of start and first
** half of the END frame number is the same for all the channels. So the same 
** #define can be used.*/
#define WCDMA_EDL_CHAN_FRM_NUM_INFO_START_FN_BMSK 0x0F
#define WCDMA_EDL_CHAN_FRM_NUM_INFO_START_FN_SHIFT 0

#define WCDMA_EDL_CHAN_FRM_NUM_INFO_FINAL_FN_BMSK 0xF0
#define WCDMA_EDL_CHAN_FRM_NUM_INFO_FINAL_FN_SHIFT 4


#define WCDMA_EDL_CHAN_HICH_STTD_SIG_INFO_SIG_BMSK 0x3F
#define WCDMA_EDL_CHAN_HICH_STTD_SIG_INFO_SIG_SHIFT 0

#define WCDMA_EDL_CHAN_HICH_STTD_SIG_INFO_STTD_BMSK 0x40
#define WCDMA_EDL_CHAN_HICH_STTD_SIG_INFO_STTD_SHIFT 6

#define WCDMA_EDL_CHAN_RGCH_INFO_SIG_BMSK 0x3F
#define WCDMA_EDL_CHAN_RGCH_INFO_SIG_SHIFT 0



/* The macro below can be used to set all the multiple fields of all the entries in the
** EUL_DL_CHAN_INFO_LOG_PKT*/
#define WCDMA_SET_EDL_CHAN_LOG_INFO_FIELD(buf, field1, field2, value) \
  (buf) = (((buf) & (~WCDMA_EDL_CHAN_##field1##_##field2##_BMSK)) | \
           (((value) << WCDMA_EDL_CHAN_##field1##_##field2##_SHIFT) & \
           WCDMA_EDL_CHAN_##field1##_##field2##_BMSK))


/* Structure defined to store the information fro logging the AGCH data in the log packet.
The AGCH data needs to be logged only once per log packet. So this information is put 
in the header where the num_eul_cells in present.
*/

/*Log Packet WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET */
typedef PACKED struct PACKED_POST
{
  #if defined(FEATURE_WCDMA_DC_HSUPA) || defined(FEATURE_WCDMA_16_QAM)
  /*Number of EUL cells. This will define the final size of this log packet*/
  uint8 num_eul_cells;
#endif /*FEATURE_WCDMA_DC_HSUPA */
/* This byte gives information about which ERNTI is present
** primary/secondary, AGCH present and action. This byte also
** logs the serving cell index*/
uint8 ernti_present_agch_info;
/*AGCH OVSF code*/
uint8 ovsf;
/* LS Byte of the start Sub FN.*/
uint8 start_fn;
/*MS nibble of start sub FN and LS nibble for final sub FN*/
uint8 end_start_fn;
/* MS byte of the final sub FN*/
uint8 end_fn;
/*AGCH to DPCH offset. THis is normalized*/
uint8 ch2dpch_offset;
/*The value of Primary ERNTI*/
uint16 pri_ernti;
/*The value of secondary ERNTI*/
uint16 sec_ernti;
/*This byte logs the AGCH_STTD, the grant type
** for the initial grant and the INIT_SG value */
uint8 sttd_sg_tti_info;
}agch_common_hdr_info_struct_type;

/*Log Packet WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET */
typedef PACKED struct PACKED_POST{
/*OVSF code for HICH*/
uint8 ovsf;
/* LS Byte of the start Sub FN.*/
uint8 start_fn;
/*MS nibble of start sub FN and LS nibble for final sub FN*/
uint8 end_start_fn;
/* MS byte of the final sub FN*/
uint8 end_fn;
/*HICH to DPCH offset. This is normalized*/
uint8 ch2dpch_offset;
/*Also logged is the offset of HICH wrt PCCPCH*/
uint8 tau_hich;
/*This byte has the HICH_STTD and HICH signature index*/
uint8 sttd_hich_sig_idx;
} hich_log_info_struct_type;


/*Log Packet WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET */
typedef PACKED struct PACKED_POST{
/* LS Byte of the start Sub FN.*/
uint8 start_fn;
/*MS nibble of start sub FN and LS nibble for final sub FN*/
uint8 end_start_fn;
/* MS byte of the final sub FN*/
uint8 end_fn;
/*RGCH to DPCH offset. This is normalized*/
uint8 ch2dpch_offset;
/*The signature index used for RGCH*/
uint8 sig_idx;
} rgch_log_info_struct_type;


/*Log Packet WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET */
typedef PACKED struct PACKED_POST
{
/*The Byte rl_info RLS_index, Cell index*/
uint8 rl_info;
/*Indicates whether RGCH is present and channel action. Also gives TPC_IDX*/
uint8 rgch_info_tpc_idx;
/* DPCH hardware channel and HICH present info. Also has the LSbit of PSC*/
uint8 dpch_hich_psc_info;
 /*Gives the PSC bit[1:8]*/
uint8 psc;
 /*Reference finger for this RL*/
uint8 ref_finger;
}per_rl_common_hdr_info_struct_type;


/*Log Packet WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET */
typedef PACKED struct PACKED_POST{
/* The first bye has some general info about RL*/
per_rl_common_hdr_info_struct_type per_rl_common_hdr_info;

  /* This structure describes the parameters about the HICH present on a specific RL */
hich_log_info_struct_type hich_info;

  /* This structure describes the parameters about the RGCH present on a specific RL */
rgch_log_info_struct_type rgch_info;
} dl_chan_info_struct_type;

/*Log Packet WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET */
LOG_RECORD_DEFINE(WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET)
  #if defined(FEATURE_WCDMA_DC_HSUPA) || defined(FEATURE_WCDMA_16_QAM)
  uint8                            version;
  uint8                            carrier_state;
#else
/*Number of EUL cells. This will define the final size of this log packet*/
uint8 num_eul_cells;
  #endif /* FEATURE_WCDMA_DC_HSUPA || FEATURE_WCDMA_16_QAM */

  agch_common_hdr_info_struct_type agch_common_hdr_info[WL1_MAX_UL_CARR_IDX];

/* The array of structure below is dependent on the number of EUL cells
** It is of variable size.*/
  dl_chan_info_struct_type         per_cell_info[WL1_MAX_UL_CARR_IDX][EUL_MAX_RL];
LOG_RECORD_END

/*Log Packet WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET */
/* The below macro will give the length of the logpacket 4301. The input parameter is the number of EUL cells*/
#define WCDMA_LOG_EDL_CHAN_INFO_LOG_PKT_LEN(num_cells, carr_idx)   ( (dword)((( WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET_type *) 0)->agch_common_hdr_info) )\
                                                                         + ((carr_idx + 1) * sizeof(agch_common_hdr_info_struct_type)) \
                                                                         + ((carr_idx + 1) * num_cells * sizeof(dl_chan_info_struct_type))

/*==================WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET =================*/
#define E_DL_CHAN_RES_LOGGING_NUM_SAMPLES_2MS 40
#define E_DL_CHAN_RES_LOGGING_NUM_SAMPLES_10MS 20

#define EDL_DECODE_LOGGING_NUM_SAMPLES         40
#define E_DL_CHAN_RES_MAX_SAMPLES E_DL_CHAN_RES_LOGGING_NUM_SAMPLES_2MS

#define WCDMA_EDL_CHAN_CONFIG_INFO_TTI_VAL_BMSK 0x01
#define WCDMA_EDL_CHAN_CONFIG_INFO_TTI_VAL_SHIFT 0

#define WCDMA_EDL_CHAN_CONFIG_INFO_NUM_CELLS_BMSK 0x0E
#define WCDMA_EDL_CHAN_CONFIG_INFO_NUM_CELLS_SHIFT 1

#define WCDMA_EDL_CHAN_CONFIG_INFO_SERV_CELL_BMSK 0x30
#define WCDMA_EDL_CHAN_CONFIG_INFO_SERV_CELL_SHIFT 4 

#define WCDMA_EDL_CHAN_CONFIG_INFO_PER_CELL_PSC_BIT8_BMSK 0x01
#define WCDMA_EDL_CHAN_CONFIG_INFO_PER_CELL_PSC_BIT8_SHIFT 0

#define WCDMA_EDL_CHAN_CONFIG_INFO_PER_CELL_COMB_IDX_BMSK 0x0E
#define WCDMA_EDL_CHAN_CONFIG_INFO_PER_CELL_COMB_IDX_SHIFT 1

#define WCDMA_EDL_CHAN_NSLOTS_ENG_NSLOTS_BMSK 0x0F
#define WCDMA_EDL_CHAN_NSLOTS_ENG_NSLOTS_SHIFT 0

#define WCDMA_EDL_CHAN_NSLOTS_ENG_BIT0TO3ENG_BMSK 0xF0
#define WCDMA_EDL_CHAN_NSLOTS_ENG_BIT0TO3ENG_SHIFT 4

#define WCDMA_EDL_CHAN_THRESH_ENG_BIT12TO14_ENG_BMSK 0x07
#define WCDMA_EDL_CHAN_THRESH_ENG_BIT12TO14_ENG_SHIFT 0

#define WCDMA_EDL_CHAN_THRESH_ENG_BIT0TO4_THRESH_BMSK 0xF8
#define WCDMA_EDL_CHAN_THRESH_ENG_BIT0TO4_THRESH_SHIFT 3

/* The macro below can be used to set all the multiple fields of all the entries in the
** EUL_DL_CHAN_INFO_LOG_PKT */
#define WCDMA_SET_EDL_CHAN_LOG_INFO_FIELD(buf, field1, field2, value) \
  (buf) = (((buf) & (~WCDMA_EDL_CHAN_##field1##_##field2##_BMSK)) | \
           (((value) << WCDMA_EDL_CHAN_##field1##_##field2##_SHIFT) & \
           WCDMA_EDL_CHAN_##field1##_##field2##_BMSK))

typedef PACKED struct PACKED_POST{
uint8 psc;
uint8 index_psc_bit8;
}e_dllog_index_psc_struct_type;


typedef PACKED struct PACKED_POST{
uint8 num_cells_tti;
e_dllog_index_psc_struct_type e_dllog_index_psc_struct[EUL_MAX_RL];
}e_dllog_chan_config_info_struct_type;

typedef PACKED struct PACKED_POST{
  uint8 num_slots_eng;
  uint8 energy;
  uint8 thresh_eng;
  uint8 thresh;
}e_dllog_chan_ps_info_struct_type;

typedef PACKED struct PACKED_POST{
  uint8 num_samples;
  uint16 sub_fn_first_result;
  e_dllog_chan_ps_info_struct_type e_dllog_chan_ps_info[E_DL_CHAN_RES_MAX_SAMPLES];
}e_dllog_per_rl_chan_results_info_struct_type;

LOG_RECORD_DEFINE(WCDMA_EUL_DL_HICH_RESULTS_INFO_LOG_PACKET)
  #if defined(FEATURE_WCDMA_DC_HSUPA) || defined(FEATURE_WCDMA_16_QAM) 
  uint8                                        version;
  uint8                                        carrier_state;
  #endif /*FEATURE_WCDMA_DC_HSUPA || FEATURE_WCDMA_16_QAM*/

  e_dllog_chan_config_info_struct_type         hich_config_info[WL1_MAX_UL_CARR_IDX];
  e_dllog_per_rl_chan_results_info_struct_type hich_info[WL1_MAX_UL_CARR_IDX][EUL_MAX_RL];
LOG_RECORD_END

LOG_RECORD_DEFINE(WCDMA_EUL_DL_RGCH_RESULTS_INFO_LOG_PACKET)
  #if defined(FEATURE_WCDMA_DC_HSUPA) || defined(FEATURE_WCDMA_16_QAM)
  uint8                                        version;
  uint8                                        carrier_state;
  #endif /*FEATURE_WCDMA_DC_HSUPA || FEATURE_WCDMA_16_QAM*/

  e_dllog_chan_config_info_struct_type         rgch_config_info[WL1_MAX_UL_CARR_IDX];
  e_dllog_per_rl_chan_results_info_struct_type rgch_info[WL1_MAX_UL_CARR_IDX][EUL_MAX_RL];
LOG_RECORD_END

typedef PACKED struct PACKED_POST {
uint8 agch_pre_dec_err_valid;
uint16 agch_dec_energy;
uint8 agch_val_scope_ernti;
uint16 vd_output;
uint16 cm_slot_bmsk;
uint16 rssi_sum;
}e_dl_log_agch_ps_info_struct_type;

typedef PACKED struct PACKED_POST{
uint8 agch_num_samp;
uint16 sub_fn_first_result;
e_dl_log_agch_ps_info_struct_type agch_per_sample_info[E_DL_CHAN_RES_MAX_SAMPLES];
}e_dl_log_agch_info_struct_type;

#define WCDMA_EDL_AGCH_VALID_AG_VALID_BMSK 0xC0
#define WCDMA_EDL_AGCH_VALID_AG_VALID_SHIFT 6

#define WCDMA_EDL_AGCH_VALID_SYM_ERR_BMSK 0x3F
#define WCDMA_EDL_AGCH_VALID_SYM_ERR_SHIFT 0

#define WCDMA_EDL_AGCH_VALUE_AG_VAL_BMSK 0x3F
#define WCDMA_EDL_AGCH_VALUE_AG_VAL_SHIFT 0

#define WCDMA_EDL_AGCH_VALUE_AG_SCOPE_BMSK 0x40
#define WCDMA_EDL_AGCH_VALUE_AG_SCOPE_SHIFT 6

#define WCDMA_EDL_AGCH_VALUE_P_OR_S_BMSK 0x80
#define WCDMA_EDL_AGCH_VALUE_P_OR_S_SHIFT 7

#define WCDMA_SET_EDL_AGCH_LOG_INFO_FIELD(buf, field1, field2, value) \
  (buf) = (((buf) & (~WCDMA_EDL_##field1##_##field2##_BMSK)) | \
           (((value) << WCDMA_EDL_##field1##_##field2##_SHIFT) & \
           WCDMA_EDL_##field1##_##field2##_BMSK))

LOG_RECORD_DEFINE(WCDMA_EUL_DL_AGCH_RESULTS_INFO_LOG_PACKET)
  #if defined(FEATURE_WCDMA_DC_HSUPA) || defined(FEATURE_WCDMA_16_QAM)
  uint8                          version;
  uint8                          carrier_state;
  #endif /* FEATURE_WCDMA_DC_HSUPA || FEATURE_WCDMA_16_QAM */

  e_dl_log_agch_info_struct_type agch_info[WL1_MAX_UL_CARR_IDX];
LOG_RECORD_END

#define E_DL_LOG_PKT_SAMPLE_BUF_MAX  2


/* ================ EUL_DL_DECODE_LOG_PACKET ===============*/


/*#define for the common header fields*/
#define WCDMA_EDL_DEC_COMMON_HDR_TTI_TTI_VAL_BMSK 0x01
#define WCDMA_EDL_DEC_COMMON_HDR_TTI_TTI_VAL_SHIFT 0

#define WCDMA_EDL_DEC_COMMON_HDR_TTI_NUM_CELLS_BMSK 0x0E
#define WCDMA_EDL_DEC_COMMON_HDR_TTI_NUM_CELLS_SHIFT 1

#define WCDMA_EDL_DEC_COMMON_HDR_TTI_SERV_CELL_BMSK 0x30
#define WCDMA_EDL_DEC_COMMON_HDR_TTI_SERV_CELL_SHIFT 4

#define WCDMA_EDL_DEC_COMMON_HDR_TTI_MRC_INFO_BMSK 0x80
#define WCDMA_EDL_DEC_COMMON_HDR_TTI_MRC_INFO_SHIFT 7

#define WCDMA_EDL_DEC_COMMON_HDR_PER_CELL_PSC_BIT8_BMSK 0x01
#define WCDMA_EDL_DEC_COMMON_HDR_PER_CELL_PSC_BIT8_SHIFT 0

#define WCDMA_EDL_DEC_COMMON_HDR_PER_CELL_RG_IDX_BMSK 0x0E
#define WCDMA_EDL_DEC_COMMON_HDR_PER_CELL_RG_IDX_SHIFT 1

#define WCDMA_EDL_DEC_COMMON_HDR_PER_CELL_TPC_IDX_BMSK 0x70
#define WCDMA_EDL_DEC_COMMON_HDR_PER_CELL_TPC_IDX_SHIFT 4

#define WCDMA_EDL_DEC_COMMON_HDR_PER_CELL_SOFT_C_BMSK 0x80
#define WCDMA_EDL_DEC_COMMON_HDR_PER_CELL_SOFT_C_SHIFT 7

#define WCDMA_EDL_DEC_COMMON_HDR_AGCH_DELTA_CURR_SUB_FN_BMSK 0x0F
#define WCDMA_EDL_DEC_COMMON_HDR_AGCH_DELTA_CURR_SUB_FN_SHIFT 0

#define WCDMA_EDL_DEC_COMMON_HDR_AGCH_DELTA_AGCH_DELTA_BMSK 0x70
#define WCDMA_EDL_DEC_COMMON_HDR_AGCH_DELTA_AGCH_DELTA_SHIFT 4

#define WCDMA_EDL_DEC_COMMON_HDR_HICH_DELTA_CELL_1_3_BMSK 0x07
#define WCDMA_EDL_DEC_COMMON_HDR_HICH_DELTA_CELL_1_3_SHIFT 0

#define WCDMA_EDL_DEC_COMMON_HDR_HICH_DELTA_CELL_2_4_BMSK 0x38
#define WCDMA_EDL_DEC_COMMON_HDR_HICH_DELTA_CELL_2_4_SHIFT 3

#define WCDMA_EDL_DEC_COMMON_HDR_RGCH_DELTA_CELL_1_3_BMSK 0x0F
#define WCDMA_EDL_DEC_COMMON_HDR_RGCH_DELTA_CELL_1_3_SHIFT 0

#define WCDMA_EDL_DEC_COMMON_HDR_RGCH_DELTA_CELL_2_4_BMSK 0xF0
#define WCDMA_EDL_DEC_COMMON_HDR_RGCH_DELTA_CELL_2_4_SHIFT 4


#define WCDMA_EDL_DEC_AGCH_VALID_NEW_AGCH_BMSK 0x80
#define WCDMA_EDL_DEC_AGCH_VALID_NEW_AGCH_SHIFT 7

#define WCDMA_EDL_DEC_AGCH_VALID_AG_VALID_BMSK 0x40
#define WCDMA_EDL_DEC_AGCH_VALID_AG_VALID_SHIFT 6

#define WCDMA_EDL_DEC_AGCH_VALID_SYM_ERR_BMSK 0x3F
#define WCDMA_EDL_DEC_AGCH_VALID_SYM_ERR_SHIFT 0

#define WCDMA_EDL_DEC_AGCH_VALUE_AG_VAL_BMSK 0x3F
#define WCDMA_EDL_DEC_AGCH_VALUE_AG_VAL_SHIFT 0

#define WCDMA_EDL_DEC_AGCH_VALUE_AG_SCOPE_BMSK 0x40
#define WCDMA_EDL_DEC_AGCH_VALUE_AG_SCOPE_SHIFT 6

#define WCDMA_EDL_DEC_AGCH_VALUE_P_OR_S_BMSK 0x80
#define WCDMA_EDL_DEC_AGCH_VALUE_P_OR_S_SHIFT 7

/* The below #defines for bit mask and shift are the same 
** for the HICH and RGCH channels.*/
#define WCDMA_EDL_DEC_NSLOTS_ENG_NEW_CHAN_BMSK 0x80
#define WCDMA_EDL_DEC_NSLOTS_ENG_NEW_CHAN_SHIFT 7

#define WCDMA_EDL_DEC_NSLOTS_ENG_NSLOTS_BMSK 0x78
#define WCDMA_EDL_DEC_NSLOTS_ENG_NSLOTS_SHIFT 3

#define WCDMA_EDL_DEC_NSLOTS_ENG_BIT0TO3_ENG_BMSK 0x07
#define WCDMA_EDL_DEC_NSLOTS_ENG_BIT0TO3_ENG_SHIFT 0

/* The macro below can be used to set all the multiple fields of all the entries in the
** EUL_DL_DEC_INFO_LOG_PKT */
#define WCDMA_SET_EDL_DEC_LOG_INFO_FIELD(buf, field1, field2, value) \
  (buf) = (((buf) & (~WCDMA_EDL_DEC_##field1##_##field2##_BMSK)) | \
           (((value) << WCDMA_EDL_DEC_##field1##_##field2##_SHIFT) & \
           WCDMA_EDL_DEC_##field1##_##field2##_BMSK))

typedef PACKED struct PACKED_POST{
uint8 psc;
uint8 tpc_rg_idx;
uint8 tau_dpch;
}per_cell_config_info_struct_type;

typedef PACKED struct PACKED_POST{

uint8 num_cells_serv_cell_tti;
per_cell_config_info_struct_type per_cell_config_info[EUL_MAX_RL];
uint8 curr_sub_fn;
uint8 agch_delta_curr_sub_fn;
uint8 hich_delta_cell_1_2;
uint8 hich_delta_cell_3_4;
uint8 rgch_delta_cell_1_2;
uint8 rgch_delta_cell_3_4;
}e_dl_dec_common_hdr_info_struct_type;

typedef PACKED struct PACKED_POST
{
uint8 agch_pre_dec_err_valid;
uint16 agch_dec_energy;
uint8 agch_val_scope_ernti;
}e_dl_log_agch_per_sample_info_struct_type;

typedef PACKED struct PACKED_POST{
uint8 agch_num_samp;
e_dl_log_agch_per_sample_info_struct_type agch_per_sample_info[EDL_DECODE_LOGGING_NUM_SAMPLES];
}e_dl_dec_agch_info_struct_type;

typedef PACKED struct PACKED_POST{
uint8  hich_num_slots_eng;
uint16 hich_energy;
uint16 hich_thresh;
uint32 sc_weight;
}e_dl_dec_hich_per_sample_info_struct_type;


typedef PACKED struct PACKED_POST{
uint8 hich_num_samples;
e_dl_dec_hich_per_sample_info_struct_type hich_per_sample_info[EDL_DECODE_LOGGING_NUM_SAMPLES];
}e_dl_dec_hich_info_struct_type;


typedef PACKED struct PACKED_POST{
uint8  rgch_num_slots_eng;
uint16 rgch_energy;
uint16 rgch_thresh;
}e_dl_dec_rgch_per_sample_info_struct_type;

typedef PACKED struct PACKED_POST {
uint8 rgch_num_samples;
e_dl_dec_rgch_per_sample_info_struct_type rgch_per_sample_info[EDL_DECODE_LOGGING_NUM_SAMPLES];
}e_dl_dec_rgch_info_struct_type;



LOG_RECORD_DEFINE(WCDMA_EUL_DL_DECODE_INFO_LOG_PACKET)
e_dl_dec_common_hdr_info_struct_type common_hdr_info;
e_dl_dec_agch_info_struct_type agch_info;
e_dl_dec_hich_info_struct_type hich_info[EUL_MAX_RL];
e_dl_dec_rgch_info_struct_type rgch_info[EUL_MAX_RL];

LOG_RECORD_END


#define WCDMA_LOG_EDL_DEC_INFO_LOG_PKT_LEN  sizeof(WCDMA_EUL_DL_DECODE_INFO_LOG_PACKET_type)

#endif /* EDLLOG_H */
