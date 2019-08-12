/**
  @file mhi_hwacc.c
  @brief
  This file contains the APIs required for the interaction between the MHI engine
  running on the APSS processor and the MHI engine running on its 
  hardware accelerated counterpart (ex: IPA).

  In the current implementation all calls to IPA are synchronous.

*/
/*
===============================================================================
   
                             Edit History
   
 $Header: 
  
when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/15/13   am      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/


#include "mhi_hwacc.h"
#include "mhi_pm.h"
#include "mhi_link.h"
#include "mhi_mmio.h"
#include "mhi_core.h"

#define MSIADDR  0x00080188

/* Identifiers that pair two ch_ids into a bi-directional pipe */
#define IP_DATA_SERVICES_4  4
#define IP_DATA_SERVICES_5  5
#define IP_DATA_SERVICES_6  6
#define IP_DATA_SERVICES_7  7


#ifdef MHIIPA
/* IPA Configuration */
static ipa_per_mhi_config ipa_config;
static ipa_per_mhi_ch_info_type ch_id_array_ptr[] = { {HWCHANNELBASE+0,IPA_PER_MHI_CH_DIR_OUTBOUND,IP_DATA_SERVICES_4}, 
                                                      {HWCHANNELBASE+1,IPA_PER_MHI_CH_DIR_INBOUND ,IP_DATA_SERVICES_4}, 
                                                      {HWCHANNELBASE+2,IPA_PER_MHI_CH_DIR_OUTBOUND,IP_DATA_SERVICES_5}, 
                                                      {HWCHANNELBASE+3,IPA_PER_MHI_CH_DIR_INBOUND ,IP_DATA_SERVICES_5}, 
                                                      {HWCHANNELBASE+4,IPA_PER_MHI_CH_DIR_OUTBOUND,IP_DATA_SERVICES_6}, 
                                                      {HWCHANNELBASE+5,IPA_PER_MHI_CH_DIR_INBOUND ,IP_DATA_SERVICES_6}, 
                                                      {HWCHANNELBASE+6,IPA_PER_MHI_CH_DIR_INBOUND ,IP_DATA_SERVICES_7}, 
                                                      {HWCHANNELBASE+7,IPA_PER_MHI_CH_DIR_OUTBOUND,IP_DATA_SERVICES_7} 
                                                    };

/**
 * @brief        mhi_hw_acc_cb
 *
 *               Callback invoked by hardware accelerator when certain events are
 *               detected and need the awareness of the MHI driver. 
 *
 * @param[in]    ind_type - Type of indication                
 * @param[in]    *info    - Pointer to a data structure associated with the indication
 * @param[in]    cb_data  - Callback data passed at the time of callback registration
 *
 * @dependencies
 *               ipa_per_reg_cb must be called to register this cb. 
 * 
 * @return       
 *               None   
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_cb(uint32 ind_type, void* info, void* cb_data)
{
   mhi_ring_element_t el;

   mhi_osal_debug(MSG_LOW,"mhi_hw_acc_cb: Enter");

   switch (ind_type)
   {
      case IPA_PER_MHI_IND_INVALID:
         /* Should never happen */
         mhi_osal_debug(MSG_FATAL,"mhi_hw_acc_cb: IPA_PER_MHI_IND_INVALID");
         break;

         /* Sent by IPA driver after MHI engine indicates that it was loaded properly and ready to 
          * be configured for processing of HW-accelerated MHI channels. Sent with ipa_per_mhi_ind_mhi_engine_init_completed 
          * information structure.
          */
      case IPA_PER_MHI_IND_ENGINE_INIT_COMPLETED:
         mhi_osal_debug(MSG_MEDIUM,"mhi_hw_acc_cb: IPA_PER_MHI_IND_ENGINE_INIT_COMPLETED");
         break;

         /* Sent by IPA driver when a data activity is detected while HW-accelerated
          * data path is suspended. Sent with ipa_per_mhi_ind_mhi_data_activity 
          * information structure.
          */
      case IPA_PER_MHI_IND_MHI_CH_DATA_ACTIVITY:
         mhi_osal_debug(MSG_MEDIUM,"mhi_hw_acc_cb: IPA_PER_MHI_IND_MHI_CH_DATA_ACTIVITY"); 
         mhi_pm_hw_acc_wakeup();
         break;

         /* Error in a HW-accelerated MHI channel: sent with ipa_per_mhi_ind_mhi_ch_error information structure. */
      case IPA_PER_MHI_IND_MHI_CH_ERROR:
         mhi_osal_debug(MSG_MEDIUM,"mhi_hw_acc_cb: IPA_PER_MHI_IND_MHI_CH_ERROR");

         el.tEvent.ptr=0; /* Should be actual element, need to get from IPA */ 
         el.tEvent.chid=((ipa_per_mhi_ind_mhi_ch_error*)info)->cci; 
         el.tEvent.code=MHI_CMD_CODE_RING_EL_ERROR;
         el.tEvent.type=MHI_RING_EL_TYPE_TRANSFER_EVENT;

         /* Send the event */
         if( SUCCESS != mhi_core_send_event(PRIMARY_ER,&el, 1))
         {
            mhi_osal_debug(MSG_FATAL, "mhi_hw_acc_cb: Could not generate event on primary event ring"); 

         }

         break;

         /* General error in the processing of HW-accelerated MHI channels: sent with 
          * ipa_per_mhi_ind_mhi_error information structure.
          */
      case IPA_PER_MHI_IND_MHI_ERROR:
         mhi_osal_debug(MSG_FATAL,"mhi_hw_acc_cb: IPA_PER_MHI_IND_MHI_ERROR");
         break;

      default:
         /* Should never happen */
         mhi_osal_debug(MSG_FATAL,"mhi_hw_acc_cb");
         break;
   }
}
#endif

