#ifndef WL1DEC_H
#define WL1DEC_H

/*============================================================================
                           D L D E C . H
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dldec.h_v   1.33   11 Jul 2002 22:14:10   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1dec.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
07/01/14    hk      Header file inclusion cleanup
06/10/14    bs      Added support for cctrch ID alloc/dealloc history profiling
01/27/14    ac      Change ctch_pdu_decode_error_cb(int32) to (dword) to 
                    be consistent with NAS
11/21/13    dm      Post local command for SFN/CFN sanity check.
06/14/13    ms      WL1 inform BMC once CTCH CRC error happens
02/20/13    hk      Mainlined FEATURE_WCDMA_DL_ENHANCED
11/14/12    jhl     Added prototype for dl_set_trch_state_interf_resel
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/28/12    zr      Added feature definitions for Dime
01/27/12    vs      nikel feature cleanup.
08/18/11    rc      Removed turbo call callback API.
08/14/11    mc      Added API to send CCTrCh dirty bit sync command
07/05/11    vsr     Porting SCHIC disable for voice
06/24/11    mc      Increase CCTrCh dirty bitmask to 32 bits
03/14/11    ms      Mainlined MSMHW_DEC_REV_6275
09/21/10    scm     Support CTCH DRX lengths greater than 4096 frames.
08/23/10    ks      Removed inclusion of mdsp header for Genesis (featurized)
05/08/09   sup      Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                    srchlog.h to wdec.h, wenci.h, wl1m.h, wsrch.h, wsrchi.h, wsrchlog.h
04/24/09    mr      Fixed Compiler Warnings
04/02/09    ms      Mainlined changes under FEATURE_WCDMA_L1_DEC_DOB_STATUS_FIFO_USE_Q
                    Added macros DEC_DOB_HANDLE_INTLOCK and 
                    DEC_TSN_Q_INTLOCK.
03/19/09    ms      Externed dec_cctrch_dirty_bmsk.
10/03/08    hk      Bringing in 7k Mailine fixes
01/25/08    mc      Added support for MCAL
1/23/08     mc      Sync up to mainline tip
12/21/07    mc      Changed prototype of CCTrCh setup function.
12/19/07    mc      Corrected use of feature flags and remove prototype of
                    an obsolete function.
08/20/07    kps     Expose dl_num_times_sfn_mismatch for diag
08/20/07    vsr     Moved in dl_decode_sfn_prime()
07/31/07    mc      Fix MBMS related feature definitions
06/18/07    mc      Added prototype to query DCH CCTrCh loopback status.
03/16/07    mc      Changed prototype of dl_update_mdsp_beta_table to 
                    include RxD current state.
04/17/07    mc      Added structure to pass MTCH TTI query and changed query
                    function API.
03/03/07    mc      Sync up for MBMS phase2A integration
12/12/06    mc      Synced up for MBMS demo - 3042 baseline
11/30/06    mc      Added support for MBMS demo
06/01/06    rmak    Changed prototype of dl_set_cctrch_enable_state() and
                    dl_set_trch_enable_state to return success of state change
03/22/06    gs      Changed dl_loopback_mode type declaration to internal
                    loopback mode definition
11/22/05    gs      Added macro DL_L1_RRC_CCTCRH_ID_SWAPPED for CCTrCh swapped Id
08/24/05    gs      Updated externalization of dl_validate_cctrch_setup_req
07/28/05    gs      Added macro DL_DEC_MAX_CCTRCH_WITH_CTFC_EXISTENCE
04/18/05    gs      Merged code from 6275 baseline
02/27/05    scm     Set DL_NUM_MAX_RRC_NON_BCH_CCTRCH to 2 for BMC support.
02/26/05    gs      Externalized function dl_flush_hs_info_from_dob
11/04/04    vb      Code changes for flushing the periodic logpakts to diag
                    after tearing down the physical channels
11/01/04    vb      Externalized the definition dl_dch_trch_max_blk_tf_idx and
                    Added code under feature FEATURE_L1_MANAGE_WIND_DOWN to
                    freeze dl OLPC under wind down detect condition
10/11/04    gs      Removed CCTrCh DMA related declarations
08/10/04    gs/src  Added further CCTrCH table and accounting changes for
                    concurrent P-CCPCH TTI hypothesis testing.
08/06/04    gs      Added type definition for DL_DEC_FAILED_BLK_QUERY_CB_FUNC_TYPE
                    Externalized array dl_errored_blk_pass_thro_mode
                    Externalized function dl_dec_register_failed_trblk_query_cb_func
06/04/04    gs/src  Changes to support multiple neighbour BCH CCTrCH table
                    indices to go with generalised concurrent and sequential
                    TTI-alignment hypothesis testing algorithms for P-CCPCH.
05/04/04    gs      Externalized globals dl_cctrch_enable_cfn and
                    dl_trblk_tti_bdry_check_pending.
                    Moved macro DLDEC_GET_CFN_TTI_ALIGN_TYPE from dldec.c to
                    this header file
04/13/04    gs      Changed prototype of function dl_set_cctrch_enable_state
02/25/04    gs      Added macro for safeguarding the errorneous DOB status FIFO
                    from sending it to demod under feature
                    FEATURE_L1_MDSP_ERROR_RECOVERY_P1
12/15/03    gs      Add dl_get_trch_longest_tti().
11/17/03    gs      Declared new function dl_flush_all_dob_status_fifo().
11/12/03    m       DL OLPC related code change to cleanup existing
                    implementation
10/01/02    src     Changed the moving window length for TPC accumulation from
                    2 to 3 as well as the corresponding up threshold from 27 to
                    40. This is to reduce the probability of hitting the wind-
                    up ceiling when compressed-mode gaps are active.
05/28/03    scm     Externalize dl_bch_cctrch_info and dl_bch_ctfc_info.
04/25/03    m       Externalized dl_cctrch_ptrs declaration to be used in
                    dltrchmeas.c for SIR optimization
04/16/03    m       Allows a new Beta table to be supplied to MDSP
03/12/03    scm     Add extern to function dl_get_shortest_tti().
12/04/02    m       Changes required to fix final missing BLER dump
11/14/02    m       Implemented OLPC and windup algorithms
10/21/02    gs      Externalized variable dl_cctrch_table_index_id
09/19/02    gs      Externalized array dl_trch_tti.
09/19/02    scm     Don't FEATURE_DUALMODE_BASELINE around dl_bler_meas_delay_timer.
07/11/02    gs      Corrected the array length of variables dl_trch_id_cctrcht_index
                    and dl_trch_id_table_index.
06/25/02    m       Modified prototype for function dl_update_cctrch_log_info.
                    This modification is necessary to reuse the function
                    for log on demand purpose.
06/07/02    sh      Externed dl_cctrch_avail_status variable.
05/29/02    m       Added support for log on demand
                    1.  Prototype for logging DL CCTrCH
                    2.  Prototype for logging DL TFC
05/17/02    gs      1. Changed the prototype of function dl_btfd_validate

05/06/02    scm     Change FEATURE_WCDMA_L1_START_STOP to
                    FEATURE_CHEETAH_BASELINE, to match RRC.
05/03/02    scm     Implementing support of START/STOP primitives from RRC.
                    Code wrapped in FEATURE_WCDMA_L1_START_STOP define.
05/01/02    yus     Changed FEATURE_MSM5200c to MSMHW_DEC.
02/28/02    gs      1. Merged the code changes for MSM5200C.

10/09/01    gs      1. Added the array size to the array definitions for
                       turbo parameter prime numbers and CCTrCh reference type
                       bit field definitions.
                    2. Defined the size of CcTrCh reference type enum types
                       at the end of definition.

09/27/01    gs      1. Externalized the function dl_set_trch_enable_state.

09/14/01    gs      1. Externalized the variable dl_trch_ref_type and 
                       dl_trch_table_index_id
                    2. Removed the enum types dl_dec_tb_evt_cmd_enum_type,
                       dl_dec_tb_evt_mode_enum_type for experimental support
                       of feature DL_DEC_MULTI_TB_HDR_DATA_EVT_SUPPORTED 

08/21/01    gs      1. Externalized the loopback mode info array.

08/14/01    gs      1. Externalized the non zero CRC criteria info.
                    2. Externalized the function to get the longest TTI in a CCTrCh

07/17/01    gs      1. Added the macro define DL_DEC_MAX_CCTRCH_BTFD_INFO.
                    2. Changed the structure element num_tb to num_total_tb in
                       dl_dec_tbset_hdr_fail_drop_info structure.
                    3. Externalized the new globals defined in dldec.c.

06/18/01    gs      1. Added the structure definition for structure
                       dl_dec_tbset_hdr_fail_drop_info for failed dropped
                       TB for the feature FEATURE_TB_IND_DETAIL_INFO.

06/12/01    gs      1. Added the structure member drop_phychan_type in
                       cctrch setup cmd structure.

06/07/01    gs      1. Externalized the CRC length info.

05/29/01    gs      1. Updated the externalization of optimized array declaration.

05/11/01    gs      1. Added the macro to define the constant values related
                       to BTFD validation.
                    2. Added enum for TB event mode type.
                    3. Externalized num TB and TF info related variable.
                    4. Externalized new function created for BTFD related coding.

03/29/01    gs      1. Added the prototype declaration for the function to
                       update the log packet.

03/08/01    gs      1. Changed the dl_cctrch_cmd_type to new structure
                       definition. Earlier it was typedefed to l1_setup_cmd_type.
                    2. Changed the prototype declaration for dl_cctrch_setup
                       and dl_validate_cctrch_setup_req

02/05/01    gs      1. Externalized the function to set the CCTrCh state.

01/12/01    gs      1. Externalized the flag to call the DMA transfer timeout
                       tick update function.
                    2. Externalized the DMA transfer timeout tick update
                       function.

01/05/01    gs      1. Removed the function dl_bch_tti_sync_estab

12/15/00    gs      1. Added the history section to file
                    2. Changed the declaration of the function
                       dl_decode_nbch_sfn to dl_decode_bch_sfn.

12/14/00    sh      1. Added extern uint8 cctrch_ref_to_trch_bf_table[]

12/13/00    gs      1. Added the macro to convert the CCTrCh reference type
                       to TrCh BF.

11/22/00    gs      1. Changed the enum DL_CCTRCH_BCH_NBCH_INIT_TYPE to
                       DL_CCTRCH_BCH_INIT_TYPE.
                    2. Added the enum DL_CCTRCH_BCH_OFF_TYPE to enum definition
                       for bch substate.
                    3. Added the enum definition dl_bch_usage_enum_type.
                    4. Added the function declaration for new function dl_bch_ctrl.

11/02/00    sh      1. Removed underscores from file name
===========================================================================*/
/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "l1rrcif.h"
#include "l1macdata.h"
#include "dlcctrchlog.h"
#include "queue.h"
#include "msm.h"
#include "dldec_common.h"
#include "wdec.h"
#include "l1extif.h"

