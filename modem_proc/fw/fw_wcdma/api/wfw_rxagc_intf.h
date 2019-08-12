/*! @brief!
  @file

  @brief
  WCDMA FW AGC interface definitions header file
*/

/*! @brief===========================================================================

  Copyright (c) 2007-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*! @brief===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_wcdma/api/wfw_rxagc_intf.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/11/11   WX      Add DVGA gain offset async parameter
04/07/11   WX      RxAGC async write/read interface update for NikeL
06/04/10   WB      Moved RxAGC cmd word constant defintions from wfw_rxagc.h
05/27/10   WB      Add rxAgcOn flag to interface 
10/20/09   NC      Remove extra LNA SBI interface parameter
09/01/09   EM      New interface change to support 6 LNA gain states
03/30/09   EM      Split SWI to LNA processing and dual carrier process
===========================================================================*/

#ifndef WFW_AGC_INTF_H
#define WFW_AGC_INTF_H

/*! @brief===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Public constants and enums */
/* -------------------------- */
#define WFW_RXAGC_MAX_ANTENNAS 2
#define WFW_RXAGC_MAX_LNAS_PER_ANTENNA 2
#define WFW_RXAGC_MAX_CARRIERS 2

/* Structure definitions */
/* --------------------- */

/*! @name Structure definition for AGC SW-FW interface */
/*!@{*/
/*! @brief WCDMA FW AGC information update */

/*! @brief RxAGC Async Read/Write variables*/
typedef struct
{

  /*! @brief Specifies control flag for enabling the jammer detection */
  UWord8 rxJammerDetectEnable;

  /*! @brief Specifies the accumulated log energy of reference
     signal during steady state.*/
  UWord16 rxLogReference;

  /*! @brief Specifies the scale factor for AGC Adjust Value in
     steady state. */
  UWord16 rxGainConstant;

  /*! @brief Initial RSSI value for the LNA state 0-5 */
  Word16 rxAgcValueCmInit[6];

  /*! @brief Specifies the accumulated log energy of reference signal
     during compress mode. */
  Word16 rxCmLogReference;

  /*! @brief RxAGC DVGA gain offset setting */
  Word16 rxDvgaOffset;

}WfwRxAgcAsyncWrStruct;

typedef struct
{
  /*! NBR tunable parameters */
  /*! rotator frequency (in KHz) to be programmed to RXFE_NB_CSR_FREQ_OFFSET_NB0; mainly used by RFSW */
  UWord32 rotatorFreq;           
  /*! num of bpgs to accumulate for NBEE */
  UWord8  accumBpg;              
  /*! threshold1 - for checking against Gmax_Gmin_Power_Diff which is log2(GmaxPwr/GminPwr) */
  Word8   threshold1;            
  /*! threshold2 - for checking against W_G_Power_Diff which is log2(WPwr/GmaxPwr) */
  Word8   threshold2;            
} WfwNbrAsyncWrStruct;


typedef struct
{
  /*! @brief Specifies a flag for freezing internal accumulation of real
       data. */
  UWord8 rxFreezeAccum;

  /*! @brief Specifies a flag for overriding LNA Range. */
  UWord8 rxLnaRForceOride;

  /*! @brief Specifies overwrite value for LNA Range. This value is
     used to set the initial internal lna state variable (lnaDecision) to the mid gain
     value. This should match with the initial gain settings of the LNAs by the ARM */
  UWord8 rxLnaRForceWR;

  /*! @brief Specifies the number of interrupts before switching off
     both/either LNA in steady state. */
  UWord16 rxNonBypassTimer;

  /*! @brief Specifies the number of interrupts before switching off
     both/either LNA in steady state. */
  UWord16 rxBypassTimer;

  /*! @brief Specifies the number of interrupts before switching up
      the LNA state */
  UWord16 lnaUpDelayCntInit;

  /*! @brief Specifies the number of interrupts before switching down
      the LNA state */
  UWord16 lnaDownDelayCntInit;

  /*! @brief Specifies the number of interrupts to stay at the current
      LNA state */
  UWord16 lnaStayCntInit;


  /*! @brief LNA Gain Offset for state 0-5 */
  Word16 rxLnaOffset[6];

  /*! @brief Limits max value of AGC loop integrator. */
  Word16 rxAgcMaxVal;

  /*! @brief Linearizer underflow (LNA 1 bypassed). */
  Word16 rxAgcValMin[6];

  /*! @brief Lowest Rise threshold for LNA 
     The first 4 items are for LNARise 
     (LNARise? in MSW, 0x8000 in LSW) Ex: LNARise?8000 
     The second 4 items are for LNAFall
     (0x7fff in MSW, LNAFall1 in LSW) Ex: 0x7fffLNAFall?

     New in Genesis: 6 gain states hence 5 sets of Lna rise/Fall values
      */     
  Word32 rxLnaRiseFall[10];

  /*! @brief This threshold is used to switch from LNA state 2 to state 3.
     If the CGAGC accumulation is greater than cmLnaRise1
     then the LNAs will switch over to a low gain state of 3 from a
     state of 2. */
  UWord32 rxCmLnaRise1;

  /*! @brief This threshold is used to switch from LNA state 3 to state 4.
     If the CGAGC accumulation is greater than cmLnaRise2
     then the LNAs will switch over to a low gain state of 4 from a
     state of 3. */
  UWord32 rxCmLnaRise2;

  /*! @brief This threshold is used to switch from LNA state 4 to state 5.
     If the CGAGC accumulation is greater than cmLnaRise3
     then the LNAs will switch over to a low gain state of 4 from a
     state of 3. */
  UWord32 rxCmLnaRise3;

  /*! @brief This threshold is used to switch from LNA state 2 and
     state1. If the CGAGC accumulation is less than cmLnaFall1
     then the LNAs switch over to a high gain state of 1from a
     state of 2. */
  UWord32 rxCmLnaFall1;

  /*! @brief This threshold is used to switch from LNA state 1 and state
     0. If the CGAGC accumulation is less than cmLnaFall2 then
     the LNAs will switch over to a high gain state of 0 from a
     state of 1. */
  UWord32 rxCmLnaFall2;
}WfwRxAgcLnaAsyncWrStruct;


