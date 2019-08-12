/**
  @file geran_grr_types.h

  @brief Defines GRR functions available to software outside GERAN.
*/

/*============================================================================

  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.

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

============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/geran_grr_types.h#1 $ */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/10   gjohn   Initial revision
===========================================================================*/

#ifndef GERAN_GRR_TYPES_H
#define GERAN_GRR_TYPES_H

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#define GERAN_GRR_SI_MESSAGE_MAX_LENGTH 23

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/**
  @brief Enumeration of band indicator.
*/
typedef enum
{
  GERAN_GRR_BAND_IND_DCS_1800,
  GERAN_GRR_BAND_IND_PCS_1900,
  GERAN_GRR_BAND_IND_MAX
} geran_grr_band_ind_e;

/*!
  @brief This structure contains a single OTA-encoded GERAN system information
         message.
*/
typedef struct
{
  uint8 length;
  uint8 data[GERAN_GRR_SI_MESSAGE_MAX_LENGTH];
} geran_grr_si_message_t;

/*! @brief GSM cell Cell ID structure */
typedef struct
{
  uint8 ncc;
  uint8 bcc;
} geran_grr_gsm_cell_id_s;

/*!
  @brief This structure contains the inter-rat measurement data for timing transfer 
*/
typedef struct
{
  /*! @brief Indicates ARFCN of the GSM cell. */
  uint16 arfcn;

  /*! @brief Indicates whether the ARFCN belongs to 1800 or 1900 band
  if it lies in the 'overlap' range 512 - 810. */
  geran_grr_band_ind_e band_ind;

  /*! @brief Indicates the BSIC of the GSM cell. */
  geran_grr_gsm_cell_id_s bsic;

  /* Frame offset */
  uint32 fn_offset;
  
  /* qbit offset */
  uint16 qbit_offset;

  /* RSSI x 16 */
  int16  rssi_dBm_x16;

  /* frequency offset */
  int16  frequency_offset;

} geran_grr_irat_measurement_t;


#endif /* GERAN_GRR_TYPES_H */
