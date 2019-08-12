/*!
  @file
  rfnv_wcdma.c

  @brief

  @details
  This module retrieves all NV items that will be used by the WCDMA RF unit.

*/

/*===========================================================================

Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/nv/src/rfnv_wcdma.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   sub     F3 logging error fix in load rx static NV
03/26/15   ag      Corrected center channel in load Rx dynamic V2 NV
11/20/14   sd      Skip Tx Cal Chan list loading for B9, if B3 load is already done
11/10/14   kai     Fixed max number of PA states read error for W B6, B19
11/03/14   kai     Added support for WCDMA Rx V3 static NVs
10/31/14   ac      tuner support for DBDC
10/17/14   weiz	   Support of PA static compressed NV
10/13/14   vbh     Add support for NV container and get center channel number from NV, not hardcode to 7
09/26/14   kai     Added support for WCDMA Rx V2 NVs
09/20/14   zhh     fix KW warnings
05/01/14   aro     NV item to hold char data for DC ET Delay vs freq vs temp
02/26/14   zhh     Update XPT Delay structure to support DC-UPA
04/14/14   zhh     PinPout NV for DCUPA
07/22/14   bn      Removing redundant PA state loop in rfnv_wcdma_load_lin_vs_temp_vs_freq
03/20/14   zhh     update new MPR backoff NV
04/20/14   bn      WCDMA 8 PA state support and related cleanup
02/21/14   rmb     Add global NV pointers for each of band specific static NVs.
01/15/14   rmb     Add NV support for Noise Floor Correction.
11/25/13   rmb     Add Pin Cal versus freq related NVs for all PA states and bands.
11/08/13   vbh     corrected enum name in copy_rx_dynamic_data_from_legacy, copy_rx_static_data_from_legacy
10/23/13   vbh     Set the right variant marker version
10/17/13   vbh     Set default NV values for static and dynamic settings
09/20/13   dw      Replace rfcom_multi_carrier_hspa_id_type with rfcom_multi_carrier_id_type 
                   for carrier type indexing
08/27/13   vs      Set Rx static and Cal NVs to default values
08/23/13   vs      Added Rx static and Cal NVs
08/12/13   kai     Added DPD scaling support
08/01/13   kr      replace memcpy with memscpy
07/24/13   kai     Added XPT path delay offset vs temp vs freq
07/16/13   vs      Added CL tuner NVs
07/12/13   kcj     Support checking for critical NV items.
07/08/13   kcj     Can't use designated initialization with OFT's compiler. 
06/05/13   kcj     Dynamically allocated NV struct memory. 
05/16/13   cd      Temp Comp - Fixed scenario where both legacy and new Temp 
                   comp NVs are present in the phon
05/01/13   cd      Fixed KW error
04/30/13   cd      Add NV loading support for Lin vs Temp items into common
                   TxLin data types
06/21/12   id      Added support for HDET Offset and Span NV Write
06/12/12   id      Add support to get HDET Vs AGC NV Item Id per tech and band
05/14/12   dj      Add chan to freq conversion handler 
04/10/12   id      Added support for fetching Tx Multi Lin global structure 
                   and freq to chan conversion handler
12/15/11   swb     Add function call to initialize ICI coefficient NV status
11/16/11   ac      B19 call fix
04/11/11   sar     Relocated rfwcdma_core_convert_band_rfcom_to_rfnv to this file 
                   and renamed to rfnv_wcdma_convert_band_rfcom_to_rfnv.
03/04/11   ad      Fix writing to incorrect RFNV band for first sweep APT data    
02/24/11   ad      Connected SMPS NV to RFC structure for WCDMA
02/14/11   ac      vikingC IMT nv support for APT   
01/31/11   ad/dw   Fix RACH No ACK on B5 bands    
10/15/10   ac      rfnv load fix for B8 without resets
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
09/30/10   ac      efs support for all bands
06/21/10    dw     Added support for W1500 (BC11)
01/08/10   ckl     Removed high level lint error.
05/14/09   ckl     Added rfwcdma_nv_retrive_items()
03/15/09   ckl     Adapted rfcommon_nv.h
03/09/09   ckl     Code cleanup
10/21/05   ap      Added Support for W900 
05/25/05   adm     Added Support for W1800 for Triple Taco
05/16/05    rv     Initialize nv_tbl_ptr to 2100 table to avoid null ptr dereferencing.
03/29/05    xw     Deleted RF_HAS_WCDMA2100 etc for single RF driver.
06/07/04    eh     Modified rfnv_wcdma_get_nv_tbl_ptr to ERR_FATAL if band is invalid.
05/21/04   dhh     Added 800 band wcdma rf nv support.
04/17/04   jtn     Lint.
08/21/03   ap      Final Modifications for new NV mould
06/17/03   eh      Initial Revision.

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfcommon_nv.h"
#include "rfnv_wcdma.h"
#include "rfnv_wcdma_2100.h"
#include "rfnv_wcdma_1900.h"
#include "rfnv_wcdma_800.h"
#include "rfnv_wcdma_1800.h"
#include "rfnv_wcdma_900.h"
#include "rfnv_wcdma_bc4.h"
#include "rfnv_wcdma_1500.h"
#include "msg.h" 
#include "err.h"
#include "rfcommon_nv_mm.h"
#include "rfwcdma_core.h"
#include "modem_mem.h"
#include "rfnv_wcdma_types.h"
#include "rfnv_wcdma_nv_copy_funcs.h"
/* memscpy */
#include "rfumts.h"
#include "stringl.h"


/*===========================================================================
                        
                           CONSTANT DEFINITIONS

===========================================================================*/

rfwcdma_nv_rx_dyn_version_type rx_dyn_ver_nv = RFWCDMA_NV_RX_DYNAMIC_DATA_V1;
rfwcdma_nv_rx_static_version_type rx_stat_ver_nv = RFWCDMA_NV_RX_STATIC_DATA_V1;

/*===========================================================================
                        
                           DATA DEFINITIONS

===========================================================================*/

/* WCDMA RF NV tbl with default values */
/* If the non-calibrated items are not read from NV, the default values are used */
const rfnv_wcdma_nv_tbl_type rfnv_wcdma_default_nv_tbl =
{
/* Intermod Level */
  18,

/* Intermod Level 2 */
  168,

/* Intermod Level 3 */
  248,

/* Intermod Level 4 */
  0,

/* VGA gain offset versus temperature, 8 elements */
  {0,0,0,0,0,0,0,0},

/* IM2 I value */
  32,

/* IM2 Q value */
  32,

/* IM2 transconductor value */
  2,

/* Intermod Level */
  -152,

/* Intermod Level 2 */
  88,

/* Intermod Level 3 */
  248,

/* Intermod Level 4 */
  0,

/* LNA non-bypass hold timer */
  3,   /* in 66 us unit */

/* LNA bypass hold timer */
  15,  /* in 66 us unit */

/* Minimum output from the AGC_VALUE accumulator for Mixer high and LNA mid gain */
  -212,

/* Minimum output from the AGC_VALUE accumulator for Mixer high and LNA low gain */
  -80,

/* Minimum output from the AGC_VALUE accumulator for Mixer low and LNA low gain */
  112,

/* Maximum output from the AGC_VALUE accumulator */
  511,

/* Phase rotation setting for LNA decision */
  0,

/* Tx power limit versus temperature, 8 elements */
/* Each element = NV data plus RFNV_WCDMA_TX_LIM_OFFSET during copy */
/* These value - 768 = TX power limit value which is */
/* Equivalent to 23.5dbm for -57.33 dbm min TX power */
  { 962, 962, 962, 962, 962, 962, 962, 962},

/* UTRAN Tx power limit versus temperature offset, 8 elements */
  { 0, 0, 0, 0, 0, 0, 0, 0},

/* Tx power limit versus frequency */
  { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

/* CDMA Tx power limiter loop gain, scaled by RF_CDMA_TX_LIM_ADJ_SCALE */
  2,

/* CDMA expected HDET versus AGC */
  { 22,25,29,33,36,43,49,56,64,71,80,88,98,110,123,138,0},

/* ADC HDET reading offset */
  64,

/* ADC HDET reading span */
  54,

/* UE maximum TX power in dbm unit */
  24,

/* searcher Out of Service Threshold */
  -102,


  (uint32) RFWCDMA_NV_MDSP_BTFDELAY_VAL,
  RFWCDMA_NV_RX_DELAY_VAL,

/* AGC_PA_ON_RISE_DELAY */
  936,

/* AGC_PA_ON_FALL_DELAY */
  1092,

/* AGC_TX_ON_RISE_DELAY */
  744,

/* AGC_TX_ON_FALL_DELAY */
  1092,

/* AGC_UPDATE_TX_AGC_TIME */
  2240,

/* PA gain range update time */
  5600,
  5050,

/* RFR UHF VCO Coarse tune values for IMT band */
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

/* PRACH PA R1 offsets */
  0,
  0,

/* Cal channel list, reserve RF chain index for future use. 1 chain used. */
/* tx cal channels */
  {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  },

/* tx cal channels in frequency */
  {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  },

/* Interpolation enable */
  0,

/* PRACH PA_R2 rise offset */
  0,

/* PRACH PA_R2 fall offset */
  0,

/* PRACH PA_R3 rise offset */
  0,

/* PRACH PA_R3 fall offset */
  0,

/* DCH AGC Update Tx AGC time - MDSP_modUpdateTxAGCInChipx8 */
  3920,

/* AGC Tx Adj PDM delay */
0,

/* HS Power switch from 2 --> 1 state */
  295,

/* HS Power switch from 1 --> 2 state */
  247,

/* HS Power switch from 2 --> 1 state */
  511,

/* HS Power switch from 1 --> 2 state */
  511,

/* HS Power switch from 3 --> 2 state */
  511,

/* HS Power switch from 2 --> 3 state */
  511,

/* Max Power Backoff Voltages in ADC counts */
  { 47, 89, 140},

/* Max Power Backoff in dB12 units @ Volt1 */
  { 0, 0, 12, 24},

/* Max Power Backoff in dB12 units @ Volt2 */
  { 0, 0, 12, 24},

/* Max Power Backoff in dB12 units @ Volt3 */
  { 0, 0, 12, 24},

/* In-Phase 16-bit signed A coefficient for IMT pri RXFE mismatch comp */
  0,

/* In-Phase 16-bit signed B coefficient for IMT pri RXFE mismatch comp */
  0x8000,

/* IMT primary RXFE ICI filter I channel coefficients 0_1 */
0x903FE,

/* IMT primary RXFE ICI filter I channel coefficients 2_3 */
0x38E000B,

/* IMT primary RXFE ICI filter I channel coefficients 4_5 */
0x33D00D8,

/* IMT primary RXFE ICI filter I channel coefficients 6_7 */
0xFD03F5,

/* IMT primary RXFE ICI filter I channel coefficients 8_9 */
0x101009B,

/* IMT primary RXFE ICI filter I channel coefficients 10_11 */
0x39003C2,

/* IMT primary RXFE ICI filter I channel coefficients 12_13 */
0x37F00A9,

/* IMT primary RXFE ICI filter I channel coefficients 14_15 */
0x3F3003B,

/* IMT primary RXFE ICI filter Q channel coefficients 0_1 */
0x0,

/* IMT primary RXFE ICI filter Q channel coefficients 2_3 */
0x0,

/* IMT primary RXFE ICI filter Q channel coefficients 4_5 */
0x0,

/* IMT primary RXFE ICI filter Q channel coefficients 6_7 */
0x0,

/* IMT primary RXFE ICI filter Q channel coefficients 8_9 */
0x0,

/* IMT primary RXFE ICI filter Q channel coefficients 10_11 */
0x0,

/* IMT primary RXFE ICI filter Q channel coefficients 12_13 */
0x0,

/* IMT primary RXFE ICI filter Q channel coefficients 14_15 */
0x0,

/* IMT primary RXFE ICI filter IQ trunc bits */
0xB5,

/* diversity path  rx agc offset for each vga gain based on temperature */
  {0,0,0,0,0,0,0,0},

/* diversity path  im threshold from g0 to g1 */
  -152,

/* diversity path  im threshold from g2 to g1 gain states */
  88,

/* diversity path  im threshold from g3 to g2 gain states */
  248,

/* diversity path  im threshold from g4 to g3 gain states */
  736,


/* diversity path  i cancellation value for lower in-band jammer */
  32,

/* diversity path  q cancellation value for lower in-band jammer */
  32,

/* diversity path  transconductor value for lower in-band jammer */
  2,


/* diversity path lna non-bypass hold timer from low to high gain - multiples of 66us */
  3, //???  /* in 66 us unit */

/* diversity path lna bypass hold timer from high to low gain - multiples of 66us */
  15, //???  /* in 66 us unit */

/* dummy array not used in this band */
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

/* diversity mode select switch settings */
  {0,1,0},

/* I accumulator for DC in WCDMA 2100 band */
  { 256, 256, 256, 256, 256, 256},

/* Q accumulator for DC in WCDMA 2100 band */
  { 256, 256, 256, 256, 256, 256},

/* WCDMA expected HDET versus AGC 12-bit HDET */
  { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

/* Cx16 delay between primary and diversity paths */
  0,

/* primary path LNA phase control */
  {0, 1, 3, 7, 0xF, 0x1F},

/* diversity path LNA phase control */
  {0, 1, 3, 7, 0xF, 0x1F},

/* MPR based Tx Channel beta scaling for Release 6 waveforms */
  {32767, 29205, 27571, 26029, 24573, 23198, 21900},

/* TxAGC offset to compensate for MPR based Tx Channel beta scaling */
  {0, 10, 15, 20, 25, 30, 35},

/* MPR backoff array for Release 6 waveforms */
  {0, 0, 5, 10, 15, 20, 25},

/* In-Phase 16-bit signed A coefficient for IMT div RXFE mismatch comp */
  0,

/* In-Phase 16-bit signed B coefficient for IMT div RXFE mismatch comp */
  0x8000,

/* IMT diversity RXFE ICI filter I channel coefficients 0_1 */
0x903FE,

/* IMT diversity RXFE ICI filter I channel coefficients 2_3 */
0x38E000B,

/* IMT diversity RXFE ICI filter I channel coefficients 4_5 */
0x33D00D8,

/* IMT diversity RXFE ICI filter I channel coefficients 6_7 */
0xFD03F5,

/* IMT diversity RXFE ICI filter I channel coefficients 8_9 */
0x101009B,

/* IMT diversity RXFE ICI filter I channel coefficients 10_11 */
0x39003C2,

/* IMT diversityRXFE ICI filter I channel coefficients 12_13 */
0x37F00A9,

/* IMT diversity RXFE ICI filter I channel coefficients 14_15 */
0x3F3003B,

/* IMT diversity RXFE ICI filter Q channel coefficients 0_1 */
0x0,

/* IMT diversity RXFE ICI filter Q channel coefficients 2_3 */
0x0,

/* IMT diversity RXFE ICI filter Q channel coefficients 4_5 */
0x0,

/* IMT diversity RXFE ICI filter Q channel coefficients 6_7 */
0x0,

/* IMT diversity RXFE ICI filter Q channel coefficients 8_9 */
0x0,

/* IMT diversity RXFE ICI filter Q channel coefficients 10_11 */
0x0,

/* IMT diversity RXFE ICI filter Q channel coefficients 12_13 */
0x0,

/* IMT diversity RXFE ICI filter Q channel coefficients 14_15 */
0x0,

/* IMT diversity RXFE ICI filter IQ trunc bits */
0xB5,

/* wcdma_rxf_ici_cal_data_car0 */
 {0x3503F2, 0x903C2, 0x3D20035, 0x8803D2, 0xA90107, 0x3B303DC, 0x3A70079, 0x3F60028,
   0x1803F9, 0x303E5, 0x3F10016, 0x3303E9, 0x560076 , 0x3D903F5, 0x3D80039, 0x3FB0012, 0x2B5},

/* wcdma_rxf_ici_cal_data_car1 */
{0x3503F2 , 0x903C2 , 0x3D20035, 0x8803D2, 0xA90107, 0x3B303DC, 0x3A70079, 0x3F60028 , 
  0x3E80007, 0x3FD001B , 0xF03EA , 0x3CD0017 ,0x3AA038A,0x27000B,0x2803C7, 0x503EE, 0x2B5},

/* c1_wcdma_rxf_ici_cal_data_car0 */
 {0x3503F2, 0x903C2, 0x3D20035, 0x8803D2, 0xA90107, 0x3B303DC, 0x3A70079, 0x3F60028,
   0x1803F9, 0x303E5, 0x3F10016, 0x3303E9, 0x560076 , 0x3D903F5, 0x3D80039, 0x3FB0012, 0x2B5},

/* c1_wcdma_rxf_ici_cal_data_car1 */
{0x3503F2 , 0x903C2 , 0x3D20035, 0x8803D2, 0xA90107, 0x3B303DC, 0x3A70079, 0x3F60028 , 
  0x3E80007, 0x3FD001B , 0xF03EA , 0x3CD0017 ,0x3AA038A,0x27000B,0x2803C7, 0x503EE, 0x2B5},

/*SAR Back-off NV*/
{300,300,300,300,300,300,300,300},

/* Ant Tuner NV */
{   {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
},

/* lin_temp_comp_data */
{0},

/* tx_gtr_thresh */
{0,0,0,0},

/* EPT DPD static params for expansion and processing */
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

/* ET path delay value*/
0,

/* Max Transmit Power Limit Vs Frequency */
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

/* HDET Vs Temperature */
{0,0,0,0,0,0,0,0},

/* Allowable HDET correction range from static temp comp*/
{
  {-50,-50,-50,-50,-50,-50,-50,-50},
  { 50, 50, 50, 50, 50, 50, 50, 50},
},

/* XPT path delay 16channels delay info */
{
  0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
},

/* XPT DC delay 16channels delay info */
{
  0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
},

/* XPT path delay 16channels delay info */
{
  0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  },
},

/* XPT path DC delay 16channels delay info */
{
  0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  },
},


/* CL antenna tuner control info */
{
  0,
  NULL
},

/* XPT DPD scaling */
{
  0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
},

/* Rx static data*/
 {
  {0},
 },

/* Rx dynamic data*/
 {
  {0},
 },

/* PRx Noise floor */
-452,

/* PRx Noise floor Car 1 */
-452,

/* DRx Noise floor Car 0 */
-452,

/* DRx Noise floor Car 1 */
-452,

/* MPR backoff valuee */
{
  0,
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},
  },
},

