
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM 1X UP Interface

General Description
  This file contains implementations for TM 1X UP Interface
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/src/tm_1x_up.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/14   skm    NV Async Read changes 
07/02/08    ank     Added LBS support for CDG OMH R-UIM Spec
05/27/07    rw      Add interface to 1x UP sub-module to register TM-Core callback
12/22/06    ank     Added support for MSBased Throttling feature.
12/11/06    ank     Changes to read NV items.
11/15/06    ank     Added tm_1x_up_create_custom_header, tm_1x_up_read_msid_fields for Vx and Trusted call flows.
10/25/06    ank     Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "cai.h"
#include "paixlate.h"
#include "sm_nv.h"
#include "tm_1x_up.h"
#include "tm_1x_up_smod_iface.h"
#include "tm_nv.h"

paii_msid_hdr_type tm_1x_up_msid_fields;
/* NV buffer, used to read NV items */
static nv_item_type      tm_1x_up_nv_read_buffer;
tm_1x_up_config_s_type   tm_1x_up_config;

/*===========================================================================

FUNCTION tm_1x_up_create_custom_header

DESCRIPTION
  This function creates the custom header required for TCP/IP implementation.

DEPENDENCIES
  None

RETURN VALUE
  Length of created header in bytes.

SIDE EFFECTS
  None

===========================================================================*/