typedef struct
{
  /*! @brief This variable indicates if RxAGC is turned ON (1) for 
      this chain or turned OFF (0) */
  UWord8 rxAgcOn;

  /*! @brief This variable indicates if RxAGC is enabled (1) for 
      this chain or disabled (0) */
  UWord8 rxAgcEnable;

  /*! @brief Specifies internal accumulation of AGC Adjust Value.
     RSSI = rxAgcValue .12 + constant. */
  Word16 rxAgcValue;

  /*! @brief Accumulated received signal. This quantity models the
     received power at the antenna. It is the value which is
     sent back to the adder in the feedback loop */
  Word16 rxAccumOut;

  /*! @brief This variable contains value of the Rx AGC RSSI in
     db10 units, in a compressed mode gap. */
  Word16 cmSrchRxAgcVal;

  /*!@}*/
}WfwRxAgcAsyncRdStruct;

typedef enum
{
  WFW_NBR_RESULT_WCDMA_SIGNAL_DETECTED = 0,
  WFW_NBR_RESULT_GSM_SIGNAL_DETECTED = 1,
  WFW_NBR_RESULT_MAX
} WfwNbrResultType;

#define WFW_NBR_NUM_GSM_NB     3

/*! max bpg count for NBR accummulation */
#define WFW_NBR_LOG_2_MAX_ACCUM_BPGS        5
#define WFW_NBR_MAX_ACCUM_BPGS              (1 << WFW_NBR_LOG_2_MAX_ACCUM_BPGS)

typedef struct
{
  /*! NBR result */
  WfwNbrResultType nbrResult;                 
  /*! accummulated nbr power */
  UWord32 i2q2NbrAccPwr[WFW_NBR_NUM_GSM_NB];    
  /*! per bpg nbr power */
  UWord32 i2q2NbrPwrs[WFW_NBR_MAX_ACCUM_BPGS][WFW_NBR_NUM_GSM_NB];
  /*! accummulated prim NB power */
  UWord32 i2q2PrimNbAccPwr;                     
  /*! per bpg nbr power */
  UWord32 i2q2PrimNbPwrs[WFW_NBR_MAX_ACCUM_BPGS];                     
  /*! P_Gsm min */
  UWord32 minG;
  /*! P_Gsm max */
  UWord32 maxG;
  /*! P_Gsm_diff */
  Word32  pwrGsmDiff;                           
  /*! P_W_G_diff */
  Word32  pwrWGDiff;                            
  /*! rxagc power in dB10 */
  Word32  rxAgcPwrdB10;                         
  /*! latch of rxagc->rxAccumOutEval */ 
  Word32  rxAccumOutEval;                       

} WfwNbrAsyncRdStruct;

typedef struct
{
  /*! @brief WB power estimator result */
  UWord32 wbPwrAccumResult;

  /*! @brief LNA state for the WB pwr result */
  UWord16 wbPwrLnaDecision;
}WfwRxAgcWbPwrRdStruct;

typedef struct
{

  /*! @brief Its a read only variable which gives the values of LNA
     state. */
  UWord16 rxLnaDecision;

  /*! @brief This variable will log the LNA state in Compressed 
      mode or Interrat GAP */
  UWord16 cmRxLnaDecision;

  /*! @brief WB power estimator report to L1 */
  WfwRxAgcWbPwrRdStruct wbPwr;

  /*!@}*/
}WfwRxAgcLnaAsyncRdStruct;
 
/*! @brief RxAGC Command Processing Bmsk Struct */
typedef union
{
  struct
  {
    /*! @brief Whether AGC cmd is pending on Antenna 0 Carrier 0 */
    UWord32 a0c0 : 1;  /* Bits 0 */
    /*! @brief Whether AGC cmd is pending on Antenna 1 Carrier 0 */
    UWord32 a1c0 : 1;  /* Bits 1 */
    /*! @brief Whether AGC cmd is pending on Antenna 0 Carrier 1 */
    UWord32 a0c1 : 1;  /* Bits 2 */
    /*! @brief Whether AGC cmd is pending on Antenna 1 Carrier 1 */
    UWord32 a1c1 : 1;  /* Bits 3 */
    /*! @brief Whether AGC cmd is pending on Antenna 0 Carrier 2 */
    UWord32 a0c2 : 1;  /* Bits 4 */
    /*! @brief Whether AGC cmd is pending on Antenna 1 Carrier 2 */
    UWord32 a1c2 : 1;  /* Bits 5 */
    /*! @brief Whether AGC cmd is pending on Antenna 0 Carrier 3 */
    UWord32 a0c3 : 1;  /* Bits 6 */
    /*! @brief Whether AGC cmd is pending on Antenna 1 Carrier 3 */
    UWord32 a1c3 : 1;  /* Bits 7 */
    UWord32      : 24; /* Unused Bits  */
  };
  /*! @brief RxAGC Command Processing Bmsk Word */
  UWord32 word;
} WfwRxAgcCmdProcBmskStruct;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* WFW_AGC_INTF_H */

