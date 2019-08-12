#ifndef MCALWCDMA_DECI_H
#define MCALWCDMA_DECI_H

/*============================================================================*/
/** @file  
 * This module contains definition declaration for MCAL (modem core abstraction
 * layer) WCDMA decoder module. This header filer is only intended to be included
 * in MCAL WCDMA decoder related modules for internal use only.
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/mcalwcdma_deci.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/16/15    ap      Initial check-in for FR 30856
02/24/15    svh     Enable HS decob check only on TA.
01/09/15    svh     Added check to catch HS decode done processing pending
                    for more than 2 TTIs.
07/24/14    hdk     Jalokia DRM buffer changes.
07/23/14    hdk     Resolving compilation error by removing def of REGISTER64,MY_DWORD, MY_WORD.
05/22/14    tsk     Featurization for JOLOKIA 1.0
05/15/14    ar      Changes for Thor compilation
05/09/14    vs      0x41BA version2 changes for DSDx.
12/04/13    ac      Add LVA non-first traceback and skip decode flags
10/30/13    bs      Extended TSN support added for 3C
09/09/13    ac      Added TDEC DOB start address
04/04/13    vvv     Added support to send decode health to Layer2
03/16/13    ms      Added support for two SCCPCH DRMB segments to support
                    two concurrent SCCPCH
03/14/13    rc      Updated zsb bit mask.
03/05/13    jhl     Cleanup F8 related code
02/20/13    jhl     Invoke A2 DL PHY commit after writing 5 DMA tasks
09/27/12    jhl     Added extern delcaration of function dl_cctrch_enabled to 
                    clear compiler issue
08/28/12    vs      Removed the api support for mcalwcdma_decdata_a2_invalidate_dcache
08/16/12    vsr     MBMS CCTRCH cleanup
08/10/12    pr      Dime Bringup Changes
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/12/12    pr      Changes for Dime Bringup by Madhu/Varsha
05/23/12    rgn     Changed DEC_MAX_DOB_SIZE to VD & TD specific sizes
05/07/12    vs      Introduced a new API mcalwcdma_decdata_a2_invalidate_dcache
06/11/12    vs      MBMS featue cleanup.
05/03/12    vsr     FET/DED/ED-V2 Feature checkin
03/16/12    ms      Added support for FEATURE_WCDMA_HS_FACH
01/29/12    vs      feature cleanup.
12/07/11    ms      Renamed the macros defined for DRM buffer size
12/07/11    ms      Added support to configure DRMB size based on modem
                    capability
03/14/11    ms      Mainlined FEATURE_WCDMA_MDSP_INTERFACE_REV_6275
12/21/10    dm      Moved mutex initializations to wl1m while doing 
                    task initialization.
06/30/10    ms      Added last allocated cctrch idx constants
04/23/10    ms      Removed redundant declaration of mcalwcdma_dec_cfn_adjust_for_tti
04/09/10    ms      Cleaned up profiling related vars
03/01/10    mc      Include P2 header files under FEATURE_WCDMA_PP_HW_DL
02/05/10    ms      Taxis/Genesis code merge
11/16/09    ms      Corrected ciphering algo value for 8220
11/11/09    rgn     Expanded DRMB size
11/09/09    ms      Added A2 Downlink support
09/02/09    ms      Added support for fully re-using Large DSM items using
                    DUP mechanism.
07/20/09    sq      Added support for UMTS MOB build
04/24/09    mr      Fixed Compiler Warnings
04/16/09    ms      Added typedef for P2 HW reg shadow structure.
04/09/09    ms      Code cleanup.
04/02/09    ms      Added macros MCALWCDMA_DEC_UPDATE_SRC_DESC and
                    MCALWCDMA_DEC_UPDATE_DST_DESC to update DM descriptors.
04/01/09    ms      Set MCALWCDMA_MAX_NUM_TF_POOL and MCALWCDMA_MAX_NUM_TFCI_POOL
                    to WFW defines.
03/12/09    vc      increased the DOB size to 32720 from 22220.
03/06/09    ms      Removed code under FEATURE_WCDMA_L1_DEC_BURST_READ.
03/02/09    ms      Removed SW F8 profiling variables.
02/27/09    mc      Added code to update HS DOB FIFO index after DM done
02/08/09    mc      Added debug profiling points for DMOV and F8
12/13/08    ms      Added extern declarations for data structures shared
                    between mcalwcdma_dec.c and mcalwcdma_decdata.c
12/01/08    mc      Changed IMEM base to 0x58004000.
11/09/08    ms      Removed unused macro DEC_DOB_EXTENSION_BUF_SIZE
                    Set MCALWCDMA_DEC_UMTS_DL_HW_TASK_Q_SIZE to 13696 bytes.
11/01/08    ms      Corrected HS DRMB segment index to 6.
10/03/08    hk      Bringing in 7k Mailine fixes
08/08/08    ms      Using 0x58010000 as the base address for P2 tasks
07/21/08    ms      PP related fixes
06/20/08    ms      Featurization cleanup.
                    Added support for Snow3G deciphering algorithm.
06/19/08    ms      Code cleanup.
                    Replaced FEATURE_WCDMA_REL7_L2_IMPROVEMENT with 
                    FEATURE_HSDPA_MAC_EHS.
06/18/08    ms      Removed occurences of FEATURE_WCDMA_HSPA_DEC.
                    Defined IMEM_L1_BASE to be 0x58010000.
                    Updated P2 Task Q related Macros.
                    Added prototype for new function
                    mcalwcdma_dec_write_nop_task_to_mem_and_align_wr_ptr.
06/12/08    ms      Increased P2 UMTS DL HW TaskQ size
06/10/08    ms      IMEM support.
                    Function names are changed.
06/27/07    mc      Added support for burst read
06/18/07    rgn     Added support for enhanced DL
04/24/07    mc      Lint fixes
04/05/07    mc      Support for MBMS TrCh enable/disable
03/30/07    mc      Support for TFCI log packet
03/03/07    mc      Sync up for MBMS phase2A integration
26/01/07    mc      added code to suport rm changes for 80msec TTI
12/12/06    mc      Synced up for MBMS demo - 3042 baseline
11/29/06    mc      Branched for MBMS demo.
09/28/06    mc      Added to //depot/asic/msmshared/umts/wcdma/l1/mcal/
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "queue.h"
#include "mcalwcdma_dec.h"
#include "a2_common.h"
#include "dldec_common.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Maximum number of A2 DL PHY tasks written to A2 after which L1 
   needs to commit the tasks */
#define MCALWCDMA_DEC_MAX_A2_TASK_FOR_COMMIT 5

/* Internal implementation of CEIL function be used in constants */
#define DEC_CEIL(x,y) (((x/y)*y)==x)?(x/y):((x/y)+1)

/*     CCTrCh header info       */    
/* ---------------------------- */

/* Number of 32 bit word in mDSP CCTrCh header entry */
#define DEC_MDSP_NUM_DWORD_CCTRCH_HDR     1

/* Invalid index. Used as general purpose indicator */
#define MCALWCDMA_DEC_IDX_INVALID 0xFF

/* Transport channel table info */    
/* ---------------------------- */

/* Number of 32 bit word in mDSP transport channel table entry */
#define DEC_MDSP_NUM_DWORD_TRCHT_ENTRY    1
/* Number of 32 bit word in mDSP transport channel table */
#define DEC_MDSP_NUM_MAX_DWORD_TRCHT \
 (MCALWCDMA_DEC_TOTAL_TRCHT_PER_CCTRCH * DEC_MDSP_NUM_DWORD_TRCHT_ENTRY)

/* TTI order table info */    
/* -------------------- */

/* Number of bits in a TTI order entry */
#define DEC_MDSP_NUM_BITS_TTI_ODR_ENTRY 8
/* number of word32 in TTI order entries */
#define DEC_MDSP_NUM_MAX_DWORD_TTI_ODRT \
 ((MCALWCDMA_DEC_TOTAL_TRCHT_PER_CCTRCH * DEC_MDSP_NUM_BITS_TTI_ODR_ENTRY)/32)
/* Bitmask for 8-bit TTI Order entry */
#define MCALWCDMA_DEC_TTI_ODR_ENTRY_BMASK   0xFF

/*      TF table info      */    
/* ----------------------- */

/* Total number of TF table in mDSP */
#define DEC_TOTAL_TFT_PER_CCTRCH 64
/* Number of 32 bit word in mDSP transport format table entry */
#define DEC_MDSP_NUM_DWORD_TFT_ENTRY 6
/* Maximum items in the TF pool */
#define MCALWCDMA_MAX_NUM_TF_POOL   WFW_DBACK_NUM_TF_POOL_TABLE_ENTRIES
/* Invalid TF pool index */
#define MCALWCDMA_INVALID_TF_POOL   0xFFFF

/* Number of bits to store one TF soft link. There can be 64 TF per non-MBMS 
   CCTrCh and a total of 64 TF for all MBMS S-CCPCH carrying MTCH. thus the 
   toal number of TF in use in steady state can be 128.
   The TF pool is of 256 elements so it requires 8 bits to access any of the 256 
   elements  */ 
