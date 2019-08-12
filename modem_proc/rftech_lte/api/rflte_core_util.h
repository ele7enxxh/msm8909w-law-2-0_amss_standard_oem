/*! 
 @file rflte_core_util.h 
 
 @brief 
  This is the header file file for utility functions for the LTE technology. 
 
 @details 
 
*/ 
  
/*=========================================================================== 
  
Copyright (c) 2009 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved. 
  
                           EDIT HISTORY FOR FILE 
  
This section contains comments describing changes made to this file. 
Notice that changes are listed in reverse chronological order. 

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rflte_core_util.h#1 $
  
When       who     what, where, why 
------------------------------------------------------------------------------- 
07/05/16   cv      Adding LTE B66 support
12/11/14   as      Added rflte_util_get_rx_freq_from_rx_chan() function
09/04/14   php     Change API to accept device as parameter
08/01/14   ndb     Added support for extended EARFCN for LTE
01/09/14   svi     Split Band Support : Tx path loss addition
08/12/13   bsh     Separate core and util functions
07/17/13   aca     Exported rflte_core_get_txagc_from_dbm10() & rflte_core_get_dbm10_from_txagc()
05/03/13   gvn     STG config for LTE RSB Cal
04/25/13   gvn     Support for Split bands for LTE
04/05/13   kg      Added API's to return rfcom type band mask and max/min
                   freq based on band
03/18/13   sbm     Defined debug NV mask for dispatch messages.
02/19/13   sbm     Bandedge max power backoff support in LTE(Nikel CR375880)
02/15/13   aca     API to convert Tx channel to Rx channel
02/05/13   kg      Added API's to return rfcom type band mask and max/min
                   freq based on band
01/17/13   gvn     Move some APIs to rflte_ext_mc.h since they are used by MCS team
12/06/12   gvn     Add helper function to convert LTE band from rfcom band to rfnv band
11/29/12   gvn     Add rflte_core_get_num_rb_from_bandwidth() (Merge from Nikel)
11/15/12   adk     Backed out Qtuner code
11/09/12   gvn     Add NS_11 support for channel bandwidth 15MHZ and 20MHZ(Merge from Nikel)
11/09/12   gvn     Merge from Nikel. Add new AMPR NVs support (multiple NS support per band) 
09/14/12   jf      Qtuner LTE support (initial)
09/07/12   sbm     Remove rflte_core_get_tx_cal_chan_freq()
09/07/12   sbm     nv tlb ptr changes for CA
06/17/12   pl      Update get NV interface to include device.
06/12/12   id      Add support to get HDET Vs AGC NV Item Id per tech and band 
05/23/12   ndb     Implementation of IM3 backoff for GPS desense
03/22/12   id      Added functions for 
                   1. Tx freq to chan conversion
                   2. Get Tx Multi Lin NV ID 
02/22/12   aca     Cleaned up LTE duplicate code for checking if a band is TDD
01/11/12   cri     Update to LTE Tx Frequency comensation algorithm
09/07/11   whc     New API to decouple PA State from PA Range  
08/18/11   pl      Added an interface to get tx_freq from just tx_channel
08/10/11   tnt     Initial Ant Tuner support  
06/23/11   pl      remove unused functions 
04/14/11   aca     Npler autogen support
04/01/11   bmg     Added extern "C" for C++ safety.
03/17/11   tnt     Add IQ capture functionality
03/02/11   tnt     Add rflte_core_convert_rfc_band_to_rfcom_band() 
01/05/11   pl      Add band info for gpio/grfc related interfaces
12/23/10   cri     Added SPI support for RAFT circuit
05/21/10   can     Added LTE TX RB based FREQ COMP support.  
05/06/10   tnt     Adding rflte_core_get_tx_freq_lo_hi_from_band
04/20/10   pl      Adding Band helper function 
04/19/10   qma     Disable RX function addition.
04/16/10   tnt     Disable TX function addition.
04/16/10   tnt     Adding support for run-time band change for PA_ON_SRC
03/12/10   tnt     Adding code to support GRFC for RF signals
08/25/09   qma     Added rflte_core_init_nv_table() to transfer rfcom LTE 
                   bands to rf nv LTE bands
08/07/09   can     New band and function addition for LTE.
04/16/09   can     Changes for integration. 
04/15/09   can     initial version for MDM9K. 
  
============================================================================*/ 
#ifndef RFLTE_CORE_UTIL_H
#define RFLTE_CORE_UTIL_H

#include "rflte_util.h"
#include "rflte_nv.h"
#include "rflte_core_txpl.h"
#include "rflte_core_iq_capture.h" 

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================
         DEFINES FOR FUNCTIONS THAT ARE MOVED TO OTHER FILES      
==========================================================================*/	

/*This is required to allow any file ourside rftech lte to continue using 
  rflte_core_util. In future this will be removed*/

