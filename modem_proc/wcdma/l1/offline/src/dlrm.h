#ifndef DLRM_H
#define DLRM_H

/*============================================================================
                                  D L R M . H
GENERAL DESCRIPTION
  This module implements the function that are required for the downlink
  operation. The function included in this module control the operation of
  demodulator front and back end.

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dlrm.h_v   1.4   30 May 2001 12:50:50   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlrm.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
01/28/12    vs      Nikel feature cleanup.
05/31/11    geg     Changes for RRC and L2 api files clean-up wcdma dependencies
05/08/09    sup     Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                    srchlog.h to wdec.h, wenci.h, wl1m.h, wsrch.h, wsrchi.h, wsrchlog.h
06/10/08    ms      Function and variable names are changed.
01/25/08    mc      Added support for MCAL
02/22/07    mc      Increased num coded bits per TTI to 32 bits to support
                    80msec TTI
05/29/01    gs      Updated the externalization of optimized array declaration.

05/11/01    gs      Externalized the dl_rm_db_tfi when L1-RRC interface use
                    TFI instead of CTFC.
04/19/01    gs      Updated the declaration of TFI DB table array for the
                    feature to use CTFC or TFI for a TFC.
03/29/01    gs      Externalized the dl_rm_delta_n_tti variable for updating
                    log packet information.
11/02/01    gs      Checked in file in to PVCS                    
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "mcalwcdma_deci.h"
#include "mcalwcdma_dec.h"

#include "l1rrcif.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Number of milliseconds in one radio frame. */
#define DLRM_MILLISEC_PER_RADIO_FRAME 10

/* Maximum code block size for convolutional coding. */
#define DLRM_MAX_COV_CODE_BLK_SIZE    504
/* Maximum code block size for turbo coding. */
#define DLRM_MAX_TURBO_CODE_BLK_SIZE  5114

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* This stores the TFI for all CTFCs in this CCTrCh. This is used primarily
   for calculating the rate matching parameter in flexible position mode. The
   database is stored as TFI for each TrChs in CCTrch for a given CTFC. The
   first index of the array points to the CTFC and second points to the desired
   TrCh. */
extern uint8 *dl_rm_db_tfi[DEC_TOTAL_CTFC_ENTRIES_PER_TABLE];

/* This array stores the delta N per TTI values for all TF in each TrCh. In this
   2 dimensional array the first index points to the TrCh and second index points
   to TF. This value is a signed value.  In case of fixed position RM calculation
   only the element at index 0 of the TF has delta N for that TrCh. */
extern int32 dl_rm_delta_n_tti[MCALWCDMA_DEC_TOTAL_TRCHT_PER_CCTRCH][DEC_TOTAL_TFT_PER_CCTRCH];

/* This variable stores the structure for the rate matching parameters */
extern dec_rm_param_struct_type *dl_rm_rm_params[MCALWCDMA_DEC_TOTAL_TRCHT_PER_CCTRCH];


/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION dl_rm_calc_rm_param

DESCRIPTION
  This function calculates the rate matching parameters. Before calculating
  the RM parameters the various functions should be called to calculate the
  required values like code block parameters, P value, TFI from CTFC values
  etc. This function calculates the following optimized rate matching and
  other parameters.
  - DRM scale factor
  - ePlusOverA
  - eMinusOverA
  - Delta N Odd
  - rate matched size per radio frame

DEPENDENCIES
  Code block parameter calculation and conversion of CTFC values to TFI
  should be done prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  Updates the global variables for the rate matching parameters.
===========================================================================*/

extern void dl_rm_calc_rm_param(
  /* number of bits that can be in the physical channel(s) that are
    associated with this CCTrCh */
  uint32 physical_channel_bits,
  /* CCTrCh setup information. */
  mcalwcdma_dec_cctrch_info_struct_type *cctrch_info,
  /* TFCS information. */
  mcalwcdma_dec_ctfc_info_struct_type   *ctfc_info,
  /* Pointer to CCTrCh table information structure */
  dec_cctrch_table_info_struct_type *cctrch_table_info);

/*===========================================================================
FUNCTION dl_rm_set_value_no_rm

DESCRIPTION
  This function sets the following values in case there is no rate matching.
  - Number of radio frame per TTI
  - Number of coded bits per radio frame

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_rm_set_value_no_rm(
  /* CCTrCh setup information. */
  mcalwcdma_dec_cctrch_info_struct_type *cctrch_info);