word tm_1x_up_create_custom_header
(
    byte* start                  /* Buffer to hold returned custom header */
)
{
    boolean             status;     /* holds return from translate routine */
    word                buffer_pos;              /* Bit position in buffer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    status = paix_xlate_int_msid_hdr(start, &tm_1x_up_msid_fields,&buffer_pos);
    if (status == FALSE)
    {
        MSG_ERROR( "Error translating.", 0, 0, 0);
        return 0;
    }

    return NUM_BYTES_IN_BIT(buffer_pos);
} /* tm_1x_up_create_custom_header() */

/*===========================================================================

FUNCTION   tm_1x_up_read_msid_fields

DESCRIPTION
  This function is used to read NAM parameters from NV for initializing MSID

RETURN VALUE
  None

============================================================================*/
void tm_1x_up_read_msid_fields( void )
{
   MSG_HIGH("MSID RUIM READ START",0,0,0);

  (void)tm_core_nv_async_read(NV_CURR_NAM_I,(void *)&tm_1x_up_nv_read_buffer);

  (void)tm_core_nv_async_read(NV_MIN1_I,(void *)&tm_1x_up_nv_read_buffer);

  (void)tm_core_nv_async_read(NV_MIN2_I,(void *)&tm_1x_up_nv_read_buffer);

  (void)tm_core_nv_async_read(NV_IMSI_MCC_I,(void *)&tm_1x_up_nv_read_buffer);

  (void)tm_core_nv_async_read(NV_IMSI_11_12_I,(void *)&tm_1x_up_nv_read_buffer);

} /* End tm_1x_up_read_msid_fields( ) */

/*===========================================================================

FUNCTION   tm_1x_up_read_msid_fields_complete

DESCRIPTION
  This function is used to read NAM parameters from NV for initializing MSID
  The functoin is call after PRTL_EVENT_TYPE_RUIM_NV_READ event is sent by
  tm_core post Asynchronus read completion for MSID fields.

RETURN VALUE
  None

============================================================================*/
void tm_1x_up_read_msid_fields_complete( const prtl_event_u_type  *event_payload )
{
  byte   tm_1x_up_nam;
  dword  tm_1x_up_imsi_s1;
  word   tm_1x_up_imsi_s2;

  tm_1x_up_msid_fields.msid_type = CAI_IMSI_ADDR_TYPE;
  tm_1x_up_msid_fields.msid_len  = 7;
  tm_1x_up_msid_fields.msid.type2.imsi.imsi_class = 0;
  tm_1x_up_msid_fields.msid.type2.imsi.class_type.zero.imsi_class_0_type = 3;

  if (event_payload == NULL) 
  {
     MSG_ERROR("MSID RUIM READ Failed, event_payload NULL",0,0,0);
     return;
  }

  if ( event_payload->ruim_read_nv_payload.tm_1x_up_curr_nam > NV_MAX_NAMS )
  {
    tm_1x_up_nam = 0;
  }
  else
  {
    tm_1x_up_nam = event_payload->ruim_read_nv_payload.tm_1x_up_curr_nam;
  }
  MSG_MED("CURRENT NAM : %d",tm_1x_up_nam,0,0);

  tm_1x_up_nv_read_buffer.min1.nam = tm_1x_up_nam;
  tm_1x_up_imsi_s1 = event_payload->ruim_read_nv_payload.tm_1x_up_imsi_s1;
  MSG_MED("IMSI_S1 : %d",tm_1x_up_imsi_s1,0,0);

  tm_1x_up_nv_read_buffer.min2.nam = tm_1x_up_nam;
  tm_1x_up_imsi_s2 = event_payload->ruim_read_nv_payload.tm_1x_up_imsi_s2;
  MSG_MED("IMSI_S2 : %d",tm_1x_up_imsi_s2,0,0);

  /* Set IMSI_S by concatenating IMSI_S2 and IMSI_S1 into a qword */
  qw_set( tm_1x_up_msid_fields.msid.type2.imsi.class_type.zero.type.three.imsi_s, (dword) ((tm_1x_up_imsi_s2 & 0x0300) >> 8),
            ((tm_1x_up_imsi_s1 & 0x00FFFFFFL) |
             (((dword) tm_1x_up_imsi_s2 & 0x000000FFL) << 24)) );

  tm_1x_up_nv_read_buffer.imsi_mcc.nam = tm_1x_up_nam;
  tm_1x_up_msid_fields.msid.type2.imsi.class_type.zero.type.three.mcc = event_payload->ruim_read_nv_payload.mcc;
  MSG_MED("MCC : %d",tm_1x_up_msid_fields.msid.type2.imsi.class_type.zero.type.three.mcc,0,0);

  tm_1x_up_nv_read_buffer.imsi_11_12.nam = tm_1x_up_nam;
  tm_1x_up_msid_fields.msid.type2.imsi.class_type.zero.type.three.imsi_11_12 = event_payload->ruim_read_nv_payload.imsi_11_12;
  MSG_MED("IMSI_11_12 : %d",tm_1x_up_msid_fields.msid.type2.imsi.class_type.zero.type.three.imsi_11_12,0,0);

} /* End tm_1x_up_read_msid_fields_complete( ) */

/*===========================================================================

FUNCTION tm_1x_up_msb_throttling_init

DESCRIPTION
  This function is used to initialize throttling related values.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_msb_throttling_init(void)
{
  (void)sm_nv_read(NV_GPS1_MSB_THROTTLE_ENABLE_I, (void *)&tm_1x_up_nv_read_buffer);
  tm_1x_up_config.msb_throttling_info.msb_throttling_enabled  = (byte)tm_1x_up_nv_read_buffer.gps1_msb_throttle_enable;  

  (void)sm_nv_read( NV_GPS1_MSB_BACK_OFF_FACTOR_I, (void *)&tm_1x_up_nv_read_buffer );
  tm_1x_up_config.msb_throttling_info.back_off_factor         = tm_1x_up_nv_read_buffer.gps1_msb_back_off_factor;

  /* Instead of using #define, read from NV and convert to qword here.
  */
  (void)sm_nv_read( NV_GPS1_MSB_BACK_OFF_MIN_I, (void *)&tm_1x_up_nv_read_buffer );
  convert_to_qword(tm_1x_up_config.msb_throttling_info.back_off_min_duration, tm_1x_up_nv_read_buffer.gps1_msb_back_off_min);

  convert_to_qword(tm_1x_up_config.msb_throttling_info.back_off_time, tm_1x_up_nv_read_buffer.gps1_msb_back_off_min);

  (void)sm_nv_read( NV_GPS1_MSB_BACK_OFF_MAX_I, (void *)&tm_1x_up_nv_read_buffer );
  convert_to_qword(tm_1x_up_config.msb_throttling_info.back_off_max_duration, tm_1x_up_nv_read_buffer.gps1_msb_back_off_max);

  (void)sm_nv_read( NV_GPS1_MSB_BACK_OFF_RESET_I, (void *)&tm_1x_up_nv_read_buffer );
  convert_to_qword(tm_1x_up_config.msb_throttling_info.back_off_reset_duration, tm_1x_up_nv_read_buffer.gps1_msb_back_off_reset);

  MSG_HIGH("max[0]=%d,max[1]=%d",tm_1x_up_config.msb_throttling_info.back_off_max_duration[0],tm_1x_up_config.msb_throttling_info.back_off_max_duration[1],0);
  MSG_HIGH("min[0]=%d,min[1]=%d",tm_1x_up_config.msb_throttling_info.back_off_min_duration[0],tm_1x_up_config.msb_throttling_info.back_off_min_duration[1],0);
  MSG_HIGH("reset[0]=%d,reset[1]=%d",tm_1x_up_config.msb_throttling_info.back_off_reset_duration[0],tm_1x_up_config.msb_throttling_info.back_off_reset_duration[1],0);

  qw_set(tm_1x_up_config.msb_throttling_info.throttle_end_time,0,0);
  qw_set(tm_1x_up_config.msb_throttling_info.msb_last_request_time,0,0);
  tm_1x_up_config.msb_throttling_info.mpc_throttle_check = FALSE;

#ifdef FEATURE_CALL_THROTTLE                
  tm_1x_up_config.msb_throttling_info.safe_dload_req = FALSE;
#endif /* FEATURE_CALL_THROTTLE */    
} /* end tm_1x_up_msb_throttling_init() */