/* force feature define based on master feature list in cust file */
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
#define FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC
#endif

#define FEATURE_WCDMA_L1_DEC_DOB_STATUS_FIFO_USE_Q
#define FEATURE_WCDMA_L1_DEC_ENHANCED_DATA_XFER_MODE

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* minumum TB energy matrix for TBset for Conv coded TrCh.
   If the egy matric value is lower than this CRC is forced to fail */
#define DEC_MIN_TB_EGY_MATRIC_THRESH 256
/* Maximum symbol error on the passed block. Used in case of BTFD. 
   If a TF which passes CRC has symbol error > threshold, the TF is dropped */
#define DEC_MAX_SYMBOL_ERR_THRESHOLD    10

#define DEC_MDSP_SF_CFN_TTI_TYPE(cfn) \
  ((((cfn + 1) % 2) != 0) ? L1_TTI_10MS : \
   (((cfn + 1) % 4) != 0) ? L1_TTI_20MS : \
   (((cfn + 1) % 8) != 0) ? L1_TTI_40MS : L1_TTI_80MS)

/* Maximum number of CCTrCh used */
#ifdef FEATURE_UMTS_BMC
 /* For broadcast message support on CTCH mapped to SCCPCH, we may end up with
 ** 2 CCTrCh's, one for CTCH and one other channels like, PCCH, etc. */
#define DL_NUM_MAX_RRC_NON_BCH_CCTRCH 2
#else
 #define DL_NUM_MAX_RRC_NON_BCH_CCTRCH 1
#endif

#define DL_NUM_MAX_RRC_SBCH_CCTRCH    1
#define DL_NUM_MAX_L1_BCH_INIT_CCTRCH 4 /* for internal L1 BCH init setup */
#define DL_NUM_MAX_L1_NBCH_CCTRCH     2 /* for internal L1 NBCH SFN decode */

/* Number of CCTrCh with TFCI existense */
/*CTCH based SCCPCH and PCCH based SCCPCH can exist on two different CCTRCH new DL*/
#ifdef FEATURE_UMTS_BMC
#define DL_DEC_MAX_CCTRCH_WITH_CTFC_EXISTENCE 2
#else
#define DL_DEC_MAX_CCTRCH_WITH_CTFC_EXISTENCE 1
#endif

/* Macros used for BTFD validation */
/* ------------------------------- */

/* DL_DEC_MAX_CCTRCH_WHILE_BTFD indicate the total CCTrCh that can be
   setup when by RRC when any of the CCTrCh need BTFD. Thge reconfig is
   not considered as extra CCTrCh. */
#define DL_DEC_MAX_CCTRCH_WHILE_BTFD 2

/* DL_DEC_MAX_CCTRCH_WITH_BTFD indicate the total number of CCTrCh that
   can be with BTFD requirement */
#define DL_DEC_MAX_CCTRCH_WITH_BTFD  1

/* DL_DEC_MAX_CCTRCH_BTFD_INFO indicate the number of element in the array
   of the variable/structure that are required by BTFD module. This takes
   in to account the 1 extra data structure required for reconfig request */
#define DL_DEC_MAX_CCTRCH_BTFD_INFO  (DL_DEC_MAX_CCTRCH_WITH_BTFD + 1)

#define DL_DEC_MAX_DATA_BITS_BTDF    600
#define DL_DEC_MAX_TFC_BTFD          64
#define DL_DEC_MAX_TRCH_W_CRC_BTFD   3
#define DL_DEC_MAX_TF_ALL_TRCH_W_CRC_BTFD 16

