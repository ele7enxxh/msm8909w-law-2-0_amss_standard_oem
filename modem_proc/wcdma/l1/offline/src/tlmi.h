/*===========================================================================
                               T L M I. H

GENERAL DESCRIPTION

  This file contains the declarations of functions and data structure
  definitions that are internal to the Time Line Manager (TLM). This file is
  internal to TLM and no other module should include it.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright(c)2007 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/tlm.c_v   1.153   17 Jul 2002 16:36:16   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/tlmi.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
10/24/13    kcm     Corrected the TLM_STRIP_CX1_FROM_FRC_DYNAMIC macro
10/7/13     kcm     Corrected the TLM_STRIP_FRAMES_FROM_FRC_DYNAMIC Macro
09/14/13    pr      VSTMR Changes for BOLT
11/14/13    pj      Wrapped HWIO access with WL1_HWIO macro
05/09/13    pr      Featurized code under FEATURE_WCDMA_BOLT_RUMI_HWIO_DEMOD as part of Bolt Bringup.
03/09/12    amj     Moved some arithmetic CFN/SFN macros to tlm.h
02/15/12    hk      Fixed compilation warning
02/15/12    hk      Timeline alignment changes for HHO & W2G Failure recovery
01/11/12    hk      Mechanism to calculate CFN from CFN-WRC diff
07/07/11    hk      Up-merge from offline/main/latest
03/29/11    hk      Removed the inclusion of msmhwioreg.h from tlm.h
10/08/10    vb      Added support for TLM thin API to provide aligned cell SFN 
08/18/10    ks      Corrected comparison range of frame/cx8 quantity in TFN
                    validity check macro
08/17/10    hk      Changed the macro name to avoid quality alert
07/21/10    hk      Workaround to calculate the Transmit System Time
03/31/10    hk      Corrected the calculation of SFN_CFN difference
11/11/09    hk      Corrected the calculation of SFN_CFN difference
07/02/09    hk      Changed the implementation to calculate Rx_Tx difference
                    from STMR status dump
05/25/09    hk      Fixed compiler warnings
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED and removing RUMI_BRING_UP
10/16/08    hk      Changes to make use the hardware WRC
10/08/08    hk      Limited the range of TST TFN to 0-7
10/03/08    hk      Bringing in 7k Mailine fixes
01/25/08    vsr     Checking in Enhanced DL files onto mainline
08/08/07   gv/vp    Added support for MCAL_EDL and consolidated APIs in dlphch
                    to get rl_offset.
07/05/07    vsr     Moved TLM GET CFN macro to tlm.h
05/10/07    vsr     Lint fix
02/21/07    vp      Correction in range check before normalizing.
                    
                    ===========================================================================*/
#ifndef TLM_I_H
#define TLM_I_H
/*--------------------------------------------------------------------------
                      Included Header Files
--------------------------------------------------------------------------*/
#include "wcdma_variation.h"
#ifndef TLM_UNIT_TEST
#include "customer.h"
#include "comdef.h"
#include "msmhwioreg.h"
#include "l1const.h"
#endif

#include "l1msm.h"

/*------------------GLOBAl VARIABLES DEFINITION BEGIN------------------------------------*/

/*This value is MDSP_WCDMA_DESKEW_DEPTH_IN_CHIPS*8 */
#define MDSP_WCDMA_DESKEW_DEPTH_IN_CX8 (MDSP_WCDMA_DESKEW_DEPTH_IN_CHIPS * 8)

/* This Macro returns the current alignment type
*/
#define TLM_GET_ALIGN_TYPE (seq_get_current_align_type())

#define TLM_IS_FRC_ALIGNED()     \
  (TLM_GET_ALIGN_TYPE == SEQ_FRAME_REF_ALIGN_TYPE)

#define TLM_IS_NON_FRC_ALIGNED() (!TLM_IS_FRC_ALIGNED())

/*This is half a wcdma frame in cx8
*/
#define TLM_MIN_TM_DIFF_TO_ADJ_OFF (153600L)

#define TLM_UL_NOM_DL_ACTUAL_OFFSET_IN_CX8    \
  ((UL_DL_DPCH_CHIP_OFFSET - SRCH_RF_TX_DELAY) * CHIPX8_PER_CHIP)

/*This is the limit for re-reading for cfn from hw. Value is 75 ticks in cx8;
 * This value should be used when there is no intlock involved between 
 * cfn read and cx8 read
 */
#define TLM_COMB_CX8_MAX_FOR_RE_READ_LONG (153600L)

/*This macro returns the event IDs of the necessary SFN and CFN update event
 */
#define TLM_MASK_FOR_SFN_CFN_EVENTS        \
       (MCALWCDMA_EVT_EVENT_ID_TO_MASK(SEQ_NON_DCH_SFN_CFN_UPDATE_EVENT))

