/*===========================================================================
                     D S 3 G _ Q M I _ N A S _ C L I E N T . C

DESCRIPTION
This file defines function, variables and data structures for QMI NAS client

EXTERNALIZED FUNCTIONS


 Copyright (c)2011 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3g_qmi_nas_client.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/02/12    var    Initial revision

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "data_msg.h"
#include "modem_mem.h"

#ifdef FEATURE_8960_SGLTE_FUSION
#include "ds3g_qmi_nas_client.h"
#include "ds3gdsdif.h"
#include "dstaski.h" 
#include "ds3gutil.h"

/*===========================================================================

                         LOCAL DATA DECLARATIONS

===========================================================================*/
#define DS3G_NAS_CLIENT_CB_DATA 0xDEAD

/*---------------------------------------------------------------------------
   QMI Notifier handler
---------------------------------------------------------------------------*/
static qmi_client_type ds3g_qmi_nas_client_notifier_handle = NULL;

/*---------------------------------------------------------------------------
   QMI Client handler
---------------------------------------------------------------------------*/
static qmi_client_type ds3g_qmi_nas_client_handle = NULL;

/*---------------------------------------------------------------------------
   NAS Client cookie
---------------------------------------------------------------------------*/
static int ds3g_qmi_nas_client_client_cb_data = DS3G_NAS_CLIENT_CB_DATA;

/*---------------------------------------------------------------------------
   Notifier OS param
---------------------------------------------------------------------------*/
static qmi_client_os_params  ds3g_qmi_nas_client_notifier_os_params;

/*---------------------------------------------------------------------------
   Client OS param
---------------------------------------------------------------------------*/
static qmi_client_os_params  ds3g_qmi_nas_client_os_params;

/*===========================================================================

                        EXTERNAL FUNCTION DEFINITION

===========================================================================*/

/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_INIT

DESCRIPTION    Initializes the QMI-NAS client during power-up and register 
               with QMI NAS service for system information indication

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds3g_qmi_nas_client_init(void)
{
  qmi_client_error_type         retval;
  qmi_idl_service_object_type   nas_svc_obj=NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  DATA_3GMGR_MSG0(MSG_LEGACY_MED,"ds3g_qmi_nas_client_init()");

  /*-----------------------------------------------------------------------
    This function will be called during powerup. Initialize notifier
    and client handles
  -----------------------------------------------------------------------*/
  ds3g_qmi_nas_client_notifier_handle = NULL;
  ds3g_qmi_nas_client_handle = NULL;
  
  /*-----------------------------------------------------------------------
     Get NAS service object
  -----------------------------------------------------------------------*/
  nas_svc_obj = nas_get_service_object_v01();
  if(!nas_svc_obj)
  {
    DATA_ERR_FATAL("Unable to get NAS service object");
    return;
  }
  DATA_3GMGR_MSG0(MSG_LEGACY_MED,"Get nas service object success");

  /*-----------------------------------------------------------------------
    Initialize OS Signal required by QCCI to notify DS task when NAS
    service comes up 
  -----------------------------------------------------------------------*/
  ASSERT(IS_IN_DS_TASK());
  memset(&ds3g_qmi_nas_client_notifier_os_params,0,sizeof(qmi_cci_os_signal_type));
  ds3g_qmi_nas_client_notifier_os_params.tcb = rex_self();
  ds3g_qmi_nas_client_notifier_os_params.sig = DS_NOTIFY_NAS_SVC_SIG;

  /*-----------------------------------------------------------------------
    Send QMI notifier init
  -----------------------------------------------------------------------*/
  retval = qmi_client_notifier_init(nas_svc_obj,
                                    &ds3g_qmi_nas_client_notifier_os_params,
                                    &ds3g_qmi_nas_client_notifier_handle);
  if((QMI_NO_ERR != retval) || (!ds3g_qmi_nas_client_notifier_handle))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Error during qmi_client_notifier_init error=%d",retval);
    ds3g_qmi_nas_client_notifier_handle = NULL;
  }

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                  "ds3g_qmi_nas_client_init(): Initialization successful");
  return;
}/*ds3g_qmi_nas_client_init()*/


/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_PROCESS_SIGNALS

DESCRIPTION    Funtion to process NAS service notify signal. This function is 
               used to process signal sent to DS task by QCCI notifying NAS
               service.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   Signal DS_NOTIFY_NAS_SVC_SIG will be cleared
===========================================================================*/
void ds3g_qmi_nas_client_process_signal
(
  rex_sigs_type    set_sigs /* Mask containing set signals */
)
{
  qmi_client_error_type                       retval;
  qmi_service_info                            info;
  unsigned int                                num_entries=1;
  unsigned int                                num_services;
  nas_indication_register_req_msg_v01         nas_ind_req, nas_ind_req_ssp;
  nas_indication_register_resp_msg_v01*       nas_ind_reg_resp_msg=NULL;
  nas_indication_register_resp_msg_v01*       nas_ind_reg_ssp_resp_msg=NULL;
  nas_get_sys_info_resp_msg_v01*              nas_get_sys_info_resp_msg=NULL;
  qmi_idl_service_object_type                 nas_service_obj=NULL;
  qmi_txn_handle                              get_sys_txn_hdl;
  qmi_txn_handle                              ind_reg_txn_hdl;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_3GMGR_MSG0(MSG_LEGACY_MED,"ds3g_qmi_nas_client_process_signal()");

  /*-----------------------------------------------------------------------
     Check if correct signal is being processed.
     If Yes, clear the signal and continue
     If No, return immediately
  -----------------------------------------------------------------------*/
  if( (set_sigs & DS_NOTIFY_NAS_SVC_SIG) != 0 )
  {
    ASSERT(IS_IN_DS_TASK());

    DATA_3GMGR_MSG0(MSG_LEGACY_MED,"Clearing signal DS_NOTIFY_NAS_SVC_SIG");
    rex_clr_sigs(rex_self(), DS_NOTIFY_NAS_SVC_SIG );
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Process signal called for incorrect signal 0x%x",
                    set_sigs);
    return;
  }

  /*-----------------------------------------------------------------------
     If ds3g_qmi_nas_client_handle!=NULL, then client_init aleady
     done. The signal might be false signal. Return immediatley.
  -----------------------------------------------------------------------*/
  if(ds3g_qmi_nas_client_handle != NULL)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "False signal, client handle: 0x%x not NULL. RETURN",
                    ds3g_qmi_nas_client_handle);
    return;
  }

  /*-----------------------------------------------------------------------
     Get NAS service object
  -----------------------------------------------------------------------*/
  nas_service_obj = nas_get_service_object_v01();
  if(!nas_service_obj)
  {
    DATA_ERR_FATAL("Unable to get NAS service object");
    return;
  }

  /*------------------------------------------------------------------
     Check if the NAS service is up, if not return. Else,
     Get addressing information for accessing nas service
  ------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_get_service_list(nas_service_obj,
                                               &info,
                                               &num_entries,
                                               &num_services))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                    "QMI-NAS service not up, continue to wait for signal");
    return;
  }
  
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"QMI-NAS Service up");

  /*----------------------------------------------------------------------- 
    Release the notifier handlle. QMI Service notify signal need not be  
    handled anymore.
    QCCI uses notify signal during power-up to signal the
    availability of QMI service. Once signal is processed, release the
    notifier_handle, so that QCCI will not send anymore signals.
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_release(ds3g_qmi_nas_client_notifier_handle))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Notifier release failed");
    ds3g_qmi_nas_client_notifier_handle = NULL;
    return;
  }

  /*-----------------------------------------------------------------------
    Initialize the arguments for QMI Client Init
  ------------------------------------------------------------------------*/
  memset(&ds3g_qmi_nas_client_os_params,0, sizeof(qmi_client_os_params));
  ds3g_qmi_nas_client_os_params.tcb = rex_self();
  ds3g_qmi_nas_client_os_params.sig = DS_SEND_MSG_WAIT_SIG;

  /*----------------------------------------------------------------------- 
       Initialize connection to the NAS service
  -----------------------------------------------------------------------*/
  retval =  qmi_client_init(&info,
                            nas_service_obj,
                            ds3g_qmi_nas_client_ind_cb,
                            &ds3g_qmi_nas_client_client_cb_data,
                            &ds3g_qmi_nas_client_os_params,
                            &ds3g_qmi_nas_client_handle);

  if((retval != QMI_NO_ERR) || (!ds3g_qmi_nas_client_handle))
  {
     DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                     "QMI NAS client init failed.  retval:%d "
                     "ds3g_qmi_nas_client_handle:%d",
                      retval, ds3g_qmi_nas_client_handle);
     ds3g_qmi_nas_client_handle = NULL;

     /* If QMI client init step fails then we need to re-register for 
        notification and try initialization again */
     ds3g_qmi_nas_client_init();
     return;
  }
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"QMI NAS client init successful");

  /*----------------------------------------------------------------------- 
      Register for Error callback
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != qmi_client_register_error_cb(ds3g_qmi_nas_client_handle,
                                                ds3g_qmi_nas_client_error_cb,
                                                &ds3g_qmi_nas_client_client_cb_data))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"QMI NAS register error callback failed");
  }
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"QMI NAS register error cb successful");
  
  /*-----------------------------------------------------------------------
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/ 
  nas_get_sys_info_resp_msg 
    = (nas_get_sys_info_resp_msg_v01*)
        modem_mem_alloc(sizeof(nas_get_sys_info_resp_msg_v01),
                        MODEM_MEM_CLIENT_DATA_CRIT);

  if(nas_get_sys_info_resp_msg == NULL)
  {
    DATA_ERR_FATAL("Couldn't get data_block_ptr memory");
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "Allocated mem for get sys info resp message 0x%x",
                  nas_get_sys_info_resp_msg);

  memset(nas_get_sys_info_resp_msg,0,sizeof(nas_get_sys_info_resp_msg_v01));
  
  /*-----------------------------------------------------------------------
    Send request QMI_NAS_GET_SYS_INFO_REQ_MSG to QMI-NAS service.
    QMI_NAS_GET_SYS_INFO_REQ message has no TLV. 
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != 
       qmi_client_send_msg_async(ds3g_qmi_nas_client_handle,
                                 QMI_NAS_GET_SYS_INFO_REQ_MSG_V01,
                                 NULL,
                                 0,
                                 nas_get_sys_info_resp_msg,
                                 sizeof(nas_get_sys_info_resp_msg_v01),
                                 ds3g_qmi_nas_client_recv_get_sys_info_resp,
                                 &ds3g_qmi_nas_client_client_cb_data,
                                 &get_sys_txn_hdl))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Send NAS get sys info request failed. Free mem 0x%x",
                    nas_get_sys_info_resp_msg);
    modem_mem_free(nas_get_sys_info_resp_msg,MODEM_MEM_CLIENT_DATA);
    return;
  }
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"Send NAS get sys info request successful");

  /*----------------------------------------------------------------------- 
      Create indication register request message
  ------------------------------------------------------------------------*/ 
  memset(&nas_ind_req,0,sizeof(nas_indication_register_req_msg_v01));
  nas_ind_req.sys_info_valid = 1;
  nas_ind_req.sys_info = 1;

  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  nas_ind_reg_resp_msg 
    = (nas_indication_register_resp_msg_v01*)
        modem_mem_alloc(sizeof(nas_indication_register_resp_msg_v01),
                        MODEM_MEM_CLIENT_DATA_CRIT);

  if(nas_ind_reg_resp_msg == NULL)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Couldn't get data_block_ptr memory. Free mem 0x%x",
                    nas_get_sys_info_resp_msg);
    modem_mem_free(nas_get_sys_info_resp_msg,MODEM_MEM_CLIENT_DATA);
    ASSERT(0);
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "Allocated mem for indication register resp message 0x%x",
                  nas_ind_reg_resp_msg);

  memset(nas_ind_reg_resp_msg,0,sizeof(nas_indication_register_resp_msg_v01));

  /*----------------------------------------------------------------------- 
    Send request QMI_NAS_INDICATION_REGISTER_REQ_MSG to QMI-NAS service
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != 
     qmi_client_send_msg_async(ds3g_qmi_nas_client_handle,
                               QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                               &nas_ind_req,
                               sizeof(nas_indication_register_req_msg_v01),
                               nas_ind_reg_resp_msg,
                               sizeof(nas_indication_register_resp_msg_v01),
                               ds3g_qmi_nas_client_recv_ind_reg_resp,
                               &ds3g_qmi_nas_client_client_cb_data,
                               &ind_reg_txn_hdl))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Send NAS indication register request failed. Free mem 0x%x",
                    nas_ind_reg_resp_msg);
    modem_mem_free(nas_ind_reg_resp_msg,MODEM_MEM_CLIENT_DATA);
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Free mem 0x%x",nas_get_sys_info_resp_msg);
    modem_mem_free(nas_get_sys_info_resp_msg,MODEM_MEM_CLIENT_DATA);
    ASSERT(0);
    return;
  }
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                  "Send NAS indication register request successful");


  /*----------------------------------------------------------------------- 
      Create indication register request message
  ------------------------------------------------------------------------*/ 
  memset(&nas_ind_req_ssp,0,sizeof(nas_indication_register_req_msg_v01));
  nas_ind_req_ssp.reg_sys_sel_pref_valid = 1;
  nas_ind_req_ssp.reg_sys_sel_pref = 1;

  /*----------------------------------------------------------------------- 
    Allocate dynamic memory for response message. Memory will be freed
    during response message callback handler
  ------------------------------------------------------------------------*/
  nas_ind_reg_ssp_resp_msg 
    = (nas_indication_register_resp_msg_v01*)
        modem_mem_alloc(sizeof(nas_indication_register_resp_msg_v01),
                        MODEM_MEM_CLIENT_DATA_CRIT);

  if(nas_ind_reg_ssp_resp_msg == NULL)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Couldn't get data_block_ptr memory. Free mem 0x%x",
                    nas_ind_reg_ssp_resp_msg);
    ASSERT(0);
    return;
  }

  DATA_3GMGR_MSG1(
     MSG_LEGACY_HIGH,
     "Allocated mem for indication register resp for sys sel pref message 0x%x",
     nas_ind_reg_ssp_resp_msg);

  memset(nas_ind_reg_ssp_resp_msg,0,
         sizeof(nas_indication_register_resp_msg_v01));

  /*----------------------------------------------------------------------- 
    Send request QMI_NAS_INDICATION_REGISTER_REQ_MSG to QMI-NAS service
  ------------------------------------------------------------------------*/
  if(QMI_NO_ERR != 
     qmi_client_send_msg_async(ds3g_qmi_nas_client_handle,
                               QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                               &nas_ind_req_ssp,
                               sizeof(nas_indication_register_req_msg_v01),
                               nas_ind_reg_ssp_resp_msg,
                               sizeof(nas_indication_register_resp_msg_v01),
                               ds3g_qmi_nas_client_recv_ind_reg_resp,
                               &ds3g_qmi_nas_client_client_cb_data,
                               &ind_reg_txn_hdl))
  {
    DATA_3GMGR_MSG1(
       MSG_LEGACY_ERROR,
       "Send NAS indication register request for sys sel pref failed. Free mem 0x%x",
       nas_ind_reg_ssp_resp_msg);
    modem_mem_free(nas_ind_reg_ssp_resp_msg,MODEM_MEM_CLIENT_DATA);
    return;
  }
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                  "Send NAS ind register req for sys sel pref successful");
  
  return;
}/*ds3g_qmi_nas_client_process_signals()*/


