#ifndef SRCH_FING_DRIVER_I_H
#define SRCH_FING_DRIVER_I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E A R C H E R    F I N G E R    D R I V E R

                         H E A D E R   F I L E


GENERAL DESCRIPTION
   This file contains exports from srch_fing_driver.c.

  Copyright (c) 2004 - 2014 Qualcomm Technologies, Inc.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/srch_fing_driver_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/23/15   bb      Changes to de-assign finger if it is out of sync between 
                   sector pointer asosciated to finger and pilot PN that is 
                   actually tracking 
11/07/14   pk      Multiple finger support for Jolokia.
10/08/14   srk     Limit the Max Fing count to 3 for Jolokia until Q6 issue
                   is fixed.
07/07/14   bb      Changes to maintain one to one mapping between WALSH and
                   QOF sector assignments of QLIC engine
04/16/14   cjb     Fixed second chain test feature on BOLT with WTR-3925 RF card
03/31/14   vks     Modify srch_fing_set_fingers_pos to take in finger spread
03/09/14   ssh     Changes to slam fingers before lock (BOLT)
02/18/14   cjb     Changes to support SHDR on BOLT modem with WTR3925
01/31/14   vks     Multiple finger support for bolt
09/20/13   vks     Finger driver cleanup - remove obsolete attributes
09/16/13   cjb     Limit max fingers to 1 for bring-up
09/16/13   vks     Read finger position from firmware
08/23/13   vks     Support for control of immed fing writes obsolete on bolt
05/31/13   bb      Changes to avoid enabling NS triage fingers in non
                   Traffic state
12/03/12   bb      Changes to define srch_fing_is_div_config_enabled()
11/05/12   srk     Create an API to return the first finger.
01/30/12   vks     Add api to set position for multiple fingers.
01/23/12   srk     Feature Cleanup
01/13/12   srk     Feature Cleanup: Mainlined Always ON features
01/11/12   srk     Feature Cleanup.
12/20/11   srk     Feature clean up: Removed FEATURE_IS95B_MDR
12/18/11   adw     Safety check max fing cnt when defining NS fings to protect
                   against issues if ever a reduced finger set is used.
10/31/11   jtm     Feature clean up.
10/27/11   trc     Add srch_fing_allow_immed_writes() API
10/12/11   trc     Bump FIRST_FING to 0, giving 6 / 6 fingers
10/07/11   trc     Bump FIRST_FING to 1, giving 5 / 6 fingers
09/23/11   vks/trc Move to FIRST_FING macro instead of static finger0.
                   Start with 2 and end with MAX resulting in 4/6 total fingers
08/26/11   vks     Reduce # of fingers to 2 as MCPM support is not yet there.
08/18/11   adw     Mainline 1x message router support.
08/17/11   vks     Increase finger count to 6.
08/12/11   adw     Support second chain test [Option-2] capability on NikeL.
06/15/11   adw     Moved QLIC LPN advance and RTC adjust into combiner init.
06/03/11   adw     SV support.
05/26/11   adw     Temporarily run with only two fingers on NikeL.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/20/11   adw     Use single finger for rumi.
04/05/11   adw     Reduce the number of fingers for rumi.
12/16/10   vks     Remove FEATURE_IS2000_REL_C_DV and FEATURE_IS2000_REL_D_DV
                   support.
11/01/10   jtm     Modem 1x SU API changes.
09/23/10   vks     Add support for QLIC logging.
09/22/10   bb      Code changes for Neighbor Set Triage feature
05/31/10   vks     Restore the original finger counts (6 for regular operation)
05/24/10   bb      Code changes for "QLIC Management Algorithm" feature
05/12/10   cjb     Added srch_fing_qlic_override to store corresponding NV
04/10/10   vks     Reduce the number of fingers to 3
04/27/10   bb      Define ZERO_QOF_SET_ID value
04/08/10   cjb     Added srch_fing_qlic_override to store corresponding NV
02/18/10   bb      Added code changes to support QOF functionality
02/12/10   vks     Remove bringup featurization for MAX_FING_CNT as MDM9K
                   now has support for 6 fingers.
01/15/10   bb      Externalize QLIC_DEFAULT_RTF_ADJ for DEM1X RTC compensation
12/19/09   vks     Featurize MAX_FING_CNT=1 for Genesis bringup.
12/04/09   vks     Move HW_MAX_FING_CNT definition to srch_hw.h.
11/09/09   vks     Added Message Router and Q6 FW support under feature
                   FEATURE_1X_SUPPORTS_MSGR.
09/18/09   adw     Support dynamic allocation of finger driver data structure.
04/28/09   tjc     Added function to return the cell id of a specified finger
04/24/09   adw     Added FING_CHAN_INVALID to chan_id_enum, replaced
                   NUM_DEMOD_CHANNELS with DEMOD_MAX_CHANS.
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Cleaned up incs and eliminated implicit includes.
03/26/09   adw     Remove T_MSM6700 featurization.
03/25/09   adw     Include modem_1x_defs.h for 1x specific modem defines.
06/20/08   mca     Merge from //depot
02/13/08   aps     Combined rx1_demod and rx0_demod to srch_find_rx_demod()
10/30/07   sst     Add function to re-enable locking after force (un)lock
04/27/07   aps     Added srch_fing_set_pos()
04/23/07   sst     Split up QLIC Support and QLIC Algorithm using defines
04/12/07   sst     Added support for QLIC logging
03/09/07   pa      Added srch_fing_in_lock()
02/23/07   sst     Add QLIC support
10/23/06   sst     Merge from QSC6055 branched files
                   Update number of fingers for T_RUMI
03/13/06   kwo     Removed srch_fing_get_lock_status()
11/10/05   kwo     Added srch_fing_set_lecio()
11/07/05   bt      Added srch_fing_rx0_demod and srch_fing_rx1_demod.
11/07/05   kwo     Removed _set_walsh_and_demod_chan() srch_fing_cmd()
                   Added srch_fing_idle_cmd() and srch_fing_other_cmd()
10/03/05   ejv     Removed raw_rssi.
09/27/05   ejv     Change rssi value passed in to srch_fing_set_rssi to be
                   16 bits.
