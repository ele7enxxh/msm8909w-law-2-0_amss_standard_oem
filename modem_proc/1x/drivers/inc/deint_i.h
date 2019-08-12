#ifndef DEINT_I_H
#define DEINT_I_H

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

$Header: //components/rel/1x.mpss/3.0/drivers/inc/deint_i.h#3

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/13   cjb     Changes to support DEMBACK_1
02/24/12   srk     Mainlined FEATURE_IS2000_P2
09/13/11   srk     Added a function deint_queue_init()
08/15/11   jtm     Klocwork Fixes.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
12/15/10   jtm     Feature cleanup.
12/03/10   jtm     Removed MDR/SCCH support.
09/12/09   trc     Remove unnecessary deint_delete_fch_task_list_for_rc11()
08/05/09   jtm     Lint fixes.
07/07/09   jtm     Lint fixes.
07/06/09   jtm     Mainlined RC11 definition.
06/06/09   vlc     Added support for 1x Advanced (RC11.)
04/01/09   jtm     Added customer.h.
11/19/08   jtm     Merged Common Modem Interface changes from CMI sandbox
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

                     INCLUDE FILES FOR MODULE

===========================================================================*/

//common
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"

//drivers
#include "cai.h"
#include "cai_v.h"
#include "cai_i.h"
#include "dec5000.h"
#include "dec5000_v.h"
#include "deint.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===============================================================
              RC DEFINITION AND CONVERSION MACROS
================================================================*/

typedef enum {
  DEINT_RC_INVALID = 0,
  DEINT_RC1 = 1,
  DEINT_RC2,
  DEINT_RC3,
  DEINT_RC4,
  DEINT_RC5,
  DEINT_RC11 = 11
} deint_rc_type;

#define DEINT_FCH_RC_BASE DEINT_RC1
#define DEINT_SCH_RC_BASE DEINT_RC3
#define DEINT_DCCH_RC_BASE DEINT_RC3

/* This macro is not used for DEINT_RC11 because the deinterleaver is programmed
   by the MDSP, not SW.
*/
#define DEINT_FCH_RC_INDEX(fch_rc)\
  ((((uint16) (fch_rc) - (uint16) DEINT_FCH_RC_BASE) > 0) ? \
                          ((uint16) (fch_rc) - (uint16) DEINT_FCH_RC_BASE): 0)

#define DEINT_DCCH_RC_INDEX(dcch_rc)\
  ((((uint16) (dcch_rc) - (uint16) DEINT_DCCH_RC_BASE) > 0) ? \
                          ((uint16) (dcch_rc) - (uint16) DEINT_DCCH_RC_BASE): 0)

#define DEINT_SCH_RC_GET_INDEX(sch_rc)\
  (((uint16) (sch_rc) == (uint16) DEINT_RC11) ? \
                     3 : ((uint16) (sch_rc) - (uint16) DEINT_SCH_RC_BASE))

#define DEINT_SCH_RC_INDEX(sch_rc)\
  ( ( (  (uint16) (sch_rc) == (uint16) DEINT_RC11 ) ||       \
      ( ((uint16) (sch_rc) >= (uint16) DEINT_SCH_RC_BASE) && \
        ((uint16) (sch_rc) <= DEINT_RC5) )                   \
    ) ? DEINT_SCH_RC_GET_INDEX(sch_rc) : 0                   \
  )

/*Macro for bounds checking arrays*/
#define DEINT_UPPER_BOUNDS_CHECK(index, array_size) \
           ( ((uint16) (index) < (uint16) (array_size)) ? \
                           (uint16) (index) : ((uint16) (array_size) - 1) )
typedef enum {
  DEINT_5MS_FRAME,
  DEINT_10MS_FRAME,
  DEINT_20MS_FRAME,
  DEINT_40MS_FRAME,
  DEINT_80MS_FRAME,
  DEINT_160MS_FRAME,
  DEINT_FRAME_LEN_COUNT
} deint_frame_len_type;

typedef enum {
  DEINT_CH0,
  DEINT_CH1,
  DEINT_CH2,
  DEINT_CH_COUNT,
  DEINT_CH_INVALID = -1
} deint_ch_id_type;


/*===========================================================================
                           MACRO DECLARATIONS
        (to hide chip-specific differences between MSM registers)
=============================================================================*/

/*===========================================================================
                      FUNCTION DECLARATIONS

=============================================================================*/


/*===========================================================================

FUNCTION DEINT_SET_SC_MODE

DESCRIPTION
    This function calls dec_sc_init() which puts the decoder in continous mode
    and resets the SVD.  This function loads the deinterleaver packet parameter
    for SYNC Channel.

DEPENDENCIES
    deint_init() should have been called

RETURN VALUE
  None

SIDE EFFECTS
   The deinterleaver packet parameters for SYNC Channel are loaded in PACKET
   RAM.
=============================================================================*/
void deint_set_sc_mode
(
  void
);

