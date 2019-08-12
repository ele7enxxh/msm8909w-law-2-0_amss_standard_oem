/*!
  @file
  lte_ml1_pos_prs_util.h

  @brief
  This file provides the interface for utility functions
 
*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.



$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/ML1/pos/src/lte_ml1_pos_prs_log.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/11   yp      Initial version
===========================================================================*/

#include "intf_prs_fd_logging_types.h"

/*===========================================================================

                     INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define LOG_LTE_ML1_PRS_FD_BUFFER_C 0x18DA
#define LOG_LTE_ML1_PRS_PROG_PARAMS_C 0x18DB

/*! LTE PRS cell update cell log information structure */
typedef struct 
{
  uint32 earfcn;           /*!< E-ARFCN */
  uint16 phy_cell_id;      /*!< Physical cell id */
  uint8  ant_port_config;  /*!< Antenna port config */
  uint8  bandwidth;        /*!< PRS bandwidth */
  uint8  num_dl_subframes; /*!< Number of downlink subframes */
  uint8  num_hypotheses;   /*!< Number of hypotheses */
  uint8  muting_valid;     /*!< Whether the muting fields are valid */
  uint8  muting_repeat;    /*!< Number of occasions before repeating */
  uint16 muting_pattern;   /*!< Muting pattern */
  uint16 config_index;     /*!< Configuration index */

  struct
  {
    int8   symbol_offset_index;   /*!< Symbol offset index */
    uint8  reserved1;             /*!< Reserved */
    uint16 prune_win_size;        /*!< Prune window size */
    uint16 prune_win_start_index; /*!< Prune window start index */
    uint16 reserved2;             /*!< Reserved */

  } hypothesis[ LTE_CPHY_PRS_MAX_CELL_HYP ]; /*!< Hypotheses */

} lte_ml1_pos_prs_cell_update_cell_log_t;

/*! PRS cell update log information */
typedef struct 
{
  uint8  version;     /*!< Log packet version */
  uint8  num_nghbrs;  /*!< Number of cells */
  uint16 phy_cell_id; /*!< Serving cell id */

  /*! Reference cell */
  lte_ml1_pos_prs_cell_update_cell_log_t ref;

  /*! Neighbor cells */
  lte_ml1_pos_prs_cell_update_cell_log_t nghbrs[ LTE_CPHY_PRS_MAX_NGHBR_CELLS ];

} lte_ml1_pos_prs_cell_update_log_t;

/*! PRS cell hypothesis log information */
typedef struct
{
  uint32 earfcn;      /*!< E-ARFCN */
  uint16 phy_cell_id; /*!< Physical cell id */
  uint8  cell_idx;    /*!< Cell index */
  uint8  hyp_idx;     /*!< Hypothesis index */
  uint8  muted;       /*!< Whether the hypothesis is muted */
  uint8  measured;    /*!< Whether the hypothesis has been measured */
  uint8  meas_req;    /*!< Whether the measurement has been requested */
  uint8  reserved1;

} lte_ml1_pos_prs_cell_hypothesis_log_t;

/*! PRS cell hypotheses log information */
typedef struct 
{
  uint8  version;           /*!< Log packet version */
  uint8  num_hypotheses;    /*!< Number of hypotheses */
  uint16 reserved1;
  uint32 serv_earfcn;       /*!< Serving cell E-ARFCN */
  uint16 serv_cell_id;      /*!< Serving cell id */
  uint16 ref_cell_id;       /*!< Reference cell id */
  uint16 curr_sf;           /*!< Current serving SF number */
  uint16 ref_cell_sf_delta; /*!< Reference cell SF delta from serving cell */
  uint16 next_occ_sf;       /*!< Next PRS occasions SF number */
  uint16 reserved2;
  uint32 duration;          /*!< Duration of PRS occasions */
  uint32 curr_rtc_lo;       /*!< Current RTC value LO */
  uint32 curr_rtc_hi;       /*!< Current RTC value HI */
  uint32 start_rtc_lo;      /*!< Start RTC value LO */
  uint32 start_rtc_hi;      /*!< Start RTC value HI */
  uint32 end_rtc_lo;        /*!< End RTC value LO */
  uint32 end_rtc_hi;        /*!< End RTC value HI */

  /*! Cell hypotheses */
  lte_ml1_pos_prs_cell_hypothesis_log_t hypotheses[ LTE_ML1_POS_PRS_MAX_HYP ];

} lte_ml1_pos_prs_cell_hypotheses_log_t;
    