/*----------------------------------------------------------------------------*/
#define rflte_core_get_uarfcn_from_rx_freq(rx_freq, band) rflte_util_get_uarfcn_from_rx_freq(rx_freq, band)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_uarfcn_from_tx_freq(tx_freq, band) rflte_util_get_uarfcn_from_tx_freq(tx_freq, band)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_tx_freq_lo_hi_from_band(band, f_ul_low, f_ul_hi) rflte_util_get_tx_freq_lo_hi_from_band(band, f_ul_low, f_ul_hi)

/*----------------------------------------------------------------------------*/
rfcom_lte_band_type rflte_core_get_band_from_rx_uarfcn(rfcom_lte_earfcn_type rx_chan);

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#define rflte_core_get_tx_freq_from_tx_chan(tx_chan) rflte_util_get_tx_freq_from_tx_chan(tx_chan)

#define rflte_core_get_rx_freq_from_rx_chan(rx_chan) rflte_util_get_rx_freq_from_rx_chan(rx_chan)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_tx_freq_lo_hi_from_rb(tx_chan, tx_bw, rb_start, rb_block, f_ul_low, f_ul_hi) rflte_util_get_tx_freq_lo_hi_from_rb(tx_chan, tx_bw, rb_start, rb_block, f_ul_low, f_ul_hi)

/*----------------------------------------------------------------------------*/
#define rflte_core_init_nv_table(device, band) rflte_util_init_nv_table(device, band)

/* ----------------------------------------------------------------------- */
#define rflte_core_lte_band_helper(rfcom_band) rflte_util_lte_band_helper(rfcom_band)

/* ----------------------------------------------------------------------- */
#define rflte_core_convert_rfc_band_to_rfcom_band(rfc_band) rflte_util_convert_rfc_band_to_rfcom_band(rfc_band)

/* ----------------------------------------------------------------------- */
#define rflte_core_convert_rfcom_band_to_rfnv_band(rfcom_band) rflte_util_convert_rfcom_band_to_rfnv_band(rfcom_band)

/* ----------------------------------------------------------------------- */
#define rflte_core_get_tx_cal_boundaries(tx_band_cal_data, device, band) rflte_util_get_tx_cal_boundaries(tx_band_cal_data, device, band)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_pa_range_from_pa_state(pa_state, rflte_nv_tbl_ptr) rflte_util_get_pa_range_from_pa_state(pa_state, rflte_nv_tbl_ptr)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_first_band_from_mask(band_mask) rflte_util_get_first_band_from_mask(band_mask)

/*----------------------------------------------------------------------------*/
#define rflte_core_is_band_tdd(rf_band) rflte_util_is_band_tdd(rf_band)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_rx_uarfcn_from_tx_uarfcn(tx_chan, band) rflte_util_get_rx_uarfcn_from_tx_uarfcn(tx_chan, band)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_tx_uarfcn_from_rx_uarfcn(rx_chan, band) rflte_util_get_tx_uarfcn_from_rx_uarfcn(rx_chan, band)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_ul_arfcn_from_freq(freq) rflte_util_get_ul_arfcn_from_freq(freq)

/*----------------------------------------------------------------------------*/
#define rflte_core_convert_nv_band_to_rfcom_band(band) rflte_util_convert_nv_band_to_rfcom_band(band)

/*----------------------------------------------------------------------------*/
#define rflte_core_check_rfc_band_support(void) rflte_util_check_rfc_band_support(void)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_hdetVsAgc_nvid(band) rflte_util_get_hdetVsAgc_nvid(band)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_num_rb_from_bandwidth(bw) rflte_util_get_num_rb_from_bandwidth(bw)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_min_max_rx_freqs(band, min, max) rflte_util_get_min_max_rx_freqs(band, min, max)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_band_mask(band) rflte_util_get_band_mask(band)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_min_max_tx_freqs(band, ful_low, ful_high) rflte_util_get_min_max_tx_freqs(band, ful_low, ful_high)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_band_num(band, num) rflte_util_rfcom_band_to_integer_band(band, num)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_fullband_from_subband(lte_rfcom_band) rflte_util_get_fullband_from_subband(lte_rfcom_band)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_dbm10_from_txagc(txagc) rflte_util_get_txagc_from_dbm10(txagc)

/*----------------------------------------------------------------------------*/
#define rflte_core_get_txagc_from_dbm10(dbm_10) rflte_util_get_dbm10_from_txagc(dbm_10)

/*----------------------------------------------------------------------------*/
uint32 rflte_core_get_rx_freq_from_uarfcn(rfcom_lte_earfcn_type rx_chan, rfcom_lte_band_type band);
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#define rflte_core_get_bw_in_khz(bw) rflte_util_get_bw_in_khz(bw)

/*----------------------------------------------------------------------------*/
rfcom_lte_band_type rflte_core_get_band_from_tx_uarfcn(rfcom_lte_earfcn_type tx_chan);

/*----------------------------------------------------------------------------*/
uint32 rflte_core_get_tx_freq_from_uarfcn(rfcom_lte_earfcn_type tx_chan, rfcom_lte_band_type band);

/*----------------------------------------------------------------------------*/
int8 rflte_util_get_path_loss_offset(rfcom_lte_band_type band, rfcom_lte_earfcn_type chan, rflte_nv_tbl_type *lte_nv_tbl_ptr);

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif

