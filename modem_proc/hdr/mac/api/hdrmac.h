#ifndef HDRMAC_H
#define HDRMAC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

 
         M A C   L A Y E R   C O M M O N   D E C L A R A T I O N S

DESCRIPTION
  This header file contains general types and macros that are common to the 
  HDR MAC layer.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*=============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrmac.h_v   1.4   08 Jan 2002 14:29:18   kevins  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrmac.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
01/31/15   vke     Added API to check silence frame
10/21/14   vke     Removal of connection close enum
10/10/14   arm     Fixed compilation warning in hdrrmac
09/22/14   arm     HDR QTA code cleanup
09/09/14   vke     Redesign fix for connection close during QTA gap
06/04/14   arm     Changes to support DO to G QTA
08/06/13   rmg     Added power down reason to RMAC power down command.
10/19/11   smd     SU API cleanup update.
04/25/11   ljl     Added tx powerdown callback.
07/13/07   sju     Changed hdrmac_mac_index_type to uint16 for revB
06/13/07   etv     Added support for OFS Outage notification.
09/14/06   kss     Add hdrrmac_notify_suspend().
11/17/03   kss     Add time parameter to hdrrmac_resume_connection.
01/07/02   kss     Added function prototypes for powerup, powredown, suspend, 
                   and resume.
09/15/00   dna     Created module.

=============================================================================*/

/* <EJECT> */

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "hdrl1_api.h"
/*=============================================================================

                            Data Declarations

=============================================================================*/
typedef uint8 hdrmac_rab_len_type;
typedef uint8 hdrmac_rab_offset_type;
typedef uint8 hdrmac_drc_len_type;
typedef uint16 hdrmac_mac_index_type;
typedef uint8 hdrmac_drc_cover_type;

/* Value used to cancel a suspend time set with hdrrmac_notify_suspend(). */
#define HDRRMAC_CANCEL_SUSPEND -1

/* QTA Total time taken */
#define HDRRMAC_QTA_TOTAL_TA_TIME_FRAMES 3 

/* Quad word frame compare macro */
#define HDRRMAC_QW_FRAME_CMP(frame1, frame2) (qw_cmp(frame1, frame2) >=0)

/* Tx stop reason */
typedef enum
{
  HDRRMAC_TX_STOP_FOR_HHO,
  HDRRMAC_TX_STOP_FOR_RETUNE,
  HDRRMAC_TX_STOP_FOR_TUNEAWAY,
  HDRRMAC_TX_STOP_FOR_DEACTIVATE,
#ifdef FEATURE_HDR_QTA
  HDRRMAC_TX_STOP_FOR_TUNEAWAY_QTA,
#endif /* FEATURE_HDR_QTA */
  HDRRMAC_TX_STOP_REASON_MAX
} hdrrmac_tx_stop_reason_enum_type;

/* Callback function prototype for tx powerdown */
typedef void (*hdrrmac_tx_powerdown_cb_type) ( void );

/*===========================================================================

FUNCTION HDRRMAC_NOTIFY_SUSPEND

DESCRIPTION
  This function tells the RTC MAC about an upcoming tune away.
    
DEPENDENCIES
  None.

PARAMETERS
  slots_to_suspend - number of slots until suspension (tuneaway) is 
                     scheduled.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrrmac_notify_suspend
(
  int slots_until_suspend
);

/*===========================================================================

FUNCTION HDRRMAC_SUSPEND_CONNECTION                            

DESCRIPTION
  This function tells the MAC layer to cease sending data over the reverse
  link.  After the currently Tx'ing frame is complete, the PA will be turned
  off.  
  
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac_suspend_connection( void );

/*===========================================================================

FUNCTION HDRRMAC_RESUME_CONNECTION

DESCRIPTION
  This function tells the MAC layer that it may resume sending data over the
  reverse link.

DEPENDENCIES
  None.

PARAMETERS
  ms - the time (in mS) that the link was off.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrmac_resume_connection
(
  int ms  /* time, in mS, that connection was suspended. */
#ifdef FEATURE_HDR_QTA
  ,boolean is_qta   /* Is the resume connection after QTA? */
#endif /* FEATURE_HDR_QTA */
);

/*===========================================================================

FUNCTION HDRRMAC_POWERDOWN_TX

DESCRIPTION
  Powers down the Tx section of RF.  Must only be called when the RF is 
  needed while an HDR connection is up.
      
DEPENDENCIES
  None.

PARAMETERS
  powerdown_reason - reason for tx powerdown
  cb               - callback to be called once powerdown is complete
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac_powerdown_tx
( 
  /* reason for powerdown */
  hdrrmac_tx_stop_reason_enum_type powerdown_reason, 

  /* callback for tx powerdown */
  hdrrmac_tx_powerdown_cb_type cb 
);        

/*===========================================================================

FUNCTION HDRRMAC_POWERUP_TX

DESCRIPTION
  Powers up the Tx section of RF.  hdrrmac_powerdown_tx() must have been 
  called first.
      
DEPENDENCIES
  Hdrrmac_powerdown_tx() must have been called.
  
PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrmac_powerup_tx
(
#ifdef FEATURE_HDR_QTA 
   boolean is_qta /* Is power up tx after QTA? */
#else
  void
#endif /* FEATURE_HDR_QTA */
);

#ifdef FEATURE_HDR_QTA
/*=============================================================================
FUNCTION HDRRMAC_DEACTIVATE_ON_TUNEBACK                     EXTERNAL

DESCRIPTION
  This function post pending deactivate command after HDR_SRCH tuneback

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
==============================================================================*/
void hdrrmac_deactivate_on_tune_back( void );
/*=============================================================================
FUNCTION HDRRMAC_IS_TX_ON                                        EXTERNAL

DESCRIPTION
  This function checks if TX is on.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  TRUE: if TX is ON ( TX_POWERING_UP, TX_ON, TX_OFF_RF_ON )
  FALSE: if TX is OFF ( both RF & SW )

SIDE EFFECTS
  None.
==============================================================================*/
boolean hdrrmac_is_tx_on( void );
#endif /* FEATURE_HDR_QTA */

/*===========================================================================

FUNCTION HDRRMAC_IS_SILENCE_FRAME                                    EXTERNAL

DESCRIPTION
  This function checks whether the given frame is in silence duration

DEPENDENCIES
  None
  
PARAMETERS
  frame_num   -  frame which needs to be checked for RLSI

RETURN VALUE
  TRUE - Given Frame is in silence duration
  FALSE - Given Frame is not in silence duration
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrrmac_is_silence_frame 
( 
  qword frame_num   /* frame which needs to be checked for RLSI */
);

#endif /* HDRMAC_H */
