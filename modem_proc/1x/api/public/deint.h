#ifndef DEINT_H
#define DEINT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         M O B I L E   CHANNEL DEINTERLEAVER    S E R V I C E S

GENERAL DESCRIPTION
  Library of routines to initialize, and operate on, the Qualcomm CDMA
 Channel Deinterleaver chip

  Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
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

============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/vcs/deint.h_v   1.14   26 Aug 2002 16:41:06   sanjayr  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/deint.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/09   jtm     Removed FEATURE_1X_DISABLE_CMI.
04/08/09   adw     Added temporary CMI-lite featurization.
03/20/09   jtm     Cleaned up header file inclusions.
03/04/09   adw     Removed the temporary _v.h include.
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_DRIVERS.00.00.06
07/21/08   jtm     Fixed Klockwork errors. Also added DEINT_UPPER_BOUNDS_CHECK macro.
07/16/08   jtm     Removed COSIM_TEST. Modified DEINT_FCH_RC_INDEX and
                   DEINT_SCH_RC_INDEX to perform bounds checking.
05/21/08   vlc     Removed extra new line characters, cosmetic changes only.
03/31/03   bn      Make function deint_allocate_CH0_RAM becomes public
04/21/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
05/06/04   sr      Moved out DV driver code to separate file
04/13/04   sr      Replaced num_pdcch with hw_test_mode input parameter to
                   deint_set_pdcch_mode().
09/03/03   dna     Ported for MSM6500
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
02/25/03   vlc     Changes are for MSM6100, Release A:
                   Added function prototypes for deint_atmr_isr2 () and
                   deint_atmr_isr2_init ().
08/26/02   sr      Added deint_bcch_seq_repetition()
06/12/02   vlc     Removed prototype for unused functions
                   deint_delete_fch_task_list() and deint_delete_dcch_task_list().
06/04/02   vlc     Added prototype for function deint_delete_fch_dcch_task_list(),
                   deint_delete_fch_task_list(), and deint_delete_dcch_task_list().
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up
                   (feature FEATURE_IS2000_REL_A_VP2.)
02/25/02   sr      Merged in Release A common channels from the branch
10/04/01   sr      Defined PRAT_COUNT and undid some cosmetic changes
09/25/01   lad     Moved internal constants and macros to deint.c.
08/10/01   lh      Added 32X support.
07/04/01   sr      Added #defines for SCH RC 3, 4, 5 1X SCH deinterleaver
                   parameters
02/20/01   sr      Added support for 32X
12/21/00   tc      Fixed the forward link turbo code.
10/24/00   rmd     Moved bits to msm5XXXbits.h.
09/06/00   djd/et  Added ifdef MSMHW_TURBO_CODING for turbo coding software.
09/06/00   rm      Adde clock regime (masks ) changes for MSM5105.
07/12/00   rm      Added support for Multiframe interleaving.
05/17/00   rm      Added deint_create_dummy_sch_task_list().
05/10/00   rm      Added rc parameter in deint_create_sch_task_list fn.
04/21/00   hrk     Added DEINT_RC_INVALID entry to deint_rc_type.
03/23/00   bgc     Changed yamamoto threshholds for RC1 and RC2 to
                   [1, 1, 3, 3] for [full, half, quarter, eighth].  These
                   are the same thresholds as MSM3000 for RC1 and RC2
02/16/00   eh      Updated LTU CRC LEN for MSM5000B.
01/28/00   rm      Added in support for 1X SCH (RC3, RC4 & RC5).
01/24/00   rm      Added in support to sequence read/write parameters. The
                   write parameters are programmed one frame ahed of the read
                   parameters. This will allow us clean transiton if SCH rates
                   change or a change in RC, without loss of a frame.
11/15/99   rm      Added in support for RC5 SCH (convolutional codes).
11/09/99   rm      Added in support for RC3,RC4 & RC5 SCH (TURBO DECODER).
10/17/99   rm      Added in support for IS95B Supplemental Code Channel.
09/24/99   rm      Added in support for SCH (Rc3 & RC4).
08/11/99   rm      Added support for RC4 and RC5 for F-FCH.
07/31/99   rm      Created file. Interfaces to support channel deinterleaver.

============================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*============================================================================

FUNCTION DEINT_INIT

DESCRIPTION
  Enables the DEC clock regime and selects CHIPX8 clock.
  Calls dec_init to initialize the SVD Polynomials.

DEPENDENCIES
  DEC Clock regime should have been enabled.

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void deint_init
(
  void
);


#endif /* DEINT_H */
