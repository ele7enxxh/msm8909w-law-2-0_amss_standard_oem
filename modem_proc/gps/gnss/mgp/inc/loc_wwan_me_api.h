/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Location Wireless WAN Measurement Engine API Header

GENERAL DESCRIPTION
  This is the measurement engine header that is visible to external modules
  to use the Location Wireless WAN Measurement (loc_wwan_me) apis

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/mgp/inc/loc_wwan_me_api.h#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef LOC_WWAN_ME_API_H
#define LOC_WWAN_ME_API_H
/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "loc_wwan_meas_lte_api.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/
#define LWM_WWAN_TO_ME_IPC_VERSION_ID         2
#define LWM_ME_TO_PE_RPT_VERSION_ID           1

/* maximum entires in report to PE from ME */
// TODO adjust for actual target obeservations.
#define LOC_WWAN_ME_MAX_CELL_ENTRIES          24 

/* maximum entires in IPC to ME from WWAN */
#define LOC_WWAN_ME_MAX_MEAS_PER_RPT          10

#define LOC_WWAN_ME_GERAN_MAX_PLMN_ID_SIZE    3

#ifdef _ME_PLAYBACK_OFFLINE
  //from lte_rrc_ext_msg.h
  #define LTE_RRC_MAX_MCC_DIGITS        3 /*!< Maximum number of digits in MCC */
  #define LTE_RRC_MAX_MNC_DIGITS        3 /*!< Maximum number of digits in MNC */
#endif //_ME_PLAYBACK_OFFLINE

/*--------------------------------------------------------------------------
 * Type definitions
 *-----------------------------------------------------------------------*/
#ifdef _ME_PLAYBACK_OFFLINE
//from lte_rrc_ext_msg.h
/*! @brief Data structure for PLMN type
*/
typedef struct
{
  uint32 mcc[LTE_RRC_MAX_MCC_DIGITS]; /*!< Mobile country code */
  uint32 num_mnc_digits; /*!< Number of digits in MNC */
  uint32 mnc[LTE_RRC_MAX_MNC_DIGITS]; /*!< Mobile network code */
} lte_rrc_plmn_s; //from lte_rrc_ext_msg.h

/*! @brief
  LTE support bandwidths
  Defined in 36.101 Section 5.4.2.1
*/
typedef enum
{
  /*! 1.4MHz bandwidth */
  LTE_BW_NRB_6 = 6,
  /*! 3MHz bandwidth */
  LTE_BW_NRB_15 = 15,
  /*! 5MHz bandwidth */
  LTE_BW_NRB_25 = 25,
  /*! 10MHz bandwidth */
  LTE_BW_NRB_50 = 50,
  /*! 15MHz bandwidth */
  LTE_BW_NRB_75 = 75,
  /*! 20MHz bandwidth */
  LTE_BW_NRB_100 = 100,
  /*! Minimum supported bandwidth */
  LTE_BW_MIN = LTE_BW_NRB_6,
  /*! Maximum supported bandwidth */
  LTE_BW_MAX = LTE_BW_NRB_100,
  /*! Unknown Bandwidth */
  LTE_BW_UNKNOWN
} lte_bandwidth_e; //from lte_as.h
#endif //_ME_PLAYBACK_OFFLINE

/* tech type */
typedef enum
{
  LOC_WWAN_ME_GSM     = 0,
  LOC_WWAN_ME_WCDMA   = 1,
  LOC_WWAN_ME_LTE     = 2,
  LOC_WWAN_ME_TDSCDMA = 3,
  LOC_WWAN_ME_CDMA    = 4,
  LOC_WWAN_ME_HDR     = 5,
  LOC_WWAN_ME_MAX     = 6
}
loc_wwan_me_TechType;

/************************************ WCDMA ************************************/

typedef struct
{
  /* PLMN ID - MCC+ MNC */
  /* MCC */
  uint32 q_Mcc;

  /* MNC */
  uint32 q_Mnc;
  
  /* Indicates the LAC */
  uint16 w_LacId; 

  /* Indicates the service cell ID */
  uint32 q_ServCellId;

  /* WCDMA frequency assignment for the cell */
  uint16  w_Uarfcn;

  /* Primary Scrambling Code (PSC) */
  uint16 w_Psc;
}
loc_wwan_me_WcdmaCellInfoType;