09/12/05   ejv     Define constants for finger lock values.
08/03/05   kwo     Added srch_fing_get_active_lecio()
07/19/05   grl     Changed srch_fing_set_demod_chain to srch_fing_set_chain.
06/30/05   sfm     Added variables and constants for relative finger lock
06/13/05   sfm     Changed srch_fing_set_primary_chain() to
                   srch_fing_set_demod_chain()
05/31/05   rng     Merged in from SHDR Sandbox.
05/26/05   ejv     Added constants from dmod.h.
05/24/05   ejv     Regrouped global externs and added some from srchi.h.
                   Mainlined 2 interface functions and added numerous more
                   to support the library.
05/20/05   ejv     Removed srch_fing_pos_offset.
05/18/05   kwo     Added srch_set_ant_status(), srch_fing_get_tt_fing_pos() and
                   srch_fing_get_max_fing_cnt()
05/04/05   ejv     Moved Antenna-specific Parms from srchi.h.
04/1/05    kwo     Added srch_fing_update_active_filter()
04/01/05   sfm     Changed srch_fings[].locked to be boolean and
                   triage cleanup changes
03/16/05   ejv     Only use 4 fingers for DV builds.
03/09/05   sfm     Added srch_fing_scch_active()
02/03/05   kwo     Added srch_fing_reassign_sect()
01/17/05   pj      Updated comment for srch_fing_get_master_finger_data.
12/27/04   sfm     Removed srch_fing_reset_qof(), srch_fing_set_qof(),
                   and srch_fing_cfg_all_qof() and added
                   srch_fing_disable_all_demod_chans()
11/18/04   ejv     Remove unused function prototypes.
11/17/04   sfm     Removed change_flags from srch_fings[] and removed
                   remaining FINGER_CHANGED_ constants
11/12/04   sfm     Changed srch_fing_get_master_finger_data() to
                   return a boolean indicating wether data was available
11/10/04   ejv     Added additional active channel flags.
11/09/04   ejv     Change uint8 to uint16 in srch_fing_set_pwr_comb_thresh.
11/05/04   ejv     Added srch_fing_get_mask.
10/08/04   abh     Removed obsoleted functions save/restore_context() and
                   FEATURE_SRCH4 cleanup.
10/06/04   kwo     Added support for pilot filter sub packet
09/30/04   sfm     Mainlined FEATURE_FINGER_DRIVER
09/28/04   ejv     Added srch_fing_tc_cmd.
09/27/04   jcm     Renamed srch_fing_pilot_filter_gain() ->
                   srch_fing_get_pilot_filter_gain()
09/27/04   jcm     Removed redundant srch_fing_get_pilot_filter_gain()
09/24/04   sfm     Comment changes
09/20/04   sfm     Added srch_fing_init()
09/15/04   sfm     Created out of srch_fing.h

==========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"

/* Srch */
#include "srch_chan_t.h"
#include "srch_genlog.h"
#include "srch_genlog_i.h"
#include "srch_hw_t.h"
#include "srch_hw_t_i.h"
#include "srch_rx.h"
#include "srch_rx_t.h"
#include "srch_rx_t_i.h"

/* Feature Dependent */
#include "cdmafw_msg.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
                       Finger-related Constants
--------------------------------------------------------------------------*/

/* Base finger from which to start */
#define FIRST_FING               0

#define  MAX_FING_CNT            8

/* maximum number of fingers used for neigbhbors */
#if MAX_FING_CNT < 8
  #define  MAX_NS_FING_CNT   0
#else
  #define  MAX_NS_FING_CNT   2
#endif
/* offset value for NS fingers */
#define MAX_NS_FING_OFFSET   (MAX_FING_CNT - MAX_NS_FING_CNT)
/* How many fingers to use (must be <= HW_MAX_FING_CNT - defined in
                                                         srch_hw.h) */

#define NUM_FING_ACCUMS_20MS 14
  /* number of per-finger accumulators with 20ms frame boundaries */
#define NUM_FING_ACCUMS_10MS 0
  /* number of per-finger accumulators with 10ms frame boundaries */
#define NUM_FING_ACCUMS      (NUM_FING_ACCUMS_20MS + NUM_FING_ACCUMS_10MS)
  /* total number of per-finger accumulators available */

#define DMOD_TT_K1_12HZ_V       0x02   /* 12.5 Hz value */
#define DMOD_TT_K1_25HZ_V       0x08   /* 25 value      */
#define DMOD_TT_K1_50HZ_V       0x20   /* 50 value      */
#define DMOD_TT_K1_100HZ_V      0x7F   /* 100Hz value   */
  /* Finger Time Tracking Gain K1 Register                */

#define DMOD_TT_K2_12HZ_V       0x10   /* 12.5 Hz value */
#define DMOD_TT_K2_25HZ_V       0x20   /* 25 Hz value   */
#define DMOD_TT_K2_50HZ_V       0x40   /* 50 Hz value   */
#define DMOD_TT_K2_100HZ_V      0x7F   /* 100Hz value   */
  /* Finger Time Tracking Gain K2 Register                */

/*--------------------------------------------------------------------------
                      Lock Threshold Algorithm Values
--------------------------------------------------------------------------*/
/* To convert RSSI write units to dB...
   10 * log (rssi / 18 / 2^11) */
#define SRCH_FING_LOCK_LO           0x40   /* -27.6dB */
#define SRCH_FING_LOCK_HI           0x160  /* -20.2dB */
#define SRCH_FING_LOCK_NEW          0x100  /* -21.6dB */
#define SRCH_FING_PCTL_LOCK         0x280  /* -17.6dB */

#define SRCH_USE_UPPER_LOCK_THRESH      0xFFFF
#define SRCH_USE_RELATIVE_LOCK_THRESH   0

/*--------------------------------------------------------------------------
                      Relative Lock Threshold Values
--------------------------------------------------------------------------*/
/* To convert RSSI write units to dB...
   10 * log (rssi / 18 / 2^11) */