/*===========================================================================
FUNCTION dl_rm_calc_delta_n_flexpos

DESCRIPTION
  This function calculates delta N value for all TF of all TrCh of a CCTrCh for
  flexible position rate matching parameter calculation. In addition to this
  in the process of calculating the delta N it calculates following global
  variables that are used for calculating the RM parameter values.
  - Number of radio frame per TTI
  - Number of coded bits per radio frame
  - Delta N TTI
  - Delta N radio frame

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates the delta N values in global variable.
===========================================================================*/

extern void dl_rm_calc_delta_n_flexpos(
  /* CCTrCh setup information. */
  mcalwcdma_dec_cctrch_info_struct_type *cctrch_info,
  /* TFCS information. */
  mcalwcdma_dec_ctfc_info_struct_type   *ctfc_info);

/*===========================================================================
FUNCTION dl_rm_calc_delta_n_fixedpos

DESCRIPTION
  This function calculates delta N value for all TF of all TrCh of a CCTrCh for
  flexible position rate matching parameter calculation. In addition to this
  in the process of calculating the delta N it calculates following global
  variables that are used for calculating the RM parameter values.
  - Number of radio frame per TTI
  - Number of coded bits per radio frame
  - Index of TF having maximum coded bits in a TrCh.
  - Delta N TTI (Only TF index 0 is valid)
  - Delta N radio frame (Only TF index 0 is valid)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates the delta N values in global variable.
===========================================================================*/

extern void dl_rm_calc_delta_n_fixedpos(
  /* CCTrCh setup information. */
  mcalwcdma_dec_cctrch_info_struct_type *cctrch_info);

/*===========================================================================
FUNCTION dl_rm_get_num_coded_bits

DESCRIPTION
  This function calculates the number of coded bits per TTI. Each code block
  in the TTI is coded as per the coding type. The number of coded bits is
  always 0 if the number of number of TB is 0. The code block size is not
  essentially 0 when TB block size is 0 (The Code Block size is min 40 for
  TC). This function assumes that number of TB is not 0. The formula to get
  the coded bits for a code block is:
    Uncoded concatenated bits with crc = K
    Coded bits = Y
    For No Coding :                Y = K
    For Conv 1/2 code Coded bits : Y = 2K + 16
    For Conv 1/3 code Coded bits : Y = 3K + 24
    For Turbo Coded bits (always 1/3) : Y = 3K + 12

DEPENDENCIES
  None

RETURN VALUE
  Number of coded bits in a TTI (or in all code blocks)

SIDE EFFECTS
  Updates the delta N values in global variable.
===========================================================================*/

extern uint32 dl_rm_get_num_coded_bits_tti(
  /* The code block info */
  mcalwcdma_dec_code_blk_param_struct_type *code_blk_info,
  /* coding type of the TrCh */
  mcalwcdma_dec_coding_enum_type trch_coding_type);

/*===========================================================================
FUNCTION dl_rm_calc_drm_scale_factor

DESCRIPTION
  This function calculates the DRM scale factor. The inputs to this function
  are:
    - coding type
    - number of rate matched per TTI
    - number of coded bits per TTI
  For turbo coded TrCh the DRM scale factor is:
    DRM scale factor (turbo) = ROUND(186 / SQRT[Rturbo(min)])
  For conv. coded TrCh the DRM scale factor is:
    DRM scale factor (conv) = 128 / SQRT[Rconv]

    where R = num rate matched bits / num coded bits

  In more simplified way the DRM scale factor can be wriiten as:
  For turbo coded:
     SQRT[(186 * 186 * num coded bits) / num rate matched bits]
  For conv coded:
     SQRT[(128 * 128 * num coded bits) / num rate matched bits]
  or SQRT[(num coded bits << 14) / num rate matched bits]

  This function assumes that for turbo coding the number of rate matched bits
  and number of coded bits are corresponding to Rtirbo(min).
  The number of rate matched bits and coded bits can be specified per radio
  frame ot per TTI however both should have same base.

DEPENDENCIES
  None

RETURN VALUE
  DRM scale factor upto 10 bit precision.

SIDE EFFECTS
  None
===========================================================================*/

extern uint16 dl_rm_calc_drm_scale_factor(
  /* coding type of TrCh for which DRM scale factor is to be calculated */
  mcalwcdma_dec_coding_enum_type coding_type,
  /* number of rate matched bits in the TF of this TrCh */
  uint32 num_rm_bits,
  /* number of coded bits in the TF of this TrCh */
  uint32 num_coded_bits);

#ifdef DL_RM_DEBUG
#error code not present
#endif
#endif /* DLRM_H */