typedef struct
{
  /* Meas uncertainity. Includes aging */
  uint16 w_Unc;

  /* Ec/Io in the best paths found in a sweep */
  int16 x_Ecio;

  /* Recieved Signal Power */
  int16 x_Rscp;
}
loc_wwan_me_WcdmaMeasInfoType;

/************************************ LTE ************************************/

typedef struct
{
  /*! Unique Cell ID */
  loc_wwan_me_LtePlmnIpcType z_UniqueId;
  
  /*! Physical cell ID */
  uint16 w_PhyCellId;

  /*! Tracking Area Code (for serving cells only) */
  uint16 w_Tac;

  /*! Global Cell ID (for serving cells only) */
  uint32 q_GlobalCellId;

  /*! Downlink bandwidth */
  #ifdef _ME_PLAYBACK_OFFLINE
    uint8 e_Bandwidth;    
  #else
    loc_wwan_me_LteBwType e_Bandwidth;
  #endif  

  /*! Number of Tx antennas detected; 0 indicates MIB invalid or unknown */
  uint8 u_NumAntennas;

  /*! E-ARFCN */
  lte_earfcn_t w_Earfcn;
  
} loc_wwan_me_LteCellInfoType;

typedef struct
{
  /* Meas uncertainity. */
  uint16 w_Unc;

  /*! Avg Reference signal - received power [0...97] in dBm, Q7 format */
  uint8 l_RsrpAvg;

  /*! Avg Reference signal - received quality [0...34] in dBm, Q7 format */
  uint8 l_RsrqAvg;
  
} loc_wwan_me_LteMeasInfoType;

/************************************ GERAN ************************************/

typedef enum
{
  LWM_GERAN_BAND_GSM_450,
  LWM_GERAN_BAND_GSM_480,
  LWM_GERAN_BAND_GSM_710,
  LWM_GERAN_BAND_GSM_750,
  LWM_GERAN_BAND_GSM_850,
  LWM_GERAN_BAND_EGSM_900,
  LWM_GERAN_BAND_PGSM_900,
  LWM_GERAN_BAND_RGSM_900,
  LWM_GERAN_BAND_DCS_1800,  
  LWM_GERAN_BAND_PCS_1900, 
  LWM_GERAN_BAND_UNKNOWN = 0xFF 
} 
loc_wwan_me_GeranBandType;

typedef struct
{
  /*
   * Cell ID. Range 0..65535, cell id not available: 0xffffffff
   * All data is invalid when q_CellId invalid
   */
  uint32 q_CellId;

  /*
   * Location Area Code (2 octet field). This field is the binary representation of
   * the Location Area Code, see 3GPP TS 23.003 [10]. The LAC field consists of 16 bits.
   * Bit 8 in octet 1 is the most significant bit and bit 1 in octet 2 is the least significant bit.
   */
  uint16 w_Lac;

  /* MCC and MNC */
  uint16 w_Mcc;
  uint16 w_Mnc;

  /* Absolute radio-frequency channel number. Range 0..1023 */
  uint16 w_Arfcn;

  /*
   * The base station identity code is a local colour code that allows an MS
   * to distinguish between different neighbouring base stations. Range 0..63.
   * BSIC is a 6 bit code which is structured as:
   * 3 LSBs: BS color code (BCC)
   * 3 MSBs: PLMN color code (NCC)
   */
  uint8 b_Bsic;

  /* GERAN band info */
  loc_wwan_me_GeranBandType e_Band;
}
loc_wwan_me_GeranCellInfoType;

