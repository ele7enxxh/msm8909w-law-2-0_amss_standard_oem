#ifndef IRATMAN_LOG_H
#define IRATMAN_LOG_H

/*!
  @file   iratman_log.h

  @brief  IRAT List Manager logging header file

  @details
  Defines IRAT List Manager logging related types and interfaces
  used internally.

*/

/*===========================================================================

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/iratman/inc/iratman_log.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/14   pk      Chnaged IRATMAN_INFO_CURR_VER to 4
05/05/14   pk      Changes for FR-17879 (Extended EARFCN and band range)
12/06/13   bph     FR3839 - dynamic tbackoff
06/04/13   bph     Update logger for signed EARFCN priority and separate API
                   vs. timer expiry Tbackoff updating.
03/25/13   bph     Store RAT's node (more or less) preferrence
01/08/13   cjb     Started using new IRAT List Manager log packet.
01/08/13   cjb     First working version.
11/07/12   sst     Initial logging version.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>    /* Definition of basic types and macros */
#include <customer.h>

/* IRATMan exported header */
#include "iratman.h"

/* Other */
#include "lte_as.h"
#include "lte_cphy_irat_meas_msg.h"
#include "msg.h"
#include "timetick.h"

/*===========================================================================

                    INTERNAL DEFINES

===========================================================================*/
/* IRAT List Manager log packet */
#define IRATMAN_INFO_C              (0x1754)

#define IRATMAN_INFO_CURR_VER       4


/*===========================================================================

                    INTERNAL ENUM'S, MACROS, TYPEDEFS

===========================================================================*/
/* Max number of RX antennae used for LTE measurement */
#define IRATMAN_LOG_LTE_NUM_RX_ANTENNAS 2

/*! @brief    IRATman logging event enum type */
typedef enum
{
  IRATMAN_LOG_INVALID               = 0x0,

  /* ------------ EVENT LIST ------------ */
  IRATMAN_LOG_ADD_EVENT             = 0x1,
  IRATMAN_LOG_MEAS_PEND_EVENT       = 0x2,
  IRATMAN_LOG_MEAS_REQ_EVENT        = 0x3,
  IRATMAN_LOG_MEAS_RPT_EVENT        = 0x4,
  IRATMAN_LOG_ABORT_EVENT           = 0x5,
  IRATMAN_LOG_DELETE_EVENT          = 0x6,
  IRATMAN_LOG_DELETE_ALL_EVENT      = 0x7,
  IRATMAN_LOG_TRIM_EVENT            = 0x8,
  IRATMAN_LOG_RESELECTION_EVENT     = 0x9,
  IRATMAN_LOG_T_RESEL_START_EVENT   = 0xA,
  IRATMAN_LOG_T_RESEL_ABORT_EVENT   = 0xB,
  IRATMAN_LOG_T_RESEL_EXP_EVENT     = 0xC,
  IRATMAN_LOG_T_BACKOFF_START_EVENT = 0xD,
  IRATMAN_LOG_T_BACKOFF_ABORT_EVENT = 0xE,
  IRATMAN_LOG_T_BACKOFF_EXP_EVENT   = 0xF,
  /* ------------------------------------ */

  IRATMAN_LOG_MAX_EVENT             = 0x10
} iratman_log_event_enum;

/*! @brief RAT identifier for logging */
typedef enum
{
  RAT_LOG_INT    = RAT_INT,       //!< Internal identifier
  RAT_LOG_1X     = RAT_1X,        //!< CDMA 2000 RAT
  RAT_LOG_HDR    = RAT_HDR,       //!< HDR/EV-DO/eHRPD RAT
  RAT_LOG_LTE    = RAT_LTE,       //!< LTE RAT
  RAT_LOG_1X_HDR,                 //!< CDMA 2000 & HDR RAT
  RAT_LOG_MAX                     //!< Number of log RAT types
} iratman_log_rat_e_type;


/*-------------- LTE nodes --------------*/

