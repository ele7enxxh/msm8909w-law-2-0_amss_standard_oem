#ifndef CMAPI_H
#define CMAPI_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
  @file   cmapi.h
  @brief  This header file contains all the definitions necessary for clients 
          to interface with the Common Modem module.
*/

/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.

      The cmapi_mainpage.dox file contains the group/module descriptions that
      are displayed in the output PDF generated using Doxygen and LaTeX. To
      edit or update any of the group/module text in the PDF, edit the
      cmapi_mainpage.dox file or contact Tech Pubs.
=============================================================================*/

/*=============================================================================
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*=============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/cmapi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/13   dm      (Tech Pubs) Edited/added Doxygen comments and markup.
07/12/13   sd      (Tech Pubs) Continued edit.
06/04/13   sd      (Tech Pubs) Continued edit.
06/04/13   sd      (Tech Pubs) Edited/added Doxygen comments and markup.
05/17/13   rj      Add CMAPI to set LTE FGI
06/15/12   rj      Add additional LTE parameters (FR 2303) 
05/25/12   rj      Add CMAPI frequency scan API
05/16/12   rj      Add PHYCellId / SINR to cmapi_signal_info_s_type
04/20/12   rj      Add changes to deprecate cm_get_current_service_status
02/21/12   rj      Add DRX Parameter for LTE Serving Cell
12/06/11   rj      Add PA Gain State within TX PWR
10/18/11   rj      Add additional fields for LTE Serving Cell Freq 
07/28/11   rj      Added changes for LTE CDMA Neighbors 
06/10/11   rj      Update cmapi_signal_info_s_type to include phase for each 
                   RX chain
06/10/11   rj      Change Header from Call Manager to Common Modem
03/31/11   np      Added LTE Signal Reporting Config API
03/11/11   xs      Added diversity control API
03/08/11   xs      Added API support for signal info query

=============================================================================*/

#include "comdef.h"   /* Definition for basic types and macros */
#include "cmapi_types.h"

#include <amssassert.h>
#include <atomic_ops.h>

/** @addtogroup cmapi_constants
@{ */

/** Invalid signal information value. */
#define CMAPI_SIGNAL_INFO_INVALID_VALUE    0x80000000//-2147483648

/** Invalid band value. */
#define CMAPI_BAND_INVALID_VALUE       -1

/** Invalid channel value. */
#define CMAPI_CHANNEL_INVALID_VALUE    -1

/** Macro for creating a bitmask by shifting 1 left by the number indicated 
    in the val value. */
#define CMAPI_BM( val ) ( 1<< ((int)(val)) )

/** Number of cells to be reported per frequency; or in the
    case of GSM, the number of frequencies in a frequency group. */
#define CMAPI_LTE_NGBR_IND_NUM_CELLS 8

/** Number of frequencies to report per RAT; or in the 
    case of GSM, the number of frequencies in a frequency group. */
#define CMAPI_LTE_NGBR_IND_NUM_FREQS 2

/** Number of LTE layers. */
#define CMAPI_LTE_MAX_NUM_LAYER  2

/** Maximum number of reported cells in WCDMA. */
#define CMAPI_WCDMA_MAX_CELLS_REPORTED  8

/** Maximum number of active set cells in WCDMA. */
#define CMAPI_WCDMA_MAX_ASET_CELLS      6

/** Interfrequency search in WCDMA. */
#define CMAPI_WCDMA_SRCH_INTER_FREQ_MAX 2

/** Maximum number of LTE frequencies reported in WCDMA. */
#define CMAPI_WCDMA_SRCH_LTE_EARFCN_MAX 4

/** WCDMA invalid signal information. */
#define CMAPI_WCDMA_INVALID_SIGNAL_INFO 0xFFFF

/** Invalid phase information. */
#define CMAPI_PHASE_INFO_INVALID        0xFFFFFFFF 

/** Invalid timing advance information. */
#define CMAPI_TIMING_ADVANCE_INVALID    0xFFFFFFFF 

/** Invalid CMAPI value. */
#define CMAPI_INVALID_VALUE 0xFFFF

/** Invalid power amplifier state information. */
#define CMAPI_PA_STATE_INFO_INVALID     0xFF 

/* RAT bitmap */

/** CMAPI identifier for LTE for capturing scan data.  */
#define CMAPI_LTE_SCAN   0x1

/** CMAPI identifier for WCDMA for capturing scan data. */
#define CMAPI_WCDMA_SCAN 0x2

/** CMAPI identifier for GSM/EDGE radio access network for capturing scan 
    data. */
#define CMAPI_GERAN_SCAN 0x4

/* Per 1X Folks, This should not change */
/** Minimum CDMA received signal strength indicator value. */
#define CMAPI_CDMA_RSSI_MIN_VALUE 125

/** Maximum time slots for the TD-SCDMA uplink. */
#define CMAPI_TDSSRCH_UL_MAX_NUM_TS  6

/** Maximum number of eMBSFN areas */
#define CMAPI_LTE_EMBMS_MAX_MBSFN_AREA      8

/** Maximum number of PMCHs per eMBSFN area */
#define CMAPI_LTE_MAX_PMCH_PER_MBSFN_AREA   15

/** Maximum number of active PMCHs */
#define CMAPI_LTE_MAX_ACTIVE_PMCH           8

/** Maximum active eMBMS sessions */
#define CMAPI_LTE_EMBMS_MAX_ACTIVE_SESSIONS 8

/** Maximum number of secondary cells for LTE.
 *  This should be set to the same value as LTE_ML1_MAX_SCC */
#define CMAPI_LTE_ML1_MAX_SCC               2

/** Macro for checking whether the memory is 32-bit aligned. */
#define IS_ALIGNED(mem_ptr) \
  (( ((uint32)(mem_ptr)) & 0x00000003)? FALSE : TRUE ) 

/** @} */ /* end_addtogroup cmapi_constants */

/** @addtogroup cmapi_datatypes
@{ */

/** Paging interval UE ID.
*/
typedef uint16 cmapi_lte_l1_drx_ue_id_t;

/** System mode.
*/
typedef enum cmapi_sys_mode_e{

  CMAPI_SYS_MODE_CDMA,  /**< CDMA mode. */
  CMAPI_SYS_MODE_HDR,   /**< HDR mode. */   
  CMAPI_SYS_MODE_GSM,   /**< GSM mode. */    
  CMAPI_SYS_MODE_WCDMA, /**< WCDMA mode. */   
  CMAPI_SYS_MODE_LTE,   /**< LTE mode. */
  CMAPI_SYS_MODE_TDS    /**< TD-SCDMA mode. */
}cmapi_sys_mode_e_type;

/** Supported Rx chains.
*/
typedef enum cmapi_rx_chain_e{

  CMAPI_RX_CHAIN_0 = 0, /**< Rx chain 0. */   
  CMAPI_RX_CHAIN_1,     /**< Rx chain 1. */
  CMAPI_RX_CHAIN_2,     /**< Rx chain 2. */
  CMAPI_RX_CHAIN_3,     /**< Rx chain 3. */
  CMAPI_RX_CHAIN_MAX    /**< For internal use only. */
        
}cmapi_rx_chain_e_type;

/** Use Rx chains enum. This is used to build the upper nibble in the 
 * cmapi_rx_diversity_bit_mask_e bitmask below.
*/
typedef enum cmapi_use_rx_chain_e{

  CMAPI_USE_RX_CHAIN_0 = 0x10,     /**< Rx chain 0. */   
  CMAPI_USE_RX_CHAIN_1 = 0x20,     /**< Rx chain 1. */
  CMAPI_USE_RX_CHAIN_2 = 0x40,     /**< Rx chain 2. */
  CMAPI_USE_RX_CHAIN_3 = 0x80,     /**< Rx chain 3. */
  CMAPI_USE_RX_CHAIN_MAX    /**< For internal use only. */
        
}cmapi_use_rx_chain_e_type;

/** Rx chain diversity information. The lower 4 bits are for legacy scenarios
 * where one or more Rx chains are turned on and in use.
 * The upper 4 bits were added to support a special TDS scenario where 
 * both Rx chains are turned on but only primary is in use. In this case,
 * the lower 4 bits denote which Rx Chains are turned on and the upper 4 bits
 * denote which Rx chains are in use.
*/
typedef enum cmapi_rx_diversity_bit_mask_e{

  CMAPI_BM_RX_CHAIN_NONE = 0,                                 
  /**< Signal information is not available on either Rx chain. */

  CMAPI_BM_RX_CHAIN_0 = CMAPI_BM(CMAPI_RX_CHAIN_0),           
  /**< Signal information is available on Rx chain 0. */

  CMAPI_BM_RX_CHAIN_1 = CMAPI_BM(CMAPI_RX_CHAIN_1),           
  /**< Signal information is available on Rx chain 1. */

  CMAPI_BM_RX_CHAIN_2 = CMAPI_BM(CMAPI_RX_CHAIN_2),           
  /**< Signal information is available on Rx chain 2. */

  CMAPI_BM_RX_CHAIN_3 = CMAPI_BM(CMAPI_RX_CHAIN_3),           
  /**< Signal information is available on Rx chain 3. */

  CMAPI_BM_RX_CHAIN_ALL = CMAPI_BM(CMAPI_RX_CHAIN_0) |
                                CMAPI_BM(CMAPI_RX_CHAIN_1),    
  /**< Signal information is available on Rx chains 0 and 1. */

  CMAPI_BM_RX_CHAIN_ALL_SV = CMAPI_BM(CMAPI_RX_CHAIN_2) |
                                CMAPI_BM(CMAPI_RX_CHAIN_3),   
  /**< Signal information is available on Rx chains 2 and 3. */

  CMAPI_BM_RX_CHAIN_PRX_ONLY_BOTH_CHAINS_ON = CMAPI_USE_RX_CHAIN_0 | CMAPI_BM_RX_CHAIN_ALL
  /**< Rx Chains 0 and 1 are enabled but only use PRx. */

}cmapi_rx_diversity_bit_mask_e_type;

/** Specifies how often the LTE signal must be reported to the upper layers.
*/
typedef enum  
{
  CMAPI_LTE_SIGNAL_REPORTING_RATE_DEFAULT = 0, /**< Reports using the default
                                                    configuration. */
  CMAPI_LTE_SIGNAL_REPORTING_RATE_1_SEC   = 1, /**< Reports every 1 sec. */
  CMAPI_LTE_SIGNAL_REPORTING_RATE_2_SEC   = 2, /**< Reports every 2 sec. */
  CMAPI_LTE_SIGNAL_REPORTING_RATE_3_SEC   = 3, /**< Reports every 3 sec. */
  CMAPI_LTE_SIGNAL_REPORTING_RATE_4_SEC   = 4, /**< Reports every 4 sec. */
  CMAPI_LTE_SIGNAL_REPORTING_RATE_5_SEC   = 5, /**< Reports every 5 sec. */
} cmapi_lte_signal_reporting_rate_e;

/** Indicates the required averaging period that must be used for the LTE 
    signal. 
*/
typedef enum  
{
  CMAPI_LTE_SIGNAL_AVERAGING_PERIOD_DEFAULT = 0, /**< Averages using the default
                                                      configuration. */
  CMAPI_LTE_SIGNAL_AVERAGING_PERIOD_1_SEC   = 1, /**< Averages over 1 sec. */
  CMAPI_LTE_SIGNAL_AVERAGING_PERIOD_2_SEC   = 2, /**< Averages over 2 sec. */
  CMAPI_LTE_SIGNAL_AVERAGING_PERIOD_3_SEC   = 3, /**< Averages over 3 sec. */
  CMAPI_LTE_SIGNAL_AVERAGING_PERIOD_4_SEC   = 4, /**< Averages over 4 sec. */
  CMAPI_LTE_SIGNAL_AVERAGING_PERIOD_5_SEC   = 5, /**< Averages over 5 sec. */

} cmapi_lte_signal_averaging_period_e;