/* This Id is used by CCTrCh setup for L1 internal BCH CCTrCh setup for
   BCH init at ACQ or for NBCH setup for SFN decode */
#define DL_L1_INT_BCH_CCTRCH_ID 0xFF
#define DL_L1_INVALID_CCTRCH_ID 0xFE
#define DL_L1_RRC_CCTCRH_ID_SWAPPED 0xFD
#define DL_NUM_TRCH_BCH         1
#define DL_NUM_TF_BCH           1

#define DL_BCH_NUM_TB                     MCALWCDMA_DEC_BCH_NUM_TB          
#define DL_BCH_TB_SIZE_BITS               MCALWCDMA_DEC_BCH_TB_SIZE_BITS    

/* CcTrCh index / Id that indicates the unsued Id */
#define DL_L1_ID_INDEX_UNUSED NO_CCTRCH_INDEX

#define DLDEC_GET_CFN_TTI_ALIGN_TYPE(cfn) \
  ((((cfn) % 2) != 0) ? L1_TTI_10MS : \
   (((cfn) % 4) != 0) ? L1_TTI_20MS : \
   (((cfn) % 8) != 0) ? L1_TTI_40MS : L1_TTI_80MS)

/* Maximum code block size for convolutional coding. */
#define RM_MAX_COV_CODE_BLK_SIZE    504

/* cctrch history profiling buffer max size */
#define DEC_CCTRCH_HIST_BUFF_MAX_SIZE 32

/* Mutex for DOB handle */
extern rex_crit_sect_type dob_handle_crit_sect;

#define DEC_DOB_HANDLE_INTLOCK()  REX_ISR_LOCK(&dob_handle_crit_sect)
#define DEC_DOB_HANDLE_INTFREE()  REX_ISR_UNLOCK(&dob_handle_crit_sect)

#define DEC_TSN_Q_INTLOCK()  REX_ISR_LOCK(&dec_tsn_q_crit_sect)
#define DEC_TSN_Q_INTFREE()  REX_ISR_UNLOCK(&dec_tsn_q_crit_sect)

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef enum
{
  DEC_LOOPBACK_NA,       /* Transport channel not in loopback mode */
  DEC_LOOPBACK_MODE_1_1, /* Loopback mode 1 only CRC passed block */
  DEC_LOOPBACK_MODE_1_2, /* Loopback mode 1 all blocks (CRC pass/fail) */
  DEC_LOOPBACK_MODE_2    /* Loopback mode 2 */
} dec_loop_back_mode_type_enum_type;

typedef enum
{
  /* status FIFO is enpty and can be used */
  DEC_DOB_STATUS_FIFO_EMPTY,
  /* status FIFO belongs tp CCTrCh that need BTFD decision */
  DEC_DOB_STATUS_FIFO_BTFD,
  /* status FIFO points to DOB data that need to be delivered to MAC */
  DEC_DOB_STATUS_FIFO_DELIVER
} dec_dob_status_fifo_state_enum_type;

/* DOB handle information */
typedef struct
{
  /* Link for the queue routines to manage this item on the queue */
  q_link_type link;
  /* index of this element in array */
  uint8 index;

  /* information is valid or not */
  boolean valid;

  boolean mult_tf_pass;        /* Indicator that multiple transport formats*/
                               /* had passing CRCs (BTFD)                  */

  /* State of DOB handle */
  dec_dob_status_fifo_state_enum_type state;
  /* DOB handle from MCAL */
  mcalwcdma_dec_dob_status_handle_type dob_handle;
} dec_dob_status_handle_type;

/* This structure defines the TB information associated with the mDSP
   status FIFO entry */
typedef struct
{
  /* If this TB valid. This valid state reflect only the validity by
    mDSP status FIFO ot DOB TB set status information */
  boolean valid;
  
  /* The index of the TB hdr information in TB set hedder info */
  uint16 tb_hdr_info_in_tbset_index;
} dec_tb_info_status_fifo_struct_type;

/* This structure defines the information required to read the TB data */
typedef struct
{
  /* The TB index in the TB set that is to be read in the following buffer */
  uint8 tb_index;
  
  /* TB hesder info index in TB set */
  uint8 tb_hdr_info_index;
  
  /* The TB buffer pointer(s) in which to read the TB data */
  l1_dl_tb_buf_multi_seg tb_data_buf;
} dec_tb_data_read_info_struct_type;

/* Decoder call back function type.
   This function type is called after any TBset status read */
typedef void DEC_CCTRCH_T_CB_FUNC_TYPE(uint8);

/* This enum defines the BCH or NBCH CCTrCh sub states */
typedef enum
{
     DL_CCTRCH_BCH_OFF_TYPE,          /* BCH or NBCH not doing anything */
     DL_CCTRCH_BCH_INIT_TYPE,         /* BCH or NBCH used to TTI sync   */
     DL_CCTRCH_NBCH_SFN_DECODE_TYPE,  /* NBCH used for SFN decode by L1 */
     DL_CCTRCH_NBCH_SFN_DECODED_TYPE, /* NBCH SFN hs been decode        */
     DL_CCTRCH_BCH_DATA_DL_TYPE,      /* BCH or NBCH set by RRC that is
                                        ready for data delivery */
     DL_CCTRCH_NOT_BCH_TYPE    /* used for all CCTrCh that are not BCH */
} dl_bch_sub_state_enum_type;

/* This enum defines the BCH usage statets that can be set by demod */
typedef enum {
     DL_BCH_USE_OFF,             /* Turn the BCH TB processing off */
     DL_BCH_USE_STATUS_TO_DEMOD, /* Start giving TB set status to demod */
     DL_BCH_USE_DELIVER_TO_MAC   /* start deliverying BCH TB to MAC */
} dl_bch_usage_enum_type;

/* This enum defines the reference type of the CCTrCh */
typedef enum
{
     DL_CCTRCH_SBCH_TYPE,         /* Serving cell BCH CCTrCh set by RRC */
     DL_CCTRCH_NBCH_TYPE,         /* neighbor BCH CCTrCh set by RRC */
     DL_CCTRCH_DCH_TYPE,          /* DCH CCTrCh set by RRC */
     DL_CCTRCH_DSCH_TYPE,         /* DSCH CCTrCh set by RRC */
     DL_CCTRCH_PCH_TYPE,          /* PCH CCTrCh set by RRC */
     DL_CCTRCH_FACH_TYPE,         /* FACH CCTrCh set by RRC */
     DL_CCTRCH_PCH_FACH_TYPE,     /* PCH_FACH CCTrCh set by RRC */
     DL_CCTRCH_HS_DSCH_TYPE,      /* HS DSCH CCTrCh set by RRC and used for cctrch profiling */
     DL_CCTRCH_INVALID,           /* Invalid CCTrCh */
     DL_NUM_CCTRCH_REF_TYPE       /* number of CCTrCh reference types */
} dl_cctrch_ref_type_enum_type;

extern const uint8 cctrch_ref_to_trch_bf_table[DL_NUM_CCTRCH_REF_TYPE];

/* macro to convert the CCTrCh reference type to TrCh BF */
#define DL_CCTRCH_REF_TO_TRCH_BF(x) (cctrch_ref_to_trch_bf_table[x])