#define TLM_MASK_FOR_SFN_CFN_CM_EVENTS        \
       (MCALWCDMA_EVT_EVENT_ID_TO_MASK(SEQ_NON_DCH_SFN_CFN_UPDATE_EVENT) | MCALWCDMA_EVT_EVENT_ID_TO_MASK(SEQ_CM_UPDATE_EVENT))

#define TLM_SET_ANY_ALIGN_HAPPENED()        \
  do {        \
       tlm_db.any_align_happened = TRUE;        \
  } while (0)

#define TLM_RESET_ANY_ALIGN_HAPPENED()        \
  do {        \
       tlm_db.any_align_happened = FALSE;        \
  } while (0)

  #define TLM_HAS_ANY_ALIGN_HAPPENED()        \
    (tlm_db.any_align_happened == TRUE)

#define TLM_INIT_TFN_TO_ZERO(param)         \
  do {                                                                              \
    (param).frame_qty = 0;                                                  \
    (param).chip_offset_cx8_qty = 0;                                    \
  }while(0)

#define TLM_INIT_FRC_TO_ZERO(param)         \
  do {                                                                              \
    (param).frame_qty = 0;                                                  \
    (param).chip_offset_cx8_qty = 0;                                    \
  }while(0)

#define TLM_PREP_TLM_TFN(tlm_tfn, tfn_frames, tfn_chips)         \
  do {         \
    (tlm_tfn).frame_qty = tfn_frames;         \
    (tlm_tfn).chip_offset_cx8_qty = tfn_chips;         \
  }while(0)

#define TLM_PREP_TLM_CFN(tlm_cfn, cfn_frames, cfn_chips)         \
  do {         \
    (tlm_cfn).frame_qty = cfn_frames;         \
    (tlm_cfn).chip_offset_cx8_qty = cfn_chips;         \
  }while(0)

#ifdef FEATURE_WCDMA_TLM_LOG_ALL_STEPS
#error code not present
#else /*FEATURE_WCDMA_TLM_LOG_ALL_STEPS*/

#define TLM_LOG_SUB_CHIP_OPERATION(result, minuend, subtrahend)         \
  do {         \
  MSG_LOW("sub_chip_oper: res:%d min:%d sub:%d", result, minuend, subtrahend);         \
  } while (0)

#define TLM_LOG_ADD_CHIP_OPERATION(result, minuend, subtrahend)         \
  do {         \
  MSG_LOW("add_chip_oper: res:%d min:%d sub:%d", result, minuend, subtrahend);         \
  } while (0)

#define TLM_LOG_TLM_WRC(tlm_wrc)         \
  do {         \
    MSG_LOW("tlm_wrc: (f,cx8):%d, %d", (tlm_wrc).frame_qty, (tlm_wrc).chip_offset_cx8_qty, 0);         \
  }while(0)

#define TLM_LOG_TLM_TFN(tlm_tfn)         \
  do {         \
    MSG_LOW("tlm_tfn: (f,cx8):%d, %d", (tlm_tfn).frame_qty, (tlm_tfn).chip_offset_cx8_qty, 0);         \
  }while(0)

#define TLM_LOG_TLM_CFN(tlm_cfn)         \
  do {         \
    MSG_LOW("tlm_cfn: (f,cx8):%d, %d", (tlm_cfn).frame_qty, (tlm_cfn).chip_offset_cx8_qty, 0);         \
  }while(0)

#define TLM_LOG_TLM_FRC(frc_quant)         \
  do {         \
    MSG_LOW("tlm_frc: (f,cx8):%d, %d", (frc_quant).frame_qty, (frc_quant).chip_offset_cx8_qty, 0);         \
  }while(0)

#define TLM_LOG_TLM_TST(tlm_tst)         \
  do {         \
    MSG_LOW("tlm_tst: (f,cx8):%d, %d", (tlm_tst).frame_qty, (tlm_tst).chip_offset_cx8_qty, 0);         \
  }while(0)

#define TLM_LOG_NW_DIFF_QTY(nw_diff)         \
  do {         \
    MSG_HIGH("tlm_nw_diff: (f,cx1):%d, %d", (nw_diff).frame_qty, (nw_diff).chip_offset_cx1_qty, 0);         \
  }while(0)

#define TLM_LOG_TLM_WRC_TFN_DIFF(wrc_tfn)         \
  do {         \
    MSG_HIGH("wrc_tfn_diff: (f,cx8):%d, %d", (wrc_tfn).frame_qty, (wrc_tfn).chip_offset_cx8_qty, 0);         \
  }while(0)