/* DC lin_temp_comp_data */
{0, 0},

/* pa_static */
{
 {0}, //header
 {0}, //pa_range_map
 {0}, //pa_fall_swpt [0th element -> timer hystersis]
 {0}, //pa_rise_swpt
 {0}, //tx_rot_angle
 {0}, //pa_comp_up
 {0}, //pa_comp_down
 { //pin values
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
 },
 { //pout values
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
 },
 {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}}, //pa_comp_up_vs_freq
 {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}}, //pa_comp_down_vs_freq
 {{0},{0},{0},{0},{0}},  //reserved
},

/* dc_pa_static */
{
 {0}, //header
 {0}, //pa_range_map
 {0}, //pa_fall_swpt [0th element -> timer hystersis]
 {0}, //pa_rise_swpt
 {0}, //tx_rot_angle
 {0}, //pa_comp_up
 {0}, //pa_comp_down
 { //pin values
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
 },
 { //pout values
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
 },
 {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}}, //pa_comp_up_vs_freq
 {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}}, //pa_comp_down_vs_freq
 {{0},{0},{0},{0},{0}},  //reserved
},

/* Ant Tuner NV */
{   {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
},
}; /* rfnv_wcdma_nv_tbl_type rfnv_wcdma_default_nv_tbl */

/*===========================================================================
                        
                        FUNCTION DEFINITIONS

===========================================================================*/
/* Function to determine which, if any RFCOM band we should use for a
   given NV table */
static rfcom_wcdma_band_type rfnv_wcdma_get_band_for_nv_table(rfnv_wcdma_table_type nv_band);

/* Init the copy function pointers. */
void rfnv_wcdma_init_nv_copy_function_list(void);

/* Function used for getting NV status of ICI coefficients.  Used in this file   
   because we need the tcb task pointer, which is available during startup */
void rfwcdma_msm_get_ici_nv_status(rex_tcb_type *task_ptr,
                               rex_sigs_type task_wait_sig,
                               void (*task_wait_handler)( rex_sigs_type ));

rfnv_wcdma_nv_tbl_type *rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_supported_mode_type band );
rfnv_wcdma_nv_tbl_type *rfnv_wcdma_tbl_ptr[RFCOM_NUM_WCDMA_BANDS]=
{
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};
static uint64 band_mask ;

/* Flag indicating if the descriptive lists have been initialized. */
static boolean rfnv_wcdma_nv_lists_initialized = FALSE;

/*!
  @brief   
    This structure holds the HDET Vs AGC NVs  
    for all supported WCDMA bands */
rfnv_wcdma_hdet_vs_agc_nv_type rfnv_wcdma_hdet_vs_agc_nv_list[RFCOM_NUM_WCDMA_BANDS] =
{
    {RFNV_WCDMA_2100_EXP_HDET_VS_AGC_V2_I},  /* BC1 */
    {RFNV_WCDMA_1900_EXP_HDET_VS_AGC_V2_I},  /* BC2 */
    {RFNV_WCDMA_1800_EXP_HDET_VS_AGC_V2_I},  /* BC3 */  
    {RFNV_WCDMA_BC4_EXP_HDET_VS_AGC_V2_I},   /* BC4 */  
    {RFNV_WCDMA_800_EXP_HDET_VS_AGC_V2_I},   /* BC5 */
    {RFNV_WCDMA_900_EXP_HDET_VS_AGC_V2_I},   /* BC8 */
    {RFNV_WCDMA_1800_EXP_HDET_VS_AGC_V2_I},  /* BC9 */
    {RFNV_WCDMA_1500_EXP_HDET_VS_AGC_V2_I},  /* BC11 */
    {RFNV_WCDMA_800_EXP_HDET_VS_AGC_V2_I}   /* BC19 */
};

/* Descripter lists defining the band specific enum for each NV type. */
rfnv_wcdma_nv_items_list * nv_item_lists[RFNV_WCDMA_NUM_TABLES] = 
{
  &rfnv_wcdma_2100_nv_items_list,
  &rfnv_wcdma_1900_nv_items_list,
  &rfnv_wcdma_1800_nv_items_list,
  &rfnv_wcdma_bc4_nv_items_list,
  &rfnv_wcdma_800_nv_items_list,
  &rfnv_wcdma_900_nv_items_list,
  &rfnv_wcdma_1500_nv_items_list
};

