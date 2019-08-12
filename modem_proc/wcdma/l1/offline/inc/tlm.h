/*===========================================================================
                               T L M . H

GENERAL DESCRIPTION

  This file contains all of the declarations of externalized functions and 
  data structure definitions for the Time Line Manager (TLM).

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright(c)2007 - 2014 by Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/tlm.c_v   1.153   17 Jul 2002 16:36:16   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/tlm.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
02/11/15    ap      TLM fix for X2W HHO scenarios 
07/23/14    abs     Added new API TLM_GET_SUBFN_INFO_FROM_WRC, new macro TLM_ADD_SUBFN_QUANT
                    and new data structure TLM_SUBFN_STRUCT_TYPE
06/26/14    hk      DL Features Mainline
05/30/14    pr      Use cell info to calculate CFN from CFN-WRC Diff
03/06/14    pr      L1 support for MVS voice packet exchange.
09/14/13    pr      VSTMR Changes for BOLT
10/14/13    pr/sr   Channel config optimizations
08/12/13    pr      Change the mechanism in tlm set align for timing unknown cases.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv      Feature HS RACH Initial code check in
05/29/12    kcm     Added API TLM_GET_ALIGN_CHAN_POS to get the dpch pos from 
                    Rx combiner
04/16/12    srk     Enabled time tracking for CFN_WRC diff
03/16/12    amj     Added macro for getting bpg cnt from cx8 quantity.
03/09/12    amj     Moved some APIs to do some CFN/SFN arithmetic here.
02/15/12    hk      Fixed compilation error
02/15/12    hk      Fixed compilation warning
02/15/12    hk      Timeline alignment changes for HHO & W2G Failure recovery
02/10/12    hk      Changes to pick the TTO based on ref cell for time tracking
01/11/12    hk      Mechanism to calculate CFN from CFN-WRC diff
11/21/11    sp      API to provide Tx Sys Time for UL
07/07/11    hk      Up-merge from offline/main/latest
04/28/11    sj      upmerge from offline/main
04/08/11    rgn     Added macro to get subframe number from cx1/8
03/29/11    hk      Removed the inclusion of msmhwioreg.h from tlm.h
03/29/11    hk      Added a new API to calculate the Channel CFN
03/29/11    hk      Added a new TLM API to get RxTL SFN and WRC
03/11/11    dp      Ported CPC DRX changes to offline Architecture
12/29/10    dm      Added function for calculating AICH and RACH timing for 
                    RACH debugging
10/15/10    yh      Added support for W2L Compressed mode
10/08/10    vb      Added support for TLM thin API to provide aligned cell SFN
08/14/10    vb      Added util function to get aligned channel time 
                    using sfn_wrc of aligned cell
07/23/10    hk      Corrected the calculation of TST-WRC diff
07/21/10    hk      Workaround to calculate the Transmit System Time
05/03/10    hk      Fixed compiler warnings
02/10/10    ms      Corrected redeclaration of seq_align_type
09/30/09    hk      Replaced INTLOCK with Mutex
09/29/09    hk      Added support for Extended SFN counter
07/02/09    hk      Changed the implementation to calculate Rx_Tx difference
                    from STMR status dump
05/08/09    hk      Added a function to calculate Tm
03/11/09    rgn     Added macros to get dyanmic frc num/cx count
02/09/09    vb      Added proto type for align cell info update for aset proc
12/29/08    hk      Added a new function to calculate channel cfn from combiner
01/15/09    vsr     Reverting last change
01/14/09    vb      Corrected procedure to calculate ch_frame_pos_wrt_rxtl
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED and removing RUMI_BRING_UP
11/19/08    vsr     Added changes to update align cell info for TM HHO
10/16/08    hk      Changes to make use the hardware WRC
10/03/08    hk      Bringing in 7k Mailine fixes
05/28/08    vsr     Fixing TLM related merge issues
01/25/08    vsr     Checking in Enhanced DL files onto mainline
12/20/07    vp      Added support for CNT_C_SFN difference computation.
08/08/07   gv/vp    Added support for MCAL_EDL and consolidated APIs in dlphch
                    to get rl_offset.
07/05/07    vsr     Moved TLM GET CFN macro to tlm.h, added proto type 
                    tlm_update_combiner_cfn_change
05/17/07    vsr     Corrected tlm functions maintaining cfn snapshots and hw_sw_cfn_diff
04/25/07    vp/vsr  Lint Fixes
03/21/07    scm     Add boolean status return value to tlm_get_ch_sfn().
03/02/07    scm     Fix lint issue.
02/21/07    vp      Removed unwanted snapshot functions
                    
                    ===========================================================================*/
#ifndef TLM_H
#define TLM_H
/*--------------------------------------------------------------------------
                      Included Header Files
--------------------------------------------------------------------------*/
#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "l1const.h"
#include "stmr.h"

/*------------------GLOBAl VARIABLES DEFINITION BEGIN------------------------------------*/
/* This Macro reads the current software cfn
*/
#define TLM_GET_CFN() (seq_get_cfn())

/* This Macro reads the current software sfn
*/
#define TLM_GET_TFN() (seq_get_sfn())

#define TLM_INVALID_TFN_FRAME_QUANT (0xFFFF)

/*Extended SFN frame quantity is a signed 32 bit quantity*/
#define TLM_INVALID_EXTENDED_SFN_FRAME_QUANT (-1)

#define TLM_INVALID_TFN_CX8_QUANT (0xFFFFFFFF)

#define L1_INVALID_RX_TX_DIFF_VAL (0xFFFF)

#define TLM_INVALID_CFN_FRAME_QUANT (0xFFFF)

#define TLM_INVALID_CFN_CX8_QUANT (0xFFFFFFFF)

/* This Macro holds the resolution of TLM module.
*/
#define TLM_UNITS_PER_FRAME (CHIPX8_PER_FRAME)

#define TLM_HALF_FRAME_CX8 (153600L)

/* This Macro holds the maximum value of the Extended SFN quantity.
*/
#define TLM_MAX_EXTENDED_SFN_COUNT (MAX_EXTENDED_SFN_COUNT)

/* This Macro holds the maximum value of the TFN quantity.
*/
#define TLM_MAX_TFN_COUNT (MAX_SFN_COUNT)

/* This Macro holds the maximum value of the CFN quantity.
*/
#define TLM_MAX_CFN_COUNT (MAX_CFN_COUNT)

/* This Macro holds the maximum value of the FRC quantity.
*/
#define TLM_MAX_FRC_COUNT (4)

/* This Macro holds the maximum value of the TST (Tx System Time) quantity.
*/
#define TLM_MAX_TST_COUNT (0x8)

#ifdef FEATURE_WCDMA_HS_RACH
#define TLM_INVALID_CFN_FRAME_QUANT (0xFFFF)

#define TLM_INVALID_CFN_CX8_QUANT (0xFFFFFFFF)
#endif /* FEATURE_WCDMA_HS_RACH*/

/*This macro copies two tfn quantities passed as parameter*/
#define TLM_COPY_TLM_TFN(source, dest)         \
  do {         \
    (dest).frame_qty = (source).frame_qty;         \
    (dest).chip_offset_cx8_qty = (source).chip_offset_cx8_qty;         \
  }while(0)

/*This macro could be made as MSG_ERROR or ERR_FATAL*/
#define TLM_ERROR(string,param1, param2, param3)   \
            ERR_FATAL(string, param1, param2, param3)

/* This Macro reads the source of the RX timeline
*/
#define TLM_GET_RXTL_SRC() (stmr_get_cntr_src(MCALWCDMA_EVT_RX_TL_INTR))
#define TLM_GET_TXTL_SRC() (stmr_get_cntr_src(MCALWCDMA_EVT_TX_TL_INTR))


#define TLM_ADD_TWO_SFN(result, addend1, addend2)         \
  do {         \
    (result).chip_offset_cx8_qty = (addend1).chip_offset_cx8_qty + (addend2).chip_offset_cx8_qty;         \
    if ((result).chip_offset_cx8_qty >= TLM_UNITS_PER_FRAME)         \
    {         \
      (result).chip_offset_cx8_qty = (result).chip_offset_cx8_qty - TLM_UNITS_PER_FRAME;         \
      (result).frame_qty = (addend1).frame_qty + (addend2).frame_qty+1;         \
    }         \
    else         \
    {         \
      (result).frame_qty = (addend1).frame_qty + (addend2).frame_qty;         \
    }         \
    if((result).frame_qty >= TLM_MAX_TFN_COUNT)         \
    {         \
      (result).frame_qty = (result).frame_qty - TLM_MAX_TFN_COUNT;         \
    }         \
  }while(0)