#define TLM_LOG_SRCH_SFN_WRC_DIFF(sfn_wrc)         \
  do {         \
    MSG_LOW("sfn_wrc: vld:%d fr:%d cx8:%d", (sfn_wrc).sfn_wrc_valid,         \
      (sfn_wrc).sfn_wrc_diff.frame_qty, (sfn_wrc).sfn_wrc_diff.chip_offset_cx8_qty);         \
  } while(0)

#define TLM_LOG_TLM_SFN_WRC_DIFF(sfn_wrc)         \
  do {         \
    MSG_HIGH("sfn_wrc: (f, cx8):%d, %d, %d",         \
      (sfn_wrc).frame_qty, (sfn_wrc).chip_offset_cx8_qty, 0);         \
  } while(0)

#endif /*FEATURE_WCDMA_TLM_LOG_ALL_STEPS*/


#define TLM_CHECK_FOR_OFF_ADJ(tm_new, tm_old)       \
  (abs((int32)(tm_new - tm_old)) >= TLM_MIN_TM_DIFF_TO_ADJ_OFF)

#define TLM_MIN_TM_DIFF_FOR_ERROR (12)

#define TLM_CHECK_FOR_ERRONEOUS_NEW_TM(tm_new, tm_old)       \
  (abs((int32)(tm_new - tm_old)) >= TLM_MIN_TM_DIFF_FOR_ERROR)

#define TLM_CH_TIME_FOR_KNOWN_CELL(sfn_wrc) (TLM_CELL_TIME_KNOWN(sfn_wrc))

#define TLM_CH_TIME_FOR_UNKNOWN_CELL(sfn_wrc) (TLM_CELL_TIME_UNKNOWN(sfn_wrc))

#define TLM_CH_TIME_FOR_UNKNOWN_CELL_WRT_FRC(sfn_wrc) ((TLM_CELL_TIME_UNKNOWN(sfn_wrc)) && (TLM_IS_FRC_ALIGNED()))

#define TLM_CH_TIME_FOR_UNKNOWN_CELL_WRT_NON_FRC(sfn_wrc) ((TLM_CELL_TIME_UNKNOWN(sfn_wrc)) && (TLM_IS_NON_FRC_ALIGNED()))

#define TLM_CELL_TIME_KNOWN(sfn_wrc) (sfn_wrc.sfn_wrc_valid)

#define TLM_CELL_TIME_UNKNOWN(sfn_wrc) (!TLM_CELL_TIME_KNOWN(sfn_wrc))

/*This macro should be used only when l1_max and hr_max are "power of 2"*/
#define TLM_COMPUTE_HW_HIGHER_RANGE(sw_hr,hw_lr,hw_hr,lr_max,hr_max)         \
  do {         \
    int16 sw_lr;         \
    sw_lr = sw_hr & (lr_max -1);         \
    if (sw_lr == hw_lr)         \
      hw_hr = sw_hr;         \
    else if( sw_lr < hw_lr)         \
     hw_hr = (int16)normalize((int32)(sw_hr + (int32)((int32)hw_lr - (int32)sw_lr)), (uint32)hr_max);             \
    else         \
      hw_hr = (int16)normalize((int32)(sw_hr + normalize((int32)((int32)hw_lr - (int32)sw_lr), (uint32)lr_max)), (uint32)hr_max);         \
  }while (0)

/*TST (Transmit System Time) Macros*/

/* This Macro reads the cx8 quantity of tst from a stmr tst read.
*/
#define STMR_TX_TIME_STATUS_PRI_TX_FRAME_COUNT_BMSK 0x7ffff
#define STMR_TX_TIME_STATUS_PRI_TX_FRAME_COUNT_SHFT 0x0
#define STMR_TX_TIME_STATUS_PRI_TX_TTI_COUNT_BMSK 0x7ff80000
#define STMR_TX_TIME_STATUS_PRI_TX_TTI_COUNT_SHFT 0x13


#define TLM_STRIP_CX8_FROM_TST_STATUS(stmr_tst)   \
            (((stmr_tst) & STMR_TX_TIME_STATUS_PRI_TX_FRAME_COUNT_BMSK)   \
             >> STMR_TX_TIME_STATUS_PRI_TX_FRAME_COUNT_SHFT)

/* This Macro reads the tti quantity of tst from a stmr tst read.
*/
#define TLM_STRIP_TTI_FROM_TST_STATUS(stmr_tst)   \
            ((((stmr_tst) & STMR_TX_TIME_STATUS_PRI_TX_TTI_COUNT_BMSK)   \
             >> STMR_TX_TIME_STATUS_PRI_TX_TTI_COUNT_SHFT) & (TLM_MAX_TST_COUNT-1))

