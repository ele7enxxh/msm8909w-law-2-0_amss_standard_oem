#ifndef DS3GCMIF_STATS_H
#define DS3GCMIF_STATS_H

/*===========================================================================

              D S 3 G C M I F   S T A T S  M O D E M  M O D U L E
 
GENERAL DESCRIPTION:

  DSMGR files that implements the APIs provided to CM. When CM calls these
  APIs, this module in turn either returns the required data or posts command
  to DS task so that all the required calculation is done in DS task.

  Also interacts with Hdrrlpstats.c/.h to get the required Data statistics
  when CM enquires.

EXTERNALIZED FUNCTIONS:
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None. 
 
  Copyright (c) 2010 - 2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

   $Header: //source/qcom/qct/modem/data/common/dsmgr/rel/07.02.01.01/src/ds3gcmif_stats.h#1

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/13    sd     Mainlining Modem Statistics Feature
05/26/11    dvk    Global Variables Cleanup
07/07/10    ms     Data statistics support for FTD. Initial version.
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dsmgr_api.h"
#include "dstask_v.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Field Test Display Field masks for Data services 
---------------------------------------------------------------------------*/  

#define DS_DATA_RATE_STATS_MASK                  0x01
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS3G_PROCESS_STATS_INIT_CMD

DESCRIPTION: 
  This function handles the DS_STATS_INIT_CMD posted by Update_stats_mask
  called from CM task context. This function inturn calls hdrrlpstats
  initialization routines.
 
DEPENDENCIES
  None

PARAMETERS
  DS Command pointer.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_process_stats_init_cmd
(
  ds_cmd_type *cmd_ptr
);

 /*===========================================================================
 FUNCTION DS3G_GET_DS_MODEM_STATS_MASK

 DESCRIPTION
  This function returns Modem Status Mask

 DEPENDENCIES
  None

 PARAMETERS
  None
  
 RETURN VALUE
  unit16 - returns ds_modem_stats_mask

 SIDE EFFECTS
  None
 ===========================================================================*/
 uint16 ds3g_get_ds_modem_stats_mask
 (
   void
 );

 /*===========================================================================
 FUNCTION DS3G_GET_DS_MODEM_STATS_CHANGED_FIELDS_BIT_MASK

 DESCRIPTION
  This function returns Modem STATUS CHANGED FIELDS BIT MASK

 DEPENDENCIES
  None

 PARAMETERS
  None
  
 RETURN VALUE
  unit16 - ds_modem_stats_changed_fields_bit_mask

 SIDE EFFECTS
  None
 ===========================================================================*/
 uint16 ds3g_get_ds_modem_stats_changed_fields_bit_mask
 (
   void
 );

 /*===========================================================================
 FUNCTION DS3G_SET_DS_MODEM_STATS_CHANGED_FIELDS_BIT_MASK

 DESCRIPTION
  This function sets Modem STATUS CHANGED FIELDS BIT MASK

 DEPENDENCIES
  None

 PARAMETERS
  uint16 - changed_fields_bit_mask
  
 RETURN VALUE
  None.

 SIDE EFFECTS
  None
===========================================================================*/
void ds3g_set_ds_modem_stats_changed_fields_bit_mask
(
  uint16 changed_fields_bit_mask
);

#endif /* #ifndef DS3GCMIF_STATS_H */
