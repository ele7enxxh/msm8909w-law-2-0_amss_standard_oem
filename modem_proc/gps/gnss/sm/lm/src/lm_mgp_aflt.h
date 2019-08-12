 /*======================================================================

                         Location Manager API definitions file

 GENERAL DESCRIPTION
  This file contains API definitions by which Location Manager(LM) services can 
  be accessed by other tasks (e.g Transport Manager). 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header $$DateTime $$Author $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 -
 6/20/06   mr   Initial creation of header file.
======================================================================*/
#ifndef LM_MGP_AFLT_H
#define LM_MGP_AFLT_H


#include "gps_variation.h"
#include "lm_api.h"

/****************** Function Declearations**************************************/

/*
 ******************************************************************************
 * lm_mgp_aflt_get_results 
 *
 * Function description:
 *
 * This function is to request for PPM results
 *
 * Parameters: 
 * u_EndPpm -  stop PPm processing afer PPM results are sent
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/
void lm_mgp_aflt_get_results(boolean);


/*
 ******************************************************************************
 * lm_mgp_aflt_start_ppm_processing 
 *
 * Function description:
 *
 * This function turns AFLT engine to ON for a MS-Assisted/Hybrid session.
 *
 * Parameters: 
 * p_CdmaReq - Pointer to structure containing CDMA(PPM) request from TM
 *
 * Return value: 
 *
 * boolean - AFLT processng successfully started or not
 *
 ******************************************************************************
*/
boolean lm_mgp_aflt_start_ppm_processing
(
  const lm_session_request_cdma_s_type* const p_CdmaReq
);

/*
 ******************************************************************************
 * lm_mgp_aflt_stop_ppm_processing 
 *
 * Function description:
 *
 * This function turns AFLT to OFF.
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_aflt_stop_ppm_processing(void);


/*
 ******************************************************************************
 * lm_mgp_aflt_continue_ppm_processing 
 *
 * Function description:
 *
 * This function updates PPM processing params. Currently it only updates the PPM
 * timeout value. The currently running PPM timeout counter is stopped and a new
 * one is started.
 *
 * Parameters: 
 * p_CdmaReq - Request containing params to be modified for PPM 
 *
 * Return value: 
 *
 * boolean - AFLT processng params successfully updated or not
 *
 ******************************************************************************
*/
boolean lm_mgp_aflt_continue_ppm_processing(const lm_session_request_cdma_s_type* const p_CdmaReq);


/*
 ******************************************************************************
 * lm_mgp_aflt_handle_ppm_meas_report 
 *
 * Function description:
 *
 * This function handles the PPM report from CDMA searcher. 
 *
 * Parameters: 
 * *p_msg - IPC message containing ppm measurement report
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_aflt_handle_ppm_meas_report(os_IpcMsgType* p_msg);

#endif /* LM_MGP_AFLT_H */
