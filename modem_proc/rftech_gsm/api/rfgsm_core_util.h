#ifndef RFGSM_CORE_UTIL_H
#define RFGSM_CORE_UTIL_H

/*!
   @file rfgsm_core_util.h

   @brief

   @details

*/

/*===========================================================================
Copyright (c) 2009-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_core_util.h#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/14/14   ec      Fix compiler warning 
07/14/14   ec      Types and Prototypes for HL/LL table maintainence
07/18/13   svi     Adding [QTUNER][RL] GSM NV Support.
04/05/13   kg      Adding new API to get band num from the rfcom band type enum 
08/20/09   sr      made the include file compatible with C++.
01/12/11   lcl     Added rfgsm_core_power_lvl_to_pcl prototype 
05/25/10   pl      Add API function to convert rfcom_gsm_band_type to rf_card_band 
05/21/10   tws     Add API function to translate rf_card_band type to 
                   rfcom_gsm_band type
12/05/09   sr      Added new function to return chan_index for G900 band.
07/06/09   sr      changes to make the operating band to rfcom_gsm_band type
06/14/09   sr      Initial revision.

============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "rfcom.h"
#include "rfgsm_nv_cmn.h"

// GSM Band Tx boundary definitions in Hz
#define RFGSM_CORE_850_TX_MIN_FREQ   824200000
#define RFGSM_CORE_850_TX_MAX_FREQ   848800000
#define RFGSM_CORE_900_TX_MIN_FREQ   880200000
#define RFGSM_CORE_900_TX_MAX_FREQ   914800000
#define RFGSM_CORE_1800_TX_MIN_FREQ 1710200000
#define RFGSM_CORE_1800_TX_MAX_FREQ 1784800000
#define RFGSM_CORE_1900_TX_MIN_FREQ 1850200000
#define RFGSM_CORE_1900_TX_MAX_FREQ 1909800000

// GSM Band Rx boundary definitions in Hz
#define RFGSM_CORE_850_RX_MIN_FREQ	  869200000
#define RFGSM_CORE_850_RX_MAX_FREQ	  893800000
#define RFGSM_CORE_900_RX_MIN_FREQ	  925200000
#define RFGSM_CORE_900_RX_MAX_FREQ	  959800000
#define RFGSM_CORE_1800_RX_MIN_FREQ	1805200000
#define RFGSM_CORE_1800_RX_MAX_FREQ	1879800000
#define RFGSM_CORE_1900_RX_MIN_FREQ	1930200000
#define RFGSM_CORE_1900_RX_MAX_FREQ	1989800000

// GSM Band Tx/Rx mask definitions
#define RFGSM_CORE_GSM850_RX_MASK_IDX  0
#define RFGSM_CORE_GSM900_RX_MASK_IDX  1
#define RFGSM_CORE_GSM1800_RX_MASK_IDX 2
#define RFGSM_CORE_GSM1900_RX_MASK_IDX 3
#define RFGSM_CORE_GSM850_TX_MASK_IDX  4
#define RFGSM_CORE_GSM900_TX_MASK_IDX  5
#define RFGSM_CORE_GSM1800_TX_MASK_IDX 6
#define RFGSM_CORE_GSM1900_TX_MASK_IDX 7

#define RFGSM_CORE_GSM850_RX_MASK  (uint8)(1<<(uint8)(RFGSM_CORE_GSM850_RX_MASK_IDX))
#define RFGSM_CORE_GSM900_RX_MASK  (uint8)(1<<(uint8)(RFGSM_CORE_GSM900_RX_MASK_IDX))
#define RFGSM_CORE_GSM1800_RX_MASK (uint8)(1<<(uint8)(RFGSM_CORE_GSM1800_RX_MASK_IDX))
#define RFGSM_CORE_GSM1900_RX_MASK (uint8)(1<<(uint8)(RFGSM_CORE_GSM1900_RX_MASK_IDX))
#define RFGSM_CORE_GSM850_TX_MASK  (uint8)(1<<(uint8)(RFGSM_CORE_GSM850_TX_MASK_IDX))
#define RFGSM_CORE_GSM900_TX_MASK  (uint8)(1<<(uint8)(RFGSM_CORE_GSM900_TX_MASK_IDX))
#define RFGSM_CORE_GSM1800_TX_MASK (uint8)(1<<(uint8)(RFGSM_CORE_GSM1800_TX_MASK_IDX))
#define RFGSM_CORE_GSM1900_TX_MASK (uint8)(1<<(uint8)(RFGSM_CORE_GSM1900_TX_MASK_IDX))

//GSM band ARFCN boundaries
#define RFGSM_CORE_850_MIN_ARFCN 128
#define RFGSM_CORE_850_MAX_ARFCN 251
#define RFGSM_CORE_850_NUM_ARFCN (RFGSM_CORE_850_MAX_ARFCN - RFGSM_CORE_850_MIN_ARFCN)
#define RFGSM_CORE_900_MIN_ARFCN 975
#define RFGSM_CORE_900_INT_ARFCN 1023
#define RFGSM_CORE_900_MAX_ARFCN 124
#define RFGSM_CORE_900_NUM_ARFCN ((RFGSM_CORE_900_INT_ARFCN - RFGSM_CORE_900_MIN_ARFCN) + RFGSM_CORE_900_MAX_ARFCN)
#define RFGSM_CORE_1800_MIN_ARFCN 512
#define RFGSM_CORE_1800_MAX_ARFCN 885
#define RFGSM_CORE_1800_NUM_ARFCN (RFGSM_CORE_1800_MAX_ARFCN - RFGSM_CORE_1800_MIN_ARFCN)
#define RFGSM_CORE_1900_MIN_ARFCN 512
#define RFGSM_CORE_1900_MAX_ARFCN 810
#define RFGSM_CORE_1900_NUM_ARFCN (RFGSM_CORE_1900_MAX_ARFCN - RFGSM_CORE_1900_MIN_ARFCN)

#define RFGSM_CORE_NUM_GSM_ARFCN  (RFGSM_CORE_850_NUM_ARFCN + RFGSM_CORE_900_NUM_ARFCN + RFGSM_CORE_1800_NUM_ARFCN + RFGSM_CORE_1900_NUM_ARFCN)

#define RFGSM_CORE_850_HL_TABLE_BASE  0
#define RFGSM_CORE_900_HL_TABLE_BASE  RFGSM_CORE_850_NUM_ARFCN
#define RFGSM_CORE_1800_HL_TABLE_BASE (RFGSM_CORE_900_HL_TABLE_BASE + RFGSM_CORE_900_NUM_ARFCN)
#define RFGSM_CORE_1900_HL_TABLE_BASE (RFGSM_CORE_1800_HL_TABLE_BASE + RFGSM_CORE_1800_NUM_ARFCN)

/*----------------------------------------------------------------------------*/
rfgsm_nv_supported_type rfgsm_core_rfcom_band_to_nvband(rfcom_gsm_band_type );

