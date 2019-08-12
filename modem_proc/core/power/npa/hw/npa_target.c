/*============================================================================
@file npa_scheduler_bimc_vote.c

NPA target configuration data

Copyright (c) 2010-2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.mpss/3.5.c12.3/power/npa/hw/npa_target.c#1 $
============================================================================*/

#include "npa_scheduler.h"
#include "npa_scheduler_internal.h"

 /* Request 100 Mhz BIMC clock so that NPA scheduler can process immdeiate requests fast */
void npa_scheduler_bimc_suppressible_request(npa_scheduler_action *action )
{
   if ((NPA_SCHEDULED_ACTION_TIMEOUT == action->type)  &&  npa_scheduler.clk_bimc_client )
   {
      npa_issue_suppressible_request( npa_scheduler.clk_bimc_client, 100000 );
   }
}
   
  /* Remove BIMC clock vote */ 
 void npa_scheduler_bimc_complete_request(npa_scheduler_action *action)
 {  
   if ( (NPA_SCHEDULED_ACTION_TIMEOUT == action->type)  && (npa_scheduler.clk_bimc_client) )
   { 
     npa_complete_request(npa_scheduler.clk_bimc_client);
   }
 } 
    
   /**
 * <!-- npa_init_scheduler_clk_bimc_callback -->
 *
 * @brief Creates query handle to "clk/bimc"
 * 
 * In npa_init_scheduler, this function is set as the resource available
 * callback for when "clk/bimc" becomes available.
 * All parameters are ignored but defined to meet calling convention.
 *
 * @param context : Not used
 * @param event_type: Not used
 * @param data : Not used
 * @param data_size: Not used
 */
static void npa_init_scheduler_clk_bimc_callback( void *context,
                                                 unsigned int event_type,
                                                 void *data,
                                                 unsigned int data_size)
{
  /* Error if client is already created */
  CORE_VERIFY(NULL == npa_scheduler. clk_bimc_client);
  /* create the clk client */
  npa_scheduler. clk_bimc_client
    = npa_create_sync_client( "/clk/bimc", 
                              "npa_scheduler_clk_bimc_client",
                              NPA_CLIENT_SUPPRESSIBLE );
  CORE_VERIFY_PTR(npa_scheduler.clk_bimc_client);
     
}

 /* need to wait until /clk/bimc is available before
   * the scheduler can create the client.
   */
void npa_scheduler_bimc_available_cb_fn(void) 
{
  npa_resource_available_cb( "/clk/bimc",
			     npa_init_scheduler_clk_bimc_callback,
			     NULL );

}
