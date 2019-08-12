#ifndef SRCH_HW_T_H
#define SRCH_HW_T_H
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/srch_hw_t.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
03/07/11   mca     Rename srch_set_enum_type to srch_sect_set_enum_type
01/25/11   adw     Added fast and slow search filters to sector data struct.
                   Added flag to reporting info structure.
12/09/10   mca     Added support to save individual div srch energies
11/30/10   adw     Added flag to reporting info structure.
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
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
#include "comdef.h"

/* Srch */
#include "srch_rx_t.h"
#include "srch_srch4.h"
#include "srchmc.h"
#include "srchmc_v.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Constants and macros
-------------------------------------------------------------------------*/

#define SRCH_MAX_PEAKS      6
  /* maximum number of peaks for all searchers */

#define SRCH_NUM_PEAKS      4
  /* number of peaks for searcher 1, 2, & 3 */

#define STATUS_DUMP_WAIT_TIME (10 + CLK_MS_PER_TICK)
  /* Time allowed for status dump interrupt to occur after the last programmed
     search should have been finished on the searcher hardware */

#define SRCH_DYNAMIC_LECIO_THRESHOLD  0
  /* This value is used for sector->lecio.threshold if a dynamic lecio
     threshold is requested based on window size */

/* convert coherent and non_coherent values to sector struct equivalents */
#define COH2SECT(coh)            ((coh) >> 2)
#define NONCOH2SECT(non_coh)     ((non_coh) - 1)
#define GAIN2TRUNC(gain)         (5 - (gain))
  /* Convert coherent and non_coherent values to sector struct equivalents */

#define SECT2COH(coh)            ((coh) << 2)
#define SECT2NONCOH(non_coh)     ((non_coh) + 1)
#define SECT2PREGAIN(gain)       (2 << (gain))
#define TRUNC2GAIN(trunc)        (5 - (trunc))
  /* Convert coherent and non_coherent values from sector struct equivalents */


/*--------------------------------------------------------------------------
                          Sector State Codes

   Each sector is assigned to a Set as per IS-95.  This enum defines
   tag values for each set.
--------------------------------------------------------------------------*/

typedef enum
{
  SET_NONE,            /* Sector does not belong to any set */
  SET_A,               /* Sector belongs to Active Set */
  SET_C,               /* Sector belongs to Candidate Set */
  SET_PC_N,            /* Sector belongs to Neighbor Set, but is currently
                          considered a Pre-Candidate Set member */
  SET_N,               /* Sector belongs to Neighbor Set */
  SET_PC_R,            /* Sector belongs to the Remaining Set, but is
                          currently considered a Pre-Candidate Set member */
  SET_CF_N,            /* Sector belongs to the Candidate Freq set */
  SET_R                /* Sector belongs to the Remaining Set */
}
srch_sect_set_enum_type;

/*--------------------------------------------------------------------------
                            TRPT Structure

  This structure is used in the sector structure and in temporary sector
  list structures to contain values used in reporting to the base station.
--------------------------------------------------------------------------*/
typedef struct
{
  int2  pos;
    /* POS_MATH position of this sector's best (usually earliest)
       path in 1/8 chip units. Numbers will be small deltas about
       POS_MATH_OFFSET. */

  int2  ear_pos;
    /* POS_MATH position of this sector's latest earliest path.
       In 1/8 chip units. Numbers will be small deltas about
       POS_MATH_OFFSET. */

  int2  nom_off;
    /* Initial offset of pilot from center in 1/8 chip units.*/

  byte  rev_dens;
    /* "Fast-Revisit" search density for speeding up looks at
        pilots apporaching T_ADD report threshold */

  byte  rev_indx;
    /* "Fast-Revisit" search index */

  dword  drop_timer;
    /* Handoff drop timer.  This value is set to the current system
       time in 26.66..ms units every time the pilot is observed to be
       above the t_drop threshold.  If this value plus the T_TDROP
       duration ever exceeds the current system time, the handoff
       drop timer will be considered expired. */

  int2  cnt;
    /* Number of entries in the following list */

  int2  pilots[ SRCH_ASET_MAX + 1 ];
    /* List of pilot offsets, used to keep track of what has been
       reported to the base station */

  byte  pilot_cnt[ SRCH_ASET_MAX ];
    /* T_COMP state machine, used to speed up reporting when
       there are many pilots in the Active/Candidate sets */

  boolean  below_t_add;
    /* Flag indicating that the pilot is below T_ADD */
}
srch_trpt_struct_type;


