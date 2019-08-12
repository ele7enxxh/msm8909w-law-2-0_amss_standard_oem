#ifndef SRCH_SRCH4_H
#define SRCH_SRCH4_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 S R C H    S R C H 4   M O D U L E

GENERAL DESCRIPTION
      This module contains functions/definitions exported to SRCH4 module
      from 1X SRCH.

       *********************** IMPORTANT *********************************
      ONLY SRCH4 MODULE (outside of 1x SRCH) SHOULD INCLUDE THIS HEADER FILE
       *******************************************************************


 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Copyright (c) 2009 - 2013 Qualcomm Technologies, Inc. 
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
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/srch_srch4.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/11   jtm     Added member in results type to save max lecio.
07/25/11   vks     Export api to return carrier type.
12/09/10   mca     Added support to save individual div srch energies
10/28/10   mca     Added support for diversity search combining methods.
11/09/10   jtm     Initial SU API split.
06/04/10   cjb     Adding function prototype for srch_conv_linear_to_db()
12/01/09   vks     Initial implementation

===========================================================================*/



/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/* Common */
#include "comdef.h"

/* CP */
#include "cai_v.h"
#include "caii_v.h"

/* MCS */
#include "srch4drv_t.h"

/* SRCH */
#include "srch_rx_div.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* maximum number of peaks for all searchers */
#define SRCH_SRCH4_MAX_PEAKS      6

/* Sector parm structure */
typedef struct
{
  /* The pilot offset (0-511) that defines this sector. */
  uint16                     pilot;

  /* The sweep window size to use when searching for this sector.
     Units are 1/8 chips to make slew math clear and simple. */
  int32                      win_size;

  /* The calculated position offset. Used to move the search window in
     searcher 4. Units are 1/8 chips. Useful for tracking moving peaks */
  int32                      win_pos_offset;

  /* Number of chips to integrate before converting to energy */
  uint16                     coh_int;

  /* Number of passes to accumulate non-coherently */
  /* Encoding: 0 = 1 pass, 1 = 2 passes, ... , 7 = 8 passes. */
  uint8                      non_coh_int;

  /* Pilot set membership info */
  uint32                     pilot_set_mask;

  struct
  {
    /* Transmit diversity info: power multiplier and mode */
    caii_add_pilot_rec_type    data;

    /* Type indicator for additional pilot record types */
    cai_pilot_rec_type         type;
  } pilot_rec;

} srch_sect_parm_type;

/* Sector search results structure */
typedef struct
{

  /* Absolute position of all energy paths found in the
     window in 1/8 chip units. */
  int32                      max_pos[ SRCH_SRCH4_MAX_PEAKS ];

  /* Linear ECIO of the maximum energy path */
  uint32                     max_lecio[ SRCH_SRCH4_MAX_PEAKS ];

  struct
  {
    /* total linear ecio for the sector */
    uint32                   total;

    /* total linear ecio for the sector's individual div srch */
    uint32                   total_rx0;

    /* total linear ecio for the sector's individual div srch */
    uint32                   total_rx1;

    /* max linear ecio for the sector's individual div srch */
    uint32                   max_rx0;

    /* max linear ecio for the sector's individual div srch */
    uint32                   max_rx1;

    /* threshold for creating total lecio total or 0 to look up
       threshold as a function of window size */
    uint32                   threshold;
  } lecio;

  /* Absolute position in units of 1/8 chip of
     the END of the search window. */
  int32                      win_pos;

} srch_sect_result_type;

/* 1x SRCH version of searcher 4 antenna select */
typedef enum
{
  SRCH_SRCH4_ANT_0         = 0,               /* antenna 0 mode    */
  SRCH_SRCH4_ANT_1         = 1,               /* antenna 1 mode    */
  SRCH_SRCH4_ANT_DUAL      = 2,               /* dual antenna mode */
  SRCH_SRCH4_ANT_MAX,
} srch_srch4_ant_select_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_UTIL_WIN_THRESH_LECIO

DESCRIPTION    This function looks up the energy to threshold peaks as a
               function of window size.

DEPENDENCIES   None.

RETURN VALUE   Threshold in linear ecio.

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_util_win_thresh_lecio
(
  uint32 win_size /* window size in cx8 */
);

/*=============================================================================

FUNCTION       SRCH_UTIL_CALC_LECIO

DESCRIPTION    This function converts the peak energies to linear ecio and
               does the total linero ecio calc with filtering.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

=============================================================================*/
extern void srch_util_calc_lecio
(
  srch_sect_result_type *results, /* sector to calc linear ecio */
  srch_sect_parm_type   *parms    /* sector to calc linear ecio */
);

/*=============================================================================

FUNCTION       SRCH_UTIL_CALC_LECIO_THRESH

DESCRIPTION    This calculates the lecio threshold for the sector.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

=============================================================================*/
extern void srch_util_calc_lecio_thresh
(
  srch_sect_result_type *results, /* sector to calc linear ecio */
  srch_sect_parm_type   *parms    /* sector to calc linear ecio */
);

/*===========================================================================

FUNCTION       SRCH_PEAK_FILL_RAND_BUF

DESCRIPTION    This function takes randomsearch peak data and loads it into a
               buffer for use by DH.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_peak_fill_rand_buf
(
  srch_sect_result_type *results,   /* pointer to a sector to take data from */
  uint32                 num_peaks  /* number of peaks to use */
);