/**
 * @brief        mhi_hw_acc_init
 *
 *               Initialize the MHI hardware accelerator (hw acc).
 *
 *@param[in]     *hw_acc_info - pointer to the hw acc context.                
 *
 * @dependencies
 *               None 
 * 
 * @return       
 *               SUCCESS if successful.   
 *               ERROR  if error occurred.   
 * 
 * @sideeffect   None 
 */
uint32 mhi_hw_acc_init(mhi_hw_acc_info_t *hw_acc_info)
{
   mhi_osal_debug(MSG_LOW,"mhi_hw_acc_init: Enter"); 

#ifdef MHIIPA  
   /* Make sure IPA hardware is fully up */
   mhi_hw_acc_disable_pm();

   /* Initialize IPA */
   ipa_config.msi_addr=MSIADDR; 
   ipa_config.mmio_addr=MMIO_BASEPHYS+0x100; 
   ipa_config.ch_id_array_ptr=ch_id_array_ptr;
   ipa_config.num_channels=HWCHANNELNUM;

   /* Enable ipa db routing  */
   mhi_link_setup_hwacc_db_routing(IPA_UC_MAILBOX_CDB_TARGET,IPA_UC_MAILBOX_EDB_TARGET);

   if (ipa_per_mhi_init(&ipa_config))
   {
      return ERROR;
   }

   ipa_per_reg_cb(IPA_PER_TYPE_MHI,&mhi_hw_acc_cb,NULL);

#endif

   return SUCCESS;
}  

/**
 * @brief        mhi_hw_acc_reset
 *
 *               Reset the hw acc.
 *
 * @dependencies
 *               None 
 * 
 * @return       
 *               None
 *
 * @sideeffect   None 
 */
void mhi_hw_acc_reset(void)
{
   int32 ret;

   mhi_osal_debug(MSG_HIGH,"mhi_hw_acc_reset: Enter"); 

#ifdef MHIIPA  
   /* This API may expect an error type as input in future */
   ret = ipa_per_mhi_reset(NULL);

   if (SUCCESS != ret)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_hw_acc_reset: ipa_per_mhi_reset failed with error %d",ret);
   }

   /* enable ipa db routing  */
   mhi_link_setup_hwacc_db_routing(IPA_UC_MAILBOX_CDB_TARGET,IPA_UC_MAILBOX_EDB_TARGET); 

   /* Make sure IPA hardware is fully up */
   mhi_hw_acc_disable_pm();
#else
   NOT_USED(ret);
#endif
}

/**
 * @brief        mhi_hw_acc_start
 *
 *               Start the hw acc.
 *
 *
 * @dependencies
 *               None 
 * 
 * @return       
 *               None 
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_start(void)
{
   int32 ret;

   mhi_osal_debug(MSG_HIGH,"mhi_hw_acc_start: Enter");

#ifdef MHIIPA 
   /* Notify IPA MHI engine to start*/
   ret = ipa_per_mhi_start(*mhiCtx.mhi_hw_acc_info.p_mhictrl_pa, *mhiCtx.mhi_hw_acc_info.p_mhidata_pa); 
   if(SUCCESS != ret)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_hw_acc_start: ipa_per_mhi_start failed with error %d",ret);
   }