typedef struct
{
  /* Meas uncertainity. Includes aging */
  uint16 w_Unc;

  /*
   * Received Signal Level. Range 0..63, See 45.008 sect 8.1.4, default: 0
   * RXLEV 0 = less than      110 dBm + SCALE.
   * RXLEV 1 =  110 dBm + SCALE    to   109 dBm + SCALE.
   * RXLEV 2 =  109 dBm + SCALE    to   108 dBm + SCALE.
   *     :
   *     :
   * RXLEV 62  =  49 dBm + SCALE   to   48 dBm + SCALE.
   * RXLEV 63  = greater than     48 dBm + SCALE.
   * where SCALE is an offset that is used in the ENHANCED MEASUREMENT REPORT and
   * PACKET ENHANCED MEASUREMENT REPORT messages, otherwise it is set to 0.
   */
  uint8 b_Rxlev;

  //todo
  /* Range 0..7, 0xffff - invalid */
  uint8 b_RxQualSub;
}
loc_wwan_me_GeranMeasInfoType;

/************************************* main DB ***********************************/

/*
  Bit 0 -> 1 = Measured as serving cell / 0 = Measured as Neighbor cell 
  Bit 1 -> 1 = Measured in traffic / 0 = Measured in idle  
*/

#define NGBR_CELL_IDLE 0  /* 00 */
#define SERV_CELL_IDLE 1      /* 01 */   
#define NGBR_CELL_TRAFFIC 2   /* 10 */
#define SERV_CELL_TRAFFIC 3   /* 11 */

typedef struct
{
  #ifdef _ME_PLAYBACK_OFFLINE
    uint8 e_ZppTech;    
  #else
    loc_wwan_me_TechType e_ZppTech;
  #endif   

  /*   Bit 0 -> 1 = Unique ID valid / 0 = Unique ID invalid   */
  uint8 b_cellFlags;

  /* WWAN cell info */
  union
  {
    loc_wwan_me_LteCellInfoType z_Lte;
    loc_wwan_me_WcdmaCellInfoType z_Wcdma;
    loc_wwan_me_GeranCellInfoType z_Geran;
  }
  z_CellInfo;
} loc_wwan_me_WwanCellInfoType;

typedef struct
{
  /* measurement timestamp */
  time_type t_Timestamp;

  /*
  Bit 0 -> 1 = Measured as serving cell / 0 = Measured as Neighbor cell 
  Bit 1 -> 1 = Measured in traffic / 0 = Measured in idle  
  */
  uint8 b_measFlags;

  /* measurement info for the WWAN cell */
  union
  {
    loc_wwan_me_LteMeasInfoType z_Lte;
    loc_wwan_me_WcdmaMeasInfoType z_Wcdma;
    loc_wwan_me_GeranMeasInfoType z_Geran;
  }
  z_MeasInfo;
} loc_wwan_me_WwanMeasInfoType;

typedef struct
{
  loc_wwan_me_WwanCellInfoType z_cell;
  loc_wwan_me_WwanMeasInfoType z_meas;
} loc_wwan_me_WwanInfoType;

/* API sent to ME from WWAN */
typedef struct
{
  /* number of cell meas. in this IPC */
  uint16 w_Count;

  /* time when RPT is sent to ME */
  time_type t_QxdmTime;

  /* IPC id or num. of IPCs sent to ME from WWAN */
  uint32 q_IpcCount;

  /* version ID of this structure */
  uint16 w_Version;

  /* WWAN info */
  loc_wwan_me_WwanInfoType z_WwanInfo[LOC_WWAN_ME_MAX_MEAS_PER_RPT];
} loc_wwan_me_WwanMeApiType;

typedef struct
{
  time_type t_ReportTimestamp;
  uint8 b_numOfCellMeas; 
  int8 b_indexOfServCell[LOC_WWAN_ME_MAX];

  /* number of ME-PE IPCs sent */
  uint32 q_IpcCount;

  /* version ID of this structure */
  uint16 w_Version;

  loc_wwan_me_WwanInfoType z_wwanInfo[LOC_WWAN_ME_MAX_CELL_ENTRIES];
  int16 l_uncertainty[LOC_WWAN_ME_MAX_CELL_ENTRIES];
} loc_wwan_me_WwanMeasSetType;

/*
 ******************************************************************************
 * Function loc_wwan_me_SendInitIPC
 *
 * Description:
 *
 *  Sends PGI the IPC message from WWAN ME initialization.
 *
 * Parameters:
 *  boolean premium_services_config.
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
void loc_wwan_me_SendInitIPC(boolean b_PremiumServicesConfig);


#endif /* LOC_WWAN_ME_API_H */