#define TLM_SUB_TWO_SFN(result, minuend, subtrahend )         \
  do {         \
    (result).chip_offset_cx8_qty = (minuend).chip_offset_cx8_qty - (subtrahend).chip_offset_cx8_qty;         \
    if ((result).chip_offset_cx8_qty < 0)         \
    {         \
      (result).chip_offset_cx8_qty = (result).chip_offset_cx8_qty + TLM_UNITS_PER_FRAME;         \
      (result).frame_qty = ((minuend).frame_qty - (subtrahend).frame_qty) - 1;         \
    }         \
    else         \
    {         \
      (result).frame_qty = (minuend).frame_qty - (subtrahend).frame_qty;         \
    }         \
    if((result).frame_qty < 0)         \
    {         \
      (result).frame_qty = (result).frame_qty + TLM_MAX_TFN_COUNT;         \
    }         \
  }while(0)

#define TLM_SUB_TWO_EXTENDED_SFN(result, minuend, subtrahend )         \
  do {         \
    (result).chip_offset_cx8_qty = (minuend).chip_offset_cx8_qty - (subtrahend).chip_offset_cx8_qty;         \
    if ((result).chip_offset_cx8_qty < 0)         \
    {         \
      (result).chip_offset_cx8_qty = (result).chip_offset_cx8_qty + TLM_UNITS_PER_FRAME;         \
      (result).frame_qty = ((minuend).frame_qty - (subtrahend).frame_qty) - 1;         \
    }         \
    else         \
    {         \
      (result).frame_qty = (minuend).frame_qty - (subtrahend).frame_qty;         \
    }         \
    if((result).frame_qty < 0)         \
    {         \
      (result).frame_qty = (result).frame_qty + TLM_MAX_EXTENDED_SFN_COUNT;         \
    }         \
  }while(0)

#define TLM_SUB_SFN_CFN(result, minuend_sfn, subtrahend_cfn)         \
  do {         \
    (result).chip_offset_cx8_qty = (minuend_sfn).chip_offset_cx8_qty - (subtrahend_cfn).chip_offset_cx8_qty;         \
    if ((result).chip_offset_cx8_qty < 0)         \
    {         \
      (result).chip_offset_cx8_qty = (result).chip_offset_cx8_qty + TLM_UNITS_PER_FRAME;         \
      (result).frame_qty = (minuend_sfn).frame_qty - (subtrahend_cfn).frame_qty-1;         \
    }         \
    else         \
    {         \
      (result).frame_qty = (minuend_sfn).frame_qty - (subtrahend_cfn).frame_qty;         \
    }         \
    if((result).frame_qty < 0)         \
    {         \
      (result).frame_qty = (result).frame_qty + TLM_MAX_TFN_COUNT;         \
    }         \
  }while(0)

#define TLM_CONVERT_TFN_TO_CFN(tfn, cfn)       \
  do {       \
    (cfn).frame_qty = (tfn).frame_qty % TLM_MAX_CFN_COUNT;       \
    (cfn).chip_offset_cx8_qty = (tfn).chip_offset_cx8_qty;       \
  } while (0)

#define TLM_CONVERT_FRAME_QUANT_TFN_TO_CFN(tfn, cfn)       \
  do {       \
    (cfn) = (tfn) % TLM_MAX_CFN_COUNT;       \
  } while (0)

/*Macros for CFN: to add and subtract*/
#define TLM_ADD_TWO_CFN(result, addend1, addend2)         \
  do {         \
    (result).chip_offset_cx8_qty = (addend1).chip_offset_cx8_qty + (addend2).chip_offset_cx8_qty;         \
    if ((result).chip_offset_cx8_qty >= TLM_UNITS_PER_FRAME)         \
    {         \
      (result).chip_offset_cx8_qty = (result).chip_offset_cx8_qty - TLM_UNITS_PER_FRAME;         \
      (result).frame_qty = (addend1).frame_qty + (addend2).frame_qty+1;         \
    }         \
    else         \
    {         \
      (result).frame_qty = (addend1).frame_qty + (addend2).frame_qty;         \
    }         \
    if((result).frame_qty >= TLM_MAX_CFN_COUNT)         \
    {         \
      (result).frame_qty = (result).frame_qty - TLM_MAX_CFN_COUNT;         \
    }         \
  }while(0)

#define TLM_SUB_TWO_CFN(result, minuend, subtrahend )         \
  do {         \
    (result).chip_offset_cx8_qty = (minuend).chip_offset_cx8_qty - (subtrahend).chip_offset_cx8_qty;         \
    if ((result).chip_offset_cx8_qty < 0)         \
    {         \
      (result).chip_offset_cx8_qty = (result).chip_offset_cx8_qty + TLM_UNITS_PER_FRAME;         \
      (result).frame_qty = (minuend).frame_qty - (subtrahend).frame_qty-1;         \
    }         \
    else         \
    {         \
      (result).frame_qty = (minuend).frame_qty - (subtrahend).frame_qty;         \
    }         \
    if((result).frame_qty < 0)         \
    {         \
      (result).frame_qty = (result).frame_qty + TLM_MAX_CFN_COUNT;         \
    }         \
  }while(0)

#define TLM_SUB_TWO_CFN_FRAME_QUANT(result, minuend, subtrahend)         \
  do {         \
    (result) = (minuend) - (subtrahend);         \
    if((result) < 0)         \
    {         \
      (result) = (result) + TLM_MAX_CFN_COUNT;         \
    }         \
  }while(0)

#define TLM_SNAPSHOT_MASK_NONE ((tlm_snapshot_needed_mask_type)0)

#define TLM_SNAPSHOT_MASK_WRC ((tlm_snapshot_needed_mask_type)0x1)

#define TLM_SNAPSHOT_MASK_RXTL_TFN ((tlm_snapshot_needed_mask_type)0x2)

#define TLM_SNAPSHOT_MASK_FRC ((tlm_snapshot_needed_mask_type)0x4)

#define TLM_SNAPSHOT_MASK_TST ((tlm_snapshot_needed_mask_type)0x8)

#define TLM_SNAPSHOT_MASK_RQSTD_COMB_CFN ((tlm_snapshot_needed_mask_type)0x10)

/*Callers of tlm_get_snapshot(), tlm as well as non-tlm,  should not use the
 * bit mask TLM_SNAPSHOT_MASK_RXTL_SRC_COMB_CFN. It is for the internal purposes of
 * tlm_get_snapshot()*/
#define TLM_SNAPSHOT_MASK_RXTL_SRC_COMB_CFN ((tlm_snapshot_needed_mask_type)0x20)

#define TLM_SNAPSHOT_SMALLEST_BIT_MASK (TLM_SNAPSHOT_MASK_WRC)

#define TLM_SNAPSHOT_LARGEST_BIT_MASK (TLM_SNAPSHOT_MASK_RXTL_SRC_COMB_CFN)

/*RXTL_TFN and WRC can be computed after the STMR read is done.
 * Only the STMR read needs a WCDMA_INTLOCK; RXTL_TFN can be computed 
 * outsude of WCDMA_INTLOCK*/
#define TLM_REMOVE_MASK_FOR_STMR_READ ((tlm_snapshot_needed_mask_type)(~(TLM_SNAPSHOT_MASK_RXTL_TFN | TLM_SNAPSHOT_MASK_WRC)))

#define TLM_DEPENDANT_MASK_FOR_WRC (TLM_SNAPSHOT_MASK_FRC)

#define TLM_DEPENDANT_MASK_FOR_RXTL_TFN (TLM_SNAPSHOT_MASK_NONE)

#define TLM_DEPENDANT_MASK_FOR_FRC (TLM_SNAPSHOT_MASK_NONE)

#define TLM_DEPENDANT_MASK_FOR_TST (TLM_SNAPSHOT_MASK_NONE)

#define TLM_DEPENDANT_MASK_FOR_RQSTD_COMB_CFN (TLM_SNAPSHOT_MASK_NONE)

#define TLM_DEPENDANT_MASK_FOR_RXTL_SRC_COMB_CFN (TLM_SNAPSHOT_MASK_NONE)

#define TLM_SNAPSHOT_MASK_HAS_RXTL_TFN(mask)  \
  ((mask & TLM_SNAPSHOT_MASK_RXTL_TFN) != 0)

#define TLM_SNAPSHOT_MASK_HAS_WRC(mask)  \
  ((mask & TLM_SNAPSHOT_MASK_WRC) != 0)

#define TLM_SNAPSHOT_MASK_HAS_FRC(mask)  \
  ((mask & TLM_SNAPSHOT_MASK_FRC) != 0)

#define TLM_SNAPSHOT_MASK_HAS_TST(mask)  \
  ((mask & TLM_SNAPSHOT_MASK_TST) != 0)

#define TLM_SNAPSHOT_MASK_HAS_RQSTD_COMB_CFN(mask)  \
  ((mask & TLM_SNAPSHOT_MASK_RQSTD_COMB_CFN) != 0)

