/*============================================================================
@file npa_scheduler_bimc_vote.c

NPA target configuration data

Copyright (c) 2010-2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.mpss/3.5.c12.3/power/npa/hw/8937/npa_target.c#1 $
============================================================================*/

#include "npa_scheduler.h"
#include "npa_scheduler_internal.h"

 /* Request 100 Mhz BIMC clock so that NPA scheduler can process immdeiate requests fast */
void npa_scheduler_bimc_suppressible_request(npa_scheduler_action *action)
{

}
   
  /* Remove BIMC clock vote */ 
 void npa_scheduler_bimc_complete_request(npa_scheduler_action *action)
 {  

 } 
 
 #if 0
   /**
 * <!-- npa_init_scheduler_clk_bimc_callback -->
 *
 * @brief Creates query handle to "clk/bimc"
 * 
 * In npa_init_scheduler, this function is set as the resource available
 * callback for when "clk/bimc" becomes available.
 */
static void npa_init_scheduler_clk_bimc_callback( void *context,
                                                 unsigned int event_type,
                                                 void *data,
                                                 unsigned int data_size)
{
     
}
#endif

 /* need to wait until /clk/bimc is available before
   * the scheduler can create the client.
   */
void npa_scheduler_bimc_available_cb_fn(void) 
{
 
}