/* Pointers to the nv container struct for each band. */
rfnv_wcdma_nv_tbl_type* nv_tables[RFNV_WCDMA_NUM_TABLES] = 
{
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

static rfwcdma_nv_pa_static_ver_type rfnv_wcdma_pa_static_nv_version[RFCOM_NUM_WCDMA_BANDS] = 
{
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA
};

static rfwcdma_nv_pa_static_ver_type rfnv_wcdma_dc_pa_static_nv_version[RFCOM_NUM_WCDMA_BANDS] = 
{
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA,
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA
};

/* Defines how each NV item type will be copied into the */
/* NV container struct. */
rfnv_wcdma_nv_copy_funcs_list nv_copy_funcs;

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Initializes the list of copy function pointers.

  @details
  Initialization is done in this function instead of statically for readability.
  Descriptive static init was originally used but the off-target environment
  only supports C89 which does not have that feature.

*/
void rfnv_wcdma_init_nv_copy_function_list(void)
{
  nv_copy_funcs.copy_funcs_type.copy_lna_hs_range_rise = rfnv_wcdma_copy_lna_hs_range_rise;
  nv_copy_funcs.copy_funcs_type.copy_lna_hs_range_rise_2 = rfnv_wcdma_copy_lna_hs_range_rise_2;
  nv_copy_funcs.copy_funcs_type.copy_lna_hs_range_rise_3 = rfnv_wcdma_copy_lna_hs_range_rise_3;
  nv_copy_funcs.copy_funcs_type.copy_lna_hs_range_rise_4 = rfnv_wcdma_copy_lna_hs_range_rise_4;
  nv_copy_funcs.copy_funcs_type.copy_lna_hs_range_rise_5 = rfnv_wcdma_copy_lna_hs_range_rise_5;
  nv_copy_funcs.copy_funcs_type.copy_lna_hs_range_fall = rfnv_wcdma_copy_lna_hs_range_fall;
  nv_copy_funcs.copy_funcs_type.copy_lna_hs_range_fall_2 = rfnv_wcdma_copy_lna_hs_range_fall_2;
  nv_copy_funcs.copy_funcs_type.copy_lna_hs_range_fall_3 = rfnv_wcdma_copy_lna_hs_range_fall_3;
  nv_copy_funcs.copy_funcs_type.copy_lna_hs_range_fall_4 = rfnv_wcdma_copy_lna_hs_range_fall_4;
  nv_copy_funcs.copy_funcs_type.copy_lna_hs_range_fall_5 = rfnv_wcdma_copy_lna_hs_range_fall_5;
  nv_copy_funcs.copy_funcs_type.copy_im_hs_level = rfnv_wcdma_copy_im_hs_level;
  nv_copy_funcs.copy_funcs_type.copy_im_hs_level_2 = rfnv_wcdma_copy_im_hs_level_2;
  nv_copy_funcs.copy_funcs_type.copy_im_hs_level_3 = rfnv_wcdma_copy_im_hs_level_3;
  nv_copy_funcs.copy_funcs_type.copy_im_hs_level_4 = rfnv_wcdma_copy_im_hs_level_4;
  nv_copy_funcs.copy_funcs_type.copy_vga_gain_offset = rfnv_wcdma_copy_vga_gain_offset;
  nv_copy_funcs.copy_funcs_type.copy_vga_gain_offset_vs_freq = rfnv_wcdma_copy_vga_gain_offset_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_c1_vga_gain_offset_vs_freq = rfnv_wcdma_copy_c1_vga_gain_offset_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_vga_gain_offset_vs_temp = rfnv_wcdma_copy_vga_gain_offset_vs_temp;
  nv_copy_funcs.copy_funcs_type.copy_im2_i_value = rfnv_wcdma_copy_im2_i_value;
  nv_copy_funcs.copy_funcs_type.copy_im2_q_value = rfnv_wcdma_copy_im2_q_value;
  nv_copy_funcs.copy_funcs_type.copy_im2_transconductor_value = rfnv_wcdma_copy_im2_transconductor_value;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_rise = rfnv_wcdma_copy_lna_range_rise;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_rise_2 = rfnv_wcdma_copy_lna_range_rise_2;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_rise_3 = rfnv_wcdma_copy_lna_range_rise_3;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_rise_4 = rfnv_wcdma_copy_lna_range_rise_4;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_rise_5 = rfnv_wcdma_copy_lna_range_rise_5;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_fall = rfnv_wcdma_copy_lna_range_fall;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_fall_2 = rfnv_wcdma_copy_lna_range_fall_2;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_fall_3 = rfnv_wcdma_copy_lna_range_fall_3;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_fall_4 = rfnv_wcdma_copy_lna_range_fall_4;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_fall_5 = rfnv_wcdma_copy_lna_range_fall_5;
  nv_copy_funcs.copy_funcs_type.copy_im_level = rfnv_wcdma_copy_im_level;
  nv_copy_funcs.copy_funcs_type.copy_im_level_2 = rfnv_wcdma_copy_im_level_2;
  nv_copy_funcs.copy_funcs_type.copy_im_level_3 = rfnv_wcdma_copy_im_level_3;
  nv_copy_funcs.copy_funcs_type.copy_im_level_4 = rfnv_wcdma_copy_im_level_4;
  nv_copy_funcs.copy_funcs_type.copy_nonbypass_timer = rfnv_wcdma_copy_nonbypass_timer;
  nv_copy_funcs.copy_funcs_type.copy_bypass_timer = rfnv_wcdma_copy_bypass_timer;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_offset = rfnv_wcdma_copy_lna_range_offset;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_offset_2 = rfnv_wcdma_copy_lna_range_offset_2;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_offset_3 = rfnv_wcdma_copy_lna_range_offset_3;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_offset_4 = rfnv_wcdma_copy_lna_range_offset_4;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_offset_5 = rfnv_wcdma_copy_lna_range_offset_5;
  nv_copy_funcs.copy_funcs_type.copy_lna_offset_vs_freq = rfnv_wcdma_copy_lna_offset_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_offset_vs_freq = rfnv_wcdma_copy_c1_lna_offset_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_lna_offset_vs_freq_2 = rfnv_wcdma_copy_lna_offset_vs_freq_2;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_offset_vs_freq_2 = rfnv_wcdma_copy_c1_lna_offset_vs_freq_2;
  nv_copy_funcs.copy_funcs_type.copy_lna_offset_vs_freq_3 = rfnv_wcdma_copy_lna_offset_vs_freq_3;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_offset_vs_freq_3 = rfnv_wcdma_copy_c1_lna_offset_vs_freq_3;
  nv_copy_funcs.copy_funcs_type.copy_lna_offset_vs_freq_4 = rfnv_wcdma_copy_lna_offset_vs_freq_4;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_offset_vs_freq_4 = rfnv_wcdma_copy_c1_lna_offset_vs_freq_4;
  nv_copy_funcs.copy_funcs_type.copy_rx_agc_min = rfnv_wcdma_copy_rx_agc_min;
  nv_copy_funcs.copy_funcs_type.copy_rx_agc_min_2 = rfnv_wcdma_copy_rx_agc_min_2;
  nv_copy_funcs.copy_funcs_type.copy_rx_agc_min_3 = rfnv_wcdma_copy_rx_agc_min_3;
  nv_copy_funcs.copy_funcs_type.copy_rx_agc_max = rfnv_wcdma_copy_rx_agc_max;
  nv_copy_funcs.copy_funcs_type.copy_agc_phase_offset = rfnv_wcdma_copy_agc_phase_offset;
  nv_copy_funcs.copy_funcs_type.copy_r1_rise = rfnv_wcdma_copy_r1_rise;
  nv_copy_funcs.copy_funcs_type.copy_r1_fall = rfnv_wcdma_copy_r1_fall;
  nv_copy_funcs.copy_funcs_type.copy_tx_lim_vs_temp = rfnv_wcdma_copy_tx_lim_vs_temp;
  nv_copy_funcs.copy_funcs_type.copy_utran_tx_lim_vs_temp_offset = rfnv_wcdma_copy_utran_tx_lim_vs_temp_offset;
  nv_copy_funcs.copy_funcs_type.copy_tx_lim_vs_freq = rfnv_wcdma_copy_tx_lim_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_tx_lim_adj_gain = rfnv_wcdma_copy_tx_lim_adj_gain;
  nv_copy_funcs.copy_funcs_type.copy_exp_hdet_vs_agc = rfnv_wcdma_copy_exp_hdet_vs_agc;
  nv_copy_funcs.copy_funcs_type.copy_hdet_off = rfnv_wcdma_copy_hdet_off;
  nv_copy_funcs.copy_funcs_type.copy_hdet_spn = rfnv_wcdma_copy_hdet_spn;
  nv_copy_funcs.copy_funcs_type.copy_max_tx_power = rfnv_wcdma_copy_max_tx_power;
  nv_copy_funcs.copy_funcs_type.copy_out_of_service_thresh = rfnv_wcdma_copy_out_of_service_thresh;
  nv_copy_funcs.copy_funcs_type.copy_enc_btf_dly = rfnv_wcdma_copy_enc_btf_dly;
  nv_copy_funcs.copy_funcs_type.copy_rx_delay = rfnv_wcdma_copy_rx_delay;
  nv_copy_funcs.copy_funcs_type.copy_agc_pa_on_rise_delay = rfnv_wcdma_copy_agc_pa_on_rise_delay;
  nv_copy_funcs.copy_funcs_type.copy_agc_pa_on_fall_delay = rfnv_wcdma_copy_agc_pa_on_fall_delay;
  nv_copy_funcs.copy_funcs_type.copy_agc_tx_on_rise_delay = rfnv_wcdma_copy_agc_tx_on_rise_delay;
  nv_copy_funcs.copy_funcs_type.copy_agc_tx_on_fall_delay = rfnv_wcdma_copy_agc_tx_on_fall_delay;
  nv_copy_funcs.copy_funcs_type.copy_agc_update_tx_agc_time = rfnv_wcdma_copy_agc_update_tx_agc_time;
  nv_copy_funcs.copy_funcs_type.copy_pa_gain_up_time = rfnv_wcdma_copy_pa_gain_up_time;
  nv_copy_funcs.copy_funcs_type.copy_pa_gain_down_time = rfnv_wcdma_copy_pa_gain_down_time;
  nv_copy_funcs.copy_funcs_type.copy_tx_rot_ang_pa_00 = rfnv_wcdma_copy_tx_rot_ang_pa_00;
  nv_copy_funcs.copy_funcs_type.copy_tx_rot_ang_pa_01 = rfnv_wcdma_copy_tx_rot_ang_pa_01;
  nv_copy_funcs.copy_funcs_type.copy_tx_rot_ang_pa_10 = rfnv_wcdma_copy_tx_rot_ang_pa_10;
  nv_copy_funcs.copy_funcs_type.copy_tx_rot_ang_pa_11 = rfnv_wcdma_copy_tx_rot_ang_pa_11;
  nv_copy_funcs.copy_funcs_type.copy_rtr_vco_coarse_tuning = rfnv_wcdma_copy_rtr_vco_coarse_tuning;
  nv_copy_funcs.copy_funcs_type.copy_pa_compensate_up = rfnv_wcdma_copy_pa_compensate_up;
  nv_copy_funcs.copy_funcs_type.copy_pa_compensate_down = rfnv_wcdma_copy_pa_compensate_down;
  nv_copy_funcs.copy_funcs_type.copy_prach_r1_rise_offset = rfnv_wcdma_copy_prach_r1_rise_offset;
  nv_copy_funcs.copy_funcs_type.copy_prach_r1_fall_offset = rfnv_wcdma_copy_prach_r1_fall_offset;
  nv_copy_funcs.copy_funcs_type.copy_tx_cal_chan = rfnv_wcdma_copy_tx_cal_chan;
  nv_copy_funcs.copy_funcs_type.copy_rx_cal_chan = rfnv_wcdma_copy_rx_cal_chan;
  nv_copy_funcs.copy_funcs_type.copy_c1_rx_cal_chan = rfnv_wcdma_copy_c1_rx_cal_chan;
  nv_copy_funcs.copy_funcs_type.copy_r2_rise = rfnv_wcdma_copy_r2_rise;
  nv_copy_funcs.copy_funcs_type.copy_r2_fall = rfnv_wcdma_copy_r2_fall;
  nv_copy_funcs.copy_funcs_type.copy_r3_rise = rfnv_wcdma_copy_r3_rise;
  nv_copy_funcs.copy_funcs_type.copy_r3_fall = rfnv_wcdma_copy_r3_fall;
  nv_copy_funcs.copy_funcs_type.copy_pa_compensate_up_r2 = rfnv_wcdma_copy_pa_compensate_up_r2;
  nv_copy_funcs.copy_funcs_type.copy_pa_compensate_down_r2 = rfnv_wcdma_copy_pa_compensate_down_r2;
  nv_copy_funcs.copy_funcs_type.copy_pa_compensate_up_r3 = rfnv_wcdma_copy_pa_compensate_up_r3;
  nv_copy_funcs.copy_funcs_type.copy_pa_compensate_down_r3 = rfnv_wcdma_copy_pa_compensate_down_r3;
  nv_copy_funcs.copy_funcs_type.copy_prach_r2_rise_offset = rfnv_wcdma_copy_prach_r2_rise_offset;
  nv_copy_funcs.copy_funcs_type.copy_prach_r2_fall_offset = rfnv_wcdma_copy_prach_r2_fall_offset;
  nv_copy_funcs.copy_funcs_type.copy_prach_r3_rise_offset = rfnv_wcdma_copy_prach_r3_rise_offset;
  nv_copy_funcs.copy_funcs_type.copy_prach_r3_fall_offset = rfnv_wcdma_copy_prach_r3_fall_offset;
  nv_copy_funcs.copy_funcs_type.copy_pa_range_map = rfnv_wcdma_copy_pa_range_map;
  nv_copy_funcs.copy_funcs_type.copy_dch_agc_update_tx_agc_time = rfnv_wcdma_copy_dch_agc_update_tx_agc_time;
  nv_copy_funcs.copy_funcs_type.copy_agc_tx_adj_pdm_delay = rfnv_wcdma_copy_agc_tx_adj_pdm_delay;
  nv_copy_funcs.copy_funcs_type.copy_hs_r1_rise = rfnv_wcdma_copy_hs_r1_rise;
  nv_copy_funcs.copy_funcs_type.copy_hs_r1_fall = rfnv_wcdma_copy_hs_r1_fall;
  nv_copy_funcs.copy_funcs_type.copy_hs_r2_rise = rfnv_wcdma_copy_hs_r2_rise;
  nv_copy_funcs.copy_funcs_type.copy_hs_r2_fall = rfnv_wcdma_copy_hs_r2_fall;
  nv_copy_funcs.copy_funcs_type.copy_hs_r3_rise = rfnv_wcdma_copy_hs_r3_rise;
  nv_copy_funcs.copy_funcs_type.copy_hs_r3_fall = rfnv_wcdma_copy_hs_r3_fall;
  nv_copy_funcs.copy_funcs_type.copy_max_pwr_backoff_voltages = rfnv_wcdma_copy_max_pwr_backoff_voltages;
  nv_copy_funcs.copy_funcs_type.copy_max_pwr_backoff_volt1 = rfnv_wcdma_copy_max_pwr_backoff_volt1;
  nv_copy_funcs.copy_funcs_type.copy_max_pwr_backoff_volt2 = rfnv_wcdma_copy_max_pwr_backoff_volt2;
  nv_copy_funcs.copy_funcs_type.copy_max_pwr_backoff_volt3 = rfnv_wcdma_copy_max_pwr_backoff_volt3;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_mis_comp_a_coeff = rfnv_wcdma_copy_wcdma_rxf_mis_comp_a_coeff;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_mis_comp_b_coeff = rfnv_wcdma_copy_wcdma_rxf_mis_comp_b_coeff;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_i_coef0_1 = rfnv_wcdma_copy_wcdma_rxf_ici_i_coef0_1;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_i_coef2_3 = rfnv_wcdma_copy_wcdma_rxf_ici_i_coef2_3;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_i_coef4_5 = rfnv_wcdma_copy_wcdma_rxf_ici_i_coef4_5;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_i_coef6_7 = rfnv_wcdma_copy_wcdma_rxf_ici_i_coef6_7;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_i_coef8_9 = rfnv_wcdma_copy_wcdma_rxf_ici_i_coef8_9;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_i_coef10_11 = rfnv_wcdma_copy_wcdma_rxf_ici_i_coef10_11;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_i_coef12_13 = rfnv_wcdma_copy_wcdma_rxf_ici_i_coef12_13;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_i_coef14_15 = rfnv_wcdma_copy_wcdma_rxf_ici_i_coef14_15;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_q_coef0_1 = rfnv_wcdma_copy_wcdma_rxf_ici_q_coef0_1;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_q_coef2_3 = rfnv_wcdma_copy_wcdma_rxf_ici_q_coef2_3;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_q_coef4_5 = rfnv_wcdma_copy_wcdma_rxf_ici_q_coef4_5;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_q_coef6_7 = rfnv_wcdma_copy_wcdma_rxf_ici_q_coef6_7;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_q_coef8_9 = rfnv_wcdma_copy_wcdma_rxf_ici_q_coef8_9;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_q_coef10_11 = rfnv_wcdma_copy_wcdma_rxf_ici_q_coef10_11;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_q_coef12_13 = rfnv_wcdma_copy_wcdma_rxf_ici_q_coef12_13;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_q_coef14_15 = rfnv_wcdma_copy_wcdma_rxf_ici_q_coef14_15;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_iq_trunc = rfnv_wcdma_copy_wcdma_rxf_ici_iq_trunc;
  nv_copy_funcs.copy_funcs_type.copy_c1_vga_gain_offset = rfnv_wcdma_copy_c1_vga_gain_offset;
  nv_copy_funcs.copy_funcs_type.copy_c1_vga_gain_offset_vs_temp = rfnv_wcdma_copy_c1_vga_gain_offset_vs_temp;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_rise = rfnv_wcdma_copy_c1_lna_range_rise;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_rise_2 = rfnv_wcdma_copy_c1_lna_range_rise_2;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_rise_3 = rfnv_wcdma_copy_c1_lna_range_rise_3;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_rise_4 = rfnv_wcdma_copy_c1_lna_range_rise_4;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_rise_5 = rfnv_wcdma_copy_c1_lna_range_rise_5;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_fall = rfnv_wcdma_copy_c1_lna_range_fall;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_fall_2 = rfnv_wcdma_copy_c1_lna_range_fall_2;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_fall_3 = rfnv_wcdma_copy_c1_lna_range_fall_3;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_fall_4 = rfnv_wcdma_copy_c1_lna_range_fall_4;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_fall_5 = rfnv_wcdma_copy_c1_lna_range_fall_5;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_offset = rfnv_wcdma_copy_c1_lna_range_offset;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_offset_2 = rfnv_wcdma_copy_c1_lna_range_offset_2;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_offset_3 = rfnv_wcdma_copy_c1_lna_range_offset_3;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_offset_4 = rfnv_wcdma_copy_c1_lna_range_offset_4;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_offset_5 = rfnv_wcdma_copy_c1_lna_range_offset_5;
  nv_copy_funcs.copy_funcs_type.copy_c1_im_level = rfnv_wcdma_copy_c1_im_level;
  nv_copy_funcs.copy_funcs_type.copy_c1_im_level_2 = rfnv_wcdma_copy_c1_im_level_2;
  nv_copy_funcs.copy_funcs_type.copy_c1_im_level_3 = rfnv_wcdma_copy_c1_im_level_3;
  nv_copy_funcs.copy_funcs_type.copy_c1_im_level_4 = rfnv_wcdma_copy_c1_im_level_4;
  nv_copy_funcs.copy_funcs_type.copy_c1_im2_i_value = rfnv_wcdma_copy_c1_im2_i_value;
  nv_copy_funcs.copy_funcs_type.copy_c1_im2_q_value = rfnv_wcdma_copy_c1_im2_q_value;
  nv_copy_funcs.copy_funcs_type.copy_c1_im2_transconductor_value = rfnv_wcdma_copy_c1_im2_transconductor_value;
  nv_copy_funcs.copy_funcs_type.copy_c1_nonbypass_timer = rfnv_wcdma_copy_c1_nonbypass_timer;
  nv_copy_funcs.copy_funcs_type.copy_c1_bypass_timer = rfnv_wcdma_copy_c1_bypass_timer;
  nv_copy_funcs.copy_funcs_type.copy_rfr_vco_coarse_tune_2_1900 = rfnv_wcdma_copy_rfr_vco_coarse_tune_2_1900;
  nv_copy_funcs.copy_funcs_type.copy_rf_drx_mode_sel = rfnv_wcdma_copy_rf_drx_mode_sel;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_iaccum = rfnv_wcdma_copy_wcdma_iaccum;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_qaccum = rfnv_wcdma_copy_wcdma_qaccum;
  nv_copy_funcs.copy_funcs_type.copy_timer_hysterisis = rfnv_wcdma_copy_timer_hysterisis;
  nv_copy_funcs.copy_funcs_type.copy_exp_hdet_vs_agc_v2 = rfnv_wcdma_copy_exp_hdet_vs_agc_v2;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_c0_c1_delay = rfnv_wcdma_copy_wcdma_c0_c1_delay;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_lna_phase_ctl = rfnv_wcdma_copy_wcdma_lna_phase_ctl;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_lna_phase_ctl = rfnv_wcdma_copy_c1_wcdma_lna_phase_ctl;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rel6_tx_beta_scaling_comp = rfnv_wcdma_copy_wcdma_rel6_tx_beta_scaling_comp;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rel6_tx_agc_offset = rfnv_wcdma_copy_wcdma_rel6_tx_agc_offset;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rel6_tx_mpr_backoff = rfnv_wcdma_copy_wcdma_rel6_tx_mpr_backoff;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_mis_comp_a_coeff = rfnv_wcdma_copy_c1_wcdma_rxf_mis_comp_a_coeff;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_mis_comp_b_coeff = rfnv_wcdma_copy_c1_wcdma_rxf_mis_comp_b_coeff;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_i_coef0_1 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef0_1;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_i_coef2_3 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef2_3;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_i_coef4_5 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef4_5;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_i_coef6_7 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef6_7;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_i_coef8_9 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef8_9;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_i_coef10_11 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef10_11;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_i_coef12_13 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef12_13;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_i_coef14_15 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef14_15;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_q_coef0_1 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef0_1;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_q_coef2_3 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef2_3;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_q_coef4_5 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef4_5;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_q_coef6_7 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef6_7;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_q_coef8_9 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef8_9;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_q_coef10_11 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef10_11;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_q_coef12_13 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef12_13;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_q_coef14_15 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef14_15;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_iq_trunc = rfnv_wcdma_copy_c1_wcdma_rxf_ici_iq_trunc;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_rise_car1 = rfnv_wcdma_copy_lna_range_rise_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_rise_2_car1 = rfnv_wcdma_copy_lna_range_rise_2_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_rise_3_car1 = rfnv_wcdma_copy_lna_range_rise_3_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_rise_4_car1 = rfnv_wcdma_copy_lna_range_rise_4_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_rise_5_car1 = rfnv_wcdma_copy_lna_range_rise_5_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_fall_car1 = rfnv_wcdma_copy_lna_range_fall_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_fall_2_car1 = rfnv_wcdma_copy_lna_range_fall_2_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_fall_3_car1 = rfnv_wcdma_copy_lna_range_fall_3_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_fall_4_car1 = rfnv_wcdma_copy_lna_range_fall_4_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_fall_5_car1 = rfnv_wcdma_copy_lna_range_fall_5_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_offset_car1 = rfnv_wcdma_copy_lna_range_offset_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_offset_2_car1 = rfnv_wcdma_copy_lna_range_offset_2_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_offset_3_car1 = rfnv_wcdma_copy_lna_range_offset_3_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_offset_4_car1 = rfnv_wcdma_copy_lna_range_offset_4_car1;
  nv_copy_funcs.copy_funcs_type.copy_lna_range_offset_5_car1 = rfnv_wcdma_copy_lna_range_offset_5_car1;
  nv_copy_funcs.copy_funcs_type.copy_vga_gain_offset_car1 = rfnv_wcdma_copy_vga_gain_offset_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_rise_car1 = rfnv_wcdma_copy_c1_lna_range_rise_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_rise_2_car1 = rfnv_wcdma_copy_c1_lna_range_rise_2_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_rise_3_car1 = rfnv_wcdma_copy_c1_lna_range_rise_3_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_rise_4_car1 = rfnv_wcdma_copy_c1_lna_range_rise_4_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_rise_5_car1 = rfnv_wcdma_copy_c1_lna_range_rise_5_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_fall_car1 = rfnv_wcdma_copy_c1_lna_range_fall_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_fall_2_car1 = rfnv_wcdma_copy_c1_lna_range_fall_2_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_fall_3_car1 = rfnv_wcdma_copy_c1_lna_range_fall_3_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_fall_4_car1 = rfnv_wcdma_copy_c1_lna_range_fall_4_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_fall_5_car1 = rfnv_wcdma_copy_c1_lna_range_fall_5_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_offset_car1 = rfnv_wcdma_copy_c1_lna_range_offset_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_offset_2_car1 = rfnv_wcdma_copy_c1_lna_range_offset_2_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_offset_3_car1 = rfnv_wcdma_copy_c1_lna_range_offset_3_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_offset_4_car1 = rfnv_wcdma_copy_c1_lna_range_offset_4_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_lna_range_offset_5_car1 = rfnv_wcdma_copy_c1_lna_range_offset_5_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_vga_gain_offset_car1 = rfnv_wcdma_copy_c1_vga_gain_offset_car1;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_cal_data_car0 = rfnv_wcdma_copy_wcdma_rxf_ici_cal_data_car0;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_rxf_ici_cal_data_car1 = rfnv_wcdma_copy_wcdma_rxf_ici_cal_data_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_cal_data_car0 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_cal_data_car0;
  nv_copy_funcs.copy_funcs_type.copy_c1_wcdma_rxf_ici_cal_data_car1 = rfnv_wcdma_copy_c1_wcdma_rxf_ici_cal_data_car1;
  nv_copy_funcs.copy_funcs_type.copy_wcdma_db10_sar_back_off_limit = rfnv_wcdma_copy_wcdma_db10_sar_back_off_limit;
  nv_copy_funcs.copy_funcs_type.copy_ant_tuner = rfnv_wcdma_copy_ant_tuner;
  nv_copy_funcs.copy_funcs_type.copy_pa_comp_up_r1_vs_freq = rfnv_wcdma_copy_pa_comp_up_r1_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_pa_comp_up_r2_vs_freq = rfnv_wcdma_copy_pa_comp_up_r2_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_pa_comp_up_r3_vs_freq = rfnv_wcdma_copy_pa_comp_up_r3_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_pa_comp_dn_r1_vs_freq = rfnv_wcdma_copy_pa_comp_dn_r1_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_pa_comp_dn_r2_vs_freq = rfnv_wcdma_copy_pa_comp_dn_r2_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_pa_comp_dn_r3_vs_freq = rfnv_wcdma_copy_pa_comp_dn_r3_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_tx_gtr_thresh = rfnv_wcdma_copy_tx_gtr_thresh;
  nv_copy_funcs.copy_funcs_type.copy_tx_ept_dpd_config_params = rfnv_wcdma_copy_tx_ept_dpd_config_params;
  nv_copy_funcs.copy_funcs_type.copy_et_delay_value = rfnv_wcdma_copy_et_delay_value;
  nv_copy_funcs.copy_funcs_type.copy_mtpl_vs_freq = rfnv_wcdma_copy_mtpl_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_hdet_vs_temp = rfnv_wcdma_copy_hdet_vs_temp;
  nv_copy_funcs.copy_funcs_type.copy_hdet_mod_range = rfnv_wcdma_copy_hdet_mod_range;
  nv_copy_funcs.copy_funcs_type.copy_tx_lin_vs_temp_0 = rfnv_wcdma_copy_tx_lin_vs_temp_0;
  nv_copy_funcs.copy_funcs_type.copy_tx_lin_vs_temp_1 = rfnv_wcdma_copy_tx_lin_vs_temp_1;
  nv_copy_funcs.copy_funcs_type.copy_tx_lin_vs_temp_2 = rfnv_wcdma_copy_tx_lin_vs_temp_2;
  nv_copy_funcs.copy_funcs_type.copy_tx_lin_vs_temp_3 = rfnv_wcdma_copy_tx_lin_vs_temp_3;
  nv_copy_funcs.copy_funcs_type.copy_tx_lin_vs_temp_0_addl = rfnv_wcdma_copy_tx_lin_vs_temp_0_addl;
  nv_copy_funcs.copy_funcs_type.copy_tx_lin_vs_temp_1_addl = rfnv_wcdma_copy_tx_lin_vs_temp_1_addl;
  nv_copy_funcs.copy_funcs_type.copy_tx_lin_vs_temp_2_addl = rfnv_wcdma_copy_tx_lin_vs_temp_2_addl;
  nv_copy_funcs.copy_funcs_type.copy_tx_lin_vs_temp_3_addl = rfnv_wcdma_copy_tx_lin_vs_temp_3_addl;
  nv_copy_funcs.copy_funcs_type.copy_tx_lin_vs_temp_vs_freq_p_in = rfnv_wcdma_copy_tx_lin_vs_temp_vs_freq_p_in;
  nv_copy_funcs.copy_funcs_type.copy_tx_lin_vs_temp_vs_freq_p_out = rfnv_wcdma_copy_tx_lin_vs_temp_vs_freq_p_out;
  nv_copy_funcs.copy_funcs_type.copy_tx_xpt_delay = rfnv_wcdma_copy_tx_xpt_delay;
  nv_copy_funcs.copy_funcs_type.copy_tx_delay_offsets = rfnv_wcdma_copy_tx_delay_offsets;
  nv_copy_funcs.copy_funcs_type.copy_ant_tuner_cl_ctrl_info = rfnv_wcdma_copy_ant_tuner_cl_ctrl_info;
  nv_copy_funcs.copy_funcs_type.copy_dpd_scaling_data = rfnv_wcdma_copy_dpd_scaling_value;
  nv_copy_funcs.copy_funcs_type.copy_rx_static_data = rfnv_wcdma_copy_rx_static_data;
  nv_copy_funcs.copy_funcs_type.copy_rx_dyn_data = rfnv_wcdma_copy_rx_cal_data;
  nv_copy_funcs.copy_funcs_type.copy_noise_floor = rfnv_wcdma_copy_noise_floor;
  nv_copy_funcs.copy_funcs_type.copy_noise_floor_car1 = rfnv_wcdma_copy_noise_floor_car1;
  nv_copy_funcs.copy_funcs_type.copy_c1_noise_floor_car0 = rfnv_wcdma_copy_c1_noise_floor_car0;
  nv_copy_funcs.copy_funcs_type.copy_c1_noise_floor_car1 = rfnv_wcdma_copy_c1_noise_floor_car1;
  nv_copy_funcs.copy_funcs_type.copy_pin_cal_vs_freq = rfnv_wcdma_copy_pin_cal_vs_freq;
  nv_copy_funcs.copy_funcs_type.copy_tx_xpt_dc_delay= rfnv_wcdma_copy_tx_xpt_dc_delay;
  nv_copy_funcs.copy_funcs_type.copy_mpr_backoff = rfnv_wcdma_copy_mpr_backoff;
  nv_copy_funcs.copy_funcs_type.copy_dc_tx_lin_vs_temp_vs_freq_p_in = rfnv_wcdma_copy_dc_tx_lin_vs_temp_vs_freq_p_in;
  nv_copy_funcs.copy_funcs_type.copy_dc_tx_lin_vs_temp_vs_freq_p_out = rfnv_wcdma_copy_dc_tx_lin_vs_temp_vs_freq_p_out;
  nv_copy_funcs.copy_funcs_type.copy_tx_dc_delay_offsets = rfnv_wcdma_copy_tx_dc_delay_offsets;
  nv_copy_funcs.copy_funcs_type.copy_pa_static = rfnv_wcdma_copy_pa_static;
  nv_copy_funcs.copy_funcs_type.copy_pa_static_z = rfnv_wcdma_copy_pa_static;
  nv_copy_funcs.copy_funcs_type.copy_dc_pa_static = rfnv_wcdma_copy_dc_pa_static;
  nv_copy_funcs.copy_funcs_type.copy_dc_pa_static_z = rfnv_wcdma_copy_dc_pa_static;
  nv_copy_funcs.copy_funcs_type.copy_rx_static_data_v2 = rfnv_wcdma_copy_rx_static_data;
  nv_copy_funcs.copy_funcs_type.copy_rx_dyn_data_v2 = rfnv_wcdma_copy_rx_cal_data_v2;
  nv_copy_funcs.copy_funcs_type.copy_ant_tuner_car1 = rfnv_wcdma_copy_ant_tuner_car1;
  nv_copy_funcs.copy_funcs_type.copy_c0_rx_static_data_v3 = rfnv_wcdma_copy_c0_rx_static_data;
  nv_copy_funcs.copy_funcs_type.copy_c1_rx_static_data_v3 = rfnv_wcdma_copy_c1_rx_static_data;
  nv_copy_funcs.copy_funcs_type.copy_c2_rx_static_data_v3 = rfnv_wcdma_copy_c2_rx_static_data;
  nv_copy_funcs.copy_funcs_type.copy_c3_rx_static_data_v3 = rfnv_wcdma_copy_c3_rx_static_data;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Returns the WCDMA band for a given NV band.

  @details
  Returns the RFCOM WCDMA band for a NV band if the band is supported.

  @param nv_band NV table band

  @return rfcom_wcdma_band_type WCDMA RFCOM band
  
*/
rfcom_wcdma_band_type rfnv_wcdma_get_band_for_nv_table(rfnv_wcdma_table_type nv_band)
{
  rfcom_wcdma_band_type band = RFCOM_BAND_INVALID;

  band_mask = rfcommon_nv_get_rf_bc_config(RF_PATH_0);

  switch (nv_band)
  {
    case RFNV_WCDMA_TABLE_IMT_2100:
      if( band_mask & ((uint64)SYS_BAND_MASK_WCDMA_I_IMT_2000) )
      {
        band = RFCOM_BAND_IMT;
      }
      break;
    case RFNV_WCDMA_TABLE_PCS_1900:
      if( band_mask & ((uint64)SYS_BAND_MASK_WCDMA_II_PCS_1900) )
      {
        band = RFCOM_BAND_1900;
      }
      break;
    case RFNV_WCDMA_TABLE_1800:
      if (band_mask & ((uint64)SYS_BAND_MASK_WCDMA_III_1700) )
      {
        band = RFCOM_BAND_BC3;
      }
      else if( band_mask & ((uint64)SYS_BAND_MASK_WCDMA_IX_1700) )
      {
        band = RFCOM_BAND_BC9;
      }
      break;
    case RFNV_WCDMA_TABLE_BC4:
      if( band_mask & ((uint64)SYS_BAND_MASK_WCDMA_IV_1700))
      {
        band = RFCOM_BAND_BC4;
      }
      break;
    case RFNV_WCDMA_TABLE_800:
      if( band_mask & ((uint64)SYS_BAND_MASK_WCDMA_V_850|(uint64)SYS_BAND_MASK_WCDMA_VI_800|(uint64)SYS_BAND_MASK_WCDMA_XIX_850))
      {
        band = RFCOM_BAND_800;
      }
      break;
    case RFNV_WCDMA_TABLE_BC8_900:
      if( band_mask & ((uint64)SYS_BAND_MASK_WCDMA_VIII_900 ))
      {
        band = RFCOM_BAND_BC8;
      }
      break;
    case RFNV_WCDMA_TABLE_BC11_1500:
      if( band_mask & ((uint64)SYS_BAND_MASK_WCDMA_XI_1500 ))
      {
        band = RFCOM_BAND_BC11;
      }
      break;
    case RFNV_WCDMA_NUM_TABLES:
    case RFNV_WCDMA_TABLE_INVALID:
    default:
      break;
  }

  return band;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Initializes MSM configuration based on RF HW.

  @details
  Returns pointer to WCDMA RF NV table.

  @param band Supported band

  @return rfnv_wcdma_nv_tbl_type WCDMA NV calibration table pointer
  
*/
rfnv_wcdma_nv_tbl_type *rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_supported_mode_type band )
{
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr = NULL;

  /*lint -e787 not all enum's used in switch */
  switch (band)
  {
    case RFNV_WCDMA_2100:
      nv_tbl_ptr = nv_tables[RFNV_WCDMA_TABLE_IMT_2100];
      break;
    
    case RFNV_WCDMA_1900:
      nv_tbl_ptr = nv_tables[RFNV_WCDMA_TABLE_PCS_1900];
      break;

    case RFNV_WCDMA_1800:
      nv_tbl_ptr = nv_tables[RFNV_WCDMA_TABLE_1800];
      break;

    case RFNV_WCDMA_BC4:
      nv_tbl_ptr = nv_tables[RFNV_WCDMA_TABLE_BC4];
      break;

    case RFNV_WCDMA_900:
      nv_tbl_ptr = nv_tables[RFNV_WCDMA_TABLE_BC8_900];
      break;

    case RFNV_WCDMA_800:
      nv_tbl_ptr = nv_tables[RFNV_WCDMA_TABLE_800];
      break;

    case RFNV_WCDMA_1500:
      nv_tbl_ptr = nv_tables[RFNV_WCDMA_TABLE_BC11_1500];
      break;      

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"Invalid band=%d. Cannot initialize WCDMA NV table", band);
  }
  /*lint +e787 */

  return nv_tbl_ptr;
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read WCDMA RF NV items out of NV memory and into
  rfnv_wcdma_nv_tbl_type containers.

  @details

  @param task_ptr  : Calling task's TCB pointer
  @param task_wait_sig : Task signal to wait for when reading NV.
  @param task_wait_handler : Task's wait function to be called when reading NV

  @retval
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

*/
rfcommon_nv_status_type rfwcdma_nv_retrive_items(
  rex_tcb_type *task_ptr,
  rex_sigs_type task_wait_sig,
  void (*task_wait_handler)( rex_sigs_type ))
{
  rfcommon_nv_status_type status = RFCOMMON_NV_READ_SUCCESS;
  rfnv_wcdma_table_type nv_band_ctr;
  rfcom_wcdma_band_type band;
  uint16 nv_item_ctr;
  nv_stat_enum_type nv_status;
  rfnv_item_type* nv_item_buffer = NULL;
  rfnv_item_size_type returned_size;
  rfnv_item_id_enum_type rfnv_item;
  boolean pa_static_nv_read_status;

  // We need to initialize the various NV item lists the first time.
  if (FALSE == rfnv_wcdma_nv_lists_initialized)
  {
    // Init the copy function pointers list.
    rfnv_wcdma_init_nv_copy_function_list();

    // Init the NV enum lists for each band.
    rfnv_wcdma_1500_init_nv_item_list();
    rfnv_wcdma_1800_init_nv_item_list();
    rfnv_wcdma_1900_init_nv_item_list();
    rfnv_wcdma_2100_init_nv_item_list();
    rfnv_wcdma_800_init_nv_item_list();
    rfnv_wcdma_900_init_nv_item_list();
    rfnv_wcdma_bc4_init_nv_item_list();

    rfnv_wcdma_nv_lists_initialized = TRUE;
  }

  // Allocate memory from modem heap for RFNV items
  nv_item_buffer = modem_mem_alloc(sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA);
  if(NULL == nv_item_buffer)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "rfwcdma_nv_retrive_items: Could not allocate NV buffer memory on heap!", 0 );
    return RFCOMMON_NV_READ_FAILED;
  }

  /* Loop through all of the possible NV bands. */
  for (nv_band_ctr=RFNV_WCDMA_TABLE_IMT_2100; nv_band_ctr<RFNV_WCDMA_NUM_TABLES; nv_band_ctr++)
  {
    /* Determine if the band is supported before reading NVs. */
    band = rfnv_wcdma_get_band_for_nv_table(nv_band_ctr);

    if (band != RFCOM_BAND_INVALID)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfwcdma_nv_retrieve_items: band %d supported", nv_band_ctr);

      // Allocate memory for the NV container from modem heap if we haven't already.
      if (NULL == nv_tables[nv_band_ctr])
      {
        nv_tables[nv_band_ctr] = modem_mem_alloc( sizeof(rfnv_wcdma_nv_tbl_type), MODEM_MEM_CLIENT_RFA );

        // Check that memory allocation succeeded.
        if (NULL == nv_tables[nv_band_ctr])
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
                "rfwcdma_nv_retrive_items: Could not allocate NV memory on heap! band %d", band );
          return RFCOMMON_NV_READ_FAILED;
        }
        else
        {
          // Initialize the table with default values.
          memscpy(nv_tables[nv_band_ctr], sizeof(rfnv_wcdma_nv_tbl_type), &rfnv_wcdma_default_nv_tbl,sizeof(rfnv_wcdma_nv_tbl_type));
        }
      }

      rfnv_wcdma_set_rx_cal_data_to_default(nv_tables[nv_band_ctr]);

      rfnv_wcdma_set_rx_static_data_to_default(nv_tables[nv_band_ctr]);
      
      // Iterate through the NV item list.
      for (nv_item_ctr=0; nv_item_ctr<RFNV_WCDMA_ITEMS_MAX; nv_item_ctr++)
      {
        rfnv_item = (nv_item_lists[nv_band_ctr]->nv_items_array[nv_item_ctr]).rfnv_item_id;

        // Check whether the nv_item is the PA static NV item, the call the common PA static function
        if ( TRUE == rfnv_wcdma_is_pa_static_nv(rfnv_item) ) 
        {
          /* Get PA static NV data from EFS */
          pa_static_nv_read_status = 
             rfcommon_load_pa_static_nv ( RFCOM_WCDMA_MODE,
                                          nv_item_buffer,
                                          rfnv_item,
                                          task_ptr,
                                          task_wait_sig,
                                          task_wait_handler );
          /* Validate NV read */
          if ( FALSE == pa_static_nv_read_status )
          {
            MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "NV read failed on critical WCDMA RF NV item %d", rfnv_item );
			/* Do not set the status to fail if one of the PA static failed to read. The is the possible other */
            // status = RFCOMMON_NV_READ_FAILED;
          }/* if ( FALSE == pa_static_nv_read_status ) */

          else
          {
            /* Copy from local buffer into RF NV table */        
            (nv_copy_funcs.copy_funcs_array[nv_item_ctr])(nv_tables[nv_band_ctr],nv_item_buffer,band);
          }/* else to if ( FALSE == pa_static_nv_read_status ) */

        }/* ( TRUE == rfnv_wcdma_is_pa_static_nv ( rfnv_item ) ) */
        
        // Only read the items we're actually interested in.
        else if ( rfnv_item < RFNV_ITEMS_ENUM_MAX)
        {
          // Need to set the return size each time. It will get changed by the call to
          // rfcommon_rfnv_get_item.
          returned_size = sizeof(rfnv_item_type);

          // Copy the item in nv_item_list[band]->array[nv_item_ctr]
          nv_status = rfcommon_rfnv_get_item( rfnv_item,
                                              nv_item_buffer,
                                              &returned_size,
                                              task_ptr,
                                              task_wait_sig,
                                              task_wait_handler);
                                              
          // Copy the value into the NV container if the NV read was successful.
          if (nv_status == NV_DONE_S)
          {
            (nv_copy_funcs.copy_funcs_array[nv_item_ctr])(nv_tables[nv_band_ctr],nv_item_buffer,band);
          }
          else if (nv_status == NV_NOTACTIVE_S)
          {
            // Check if this NV item was critical.
            if ((nv_item_lists[nv_band_ctr]->nv_items_array[nv_item_ctr]).rfnv_item_attrib & NV_ITEM_ATTRIB_CRTCL_BMSK)
            {
              MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Critical WCDMA RF NV item %d is not available", rfnv_item );

              // Return the reason for failure.
              status = RFCOMMON_NV_INACTIVE_ITEM;
            }
          }
          else
          {
            // Check if this NV item was critical.
            if ((nv_item_lists[nv_band_ctr]->nv_items_array[nv_item_ctr]).rfnv_item_attrib & NV_ITEM_ATTRIB_CRTCL_BMSK)
            {
              MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "NV read failed on critical WCDMA RF NV item %d", rfnv_item );

              // Return the reason for failure.
              status = RFCOMMON_NV_READ_FAILED;
            }
          }
        } // if nv_item<RFNV_ITEMS_ENUM_MAX
      } // for nv_items
    } // if (band != RFCOM_BAND_INVALID)
  } // for bands

  /* Get NV status of ICI coefficients */
  rfwcdma_msm_get_ici_nv_status(task_ptr, task_wait_sig, task_wait_handler);

  // Free RFNV buffer
  modem_mem_free(nv_item_buffer, MODEM_MEM_CLIENT_RFA);
  nv_item_buffer = NULL;

  return status;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function converts rfcom_wcdma_band_type to rfnv_wcdma_supported_mode_type.

  @param band rfcom_wcdma_band_type WCDMA band

  @return rfnv_wcdma_supported_mode_type