/** Indicates the LTE On Duration timer.
*/
typedef enum
{
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF1   = 1, 
  /**< LTE On Duration timer on physical downlink control channel (PDCCH) 
         subframe 1. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF2   = 2,
  /**< LTE On Duration timer on PDCCH subframe (PSF) 2. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF3   = 3,
  /**< LTE On Duration timer on PSF 3. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF4   = 4,
  /**< LTE On Duration timer on PSF 4. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF5   = 5,
  /**< LTE On Duration timer on PSF 5. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF6   = 6,
  /**< LTE On Duration timer on PSF 6. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF8   = 8,
  /**< LTE On Duration timer on PSF 8. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF10  = 10,
  /**< LTE On Duration timer on PSF 10. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF20  = 20,
  /**< LTE On Duration timer on PSF 20. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF30  = 30,
  /**< LTE On Duration timer on PSF 30. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF40  = 40,
  /**< LTE On Duration timer on PSF 40. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF50  = 50,
  /**< LTE On Duration timer on PSF 50. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF60  = 60,
  /**< LTE On Duration timer on PSF 60. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF80  = 80,
  /**< LTE On Duration timer on PSF 80. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF100 = 100,
  /**< LTE On Duration timer on PSF 100. */
  CMAPI_LTE_L1_ON_DURATION_TIMER_PSF200 = 200
  /**< LTE On Duration timer on PSF 200. */
} cmapi_lte_l1_on_duration_timer_e;

/** Indicates the LTE DRX Inactivity timer.
*/
typedef enum
{
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF1    = 1,
  /**< LTE DRX Inactivity timer on PSF 1. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF2    = 2,
  /**< LTE DRX Inactivity timer on PSF 2. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF3    = 3,
  /**< LTE DRX Inactivity timer on PSF 3. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF4    = 4,
  /**< LTE DRX Inactivity timer on PSF 4. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF5    = 5,
  /**< LTE DRX Inactivity timer on PSF 5. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF6    = 6,
  /**< LTE DRX Inactivity timer on PSF 6. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF8    = 8,
  /**< LTE DRX Inactivity timer on PSF 8. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF10   = 10,
  /**< LTE DRX Inactivity timer on PSF 10. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF20   = 20,
  /**< LTE DRX Inactivity timer on PSF 20. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF30   = 30,
  /**< LTE DRX Inactivity timer on PSF 30. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF40   = 40,
  /**< LTE DRX Inactivity timer on PSF 40. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF50   = 50,
  /**< LTE DRX Inactivity timer on PSF 50. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF60   = 60,
  /**< LTE DRX Inactivity timer on PSF 60. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF80   = 80,
  /**< LTE DRX Inactivity timer on PSF 80. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF100  = 100,
  /**< LTE DRX Inactivity timer on PSF 100. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF200  = 200,
  /**< LTE DRX Inactivity timer on PSF 200. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF300  = 300,
  /**< LTE DRX Inactivity timer on PSF 300. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF500  = 500,
  /**< LTE DRX Inactivity timer on PSF 500. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF750  = 750,
  /**< LTE DRX Inactivity timer on PSF 750. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF1280 = 1280,
  /**< LTE DRX Inactivity timer on PSF 1280. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF1920 = 1920,
  /**< LTE DRX Inactivity timer on PSF 1920. */
  CMAPI_LTE_L1_DRX_INACTIVITY_TIMER_PSF2560 = 2560
  /**< LTE DRX Inactivity timer on PSF 2560. */
} cmapi_lte_l1_drx_inactivity_timer_e;

/** Indicates the LTE DRX retransmission (RETX) timer.
*/
typedef enum
{
  CMAPI_LTE_L1_DRX_RETX_TIMER_SF1  = 1,
  /**< LTE DRX RETX timer on subframe (SF) 1. */
  CMAPI_LTE_L1_DRX_RETX_TIMER_SF2  = 2,
  /**< LTE DRX RETX timer on SF 2. */
  CMAPI_LTE_L1_DRX_RETX_TIMER_SF4  = 4,
  /**< LTE DRX RETX timer on SF 4. */
  CMAPI_LTE_L1_DRX_RETX_TIMER_SF6  = 6,
  /**< LTE DRX RETX timer on SF 6. */
  CMAPI_LTE_L1_DRX_RETX_TIMER_SF8  = 8,
  /**< LTE DRX RETX timer on SF 8. */
  CMAPI_LTE_L1_DRX_RETX_TIMER_SF16 = 16,
  /**< LTE DRX RETX timer on SF 16. */
  CMAPI_LTE_L1_DRX_RETX_TIMER_SF24 = 24,
  /**< LTE DRX RETX timer on SF 24. */
  CMAPI_LTE_L1_DRX_RETX_TIMER_SF33 = 33
  /**< LTE DRX RETX timer on SF 33. @newpage */
} cmapi_lte_l1_drx_retx_timer_e;


/** Indicates the LTE DRX long cycle.
*/
typedef enum
{
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF10   = 10,   /**< LTE DRX long cycle on SF 10. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF20   = 20,   /**< LTE DRX long cycle on SF 20. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF32   = 32,   /**< LTE DRX long cycle on SF 32. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF40   = 40,   /**< LTE DRX long cycle on SF 40. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF64   = 64,   /**< LTE DRX long cycle on SF 64. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF80   = 80,   /**< LTE DRX long cycle on SF 80. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF128  = 128,  /**< LTE DRX long cycle on SF 128. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF160  = 160,  /**< LTE DRX long cycle on SF 160. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF256  = 256,  /**< LTE DRX long cycle on SF 256. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF320  = 320,  /**< LTE DRX long cycle on SF 320. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF512  = 512,  /**< LTE DRX long cycle on SF 512. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF640  = 640,  /**< LTE DRX long cycle on SF 640. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF1024 = 1024, /**< LTE DRX long cycle on SF 1024. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF1280 = 1280, /**< LTE DRX long cycle on SF 1280. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF2048 = 2048, /**< LTE DRX long cycle on SF 2048. */
  CMAPI_LTE_L1_DRX_LONG_CYCLE_SF2560 = 2560  /**< LTE DRX long cycle on SF 2560. */

} cmapi_lte_l1_drx_long_cycle_e;


/** Indicates the LTE DRX short cycle.
*/
typedef enum
{
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF2   = 2,   /**< LTE DRX short cycle on SF 2. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF5   = 5,   /**< LTE DRX short cycle on SF 5. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF8   = 8,   /**< LTE DRX short cycle on SF 8. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF10  = 10,  /**< LTE DRX short cycle on SF 10. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF16  = 16,  /**< LTE DRX short cycle on SF 16. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF20  = 20,  /**< LTE DRX short cycle on SF 20. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF32  = 32,  /**< LTE DRX short cycle on SF 32. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF40  = 40,  /**< LTE DRX short cycle on SF 40. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF64  = 64,  /**< LTE DRX short cycle on SF 64. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF80  = 80,  /**< LTE DRX short cycle on SF 80. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF128 = 128, /**< LTE DRX short cycle on SF 128. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF160 = 160, /**< LTE DRX short cycle on SF 160. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF256 = 256, /**< LTE DRX short cycle on SF 256. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF320 = 320, /**< LTE DRX short cycle on SF 320. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF512 = 512, /**< LTE DRX short cycle on SF 512. */
  CMAPI_LTE_L1_DRX_SHORT_CYCLE_SF640 = 640  /**< LTE DRX short cycle on SF 640. 
                                                 @newpage */

} cmapi_lte_l1_drx_short_cycle_e;

/* 3GPP TS 36.331 */
/** Narrowband (NB) parameter used to specify the number of paging groups. 
    Refer to @xhyperref{S3,[S3]}, Section 6.3.2.
*/
typedef enum
{
  CMAPI_LTE_L1_NB_FOURT = 0,
  /**< Valid value for NB is defined as 4T where T is the default paging 
       cycle. */

  CMAPI_LTE_L1_NB_TWOT,
  /**< Valid value for NB is defined as 2T where T is the default paging 
       cycle. */

  CMAPI_LTE_L1_NB_ONET,
  /**< Valid value for NB is defined as 1T where T is the default paging 
       cycle. */

  CMAPI_LTE_L1_NB_HALFT,
  /**< Valid value for NB is defined as T/2 where T is the default paging 
       cycle. */

  CMAPI_LTE_L1_NB_QUARTERT,
  /**< Valid value for NB is defined as T/4 where T is the default paging 
       cycle. */

  CMAPI_LTE_L1_NB_ONEEIGHTT,
  /**< Valid value for NB is defined as T/8 where T is the default paging 
       cycle. */

  CMAPI_LTE_L1_NB_ONSIXTEENTHT,
  /**< Valid value for NB is defined as T/16 where T is the default paging 
       cycle. */

  CMAPI_LTE_L1_NB_ONETHIRTYSECONDT
  /**< Valid value for NB is defined as T/32 where T is the default paging 
       cycle. */

} cmapi_lte_l1_nb_e;

/** CMAPI LTE CQI reporting modes.
*/
typedef enum
{
  CMAPI_LTE_L1_CQI_MODE_APERIODIC_RM12 = 0, /**< Aperiodic reporting mode 12. */
  CMAPI_LTE_L1_CQI_MODE_APERIODIC_RM20,     /**< Aperiodic reporting mode 20. */
  CMAPI_LTE_L1_CQI_MODE_APERIODIC_RM22,     /**< Aperiodic reporting mode 22. */
  CMAPI_LTE_L1_CQI_MODE_APERIODIC_RM30,     /**< Aperiodic reporting mode 30. */
  CMAPI_LTE_L1_CQI_MODE_APERIODIC_RM31,     /**< Aperiodic reporting mode 31. */
  CMAPI_PUSCH_CSF_MAX_REPORTING_MODES,      /**< Maximum number of reporting
                                                 modes. */
} cmapi_lte_l1_cqi_reporting_mode_aperiodic_e;

/** CQI format indicator. 
*/
typedef enum
{
  CMAPI_LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_WIDEBAND,
  /**< Periodic wideband. */

  CMAPI_LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND
  /**< Periodic sub-band. */

}cmapi_lte_l1_cqi_format_indicator_e;

/* Defined in lte_LL1_dl_pdsch_params_struct
  from Espresso UE LL-ML API Specification.  
  Refer to 36.213, Table 7.1-1. */
