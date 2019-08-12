/*
  Copyright (C) 2013-2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/vocsvc/src/vocsvc_cm.c#1 $
  $Author: mplcsds1 $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/
#include <stddef.h>
#include <string.h>

#include "apr_api_inline.h"
#include "aprv2_api_inline.h"
#include "apr_event.h"
#include "apr_log.h"
#include "apr_misc.h"
#include "apr_errcodes.h"
#include "aprv2_msg_if.h"


#include "msg.h"
#include "vocsvc.h"
#include "cm.h"

#define VOCSVC_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { apr_log_panic_printf( "vocsvc_cm:%d error[%d]", __LINE__, rc, 0 ); } }

static uint32_t vocsvc_cm_apr_handle = APR_NULL_V;

static char_t vocsvc_cm_my_dns[] = "oem.cm";
static uint16_t vocsvc_cm_my_addr = 0;

static char_t mws_cm_dns[] = "qcom.apps.mws"; /* HACK: Name to be changed in future releases.*/
static uint16_t mws_cm_addr;

static uint16_t modem_mws_enabled = 0;

#define VOCSVC_CM_CMD_TOKEN 0x00011305
#define MWS_CM_CMD_TOKEN 0x00011304
#define MWS_ICM_EVT_SEND_STATUS 0x00011303
#define VOCSVC_CM_PORT APR_NULL_V

#define VOCSVC_CM_CALL_TYPE_VOICE 0
#define VOCSVC_CM_CALL_TYPE_EMERGENCY 9

#define VOCSVC_CM_CALL_EVENT_ORIGINATING 0
#define VOCSVC_CM_CALL_EVENT_END 3
#define VOCSVC_CM_CALL_EVENT_INCOMING 5
#define VOCSVC_CM_CALL_EVENT_CONNECT 6

cm_client_id_type vocsvc_cm_client_id;

typedef struct vocsvc_cm_pkt_t
{
   uint32_t call_type;
   uint32_t call_event_type;
}vocsvc_cm_pkt_t;

extern void vocsvc_cm_call_event_cb(cm_call_event_e_type call_event,
  const cm_mm_call_info_s_type *call_info_ptr);

static int32_t vocsvc_cm_apr_isr_dispatch_fn ( aprv2_packet_t* packet , void* dispatch_data )
{
  uint32_t rc = APR_EOK;

  switch ( packet->opcode )
  {
    case APRV2_IBASIC_EVT_ACCEPTED:
    case APRV2_IBASIC_RSP_RESULT:
    rc = __aprv2_cmd_free( vocsvc_cm_apr_handle, packet );
    break;

    default:
    break;
  }

  return rc;
}

void vocsvc_reg_with_cm( void )
{
  cm_client_status_e_type  vocsvc_cm_init_return_status   = CM_CLIENT_OK;
  cm_client_status_e_type  vocsvc_cm_reg_return_status    = CM_CLIENT_OK;
  uint32_t rc = APR_EOK;

  vocsvc_cm_init_return_status = cm_client_init(CM_CLIENT_TYPE_FLOATING1,
                                        &vocsvc_cm_client_id);

  if ( vocsvc_cm_init_return_status != CM_CLIENT_OK ) {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL, \
           "vocsvc_reg_with_cm:cm_client_init() returned bad status %d",
               (int32)vocsvc_cm_init_return_status );
    rc = APR_EFAILED;
    VOCSVC_PANIC_ON_ERROR(rc);
  }

  (void)cm_client_act(vocsvc_cm_client_id);

  /*! register for Incoming call event                                        */
  /*! cm_mm_client_call_reg()                                                 */
  /*! 1st arg --> pointer to the client struct                                */
  /*! 2nd arg --> pointer to a cb function for call events                    */
  /*! 3rd arg --> event registration                                          */
  /*! 4th arg --> from this call event                                        */
  /*! 5th arg --> to this call event                                          */
  /*! 6th arg --> pointer to a cb function for errors                         */

  vocsvc_cm_reg_return_status = cm_mm_client_call_reg(
                        vocsvc_cm_client_id,
                        vocsvc_cm_call_event_cb,
                        CM_CLIENT_EVENT_REG,
                        CM_CALL_EVENT_INCOM,
                        CM_CALL_EVENT_INCOM,
                        NULL );

  if (vocsvc_cm_reg_return_status != CM_CLIENT_OK) {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vocsvc_reg_with_cm: Failed to " \
         "register for CM Incoming Call event");
  }

  /*! register for Originated call event */

  vocsvc_cm_reg_return_status = cm_mm_client_call_reg(
                    vocsvc_cm_client_id,
                    vocsvc_cm_call_event_cb,
                    CM_CLIENT_EVENT_REG,
                    CM_CALL_EVENT_ORIG,
                    CM_CALL_EVENT_ORIG,
                    NULL );

  if (vocsvc_cm_reg_return_status != CM_CLIENT_OK) {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vocsvc_reg_with_cm:Failed to " \
         "register for CM Originated Call event");
  }

  /*! register for Connect call event */
  vocsvc_cm_reg_return_status = cm_mm_client_call_reg(
                        vocsvc_cm_client_id,
                        vocsvc_cm_call_event_cb,
                        CM_CLIENT_EVENT_REG,
                        CM_CALL_EVENT_CONNECT,
                        CM_CALL_EVENT_CONNECT,
                        NULL );
  if (vocsvc_cm_reg_return_status != CM_CLIENT_OK) {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vocsvc_reg_with_cm:Failed to " \
         "register for CM Connect Call event");
  }

 /*! register for Call End event */

  vocsvc_cm_reg_return_status = cm_mm_client_call_reg(
                    vocsvc_cm_client_id,
                    vocsvc_cm_call_event_cb,
                    CM_CLIENT_EVENT_REG,
                    CM_CALL_EVENT_END,
                    CM_CALL_EVENT_END,
                    NULL );

  if (vocsvc_cm_reg_return_status != CM_CLIENT_OK) {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vocsvc_reg_with_cm:Failed to " \
         "register for CM Call End event");
  }


} /* end of vocsvc_reg_with_cm */

