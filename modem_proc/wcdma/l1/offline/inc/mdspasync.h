#ifndef MDSP_ASYNC_H
#define MDSP_ASYNC_H

/*===========================================================================
                             M S D P _ S Y N C . H

DESCRIPTION
  The async command interface between the ARM and the mDSP on the MSM7200.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006-2007 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mdspasync.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/13/14    ymu     Fix for secondary Happy bit not Tx correctly
10/13/14    ymu     Fix for HICH CM mask populating incorrectly for DC-HSUPA
10/01/14    gv      Code cleanup. Removal of stale and unused code. 
07/28/14    pkg     Code Check-in for 16QAM feature.
04/27/14    jkb     Add check for Feature dual sim with feature qta 
01/22/14    gp      Fix for race condition during QTA cleanup
11/04/13    yw      CM support for DCHSUPA
09/23/13    as      Porting Dual Sim from Triton to Dime.
05/22/13    oh      Adding DC-HSUPA framework support
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv      Feature HS RACH Initial code check in
02/01/12    raj     Nikel Feature cleanup.
02/01/12    gnk     Nikel Feature cleanup.
02/01/12    gnk     Nikel feature cleanup
11/29/11    gnk     Removed some unused features
11/24/11    gnk     Removed unused features
03/30/11    gnk     Mainlined several features
06/21/10    ksr     Compiler warning fixes
06/11/10    sa      Change format of Beta_ec in FW-SW interface structure from Q11 to Q8
04/09/10    ksr     Changes to move logging variables to dynamic memory approach
02/08/10    oh      Taxis/Genesis Code merge. 
01/22/10    ksr     Merging the Mustang Tip 
01/18/10    sa      Rename FEATURE_WCDMA_HSUPA_SUBFN_MISMATCH to 
                    DEBUG_WCDMA_HSUPA_SUBFN_MISMATCH 
11/18/09    gv      Use db10 instead of db12 units for computation
10/28/09    m       Added debug code to catch FW SW subFN mismatch
07/22/09    gnk     Mainlining the feature FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
06/29/09    oh      Rel 7 E-MPR support for MIMO HS Types.
03/04/09    sa      Changes to support for EDL channel threshold computation with CM.
02/18/09    sa      Extern EDL result buffer read indices for the purpose 
                    of initialising them during EUL START.
01/30/09    ka      Added Support for Enhanced MPR algorithm
12/10/08    mc      Added support for F/W diag debug command
11/28/08    gnk     Removed the RUMI_BRING_UP feature 
11/18/08    sa      Changes to support FEATURE_WCDMA_HSUPA_2MS_TTI 
10/03/08    hk      Bringing in 7k Mailine fixes
10/01/08    mc      Added support for MIMO UL.
08/13/08    gv      EUL fixes for stop and other clean up.
07/31/08    oh      EUL changes for Taxis.
01/07/08    asm     Added changes for MPR based beta scaling phase 2
10/08/07    gv      Added #define for minimum num slots decoded.
09/27/07    oh      Interface change to let FW know the Pseudo Gap.
09/24/07    oh      Interface change to let FW know the NTX1 value.   
08/01/07    oh      Interface change to let FW know if UL DPDCH is configured.
06/13/07    gv      Added code for square root combining.
04/04/07    yiz     Add MTPL_no_mpr to FW interface for REL-5
01/30/07    yiz     Added MTPL_no_mpr to FW Tx power limits table
12/14/06    ub      Renamed constant for 12 slots of HICH and RGCH decode
12/08/06    am      Add HSUPA CM support
11/09/06    asm     Added Support for Rel6 Maximum Power Reduction calculations
11/08/06    ub      Added support for Nslots field in FW HICH/RGCH res buf
10/16/06    ub      Added support for EUL DL CM
10/03/06    gv      Modified the ENUM mdspasync_hich_rgch_res_read_enum_type
                    to explicitly define the enum valuees and add one more
                    to perform the bounds checking in the for loop while 
                    reading the HICH and RGCH results.
09/25/06    gv      Added a enum to differentiate between the HICH and RGCH
                    buffer read. Also modified the #define for EDL buffer
                    read bit mask, as a result of code cleanup
08/14/06    yiz     Mainline HSUPA code
03/22/06    am      Added API to read the async results from mDSP
03/21/06    am      Added MTPL data to txpwrctrlparam buffer & corrected APIs
01/14/06    am      file created.
                    
===========================================================================*/

/* <EJECT> */

/*===========================================================================

                     DEFINE/UNDEFINE LOCAL FEATURES  

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "mdsputil.h"
#include "eulsg.h"
#include "eulcfg.h"

#include "wfw_sw_intf.h"


/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/

/* SW passes A_hsX15*2^11 to FW */
#define MDSP_EDPCH_BETA_HS_SHFT  11


/* SW passes A_EcX15*2^8 to FW */
#define MDSP_EDPCH_BETA_EC_SHFT  8

/*mDSP EDPCH async param db size   FW HLD-2.1.1.4*/
#define   MDSP_EDPCH_PARAM_LEN                    11