/** Downlink transmission mode. 
*/
typedef enum
{
  /*----------------------------------------------------------------------------
                            Values signalled over the air
   ---------------------------------------------------------------------------*/
  CMAPI_LTE_L1_DL_TM1 = 1,
    /**< Downlink transmission mode 1. */
  CMAPI_LTE_L1_DL_TM2 = 2,
    /**< Downlink transmission mode 2. */
  CMAPI_LTE_L1_DL_TM3 = 3,
    /**< Downlink transmission mode 3. */
  CMAPI_LTE_L1_DL_TM4 = 4,
    /**< Downlink transmission mode 4. */
  CMAPI_LTE_L1_DL_TM5 = 5,
    /**< Downlink transmission mode 5. */
  CMAPI_LTE_L1_DL_TM6 = 6,
    /**< Downlink transmission mode 6. */
  CMAPI_LTE_L1_DL_TM7 = 7,
    /**< Downlink transmission mode 7. */
  CMAPI_LTE_L1_DL_TM8_REL9 = 8,
    /**< Downlink transmission mode 8. */
  CMAPI_LTE_L1_DL_TRANSMISSIONMODE_SPARE1 = 9,
    /**< Spare downlink transmission mode for future use. */

  /*----------------------------------------------------------------------------
                            Translated values for L1
   ---------------------------------------------------------------------------*/
  
  CMAPI_LTE_L1_DL_TX_MODE_INVALID = 0,
  /**< Invalid mode. */

  CMAPI_LTE_L1_DL_TX_MODE_SINGLE_ANT_PORT0 = CMAPI_LTE_L1_DL_TM1,
  /**< Single antenna port. */

  CMAPI_LTE_L1_DL_TX_MODE_TD_RANK1 = CMAPI_LTE_L1_DL_TM2,
  /**< Transmit diversity. */

  CMAPI_LTE_L1_DL_TX_MODE_OL_SM = CMAPI_LTE_L1_DL_TM3,
  /**< Open-loop spatial multiplexing. */

  CMAPI_LTE_L1_DL_TX_MODE_CL_SM = CMAPI_LTE_L1_DL_TM4,
  /**< Close-loop spatial multiplexing. */

  CMAPI_LTE_L1_DL_TX_MODE_MU_MIMO = CMAPI_LTE_L1_DL_TM5,
  /**< Multi-user multiple input, multiple output. */

  CMAPI_LTE_L1_DL_TX_MODE_CL_RANK1_PC = CMAPI_LTE_L1_DL_TM6,
  /**< Close-loop rank 1 precoding. */

  CMAPI_LTE_L1_DL_TX_MODE_SINGLE_ANT_PORT5 = CMAPI_LTE_L1_DL_TM7,
  /**< Single-antenna port(5), beam forming. */

  CMAPI_LTE_L1_DL_TX_MAX_NUM_MODES = 9
  /**< Maximum number of modes. */

} cmapi_lte_l1_dl_tx_mode_e;

/** LTE level 1 Tx antenna selection.
*/
typedef enum {
  CMAPI_LTE_L1_TX_ANTENNA_SELECTION_CLOSED_LOOP,
  /**< Closed loop. */

  CMAPI_LTE_L1_TX_ANTENNA_SELECTION_OPEN_LOOP
  /**< Open loop. */

} cmapi_lte_l1_tx_antenna_selection_e;

/** Identifies whether the feature group indicator is for Release 8, Release 9, 
    or Release 10.
*/
typedef enum
{
  
  CMAPI_LTE_RRC_EXT_FGI_REL_8, /**< FGI for Release 8. */
  CMAPI_LTE_RRC_EXT_FGI_REL_9, /**< FGI for Release 9. */
  CMAPI_LTE_RRC_EXT_FGI_REL_10 /**< FGI for Release 10.*/
} cmapi_lte_fgi_rel_type_e;

/** Enums to identify LTE modulation types.
*/
typedef enum 
{
  CMAPI_LTE_MODULATION_BPSK = 0, /**< Binary Phase Shift Keying. */
  CMAPI_LTE_MODULATION_QPSK,     /**< Quadrature Phase Shift Keying. */
  CMAPI_LTE_MODULATION_16QAM,    /**< 16-state Quadrature Amplitude Modulation. */
  CMAPI_LTE_MODULATION_64QAM,    /**< 64-state Quadrature Amplitude Modulation. */
  CMAPI_LTE_MODULATION_UNKNOWN   /**< Unknown Modulation. */
}cmapi_lte_modulation_e_type;

/** Enum used to build the bitmask for cmapi_efs_read() Each bit corresponds to a file in EFS
  * that will be reread if set. 
*/
typedef enum 
{
  CMAPI_EFS_LTE_CATEGORY         = 0x1,
  CMAPI_EFS_LTE_CA_DISABLE       = 0x2,
  CMAPI_EFS_LTE_RESERVED_1       = 0x4,
  CMAPI_EFS_LTE_RESERVED_2       = 0x8,
  CMAPI_EFS_LTE_RESERVED_3       = 0x10,
  CMAPI_EFS_LTE_RESERVED_4       = 0x20,
  CMAPI_EFS_LTE_FGI_REL10        = 0x40,
  CMAPI_EFS_LTE_FGI_TDD_REL10    = 0x80,
  CMAPI_EFS_LTE_INVALID          = 0xFFFFFFFFFFFFFFFFuLL //To force enum to be a 64bit one.
}cmapi_efs_lte_files_e_type;

/** LTE modulation struct type.
*/
typedef struct
{
  uint8 num_layer;
  /**< Number of LTE layers. */

  cmapi_lte_modulation_e_type modulation[CMAPI_LTE_MAX_NUM_LAYER];
  /**< Modulation type. */

}cmapi_lte_mod_s;

/** LTE modulation info for uplink and downlink struct type.
*/
typedef struct
{
  cmapi_lte_mod_s   uplink;
  /**< LTE uplink modulation */

  cmapi_lte_mod_s   downlink;
  /**<  LTE downlink modulation */

}cmapi_lte_modulation_info_s;

/** LTE modulation info.
*/
typedef struct
{
  cmapi_lte_modulation_info_s lte_modulation;
  /**<  LTE modulation info. */

}cmapi_modulation_info_s;


/** WCDMA cell parameters (LTE neighbor).
*/
typedef struct
{
  uint16                             psc;
  /**< Primary scrambling code. */
  
  int16                            cpich_rscp;
  /**< Absolute power level of the common pilot channel as received by the UE 
       in dBm x10. Refer to @xhyperref{S1,[S1]}. */ /* 3GPP TS 25.304 */

  int16                            cpich_ecno;
  /**< Ratio of the received energy per PN chip for the common pilot channel to 
       the total received power spectral density at the UE antenna connector in 
       dB x10. Refer to @xhyperref{S1,[S1]}. */ /* 3GPP TS 25.304 */
 
  /** WCDMA cell parameters (LTE neighbor). */
  struct
  {
    int16                            srxlev;
    /**< Suitable receive level. */

    int32                            squal;
    /**< Squal */
  } idle; 

} cmapi_lte_ngbr_ind_w_cell_s; 

/** Stores CDMA cell parameters (LTE neighbor).
*/
typedef struct
{
  uint16                           pilot_pn_offset;
  /**< Neighbor cell pilot PN offset. Range: 0 to 511. */

  uint16                           pilot_pn_phase;
  /**< Neighbor cell pilot PN phase. Range: 0 to 32767. */

  int16                            pilot_strength;
  /**< Neighbor cell pilot Ec/Io. Range: 1 to 63. @newpagetable */

} cmapi_lte_ngbr_ind_c_cell_s;

/** Stores GSM cell parameters (LTE neighbor).
*/
typedef struct
{
  uint16 arfcn;
  /**< GSM frequency being reported. */

  boolean band_1900;
  /**< Indicates the band for the GSM ARFCN; only valid if the ARFCN value is 
       in the overlapping region. Values: \n
       - TRUE and the cell is in the overlapping region -- 1900 band \n
       - FALSE -- 1800 band @tablebulletend */

  boolean cell_id_valid;
  /**< Indicates whether the BSIC ID is valid. */

  uint8 bsic_id;
  /**< Base station identity code ID. */

  int16 rssi;
  /**< Measured received signal strength indicator value in dB x10. */

  /** Contains srxlev only. */
  struct
  {
    int16                            srxlev;
    /**< Suitable receive level. */
  } idle; 

} cmapi_lte_ngbr_ind_g_cell_s;

/** Stores eUTRAN cell parameters (LTE neighbor).
*/
typedef struct
{
  uint16 pci;
  /**< Physical cell ID. */

  int16 rsrq;
  /**< Current reference signal receive quality as measured by L1.
       The quantities are in dB x10.  Range: -20.0 to -3.0 dB. */
   
  int16 rsrp;
  /**< Current reference signal receive power in dBm x10 as measured by L1.
       Range: -44 to -140. */

  int16 rssi;
  /**< Current received signal strength indicator as measured by L1.
       Values are in dBm x10. Range: -120.0 to 0.  */

  int16 sinr;
  /**< Logarithmic value of SINR
       Values are in 1/5th of a dB. Range 0-250 which translates to -20dB - +30dB  */
  
  /** Contains srxlev only. */
  struct
  {
    int16                            srxlev;
    /**< Suitable receive level. */
  } idle; 
  
} cmapi_lte_ngbr_ind_l_cell_s;

/** LTE intraconfiguration parameters.
*/
typedef struct
{

  uint8 cell_resel_priority;
  /**< Priority for the serving frequency. */

  uint8 s_non_intra_search;
  /**< Threshold to control non-intrafrequency searches. */

  uint8 thresh_serving_low;
  /**< Serving frequency threshold is low. */

  uint8 s_intra_search;
  /**< Current cell threshold the measurement must fall below to consider 
       intrafrequency for reselection. */

  int32 Q_rxlevmin;
  /**< Minimum reference signal received power level for camping 
       (intrafrequency). */
  
  int8  Q_qualmin;
  /**< Minimum Ec/No level specified in the SIB for intrafrequency. */
  
  int8  Srxlev;
  /**< Suitable receive level. */
  
  int8  Squal;
  /**< Difference between the Ec/No level measured by the UE and the minimum  
       Ec/No level specified in the SIB (Q_qualmin). @newpagetable */

} cmapi_lte_ngbr_ind_intra_freq_idle_param_s;

/** LTE serving cell parameters.
*/
typedef struct
{
 
  uint32 earfcn;
  /**< EARFCN of the serving cell. Range: 0 to 65535. */
  
  uint16 mcc;
  /**< Mobile country code. */

  uint16 mnc;
  /**< Mobile network code. */

  uint16 tac;
  /**< Total access communication. */
  
  uint32 num_mnc_digits; 
  /**< Number of digits in mobile network code. */
  
  uint32 serving_cell_id;
  /**< LTE serving cell ID. This is the cell ID of the serving cell and can be 
       found in the cell list. Range: 0 to 503. */

  uint8 freq_band_ind;
  /**< Operating band of the serving cell. Range: 1 to 64. */

  uint8 dl_bandwidth; 
  /**< Transmission bandwidth configuration of the serving cell on the 
       downlink. Range: 0 to 5. */
 
  uint8 ul_bandwidth;
  /**< Transmission bandwidth configuration of the serving cell on the uplink. 
       Range: 0 to 5. */

 int8 serv_rssnr;
  /**< Average reference signal signal-to-noise ratio of the serving cell over 
       the last measurement period in decibels. Range: -10 to 30. */
 
  cmapi_lte_ngbr_ind_l_cell_s cell;
  /**< LTE cell parameters. */

  uint16  idle_drx_cycle_len;
  /**< Idle DRX cycle length in 10 ms radio-frame units. */ 

  uint16  cdrx_short_cycle_len;
  /**< Connected mode DRX short cycle length in 10 ms radio-frame units. */ 

  uint16  cdrx_long_cycle_len;
  /**< Connected mode DRX long cycle length in 10 ms radio-frame units.
       @newpagetable */ 

} cmapi_lte_serv_cell_freq_param_s;

/** Carrier aggregation secondary cell (CA SCell) deactivation timer settings.
*/
typedef enum
{
  CMAPI_CA_SCELL_DEACTIVATION_TIMER_2RF = 2,       /**< Two radio frames. */ 
  CMAPI_CA_SCELL_DEACTIVATION_TIMER_4RF = 4,       /**< Four radio frames. */ 
  CMAPI_CA_SCELL_DEACTIVATION_TIMER_8RF = 8,       /**< Eight radio frames. */ 
  CMAPI_CA_SCELL_DEACTIVATION_TIMER_16RF = 16,     /**< 16 radio frames. */ 
  CMAPI_CA_SCELL_DEACTIVATION_TIMER_32RF = 32,     /**< 32 radio frames. */ 
  CMAPI_CA_SCELL_DEACTIVATION_TIMER_64RF = 64,     /**< 64 radio frames. */ 
  CMAPI_CA_SCELL_DEACTIVATION_TIMER_128RF = 128,   /**< 128 radio frames. */ 
  CMAPI_CA_SCELL_DEACTIVATION_TIMER_INFINITY = 256 /**< Deactivation timer never
                                                        expires. */ 
} cmapi_ca_scell_deact_timer_e;