/*===========================================================================

FUNCTION tm_1x_up_read_nv

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_1x_up_read_nv(void)
{
  uint16 i;

  /* Init the PDE IP Addr */
  (void)sm_nv_read( NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I, (void *)&tm_1x_up_nv_read_buffer );
  tm_1x_up_config.pde_adrs = tm_1x_up_nv_read_buffer.cgps_1x_pde_server_addr_ipv4;

  /* Init the Port id */
  (void)sm_nv_read( NV_CGPS_1X_PDE_SERVER_PORT_I, (void *)&tm_1x_up_nv_read_buffer );
  tm_1x_up_config.port_id = tm_1x_up_nv_read_buffer.cgps_1x_pde_server_port;

  /* Use NV_GPS1_CAPABILITIES_I to turn on/off CLM */
  (void)sm_nv_read(NV_GPS1_CAPABILITIES_I, (void*) &tm_1x_up_nv_read_buffer );
  tm_1x_up_config.coarse_location_enabled = tm_1x_up_nv_read_buffer.gps1_capabilities;

  tm_1x_up_msb_throttling_init();

  memset( &tm_1x_up_config.pde_addr_url[0], 0, TM_PDE_MAX_URL_SIZE );
  (void)sm_nv_read( NV_CGPS_1X_PDE_SERVER_ADDR_URL_I, (void *)&tm_1x_up_nv_read_buffer);
  i=0;
  while ( ( tm_1x_up_nv_read_buffer.cgps_1x_pde_server_addr_url[i] != 0 ) &&  
          ( i < ( TM_PDE_MAX_URL_SIZE -1 ) )
        )
  {
    tm_1x_up_config.pde_addr_url[i] = tm_1x_up_nv_read_buffer.cgps_1x_pde_server_addr_url[i];
    i++;
  }   
}

#ifdef FEATURE_UIM_SUPPORT_LBS
/*===========================================================================

FUNCTION tm_1x_up_ruim_read

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_1x_up_ruim_read(void)
{

  /* READ the PDE IP Addr */
  (void)tm_core_nv_async_read( NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I, (void *)&tm_1x_up_nv_read_buffer );

  /* READ the Port id */
  (void)tm_core_nv_async_read( NV_CGPS_1X_PDE_SERVER_PORT_I, (void *)&tm_1x_up_nv_read_buffer );

  /* Use NV_GPS1_CAPABILITIES_I to turn on/off CLM */
  (void)tm_core_nv_async_read(NV_GPS1_CAPABILITIES_I, (void*) &tm_1x_up_nv_read_buffer );

  (void)tm_core_nv_async_read( NV_CGPS_1X_PDE_SERVER_ADDR_URL_I, (void *)&tm_1x_up_nv_read_buffer);
 
}

/*===========================================================================

FUNCTION tm_1x_up_ruim_read

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_1x_up_ruim_read_complete(const prtl_event_u_type  *event_payload)
{
    if (event_payload == NULL) 
    {
       MSG_ERROR("RUIM 1X UP Read Failed, event_payload NULL",0,0,0);
       return;
    }
    /* INIT the PDE IP Addr */
    if (event_payload->ruim_read_nv_payload.pde_adrs != NULL) 
    {
       tm_1x_up_config.pde_adrs = event_payload->ruim_read_nv_payload.pde_adrs;
    }
    /* INIT the PDE PORT */
    if (event_payload->ruim_read_nv_payload.pde_port != NULL)
    {
        tm_1x_up_config.port_id = event_payload->ruim_read_nv_payload.pde_port;
    }
    MSG_MED("RUIM ASYNC READ 1X UP PDE IP: %x PORT :%x",tm_1x_up_config.pde_adrs, tm_1x_up_config.port_id, 0);

    tm_1x_up_config.coarse_location_enabled = event_payload->ruim_read_nv_payload.coarse_location_enabled;

    /*Copy the PDE URL if provided*/
    if (event_payload->ruim_read_nv_payload.pde_addr_url!=NULL) 
    {
       memscpy((void *) &(tm_1x_up_config.pde_addr_url[0]),
              sizeof(tm_1x_up_config.pde_addr_url),
              (const void *)&(event_payload->ruim_read_nv_payload.pde_addr_url[0]),
               sizeof(event_payload->ruim_read_nv_payload.pde_addr_url));         
    }
    return;
}

#endif /* FEATURE_UIM_SUPPORT_LBS */

/*===========================================================================

FUNCTION tm_1xup_prtl_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_1xup_prtl_init(void)
{
  MSG_MED("TM_1X_UP Init", 0, 0, 0);

  tm_1x_up_read_nv();

  /* Initialize the 1x UP sub-modules */
  tm_1x_up_smod_prtl_init();
}