/* CCTRCH history profiling. profiles at given instance cctrch is allocated or deallocated */
typedef enum
{
  /* CCTRCH History. CCTRCH Allocated */
  DEC_CCTRCH_ALLOCATED,
  /* CCTRCH History. CCTRCH Deallocated */
  DEC_CCTRCH_DEALLOCATED
} dec_cctrch_hist_alloc_dealloc_enum_type;

/* CCTRCH history profiling. profiles at given instance cctrch is allocated or deallocated as part of 
 * cphy setup req or cell trans req or idle req 
 */
typedef enum
{
  /* CCTRCH alloc dealloc is cphy setup type */
  DEC_CCTRCH_CPHY_SETUP_REQ_TYPE,
  /* CCTRCH alloc dealloc is cell trans type */
  DEC_CCTRCH_CELL_TRANS_REQ_TYPE,
  /* CCTRCH alloc dealloc is cphy setup type */
  DEC_CCTRCH_IDLE_REQ_TYPE
} dec_cctrch_hist_req_mask_enum_type;


/*CCRTCH alloc/dealloc profiling */
typedef struct
{
  uint32 req_mask;
  uint32 curr_time_tick;
  uint8 cfn;
  uint8 cctrch_id;
  dl_cctrch_ref_type_enum_type cctrch_ref_type;
  dec_cctrch_hist_alloc_dealloc_enum_type cctrch_alloc_dealloc;
  dec_cctrch_hist_req_mask_enum_type cphy_setup_or_cell_trans;
}dec_cctrch_alloc_hist_struct_type;


/* define the CCTrCh setup command type */
typedef struct
{
     l1_dl_cctrch_info_struct_type   *dl_cctrch_info; /* CCTrCh info */
     l1_dl_ctfc_info_struct_type     *dl_ctfc_info;   /* CTFC info */
     l1_setup_ops_enum_type          dl_op_type;      /* Phychan operation ADD or CFG*/
     l1_dl_phychan_enum_type         dl_phychan_type; /* ADD/CFG PhyChan type */
     l1_dl_phychan_db_struct_type    *dl_phychan_db;  /* PhyChan info */
     uint8                           drop_cctrch_idx; /* CCTrCh index being dropped */
} dl_cctrch_setup_cmd_type;


/* defines the structure for saving the error information for the
   TB header information required as the part of detailed info
   for debug purpose only */
typedef struct
{
  /* Number of transport blocks in this transport block set. */
  uint8 num_total_tb;
  
  /* CCTrCh Ids for the transport block set */
  uint8 cctrch_id;
  
  /* TrCh Ids for the transport block set */
  uint8 trch_id;
  
  uint8 tfi; /* TF index for this TBset or TrCh */

  uint32 crc_result_bits; /* CRC pass fail result bits for each TB */
  boolean tfci_decode_error; /* TFCI decode error for this TBset */
  
  /* TrCh reference type for the transport block set */
  l1_dl_trch_ref_type_enum_type trch_ref_type;
  
  /* CFN for this transport block set */
  uint8 cfn;
} dl_dec_tbset_hdr_fail_drop_info;

#ifdef FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC
/* call back function type for failed block passing to higher layer info */
typedef uint16 (*DL_DEC_FAILED_BLK_QUERY_CB_FUNC_TYPE)(void);
#endif /* FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* This is the TF table base for the TrCh in a CcTrCh. This is used to convert
   the TFT index returned by mDSP in DOB status FIFO to the TrCh TF index */
extern uint8 dec_tft_base[DEC_TOTAL_CCTRCH_TABLES][DEC_TOTAL_TRCHT_PER_CCTRCH];

/* This array stores the status fifo information. This information is refered
   to by all other function to get the starting address of the DOB. This
   information is also used by header and data reading function to get the
   transport format Id to get the transport block size and number of transport
   blocks */
extern dec_dob_status_handle_type
dec_dob_status_handle[MCALWCDMA_DEC_MAX_DOB_HANDLES];

/* Q for DOB status FIFO */
extern q_type dec_dob_status_fifo_q;
extern q_type dec_dob_status_fifo_free_q;


/* This array stores the number of TrCh of a CCTrCh */
extern uint8 dl_num_trch[DEC_TOTAL_CCTRCH_TABLES];
/* This array stores the number of TF in the TrChs of a CCTrCh */
extern uint8 dl_num_tf[DEC_TOTAL_CCTRCH_TABLES][DEC_TOTAL_TRCHT_PER_CCTRCH];
/* This array holds the TrCh reference type information of the TrChs
   (Tb set for DOB data) */
extern l1_dl_trch_ref_type_enum_type
dl_trch_ref_type[DEC_TOTAL_CCTRCH_TABLES][DEC_TOTAL_TRCHT_PER_CCTRCH];

/* This variable stores CCTrCh table availability status. mDSP has 6 CCTrCh
   tables out of which any 3 can be used as active ones. Other 3 are used
   to reconfigure the 3 currently active CCTrCh without the loss of data */
extern boolean dl_cctrch_avail_status[DEC_TOTAL_CCTRCH_TABLES];

/* This variable contains the CCTrCh table index for the CCTrCh Id passed
   to the setup function. Each CCTrCh has got unique Id and a new CCTrCh
   table index is associated with the new configured CCTrCh. If this is a
   modify request the previous CCTrCh table index is freed from the CCTrCh
   Id. */
extern uint8 dl_cctrch_id_table_index[L1_CCTRCH_ID_COUNT];
/* This variable contains the CCTrCh Ids corresponding to the CCTrCh table
   index. This is used for getting the CCTrCh table Ids from the table index
   passed in the link FIFO as the mDSP version of CCTrCh Id. */
extern uint8 dl_cctrch_table_index_id[DEC_TOTAL_CCTRCH_TABLES];

/* This array keeps the CCTrCh table index to which the TrCh Id passed 
   by RRC, belongs */
extern uint8 dl_trch_id_cctrcht_index[L1_MAX_TRCH_ID + 1];
/* This array keeps the TrCh table index to which the TrCh Id passed 
   by RRC, belongs */
extern uint8 dl_trch_id_table_index[L1_MAX_TRCH_ID + 1];
/* This variable contains the TrCh Ids corresponding to the TrCh table index.
   This is used for getting the TrCh table Ids from the table index passed
   in the link FIFO as the mDSP version of TrCh Id. */
extern uint8 dl_trch_table_index_id[DEC_TOTAL_CCTRCH_TABLES][DEC_TOTAL_TRCHT_PER_CCTRCH];

/* This array holds the coding type information of the TrChs
   (Tb set for DOB data) */
extern l1_dl_coding_enum_type
dl_trch_coding_type[DEC_TOTAL_CCTRCH_TABLES][DEC_TOTAL_TRCHT_PER_CCTRCH];

/* This array holds the crc length information of the TrChs */
extern uint8 dl_trch_crc_length[DEC_TOTAL_CCTRCH_TABLES][DEC_TOTAL_TRCHT_PER_CCTRCH];

/* this array hold the boolean values that indicate that
   1. If the CCTrCh is TFCI case: Then this TrCh has non zero CRC type.
   2. No-TFCI case: This TrCh has all the TF having non zero CRC. That means
                    no TF has 0 number of TB.
   This decision is used at number of places so it is good to calculate it
   once and use it at different places */
extern boolean dl_non_zero_crc_criteria[DEC_TOTAL_CCTRCH_TABLES][DEC_TOTAL_TRCHT_PER_CCTRCH];

