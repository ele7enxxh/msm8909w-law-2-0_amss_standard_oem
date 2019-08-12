// -*- Mode: C -*-
//=========================================================================== 
// FILE: qmi_nas_hdr.c
//
// SERVICE: QMI
//
// DESCRIPTION:
// 
//  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_nas_hdr.c#1 $ 
//   
// Copyright (c) 2011 Qualcomm Technologies Inc. All Rights Reserved. 
//=========================================================================== 

#include "comdef.h"
#include "customer.h"

#include "msg_diag_service.h"

#include "qmi_nas_hdr.h"

#include "qmi_nas.h" // QMI_NAS_MEM_ALLOC
#include "qmi_nas_internal.h"
#include "qm_util.h"

//=========================================================================== 
// FUNCTION: qmi_nas_init_hdr
// 
// DESCRIPTION:
//   Initialize any hdr state vars
//     
// RETURN:
//   None
//=========================================================================== 
//
void qmi_nas_init_hdr( void )
{
   // place holder for future hdr state vars
   QM_MSG_HIGH("qmi_nas_init_hdr: Enter");
} 
 
//=========================================================================== 
// FUNCTION: qmi_nas_register_to_hdr
// 
// DESCRIPTION:
//   Register QMI NAS callback w/HDR
//     
// RETURN:
//   TRUE - success
//   FALSE - failure
//=========================================================================== 
//
boolean qmi_nas_register_to_hdr(void)
{
#ifdef FEATURE_HDR
   hdrerrno_enum_type ret_val;
   int i;

   // Register for call callbacks 
   for ( i = HDRMC_CB_EVENT_SESSION_CLOSE; i <= HDRMC_CB_EVENT_UATI_UPDATE; i++)
   {
      ret_val = hdrmc_client_cb_reg(HDRMC_CB_CLIENT_QMI, 
                                    (hdrmc_cb_event_enum_type)i, 
                                    HDRMC_CB_REG, &qmi_nas_hdr_evt_cb, NULL);
  
      if ( ret_val != E_SUCCESS )
      {
         QM_MSG_ERROR_2("Failed to register for HDR event %d w/%d", i, ret_val);
         return FALSE;
      }
   }

   return TRUE;
#else
   return FALSE;
#endif

} 

//=========================================================================== 
// FUNCTION: qmi_nas_hdr_evt_cb
//
// DESCRIPTION:
//   Callback function registered w/ and invoked by HDR to report HDR events
//
// RETURN:
//   None
//=========================================================================== 
//
void qmi_nas_hdr_evt_cb( hdrmc_cb_params_rsp_union_type *parms, void* client_data )
{
   struct qmi_nas_cmd_hdr_payload *payload;

   (void) client_data; // ignore this param for now

   QM_MSG_HIGH_1("qmi_nas_hdr_evt_cb: parms(!NULL):%d", (NULL !=parms)?1:0 );

   if (!parms) {
      return;
   }

   // Allocate memory for our payload and make a deep copy so we can
   // go out of scope before data is processed (ie. data is processed in a 
   // separate thread, dcc thread)
   payload = (struct qmi_nas_cmd_hdr_payload*)modem_mem_alloc( sizeof(*payload), MODEM_MEM_CLIENT_QMI_MMODE );

   if (!payload) {
      qmi_voice_mem_error();
      return;
   }

   // Copy data into HDR payload
   payload->id = QMI_NAS_CMD_HDR;
   memscpy(&payload->parms, sizeof(payload->parms), parms, sizeof(*parms));

   qmi_mmode_send_cmd( QMI_MMODE_CMD_NAS_HDR_EVT_CB, payload);
}


