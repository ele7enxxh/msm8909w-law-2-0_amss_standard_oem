
/*!
  @file
  ddr_coex_npa.c

  @brief
  This file sends notifications to the RPM via NPA to change the DDR frequency
  plan

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/ddr_coex/src/ddr_coex_npa.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/21/13   jm      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "npa.h"
#include "lmtsmgr_task.h"


/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
#define MAX_FREQ_PLANS 0xFF
#define LMTSMGR_ARG_NOT_USED(arg) if(arg){}

const char* ddrNpaResource = "/clk/bimc_freq_plan";

typedef struct
{
  uint32 freq_plan;
  npa_client_handle handle;
} ddr_coex_npa_type;

static ddr_coex_npa_type ddr_coex_npa = 
{
  /* freqPlan */
  MAX_FREQ_PLANS,      

  /* NPA Client handle */
  NULL    
};

/*=============================================================================

                                FUNCTIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  ddr_coex_npa_update_freq_plan

=============================================================================*/
/*!
    @brief
    Checks if freq plan has changed and sends a notifiction via NPA to RPM
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_npa_update_freq_plan
(
  uint32 freq_plan
)
{

#ifdef FEATURE_MODEM_DDR_MGMT
  /* check if the level has changed */
  if (ddr_coex_npa.freq_plan != freq_plan)
  {
    if (ddr_coex_npa.handle == NULL)
    {
      /* NPA Client has not been created yet... */
      LMTSMGR_MSG_2(ERROR, "Freq Plan changed from %d -> %d but NPA client not ready",
                    ddr_coex_npa.freq_plan, freq_plan);
    }
    else
    {
      /* Send the level via NPA */
      npa_issue_suppressible_request(ddr_coex_npa.handle, freq_plan);

      LMTSMGR_MSG_2(HIGH, "Freq Plan changed from %d -> %d, sent to RPM via NPA",
                    ddr_coex_npa.freq_plan, freq_plan);
    }

    /* copy to current level */
    ddr_coex_npa.freq_plan = freq_plan;
  }
#else
LMTSMGR_ARG_NOT_USED(freq_plan);
LMTSMGR_ARG_NOT_USED(ddr_coex_npa.freq_plan);
#endif
}

#ifdef FEATURE_MODEM_DDR_MGMT
/*=============================================================================

  FUNCTION:  ddr_coex_npa_resource_cb

=============================================================================*/
/*!
    @brief
    Callback that gets called when the NPA resource becomes available
 
    @return
    None
*/
/*===========================================================================*/
static void ddr_coex_npa_resource_cb( void *context,
                                 unsigned int  event_type, 
                                 void *data,       
                                 unsigned int  data_size )
{
  // Create a synchronous request client to the resource.
  ddr_coex_npa.handle = npa_create_async_client_cb(ddrNpaResource, 
                                              "Modem_MCS",           
                                              NPA_CLIENT_REQUIRED,
                                              NULL,
                                              NULL ); /* TODO: register a callback */
  if (ddr_coex_npa.handle == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Could not create NPA client for DDR resource");
  }
  else
  {
    LMTSMGR_MSG_1(HIGH, "NPA client handle 0x%x created for modem_bcl resource", 
                   ddr_coex_npa.handle);

    /* If the current freq plan level is anything other than MAX, then send it immediately 
     This can happen if the callback gets called in the middle of tech operation... */
    if (ddr_coex_npa.freq_plan != MAX_FREQ_PLANS)
    {
      npa_issue_suppressible_request(ddr_coex_npa.handle, ddr_coex_npa.freq_plan);

      LMTSMGR_MSG_1(HIGH, "Sent freq plan %d to RPM via NPA", ddr_coex_npa.freq_plan);
    }
  }
}
#endif

/*=============================================================================

  FUNCTION:  ddr_coex_npa_init

=============================================================================*/
/*!
    @brief
    Registers with NPA for the resource available callback
 
    @return
    None
*/
/*===========================================================================*/
void ddr_coex_npa_init(void)
{
#ifdef FEATURE_MODEM_DDR_MGMT
  // Wait for the /therm/sensor/modem_bcl_warning resource to be available.
  npa_resource_available_cb( ddrNpaResource,
                             ddr_coex_npa_resource_cb,
                             NULL);
#endif                             
}


