#ifndef DLDEC_H
#define DLDEC_H

/*============================================================================
                           D L D E C . H
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dldec.h_v   1.33   11 Jul 2002 22:14:10   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/dldec.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
01/19/12    zr      Mainlined FEATURE_MCAL_WCDMA
05/08/09   sup    Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                    srchlog.h to wdec.h, wenci.h, wl1m.h, wsrch.h, wsrchi.h, wsrchlog.h
01/25/08    mc      Obsoleted the file for MCAL
08/20/07    kps     Expose dl_num_times_sfn_mismatch for diag
06/18/07    mc      Added prototype to query DCH CCTrCh loopback status.
03/16/07    mc      Changed prototype of dl_update_mdsp_beta_table to 
                    include RxD current state.
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
#include "wl1dec.h"

#endif /* DLDEC_H */