#else
   NOT_USED(ret);
#endif 
}

/**
 * @brief        mhi_hw_acc_enable_pm
 *
 *               Enable hw acc power management.
 *
 *
 * @dependencies
 *               None  
 * 
 * @return       
 *               None
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_enable_pm(void)
{
   int32 ret;

   mhi_osal_debug(MSG_LOW,"mhi_hw_acc_enable_pm: Enter");

#ifdef MHIIPA
   ret = ipa_per_pm_release(IPA_PER_TYPE_MHI,IPA_PER_PM_RES_CLK_GATING_BM,NULL);
   if(SUCCESS != ret)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_hw_acc_enable_pm: ipa_per_pm_release failed with error %d",ret);
   }
#else
   NOT_USED(ret);
#endif
}

/**
 * @brief        mhi_hw_acc_disable_pm
 *
 *               Disable hw acc power management.
 *
 *
 * @dependencies
 *               mhi_hw_acc_start must be called first.
 * 
 * @return       
 *               None
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_disable_pm(void)
{
   int32 ret;

   mhi_osal_debug(MSG_LOW,"mhi_hw_acc_disable_pm: Enter");

#ifdef MHIIPA
   ret = ipa_per_pm_request(IPA_PER_TYPE_MHI,NULL);
   if(SUCCESS != ret)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_hw_acc_disable_pm: ipa_per_pm_request failed with error %d",ret);
   }
#else
   NOT_USED(ret);
#endif
}

/**
 * @brief        mhi_hw_acc_suspend
 *
 *               Suspend the MHI channels dedicated to the hw acc.
 *
 *
 * @dependencies
 *               mhi_hw_acc_start must be called first. 
 * 
 * @return       
 *               None
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_suspend(void)
{
   int32 ret;

   mhi_osal_debug(MSG_LOW,"mhi_hw_acc_suspend: Enter");

#ifdef MHIIPA
   ret = ipa_per_state_change(IPA_PER_TYPE_MHI,IPA_PER_STATE_CMD_SUSPEND, NULL);
   if(SUCCESS != ret)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_hw_acc_suspend: ipa_per_state_change failed with error %d",ret);
   }
#else
   NOT_USED(ret);
#endif
}

/**
 * @brief        mhi_hw_acc_resume
 *
 *               Resume the MHI channels dedicated to the hw acc.
 *
 *
 * @dependencies
 *               mhi_hw_acc_start must be called first.
 *               Ideally should be called only if mhi_hw_acc_suspend was called prior to this. 
 * 
 * @return       
 *               None
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_resume(void)
{
   int32 ret;

   mhi_osal_debug(MSG_LOW,"mhi_hw_acc_resume: Enter");

#ifdef MHIIPA
   ret = ipa_per_state_change(IPA_PER_TYPE_MHI,IPA_PER_STATE_CMD_RESUME, NULL);
   if(SUCCESS != ret)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_hw_acc_resume: ipa_per_state_change failed with error %d",ret);
   }
#else
   NOT_USED(ret);
#endif
}

/** 
 * @brief      mhi_hw_acc_ch_cmd
 *
 *             Relay a channel command to the hardware accelerator 
 *              
 * 
 * @param[in]  chid - channel id 
 * @param[in]  cmd  - command
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS/ERROR 
 * 
 * @sideeffect None 
 */
uint32 mhi_hw_acc_ch_cmd(uint32 chid , ch_cmd_t cmd)
{
#ifdef MHIIPA
   int32 ret = ERROR; 
   switch(cmd)
   {
      case CH_CMD_RESET: 
         ret = ipa_per_mhi_ch_state_change(chid, IPA_PER_MHI_CH_STATE_CMD_RESET, 0);
         break;
      case CH_CMD_STOP: 
         ret = ipa_per_mhi_ch_state_change(chid, IPA_PER_MHI_CH_STATE_CMD_STOP, 0);
         break;
      case CH_CMD_START: 
         ret = ipa_per_mhi_ch_state_change(chid, IPA_PER_MHI_CH_STATE_CMD_START, 0);
         break;

   }

   if (SUCCESS != ret) 
      return ERROR; 
#endif

   return SUCCESS; 
}



