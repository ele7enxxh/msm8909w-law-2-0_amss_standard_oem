#ifndef HDRRLPSTATS_H
#define HDRRLPSTATS_H
/*===========================================================================

                      H D R R L P   S T A T S
 
GENERAL DESCRIPTION:
  This file is the Interface for DSMGR to get the required statistics related
  to HDR MRLPs. Whenever client /CM interested to know about the data rate,
  they notify the data services using DSMGR- CM Interface.

  DSMGR modules informs the HDRRLPSTATS module about the statistics mask
  that might have got set or cleared. If the mask is set, then this module
  initializes the required data and starts the timer etc. Once the timer
  expires, the required data rate on Fwd and Rev link are calculated. This
  file also provides an API to enquire the latest Data rates(fwd/rev link).
  
  It also contains the callback function registered with timer module that gets
  called when stats timer gets expired. During cleanup, clears the
  timer and resets all the statistics calculated so far.

EXTERNALIZED FUNCTIONS:
  hdrrlpstats_process_sig()
  hdrrlpstats_init()
  hdrrlpstats_get_data_rate()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrrlpstats_init() takes care of initializing the required data,timers etc.
  hdrrlpstats_stop_timer() takes care of stopping the stats timer.

  Copyright (c) 2010-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/modem/data/1x/hdrdata/rel/07.02.01.04/src/hdrrlpstats.h#1

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/13    sd     Mainlining Modem Statistics Feature
07/07/10    ms     Data statistics support for FTD. Initial version.
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "rex.h"
#include "sys.h"
#include "cm.h"
#include "hdrrlp_api.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*===========================================================================

              EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION HDRRLP_CLEAR_DATA_RATE_STATS_MASK

DESCRIPTION
  This function Clears the Modem Statistics mask, whenever Client requested
  to clear the stats mask. It memsets the Fwd and Rev link data rate and
  stops the stats timer.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS 
  None 
===========================================================================*/
void hdrrlp_clear_data_rate_stats_mask
(
  void
);

/*=========================================================================== 
FUNCTION       HDRRLPSTATS_GET_DATA_RATE 
 
DESCRIPTION    This function is an API to get the latest values of Fwd 
               and Rev Data rate 
                
DEPENDENCIES   None. 

PARAMETERS     ds_link_info
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void hdrrlpstats_get_data_rate
(
  sys_ds_hdr_link_info_s_type *ds_link_info
);

/*===========================================================================
FUNCTION HDRRLPSTATS_INIT

DESCRIPTION
  This function initializes the stats timer and the required parameters
  to calculate the Forward And Reverse data rates. Updates the number of
  bytes Txed and Rxed so far over all the RLP flows that are active at
  that point of time.
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrlpstats_init
(
  void
);
#endif /* HDRRLPSTATS_H */