*/
rfnv_wcdma_supported_mode_type rfnv_wcdma_convert_band_rfcom_to_rfnv (rfcom_wcdma_band_type band)
{
  rfnv_wcdma_supported_mode_type w_band = RFNV_WCDMA_SUPPORTED_MODE_NUM;
  
  switch (band)
  {
    case RFCOM_BAND_IMT:
      w_band = RFNV_WCDMA_2100;
      break;

    case RFCOM_BAND_1900:
      w_band = RFNV_WCDMA_1900;
      break;    
      
    case RFCOM_BAND_BC4:
      w_band = RFNV_WCDMA_BC4;
      break;

	  case RFCOM_BAND_BC19:
    case RFCOM_BAND_800:
      w_band = RFNV_WCDMA_800;
      break;

    case RFCOM_BAND_BC8:
      w_band = RFNV_WCDMA_900;
      break;

    case RFCOM_BAND_BC3:
    case RFCOM_BAND_BC9:
      w_band = RFNV_WCDMA_1800;
      break;

    case RFCOM_BAND_BC11:
      w_band = RFNV_WCDMA_1500;
      break;
	
    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band in rfnv_wcdma_convert_band_rfcom_to_rfnv, %d", band);
      break;
  }

  return (w_band);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the Tx multilin global structure and NV Id based on Tx Int Cal band type

  @details
  This function gets the corresponding global TxLin data structure.
  It also returns the NV Id for the multilin Tx item for the corresponding band
 
  @param
  band - The band for which Tx Int Cal was done and results are returned
 
  @param
  device - The device for which Tx Int Cal was done and results are
           returned
  @param
  item_code - Argument for filling up the NV ID of multi lin cal data
 
  @param
  tx_band_cal_data - Argument to hold the address of the global Multi Lin Tx
                     structure
 
  @return
    TRUE or FALSE
*/
#if 0
boolean rfnv_wcdma_get_txlin_cal_data 
(
  uint8 band,
  uint8 device,
  rfnv_item_id_enum_type* item_code,
  tx_band_cal_type **tx_band_cal_data
)
{
  rfwcdma_int_cal_bc_type wcdma_band;
  rfcom_wcdma_band_type rfcom_wcdma_band;

  wcdma_band = (rfwcdma_int_cal_bc_type)(band);
  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Tx Internal Cal : FTM WCDMA Band B%d", 
        wcdma_band);

  rfcom_wcdma_band = convert_wcdma_int_cal_band_to_rfcom_band(wcdma_band);
  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Tx Internal Cal : RFCOM WCDMA Band B%d", 
        rfcom_wcdma_band);

  if( rfcom_wcdma_band == RFCOM_NUM_WCDMA_BANDS )
  {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Incorrect bit enabled for WCDMA "
                                          "Int Dev Cal", 0);
      return FALSE;
  }

  *tx_band_cal_data = rfwcdma_core_txlin_get_band_data( rfcom_wcdma_band );
  *item_code = rfwcdma_core_txlin_get_band_nvid( rfcom_wcdma_band );
  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Tx Internal Cal, Item Code : %d", *item_code);
  
  return TRUE;
}
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return function pointer for converting frequency to channel

  @details
  This function returns the handler for converting frequency to channel
  for WCDMA
 
  @param
    band - The band for which Tx Int Cal is being performed
 
  @return
    rfcommon_core_convert_chan_handler_type - freq to channel conversion
                                              handler