/* address of async mDSP EDPCH  beta ec param in the async EDPCH param db  
   FW HLD-2.1.1.4*/
#define MDSP_EDPCH_PARAM_BUF0_BETA_EC_ADDR  MDSP_EDPCH_PARAM_BUF0+14
#define MDSP_EDPCH_PARAM_BUF1_BETA_EC_ADDR  MDSP_EDPCH_PARAM_BUF1+14


/* HICH and RGCH result buffer start and end addresses. The HICH and RGCH 
   result buffers are double buffered for max of 4 RL. Info for each RL is
   of size 4 words. First word contains dirty bit and target sub fn. Second 
   and third words contain energy value. Fourth word contains number of slots
   over which HICH/RGCH was decoded for the result.
*/
#define MDSP_EDL_HICH_RES_BUF_START_ADDR         MDSP_ehichResBuf
#define MDSP_EDL_HICH_RES_BUF_END_ADDR MDSP_EDL_HICH_RES_BUF_START_ADDR+31

#define MDSP_EDL_RGCH_RES_BUF_START_ADDR        MDSP_ergchResBuf
#define MDSP_EDL_RGCH_RES_BUF_END_ADDR MDSP_EDL_RGCH_RES_BUF_START_ADDR+31


/* The following are the fields in the EDPCH async param information 
 required by mDSP-FW HLD:Figure 2 13 */

/* These bits indicate the bank index to which SW will write the data
  -bits 0-3 in row 0.*/
#define EUL_EDPCH_PARAM_DB_ERAM_BANK_IDX_BMSK       0x000F
#define EUL_EDPCH_PARAM_DB_ERAM_BANK_IDX_REG        0
#define EUL_EDPCH_PARAM_DB_ERAM_BANK_IDX_SHFT       0

/* These bits indicate the Ntx1 value which SW will write the data
  -bits 4-7 in row 0.*/
#define EUL_EDPCH_PARAM_DB_NTX1_BMSK                0x00F0
#define EUL_EDPCH_PARAM_DB_NTX1_REG                 0
#define EUL_EDPCH_PARAM_DB_NTX1_SHFT                4


/* These bits indicate Number of systematic bits that would help DSP 
calculate the RM parameters -bits 0-14 in row 1*/
#define EUL_EDPCH_PARAM_DB_NEJ_BMSK                0xFFFF
#define EUL_EDPCH_PARAM_DB_NEJ_REG                 1
#define EUL_EDPCH_PARAM_DB_NEJ_SHFT                0

/* Thess bits indicate  Ne,data,j - bits 0-15 in row 2*/
#define EUL_EDPCH_PARAM_DB_TOTAL_DATA_BMSK          0xFFFF
#define EUL_EDPCH_PARAM_DB_TOTAL_DATA_REG           2
#define EUL_EDPCH_PARAM_DB_TOTAL_DATA_SHFT          0

/* These bits indicate  the happy bit info- 25.212-670  4.9.2
 - bit 0 in row 3*/
#define EUL_EDPCH_PARAM_DB_HAPPY_BIT_BMSK           0x0001
#define EUL_EDPCH_PARAM_DB_HAPPY_BIT_REG            3
#define EUL_EDPCH_PARAM_DB_HAPPY_BIT_SHFT           0

/* These bits indicate Retransmission sequence number 
25.212-670- table 16- bits 1-2 in row 3*/
#define EUL_EDPCH_PARAM_DB_RSN_BMSK                 0x0006
#define EUL_EDPCH_PARAM_DB_RSN_REG                  3
#define EUL_EDPCH_PARAM_DB_RSN_SHFT                 1

/* These bits indicate The E-TFI used-25.212-670  4.9.2 
 - bits 3-9 in row 3*/
#define EUL_EDPCH_PARAM_DB_ETFI_BMSK                0x03F8
#define EUL_EDPCH_PARAM_DB_ETFI_REG                 3
#define EUL_EDPCH_PARAM_DB_ETFI_SHFT                3

/* This info is used to map the data sent on different E-DPDCH to
 the appropriate I & Q channel.Bit 12 - CH_3_4_ENABLE    
  Bit 11 - I_Q_SELECT  - bits 11-12 in row 3*/
#define EUL_EDPCH_PARAM_DB_I_Q_SELECT_BMSK       0x0800
#define EUL_EDPCH_PARAM_DB_I_Q_SELECT_REG        3
#define EUL_EDPCH_PARAM_DB_I_Q_SELECT_SHFT       11

#define EUL_EDPCH_PARAM_DB_CHAN_3_4_ENABLE_BMSK       0x1000
#define EUL_EDPCH_PARAM_DB_CHAN_3_4_ENABLE_REG        3
#define EUL_EDPCH_PARAM_DB_CHAN_3_4_ENABLE_SHFT       12

/* These bits indicate the total number of E-DPDCH channels with SF2 
configured for the TTI - bits 13-14 in row 3*/
#define EUL_EDPCH_PARAM_DB_NUM_SF2_EDPDCH_BMSK      0x6000
#define EUL_EDPCH_PARAM_DB_NUM_SF2_EDPDCH_REG       3
#define EUL_EDPCH_PARAM_DB_NUM_SF2_EDPDCH_SHFT      13