#define TLM_STRIP_FULL_TTI_FROM_TST_STATUS(stmr_tst)   \
            (((stmr_tst) & STMR_TX_TIME_STATUS_PRI_TX_TTI_COUNT_BMSK)   \
             >> STMR_TX_TIME_STATUS_PRI_TX_TTI_COUNT_SHFT)

/*This macro strips the cx8 and tti quantity of a tst_status*/
#define TLM_PREP_TST_FROM_STMR_READ(tst_quant, stmr_read)    \
  do {    \
    (tst_quant).frame_qty = TLM_STRIP_TTI_FROM_TST_STATUS(stmr_read);    \
    (tst_quant).chip_offset_cx8_qty = TLM_STRIP_CX8_FROM_TST_STATUS(stmr_read);    \
  }while(0)

/*This macro strips the cx8 and full frame quantity of a tst_status*/
#define TLM_PREP_FULL_TST_FROM_STMR_READ(tst_quant, stmr_read)    \
  do {    \
    (tst_quant).frame_qty = TLM_STRIP_FULL_TTI_FROM_TST_STATUS(stmr_read);    \
    (tst_quant).chip_offset_cx8_qty = TLM_STRIP_CX8_FROM_TST_STATUS(stmr_read);    \
  }while(0)

/* This Macro reads the cx8 quantity of frc from a stmr frame ref read.
*/
#define STMR_REF_COUNT_STATUS_PRI_FRAME_REF_COUNT_BMSK 0x7ffff
#define STMR_REF_COUNT_STATUS_PRI_FRAME_REF_COUNT_SHFT 0x0
#define STMR_REF_COUNT_FRAME_REF_COUNT_BMSK 0xffff
#define STMR_REF_COUNT_FRAME_REF_COUNT_SHFT 0x3
#define STMR_REF_COUNT_STATUS_PRI_FRAME_NUM_BMSK 0x7ff80000
#define STMR_REF_COUNT_STATUS_PRI_FRAME_NUM_SHFT 0x13
#define STMR_REF_COUNT_FRAME_NUM_BMSK 0xfff0000
#define STMR_REF_COUNT_FRAME_NUM_SHFT 0x10

#define TLM_STRIP_CX8_FROM_FRC_STATUS(stmr_frc)   \
            (((stmr_frc) & STMR_REF_COUNT_STATUS_PRI_FRAME_REF_COUNT_BMSK)   \
             >> STMR_REF_COUNT_STATUS_PRI_FRAME_REF_COUNT_SHFT)

#define TLM_STRIP_CX1_FROM_FRC_DYNAMIC(stmr_frc)   \
            (((stmr_frc) & STMR_REF_COUNT_STATUS_PRI_FRAME_REF_COUNT_BMSK)   \
             >> STMR_REF_COUNT_FRAME_REF_COUNT_SHFT)

/* This Macro reads the frame quantity of frc from a stmr frame ref read.
*/
#define TLM_STRIP_FRAMES_FROM_FRC_STATUS(stmr_frc)   \
            (((stmr_frc) & STMR_REF_COUNT_STATUS_PRI_FRAME_NUM_BMSK)   \
             >> STMR_REF_COUNT_STATUS_PRI_FRAME_NUM_SHFT)
#define TLM_STRIP_FRAMES_FROM_FRC_DYNAMIC(stmr_frc)   \
            (((stmr_frc) & STMR_REF_COUNT_STATUS_PRI_FRAME_NUM_BMSK)   \
             >> STMR_REF_COUNT_STATUS_PRI_FRAME_NUM_SHFT)

#define TLM_PREP_WRC_FROM_STMR_DYNAMIC(frc_quant, stmr_read)    \
  do {    \
     stmr_read = STMR_REF_COUNT_DYNAMIC(); \
    (frc_quant).frame_qty = ((TLM_STRIP_FRAMES_FROM_FRC_DYNAMIC(stmr_read)));    \
    (frc_quant).chip_offset_cx8_qty = ((TLM_STRIP_CX1_FROM_FRC_DYNAMIC(stmr_read)) << 3);    \
  }while(0)


/*Macros for FRC*/
#define TLM_PREP_FRC_FROM_STMR_READ(frc_quant, stmr_read)    \
  do {    \
    (frc_quant).frame_qty = ((TLM_STRIP_FRAMES_FROM_FRC_STATUS(stmr_read)) % TLM_MAX_FRC_COUNT);    \
    (frc_quant).chip_offset_cx8_qty = TLM_STRIP_CX8_FROM_FRC_STATUS(stmr_read);    \
  }while(0)

/*Macros for FRC*/
#define TLM_PREP_WRC_FROM_STMR_READ(frc_quant, stmr_read)    \
  do {    \
    (frc_quant).frame_qty = ((TLM_STRIP_FRAMES_FROM_FRC_STATUS(stmr_read)));    \
    (frc_quant).chip_offset_cx8_qty = TLM_STRIP_CX8_FROM_FRC_STATUS(stmr_read);    \
  }while(0)
  