*/
rfcommon_core_convert_chan_handler_type rfnv_wcdma_get_tx_freq_to_chan_handler 
(
  uint8 band
)
{
  return ((rfcommon_core_convert_chan_handler_type)(rfwcdma_core_get_ul_arfcn_from_freq));
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return band and path Specfic Calibration NV Table

  @details
  This function is the only accessor function to return the NV table for
  any RF Calibration NV items.

  @param band
  Band for which the NV table is requested
 
  @return
  NV table corresponding to the band and nv_path
*/
rfcommon_core_convert_chan_handler_type
rfnv_wcdma_get_chan_to_tx_freq_handler
(
  uint8 band
)
{
  return ((rfcommon_core_convert_chan_handler_type)rfwcdma_core_get_freq_from_ul_arfcn);
} /* rfnv_wcdma_get_chan_to_tx_freq_handler */


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the HDET Vs AGC NV ID for
  the band requested

  @details

  @param band WCDMA band

  @return rfnv_item_id_enum_type HDET Vs AGC NV ID

*/
boolean 
rfnv_wcdma_get_hdet_vs_agc_nvid
( 
  uint8 band,
  uint8 device,
  rfnv_item_id_enum_type* item_code
)
{
  rfwcdma_int_cal_bc_type wcdma_band;
  rfcom_wcdma_band_type rfcom_wcdma_band;

  wcdma_band = (rfwcdma_int_cal_bc_type)(band);

  rfcom_wcdma_band = convert_wcdma_int_cal_band_to_rfcom_band(wcdma_band);

  if( rfcom_wcdma_band == RFCOM_NUM_WCDMA_BANDS )
  {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Incorrect bit enabled for WCDMA "
                                          "Int Dev Cal", 0);
      return FALSE;
  }
  else
  {
    item_code[0] = rfnv_wcdma_hdet_vs_agc_nv_list[rfcom_wcdma_band].hdet_vs_agc_nv;
    item_code[1] = RFNV_ITEMS_ENUM_MAX;
    item_code[2] = RFNV_ITEMS_ENUM_MAX;
  }
  
  return TRUE;
} /*rfnv_wcdma_get_hdet_vs_agc_nvid*/

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load Legacy Lin vs Temp offset data into common Tx Lin data structure type
  for all bands

  @details

  @param nv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param rfnv_item_ptr
  Pointer to the NV buffer where data loaded from NV currently is stored
 
  @param pa_state
  PA state for which Lin vs temp legacy must be loaded
*/
void 
rfnv_wcdma_load_legacy_lin_vs_temp_offset
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr,
  rfnv_item_type         *rfnv_item_ptr,
  uint8 pa_state
)
{
  nv_item_type* item_ptr = (nv_item_type*) rfnv_item_ptr;
  uint16 temp_index;

  if ( nv_tbl_ptr == NULL )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,
         "rfnv_wcdma_load_legacy_lin_vs_temp_offset: "
         "Null NV table pointer");
    return;
  }

  /* Set legacy NV flag to TRUE */
  nv_tbl_ptr->lin_temp_comp_data.is_legacy_temp_comp_nv = TRUE;

  for ( temp_index = 0; temp_index < NV_TEMP_TABLE_SIZ; temp_index++ )
  {
    nv_tbl_ptr->
        lin_temp_comp_data.legacy_lin_vs_temp[pa_state].offset[temp_index] = 
      (int16)item_ptr->wcdma_tx_lin_vs_temp_0[temp_index];
  }

  /* Also initialize the equidistant therm bins data in the NV 
  data structure */
  rfnv_wcdma_load_equidistant_therm_bins_data( nv_tbl_ptr );

}/* rfnv_wcdma_load_legacy_lin_vs_temp_offset */


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load Legacy Lin vs Temp vs Freq delta data into common Tx Lin data
  structure type for all bands

  @details

  @param nv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param rfnv_item_ptr
  Pointer to the NV buffer where data loaded from NV currently is stored
 
  @param pa_state
  PA state for which Lin vs temp legacy must be loaded
*/
void 
rfnv_wcdma_load_legacy_lin_vs_temp_vs_freq_delta
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr,
  rfnv_item_type         *rfnv_item_ptr,
  uint8 pa_state
)
{
  nv_item_type* item_ptr = (nv_item_type*) rfnv_item_ptr;
  int8 interim_comp_vs_temp_vs_freq[NV_FREQ_TABLE_SIZ]
                                   [NV_TEMP_TABLE_SIZ];
  uint16 temp_index, freq_index;

  if ( nv_tbl_ptr == NULL )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,
         "rfnv_wcdma_load_legacy_lin_vs_temp_vs_freq_delta: "
         "Null NV table pointer");
    return;
  }

  /* Set legacy NV flag to TRUE */
  nv_tbl_ptr->lin_temp_comp_data.is_legacy_temp_comp_nv = TRUE;

  memscpy(interim_comp_vs_temp_vs_freq, (sizeof(int8) * NV_TEMP_TABLE_SIZ * NV_FREQ_TABLE_SIZ),
         (int8*)item_ptr,
         (sizeof(int8) * NV_TEMP_TABLE_SIZ * NV_FREQ_TABLE_SIZ));

  for ( temp_index = 0; temp_index < NV_TEMP_TABLE_SIZ; temp_index++ )
  {
    for ( freq_index = 0; freq_index < NV_FREQ_TABLE_SIZ; freq_index++ )
    {
      nv_tbl_ptr->
        lin_temp_comp_data.legacy_lin_vs_temp[pa_state].
                              temp_vs_freq_delta[temp_index][freq_index] =
                  (int16)interim_comp_vs_temp_vs_freq[freq_index][temp_index];
    }
  }

}/* rfnv_wcdma_load_legacy_lin_vs_temp_vs_freq_delta */

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Tx characterization channel list, for use with characterized
  Lin vs Temp vs Freq data

  @details

  @param nv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param rfnv_item_ptr
  Pointer to the NV buffer where data loaded from NV currently is stored
 
  @param band
  Band for the Tx channel to frequency conversion functionality
*/
void 
rfnv_wcdma_load_lin_vs_temp_tx_char_chan
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr,
  rfnv_item_type         *rfnv_item_ptr,
  rfcom_wcdma_band_type  band
)
{
  nv_item_type* item_ptr = (nv_item_type*) rfnv_item_ptr;
  uint8 freq_index;

  if ( nv_tbl_ptr == NULL )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,
         "rfnv_wcdma_load_lin_vs_temp_tx_char_chan: "
         "Null NV table pointer");
    return;
  }

  for ( freq_index = 0; freq_index < NV_FREQ_TABLE_SIZ; freq_index++ )
  {
    /* Populate the Lin vs Temp static NV data structure for 
    use with characterization data */
    if ( item_ptr->wcdma_1500_tx_cal_chan[freq_index] != 0 )
    {
      nv_tbl_ptr->lin_temp_comp_data.tx_char_chan[freq_index] = 
                         (uint16)item_ptr->wcdma_1500_tx_cal_chan[freq_index];
      nv_tbl_ptr->lin_temp_comp_data.tx_char_freq[freq_index] =
        rfwcdma_core_get_freq_from_uarfcn( 
                        band,
                        (uint16)item_ptr->wcdma_1500_tx_cal_chan[freq_index],
                        RFWCDMA_CORE_UARFCN_UL );
    }
    else
    {
      /* At the first channel entry with '0', assume that the
      channel list is terminated and break out of the loop */
      break;
    }
  }
  nv_tbl_ptr->lin_temp_comp_data.tx_char_chan_size = freq_index;

}/* rfnv_wcdma_load_lin_vs_temp_tx_char_chan */

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Tx characterization channel list, for use with characterized
  Lin vs Temp vs Freq data

  @details

  @param nv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
*/
void
rfnv_wcdma_load_equidistant_therm_bins_data
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr
)
{
  uint8 therm_bin;

  /* Initialize the therm bins to use equally spaced bins */
  for ( therm_bin = 0; therm_bin < NV_TEMP_TABLE_SIZ; therm_bin++ )
  {
    nv_tbl_ptr->lin_temp_comp_data.tx_therm_bins[therm_bin] = 
                                 therm_bin * RFWCDMA_NV_SINGLE_TEMP_BIN_SIZE;
  }

  return;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load Lin vs Temp vs Freq Pin and Pout NV data offset data into common Tx
  Lin data structure type for all bands

  @details

  @param nv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param rfnv_item_ptr
  Pointer to the NV buffer where data loaded from NV currently is stored
 
  @param is_pin
  Flag to indicate if Pin data or not. If not, the data is copied to Pout
*/
void 
rfnv_wcdma_load_lin_vs_temp_vs_freq
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr,
  boolean                is_pin
)
{
  uint8 pa_index_ctr = 0;
  uint16 temp_index, freq_index;

  /* Initialize the temp comp data structure */
  nv_tbl_ptr->lin_temp_comp_data.has_pin_pout_temp_comp_nv = TRUE;

  if ( is_pin )
  {
    for (pa_index_ctr=0; pa_index_ctr<RFCOM_TXLIN_NUM_LINEARIZERS; pa_index_ctr++)
    {
      for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
        {
          nv_tbl_ptr->lin_temp_comp_data.lin_vs_temp_vs_freq.pin[pa_index_ctr][temp_index][freq_index] = 
            nv_tbl_ptr->pa_static.tx_lin_pin[temp_index][freq_index][pa_index_ctr];
        }
      }
    }
  }
  else
  {
    for (pa_index_ctr=0; pa_index_ctr<RFCOM_TXLIN_NUM_LINEARIZERS; pa_index_ctr++)
    {
      for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
        {
          nv_tbl_ptr->lin_temp_comp_data.lin_vs_temp_vs_freq.pout[pa_index_ctr][temp_index][freq_index] = 
            nv_tbl_ptr->pa_static.tx_lin_pout[temp_index][freq_index][pa_index_ctr];
        }
      }
    }  
  }

  /* Also initialize the equidistant therm bins data in the NV 
  data structure */
  rfnv_wcdma_load_equidistant_therm_bins_data( nv_tbl_ptr );

  return;
}/* rfnv_wcdma_load_lin_vs_temp_vs_freq */

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Copy the Passed the legacy NV value into the new NV structure 

  @details
  Legacy NV structure will be deprecated going forward and all legacy NVs are copied into new NV

  @param rx_chain
   Rx chain used
  
  @param carr_id
   Carrier used (SC/DC/3C)
 
  @param element
  Type of element inside the NV structure

  @param index
   Index used to find the placeholder inside the new NV structure

  @param val
  Value of the NV to be copied over
  
  @param rfnv_item_ptr
  Pointer to the RFNV item 
  
*/
void rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv
(
  rfcom_wcdma_band_type band,
  rfm_device_enum_type device_id,
  rfcom_multi_carrier_id_type carr_id,
  rfwcdma_nv_rx_dynamic_element_type element,
  uint8 index,
  int16 val,
  rfnv_wcdma_nv_tbl_type *rfnv_item_ptr
)
{
  static boolean init_flag[RFCOM_NUM_WCDMA_BANDS]={FALSE};
  rfwcdma_nv_rx_dynamic_payload_v2_type *rx_data_ptr = NULL ;
  rfwcdma_nv_rx_dynamic_data_type *rx_dyn_data_ptr = NULL;
  rfm_device_enum_type db_device_id = RFM_DEVICE_2;
  
  /* Access the NV structure through the payload. Assume its V2 be default */
  rx_dyn_data_ptr = &(rfnv_item_ptr->rx_dyn_data);
  rx_data_ptr = &(rx_dyn_data_ptr->payload.rx_dyn_v2);

  /* Set the version to 2 */
  rx_dyn_data_ptr->rx_dyn_variant_marker.version = RFWCDMA_NV_RX_DYNAMIC_DATA_V2;

  if(!init_flag[band]) 
  {
   rfnv_wcdma_set_rx_cal_data_to_default(rfnv_item_ptr);
   init_flag[band] = TRUE;
  }

  if (device_id == RFM_DEVICE_0)
  {
    db_device_id = RFM_DEVICE_2;
  }
  else if (device_id == RFM_DEVICE_1)
  {
    db_device_id = RFM_DEVICE_3;
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rfnv_wcdma_copy_rx_static_data_from_legacy_nv: "
          "device %d not supported", device_id);
    return;
  }

  switch(element)
  {
    case RX_CAL_CHAN_LIST:
      rx_data_ptr->rx_cal_chan[device_id][index] = val;
      rx_data_ptr->rx_cal_chan[db_device_id][index] = val;
      break;

    case VGA_OFFSET:
      rx_data_ptr->rx_offsets[device_id][carr_id].vga_offset = val;
      rx_data_ptr->rx_offsets[db_device_id][carr_id].vga_offset = val;
      break;

    case LNA_OFFSET:
      rx_data_ptr->rx_offsets[device_id][carr_id].lna_offsets[index] = val;
      rx_data_ptr->rx_offsets[db_device_id][carr_id].lna_offsets[index] = val;
      break;

    case VGA_OFFSET_VS_FREQ:
      rx_data_ptr->freq_offset[device_id][index].vga_gain_offset_vs_freq = val;
      rx_data_ptr->freq_offset[db_device_id][index].vga_gain_offset_vs_freq = val;
      break;

    case LNA_OFFSET_VS_FREQ_1:
      rx_data_ptr->freq_offset[device_id][index].lna_offset_vs_freq_1 = val;
      rx_data_ptr->freq_offset[db_device_id][index].lna_offset_vs_freq_1 = val;
      break;

    case LNA_OFFSET_VS_FREQ_2:
      rx_data_ptr->freq_offset[device_id][index].lna_offset_vs_freq_2 =  val;
      rx_data_ptr->freq_offset[db_device_id][index].lna_offset_vs_freq_2 =  val;
      break;

    case LNA_OFFSET_VS_FREQ_3:
      rx_data_ptr->freq_offset[device_id][index].lna_offset_vs_freq_3 = val;
      rx_data_ptr->freq_offset[db_device_id][index].lna_offset_vs_freq_3 = val;
      break;

    case LNA_OFFSET_VS_FREQ_4:
      rx_data_ptr->freq_offset[device_id][index].lna_offset_vs_freq_4 = val;
      rx_data_ptr->freq_offset[db_device_id][index].lna_offset_vs_freq_4 = val;
      break;
 
    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv: Invalid element type %d",element);
      break;
  }
}/* rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv */
   