/*----------------------------------------------------------------------------*/
rfcom_gsm_band_type rfgsm_core_convert_arfcn_to_band( uint16 chan );

/*----------------------------------------------------------------------------*/
rfcom_gsm_band_type rfgsm_core_convert_rfband_to_rfcom( rfgsm_band_type );

/*----------------------------------------------------------------------------*/
uint16 rfgsm_core_G900_chan_index_get(uint16 chan );

/*----------------------------------------------------------------------------*/
rfcom_gsm_band_type rfgsm_core_rf_card_band_to_rfcom_gsm_band( rf_card_band_type );

/*----------------------------------------------------------------------------*/
rf_card_band_type rfgsm_core_rfcom_gsm_band_to_rf_card_band( rfcom_gsm_band_type );

/*----------------------------------------------------------------------------*/
uint16 rfgsm_core_power_lvl_to_pcl(rfcom_gsm_band_type band, uint16 pwr_lvl);

/*----------------------------------------------------------------------------*/
uint8 rfgsm_core_get_rx_band_mask(rfcom_gsm_band_type band);

/*----------------------------------------------------------------------------*/
uint8 rfgsm_core_get_tx_band_mask(rfcom_gsm_band_type band);

/*---------------------------------------------------------------------------------------*/
uint8 rfgsm_core_get_min_max_tx_freqs(rfcom_gsm_band_type band , uint32* min, uint32* max);

/*---------------------------------------------------------------------------------------*/
uint8 rfgsm_core_get_min_max_rx_freqs(rfcom_gsm_band_type band , uint32* min, uint32* max);

/*---------------------------------------------------------------------------------------*/
boolean rfgsm_core_get_norm_chan(rfcom_gsm_band_type band, uint16* arfcn, uint32* norm_chan);

/*---------------------------------------------------------------------------------------*/
boolean rfgsm_core_get_hl_table_base(rfcom_gsm_band_type band, uint32* base);

/*---------------------------------------------------------------------------------------*/
boolean rfgsm_core_get_get_band_num(rfcom_gsm_band_type band , uint16* num);
/*---------------------------------------------------------------------------------------*/
int16 rfgsm_core_dbm10_to_dbm100(int16 power);
/*---------------------------------------------------------------------------------------*/
int16 rfgsm_core_dbm100_to_dbm10(int16 power);

#ifdef __cplusplus
}
#endif



#endif /* RFGSM_CORE_UTIL_H */