/*--------------------------------------------------------------------------
                              Sector Structure

   The Sector is in many ways the basic element that the searcher task
   deals with.

   As used in the searcher task, a Sector is a signal or hoped-for signal
   at a particular pilot offset.  This may correspond to one sector of a
   sectored cell, or to an unsectored cell's single sector.

   The sector struct contains different types of information:

      * How to demodulate the sector
      * How to search for the sector's signal
      * The results of the most recent search of the sector
      * The combined results of all recent searches of the sector

   So, the sector struct can be passed around to various kinds of
   functions without also passing a lot of associated information.
   Which fields of the sector are meaningful at any given time varies.
--------------------------------------------------------------------------*/

typedef struct
{
  uint32 lecio;
    /* filtered linear ecio for the sector */

  uint32 lecio_rx0;
    /* filtered linear ecio for the sector's individual div srch */

  uint32 lecio_rx1;
    /* filtered linear ecio for the sector's individual div srch */

  uint32 len;
    /* number or samples in the filter (ie length)  */

  uint32 max_len;
    /* allow the filter to grow to this length */

  uint32 lecio_biased;
    /* the filtered energy biased based on neighbor info for HO decision */
}
srch_filter_struct_type;
  /* Long Term Average (Filtered) Linear Ecio from the searcher */

typedef struct
{

  /* Searcher Parameters */
  srch_sect_parm_type        search_parms;

  /* Search Results */
  srch_sect_result_type      search_results;

  /* Band Class for this sector:  0 = 800mhz, 1 = PCS */
  srch_rx_band_type          band_class;

  /* CDMA frequency assignment for this sector */
  srch_rx_chan_type          cdma_freq;

  /* Priority of this particular sector, used in neighbor searches */
  uint8                      pri;

  /* Parameters for the FCH */
  srch_fch_info_type         fch_info;

  /* Parameters for the DCCH */
  srch_dcch_info_type        dcch_info;

  /* Parameters for the SCCH */
  srch_scch_info_type        scch_info;

  srch_sch_info_type2        sch_info;

  /* Parameters for CPCCH */
  srch_cpcch_info_type       cpcch_info;

  /* information related to PDCH that may change per pilot */
  srch_dv_pilot_info_type    dv_pilot_info;

  /* Neighbor Window Search Offset for the pilot */
  uint8                      srch_offset_nghbr;

  /* The sweep window size of neighbor set received form the over-
     the-air messages. Units are 1/8 chips to make slew math clear
     and simple. */
  int16                      ota_win_n;

  /* The "age" of the sector; used only in the Neighbor Set */
  uint8                      age;

  /* Which set the sector is currently assigned to */
  srch_sect_set_enum_type    set;

  /* Pre-Candidate Set state variable */
  int16                      pc_state;

  /* Cell index.  This indicates which cell in the local cell
     table contains this sector.  The cell numbering may change
     whenever a Handoff Direction Messsage is received. */
  uint32                     cell;


  /* The current POS_MATH position of the center of the search window.
     Units are 1/8 chips. Numbers will be relatively small deltas
     about POS_MATH_OFFSET */
  int16                      pos;

  /* Reporting info including filtered energy */
  srch_trpt_struct_type      trpt;

  /* Long Term Average (Filtered) Linear Ecio from the searcher */
  srch_filter_struct_type    srch_filter;

  /* Search Fast Filter tracking the current "goodness" of the sector */
  srch_filter_struct_type    srch_fast_filter;

  /* Search Slow Filter tracking the historical "goodness" of the sector */
  srch_filter_struct_type    srch_slow_filter;

  /* Long Term Average (Filtered) Linear Ecio from the finger */
  struct
  {
    /* filtered linear ecio for the sector */
    uint32                   lecio;

    /* number or samples in the filter (ie length)  */
    uint32                   len;

    /* allow the filter to grow to this length */
    uint32                   max_len;
  } fing_filter;

  /* TC state centering slew memory in 1/8 chips */
  int16                      center_slew;

  /* Centering slew hysteresis counter */
  int16                      center_cnt;

  /* Centering slew throttling flag */
  boolean                    center_throttle;

  /* Hard-handoff related earliest path offset from center of
     srch win. In 1/8 chip units. */
  int16                      hard_off;

  /* The time (ms) when this sector was last
     searched regardless of the srch parms */
  uint32                     last_time_searched;

  /* The time (ms) when this sector was last
     searched with OTA parameters */
  uint32                     last_ota_win_searched;

  /* The time (ms) when this sector was searched and it's strongest
     path was above the REACQ_WIN_NOISE_THRESH_LECIO threshold */
  uint32                     last_time_seen_energy;

  /* the last value used to determine which peaks are paths */
  uint32                     last_threshold;

  /* Remember what window size the latest search was done at */
  int32                      last_win_siz;

  /* Access Entry Handoff allowed flag */
  boolean                    access_entry_ho;

  /* Access Handoff allowed flag */
  boolean                    access_ho_allowed;

  /* Flag tells whether we have attempted to access this pilot.
     Note: SRCH doesn't use this flag, it's merely for MC coding
           convenience */
  boolean                    access_attempted;

  boolean                    bcch_supported;

  /* Flag tells whether this sector (when Candidate Freq Neighbor), it to be
     searched. */
  boolean                    cfs_srch_flag;

}
srch_sect_struct_type;