/** Carrier aggregation secondary cell information.
*/
typedef struct
{
  uint32 earfcn;
  /**< EARFCN of the CA SCell. Range: 0 to 65535. */

  uint32 cell_id;
  /**< LTE CA SCell cell ID that can be found in the cell list. 
       Range: 0 to 503. */

  uint8 freq_band_ind;
  /**< Operating band of the CA SCell. Range: 1 to 64. */

  uint8 dl_bandwidth; 
  /**< Transmission bandwidth configuration of the serving cell 
       on the downlink. Range: 0 to 5. */

  int8 serv_rssnr;
  /**< Average reference signal signal-to-noise ratio of the serving cell over 
       the last measurement period in decibels. Range: -10 to 30. */
 
  cmapi_lte_ngbr_ind_l_cell_s cell;
  /**< LTE cell parameters. */

  cmapi_ca_scell_deact_timer_e scell_deact_timer; 
  /**< SCell deactivation timer. */

  boolean ca_activated;
  /**< Indicates whether CA is activated or deactivated. */

} cmapi_lte_ca_scell_param_s;

/** LTE intrafrequency parameters.
*/
typedef struct
{
  boolean sib3;
  /**< Determines whether SIB3 has been received by the UE. */
  
  uint32 earfcn;
  /**< EARFCN of the serving cell. */
  
  uint16 serving_cell_id;
  /**< LTE serving cell ID. */

  uint8 num_lte_cells;
  /**< Number of LTE cells. */

  cmapi_lte_ngbr_ind_l_cell_s 
    cell[CMAPI_LTE_NGBR_IND_NUM_CELLS];
  /**< LTE cell parameters. */

  cmapi_lte_ngbr_ind_intra_freq_idle_param_s idle;
  /**< LTE intraconfiguration parameters. @newpagetable */	
  
} cmapi_lte_ngbr_ind_intra_freq_param_s;

/** Reselection parameters provided in SIB3.
*/
typedef struct
{

  uint8 threshX_low;
  /**< To be considered for reselection, the suitable receive level value of an
       evaluated lower priority cell must be greater than this value. */
 
  uint8 threshX_high;
  /**< To be considered for reselection, the suitable receive level value of an 
       evaluated higher priority cell must be greater than this value. */
 
  uint8 cell_resel_priority;
  /**< Cell reselection priority. */

} cmapi_lte_ngbr_ind_inter_freq_idle_param_s;

/** Idle DRX information.
*/
typedef struct
{
  uint16 paging_cycle; /**< UE paging cycle in milliseconds. */
  cmapi_lte_l1_nb_e nb; /**< Used to derive the number of paging groups. */
  cmapi_lte_l1_drx_ue_id_t ue_id; /**< UE identity (IMSI) mod 1024. */

} cmapi_lte_cphy_idle_drx_param_s;


/** DRX cycle and DRX start offset.
*/
typedef struct
{
  cmapi_lte_l1_drx_long_cycle_e long_drx_cycle; 
  /**< DRX cycle. */

  uint32 offset;
  /**< DRX start offset. @newpagetable */

} cmapi_lte_l1_drx_long_cycle_start_offset_s;

/** LTE interfrequency parameters.
*/
typedef struct
{
  uint32 earfcn;
  /**< EARFCN of the serving cell. */

  uint8 num_lte_cells;
  /**< Number of LTE cells. */

  cmapi_lte_ngbr_ind_l_cell_s 
    cells[CMAPI_LTE_NGBR_IND_NUM_CELLS];
  /**< LTE cell parameters. */

  cmapi_lte_ngbr_ind_inter_freq_idle_param_s idle;
  /**< Per-frequency idle parameter. */

} cmapi_lte_ngbr_ind_inter_freq_layer_param_s;

/** Interfrequency parameters for the neighbor power indication.
*/
typedef struct
{
  uint8 num_freqs;
  /**< Number of frequencies. */ 
  
  cmapi_lte_ngbr_ind_inter_freq_layer_param_s 
    freqs[CMAPI_LTE_NGBR_IND_NUM_FREQS];
  /**< Interfrequency neighbor information. @newpagetable */

} cmapi_lte_ngbr_ind_inter_freq_param_s;

/* 3GPPS TS 36.331 */
/** GSM/EDGE Radio Access Network (GERAN) frequency group.

  This defines a group of GERAN frequencies with associated priority and
  reselection parameters. This GERAN frequency group is analogous to
  a frequency in another RAT. Refer to @xhyperref{S3,[S3]}, Section 6.4.
*/
typedef struct
{
  uint8 cell_resel_priority;
  /**< Priority of this frequency group. */

  uint8 thresh_gsm_high;
  /**< Reselection threshold for high priority layers. */

 uint8 thresh_gsm_low;
  /**< Reselection threshold for low priority layers. */
 
  uint8 ncc_permitted;
  /**< Bitmask that specifies whether a neighbor with a particular network 
       color code is to be reported. Bit n set to 1 means that a neighbor 
       with NCC n is to be included in the report.

       This flag is synonymous with a blacklist in other RATs. */

} cmapi_lte_ngbr_ind_irat_gsm_idle_freq_group_s;

/** Idle mode inter-RAT per WCDMA frequency information.
*/
typedef struct
{

  uint8 cell_resel_priority;
  /**< Cell reselection priority. Range: 0 to 7. */

  uint16 thresh_Xhigh;
  /**< Reselection threshold for high priority layers. */

  uint16 thresh_Xlow;
  /**< Reselection threshold for low priority layers. */

}cmapi_lte_ngbr_ind_wcdma_freq_idle_info_s;


/** Per-frequency group GSM parameters.
*/
typedef struct
{
  uint8 num_gsm_arfcn;
  /**< Number of GSM ARFCNs indicated, and the size of the GSM array. */

  cmapi_lte_ngbr_ind_g_cell_s 
    arfcn[CMAPI_LTE_NGBR_IND_NUM_CELLS];
  /**< GSM cell parameters. */

  cmapi_lte_ngbr_ind_irat_gsm_idle_freq_group_s idle;
  /**< Per-group idle parameters. */

} cmapi_lte_ngbr_ind_gsm_freq_group_info_s;

/** Defines the mid-RSSI structure of every frequency (TD-SCDMA).
*/
typedef struct
{
  uint16 primary_frequency;
  /**< Primary frequency. */
  
  int16   serving_cell_primary_freq_Rssi;
  /**< Serving cell's primary frequency RSSI chain 0. */

  int16  serving_cell_primary_freq_Rssi_rx1;
  /**< Serving cell's primary frequency RSSI chain 1. */

}cmapi_tdssrch_freq_syncdlRssi_type;

/** Defines the Tx power for uplink timeslots (TD-SCDMA).
*/
typedef struct
{

  int16 tdssrch_uplink_tx_pwr[CMAPI_TDSSRCH_UL_MAX_NUM_TS];
  /**< Tx power for uplink timeslots. */

}cmapi_tdssrch_uplink_tx_power_type;

/** GSM parameters.
*/
typedef struct
{

  uint8 num_freq_groups;
  /**< Number of GSM frequency groups and the size of the frequency group 
       array. */

  cmapi_lte_ngbr_ind_gsm_freq_group_info_s freq_group[CMAPI_LTE_NGBR_IND_NUM_FREQS];
  /**< GSM neighbor frequency information. @newpagetable */
  
} cmapi_lte_ngbr_ind_gsm_info_s;

/** WCDMA frequency.
*/
typedef struct 
{
  uint16                         uarfcn;
  /**< WCDMA layer frequency. */

  uint8 num_wcdma_cells;
  /**< Number of WCDMA cells indicated and the size of the WCDMA array. */

  cmapi_lte_ngbr_ind_w_cell_s 
    cell[CMAPI_LTE_NGBR_IND_NUM_CELLS];
  /**< WCDMA cell parameters. */

  cmapi_lte_ngbr_ind_wcdma_freq_idle_info_s idle;
  /**< WCDMA frequency information for the inter-RAT Idle mode. */
  
} cmapi_lte_ngbr_ind_wcdma_freq_info_s;

/** WCDMA parameters.
*/
typedef struct
{
  uint8 num_wcdma_freqs;
  /**< Number of WCDMA frequencies and the size of the freq array. */

  cmapi_lte_ngbr_ind_wcdma_freq_info_s freq [CMAPI_LTE_NGBR_IND_NUM_FREQS];
  /**< WCDMA frequency. */

} cmapi_lte_ngbr_ind_wcdma_info_s;

/** CDMA frequency.
*/
typedef struct 
{
  uint16 channel_num;
  /**< Channel number. */

  uint16 band_class;
  /**< Band class. */

  uint8 num_cdma_cells;
  /**< Number of CDMA cells indicated and the size of the CDMA array. */

  cmapi_lte_ngbr_ind_c_cell_s 
    cell[CMAPI_LTE_NGBR_IND_NUM_CELLS];
  /**< CDMA cell parameters. */

} cmapi_lte_ngbr_ind_cdma_freq_info_s;

/** CDMA parameters.
*/
typedef struct
{
  uint8 num_cdma_freqs;
  /**< Number of CDMA frequencies and the size of the freq array. */

  cmapi_lte_ngbr_ind_cdma_freq_info_s freq [CMAPI_LTE_NGBR_IND_NUM_FREQS];
  /**< CDMA neighbor frequency information. */

} cmapi_lte_ngbr_ind_cdma_info_s;

/** Connected mode DRX dedicated information. Refer to @xhyperref{S2,[S2]}.
*/
  /* 3GPP TS 36.321 */
typedef struct
{
  boolean drx_enable;
  /**< Indicates whether to enable the Dedicate mode DRX. */

  cmapi_lte_l1_on_duration_timer_e on_duration_timer;
  /**< On Duration timer. The value is the number of PDCCH subframes. The  
       psf1 value corresponds to one PDCCH subframe, psf2 corresponds to two 
       PDCCH subframes, etc. Default: FFS. */ 

  cmapi_lte_l1_drx_inactivity_timer_e inactivity_timer;
  /**< DRX Inactivity timer. The value is the number of PDCCH subframes. The 
       psf1 value corresponds to one PDCCH subframe, psf2 corresponds to two 
       PDCCH subframes, etc. Default: FFS. */ 

  cmapi_lte_l1_drx_retx_timer_e retx_timer;
  /**< DRX Retransmission timer. The value is the number of PDCCH subframes. */ 

  cmapi_lte_l1_drx_long_cycle_start_offset_s long_drx_cycle_start_offset;
  /**< Long DRX cycle and DRX start offset. The long_drx_cycle value 
       is the number of subframes. The sf10 value corresponds to 10 subframes, 
       sf20 corresponds to 20 subframes, etc. If short_drx_cycle is configured, 
       the value is a multiple of the short_drx_cycle value. 

       The DRX start offset value is an integer. In TDD, this can point to a DL 
       or UL subframe. */

  boolean short_drx_cycle_enable;
  /**< Indicates whether short_drx_cycle is enabled. */

  cmapi_lte_l1_drx_short_cycle_e short_drx_cycle;
  /**< Short DRX cycle. */

  uint32 short_drx_cycle_timer;
  /**< DRX short cycle timer. The value is in multiples of short_drx_cycle. A 
       value of 1 corresponds to one short_drx_cycle value, 2 corresponds to 
       two short_drx_cycle values, etc. Default: FFS.*/

} cmapi_lte_cphy_drx_cfg_dedicated_param_s;