/* These bits indicates whether UL DPDCH is configured
for the TTI - bit 15 in row 3*/
#define EUL_EDPCH_PARAM_DB_DCH_CONFIG_BMSK          0x8000
#define EUL_EDPCH_PARAM_DB_DCH_CONFIG_REG           3
#define EUL_EDPCH_PARAM_DB_DCH_CONFIG_SHFT          15

/* These bits indicate  The SF of the E-DPDCH channel1.
 - bits 0-2 in row 4*/
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF1_BMSK          0x0007
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF1_REG           4
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF1_SHFT          0

/* These bits indicate OVSF code number for the E-DPDCH channel1
 -bits 3-8 in row 4*/
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF1_BMSK        0x07F8
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF1_REG         4
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF1_SHFT        3

/* This bit indicates  if there is a valid information for EDPDCH channel1
 - bit 15 in row 4*/
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT1_BMSK          0x8000
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT1_REG           4
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT1_SHFT          15

/* These bits indicate The SF of the E-DPDCH channel2. - bits 0-2 in row 5*/
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF2_BMSK          0x0007
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF2_REG           5
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF2_SHFT          0
/* These bits indicate OVSF code number for the E-DPDCH channel2
-bits 3-8 in row 5*/
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF2_BMSK        0x07F8
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF2_REG         5
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF2_SHFT        3

/* This bit indicates if there is a valid information for EDPDCH channel2
 - bit 15 in row 5*/
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT2_BMSK          0x8000
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT2_REG           5
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT2_SHFT          15

/* These bits indicate The SF of the E-DPDCH channel3. 
- bits 0-2 in row 6*/
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF3_BMSK          0x0007
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF3_REG           6
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF3_SHFT          0

/* These bits indicate OVSF code number for the E-DPDCH channel3
-bits 3-8 in row 6*/
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF3_BMSK        0x07F8
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF3_REG         6
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF3_SHFT        3

/* This bit indicates if there is a valid information for EDPDCH channel3
 - bit 15 in row 6*/
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT3_BMSK          0x8000
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT3_REG           6
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT3_SHFT          15

/* These bits indicate The SF of the E-DPDCH channel4.- bits 0-2 in row 7*/
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF4_BMSK          0x0007
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF4_REG           7
#define EUL_EDPCH_PARAM_DB_EDPDCH_SF4_SHFT          0

/* These bits indicate OVSF code number for the E-DPDCH channel4
-bits 3-8 in row 7*/
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF4_BMSK        0x07F8
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF4_REG         7
#define EUL_EDPCH_PARAM_DB_EDPDCH_OVSF4_SHFT        3

/* This bit indicates  if there is a valid information for EDPDCH channel3
 - bit 15 in row 7*/
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT4_BMSK          0x8000
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT4_REG           7
#define EUL_EDPCH_PARAM_DB_DIRTY_BIT4_SHFT          15

/* These bits indicate Gain factor for E-DPCCH channel bits 0-15 in row 8*/
#define EUL_EDPCH_PARAM_DB_BETAEC_BMSK              0xFFFF
#define EUL_EDPCH_PARAM_DB_BETAEC_REG               8
#define EUL_EDPCH_PARAM_DB_BETAEC_SHFT              0

/* These bits indicate Gain factor for E-DPDCH channel bits 0-15 in row 9*/
#define EUL_EDPCH_PARAM_DB_BETAED_BMSK              0xFFFF
#define EUL_EDPCH_PARAM_DB_BETAED_REG               9
#define EUL_EDPCH_PARAM_DB_BETAED_SHFT              0

/* These bits indicate Gain factor for E-DPDCH channel with SF 2- bits 0-15
   in row 10
*/
#define EUL_EDPCH_PARAM_DB_BETAED_SF2_BMSK          0xFFFF
#define EUL_EDPCH_PARAM_DB_BETAED_SF2_REG           10
#define EUL_EDPCH_PARAM_DB_BETAED_SF2_SHFT          0

/*The bufIdx is bit for double buffering. It is in CM table 0 only. It indicates 
   mask0 or mask1 should be used.  
*/
#define EUL_EDPCH_CM_PARAM_DB_IDX_SHFT    15

/*fill the buf idx*/
#define MDSPASYNC_WRITE_EUL_CM_BUF_IDX(val) \
    mdsp_write(((volatile uint16 *)(MDSP_eulCMSlotBitMask0)), ((uint16)(val)))
    

/* macro for deciding the bit position ,location in shadow buffer for 
   EDPDCH async param 
*/
#define EUL_EDPCH_PARAM_DB_SET_PARAM(buffer, param, val) \
  MDSP_SET_BUF_VAL(buffer, \
                 EUL_EDPCH_PARAM_DB_##param##_REG, \
                 EUL_EDPCH_PARAM_DB_##param##_SHFT, \
                 EUL_EDPCH_PARAM_DB_##param##_BMSK, \
                 val)

