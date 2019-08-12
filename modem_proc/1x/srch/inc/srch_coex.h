#ifndef SRCH_COEX_H
#define SRCH_COEX_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S E A R C H   COEX  HEADER FILE

GENERAL DESCRIPTION
     This module is for the 1X Coex Manager support as part of the Dual Sim
     Dual Active Functionality.


  Copyright (c) 2013 - 2013 Qualcomm Technologies, Inc. 
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

$Header:

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/12/14   jh      added srch_coex_set_band_avoid and NV item to enable
10/08/14   jh      added srch_coex_power_updates prototype
01/08/14   ab      DDR power management Support / Feature Cleanup
10/11/13   eye     Moved DSDA mode check API in coex header.
07/30/13   srk     Use MCS API to determine if the frame could be de-sensed.
06/04/13   ab      Cleanup coex manager changes
05/20/13   eye     Created internal srch coex header.
03/21/13   ab      Implementation, first cut.
===========================================================================*/


/*===========================================================================

        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"

#ifdef FEATURE_MODEM_1X_COEXISTENCE

/* Srch */
#include "srch_msgr.h"

/* Other */
#include "slpc.h"
#include "stm.h"
#include "timer.h"
#include "wwan_coex_mgr.h"
#include "cdmafw_msg.h"


/*===========================================================================
          LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/


/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/
typedef enum
{
  /* No RX activity expected */
  COEX_RX_ACTIVITY_NONE,

  /* Tier 1 RX  activities  ->  ACTIVITY_TIER_10 */
  COEX_RX_REACQ_TUNE_IDLE,
  COEX_RX_REACQ_SRCH_IDLE,

  /* Tier 2  RX  activities ->  ACTIVITY_TIER_20 */
  COEX_RX_SRCH_ACQISITION,
  COEX_RX_SRCH_IDLE,
  COEX_RX_SRCH_TRAFFIC,
  COEX_RX_ACTIVITY_MAX
}
coex_rx_activity_enum;

typedef enum
{
  /* No TX activity expected */
  COEX_TX_ACTIVITY_NONE,

  /* Tier 2  TX  activities ->  ACTIVITY_TIER_20 */
  COEX_TX_FRAME_TRANSMIT
}
coex_tx_activity_enum;

/*===========================================================================

FUNCTION      SRCH_COEX_DISABLE_DTX

DESCRIPTION   This function returns TRUE if Device only DTX needs to be
              disabled in DSDA operation.

DEPENDENCIES  None

RETURN VALUE  TRUE  : MCS asks 1x to disable DTX
              FALSE : It is OK to DTX current frame

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_coex_disable_dtx( void );

/*===========================================================================

FUNCTION      SRCH_COEX_DSDA_IS_ENABLED

DESCRIPTION   This function returns TRUE if DSDA is enabled
          
DEPENDENCIES  None

RETURN VALUE  TRUE  : DSDA ENABLED
              FALSE : DSDA DISABLED

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_coex_dsda_is_enabled( void );

/*===========================================================================

FUNCTION      SRCH_COEX_SEND_FREQUENCY_INDICATION

DESCRIPTION   This function sends frequency indication MSG to MCS-COEX

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
extern void srch_coex_send_frequency_indication
(
  /* RX activity info */
  coex_rx_activity_enum rx_activity,

  /* TX activity info */
  coex_tx_activity_enum tx_activity
);

/*===========================================================================

FUNCTION      SRCH_COEX_POWER_UPDATES

DESCRIPTION   Function called by SRCHCOM_SM, which calls update power functions

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
extern void srch_coex_power_updates( void );

/*===========================================================================

FUNCTION       SRCH_COEX_SET_BAND_AVOID

DESCRIPTION    This function enables/disables Band Avoidance

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_coex_set_band_avoid
(
  boolean enable
);

#endif /* FEATURE_MODEM_1X_COEXISTENCE */

#endif /* SRCH_COEX_H */