/* This array saves the TTI of each TrCh in all CCTrCh */
extern l1_tti_enum_type dl_trch_tti[DEC_TOTAL_CCTRCH_TABLES][DEC_TOTAL_TRCHT_PER_CCTRCH];

/* This array stores the TF information for all CCTrCh . This is required by
   the transport block handling module to get the information about the
   transport block set size and number of transport blocks. */
extern l1_dl_tf_info_struct_type
*dl_tf_info[DEC_TOTAL_CCTRCH_TABLES][DEC_TOTAL_TRCHT_PER_CCTRCH];

/* CFN at which CCTrCh is enabled */
extern uint8 dl_cctrch_enable_cfn[DEC_TOTAL_CCTRCH_TABLES];
/* TTI boundry check pending */
extern boolean dl_trblk_tti_bdry_check_pending[DEC_TOTAL_CCTRCH_TABLES][DEC_TOTAL_TRCHT_PER_CCTRCH];

#ifdef FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC
extern boolean
dl_errored_blk_pass_thro_mode[DEC_TOTAL_CCTRCH_TABLES][DEC_TOTAL_TRCHT_PER_CCTRCH];
#endif /* FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC */

extern rex_timer_type dl_bler_meas_delay_timer;

extern dl_cctrch_ref_type_enum_type dl_cctrch_ref_type[DEC_TOTAL_CCTRCH_TABLES];

extern boolean windup_detected;

/* This array has TF index of TrChs in CCTrCh that has max blocks and is the
   part of TFC table. If there is no TF index existing in that TrCh then value
   is set to -1 */ 

extern int8 dl_dch_trch_max_blk_tf_idx[DEC_TOTAL_TRCHT_PER_CCTRCH];

/* Number of times SFN mismatch is detected */
extern uint32 dl_num_times_sfn_mismatch;

/* Dirty Bitmask to indicate which of the
   CCTrCh needs to be updated in firmware. */
extern uint32 dec_cctrch_dirty_bmsk;

/* CCTRCH allocation deallocation profiling database */
extern dec_cctrch_alloc_hist_struct_type dec_cctrch_alloc_hist[DEC_CCTRCH_HIST_BUFF_MAX_SIZE];
extern uint8 dec_cctrch_alloc_hist_idx;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION wl1_dec_init

DESCRIPTION
  This function initializes the variables related to this module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wl1_dec_init(void);

/*===========================================================================
FUNCTION dec_free_status_handle

DESCRIPTION
This function frees the DOB status FIFO in MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_free_status_handle(
  /* status FIFO index in mDSP to be freed */
  uint16 fifo_index);

/*===========================================================================
FUNCTION dec_set_cctrch_idx_cb_func

DESCRIPTION
  This function register the call back function for CCTtrCh index. If CCTrCh
  index is out of range it will return with no action and MSG_ERROR. If call
  back function pointer is NULL then it is equivalent to deregistration.
  
  The registered functions are called when any TrCh (TBset) status is read.
  These functions are passed with argument of DOB status FIFO index that has
  details about DOB location, CCTrCh's TrCh and TF index, CRC pas fail, TB size
  etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates CCTrCh table index call back function pointer
==========================================================================*/

extern void dec_set_cctrch_idx_cb_func(
  /* CcTrCh index for which call back function need to be registered */
  uint8 cctrch_table_idx,
  /* call back function to be registered */
  DEC_CCTRCH_T_CB_FUNC_TYPE *cb_func);

/*===========================================================================
FUNCTION        DEC_SUBMIT_TBSET_STAT_LOGPKT

FILE NAME       dlcmd.c

DESCRIPTION     This function submits the tbset statistics log pkt.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
//extern void dec_submit_tbset_stat_logpkt(void);

/*===========================================================================
FUNCTION dl_dec_initialize

DESCRIPTION
  This function initialize the decoder sub modules. This is called by L1
  manager at the startup. It performs the following initializations.
  - Sets the total number of CcTrCh used to 0 and initialize the cctrch table
    index for all id to 0xFF, all CCTrCh table Ids for all table index to
    0xFF. Sets the CCTrCh table availability status to TRUE.. Sets the TFCI
    table index availabilty to TRUE and set the TFCI index for CCTrCh table
    index to 0xFF.
  - Calls all the initialization function in decoder sub modules.
  - Set the read done status for header information to TRUE
  - Set the read done status for data information to TRUE
  - Initialize the buffer pointer to the transport block data to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_dec_init(void);

/*===========================================================================
FUNCTION dl_set_ciphering_keys

DESCRIPTION
  This function validates and sets the ciphering key information for downlink
  deciphering operation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE validate and update result.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_set_ciphering_keys(
     /* DL ciphering info structure */
     l1_cipher_key_info_struct_type *dl_cipher_info);

/*===========================================================================
FUNCTION dl_validate_ciphering_key_req

DESCRIPTION
  This function validates the ciphering key information for downlink
  deciphering operation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE validate result.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_validate_ciphering_key_req(
     /* DL ciphering info structure to validate the information */
     l1_cipher_key_info_struct_type *dl_cipher_info);

/*===========================================================================
FUNCTION dec_cctrch_alloc_hist_profile

DESCRIPTION
  This function profiles cctrch allocation and deallocation history to debug related issues.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/

extern void dec_cctrch_alloc_hist_profile(uint8 cctrch_id,
                                          dl_cctrch_ref_type_enum_type cctrch_ref_type,
                                          dec_cctrch_hist_alloc_dealloc_enum_type cctrch_alloc_dealloc);


/*===========================================================================
FUNCTION dl_cctrch_setup

DESCRIPTION
  This function configure or modify the CCTrCh information. This is called
  by L1 manager. It goes through the currently existing CCTrCh Id and if
  CCTrCh Id in this info structure is active it assumes this function call
  as modify request otherwise as new configure request. This function calls
  the functions to calculate the rate matching parameters, rate matched size,
  code block parameters (number and size) and setup mDSP tables.

DEPENDENCIES
  None

RETURN VALUE
  Returns the CCTrCh table index setup. It retuens 0xFF in case of any
  failure. The reason for the failure can be as follows:
   - No free mDSP CCTrCh table exists to update CCTrCh info to mDSP.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 dl_cctrch_setup(
     /* DL CCTrCh setup command packet */
     dl_cctrch_setup_cmd_type *cctrch_setup_cmd_pkt
  );


/*===========================================================================

FUNCTION dl_set_cctrch_enable_state

DESCRIPTION
  This function sets the CCTrCh enable state. Before setting the state of the
  CCTrCh table index it checks if that CCTrCh table index is in use and is
  a valid CCTrCh index.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on success of state change

SIDE EFFECTS
  Updates the state in array element of array dl_cctrch_enabled.
===========================================================================*/

extern boolean dl_set_cctrch_enable_state(
     /* CCTrCh table index for which the enable state is to be set */
     uint8 cctrch_index,
     /* CCTrCh state to be set */
     boolean state,
     /* CFN at which corresponding H/W ch is enabled/diabled */
     uint8 en_dis_cfn);

/*===========================================================================
FUNCTION dl_set_trch_enable_state

DESCRIPTION
  This function sets the TrCh enable state. It need the CCTrCh table index
  and TrCh reference type to enable or disable the state. Before setting the
  state of the TrCh table index it checks if that CCTrCh table index is in
  use and is a valid CCTrCh index.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on success of state change

SIDE EFFECTS
  Updates the state in array element of array dl_cctrch_enabled.
===========================================================================*/