/* indicate mDSP that the  data write into ERAM bank is complete.
   MDSP_PDU_WRITE_COMPLETE_FLAG will be checked by FW just before configuring
   the HW for the next HARQ data. 1-> implies SW managed to write the date for
   the next HARQID in the ERAM bank indicated to the FW
*/
#define MDSPASYNC_WRITE_EUL_PDU_DONE(val) \
    mdsp_write(((volatile uint16 *)(MDSP_eulPduWriteCompleteFlag)), ((uint16)(val)))

/* Macro for computing the combined energy. This will be based on whether we
** are using SQRT combining or not. When we are computing the square of energy
** we should keep the sign intact otherwise down will be treated as UP unecessarily*/
#define MDSPASYNC_COMPUTE_SQUARE_OF_ENERGY(square_eng,eng) \
  if(eng < 0) \
  { \
    square_eng = (-1)*(((int64)eng)*eng); \
  } \
  else \
  { \
    square_eng = (((int64)eng)*eng); \
  }

/*bit masks corresponding to fields in the FW Async DL result read buffer  as 
  given in the FW HLD -Figure 2 20  
*/
/* bit mask to know the result buffer index*/
#define      E_DL_CHAN_RESULT_BUF_IDX_BMSK          0x0001
#define      E_DL_CHAN_RESULT_BUF_IDX_SHFT          0

/*bit mask to know that  new data is available for the DL channel  from FW*/
#define      E_DL_CHAN_INFO_AVAIL_BMSK              0x8000
#define      E_DL_CHAN_INFO_AVAIL_SHFT              15
#define      E_DL_CHAN_SUB_FN_BMSK                  0x07FF

/*bit mask for AG data returned by FW*/
/* AG value field bit mask*/
#define      AGCH_AG_VAL_BMSK                       0x003E
#define      AGCH_AG_VAL_SHFT                     1
/* AG scope bit mask and shift value*/
#define      AGCH_AG_SCOPE_BMSK                     0x0001

/* AG primary indentity bit mask and shift value*/
#define      AGCH_AG_ID_PRIMARY_BMSK                0x0080
#define      AGCH_AG_ID_PRIMARY_SHFT                7
/* AG secondary indentity bit mask and shift value*/
#define      AGCH_AG_ID_SECONDARY_BMSK              0x0040
#define      AGCH_AG_ID_SECONDARY_SHFT              6

/*These defines the bit mask and shift to read the bit fields from the 
** MDSP results buffer for HICH and RGCH.*/
#define E_DL_MAX_AGCH_RES_BUFFS       2

#define E_DL_CHAN_MAX_RES_BUFFS       8
#define E_DL_CHAN_DIRTY_BIT_BMSK           0x8000
#define E_DL_CHAN_DIRTY_BIT_SHIFT          15
#define E_DL_CHAN_BUF_IDX_BMSK              0x1800
#define E_DL_CHAN_BUF_IDX_SHIFT             11
#define E_DL_CHAN_SUBFN_BMSK                  0x07FF
#define E_DL_CHAN_DIRTY_BIT_CLEAR_BMSK   0x7FFF

/* Give the number of slots that is used for accumulating energy for Serving RG*/
#define NUM_SLOTS_HICH_S_RG_DECODE              12

/* Give the number of slots that is used for accumulating energy for Non-serving RG*/
#define NUM_SLOTS_NS_RG_DECODE                  15

/* Give the number of slots that is used for accumulating energy for Serving RG*/
#define NUM_SLOTS_HICH_S_RG_DECODE_2MS              3

/* This is the minimum number of slots that should be used for threshold scaling.
** the num_slots are set to this value if the value is lesser than 3.*/
#define MIN_NUM_SLOTS_FOR_SCALING      3

/*This is a serving Flag if set to one indicates that the RGCH/HICH belongs 
  to the serving cell
*/
#define      EUL_SRV_CELL_FLAG_BMSK                 0x4000
#define      EUL_SRV_CELL_FLAG_SHFT                 14

/*HICH data returned by FW*/
/*The result after decoding the HICH for each cell in active set*/
#define      HICH_RESULT_VAL_BMSK                   0x0018
#define      HICH_RESULT_VAL_SHFT                   3
/*Each of the HICH decoded results are indicated with their resp RLS indices
*/
#define      HICH_RLS_IDX_BMSK                      0x0007

/*RGCH data returned by FW*/
/*The result after decoding the HICH for each cell in active set*/
#define      RGCH_RESULT_VAL_BMSK                   0x0018
#define      RGCH_RESULT_VAL_SHFT                   3
/*Each of the RGCH decoded results are indicated with resp RG comb indices
*/
#define      RGCH_RG_COMB_IDX_BMSK                  0x0007


#ifdef FEATURE_WCDMA_REL6
/*mDSP async txPwrCtrlParam  db size without MTPL values  FW HLD-2.1.1.4*/
#define   MDSP_TX_PWRCTRL_PARAM_LEN                    7
/*mDSP async txPwrCtrlParam MTPL-only db size  FW HLD-2.1.1.4*/
#define   MDSP_TX_PWRCTRL_MTPL_PARAM_LEN               4
#else  /* FEATURE_WCDMA_REL6 */
/*mDSP async txPwrCtrlParam  db size   FW HLD-2.1.1.4*/
#define   MDSP_TX_PWRCTRL_PARAM_LEN                    11
#endif  /* FEATURE_WCDMA_REL6 */ 