#define SRCH_RELATIVE_LOCK_LO_CELL  0x38   /* -28.2dB */
#define SRCH_RELATIVE_LOCK_LO_PCS   0x6F   /* -25.2dB */
#define SRCH_RELATIVE_LOCK_HI       0x199A /* - 7.5dB */

/*--------------------------------------------------------------------------
                      QLIC related values
--------------------------------------------------------------------------*/
/* Definition for QLIC RTC retard factor */
#define QLIC_DEFAULT_RTF_ADJ      4
#define QLIC_DEFAULT_LPN_ADV      5

/* Following defintions are to support QOF
 * feature functionality.
 */
#define INVALID_QOF_SET_ID (uint8) (~0)
#define MAX_QOF_SET_IDS    0x03
#define ZERO_QOF_SET_ID    0

/* Following defintion are used to support
 * "QLIC Management Algorithm" feature functionality.
 */
#define INVALID_QLIC_OMEGA_VALUE   (uint32) (~0)
#define QOF_OMEGA_OFFSET           25
#define QLIC_OMEGA_THR_VALUE       225

/*--------------------------------------------------------------------------
                      Finger Channel Enum Summary
--------------------------------------------------------------------------*/

/* Enum to map channels to finger accumulators */
typedef enum
{
  /* Invalid channel */
  FING_CHAN_INVALID = -1,

  /* Pilot channels */
  PILOT_EARLYLATE = 0,
  PILOT_ONTIME,

  /* IS-2000 channels */
  CHAN_0,
  CHAN_1,
  CHAN_2,
  CHAN_3,
  CHAN_4,
  CHAN_5,
  CHAN_6,
  CHAN_7,
  CHAN_8,

  /* Transmit diversity pilot */
  PILOT_EARLYLATE_TD,
  PILOT_ONTIME_TD,

  /* IS-95B Supplemental channels (SCCH) */
  CHAN_1_SUP,
  CHAN_2_SUP,
  CHAN_3_SUP,
  CHAN_4_SUP,
  CHAN_5_SUP,
  CHAN_6_SUP,
  CHAN_7_SUP,

  /* Size of enum */
  FING_CHAN_MAX
}
srch_chan_id_enum_type;

/*--------------------------------------------------------------------------
                          Delayed Finger States

   Used to prevent commanding a new slew when the finger is still slewing
--------------------------------------------------------------------------*/

typedef enum
{
  DELAYED_FINGER_TRACKING,   /* Slew commands allowed */
  DELAYED_FINGER_RESERVED,   /* Finger reserved for slamming */
  DELAYED_FINGER_UNRESERVED  /* Returning to tracking mode. */
}
delayed_fing_state_type;

/*--------------------------------------------------------------------------
                          Active Channel Flags
--------------------------------------------------------------------------*/

#define FING_FCH_ACTIVE      0x00000001
#define FING_SCH_ACTIVE      0x00000002
#define FING_SCCH_ACTIVE     0x00000004
#define FING_DCCH_ACTIVE     0x00000008
#define FING_PDCCH_ACTIVE    0x00000010
#define FING_CPCCH_ACTIVE    0x00000020
#define FING_RCCH_ACTIVE     0x00000040
#define FING_ACKCH_ACTIVE    0x00000080
#define FING_GCH_ACTIVE      0x00000100

/* macro to set or clear a bit in the active channel mask based on the result
   of a boolean expression that indicates if the channel is active or not */

#define FING_CHAN_MASK( cond, fing_p, chan_flag )   \
  if ( cond )                                       \
  {                                                 \
    (fing_p)->active_channel_mask |= (chan_flag);   \
  }                                                 \
  else                                              \
  {                                                 \
    (fing_p)->active_channel_mask &= ~(chan_flag);  \
  }

/*--------------------------------------------------------------------------
                             Finger Structure

   This structure is used in the table of fingers.  It really has two
   kinds of functions:

     (1) It keeps track of the status of each finger, and

     (2) It centralizes all the information needed to command the finger
         to its new assignment.  The routines that need to adjust a finger
         simply adjust the values in its table entry at will, then call
         srch_fing_cmd() to command the hardware accordingly.

   Second structure contains information common to all fingers, instead of
   per-finger information.

--------------------------------------------------------------------------*/
typedef struct
{
  uint16  new_ass;   /* Depricated */
    /* If nonzero, this finger has been changed and needs to be
       given its new assignment.  There is no attempt to keep track
       of what has changed; everything must be reprogrammed whenever
       this flag is nonzero. */

  srch_sect_struct_type *sect_ptr;
    /* Pointer to the sector struct associated with this finger.
       If the finger is NOT assigned, the pointer is set to NULL. */

  uint16  path;
    /* Index into the array of paths, indicating which path this
       finger has been assigned to. */

  uint16  fch_walsh_chn;
    /* Walsh channel to be used for the FCH in non-traffic states */

  uint16  pwr_inx;
    /* Index indicating which group of sectors this finger is to be
       combined with for power control purposes.  Used to configure
       the power combiner.  Only indexes 0, 1, and 2 are assigned
       to power combiner groups.  MSM6700 increases this to 6 groups. */

  uint16  assg_cnt;
    /* Number of times the finger has been assigned. */

  uint16  lock_age;
    /* Keeps track of how long the finger has been in lock.  This
       value is incremented whenever an active window search is
       completed and the finger is in lock. */

  uint16  unlock_age;
    /* Keeps track of how long the finger as been out of lock.  This
       value is incremented whenever an active window search is
       completed and the finger is out of lock. */

  int32  slew;
    /* How far to slew the finger at this command. Signed! */

  int16  pos;
    /* POS_MATH position of this finger relative to reference */

  int32  last_locked_raw_pos;
    /* Last value of raw_pos for which the finger was in lock.  This is
       used to limit how far the finger can wander off its assigned
       position before it gets de-assigned. */

  boolean  locked;
    /* Whether this finger is locked (as of the last time we did
       a srch_fing_dmp() ). */

  uint16  rssi;
    /* Received signal strength indicator value for this finger
       (as of the last srch_fing_dmp() ).  The demod chip provides
       16 bits of RSSI, but we only use the 8 MSBs of that. */

  uint16  rssi_hr;
    /* This is the high resolution rssi (i.e. 15 bits).
       This is the combined RSSI for both receiver chains. */

  uint16 rx0_rssi_hr;
    /* This is the high resolution rssi for rx0. */

  uint16 rx1_rssi_hr;
    /* This is the high resolution rssi for rx1. */

  uint32  rx0_lecio;
  uint32  rx1_lecio;
  uint32  lecio;
    /* Linear ecio values corresponding to the respective finger rssi */

  int32  raw_pos;
    /* The finger's position, as of the last srch_fing_dmp(). */

  boolean ttl_slow;
    /* TRUE if slow TTL is desired, FALSE if fast (regular) TTL is needed */

  uint32 active_channel_mask;
    /* mask indicating which channels are active */

  srch_chan_type *demod_chan[DEMOD_MAX_CHANS];
    /* pointer the channel info for each active data channel */
}
srch_fing_struct_type;