/*===========================================================================

                        LOCAL FUNCTION DEFINITION

===========================================================================*/
/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_IND_CB

DESCRIPTION    QMI-NAS indication callback handler. This function is called 
               by QCCI whenver there is a NAS indication message.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void  ds3g_qmi_nas_client_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                          *ind_buf_ptr,
 unsigned int                   ind_buf_len,
 void                          *ind_cb_data
)
{
  qmi_client_error_type             retval;
  nas_sys_info_ind_msg_v01*         sys_info_ind;
  ds_dsd_event_info_type            dsd_event_info = {0};
  nas_service_domain_enum_type_v01  srv_domain = SYS_SRV_DOMAIN_NO_SRV_V01;
  ds_dsd_client_radio_event_type    dsd_radio_event = DS_DSD_CLIENT_INVALID_EV;
  nas_system_selection_preference_ind_msg_v01* sys_sel_pref_ind;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,"ds3g_qmi_nas_client_ind_cb(): msg_id:%u",
                  msg_id);

  /*---------------------------------------------------------------------
     Check if indication buffer pointer is valid
  ---------------------------------------------------------------------*/
  if((ind_buf_ptr == NULL) || (ind_cb_data == NULL))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Null pointer passed");
    return;
  }

  /*---------------------------------------------------------------------
     Check if calback data is valid
  ---------------------------------------------------------------------*/
  if((int)(*((int*)ind_cb_data)) != DS3G_NAS_CLIENT_CB_DATA)
  {    
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Invalid callback data");
    return;
  }
  
  /*---------------------------------------------------------------------
     Check if msg id is correct
  ---------------------------------------------------------------------*/  
  if((msg_id != QMI_NAS_SYS_INFO_IND_MSG_V01) &&
     (msg_id != QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "ds3g_qmi_nas_client_ind_cb(): Invalid msg_id: %u",msg_id);
    return;
  }

  if(msg_id == QMI_NAS_SYS_INFO_IND_MSG_V01)
  {
    /*-----------------------------------------------------------------------
        Allocate dynamic memory for indication message 
    ------------------------------------------------------------------------*/ 
    sys_info_ind = modem_mem_alloc(sizeof(nas_sys_info_ind_msg_v01),
                                   MODEM_MEM_CLIENT_DATA_CRIT);
    if(sys_info_ind == NULL)
    {
      DATA_ERR_FATAL("Couldn't get data_block_ptr memory");
      return;
    }

    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"Allocated mem for ind message 0x%x",
                    sys_info_ind);

    memset(sys_info_ind,0,sizeof(nas_sys_info_ind_msg_v01));

    /*----------------------------------------------------------------------- 
      Decode the NAS Indication message using QCCI api
    ------------------------------------------------------------------------*/
    retval = qmi_client_message_decode(user_handle,
                                       QMI_IDL_INDICATION,
                                       msg_id,
                                       ind_buf_ptr,
                                       ind_buf_len,
                                       sys_info_ind,
                                       sizeof(nas_sys_info_ind_msg_v01));

    if(QMI_NO_ERR != retval)
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                      "Unable to decode message error=%d Free mem 0x%x",
                      retval,sys_info_ind);
      modem_mem_free(sys_info_ind,MODEM_MEM_CLIENT_DATA);
      return;
    }
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"message decode success");

    /*----------------------------------------------------------------------- 
       Logic to determine GSM acq or lost
         If(srv_status== SYS_SRV_STATUS_SRV)
           If( (srv_domain == SYS_SRV_DOMAIN_PS_ONLY) || (srv_domain == SYS_SRV_DOMAIN_CS_PS_ONLY))
             Notify GSM ACQ on DSD
           Else
             Notify GSM Lost on DSD  
         Else
           Notify GSM Lost on DSD
    ------------------------------------------------------------------------*/

    /*----------------------------------------------------------------------- 
       Check GSM Service staus info (TLV 0x12)
    ------------------------------------------------------------------------*/
    if(sys_info_ind->gsm_srv_status_info_valid == 1)
    {
      if(sys_info_ind->gsm_srv_status_info.srv_status == SYS_SRV_STATUS_SRV)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"GSM Service status: %d",
                        sys_info_ind->gsm_srv_status_info.srv_status);

        /*----------------------------------------------------------------------- 
           Check GSM system info (TLV 0x17)
        ------------------------------------------------------------------------*/
        if(sys_info_ind->gsm_sys_info_valid == 1)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"GSM sys info valid");

          /* Check if srv domain is valid*/
          if(sys_info_ind->gsm_sys_info.common_sys_info.srv_domain_valid == 1)
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"GSM srv domain valid");

            /* Get GSM service domain */
            srv_domain = sys_info_ind->gsm_sys_info.common_sys_info.srv_domain;
            dsd_radio_event 
              = ds3g_qmi_nas_client_conv_srv_domain_to_radio_event(srv_domain);

            dsd_event_info.event = dsd_radio_event;
            dsd_event_info.data.attach_radio.is_remote = TRUE;
            dsd_event_info.data.attach_radio.so_mask = 0;
            dsd_event_info.data.attach_radio.srv_domain = srv_domain;
            dsd_event_info.data.attach_radio.srv_status 
              = sys_info_ind->gsm_srv_status_info.srv_status;

            if(sys_info_ind->gsm_sys_info.gsm_specific_sys_info.egprs_supp_valid == 1)
            {

              if(sys_info_ind->gsm_sys_info.gsm_specific_sys_info.egprs_supp == 1)
              {
                dsd_event_info.data.attach_radio.rat_mask = PS_SYS_RAT_3GPP_EDGE;
              }
              else
              {
                dsd_event_info.data.attach_radio.rat_mask = PS_IFACE_3GPP_GPRS;
              }
            }
            else
            {
              dsd_event_info.data.attach_radio.rat_mask = PS_IFACE_3GPP_GPRS;
            }

            /*---------------------------------------------------------------------
                Notify DSD module
            ---------------------------------------------------------------------*/
            DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                            "Notifying DSD. sys_mode:%d, event:%d, is_remote:%d",
                            DS_DSD_RADIO_GSM,
                            dsd_event_info.event,
                            dsd_event_info.data.attach_radio.is_remote);
            DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                            "Notifying DSD. srv_status:%d, srv_domain:%d, rat_mask:%d",
                            dsd_event_info.data.attach_radio.srv_status,
                            dsd_event_info.data.attach_radio.srv_domain,
                            dsd_event_info.data.attach_radio.rat_mask);
            ds_dsd_ext_event_notify(DS_DSD_RADIO_GSM, &dsd_event_info);
          }/* if(srv_domain_valid)*/
          else
          {
            /*---------------------------------------------------------------------
             GSM srv domain invalid. No need to notify DSD
            ---------------------------------------------------------------------*/
            DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"GSM srv domain invalid");
          }
        } /* if(gsm_sys_info_valid)*/
        else
        {
          /*---------------------------------------------------------------------
              GSM Sys info invalid. No need to notify DSD
          ---------------------------------------------------------------------*/
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"GSM sys info invalid");
        }
      }/* if(srv_status == SYS_SRV_STATUS_SRV)*/
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"GSM Service status: %d",
                        sys_info_ind->gsm_srv_status_info.srv_status);
        /*---------------------------------------------------------------------
           GSM Service status NO_SRV. Notify loss to DSD
        ---------------------------------------------------------------------*/
        dsd_event_info.event = DS_DSD_CLIENT_RADIO_LOST_EV;
        dsd_event_info.data.attach_radio.is_remote = TRUE;
        dsd_event_info.data.attach_radio.rat_mask = PS_SYS_RAT_3GPP_NULL_BEARER;
        dsd_event_info.data.attach_radio.so_mask = 0;
        dsd_event_info.data.attach_radio.srv_domain = SYS_SRV_DOMAIN_NO_SRV;
        dsd_event_info.data.attach_radio.srv_status = SYS_SRV_STATUS_NO_SRV;

        /*---------------------------------------------------------------------
            Notify DSD module
        ---------------------------------------------------------------------*/
        DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                        "Notifying DSD. sys_mode:%d, event:%d, is_remote:%d",
                        DS_DSD_RADIO_GSM,
                        dsd_event_info.event,
                        dsd_event_info.data.attach_radio.is_remote);
        DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                        "Notifying DSD. srv_status:%d, srv_domain:%d, rat_mask:%d",
                        dsd_event_info.data.attach_radio.srv_status,
                        dsd_event_info.data.attach_radio.srv_domain,
                        dsd_event_info.data.attach_radio.rat_mask);
        ds_dsd_ext_event_notify(DS_DSD_RADIO_GSM, &dsd_event_info);
      }
    } /* if(gsm_srv_status_info_valid) */
    else
    {
      /*---------------------------------------------------------------------
          GSM service status info invalid. No need to notify DSD
      ---------------------------------------------------------------------*/
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"GSM service status info invalid");
    }

    /*-----------------------------------------------------------------------
       Release the memory allocated for response message
    -----------------------------------------------------------------------*/ 
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"Freeing mem allocated to resp msg 0x%x",
                    sys_info_ind);
    modem_mem_free(sys_info_ind,MODEM_MEM_CLIENT_DATA);
  }

  if(msg_id == QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01)
  {
    /*-----------------------------------------------------------------------
        Allocate dynamic memory for indication message 
    ------------------------------------------------------------------------*/ 
    sys_sel_pref_ind = 
      modem_mem_alloc(sizeof(nas_system_selection_preference_ind_msg_v01),
                      MODEM_MEM_CLIENT_DATA_CRIT);
    if(sys_sel_pref_ind == NULL)
    {
      DATA_ERR_FATAL("Couldn't get data_block_ptr memory");
      return;
    }

    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"Allocated mem for ind message 0x%x",
                    sys_sel_pref_ind);

    memset(sys_sel_pref_ind,0,
           sizeof(nas_system_selection_preference_ind_msg_v01));

    /*----------------------------------------------------------------------- 
      Decode the NAS Indication message using QCCI api
    ------------------------------------------------------------------------*/
    retval = qmi_client_message_decode(user_handle,
                                       QMI_IDL_INDICATION,
                                       msg_id,
                                       ind_buf_ptr,
                                       ind_buf_len,
                                       sys_sel_pref_ind,
                                       sizeof(nas_system_selection_preference_ind_msg_v01));

    if(QMI_NO_ERR != retval)
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                      "Unable to decode message error=%d Free mem 0x%x",
                      retval,sys_sel_pref_ind);
      modem_mem_free(sys_sel_pref_ind,MODEM_MEM_CLIENT_DATA);
      return;
    }
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"message decode success");

    /*----------------------------------------------------------------------- 
       Logic to determine if system selection changed to CS only
         If((srv_domain != SYS_SRV_DOMAIN_PS_ONLY) &&
             (srv_domain != SYS_SRV_DOMAIN_CS_PS_ONLY))
           Notify GSM Lost on DSD  
    ------------------------------------------------------------------------*/

    if(sys_sel_pref_ind->srv_domain_pref_valid == 1)
    {
      if(sys_sel_pref_ind->srv_domain_pref == QMI_SRV_DOMAIN_PREF_CS_ONLY_V01)
      {
        /* Notify GSM lost to DSD only if preferred system is remote */
        //NOTE ruishi check how to pass subs id here
        if(ds3gdsdif_get_preferred_sys(
                         ds3gsubsmgr_subs_id_cm_to_ds3g(ds3g_get_ps_subs_id()))
             == SYS_SYS_MODE_NO_SRV)

        {
          DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "system selection pref. srv domain: %d", 
                          sys_sel_pref_ind->srv_domain_pref);
          /*---------------------------------------------------------------------
             System selection preference change to CS only. Notify loss to DSD
          ---------------------------------------------------------------------*/
          dsd_event_info.event = DS_DSD_CLIENT_RADIO_LOST_EV;
          dsd_event_info.data.attach_radio.is_remote = TRUE;
          dsd_event_info.data.attach_radio.rat_mask = PS_SYS_RAT_3GPP_NULL_BEARER;
          dsd_event_info.data.attach_radio.so_mask = 0;
          dsd_event_info.data.attach_radio.srv_domain = SYS_SRV_DOMAIN_NO_SRV;
          dsd_event_info.data.attach_radio.srv_status = SYS_SRV_STATUS_NO_SRV;

          /*---------------------------------------------------------------------
              Notify DSD module
          ---------------------------------------------------------------------*/
          DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                          "Notifying DSD. sys_mode:%d, event:%d, is_remote:%d",
                          DS_DSD_RADIO_GSM,
                          dsd_event_info.event,
                          dsd_event_info.data.attach_radio.is_remote);
          DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                          "Notifying DSD. srv_status:%d, srv_domain:%d, rat_mask:%d",
                          dsd_event_info.data.attach_radio.srv_status,
                          dsd_event_info.data.attach_radio.srv_domain,
                          dsd_event_info.data.attach_radio.rat_mask);

          ds_dsd_ext_event_notify(DS_DSD_RADIO_GSM, &dsd_event_info);
        }
      }
    }

    /*-----------------------------------------------------------------------
       Release the memory allocated for response message
    -----------------------------------------------------------------------*/ 
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                    "Freeing mem allocated to resp message 0x%x",
                    sys_sel_pref_ind);
    modem_mem_free(sys_sel_pref_ind,MODEM_MEM_CLIENT_DATA);
  }

  return;
}/*ds3g_qmi_nas_client_ind_cb()*/

