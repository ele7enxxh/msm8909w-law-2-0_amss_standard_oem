#ifndef SRCH_GPS_TRANS_H
#define SRCH_GPS_TRANS_H
/*===========================================================================

              S R C H _ G P S _ T R A N S    H E A D E R    F I L E

DESCRIPTION
   This file contains definitions used to interface between searcher and
   gps.

  Copyright (c) 2004 - 2013 Qualcomm Technologies, Inc. 
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

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/srch_gps_trans.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Eliminated implicit includes.
12/06/05   bt      Remove srch_gps_trans_get_home_band and 
                   srch_gps_trans_get_home_chan
11/02/05   awj     Added trans functions for srch_rx calls, stripped PCG
10/28/05   br      Added proper featurization for friendly wakeup.
10/10/05   bt      Added srch_gps_trans_1x_connected.
10/05/05   bt      Added srch_gps_trans_get_1x_serving_band_chan.
08/18/05   br      Simplified "Friendly Wake-up" mechanism.
06/29/05   br      Removed unused functions. Removed GPS/PPM featurizations.
06/28/05   ejv     Remove include of srch_hw_t.h.  Change srch_trpt_struct_type
                   to a uint8, since only ecio value is required.
06/06/05   br      Mainline feature GPSONE_PGI2.
05/31/05   rng     Merged in from SHDR Sandbox.
05/23/05   aar     Mainlined FEATURE_GPS_SRCH4_ENGINE.
05/18/05   kwo     Added srch_gps_trans_get_tt_fing_position() and
                   srch_gps_trans_get_max_fing_cnt(), 
                   srch_gps_trans_get_current_sci()
05/11/05   br      Included proper file.
05/04/05   kwo     Added srch_gps_trans_get_current_sci()
04/19/05   gs/aar  Added srch_gps_trans_request_rvafc().
03/09/05   sfm     Added srch_gps_trans_needs_ant1() and
                   srch_gps_trans_has_antenna1()
11/12/04   sfm     Changed srch_gps_trans_fing_get_master_finger_data() to
                   return a boolean indicating wether data was available
10/08/04   abh     Removed obsoleted functions srch_gps_save/restore_context().
10/07/04   sfm     Added several new functions for finger driver interface
10/07/04   sfm     Added srch_gps_trans_get_master_finger_data() and
                   srch_gps_trans_get_position()
09/30/04   br      Added more interfacing functions.
06/22/04   rng     Added srch_gps_trans_friendly_wakeup()
04/05/04   rng     Update to current coding standards.
03/23/04   rng     Created this file to hold SRCH/GPS interface functions.

===========================================================================*/


/*==========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"

/* Srch */
#include "srchmc.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Constants and Macros
--------------------------------------------------------------------------*/

/* How long are we allowed to tune away on traffic?  */
#define SRCH_GPS_TC_TUNEAWAY_MS 2100

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* Struct to hold the pertinent data to pass to the GPS subsystem */
typedef struct
{
  uint16                 pilot;
  uint32                 win_siz;
  uint8                  band_class;
  uint16                 cdma_freq;
  uint8                  ecio;
} srch_gps_sect_type;

/* Struct to hold the pertinent data to pass to the GPS subsystem */
typedef struct
{
  uint16                 pilot;
  uint32                 win_siz;
} srch_gps_nset_sect_type;

/* The Active set collection of sectors */
typedef struct
{
  byte sect_cnt;
  srch_gps_sect_type  sectors[ SRCH_ASET_MAX ];
} srch_gps_aset_type;

/* The Candidate set collection of sectors */
typedef struct
{
  byte sect_cnt;
  srch_gps_sect_type  sectors[ SRCH_CSET_MAX ];
} srch_gps_cset_type;

/* The Neighbour set collection of sectors */
typedef struct
{
  byte sect_cnt;
  srch_gps_nset_sect_type  sectors[ SRCH_NSET_MAX ];
} srch_gps_nset_type;

#define SRCH_GPS_TRANS_NO_TUNE_AGC    0xFF
#define SRCH_GPS_TRANS_NO_TUNE_BAND   0xFF
#define SRCH_GPS_TRANS_NO_TUNE_CHAN   0xFFFF

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */


/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_GPS_TRANS_GET_HOME_RX_AGC

DESCRIPTION    This function returns the AGC of the 1x demod chain

DEPENDENCIES   This function can be called at any time.

RETURN VALUE   Current AGC, or AGC when last tuned 

SIDE EFFECTS   None.

===========================================================================*/
int8 srch_gps_trans_get_home_rx_agc( void );

/*===========================================================================

FUNCTION SRCH_GPS_TRANS_GET_ASET

DESCRIPTION    This function fetches the data needed for PPM from the searcher
               sector structures.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_gps_trans_get_aset
(
  srch_gps_aset_type *aset   /* Pointer to data storage */
);