/** LTE PMCH (Physical Multicast Channel) BLER info */
typedef struct  
{
  uint32   pmch_id;
  /**< PMCH ID. */

  uint32   num_crc_pass;
  /**< Number of CRC passes. */

  uint32   num_crc_fail;
  /**< Number of CRC failures. */

} cmapi_lte_ml1_pmch_bler_info_s;

/**< Current signal strength info for a MBSFN (Multimedia Broadcast 
     Multicast Service Single Frequency Network) Area. */
typedef struct
{
  uint8 area_id;
  /**< MBSFN area id: 0 - 255 */

  int16 snr;
  /**< The average SNR of the serving cell over last 
       measurement period in decibels Q3[2 ^3] format
       result SNR in cnf = raw SNR value * 2^3
       Range -10..30 */

  int16 excess_snr;
  /**< The excess SNR of the serving cell over last 
       measurement period in decibels Q3[2 ^3] format
       Range -10..30 */

  int8 signal_level;
  /**< The signal level of the serving cell over last 
       measurement period 
       Range 0 to 5 */
 
} cmapi_lte_cphy_mbsfn_area_signal_strength_s;

/** Contains current signal quality at L1 for each MBSFN Area. */
typedef struct
{  
  uint8                                         num_mbsfn_areas;
  /** Number of MBSFN areas whose SNR values are populated in the array below */
  
  cmapi_lte_cphy_mbsfn_area_signal_strength_s   mbsfn_area_signal_strength[CMAPI_LTE_EMBMS_MAX_MBSFN_AREA];
  /** SNR info per MBSFN Area */

} cmapi_lte_rrc_mbsfn_snr_info_s;

/** LTE PMCH info for an area. */
typedef struct 
{
  uint16                                 area_id;
  /**< Area ID. */

  uint16                                 num_pmch;
  /**< Number of PMCHs in this area. */

  cmapi_lte_ml1_pmch_bler_info_s         pmch_bler[CMAPI_LTE_MAX_PMCH_PER_MBSFN_AREA];
  /**< Each PMCH info. */

} cmapi_lte_ml1_pmch_bler_per_area_info_s;

/** LTE PMCH BLER (Block error rate) info for all areas. */
typedef struct 
{
  uint32                                   num_area;
  /**< Number of areas in PMCH BLER info specified in the below array */

  cmapi_lte_ml1_pmch_bler_per_area_info_s  pmch_bler_per_area[CMAPI_LTE_EMBMS_MAX_MBSFN_AREA];  
  /**< PCMG bler stats for each area */

} cmapi_lte_ml1_pmch_bler_area_info_s;

/** LTE PMCH info */
typedef struct 
{
  boolean                              is_valid;
  /**< Indicates if there is any activated TMGI 
       (Temporary Mobile group Identity) */

  cmapi_lte_rrc_mbsfn_snr_info_s       mbsfn_snr_info;
  /**< Signal quality info */

  cmapi_lte_ml1_pmch_bler_area_info_s  pmch_bler_info;
  /**< PMCH bler info */

} cmapi_lte_mbsfn_snr_bler_info_s;

/** Structure for PMCH data-MCS for all configured 
    PMCHs in an MBSFN area. */
typedef struct
{
  uint8 mbsfn_area_id;
  /** MBSFN area id (0..255) from SIB13 (System Information
      Block 13). */

  uint8 pmch_id;
  /** PMCH id (1..15), First PMCH config is treated as PMCH id 1. */

  uint8 datamcs;
  /** Data MCS(Modulation and Coding scheme) for the PMCH. */

} cmapi_lte_rrc_pmch_datamcs_info_s;

/** Structure for PMCH data-MCS info for all PMCHs actively being monitored. */
typedef struct
{
  uint8 num_pmch_datamcs_info;
  /** Number of PMCHs for which dataMCS info has been included. */

  cmapi_lte_rrc_pmch_datamcs_info_s pmch_datamcs[CMAPI_LTE_MAX_ACTIVE_PMCH];
  /** PMCH data-MCS of all PMCHs actively being monitored. */
  
} cmapi_lte_rrc_datamcs_info_s;

/** MTCH (Multicast traffic channel) info */
typedef struct
{
  uint8 area_id;
  /** MBSFN area identity */

  uint8 pmch_id;
  /** PMCH identity */

  uint8 lc_id;
  /** Logical channel identity */

  uint64 num_mtch_tb_bytes;
  /** Number of received bytes for MTCH transport block */

} cmapi_mtch_info_s;

/** MAC active eMBMS info. */
typedef struct
{
  uint8 num_active_embms_comb; 
  /** Number of active area_id, pmch_id combinations. */

  cmapi_mtch_info_s mtch_info_arr[CMAPI_LTE_EMBMS_MAX_ACTIVE_SESSIONS];
  /** MTCH info array */

} cmapi_lte_mac_active_embms_info_s;

/** CMAPI LTE eMBMS information. */
typedef struct
{
  uint8 eMBMS_coverage_status;

  cmapi_lte_rrc_datamcs_info_s        datamcs;
  /** Data MCS info. */

  cmapi_lte_mbsfn_snr_bler_info_s     pmch;
  /** PMCH stats info. */

  cmapi_lte_mac_active_embms_info_s   mac_active; 
  /** MAC active info. */

}cmapi_lte_eMBPS_info_s;

/** Channel quality indication for periodic reporting.
*/
typedef struct
{

  boolean enable;
  /**< Indicates whether periodic reporting is enabled. */

  uint16 pucch_resource_index;
  /**< Physical uplink control channel resource index. Range: 0 to 767. */

  uint16 cqi_pmi_cfg_index;
  /**< CQI/PMI periodicity and offset configuration index. Range: 0 to 511. */

  cmapi_lte_l1_cqi_format_indicator_e format_indicator;
  /**< PUCCH CQI feedback type. Refer to @xhyperref{S2,[S2]}, Table 7.2.2-1. 
       Depending on the transmission mode, the reporting mode is implicitly 
       given in the table. */ /* 3GPP TS 36.213 */

  uint8 subband_cqi_k;
  /**< Parameter K. Used only if the CQI format indicator is set to 
       CMAPI_LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_ SUBBAND. Range: 1 to 4. */

  boolean ri_cfg_index_enable;
  /**< Indicates whether the rank indicator configuration index is enabled. */

  uint16 ri_cfg_index;
  /**< Rank indicator configuration index. */

  boolean sim_ack_nak_cqi;
  /**< Indicates whether the simultaneous transmission of ACK/NACK and CQI is 
       allowed. */

}cmapi_lte_l1_cqi_reporting_periodic_s;
/* Tech Pubs note: A Space was added before SUBBAND in PERIODIC_ SUBBAND to 
   facilitate formatting. */

/** CQI Reporting Release 9 parameters.
*/
typedef struct
{
  boolean cqi_mask_enable;
  /**< Indicates whether the CQI mask is enabled. */

  boolean pmi_ri_report_configured;
  /**< Indicates whether the Precoding Matrix Indicator and Rank Indicator 
       report is configured. @newpagetable */

}cmapi_lte_l1_cqi_rel9_param_s;

/** CMAPI LTE channel quality indication reporting parameters.
*/
typedef struct
{
  boolean enable;
  /**< Indicates whether CQI reporting is enabled. */

  boolean cqi_reporting_mode_aperiodic_enable;
  /**< Indicates whether the CQI aperiodic reporting mode is enabled. */

  cmapi_lte_l1_cqi_reporting_mode_aperiodic_e cqi_reporting_mode_aperiodic;
  /**< CQI aperiodic reporting mode. */

  int8 nom_pdsch_rs_epre_offset;
  /**< Provides the nominal measurement offset in dB between the physical 
       downlink shared channel and the reference signal energy per resource 
       block used by the UE when computing CQI. \n 
       Range: 1 to 6. */

  boolean cqi_reporting_periodic_present;
  /**< Indicates whether the reporting periodic information is present. */

  cmapi_lte_l1_cqi_reporting_periodic_s cqi_reporting_periodic;
  /**< CQI reporting periodic information. Only valid when 
       cqi_reporting_periodic_present is set to TRUE. */

  boolean cqi_rel9_param_present;
  /**< Indicates whether the CQI Release 9 parameters are present. */

  cmapi_lte_l1_cqi_rel9_param_s cqi_rel9_param;
  /**< CQI Release 9 parameters. */

} cmapi_lte_cphy_cqi_reporting_param_s;

/** Stores the LTE ML1 common RSSI signal parameters.
*/
typedef struct
{
  uint16                channel;
  /**< Active channel of the current system. */ 
  
  uint8                 band;
  /**< Active band of the current system. */
  
  uint8                 reserved;
  /**< Reserved. */
  
  uint16                phy_cell_id; 
  /**< Physical cell ID information of the serving cell. */
  
  int16                 tx_power; 
  /**< Tx power. @newpagetable */    
}cmapi_lte_ml1_common_rssi_signal_param_s;

/** LTE ML1 common RSSI signal parameters.
*/
typedef union
{
  
  cmapi_lte_ml1_common_rssi_signal_param_s    data;
  /**< Data to extract various signal values. */
  
  atomic64_word_t                             value;
  /**< Value to read/write the value atomically. */
} cmapi_lte_ml1_common_rssi_signal_param_u;


/******************************************************************************
                      Antenna info dedicated config
******************************************************************************/
/* 3GPPS TS 36.331 */
/** Antenna information dedicated configuration parameters. Refer to 
    @xhyperref{S3,[S3]}, Section 6.3.2.
*/
typedef struct {
  cmapi_lte_l1_dl_tx_mode_e transmission_mode;
  /**< Antenna transmission mode for the PDSCH. */

  uint64 codebook_subset_restriction;
  /**< Bitmask of the codebook restriction. The number of bits used in the 
       restriction varies with the transmission mode. The bitmask is placed in 
       the lower order bits.

       The number of bits are: \n
       - n2TxAntenna-tm3 -- 2
       - n4TxAntenna-tm3 -- 4
       - n2TxAntenna-tm4 -- 6
       - n4TxAntenna-tm4 -- 64
       - n2TxAntenna-tm5 -- 4
       - n4TxAntenna-tm5 -- 16
       - n2TxAntenna-tm6 -- 4
       - n4TxAntenna-tm6 -- 16
       @tablebulletend
  */

  boolean tx_antenna_selection_enabled;
  /**< Indicates whether the UE transmit antenna selection is enabled. */

  cmapi_lte_l1_tx_antenna_selection_e tx_antenna_selection_ctrl;
  /**< Indicates whether the UE transmit antenna selection control is 
       closed loop or open loop. @newpagetable */

} cmapi_lte_cphy_antenna_cfg_dedicated_param_s;

