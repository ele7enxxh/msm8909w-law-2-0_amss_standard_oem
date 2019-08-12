/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GFC_QMI module which is the interface between QMI and GM 

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/gfc_qmi/src/gfc_qmi_internal.h#1 $
$DateTime: 2016/12/13 07:59:45 $

=============================================================================

EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/29/12   ssu     Adding geofence purge request support.
11/15/11   ssu     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef GFC_QMI_INTERNAL_H
#define GFC_QMI_INTERNAL_H


#define NON_QMI_TRXN_ID 0xABABABAB

/* The data type is used to synchronously return the status of the Geofencing operation from GFC_QMI . */
typedef enum
{
  GFC_QMI_OP_TYPE_ADD_GF,
  GFC_QMI_OP_TYPE_DELETE_GF,
  GFC_QMI_OP_TYPE_PURGE_GF,
  GFC_QMI_OP_TYPE_EDIT_GF,
  GFC_QMI_OP_TYPE_QUERY_GF,
  GFC_QMI_OP_TYPE_QUERY_GF_IDS,
  GFC_QMI_OP_TYPE_SET_GF_ENGINE_CONFIG,
  GFC_QMI_OP_TYPE_SET_GF_CLIENT_CONFIG,  
  GFC_QMI_OP_TYPE_MAX = 0x1FFFFFFF
} gfc_qmi_op_type;


/*===========================================================================
gfc_qmi_internal_register_cbs

Description:
   This function is used by GFC_QMI to register the function callbacks with the QMI

Parameters:   
  p_geofence_func_cbs: The function callbacks from the QMI .

Return value: 
  FALSE: If the registration succeeds.
  TRUE: Otherwise. 
=============================================================================*/
boolean gfc_qmi_internal_register_cbs(
  const gfc_qmi_cb_rsp_s_type* p_geofence_func_cbs
);


/*===========================================================================
gfc_qmi_internal_val_and_fill_req

Description:
   This function is used to validate and populate the request parameters for the GFC_QMI.
 
Parameters: 
  e_op_type: The type of the GFC_QMI operation requested. 
  p_inp_request: The request from QMI which has the input parameters .
  p_gm_request: The request which is sent to the GM to setup the geofence.
  p_return_code: The return code from the operation

Return value: 
  FALSE: If the parameters are not specified correctly.
  TRUE: If the parameters are specified correctly. 
=============================================================================*/
boolean gfc_qmi_internal_val_and_fill_req(
  gfc_qmi_op_type e_op_type,
  void *p_inp_request,
  void *p_gm_request,
  gfc_qmi_operation_result_e_type *p_return_code
);



/*===========================================================================
gfc_qmi_internal_get_client_handle

Description:
   This function is used to retrieve the client handle of GFC_QMI to the
   GM core.

Parameters:   
  None.

Return value: 
  The client handle of GFC_QMI to GM core. 
=============================================================================*/
sm_gm_client_handle gfc_qmi_internal_get_client_handle(void);

/*
******************************************************************************
* gfc_qmi_internal_set_engine_config_at_bootup
*
* Function description:
*
*   This function is used to set the geofence engine configuration
*   at boot up time , by reading the required configuration items from NV.
*
* Parameters: None.
*   
* Return value: None.
*
******************************************************************************
*/
void gfc_qmi_internal_set_geofence_engine_config_at_bootup(void);

#endif // GFC_QMI_INTERNAL_H