/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_RECV_GET_SYS_INFO_RESP

DESCRIPTION    QMI-NAS receive GET_SYS_INFO_RESP message handler. This 
               callback is called by QCCI in response to async NAS message
               QMI_NAS_GET_SYS_INFO_REQ message.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   Release the memory assigned for GET_SYS_INFO_RESP message
===========================================================================*/
void ds3g_qmi_nas_client_recv_get_sys_info_resp
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                          *resp_c_struct,
 unsigned int                   resp_c_struct_len,
 void                          *resp_cb_data,
 qmi_client_error_type          transp_err
)
{
  nas_get_sys_info_resp_msg_v01    *sys_info_resp = NULL;
  ds_dsd_event_info_type            dsd_event_info = {0};
  nas_service_domain_enum_type_v01  srv_domain = SYS_SRV_DOMAIN_NO_SRV_V01;
  ds_dsd_client_radio_event_type    dsd_radio_event = DS_DSD_CLIENT_INVALID_EV;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                  "ds3g_qmi_nas_client_recv_get_sys_info_resp()");
  
  /*---------------------------------------------------------------------
     Check if response pointer is valid
  ---------------------------------------------------------------------*/
  if((resp_c_struct == NULL) || (resp_cb_data == NULL))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Null pointer passed");
    return;
  }

  /*---------------------------------------------------------------------
     Check if calback data is valid
  ---------------------------------------------------------------------*/
  if((int)(*((int*)resp_cb_data)) != DS3G_NAS_CLIENT_CB_DATA)
  {    
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Invalid callback data");
    return;
  }

  /*----------------------------------------------------------------------- 
     Message already decoded. Hence no need to call QCCI decode api
  ------------------------------------------------------------------------*/
  sys_info_resp = (nas_get_sys_info_resp_msg_v01*)resp_c_struct;

  if(msg_id != QMI_NAS_GET_SYS_INFO_RESP_MSG_V01)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,"Invalid msg_id=%u. Free mem 0x%x",
                    msg_id,sys_info_resp);
    modem_mem_free(sys_info_resp,MODEM_MEM_CLIENT_DATA);
    return;
  }

  if(sys_info_resp->resp.error != QMI_ERR_NONE_V01)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                    "Error in get_sys_info_resp error=%d. Free mem 0x%x",
                    sys_info_resp->resp.error,sys_info_resp);
    modem_mem_free(sys_info_resp,MODEM_MEM_CLIENT_DATA);
    return;
  }

  /*----------------------------------------------------------------------- 
     Logic to determine GSM acq or lost
       If(srv_status== SYS_SRV_STATUS_SRV)
         If( (srv_domain == SYS_SRV_DOMAIN_PS_ONLY) || (srv_domain == SYS_SRV_DOMAIN_CS_PS_ONLY))
           Notify GSM ACQ on DSD
         Else
           Notify GSM Lost on DSD  
       Else
         Notify GSM Lost on DSD
  ------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------- 
   Check GSM Service staus info (TLV 0x12)
  ------------------------------------------------------------------------*/
  if(sys_info_resp->gsm_srv_status_info_valid == 1)
  {
    if(sys_info_resp->gsm_srv_status_info.srv_status == SYS_SRV_STATUS_SRV)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"GSM Service status: %d",
                      sys_info_resp->gsm_srv_status_info.srv_status);

      /*----------------------------------------------------------------------- 
         Check GSM system info (TLV 0x17)
      ------------------------------------------------------------------------*/
      if(sys_info_resp->gsm_sys_info_valid == 1)
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"GSM sys_info valid");
    
        /* Check if srv domain is valid*/
        if(sys_info_resp->gsm_sys_info.common_sys_info.srv_domain_valid == 1)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"GSM srv domain valid");
    
          /* Get GSM service domain */
          srv_domain = sys_info_resp->gsm_sys_info.common_sys_info.srv_domain;
          dsd_radio_event 
            = ds3g_qmi_nas_client_conv_srv_domain_to_radio_event(srv_domain);
      
          dsd_event_info.event = dsd_radio_event;
          dsd_event_info.data.attach_radio.is_remote = TRUE;
          dsd_event_info.data.attach_radio.so_mask = 0;
          dsd_event_info.data.attach_radio.srv_domain = srv_domain;
          dsd_event_info.data.attach_radio.srv_status
            = sys_info_resp->gsm_srv_status_info.srv_status;

          if(sys_info_resp->gsm_sys_info.gsm_specific_sys_info.egprs_supp_valid == 1)
          {
            if(sys_info_resp->gsm_sys_info.gsm_specific_sys_info.egprs_supp == 1)
            {
              dsd_event_info.data.attach_radio.rat_mask = PS_SYS_RAT_3GPP_EDGE;
            }
            else
            {
              dsd_event_info.data.attach_radio.rat_mask = PS_IFACE_3GPP_GPRS;
            }
          }
          else
          {
            dsd_event_info.data.attach_radio.rat_mask = PS_IFACE_3GPP_GPRS;
          }
      
          /*---------------------------------------------------------------------
              Notify DSD module
          ---------------------------------------------------------------------*/
          DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                          "Notifying DSD. sys_mode:%d, event:%d, is_remote:%d",
                          DS_DSD_RADIO_GSM,
                          dsd_event_info.event,
                          dsd_event_info.data.attach_radio.is_remote);
          DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                          "Notifying DSD. srv_status:%d, srv_domain:%d, rat_mask:%d",
                          dsd_event_info.data.attach_radio.srv_status,
                          dsd_event_info.data.attach_radio.srv_domain,
                          dsd_event_info.data.attach_radio.rat_mask);
          ds_dsd_ext_event_notify(DS_DSD_RADIO_GSM, &dsd_event_info);
        }/* if(srv_domain_valid)*/
        else
        {
          /*---------------------------------------------------------------------
           GSM srv domain invalid. No need to notify DSD
          ---------------------------------------------------------------------*/
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"GSM srv domain invalid");
        }
      }/* if(gsm_sys_info_valid)*/
      else
      {
        /*---------------------------------------------------------------------
           GSM sys info invalid. No need to notify DSD
        ---------------------------------------------------------------------*/
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"GSM sys info invalid");
      }
    }/* if(srv_status == SYS_SRV_STATUS_SRV)*/
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"GSM Service status: %d", 
                      sys_info_resp->gsm_srv_status_info.srv_status);
      /*---------------------------------------------------------------------
         GSM Service status NO_SRV. Notify loss to DSD
      ---------------------------------------------------------------------*/
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_LOST_EV;
      dsd_event_info.data.attach_radio.is_remote = TRUE;
      dsd_event_info.data.attach_radio.rat_mask = PS_SYS_RAT_3GPP_NULL_BEARER;
      dsd_event_info.data.attach_radio.so_mask = 0;
      dsd_event_info.data.attach_radio.srv_domain = SYS_SRV_DOMAIN_NO_SRV;
      dsd_event_info.data.attach_radio.srv_status = SYS_SRV_STATUS_NO_SRV;
  
      /*---------------------------------------------------------------------
          Notify DSD module
      ---------------------------------------------------------------------*/
      DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                      "Notifying DSD. sys_mode:%d, event:%d, is_remote:%d",
                      DS_DSD_RADIO_GSM,
                      dsd_event_info.event,
                      dsd_event_info.data.attach_radio.is_remote);
      DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                      "Notifying DSD. srv_status:%d, srv_domain:%d, rat_mask:%d",
                      dsd_event_info.data.attach_radio.srv_status,
                      dsd_event_info.data.attach_radio.srv_domain,
                      dsd_event_info.data.attach_radio.rat_mask);
      ds_dsd_ext_event_notify(DS_DSD_RADIO_GSM, &dsd_event_info);
    }
  } /* if(gsm_srv_status_info_valid) */
  else
  {
    /*---------------------------------------------------------------------
        GSM service status info invalid. No need to notify DSD
    ---------------------------------------------------------------------*/
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"GSM service status info invalid");
  }

  /*-----------------------------------------------------------------------
     Release the memory allocated for response message
  -----------------------------------------------------------------------*/ 
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"Freeing mem allocated to resp message 0x%x",
                  sys_info_resp);
  modem_mem_free(sys_info_resp,MODEM_MEM_CLIENT_DATA);
  
  return;
}/*ds3g_qmi_nas_client_recv_get_sys_info_resp()*/