#define TLM_SNAPSHOT_MASK_FOR_DBG_DCH_NON_FRC (TLM_SNAPSHOT_MASK_RQSTD_COMB_CFN | TLM_SNAPSHOT_MASK_TST)

#define TLM_SNAPSHOT_MASK_FOR_DBG_NON_DCH_NON_FRC (TLM_SNAPSHOT_MASK_RQSTD_COMB_CFN)

#define TLM_SNAPSHOT_MASK_FOR_DBG_FRC_ALGN (TLM_SNAPSHOT_MASK_FRC)

#define TLM_SNAPSHOT_MASK_FOR_DBG_CH_SETUP (TLM_SNAPSHOT_MASK_FRC | TLM_SNAPSHOT_MASK_RQSTD_COMB_CFN)

#define TLM_SNAPSHOT_MASK_FOR_SFN_SFN_DIFF (TLM_SNAPSHOT_MASK_RXTL_TFN | TLM_SNAPSHOT_MASK_WRC)

#define TLM_SNAPSHOT_MASK_FOR_SFN_CFN_DIFF (TLM_SNAPSHOT_MASK_TST | TLM_SNAPSHOT_MASK_RXTL_TFN | TLM_SNAPSHOT_MASK_WRC)

#define TLM_SNAPSHOT_MASK_FOR_CNT_C_SFN_DIFF (TLM_SNAPSHOT_MASK_TST | TLM_SNAPSHOT_MASK_RXTL_TFN | TLM_SNAPSHOT_MASK_WRC)

#define TLM_SNAPSHOT_MASK_FOR_SFN_OF_CELLS (TLM_SNAPSHOT_MASK_WRC)

#define TLM_SNAPSHOT_MASK_FOR_BEF_ALIGN (TLM_SNAPSHOT_MASK_RXTL_TFN | TLM_SNAPSHOT_MASK_FRC)

#define TLM_SNAPSHOT_MASK_FOR_AFT_ALIGN (TLM_SNAPSHOT_MASK_RXTL_TFN | TLM_SNAPSHOT_MASK_FRC)

#define TLM_SNAPSHOT_MASK_FOR_CH_SFN (TLM_SNAPSHOT_MASK_WRC)

#define TLM_SNAPSHOT_MASK_FOR_SFN_WRC_ADJUST (TLM_SNAPSHOT_MASK_WRC | TLM_SNAPSHOT_MASK_FRC)

#define TLM_SNAPSHOT_MASK_FOR_CH_CFN_INFO (TLM_SNAPSHOT_MASK_WRC | TLM_SNAPSHOT_MASK_RXTL_TFN | TLM_SNAPSHOT_MASK_FRC)

#define TLM_SNAPSHOT_MASK_FOR_CH_CFN_FROM_COMB (TLM_SNAPSHOT_MASK_RQSTD_COMB_CFN | TLM_SNAPSHOT_MASK_RXTL_TFN | TLM_SNAPSHOT_MASK_WRC)

#define TLM_SNAPSHOT_MASK_FOR_CH_CFN_FROM_WRC_DIFF (TLM_SNAPSHOT_MASK_RXTL_TFN | TLM_SNAPSHOT_MASK_WRC | TLM_SNAPSHOT_MASK_FRC)

#define TLM_SNAPSHOT_MASK_FOR_COMP_SFN_WRC_DIFF (TLM_SNAPSHOT_MASK_RXTL_TFN | TLM_SNAPSHOT_MASK_RQSTD_COMB_CFN)

#define TLM_SNAPSHOT_MASK_FOR_DBG_TST (TLM_SNAPSHOT_MASK_RXTL_TFN| TLM_SNAPSHOT_MASK_TST | TLM_SNAPSHOT_MASK_FRC | TLM_SNAPSHOT_MASK_WRC)

#define TLM_SNAPSHOT_MASK_FOR_BEF_TL_INTR (TLM_SNAPSHOT_MASK_RXTL_TFN)

#define TLM_SNAPSHOT_MASK_FOR_AFT_TL_INTR (TLM_SNAPSHOT_MASK_RXTL_TFN)

#define TLM_SNAPSHOT_MASK_FOR_PHY_CH_CFG_READY (TLM_SNAPSHOT_MASK_FRC | TLM_SNAPSHOT_MASK_RQSTD_COMB_CFN)

#define TLM_SNAPSHOT_MASK_FOR_RXTL_CFN (TLM_SNAPSHOT_MASK_RXTL_TFN)

#define TLM_SNAPSHOT_MASK_FOR_DPCH_POS (TLM_SNAPSHOT_MASK_FRC | TLM_SNAPSHOT_MASK_RQSTD_COMB_CFN)

#define TLM_SNAPSHOT_MASK_FOR_CALC_TM (TLM_SNAPSHOT_MASK_FRC | TLM_SNAPSHOT_MASK_TST | TLM_SNAPSHOT_MASK_WRC)

#define TLM_SNAPSHOT_MASK_FOR_CALC_TX_POS (TLM_SNAPSHOT_MASK_FRC | TLM_SNAPSHOT_MASK_TST | TLM_SNAPSHOT_MASK_WRC)

#define TLM_SNAPSHOT_MASK_FOR_RACH_DEBUG (TLM_SNAPSHOT_MASK_FRC | TLM_SNAPSHOT_MASK_TST | TLM_SNAPSHOT_MASK_WRC | TLM_SNAPSHOT_MASK_RQSTD_COMB_CFN)

#define TLM_SNAPSHOT_MASK_FOR_CFN_WRC_DIFF (TLM_SNAPSHOT_MASK_WRC | TLM_SNAPSHOT_MASK_RQSTD_COMB_CFN | TLM_SNAPSHOT_MASK_FRC | TLM_SNAPSHOT_MASK_RXTL_TFN)

#define TLM_SUB_CHIP_QUANT(result, minuend, subtrahend)       \
  do {       \
    (result) = (minuend) - (subtrahend);         \
    if ((result) < 0)         \
    {         \
      (result) = (result) + TLM_UNITS_PER_FRAME;         \
    }         \
  } while(0)

#define TLM_ADD_CHIP_QUANT(result, addend1, addend2)         \
  do {         \
    (result) = (addend1) + (addend2);         \
    if ((result) >= TLM_UNITS_PER_FRAME)         \
    {         \
      (result) = (result) - TLM_UNITS_PER_FRAME;         \
    }         \
  } while(0)

#define TLM_ADD_SUBFN_QUANT(result, addend1, addend2)         \
  do {         \
    (result) = (addend1) + (addend2);         \
    if ((result) >= MAX_SUBFN_COUNT)         \
    {         \
      (result) = (result) - MAX_SUBFN_COUNT;         \
    }         \
  } while(0)

/*Macros for TFN: to add and subtract*/
#define TLM_ADD_TWO_TFN_FRAME_QUANT(result, addend1, addend2)         \
  do {         \
    (result) = (addend1) + (addend2);         \
    if((result) >= TLM_MAX_TFN_COUNT)         \
    {         \
      (result) = (result) - TLM_MAX_TFN_COUNT;         \
    }         \
  }while(0)

#define TLM_SUB_TWO_TFN_FRAME_QUANT(result, minuend, subtrahend)         \
  do {         \
    (result) = (minuend) - (subtrahend);         \
    if((result) < 0)         \
    {         \
      (result) = (result) + TLM_MAX_TFN_COUNT;         \
    }         \
  }while(0)

#define TLM_INVALIDATE_EXTENDED_SFN(extended_sfn)    \
  do {    \
    (extended_sfn).frame_qty = (int32) TLM_INVALID_EXTENDED_SFN_FRAME_QUANT;    \
    (extended_sfn).chip_offset_cx8_qty = (int32) TLM_INVALID_TFN_CX8_QUANT;    \
  } while(0)

#define TLM_GET_SUBFR_FROM_CX1(cx1_val, subfr_val) \
   if(cx1_val >= 30720)                            \
   {                                               \
     subfr_val = 4;                                \
   }                                               \
   else if(cx1_val >= 23040)                       \
   {                                               \
     subfr_val = 3;                                \
   }                                               \
   else if(cx1_val >= 15360)                       \
   {                                               \
     subfr_val = 2;                                \
   }                                               \
   else if(cx1_val >= 7680)                        \
   {                                               \
     subfr_val = 1;                                \
   }                                               \
   else                                            \
   {                                               \
     subfr_val = 0;                                \
   }        