#define MCALWCDMA_DEC_BITS_PER_TF         8
/* Number of bits of TF pool info in one W32 */
#define MCALWCDMA_NUM_BITS_TF_PER_W32     32
/* Number of 32 bit word in mDSP transport format table for store 64 TF .
   One W32 can store 4 TF so a total of 16 W32 are needed for 64 TF */
#define DEC_MDSP_NUM_MAX_DWORD_TFT   \
  (DEC_TOTAL_TFT_PER_CCTRCH/(MCALWCDMA_NUM_BITS_TF_PER_W32/MCALWCDMA_DEC_BITS_PER_TF))


/*     TFCI table info     */    
/* ----------------------- */

/* Number of 32 bit word in mDSP TFCI table entry */
#define DEC_MDSP_NUM_DWORD_TFCIT_ENTRY 3
/* Number of TFCI entries in CCTrCh table */
#define DEC_TOTAL_TFCI_PER_CCTRCH       128
/* Maximum items in the CTFC pool */
#define MCALWCDMA_MAX_NUM_TFCI_POOL   WFW_DBACK_NUM_TFCI_POOL_TABLE_ENTRIES
#define MCALWCDMA_INVALID_TFCI_POOL   0xFFFF

/* Number of bits to store one TFC soft link. Total number of TFC per CCTrCh is
   128 and the TFC pool is made of 288 elements so it requires 9 bits to access 
   any of the 288 elements  */ 
#define MCALWCDMA_DEC_BITS_PER_TFC        9
/* Number of bits used in one W32 to store TFCI pool indices 
   2 TFCI entries will be placed in one W32 so a total of 18 bits are used per W32 */
#define MCALWCDMA_NUM_BITS_TFCI_PER_W32  ((32/MCALWCDMA_DEC_BITS_PER_TFC) * MCALWCDMA_DEC_BITS_PER_TFC)
/* Total number of W32 to accomodate 128 TF. One W32 can store 2 TFCI and 14 bits 
   are unused */ 
#define DEC_MDSP_NUM_MAX_DWORD_TFCI_TABLE   \
  (DEC_CEIL(DEC_TOTAL_TFCI_PER_CCTRCH, (MCALWCDMA_NUM_BITS_TFCI_PER_W32/MCALWCDMA_DEC_BITS_PER_TFC)))

/*    CCTrCh table info    */
/* ----------------------- */

/* Number of 32 bit word in mDSP CCTrCh table */
/*lint -emacro(506,DEC_MDSP_NUM_MAX_DWORD_CCTRCH_TABLE) Max number of W32 needed to 
  store a CCTrCh table in mDSP */ 
#define DEC_MDSP_NUM_MAX_DWORD_CCTRCH_TABLE \
 (DEC_MDSP_NUM_DWORD_CCTRCH_HDR + \
  DEC_MDSP_NUM_MAX_DWORD_TTI_ODRT + \
  DEC_MDSP_NUM_MAX_DWORD_TRCHT + \
  DEC_MDSP_NUM_MAX_DWORD_TFT + \
  DEC_MDSP_NUM_MAX_DWORD_TFCI_TABLE) 

/* Non PCCPCH R99 first and last CCTrCh table indices */
#define MCALWCDMA_DEC_FIRST_NON_PCCPCH_TABLE_IDX 0
#define MCALWCDMA_DEC_LAST_NON_PCCPCH_TABLE_IDX  (WFW_DBACK_NUM_CCTRCH_TABLES - 1)

/* Number of words in PCCPCH CCTrCh */
#define DEC_MDSP_NUM_W16_PCCPCH_CCTRCH_TABLE    1
#define MCALWCDMA_DEC_FIRST_PCCPCH_TABLE_IDX    WFW_DBACK_PCCPCH_CCTRCH_ID_FIRST
#define MCALWCDMA_DEC_LAST_PCCPCH_TABLE_IDX     WFW_DBACK_PCCPCH_CCTRCH_ID_LAST

#define MCALWCDMA_DEC_R99_FACH_TRCH_TYPE   0x00
#define MCALWCDMA_DEC_MCCH_FACH_TRCH_TYPE  0x01
#define MCALWCDMA_DEC_MTCH_FACH_TRCH_TYPE  0x02
#define MCALWCDMA_DEC_MSCH_FACH_TRCH_TYPE  0x04
#define MCALWCDMA_DEC_NOT_FACH_TRCH_TYPE   0xFF


#define DEC_MIN_RATE_MATCH_ATTRIB 1
#define DEC_MAX_RATE_MATCH_ATTRIB 256
/* Maximum TF possible per TrCh (From RRC spec 25.331 2000-12 V3.5.0 Page 442,443) */
#define DEC_MAX_TF_PER_TRCH 32
/* maximum number of TB in one TF. This is set to limit of maximum number of
   TB that can be received from all TrCh in CCTrCh at end of frame */
#define DEC_MAX_TB_PER_TF 32
/* the max TrBlk size in bits */
#define DEC_MAX_TB_BIT_SIZE 6400

/* Minimum DSM item size in bytes */
#define DEC_MIN_DSM_ITEM_SIZE 48

/* Maximum possible DSM item chain pointers.
   The 4 is added to the TB size bytes. 1 for any fractions output
   of a byte after dividing by 8 and 3 for possible 3 extra word if
   there is non word32 align number */
#define DEC_MAX_DSM_ITEM_CHAIN_DEPTH ((((DEC_MAX_TB_BIT_SIZE)/8) + 4)/(DEC_MIN_DSM_ITEM_SIZE))
 
/* Number of bits required for BCH block
 * PCCPCH 246 bits + 16 bit CRC + 8 Tail bits = 270
 */
#define DEC_BCH_BLOCK_SIZE_IN_BITS 270

#ifdef FEATURE_WCDMA_JOLOKIA_MODEM
/*BCH DRM bits = 270*3, encoded at conv 1/3 rate*/
#define DEC_DRM_BUF_SIZE_BCH_BITS (3*DEC_BCH_BLOCK_SIZE_IN_BITS)
/* DRM buffer size required for BCH code block */
#define DEC_DRM_BUF_SIZE_BCH_BLOCK DEC_DRM_BUF_SEGMENT_SIZE(DEC_DRM_BUF_SIZE_BCH_BITS)
#else
  /* DRM buffer size required for BCH code block */
  #define DEC_DRM_BUF_SIZE_BCH_BLOCK DEC_DRM_BUF_SEGMENT_SIZE(DEC_BCH_BLOCK_SIZE_IN_BITS)
#endif

/* DRM buffer strorage direction macro */
#define DEC_DRM_BUF_ST_DIR_INC 1
#define DEC_DRM_BUF_ST_DIR_DEC 0

/* Total DRM buffer size of 1888 words in the non-concurrent mode */
#define DEC_DRM_BUFFER_SIZE 1888
#define DEC_DRM_BUF_SYMBOLS_PER_WORD WFW_DEMOD_DRMB_NUM_BANKS

#define DEC_DRM_BUF_SEGMENT_SIZE(size)  CEIL((size * 1), DEC_DRM_BUF_SYMBOLS_PER_WORD)

#define DEC_DRM_BUF_UNUSED              0xFFFF

#define DEC_DRM_BUF_HARQ_MEM_ST_DIR DEC_DRM_BUF_ST_DIR_DEC

#ifndef TEST_FRAMEWORK
#define IMEM_L1_BASE  0x58004000
#else
#error code not present
#endif

/* DRM buffer storage locations are their parameters */

/* Number of DRM segments for BCH type CCTrCh */
#ifdef FEATURE_WCDMA_JOLOKIA_MODEM
#define MCALWCDMA_DEC_CCTRCH_BCH_TYPE_NUM_DRM_INFO 4
#else
#define MCALWCDMA_DEC_CCTRCH_BCH_TYPE_NUM_DRM_INFO 8
#endif

/* Number of DRM segments for R99 type CCTrCh */
#define MCALWCDMA_DEC_CCTRCH_R99_TYPE_NUM_DRM_INFO 2

/* Number of areas in which DRM buffer is divided */
#define MCALWCDMA_DEC_NUM_DRM_BUF_AREA  \
  (MCALWCDMA_DEC_CCTRCH_R99_TYPE_NUM_DRM_INFO + \
   MCALWCDMA_DEC_CCTRCH_BCH_TYPE_NUM_DRM_INFO)

/* R99 CCTrCh uses DRM buffer parameter info starting
   at index 8 (may use index 9 if 8 is already allocated */
#ifdef FEATURE_WCDMA_JOLOKIA_MODEM
#define MCALWCDMA_DEC_CCTRCH_R99_TYPE_DRM_INFO_IDX 4
#else
#define MCALWCDMA_DEC_CCTRCH_R99_TYPE_DRM_INFO_IDX 8
#endif

/* TURBO BETA SELECT types */