/*===========================================================================

FUNCTION       SRCH_CONV_MNRD_TO_TRUNC

DESCRIPTION    This function calculates trucation value from coherent
               length, non-coherent length.
               Truncation is calculated here assuming receive diversity
               is not set.

               T = ceil{ 0.5 * [ log2(2 * EcIo * Nnc *(sigma_in*G*Nc)^2 ) - 15]

DEPENDENCIES   None.

RETURN VALUE   Truncation value.

SIDE EFFECTS   None.
===========================================================================*/
uint8 srch_conv_mnrd_to_trunc
(
  uint16   coh_int,
  uint8    non_coh_int
);

/*===========================================================================

FUNCTION       SRCH_CONV_ENG_TO_LECIO

DESCRIPTION    This function process converts a searcher energy to linear
               Ec/Io * 9 * 2^16.

               For derivation of the formulas please see the systems memo
               "Searcher Energy and Finger RSSI to Ec/Io Conversion."

DEPENDENCIES   The conversion to dB depends on the 9 * 2^16 scaling.

RETURN VALUE   Linear Ec/Io * 9 * 2^16

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_conv_eng_to_lecio
(
  uint16   srch_eng,      /* hardware srch energy */
  uint16   c_int,         /* coherent integration in chips */
  uint8    nc_int,        /* noncoherent integration passes minimum is 1 */
  uint8    trunc          /* coherent truncation in bits */
);

/*===========================================================================

FUNCTION       SRCH_CONV_LINEAR_TO_DB

DESCRIPTION    This function converts from linear units to db units in 1/8db

DEPENDENCIES   None.

RETURN VALUE   The ecio in 1/8 db units.

SIDE EFFECTS   None.

===========================================================================*/
extern uint32  srch_conv_linear_to_db
(
  uint32 lecio /* The lineary ecio to be converted */
);

/*===========================================================================

FUNCTION       SRCH_SRCH4_GET_FREQ_OFFSET_CORRECTION

DESCRIPTION    Returns the currenty frequency error seen by the AFC module.

DEPENDENCIES   None.

RETURN VALUE   Offset correction in int16 units.

SIDE EFFECTS   None.

===========================================================================*/
int16 srch_srch4_get_freq_offset_correction( void );

/*===========================================================================

FUNCTION       SRCH_SRCH4_GET_1X_ANTENNA_TYPE

DESCRIPTION    Returns the antenna type for 1X based on diversity enabled or
               not.

DEPENDENCIES   None.

RETURN VALUE   srch_srch4_ant_select_type.

SIDE EFFECTS   None.

===========================================================================*/
srch_srch4_ant_select_type srch_srch4_get_1x_antenna_type
(
  boolean                   rd              /* flag to indicate use of RD  */
);

/*===========================================================================

FUNCTION SRCH_SRCH4_GET_AFLT_ANTENNA_TYPE

DESCRIPTION    Returns the antenna type for AFLT based on diversity enabled or
               not.

DEPENDENCIES   None.

RETURN VALUE   srch_srch4_antenna_type.

SIDE EFFECTS   None.

===========================================================================*/
srch_srch4_ant_select_type srch_srch4_get_aflt_antenna_type( void );

/*===========================================================================

FUNCTION       SRCH_SRCH4_GET_AFLT_CARRIER_TYPE

DESCRIPTION    Returns the carrier type for AFLT based on the antenna in use
               by 1x.

DEPENDENCIES   None.

RETURN VALUE   srch4_carrier_type.

SIDE EFFECTS   None.

===========================================================================*/
srch4_carrier_type srch_srch4_get_aflt_carrier_type( void );

/*===========================================================================

FUNCTION       SRCH_SRCH4_CONFIRM_TD_FOR_PILOT

DESCRIPTION    This function checks if the pilot type is a TD pilot or not.
               If not, return transmit_diversity as FALSE.

DEPENDENCIES   None.

RETURN VALUE   transmit_diversity.

SIDE EFFECTS   None.

===========================================================================*/
boolean srch_srch4_confirm_td_for_pilot
(
  boolean               transmit_diversity, /* flag to indicate use of TD  */
  srch_sect_parm_type   *parms              /* search parameters           */
);

/*===========================================================================

FUNCTION       SRCH_SRCH4_MERGE_RD_RESULTS

DESCRIPTION    This function merges the two diversity searches.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   This updates the sector structure with the merged results.

===========================================================================*/
extern void srch_srch4_merge_rd_results
(
  srch4_task_struct_type *task_res,       /* array of task struct (results)*/
  srch_sect_result_type  *results,        /* sector results                */
  srch_sect_parm_type    *parms           /* sector parameters             */
);

/*===========================================================================

FUNCTION       SRCH_SRCH4_MERGE_TD_RESULTS

DESCRIPTION    This function merges the common and diversity paths from
               a transmit diversity search.

DEPENDENCIES   Sector must have a TD pilot.

RETURN VALUE   None

SIDE EFFECTS   This updates the sector structure with the merged results.

===========================================================================*/
extern void srch_srch4_merge_td_results
(
  srch4_task_struct_type *task_res,       /* array of task struct (results)*/
  srch_sect_result_type  *results,        /* sector results                */
  srch_sect_parm_type    *parms           /* sector parameters             */
);

#endif /* SRCH_SRCH4_H */
