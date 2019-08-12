#ifndef DEC_H
#define DEC_H

/*============================================================================
                                  D E C . H
DESCRIPTION

Copyright (c) 2000 - 2011 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dec.h_v   1.28   01 May 2002 09:17:28   yshi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wdec.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
03/14/11    ms      Mainlined MSMHW_DEC_REV_6275, FEATURE_6275_COMPILE,
                    FEATURE_WCDMA_MDSP_INTERFACE_REV_6275, MSMHW_DEC_REV_6280,
                    FEATURE_WCDMA_MDSP_INTERFACE_REV_6280
08/23/10    ks      Removed inclusion of mdsp header for Genesis (featurized)
01/25/08    mc      Obsoleted the file for MCAL
12/11/07    mc      Featurized decoder DRM and DOB size as per UE capability. 
                    Eliminates runtime query of the UE capability. 
09/10/07    mc      Added support for GCF TC 14.6.x
06/27/07    mc      Added support for burst read
06/20/06    bd      Renamed global vars/functions to avoid conflict with 1x names
06/12/06    gs      Fixed DOB size for MSM6260/55A/45
05/26/06    gs      Featurized TF Id interface change using #define of
                    variable exported by mDSP
05/22/06    gs      Change TF Id in mDSP DOB status FIFO bit field
05/10/06    mc      Added runtime detection for MSM6280 and MSM6260.
03/22/06    gs      Defined internal definition of loopback modes
10/14/05    gs      Deciphering regiter bit width, bit position and other
                    related changes
09/14/05    gs      Updated DRM buffer size, DOB size, TrCh address bit mask
                    for 6280
06/22/05    scm     Add ctch_sfn to dec_dob_status_fifo_struct_type.
06/03/05    gs      Updated tb_hdr_info_in_tbset_index to UINT16 to mitigate
                    lint error in structure dec_tb_info_status_fifo_struct_type
04/18/05    gs      Merged code from 6275 baseline
01/11/05    kp      Added code under feature FEATURE_WCDMA_PC_MULT_CRC_PASS to treat 
                    to treat multi CRC pass blocks as good frames for OLPC Algorithm
11/04/04    vb      Code changes for flushing the periodic logpakts to diag
                    after tearing down the physical channels
10/11/04    gs      Removed featurization of number of CCTrCh tables to 4.
                    L1 never use more than 4 CCTrCh table.
                    Added macros, enum, structure definitions for supporting
                    HSDPA
08/06/04    gs      Enabled feature FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC if
                    feature FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING is defined.
01/14/04    gs      Added macro for updating deciphering continue bit
12/22/03    gs      Typedefed function type DEC_CCTRCH_T_CB_FUNC_TYPE
                    Externalized function dec_set_cctrch_idx_cb_func
11/12/03    m       Power control improvements for BTFD calls
06/03/03    yus     Add HWIO_ADDR macro to convert register name to its 
                    address.
04/29/03    gs      Set the DOB status FIFO size 32/16 based on the existance
                    of macro MDSP_dbackDobStatusFifoSize in mdspext.h
02/24/03    gs      Externalized variable dec_num_dob_status_fifo_rece
01/28/03    m       Linted code
08/09/02    gs      Replaced for loop in mDSP MSMC write for delay to clk_busy_wait
                    to make it independent of CPU speed, memory type etc.
05/01/02    yus     Changed FEATURE_MSM5200c to MSMHW_DEC.
02/28/02    gs      1. Corrected the TFCI table index BitMask for CCTrCh table
                    2. Externalized the variable for DL TBset status log packet
                    3. Merged the changes for MSM5200C

11/15/01    gs      1. Externalized the global dec_last_rece_mdsp_status_fifo_info.

11/02/00    asr     1. Increased DEC_MIN_TB_EGY_MATRIC_THRESH from 100 to 256
09/14/01    gs      1. Externalized variable dec_last_read_mdsp_fifo_result and
                       Updated the comments.
                    2. Defined the macro DEC_MIN_TB_EGY_MATRIC_THRESH for minimum
                       acceptable energy threshold.

09/11/01    gs      1. Moved the structure member CRC data bits array pointer
                       to the end of structure.

08/21/01    gs      1. Added the CRC data bits array pointer to the DOB status
                       FIFO structure.

07/17/01    gs      1. Corrected the macro that sets the Deciphering parameters.

06/29/01    gs      1. Added the macro to define the MAX DOB size.
06/29/01    gs      1. Externalized the Decob cfg shadow register var.
                    2. Fixed the Decob cfg problem. Decob cfg register is
                       readonly. To set the bit fields register was read back.
                       Changed that to use shadow register values.

06/18/01    gs      1. Externalized the function dec_read_dob_data.
                    2. Optimized the MDSP MEMC write delay loop.

05/29/01    gs      1. Merged the changes from branch 1.14.1.1 to main line code

05/21/01    jrs     Defined away DEC_WRITE_MEMC for FEATURE_AL1 builds.

05/02/01    jrs     Added types to the DMA_OUT macro.

05/11/01    gs      1. Added the macro to get the TTI alignment at given CFN.
                    2. Added enum for DOB status FIFO state.
                    3. Added structure member state in mdsp status FIFO info
                       structure.
                    4. Added structure for TB set status info. Added the member
                       mdsp status FIFO index in TB set status structure.
                    5. Externalized the variables dec_mdsp_dob__status_fifo_head_idx,
                       dec_last_read_status_fifo_valid and dec_last_read_fifo_info_idx.
                    6. Changed the prototype defijition of the functions as per
                       changes in source file.

04/19/01    sh      1. Changed FEATURE_VST to FEATURE_AL1

04/19/01    gs      1. Added the macro to write to MEMC of mDSP

03/29/01    gs      1. Added the structure member tfci_decode_fail in structure
                       dec_dob_status_fifo_struct_type.
                    2. Added the macro to get the bitmask/shift/register index
                       for TFCI decode fail info from mDSP DOB status FIFO.

01/05/01    gs      1. Defined the macros for the TURBO BETA SELECT index values.
                    2. Removed the mDSP status FIFO structure member tb_hdr_read_skip.

12/21/00    gs      1. Added the defines for masks for MDSP destination address 
                       and Tx type to MDSP.
                    2. Changed the header file name l1rrcctrl.h to l1rrcif.h

12/11/00    gs      1. Externalized the dec_tft_base array to store the TFT
                       base for TrChs in CCTrCh. This information is filled
                       by dldec module during CCTrCh setup.

11/29/00    gs      1. Externalize the dec_crc_included_in_tb_data global
                       variable defined in dec.c

11/20/00    gs      1. Changed the DBACK_DECOB_INT_CLEAR to DBACK_DECOB_STATUS_CLEAR
                       status as per new register name changes

11/09/00    gs      1. Added the macros for DBACK DOB config register components
                       shifts, masks and macros to set their values.
                    2. Added  the enum for Endian type.
                    3. Changed function declaration for dec_set_ciphering_key
                       to accept the byte array instead of word32 array.

10/31/00    gs      Externalized the dec_tbset_status for getting the energy
                    of TBset
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "mcalwcdma_dec.h"

#endif /* DEC_H */
