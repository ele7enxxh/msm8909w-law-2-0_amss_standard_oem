/*=============================================================================

FILE:         qdss_ctrl.c

DESCRIPTION:  Implements handlers for diag comamnds to configure qdss on
              a particular core
================================================================================
              Copyright (c) 2012 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "qdss_ctrl_diag.h"
#include "qdss_ctrl_etm.h"
#include "qdss_ctrl_utils.h"
#include "qdss_ctrl_priv.h"
#include "ClockDefs.h"
#include "qdss_control.h"
#include "qurt/qurt_trace.h"

#define ETM_DEFAULT_MODE_PC_AND_MEMORY  QURT_ETM_TYPE_PC_AND_MEMORY_ADDR

struct qdss_ctrl_s qdss_ctrl_ctxt;

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_CTRL_FILTER_ETM diag command

  @return 0 if successful, error code otherwise
 */

int qdss_ctrl_filter_etm_handler(qdss_ctrl_filter_etm_req *pReq,
                          int req_len,
                          qdss_ctrl_filter_etm_rsp *pRsp,
                          int rsp_len)
{
   int nErr = DAL_ERROR;

   nErr = qdss_control_set_etm(pReq->state);
   pRsp->result = nErr;
   return nErr;
}


int qdss_control_get_etm(uint8 *state)
{
   *state = qdss_ctrl_ctxt.etm_state;
   return 0;
}


int qdss_control_set_etm(uint8 state)
{
   int nErr = DAL_ERROR;
   unsigned char etm_mode = 0;

   if ((0==qdss_ctrl_ctxt.hSync) || (0==qdss_ctrl_ctxt.npa_client)) {
      nErr= DAL_SYNC_ENTER_FAILED;
      return nErr;
   }

   DALSYS_SyncEnter(qdss_ctrl_ctxt.hSync);

   if (state) {
      if (!qdss_ctrl_ctxt.clk_enabled) {
         npa_issue_required_request(qdss_ctrl_ctxt.npa_client, CLOCK_QDSS_LEVEL_DEBUG);
         qdss_ctrl_ctxt.clk_enabled = TRUE;

      }
	/*State: D0 bit serves the enable or disable ETM, D1 - D7 bits serves ETM configuration mode */
		if (state == 1) {	//request from QMI/Debug Agent or Diag with default mode
			etm_mode =  qdss_ctrl_ctxt.etm_default_state;
		}
		else { /*request from Diag */
			etm_mode = (state >> 1); //State right shifted 1 time to get the mode value. 
		}
			
		nErr = qdss_ctrl_etm_enable(etm_mode);
   }
   else {
      if (qdss_ctrl_ctxt.clk_enabled) {
         nErr = qdss_ctrl_etm_disable();
         qdss_ctrl_ctxt.clk_enabled = FALSE;
         npa_issue_required_request(qdss_ctrl_ctxt.npa_client, CLOCK_QDSS_LEVEL_OFF);
      }
      else {
         nErr = DAL_SUCCESS;
      }
   }

   if (DAL_SUCCESS == nErr) {
      qdss_ctrl_ctxt.etm_state = state;
   }

   DALSYS_SyncLeave(qdss_ctrl_ctxt.hSync);
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_ETM_CONFIG_TESTBUS diag command

  @return 0 if successful, error code otherwise
 */

int qdss_ctrl_etm_testbus_handler(qdss_ctrl_etm_testbus_req *pReq,
                                     int req_len,
                                     qdss_ctrl_etm_testbus_rsp *pRsp,
                                     int rsp_len)
{	
   int nErr = DAL_ERROR;
      
   nErr = qurt_etm_testbus_set_config(pReq->testbus_mode);

   pRsp->etm_cmd_id = pReq->etm_cmd_id;
   pRsp->result = nErr;
   return nErr;
}
/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_ETM_CONFIG_DEFAULT_MODE diag command

  @return 0 if successful, error code otherwise
 */
									 
int qdss_ctrl_etm_default_handler(qdss_ctrl_etm_default_req *pReq,
                                     int req_len,
                                     qdss_ctrl_etm_default_rsp *pRsp,
                                     int rsp_len)
{	
	qdss_ctrl_ctxt.etm_default_state = pReq->default_etm_mode;
	
	pRsp->etm_cmd_id = pReq->etm_cmd_id;
	pRsp->result = QDSS_CTRL_SUCCESS;
	
	return QDSS_CTRL_SUCCESS;

}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_CTRL_FILTER_ETM_RPM diag command

  @return 0 if successful, error code otherwise
 */

int qdss_ctrl_filter_etm_rpm_handler(qdss_ctrl_filter_etm_rpm_req *pReq,
                          int req_len,
                          qdss_ctrl_filter_etm_rpm_rsp *pRsp,
                          int rsp_len)
{
	int nErr = DAL_ERROR;
	
	nErr = qdss_control_set_etm_rpm(pReq->state);
	pRsp->result = nErr;
	return nErr;
}


int qdss_control_get_etm_rpm(uint8 *state)
{
   *state = qdss_ctrl_ctxt.etm_state_rpm;
   return 0;
}

int qdss_control_set_etm_rpm(uint8 state)
{
	int nErr = DAL_ERROR;
	
   if ((0==qdss_ctrl_ctxt.hSync) || (0==qdss_ctrl_ctxt.npa_client)) {
      nErr= DAL_SYNC_ENTER_FAILED;
      return nErr;
	}

   DALSYS_SyncEnter(qdss_ctrl_ctxt.hSync);
	
	nErr = qdss_ctrl_etm_rpm_set(state);
   if (DAL_SUCCESS == nErr) {
      qdss_ctrl_ctxt.etm_state_rpm = state;
   }
	
   DALSYS_SyncLeave(qdss_ctrl_ctxt.hSync);
	return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief Initializes the QDSS control subsystem.

  This is called from RC init.

  @return None
 */
void qdss_ctrl_init(void)
{
   int nErr;
   qdss_ctrl_diag_init();
   qdss_ctrl_ctxt.hSync = 0;
   qdss_ctrl_ctxt.clk_enabled = FALSE;
   qdss_ctrl_ctxt.etm_state = 0;
   qdss_ctrl_ctxt.hTracer = 0;
   qdss_ctrl_ctxt.test_data_count = 0;
   qdss_ctrl_ctxt.etm_default_state = ETM_DEFAULT_MODE_PC_AND_MEMORY;

   TRY(nErr,DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                              &qdss_ctrl_ctxt.hSync, NULL));
   qdss_ctrl_ctxt.npa_client =
      npa_create_sync_client("/clk/qdss", "qdss", NPA_CLIENT_REQUIRED);

   CATCH(nErr) {
   }
   IGNORE(nErr);
}