#define TLM_ADD_SFN_FRC(result, addend1, addend2)         \
  TLM_ADD_TWO_SFN(result, addend1, addend2)

#define TLM_SUB_TWO_FRC(result, minuend, subtrahend )    \
  do {    \
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
      (result).frame_qty = (result).frame_qty + TLM_MAX_FRC_COUNT;         \
    }         \
  }while(0)

#define TLM_COPY_TWO_FRAME_QUANT(src, dest)         \
  do {         \
    (dest).frame_qty = (src).frame_qty;         \
    (dest).chip_offset_cx8_qty = (src).chip_offset_cx8_qty;         \
  }while(0)
  
/*This macro disables all SFN and CFN related events
 */
#define TLM_DISABLE_SFN_CFN_EVENTS()        \
  do {        \
        MCALWCDMA_EVT_DISABLE_EVENTS(        \
      MCALWCDMA_EVT_GEN_TL_INTR,        \
      TLM_MASK_FOR_SFN_CFN_EVENTS);        \
  } while(0)

#define TLM_TFN_TO_NW_CONVERSION_ROUNDOFF_QUANT (CHIPX4_PER_CHIP)

#define TLM_CONVERT_TFN_QUANT_TO_NW_DIFF_QUANT(tfn, nw_diff)    \
  do {    \
    tlm_tfn_struct_type rounded_off_tfn;    \
    TLM_ADD_TWO_SFN(rounded_off_tfn, (tfn), tfn_to_nw_roundoff_quant);    \
    TLM_CONVERT_FRAME_QUANT_TFN_TO_CFN(rounded_off_tfn.frame_qty, (nw_diff).frame_qty);    \
    (nw_diff).chip_offset_cx1_qty = (rounded_off_tfn).chip_offset_cx8_qty >> CHIPX8_TO_CHIP_RSHIFT;    \
  } while(0)

#define TLM_ROUNDOFF_CX8_TO_CX1(cx8_val, cx1_val)    \
  do {    \
    TLM_ADD_CHIP_QUANT((cx8_val), (cx8_val), CHIPX4_PER_CHIP);    \
    (cx1_val) = (uint16)(((uint32)(cx8_val)) >> CHIPX8_TO_CHIP_RSHIFT);    \
  } while(0)

/*This macro enables all SFN and CFN related events
 */
#define TLM_ENABLE_SFN_CFN_EVENTS()        \
  do {        \
        MCALWCDMA_EVT_ENABLE_EVENTS(        \
      MCALWCDMA_EVT_GEN_TL_INTR,        \
      TLM_MASK_FOR_SFN_CFN_EVENTS);        \
  } while(0)

/*This macro enables all SFN, CFN and CM related events
 */
#define TLM_ENABLE_SFN_CFN_CM_EVENTS()        \
  do {        \
        MCALWCDMA_EVT_ENABLE_EVENTS( MCALWCDMA_EVT_GEN_TL_INTR,        \
        TLM_MASK_FOR_SFN_CFN_CM_EVENTS);        \
  } while(0)

/*#define TLM_ENABLE_EVENTS_AFTER_ALIGN()        \
  do {        \
    if (L1_IS_IN_DCH_TO_DCH_HHO())        \
    {        \
      TLM_ENABLE_SFN_CFN_CM_EVENTS();        \
      MSG_HIGH("Keeping CM events alive for interF HHO", 0, 0, 0);        \
    }        \
    else        \
    {        \
      TLM_ENABLE_SFN_CFN_EVENTS();        \
    }        \
  } while(0)*/

#define TLM_DISABLE_EVENTS_BEFORE_ALIGN()        \
  do {        \
  TLM_DISABLE_SFN_CFN_EVENTS();        \
  } while(0)

#define TLM_ALIGN_IS_PARAMS_OK(cntr, cell_info_ptr, ch_off_ptr)    \
  do {    \
    if ((cntr != STMR_FRAME_REF_COUNTER) &&    \
        ((cell_info_ptr == NULL) || (ch_off_ptr == NULL) || (! TLM_HAS_ANY_ALIGN_HAPPENED())))    \
    {    \
      ERR_FATAL("invalid params to tlm_set_align cntr:%d cell_info:%d ch_off:%d", cntr, cell_info_ptr, ch_off_ptr);    \
    }    \
  } while(0)

#define TLM_RCA_CX8_MAX_QUANT_FOR_ZERO_FRAME (153599)