#define TLM_GET_SUBFR_FROM_CX8(cx1_val, subfr_val) \
   if(cx1_val >= 245760)                           \
   {                                               \
     subfr_val = 4;                                \
   }                                               \
   else if(cx1_val >= 184320)                      \
   {                                               \
     subfr_val = 3;                                \
   }                                               \
   else if(cx1_val >= 122880)                      \
   {                                               \
     subfr_val = 2;                                \
   }                                               \
   else if(cx1_val >= 61440)                       \
   {                                               \
     subfr_val = 1;                                \
   }                                               \
   else                                            \
   {                                               \
     subfr_val = 0;                                \
   }        

#define TLM_GET_BPG_CNT_FROM_CX8(cx8_val, bpg_cnt) \
   bpg_cnt = cx8_val >> (CHIPX8_TO_CHIP_RSHIFT + CHIP_TO_BPG_RSHIFT) /** divide by 2048 */

#define TLM_INVALIDATE_TFN(tfn)    \
  do {    \
    (tfn).frame_qty = (int16) TLM_INVALID_TFN_FRAME_QUANT;    \
    (tfn).chip_offset_cx8_qty = (int32) TLM_INVALID_TFN_CX8_QUANT;    \
  } while(0)

#define TLM_INVALIDATE_CFN(cfn)    \
  do {    \
    (cfn).frame_qty = (int16) TLM_INVALID_CFN_FRAME_QUANT;    \
    (cfn).chip_offset_cx8_qty = (int32) TLM_INVALID_CFN_CX8_QUANT;    \
  } while(0)

#define TLM_IS_CFN_INVALID(cfn)    \
  ((((cfn).frame_qty == (int16) TLM_INVALID_CFN_FRAME_QUANT) ||    \
  ((cfn).chip_offset_cx8_qty == (int32) TLM_INVALID_CFN_CX8_QUANT))? TRUE : FALSE)

#define TLM_IS_TFN_INVALID(tfn)    \
  ((((tfn).frame_qty == (int16) TLM_INVALID_TFN_FRAME_QUANT) ||    \
  ((tfn).chip_offset_cx8_qty == (int32) TLM_INVALID_TFN_CX8_QUANT))? TRUE : FALSE)

#define TLM_INVALIDATE_CFN(cfn)    \
  do {    \
    (cfn).frame_qty = (int16) TLM_INVALID_CFN_FRAME_QUANT;    \
    (cfn).chip_offset_cx8_qty = (int32) TLM_INVALID_CFN_CX8_QUANT;    \
  } while(0)

/*--------------------------------------------------------------------------
                   ENUM: TLM_CH_TIME_COMP_SCENARIO_ENUM_TYPE

This enum holds the types of channel time computation scenarios
--------------------------------------------------------------------------*/
typedef enum {
  TLM_CH_TIME_KNOWN_CELL_FRC_ALIGNED,
  TLM_CH_TIME_KNOWN_CELL_NON_FRC_ALIGNED,
  TLM_CH_TIME_UNKNOWN_CELL_FRC_ALIGNED,
  TLM_CH_TIME_UNKNOWN_CELL_NON_FRC_ALIGNED,
  TLM_CH_TIME_COMP_SCENARIO_NUM_TYPES
} tlm_ch_time_comp_scenario_enum_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_COMPUTE_SFN_WRC_INPUT_TYPE

This structure holds the information regarding the pccpch on which the CRC
state machine passed.
--------------------------------------------------------------------------*/
typedef struct {
/*Range is 0-4095*/
  uint16 decoded_sfn;

/*Range is 0-255*/
  uint16 mdsp_cfn;

/*Range is 0-7 for MSM7200*/
  uint8 hwch;
} tlm_compute_sfn_wrc_input_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_CFN_STRUCT_TYPE

This structure holds a CFN quantity in chipx8 resolution.The maximum value
for this quantity is 256 wcdma frames.
--------------------------------------------------------------------------*/
typedef struct
{
/*Range is 0-255*/
  int16 frame_qty;

/*Range is 0-307199*/
  int32 chip_offset_cx8_qty;
} tlm_cfn_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_NW_DIFF_STRUCT_TYPE

This structure holds a CFN quantity in chipx1 resolution.The maximum value
for this quantity is 256 wcdma frames.
--------------------------------------------------------------------------*/
typedef struct
{
/*Range is 0-255*/
  int16 frame_qty;

/*Range is 0-38399*/
  int32 chip_offset_cx1_qty;
} tlm_nw_diff_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_RX_TX_DIFF_STRUCT_TYPE

This structure holds the earliest Rx time calculated from the earliest finger 
  and the Rx Tx time difference
--------------------------------------------------------------------------*/
typedef struct {
  uint16 earliest_rx_time_cx1;
  uint16 rx_tx_diff_cx1;
} tlm_rx_tx_diff_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_TFN_STRUCT_TYPE

This structure holds a TFN quantity in chipx8 resolution.The maximum value
for this quantity is 4096 wcdma frames.
--------------------------------------------------------------------------*/
typedef struct {
/*Range is 0-4095*/
  int16 frame_qty;

/*Range is 0-307199*/
  int32 chip_offset_cx8_qty;
} tlm_tfn_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_SUBFN_STRUCT_TYPE

This structure holds a SubFN quantity in bpg and chipx8 resolution.The maximum value
for this quantity is 4096 wcdma frames.
--------------------------------------------------------------------------*/
typedef struct
{
/*Range is 0-4095*/
  int16 frame_qty;

/*Range is 0-149*/
  int8 bpg_qty;
} tlm_subfn_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_EXTENDED_SFN_STRUCT_TYPE

This structure holds a EXTENDED SFN quantity in chipx8 resolution.The maximum value
for this quantity is 0x7FFFFFFF wcdma frames.
--------------------------------------------------------------------------*/
typedef struct {
/*Range is 0-0x7FFFFFFF*/
  int32 frame_qty;

/*Range is 0-307199*/
  int32 chip_offset_cx8_qty;
} tlm_extended_sfn_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_CH_CFN_INFO_STRUCT_TYPE

This structure holds the cfn of a channel, difference between cfn and wrc, the position of the
channel frame boundary wrt the rx timeline.
--------------------------------------------------------------------------*/
typedef struct {
  int32 ch_frame_pos_wrt_rxtl;

  tlm_cfn_struct_type ch_cfn;

  tlm_tfn_struct_type wrc_cfn_diff;

  tlm_tfn_struct_type chan_cfg_time_wrc;

  tlm_tfn_struct_type rxtl_tfn;
} tlm_ch_cfn_info_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_FRC_STRUCT_TYPE

This structure holds a FRC quantity in chipx8 resolution.The maximum value
for this quantity is 4 wcdma frames.
--------------------------------------------------------------------------*/
typedef struct {
/*Range is 0-3*/
  int16 frame_qty;

/*Range is 0-307199*/
  int32 chip_offset_cx8_qty;
} tlm_frc_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_TST_STRUCT_TYPE

This structure holds a TST(Transmit System Time) quantity in chipx8 resolution.The maximum value
for this quantity is 8 wcdma frames.
--------------------------------------------------------------------------*/
typedef struct {
/*Range is 0-7*/
  int16 frame_qty;

/*Range is 0-307199*/
  int32 chip_offset_cx8_qty;
} tlm_tst_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_CELL_INFO_STRUCT_TYPE

This structure holds information that will identify a WCDMA cell.
--------------------------------------------------------------------------*/
typedef struct 
{
/*the psc of the cell*/
  uint16 psc;

/*the frequency of the cell*/
  uint16  freq;
} tlm_cell_info_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_SLIGN_TIMING_INFO_STRUCT_TYPE

This structure holds the key TLM data just before and after a align change.
--------------------------------------------------------------------------*/
typedef struct
{
  tlm_tfn_struct_type wrc_bef_align;

  tlm_tfn_struct_type tfn_bef_align;

  tlm_frc_struct_type frc_bef_align;

} tlm_align_timing_info_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_TFN_WRC_SS_TYPE

This structure holds TFN and WRC snapshot.
--------------------------------------------------------------------------*/
typedef struct {
  tlm_tfn_struct_type tfn;

  tlm_tfn_struct_type wrc;
} tlm_tfn_wrc_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_DB_STRUCT_TYPE

This structure holds all the key TLM data.
--------------------------------------------------------------------------*/
typedef struct
{
  tlm_tfn_struct_type wrc_tfn_diff;

  boolean tlm_align_pending;

  boolean tlm_align_just_happened;

  tlm_align_timing_info_struct_type align_info;

  stmr_counter_source_enum_type align_cntr;

  boolean align_cntr_hw_cfn_can_be_used;

  tlm_tfn_struct_type tfn_bef_hw_cfn_usage_stopped;

  tlm_frc_struct_type frc_bef_hw_cfn_usage_stopped;

  boolean align_cntr_hw_sw_cfn_diff_update_needed;

  boolean align_cntr_slaming_pending;

  int16 hw_sw_cfn_diff;

  boolean any_align_happened;

  tlm_cell_info_struct_type align_cell_info;

  tlm_tfn_struct_type align_ch_off;

  tlm_tfn_struct_type align_cell_sfn_wrc_diff;
  
  boolean align_cell_sfn_wrc_valid;

  tlm_tfn_wrc_struct_type tfn_wrc_before_tl_interuption;

  tlm_cfn_struct_type cfn_wrc_diff;
} tlm_db_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_TFN_TST_SS_TYPE

