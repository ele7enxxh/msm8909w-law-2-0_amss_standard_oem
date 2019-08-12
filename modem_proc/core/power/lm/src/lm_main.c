/*==============================================================================

  FILE:         lm_main.c

  OVERVIEW:     This file provides S1 LM initializations and main functions

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: 
$DateTime: 
==============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "lm_internal.h"
#include "qurt.h"
#include "lm_hwio.h"

 /* Flags to control LM behaviour  */
static volatile uint32 lm_feature_enable      = TRUE;
static volatile uint32 lm_test_code_enable    = FALSE;
   
 /* Global lm enable/disable flag */
 volatile boolean g_lm_nv_enable = TRUE;

 /* Limits Manager context */
 LMContext gLMContext; 
 
/**
 * <!-- lm_cx_prepost_change_cb -->
 * 
 * @brief Called synchronously before and after cx state is changed. In the call
 *        back, updates modem limits vote based on pre/post event data
 *        
 * @param context    : Not used
 * @param event_type : Specifies event type
 * @param data       : pre, post change event data
 * @param data_size  : Specifies data size
 */
 void lm_cx_prepost_change_cb( void             *context,
                               unsigned int      event_type,
                               void             *data,
                               unsigned int      data_size)
{
  
  npa_prepost_change_data *event_data = (npa_prepost_change_data *)data;
  uint32 lm_vote;
    
  LM_MUTEX_ACQUIRE();
  if(event_type == NPA_EVENT_PRE_CHANGE)
  {
    /* Just to check whether cx_state maintained by LM is same with real CX resource 
       or not, these should match in pre change event */
    //CORE_VERIFY(event_data->from_state == gLMContext.cx_state);

    /* if transitioning to higher corner level, LM should act before switching itself */
    if(event_data->to_state > gLMContext.cx_state )
    {		  
    ULOG_RT_PRINTF_3( gLMContext.ulog_handle, 
                      "Received CX change event -  event_type = 0x%x with from_state = %d, to_state = %d ",
                      event_type, event_data->from_state, event_data->to_state );

	  lm_vote = lm_get_limts_vote(event_data->to_state);
      if(gLMContext.lm_vote != lm_vote)
      {
        gLMContext.lm_vote = lm_vote;
        lm_apply_limits_vote(lm_vote);
      }
      gLMContext.cx_state = event_data->to_state;
	  
	  /* Write int data and send interrupt to apps */
	  lm_send_interrupt_data(gLMContext.cx_state);
    }	
  }
  else if(event_type == NPA_EVENT_POST_CHANGE)
  {

	/* if transitioning to lower corner level, LM should act after switching  */
    if(event_data->to_state < gLMContext.cx_state)
    {
	  ULOG_RT_PRINTF_3( gLMContext.ulog_handle, 
                      "Received CX change event -  event_type = 0x%x with from_state = %d, to_state = %d ",
                      event_type, event_data->from_state, event_data->to_state );
					  
	  /* get vote to be passed on to APPS from the cx state */
      lm_vote = lm_get_limts_vote(event_data->to_state);
      if(gLMContext.lm_vote != lm_vote)
      {
        gLMContext.lm_vote = lm_vote;
        lm_apply_limits_vote(lm_vote);
      }
      gLMContext.cx_state = event_data->to_state;
						
	  /* Write int data and send interrupt to apps */
	  lm_send_interrupt_data(gLMContext.cx_state);
	}
  }
  else
  {
    ULOG_RT_PRINTF_1(gLMContext.ulog_handle, "Received unregistered change event, event_type = 0x%x", event_type);
  }
		
  LM_MUTEX_RELEASE();
}
 
 /**
 * <!-- lm_cx_resource_available_cb -->
 * 
 * @brief Queries CX resource current state and registers for pre and post
 * change notification.
 *          
 * @param context : Not used
 * @param event_type: Not used
 * @param data : Not used
 * @param data_size: Not used
 */
 void lm_cx_resource_available_cb( void         *context,
                                  unsigned int  event_type,
                                  void *data,
                                  unsigned int data_size)
{
  npa_query_type query_result;
  uint32 lm_vote = 0;
	
  /*Error if query handle is already created */
  CORE_VERIFY(NULL == gLMContext.cx_query_handle);
	
  CORE_VERIFY_PTR(
    gLMContext.cx_query_handle = npa_create_query_handle( VDD_CX_NODE ) );
	 
  CORE_VERIFY(NPA_QUERY_SUCCESS == npa_query(gLMContext.cx_query_handle,
                                             NPA_QUERY_CURRENT_STATE,
                                             &query_result));
	   
  gLMContext.cx_state = query_result.data.state;
  
  /* Apply the limits vote during LM initialization based
     on current CX state */
  lm_vote = lm_get_limts_vote(gLMContext.cx_state);
  lm_apply_limits_vote(lm_vote);
  gLMContext.lm_vote = lm_vote;
  ULOG_RT_PRINTF_2( gLMContext.ulog_handle,
                    "Applied lm_vote = %d, cx_state= %d during LM initialisation ",
                    lm_vote, gLMContext.cx_state );
  
  /* Write int data and send interrupt to apps */
  lm_send_interrupt_data(gLMContext.cx_state);
	
  /* create pre and pose change event */
  gLMContext.cx_event_handle = npa_create_custom_event( VDD_CX_NODE,     
                                                       "CX pre/post change event",
                                                       NPA_TRIGGER_PRE_CHANGE_EVENT | NPA_TRIGGER_POST_CHANGE_EVENT,
                                                       NULL,
                                                       lm_cx_prepost_change_cb,
                                                       NULL ) ; 
  CORE_VERIFY_PTR(gLMContext.cx_event_handle);
  ULOG_RT_PRINTF_0( gLMContext.ulog_handle, "Registered with CX pre, post change events " );
}
  
 /**
 * <!-- lm_sleep_lpr_available_cb -->
 * 
 * @brief Registers with Sleep LPR and enabled lm low power mode
 *           
 *  @return None
 */
 void lm_sleep_lpr_available_cb( void          *context,
                                      unsigned int  event_type,
                                      void *data,
                                      unsigned int data_size)
{
  /* Error if query handle is already created */
  CORE_VERIFY(NULL == gLMContext.sleep_lpr_client_handle);

  /* create a client handle to sleep lpr node */
  CORE_VERIFY_PTR( 
   gLMContext.sleep_lpr_client_handle = npa_create_sync_client( SLEEP_LPR_NODE_NAME,
                                                               "LM",
                                                               NPA_CLIENT_REQUIRED ) );
  sleep_define_lpr_str( "LM", gLMContext.sleep_lpr_client_handle );

  /* enable lm lprms */  
  npa_issue_required_request( gLMContext.sleep_lpr_client_handle, SLEEP_ALL_LPRMS );
  ULOG_RT_PRINTF_0( gLMContext.ulog_handle, "Registered with Sleep LPR " );
}
 
 
 /** 
 *   <!--  lm_enter_sleep -->
 *  
 *   @brief Entering power collapse mode. Write value to APSS
 *   indicating mpss going to Power collaspe.
 *
 *   @param wakeupTick Not Used
 *
 *   @return None
 */
