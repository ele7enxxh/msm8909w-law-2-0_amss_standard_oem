#ifndef SRCH_HW_T_I_H
#define SRCH_HW_T_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E A R C H E R    H A R D W A R E    T Y P E S

            H E A D E R F I L E

GENERAL DESCRIPTION
   This file contains constants and type definitions that pertain to search 1
   searching as well as other hw related operations.

  Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc. 
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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/srch_hw_t_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/14   bph     KW errors
01/23/12   srk     Feature Cleanup
01/06/12   ppr     Mainlined AlwaysOn FEATURE_DH feature
11/01/10   jtm     Modem 1x SU API changes.
12/03/09   vks     Moved srch_sect_parm_type and srch_sect_result_type from
                   srch_hw_t.h to srch_srch4.h. Include srch_srch4.h
08/13/09   jtm     Removed deprecated clk.h include.
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Cleaned up incs and eliminated implicit includes.
03/26/09   adw     Removed T_MSM6700 featurization and unused defines.
02/10/09   adw     Merge from 2h08 branch, based on VU_MODEM_1X_SRCH.13.00.01
12/15/06   tjc     Modified traffic to use the sector module
11/19/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_SRCH.12.00.13
08/21/06   aps/awj removed references to trunc parameter
08/21/06   va/rkc  Added antenna, changed max_inx to max_pos in the PPM Pilot
                   struct type.
07/03/06   awj     Moved pilot_rec_type to sect parms, removed parm translation
06/08/06   awj     Remove max_inx and max_eng from sector structures
05/31/06   awj     Further sect and srch4 updates
05/12/06   awj     Updated sector structure.
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
01/16/06   awj     Lint cleanup
08/08/05   ejv     Remove FEATURE_SRCH_4_SHOULDERS.
07/28/05   ejv     Extend lecio support throughout SRCH task.
05/31/05   rng     Merged in from SHDR Sandbox.
05/19/05   ejv     Set SRCH_MIN_PEAKS to 6 for 6700+.
04/01/05   kwo     Added finger filter
04/01/05   sfm     Added last_threshold to srch_sect_struct_type
12/15/04   kwo     Rework of DH interface
12/08/04   jcm     Removed unused config member from srch_sect_struct_type
12/07/04   sst     Added win_pos_offset field to sect structure
12/02/04   bt      Add last_time_seen_energy to sector structure.
10/28/04   kwo     Added lecio_biased and last_ota_win_searched
09/09/04   ejv     Remove REL_A and REL_C_DV featurization.
09/04/04   jcm     Moved srch_hw types to srch_hw_t.h

==========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"

/* Srch */
#include "srch_hw_t.h"
#include "srch_rx_t.h"
#include "srch_rx_t_i.h"
#include "srch_srch4.h"
#include "srchmc.h"
#include "srchmc_v.h"
#include "srchmc_i.h"

/* Feature Dependent */
#ifdef FEATURE_SEC
#include "secutil.h"
#endif /* FEATURE_SEC */


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
                   Searcher demod parameter type

  This structure is used throughout Search to carry around
  and load some of the common demodulation parameters for the searcher.
--------------------------------------------------------------------------*/

typedef struct
{
  int4  win_siz;
    /* Size of the search window in 1/8 chip units (Used mainly in ACQ) */

  byte  gain;
    /* Gain in the searcher hardware */

  word  chps;
    /* Number of chips to integrate coherently */

  byte  nc_int;
    /* Number of integrations to add non-coherently */

  word  thresh;
    /* Threshold to accept a window (used in ACQ only) */
}
srch_sect_parm_struct_type;




/*--------------------------------------------------------------------------
                Declaration of ISR structure variables
--------------------------------------------------------------------------*/

typedef struct
{
  word  phase80;
    /* 0,1,2 count of 26ms periods within an 80ms */

  dword  tick26;
    /* Overall count of 26.66.. ms roll periods */
}
srch_roll_isr_vars_struct_type;
  /* PN roll ISR variables */

/*------------------------------------------------------------------------*/

typedef struct
{
  int4 current_pos;
    /* Current "reported" searcher position */

  int4 raw_pos;
    /* Raw searcher position */

  int4 expected_pos;
    /* Expected searcher position */

  int2 pn_offset;
    /* The cumulative offset PN mask */

  int4 pos_offset;
    /* The cumulative offset position in chipx8 */

  int2 offset_nghbr_slew;
    /* The amount of slew required for neighbor search offset */
}srch_position_data_type;


/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  X                                                                  X
  X     Temporary placement of security and rf functions...          X
  X     hopefully to be moved soon to their own files/subsystems     X
  X                                                                  X */

#ifdef FEATURE_SEC
/*---------------------------------------------------------------------------
                 Variables pertaining to rand buffer fill
---------------------------------------------------------------------------*/

#define SRCH_SEC_TEMP_BUF_SIZE 64

typedef struct
{
  byte    fill_buf[ SRCH_SEC_TEMP_BUF_SIZE ];
    /* Data to merge into random number buffer */

  byte    *ptr;
    /* Pointer used to insert data into fill_buf */
}
srch_sec_type;
  /* Random number generator for SECurity features. */

extern srch_sec_type srch_sec;

#define SRCH_SEC_RND_INIT( ) srch_sec.ptr = srch_sec.fill_buf;
  /* This macro initializes "ptr" for putting data in the "fill_buf" */

#define SRCH_SEC_RND_DATA(x) \
  if(!secutil_rnd.done) { *srch_sec.ptr++ = (byte)(x); }

#define SRCH_SEC_RND_DONE( ) \
  secutil_fill_rnd_data(srch_sec.fill_buf, \
  ((srch_sec.ptr-srch_sec.fill_buf) > SRCH_SEC_TEMP_BUF_SIZE) ? \
  SRCH_SEC_TEMP_BUF_SIZE : (srch_sec.ptr - srch_sec.fill_buf) );
  /* This macro adds the fill buffer to the random number buffer */

#else /* !FEATURE_SEC */

#define SRCH_SEC_RND_INIT( )
#define SRCH_SEC_RND_DATA(x)
#define SRCH_SEC_RND_DONE( )
  /* No-Op */
#endif /* FEATURE_SEC */


/*---------------------------------------------------------------------------
                 Variables pertaining to rand buffer fill
---------------------------------------------------------------------------*/

extern byte srch_rand_buf[ ];
  /* This is the buffer where we will store the data */



/*X                                                                  X
  X      End of temporary stuff                                      X
  X                                                                  X
  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/

#endif /* SRCH_HW_T_I_H */