This structure holds TFN and TST snapshot.
--------------------------------------------------------------------------*/
typedef struct {
  tlm_tfn_struct_type tfn;

  uint32 tst_status;
} tlm_tfn_tst_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_SNAPSHOT_STRUCT_TYPE

This structure holds TFN, WRC, TST and FRC snapshot.
--------------------------------------------------------------------------*/
typedef struct {
  tlm_tfn_struct_type rxtl_tfn;

  tlm_cfn_struct_type comb_cfn;

  tlm_tfn_struct_type wrc;

  tlm_tst_struct_type tst;

  tlm_frc_struct_type frc;
} tlm_snapshot_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_CH_TIME_COMP_PARAMS_STRUCT_TYPE

This structure holds the characteristics of each channel computation scenario.
--------------------------------------------------------------------------*/
typedef struct {
  boolean cell_time_known;

  boolean ref_cnt_adj_needed;

  boolean deskew_depth_to_be_applied;
} tlm_ch_time_comp_params_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_RX_TX_RACH_DEBUG_STRUCT_TYPE

This structure holds the Rx and Tx times calculated for RACH debugging
--------------------------------------------------------------------------*/
typedef struct {
  int32 rx_pos;
  int32 rx_pos_ref;
  int32 tx_pos;
  int32 tx_pos_ref;
} tlm_rx_tx_pos_rach_debug_struct_type;

/*--------------------------------------------------------------------------*/

typedef uint16 tlm_snapshot_needed_mask_type ;

typedef enum {
TLM_SNAPSHOT_ENUM_WRC,
TLM_SNAPSHOT_ENUM_RXTL_TFN,
TLM_SNAPSHOT_ENUM_FRC,
TLM_SNAPSHOT_ENUM_TST,
TLM_SNAPSHOT_ENUM_RQSTD_COMB_CFN,
TLM_SNAPSHOT_ENUM_RXTL_SRC_COMB_CFN,
TLM_SNAPSHOT_ENUM_NUM_TYPES
} tlm_snapshot_needed_enum_type ;

/** wakeup/sleep input structure type */
typedef struct
{
  /** cfn to wakeup */
  uint8 occasion_cfn;
  union
  {
    /** wakeup occasion in cx1 */
  	struct
  	{
      uint16 cx1_count;
  	}in_cx1;
    struct
    {
      /** wakeup occassion tick count */
      uint8 chip_cnt_sub_tick;
      uint8 tick_cnt;
    }in_ticks;
  }sub_frame_time;
} tlm_cfn_info_in_cfn_tick;

/*------------------GLOBAl VARIABLES DEFINITION END------------------------------------*/

/*------------------EXTERNAL FUNCTIONS DECLARATION BEGIN------------------------------------*/
#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================
FUNCTION              TLM_GET_RX_TL_CFN_FROM_HW

FILE NAME            tlm.c

DESCRIPTION          The function populated the CFN value in the pointer passed as part of argument.

INPUT PARAMS         A pointer to the TLM CFN struct. This data is used by the called function. 

OUTPUT PARAMS        None

EXECUTION CONTEXT  EUL ISR

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/

extern void tlm_get_rx_tl_cfn_from_hw(tlm_cfn_struct_type *rxtl_cfn);
#endif /* FEATURE_WCDMA_HS_RACH */

extern tlm_db_struct_type tlm_db;

extern tlm_cfn_struct_type comb_cell_pos_diff_qty;

/*************************TLM-L1M INTERFACE FUNCTIONS*************************/

/*===========================================================================
FUNCTION             TLM_INIT

FILE NAME            tlm.c

DESCRIPTION          This function resets the tlm status and state variables.
                     This function must be called everytime the UE enters WCDMA
                     mode.

INPUT PARAMS         None

OUTPUT PARAMS        None

EXECUTION CONTEXT    Task-Yes; Interrupts-No

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_init( void );

/*===========================================================================
FUNCTION             tlm_dch_init

FILE NAME            tlm.c

DESCRIPTION          

INPUT PARAMS         None

OUTPUT PARAMS        None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_dch_init (void);

/*************************TLM-ALIGNMENT MANAGER INTERFACE FUNCTIONS*************************/
/*===========================================================================
FUNCTION             TLM_SET_ALIGN

FILE NAME            tlm.c

DESCRIPTION          This function aligns the L1 timeline to the counter
                     given by the parameter. It does the following things: 
                     Compute and update the SFN/CFN for the new timeline
                     and Compute the new wrc_tfn_diff.

INPUT PARAMS
                     cntr: This parameter mentions the System Timer Counter
                     to which L1 timeline should be aligned to.
                     cell_info: This paramter is used only if cntr is not FRC.
                     When used, this paramter mentions the cell to which the
                     combiner(cntr) belongs.
                     ch_off: This paramter is used only if cntr is not FRC.
                     When used, this parameter mentions the offset that
                     the combiner (cntr) has w.r.t the pccpch of the same
                     cell.

OUTPUT PARAMS        None

EXECUTION CONTEXT    Task-yes; Interrupt-No

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
/* These enums define whether a STMR interrupt source is aligned to
 * a physical channel source or the frame reference counter.
 */
#ifndef SEQ_H
typedef enum
{
  SEQ_FRAME_REF_ALIGN_TYPE,
  SEQ_PCCPCH_ALIGN_TYPE,
  SEQ_PICH_ALIGN_TYPE,
  SEQ_AICH_ALIGN_TYPE,
  SEQ_DPCH_ALIGN_TYPE,
  SEQ_NUM_ALIGN_TYPE
} seq_align_type;/*SEQ_TLM_PROBLEM*/
#define SEQ_TYPE_DEFINED_IN_TLM
#endif

extern void tlm_set_align(seq_align_type align_type, stmr_counter_source_enum_type cntr,
    tlm_cell_info_struct_type *cell_info, tlm_tfn_struct_type *ch_off);
/*extern void tlm_set_align(stmr_counter_source_enum_type cntr,
    tlm_cell_info_struct_type *cell_info, tlm_tfn_struct_type *ch_off);*/

/*===========================================================================
FUNCTION             TLM_SET_ALIGN_FROM_STORED_INFO

FILE NAME            tlm.c

DESCRIPTION          This function aligns the L1 timeline to the counter
                     given by the parameter. It does the following things: 
                     Compute and update the SFN/CFN for the new timeline
                     and Compute the new wrc_tfn_diff.

INPUT PARAMS
                     cntr: This parameter mentions the System Timer Counter
                     to which L1 timeline should be aligned to.
                     cfn_wrc: This paramter is used to calculate the CFN.
                     This is stored while performing the DPCH drop on the old 
                     cell while doing a HHO
                     tfn_wrc: This paramter is used to calculate the SFN.

OUTPUT PARAMS        None

EXECUTION CONTEXT    Task-yes; Interrupt-No

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_set_align_from_stored_info (seq_align_type align_type,  stmr_counter_source_enum_type cntr,
    tlm_cfn_struct_type *cfn_wrc, tlm_tfn_struct_type *tfn_wrc);

/*************************TLM-MCAL_DL INTERFACE FUNCTIONS*************************/