#define TLM_BUILD_RCA_COMPLETE(ref_cnt)    \
  do {    \
    ref_cnt.chip_offset_cx8_qty = tlm_get_ref_cnt_adj();    \
    if (ref_cnt.chip_offset_cx8_qty <= TLM_RCA_CX8_MAX_QUANT_FOR_ZERO_FRAME)    \
    {    \
      ref_cnt.frame_qty = 0;    \
    }    \
    else    \
    {    \
      ref_cnt.frame_qty = -1;    \
    }    \
  } while(0)

#define TLM_COMP_TFN_FOR_WRC_CALC(tfn, adj_tfn)    \
  do {    \
    if (TLM_IS_FRC_ALIGNED())    \
    {    \
      tlm_tfn_struct_type ref_cnt;    \
      TLM_BUILD_RCA_COMPLETE(ref_cnt);    \
      TLM_ADD_TWO_SFN(adj_tfn, tfn, ref_cnt);    \
    }    \
    else    \
    {    \
      TLM_COPY_TLM_TFN(tfn, adj_tfn);    \
    }    \
  } while(0)

#define TLM_COMP_WRC_FROM_TFN(wrc, tfn)    \
  do {    \
    TLM_ADD_TWO_SFN(wrc, tfn, tlm_db.wrc_tfn_diff);    \
    if (TLM_IS_FRC_ALIGNED())    \
    {    \
      tlm_tfn_struct_type ref_cnt;    \
      TLM_BUILD_RCA_COMPLETE(ref_cnt);    \
      TLM_ADD_TWO_SFN(wrc, wrc, ref_cnt);    \
    }    \
  } while(0)

#define TLM_COMP_WRC_POS_WRT_REAL_RXTL(wrc_pos_wrt_rxtl, tfn_time, wrc_time)    \
  do {    \
    (wrc_pos_wrt_rxtl) = (tfn_time) - (wrc_time);         \
    if (TLM_IS_FRC_ALIGNED())    \
    {    \
      uint32 ref_cnt_cx8;    \
      ref_cnt_cx8 = tlm_get_ref_cnt_adj();    \
      (wrc_pos_wrt_rxtl) = (wrc_pos_wrt_rxtl) + (ref_cnt_cx8);    \
    }    \
    if (((wrc_pos_wrt_rxtl) >= TLM_UNITS_PER_FRAME) || ((wrc_pos_wrt_rxtl) < 0))         \
    {         \
      wrc_pos_wrt_rxtl = normalize(wrc_pos_wrt_rxtl, TLM_UNITS_PER_FRAME);         \
    }         \
  } while(0)

/*This macro copies two frc quantities passed as parameter*/
#define TLM_COPY_TLM_FRC(source, dest)         \
  do {         \
    (dest).frame_qty = (source).frame_qty;         \
    (dest).chip_offset_cx8_qty = (source).chip_offset_cx8_qty;         \
  }while(0)

#define TLM_COPY_TLM_FRC_TO_TFN(source, dest)         \
  do {         \
    (dest).frame_qty = (source).frame_qty;         \
    (dest).chip_offset_cx8_qty = (source).chip_offset_cx8_qty;         \
  }while(0)

#define TLM_GET_SFN_FOR_FRC_ALIGNMENT(tfn)    \
  do {    \
    tlm_snapshot_struct_type ss;    \
    tlm_get_snapshot(&ss, TLM_SNAPSHOT_MASK_FRC, STMR_MAX_NUM_COUNTERS);    \
    TLM_COPY_TLM_FRC_TO_TFN(ss.frc, (tfn));    \
  } while(0)

#define TLM_COMP_FULL_TST_CFN_FRAME_QTY(full_rx_tfn_fr, full_tst_cfn_fr, sh_tst_fr)    \
  do {    \
    int16 sh_tfn_fr, full_tst_tfn_fr;    \
    sh_tfn_fr = (full_rx_tfn_fr & (TLM_MAX_TST_COUNT-1));    \
    if (sh_tst_fr == sh_tfn_fr)    \
    {    \
      full_tst_tfn_fr = full_rx_tfn_fr;    \
    }    \
    else    \
    {    \
      TLM_SUB_TWO_TFN_FRAME_QUANT(full_tst_tfn_fr, full_rx_tfn_fr, 1);    \
    }    \
    TLM_CONVERT_FRAME_QUANT_TFN_TO_CFN(full_tst_tfn_fr, full_tst_cfn_fr);    \
  } while(0)

/* On offline targets, Rx frame boundary happens first followed by the Tx frame boundary
  When a STMR snapshot is taking between these frame boundaries, Tx frame number
  will be lagging Rx Frame number by 1. In order to compensate this, TST_TFN will 
  become 1 less than than the RxTL_TFN */