#define DEC_TURBO_BETA_SELECT_NO_LUT 0
#define DEC_TURBO_BETA_SELECT_3      1
#define DEC_TURBO_BETA_SELECT_6      2
#define DEC_TURBO_BETA_SELECT_9      3
#define DEC_TURBO_BETA_SELECT_12     4

/* CCTrCh TrCh TF Bit field masks, shifts etc. */
/* ------------------------------------------- */

/* CCTrCh table headers */
/* -------------------- */

/* Dirty Bit: Bit 31 */
#define DEC_CCTRCHT_DIRTY_BIT_BMSK 0x80000000
#define DEC_CCTRCHT_DIRTY_BIT_REG  0
#define DEC_CCTRCHT_DIRTY_BIT_SHFT 31

/* DRM buffer start addr: Bit 30-16 */
#define DEC_CCTRCHT_DBACK_ST_ADDR_BMSK 0x7FFF0000
#define DEC_CCTRCHT_DBACK_ST_ADDR_REG  0
#define DEC_CCTRCHT_DBACK_ST_ADDR_SHFT 16

/* TFCI bit : bit 15 */
#define DEC_CCTRCHT_TFCI_BMSK 0x00008000
#define DEC_CCTRCHT_TFCI_REG  0
#define DEC_CCTRCHT_TFCI_SHFT 15

/* DRM buffer storage dir: Bit 14 */
#define DEC_CCTRCHT_STORAGE_DIR_BMSK 0x00004000
#define DEC_CCTRCHT_STORAGE_DIR_REG  0
#define DEC_CCTRCHT_STORAGE_DIR_SHFT 14

/* TFCI table index : bit 13 */
#define DEC_CCTRCHT_TFCIT_IDX_BMSK 0x00002000
#define DEC_CCTRCHT_TFCIT_IDX_REG  0
#define DEC_CCTRCHT_TFCIT_IDX_SHFT 13

/* MBMS Num: Bit 8-5 */
#define DEC_CCTRCHT_MBMS_NUM_BMSK 0x000001E0
#define DEC_CCTRCHT_MBMS_NUM_REG  0
#define DEC_CCTRCHT_MBMS_NUM_SHFT 5

/* Number of TrCh : bit 4-0 */
#define DEC_CCTRCHT_NUM_TRCH_BMSK 0x0000001F
#define DEC_CCTRCHT_NUM_TRCH_REG  0
#define DEC_CCTRCHT_NUM_TRCH_SHFT 0

/* Transport channel table */
/* ---------------------- */


/* TrCh enable/disable : bit 30 */
#define DEC_TRCHT_ENABLE_BMSK 0x40000000
#define DEC_TRCHT_ENABLE_REG  0
#define DEC_TRCHT_ENABLE_SHFT 30

/* max TF size index in the TrCh : bit 28-23 */
#define DEC_TRCHT_MAX_TFI_BMSK 0x1F800000
#define DEC_TRCHT_MAX_TFI_REG  0
#define DEC_TRCHT_MAX_TFI_SHFT 23

/* TTI of the TrCh : bit 22-21 */
#define DEC_TRCHT_TTI_BMSK 0x00600000
#define DEC_TRCHT_TTI_REG  0
#define DEC_TRCHT_TTI_SHFT 21

/* CRC type of the TrCh : bit 20-18 */
#define DEC_TRCHT_CRC_BMSK 0x001C0000
#define DEC_TRCHT_CRC_REG  0
#define DEC_TRCHT_CRC_SHFT 18

/* Code type of the TrCh : bit 17-16 */
#define DEC_TRCHT_CODE_TYPE_BMSK 0x00030000
#define DEC_TRCHT_CODE_TYPE_REG  0
#define DEC_TRCHT_CODE_TYPE_SHFT 16

/* Transport format table base of the TrCh : bit 15-10 */
#define DEC_TRCHT_TFT_BASE_BMSK 0x0000FC00
#define DEC_TRCHT_TFT_BASE_REG  0
#define DEC_TRCHT_TFT_BASE_SHFT 10

/* Number of transport formats in the TrCh */
#define DEC_TRCHT_NUM_TF_BMSK 0x0000007F
#define DEC_TRCHT_NUM_TF_REG  0
#define DEC_TRCHT_NUM_TF_SHFT 0


/* Number of transport formats in the TrCh : bit 6-0 */
#define DEC_TRCHT_NUM_TF_BMSK 0x0000007F
#define DEC_TRCHT_NUM_TF_REG  0
#define DEC_TRCHT_NUM_TF_SHFT 0

/* Transport format Pool table */
/* ---------------------- */

/* Rate matched size per radio frame : bit 29-16 */
#define DEC_TFT_RM_SIZE_PER_RD_FR_BMSK 0x3FFF0000
#define DEC_TFT_RM_SIZE_PER_RD_FR_REG  0
#define DEC_TFT_RM_SIZE_PER_RD_FR_SHFT 16

/* Number of transport blocks */
#define DEC_TFT_NUM_TRBLK_BMSK 0x0000FC00
#define DEC_TFT_NUM_TRBLK_REG  0
#define DEC_TFT_NUM_TRBLK_SHFT 10

/* Number of code blocks */
#define DEC_TFT_NUM_CODE_BLK_BMSK 0x0000003F
#define DEC_TFT_NUM_CODE_BLK_REG  0
#define DEC_TFT_NUM_CODE_BLK_SHFT 0

/* Code block size */
#define DEC_TFT_CODE_BLK_SIZE_BMSK 0x1FFF0000
#define DEC_TFT_CODE_BLK_SIZE_REG  1
#define DEC_TFT_CODE_BLK_SIZE_SHFT 16

/* Transport block size */
#define DEC_TFT_TRBLK_SIZE_BMSK 0x00001FFF
#define DEC_TFT_TRBLK_SIZE_REG  1
#define DEC_TFT_TRBLK_SIZE_SHFT 0

/* DRM scale factor */
#define DEC_TF_DRM_SCALE_FACTOR_BMSK 0xE0000000 
#define DEC_TF_DRM_SCALE_FACTOR_REG  2
#define DEC_TF_DRM_SCALE_FACTOR_SHFT 29

/* RM parameter ePlusOverA */
#define DEC_TFT_EPLUSOVERA_BMSK 0x000FFFFF
#define DEC_TFT_EPLUSOVERA_REG  2
#define DEC_TFT_EPLUSOVERA_SHFT 0

/* delta N Odd */
#define DEC_TF_DELTA_N_ODD_BMSK 0x80000000
#define DEC_TF_DELTA_N_ODD_REG  3
#define DEC_TF_DELTA_N_ODD_SHFT 31 

/* Number of filler bits */
#define DEC_TFT_NUM_FILLER_BITS_BMSK 0x7E000000
#define DEC_TFT_NUM_FILLER_BITS_REG  3
#define DEC_TFT_NUM_FILLER_BITS_SHFT 25

/* YAM threshold Id */
#define DEC_TFT_YAM_TH_ID_BMSK 0x01E00000
#define DEC_TFT_YAM_TH_ID_REG  3
#define DEC_TFT_YAM_TH_ID_SHFT 21

/* Rate matching type */
#define DEC_TFT_RM_TYPE_BMSK 0x00180000
#define DEC_TFT_RM_TYPE_REG  3
#define DEC_TFT_RM_TYPE_SHFT 19

/* RM paraneter eMinusOverA */
#define DEC_TFT_EMINUSOVERA_BMSK 0x0007FFFF
#define DEC_TFT_EMINUSOVERA_REG  3
#define DEC_TFT_EMINUSOVERA_SHFT 0

/* Turbo parameter: Last row bit exchange */
#define DEC_TFT_TP_LAST_ROW_BIT_EXG_BMSK 0x40000000
#define DEC_TFT_TP_LAST_ROW_BIT_EXG_REG  4
#define DEC_TFT_TP_LAST_ROW_BIT_EXG_SHFT 30

/* Turbo parameter: Num row index */
#define DEC_TFT_TP_NUM_ROW_IDX_BMSK 0x30000000
#define DEC_TFT_TP_NUM_ROW_IDX_REG  4
#define DEC_TFT_TP_NUM_ROW_IDX_SHFT 28

/* Turbo parameter: Num column */
#define DEC_TFT_TP_NUM_COL_BMSK 0x01FF0000
#define DEC_TFT_TP_NUM_COL_REG  4
#define DEC_TFT_TP_NUM_COL_SHFT 16

/* Turbo parameter: Prime number index */
#define DEC_TFT_TP_PRIME_NUM_IDX_BMSK 0x00003F00
#define DEC_TFT_TP_PRIME_NUM_IDX_REG  4
#define DEC_TFT_TP_PRIME_NUM_IDX_SHFT 8

/* Turbo parameter: MAX interation number */
#define DEC_TFT_TP_MAX_ITER_NUM_BMSK 0x0000000F
#define DEC_TFT_TP_MAX_ITER_NUM_REG  4
#define DEC_TFT_TP_MAX_ITER_NUM_SHFT 0