/*! @brief  IRATman logging LTE add node type (Hand packed: sorted by size) */
typedef struct
{
  /*! Evolved Absolute Radio Frequency Channel Number (LTE freq number) */
  uint32  earfcn;

  /*! Priority of the EARFCN of the neighboring EUTRA network */
  int8    earfcn_priority;

  /*! LTE measuring bandwidth */
  uint8   bandwidth;

  /*! Threshold of serving(home_rat) frequency */
  uint8   thresh_serving;

  /*! Measurement comparison algorithm to be used for evaluation */
  uint8   meas_comp_algo;

  /*! Minimum received RSRP level from the EUTRA cell */
  uint8   rx_lev_min_eutra;

  /*! Offset to Minimum reference power level to select EUTRA */
  uint8   rx_lev_min_eutra_offset;

  /*! Maximum TX power in dBm */
  uint8   pe_max;

  /*! Minimum required signal threshold used for selecting an EARFCN */
  uint8   thresh_x;

  /*! Minimum received RSRQ level from the EUTRA cell */
  uint8   rsrq_min_eutra;

  /*! Offset to minimum RSRQ level to select EUTRA */
  uint8   rsrq_min_eutra_offset;

  /*! Minimum required signal quality threshold used for selecting an EARFCN */
  uint8   rsrq_thresh_x;

  /*! RATs using the LTE node */
  uint8   rats_known;

  /*! List requested by RAT */
  uint8   list;

  /*! List to which this node is actually a part of */
  uint8   list_actual;

  /*! Max Reselection timer */
  uint8   max_treselect;

  /*! Max Backoff timer duration (in ms) */
  uint8   max_tbackoff;

  /*! Min Backoff timer duration (in ms) */
  uint8   min_tbackoff;

  /*! Padding for word(32-bit) alignment */
  uint8   res1;
  uint8   res2;
  uint8   res3;
} iratman_log_lte_add_type;


/*! @brief  IRATman logging LTE measurement request node type (Hand packed) */
typedef struct
{
  /*! LTE measurement node returned */
  struct
  {
    /*! Evolved Absolute Radio Frequency Channel Number (LTE freq number) */
    uint32  earfcn;

    /*! Meas node data is valid */
    uint8   valid;

    /*! LTE measuring bandwidth */
    uint8   bandwidth;

  } meas_node;

  /*! Home RAT signal strength in -0.5 dB unit */
  uint8   home_rat_ecio;

  /*! Padding for word(32-bit) alignment */
  uint8   res1;

} iratman_log_lte_meas_req_type;


/*! @brief  IRATman logging LTE measurement report type (Hand packed) */
typedef struct
{
  /*! Evolved Absolute Radio Frequency Channel Number (LTE freq number) */
  uint32  earfcn;

  /*! Mobile maximum TX power as reported by LTE in dBm */
  int16   lte_max_tx_power;

  /*! LTE measuring bandwidth */
  uint8   bandwidth;

  /*! Measurement comparison algorithm to be used for evaluation */
  uint8   meas_comp_algo;

  /*! Minimum required RSRP threshold (ThreshX * 2) in dBm */
  uint8   rsrp_thresh;

  /*! Minimum required RSRQ threshold (ThreshXQ) in dBm */
  uint8   rsrq_thresh;

  /*! Number of LTE cells measured */
  uint8   num_cells;

  /*! RESEL info present */
  uint8   resel_valid;

  /*! Padding for word(32-bit) alignment */

} iratman_log_lte_meas_rpt_type;