extern boolean dl_set_trch_enable_state(
  /* CCTrCh table index for which the enable state is to be set */
  uint8 cctrch_index,
  /* TrCh reference type */
  l1_dl_trch_ref_type_enum_type trch_ref_type,
  /* CCTrCh state to be set */
  boolean state);

/*===========================================================================
FUNCTION dl_set_trch_idx_enable_state

DESCRIPTION
  This function sets the TrCh index enable state. It need the CCTrCh table index
  and TrCh index to enable or disable the state. Before setting the
  state of the TrCh table index it checks if that CCTrCh table index is in
  use and is a valid CCTrCh index.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on success of state change

SIDE EFFECTS
  Updates the state in array element of array dl_cctrch_enabled.
===========================================================================*/

extern boolean dl_set_trch_idx_enable_state(
  /* CCTrCh table index for which the enable state is to be set */
  uint8 cctrch_index,
  /* TrCh index */
  uint8 trch_index,
  /* TrCh state to be set */
  boolean state);

/*===========================================================================
FUNCTION dl_send_cctrch_dirty_bit_update_cmd

DESCRIPTION
  This function sends a sync command to FW if the CCTrCh dirty bitmask is 
  non-zero. This means that some Trch enable state has changed and FW needs
  to be notofoed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_send_cctrch_dirty_bit_update_cmd( void );

/*===========================================================================
FUNCTION dl_set_trch_state_interf_resel

DESCRIPTION
  This function sets the TrCh enable state based on Resel start or stop

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_set_trch_state_interf_resel(
  /* TRUE: Resel start - TrCh to be disabled 
     FALSE: Resel stop - Trch to be re-enabled */
  boolean state);

/*===========================================================================
FUNCTION dl_validate_cctrch_setup_req

DESCRIPTION
  This function validates the parameters for the CCTrCh set command pacaket.
  In navigates through all the paramaters and check wheather the values are
  valid and with in range.

DEPENDENCIES
  None

RETURN VALUE
  TRUE of FALSE as per the validation result.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_validate_cctrch_setup_req(
     /* DL CCTrCh setup command packet */
     dl_cctrch_setup_cmd_type *cctrch_setup_cmd_pkt,
     /* shared PhCh CCTrCh table index */
     uint8 shared_phch_cctrch_table_idx);

/*===========================================================================
FUNCTION dl_btfd_validate

DESCRIPTION
  This function validates the BTFD related restriction on the CCTrCh.
  These are based on the restriction specified in 25.212 V3.5.0 2000-12
  chapter 4.3.1.

DEPENDENCIES
  None

RETURN VALUE
  TRUE of FALSE as per the validation result.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_btfd_validate(
  /* DL CCTrCh setup command packet */
  dl_cctrch_setup_cmd_type *cctrch_setup_cmd_pkt,
  /* indicate that phychan drop pending is BTFD */
  boolean drop_chan_btfd);

/*===========================================================================
FUNCTION dl_set_bch_cctrch_info

DESCRIPTION
  This function create the BCH CCTrCh info. The BCH CCTrCh command packet
  doesn't come with CCTrCh info from RRC. Also this information is required
  by the interbal BCH setup by L1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_set_bch_cctrch_info(
     /* CCTrCh setup information from RRC. */
     l1_dl_cctrch_info_struct_type   *cctrch_info);

/*===========================================================================
FUNCTION dl_cctrch_setup_bch

DESCRIPTION
  This function setup the a CCTrCh table for initial BCH setup for getting
  the timing and SFN count of the BCH or for the neighbor BCH to get the SFN
  value. This is called demod module.

DEPENDENCIES
  None

RETURN VALUE
  Table index of neighbour BCH, 0xFF if not successful.

SIDE EFFECTS
  Set the boolean dl_send_bch_init_trblk_flag to false.
===========================================================================*/

extern uint8 dl_cctrch_setup_bch(void);

/*===========================================================================
FUNCTION dl_get_cctrch_ref_type

DESCRIPTION
  This function returns the CCTrCh reference type for the CCTrCh table index.

DEPENDENCIES
  None

RETURN VALUE
  CCTrCh reference type of the CCTrCh.

SIDE EFFECTS
  None
===========================================================================*/

extern dl_cctrch_ref_type_enum_type dl_get_cctrch_ref_type(
     /* CCTrCh mDSP table index for which the ref type is to be returned */
     uint8 cctrch_table_index);

/*===========================================================================
FUNCTION dl_gen_cctrch_ref_type

DESCRIPTION
  This function generate the CCTrCh type depending on the TrCh ref types of
  the transport channel contained in it.

DEPENDENCIES
  None

RETURN VALUE
  CCTrCh reference type of the CCTrCh.

SIDE EFFECTS
  None
===========================================================================*/

extern dl_cctrch_ref_type_enum_type dl_gen_cctrch_ref_type(
     /* pointer to CCTrCh info strcuture for which the ref. type is to be
        generated */
     l1_dl_cctrch_info_struct_type *cctrch_info);

/*===========================================================================
FUNCTION dl_get_longest_tti

DESCRIPTION
  This function returns the longest TTI among the TrChs in a CCTrCh. The
  CCTrCh table index is passed as function parameter.

DEPENDENCIES
  None

RETURN VALUE
  l1_tti_enum_type -> L1_TTI_10MS, L1_TTI_20MS, L1_TTI_40MS, or L1_TTI_80MS.

SIDE EFFECTS
  None
===========================================================================*/

extern l1_tti_enum_type dl_get_longest_tti(
  /* CcTrCh table index in mDSP */
  uint8 cctrch_idx);

/*===========================================================================
FUNCTION dl_get_trch_longest_tti

DESCRIPTION
  This function returns the longest TTI among the specific TrChs reference
  type in a CCTrCh. The CCTrCh table index and TrCh reference type is passed
  as function argument. Use ORed constant L1_DL_TRCH_REFTYPE_XXXX_BF defined
  in l1sapcommon.h to pass desired TrCh ref type bit field

DEPENDENCIES
  None

RETURN VALUE
  l1_tti_enum_type -> L1_TTI_10MS, L1_TTI_20MS, L1_TTI_40MS, or L1_TTI_80MS.
  If all of the TrCh BF is not in this CCTrCh then it returns L1_NUM_TTI as
  invalid TTI type.

SIDE EFFECTS
  None
===========================================================================*/

extern l1_tti_enum_type dl_get_trch_longest_tti(
  /* CcTrCh table index in mDSP */
  uint8 cctrch_idx,
  /* TrCh reference type bit field */
  uint8 trch_ref_type_bf);

/*===========================================================================
FUNCTION dl_get_shortest_tti

DESCRIPTION
  This function returns the shortest TTI among the TrChs in a CCTrCh. The
  CCTrCh table index is passed as function parameter.

DEPENDENCIES
  None

RETURN VALUE
  l1_tti_enum_type -> L1_TTI_10MS, L1_TTI_20MS, L1_TTI_40MS, or L1_TTI_80MS.

SIDE EFFECTS
  None
===========================================================================*/