typedef struct
{
  uint16  ttrk_k1_gain;
  uint16  ttrk_k2_gain;
    /* The gain to be used for finger time tracking. */

  uint16  lock_hi;
    /* Lock indicator threshold value: when the RSSI goes above
       this value, the finger will go into lock. */

  uint16  lock_lo;
    /* Lock indicator threshold value: when the RSSI goes below
       this value, the finger will go out of lock. */

  uint16  lock_new;
    /* Lock indicator preset value: the RSSI accumulator is preset
       to this value when the finger is slewing. */

  boolean changed;
    /* The common parameter information has changed.  Transfer parameters
       to demodulator on next srch_fing_cmd( ) */

  uint16  pctl_lock;
    /* Power control lock threshold - this threshold is downstream
       from the symbol combiner lock thresholds so if this is zeroed,
       you get the locks defined above */
}
srch_fing_parm_struct_type;


/*--------------------------------------------------------------------------
                     General Global Finger Variables
--------------------------------------------------------------------------*/

/* This variable keeps track of the current Master Finger, which is being
   tracked by Symbol combiner. Note that normally this is same as
   post_slam_tt_fing. However there are some small timing windows when
   post_slam_tt_fing is the "soon to be" master finger and not the current M.F.
*/
extern uint16 srch_curr_ttrk_fing;

/* The number of fingers available in the MSM */
extern uint16 srch_fing_cnt;

/* The number of unreserved fingers usable in the searcher.  During the idle
   paging mode, the last finger may be reserved for a 6.67ms
   delayed timeline generation. */
extern uint16 srch_fingers;

/* The array of fingers. */
extern srch_fing_struct_type  srch_fings[ ];

/* Non per-finger data */
extern srch_fing_parm_struct_type srch_fings_common;

/* Finger on which 6.67ms delayed PN rolls are created */
extern uint16 srch_delayed_fing;

/* To store the NV parameter for QLIC override */
extern boolean srch_fing_qlic_override;


/*---------------------------------------------------------------------------
        Types and Declarations pertaining to Antenna-specific Parms
---------------------------------------------------------------------------*/
typedef enum
{
  SRCH_ANT0 = 0,      /* Refers to Ant0 status/parms */
  SRCH_ANT1,          /* Refers to Ant1 status/parms */
  SRCH_ANT_MAX        /* Max number of antennae (MUST BE LAST) */
}
srch_ant_enum_type;

/* Struct containing rf/antenna specific default parms */
typedef struct
{
  /* Maps channels to accumulators for antenna x,
     includes TD and SCCH */
  uint8 const fing_accum[ FING_CHAN_MAX ];

  /* add other srch-antenna-specific things here */
}
srch_rf_ant_parms_struct_type;

/* Struct to pull together both sets of default antenna parms.
   These parms should be initialized and declared const if appropriate. */
typedef struct
{
  /* Store array of antennae info here */
  srch_rf_ant_parms_struct_type ant[ SRCH_ANT_MAX ];
}
srch_rf_ant_struct_type;

/* Struct with pointers to antennae data.  Should be set
   to NULL when an antenna is not in use by 1x and should point
   to srch_rf_parms.ant[x] when being used.  Call srch_set_ant_status(...)
   to enable/disable per antenna. */
typedef struct
{
  /* Pointer to array of antennae info */
  srch_rf_ant_parms_struct_type * ant[ SRCH_ANT_MAX ];
}
srch_rf_struct_type;

/* Used by various SRCH functions/macros to indicate per-antenna info */
extern srch_rf_struct_type srch_rf;


/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_FING_RESET

DESCRIPTION    This function does the finger initialization that needs to
               happen for dmod_reset.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Various finger registers are initialized.

===========================================================================*/
extern void srch_fing_reset( void );

/*===========================================================================

FUNCTION        SRCH_FING_RESET_CHANS

DESCRIPTION     This function returns the number of unreserved fingers.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
extern void srch_fing_reset_chans( void );

/*===========================================================================

FUNCTION       SRCH_FING_ACQ_CHAN_ENABLE

DESCRIPTION    This function enables CH0 on all fingers for the ACQ state.

DEPENDENCIES   srch_dmod_reset should have been called.

RETURN VALUE   None.

SIDE EFFECTS   Finger registers are written.

===========================================================================*/
extern void srch_fing_acq_chan_enable( void );

/*===========================================================================

FUNCTION       SRCH_FING_DISABLE

DESCRIPTION    This function disables a range of fingers.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Demod chip registers are written.

===========================================================================*/
extern void srch_fing_disable
(
  uint32 first_fing,      /* first finger to disable */
  uint32 last_fing        /* last finger to disable */
);

/*===========================================================================

FUNCTION       SRCH_FING_ENABLE

DESCRIPTION    This function enables a specific finger.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Demod chip registers are written.

===========================================================================*/
extern void srch_fing_enable
(
  uint16 finger  /* Which finger to disable */
);

/*===========================================================================

FUNCTION       SRCH_FING_DMP

DESCRIPTION    This function gets the status of the demodulating fingers.

DEPENDENCIES   srch_dump_isr_vars,
               srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_dmp
(
  srch_fing_struct_type  fings[ ],    /* The fingers array */
  uint16                 fing_cnt     /* How many fingers in the array */
);