/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_RECV_IND_REG_RESP

DESCRIPTION    QMI-NAS receive IND_REG_RESP message handler. This callback 
               is called by QCCI in response to async NAS message
               QMI_NAS_INDICATION_REGISTER_REQ message.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   Release the memory assigned for IND_REG_RESP message
===========================================================================*/
void ds3g_qmi_nas_client_recv_ind_reg_resp
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                          *resp_c_struct,
 unsigned int                   resp_c_struct_len,
 void                          *resp_cb_data,
 qmi_client_error_type          transp_err
)
{
  nas_indication_register_resp_msg_v01* nas_ind_reg_resp = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                  "ds3g_qmi_nas_client_recv_ind_reg_resp()");

  /*---------------------------------------------------------------------
     Check if response pointer is valid
  ---------------------------------------------------------------------*/
  if((resp_c_struct == NULL) || (resp_cb_data == NULL))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Null pointer passed");
    return;
  }

  /*---------------------------------------------------------------------
     Check if calback data is valid
  ---------------------------------------------------------------------*/
  if((int)(*((int*)resp_cb_data)) != DS3G_NAS_CLIENT_CB_DATA)
  {    
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Invalid callback data");
    return;
  }

  nas_ind_reg_resp = (nas_indication_register_resp_msg_v01*)resp_c_struct;

  /*---------------------------------------------------------------------
    Check if msg_id is correct
  ---------------------------------------------------------------------*/
  if(msg_id == QMI_NAS_INDICATION_REGISTER_RESP_MSG_V01)
  {
    /*-------------------------------------------------------------------
      Check if response is successful
    -------------------------------------------------------------------*/
    if(nas_ind_reg_resp->resp.error != QMI_ERR_NONE_V01)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "Error in ind_reg_response msg. error=%d",
                      nas_ind_reg_resp->resp.error);
    }
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                      "ds3g_qmi_nas_client_recv_ind_reg_resp(): Success");
    }
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "ds3g_qmi_nas_client_recv_ind_reg_resp(): Invalid msg_id=%u",
                    msg_id);
  }

  /*-----------------------------------------------------------------------
     Release the memory allocated for response message
  -----------------------------------------------------------------------*/ 
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,"Freeing mem allocated to resp message 0x%x",
                  nas_ind_reg_resp);
  modem_mem_free(nas_ind_reg_resp,MODEM_MEM_CLIENT_DATA);

  return;
}/*ds3g_qmi_nas_client_recv_ind_reg_resp()*/