/* Turbo parameter: Turbo beta sel */
#define DEC_TFT_TP_TURBO_BETA_SEL_BMSK 0xF0000000
#define DEC_TFT_TP_TURBO_BETA_SEL_REG  5
#define DEC_TFT_TP_TURBO_BETA_SEL_SHFT 28

/* Turbo parameter: Prime number */
#define DEC_TFT_TP_PRIME_NUM_BMSK 0x01FF0000
#define DEC_TFT_TP_PRIME_NUM_REG  5
#define DEC_TFT_TP_PRIME_NUM_SHFT 16

/* Turbo parameter: Last row */
#define DEC_TFT_TP_LAST_ROW_BMSK 0x00001F00
#define DEC_TFT_TP_LAST_ROW_REG  5
#define DEC_TFT_TP_LAST_ROW_SHFT 8

/* Turbo parameter: Last column */
#define DEC_TFT_TP_LAST_COL_BMSK 0x000000FF
#define DEC_TFT_TP_LAST_COL_REG  5
#define DEC_TFT_TP_LAST_COL_SHFT 0

/* CTFC (or TFCI) table */
/* -------------------- */

/* TFCI entry valid bit */
#define DEC_TFCIT_VALID_BMSK 0x80000000
#define DEC_TFCIT_VALID_REG  0
#define DEC_TFCIT_VALID_SHFT 31

/* PDSCH spreading factor */
#define DEC_TFCIT_PDSCH_SF_BMSK 0x70000000
#define DEC_TFCIT_PDSCH_SF_REG  0
#define DEC_TFCIT_PDSCH_SF_SHFT 28

/* PDSCH OVSF code */
#define DEC_TFCIT_PDSCH_OVSF_CODE_BMSK 0xFF000000
#define DEC_TFCIT_PDSCH_OVSF_CODE_REG  1
#define DEC_TFCIT_PDSCH_OVSF_CODE_SHFT 24

/* TFI index 0 */
#define DEC_TFCIT_TFI0_BMSK 0x0000003F
#define DEC_TFCIT_TFI0_REG  0
#define DEC_TFCIT_TFI0_SHFT 0

/* TFI index 1 */
#define DEC_TFCIT_TFI1_BMSK 0x00000FC0
#define DEC_TFCIT_TFI1_REG  0
#define DEC_TFCIT_TFI1_SHFT 6

/* TFI index 2 */
#define DEC_TFCIT_TFI2_BMSK 0x0003F000
#define DEC_TFCIT_TFI2_REG  0
#define DEC_TFCIT_TFI2_SHFT 12

/* TFI index 3 */
#define DEC_TFCIT_TFI3_BMSK 0x00FC0000
#define DEC_TFCIT_TFI3_REG  0
#define DEC_TFCIT_TFI3_SHFT 18

/* TFI index 4 */
#define DEC_TFCIT_TFI4_BMSK 0x0000003F
#define DEC_TFCIT_TFI4_REG  1
#define DEC_TFCIT_TFI4_SHFT 0

/* TFI index 5 */
#define DEC_TFCIT_TFI5_BMSK 0x00000FC0
#define DEC_TFCIT_TFI5_REG  1
#define DEC_TFCIT_TFI5_SHFT 6

/* TFI index 6 */
#define DEC_TFCIT_TFI6_BMSK 0x0003F000
#define DEC_TFCIT_TFI6_REG  1
#define DEC_TFCIT_TFI6_SHFT 12

/* TFI index 7 */
#define DEC_TFCIT_TFI7_BMSK 0x00FC0000
#define DEC_TFCIT_TFI7_REG  1
#define DEC_TFCIT_TFI7_SHFT 18

/* TFI index 8 */
#define DEC_TFCIT_TFI8_BMSK 0x0000003F
#define DEC_TFCIT_TFI8_REG  2
#define DEC_TFCIT_TFI8_SHFT 0

/* TFI index 9 */
#define DEC_TFCIT_TFI9_BMSK 0x00000FC0
#define DEC_TFCIT_TFI9_REG  2
#define DEC_TFCIT_TFI9_SHFT 6

/* TFI index 10 */
#define DEC_TFCIT_TFI10_BMSK 0x0003F000
#define DEC_TFCIT_TFI10_REG  2
#define DEC_TFCIT_TFI10_SHFT 12

/* TFI index 11 */
#define DEC_TFCIT_TFI11_BMSK 0x00FC0000
#define DEC_TFCIT_TFI11_REG  2
#define DEC_TFCIT_TFI11_SHFT 18


/* PCCPCH CCTrCh bitmasks */
/* ---------------------- */

#define DEC_PCCPCH_CCTRCH_EVEN_ENABLE_BMSK 0x0001
#define DEC_PCCPCH_CCTRCH_EVEN_ENABLE_SHFT 0

#define DEC_PCCPCH_CCTRCH_EVEN_DRMB_IDX_BMSK 0x06
#define DEC_PCCPCH_CCTRCH_EVEN_DRMB_IDX_SHFT 1

#define DEC_PCCPCH_CCTRCH_ODD_ENABLE_BMSK 0x0008
#define DEC_PCCPCH_CCTRCH_ODD_ENABLE_SHFT 3

#define DEC_PCCPCH_CCTRCH_ODD_DRMB_IDX_BMSK 0x30
#define DEC_PCCPCH_CCTRCH_ODD_DRMB_IDX_SHFT 4


#define DEC_MDSP_SF_CFN_TTI_TYPE(cfn) \
  ((((cfn + 1) % 2) != 0) ? L1_TTI_10MS : \
   (((cfn + 1) % 4) != 0) ? L1_TTI_20MS : \
   (((cfn + 1) % 8) != 0) ? L1_TTI_40MS : L1_TTI_80MS)

/* Maximum decoder output buffer status FIFO entries */
#define DEC_MAX_NON_HS_STATUS_FIFO_COUNT  WFW_DBACK_NON_HS_DECOB_STATUS_FIFO_SIZE
#define DEC_MAX_HS_STATUS_FIFO_COUNT      WFW_DBACK_HS_DECOB_STATUS_FIFO_SIZE

#define DEC_MAX_STATUS_FIFO_COUNT DEC_MAX_NON_HS_STATUS_FIFO_COUNT + \
                                  DEC_MAX_HS_STATUS_FIFO_COUNT

/* Number of word 16 in a status FIFO entry */
#define DEC_NUM_WORD16_PER_FIFO_ENTRY 5

/* Number of MAC-d PDUs size per Q received in one HS sub frame */
#define DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT 8

/* Number of word 32 in a TB status */
#define DEC_NUM_WORD32_PER_TB_SET_STATUS_CONV_CODING  4
#define DEC_NUM_WORD32_PER_TB_SET_STATUS_TURBO_CODING 1

/* DOB buffer size */
#define DEC_MAX_VD_DOB_SIZE WFW_DEMOD_NON_HS_VD_DOB_MEM_SIZE 
#define DEC_MAX_TD_DOB_SIZE WFW_DEMOD_TD_DOB_MEM_SIZE 

/* Burst read address constants */
/* ---------------------------- */

/* DOB status item bit mask shifts */
/* ------------------------------- */

/* for viterbi decoder */
#define DEC_DOB_ST_VD_TB_CRC_BITS_BMSK 0xFFFFFFFF
#define DEC_DOB_ST_VD_TB_CRC_BITS_REG  0
#define DEC_DOB_ST_VD_TB_CRC_BITS_SHFT 0

#define DEC_DOB_ST_VD_TRCH_EGY_MAT_BMSK 0x0000FFFF
#define DEC_DOB_ST_VD_TRCH_EGY_MAT_REG  1
#define DEC_DOB_ST_VD_TRCH_EGY_MAT_SHFT 0

#define DEC_DOB_ST_VD_TRCH_SE_CNT_BMSK 0xFFFF0000
#define DEC_DOB_ST_VD_TRCH_SE_CNT_REG  2
#define DEC_DOB_ST_VD_TRCH_SE_CNT_SHFT 16

#define DEC_DOB_ST_VD_CB_Q_BITS_BMSK 0x0000FFFF
#define DEC_DOB_ST_VD_CB_Q_BITS_REG  2
#define DEC_DOB_ST_VD_CB_Q_BITS_SHFT 0

/* Last Best Metric */
#define DEC_DOB_ST_VD_TRCH_LBM_BMSK  0x00007F00
#define DEC_DOB_ST_VD_TRCH_LBM_REG   3
#define DEC_DOB_ST_VD_TRCH_LBM_SHFT  8

/* Last State Metric */
#define DEC_DOB_ST_VD_TRCH_LSM_BMSK  0x0000007F
#define DEC_DOB_ST_VD_TRCH_LSM_REG   3
#define DEC_DOB_ST_VD_TRCH_LSM_SHFT  0

/* VD energy metric 32 bit word */
#define DEC_DOB_ST_VD_TRCH_ZSB_BMSK  0xFFFFFFFF
#define DEC_DOB_ST_VD_TRCH_ZSB_REG   3
#define DEC_DOB_ST_VD_TRCH_ZSB_SHFT  0