void lm_enter_sleep(uint64 wakeupTick)
{
  ULOG_RT_PRINTF_0( gLMContext.ulog_handle, "Enter LM sleep LPR " );   
  //remove limits vote if at all
  if(gLMContext.lm_vote)
  {
    lm_apply_limits_vote(0);
  }
  lm_send_interrupt_data(PMIC_NPA_MODE_ID_CORE_RAIL_OFF);
}
  
 
/** 
 *   <!--  lm_exit_sleep -->
 *  
 *   @brief Applies limits vote whatever existed before going to sleep mode.
 *   indicating mpss coming out of sleep - need Cx
 *   @param None
 *
 *   @return None
 */
void lm_exit_sleep(void)
{
    ULOG_RT_PRINTF_0( gLMContext.ulog_handle, "Exit LM sleep LPR " );
    //Apply limits vote if at all removed while going to sleep
    if(gLMContext.lm_vote)
    {
      lm_apply_limits_vote(gLMContext.lm_vote);
    }
	lm_send_interrupt_data(gLMContext.cx_state);
}

 
/**
 *   <!-- lm_initialize -->
 * 
 * @brief Initializes limits management. As a part of this, it will wait for cx and sleep_lpr
 * resource availability. This function will be invoked by RC_Init
 *  
 * @return None
 */
void lm_initialize( void )
{
  memset((void *)&gLMContext, 0, sizeof(gLMContext));
  if(NULL == gLMContext.ulog_handle)
  {
    // Create Ulog handle
    CORE_DAL_VERIFY(ULogFront_RealTimeInit(&gLMContext.ulog_handle,
                                            LM_DEFAULT_LOG_NAME,
                                            LM_ULOG_DEFAULT_BUFFER_SIZE_BYTES,
                                            ULOG_MEMORY_LOCAL,
                                            ULOG_LOCK_OS ) );
  }
  
  // Header allows the visualization tool to apply the appropriate parser.
  ULogCore_HeaderSet( gLMContext.ulog_handle,
                      "Content-Type: text/tagged-log-1.0; title=LM" );
  ULOG_RT_PRINTF_0( gLMContext.ulog_handle, "S1 LM Initialization " );
  
  lm_efs_initialize();
  if( g_lm_nv_enable == FALSE || lm_feature_enable == FALSE)
  {
    ULOG_RT_PRINTF_0( gLMContext.ulog_handle, "S1 LM feature disabled as lm_feature_enable flag/NV item is disabled" );
	return;
  }
  
  ULOG_RT_PRINTF_3( gLMContext.ulog_handle,
                    "Clk div vote addr: 0x%x, Apps int data addr : 0x%x, Apps int addr : 0x%x ",
                    HWIO_GCC_S1LM_MODEM_DIV_VOTE_ADDR,
                    HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_ADDR,
                    HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_ADDR);
   
  gLMContext.cx_state = 0;

  CORE_VERIFY_PTR( gLMContext.lm_mutex_handle = Core_MutexCreate( CORE_MUTEXATTR_DEFAULT ) );
  
  /* Register and wait for the external resources to be available */
  npa_resource_available_cb( VDD_CX_NODE, lm_cx_resource_available_cb, NULL );

  /* Register and wait for the external resources to be available */
  npa_resource_available_cb( SLEEP_LPR_NODE_NAME, lm_sleep_lpr_available_cb, NULL );
   
  
  
  if( TRUE == lm_test_code_enable )
  {
    npa_client_handle gDummy;
	gDummy = npa_create_sync_client( "/pmic/client/rail_cx",
                                   "Dummyclient",
                                   NPA_CLIENT_REQUIRED);

    npa_issue_required_request(gDummy, 3);
    npa_issue_required_request(gDummy, 6);
    npa_issue_required_request(gDummy, 1);
  }
}