/*===========================================================================

FUNCTION       SRCH_FING_RESTART

DESCRIPTION    This function restarts all fingers after sleep, by resetting the
               RSSI, enabling the finger, and ensuring that the finger starts
               out-of-lock.  It also resets common finger parameters.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Demod chip registers are written.

===========================================================================*/
extern void srch_fing_restart
(
  uint32 first_fing,   /* first finger to restart */
  uint32 last_fing     /* last finger to restart  */
);
/*===========================================================================

FUNCTION       SRCH_FING_MOVE

DESCRIPTION    This function moves the fingers in time, without disturbing
               other finger registers.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Demod chip registers are written.

===========================================================================*/
extern void srch_fing_move
(
  uint16 finger,   /* Which finger to move */
  int32  slew      /* Slew value to move fingers */
);

/*===========================================================================

FUNCTION       SRCH_FING_FORCE_LOCK

DESCRIPTION    This function forces a particular finger into lock, and
               removes the force-lock on a particular finger.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Finger control registers are written.

===========================================================================*/
extern void srch_fing_force_lock
(
  /* Which finger we want to force into lock */
  uint16 finger,

  /* TRUE if we want to force this finger into lock, else remove force lock */
  boolean in_lock
);

/*===========================================================================

FUNCTION       SRCH_FING_SET_TC_WALSH

DESCRIPTION    This function sets the finger walsh codes.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Finger registers are updated.

===========================================================================*/
extern void srch_fing_set_tc_walsh
(
  /* The finger to set */
  uint16 f,

  /* Pointer to the data structure for the given finger */
  srch_fing_struct_type* fing_ptr
);

/*===========================================================================

FUNCTION       SRCH_FING_TC_CMD

DESCRIPTION    This function updates finger registers for SRCH_TC_STATE.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Finger registers are written.

===========================================================================*/
extern void srch_fing_tc_cmd
(
  srch_fing_struct_type fings[],       /* finger array */
  uint16                fing_cnt       /* number of fingers to use */
);

/*===========================================================================

FUNCTION       SRCH_FING_IDLE_CMD

DESCRIPTION    This function updates finger registers for SRCH_ZZ_STATE and
               SRCH_PC_STATE and SRCH_GPS_STATE.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Finger registers are written.

===========================================================================*/
extern void srch_fing_idle_cmd
(
  srch_fing_struct_type fings[],       /* finger array */
  uint16                fing_cnt       /* number of fingers to use */
);

/*===========================================================================

FUNCTION       SRCH_FING_OTHER_CMD

DESCRIPTION    This function updates finger registers for states other than
               SRCH_TC_STATE, SRCH_PC_STATE, and SRCH_ZZ_STATE.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Finger registers are written.

===========================================================================*/
extern void srch_fing_other_cmd
(
  srch_fing_struct_type fings[],       /* finger array */
  uint16                fing_cnt       /* number of fingers to use */
);

/*===========================================================================

FUNCTION       SRCH_FING_SET_RSSI

DESCRIPTION    This function writes a value into the finger RSSI register.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Demod chip registers are written.

===========================================================================*/
extern void srch_fing_set_rssi
(
  uint16 finger,  /* Which finger to write to */
  uint16  rssi    /* RSSI value to write to finger */
);

/*===========================================================================

FUNCTION       SRCH_FING_SET_LECIO

DESCRIPTION    This function initializes a finger to a specific linear ecio.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Demod chip registers are written.

===========================================================================*/
extern void srch_fing_set_lecio
(
  uint16 finger,    /* Which finger to write to */
  uint32 lecio      /* Linear ecio value to write to finger */
);

/*===========================================================================

FUNCTION       SRCH_FING_MERGE_DETECT

DESCRIPTION    This function checks all the finger positions and lock status.
               If two in-lock fingers are too close in position to one another,
               we deassign the weaker one, leaving the stronger one to
               demodulate the energy path. This minimizes cases when there is
               concern that having more than one finger riding on a multipath
               will somehow interfere with data demodulation or power control
               combining.

               Note - it is assumed that this will be executed in the context of
               the finger dump routine such that finger data has just been
               snapshotted.

DEPENDENCIES   srch_fings, srch_fing_cnt, srch_state, srchtri_fing_gran.

RETURN VALUE   None.

SIDE EFFECTS   Updates finger positions in srch_fings structure.
               May deassign one or more fingers.

===========================================================================*/
extern void srch_fing_merge_detect
(
  /* indicates the de-assignment threshold for fingers that have merged */
  uint16  srch_merge_deass_granularity
);

/*===========================================================================

FUNCTION       SRCH_FING_HW_MERGE_DETECT

DESCRIPTION    This function enables/disables hardware finger merge protection.

DEPENDENCIES   Hardware finger merge protection is only supported in MSM5105,
               MSM5100, and newer.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_hw_merge_detect( boolean enable );

/*===========================================================================

FUNCTION       SRCH_FING_GET_MASTER_FINGER_DATA

DESCRIPTION    This function retrieves the pilot offset, position and RSSI of
               the master finger.

DEPENDENCIES   Finger should not be moving at the time of calling this function
               Otherwise this function would just return one sample out of a
               moving position which would potentially create discrepencies in
               the results.

RETURN VALUE   TRUE if return by reference is valid, FALSE otherwise.

SIDE EFFECTS   None.

=============================================================================*/
extern boolean srch_fing_get_master_finger_data
(
  uint16* pn,
  int32*  pos,
  uint16* rssi
);

/*===========================================================================

FUNCTION       SRCH_FING_FORCE_UNLOCK_ALL

DESCRIPTION    This function forces all the fingers out of lock.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   If not followed by a "Force Unlock" removal, it will guarantee
               a call drop!!

===========================================================================*/
extern void srch_fing_force_unlock_all( void );