extern l1_tti_enum_type dl_get_shortest_tti(
  /* CcTrCh table index in mDSP */
  uint8 cctrch_idx);

extern l1_tti_enum_type dl_dec_get_trch_idx_tti_val(uint8 cctrch_idx, uint8 trch_idx);

/*===========================================================================
FUNCTION dl_query_dch_cctrch_loopback_status

DESCRIPTION
  This function queries whether any TrCh of the DCH CCTrCh is configured in
  loopback mode. If so it returns TRUE else FALSE. It queries the DL API to
  get the CCTrCh index associated with the DPCH combiner and then cross 
  verifies it with internal decoder database. Looping through all the TrCh 
  of the CCTrCh, it finds out if any TrCh is configured for any kind of 
  loopback mode.

DEPENDENCIES
  This function is supposed to be called only when L1 is in DCH state. If
  not, then it will print out error message that DCH CCTrCh does not exist.

RETURN VALUE
  TRUE: DCH CCTrCh has some TrCh configured for loopback mode
  FALSE: None of the DCH TrCh is in loopback mode.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_query_dch_cctrch_loopback_status(void);

/*===========================================================================
FUNCTION dldec_cctrch_id_exists

DESCRIPTION
  This function checks if there is any CCTrCh table index associated with this
  CCTrCh Id.

DEPENDENCIES
  None

RETURN VALUE
  Boolean status based on CCTrCh table index existance for this Id.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dldec_cctrch_id_exists(
  /* CCTrCh Id to look for its existence */
  uint8 cctrch_id);

/*===========================================================================
FUNCTION dldec_trch_idx_exists

DESCRIPTION
  This function checks if there is TrCh index is valid in that CCTrCh Id. It
  use the CCTrCh Id exists function to check for its CcTrCh index to be valid.
  If valid it gets CcTrCh tabel index and see if TrCh index is within its number
  of TrCh

DEPENDENCIES
  None

RETURN VALUE
  Boolean status based on table index existance for this Id.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dldec_trch_idx_exists(
  /* CCTrCh Id to look for its existence */
  uint8 cctrch_id,
  /* TrCh index in CcTrCh */
  uint8 trch_idx);

/*===========================================================================
FUNCTION dl_get_cctrch_id_table_index

DESCRIPTION
  This function returns the CCTrCh table index for given Id.
  Id index must be valid.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 dl_get_cctrch_id_table_index(
  /* CCTrCh Id */
  uint8 cctrch_id);

/*===========================================================================
FUNCTION dl_get_cctrch_table_index_id

DESCRIPTION
  This function returnd the CCTrCh table index Id.
  Table index must be valid.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 dl_get_cctrch_table_index_id(
  /* CCTrCh table index */
  uint8 cctrch_table_index);

/*===========================================================================
FUNCTION dl_set_cctrch_table_index_id

DESCRIPTION
  This function sets the CCTrCh table index Id.
  Table index must be valid.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_set_cctrch_table_index_id(
  /* CCTrCh table index */
  uint8 cctrch_table_index,
  /* New CCTrCh Id */
  uint8 cctrch_id);

/*===========================================================================
FUNCTION dl_cctrch_free

DESCRIPTION
  This function set the status of the CCTrCh table index to available. It
  also sets the status TFCI table index (if any) associated with this CCTrCh
  index to available. When the CCTrCh is removed or modified the currently
  associated CCTrCh table index is freed after the successful completion of
  the action taking place in the demod module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the CCTrCh has the transport channal of the type BCH_INIT or NBCH it
  sets the corresponding flags to FALSE.
===========================================================================*/

extern void dl_cctrch_free(
     /* CCTrCh index in mDSP tables passed allocated to CCTrCh */
     uint8 cctrch_index);

/*===========================================================================
FUNCTION dl_get_cctrch_index

DESCRIPTION
  This function gets the CCTrCh table entry index associated with CCTrCh Id.
  This function is called by demod block to get the CCTrCh Id for setting up
  or reconfiguring the channels.

DEPENDENCIES
  None

RETURN VALUE
  CCTrCh table index. The possible values are from 0 to 5. Returns 0xFF if
  no such CCTrCh Id exists.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 dl_get_cctrch_index(
  /* CCTrCh id passed by RRC for this CCTrCh */
  uint8 cctrch_id);

/*===========================================================================

FUNCTION dl_cctrch_setup_done

DESCRIPTION
  This function is a callback function that is to be called when the DMA
  transfer to write the CCTrCh setup related information transfer to mDSP is
  complete. This function sends a local command CCTRCH_SETUP_DONE to L1M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_cctrch_setup_done(
  /* table index of CCTrCh setup */
  uint8 cctrch_table_index,
  /* Dummy pointer to avoid featurization */
  void *mbms_info_table
  );

/*===========================================================================
FUNCTION dl_dec_maint_event_handler

DESCRIPTION
  This function do periodic things that are required to be done once
  every 10 ms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_dec_maint_event_handler(void);

/*===========================================================================
FUNCTION dl_trblk_header_read_event_handler

DESCRIPTION
  This function is a event handler function for the transport channel block
  header read event. This function is set in the initialization to be called
  periodically every radio frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_trblk_header_read_event_handler(void);

/*===========================================================================
FUNCTION dl_read_decode_tb_header

DESCRIPTION
  This function is called from TB header read event handler. It perfoems the
  series of steps using the decoder sub module to do the following functions.
  - Get the decoder output buffer status FIFO.
  - Read the transport block set status and pass the CRC information to
    demod if needed (depends on the CCTrCh type).
  - Read the transport block header and get it decoded from MAC to get the
    transport data offset and ciphering information in each transport block.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_read_decode_tb_header(void);

/*===========================================================================
FUNCTION dl_trblk_data_read_event_handler

DESCRIPTION
  This function is a event handler function for the transport channel block
  data read event. This function is set in the initialization to be called
  periodically every radio frame after the header read event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_trblk_data_read_event_handler(void);

/*===========================================================================
FUNCTION dl_read_tb_data

DESCRIPTION
  This function calls the function in decoder sub module to read and update
  L2 buffer for the transport block data in all the transport blocks and
  reset the decob status FIFO. It also sends the indication
  PHY_DL_TRBLK_DATA_IND to L2.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_read_tb_data(void);

/*===========================================================================
FUNCTION dl_decode_bch_sfn

DESCRIPTION
  This function creates the TB hdr and TB data buffer pointers and calls the
  TB read function to get the data. Then it call sthe L3 function to get the
  SFN decoded. Return the decodde SFN.

DEPENDENCIES
  None

RETURN VALUE
  Decoded SFN from L3.

SIDE EFFECTS
  None
===========================================================================*/

extern uint16 dl_decode_bch_sfn(
  uint32 *bch_tb_buf);

/*===========================================================================
FUNCTION dl_nbch_sfn_decode_cmd

DESCRIPTION
  This function transition the sub BCH state of the neighbor BCH of type 
  BCH_NBCH_INIT_TYPE to the NBCH_SFN_DECODE_TYPE. This tells the decoder
  module that next time it see a TB from this TB set, decode the SFN and
  pass it to the demod module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_nbch_sfn_decode_cmd(
     /* the NBCH CCTrCh table index for which the demod is
        asking for SFN to be decoded */
     uint8 cctrch_table_index);