/*--------------------------------------------------------------------------*/
/*!
  @brief
  Copy the Passed the legacy NV value into the new NV structure 

  @details
  Legacy NV structure will be deprecated going forward and all legacy NVs are copied into new NV

  @param rx_chain
   Rx chain used
  
  @param carr_id
   Carrier used (SC/DC/3C)
 
  @param element
  Type of element inside the NV structure

  @param index
   Index used to find the placeholder inside the new NV structure

  @param val
  Value of the NV to be copied over
  
  @param rfnv_item_ptr
  Pointer to the RFNV item 
  
*/
void rfnv_wcdma_copy_rx_static_data_from_legacy_nv
(
  rfcom_wcdma_band_type band,
  rfm_device_enum_type device_id,
  rfcom_multi_carrier_id_type carr_id,
  rfwcdma_nv_rx_static_element_type element,
  uint8 index,
  int16 val,
  rfnv_wcdma_nv_tbl_type *rfnv_item_ptr
)
{
  static boolean init_flag[RFCOM_NUM_WCDMA_BANDS]={FALSE};
  rfwcdma_nv_rx_static_data_type *rx_stat_data = NULL;
  rfwcdma_nv_rx_static_payload_v2_type *rx_data_ptr = NULL ;
  rfm_device_enum_type db_device_id = RFM_DEVICE_2;
  
  /* Access the NV structure through the payload. Assume its V2 be default */
  rx_stat_data = &(rfnv_item_ptr->rx_static_data);
  rx_data_ptr = &(rx_stat_data->payload.rx_stat_v2);

  /* Set the version to 2 */
  rx_stat_data->rx_stat_variant_marker.version = RFWCDMA_NV_RX_STATIC_DATA_V2;

  if(!init_flag[band])
  {
    rfnv_wcdma_set_rx_static_data_to_default(rfnv_item_ptr);
    init_flag[band] = TRUE;
  }

  if (device_id == RFM_DEVICE_0)
  {
    db_device_id = RFM_DEVICE_2;
  }
  else if (device_id == RFM_DEVICE_1)
  {
    db_device_id = RFM_DEVICE_3;
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rfnv_wcdma_copy_rx_static_data_from_legacy_nv: "
          "device %d not supported", device_id);
    return;
  }

  switch(element)
  {
    case LNA_RISE:
      rx_data_ptr->rx_lna_swpts[device_id][carr_id].lna_rise[index-1] = val;
      rx_data_ptr->rx_lna_swpts[db_device_id][carr_id].lna_rise[index-1] = val;
      break;

    case LNA_FALL:
      rx_data_ptr->rx_lna_swpts[device_id][carr_id].lna_fall[index-1] = val;
      rx_data_ptr->rx_lna_swpts[db_device_id][carr_id].lna_fall[index-1] = val;
      break;
 
    case LNA_HS_RISE:
      rx_data_ptr->rx_lna_swpts[device_id][carr_id].lna_hs_rise[index-1] = val;
      rx_data_ptr->rx_lna_swpts[db_device_id][carr_id].lna_hs_rise[index-1] = val;
      break;

    case LNA_HS_FALL:
      rx_data_ptr->rx_lna_swpts[device_id][carr_id].lna_hs_fall[index-1] = val;
      rx_data_ptr->rx_lna_swpts[db_device_id][carr_id].lna_hs_fall[index-1] = val;
      break;
  
    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "rfnv_wcdma_copy_rx_static_data_from_legacy_nv: Invalid element type %d",element);
      break;
  }
}/* rfnv_wcdma_copy_rx_static_data_from_legacy_nv */

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Copy the Passed the legacy NV value into the new NV structure 

  @details
  Legacy NV structure will be deprecated going forward and all legacy NVs are copied into new NV

  @param rx_chain
   Rx chain used
  
  @param carr_id
   Carrier used (SC/DC/3C)
 
  @param element
  Type of element inside the NV structure

  @param index
   Index used to find the placeholder inside the new NV structure

  @param val
  Value of the NV to be copied over
  
  @param rfnv_item_ptr
  Pointer to the RFNV item 

  @return
  NV value correspdonding to the chain and carrier
*/
int16 rfnv_wcdma_get_info_from_rx_dynamic_data
(
  rfm_device_enum_type device_id,
  rfcom_multi_carrier_hspa_id_type carr_id,
  rfwcdma_nv_rx_dynamic_element_type element,
  uint8 index,
  rfwcdma_nv_rx_dynamic_data_type *rfnv_ptr
)
{
  rfwcdma_nv_rx_dynamic_payload_v2_type *rx_data_ptr = NULL;
  int16 ret_val = 0;

  if(rfnv_ptr->rx_dyn_variant_marker.version != RFWCDMA_NV_RX_DYNAMIC_DATA_V2)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "rfnv_wcdma_get_info_from_rx_dynamic_data: Version%d is not supported",
          rfnv_ptr->rx_dyn_variant_marker.version );
  }
  else
  {
    rx_data_ptr = &(rfnv_ptr->payload.rx_dyn_v2);
  
    switch(element)
    {
      case RX_CAL_CHAN_LIST:
        ret_val = rx_data_ptr->rx_cal_chan[device_id][index];
        break;

      case VGA_OFFSET:
        ret_val = rx_data_ptr->rx_offsets[device_id][carr_id].vga_offset;
        break;

      case LNA_OFFSET:
        ret_val = rx_data_ptr->rx_offsets[device_id][carr_id].lna_offsets[index];
        break;

      case VGA_OFFSET_VS_FREQ:
        ret_val = rx_data_ptr->freq_offset[device_id][index].vga_gain_offset_vs_freq;
        break;

      case LNA_OFFSET_VS_FREQ_1:
        ret_val = rx_data_ptr->freq_offset[device_id][index].lna_offset_vs_freq_1;
        break;

      case LNA_OFFSET_VS_FREQ_2:
        ret_val = rx_data_ptr->freq_offset[device_id][index].lna_offset_vs_freq_2;
        break;

      case LNA_OFFSET_VS_FREQ_3:
        ret_val = rx_data_ptr->freq_offset[device_id][index].lna_offset_vs_freq_3;
        break;

      case LNA_OFFSET_VS_FREQ_4:
        ret_val = rx_data_ptr->freq_offset[device_id][index].lna_offset_vs_freq_4;
        break;
    }
  }
  return ret_val;
}/* rfnv_wcdma_get_info_from_rx_dynamic_data */

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Get the value from the NV structure

  @details
  Legacy NV structure will be deprecated going forward and all legacy NVs are copied into new NV

  @param device_id
   RFM Device index
  
  @param carr_id
   Carrier used (SC/DC/3C)
 
  @param element
  Type of element inside the NV structure

  @param index
   Index used to find the placeholder inside the new NV structure

  @param val
  Value of the NV to be copied over
  
  @param rfnv_item_ptr
  Pointer to the RFNV item 

  @return
  NV value correspdonding to the chain and carrier
*/

int16 rfnv_wcdma_get_info_from_rx_static_data
(
  rfm_device_enum_type device_id,
  rfcom_multi_carrier_hspa_id_type carr_id,
  rfwcdma_nv_rx_static_element_type element,
  uint8 index,
  rfwcdma_nv_rx_static_data_type *rfnv_ptr
)
{
  rfwcdma_nv_rx_static_payload_v2_type *rx_data_ptr = NULL;
  int16 ret_val = 0;
  
  if(rfnv_ptr->rx_stat_variant_marker.version != RFWCDMA_NV_RX_STATIC_DATA_V2)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "rfnv_wcdma_get_info_from_rx_static_data: Version%d is not supported",
          rfnv_ptr->rx_stat_variant_marker.version );
  }
  else
  {
    rx_data_ptr = &(rfnv_ptr->payload.rx_stat_v2) ;

    switch(element)
    {
      case LNA_RISE:
        ret_val = rx_data_ptr->rx_lna_swpts[device_id][carr_id].lna_rise[index];
        break;

      case LNA_FALL:
        ret_val = rx_data_ptr->rx_lna_swpts[device_id][carr_id].lna_fall[index];
        break;
 
      case LNA_HS_RISE:
        ret_val = rx_data_ptr->rx_lna_swpts[device_id][carr_id].lna_hs_rise[index];
        break;

      case LNA_HS_FALL:
        ret_val = rx_data_ptr->rx_lna_swpts[device_id][carr_id].lna_hs_fall[index];
        break;
    }
  }

  return(ret_val);
}/* rfnv_wcdma_get_info_from_rx_static_data */

/*--------------------------------------------------------------------------*/
/*!
  @brief
   Loading the Rx static NV into the internal structure
   
  @details
    LNA rise/fall swpts for R99/HS for each carrier and chain are loaded
	
  @param rfnv_ptr
   Pointer to the internal NV structure
  
  @param item_ptr
   Pointer to the RFNV
  
  @param band
   Band to which the NV belongs to 
 
*/
void rfnv_wcdma_load_rx_static_data_nv
(
  rfnv_wcdma_nv_tbl_type *rfnv_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band,
  uint8 input_chain_id
)
{
  /* The TYPE ID of the current element */
  uint16 type_id = 0;

  /* Keeps track of the running total of number of bytes already parsed */
  uint32 num_bytes_parsed = 0;	

  /* Contains the number of bytes for an element in NV */
  uint32 nv_element_size = 0;

  /* Keeps track of the chain_id */
  uint8 chain_id = 0;

  /* Keeps track of the carr_id */
  uint8 carr_id = 0;

  /* Keeps track of the hs_enable */
  uint8 hs_config = 0;

  /* Keeps track of default values per band */
  static boolean init_flag[RFCOM_NUM_WCDMA_BANDS]={FALSE};
  
  rfwcdma_nv_rx_static_payload_v2_type *rx_stat = NULL;

  /* used for item_ptr */
  byte *nv_item_ptr = (byte *)item_ptr;

  int i =0;

  if(rfnv_ptr == NULL || item_ptr ==  NULL )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,
         "rfnv_wcdma_load_rx_static_data_nv: "
         "NULL NV table pointer or NULL item ptr");
    return;
  }
  
  /* Read Version */
  nv_element_size = sizeof(uint16);
  memscpy((void*)&rfnv_ptr->rx_static_data.rx_stat_variant_marker.version ,
  	                                     nv_element_size,nv_item_ptr,nv_element_size);
  num_bytes_parsed += nv_element_size;
  nv_item_ptr += nv_element_size;

  if(!init_flag[band])
  {
    /* Memset all data to 0 */
    memset((void*)&rfnv_ptr->rx_static_data,0,sizeof(rfwcdma_nv_rx_static_data_type));
    /* Set NV to default values before copying */
    rfnv_wcdma_set_rx_static_data_to_default(rfnv_ptr);
    init_flag[band] = TRUE;
  }
  
  /* Read number of elements */
  memscpy((void*)&rfnv_ptr->rx_static_data.rx_stat_variant_marker.num_elements,
                                         nv_element_size,nv_item_ptr,nv_element_size);
  num_bytes_parsed += nv_element_size;
  nv_item_ptr      += nv_element_size;

  for(i=0;i<rfnv_ptr->rx_static_data.rx_stat_variant_marker.num_elements;i++)
  {
    nv_element_size = sizeof(uint16);
	  /* Read type_id */
	  memscpy((void*)&type_id ,nv_element_size,nv_item_ptr,nv_element_size);
	  num_bytes_parsed += nv_element_size;
	  nv_item_ptr 	 += nv_element_size;

    switch (type_id)
    {
      case RFNV_DATA_TYPE_LNA_SWPTS_V1:
        rx_stat = (rfwcdma_nv_rx_static_payload_v2_type *)&rfnv_ptr->rx_static_data.payload;

        /* Read chain_id */
	      nv_element_size = sizeof(chain_id);
	      memscpy((void*)&chain_id ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;

        if(chain_id >= RF_PATH_4)
        {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_static_data_nv: "
                       "chain_id= %d is not within range(>= RF_PATH_4)",chain_id);
          return;
        }
     
        /* Read carr_id */
	      nv_element_size = sizeof(carr_id);
	      memscpy((void*)&carr_id ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;

		    if(carr_id >= RFCOM_MULTI_CARRIER_ID__CUMULATIVE)
		    {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_static_data_nv: "
                  "carr_id= %d is not within range(>= RFCOM_MULTI_CARRIER_ID__CUMULATIVE)",carr_id);
          return;
        }

		    /* Copy LNA Rise SP */
		    nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_rise[0])*RFWCDMA_NUM_LNA_STATES;
		    memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_rise,
		    	                           nv_element_size,nv_item_ptr,nv_element_size);
		    num_bytes_parsed += nv_element_size;
        nv_item_ptr 	 += nv_element_size;

		    /* Copy LNA Fall SP */
		    nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_fall[0])*RFWCDMA_NUM_LNA_STATES;
		    memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_fall ,
		                               nv_element_size,nv_item_ptr,nv_element_size);
        num_bytes_parsed += nv_element_size;
        nv_item_ptr 	 += nv_element_size;

		    /* Copy LNA HS Rise SP */
		    nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_rise[0])*RFWCDMA_NUM_LNA_STATES;
		    memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_rise ,
		    	                           nv_element_size,nv_item_ptr,nv_element_size);
		    num_bytes_parsed += nv_element_size;
        nv_item_ptr 	 += nv_element_size;

        /* Copy LNA HS FALL SP */
       	nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_fall[0])*RFWCDMA_NUM_LNA_STATES;
        memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_fall ,
		    	                           nv_element_size,nv_item_ptr,nv_element_size);
        num_bytes_parsed += nv_element_size;
        nv_item_ptr 	 += nv_element_size;
        break;

      case RFNV_DATA_TYPE_LNA_SWPTS_V2:
        rx_stat = (rfwcdma_nv_rx_static_payload_v2_type *)&rfnv_ptr->rx_static_data.payload;

        /* Read chain_id */
	      nv_element_size = sizeof(chain_id);
	      memscpy((void*)&chain_id ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;

        if(chain_id >= RF_PATH_4)
        {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_static_data_nv: "
                       "chain_id= %d is not within range(>= RF_PATH_4)",chain_id);
          return;
        }
     
        /* Read carr_id */
	      nv_element_size = sizeof(carr_id);
	      memscpy((void*)&carr_id ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;

		    if(carr_id >= RFCOM_MULTI_CARRIER_ID__CUMULATIVE)
		    {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_static_data_nv: "
                  "carr_id= %d is not within range(>= RFCOM_MULTI_CARRIER_ID__CUMULATIVE)",carr_id);
          return;
        }

        /* Read hs_config */
	      nv_element_size = sizeof(hs_config);
	      memscpy((void*)&hs_config ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;

		    if(hs_config == 0)
        {
          /* Copy LNA Rise SP */
          nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_rise[0])*RFWCDMA_NUM_LNA_STATES;
          memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_rise,
                                       nv_element_size,nv_item_ptr,nv_element_size);
          num_bytes_parsed += nv_element_size;
          nv_item_ptr 	 += nv_element_size;

          /* Copy LNA Fall SP */
          nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_fall[0])*RFWCDMA_NUM_LNA_STATES;
          memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_fall ,
                                         nv_element_size,nv_item_ptr,nv_element_size);
          num_bytes_parsed += nv_element_size;
          nv_item_ptr 	 += nv_element_size;
        }
        else if (hs_config == 1)
        {
          /* Copy LNA HS Rise SP */
          nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_rise[0])*RFWCDMA_NUM_LNA_STATES;
          memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_rise ,
                                       nv_element_size,nv_item_ptr,nv_element_size);
          num_bytes_parsed += nv_element_size;
          nv_item_ptr 	 += nv_element_size;

          /* Copy LNA HS FALL SP */
          nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_fall[0])*RFWCDMA_NUM_LNA_STATES;
          memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_fall ,
                                       nv_element_size,nv_item_ptr,nv_element_size);
          num_bytes_parsed += nv_element_size;
          nv_item_ptr 	 += nv_element_size;
        }
        else
        {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_static_data_nv: "
                  "hs_config= %d is not valid",hs_config);
        }
        break;

      case RFNV_DATA_TYPE_LNA_SWPTS_V3:
        rx_stat = (rfwcdma_nv_rx_static_payload_v2_type *)&rfnv_ptr->rx_static_data.payload;

        /* Read chain_id */
	      //nv_element_size = sizeof(chain_id);
	      //memscpy((void*)&chain_id ,nv_element_size,nv_item_ptr,nv_element_size);
	      //num_bytes_parsed += nv_element_size;
	      //nv_item_ptr 	 += nv_element_size;
        chain_id = input_chain_id;

        if(chain_id >= RF_PATH_4)
        {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_static_data_nv: "
                       "chain_id= %d is not within range(>= RF_PATH_4)",chain_id);
          return;
        }
     
        /* Read carr_id */
	      nv_element_size = sizeof(carr_id);
	      memscpy((void*)&carr_id ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;

		    if(carr_id >= RFCOM_MULTI_CARRIER_ID__CUMULATIVE)
		    {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_static_data_nv: "
                  "carr_id= %d is not within range(>= RFCOM_MULTI_CARRIER_ID__CUMULATIVE)",carr_id);
          return;
        }

        /* Read hs_config */
	      nv_element_size = sizeof(hs_config);
	      memscpy((void*)&hs_config ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;

		    if(hs_config == 0)
        {
          /* Copy LNA Rise SP */
          nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_rise[0])*RFWCDMA_NUM_LNA_STATES;
          memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_rise,
                                       nv_element_size,nv_item_ptr,nv_element_size);
          num_bytes_parsed += nv_element_size;
          nv_item_ptr 	 += nv_element_size;

          /* Copy LNA Fall SP */
          nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_fall[0])*RFWCDMA_NUM_LNA_STATES;
          memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_fall ,
                                         nv_element_size,nv_item_ptr,nv_element_size);
          num_bytes_parsed += nv_element_size;
          nv_item_ptr 	 += nv_element_size;
        }
        else if (hs_config == 1)
        {
          /* Copy LNA HS Rise SP */
          nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_rise[0])*RFWCDMA_NUM_LNA_STATES;
          memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_rise ,
                                       nv_element_size,nv_item_ptr,nv_element_size);
          num_bytes_parsed += nv_element_size;
          nv_item_ptr 	 += nv_element_size;

          /* Copy LNA HS FALL SP */
          nv_element_size = sizeof(rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_fall[0])*RFWCDMA_NUM_LNA_STATES;
          memscpy((void*)rx_stat->rx_lna_swpts[chain_id][carr_id].lna_hs_fall ,
                                       nv_element_size,nv_item_ptr,nv_element_size);
          num_bytes_parsed += nv_element_size;
          nv_item_ptr 	 += nv_element_size;
        }
        else
        {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_static_data_nv: "
                  "hs_config= %d is not valid",hs_config);
        }
        break;

	    default:
        MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,
               "rfnv_wcdma_load_rx_static_data_nv: "
               "Invalid Variant Marker %d",type_id);
    }/* switch (type_id) */
  }/* for(i=0;i<rfnv_ptr->rx_static_data.rx_stat_variant_marker.num_elements;i++) */

  return;
}/* rfnv_wcdma_load_rx_static_data_nv */

