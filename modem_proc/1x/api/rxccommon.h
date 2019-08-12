#ifndef RXCCOMMON_H
#define RXCCOMMON_H
/*===========================================================================

             C D M A    R E C E I V E   C O M M O N   M O D U L E

DESCRIPTION
  This header file contains definitions for the receive common module that
  are used by the rxc unit.  This header file is internal to the rxc unit.

Copyright (c) 1997 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/rxccommon.h_v   1.13   27 Aug 2002 09:20:18   vlechan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/rxccommon.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/14/11   jtm     Save frame rate stats along with FER and SER statistics.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
10/20/10   jtm     Integrated from rel/1h10.
08/06/10   jtm     Removed declaration of fwd_rc_to_rate_set. Added declaration
                   of rxc_get_fwd_link_rate_set().
07/20/10   jtm/adw Added rxc_prepare_rx_to_sleep().
06/09/10   jtm     Remove temporary 1xA Log Code definitions.
05/07/10   jtm     Added API to update and retrieve F-FCH SER and FER stats.
03/30/10   vks     Use 1X specific feature - FEATURE_MODEM_1X_SUPPORTS_Q6
                   instead of FEATURE_QDSP6
02/04/10   jtm     Removed FEATURE_PLT.
01/10/10   vks     Use FEATURE_QDSP6 instead of T_QDSP6.
11/25/09   jtm     Created temporary define for LOG_1X_ADV_INFO_C.
08/22/09   jtm     Lint Fixes.
08/05/09   jtm     Lint fixes.
06/11/09   jtm     Replaced FEATURE_1X_Q6_CRIT_SECT with T_QDSP6.
04/13/09   adw     Replaced tdso.h with tdso_v.h include for CMI.
04/01/09   jtm     Added customer.h
03/19/09   jtm     Updated included headers.
03/17/09   jtm     Replaced INTLOCK/INTFREE, INTLOCK_SAV/INTFREE_SAV, and
                   MSM_LOCK/MSM_UNLOCK with macros that can be mapped correctly
                   based on the compiler and controlled internally to RXC.
11/19/08   jtm     Split various header files into internal and external
                   declarations for Common Modem Interface.
07/21/08   jtm     Added RXC_UPPER_BOUNDS_CHECK. Removed FEATURE_IS2000_REL_C_DV code.
05/03/07   cab     Moved to new sleep voting interface
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
04/20/04   jrp     Fixed lint warnings.
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
03/03/04   sr      Changes for DV
08/08/03   vlc     Use dec_dcch_status_type instead of dec_fch_status_type for
                   rxc_dcch_status.
07/18/03   vlc     Release A changes:  Changed FCH and DCCH
                   channel set up to better indicate when decoder data is
                   available for the appropriate channel(s) for the RXC layer.
06/16/03   bn      Fixed the "out of buffer dec_free_q" problem.
05/05/03   bn      Added new frame skipping field into rxc_slot_type to handle SMMC
08/27/01   vlc     Changed power measurement report process for VP2.
08/08/02   vlc     Fixed compiler error when FEATURE_IS95B_MAHHO is turned off.
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up
                   (feature FEATURE_IS2000_REL_A_VP2.)
02/27/02   lad     Added rxc_clear_decoder_queue().
02/25/02   bn      Added support for  MUX SVD signaling
10/30/01   bn      Added support for Release A FDCH logging
10/17/01   lad     Added inclusion of atmr.h and mccrx.h.
08/15/01   sr      Put back prototype for rxc_init_action_item(); it is called
                   in rxcpaging.c
08/02/01   lcc     Removed function prototypes for functions used only in
                   traffic channel.
06/15/01   day     Mainlined FEATURE_IS95B, FEATURE_IS2000
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2
01/29/01   bgc     Moved rxc_scr_trans_cmd() to rxctraffic.c
11/10/00   jc      Created after repartition of rxc.c

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

//common
#include "comdef.h"

//mux
#include "rxc.h"

//drivers
#include "dec5000.h"

//cp
#include "mccdma.h"
#include "mccdma_v.h"

//other
#include "rex.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*Structure to return frame and symbol error count and total to client */
typedef struct
{
  uint32 fch_sym_errors;   /*Accumulated symbol errors from all FCH traffic frames since call start or statistics reset */
  uint32 fch_sym_total;    /*Accumulated symbol count from all FCH traffic frames since call start or statistics reset */
  uint32 fch_frame_errors; /*Accumulated frame errors from all FCH traffic frames since call start or statistics reset */
  uint32 fch_frame_total;  /*Accumulated frame count from all FCH traffic frames since call start or statistics reset */
  uint32 fch_rate_cnt[5];  /*Accumulated frame rate count for FCH traffic since call start or statistics reset. */
} rxc_fer_ser_report_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION RXC_GET_FER_SER_REPORT

DESCRIPTION
  Cumulative Symbol Error retrieval API.

DEPENDENCIES
  None

RETURN VALUE
  param[out] client_data       Client held data structure to be populated with frame and symbol errors and total counts
  param[in]  reset_statistics  Pass TRUE to reset frame and symbol errors and total to zero after returning current values

  retval 0 indicates success
  retval non-zero indicates failure

SIDE EFFECTS
  None
===========================================================================*/
int rxc_get_fer_ser_report( rxc_fer_ser_report_type *client_data, boolean reset_stats );

#endif /* RXCCOMMON_H */