/*===========================================================================

FUNCTION       SRCH_FING_REMOVE_FORCE_LOCK_ALL

DESCRIPTION    This function removes the forced lock or unlock of all the
               fingers.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_remove_force_lock_all( void );

/*===========================================================================

FUNCTION       SRCH_FING_FREEZE

DESCRIPTION    This function disables the time tracking for each active finger,
               and freezes the combiner time tracking system. Then it reads the
               value from FING_COMB_OFFSET in order to calculate the difference
               between Master Finger and System Time.

DEPENDENCIES   None.

RETURN VALUE   This function returns the finger combiner offset value.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_freeze( void );

/*===========================================================================

FUNCTION       SRCH_FING_UNFREEZE

DESCRIPTION    This function unfreezes the combiner time tracking system, and
               enables the time tracking for each active finger.

DEPENDENCIES   For now, this function can only be called in Traffic Channel.
               Because the time tracking gains are set for TC.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_unfreeze( void );

/*===========================================================================

FUNCTION        SRCH_FING_DELAY_LAST_FINGER

DESCRIPTION     This function reserves the last finger for establishing a
                delayed timeline for standby-improvements.

DEPENDENCIES    Several.

RETURN VALUE    None.

SIDE EFFECTS    Last finger is reserved, disabled, and move 1/4 PN circle
                later than the other fingers.

===========================================================================*/
extern void srch_fing_delay_last_finger( void );

/*===========================================================================

FUNCTION        SRCH_FING_UNDELAY_LAST_FINGER

DESCRIPTION     This function unreserves the last finger which was used to
                establishing a delayed timeline for standby-improvements.

DEPENDENCIES    Several.

RETURN VALUE    None.

SIDE EFFECTS    Last finger is unreserved, enabled, and move back near the
                other fingers.

===========================================================================*/
extern void srch_fing_undelay_last_finger( void );

/*===========================================================================

FUNCTION       SRCH_FING_LOG_ENH_INFO

DESCRIPTION    This function formats and sends a enhanced finger info packet.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_log_enh_info
(
  srch_sect_struct_type *sect_ptr,    /* Pointer to the sector to report on */
  srch_fing_struct_type fings[],      /* The fingers array */
  uint16                fing_cnt      /* Number fingers in the fingers array */
);

/*===========================================================================

FUNCTION       SRCH_FING_SET_CHAIN

DESCRIPTION    This function sets up the fingers to the specified chain

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_set_chain
(
  srch_ant_enum_type antenna          /* antenna fingers should point to. */
);

/*===========================================================================

FUNCTION       SRCH_FING_RX_DIV_START

DESCRIPTION    This function sets up the fingers for Rx Diversity operation.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_rx_div_start(void);

/*===========================================================================

FUNCTION       SRCH_FING_RX_DIV_SCT_OVERRIDE

DESCRIPTION    This function overrides gain params for second chain test

DEPENDENCIES   fing DIV combining must be enabled.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_rx_div_sct_override
(
  boolean override             /* flag to enable/disable override */
);

/*===========================================================================

FUNCTION       SRCH_FING_RX_DIV_STOP

DESCRIPTION    This function disables receive diversity for the finger
               drivers.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_rx_div_stop(void);

/*===========================================================================

FUNCTION       SRCH_FING_SINGLE_RX_DEMOD

DESCRIPTION    This function sets up the fingers for Rx0 demod

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_single_rx_demod( void );

/*===========================================================================

FUNCTION       SRCH_FING_SET_PWR_COMB_THRESH

DESCRIPTION    This function sets the power combiner threshold.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Various finger registers are set.

===========================================================================*/
extern void srch_fing_set_pwr_comb_thresh
(
  uint32 first_fing,      /* first finger to disable */
  uint32 last_fing,       /* last finger to disable */
  uint16 threshold        /* power combiner threshold */
);

/*===========================================================================

FUNCTION       SRCH_FING_SET_COMBINER_GAIN

DESCRIPTION    This function sets the combiner gain.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Various finger registers are set.

===========================================================================*/
void srch_fing_set_combiner_gain
(
  uint32 gain     /* combiner gain */
);

/*===========================================================================

FUNCTION       SRCH_FING_DEASSIGN_FINGER

DESCRIPTION    This function de-assigns a finger for triage.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Various finger registers are set.

===========================================================================*/
void srch_fing_deassign_finger
(
  uint16 finger     /* finger to deassign */
);

/*===========================================================================

FUNCTION       SRCH_FING_RESET_TIME_TRACKING

DESCRIPTION    This function resets time tracking for the specified finger

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_reset_time_tracking
(
  uint32 finger       /* finger number */
);

/*===========================================================================

FUNCTION       SRCH_FING_GET_POSITION

DESCRIPTION    This function returns the current position of a finger.

DEPENDENCIES   None.

RETURN VALUE   The finger position.

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_fing_get_position
(
  uint16 finger     /* finger */
);

/*===========================================================================

FUNCTION       SRCH_FING_GET_COM_RSSI

DESCRIPTION    This function returns the common RSSI of a finger.

DEPENDENCIES   None.

RETURN VALUE   The finger's common RSSI.

SIDE EFFECTS   None.

===========================================================================*/
extern uint16 srch_fing_get_com_rssi
(
  uint16 finger     /* finger */
);

/*===========================================================================

FUNCTION       SRCH_FING_GET_DIV_RSSI

DESCRIPTION    This function returns the diversity RSSI of a finger.

DEPENDENCIES   None.

RETURN VALUE   The finger's diversity RSSI.

SIDE EFFECTS   None.

===========================================================================*/
extern uint16 srch_fing_get_div_rssi
(
  uint16 finger     /* finger */
);

/*=============================================================================

FUNCTION       SRCH_FING_BUILD_FING_INFO_SUB

DESCRIPTION    Builds a Finger Info Sub Packet Version 1.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

=============================================================================*/
extern void srch_fing_build_fing_info_sub
(
  srch_genlog_packet_id_type  id,         /* Id of packet to commit sub */
  sub_commit_func_type        commit_fp   /* F(x) to commit the subpacket */
);