/*--------------------------------------------------------------------------*/
/*!
 @brief
   Loading the Rx dynamic NV into the internal structure
   
  @details
    LNA/VGA offsets for each carrier and chain are loaded
	
  @param rfnv_ptr
   Pointer to the internal NV structure
  
  @param item_ptr
   Pointer to the RFNV
  
  @param band
   Band to which the NV belongs to 
   
*/
void rfnv_wcdma_load_rx_dynamic_data_nv
(
  rfnv_wcdma_nv_tbl_type *rfnv_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  /* The TYPE ID of the current element */
  uint16 type_id = 0;

  /* Keeps track of the running total of number of bytes already parsed */
  uint32 num_bytes_parsed = 0;	

  /* Contains the number of bytes for an element in NV */
  uint32 nv_element_size = 0;

  /* Contains the chain information */
  uint8 chain_id = 0;

  /* Contains the carrier ID */
  uint8 carr_id = 0;

  /* Contains the cal channel */
  uint16 channel = 0;

  /* Contains the channel index */
  uint8 chan_index = 0;
  
  int i =0;

  /* Keeps track of default values per band */
  static boolean init_flag[RFCOM_NUM_WCDMA_BANDS]={FALSE};

  rfwcdma_nv_rx_dynamic_payload_v2_type *rx_dyn = NULL;

  /* used for item_ptr */
  byte *nv_item_ptr = (byte *)item_ptr;

  if(rfnv_ptr == NULL || item_ptr ==  NULL )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_dynamic_data_nv: "
                                         "NULL NV table pointer or NULL item ptr");
    return;
  }

  /* Memset all data to 0 */
  memset((void*)&rfnv_ptr->rx_dyn_data,0,sizeof(rfwcdma_nv_rx_dynamic_data_type));
  
  /* Read the Fix part*/
  nv_element_size = sizeof(uint16)*RFNV_PATHS_TABLE_SIZ;
  //memscpy((void*)&rfnv_ptr->rx_dyn_data.ref_rx_chan ,nv_element_size,nv_item_ptr,nv_element_size);
  num_bytes_parsed += nv_element_size;
  nv_item_ptr      += nv_element_size;

  /* Read Version */
  nv_element_size = sizeof(uint16);
  memscpy((void*)&rfnv_ptr->rx_dyn_data.rx_dyn_variant_marker.version ,nv_element_size,nv_item_ptr,nv_element_size);
  num_bytes_parsed += nv_element_size;
  nv_item_ptr      += nv_element_size;
  
  if(!init_flag[band])
  {
    /* Setting the NV to default values */
    rfnv_wcdma_set_rx_cal_data_to_default(rfnv_ptr);
    init_flag[band] = TRUE ;
  }
  
  /* Read number of elements */
  memscpy((void*)&rfnv_ptr->rx_dyn_data.rx_dyn_variant_marker.num_elements ,nv_element_size,nv_item_ptr,nv_element_size);
  num_bytes_parsed += nv_element_size;
  nv_item_ptr      += nv_element_size;

  for(i=0;i<rfnv_ptr->rx_dyn_data.rx_dyn_variant_marker.num_elements;i++)
  {
	  nv_element_size = sizeof(uint16);
	  /* Read type_id */
	  memscpy((void*)&type_id ,nv_element_size,nv_item_ptr,nv_element_size);
	  num_bytes_parsed += nv_element_size;
	  nv_item_ptr	     += nv_element_size;
	    
	  switch (type_id)
	  {
      case RFNV_DATA_TYPE_RX_OFFSET_VS_FREQ_V1:
	      rx_dyn = (rfwcdma_nv_rx_dynamic_payload_v2_type *)&rfnv_ptr->rx_dyn_data.payload;
	      /* Read chain_id */
	      nv_element_size = sizeof(chain_id);
	      memscpy((void*)&chain_id ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr	     += nv_element_size;
        
	      if(chain_id >= RF_PATH_4)
        {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_dynamic_data_nv: "
                        "chain_id= %d is not within range(>= RF_PATH_4)",chain_id);
          return;
        }
        
	      /* Read channel */
	      nv_element_size = sizeof(channel);
	      memscpy((void*)&channel ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr	     += nv_element_size;
        
	      /* Read channel index */
	      nv_element_size = sizeof(chan_index);
	      memscpy((void*)&chan_index ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr	     += nv_element_size; 

		    if(chan_index >= NV_FREQ_TABLE_SIZ)
		    {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_dynamic_data_nv: "
                         "chan_index= %d is not within range(>= NV_FREQ_TABLE_SIZ)",chan_index);
          return;
        }

        /* Update cal channels list */
        rx_dyn->rx_cal_chan[chain_id][chan_index] = channel;

	      /* Copy vga_gain_offset_vs_freq */
	      nv_element_size = sizeof(rx_dyn->freq_offset[chain_id][chan_index].vga_gain_offset_vs_freq);
	      memscpy((void*)&rx_dyn->freq_offset[chain_id][chan_index].vga_gain_offset_vs_freq ,
			                                         nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;

	      /* Copy lna_offset_vs_freq_1 */
	      nv_element_size = sizeof(rx_dyn->freq_offset[chain_id][chan_index].lna_offset_vs_freq_1);
	      memscpy((void*)&rx_dyn->freq_offset[chain_id][chan_index].lna_offset_vs_freq_1 ,
			                                         nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;
        
	      /* Copy lna_offset_vs_freq_2 */
	      nv_element_size = sizeof(rx_dyn->freq_offset[chain_id][chan_index].lna_offset_vs_freq_2);
	      memscpy((void*)&rx_dyn->freq_offset[chain_id][chan_index].lna_offset_vs_freq_2 ,
			                                         nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;
		    
	      /* Copy lna_offset_vs_freq_3 */
	      nv_element_size = sizeof(rx_dyn->freq_offset[chain_id][chan_index].lna_offset_vs_freq_3);
	      memscpy((void*)&rx_dyn->freq_offset[chain_id][chan_index].lna_offset_vs_freq_3,
			                                          nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;
		    
	      /* Copy lna_offset_vs_freq_4 */
	      nv_element_size = sizeof(rx_dyn->freq_offset[chain_id][chan_index].lna_offset_vs_freq_4);
	      memscpy((void*)&rx_dyn->freq_offset[chain_id][chan_index].lna_offset_vs_freq_4 ,
			                                          nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;
        break;
		
      case RFNV_DATA_TYPE_RX_CAL_OFFSET_V1:
	      rx_dyn = (rfwcdma_nv_rx_dynamic_payload_v2_type *)&rfnv_ptr->rx_dyn_data.payload;
        /* Read nv_container_id */
	      nv_element_size = sizeof(chain_id);
	      memscpy((void*)&chain_id ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;

	      if(chain_id >= RF_PATH_4)
        {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_dynamic_data_nv: "
                        "chain_id= %d is not within range(>= RF_PATH_4)",chain_id);
          return;
        }
        
        /* Read carr_id */
	      nv_element_size = sizeof(carr_id);
	      memscpy((void*)&carr_id ,nv_element_size,nv_item_ptr,nv_element_size);
	      num_bytes_parsed += nv_element_size;
	      nv_item_ptr 	 += nv_element_size;

		    if(carr_id >= RFCOM_MULTI_CARRIER_ID__CUMULATIVE)
		    {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_dynamic_data_nv: "
                         "carr_id= %d is not within range(>= NV_FREQ_TABLE_SIZ)",carr_id);
          return;
        }

        /* Copy lna_offsets */
        nv_element_size = sizeof(rx_dyn->rx_offsets[chain_id][carr_id].lna_offsets[0])*RFWCDMA_NUM_LNA_STATES;
		    memscpy((void*)rx_dyn->rx_offsets[chain_id][carr_id].lna_offsets ,nv_element_size,
		                                                                  nv_item_ptr,nv_element_size);
		    num_bytes_parsed += nv_element_size;
	      nv_item_ptr += nv_element_size;
        /* Copy vga_offset */
       	nv_element_size = sizeof(rx_dyn->rx_offsets[chain_id][carr_id].vga_offset);
        memscpy((void*)&rx_dyn->rx_offsets[chain_id][carr_id].vga_offset ,nv_element_size,
			                                                                nv_item_ptr,nv_element_size);
        num_bytes_parsed += nv_element_size;
        nv_item_ptr 	 += nv_element_size;
        break;

	    default:
        MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_dynamic_data_nv: "
                        "Invalid Variant Marker %d",type_id);		
	  }/* switch (type_id) */
  }/* (i=0; i<rfnv_ptr->rx_dyn_data.rx_dyn_variant_marker.num_elements; i++) */

  return;
}/* rfnv_wcdma_load_rx_dynamic_data_nv */

/*--------------------------------------------------------------------------*/
/*!
 @brief
   Loading the Rx dynamic V2 NV into the internal structure
   
  @details
    LNA/VGA offsets for each carrier and chain are loaded
	
  @param rfnv_ptr
   Pointer to the internal NV structure
  
  @param rx_band_cal_data_ptr
   Pointer to the common rx cal data
  
  @param band
   Band to which the NV belongs to 
   
*/
void rfnv_wcdma_load_rx_dynamic_data_v2_nv
(
  rfnv_wcdma_nv_tbl_type *rfnv_ptr,
  rfnv_rx_cal_data_type *rx_band_cal_data_ptr,
  rfcom_wcdma_band_type band
)
{
  /* Contains the channel index */
  uint8 chan_index = 0, chan_num =0;

  /* Contains the channel index */
  uint8 path_idx = 0;

  rfnv_rx_cal_data_typeid_118_type *nv_cal_data = NULL;
  
  uint8 element_index =0;

  uint16 chan_val = 0; 
  uint8 center_chan = 0;

  /* Keeps track of default values per band */
  static boolean init_flag[RFCOM_NUM_WCDMA_BANDS]={FALSE};

  rfwcdma_nv_rx_dynamic_payload_v2_type *rx_dyn = NULL;

  if(rfnv_ptr == NULL || rx_band_cal_data_ptr ==  NULL || rx_band_cal_data_ptr->cal_118_data == NULL )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_dynamic_data_v2_nv: "
                                         "NULL NV table pointer or NULL item ptr");
    return;
  }
  
  if(!init_flag[band])
  {
    /* Memset all data to 0 */
    memset((void*)&rfnv_ptr->rx_dyn_data,0,sizeof(rfwcdma_nv_rx_dynamic_data_type));
    /* Setting the NV to default values */
    rfnv_wcdma_set_rx_cal_data_to_default(rfnv_ptr);
    init_flag[band] = TRUE ;
  }

  /* Read number of elements */
  rfnv_ptr->rx_dyn_data.rx_dyn_variant_marker.num_elements = rx_band_cal_data_ptr->variant_marker.num_elements;

  nv_cal_data = rx_band_cal_data_ptr->cal_118_data;
  rx_dyn = (rfwcdma_nv_rx_dynamic_payload_v2_type *)&rfnv_ptr->rx_dyn_data.payload;

  for(element_index=0; element_index<rfnv_ptr->rx_dyn_data.rx_dyn_variant_marker.num_elements; element_index++)
  { 
    path_idx = nv_cal_data[element_index].path_idx;
    for (chan_num =0; chan_num <16; chan_num ++)
    {
      if (nv_cal_data[element_index].channel_list[chan_num] ==0)
      {
        break;
      }
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfnv_wcdma_load_rx_dynamic_data_v2_nv: channel is %d",
                                            nv_cal_data[element_index].channel_list[chan_index]);
    }

	if (chan_num == 0)
    {
	  MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_dynamic_data_v2_nv: No channels available chan_num: %d",chan_num);
      return;
    }
    else 
    {
      center_chan = chan_num/2;
    }

    chan_val = nv_cal_data[element_index].channel_list[center_chan];

    MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfnv_wcdma_load_rx_dynamic_data_v2_nv: chan_num %d center channel %d, index %d",chan_num, chan_val, center_chan);

	  if(path_idx >= RF_PATH_4)
    {
      MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_dynamic_data_v2_nv: "
             "path_idx= %d is not within range(>= RF_PATH_4)", path_idx);
    }
    else
    {
      switch (nv_cal_data[element_index].cal_type)
      {
        case RFNV_RX_CAL_DATA_NV_TYPE_WCDMA_SC:        
          rx_dyn->rx_offsets[path_idx][0].vga_offset = nv_cal_data[element_index].freq_comp_offset[0][center_chan];
          rx_dyn->rx_offsets[path_idx][0].lna_offsets[0] = nv_cal_data[element_index].freq_comp_offset[1][center_chan];
          rx_dyn->rx_offsets[path_idx][0].lna_offsets[1] = nv_cal_data[element_index].freq_comp_offset[2][center_chan];
          rx_dyn->rx_offsets[path_idx][0].lna_offsets[2] = nv_cal_data[element_index].freq_comp_offset[3][center_chan];
          rx_dyn->rx_offsets[path_idx][0].lna_offsets[3] = nv_cal_data[element_index].freq_comp_offset[4][center_chan];

          for (chan_index =0; chan_index < RFNV_NUMBER_OF_PCLS; chan_index++)
          {
            /* Update cal channels list */
            rx_dyn->rx_cal_chan[path_idx][chan_index] = nv_cal_data[element_index].channel_list[chan_index];

            /* Copy vga_gain_offset_vs_freq */
            rx_dyn->freq_offset[path_idx][chan_index].vga_gain_offset_vs_freq =
                nv_cal_data[element_index].freq_comp_offset[0][chan_index] -
                rx_dyn->rx_offsets[path_idx][0].vga_offset;

            /* Copy lna_offset_vs_freq_1 */
            rx_dyn->freq_offset[path_idx][chan_index].lna_offset_vs_freq_1 =
                nv_cal_data[element_index].freq_comp_offset[1][chan_index] -
                rx_dyn->rx_offsets[path_idx][0].lna_offsets[0];
          
            /* Copy lna_offset_vs_freq_2 */
            rx_dyn->freq_offset[path_idx][chan_index].lna_offset_vs_freq_2 =
                nv_cal_data[element_index].freq_comp_offset[2][chan_index] -
                rx_dyn->rx_offsets[path_idx][0].lna_offsets[1];

            /* Copy lna_offset_vs_freq_3 */
            rx_dyn->freq_offset[path_idx][chan_index].lna_offset_vs_freq_3 =
                nv_cal_data[element_index].freq_comp_offset[3][chan_index] -
                rx_dyn->rx_offsets[path_idx][0].lna_offsets[2];
 
            /* Copy lna_offset_vs_freq_4 */
            rx_dyn->freq_offset[path_idx][chan_index].lna_offset_vs_freq_4 =
                nv_cal_data[element_index].freq_comp_offset[4][chan_index] -
                rx_dyn->rx_offsets[path_idx][0].lna_offsets[3];
          }
          break;
      
        case RFNV_RX_CAL_DATA_NV_TYPE_WCDMA_DC:        
          rx_dyn->rx_offsets[path_idx][1].vga_offset = nv_cal_data[element_index].freq_comp_offset[0][0];
          rx_dyn->rx_offsets[path_idx][1].lna_offsets[0] = nv_cal_data[element_index].freq_comp_offset[1][0];
          rx_dyn->rx_offsets[path_idx][1].lna_offsets[1] = nv_cal_data[element_index].freq_comp_offset[2][0];
          rx_dyn->rx_offsets[path_idx][1].lna_offsets[2] = nv_cal_data[element_index].freq_comp_offset[3][0];
          rx_dyn->rx_offsets[path_idx][1].lna_offsets[3] = nv_cal_data[element_index].freq_comp_offset[4][0];
          break;

        case RFNV_RX_CAL_DATA_NV_TYPE_WCDMA_3C:        
          rx_dyn->rx_offsets[path_idx][2].vga_offset = nv_cal_data[element_index].freq_comp_offset[0][0];
          rx_dyn->rx_offsets[path_idx][2].lna_offsets[0] = nv_cal_data[element_index].freq_comp_offset[1][0];
          rx_dyn->rx_offsets[path_idx][2].lna_offsets[1] = nv_cal_data[element_index].freq_comp_offset[2][0];
          rx_dyn->rx_offsets[path_idx][2].lna_offsets[2] = nv_cal_data[element_index].freq_comp_offset[3][0];
          rx_dyn->rx_offsets[path_idx][2].lna_offsets[3] = nv_cal_data[element_index].freq_comp_offset[4][0];
          break;

        default:
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_load_rx_dynamic_data_v2_nv: "
                          "Invalid Variant Marker %d",nv_cal_data[element_index].cal_type);		
      }/* switch (type_id) */
    }/* if(nv_cal_data[element_index]->path_idx < RF_PATH_4) */
  }/* (i=0; i<rfnv_ptr->rx_dyn_data.rx_dyn_variant_marker.num_elements; i++) */

  return;
}/* rfnv_wcdma_load_rx_dynamic_data_v2_nv */