/* These bits indicate Gain factor for DPCCH channel. bits 0-15 in row 0*/
#define TX_PWRCTRL_PARAM_DB_BETAC_BMSK               0xFFFF
#define TX_PWRCTRL_PARAM_DB_BETAC_REG                0
#define TX_PWRCTRL_PARAM_DB_BETAC_SHFT               0

/* These bits indicate Gain factor for DPDCH channel.bits 0-15 in row 1*/
#define TX_PWRCTRL_PARAM_DB_BETAD_BMSK               0xFFFF
#define TX_PWRCTRL_PARAM_DB_BETAD_REG                1
#define TX_PWRCTRL_PARAM_DB_BETAD_SHFT               0

/* These bits indicate Gain factors for HS-DPCCH channel. bits 0-15 in row 2*/
#define TX_PWRCTRL_PARAM_DB_BETAHS_DTX_BMSK          0xFFFF
#define TX_PWRCTRL_PARAM_DB_BETAHS_DTX_REG           2
#define TX_PWRCTRL_PARAM_DB_BETAHS_DTX_SHFT          0

/* These bits indicate Gain factors for HS-DPCCH channel. bits 0-15 in row 3*/
#define TX_PWRCTRL_PARAM_DB_BETAHS_ACK_BMSK          0xFFFF
#define TX_PWRCTRL_PARAM_DB_BETAHS_ACK_REG           3
#define TX_PWRCTRL_PARAM_DB_BETAHS_ACK_SHFT          0

/* These bits indicate Gain factors for HS-DPCCH channel. bits 0-15 in row 4*/
#define TX_PWRCTRL_PARAM_DB_BETAHS_NACK_BMSK         0xFFFF
#define TX_PWRCTRL_PARAM_DB_BETAHS_NACK_REG          4
#define TX_PWRCTRL_PARAM_DB_BETAHS_NACK_SHFT         0

/* These bits indicate Gain factors for HS-DPCCH channel. bits 0-15 in row 5*/
#define TX_PWRCTRL_PARAM_DB_BETAHS_CQI_BMSK          0xFFFF
#define TX_PWRCTRL_PARAM_DB_BETAHS_CQI_REG           5
#define TX_PWRCTRL_PARAM_DB_BETAHS_CQI_SHFT          0

/* These bits indicate DPCCH Initial Power+CM_delta. bits 0-15 in row 6*/
#define TX_PWRCTRL_PARAM_DB_DIP_CM_DELTA_BMSK          0xFFFF
#define TX_PWRCTRL_PARAM_DB_DIP_CM_DELTA_REG           6
#define TX_PWRCTRL_PARAM_DB_DIP_CM_DELTA_SHFT          0

/* These bits indicate Max TX Power Limit value with power back off.  
   bits 0-15 in row 7
*/
#define TX_PWRCTRL_PARAM_DB_MTPL_DTX_BMSK          0xFFFF
#define TX_PWRCTRL_PARAM_DB_MTPL_DTX_REG           7
#define TX_PWRCTRL_PARAM_DB_MTPL_DTX_SHFT          0

/* These bits indicateMax TX Power Limit value with power back off.
   bits 0-15 in row 8
*/
#define TX_PWRCTRL_PARAM_DB_MTPL_ACK_BMSK          0xFFFF
#define TX_PWRCTRL_PARAM_DB_MTPL_ACK_REG           8
#define TX_PWRCTRL_PARAM_DB_MTPL_ACK_SHFT          0

/* These bits indicate Max TX Power Limit value with power back off.
   bits 0-15 in row 9
*/
#define TX_PWRCTRL_PARAM_DB_MTPL_NACK_BMSK         0xFFFF
#define TX_PWRCTRL_PARAM_DB_MTPL_NACK_REG          9
#define TX_PWRCTRL_PARAM_DB_MTPL_NACK_SHFT         0

/* These bits indicate Max TX Power Limit value with power back off.
   bits 0-15 in row 10
*/
#define TX_PWRCTRL_PARAM_DB_MTPL_CQI_BMSK          0xFFFF
#define TX_PWRCTRL_PARAM_DB_MTPL_CQI_REG           10
#define TX_PWRCTRL_PARAM_DB_MTPL_CQI_SHFT          0

/* These bits indicate Max TX Power Limit value with no MPR.
   bits 0-15 in row 10
*/
#define TX_PWRCTRL_PARAM_DB_MTPL_NO_MPR_BMSK       0xFFFF
#define TX_PWRCTRL_PARAM_DB_MTPL_NO_MPR_REG        11
#define TX_PWRCTRL_PARAM_DB_MTPL_NO_MPR_SHFT       0