/*==========================================================================
FUNCTION DEINT_SET_PC_MODE

DESCRIPTION
  This function calls dec_pc_init() which puts the decoder in continous
  mode and resets the SVD. This function loads the deinterleaver packet
  parameters for PAGING Channel.

DEPENDENCIES
 deint_init() should have been called.

RETURN VALUE
  None

SIDE EFFECTS
   The deinterleaver packet parameters for PAGING Channel are loaded in PACKET
   RAM.
=============================================================================*/
typedef enum {
  PRAT_9600=0,   /* Paging Channel Rate 9600 bps */
  PRAT_4800,     /* paging Channel Rate 4800 bps */
  PRAT_COUNT
} deint_pc_rate_type;

void deint_set_pc_mode
(
  deint_pc_rate_type pc_rate
);

/*==========================================================================
FUNCTION DEINT_SET_FCH_MODE

DESCRIPTION
    This function calls dec_tc_init() which puts the decoder in packet
    mode and resets the SVD. This function loads the deinterleaver packet
    parameters for FCH of a given Radio Configuration.

DPENEDENCIES
    deint_init should have been called.

RETURN VALUE
  None

SIDE EFFECTS


=============================================================================*/
void deint_set_fch_mode
(
  deint_rc_type fch_rc
#ifdef FEATURE_IS2000_REL_A_SVD
, boolean power_control_channel    /* TRUE if FCH is used as Primary PC chan */
#endif /* FEATURE_IS2000_REL_A_SVD */

);

#ifdef FEATURE_IS2000_REL_A_SVD
/*==========================================================================
FUNCTION DEINT_SET_DCCH_MODE

DESCRIPTION
  This function calls dec_tc_init() which puts the decoder in packet
  mode and resets the SVD (Serial Viterbi decoder.) This function loads the
  deinterleaver packet parameters for DCCH of a given Radio Configuration.

DEPENDENCIES
  deint_init should have been called.

RETURN VALUE
  None

SIDE EFFECTS
  Reset decoders.


=============================================================================*/

extern void deint_set_dcch_mode
(
  deint_rc_type dcch_rc,
  boolean power_control_channel    /* TRUE if DCCH is used as Primary PC chn */
);
#endif /* FEATURE_IS2000_REL_A_SVD */

/*==========================================================================
FUNCTION DEINT_SET_SCH_MODE

DESCRIPTION
    This function calls dec_tc_init() which puts the decoder in packet
    mode and resets the SVD. This function loads the deinterleaver packet
    parameters for FCH of a given Radio Configuration.

DPENEDENCIES
    deint_init should have been called.

RETURN VALUE
  None

SIDE EFFECTS


=============================================================================*/
void deint_set_sch_mode
(
#ifdef FEATURE_IS2000_REL_A //32X
  uint8   num_ltu,
  uint16  ltu_len,
  dec_mux_pdu_type_type pdu_type,
#endif /* FEATURE_IS2000_REL_A */

  deint_rc_type sch_rc,
  dec_coding_type
  dec_coding,
  dec_sch_rate_type sch_rate
);

#ifdef FEATURE_IS2000_REL_A_SVD
/*=========================================================================
FUNCTION deint_delete_fch_dcch_task_list

DESCRIPTION
    This function deletes the FCH and DCCH tasks from the deinterleaver task
    RAM.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void deint_delete_fch_dcch_task_list
(
  void
);

#endif /* FEATURE_IS2000_REL_A_SVD */

/*=========================================================================
FUNCTION DEINT_DELETE_SUPPLEMENTAL_TASK_LIST

DESCRIPTION
    This function deletes the sch task from the deinterleaver task ram.
    The SCH task is the first task of the second 5ms slot.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
void deint_delete_supplemental_task_list
(
  void
);

/*=========================================================================
FUNCTION DEINT_PROCESS_SEQUENCING_CMD

DESCRIPTION
  This function checks the deint cmd_q to see if there are any pending
commands to program the read parameters corresponding to FCH/SCH.

DEPENDENCIES

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
void deint_process_sequencing_cmd
(
  void
);

#ifdef FEATURE_IS2000_REL_A
/*=========================================================================
FUNCTION deint_atmr_isr2

DESCRIPTION
  This function is invoked once a frame, at the 4th bin.  This function
  support changes for 6100 release A Turbo SCH.  These changes are the result
  of moving the Turbo SCH dummy task to the 3rd bin to give the Decoder more
  time to decode 32X turbo data rate.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void deint_atmr_isr2
(
  void
);

/*=========================================================================
FUNCTION deint_atmr_isr2_init

DESCRIPTION
  This function initializes variables used in deint_atmr_isr2().

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void deint_atmr_isr2_init
(
  void
);
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_DEINT_RAM_LOGGING
#error code not present
#endif

/*============================================================================
FUNCTION DEINT_SET_DINT_CFG

DESCRIPTION
  Configure the DINT_CFG register to point the L, M, N, and P RAMs to the
  proper physical memory pages and channels.

DEPENDENCIES
  The Dec clocks must be enabled by caller.

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void deint_set_dint_cfg ( void );

/*============================================================================

FUNCTION DEINT_QUEUE_INIT

DESCRIPTION
  Initializes the queue.
  Calls q_init to allocate memory for queue mutex.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void deint_queue_init( void );

#endif /*DEINT_I_H*/