/* FD Buffer log packet identification */
typedef enum
{
  /* Common parameters, refer to lloc_PrsFdBufCommonParamsType */
  LTE_ML1_FD_BUF_PKT_ID_COMMON = 0,

  /* Actual FD Tones, refer to lloc_PrsFdBufDataType */
  LTE_ML1_FD_BUF_PKT_ID_DATA
}lte_ml1_pos_prs_fdbuf_pkt_id_e;

/* FD buffer format enumeration */
typedef enum
{
  /* SC16 – Unpacked, S16 real part, S16 imaginary part */
  LTE_ML1_FD_BUF_FMT_SC16 = 0,

  /* PCFL20_4 – Packed, 4-bits exponent, 8-bits Real, 8-bits Imaginary */
  LTE_ML1_FD_BUF_FMT_PCFL20_4,
  
  /* Unknown format */
  LLOC_FD_BUF_FMT_UNKNOWN
}lte_ml1_pos_prs_fdbuf_fmt_e;


/* PRS FD Buffer Parameters */
typedef struct
{
  
  /* LTE system BW. O = 1.4 Mhz, 1 = 3Mhz, 2 = 5Mhz, 3 = 10Mhz, 4 = 15Mhz, 5 = 20Mhz */
  uint8 u_LteSysBw; /* ML1 & FW */

  /* FD buffer bandwidth BW. O = 1.4 Mhz, 1 = 3Mhz, 2 = 5Mhz, 3 = 10Mhz, 4 = 15Mhz, 5 = 20Mhz */
  uint8 u_FdBufferBw; /* FW */

  /* The number of RX antennas */
  uint8 u_NumRxAnt; /* FW – hardcoded */

  /* Carrier associated with this FD Buffer. It could be 0=PCC, 1=SCC1 or 2=SCC2. Currently multi-carrier search is not supported. Hence this must be populated. When multi-carrier search is supported, please look at the cell specific parameters, instead of this */
  uint8 u_Carrier; /* ML1 & FW */ 

  /* Indicates the first symbol index within relative to the beginning of the subframe that gets copied to PRS core buffer. For PRS searching it is usually 3 */
  uint8 u_SymbolOffset; /* FW – This is for overall symbol offset, not per cell */

  /* Number of symbols copied to PRS core buffer, this is driven by search parameter’s overall symbol offset. Range 11-14 for NCP, 8-11 for ECP */
  uint8 u_NumSymbols; /* FW */

  /* Number of words per symbol. For UNPACKED format this should equal [number of RBs * 12], e.g. 20Mhz=>1200 words. For PACKED, it should be less than that, e.g. 20Mhz=>751 words. */
  uint16 w_WordsPerSymbol; /* FW */
 
  /* SFN of associated with this occasion: contains both system frame number and subframe number */
  uint16 w_SystemFrameNumber; /* ML1 & FW */

  /* The number of subframes accumulated for search, i.e. NPRS */
  uint8 u_MaxDlSubframes; /* FW */

  /* The format of the FD buffer itself, i.e. SC16, PCFL20_4, etc. This uses enum loc_lte_otdoa_FdBufFmtType for content */
  uint8 u_FdBufFmt; /* FW */

  /* Subframe mask. Mostly used to assist knowing the contiguity of DL subframes in TDD system */
  uint32 q_SubframeMask; /* ??? */

  /* Indicate whether serving LTE system is TDD or FDD */
  uint8 u_IsTdd; /* ??? */

  /* Cyclic prefix mode of the serving cell */
  uint8 u_CpLength; /* Is this info available? Note that this is coming from the system, *NOT* aiding data */

  /* dc step size */
  uint16 dc_step;

  /* Reserved – Adjust byte padding size accordingly when modifying this data structure */
  //uint8 z_Reserved[2];

}lte_ml1_pos_prs_buf_common_params_t;