/*===========================================================================
FUNCTION             tlm_get_wrc_frame_qty

FILE NAME            tlm.c

DESCRIPTION            this function computes WRC frame qty without stmr status dump, 
                                so value is not gaurenteed to be acurate, can be used when accuracy is not of concern

INPUT PARAMS
                     None

OUTPUT PARAMS
                     None

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
uint16 tlm_get_wrc_frame_qty(void);

/*===========================================================================
FUNCTION             TLM_GET_CFN_FOR_CHANNEL_CONFIG

FILE NAME            tlm.c

DESCRIPTION          This function computes CFN of any channel of any cell and
                     the cfn_wrc difference.Typical users of this service
                     would store the wrc_cfn_diff.and call the
                     tlm_get_ch_cfn_from_wrc_diff() subsequent times to
                     compute the cfn of the channel. Calling
                     tlm_get_ch_cfn_from_wrc_diff() in the subsequent calls
                     would save TLM from calculating and updating sfn_wrc_diff
                     in searcher.

INPUT PARAMS
                     cell_info: the structure that identifies the cell
                     ch_off: the offset of the channel w.r.t the pccpch of
                     the cell.

OUTPUT PARAMS
                     ch_cfn: the structure that holds the computed cfn of the
                     channel
                     wrc_cfn_diff: the structure that holds the computed
                     wrc_cfn_diff

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         The function should not be called just before the frame
                     boundary of the channel. This is because by the time the
                     user uses the ch_cfn returned by TLM, the frame_qty of
                     ch_cfn could be stale by -1.

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_cfn_for_channel_config(
  tlm_cell_info_struct_type *cell_info, tlm_tfn_struct_type *ch_off,
  tlm_ch_cfn_info_struct_type *cfn_info);

/*===========================================================================
FUNCTION             TLM_GET_CH_CFN_INFO_FROM_COMB

FILE NAME            tlm.c

DESCRIPTION          This function computes CFN of any channel based on
                     the combiner number passed as input param.

INPUT PARAMS
                     rqstd_comb: the combiner based on which ch_cfn will be
                     calculated

OUTPUT PARAMS
                     ch_cfn: the structure that holds the computed cfn of the
                     channel

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         The function should not be called just before the frame
                     boundary of the channel. This is because by the time the
                     user uses the ch_cfn returned by TLM, the frame_qty of
                     ch_cfn could be stale by -1.

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
void tlm_get_ch_cfn_info_from_comb(
  stmr_counter_source_enum_type rqstd_comb,
  tlm_ch_cfn_info_struct_type *cfn_info);

/*===========================================================================
FUNCTION             TLM_GET_CH_CFN_FROM_WRC_DIFF

FILE NAME            tlm.c

DESCRIPTION          This function computes CFN of any channel of any cell from
                     the cfn_wrc difference that was computed beforehand.
                     Typical users of this service would have called
                     tlm_get_ch_cfn_info() before calling this function and
                     stored the wrc_cfn_diff.

INPUT PARAMS
                     cell_info: the structure that identifies the cell
                     ch_off: the offset of the channel w.r.t the pccpch of
                     the cell.

OUTPUT PARAMS
                     ch_cfn: the structure that holds the computed cfn of the
                     channel
                     wrc_cfn_diff: the structure that holds the computed
                     wrc_cfn_diff

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         The function should not be called just before the frame
                     boundary of the channel. This is because by the time the
                     user uses the ch_cfn returned by TLM, the frame_qty of
                     ch_cfn could be stale by -1.

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_ch_cfn_from_wrc_diff( tlm_cell_info_struct_type *cell_info, tlm_tfn_struct_type *ch_off,
  tlm_cfn_struct_type *wrc_cfn_diff, tlm_ch_cfn_info_struct_type *ch_cfn);

/*************************TLM-BCH_MANAGER INTERFACE FUNCTIONS*************************/
/*===========================================================================
FUNCTION             TLM_COMPUTE_SFN_WRC_DIFF

FILE NAME            tlm.c

DESCRIPTION          This function computes the difference between the 
                     SFN of a cell on which the PCCPCH has been decoded
                     successfully just now and the WRC.

INPUT PARAMS
                     pccpch_data: the data regarding the PCCPCH channel
                     on which the decoding passed.

OUTPUT PARAMS
                     sfn_wrc: the sfn_wrc diff between the SFN of cell and 
                     WRC.

EXECUTION CONTEXT    Task-Yes; Interrupt-No

DEPENDENCIES         The combiner on which the PCCPCH was setup should be alive.

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_compute_sfn_wrc_diff(tlm_compute_sfn_wrc_input_struct_type *pccpch_data,
  tlm_tfn_struct_type *sfn_wrc);

/*************************TLM-OTHERS INTERFACE FUNCTIONS*************************/
/*===========================================================================
FUNCTION             TLM_GET_CH_SFN

FILE NAME            tlm.c

DESCRIPTION          This function computes SFN of any channel of any cell,
                     provided some L1 module has decoded the PCCPCH timing
                     of that cell before.

INPUT PARAMS
                     cell_info: the structure that identifies the cell
                     ch_off: the offset of the channel w.r.t the pccpch of
                     the cell.

OUTPUT PARAMS
                     ch_sfn: the structure that holds the computed sfn of the
                     channel

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         The function should not be called just before the frame
                     boundary of the channel. This is because by the time the
                     user uses the ch_sfn returned by TLM, the frame_qty of
                     ch_sfn could be stale by -1.

RETURN VALUE         boolean status - TRUE on success. FALSE on failure.

SIDE EFFECTS         None
===========================================================================*/
extern boolean tlm_get_ch_sfn(
  tlm_cell_info_struct_type *cell_info, tlm_tfn_struct_type *ch_off,
  tlm_tfn_struct_type *ch_sfn);


/*===========================================================================
FUNCTION             TLM_GET_SFN_AND_WRC

FILE NAME            tlm.c

DESCRIPTION          This function computes SFN and WRC of any channel of any cell,
                     provided some L1 module has decoded the PCCPCH timing
                     of that cell before.

INPUT PARAMS
                     cell_info: the structure that identifies the cell
                     ch_off: the offset of the channel w.r.t the pccpch of
                     the cell.

OUTPUT PARAMS
                     ch_sfn: the structure that holds the computed sfn of the
                     channel
                     ch_wrc: The structure holds the computed wrc  

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         The function should not be called just before the frame
                     boundary of the channel. This is because by the time the
                     user uses the ch_sfn returned by TLM, the frame_qty of
                     ch_sfn could be stale by -1.

RETURN VALUE         boolean status - TRUE on success. FALSE on failure.

SIDE EFFECTS         None
===========================================================================*/

boolean tlm_get_sfn_and_wrc(
   tlm_cell_info_struct_type *cell_info, tlm_tfn_struct_type *ch_off, 
   tlm_tfn_struct_type *ch_sfn, tlm_tfn_struct_type *wrc);
/*===========================================================================
FUNCTION             TLM_GET_REF_CNT_ADJ

FILE NAME            tlm.c

DESCRIPTION          This function reads the value that needs to be used in 
                     adjusting the Frame Reference Counter.

INPUT PARAMS         None

OUTPUT PARAMS        None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         This function returns the ref_cnt_sdj as a uint32 value.

SIDE EFFECTS         None
===========================================================================*/
extern uint32 tlm_get_ref_cnt_adj (void);

/*===========================================================================
FUNCTION             tlm_get_sfn_for_frc_alignment

FILE NAME            tlm.c

DESCRIPTION          

INPUT PARAMS         None

OUTPUT PARAMS        None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         

SIDE EFFECTS         None
===========================================================================*/
extern uint16 tlm_get_sfn_for_frc_alignment (void);

/*===========================================================================
FUNCTION             TLM_COMP_SFN_SFN_DIFF

FILE NAME            tlm.c

DESCRIPTION          

INPUT PARAMS         None

OUTPUT PARAMS        None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_comp_sfn_sfn_diff (tlm_cell_info_struct_type *nbr_cell_info, tlm_nw_diff_struct_type *sfn_sfn_diff);

/*===========================================================================
FUNCTION             TLM_COMP_SFN_CFN_DIFF

FILE NAME            tlm.c

DESCRIPTION          

INPUT PARAMS         None

OUTPUT PARAMS        None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_comp_sfn_cfn_diff (tlm_cell_info_struct_type *nbr_cell_info, tlm_nw_diff_struct_type *sfn_cfn_diff);

/*===========================================================================
FUNCTION             TLM_GET_SFN_OF_CELLS

FILE NAME            tlm.c

DESCRIPTION          The input to the function is an array of cells for
                     which the SFN has to be computed. The function computes
                     each cell's SFN and places them in the array
                     cell_sfn_list. The output is placed in the same order as
                     the cell_info_list order.


INPUT PARAMS
                     cell_info_list[]: The array which holds the list of cells
                     for which SFN has to be computed.
                     num_of_cells: This input parameter mentiosn the size of
                     the input array cell_info_list[].

OUTPUT PARAMS
                     cell_sfn_list[]: The array that carries the sfn of the
                     input cells.The output is placed in the same order as
                     the cell_info_list order.

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         The caller of this function must have defined the array
                     cell_sfn_list with a size of atleast num_of_cells.

RETURN VALUE
                     TRUE: No change of align in progress; SFN computation
                     was attempted
                     FALSE: Change of align is in progress; SFN computation
                     was not attempted

SIDE EFFECTS         None
===========================================================================*/
extern boolean tlm_get_sfn_of_cells(tlm_cell_info_struct_type *cell_info_list, uint8 num_of_cells, tlm_tfn_struct_type *cell_sfn_list);