int32_t vocsvc_cm_apr_init ( void )
{
  uint32_t rc = APR_EOK;

  rc = __aprv2_cmd_register2(
         &vocsvc_cm_apr_handle, vocsvc_cm_my_dns, sizeof( vocsvc_cm_my_dns ),
         0, vocsvc_cm_apr_isr_dispatch_fn, NULL, &vocsvc_cm_my_addr );

  rc = __aprv2_cmd_local_dns_lookup(
         mws_cm_dns, sizeof( mws_cm_dns ), &mws_cm_addr );

  vocsvc_reg_with_cm();

  return rc;
}

int32_t vocsvc_cm_apr_deinit ( void )
{
  int32_t rc;

  rc = __aprv2_cmd_deregister( vocsvc_cm_apr_handle );

  return rc;
}

void vocsvc_cm_call_event_cb(cm_call_event_e_type call_event,
  const cm_mm_call_info_s_type *call_info_ptr)
{
  uint32_t rc  = APR_EOK;

  vocsvc_cm_pkt_t cm_apr_pkt;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vocsvc_cm_call_event_cb:CallManager " \
        "callback received %d",
        call_event);

  switch(call_info_ptr->call_type)
  {
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_EMERGENCY:
    cm_apr_pkt.call_type = VOCSVC_CM_CALL_TYPE_VOICE;
    switch(call_event)
    {
      case CM_CALL_EVENT_ORIG  :
           cm_apr_pkt.call_event_type = VOCSVC_CM_CALL_EVENT_ORIGINATING;
		   MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  \
         "vocsvc_cm_call_event_cb:CM_CALL_EVENT_ORIG");
           break;

      case CM_CALL_EVENT_INCOM :
            cm_apr_pkt.call_event_type = VOCSVC_CM_CALL_EVENT_INCOMING;
			MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  \
       "vocsvc_cm_call_event_cb:CM_CALL_EVENT_INCOM");
            break;

      case CM_CALL_EVENT_CONNECT:
            cm_apr_pkt.call_event_type = VOCSVC_CM_CALL_EVENT_CONNECT;
            MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  \
                "vocsvc_cm_call_event_cb:CM_CALL_EVENT_CONNECT");

            break;

      case   CM_CALL_EVENT_END:
            cm_apr_pkt.call_event_type = VOCSVC_CM_CALL_EVENT_END;
			MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  \
       "vocsvc_cm_call_event_cb:CM_CALL_EVENT_END");
            break;
      default:
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  \
                  "vocsvc_cm_call_event_cb:Call Event Type not supported " \
                  "call_event=%d",
                  call_event);
            return;
      }
      break;
      default:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vocsvc_cm_call_event_cb:Call " \
              "Type notsupported call_type=%d",
              call_info_ptr->call_type);
        return;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "vocsvc_cm_call_event_cb:Call Type " \
         "%d call_Event %d",
         cm_apr_pkt.call_type, cm_apr_pkt.call_event_type );
  rc = __aprv2_cmd_alloc_send(
         vocsvc_cm_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
         vocsvc_cm_my_addr, VOCSVC_CM_PORT,
         mws_cm_addr, APR_NULL_V ,
         MWS_CM_CMD_TOKEN, MWS_ICM_EVT_SEND_STATUS,
         &cm_apr_pkt, sizeof(vocsvc_cm_pkt_t) );

  VOCSVC_PANIC_ON_ERROR (rc);

  return;
}

APR_EXTERNAL int32_t vocsvc_cm_call (
  vocsvc_callindex_enum_t index,
  void* params,
  uint32_t size
)
{
  int32_t rc = APR_EOK;

  if  ( modem_mws_enabled )
  {

  switch ( index )
  {
    case VOCSVC_CALLINDEX_ENUM_INIT:
    rc = vocsvc_cm_apr_init( );
    break;

    case VOCSVC_CALLINDEX_ENUM_DEINIT:
    rc = vocsvc_cm_apr_deinit( );
    break;

    default:
    break;
  }

  }
  return rc;
}