/*! @brief  IRATman logging LTE measurement cell report type (Hand packed) */
typedef struct
{
  /*! The physical cell id of the detected cell */
  uint16  cell_id;

  /*! Instantaneous RSRP per antenna
  Range -144dBm..-44dBm with resolution of 1dBm
  Ref 36.133 Sec 9.1.4

  Values are returned in Q4 or X16 format meaning each unit is
  1/16th of a dbm.  To convert to int use CPHY_IRAT_MEAS_X16_TO_INT().
  */
  int16   rsrp_ant[IRATMAN_LOG_LTE_NUM_RX_ANTENNAS];

  /*! Instantaneous RSRQ per antenna
  Range -19.5dB ... -3dB
  Ref 36.133 Sec 9.1.7

  Values are returned in Q4 or X16 format meaning each unit is
  1/16th of a dbm.  To convert to int use CPHY_IRAT_MEAS_X16_TO_INT().
  */
  int16   rsrq_ant[IRATMAN_LOG_LTE_NUM_RX_ANTENNAS];

  /*!
   NOTE: For RSRP and RSRQ calculations we use MAX(Ant0, Ant1)
   LTE spec reference: Sections 5.1.1 and 5.1.3 of 36.214 v860
     "If receiver diversity is in use by the UE, the reported value
      shall not be lower than the corresponding RSRP/Q of any of the
      individual diversity branches."

    Values are returned in Q4 or X16 format meaning each unit is
    1/16th of a dbm.  To convert to int use CPHY_IRAT_MEAS_X16_TO_INT().
  */
  int16   rsrp_used;
  int16   rsrq_used;

  /*! Padding for word(32-bit) alignment */
  uint8   res1;
  uint8   res2;

} iratman_log_lte_meas_rpt_cell_type;


/*! @brief  IRATman logging LTE reselection information type (Hand packed) */
typedef struct
{
  /*! Frequency of the LTE cell to be selected */
  uint32  earfcn;

  /*! Cell id of the LTE cell to be selected */
  uint16  cell_id;

  /*! Calculated S_EUTRA value for the reselection cell */
  int16   s_eutra_val;

  /*! Padding for word(32-bit) alignment */

} iratman_log_lte_reselect_type;


/*! @brief  IRATman logging LTE delete node type (Hand packed) */
typedef struct
{
  /*! Evolved Absolute Radio Frequency Channel Number (LTE freq number) */
  uint32  earfcn;

  /*! RATs using the LTE node */
  uint8   rats_known;

  /*! Padding for word(32-bit) alignment */
  uint8   res1;
  uint8   res2;
  uint8   res3;

} iratman_log_lte_del_node_type;


/*! @brief  IRATman logging LTE meas timers information type (Hand packed) */
typedef struct
{
  uint32  current_time;    //!< (in sclks)
  uint32  tbackoff;        //!< (in ms)
  uint32  tbackoff_exp;    //!< (in sclks)
  uint32  treselect;       //!< (in ms)
  uint32  treselect_exp;   //!< (in sclks)
  uint32  earfcn;          //!< LTE frequency
  uint8   state;           //!< LTE node state

  /*! Padding for word(32-bit) alignment */
  uint8   res1;
  uint8   res2;
  uint8   res3;

} iratman_log_lte_timers_type;


/*-------------- Common Logging Structures --------------*/

/*! @brief    IRATman logging info structure type */
typedef struct
{
  uint8            version;         //!< Log packet version number
  uint8            event;           //!< logging event
  uint8            s_rat;           //!< Measuring RAT or source RAT
  uint8            m_rat;           //!< Measured RAT
} iratman_log_info_type;


/*===========================================================================

                    INTERNAL STRUCTURES

===========================================================================*/


/*===========================================================================

                    EXPORTED FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  iratman_log_init

===========================================================================*/
/*!
  @brief
    Initialize IRATMan logging module

  @details
    This function initializes the IRAT List Manager logging module

  @note
    Initialization should be done first by all RATs using IRATMan

  @return
    void

*/
/*=========================================================================*/
extern void iratman_log_init( void );

/*===========================================================================

  FUNCTION:  iratman_log_report_event

===========================================================================*/
/*!
  @brief
    Generate IRATMan log packet for the input event

  @details
    This function handles event based logging within IRAT List Manager

  @note
    iratman_log_init() should have been previously called

  @return
    void

*/
/*=========================================================================*/
extern void iratman_log_report_event
(
  iratman_log_event_enum     event,
  iratman_rat_e_type         home_rat, //!< Home/Originating RAT
  iratman_rat_e_type         meas_rat, //!< RAT to be measured
  void*                      info_ptr, //!< "event" specific data
  iratman_rat_node_type*     node      //!< Measurement "node" of interest
);

#endif /* IRATMAN_LOG_H */