/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_ERROR_CB

DESCRIPTION    QMI NAS error callback handler. This callback is called by QCCI 
               to notify error.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds3g_qmi_nas_client_error_cb
(
  qmi_client_type user_handle,
  qmi_client_error_type error,
  void *err_cb_data
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_3GMGR_MSG0(MSG_LEGACY_MED,"ds3g_qmi_nas_client_error_cb()");

  /*---------------------------------------------------------------------
     Check if err cb data pointer is valid
  ---------------------------------------------------------------------*/
  if(err_cb_data == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Null pointer passed");
    return;
  }
    
  /*---------------------------------------------------------------------
     Check if calback data is valid
  ---------------------------------------------------------------------*/
  if((int)(*((int*)err_cb_data)) != DS3G_NAS_CLIENT_CB_DATA)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Invalid callback data");
    return;
  }  
  
  switch(error)
  {
    /*----------------------------------------------------------------------- 
      In case of service error, client will be released. No attempt to recover
      the QMI connection will be made.
    ------------------------------------------------------------------------*/
    case QMI_SERVICE_ERR:
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"QMI NAS service is down");
      /*----------------------------------------------------------------------- 
        Release the client. 
      ------------------------------------------------------------------------*/
      if(QMI_NO_ERR != qmi_client_release(ds3g_qmi_nas_client_handle))
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Client release failed");
        return;
      }
    
      ds3g_qmi_nas_client_handle = NULL;
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "ds3g_qmi_nas_client_error_cb(): error=%ld",
                      error);
  }
  return;
}

