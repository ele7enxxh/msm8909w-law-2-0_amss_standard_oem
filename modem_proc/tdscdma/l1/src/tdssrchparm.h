#ifndef TDSSRCH_PARM_H
#define TDSSRCH_PARM_H
/*===========================================================================
   FILE: tdssrchparm.h

GENERAL DESCRIPTION:
      This header file contains parameter definitions used by search module.
      
      
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c)2000-2011 by Qualcomm Technologies Incorporated. All Rights Reserved.
      
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "customer.h"
//#include "tdsrxdiv.h" // zdang_CR

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
             CELL Reselection SEARCHER PARAMETERS
--------------------------------------------------------------------------*/

/****************************************************
             PN Async Parameters                                 
****************************************************/

#define TDSSRCH_PN_NUM_RESULT_PER_TASK 6

#define TDSSRCHACQ_MAX_VERIFY_SRCH  1
  /* maximum number of "Verify" searches in any given frequency offset */
   
/*-------------------------------------------------------------------------
               Parameters for Calulating CPICH RSCP
-------------------------------------------------------------------------*/
#define TDSSRCH_MIN_CPICH_RSCP  ((tdsl1m_query_rel5_supported()) ? 242 : 232)
/*  Minimum CPICH RSCP, corresponding to -121dBm */
  /* Minimum CPICH RSCP, corresponding to -116dBm */
/*rscp range [-5,91]*/
#define TDSSRCH_MIN_PCCPCH_RSCP  -5

/* used for meas reporting purposes */
#define TDSSRCH_MIN_CPICH_RSCP_RPTING 121
/*Xiaoning Changed, TBD*/
#define TDSSRCH_MIN_PCCPCH_RSCP_RPTING -5

#define TDSSRCH_MIN_CPICH_ECIO  49

  /* Minimum Usable CPICH Ec/Io Value for CPICH RSCP calculation,
     Corresponding to -24.5 dB
   */
#define TDS_NO_CPICH_TX_PWR (0xFF)

#define TDSSRCH_PATHLOSS_NOT_VALID (0xFFFF)
#define TDSSRCH_MAX_CPICH_PATHLOSS 158
#define TDSSRCH_MIN_CPICH_PATHLOSS 46

#define TDSSRCH_RSCP_FILT_LEN  5
  /* Filter length for RSCP. TBD. */

#define TDSSRCHDCH_PATHLOSS_MIN_VAL (46)
#define TDSSRCHDCH_PATHLOSS_MAX_VAL (158)
#define TDSSRCHDCH_PATHLOSS_TABLE_OFFSET (38)
#define TDSSRCHDCH_PATHLOSS_TBL_MAX_SIZE (120)
#define TDSSRCHDCH_SMALLEST_PATHLOSS (8)

#ifdef FEATURE_JDS_OPT
#define TDSSRCH_DEMOD_MIN_VALUE  (-128*256)
#endif
/*--------------------------------------------------------------------------
                Search Operation Control Parameter
--------------------------------------------------------------------------*/
#define  TDSSRCH_ASET_SRCH_PERIOD  2
  /* Heartbeat count for Active Set search in BCH state,corresponding to 10ms */
 
/*---------------------------------------------------------------------

         Search parameters for RAT switch acquisition 

---------------------------------------------------------------------*/
#define TDSSRCH_ANT_SEL_DIV (tdsrxdiv_ant_id)

/*--------------------------------------------------------------------------
                Search Parameters used during DRX
--------------------------------------------------------------------------*/

/* Search window size for active set cells, in chipx8 */
#define TDSSRCHZZ_ASET_WIN_SIZE  (192*8)

/*--------------------------------------------------------------------------
                Search Parameters used for DCH
--------------------------------------------------------------------------*/

#define TDSSRCHDCH_MAX_INTRAF_EVT  8 

#define TDSSRCHDCH_MAX_INTERF_EVT  8

/*--------------------------------------------------------------------------
              Other Miscellaneous Parameters 
--------------------------------------------------------------------------*/

/* M-peaks per task async mdsp variable range=[1 ..7]
in shoulder mode we can have total 3M i.e 21 peaks per antenna */
#define TDSSRCH_MAX_PILOT_PEAKS (3*TDSSRCH_PN_NUM_RESULT_PER_TASK)

/* For this intermediate release this is the RF AGC threshold below
  which, in idle mode, searcher would declare an out of service area 
  indication to RRC */

/* #define  SRCH_ENG_THRESH_FOR_OUT_OF_SERVICE  SRCH_PATH_THRESHOLD  */
/* For this intermediate release this is the energy threshold below
  which, in idle mode, searcher would declare an out of service area 
  indication to RRC */


#define TDSSRCH_TS_CNT_ADJ   0
  /* Event Time Stamp Adjustment which takes neighbor cell measurement 
     frequency into account, in units of 10ms. It is set to half of 
     neighbor measurement period. ( needs a further look)
   */
  
#define TDSSRCH_FREQ_SCAN_RAW_SCAN_STEP  4
  /* RAW scan Incremental step for Frequency scan. Each unit corresponds to 
     200KHz. "10" corresponds to 2MHz. 4 corresponds to 800 KHz step size.
   */
#define TDSSRCH_FREQ_SCAN_FINE_SCAN_STEP 1
  /* FINE scan Incremental step for Frequency scan. Each unit corresponds to 
     200KHz.
   */

#define TDSSRCH_FREQ_SCAN_FINE_SCAN_RANGE_LEFT  1
#define TDSSRCH_FREQ_SCAN_FINE_SCAN_RANGE_RIGHT  2
  /* Fine scan range around the specified frequency. Each unit corresponds to
     200KHz.
   */
   

/* The following two thresholds are to be used for service requests */ 

#define TDSSRCH_FREQ_SCAN_AGC_THRESH_FOR_SRV_REQ  (-103)
  /* AGC threshold used for WCDMA carrier frequency, in dBm.
   */

#define TDSSRCH_FINE_FREQ_SCAN_AGC_THRESH_FOR_SRV_REQ (-98)

/* The following two thresholds are to be used for PLMN search requests */

#define TDSSRCH_FREQ_SCAN_AGC_THRESH_FOR_PLMN_SRCH  (-100)
  /* AGC threshold used for WCDMA carrier frequency, in dBm.
   */

#define TDSSRCH_FINE_FREQ_SCAN_AGC_THRESH_FOR_PLMN_SRCH (-95)


/* The following two thresholds are to be used for PLMN search requests */

#define TDSSRCH_FREQ_SCAN_AGC_THRESH_FOR_BPLMN_SRCH  (-100)
/* AGC threshold used for WCDMA carrier frequency, in dBm.  */

#define TDSSRCH_FINE_FREQ_SCAN_AGC_THRESH_FOR_BPLMN_SRCH (-95)


#define TDSSRCH_WCDMA_RSCP_FILT_LEN 5 
  /* Value of N for the (N-1)/N Filter Coefficient for ASET filtered RSCP 
     used for UI display.
   */

/*+ huichunc add*/
#define TDSSRCH_TDSCDMA_MAX_FILT_SIZE 3
/*- huichunc add*/
 /* As per 3GPP, atmost 2 filter coefficients are allowed. Add one to this
    as the default, we get 3    */

#define TDSSRCH_FHO_TX_POWER_MARGIN  2
#define TDSSRCH_FHO_SIR_TARGET2_OFFSET  3
#endif /* TDSSRCH_PARM_H */