/* for turbo decoder */
#define DEC_DOB_ST_TD_TB_CRC_BITS_BMSK 0xFFFFFFFF
#define DEC_DOB_ST_TD_TB_CRC_BITS_REG  0
#define DEC_DOB_ST_TD_TB_CRC_BITS_SHFT 0

/* for uncoded TrChs */
#define DEC_DOB_ST_UNC_TB_CRC_BITS_BMSK 0xFFFFFFFF
#define DEC_DOB_ST_UNC_TB_CRC_BITS_REG  0
#define DEC_DOB_ST_UNC_TB_CRC_BITS_SHFT 0

#define DEC_DOB_ST_UNC_TRCH_EGY_MAT_BMSK 0x0000FFFF
#define DEC_DOB_ST_UNC_TRCH_EGY_MAT_REG  1
#define DEC_DOB_ST_UNC_TRCH_EGY_MAT_SHFT 0

#define DOB_TB_SET_STATUS_ITEM(buffer, item, coding_type) \
  (((buffer)[DEC_DOB_ST_##coding_type##_##item##_REG] & \
    DEC_DOB_ST_##coding_type##_##item##_BMSK) >> \
   DEC_DOB_ST_##coding_type##_##item##_SHFT)
   
/* minumum TB energy matrix for TBset for Conv coded TrCh.
   If the egy matric value is lower than this CRC is forced to fail */
#define DEC_MIN_TB_EGY_MATRIC_THRESH 256

/* --------------------------------------------------------------------------- */
/*                              TFCI decode statistics                         */
/* --------------------------------------------------------------------------- */

/* Number of bytes in a single TFCI sample */
#define MDSP_NUM_BYTES_TFCI_ENTRY   8
/* There are two banks of TFCI: even and odd. Each bank has information for 
   10 CCTrCh. The details of the structure can be found in 
   mcalwcdma_dec_decoded_tfci_info_struct_type */
#define MDSP_DECODED_TFCI_EVEN  MDSP_DECODED_TFCI
/* To get to the odd address, add offset corresponding to 10 CCTrCh for even */
#define MDSP_DECODED_TFCI_ODD   (MDSP_DECODED_TFCI + \
   (MCALWCDMA_DEC_TOTAL_CCTRCH_TABLES_NON_BCH * MDSP_NUM_BYTES_TFCI_ENTRY))
/* Each CCTrCh buffers TFCI info for 4 frames in mDSP */
#define MCALWCDMA_DEC_NUM_TFCI_BUFFER_PER_CCTRCH    4
#define MCALWCDMA_DEC_NUM_TFCI_STATS_LOGPKT     2

#define MCALWCDMA_DEC_NUM_TBSET_LOGPKT  2
#define MCALWCDMA_DEC_INVALID_INDEX     0xFF


/* MAC-HS header fields */
/* -------------------- */

/* CRC length for HS-DSCH block */
#define DEC_HS_DSCH_BLK_CRC_LEN 24

/* Number of bits in varioys fields of MAC-hs header */   
#define DEC_HS_MAC_HS_HDR_VF_NUM_BITS         1 /* Version flag */
#define DEC_HS_MAC_HS_HDR_QID_NUM_BITS        3 /* Queue Id */
#define DEC_HS_MAC_HS_HDR_TSN_NUM_BITS        6 /* Transmission seq number */
#define DEC_HS_MAC_HS_HDR_SID_NUM_BITS        3 /* MAC-d PDU size indicator */
#define DEC_HS_MAC_HS_HDR_N_MACD_PDU_NUM_BITS 7 /* Number of MAC-d s*/
#define DEC_HS_MAC_HS_HDR_F_FLAG_NUM_BITS     1 /* Final flag bit */

/* Maximum possible size of MAC-hs header */   
#define DEC_HS_MAC_HS_HDR_LEN_MAX \
  ((DEC_HS_MAC_HS_HDR_VF_NUM_BITS + \
    DEC_HS_MAC_HS_HDR_QID_NUM_BITS + \
    DEC_HS_MAC_HS_HDR_TSN_NUM_BITS) + \
   ((DEC_HS_MAC_HS_HDR_SID_NUM_BITS + \
     DEC_HS_MAC_HS_HDR_N_MACD_PDU_NUM_BITS + \
     DEC_HS_MAC_HS_HDR_F_FLAG_NUM_BITS) * \
    MCALWCDMA_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT))

/* Number of word32 required to extract MAC-hs fields of maximum length */
/*lint -emacro(506,DEC_HS_DOB_W32_READ_FOR_MAC_HS_HEADER_DECODE) Max number of W32 needed to read
  MAC-HS header is dependent on various field lengths of the header */ 
#define DEC_HS_DOB_W32_READ_FOR_MAC_HS_HEADER_DECODE \
  ((DEC_HS_MAC_HS_HDR_LEN_MAX & 0x1F) ? \
   ((DEC_HS_MAC_HS_HDR_LEN_MAX >> 5) + 1) : \
   (DEC_HS_MAC_HS_HDR_LEN_MAX >> 5))
    
/* Macro to extract MAC-hs fields */  
#define DEC_HS_MAC_HDR_GET_FIELD(buf,offset_ptr,field) \
  EXTRACT_BITS_FROM_BUF(buf,offset_ptr, DEC_HS_MAC_HS_HDR_##field##_NUM_BITS)

#define DEC_HS_MAX_LOG_CHAN 16

/* Spec 25.321 Max possible Reordering PDUs per HS TrBlk */
#define MCALWCDMA_DEC_HS_REORDER_PDU_MAX_COUNT 3

#ifdef FEATURE_HSDPA_MAC_EHS
/* Number of bits in various fields of MAC-ehs header */   
#define DEC_HS_MAC_EHS_HDR_LC_ID_NUM_BITS         4 /* Logical Channel ID */
#define DEC_HS_MAC_EHS_HDR_LEN_NUM_BITS          11 /* Length of reordering SDU */
#define DEC_HS_MAC_EHS_HDR_TSN_NUM_BITS           6 /* Transmission seq number for reordering PDU */
#define DEC_HS_MAC_EHS_HDR_EXT_TSN_NUM_BITS      14 /* Ext Transmission seq number for reordering PDU */
#define DEC_HS_MAC_EHS_HDR_SI_NUM_BITS            2 /* Segmentation indicator for reordering PDU */
#define DEC_HS_MAC_EHS_HDR_F_FLAG_NUM_BITS        1 /* Final flag bit */

/* Maximum possible size of MAC-hs header */   
#define DEC_HS_MAC_EHS_HDR_LEN_MAX \
  ((DEC_HS_MAC_EHS_HDR_LC_ID_NUM_BITS + \
    DEC_HS_MAC_EHS_HDR_LEN_NUM_BITS + \
    DEC_HS_MAC_EHS_HDR_TSN_NUM_BITS + \
    DEC_HS_MAC_EHS_HDR_SI_NUM_BITS + \
     DEC_HS_MAC_EHS_HDR_F_FLAG_NUM_BITS) * \
    MCALWCDMA_DEC_HS_REORDER_SDU_MAX_COUNT)

/* Number of word32 required to extract MAC-ehs fields of maximum length */
/*lint -emacro(506,DEC_HS_DOB_W32_READ_FOR_MAC_HS_HEADER_DECODE) Max number of W32 needed to read
  MAC-HS header is dependent on various field lengths of the header */ 
#define DEC_HS_DOB_W32_READ_FOR_MAC_EHS_HEADER_DECODE \
  ((DEC_HS_MAC_EHS_HDR_LEN_MAX & 0x1F) ? \
   ((DEC_HS_MAC_EHS_HDR_LEN_MAX >> 5) + 1) : \
   (DEC_HS_MAC_EHS_HDR_LEN_MAX >> 5))

/* Maximum possible size of EXT MAC-ehs header */   
#define DEC_HS_EXT_MAC_EHS_HDR_LEN_MAX \
  ((DEC_HS_MAC_EHS_HDR_LC_ID_NUM_BITS + \
    DEC_HS_MAC_EHS_HDR_LEN_NUM_BITS + \
    DEC_HS_MAC_EHS_HDR_EXT_TSN_NUM_BITS + \
    DEC_HS_MAC_EHS_HDR_SI_NUM_BITS + \
    DEC_HS_MAC_EHS_HDR_F_FLAG_NUM_BITS) * \
    MCALWCDMA_DEC_HS_REORDER_SDU_MAX_COUNT)

/* Number of word32 required to extract MAC-ehs fields of maximum length */
/*lint -emacro(506,DEC_HS_DOB_W32_READ_FOR_EXT_MAC_EHS_HEADER_DECODE) Max number of W32 needed to read
  EXT MAC-eHS header is dependent on various field lengths of the header */ 
#define DEC_HS_DOB_W32_READ_FOR_EXT_MAC_EHS_HEADER_DECODE \
  ((DEC_HS_MAC_EHS_HDR_LEN_MAX & 0x1F) ? \
   ((DEC_HS_MAC_EHS_HDR_LEN_MAX >> 5) + 1) : \
   (DEC_HS_MAC_EHS_HDR_LEN_MAX >> 5))

/* Macro to extract MAC-ehs fields */  
#define DEC_HS_MAC_EHS_HDR_GET_FIELD(buf,offset_ptr,field) \
  EXTRACT_BITS_FROM_BUF(buf,offset_ptr, DEC_HS_MAC_EHS_HDR_##field##_NUM_BITS)

#endif /* FEATURE_HSDPA_MAC_EHS */

#define MCALWCDMA_DEC_UPDATE_SRC_DESC(addr, len) \
  mcalwcdma_dec_src_descriptor_move_out[mcalwcdma_dec_index_src_desc] = (addr); \
  mcalwcdma_dec_src_descriptor_move_out[mcalwcdma_dec_index_src_desc + 1] = (len); \
  mcalwcdma_dec_index_src_desc += 2;

#define MCALWCDMA_DEC_UPDATE_DST_DESC(addr, len) \
  mcalwcdma_dec_dst_descriptor_move_out[mcalwcdma_dec_index_dst_desc] = (addr); \
  mcalwcdma_dec_dst_descriptor_move_out[mcalwcdma_dec_index_dst_desc + 1] = (len); \
  mcalwcdma_dec_index_dst_desc += 2;

/* Cache line size in bytes */
#define MCALWCDMA_DEC_CACHE_LINE_SIZE 32

/* Number of log packet buffer for MAC hs header logging */
#define DEC_HS_NUM_MAC_HS_HDR_LOG_PKT_BUF 2


/* Indicate invalid HS info table */
#define MCALWCDMA_DEC_HS_INFO_TABLE_INVALID_VAL 0xFF



/* Deciphering algorithms */
#define MCALWCDMA_DEC_DECIPHER_ALGO_KASUMI 0
#define MCALWCDMA_DEC_DECIPHER_ALGO_SNOW3G 1

#ifdef FEATURE_WCDMA_JOLOKIA_MODEM
#define MCALWCDMA_DEC_TDEC_DOB_START_ADDR (HWIO_DECOB_MEM_BASE_MIN_ADDR )
#elif defined (FEATURE_THOR_MODEM)
/* TODO : Correct macro name needs to be obtained from HW/Core team */
#define MCALWCDMA_DEC_TDEC_DOB_START_ADDR (0x0)
#else
/* DOB start address for TDEC */
#define MCALWCDMA_DEC_TDEC_DOB_START_ADDR (HWIO_MEM_POOL_AXI_START_ADDR + 22784)
#endif

#ifdef FEATURE_TABASCO_MODEM
/* Invalid HS DOB transfer address*/
#define MCALWCDMA_DEC_INVALID_DOB_XFER_ADDR 0xFFFF
#endif
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Call back function type declaration */
/* ----------------------------------- */

/* Enum type declaration */
/* --------------------- */


/* CCTrCh reference type enum type */
typedef enum
{
  DEC_CCTRCH_REF_TYPE_BCH,      /* CCTrCh with BCH type TrCh */
  DEC_CCTRCH_REF_TYPE_PCH_FACH, /* CCTrCh with PCH and/or FACH type TrCh */
  DEC_CCTRCH_REF_TYPE_DCH,      /* CCTrCh with DCH type TrCh */
  DEC_CCTRCH_REF_TYPE_INVALID   /* Used when can't generate CCTrCh ref type */
} dec_cctrch_ref_type_enum_type;

/* This enum defines the rate matching type for mDSP rate matching
   parameters */
 typedef enum
{
  DEC_NO_RM,     /* No rate matching to be used */
  DEC_RM_REPEAT, /* Repetition used for rate matching */
  DEC_RM_PUNCT   /* Puncturing used for rate matching */
} dec_rm_type_enum_type;

/* This enum defines the endian-ness types of the DOB */
typedef enum
{
  /* litle endian mode used to read the word32 status data */
  DEC_DOB_LITTLE_ENDIAN,
  /* big endian mode used for access the data octets as word32 reads */
  DEC_DOB_BIG_ENDIAN
} dec_dob_endian_type_enum_type;

/* Type of data transfer mode */
typedef enum
{
  DEC_DATA_XFER_MODE_BG_READ,
  DEC_DATA_XFER_MODE_READ_W_IMME_TM_CIPHER
} dec_data_xfer_mode_eum_type;

/* Structure type declaration */
/* -------------------------- */

/* DRM Buffer information structure */
typedef struct
{
  /* Availability status */
  boolean available;

  /* DRM buffer start address */
  uint16 drm_buffer_start_addr;

  /* DRM buffer size */
  uint16 drm_buf_size;

  /* DRM buffer size in bits */
  uint16 drm_buf_size_bits;

  /* DRMB ID to CCTrCh mapping */
  uint8 cctrch_idx;

  /* CcTrCh reference type that is using DRM buffer area */
  dec_cctrch_ref_type_enum_type cctrch_ref;
} mcalwcdma_dec_drm_buffer_info_struct_type;

/* TF pool structure */
typedef struct
{
  /* PLEASE DO NOT CHANGE THE ORDER OF ELEMENTS
     IN THIS STRUCTURE. 
     The members are placed strategically for memory optimization */
  uint8  next;    /**< Link to the next node */
  uint8  num_tb;  /**< Number of transport block in TBS. */
  uint16 tb_size; /**< Transport block size. */
}mcalwcdma_dec_tf_pool_info_struct_type;

typedef struct
{
  uint8  num_tb;   /**< Number of transport block in TBS. */
  uint16 tb_size;  /**< Transport block size. */
} dec_cctrch_table_tf_info_struct_type;

typedef struct
{
  /* Transport channel ID. Used only for logging */
  uint8 trch_id;

  /* Transport channel reference type */
  mcalwcdma_dec_trch_ref_type_enum_type trch_ref_type;

  /* FACH TrCh type 
     0x00   R99 FACH TrCh 
     0x01   MCCH FACH TrCh
     0x02   MTCH FACH TrCh
     0x04   MSCH FACH TrCh
     0xFF   Not a FACH transport channel */
  uint8 fach_trch_type;
  
  /* Coding type of transport chanel */
  mcalwcdma_dec_coding_enum_type coding_type;

  /* CRC length in bits */
  uint8 crc_length;

  /* TTI duration of transport channel */
  mcalwcdma_dec_tti_enum_type tti_value;

  /* DRMB size of TrCh in bits */
  uint16 drm_buffer_size;

  /* Loop back mode for this TrCh. Only Applicable if TrCh ref type is DCH */
  mcalwcdma_dec_loop_back_mode_type_enum_type loopback_mode;

  /* Rate matching attribute. USed only for logging */
  uint16 rate_match_attrib;

  /* Number of TF in this TrCh */
  uint8 num_tf;

  /* TFT base in TF table */
  uint16 tft_base;

  /* TFT index for this transport channel max sized TF */
  uint8 max_size_tft_idx;


  /* Indicates whether this TrCh is of type DCCH */
  boolean is_dcch;

    /* Indicates whether this TrCh carries voice */
  boolean is_voice;
} dec_cctrch_table_trch_info_struct_type;

/* CCTrCh table information */
typedef struct
{
  /* CCTrCh table in use flag */
  boolean in_use;

  /* CCTrCh ID for which this index is being used */
  uint8 cctrch_id;
  
  /* CCTrCh Table index corresponding to the information contained here */
  uint8 table_index;

  /* indicate number of TFC in CTFC table */
  uint8 num_tfc;

  /* CCTrCh reference type, indicate type of TrCh in that CCTrCh */
  dec_cctrch_ref_type_enum_type ref_type;

  /* DRM buffer segment index */
  uint8 drm_buf_area_id;

  /* Second DRM buffer segment index - used for PCCPCH and MTCH soft combining */
  uint16 sec_drm_buf_area_id;

  /* The field is used only for PCCPCH CCTrCh */
  /* Even or odd PCCPCH CFN enabled */
  dl_cctrch_cfn_type even_odd_cfn;

  /* Numner of TrCh in this CCTrCh */
  uint8 num_trch;

  /* TrCh info structure */
  dec_cctrch_table_trch_info_struct_type trch_info[MCALWCDMA_DEC_TOTAL_TRCHT_PER_CCTRCH];

  /* TF info structure */

  /* Total number of TF in this CCTrCh */
  uint8 num_tf;

  /* Index of the TF pool for each TF in CCTrCh */
  uint8 tf_pool_index[DEC_TOTAL_TFT_PER_CCTRCH];

  /* Head for the TFCI pool. The TFCI pool can be traversed from this index till index+num_tfc */
  uint16 tfci_pool_head;

} dec_cctrch_table_info_struct_type;

/* This structure defines the CCTrCh table structure for mDSP. This includes the
   CCTrCh header that has CTFC table index, and number of TrCh. This is followed
   by transport channel (TrCh) table and transport format (TF) table */
typedef struct
{
  uint32 cctrch_header; /* CCTrCh table header. */

  /* Transport chanel index sorted by their TTI length */
  uint32 trch_tti_order_table[DEC_MDSP_NUM_MAX_DWORD_TTI_ODRT];
  
  /* Transport channel table */
  uint32 trch_table[MCALWCDMA_DEC_TOTAL_TRCHT_PER_CCTRCH][DEC_MDSP_NUM_DWORD_TRCHT_ENTRY];
  
  /* Transport format table */
  uint32 tf_table[DEC_MDSP_NUM_MAX_DWORD_TFT];

  /* TFCI table */
  uint32 tfci_table[DEC_MDSP_NUM_MAX_DWORD_TFCI_TABLE];

  /* The number of W32 needed for storing TF and TFCI info in this CCTrCh */
  uint8 dec_max_tf_w32_in_cctrch;
  uint8 dec_max_tfc_w32_in_cctrch;
} dec_cctrch_table_struct_type;

typedef struct
{
  /* Store TrCh table value */
  uint32 trch_table_value[DEC_MDSP_NUM_DWORD_TRCHT_ENTRY];
}mcalwcdma_dec_trch_table_info_struct_type;

/* This structure stores the CCTrCh header and the corresponding address */
typedef struct
{
  uint32 cctrch_header[DEC_MDSP_NUM_DWORD_CCTRCH_HDR];
  mcalwcdma_dec_trch_table_info_struct_type trch_info[MCALWCDMA_DEC_TOTAL_TRCHT_PER_CCTRCH];
}mcalwcdma_dec_header_info_struct_type;

/* This structure defines code block parameters for a transport format. */
typedef struct
{
  /* Code block size after code block segmentation. */
  uint16 code_blk_size;
  
  /* Number of code blocks after the code block segmentation */
  uint16 num_code_blks;
  
  /* Number filler bits or padded bits after the code block segmentation. 
    These bits are always 0 and are added at the begining of code block
    or first segment. */
  uint16 num_filler_bits;
} mcalwcdma_dec_code_blk_param_struct_type;

/* This structure defines turbo parameters that are required by
   turbo decoder. */
typedef struct
{
  /* Interleaver length. This is derived directly from the code block size. */
  uint16 interleaver_length;
  
  /* Number of maximum iteration. The possible values are 6 to 8 depending
    on the code rate. */
  uint8 max_iterations;
  
  /* Row index for the interleaving pattern. The possible values are B00 to
    B11 depending number of rows. This value indicates the pattern index
    used in inter-row permutations */
  uint8 row_index;
  
  /* Number of columns. The possible values are 5 to 257 depending number of
    rows. The value in this variable is one less than the actual number
    of columns. */
  uint16 num_columns;
  
  /* Row and column product. The product is between the actual number of
    column and rows and not as in variable row_index and num_columns */
  uint16 row_column_product;
  
  /* Prime number that is used to decide the number of column value. The
    possible values of the prime number  are 7 to 257. */
  uint16 prime_num;
  
  /* Prime number index corresponding to the prime number. The prime number
    index starts from 0 for prime number 7, 1 for 11 and so on. */
  uint8 prime_num_index;
  
  /* Last column used by last code block data in turbo interleaver table. */
  uint16 last_column;
  
  /* Last row used by last code block data in turbo interleaver table. */
  uint8 last_row;
  
  /* Last row bit exchange condition for the intra and inter row
    permutations. This is set to true if the code block size is equal to
    product of number of rows and column and number of column is one less
    than prime number. */
  boolean last_row_bits_exchange;
} mcalwcdma_dec_turbo_param_struct_type;

/* This structure defines rate matching parameters that are required for
   de-rate marching. */
typedef struct
{
  /* Rate matched size per radio frame */
  uint16 rate_matched_size_per_radio_frame;
  
  /* De rate matching scale factor */
  uint16 drm_scale_factor;
  
  /* Optimized eplus/a value for rate matching parameters */
  uint32 eplus_over_a;
  
  /* Optimized eminus/a value for rate matching parameters */
  uint32 eminus_over_a;
  
  /* Indicates it the delta N per TTI is ODD or not */
  boolean delta_n_tti_odd;
  
  /* rate matching type used */
  dec_rm_type_enum_type rm_type;
} dec_rm_param_struct_type;

/* This enum defines the TrCh types that the TBSet info represents */
typedef enum
{
  DEC_TBSET_TRCH_TYPE_R99,
  DEC_TBSET_TRCH_TYPE_HS,
  DEC_TBSET_TRCH_TYPE_EHS
} mcalwcdma_dec_tbset_trch_type;

/** This structure defines the decoder output buffer status FIFO entry structure.
 * It has all the information that mDSP status FIFO has. In addition to that it
 * has got information about the transport block that is collected from the TF
 * information using the mDSP TF Id */
typedef struct
{
  q_link_type link; /**< Link to manage this item on the queue */
  uint16 index;      /**< index of this element in array */
  
  boolean valid;               /**< information is valid or not              */
  uint16 trch_start_addr;      /**< Transport channel start address in DOB   */
  boolean hs_trch_type; /**< Indicate if TBset is of HS type or not */

  /** This is HS or non -HS info based on hs_trch_type */
  union
  {
    /** non-HS (R99) TBset information */
    struct
    {
      boolean tfci_decode_pass; /**< TFCI decode pass indicator */
      boolean zsb;              /**< zero state bit */
      uint8 mdsp_cctrch_id;     /**< mDSP CCTrCh Id (mDSP CCTrCh table index) */
      uint8 mdsp_trch_id;       /**< mDSP TrCh Id (mDSP TrCh table index) */
      uint8 mdsp_tf_id;         /**< mDSP TF Id (mDSP TF table index) */
      uint8 cfn_value;          /**< CFN value reported in status FIFO */
      uint8  num_tb;            /**< Number of transport block */ 
      uint16 tb_size_bits;      /**< Transport block size in number of bits */ 
      /** Word 32 align size of TrBlk. Hardware stores the data for TrBlks
       * start at uint32 align addr */
      uint16 tb_32bit_align_size_word32;
      /** Indicate don't check CRC pass/fail and get data for block of this TrCh */
      boolean override_crc;
      mcalwcdma_dec_dob_status_type dob_status; /**< DOB status information */
     #ifdef FEATURE_UMTS_BMC
      uint16 ctch_sfn;             /* SFN pointing to start of CTCH block TTI  */
     #endif
      WfwDemodDobTypeEnum dob_type;
      boolean dcch_early_detect;    /* Indicates whether DCCH was detected by FW */
      boolean decode_result;    /**< Indicates if the decode result is good */
      boolean skipped_decode;      /**< This decode will not affect PC and BLER updates */
      boolean non_first_traceback; /**< Indicate subsequent traceback needed for CRC pass */
    } non_hs;

    /** HS TBset information */
    struct
    {
      uint8 hs_table_id;        /**< HSDPA table index Id */
      uint8 harq_id;            /**< HARQ process Id */
      boolean new_data;         /**< indicate new data bit state */
      boolean crc_pass;         /**< indicate PDSCH blocks CRC pass/fail status */
      uint16 global_sub_fn;     /**< HSDPA global sub frame number */
      uint16 hs_tb_size;        /**< HS transport block size */
      uint16 hs_hdr_size;       /**< MAC-hs header size */
      uint8  num_total_tb;      /**< Total block of all sizes */
      uint16 mac_d_pdu_sz[DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT]; /**< PDU size info */
      uint8 dob_fifo_idx;
     #ifdef FEATURE_WCDMA_HS_FACH
      uint8 hrnti_type;   /**< indicate type of H-RNTI or blind decoding */
     #endif
    } hs;

  } tbset_info;

  /** This points to the array of CRC data bits */
  uint32 *crc_data_bits_ptr;
} dec_dob_status_fifo_struct_type;

/** @name Macros for DOB status FIFO member access
 *  Now HS and non HS type depending on HSDPA feature defined.
 *  Following macros help them in easy access */
/**@{*/

#ifdef __GNUC__

 #define DEC_DOB_STATUS_FIFO(dob_status_fifo,member) \
  dob_status_fifo.tbset_info.non_hs.member
 #define DEC_DOB_STATUS_FIFO_PTR(dob_status_fifo,member) \
  dob_status_fifo->tbset_info.non_hs.member

#else /* __GNUC__ */

 #define DEC_DOB_STATUS_FIFO(dob_status_fifo,member) \
  dob_status_fifo.tbset_info.non_hs.##member
 #define DEC_DOB_STATUS_FIFO_PTR(dob_status_fifo,member) \
  dob_status_fifo->tbset_info.non_hs.##member

#endif /* __GNUC__ */

/**@}*/

/* The data structure for storing decoded TFCI info as mirrored from mDSP */
typedef struct
{
  /* Decoded CFN */
  uint16 decoded_cfn;

  /* Max energies 1 and 2 */
  uint16 max_energy1;
  uint16 max_energy2;

  /* The decoded TFCI */
  uint16 decoded_tfci;
}mcalwcdma_dec_decoded_tfci_info_struct_type;

/********************** Start F8 support  ******************/

/* 16 bit rotate left */
#define ROL16(a,b) (uint16)((a<<b)|(a>>(16-b)))

/********************** End F8 support  ******************/

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* Total DRM buffer size */
extern uint32 mcalwcdma_dec_drm_buffer_size;
extern boolean dl_cctrch_enabled[DEC_TOTAL_CCTRCH_TABLES];
/* This array stores the code block parameters of all TF of all TrCh in the
   CCTrCh under setup*/
extern mcalwcdma_dec_code_blk_param_struct_type *mcalwcdma_dec_code_blk_param[MCALWCDMA_DEC_TOTAL_TRCHT_PER_CCTRCH];

/* This array stores the status fifo information. This information is refered
   to by all other function to get the starting address of the DOB. This
   information is also used by header and data reading function to get the
   transport format Id to get the transport block size and number of transport
   blocks */
extern dec_dob_status_fifo_struct_type dec_dob_status_fifo[DEC_MAX_STATUS_FIFO_COUNT];

/* Q for DOB status FIFO */
extern q_type mcalwcdma_dec_dob_status_fifo_free_q;

/* Critical section mutex for Dec HS HARQ statistics */
extern rex_crit_sect_type dec_hs_harq_stats_crit_sect;

extern uint32 mcalwcdma_dec_umts_dl_hw_task_q_wr_ptr;

extern q_type mcalwcdma_dec_tsn_info_internal_q;

extern q_type mcalwcdma_dec_concatenated_pdu_tsn_internal_q;

extern dec_cctrch_table_info_struct_type
  mcalwcdma_dec_cctrch_info[MCALWCDMA_DEC_TOTAL_CCTRCH_TABLES];

/* The buffer index in use currently */
extern uint8 mcalwcdma_dec_tbset_logpkt_index_in_use;

/* Log buffer availability status */
extern boolean mcalwcdma_dec_tbset_logpkt_avail_buf[MCALWCDMA_DEC_NUM_TBSET_LOGPKT];

extern mcalwcdma_dec_tf_pool_info_struct_type mcalwcdma_dec_tf_pool[MCALWCDMA_MAX_NUM_TF_POOL];

/* The buffer index to be submitted */
extern uint8 mcalwcdma_dec_tbset_logpkt_index_to_submit;

/* Indicate if decoder related HSDPA information is valid or not */
extern boolean mcalwcdma_dec_hs_info_valid[MCALWCDMA_DEC_HS_INFO_COUNT];

/* Decoder related HSDPA information */
extern mcalwcdma_dec_hs_info_struct_type mcalwcdma_dec_hs_info[MCALWCDMA_DEC_HS_INFO_COUNT];

/* This array stores the turbo parameters of all TF of all TrCh in the CCTrCh.
   undet setup */
extern mcalwcdma_dec_turbo_param_struct_type *mcalwcdma_dec_turbo_params[MCALWCDMA_DEC_TOTAL_TRCHT_PER_CCTRCH];


/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION mcalwcdma_dec_do_data_transfer

DESCRIPTION
  This function does the data transfer of all blocks in header indication and
  read the data from DOB for them.
  
  It supports 2 modes of data transfer.
  1. Background data transfer: For this block must be unciphered or if ciphered
     deciphering must be done already. TM modeciphered blocks are not handled
     in this mode
  2. TM mode ciphered blocks are handled in tis mode. Blocks are deciphered
     immediately block by block before reading data out of DOB.

DEPENDENCIES
  Use following variable set outside the function
  dec_num_tbset_done_for_bg_data_xfer: Starts checking data transfer from this
    value.
  dec_num_tb_done_for_bg_data_xfer: Start checking for data transfer for this
    block in TBset
  dec_num_bits_being_deciphered: If deciphering is not in progress then
    it transfers atleast  this much amount before giving up control for
    deciphering done status check.

RETURN VALUE
  None

SIDE EFFECTS
  Set flag dec_data_xfer_waiting_for_decipher_done if encouters block with
  ciphering but not yet deciphered. Background data transfer stops with this
  condition
  Updates variables dec_num_tbset_done_for_bg_data_xfer and
  dec_num_tb_done_for_bg_data_xfer
==========================================================================*/

extern LOCAL void mcalwcdma_dec_do_data_transfer(
  /* pointer to the TB header decode info strcuture */
  mcalwcdma_dec_tb_hdr_decode_struct_type *tb_hdr_decode_info,
  /* Ptr to array of the DOB handles for these TB sets Hdr info */
  mcalwcdma_dec_dob_status_handle_type *dob_handles,
  /* Data transfer mode type */
  dec_data_xfer_mode_eum_type data_xfer_mode,
  /* Ptr to structure in which to store data */
  mcalwcdma_dec_tb_data_struct_type *dl_tb_data_info);

/*===========================================================================
FUNCTION dec_hs_init_deciph_scan

DESCRIPTION
  This function initialize the output used for deciphering a HS blocks.
  The parameter that are inited are
  - Current Size indicator information index (order is as they are in MAC-hs
    header) that is being deciphered. Value set to 0
  - Number of blocks scanned in that SID information. Value set to 0
  - Offset of first bit to be deciphered for current block from start of
    HS-DSCH transport block. Value set to 0
  - Current MAC-d PDU block offset from start of HS-DSCH block. Value set to
    MAC-hs header size.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All above mentioned variables in descriptions are initialized
===========================================================================*/

extern void dec_hs_init_deciph_scan(
  /* DOB status FIFO information structure pointer */
  dec_dob_status_fifo_struct_type *mdsp_status_fifo);

/*===========================================================================
FUNCTION dec_hs_init_hs_block_bg_xfer

DESCRIPTION
  This function initialize the output used for extracting DL HS MAC-d SDU
  from HS-DSCH block
  The parameter that are inited are
  - Current Size indicator information index (order is as they are in MAC-hs
    header) that is being transferred from DOB. Value set to 0
  - Number of blocks scanned in that SID information. Value set to 0
  - Current MAC-d PDU block offset from start of HS-DSCH block. Value set to
    MAC-hs header size.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All above mentioned variables in descriptions are initialized
===========================================================================*/

extern void dec_hs_init_hs_block_bg_xfer(
  /* DOB status FIFO information structure pointer */
  dec_dob_status_fifo_struct_type *mdsp_status_fifo);

/*===========================================================================
FUNCTION dec_hs_get_next_bg_xfer_offset

DESCRIPTION
  This function gets following parameter for current MAC-d PDU to transfer it
  out of DOB
  - DOB starting address where MAC-d SDU starts
  - Start bit position with start address
  - Length of MAC-d SDU to be transferred
  
  
DEPENDENCIES
  Init function mst have been called at the start of HS block before using
  this function.

RETURN VALUE
  None

SIDE EFFECTS
  All above mentioned variables in descriptions are modified
===========================================================================*/

extern boolean dec_hs_get_next_bg_xfer_offset(
  /* DOB status FIFO information structure pointer */
  dec_dob_status_fifo_struct_type *mdsp_status_fifo,
  /* TB set header information structure pointer */
  mcalwcdma_dec_tb_set_hdr_struct_type    *tbset_hdr_info,
  /* MAC-d PDU block information structure pointer */
  mcalwcdma_dec_tb_hdr_struct_type *tb_hdr_info);

/*===========================================================================
FUNCTION mcalwcdma_dec_proc_log_submit

DESCRIPTION
  This function is called in response to the log service request by 
  the decoder module. It checks for the state of the log packets and if
  waiting to be submitted, it calls the appropriate routines to submit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/

extern void mcalwcdma_dec_proc_log_submit(void);

/*===========================================================================
FUNCTION mcalwcdma_dec_write_copy_engine_parms

DESCRIPTION
  This function writes the copy engine parameters to the UMTS DL HW Task Q
  to move HSDPA transport blocks from one on-chip memory location to another.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if SUCCESS
  FALSE if FAILURE

SIDE EFFECTS
  None
==========================================================================*/

extern void mcalwcdma_dec_write_copy_engine_parms(
  /* Copy Count */
  uint16 copy_count,
  uint8 ignore_rmw,
  /* Length */
  uint16 length,
  /* Source address */
  uint32 src_addr,
  /* Source bit offset */
  uint8 src_bit_offset,
  /* Source endianess */
  uint8 src_endianess,
  /* Source increment */
  uint16 src_incr,
  /* Destination address */
  uint32 dest_addr,
  /* Destination bit offset */
  uint8 dest_bit_offset,
  /* Destination endianess */
  uint8 dest_endianess,
  /* Destination increment */
  uint16 dest_incr);

/*===========================================================================
FUNCTION mcalwcdma_dec_write_nop_task_to_mem_and_align_wr_ptr

DESCRIPTION
  This function writes NOOP task words to the UMTS DL HW Task Q and aligns
  the write pointer to 32-byte address.

DEPENDENCIES
  None

RETURN VALUE
  Updated write pointer if SUCCESS
  0xFFFFFFFF if FAILURE

SIDE EFFECTS
  None
==========================================================================*/

extern void mcalwcdma_dec_write_nop_task_to_mem_and_align_wr_ptr(void);


#endif /* MCALWCDMA_DECI_H */