/*macro for filling the shadow buffer with the config db values*/
#define MDSP_SET_BUF_VAL(buffer, offset, shift, mask, val) \
  buffer[offset] &= (~((uint16)(mask))); \
  buffer[offset] |= (((val) << (shift)) & (mask))
  
/* macro for deciding the bit position ,location in shadow buffer for 
EDPDCH async param */
#define TX_PWRCTRL_PARAM_DB_SET_PARAM(buffer, param, val) \
  MDSP_SET_BUF_VAL(buffer, \
                 TX_PWRCTRL_PARAM_DB_##param##_REG, \
                 TX_PWRCTRL_PARAM_DB_##param##_SHFT, \
                 TX_PWRCTRL_PARAM_DB_##param##_BMSK, \
                 val)
                 
/* macro for deciding the bit position ,location in shadow buffer for 
 * EDPDCH async mtpl param */
#define TX_PWRCTRL_MTPL_PARAM_DB_SET_PARAM(buffer, param, val) \
  MDSP_SET_BUF_VAL(buffer, \
                 TX_PWRCTRL_PARAM_DB_##param##_REG - TX_PWRCTRL_PARAM_DB_MTPL_DTX_REG, \
                 TX_PWRCTRL_PARAM_DB_##param##_SHFT, \
                 TX_PWRCTRL_PARAM_DB_##param##_BMSK, \
                 val)                 

/*This ENUM is used to differentiate between the function calls for HICH and RGCH.
** DO NOT change this ENUM as it is being used to loop through and read the 
** HICH and RGCH buffers in mdspasync.c*/
typedef enum
{
  HICH_BUF_READ = 0,
  RGCH_BUF_READ = 1,
  HICH_RGCH_BUF_READ_END = 2
}mdspasync_hich_rgch_res_read_enum_type;

/* beta table-Gain factor for  different channel FW HLD-2.1.1.4-Figure 2 13 */
typedef struct
{
  #ifdef FEATURE_WCDMA_REL6 
  /* The index of the double-buffered TxPwrCtrlBuffer - 0 or 1 */
  uint16         tx_pwr_buf_idx;
  #endif  /* FEATURE_WCDMA_REL6 */
  /* Gain factor for DPCCH channel. This value will be scaled in the FW before 
   * programming to the HW*/
  uint16         beta_c;
 
  /* Gain factor for DPDCH channel. This value will be scaled in the FW before 
   * programming to the HW*/
  uint16         beta_d;

  /* Gain factors for HS-DPCCH channel. This value will be scaled in FW before
   * programming to the HW */
  uint16         beta_hs_dtx;
  uint16         beta_hs_cqi;
  uint16         beta_hs_nack;
  uint16         beta_hs_ack;
  /*MIMO HS Types*/
  uint16         beta_hs_ack_ack;
  uint16         beta_hs_ack_nack;
  uint16         beta_hs_nack_ack;
  uint16         beta_hs_nack_nack;
  uint16         beta_hs_delta_cqi;
  
  /* DPCCH Initial Power PLUS CM_delat, in unit of dB12 */
  int16          dip_cm_delta;

  /* MTPL with back off for known 4 HS cases */
  uint16         mpr_bin_cqi;
  uint16         mpr_bin_dtx;
  uint16         mpr_bin_ack;
  uint16         mpr_bin_nack;

  /* MPR with backoff for 5 additional MIMO HS cases */
  uint16         mpr_bin_ack_ack;
  uint16         mpr_bin_ack_nack;
  uint16         mpr_bin_nack_ack;
  uint16         mpr_bin_nack_nack;
  uint16         mpr_bin_delta_cqi;
  
  /* In REL-6, mtpl_dtx may contain MPR. 
   * This mtpl has no MPR */
  uint16         mtpl_no_mpr;
} mdspasync_ul_tx_pwr_gain_factors_struct_type;


/* edpcch async params- every TTI,the EDPCCH params are written after receiving
   the feedback information via the RGCH/AGCH channels.for the current HARQ 
   FW HLD-2.1.1.4-Figure 2 13 */
typedef struct
{
  /* the happy bit info- 25.212-670  4.9.2*/
  boolean     happy_bit;
  
  /* Retransmission sequence number(RSN) is sent to NodeB to indicate redundancy 
   * version of each HARQ transmission and to assist the Node B soft buffer 
   * management.  If signaled by higher layers for UE to use RV index 0,   
   * then  RSN is programmed to 0 by SW */
  uint8       rsn;

  /* The E-TFCI used-25.212-670  4.9.2 */
  uint16      e_tfci;
} mdspasync_edpcch_param_struct_type;


/* EDPCH async param db for every TTI, the EDPCH params are written after
   receiving the feedback information via the RGCH/AGCH/HICH channels.for 
   the current HARQ FW HLD-2.1.1.4-Figure 2 13 */
