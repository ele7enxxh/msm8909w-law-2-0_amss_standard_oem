
/*!00
  @file
  loc_wwan_lte_api.h

  @brief
  Header file containing APIs for LTE ECID positioning feature

*/

/*===========================================================================

  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM ATHEROS, INCORPORATED and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM ATHEROS, INCORPORATED.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/api/loc_wwan_lte_api.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/07/12   cj      Created the file.
===========================================================================*/

#ifndef LOC_WWAN_LTE_API_H
#define LOC_WWAN_LTE_API_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"

#include "cai.h"
#include <time_svc.h>
#include "rex.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Maximum number of LTE cell measurements to be sent by ML1 in a report
*/
#define LOC_WWAN_LTE_CELL_MAX 10

/*! @brief Number of digits used for MCC and MNC
*/
#define LOC_WWAN_LTE_MAX_MCC_DIGITS 3 /*!< Maximum number of digits in MCC */
#define LOC_WWAN_LTE_MAX_MNC_DIGITS 3 /*!< Maximum number of digits in MNC */

/*! @brief Enum describing LTE cell type.
*/
typedef enum
{
  /*! this cell is a serving cell */
  LOC_WWAN_ME_LTE_CELL_TYPE_SERVING,

  /*! this cell is purely a ngbr cell */
  LOC_WWAN_ME_LTE_CELL_TYPE_NBR,
  
  /*! Max value */
  LOC_WWAN_ME_LTE_CELL_TYPE_MAX
}
loc_wwan_me_LteCellType;

/*! @brief Enum describing LTE connected status at the time of measurement.
*/
typedef enum
{
  /*! Measurement done in traffic mode */
  LOC_WWAN_ME_LTE_CALL_STATUS_TRAFFIC,

  /*! Measurement done in idle mode */
  LOC_WWAN_ME_LTE_CALL_STATUS_IDLE,

  /*! Max value */
  LOC_WWAN_ME_LTE_CALL_STATUS_MAX
}
loc_wwan_me_LteCallStatusType;

/*! @brief LTE Bandwidth 
*/
typedef enum
{
  /*! 1.4MHz bandwidth */
  LOC_WWAN_LTE_BW_NRB_6 = 6,
  /*! 3MHz bandwidth */
  LOC_WWAN_LTE_BW_NRB_15 = 15,
  /*! 5MHz bandwidth */
  LOC_WWAN_LTE_BW_NRB_25 = 25,
  /*! 10MHz bandwidth */
  LOC_WWAN_LTE_BW_NRB_50 = 50,
  /*! 15MHz bandwidth */
  LOC_WWAN_LTE_BW_NRB_75 = 75,
  /*! 20MHz bandwidth */
  LOC_WWAN_LTE_BW_NRB_100 = 100,
  /*! Minimum supported bandwidth */
  LOC_WWAN_LTE_BW_MIN = LOC_WWAN_LTE_BW_NRB_6,
  /*! Maximum supported bandwidth */
  LOC_WWAN_LTE_BW_MAX = LOC_WWAN_LTE_BW_NRB_100,
  /*! Unknown Bandwidth */
  LOC_WWAN_LTE_BW_UNKNOWN
} loc_wwan_me_LteBwType;

/*! @brief Data structure for PLMN type
*/
typedef struct
{
  uint32 mcc[LOC_WWAN_LTE_MAX_MCC_DIGITS]; /*!< Mobile country code */
  uint32 num_mnc_digits; /*!< Number of digits in MNC */
  uint32 mnc[LOC_WWAN_LTE_MAX_MNC_DIGITS]; /*!< Mobile network code */
} loc_wwan_me_LtePlmnType;

/*! @brief Struct containing LTE cell info and measurement.
*/
typedef struct
{
  /*! Slow Clock timestamp of the measurement */
  time_type t_Timestamp;

  /*! Unique Cell ID */
  loc_wwan_me_LtePlmnType z_plmn;

  /*! Global Cell ID */
  uint32 q_GCellId;

  /*!< Tracking area code of cell */
  uint16 w_Tac;
  
  /*! Physical cell ID */
  uint16 w_PhyCellId;

  /*! Downlink bandwidth */
  loc_wwan_me_LteBwType e_Bandwidth;

  /*! Number of Tx antennas detected; 0 indicates MIB invalid or unknown */
  uint8 u_NumAntennas;

  /*! E-ARFCN */
  lte_earfcn_t w_Earfcn;
  
  /*! cell type (Serving, Nbr) */
  loc_wwan_me_LteCellType e_CellType;

  /*! cell status (traffic, idle) */
  loc_wwan_me_LteCallStatusType e_CallStatus;

  /*! Meas uncertainity. */
  uint16 w_Unc;

  /*! Avg Reference signal - received power [0...97] in dBm, Q7 format */
  uint8 l_RsrpAvg;

  /*! Avg Reference signal - received quality [0...34] in dBm, Q7 format */
  uint8 l_RsrqAvg;
  
} loc_wwan_me_LteMeasType;

/*! @brief LTE report structure to be sent from ML1.
*/
typedef struct
{
  /*! Slow Clock time stamp at the time of reporting */
  time_type t_ReportTimestamp;

  /*! Number of Cell measurements being reported */
  uint8 b_numOfCellMeas; 

  /*! Cell Measurement Data */
  loc_wwan_me_LteMeasType z_LteCellData[LOC_WWAN_LTE_CELL_MAX];

} loc_wwan_me_LteMeasSetType;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  loc_wwan_me_LteMeasRpt

===========================================================================*/
/*!
    @brief
    API function to be called by LTE ML1 to report the LTE cell measurements
    for Enhanced Cell ID Positioning.

    @return
    None
*/
/*=========================================================================*/
extern void loc_wwan_me_LteMeasRpt(
  /*! Pointer to the LTE Cell meas report structure */
  const loc_wwan_me_LteMeasSetType * p_MeasRpt
);

#endif /* LOC_WWAN_LTE_API_H */