/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_CONV_SRV_STATUS_TO_RADIO_EVENT

DESCRIPTION    Utility function to covert NAS Service status type to 
               DSD Radio event type.

DEPENDENCIES   NONE

RETURN VALUE   ds_dsd_client_radio_event_type

SIDE EFFECTS   NONE
===========================================================================*/
ds_dsd_client_radio_event_type 
ds3g_qmi_nas_client_conv_srv_domain_to_radio_event
(
  nas_service_domain_enum_type_v01 srv_domain
)
{
  ds_dsd_client_radio_event_type l_dsd_radio_event_type;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                  "conv_srv_domain_to_radio_event(): srv domain: %d",
                  srv_domain);

  switch(srv_domain)
  {
    /* Acquired radio */
    case SYS_SRV_DOMAIN_PS_ONLY_V01:
    case SYS_SRV_DOMAIN_CS_PS_V01:
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"Acquired radio");
      l_dsd_radio_event_type = DS_DSD_CLIENT_RADIO_ACQUIRED_EV;
      break;

    /* Lost radio */
    default:
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"Lost radio");
      l_dsd_radio_event_type =  DS_DSD_CLIENT_RADIO_LOST_EV;
      break;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "convert srv status to radio event: event:%d",
                  l_dsd_radio_event_type);

  return l_dsd_radio_event_type;
}/* ds3g_qmi_nas_client_conv_srv_domain_to_radio_event() */

/*===========================================================================
FUNCTION      DS3G_QMI_NAS_CLIENT_IS_DSD_SYNC_ENABLED

DESCRIPTION    Utility to to check if DSD sync is enabled

DEPENDENCIES  None

RETURN VALUE  TRUE:  DSD Sync is enabled.
              FALSE: DSD Sync is disabled.

SIDE EFFECTS
===========================================================================*/
boolean ds3g_qmi_nas_client_is_dsd_sync_enabled( void )
{
  boolean retval;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  retval = (ds3g_get_disable_dsd_sync() == TRUE)?FALSE:TRUE;
  
  DATA_3GMGR_MSG1(MSG_LEGACY_MED, "dsd_sync_enabled: %d",
                  ds3g_qmi_nas_client_disable_dsd_sync);
  
  return retval;
} /* ds3g_qmi_nas_client_is_dsd_sync_enabled() */

#endif /* FEATURE_8960_SGLTE_FUSION */