/*===========================================================================
FUNCTION             TLM_DBG_CH_SETUP

FILE NAME            tlm.c

DESCRIPTION          

INPUT PARAMS         None

OUTPUT PARAMS        None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_dbg_tst( void );

/*===========================================================================
FUNCTION             tlm_adjust_wrc_tfn_frm_qty

FILE NAME            tlm.c

DESCRIPTION          

INPUT PARAMS         current sfn and new sfn

OUTPUT PARAMS        None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_adjust_wrc_tfn_frm_qty(uint16 cur_sfn, uint16 new_sfn);

/*===========================================================================
FUNCTION             tlm_update_tfn_wrc_before_timeline_interuption

FILE NAME            tlm.c

DESCRIPTION            NONE

INPUT PARAMS

OUTPUT PARAMS

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_update_tfn_wrc_before_timeline_interuption(void);

/*===========================================================================
FUNCTION             tlm_update_wrc_tfn_diff_after_timeline_interuption

FILE NAME            tlm.c

DESCRIPTION            NONE

INPUT PARAMS

OUTPUT PARAMS

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_update_wrc_tfn_diff_after_timeline_interuption(void);

/*===========================================================================

FUNCTION TLM_GET_TST_SNAPSHOT

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/
extern void tlm_get_snapshot(tlm_snapshot_struct_type *out,
  tlm_snapshot_needed_mask_type mask, stmr_counter_source_enum_type rqstd_comb);


/*===========================================================================
FUNCTION             TLM_GET_RX_TL_CFN

FILE NAME            tlm.c

DESCRIPTION            This function returns the RX timeline CFN in (cfn, cx8) units.

INPUT PARAMS
                     NONE

OUTPUT PARAMS
                     rxtl_cfn: the structure that holds the computed cfn of the
                     rx timeline

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_rx_tl_cfn(tlm_cfn_struct_type *rxtl_cfn);

/*===========================================================================
FUNCTION             tlm_update_align_cell_sfn_wrc_info

FILE NAME            tlm.c

DESCRIPTION            this function updates align cell sfn wrc diff value to tlm data base for easy access. 

DEPENDENCIES         Function should get called when ever align cell info sfn_wrc gets changed

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_update_align_cell_sfn_wrc_info(uint16 psc, uint16 freq, boolean sfn_wrc_valid, tlm_tfn_struct_type sfn_wrc_diff);

/*===========================================================================
FUNCTION             tlm_update_align_cell_info

FILE NAME            tlm.c

DESCRIPTION            NONE

INPUT PARAMS

OUTPUT PARAMS

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_update_align_cell_info(tlm_cell_info_struct_type *cell_info, tlm_tfn_struct_type *ch_off);

/*===========================================================================
FUNCTION             tlm_update_align_cell_info_for_aset

FILE NAME            tlm.c

DESCRIPTION            NONE

INPUT PARAMS

OUTPUT PARAMS

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
void tlm_update_align_cell_info_for_aset(tlm_cell_info_struct_type *cell_info, tlm_tfn_struct_type *ch_off);

/*===========================================================================
FUNCTION             TLM_INDICATE_COMBINER_SLAM_DONE

FILE NAME            tlm.c

DESCRIPTION          This function updates the align counter slam pending.

INPUT PARAMS
                     combiner

OUTPUT PARAMS
                     None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_indicate_combiner_slam_done(stmr_counter_source_enum_type hw_ch);

/*===========================================================================
FUNCTION             TLM_SET_ALIGN_CNTR_HW_CFN_DIFF_UPDATE_NEEDED

FILE NAME            tlm.c

DESCRIPTION            This function updates the align counter cfn usage.

INPUT PARAMS
                     combiner and whether update is needed

OUTPUT PARAMS
                     None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void  tlm_set_align_cntr_hw_sw_cfn_diff_upd_needed(boolean hw_sw_cfn_diff_upd_needed, stmr_counter_source_enum_type hw_ch);

/*===========================================================================
FUNCTION             TLM_CALCULATE_HW_SW_CFN_DIFF

FILE NAME            tlm.c

DESCRIPTION            This function returns the RX timeline CFN in (cfn, cx8) units.

INPUT PARAMS
                     NONE

OUTPUT PARAMS
                     rxtl_cfn: the structure that holds the computed cfn of the
                     rx timeline

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_calculate_hw_sw_cfn_diff(boolean force_cmd, boolean status_dump_needed);

/*===========================================================================
FUNCTION             TLM_GET_ALIGN_CNTR

FILE NAME            tlm.c

DESCRIPTION   

INPUT PARAMS
                     NONE

OUTPUT PARAMS

EXECUTION CONTEXT   

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
stmr_counter_source_enum_type tlm_get_align_cntr(void);

#ifdef FEATURE_WCDMA_CNT_C_SFN_DIFF

/*===========================================================================
FUNCTION             TLM_COMPUTE_SFN_WRC_DIFF

FILE NAME            tlm.c

DESCRIPTION          This function computes the difference between the 
                     SFN of a cell on which the PCCPCH has been decoded
                     successfully just now and the WRC.

INPUT PARAMS
                     pccpch_data: the data regarding the PCCPCH channel
                     on which the decoding passed.

OUTPUT PARAMS
                     sfn_wrc: the sfn_wrc diff between the SFN of cell and 
                     WRC.

EXECUTION CONTEXT    Task-Yes; Interrupt-No

DEPENDENCIES         The combiner on which the PCCPCH was setup should be alive.

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern uint16 tlm_comp_cnt_c_sfn_diff(tlm_cell_info_struct_type *cell_info);

/*------------------EXTERNAL FUNCTIONS DECLARATION END------------------------------------*/
#endif

/*===========================================================================
FUNCTION             tlm_submit_debug_message

FILE NAME            tlm.c

DESCRIPTION            this function snapshots tlm vars and prints debug message of tlm db

INPUT PARAMS
                     Cell info (optional)

OUTPUT PARAMS
                     None

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
void tlm_submit_debug_message( tlm_cell_info_struct_type *cell_info);

/*===========================================================================
FUNCTION             TLM_CALCULATE_TM

FILE NAME            tlm.c

DESCRIPTION            This function returns the Tm wrt to the cell whose ref pos is passed in Cx1 units.

INPUT PARAMS
                     cell_ref_pos: Reference position of the cell

OUTPUT PARAMS
                     Tm: Tm in Cx1

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern uint32 tlm_calculate_tm(uint32 cell_ref_pos);

/*===========================================================================
FUNCTION             TLM_CALCULATE_RX_TX_DIFF

FILE NAME            tlm.c

DESCRIPTION          This function calculates the RX_TX time difference and
                     updates in the cell structure

INPUT PARAMS
                     cell_info: Pointer to the cell for which Rx_Tx diff needs to be calculated
                     t_dpch: TTO of the DPCH

OUTPUT PARAMS
                     out: Pointer to the structure which hold the Rx time and Rx_Tx Diff

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_calculate_rx_tx_diff(tlm_cell_info_struct_type *cell_info, uint16 t_dpch, tlm_rx_tx_diff_struct_type *out);

/*===========================================================================
FUNCTION             TLM_GET_EXTENDED_SFN

FILE NAME            tlm.c

DESCRIPTION          This function returns the Extended SFN 

INPUT PARAMS         None

OUTPUT PARAMS
                     out_ext_sfn: the structure that holds the computed Extended SFN

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         boolean status - TRUE on success. FALSE on failure.

SIDE EFFECTS         None
===========================================================================*/
extern boolean tlm_get_extended_sfn(tlm_extended_sfn_struct_type *out_ext_sfn);

/*===========================================================================
FUNCTION             TLM_CALCULATE_TST_WRC_DIFF

FILE NAME            tlm.c

DESCRIPTION          This function will be called every triage in DCH state;
                     This will calculate the difference between tx time and WRC by
                     picking these values from the demod status dump result buffer

INPUT PARAMS         None

OUTPUT PARAMS
                     Stores the output in a global variable 

EXECUTION CONTEXT    Task-yes; Interrupt-No

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_calculate_tst_wrc_diff(int32 num_status_dump,  int32 ref_tx_pos);

/*===========================================================================
FUNCTION             TLM_INVALIDATE_TST_WRC_DIFF

FILE NAME            tlm.c

DESCRIPTION          This function resets the global variablw which holds the TST-WRC diff

INPUT PARAMS         None

OUTPUT PARAMS         None

EXECUTION CONTEXT    Task-yes; Interrupt-No

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_invalidate_tst_wrc_diff(void);

/*===========================================================================
FUNCTION        tlm_get_align_cell_ch_timing

FILE NAME       tlm.c

DESCRIPTION     This function is used to return the aligned channel timing w.r.t frc in cx8

DEPENDENCIES    None

RETURN VALUE   Aligned channel timing in cx8

SIDE EFFECTS    None
===========================================================================*/

uint32 tlm_get_align_cell_ch_timing(void);

