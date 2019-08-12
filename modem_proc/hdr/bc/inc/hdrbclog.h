#ifndef HDRBCLOG_H
#define HDRBCLOG_H
/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   B R O A D C A S T   P R O T O C O L 
                     L O G    F U N C T I O N S

GENERAL DESCRIPTION

   This file contains the typedefs, enums and defines used in
   the HDR Broadcast Protocol logging and diag-related functions.
   
EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS 

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004, 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/inc/hdrbclog.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/10/05   etv     Fixed lint errors.
02/02/05   etv     Fixed coding style to match convention.
11/04/04   kss     Support to enable/disable periodic metric logging.
06/26/04   kss     Created module
==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "comdef.h"

/*==========================================================================


                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

                                                      
/* Period for metric logging, in ms */
#define HDRBCLOG_LOGGING_PERIOD          20000UL
 
                                                      
/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/
 

/*===========================================================================

FUNCTION HDRBCLOG_DISABLE_SOFT_COMBINING

DESCRIPTION
  Disables soft combining in broadcast.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbclog_disable_soft_combining( void );


/*===========================================================================

FUNCTION HDRBCLOG_ENABLE_SOFT_COMBINING

DESCRIPTION
  Re-enables soft combining in broadcast (previous disabled by 
  hdrbclog_disable_soft_combining).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbclog_enable_soft_combining( void );

/*===========================================================================

FUNCTION HDRBCLOG_SET_ECB_DROP_MASK

DESCRIPTION
  Sets drop mask in the Broadcask MAC's Error Control Black handling.
  The bitmask indicates which packets in each row of the ECB to drop 
  (intended for testing and analysis of Reed-Solomon decoding).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbclog_set_ecb_drop_mask(uint32 mask);


/*===========================================================================

FUNCTION HDRBCLOG_CLEAR_LOGICAL_CHANNEL_METRICS

DESCRIPTION
  The function clears all logical channel metrics.
  
DEPENDENCIES
  None.

PARAMETERS
  None.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbclog_clear_mcs_chan_metrics(void );


/*=========================================================================

FUNCTION HDRBCLOG_LOG_MCS_CHAN_METRICS

DESCRIPTION
  Logs HDR BCMCS logical channel metrics (the HDR_MCS_LOG_CHAN_METRICS
  packet).

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrbclog_log_mcs_chan_metrics( void );


/*=========================================================================

FUNCTION HDRBCLOG_LOG_PHY_CHAN_METRICS

DESCRIPTION
  Logs HDR BCMCS physical channel metrics (the HDR_MCS_PHY_CHAN_METRICS
  packet).

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrbclog_log_phy_chan_metrics( void );


/*=========================================================================

FUNCTION HDRBCLOG_ENABLE_PERIODIC_LOGGING

DESCRIPTION
  Enables periodic logging of BCMCS stats.

DEPENDENCIES
  None

PARAMETERS
  period - the period for logging the packets (in ms)

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrbclog_enable_periodic_logging
(
  uint32 period
);


/*=========================================================================

FUNCTION HDRBCLOG_DISABLE_PERIODIC_LOGGING

DESCRIPTION
  Disables periodic logging of BCMCS stats.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrbclog_disable_periodic_logging( void );


#endif /* HDRBCLOG_H */