/*--------------------------------------------------------------------------
                  STRUCTURE: AFLT_PILOT_STRUCT_TYPE

This structure is used to maintain values applicable to a single pilot
channel. THIS IS A STRIPPED_DOWN VERSION FOR TESTING SEARCH BATCH LOGGING.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16 pn;
    /* the pilot PN Offset */

  srch_sect_set_enum_type set;
    /* the pilot set this pilot belongs to */

  int4 offset;
    /* The offset from the absolute searcher center position */

  int2 srch_win;
    /* the search window size */

  byte srch_offset;
    /* the index into the search window offset table */

  byte antenna;
    /* Antenna on which PPM searching took place */

  int2 srch_win_center_offset;
  /* this is the offset of the center of the search window for this pilot
     from {MSTR + (pilot index * 64)}.  For neighbors and remaining set
     pilots this will always be zero but for active and candidate set pilots
     it is the distance in chipX8 of the earliest arriving multipath of this
     pilot from the nominal window center, i.e. {MSTR + (pilot index * 64)} */

  uint2 num_peaks;
    /* number of peaks (based on which searcher was used) */

  uint32  max_pos[ SRCH_MAX_PEAKS ];
    /* Position of the sweep results of the maximum energy path */

  word  max_eng[ SRCH_MAX_PEAKS ];
    /* Energy of the maximum energy path */

  boolean list_end;
    /* marks the last pilot to be searched in the search list */
}
srch_ppm_pilot_struct_type;

typedef struct
{
  q_link_type  link;
    /* Link for putting this buffer on a queue. */

  srch_sect_set_enum_type  set;
    /* Which set the sector is currently assigned to */

  boolean                age;
  int2                   pilot;
  byte                   nc_int;
  byte                   chps;
  byte                   gain;
  int4                   offset;
  int4                   winsiz;
  byte                   coh_tr;
    /* The above parameters are the standard parameters used to program
       a search.  they should be calculated ahead of time and stored in
       this structure, so that the searches can be quickly loaded into
       the queue */

  boolean                dump;
    /* An extra field for the 16x searcher which signals whether or not
       we want to receive a dump interrupt after this particular search */

  int4                   exp_pos;
    /* The expected position of the searcher after it has completed this
       search on the queue, assuming no interruptions to the searcher */

  srch_sect_struct_type  *sect_ptr;
    /* Pointer to the original sector structure which this search
       belongs to.  ie. if the parameters in this search belong to
       srch_aset[ 0 ], then *sect_ptr = srch_aset[ 0 ]  This is used
       to place the final results in the correct sector structure */

  srch_ppm_pilot_struct_type *ppm_ptr;
    /* Pointer to structure in PPM Pilot table, where the PPM results
       will be stored */

  struct
  {
    uint2  max_inx[ SRCH_MAX_PEAKS ];
      /* Index in the sweep results of the maximum energy path
         in 1/8 chip units. */

    word  max_eng[ SRCH_MAX_PEAKS ];
      /* Energy of the maximum energy path */
  }
  common_path;
    /* The raw results of common pilot searches go here first */

  struct
  {
    uint2  max_inx[ SRCH_MAX_PEAKS ];
      /* Index in the sweep results of the maximum energy path
         in 1/8 chip units. */

    word  max_eng[ SRCH_MAX_PEAKS ];
      /* Energy of the maximum energy path */
  }
  diversity_path;
    /* The raw results of diversity pilot searches go here first */
}
srch1_cmd_struct_type;

#endif /* SRCH_HW_T_H */