/** Indicates to the upper layers the current signal quality of the neighbor 
  cells as reported by LTE L1.

  Due to the dynamic nature of the a cellular environment, there can 
  be times when no serving or neighbor information is available (i.e.,
  in RLF/OOS or certain types of handovers).  

  No aggregation is done at an L1 level. This is considered a current
  snapshot of the L1 conditions. It is possible to use previous snapshots
  for a period of n seconds while valid is not TRUE to prevent sudden jumps
  from being presented to the UI.
*/
typedef struct
{
  boolean valid;
  /**< Indicates the validity of the structure fields. 
       - TRUE -- One or more of the fields is valid.
       - FALSE -- None of the fields are valid. @tablebulletend */

  boolean idle;
  /**< Indicates whether the UE is in Idle mode. 
       - TRUE --  UE is in Idle mode. All Idle mode substructs are considered 
                  valid. 
       - FALSE -- All Idle mode substructs are considered invalid, except for 
                  serving cell information. @tablebulletend */

  cmapi_lte_serv_cell_freq_param_s lte_serving_cell;
  /**< LTE serving cell parameters. */
 
  boolean lte_ca_scell_present;
  /**< Indicates whether the LTE CA SCell is present. */

  cmapi_lte_ca_scell_param_s lte_ca_scell;
  /**< LTE CA SCell parameters. */

  cmapi_lte_ngbr_ind_intra_freq_param_s lte_intra;
  /**< LTE intrafrequency parameters. */
 
  cmapi_lte_ngbr_ind_inter_freq_param_s lte_inter;
  /**< LTE interfrequency parameters. */
  
  cmapi_lte_ngbr_ind_gsm_info_s gsm;
  /**< GSM cell information. */

  cmapi_lte_ngbr_ind_wcdma_info_s wcdma;
  /**< WCDMA cell information. */

  cmapi_lte_ngbr_ind_cdma_info_s cdma_1x;
  /**< CDMA 1X cell information. */

  cmapi_lte_ngbr_ind_cdma_info_s cdma_hrpd;
  /**< CDMA high-rate packet data cell information. */

  uint16   ra_rnti;  
  /**< Random access radio network temporary ID. */

  uint16   c_rnti;   
  /**< Connected state, common, and UE-specific search space radio network 
       temporary identification. */

  uint16 cqi_wb;
  /**< Wideband CQI information. */

  uint8 enb_num_tx_antenna;
  /**< Number of Tx antenna on an LTE base station. */

  cmapi_lte_cphy_drx_cfg_dedicated_param_s cdrx_cfg;
  /**< Stores the connected DRX configuration. */

  cmapi_lte_cphy_cqi_reporting_param_s cqi_cfg;
  /**< Channel quality indication configuration. */

  cmapi_lte_cphy_antenna_cfg_dedicated_param_s ant_cfg;
  /**< Stores the antenna configuration. */

  cmapi_lte_cphy_idle_drx_param_s idle_drx_cfg;
  /**< Idle DRX information. */

  uint32  timing_advance;
  /**< Timing advance is valid only if the LTE is in Connected mode; otherwise,
       it is set to Invalid Value. */

  uint16  doppler_measurement;
  /**< Doppler measurement. */

} cmapi_lte_ngbr_ind_s;

/** Intrafrequency per cell structure.
*/
typedef struct
{
  uint16  psc;
  /**< Primary scrambling code. */

  int16  ecio;
  /**< Instantaneous cell received energy per chip and interference level. */

  int16  rscp;
  /**< Instantaneous cell received signal code power.*/

  uint16  set;
  /**< Intrafrequency cell type. */

  int16  rank; 
  /**< Intrafrequency cell ranking. @newpagetable */

} cmapi_wcdma_intra_cell_info_struct_type;

/** WCDMA intrafrequency cell information.
*/
typedef struct
{
  uint16 freq;
  /**< Camped cell frequency. */

  int16  rxagc;
  /**< Receiver automatic gain control on the camped frequency. */

  uint8 num_nbr_cells;
  /**< Number of intrafrequency neighbor cells reported. */

  uint8 num_serv_cells;
  /**< Number of serving cells (cells when in a soft handover) reported. */

  cmapi_wcdma_intra_cell_info_struct_type   nbr_cell[CMAPI_WCDMA_MAX_CELLS_REPORTED];
  /**< Neighbor cell. */

  cmapi_wcdma_intra_cell_info_struct_type  serv_cell[CMAPI_WCDMA_MAX_ASET_CELLS];
  /**< Serving cell. */

} cmapi_wcdma_intra_freq_info_struct_type;

/** Interfrequency per cell structure.
*/
typedef struct
{
  uint16 psc;
  /**< WCDMA neighbor cell Primary scrambling code. */
  int16  ecio;
  /**< Instantaneous neighbor cell received energy per chip and interference 
       level. */
  int16  rscp;
  /**< Instantaneous neighbor cell received signal code power. */
  uint16  set;
  /**< Interfrequency cell type. */
  int16  rank;  
  /**< Interfrequency cell ranking. */
  int16  s_rxlev;
  /**< Interfrequency cell suitable receive level. */
} cmapi_wcdma_inter_cell_info_struct_type;


/** Interfrequency neighbor per frequency.
*/
typedef struct
{
  uint16 freq;
  /**< Frequency to which the interfrequency neighbor is tuned. */

  int16  rxagc;
  /**< Receiver automatic gain control on the interfrequency neighbor. */

  uint8 num_cells;
  /**< Number of cells to report per frequency. */

  cmapi_wcdma_inter_cell_info_struct_type cell[CMAPI_WCDMA_MAX_CELLS_REPORTED]; 
  /**< Number of interfrequency cells reported per WCDMA neighbor frequency. */

} cmapi_wcdma_inter_per_freq_info_struct_type;


/** WCDMA interfrequency cell information.
*/
typedef struct
{
  uint8 num_freq;
  /**< Number of frequencies. */

  cmapi_wcdma_inter_per_freq_info_struct_type wcdma_inter_freq[CMAPI_WCDMA_SRCH_INTER_FREQ_MAX]; 
  /**< Number of interfrequency neighbors. */

} cmapi_wcdma_inter_freq_info_struct_type;


/** GSM per cell structure.
*/
typedef struct
{
  uint16 arfcn;
  /**< Absolute radio frequency channel number. Range: 0 to 1023. */
  uint16 bsic;
  /**< Base station identity code. Base station color code in least significant 
       bit (or byte). Network color code in most significant bit (or byte). */
  int16 rssi;
  /**< Received signal strength indicator. Range: 0 to -120. */
  int16 rank;
  /**< Cell rank. Range: 0 to -200. */
  int16  s_rxlev;
  /**< GSM cell suitable receive level. */
} cmapi_wcdma_gsm_cell_info_struct_type;


/** GSM neighbor cell information reported in WCDMA.
*/
typedef struct
{
  uint8 num_cells;
  /**< Number of cells to be reported per frequency. */
  cmapi_wcdma_gsm_cell_info_struct_type cell[CMAPI_WCDMA_MAX_CELLS_REPORTED];  
  /**< Number of GSM cells reported. @newpagetable */
} cmapi_wcdma_interrat_gsm_info_struct_type;

/** LTE neighbor cell information reported in WCDMA.
*/
typedef struct
{
  uint16 cell_id;
  /**< Physical cell ID of the detected cell. */

  int16 rsrp;
  /**< Maximum reference signal received power combined across Tx-Rx pairs. In 
       linear scale. */

  int16 rsrq;
  /**< Maximum reference signal received quality value combined across Tx-Rx 
       pairs. In linear scale. */

  int16  s_rxlev;
  /**< LTE cell suitable receive level. */

} cmapi_wcdma_lte_cell_info_struct_type;

/** LTE per the EARFCN information reported in WCDMA.
*/
typedef struct
{
  uint32 earfcn;
  /**< E-UTRA absolute radio frequency channel number for searching LTE cells. */
  uint8 num_cells;
  /**< Number of LTE cells reported per LTE EARFCNs. */
  int16 priority;
  /**< Priority information. Invalid priority is -1. */
  cmapi_wcdma_lte_cell_info_struct_type cell[CMAPI_WCDMA_MAX_CELLS_REPORTED];  
  /**< Cell information for LTE EARFCN. */
} cmapi_wcdma_lte_per_earfcn_info_struct_type;
  
/** LTE neighbor frequency information reported in WCDMA.
*/
typedef struct
{
  uint8 num_earfcn;
  /**< Number of LTE EARFCNs. */
  cmapi_wcdma_lte_per_earfcn_info_struct_type lte_earfcn[CMAPI_WCDMA_SRCH_LTE_EARFCN_MAX];
  /**< LTE per the EARFCN information. @newpagetable */
} cmapi_wcdma_interrat_lte_info_struct_type;

/** WCDMA signal information.
*/
typedef struct
{
  boolean is_data_valid;
  /**< Indicates whether the fields in the structure are valid. \n
       - TRUE -- One or more of the fields is valid.
       - FALSE -- None of the fields are valid. @tablebulletend
  */

  int16 s_intra_search;
  /**< Cell selection parameter for the intrafrequency cell. */
  int16 s_inter_search;
  /**< Cell selection parameter for the interfrequency cell. */
  int16 s_search_RAT;
  /**< Cell selection parameter for the GSM cell. */
  
  cmapi_wcdma_intra_freq_info_struct_type    cmapi_wcdma_intra_cell_info;  
  /**< WCDMA intrafrequency serving and neighbor cell information. */
  cmapi_wcdma_inter_freq_info_struct_type    cmapi_wcdma_inter_cell_info;  
  /**< WCDMA interfrequency neighbor information. */
  cmapi_wcdma_interrat_gsm_info_struct_type  cmapi_wcdma_gsm_cell_info;  
  /**< GSM neighbor information. */
  cmapi_wcdma_interrat_lte_info_struct_type  cmapi_wcdma_lte_cell_info;
  /**< LTE neighbor information. */
    
} cmapi_wcdma_signal_info_struct_type;

/** Signal information for CDMA, HDR, GSM, WCDMA, LTE, or TD-SCDMA.
*/
typedef struct
{

  boolean                    is_service_available;
  /**< Indicates whether service is available. The remaining parameter values 
       are only valid when service is available; otherwise, do not use. The 
       signal information values are set to #CMAPI_SIGNAL_INFO_INVALID_VALUE.
    */
  cmapi_sys_mode_e_type      sys_mode;
  /**< System mode; see #cmapi_sys_mode_e for details. */

  /** RAT measurement information. */
  union 
  {
    cmapi_wcdma_signal_info_struct_type     wcdma_nbr_info;
    /**< WCDMA signal and neighbor information. */

    cmapi_lte_ngbr_ind_s                    lte_nbr_info;
    /**< LTE signal and neighbor information. */
    
  } meas_info_u; 
  
} cmapi_rat_meas_info_s_type;

/** Rx signal information parameters per each Rx chain.
*/
typedef struct cmapi_rx_chain_info_s{

  boolean             is_radio_tuned;
    /**< Indicates whether the Rx is tuned to a channel. \n
         If the radio is tuned, instantaneous values are set for the signal 
         information fields. \n
         If the radio is not tuned, delayed or invalid values are set depending 
         on each technology. */

  int32               rx_pwr;
    /**< Rx power value in 1/10 dBm resolution. */
  int32               ecio;
    /**< Ec/Io in -1/10 dBm. Available for CDMA, HDR, GSM, WCDMA, LTE, or 
         TD-SCDMA. */
  int32               rscp;
    /**< Received signal code power in -1/10 dBm. Available for WCDMA. */
  int32               rsrp;
    /**< Current reference signal received power in -1/10 dBm. Available for 
         LTE. */
 
  uint32              phase;
    /**< Current phase in 1/100 degrees. Range: 0.00 to 360.00. Available for 
         LTE only. */ 
}cmapi_rx_chain_info_s_type;

/** Rx power-related information.
*/
typedef struct cmapi_rx_pwr_info_s{

  boolean valid;
    /**< Flag identifying if the contents of the message are valid.  valid will 
        be set to TRUE when the measurements are valid. When FALSE the contents
        of the rest of the message is invalid and should not be used */

  cmapi_rx_diversity_bit_mask_e_type  rx_diversity_indicator;
    /**< Diversity bitmask to show which Rx chain has valid signal information; 
         see #cmapi_rx_diversity_bit_mask_e for details. 
    */

  cmapi_rx_chain_info_s_type          rx_pwr_chain[CMAPI_RX_CHAIN_MAX];
    /**< Rx signal information; see cmapi_rx_chain_info_s for details.  */

}cmapi_rx_pwr_info_s_type;


/** Tx power-related information.
*/
typedef struct cmapi_tx_pwr_info_s{

  boolean             is_in_traffic;
    /**< Indicates whether the device is in traffic.  */
  int32               tx_pwr;
    /**< Tx power value in 1/10 dBm. \n
         @note1 The tx_pwr value is only meaningful when the device is in 
         traffic. When there is no traffic, tx_pwr is invalid. */
  uint8              pa_gain_state;
    /**< Power amplifier gain state. */

}cmapi_tx_pwr_info_s_type;