/*=============================================================================

FUNCTION       SRCH_FING_BUILD_FILT_PILOT_SYM_SUB

DESCRIPTION    Builds a Pilot Filtered Sub Packet Version 1.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

=============================================================================*/
extern void srch_fing_build_filt_pilot_sym_sub
(
  srch_genlog_packet_id_type  id,         /* Id of packet to commit sub */
  sub_commit_func_type        commit_fp   /* F(x) to commit the subpacket */
);

/*=============================================================================

FUNCTION       SRCH_FING_GET_PILOT_FILTER_GAIN

DESCRIPTION    Return the current pilot filter gain which is a fuction of
               band class.

DEPENDENCIES   srch_fing_comb_common_params.

RETURN VALUE   Pilot filter gain.

SIDE EFFECTS   None.

=============================================================================*/
extern uint16 srch_fing_get_pilot_filter_gain( void );

/*===========================================================================

FUNCTION       SRCH_FING_GET_RSSI

DESCRIPTION    This function returns the RSSI of a finger.

DEPENDENCIES   None.

RETURN VALUE   The finger's RSSI.

SIDE EFFECTS   None.

===========================================================================*/
extern uint16 srch_fing_get_rssi
(
  uint16 finger     /* finger */
);

/*===========================================================================

FUNCTION SRCH_GET_BEST_FING_RSSI

DESCRIPTION    This function gets the RSSI of the best finger.
               This is intended to be called as a diagnostic tool.

DEPENDENCIES   None.

RETURN VALUE   Word RSSI of best fing assigned and in lock.
               "-1" implies no fingers are in lock.
               "-2" implies no fingers were assigned.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_get_best_fing_rssi( void );

/*===========================================================================

FUNCTION SRCH_FING_GET_MASK

DESCRIPTION    This function takes a pilot PN and returns which fingers
               are assigned to this pilot.

DEPENDENCIES   None.

RETURN VALUE   Bit mask of the fingers assigned to this pilot,
               ie. if bit0 is set, then finger0 is assigned.

SIDE EFFECTS   None.

===========================================================================*/
extern uint8 srch_fing_get_mask
(
  uint16 pilot_pn  /* Pilot PN to check on */
);

/*===========================================================================

FUNCTION SRCH_FING_REASSIGN_SECT

DESCRIPTION    This function essentually does a search and replace for the
               sector pointer.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_reassign_sect
(
  srch_sect_struct_type *old_sect,  /* old sector to find */
  srch_sect_struct_type *new_sect   /* new sector to replace it with */
);

/*===========================================================================

FUNCTION SRCH_FING_DEASSIGN_FINGERS

DESCRIPTION    This function marks the specified fingers to be
               deassigned the next time the srch_fing_cmd is called.
               (Most likely the next triage)

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_deassign_fingers
(
  uint32 first_fing,    /* first finger to mark for deassignment */
  uint32 last_fing      /* last finger to mark for deassignment */
);

/*===========================================================================

FUNCTION       SRCH_FING_LOG_INFO

DESCRIPTION    This function logs finger info with the best packet it can.
               It will start by trying the dedicated finger packets, but if
               none of them are available it will fall back on a searcher
               finger packet.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_log_info
(
  srch_sect_struct_type *sect_ptr, /* Pointer to the active */
  srch_fing_struct_type fings[ ],  /* The fingers array */
  uint16                fing_cnt   /* Num of fingers in the fingers array */
);

/*=============================================================================

FUNCTION       SRCH_FING_INIT

DESCRIPTION    Initializes static, regional, and global variables for
               srch_fing_driver.c.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

=============================================================================*/
extern void srch_fing_init( void );

/*=============================================================================

FUNCTION       SRCH_FING_DEINIT

DESCRIPTION    De-initializes the finger module.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

=============================================================================*/
extern void srch_fing_deinit( void );

/*=============================================================================

FUNCTION       SRCH_FING_DISABLE_ALL_DEMOD_CHANS

DESCRIPTION    This function sets num_chans to 0 on all fingers and then calls
               fing_drv_commit().  This causes all accumulators used by demod
               channels to be disabled and their QOF to be set to 0.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

=============================================================================*/
extern void srch_fing_disable_all_demod_chans( void );

/*===========================================================================

FUNCTION       SRCH_FING_SCCH_ACTIVE

DESCRIPTION    This function determines if an SCCH is active on any sector
               that is assigned to a finger

DEPENDENCIES   None.

RETURN VALUE   TRUE if > 0 SCCHs are active, FALSE otherwise.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_fing_scch_active
(
  srch_fing_struct_type fings[],       /* finger array */
  uint16                fing_cnt       /* number of fingers to use */
);

/*===========================================================================

FUNCTION       SRCH_FING_CHECK_FOR_SCCH

DESCRIPTION    This function determines if an SCCH is active on any sector
               that is assigned to a finger.

DEPENDENCIES   None.

RETURN VALUE   TRUE if > 0 SCCHs are active, FALSE otherwise.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_fing_check_for_scch( void );

/*=============================================================================

FUNCTION       SRCH_FING_GET_ACTIVE_LECIO

DESCRIPTION    Get the active linear ecio from the in lock fingers
               perspective.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

=============================================================================*/
extern uint32 srch_fing_get_active_lecio( void );

/*=============================================================================

FUNCTION       SRCH_FING_UPDATE_ACTIVE_FILTER

DESCRIPTION    Update the finger filter.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

=============================================================================*/
extern void srch_fing_update_active_filter( void );

/*=============================================================================

FUNCTION       SRCH_FING_GET_TT_FING_POS
DESCRIPTION    Get the position of the current time tracking finger.
               If that isn't available, fall back on the search window center.

DEPENDENCIES   None.

RETURN VALUE   Time Tracking Finger position in Cx8.

SIDE EFFECTS   None.

=============================================================================*/
extern uint32 srch_fing_get_tt_fing_pos (void);