/*--------------------------------------------------------------------------*/
/*!
  @brief
    Set the Rx static NV data to default values
  
   @details
     Before copying over the band specifc NV values, all NVs are set to defaults
    
   @param rfnv_ptr
      Pointer to the internal NV structure
  
*/
void rfnv_wcdma_set_rx_static_data_to_default(rfnv_wcdma_nv_tbl_type *rfnv_ptr)
{
  rfwcdma_nv_rx_static_version_type rx_stat_version ;
  rfwcdma_nv_rx_static_data_type *rx_stat_data = NULL;
  rfwcdma_nv_rx_static_payload_v2_type *rx_data_ptr = NULL ;
  uint8 i,j = 0;
  
  /* Access the NV structure through the payload. Assume its V1 be default */
  rx_stat_data = &(rfnv_ptr->rx_static_data);
  rx_stat_data->rx_stat_variant_marker.version = RFWCDMA_NV_RX_STATIC_DATA_V2 ;
  rx_data_ptr = &(rx_stat_data->payload.rx_stat_v2);

  rx_stat_version = rx_stat_data->rx_stat_variant_marker.version ;

  /* HS rise and fall points */
  for(i = 0; i < RF_PATH_4 ; i++)
  {
    for(j = 0; j < RFCOM_MULTI_CARRIER_ID__CUMULATIVE ; j++)
    {
     rx_data_ptr->rx_lna_swpts[i][j].lna_hs_rise[0] = 38 ;
     rx_data_ptr->rx_lna_swpts[i][j].lna_hs_rise[1] = 188 ;
     rx_data_ptr->rx_lna_swpts[i][j].lna_hs_rise[2] = 268 ;
     rx_data_ptr->rx_lna_swpts[i][j].lna_hs_rise[3] = 32767 ;
     rx_data_ptr->rx_lna_swpts[i][j].lna_hs_rise[4] = 32767 ;
     rx_data_ptr->rx_lna_swpts[i][j].lna_hs_fall[0] = -2 ;
     rx_data_ptr->rx_lna_swpts[i][j].lna_hs_fall[1] = 148 ;
     rx_data_ptr->rx_lna_swpts[i][j].lna_hs_fall[2] = 228 ;
     rx_data_ptr->rx_lna_swpts[i][j].lna_hs_fall[3] = 32767;
     rx_data_ptr->rx_lna_swpts[i][j].lna_hs_fall[4] = 32767;
    }
  }

  /* Rise and Fall points for Carrier 0 for both the paths*/
  j = RFCOM_MULTI_CARRIER_ID__0;
    
  for(i = 0; i < RF_PATH_4 ; i++)
  {
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[0] = -132 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[1] = 108 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[2] = 268 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[3] = 32767 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[4] = 32767 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_fall[0] = -172 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_fall[1] = 68 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_fall[2] = 228 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_fall[3] = 32767;
    rx_data_ptr->rx_lna_swpts[i][j].lna_fall[4] = 32767; 
  }

  /* Rise points for Carrier 1 for both the paths*/
  j = RFCOM_MULTI_CARRIER_ID__1;
    
  for(i = 0; i < RF_PATH_4 ; i++)
  {
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[0] = -312 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[1] = -212;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[2] = 38 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[3] = 188 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[4] = 318 ;
  }

  /* FAll points for Carrier 1 for PATH 0*/
  i= RF_PATH_0; j = RFCOM_MULTI_CARRIER_ID__1;
  
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[0] = -352 ;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[1] = -252;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[2] = -2 ;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[3] = 148 ;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[4] = 278 ;

     
  /* FAll points for Carrier 1 for PATH 1*/
  i = RF_PATH_1; j = RFCOM_MULTI_CARRIER_ID__1;
  
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[0] = -386 ;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[1] = -288;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[2] = -32;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[3] = 118 ;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[4] = 242 ;

  /* FAll points for Carrier 1 for PATH 2*/
  i= RF_PATH_2; j = RFCOM_MULTI_CARRIER_ID__1;
  
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[0] = -352 ;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[1] = -252;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[2] = -2 ;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[3] = 148 ;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[4] = 278 ;

   
  /* FAll points for Carrier 1 for PATH 3*/
  i = RF_PATH_3; j = RFCOM_MULTI_CARRIER_ID__1;
  
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[0] = -386 ;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[1] = -288;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[2] = -32;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[3] = 118 ;
  rx_data_ptr->rx_lna_swpts[i][j].lna_fall[4] = 242 ;

  /* Rise and Fall points for Carrier 2 for both the paths*/
  j = RFCOM_MULTI_CARRIER_ID__2;
    
  for(i = 0; i < RF_PATH_4 ; i++)
  {
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[0] = -312  ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[1] = -212 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[2] = 38 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[3] = 188 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_rise[4] = 318 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_fall[0] = -352 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_fall[1] = -252 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_fall[2] = -2 ;
    rx_data_ptr->rx_lna_swpts[i][j].lna_fall[3] = 148;
    rx_data_ptr->rx_lna_swpts[i][j].lna_fall[4] = 278; 
  }
}/* rfnv_wcdma_set_rx_static_data_to_default */

/*--------------------------------------------------------------------------*/
/*!
  @brief
   Set the Rx Cal NV data to default values

  @details
    Before copying over the band specifc NV values, all NVs are set to defaults
   
  @param rfnv_ptr
     Pointer to the internal NV structure
  
*/
void rfnv_wcdma_set_rx_cal_data_to_default(rfnv_wcdma_nv_tbl_type *rfnv_ptr)
{
  rfwcdma_nv_rx_dynamic_payload_v2_type *rx_data_ptr = NULL ;
  rfwcdma_nv_rx_dynamic_data_type *rx_dyn_data_ptr = NULL;
  uint8 i,j=0;
  
  /* Access the NV structure through the payload. Assume its V2 be default */
  rx_dyn_data_ptr = &(rfnv_ptr->rx_dyn_data);
  rx_dyn_data_ptr->rx_dyn_variant_marker.version = RFWCDMA_NV_RX_DYNAMIC_DATA_V2; 
  rx_data_ptr = &(rx_dyn_data_ptr->payload.rx_dyn_v2);

    /* LNA and VGA offset vs Freq */
  for(i = 0; i < RFM_DEVICE_4 ; i++)
  {
    for(j = 0; j < NV_FREQ_TABLE_SIZ ; j++)
    {
      rx_data_ptr->freq_offset[i][j].vga_gain_offset_vs_freq = 0 ;
      rx_data_ptr->freq_offset[i][j].lna_offset_vs_freq_1 = 0 ;
      rx_data_ptr->freq_offset[i][j].lna_offset_vs_freq_2 = 0 ;
      rx_data_ptr->freq_offset[i][j].lna_offset_vs_freq_3 = 0 ;
      rx_data_ptr->freq_offset[i][j].lna_offset_vs_freq_4 = 0 ;
    }
  }

  j = RFCOM_MULTI_CARRIER_ID__0;
    
  for(i = 0; i < RFM_DEVICE_4 ; i++)
  {
    rx_data_ptr->rx_offsets[i][j].vga_offset = -212;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[0] = 232;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[1] = 273;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[2] = 382;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[3] = 382;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[4] = 382;
  }

  j = RFCOM_MULTI_CARRIER_ID__1;
    
  for(i = 0; i < RFM_DEVICE_4 ; i++)
  {
    rx_data_ptr->rx_offsets[i][j].vga_offset = -141;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[0] = 83;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[1] = 143;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[2] = 254;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[3] = 356;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[4] = 470;
  }

  j = RFCOM_MULTI_CARRIER_ID__2;
    
  for(i = 0; i < RFM_DEVICE_4 ; i++)
  {
    rx_data_ptr->rx_offsets[i][j].vga_offset = -141;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[0] = 83;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[1] = 143;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[2] = 254;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[3] = 356;
    rx_data_ptr->rx_offsets[i][j].lna_offsets[4] = 470;
  }
}/* rfnv_wcdma_set_rx_cal_data_to_default */

/*--------------------------------------------------------------------------*/
/*!
   @brief
   rfnv_wcdma_get_pa_static_nv_version
   
   @detail
   Get pa static NV version for the band passed
   
   @param 
   rfcom_wcdma_band_type band
    
   @retval 
   version
   
*/
rfwcdma_nv_pa_static_ver_type
rfnv_wcdma_get_pa_static_nv_version
(
  rfcom_wcdma_band_type band
)
{
  rfcom_wcdma_band_type band_convert = band;
  switch (band)
  {
    case RFCOM_BAND_BC19:
      band_convert = RFCOM_BAND_800;
      break;

    case RFCOM_BAND_BC9:
      band_convert = RFCOM_BAND_BC3;
      break;
	
    default:
      band_convert = band;
      break;
  }

  if (band_convert < RFCOM_NUM_WCDMA_BANDS)
  {
    return rfnv_wcdma_pa_static_nv_version[band_convert];
  }
  else
  {
    return RFWCDMA_NV_PA_STATIC_VER_INVALID;
  }
}


/*--------------------------------------------------------------------------*/
/*!
   @brief
   rfnv_wcdma_set_pa_static_nv_version
   
   @detail
   Set pa static NV version for the band passed
   
   @param 
   rfcom_wcdma_band_type band
   int8 version
    
   @retval

*/
void
rfnv_wcdma_set_pa_static_nv_version
(
  rfcom_wcdma_band_type band,
  int8 version
)
{
  rfnv_wcdma_pa_static_nv_version[band] = version;
  
  if(version >= RFWCDMA_NV_PA_STATIC_VER_MAX)
  {
    rfnv_wcdma_pa_static_nv_version[band] = RFWCDMA_NV_PA_STATIC_VER_INVALID;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the DC Tx characterization channel list, for use with characterized
  Lin vs Temp vs Freq data

  @details

  @param nv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
*/
void
rfnv_wcdma_load_dc_equidistant_therm_bins_data
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr
)
{
  uint8 therm_bin;

  /* Initialize the therm bins to use equally spaced bins */
  for ( therm_bin = 0; therm_bin < NV_TEMP_TABLE_SIZ; therm_bin++ )
  {
    nv_tbl_ptr->dc_lin_temp_comp_data.tx_therm_bins[therm_bin] = 
                                 therm_bin * RFWCDMA_NV_SINGLE_TEMP_BIN_SIZE;
  }

  return;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load DC Lin vs Temp vs Freq Pin and Pout NV data offset data into common DC Tx
  Lin data structure type for all bands

  @details

  @param nv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param rfnv_item_ptr
  Pointer to the NV buffer where data loaded from NV currently is stored
 
  @param is_pin
  Flag to indicate if Pin data or not. If not, the data is copied to Pout
*/
void 
rfnv_wcdma_load_dc_lin_vs_temp_vs_freq
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr,
  boolean                is_pin
)
{
  uint8 pa_index_ctr = 0;
  uint16 temp_index;
  uint16 freq_index;

  /* Initialize the temp comp data structure */
  nv_tbl_ptr->dc_lin_temp_comp_data.has_pin_pout_temp_comp_nv = TRUE;

  /* Initialize the temp comp data structure */
  if ( is_pin )
  {
    for (pa_index_ctr=0; pa_index_ctr<RFCOM_TXLIN_NUM_LINEARIZERS; pa_index_ctr++)
    {
      for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
        {
          nv_tbl_ptr->dc_lin_temp_comp_data.lin_vs_temp_vs_freq.pin[pa_index_ctr][temp_index][freq_index] = 
            nv_tbl_ptr->dc_pa_static.tx_lin_pin[temp_index][freq_index][pa_index_ctr];
        }
      }
    }
  }
  else
  {
    for (pa_index_ctr=0; pa_index_ctr<RFCOM_TXLIN_NUM_LINEARIZERS; pa_index_ctr++)
    {
      for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
        {
          nv_tbl_ptr->dc_lin_temp_comp_data.lin_vs_temp_vs_freq.pout[pa_index_ctr][temp_index][freq_index] = 
            nv_tbl_ptr->dc_pa_static.tx_lin_pout[temp_index][freq_index][pa_index_ctr];
        }
      }
    }  
  }

  /* Also initialize the equidistant therm bins data in the NV 
  data structure */
  rfnv_wcdma_load_dc_equidistant_therm_bins_data( nv_tbl_ptr );

  return;
}

/*--------------------------------------------------------------------------*/
/*!
   @brief
   rfnv_wcdma_get_dc_pa_static_nv_version
   
   @detail
   Get pa static NV version for the band passed
   
   @param 
   rfcom_wcdma_band_type band
    
   @retval 
   version
   
*/

rfwcdma_nv_pa_static_ver_type
rfnv_wcdma_get_dc_pa_static_nv_version
(
  rfcom_wcdma_band_type band
)
{
  return rfnv_wcdma_dc_pa_static_nv_version[band];
}


/*--------------------------------------------------------------------------*/
/*!
   @brief
   rfnv_wcdma_set_dc_pa_static_nv_version
   
   @detail
   Set pa static NV version for the band passed
   
   @param 
   rfcom_wcdma_band_type band
   int8 version
    
   @retval

*/

void
rfnv_wcdma_set_dc_pa_static_nv_version
(
  rfcom_wcdma_band_type band,
  int8 version
)
{
  if(version >= RFWCDMA_NV_PA_STATIC_VER_MAX)
  {
    rfnv_wcdma_dc_pa_static_nv_version[band] = RFWCDMA_NV_PA_STATIC_VER_INVALID;
  }
  else
  {
    rfnv_wcdma_dc_pa_static_nv_version[band] = version;
  }
}

/*----------------------------------------------------------------------------*/
/* rfnv_wcdma_is_pa_static_nv */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  If a particular NV id is a 8 PA static NV or not.
 
  @details
  If a particular NV id is a 8 PA static NV or not.

  @param rfnv_item_id
  NV ID
  
  @retval
  TRUE  - 8 PA static NV
  FALSE - Other NV
*/

boolean
rfnv_wcdma_is_pa_static_nv
(
  rfnv_item_id_enum_type rfnv_item_id
)
{

  boolean is_pa_static_nv = FALSE;

  switch ( rfnv_item_id )
  {  
    case RFNV_WCDMA_C0_2100_PA_STATIC_I :
    case RFNV_WCDMA_C0_1900_PA_STATIC_I :    
    case RFNV_WCDMA_C0_BC4_PA_STATIC_I :
    case RFNV_WCDMA_C0_800_PA_STATIC_I :
    case RFNV_WCDMA_C0_900_PA_STATIC_I :
    case RFNV_WCDMA_C0_1800_PA_STATIC_I :
    case RFNV_WCDMA_C0_1500_PA_STATIC_I :
    case RFNV_WCDMA_C0_DC_2100_PA_STATIC_I :
    case RFNV_WCDMA_C0_DC_1900_PA_STATIC_I :    
    case RFNV_WCDMA_C0_DC_BC4_PA_STATIC_I :
    case RFNV_WCDMA_C0_DC_800_PA_STATIC_I :
    case RFNV_WCDMA_C0_DC_900_PA_STATIC_I :
    case RFNV_WCDMA_C0_DC_1800_PA_STATIC_I :
    case RFNV_WCDMA_C0_DC_1500_PA_STATIC_I :

    case RFNV_WCDMA_C0_2100_PA_STATIC_Z_I :
    case RFNV_WCDMA_C0_1900_PA_STATIC_Z_I :    
    case RFNV_WCDMA_C0_BC4_PA_STATIC_Z_I :
    case RFNV_WCDMA_C0_800_PA_STATIC_Z_I :
    case RFNV_WCDMA_C0_900_PA_STATIC_Z_I :
    case RFNV_WCDMA_C0_1800_PA_STATIC_Z_I :
    case RFNV_WCDMA_C0_1500_PA_STATIC_Z_I :
    case RFNV_WCDMA_C0_DC_2100_PA_STATIC_Z_I :
    case RFNV_WCDMA_C0_DC_1900_PA_STATIC_Z_I :    
    case RFNV_WCDMA_C0_DC_BC4_PA_STATIC_Z_I :
    case RFNV_WCDMA_C0_DC_800_PA_STATIC_Z_I :
    case RFNV_WCDMA_C0_DC_900_PA_STATIC_Z_I :
    case RFNV_WCDMA_C0_DC_1800_PA_STATIC_Z_I :
    case RFNV_WCDMA_C0_DC_1500_PA_STATIC_Z_I :

      is_pa_static_nv = TRUE;
  
    break;
  
    default:

      is_pa_static_nv = FALSE;
  
    break;
    
  }/* switch ( rfnv_item_id ) */  

  return ( is_pa_static_nv );

}/* rfnv_wcdma_nv_is_pa_static_nv */