/** Physical cell ID information.
*/
typedef struct 
{
  boolean valid;
  /**< Indicates whether the information is valid. */
  
  uint16  value;
  /**< Physical cell ID. */
  
}cmapi_serv_cell_id_info_s;

/** Signal-to-interface plus noise ratio information measured in decibels.
*/
typedef struct 
{
  boolean valid;
  /**< Indicates whether the information is valid. */
  
  int16   value;
  /**< Logarithmic value of SINR. @newpagetable */

}cmapi_serv_cell_sinr_info_s;

/** Signal information for a secondary cell in LTE.
*/
typedef struct 
{
  int32                band;
    /**< Active band of the secondary cell for LTE. */
  
  int32                channel;
  /*! Active channel of the secondary cell for LTE */  

  cmapi_rx_pwr_info_s_type rx_power;
  /**< Rx power information for the lte secondary cell; see 
         cmapi_rx_pwr_info_s for details. */

  cmapi_serv_cell_id_info_s  phy_cellid;
  /*! Rx Power info of the secondary cell for LTE */

  cmapi_serv_cell_sinr_info_s  log_sinr10xdb;
  /*! SINR info of the secondary cell for LTE*/
  
  uint8 state;
  /*! State of the secondary cell. 0 - Init, 1 - Configured, 2 - Active */

  uint8 dl_bandwidth;
  /**< Transmission bandwidth configuration of the secondary cell 
       on the downlink. Range: 0 to 5. */

}cmapi_lte_scc_signal_info_s;

/** Signal information for all secondary cell in LTE.
*/
typedef struct 
{
  cmapi_lte_scc_signal_info_s lte_scc_info[CMAPI_LTE_ML1_MAX_SCC];
  /**< Signal information for secondary cells for LTE */

}cmapi_lte_scc_signal_info_s_type;

/** Signal information for CDMA, HDR, GSM, WCDMA, LTE, or TD-SCDMA.
*/
typedef struct cmapi_signal_info_s{

  boolean                     is_service_available;
  /**< Indicates whether service is available. The remaining parameter values 
       are only valid when service is available; otherwise, do not use. The 
       signal information values are set to #CMAPI_SIGNAL_INFO_INVALID_VALUE.
    */
  cmapi_sys_mode_e_type       sys_mode;
    /**< Requested system mode for the signal information; see #cmapi_sys_mode_e 
         for details. */
  int32                       band;
    /**< Active band of the current system. */
  int32                       channel;
    /**< EARFCN. Active channel of the current system. */
  uint8                       dl_bandwidth;
  /**< Transmission bandwidth configuration of the serving cell on the 
       downlink. Range: 0 to 5. */
  cmapi_rx_pwr_info_s_type    rx_power;
    /**< Rx power information for the specified system mode; see 
         cmapi_rx_pwr_info_s for details. */
  cmapi_tx_pwr_info_s_type    tx_power;
    /**< Tx power information for the specified system mode; see 
         cmapi_tx_pwr_info_s for details. */
  cmapi_serv_cell_id_info_s   phy_cellid;
    /**< Serving cell physical ID information. */
  cmapi_serv_cell_sinr_info_s log_sinr10xdb;
    /**< Serving cell SINR information measured in decibels. */
  cmapi_modulation_info_s     modulation;
    /**< Modulation info */
  cmapi_tdssrch_freq_syncdlRssi_type  cm_freq_DwPTSRssi;
    /**< TD-SCDMA received signal strength indicator 
         information. */
  cmapi_tdssrch_uplink_tx_power_type  cm_tx_pwr_info;
    /**< TD-SCDMA Tx power information. */
  cmapi_lte_scc_signal_info_s_type cm_scc_signal_info;
    /**< LTE secondary cells signal information. */
}cmapi_signal_info_s_type;

/** Provides 1X signal information.
*/
typedef struct cmapi_1X_signal_info_s{

  boolean             is_service_available;
    /**< Indicates whether service is available. The remaining parameter values 
	     are only valid when service is available; otherwise, do not use. The 
		 signal information values are set to #CMAPI_SIGNAL_INFO_INVALID_VALUE.
    */
  int32               band;
    /**< Active band of the current system. */

  int32               channel;
    /**< Active channel of the current system. */

  boolean             is_in_traffic;
    /**< Indicates whether the device is in traffic. The tx_pwr parameter is 
	     only meaningful when in traffic. When there is no traffic, tx_pwr is 
		 invalid.
    */
  int32               tx_pwr;
    /**< Transmit power. */
}cmapi_1X_signal_info_s_type;

/** @} */ /* end_addtogroup cmapi_datatypes */

/*===========================================================================
  FUNCTION:  cmapi_get_signal_info
===========================================================================*/

/** @ingroup cmapi_get_signal_info
  Gets the signal information for one technology: CDMA, HDR, GSM, WCDMA, LTE, 
  or TD-SCDMA.

  @datatypes
  #cmapi_sys_mode_e \n
  #cmapi_signal_info_s

  @param[in]  sys_mode         System mode on which the signal information is 
                               to be requested.
  @param[out] signal_info_ptr  Pointer to the output buffer for the returning 
                               signal information.

  @return
  #cmapi_err_e 

  @dependencies
  None.

  @detdesc
  A unified function is required to report band, channel, Rx power, and Tx power for 
  CDMA, HDR, GSM, WCDMA, LTE, and TD-SCDMA technologies. 
  @keep{5}
  @par 
  The following is a high-level description of the function:
  - RAT information is provided as an argument to the function.
  - The function runs in the caller task context. Customers must implement their 
    task to access the function.
  - The function is synchronous to meet timing requirements.
  - The function is implemented inside the CM to provide an interface to the 
    customer.
  - The CM directly calls functions from each technology to get the desired signal 
    information parameters based on input RAT information.
  - Each time the function runs, information for the provided RAT is returned.
  - The timing requirement must be guaranteed by each lower layer RAT 
    technology. @newpage

  @par Algorithm 
  The following steps describe the algorithm:
  -# If sys_mode is one of the technologies (CDMA/HDR/GSM/WCDMA/LTE/TD-SCDMA) 
     and service is available on the input system mode, the CM calls the 
     corresponding synchronous function from each technology to get signal 
     information.
     - Set signal_info_ptr -> is_service_available to TRUE.
     - Set signal_info_ptr -> is_service_available to FALSE. 
  -# Fill the output buffer.
     - Return TRUE.
     - Return FALSE.

  @par Lower layer function requirement 
  The CM calls the function from a lower layer only when a service is available. 
  If there is no service, all information is invalid. Each technology provides 
  Rx and Tx signal information based on the different scenarios.

  @par Tx power information 
  Lower layer function call is made to fill in the fields within  
  #cmapi_tx_pwr_info_s.
  @par 
  Tx power information is valid when the device is in traffic.
  @par 
  Tx power information is invalid when there is no traffic. The invalid Tx 
  power value is set to #CMAPI_SIGNAL_INFO_INVALID_VALUE.

  @par Rx power information 
  Lower layer function call is made to fill in the fields within
  #cmapi_rx_pwr_info_s.
  @par 
  Based on diversity information, the Rx signal information is valid on 
  either chain, both chains, or neither chain.
  @keep{4}
  @par 
  Within each Rx chain, if the radio is tuned to a channel, instantaneous 
  values are set for information fields defined in 
  #cmapi_rx_chain_info_s.
  @par 
  Within each Rx chain, if the radio is not tuned, the delayed (last known) 
  value or invalid value is set to information fields defined in 
  #cmapi_rx_chain_info_s. If the value is invalid, fields are set to 
  #CMAPI_SIGNAL_INFO_INVALID_VALUE. Each lower layer technology is 
  responsible for determining whether the delayed value or the invalid value 
  must be set.
  @par 
  @note1hang The CMAPI suggests 1/10 dBm for the signal information value. 
  However, based on each technology, the function might not support 1/10 dBm 
  granularity.

  @par Units of the returned values 
    @keep{8}
    @vertspace{6}
    @verbatim
 Unit       CDMA        HDR         GSM                WCDMA        LTE
 _______________________________________________________________________________
 TX power   1/10 dBm    1/10 dBm    Not yet supported  1/10 dBm     1/10 dBm
 Rx power   1/10 dBm    1/10 dBm    1/10 dBm           1/10 dBm     1/10 dBm
 Ec/Io      1/10 dBm    1/10 dBm    N/A                1/10 dBm    -1/10 dBm
 RSCP       N/A         N/A         N/                 1/10 dBm     N/A
 RSRP       N/A         N/A         N/A                N/A          1/10 dBm
 Phase      N/A         N/A         N/A                N/A          1/100 degree           
    @endverbatim
    @vertspace{12}
    @note1hang The LTE Ec/Io values are defined as 1/10 dB. The 1/10 dBm unit  
    is not yet supported because it requires radio frequency and firmware 
    support. 
*/
cmapi_err_e_type         cmapi_get_signal_info
(
       cmapi_sys_mode_e_type         sys_mode, 
       cmapi_signal_info_s_type      *signal_info_ptr
);

/*===========================================================================
  FUNCTION:  cmapi_change_rx_diversity
===========================================================================*/
/** @ingroup cmapi_change_rx_diversity
  Enables/disables Rx diversity.

  @datatypes
  #cmapi_sys_mode_e \n
  #cmapi_rx_diversity_bit_mask_e

  @param[in] sys_mode                  System mode on which Rx diversity is 
                                       to be changed. 
  @param[in] enable_rx_chain_bit_mask  Indicates which Rx chain is to be
                                       enabled. Values: \n
                                       - 0x0 -- CMAPI_BM_RX_CHAIN_NONE
                                       - 0x1 -- CMAPI_BM_RX_CHAIN_0
                                       - 0x2 -- CMAPI_BM_RX_CHAIN_1
                                       - 0x3 -- CMAPI_BM_RX_CHAIN_ALL
                                       @tablebulletend

  @return
  #cmapi_err_e 

  @dependencies
  None.

  @detdesc
  The following is a high-level description of this function:
  - The sys_mode parameter is passed as one of the inputs. 
  - The Rx chain bitmask parameter indicates which Rx chain must be enabled 
    or disabled.
  - When a service is available on the input system mode or the device is in 
    FTM mode, the corresponding lower layer function is called to change the Rx 
    diversity.

  @par Algorithm 
  The following steps describe the algorithm:
  - If the service is available on the input system mode or the device is in 
    FTM mode: \n
    -# Call %cmapi_change_rx_diversity() from the lower layers.
    -# Return CMAPI_NO_SERVICE. @newpage

  @par API called during traffic 
  If %cmapi_change_rx_diversity() is called during a call: 
  - CDMA/HDR -- Diversity changes immediately.
  - WCDMA -- Diversity changes when the UE enters the next DCH. 
  - LTE -- Diversity changes immediately.
  - TD-SCDMA -- Diversity changes on the next slot 1 boundary. 
*/
cmapi_err_e_type cmapi_change_rx_diversity
(
  cmapi_sys_mode_e_type               sys_mode, 
  cmapi_rx_diversity_bit_mask_e_type  enable_rx_chain_bit_mask
);  

/*===========================================================================
  FUNCTION:  cmapi_set_lte_signal_reporting_config
===========================================================================*/
/** @ingroup cmapi_set_lte_signal_reporting_config 
  Sets the signal reporting rate and averaging period information for LTE.

  If this function is not called, LTE uses the default values for reporting 
  and averaging.

  @datatypes
  #cmapi_lte_signal_reporting_rate_e \n
  #cmapi_lte_signal_averaging_period_e

  @param[in] reporting_rate    Signal reporting rate.
  @param[in] averaging_period  Averaging period.

  @return
  TRUE -- Set the requested configuration. \n
  FALSE -- Failed to set the requested configuration.

  @dependencies
  None.
*/
boolean cmapi_set_lte_signal_reporting_config
(
  cmapi_lte_signal_reporting_rate_e       reporting_rate,
  cmapi_lte_signal_averaging_period_e     averaging_period
);