/*===========================================================================
FUNCTION        tlm_get_align_cell_sfn

FILE NAME       tlm.c

DESCRIPTION     This function returns aligned cell SFN

DEPENDENCIES    None

RETURN VALUE   aligned cell SFN frame

SIDE EFFECTS    None
===========================================================================*/
uint16 tlm_get_align_cell_sfn(void);

/*===========================================================================
FUNCTION        tlm_get_align_cell_sfn_timing

FILE NAME       tlm.c

DESCRIPTION     This function return aligned cell current timing in frame and cx8

DEPENDENCIES    None

RETURN VALUE   SFN frame, cx8 

SIDE EFFECTS    None
===========================================================================*/
void tlm_get_align_cell_sfn_timing(tlm_tfn_struct_type* sfn);

/*===========================================================================
FUNCTION        tlm_get_curr_align_cntr_val_and_frc_val

FILE NAME       tlm.c

DESCRIPTION     This function return combiner and frc timing in frame and cx8

DEPENDENCIES    None

RETURN VALUE   SFN frame, cx8 

SIDE EFFECTS    None
===========================================================================*/
extern void tlm_get_curr_align_cntr_val_and_frc_val(
  tlm_cfn_struct_type *curr_cfn_val, 
  tlm_tfn_struct_type *frc_val);

/*===========================================================================
FUNCTION             TLM_CALCULATE_RX_TX_POS_FOR_RACH_DEBUG

FILE NAME            tlm.c

DESCRIPTION          This function calculates the AICH and RACH positions wrt FRC
                               for RACH debugging. 
                   
DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_calculate_rx_tx_pos_for_rach_debug(tlm_rx_tx_pos_rach_debug_struct_type *rx_tx_pos);

/*===========================================================================
FUNCTION             TLM_GET_RXTL_SFN_AND_WRC

FILE NAME            tlm.c

DESCRIPTION          This functions returns the SFN of the aligned cell along with WRC

INPUT PARAMS
                     None

OUTPUT PARAMS
                     rxtl_sfn: SFN of the aligned cell
                     wrc: FRC in full range

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_rxtl_sfn_and_wrc
(
  tlm_tfn_struct_type *rxtl_sfn,
  tlm_tfn_struct_type *wrc
);

/*===========================================================================
FUNCTION             TLM_GET_CH_CFN

FILE NAME            tlm.c

DESCRIPTION          This function computes CFN of any channel of any cell for
                     all cases unless the CFN is used for channel configuration

INPUT PARAMS
                     cell_info: the structure that identifies the cell
                     ch_off: the offset of the channel w.r.t the pccpch of
                     the cell.

OUTPUT PARAMS
                     ch_cfn: the structure that holds the computed cfn of the
                     channel

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_ch_cfn(
  tlm_cell_info_struct_type *cell_info, tlm_tfn_struct_type *ch_off,
  tlm_ch_cfn_info_struct_type *cfn_info);

/*===========================================================================
FUNCTION             TLM_GET_WRC_DYNAMIC

FILE NAME            tlm.c

DESCRIPTION          This function just returns the dynamic WRC value

INPUT PARAMS         None

OUTPUT PARAMS
                     WRC: the structure that holds the WRC in TFN range

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_wrc_dynamic(tlm_tfn_struct_type *wrc);

/*===========================================================================
FUNCTION             TLM_UPDATE_CELL_TIMING

FILE NAME            tlm.c

DESCRIPTION          This function is used to update the SFN_WRC of the cell
                     without any bound restriction. This should only be used
                     in cases when the cell reference position changes.

INPUT PARAMS
                     cell_info: the cell for which the sfn_wrc needs to be adjusted

OUTPUT PARAMS         None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_update_cell_timing(tlm_cell_info_struct_type *cell_info);

/*===========================================================================
FUNCTION             TLM_GET_TX_SYSTEM_TIME

FILE NAME            tlm.c

DESCRIPTION          This function returns the Tx System Time

INPUT PARAMS                    

OUTPUT PARAMS        None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_tx_sys_time(tlm_tfn_struct_type *tst);

/*===========================================================================
FUNCTION             TLM_GET_CH_CFN_FROM_CFN_WRC_DIFF

FILE NAME            tlm.c

DESCRIPTION          This function computes current CFN from CFN_WRC diff.
                     This is STRICTLY not for downlink channel configurations

INPUT PARAMS         None

OUTPUT PARAMS
                     ch_cfn: the structure that holds the computed cfn of the
                     channel

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         The function should not be called just before the frame
                     boundary of the channel. This is because by the time the
                     user uses the ch_cfn returned by TLM, the frame_qty of
                     ch_cfn could be stale by -1.

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_cfn_from_cfn_wrc_diff(tlm_cfn_struct_type *ch_cfn);

/*===========================================================================
FUNCTION             TLM_SET_CFN_WRC_DIFF

FILE NAME            tlm.c

DESCRIPTION          This function updates the CFN_WRC difference
                     using the WRC_CFN difference calculated during channel config

INPUT PARAMS         WRC-CFN Difference

OUTPUT PARAMS        None

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         The function should be called only after an align change

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_set_cfn_wrc_diff(tlm_tfn_struct_type *wrc_cfn_diff);

/*===========================================================================
FUNCTION             TLM_GET_ALIGNED_CELL_PSC_AND_CH_OFF

FILE NAME            tlm.c

DESCRIPTION          This function returns the PSC and chip offset of the
                     aligned cell

INPUT PARAMS         None

OUTPUT PARAMS        PSC of the aligned cell,
                     Align chip offset of align cell

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         The function should be called only after an align change

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_aligned_cell_psc_and_ch_off(uint32 *psc, int32 *chip_offset);

/*===========================================================================
FUNCTION             TLM_GET_WRC_DIFF_INFO

FILE NAME            tlm.c

DESCRIPTION          This function computes difference between the 
                     1. Combiner CFN and WRC
                     2. TFN and WRC

INPUT PARAMS
                     comb: the combiner based on which CFN_WRC_DIFF is calculated

OUTPUT PARAMS
                     cfn_wrc_diff: the structure that holds the computed cfn_wrc_diff
                     tfn_wrc_diff: The structure that hold the computed tfn_wrc_diff

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_wrc_diff_info(stmr_counter_source_enum_type comb,
  tlm_cfn_struct_type *cfn_wrc_diff,
  tlm_tfn_struct_type *tfn_wrc_diff);

/*===========================================================================
FUNCTION             TLM_CALCULATE_TX_POS

FILE NAME            tlm.c

DESCRIPTION            This function calculates the Tx position wrt FRC from the STMR status dump.

INPUT PARAMS         None

OUTPUT PARAMS
                     Tx_time: Tx Position in Cx8

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern int32 tlm_calculate_tx_pos(void);

/*===========================================================================
FUNCTION             TLM_ADVANCE_RETARD_CFN_WRC
FILE NAME            tlm.c

DESCRIPTION          This function advances/retards the cfn_wrc_diff in tlm_db
                     advance=TRUE: Advance cfn_wrc_diff by 1cx8
                     advance=FALSE: Retard cfn_wrc_diff by 1cx8

INPUT PARAMS         
                     advance=TRUE: Advance cfn_wrc_diff by 1cx8
                     advance=FALSE: Retard cfn_wrc_diff by 1cx8

OUTPUT PARAMS        None

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_advance_retard_cfn_wrc(boolean advance);


/*===========================================================================
FUNCTION             TLM_GET_ALIGN_CHAN_POS
FILE NAME            tlm.c

DESCRIPTION          This function returns the DPCH pos from the combiner
                                      
INPUT PARAMS         None                   

OUTPUT PARAMS        DPCH pos

EXECUTION CONTEXT    Task-yes; Interrupt - yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern int32 tlm_get_align_chan_pos(void);

/*===========================================================================
FUNCTION             TLM_GET_CH_STMR_ACTION_TIME
FILE NAME            tlm.c

DESCRIPTION          This function returns the channel STMR action time in WRC

INPUT PARAMS         None

OUTPUT PARAMS        uint32 action time, bit 19:8 frame num bit7:0 bpg

EXECUTION CONTEXT    Task-yes; Interrupt - yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern uint32 tlm_get_ch_stmr_action_time(tlm_tfn_struct_type *wrc_cfn_diff, tlm_tfn_struct_type *new_action_cfn);

/*===========================================================================
FUNCTION             TLM_GET_SUBFN_INFO_FROM_WRC
FILE NAME            tlm.c

DESCRIPTION          This function converts the raw WRC to/returns the subfn and bpg values

INPUT PARAMS         uint32 wrc, tlm_subfn_struct_type *subfn_info

OUTPUT PARAMS        None

EXECUTION CONTEXT    Task-yes; Interrupt - yes

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_subfn_bpg_info_from_wrc(uint32 wrc, tlm_subfn_struct_type *subfn_info);

#endif /* TLM_H */