/*===========================================================================
FUNCTION  dl_bch_ctrl

DESCRIPTION
  This function allows for control of the use of the BCH. The BCH can be
  turned off, which disables the decoder events on the currently running
  timeline. It can be set to be enabled but have dec report crc status to
  demod and have the transport blocks discarded, or it can be set to have
  transport blocks delivered to MAC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The decoder module behavior with respect to handling BCH transport blocks
  may be altered.
===========================================================================*/

extern void dl_bch_ctrl(
     /* the BCH CCTrCh table index of the BCH CCTrCh for which the BCH sub
        state is to be setup */
     uint8 cctrch_table_index,
     /* BCH usage type to be swtiched to */
     dl_bch_usage_enum_type bch_usage);

/*===========================================================================
FUNCTION  dl_flush_all_dob_status_fifo

DESCRIPTION
  This function clear all DOB status FIFO element in decoder S/W. This
  function is intended for cleanup purpose like in prep for sleep

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The decoder module behavior with respect to handling BCH transport blocks
  may be altered.
===========================================================================*/
extern void dl_flush_all_dob_status_fifo(void);


/*===========================================================================
FUNCTION  dl_flush_hs_info_from_dob

DESCRIPTION
  This function scans thro' HDOB status FIFO Q and checks if it HS type and
  if HS type then if HS info table index is the one to be flushed. If all
  conditions are met then that DOB status FIO is removed from Q and is freed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_flush_hs_info_from_dob(
  /* HS table index that need to be flushed from DOB status FIFO Q */
  uint8 hs_table_index);


#ifdef FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC
/*===========================================================================
FUNCTION  dl_dec_register_failed_trblk_query_cb_func

DESCRIPTION
  This function registers call back funtion to query transport block
  size of TrCh for which TFCI or BTFD decision have failed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_dec_register_failed_trblk_query_cb_func(
  /* CCTrCh id */
  uint8 cctrch_id,
  /* TrCh Id */
  uint8 trch_id,
  /* function pointer to be registered */
  DL_DEC_FAILED_BLK_QUERY_CB_FUNC_TYPE cb_func);

/*===========================================================================
FUNCTION  dl_dec_get_failed_trch_tf_to_pass_thru

DESCRIPTION
  This function gets the failed TrCh TF index whose block should be passed up.

DEPENDENCIES
  None

RETURN VALUE
  Valid TF index. In case unable to find valid TF index, return invalid index
  as indication of failure.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 dl_dec_get_failed_trch_tf_to_pass_thru(
  /* CCTrCh table index for which to pass the failed TF block to higher layers */
  uint8 cctrch_index,
  /* TrCh table index for which to pass the TF block to higher layers */
  uint8 trch_index);

#endif /* FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC */

/*===========================================================================
FUNCTION  dl_auto_bler_meas

DESCRIPTION
  This function is called by L1 auto bler measure timer signal from l1 task.
  THis is used to calculate the bler for various TrCh.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_auto_bler_meas(uint8 cctrch_index);

/*===========================================================================
FUNCTION  dl_update_mdsp_beta_table

DESCRIPTION

Beta table  values are used to  scale the output of  the combiner. The
turbo decoder is sensitive to this scaling and these values may change
as scaling is optimized. Being able  to change beta scaling on the fly
will help  us test new  values and optimize.  Hence SW supplies  a new
Beta table to MDSP in this function.

Beta Table is stored in FW like:

Table index     BetaTable   (Corresponding to spreading factor)
  0                71              4
  1               100              8
  2                64             16
  3               120             32
  4                81             64
  5               114            128
  6                85            256
  7               114            512

(The values shown above are for example only)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dl_update_mdsp_beta_table(
  /* RxD ON/OFF, 0 = OFF, 1 = ON */
  boolean rxd_state
);

/*===========================================================================
FUNCTION        DL_DEC_SUBMIT_PENDING_LOGPKTS

FILE NAME       dlcmd.c

DESCRIPTION     This function calls functions those submit various 
                         periodic log pkts.

DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS
===========================================================================*/
extern void dl_dec_submit_pending_logpkts(
                     dl_cctrch_ref_type_enum_type cctrch_ref);

/*===========================================================================
FUNCTION mcalwcdma_dl_process_bmc_handle
 
DESCRIPTION
  This function processes the DOB handle to calculate the SFN of sequencer
  and also computes if the frame is wanted.

DEPENDENCIES
  None
  
RETURN VALUE
  Boolean indicating if the frame is wanted.
  
SIDE EFFECTS
  None
===========================================================================*/

extern boolean mcalwcdma_dl_process_bmc_handle(
  /* CTCH channel active */
  boolean* ctch_active, 
  /* SFN of sequencer */
  int32* sfn,
  /* CCTrCh ID from DOB */
  uint8 ctch_cctrch_id,
  /* TrCh ID from DOB */
  uint8 ctch_trch_id,
  /* CFN from DOB */
  uint8 ctch_cfn_value);

/*===========================================================================

FUNCTION DL_DECODE_SFN_PRIME

DESCRIPTION
   This function is meant to be called directly by L1 in order to decode the
   SFN-Prime from a BCH transport block. Since the SFN-Prime is the second
   IE in the transport block, it can easily be extracted and shifted around
   to form the value.

   Assumptions:
     * The processor is big-endian
     * The SFN-Prime is eleven bits long
     * The BCH transport block holds a valid System Information Message
     * Unaligned PER was used to encode the System Information Message
     * The SFN-Prime is the second IE in the System Information message
     * Based on 25.331 V3.2.0 (2000-03)

DEPENDENCIES
   None.

RETURN VALUE
   The unencoded SFN-Prime from the transport block.

SIDE EFFECTS
   None.

===========================================================================*/

unsigned short dl_decode_sfn_prime
(
   void *transport_block               /* Pointer to the BCH transport
                                          block to be decoded */
);

/*===========================================================================
FUNCTION dldec_sbch_sfn_seq_cfn_sanity_check_cmd_handler

DESCRIPTION
  This function checks the sanity of sequencer SFN/CFN. It is intended to be
  used as call back function from decoder for CcTrCh reference type SBCH
  in non DCH state. This call back function assumes that it is called near
  the middle of radio frame (avoid frame boundry by safe distance) and in
  interrupt context (to avoid any significant premetion).

  It gets sequencer SFN and compares it with SFN decoded from PCCPCH if CRC
  has passed on that TTI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Correct sequencer SFN/CFN if found misaligned w.r.t. serving cell PCCPCH
===========================================================================*/
extern void dldec_sbch_sfn_seq_cfn_sanity_check_cmd_handler(
  /* decoder SW DOB status FIFO index */
  uint16 fifo_idx);
  
/*===========================================================================
FUNCTION dl_dec_register_ctch_pdu_decode_error_cb

DESCRIPTION
  This function registers the callback for the PDU decode error indication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_dec_register_ctch_pdu_decode_error_cb(
  /* pointer to the callback */
  DL_DEC_CTCH_PDU_DECODE_ERROR_CB_FUNC_TYPE ctch_pdu_decode_error_cb);

/*===========================================================================
FUNCTION dl_dec_deregister_ctch_pdu_decode_error_cb

DESCRIPTION
  This function deregisters the callback for the PDU decode error indication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_dec_deregister_ctch_pdu_decode_error_cb(void);

#endif /* DLDEC_H */