/*===========================================================================
  FUNCTION:  cmapi_get_rat_meas_info
===========================================================================*/
/** @ingroup cmapi_get_rat_meas_info    
  Gets the neighbor measurement information for the RAT.

  @datatypes
  #cmapi_sys_mode_e \n
  #cmapi_rat_meas_info_s_type

  @param[in] sys_mode       System mode for which the neighbor information is 
                            required.
  @param[in] meas_info_ptr  Pointer to the measurement information for the RAT.

  @return
  #cmapi_err_e

  @dependencies
  None.
*/
cmapi_err_e_type cmapi_get_rat_meas_info
(
  cmapi_sys_mode_e_type                sys_mode, 
  cmapi_rat_meas_info_s_type*          meas_info_ptr      
);  

/*===========================================================================
  FUNCTION:  cmapi_start_capture_scan_data
===========================================================================*/
/** @ingroup cmapi_start_capture_scan_data
  Calls the respective RATs to start capturing scan data.
 
  @param[in] rat_bitmap              Required RAT in bitmap format. 
  @param[in] scan_data_ptr           Pointer to the 32-bit aligned memory 
                                     where the RAT starts updating the data. 
  @param[in] max_no_of_LTE_result    Maximum number of entries of frequency 
                                     scan results for LTE.
  @param[in] max_no_of_WCDMA_result  Maximum number of entries of frequency 
                                     scan results for WCDMA.
  @param[in] max_no_of_GERAN_result  Maximum number of entries of frequency 
                                     scan results for GSM.
 
  @return
  #cmapi_err_e
 
  @dependencies
  The RAT types CDMA and TD-SCDMA are not supported by this function. 
  @par
  The capture of scan data stops if the scan data memory fills up or if 
  cmapi_stop_capture_scan_info() is called.
*/
cmapi_err_e_type  cmapi_start_capture_scan_data 
(	
  uint16			rat_bitmap, 
  uint32* 			scan_data_ptr, 
  uint16 			max_no_of_LTE_result,
  uint16 			max_no_of_WCDMA_result,
  uint16 			max_no_of_GERAN_result
);

/*===========================================================================
  FUNCTION:  cmapi_stop_capture_scan_info 
===========================================================================*/
/** @ingroup cmapi_stop_capture_scan_info
  Stops the scan data capture specified in the rat_bitmap parameter.
 
  @param[in] rat_bitmap  Required RAT in bitmap format. 
 
  @return
  #cmapi_err_e
 
  @dependencies
  None.
*/
cmapi_err_e_type    cmapi_stop_capture_scan_info 
(	
  uint16			rat_bitmap
);

/*===========================================================================

  FUNCTION:  cmapi_lte_get_signal_params 

===========================================================================*/
/**	@ingroup cmapi_lte_get_signal_params
  Gets the LTE signal information. 

  @datatypes
  #cmapi_lte_ml1_common_rssi_signal_param_u
	
  @param[in] lte_signal_params  Pointer to the buffer that holds the 
	                            returning LTE signal information.  

  @return
  #cmapi_err_e
	
  @dependencies
  None.
*/
cmapi_err_e_type cmapi_lte_get_signal_params 
(	
  cmapi_lte_ml1_common_rssi_signal_param_u  *lte_signal_params 
);

/*===========================================================================

FUNCTION cmapi_1X_get_signal_params

===========================================================================*/
/**	 @ingroup cmapi_1X_get_signal_params
  Gets the 1X signal information.

  @datatypes
  #cmapi_1X_signal_info_s
	
  @param[in] signal_1X_info_ptr  Pointer to the buffer that holds the 
	                             returning 1X signal information.                       

  @return
  #cmapi_err_e
	
  @dependencies
  None.
*/
cmapi_err_e_type  cmapi_1X_get_signal_params
(
  cmapi_1X_signal_info_s_type  *signal_1X_info_ptr
  /* pointer to buf which holds returning signal info */
);

/*===========================================================================

FUNCTION cmapi_get_decoded_sib_string

===========================================================================*/
/**	@ingroup cmapi_get_decoded_sib_string
  Exposes the uncoded SIB strings (encoded in ASN.1 format).

  @param[in] encoded_si_ptr       Pointer to the encoded SIB.
  @param[in] encoded_si_size      Size of the encoded SIB. 
  @param[in] buffer               Pointer to the buffer to store the uncoded SIB. 
  @param[in] buffer_length        Buffer length.

  @return
  0 -- Success. \n
  -1 -- NULL pointers or insufficient buffer size. \n
  -2 -- Decoding failed. \n
  CMAPI_NOT_SUPPORTED -- LTE is not supported.

  @dependencies
  None.
*/
int cmapi_get_decoded_sib_string
(
  void   *encoded_si_ptr, 
  uint32  encoded_si_size, 
  void   *buffer,
  uint32  buffer_length
);

/*===========================================================================

FUNCTION cmapi_set_lte_fgi

===========================================================================*/

/** @ingroup cmapi_set_lte_fgi
  Configures the LTE feature group indicator. 

  @datatypes
  #cmapi_lte_fgi_rel_type_e

  @param[in] fgi_rel_type Indicates whether the FGI is for Release 8, 9, or 10.
  @param[in] fgi_word     Feature group indicator word; 32-bit. Values: 
                          - LTE Release 8 -- FGI bit 1 (MSB) through 
                                             bit 32 (LSB) 
                          - LTE Release 9 -- FGI bit 33 (MSB) through 
                                             bit 64 (LSB)
                          - LTE Release 10 -- FGI bit 101 (MSB) through 
                                              bit 132 (LSB) @tablebulletend 

  @return
  TRUE --  RRC accepted the FGI change requested through the CMAPI. \n
  FALSE -- RRC did not accept the FGI change requested through the CMAPI. 

  @dependencies
  RRC only accepts an FGI change if the client for this API triggers Airplane
  mode ON, makes the API call, and subsequently turns OFF the Airplane mode.
*/
boolean cmapi_set_lte_fgi
(
  cmapi_lte_fgi_rel_type_e  fgi_rel_type, 
  uint32                    fgi_word                 
);

/*===========================================================================

FUNCTION cmapi_set_lte_tdd_fgi

===========================================================================*/

/** @ingroup cmapi_set_lte_tdd_fgi
  Configures the LTE feature group indicator for TDD

  @datatypes
  #cmapi_lte_fgi_rel_type_e

  @param[in] fgi_rel_type Indicates whether the FGI is for Release 8, 9, or 10.
  @param[in] fgi_word     Feature group indicator word; 32-bit. Values: 
                          - LTE Release 8 -- FGI bit 1 (MSB) through 
                                             bit 32 (LSB) 
                          - LTE Release 9 -- FGI bit 33 (MSB) through 
                                             bit 64 (LSB)
                          - LTE Release 10 -- FGI bit 101 (MSB) through 
                                              bit 132 (LSB) @tablebulletend 

  @return
  TRUE --  RRC accepted the FGI change requested through the CMAPI. \n
  FALSE -- RRC did not accept the FGI change requested through the CMAPI. 

  @dependencies
  RRC only accepts an FGI change if the client for this API triggers Airplane
  mode ON, makes the API call, and subsequently turns OFF the Airplane mode.
*/
boolean cmapi_set_lte_tdd_fgi
(
  cmapi_lte_fgi_rel_type_e  fgi_rel_type, 
  uint32                    fgi_word                 
);

/*===========================================================================

FUNCTION cmapi_lte_set_pci_lock_req

===========================================================================*/

/** @ingroup cmapi_lte_set_pci_lock_req
  Sets the LTE-RRC PCI lock. 

  @param[in] dl_earfcn Downlink EARFCN value.
  @param[in] pci       PCI value.

  @return
  TRUE --  The request to set the PCI lock succeeded. \n
  FALSE -- The request to set the PCI lock failed. 

  @dependencies
  None.
*/
boolean cmapi_lte_set_pci_lock_req
(
  uint32  dl_earfcn, 
  uint16  pci 
);

/*===========================================================================

FUNCTION cmapi_lte_set_earfcn_lock_req

===========================================================================*/

/** @ingroup cmapi_lte_set_earfcn_lock_req
  Sets the LTE-RRC EARFCN lock.

  The EARFCN1 and EARFCN2 parameters enable locking two frequencies and toggling
  between them. To lock a single frequency, EARFCN2 can be the same as EARFCN1
  or invalid. 

  @param[in] dl_earfcn1 EARFCN value of the first lock.
  @param[in] dl_earfcn2 EARFCN value of the second lock.

  @return
  TRUE --  The request to set the EARFCN lock succeeded. \n
  FALSE -- The request to set the EARFCN lock failed. 

  @dependencies
  None.
*/
boolean cmapi_lte_set_earfcn_lock_req
(
  uint32  dl_earfcn1, 
  uint32  dl_earfcn2 
);


/*===========================================================================

FUNCTION cmapi_lte_get_eMBMS_info

===========================================================================*/
/** @ingroup cmapi_lte_get_eMBMS_info

  Get LTE eMBMS information.
 
  @param[in] cmapi_lte_eMBPS_info_s Pointer to eMBMS(enhanced Multimedia
             Broadcast/Multicast Service) info.
 
  @return
  None

  @dependencies
  None.
*/
/*===========================================================================*/
void cmapi_lte_get_eMBMS_info
(
  cmapi_lte_eMBPS_info_s *eMBMS_info_ptr 
);


/*===========================================================================

                     cmapi_change_scc_rx_diversity

===========================================================================*/
/*!
    @brief
    This function changes LTE RX diversity on a SCC

    @detail
    This function changes LTE RX diversity on a SCC.
 
    Note:
    LTE RX diversity can only changed when LTE RX is tuned, and RF S/W
    is not performing any other RF activities.
    
    @return
    TRUE:   if it is successfully changed
    FALSE:  if not.
     the failure cases include:
       -- RF S/W is busy and cannot take care of RX diversity change
           request
       -- RF S/W returns error when it performs the diversity changing
       -- LTE is busy and cannot take the request 
*/
/*=========================================================================*/
boolean cmapi_change_scc_rx_diversity
(
  uint8   scc_id,      /* SCC index, starting from 0 */
  boolean enable_rx0,  /* TRUE to enable RX0, FALSE to disable RX0 */
  boolean enable_rx1   /* TRUE to enable RX1, FALSE to disable RX1 */
);

/*===========================================================================

                     cmapi_efs_read

===========================================================================*/
/*!
    @brief
    This function calls the tech specific API to reread the files from EFS
    as specified in the 64 bit bitmask

    @return
    cmapi_err_e_type
      CMAPI_SUCCESS: if the tech specific API returns success
      Other error codes:  if the tech specific API returns failure
*/
/*=========================================================================*/
cmapi_err_e_type cmapi_efs_read
(
  cmapi_sys_mode_e_type       sys_mode,
  uint64                      bitmask
);

/*===========================================================================

                     cmapi_set_lte_deep_sleep

============================================================================*/
/** @ingroup cmapi_set_lte_deep_sleep
    Calls the lte_ml1_sleepmgr_enable_disable_deep_sleep() function to 
    to enable/disable LTE deep sleep.
    
    @datatypes
    boolean

    @param[in] setval   LTE deep sleep indicator.

    @return
*/
/*==========================================================================*/

void cmapi_set_lte_deep_sleep
(
  boolean setval
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*CMAPI_H*/