/* Data structure to ship the actual FD tones – variable size */
typedef struct
{
  /* Number of FD tones words included in this batch. This will help parser keep track how many words have been accumulated and how many words are still expected */
  uint32 q_NumWords;

  /* Sequence number. FD Tones data may be split into multiple small batches for throttling to avoid log drop. This will help parser order the batches */
  uint16 w_SeqNo;

  /* If u_SubframeNo < 6, it indicates which subframe this FD tones is associated with, i.e. 0 .. 5, to allow logging FD tones one subframe at a time. If SW is logging all subframes (for NPRS>1) within one packed chunk, then u_SubframeNo must be set to >= 6, i.e. do-not-care */
  uint8 u_SubframeNo;

  /* Reserved – Adjust byte padding size accordingly when modifying this data structure */
  uint8 z_Reserved[1];

  /* FD Tones. This field must be the last in this data structure as it can grow depending upon w_NumWords. FdTones are ordered consecutively for Rx0, then Rx1. TOTAL number of words expected for an occasion should equal:
   z_CommonParams.u_NumSymbols * w_WordsPerSymbols * u_NumRxAnt */
  uint32 q_FdTone[1];
}lte_ml1_pos_prs_buf_data_t;

/* PRS FD Buffer Log */
typedef struct
{
  /* Version number for the log */
  uint8 u_Version;

  /* Session number */
  uint32 session_number;

  /* This is populated using enum lloc_FdBufPacketIdType, identify the data structure for the field, z_PktPayload */
  uint8 u_PktId;

  /* Reserved – Adjust byte padding size accordingly when modifying this data structure */
  uint8 z_Reserved[2];

  /* Occasion sequence number. This is the global sequence number that can be used to associated with other logs */
  uint32 q_OccasionSeqNum;  /* ML1 */

  /* Total number of words of FD Tones data – this is to assist parser to allocate memory in case of out-of-order common parameters and FD Tones data */
  uint32 q_TotalWords;

  /* Payload size in bytes – just for sanity check. Must be at least 4. */
  uint32 q_PayloadSz;

  /* Payload of this log, could be different packet type. This field MUST be the last one in this data structure as size can be dynamically growing. This blob has pre-defined data structure depending on the e_PktId */
  uint32 z_PktPayload[1];

} lte_ml1_pos_prs_buf_log_t;

/* Cell-specific Parameters */
typedef struct
{
  
  /* Antenna port config, i.e. 0=1 or 2 ports, 1=4-ports. Uses lloc_PrsAntPortConfigType for enumeration */
  uint8 u_AntennaPortConfig; /* ML1 & FW */

  /* Cyclic Prefix, Uses loc_lte_otdoa_CpModeType for enumeration */
  uint8 u_CpLength; /* ML1 & FW – Should be per cell */
  
  /* The number of subframes being summed for this cell i.e. NPRS */
  uint8 u_NumDlSubframes; /* FW */

  /* PRS bandwidth. O = 1.4 Mhz, 1 = 3Mhz, 2 = 5Mhz, 3 = 10Mhz, 4 = 15Mhz, 5 = 20Mhz */
  uint8 u_PrsBw;

  /* physical cell ID  Range ~ 0 to 503 */
  uint16 w_PhyCellId;

  /* PRS Gain Bias */
  int8 b_PrsGainBias;

  /* Symbol offset index */
  int8 b_SymbolOffsetIndex;

  /* Prune window size */
  uint16 w_PruneWinSize;

  /* Prune window start index */
  uint16 w_PruneWinStartIndex;

  /* Processing mode, either PRS=0 or CRS=1 */
  uint8 u_ProcMode;

  /* Carrier associated with this cell. It could be 0=PCC, 1=SCC1 or 2=SCC2. When multi-carrier search is supported, this may be different for different cells */
  uint8 u_Carrier; /* ML1 & FW */ 

  /* For NPRS>1, whether coherent or non-coherent accumulation performed */
  uint8 u_CohNcohAccMode; /* ML1 & FW */ 

  /* Ncoh_scale */
  uint8 u_NcohScale; /* FW */ 

  /* Coh_scale */
  uint32 u_CohScale; /* FW */ 

  /* RX0 scale */
  uint16 w_Rx0Scale; /* FW */ 

  /* RX1 scale */
  uint16 w_Rx1Scale; /* FW */ 

  /* COM based win adj value used to adjust prune window start index*/
  int16 win_adj;

}lte_ml1_pos_prs_cell_params_t;