typedef struct
{
  /* 10 banks available in the HW that can be used to store the PDU. SW 
   * manages the HARQ processes and indicates to FW the bank it will
   * write the data to.*/
  uint8                     eram_bank_idx;

  /*Number of transmission slots in the first transmission*/
  uint16                    n_tx1;

  /* Number of pre rate matching bits which is equal to 3*Nsys */
  uint32                    n_ej;

  /* FW needs this information to calculate the RM parameters-this is Ne,data,j */
  uint16                    ne_data_j;

  /* edpcch async param structure */
  mdspasync_edpcch_param_struct_type  edpcch_params;

  /* Whether EDPDCH_1 is on I or Q branch
   * TX_EUL_CONFIG_EDPDCH in HW interface */
  uint8                     edpdch_iq_map;

  /* Whether EDPDCH channels 3 & 4 shall be enable in HW
   * TX_EUL_CONFIG_EDPDCH in HW interface */
  boolean                   edpdch_chan_3_4_enable;

  /* the number of SF 2 E-DPDCH */
  uint8                     num_sf2_edpdch;

  /* Number of SF-4 E-DPDCH channels */
  uint8                     num_sf4_edpdch;

  /*a bit to indicate to FW interface whether UL DPDCH is configured or not
  */
  boolean                   dpdch_cfged;

  /* The SF of the E-DPDCH channels. */
  uint8                     edpdch_sf[E_UL_MAX_NUM_EDPDCH];

  /* OVSF code number for the E-DPDCH channels */
  uint16                    edpdch_ovsf[E_UL_MAX_NUM_EDPDCH];

  /* a dirty bit for each channel that indicates to FW if there is a valid 
     information for that channel 
  */
  boolean                   dirty_bit[E_UL_MAX_NUM_EDPDCH];

  /* Gain factor for E-DPCCH channel. This value will be scaled in  FW before
   * programming to the HW.  BetaEc = 0 means DTX on E-DPCCH.*/
  uint16         beta_ec;

  /* Gain factor for E-DPDCH channel. This value will be scaled in  FW before
     programming to the HW.*/
  uint16         beta_ed;

  /* Gain factor for E-DPDCH channels with SF2. This value will be scaled in 
     the FW before programming to the HW.*/
  uint16         beta_ed_sf2;

  /* Modulation scheme for this TTI: 0-QPSK, 1-16QAM */
  uint8          mod_type;
} mdspasync_edpch_param_db_struct_type;

/* Structure to update the CM info for FW */
typedef struct
{
  boolean info_present;
  uint16  cur_cfn_bitmask_idx;
  uint16  cur_cfn_bitmask;
  uint16  cur_cfn_retain_bits_bitmask;
  uint16  next_cfn_bitmask;
  uint16  next_cfn_retain_bits_bitmask;
} mdspasync_edl_cm_ch_bitmask_info;

typedef struct
{
  mdspasync_edl_cm_ch_bitmask_info agch_cm_bitmask_info;
  mdspasync_edl_cm_ch_bitmask_info hich_cm_bitmask_info[WL1_MAX_UL_CARR_IDX][EUL_MAX_RL];
  mdspasync_edl_cm_ch_bitmask_info rgch_cm_bitmask_info[WL1_MAX_UL_CARR_IDX][EUL_MAX_RL];
} mdspasync_edl_cm_bitmask_info;

extern uint16 agchRdIdx; 
extern uint16 hichRdIdx; 
extern uint16 rgchRdIdx;

/*===========================================================================

                     FUNCTION DECLARATIONS

===========================================================================*/

extern void mdspasync_read_agch_results (uint32 curr_sub_fn);
extern void mdspasync_read_hich_rgch_results( uint32 curr_sub_fn);

/*===========================================================================

FUNCTION        MDSPASYNC_WRITE_TX_PWR_PARAMS

DESCRIPTION     This function writes Bc, Bd, Bhs for DTX/ACK/NACK/CQI,
                MTPL with back off, and DIP+CM_dleta to FW. It also sets
                Bec and Bed to 0.This function copies DPCH and HSDPA power
                control gain factors  info  to tx pwrctrl async param buffer 
                shadow  and then copies the ul DPCH and HSDPA tx pwrctrl param
                shadow to mdsp txpwrctrl async param buffer

DEPENDENCIES    None

RETURN VALUE    TRUE if all goes well, FALSE if there is any error

SIDE EFFECTS    The mDSP tx pwrctrl async param data will be sent to the mDSP

===========================================================================*/
extern boolean mdspasync_write_tx_pwr_params
(
  /* pointer to  tx pwr ctrl  async param */
  mdspasync_ul_tx_pwr_gain_factors_struct_type* tx_pwr_param_ptr  
);

#ifdef FEATURE_WCDMA_REL6
/*===========================================================================

FUNCTION        MDSPASYNC_WRITE_TX_MTPL_VALUES

DESCRIPTION     This function writes MTPL with back off to FW. This function 
                writes MTPL values for DTX, ACK, NACK, CQI to 
                tx pwrctrl async param buffer shadow at the appropriate 
                position and then copies this shadow to mdsp txpwrctrl async param buffer

DEPENDENCIES    None

RETURN VALUE    TRUE if all goes well, FALSE if there is any error

SIDE EFFECTS    The mDSP tx pwrctrl async param MTPL data will be sent to the mDSP

===========================================================================*/
extern boolean mdspasync_write_tx_mtpl_values
(
  /* pointer to  tx pwr ctrl async param */
  mdspasync_ul_tx_pwr_gain_factors_struct_type* tx_pwr_param_ptr  
);
#endif /* FEATURE_WCDMA_REL6 */