/*===========================================================================

FUNCTION        SRCH_FING_GET_FIRST_FING

DESCRIPTION     This function returns the first fing.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
extern uint32 srch_fing_get_first_fing( void );

/*===========================================================================

FUNCTION        SRCH_FING_GET_MAX_FING_CNT

DESCRIPTION     This function returns the max fing count.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
extern uint32 srch_fing_get_max_fing_cnt( void );

/*===========================================================================

FUNCTION        SRCH_FING_GET_UNRESERVED_FING_CNT

DESCRIPTION     This function returns the max fing count.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
extern uint32 srch_fing_get_unreserved_fing_cnt( void );

/*===========================================================================

FUNCTION SRCH_FING_LOAD_PARMS

DESCRIPTION    This function is called to load the finger lock
      and gain parameters.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Finger structures are updated.

===========================================================================*/
extern void srch_fing_load_parms ( void );

/*===========================================================================

FUNCTION SRCH_FING_IN_LOCK

DESCRIPTION    This function checks if any of the fingers are in lock.

DEPENDENCIES   None.

RETURN VALUE   TRUE if any of the fingers are in lock.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_fing_in_lock( void );

/*===========================================================================

FUNCTION       SRCH_FING_ENABLE_QLIC

DESCRIPTION    This function enables/disables QLIC.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Enables QLIC.

===========================================================================*/
extern void srch_fing_enable_qlic
(
  boolean enable
);

/*===========================================================================

FUNCTION       SRCH_FING_OVERRIDE_QLIC

DESCRIPTION    This function enables/disables the overide of QLIC.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Enables QLIC.

===========================================================================*/
extern void srch_fing_override_qlic
(
  boolean override
);

/*===========================================================================

FUNCTION       SRCH_FING_BUILD_QLIC_SUB

DESCRIPTION    This function builds the QLIC sub packet should only be
               called by genlog.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_build_qlic_sub
(
  srch_genlog_packet_id_type id,                 /* id of packet           */
  sub_commit_func_type       commit_fp           /* function to call to
                                                    commit the subpacket   */
);

/*===========================================================================

FUNCTION       SRCH_FING_HANDLE_QLIC_WALSH_LOG_RDY_RSP_MSG

DESCRIPTION    This function is executed when a message is received indicating
               QLIC Logging has been completed.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void srch_fing_handle_qlic_walsh_log_rdy_rsp_msg
(
  cdmafw_qlic_walsh_pwr_logging_rdy_rsp_msg_t* qlic_walsh_pwr_logging_msg_ptr
);

/*===========================================================================

FUNCTION       SRCH_FING_SET_POS

DESCRIPTION    This function writes the finger position for given finger.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void srch_fing_set_pos
(
  uint16 finger,   /* Which finger */
  uint32  pos      /* new finger position */
);

/*===========================================================================

FUNCTION       SRCH_FING_SET_FINGERS_POS

DESCRIPTION    This function writes the finger position for given fingers
               (start_finger to end_finger). Note that immediate writes
               are enabled for this writes and it is expected that this
               function will be called only when fingers are not yet
               enabled.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void srch_fing_set_fingers_pos
(
  uint16  start_finger,   /* From finger */
  uint16  end_finger,     /* Till finger */
  uint32  pos,            /* new finger position */
  uint32  finger_spread   /* Finger spread in cx8 */
);

/*===========================================================================

FUNCTION       SRCH_FING_GET_CELL

DESCRIPTION    This function gets the cell assignment for the specified finger.

DEPENDENCIES   None.

RETURN VALUE   uint16 - Cell id of finger.

SIDE EFFECTS   None.

===========================================================================*/
uint16 srch_fing_get_cell
(
 uint16 finger   /* Which finger */
);

/*===========================================================================

FUNCTION       FING_DEALLOCATE_QLIC_FOR_QOF_SET

DESCRIPTION    This function removes the "weakest" QOF set id in a QLIC
               sector or if a valid Pilot PN information passes to
               this function as a input argument, it removes QLIC engine
               for QOF set id which is associated for that particular PN.

DEPENDENCIES   None.

RETURN VALUE   Index to QLIC engine

SIDE EFFECTS   None.

===========================================================================*/
uint32 fing_dealloc_qlic_for_qof_set
(
uint16    pilot_pn                 /* Pilot PN information */
);

/*===========================================================================

FUNCTION       SRCH_FING_UPDATE_FW_SMEM_ADDR

DESCRIPTION    This function updates the Finger driver module with the
               firmware shared memory address.

DEPENDENCIES   Fing structure dynamic memory must have been allocated.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void srch_fing_update_fw_smem_addr
(
  cdmafw_shared_memory_interface_t* smem_addr    /* FW Shared memory address */
);

/*===========================================================================

FUNCTION       SRCH_FING_SET_DMOD_PILOT_GAIN

DESCRIPTION    This function sets PILOT_FILTER_GAIN according to the current
               frequency range.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Changes the content of the finger pilot filter gain register
               of the demod chip.

===========================================================================*/
extern void srch_fing_set_dmod_pilot_gain
(
  srch_rx_freq_range_type freq_range
);

/*===========================================================================

FUNCTION       SRCH_FING_IS_DIV_CONFIG_ENABLED

DESCRIPTION    This function returns whether diversity configuration is enabled
               or not

DEPENDENCIES   None.

RETURN VALUE   TRUE  -- Diversity configuration is enabled
               FALSE -- Diversity configuration is disabled

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_fing_is_div_config_enabled( void );

/*===========================================================================

FUNCTION       SRCH_FING_IS_ENABLED

DESCRIPTION    This function returns whether finger is enabled or not

DEPENDENCIES   None.

RETURN VALUE   TRUE  -- Finger is enabled
               FALSE -- Finger is disabled

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_fing_is_enabled
(
  uint32 finger_index
);

/*===========================================================================

FUNCTION       SRCH_FING_RE_ASSIGN_QLIC_QOF_SECTORS

DESCRIPTION    This function de-assigns all QLIC sectors first and after that,
               it re-assigns QLIC sectors and send message to FW with changed
               configuration

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_fing_re_assign_qlic_qof_sectors ( void );

/*===========================================================================

FUNCTION       SRCH_FING_GET_PN_OFFSET_VALUE

DESCRIPTION    This function returns pilot pn that is being tracking

DEPENDENCIES   None.

RETURN VALUE   Pilot PN value

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_fing_get_pn_offset_value( uint32 finger_index );

#endif /* SRCH_FING_DRIVER_I_H */
