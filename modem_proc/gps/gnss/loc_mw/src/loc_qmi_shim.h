#ifndef LOC_QMI_SHIM_H
#define LOC_QMI_SHIM_H
/*============================================================================
  @file loc_qmi_shim.h

  @brief
    This module glues loc-api to the QCSI framework.  It translates the 
    data-structurs to/fro between loc-api and QMI-ULP. 
    
    
  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  opyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc

============================================================================*/
/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_qmi_shim.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
2015-06-11  sjk  LB 2.0 Integration 
2011-03-08  spn  Initial revision.


=============================================================================*/

/*****************************************************************************
 * Include Files
 * *************************************************************************/

#include "customer.h"
#include "comdef.h"

#include "loc_api_2.h"
#include "loc_task.h"
#include "loc_batching.h"
#include "loc_client.h"
#include "loc_utils.h"

/*****************************************************************************
 * Preprocessor Definitions and Constants
 * *************************************************************************/


/*****************************************************************************
 * Type Declarations
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
 @brief 
  Structure with information that the client needs to provide in case an 
  indication is to be generated for a client of QMI-LOC service to receive

@warning
  The memory for p_Msg needs to be allocated by the caller. The caller will 
  need to free the memory. 
*//* ------------------------------------------------------------------------*/
typedef struct
{
  uint32         q_Id; /*< Message ID as allocated by the service definition */
  void*          p_Msg; /*< The message itself */
  uint32         q_MsgLen; /*< Length of the message */
} locQmiShimIndInfoStructT;


/*****************************************************************************
 * Function Declarations and Documentation
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
  This function is called to initialize the glue layer, so it can attach itself
  with QCSI Framework and initialize the QMI-Loc Service.

@retval  TRUE  - Successful initialization of the module
@retval  FALSE - Module initialization was not successful
*//* ------------------------------------------------------------------------*/
boolean locQmiShimInit
( 
  void 
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is called to reset and disconnect the shim layer from the QCSI
  framework and de-register the QMI-Loc Service. After this function call, 
  the loc-middleware task will need to call locQmiShimInit() to be able to 
  communicate over the QMI-framework. 
*//* ------------------------------------------------------------------------*/
void locQmiShimCleanup 
( 
  void 
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is called by the Service owner indicating that there is a 
  QMI-Event that is queued up in QCSI that needs to be handled.
*//* ------------------------------------------------------------------------*/
void locQmiShimHandleEvent 
( 
  void 
);


/* -----------------------------------------------------------------------*//**
@brief
  This function is called by the Service owner in the Service owner task 
  context. This will be triggered from the QMI-Shim when QMI-Shim has to 
  perform and queues up a OS-IPC to the Service Owner. 
*//* ------------------------------------------------------------------------*/
void locQmiShimProcessIpc 
( 
  loc_cmd_s_type * const pz_Cmd
);


/* -----------------------------------------------------------------------*//**
@brief
  This function is to be called by the module that intends to send out an 
  indication to the client of the QMI-LOC, Service. 

@return TRUE    The indication was successfully sent over QCSI
@return FALSE   The indication was not successfully sent over QCSI

@warning
  This function assumes that it is called in the context of the service 
  providers task. ( Loc-MW task )
*//* ------------------------------------------------------------------------*/
boolean locQmiShimSendInd
(
  loc_client_handle_type                  z_LocClientHandle,
  const locQmiShimIndInfoStructT         *pz_IndInfo
);

/* -----------------------------------------------------------------------*//**
@brief
  This function is to be called by the module that intends to broadcast an 
  indication to multiple clients of the QMI-LOC, Service. 

@return TRUE    The indication was successfully sent over QCSI
@return FALSE   The indication was not successfully sent over QCSI

@warning
  This function assumes that it is called in the context of the service 
  providers task. ( Loc-MW task )
*//* ------------------------------------------------------------------------*/
boolean locQmiShimBroadcastInd
(
  const uint64                           t_EventMask,
  const locQmiShimIndInfoStructT * const pz_IndInfo
);


/* -----------------------------------------------------------------------*//**
@brief
  Function to raise event to control point when the live position is available
  
@param[in]   client_ptr            Client Info Handle 
@param[in]   batching_element      Batched position report sent to control point


@retval    TRUE           event reporting success
@retval    FALSE          event reporting fail
*//* ------------------------------------------------------------------------*/
extern boolean loc_batching_report_live_position_event(loc_client_info_s_type                 *const client_ptr,
                                                       const qmiLocBatchedReportStructT_v02   *batching_element); 


/* -----------------------------------------------------------------------*//**
@brief
  Function to raise event to control point the batch is full
  
@param[in]   client_ptr            Client Info Handle


@retval    TRUE           event reporting success
@retval    FALSE          event reporting fail
*//* ------------------------------------------------------------------------*/
extern boolean loc_batching_report_batch_full_event(loc_client_info_s_type    *const client_ptr);

/* -----------------------------------------------------------------------*//**
@brief
  Function to send start batching indication to the control point 
  
@param[in]   locClientHandle       Loc MW client handle 
@param[in]   locReqId              ReqId of the batching request
@param[in]   status                Indication status
                                   
@retval    TRUE           success
@retval    FALSE          failure
*//* ------------------------------------------------------------------------*/
extern boolean locSendStartBatchingInd(  loc_client_handle_type locClientHandle,
  uint32 locReqId, qmiLocStatusEnumT_v02 status);

/* -----------------------------------------------------------------------*//**
@brief
  Function to send batching status event indication to the control point 
  
@param[in]   locClientHandle       Loc MW client handle 
@param[in]   status                Indication status
                                   
@retval    TRUE           success
@retval    FALSE          failure
*//* ------------------------------------------------------------------------*/
extern boolean locSendBatchingStatusInd( loc_client_handle_type locClientHandle,
  qmiLocBatchingStatusEnumT_v02 status);

/* -----------------------------------------------------------------------*//**
@brief
  This function is called by loc_initialize_modules to read the NV items. 
*/
/* ------------------------------------------------------------------------*/
void locQmiShimNvInit(void );

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC Indication for sub-components' commands

@param[in]   locClientHandle       Loc MW client handle 
@param[in]   q_MsgID               QMI_LOC Msg ID
@param[in]   p_IndData             Indication data pointer
@param[in]   e_IndStatus           Indication status
                                   
@retval    TRUE           success
@retval    FALSE          failure
---------------------------------------------------------------------------*/
boolean locQmi_ProcessInd
(
   loc_client_handle_type       l_ClientHandle,
   uint32                       q_MsgID,
   void*                        p_IndData,
   qmiLocStatusEnumT_v02        e_IndStatus
);

#endif /* #ifndef LOC_QMI_SHIM_H */