#define TLM_COMP_FULL_TST_TFN_FRAME_QUANTITY(full_rx_tfn_fr, full_tst_tfn_fr, sh_tst_fr)    \
  do {    \
    int16 sh_tfn_fr;    \
    sh_tfn_fr = (full_rx_tfn_fr & (TLM_MAX_TST_COUNT-1));    \
    if (sh_tst_fr == sh_tfn_fr)    \
    {    \
      full_tst_tfn_fr = full_rx_tfn_fr;    \
    }    \
    else    \
    {    \
      TLM_SUB_TWO_TFN_FRAME_QUANT(full_tst_tfn_fr, full_rx_tfn_fr, 1);    \
    }    \
  } while(0)

/* Commented out for Lint. To be uncommented when needed
#define TLM_COMP_NOM_DPCH_CFN_FROM_TST_CFN(tst_cfn, nom_dpch_cfn)    \
  do {    \
    TLM_ADD_TWO_CFN(nom_dpch_cfn, tst_cfn, nom_dpch_tst_diff_qty_in_cfn);    \
  } while(0)
*/  

#define TLM_COMP_NOM_DPCH_TFN_FROM_TST_TFN(tst_tfn, nom_dpch_tfn)    \
  do {    \
    TLM_ADD_TWO_SFN(nom_dpch_tfn, tst_tfn, nom_dpch_tst_diff_qty_in_tfn);    \
  } while(0)

#define TLM_CH_TIME_IS_CELL_TIME_KNOWN(ch_type)    \
    (tlm_ch_time_comp_params[ch_type].cell_time_known)

#define TLM_CH_TIME_IS_REF_CNT_ADJ_NEEDED(ch_type)    \
    (tlm_ch_time_comp_params[ch_type].ref_cnt_adj_needed)

#define TLM_CH_TIME_IS_MDSP_DESKEW_NEEDED(ch_type)    \
    (tlm_ch_time_comp_params[ch_type].deskew_depth_to_be_applied)

#define TLM_DBG_SFN_SFN_DIFF_RATE_IN_FACH_STATE (10)

#define TLM_DBG_SFN_CFN_DIFF_RATE_IN_DCH_STATE (50)

#define TLM_DBG_SFN_RATE (10)

#ifdef FEATURE_WCDMA_TLM_LOG_SS_STEPS
#error code not present
#else

#define TLM_LOG_SS_WRC(tlm_wrc)         \
  do {         \
    MSG_LOW("tlm_wrc: (f,cx8):%d, %d", (tlm_wrc).frame_qty, (tlm_wrc).chip_offset_cx8_qty, 0);         \
  }while(0)

#define TLM_LOG_SS_TFN(tlm_tfn)         \
  do {         \
    MSG_LOW("tlm_tfn: (f,cx8):%d, %d", (tlm_tfn).frame_qty, (tlm_tfn).chip_offset_cx8_qty, 0);         \
  }while(0)

#define TLM_LOG_SS_CFN(tlm_cfn)         \
  do {         \
    MSG_LOW("tlm_cfn: (f,cx8):%d, %d", (tlm_cfn).frame_qty, (tlm_cfn).chip_offset_cx8_qty, 0);         \
  }while(0)

#define TLM_LOG_SS_FRC(frc_quant)         \
  do {         \
    MSG_LOW("tlm_frc: (f,cx8):%d, %d", (frc_quant).frame_qty, (frc_quant).chip_offset_cx8_qty, 0);         \
  }while(0)

#define TLM_LOG_SS_TST(tlm_tst)         \
  do {         \
    MSG_LOW("tlm_tst: (f,cx8):%d, %d", (tlm_tst).frame_qty, (tlm_tst).chip_offset_cx8_qty, 0);         \
  }while(0)

#endif