/* Dynamic PRS configuration parameters – variable size */
typedef struct
{

  /* Used in rotation step */
  uint32 q_Freq;

  /* Used in rotation step */  
  uint32 q_Phase;

  /* Used in the scaling step at the end of symbol processing */                               
  uint32 q_SmoothScale;

  /* Descrambling seed */                               
  uint32 q_DescSeed;

  /* Vshift – frequency shift for PRS symbols */                               
  uint32 q_VShift;
            
}lte_ml1_pos_prs_dyn_cfg_params_t;

/* PRS Programming Params Log */
typedef struct
{
  /* Version number for the log – THIS MUST BE THE FIRST FIELD IN THIS STRUCTURE AFTER HEADER */
  uint8 u_Version;

  /* Session number */
  uint32 session_number;

  /* Number of subframes information included. This is the first index into the 3-D array at the bottom */
  uint8 u_NumSubframes;

  /* Number of cells programmed – indicates which elements in z_CellParams are valid – and also for dynamic blob at the end of this structure, 2nd index into 3-D array. */
  uint8 u_NumCells;

  /* Number of symbols processed. Usually 8 for PRS, 11 for CRS. This is the 3rd index into the 3-D array. */
  uint8 u_NumSymbols;

  /* Occasion sequence number. This is the global sequence number that can be used to associated with other logs */
  uint32 q_OccasionSeqNum;

  /* If u_SubframeNo < 6, it indicates which subframe this FD tones is associated with, i.e. 0 .. 5, to allow logging FD tones one subframe at a time. If SW is logging all subframes (for NPRS>1) within one packed chunk, then u_SubframeNo must be set to >= 6, i.e. do-not-care */
  uint8 u_SubframeNo;

  /* Cell specific parameters – common to all subframes */
  lte_ml1_pos_prs_cell_params_t z_CellParams[10];

  /* Subframe/cell/symbol specific parameters. This field must be the last one in this data structure as it can grow as many as u_NumSubframes * u_NumCells * u_NumSymbols. This MUST ALWAYS BE AT THE END OF THIS STRUCTURE */
  lte_ml1_pos_prs_dyn_cfg_params_t z_DynCfg[1][1];

}lte_ml1_pos_prs_prog_params_t;



/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  prs_log_cell_update

===========================================================================*/
/*!
    @brief
    This function logs the cell update information.
 
    @return
    None

*/
/*=========================================================================*/
void prs_log_cell_hypotheses
( 
  lte_ml1_pos_prs_s *prs_inst, 
  uint8 num_cell_flags, 
  lte_ml1_pos_prs_cell_flags_s *cell_flag_array, 
  lte_ml1_schdlr_sf_time_t next_occ_sf 
);

/*===========================================================================

  FUNCTION:  prs_log_cell_hypotheses

===========================================================================*/
/*!
    @brief
    This function logs the cell hypotheses information.
 
    @return
    None

*/
/*=========================================================================*/
void prs_log_cell_update( lte_ml1_pos_prs_s *prs_inst );

/*===========================================================================

  FUNCTION:  prs_fd_buffer_logging_enabled

===========================================================================*/
/*!
    @brief
    This function returns whether fd buffer logging is enabled
 
    @return
    fd buffer logging is enabled

*/
/*=========================================================================*/
boolean prs_fd_buffer_logging_enabled( void );