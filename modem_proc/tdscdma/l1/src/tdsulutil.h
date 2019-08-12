#ifndef TDS_UL_UTILS_H
#define TDS_UL_UTILS_H

/*============================================================================
              U P L I N K   H E A D E R   F I L E

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
physical channel configuration database and CCTrCH configuration database.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsulutil.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/30/10    cdf/cxc  file created.
                    
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "queue.h"
#include "tdsl1ulcfg.h"
#include "tdsl1macdata.h"

/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/

/* utility macros to create bit masks */
#define TDS_UL_LOW_ZEROS(n)  (-1 << (n))
#define TDS_UL_LOW_ONES(n)   (~(uint32)(TDS_UL_LOW_ZEROS(n)))


/* maximum number of physical channel configurations maintained */
#define TDS_UL_PHYCHAN_CFG_MAX           2

/* Number of prime numbers in list for Turbo interleaving parameters */
#define TDS_UL_NUM_PRIME_LIST_SZ  52

#define TDS_UL_INVALID_TX_POWER          0xFF

#define TDS_UL_TX_POWER_ALPHA_FILTER_Q8 26

#define TDS_UL_INVALID_TX_POWER_16 0x7FFF

#define TDS_UL_MIN_TX_PWER_SW_THRESHOLD_Q4 -1024 /*-64dbm in Q4 format*/

extern const uint8 tdsulutil_tti_Fi_map[TDSL1_NUM_TTI];

extern const uint8 tdsulutil_tti_SubFi_map[TDSL1_NUM_TTI];

/* Add maximum and minmum tx power for SW tx power, report warining if out of 
 * range. Since the final tx power value is calculated by FW base on SW's, here,
 * 50 and -80 are experienced value. They are only used to report warning 
 * but not cap the power.
 */
#define TDS_UL_SW_MAX_TX_PWR  800     /* 50dBm in 1/16 */
#define TDS_UL_SW_MIN_TX_PWR  (-1280) /* -80dBm in 1/16 */

/* System would like to put a sanity check on the desired pwr of UL channel 
 * that is signaled by the nodeB.
 * prx_UL_chan = max(prx_UL_chan_signalled_in_dBm, -105 dBm)
 */
#define TDS_UL_SW_MIN_DES_PWR     (-1696) /* -106dB in 1/16 */
/* + defang 01/13/2011: commented from original code */

/* Two constant table size for 10^X calculation */
#define TDSULUTIL_EXP_MAP_SIZE_Z1                        6

#define TDSULUTIL_EXP_MAP_SIZE_Z2                        31

#if 0
#ifdef FEATURE_CM_SUPPORTED
/*=========================================================================

FUNCTION UL_GET_UL_CM_DPCCH_SLOT_FMT

DESCRIPTION
  This function calculates the DPCCH slot format in compressed mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
extern uint8 ul_get_ul_cm_dpcch_slot_fmt(void);
#endif /* FEATURE_CM_SUPPORTED */
#endif
/* - defang 01/13/2011 */

/*=========================================================================

FUNCTION      UL_GET_LONGEST_TTI

DESCRIPTION
  This function gets the largest TTI from the current CCTrCh.

DEPENDENCIES
  tdsl1_ul_curr_cctrch_ptr should point to the current CCTrCh.

RETURN VALUE
  l1_tti_enum_type value of the longest TTI.

SIDE EFFECTS
  None

=========================================================================*/
tdsl1_tti_enum_type tdsulutil_get_longest_tti
(
  void
);

/*=========================================================================

FUNCTION      tdsulutil_get_minimum_tti

DESCRIPTION
  This function gets the minimum TTI from the current CCTrCh.

DEPENDENCIES
  tdsl1_ul_curr_cctrch_ptr should point to the current CCTrCh.

RETURN VALUE
  l1_tti_enum_type value of the minimum TTI.

SIDE EFFECTS
  None

=========================================================================*/
tdsl1_tti_enum_type tdsulutil_get_minimum_tti
(
  void
);

