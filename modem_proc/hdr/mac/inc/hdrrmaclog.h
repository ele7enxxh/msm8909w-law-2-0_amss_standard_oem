#ifndef HDRRMACLOG_H
#define HDRRMACLOG_H
/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   R M A C   L O G   F U N C T I O N S

GENERAL DESCRIPTION

   This file contains the databases, typedefs, enums and defines used in
   the HDR RMAC logging functions.

EXTERNALIZED FUNCTIONS

  hdrrmaclog_update_tune_away_rri_info
  hdrrmaclog_reset_tune_away_rri_info
  hdrrmaclog_log_tune_away_rri_info
  hdrrmaclog_reset_conn_state_stats
  hdrrmaclog_inc_conn_state_stats_counter 
  hdrrmaclog_log_conn_state_stats
  
REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/inc/hdrrmaclog.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/05/04   grl     Created this file
==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "comdef.h"
#include "hdrlogi.h"
#include "hdrrmac.h"
  
/*==========================================================================


                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

/* Which statistics that are being tracked to clear for logging purposes. */
typedef enum 
{
  HDRRMACLOG_ALL_STATS,
    /* Clear all of the statistics tracked in this log packet. */

  HDRRMACLOG_LAST_CALL_STATS,
    /* Clear only the statistics tracked related to the last call. */

} hdrrmaclog_reset_stats_type;

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRRMACLOG_UPDATE_TUNE_AWAY_RRI_INFO

DESCRIPTION  : This function saves the RRI information before and after 
               the last HDR to 1x tune away so that it may be logged
               when requested.
               
DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrrmaclog_update_tune_away_rri_info(
  hdrrmac_rate_enum_type rri_before,
    /* RRI before the tune away. */

  hdrrmac_rate_enum_type rri_after
    /* RRI after the tune away. */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRRMACLOG_LOG_TUNE_AWAY_RRI_INFO

DESCRIPTION  : This function logs the tune_away info log packet. It gets a
               log record from the diag log buffer, fills it with the latest 
               connected state info and commits the log record to
               the diag for delivery. 
               
DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrrmaclog_log_tune_away_rri_info( void );

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHLOG_RESET_TUNE_AWAY_RRI_INFO

DESCRIPTION  : This function resets the saved rri information from before
               and after the last tune away.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrrmaclog_reset_tune_away_rri_info
( 
  hdrrmaclog_reset_stats_type stats_reset
    /* Which statistics to clear... all or just the ones related to the 
       last call. */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRRMACLOG_LOG_CONN_STATE_STATS

DESCRIPTION  : This function logs the connected state stats. It gets a log
               record from the diag log buffer, fills it with the latest 
               connected state stats and commits the log record to
               the diag for delivery.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrrmaclog_log_conn_state_stats( void );

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRRMACLOG_INC_CONNECTED_STATE_STATS_COUNTER

DESCRIPTION  : Increments the appropriate connected state counter by
               the specified amount.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrrmaclog_inc_conn_state_stats_counter
( 
  hdrrmaclog_conn_state_stats_count_type counter,
    /* The counter to increment. */

  uint16                                 inc_count
    /* How much to increment the counter by. */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRRMACLOG_RESET_CONN_STATE_STATS

DESCRIPTION  : This function resets the RMAC connected state stats as
               specified by the stats reset parameter.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrrmaclog_reset_conn_state_stats
( 
  hdrrmaclog_reset_stats_type stats_reset
    /* Which statistics to clear... all or just the ones related to the 
       last call. */
);

#endif