/*===========================================================================

FUNCTION SRCH_GPS_TRANS_GET_NSET

DESCRIPTION    This function fetches the data needed for PPM from the searcher
               sector structures.  This only returns same frequency neighbors.
                
DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_gps_trans_get_nset
(
  srch_gps_nset_type *nset  /* Pointer to data storage */
);

/*===========================================================================

FUNCTION SRCH_GPS_TRANS_GET_CSET

DESCRIPTION    This function fetches the data needed for PPM from the searcher
               sector structures.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_gps_trans_get_cset
(
  srch_gps_cset_type *cset  /* Pointer to data storage */
);

/*===========================================================================

FUNCTION SRCH_GPS_TRANS_GET_NSET_WINSIZE

DESCRIPTION    This function fetches the window size of the first same 
               frequency neighbor.

DEPENDENCIES   None.

RETURN VALUE   uint32 win_size.

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_gps_trans_get_nset_winsize ( void );

/*===========================================================================

FUNCTION       SRCH_GPS_TRANS_FING_GET_MASTER_FINGER_DATA

DESCRIPTION    This function retrieves the pilot offset, position and RSSI of
               the master finger.

DEPENDENCIES   Finger should not be moving at the time of calling this function
               Otherwise this function would just return one sample out of a
               moving position which would potentially create discrepencies in
               the results.

SIDE EFFECTS   None.

=============================================================================*/
extern boolean srch_gps_trans_fing_get_master_finger_data
(
  uint16* pn,     /* returned PN offset */
  int32*  pos,    /* returned position */
  uint16* rssi    /* returned RSSI */
);

/*===========================================================================

FUNCTION       SRCH_GPS_TRANS_GET_TT_FING_POSITION

DESCRIPTION    This function returns the time tracking finger position

DEPENDENCIES   None.

RETURN VALUE   The finger position.

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_gps_trans_get_tt_fing_position (void);


/*===========================================================================

FUNCTION SRCH_GPS_TRANS_ENABLE_COMB_DSP_INTS

DESCRIPTION    This function enables/disables Combiner interrupts to the DSP.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/

extern void srch_gps_trans_enable_comb_dsp_ints
(
  boolean enable
);

/*===========================================================================

FUNCTION SRCH_GPS_TRANS_SET_PNROLL_ISR

DESCRIPTION    This function registers/deregisters PN Roll ISR.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/

extern void srch_gps_trans_set_pnroll_isr
(
  boolean enable
);

/*===========================================================================

FUNCTION        SRCH_GPS_TRANS_GET_CURRENT_SCI

DESCRIPTION     This function returns the current sci 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.

===========================================================================*/
extern int8 srch_gps_trans_get_current_sci( void );

/*===========================================================================

FUNCTION       SRCH_GPS_TRANS_SEND_TRYSLEEP_RPT

DESCRIPTION    This function sends the SRCH_TRYSLEEP_R command to MC so that
               MC sends a SLEEP_F cmd immediately to SRCH. This function
               should only be used when doing PPM on PC. The reason being,
               doing PPM on PC can cause SRCH to reject MC's SLEEP_F cmd if
               the PPM searching is not done. However, after the PPM searching
               is done, if we do NOT want to wait forever for MC to send
               SRCH back to sleep, one can call this function.

DEPENDENCIES   This function only sends the cmd IF srchppm module ever
               refuses to go to sleep (SLEEP_F) due to PPM. Please see
               srch_ppm_stay_in_pc() function for details.

RETURN VALUE   None.

SIDE EFFECTS   If called properly, soon after MC sends a SLEEP_F cmd to SRCH.

===========================================================================*/
extern boolean srch_gps_trans_send_trysleep_rpt( void );

/*===========================================================================

FUNCTION        SRCH_GPS_TRANS_GET_1X_SERVING_BAND_CHAN

DESCRIPTION     This function returns the 1X serving band and channel

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    band is set to serving band or SRCH_RX_NO_TUNE_BAND
                chan is set to serving chan or SRCH_RX_NO_TUNE_CHAN

===========================================================================*/
extern void srch_gps_trans_get_1x_serving_band_chan
(
  uint8  *band,
  uint16 *chan
);

/*===========================================================================

FUNCTION        SRCH_GPS_TRANS_1X_CONNECTED

DESCRIPTION     This function returns whether we are talking to a BS

DEPENDENCIES    None

RETURN VALUE    TRUE if in traffic, FALSE otherwise.

SIDE EFFECTS    None.

===========================================================================*/
extern boolean srch_gps_trans_1x_connected(void);

#endif /* SRCH_GPS_TRANS_H */
