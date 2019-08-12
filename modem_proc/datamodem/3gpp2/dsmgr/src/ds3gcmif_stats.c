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

  Copyright (c) 2010-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gcmif_stats.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/13    sd     Mainlining Modem Statistics Feature
05/26/11    dvk    Global Variables Cleanup
11/16/10    ms     Fixed compilation error for UMTS only flavor.
07/07/10    ms     Data statistics support for FTD. Initial version.
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "data_msg.h"

#include "ds3gcmif_stats.h"

#ifdef FEATURE_HDR
#include "hdrrlpstats.h"
#endif /* FEATURE_HDR */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Type for DS_STATS_INFO_TYPE */
typedef struct
{
  uint16 bit_mask;
} ds_stats_info_type;

/*===========================================================================

              EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* Global Statistics mask that can be used by all the sub modules in
   Data Services */
static uint16  ds_modem_stats_mask                    = SYS_DS_INFO_MASK_NONE;

static uint16  ds_modem_stats_changed_fields_bit_mask = SYS_DS_INFO_MASK_NONE;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*===========================================================================
FUNCTION
  DS3G_PROCESS_STATS_INIT_CMD

DESCRIPTION:
  This function handles the DS_STATS_INIT_CMD posted by Update_stats_mask
  called from CM task context. This function inturn calls hdrrlpstats
  initialization routines.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_process_stats_init_cmd
(
  ds_cmd_type *cmd_ptr
)
{
  ds_stats_info_type *stats_info_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG0(MSG_LEGACY_LOW, "Data ds3g_update_stats_mask");

  if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
    ASSERT(0);
    return;
  }

  if(cmd_ptr->hdr.cmd_id != DS_CMD_STATS_INIT)
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Incorrect command:%d", cmd_ptr->hdr.cmd_id);
    return;
  }

  stats_info_ptr = (ds_stats_info_type*)cmd_ptr->cmd_payload_ptr;

  if( stats_info_ptr->bit_mask == ds_modem_stats_mask )
  {
    /* Nothing to do , so return */
    DATA_MSG0(MSG_LEGACY_LOW,"No change in stats mask ");
    return;
  }

  DATA_MSG1(MSG_LEGACY_LOW," Current DS707_MODEM_STATS_MASK = %x ",
            ds_modem_stats_mask);

  if( stats_info_ptr->bit_mask & DS_DATA_RATE_STATS_MASK )
  {
     /* HDRMRLP Data Rate MASK SET requested by CM/Client,
        so call the corresponding function to take necessary action.
     */
     /* if it is already set, then ignore and return */
     if( ds_modem_stats_mask & DS_DATA_RATE_STATS_MASK )
     {
       DATA_MSG0(MSG_LEGACY_LOW," HDRMRLP Mask already set, so Ignore ");
     }
     else /* this mean, it was Off and setting it to 1 now. */
     {
       ds_modem_stats_mask |=
         (stats_info_ptr->bit_mask & DS_DATA_RATE_STATS_MASK);

        /* Call the Function registered during Power up */
       #ifdef FEATURE_HDR
       /*  Call the HDRLPSTATS Initialization Routines */
       hdrrlpstats_init();
       #endif /* FEATURE_HDR */
     }
  }
  else
  {
    /* We dont need to stop the running timer. We just dont restart the
     * stats timer as the below bitmask gets cleared
     */
    ds_modem_stats_mask = stats_info_ptr->bit_mask;
  }

  /* for future purpose , similary check if 2nd, or
   * 3rd bit etc are modified or not
   */

  DATA_MSG1(MSG_LEGACY_LOW,"Modified DS707_MODEM_STATS_MASK = %x",
            ds_modem_stats_mask);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return;
}/* ds3g_process_stats_init_cmd */

/*===========================================================================
FUNCTION DS3G_GET_DS_MODEM_STATS_MASK

DESCRIPTION
  This function returns Modem Status Mask

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  unit16 - ds_modem_stats_mask

SIDE EFFECTS
  None
===========================================================================*/
uint16 ds3g_get_ds_modem_stats_mask
(
  void
)
{
  return ds_modem_stats_mask;
}

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
)
{
  return ds_modem_stats_changed_fields_bit_mask;
}

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
)
{
  ds_modem_stats_changed_fields_bit_mask |= changed_fields_bit_mask;
}

/*===========================================================================
FUNCTION DS3G_UPDATE_STATS_MASK

DESCRIPTION
  This function is an Interface to CM to Set the DS Statistics Modem Mask
  this function posts the DS_STATS_INIT_CMD and passes the updated mask
  sent by the CM

DEPENDENCIES
  None

PARAMETERS
  Bitmask of type uint16

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_update_stats_mask
(
  uint16  bitmask
)
{
  ds_cmd_type         *cmd_ptr = NULL;
  ds_stats_info_type  *stats_info_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG0(MSG_LEGACY_LOW, "Data ds3g_update_stats_mask");

  /* Post a command to DS TASK */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_stats_info_type));

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  else
  {
    DATA_MSG0(MSG_LEGACY_LOW, "Posting STATS Init command to DSTASK ");
    cmd_ptr->hdr.cmd_id = DS_CMD_STATS_INIT;

    /* We do pass the Bit_mask, but as of now we dont find any use of it.
     * keeping future in mind,  if at all bit_mask needs to be saved in the
     * Apps side too, then we can use this data passed and save it
     */
    stats_info_ptr = (ds_stats_info_type*)cmd_ptr->cmd_payload_ptr;
    stats_info_ptr->bit_mask = bitmask;
    ds_put_cmd( cmd_ptr );
  }
}/* ds3g_update_stats_mask */

/*===========================================================================
FUNCTION DS3G_GET_STATS_INFO

DESCRIPTION
  This function is an Interface to CM to get the required DS Statistics
  information from DS. This function calls the API provided by the
  HDRRLPSTATS module to get the statistics.

DEPENDENCIES
  None

PARAMETERS
  ds_info - Structure that gets filled with statistics for the mask
            set by CM in ds3g_update_stats_mask().

RETURN VALUE
  True if statistics data is succesfully copied to ds_info pointer.
  Else FALSE.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_get_stats_info
(
  sys_ds_information_s_type  *ds_info
)
{
  sys_ds_hdr_link_info_s_type ds_link_info;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Initialize the Arguments before filling the data rates */

  memset(ds_info,0 , sizeof(sys_ds_information_s_type));
  memset(&ds_link_info, 0, sizeof(sys_ds_hdr_link_info_s_type));

  DATA_MSG0(MSG_LEGACY_LOW, "Data ds3g_get_stats_info:Start");

  /* Set the Changed Bitmask - to inform CM about the Statistics we are
     * passing to CM/Client.
     */
  ds_info->changed_bitmask = ds_modem_stats_changed_fields_bit_mask;

#ifdef FEATURE_HDR
  /* Call the HDRLRPSTATS API to get the Ds link Info */
  hdrrlpstats_get_data_rate(&ds_link_info);
#endif /* FEATURE_HDR */

  ds_info->ds_hdr_link_info.hdr_fwd_data_rate = ds_link_info.hdr_fwd_data_rate;
  ds_info->ds_hdr_link_info.hdr_rev_data_rate = ds_link_info.hdr_rev_data_rate;

  return TRUE;
} /* ds3g_get_stats_info */

