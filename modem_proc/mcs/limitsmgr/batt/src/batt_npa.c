
/*!
  @file
  batt_npa.c

  @brief
  This file sends notifications to the thermal service via NPA

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/batt/src/batt_npa.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/02/13   ag     Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "npa.h"
#include "cxm.h"
#include "lmtsmgr_task.h"


/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
#define MAX_BATT_WARNING_LEVELS 2

const char* npaResource = "/therm/sensor/modem_bcl_warning";

typedef struct
{
  uint32 tech_mask;
  uint32 warning_level;
} batt_warning_table_type;

batt_warning_table_type batt_table[MAX_BATT_WARNING_LEVELS] = 
{
  { ((1<<CXM_TECH_GSM2)|(1<<CXM_TECH_GSM1)),
    10
  },
  { ((1<<CXM_TECH_GSM2)|(1<<CXM_TECH_TDSCDMA)),
    15
  }
};

typedef struct
{
  batt_warning_table_type* table;
  uint32 current_level;
  npa_client_handle npaClient;
} batt_warning_type;

static batt_warning_type batt_warning = 
{
  /* warning table */
  batt_table,

  /* current level */
  0,      

  /* NPA Client handle */
  NULL    
};

/*=============================================================================

                                FUNCTIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  batt_npa_send_notification

=============================================================================*/
/*!
    @brief
    Checks if warning level has changed and sends a notifiction via NPA to the
    thermal service
 
    @return
    None
*/
/*===========================================================================*/
void batt_npa_send_notification
(
  cxm_tech_type tech1,
  cxm_tech_type tech2
)
{
  int i;
  uint32 level = 0;
  uint32 check_mask = (1<<tech1)|(1<<tech2);

  for (i=0; i<MAX_BATT_WARNING_LEVELS; i++)
  {
    if (batt_warning.table[i].tech_mask == check_mask)
    {
      /* Found it */
      level = batt_warning.table[i].warning_level;
      break;
    }
  }

  /* check if the level has changed */
  if (batt_warning.current_level != level)
  {
    if (batt_warning.npaClient == NULL)
    {
      /* NPA Client has not been created yet... */
      LMTSMGR_MSG_2(ERROR, "Warning changed from %d -> %d, NPA client not ready",
                    batt_warning.current_level, level);
    }
    else
    {
      /* Send the level via NPA */
      npa_issue_required_request(batt_warning.npaClient, level);

      LMTSMGR_MSG_2(HIGH, "Warning changed from %d -> %d, sent via NPA",
                    batt_warning.current_level, level);
    }

    /* copy to current level */
    batt_warning.current_level = level;
  }
}

/*=============================================================================

  FUNCTION:  batt_resource_cb

=============================================================================*/
/*!
    @brief
    Callback that gets called when the NPA resource becomes available
 
    @return
    None
*/
/*===========================================================================*/
static void batt_resource_cb( void *context,
                              unsigned int  event_type, 
                              void *data,       
                              unsigned int  data_size )
{
  // Create a synchronous request client to the resource.
  batt_warning.npaClient = npa_create_sync_client(npaResource, 
                                                  "Modem_MCS",           
                                                   NPA_CLIENT_REQUIRED );
  if (batt_warning.npaClient == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Could not create NPA client for modem_bcl resource");
  }
  else
  {
    LMTSMGR_MSG_1(HIGH, "NPA client handle 0x%x created for modem_bcl resource", 
                   batt_warning.npaClient);
  }

  /* If the current warning level is anything other than 0, then send it immediately 
     This can happen if the callback gets called in the middle of a call when the 
     warning level is already high... */
  if (batt_warning.current_level != 0)
  {
    npa_issue_required_request(batt_warning.npaClient, batt_warning.current_level);

    LMTSMGR_MSG_1(HIGH, "Sent warning level %d via NPA", batt_warning.current_level);
  }
}

/*=============================================================================

  FUNCTION:  batt_npa_init

=============================================================================*/
/*!
    @brief
    Registers with NPA for the resource available callback
 
    @return
    None
*/
/*===========================================================================*/
void batt_npa_init(void)
{
  // Wait for the /therm/sensor/modem_bcl_warning resource to be available.
  npa_resource_available_cb( npaResource,
                             batt_resource_cb,
                             NULL);
							 
}