/*===========================================================================
FUNCTION        MDSPASYNC_WRITE_EDPCH_PARAM

DESCRIPTION     This function is called by EDPCH FIQ that processes UL E-DCH
                data to write EDPCH control parameters to MDSP.This function
                copies edpch info from e_ul_param_db to EUL edpch async param
                buffer shadow and then copies the Eul edpch param shadow to
                mdsp eul edpch async param buffer.

DEPENDENCIES    Called by EDPCH module in FIQ

RETURN VALUE    TRUE if all goes well, FALSE if there is any error

SIDE EFFECTS    The mDSP Eul edpch async param data will be sent to the mDSP.
===========================================================================*/
extern boolean mdspasync_write_edpch_param(mdspasync_edpch_param_db_struct_type** edpch_param_ptr, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION        MDSPASYNC_INIT_E_DL_CM_SLOT_BITMASKS

DESCRIPTION     This function initializes the EUL DL CM slot bitmasks for 
                particualar channels to 0. Which channels to init the slot
                bitmask of is indicated by the passed in flag for AGCH, the
                channel bitmask for HICH and RGCH

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The mDSP EUL DL CM slot bitmasks for particualar channels
                cleared
===========================================================================*/
void mdspasync_init_e_dl_cm_slot_bitmasks (boolean init_agch, 
                                           uint16 init_hich_ch_bmask,
                                           uint16 init_rgch_ch_bmask);

/*===========================================================================
FUNCTION        MDSPASYNC_WRITE_E_DL_CM_SLOT_BITMASKS

DESCRIPTION     This function updates the EUL DL CM slot bitmasks for 
                particualar channels correpsonding to the CM gaps in the 
                next DPCH frame.
                Since EUL DL channels will mostly be not frame aligned with
                DPCH, both bitmask0 and bitmask1 will get updated by SW here
                since part of each bitmask will fall within the next DPCH 
                frame.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The mDSP EUL DL CM slot bitmasks for particualar channels
                updated
===========================================================================*/
void mdspasync_write_e_dl_cm_slot_bitmasks (mdspasync_edl_cm_bitmask_info * bitmask_buffer, uint8 num_carr);

/*===========================================================================

FUNCTION        MDSPASYNC_READ_EUL_DL_CHAN_RESULTS

DESCRIPTION     This function gets the downlink channels AGCH/RGCH/HICH decoded
                results from asynchronous read buffer in the FW. FW will fill 
                this buffer  before sending FIQ to SW.

DEPENDENCIES    This function should be called only when FIQ is raised

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void mdspasync_read_eul_dl_chan_results(uint32 curr_sub_fn);
/*===========================================================================

FUNCTION        MDSPASYNC_FLUSH_RES_BUFFERS_TTI_CHANGE

DESCRIPTION     This function flushes the contents in the result buffer during
                a TTI reconfiguration. This is needed because some residual results
                from the previous TTI are left over.

DEPENDENCIES    This function should be called only during a TTI change.

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void mdspasync_flush_result_buffers_tti_change(void);

/*===========================================================================
FUNCTION        MDSPASYNC_WRITE_EDPCH_CM_PARAM

DESCRIPTION     This function is called by EDPCH FIQ that processes UL E-DCH
                data to write EDPCH CM parameters to MDSP..

DEPENDENCIES    Called by EDPCH module in FIQ

RETURN VALUE    TRUE if all goes well, FALSE if there is any error

SIDE EFFECTS    The mDSP Eul edpch CM async param data will be sent to the mDSP.
===========================================================================*/
extern void mdspasync_write_edpch_cm_param(uint16 edch_cm_dtx_slots, 
                                             uint16 pseudo_edch_cm_dtx_slots);


/*===========================================================================
FUNCTION        MDSPASYNC_UPDATE_WFW_DEBUG_CMD

DESCRIPTION     This function is called by wcdmadiag to update async debug cmd
                info

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void mdspasync_update_wfw_debug_cmd(
  uint32 *debug_info);

#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================
FUNCTION        mdspasync_init_eul_dl_result_read_idx

DESCRIPTION     

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    Updating the global variables that store the SW read index
         for the EUL DL result buffers. These variables are set to the Wr index where the FW
         is pointing...at the point of resume.
===========================================================================*/
void mdspasync_init_eul_dl_result_read_idx(void);

#endif /*FEATURE_WCDMA_HS_RACH*/

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/*===========================================================================
FUNCTION       WL1_INIT_UL_QTA_VARS

DESCRIPTION    This funciton will reset all the QTA related EUL variables
  
DEPENDENCIES   None
 
RETURN VALUE   Will reset all the QTA related variables

SIDE EFFECTS   None
===========================================================================*/
extern void wl1_init_e_ul_qta_vars(boolean skip_eul_qta_init);
#endif

#endif /*end of MDSP_ASYNC_H*/