/*===========================================================================
FUNCTION             TLM_GET_CELL_POS

FILE NAME            tlm.c

DESCRIPTION          This function finds and returns the cell position.

INPUT PARAMS
                     cell: the cell for which the position needs to be found.

OUTPUT PARAMS        None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern uint32 tlm_get_cell_pos(srch_cell_struct_type *cell);

/*===========================================================================
FUNCTION             TLM_ADJUST_SFN_WRC_DIFF

FILE NAME            tlm.c

DESCRIPTION          This function adjusts the sfn_wrc diff value since it's
                     computation/last-adjustment.The adjustment is done based
                     on the chipx8 position of the cell and the WRC.

INPUT PARAMS
                     cell_info: the cell for which the sfn_wrc needs to be adjusted
                     sfn_wrc_bef_adj: the old value of the sfn_wrc difference

OUTPUT PARAMS
                     sfn_wrc_aft_adj: the new value of the sfn_wrc difference

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         This function assumes that the cell_info and
                     sfn_wrc_bef_adj are not NULL. Also, the sfn_wrc_diff
                     is assumed to be valid and cell_ptr returned by the
                     searcher is assumed to be non-NULL.

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_adjust_sfn_wrc_diff(tlm_cell_info_struct_type *cell_info, 
    sfn_wrc_diff_type *sfn_wrc_bef_adj, sfn_wrc_diff_type *sfn_wrc_aft_adj);

/*===========================================================================
FUNCTION             TLM_FORCE_ADJUST_SFN_WRC_DIFF

FILE NAME            tlm.c

DESCRIPTION          This function adjusts the sfn_wrc diff value since it's
                     computation/last-adjustment.The adjustment is done based
                     on the chipx8 position of the cell and the WRC. In addition
                     its predecessor (tlm_adjust_sfn_wrc_diff), it adjusts the
                     SFN_WRC even if the difference is more than 128 chips.

INPUT PARAMS
                     cell_info: the cell for which the sfn_wrc needs to be adjusted
                     sfn_wrc_bef_adj: the old value of the sfn_wrc difference

OUTPUT PARAMS
                     sfn_wrc_aft_adj: the new value of the sfn_wrc difference

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         This function assumes that the cell_info and
                     sfn_wrc_bef_adj are not NULL. Also, the sfn_wrc_diff
                     is assumed to be valid and cell_ptr returned by the
                     searcher is assumed to be non-NULL.

RETURN VALUE         None

SIDE EFFECTS         This function should be used very sparingly as force
                     update of SFN_WRC will change the cell timing abruptly
===========================================================================*/
extern void tlm_force_adjust_sfn_wrc_diff(tlm_cell_info_struct_type *cell_info, 
    sfn_wrc_diff_type *sfn_wrc_bef_adj, sfn_wrc_diff_type *sfn_wrc_aft_adj);

/*===========================================================================
FUNCTION             TLM_HANDLE_WRC_TFN_DIFF_BEF_ALIGN

FILE NAME            tlm.c

DESCRIPTION          This function takes a snapshot of the necessary
                     variables in the tlm_db before align change.

INPUT PARAMS         None

OUTPUT PARAMS        None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_handle_wrc_tfn_diff_bef_align( void );

/*===========================================================================
FUNCTION             TLM_HANDLE_WRC_TFN_DIFF_AFT_ALIGN

FILE NAME            tlm.c

DESCRIPTION          This function takes a snapshot of the necessary
                     variables in the tlm_db after align change. It re-computes
                     the wrc_tfn_diff with the help of the snapshot variables
                     in tlm_db.

INPUT PARAMS         None

OUTPUT PARAMS        None

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_handle_wrc_tfn_diff_aft_align(tlm_tfn_struct_type sfn, int16 cfn_frame_qty);

/*===========================================================================
FUNCTION             TLM_GET_SFN_WRC_DIFF_WITH_ADJ

FILE NAME            tlm.c

DESCRIPTION          This function gets the sfn_wrc_diff from the searcher
                     database, adjusts it with that cell's latest reference
                     position and WRC (TFN) position and updates it back in to
                     the searcher database.

INPUT PARAMS
                     cell_info: the structure that identifies the cell

OUTPUT PARAMS
                     out: the structure that carries the sfn_wrc_diff

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
void tlm_get_sfn_wrc_diff_with_adj(tlm_cell_info_struct_type *cell_info, sfn_wrc_diff_type *out);

/*===========================================================================
FUNCTION             TLM_GET_SFN_WRC_DIFF

FILE NAME            tlm.c

DESCRIPTION          This function gets the sfn_wrc_diff from the searcher
                     database, adjusts it with that cell's latest reference
                     position and WRC (TFN) position and updates it back in to
                     the searcher database.

INPUT PARAMS
                     cell_info: the structure that identifies the cell

OUTPUT PARAMS
                     out: the structure that carries the sfn_wrc_diff

EXECUTION CONTEXT    Not Applicable

DEPENDENCIES         None

RETURN VALUE         None

SIDE EFFECTS         None
===========================================================================*/
extern void tlm_get_sfn_wrc_diff(tlm_cell_info_struct_type *cell_info, sfn_wrc_diff_type *out);

/*===========================================================================
FUNCTION             TLM_GET_CH_COMPUTATION_TYPE

FILE NAME            tlm.c

DESCRIPTION          This function finds the type of channel CFN computation
                      to be done.

INPUT PARAMS
                     sfn_wrc_valid: sfn-wrc is valid for the cell for which
                     the CFN is being computed

OUTPUT PARAMS        None

EXECUTION CONTEXT    Task-yes; Interrupt-yes

DEPENDENCIES         None

RETURN VALUE         the type of the channel computation scenario

SIDE EFFECTS         None
===========================================================================*/
extern tlm_ch_time_comp_scenario_enum_type tlm_get_ch_computation_type(boolean sfn_wrc_valid);

#endif