/*=========================================================================

FUNCTION      tdsulutil_calc_max_tti

DESCRIPTION
  This function calculates the maximum TTI from the current CCTrCH according
  to RRC configuration. 
  This function is only used for activation time calculation.
  Each TTI configuration is not got from local copy but CPHY_SETUP_REQ message 
  pointer.

DEPENDENCIES
  None.

RETURN VALUE
  tdsl1_tti_enum_type value of the maximum TTI.

SIDE EFFECTS
  None

=========================================================================*/
tdsl1_tti_enum_type tdsulutil_calc_max_tti(tdsmac_ul_tfcs_type *ul_tfcs_ptr);

uint8 tdsulutil_get_abs_midamble_shift
(
  tdsl1_midamble_ss_burst_struct_type *midamble_and_burst_ptr,
  tdsl1_sf_enum_type sf,
  uint8 chan_code
);

/*=========================================================================

FUNCTION tdsulutil_gcd

DESCRIPTION
  This function computes the GCD(greatest common divisor) of two positive 
  integers using Euclid's division algorithm.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
uint32 tdsulutil_gcd
(
  uint32 a,
  uint32 b
);

/*=========================================================================

FUNCTION      tdsulutil_get_pccpch_tx_pwr

DESCRIPTION
  This function provide P-CCPCH Tx power to SRCH to calculate pathloss.

DEPENDENCIES
  

RETURN VALUE
  PCCPCH Tx power in dBm.

SIDE EFFECTS
  None

=========================================================================*/
uint8 tdsulutil_get_pccpch_tx_pwr(void);

/*=========================================================================

FUNCTION      tdsulutil_get_slot_info

DESCRIPTION
 This function provide slot info to SRCH

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void tdsulutil_get_slot_info(uint8 *num_ts, tdsl1_ul_dpch_ts_struct_type *ts_list);


/*=========================================================================

FUNCTION      tdsulutil_set_nw_config_max_tx_pwr

DESCRIPTION
  This function is to save max UE tx power configured by NW.

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void tdsulutil_set_nw_config_max_tx_pwr(int8 nw_cfg_max_tx_pwr);


/*=========================================================================

FUNCTION      tdsulutil_get_nw_config_max_tx_pwr

DESCRIPTION
  This function provide max UE tx power configured by NW to MEAS.

DEPENDENCIES
  

RETURN VALUE
  Tx power in dBm.

SIDE EFFECTS
  None

=========================================================================*/
int8 tdsulutil_get_nw_config_max_tx_pwr(void);

/*===========================================================================
FUNCTION	tdsulutil_dB_10log10

DESCRIPTION 

  This function taking in integer and derive the log 10 scale and multiplies
  with 10 for "digital" (dBm) power calculation.

  Output data is 10*log10(x) in 32Q23 format. Uses 10*log10(x)=3.0103*log2(x) 
  approximation to calculate. Uses lookup table for log2 with 7 MSB of input.

DEPENDENCIES 
  None

RETURN VALUE 
  dB Q12

SIDE EFFECTS 
  None
===========================================================================*/
int32 tdsulutil_dB_10log10(int32 x);

/*===========================================================================
FUNCTION	tdsulutil_dB_exp10_Q4

DESCRIPTION This function figure out 10^(power_q4) by table mapping

Input data format is dB Q4. The real range is -4.15~4.15. Use bit6~bit4 for 
integral part, bit3~bit0 for fractional part.

Output data format is dB Q12

DEPENDENCIES 
None

RETURN VALUE 
dB Q12

SIDE EFFECTS 
None
===========================================================================*/
uint64 tdsulutil_dB_exp10_Q4(int16 power_q4);

/*===========================================================================
FUNCTION tdsulutil_check_subframe_order

DESCRIPTION This function check the subframe order between 2 subframe numbers

Consider the distance between 2 subframe is less then 8192/2.
If subframe1 is earlier than or equeal to subframe2, then return TRUE
If subframe1 is later than subframe2, then return FALSE

DEPENDENCIES 
None

RETURN VALUE 
TRUE/FALSE

SIDE EFFECTS 
None
===========================================================================*/
boolean tdsulutil_check_subframe_order(uint16 subframe1, uint16 subframe2);


#endif /* TDS_UL_UTILS_H */
