/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                U M T S

                P A C K E T   D O M A I N
                                
                A T   C O M M A N D   P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes AT commands. It executes UMTS & GPRS specific packet
  domain AT commands.

  
EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsipkt_ex.c#1 $   $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/15   sc      Fixed issue in event handling to use subs_id provided in 
                   event info.
08/12/14   pg      Modified DS Profile APIs to per subs.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Added support for Dynamic ATCoP.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DSAT_ETSI_DATA
#include "stdio.h"

#include <stringl/stringl.h>
#include "msg.h"
#include "nv.h"
#include "dstaski.h"

#include "dsati.h"
#include "dsatparm.h"
#include "dsatctab.h"
#include "dsatdl.h"
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "dsatetsipkt.h"
#include "dsatetsicmif.h"
#include "ds_profile_3gppi.h"
#include "dstaski.h"
#include "nv.h"
#include "sm.h"
#include "sm_common.h"
#include "cm.h"
#include "dsatact.h"
#include "ps_ppp_defs.h"
#include "ps_utils.h"
#include "ps_in.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
  These macros used to be in dsumtspdpreg.h but had to removed from there 
  due to compilation issues seen with pdp reg rpc stub code.
-------------------------------------------------------------------------*/
#define pdp_addr_ipv4 pdp_addr.pdp_addr_ipv4
#define pdp_addr_ipv6 pdp_addr.pdp_addr_ipv6

LOCAL char *etsipkt_cgerep_event_buffer[] = {"+CGEV: REJECT ",
                                             "+CGEV: NW REACT ",
                                             "+CGEV: NW DEACT ",
                                             "+CGEV: ME DEACT ",
                                             "+CGEV: NW DETACH",
                                             "+CGEV: ME DETACH",
                                             "+CGEV: NW CLASS CG",
                                             "+CGEV: NW CLASS CC",
                                             "+CGEV: ME CLASS B",
                                             "+CGEV: ME CLASS CG",
                                             "+CGEV: ME CLASS CC",
                                             "+CGEV: PDN ACT",
                                             "+CGEV: PDN DEACT",
                                             "+CGEV: DED NW DED ACT",
                                             "+CGEV: ME ACT",
                                             "+CGEV: DED NW DEACT",
                                             "+CGEV: ME DEACT",
                                             "+CGEV: NW MODIFY",
                                             "+CGEV: ME MODIFY"
};

dsat_pdp_connect_s_type dsat_pdp_state;

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

LOCAL void etsipkt_add_gprs_msg_to_queue
(
  char                  *buffer,
  uint16                 buf_len,
  sys_modem_as_id_e_type subs_id
);


/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_TE_MT_PDP

/*===========================================================================

FUNCTION DSATETSIPKT_GET_ACCESS_CONTROL

DESCRIPTION
 This function gives the access control status for MT PDP.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the access control.

SIDE EFFECTS
  None

===========================================================================*/
dsat_pdp_access_control_e_type dsatetsipkt_get_access_control(void)
{
  return (dsat_pdp_access_control_e_type)dsatutil_get_val(
                                 DSAT_VENDOR_QCGANSM_IDX,0,0,NUM_TYPE);
} /* dsatetsipkt_get_access_control */

#endif /* FEATURE_DATA_TE_MT_PDP*/


/*===========================================================================
FUNCTION DSATETSIPKT_INIT_TOLERANCE_FROM_NV

DESCRIPTION
  This function reads pdp call setup delay tolerance from the NV during 
  initialization. If this item was never written in NV, the value is set to
  QCPDPLT_DEF_VAL.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsipkt_init_tolerance_from_nv
(
  void
)
{
  nv_stat_enum_type status;

  /*-------------------------------------------------------------------------
    Get tolerance from NV. Look at the status returned. If status !=NV_DONE_S,
    then either NV was never written or it cannot be reused. 
    Set as QCPDPLT_DEF_VAL if NV was never written.
  -------------------------------------------------------------------------*/
  status = dsatutil_get_nv_item( NV_LONG_PDPACT_TOLERANCE_FOR_TE_I, &ds_nv_item );
  if ( status != NV_DONE_S )
  {
    DSATUTIL_SET_VAL(DSAT_VENDOR_QCPDPLT_IDX,0,0,0,QCPDPLT_DEF_VAL,NUM_TYPE)
  }
  else 
  {
    /*-----------------------------------------------------------------------
      Store value retrieved from NV.
    -----------------------------------------------------------------------*/
    DSATUTIL_SET_VAL(DSAT_VENDOR_QCPDPLT_IDX,0,0,0,ds_nv_item.long_pdpact_tolerance_for_te,NUM_TYPE)
  }

} /* dsatetsipkt_init_tolerance_from_nv( ) */

/*===========================================================================
FUNCTION  ETSIPKT_ADD_GPRS_MSG_TO_QUEUE

DESCRIPTION
  Buffers the GPRS event report in MT.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void etsipkt_add_gprs_msg_to_queue
(
  char                    *buffer,
  uint16                   buf_len,
  sys_modem_as_id_e_type   subs_id
)
{
  etsipkt_gprs_msg_type *msg_ptr;
  dsatetsipkt_gprs_msg_ms_info     *gprs_dd_ms_val = NULL;

  gprs_dd_ms_val = dsat_get_base_addr_per_subs(DSAT_MS_MD_VALS, subs_id, TRUE);
  if (NULL == gprs_dd_ms_val)
  {
    return;
  }
  
  if (q_cnt(&gprs_dd_ms_val->etsipkt_gprs_msg_q) < DSAT_GPRS_EVENT_MSG_COUNT)
  {
    msg_ptr = dsat_alloc_memory(sizeof(etsipkt_gprs_msg_type), TRUE);
  }
  else
  {
    /* Get the first element in the actual queue 
       Strategy -circular buffer, discard the oldest ones */
    msg_ptr = q_get(&gprs_dd_ms_val->etsipkt_gprs_msg_q);
  }

  if (NULL == msg_ptr)
  {
    DS_AT_MSG0_ERROR("Failed to buffer the GPRS event report in MT");
  }
  else
  {
    /* Add event data to the message */
    (void) dsatutil_memscpy((void*)msg_ptr->event_data,
            GPRS_EVENT_DATA_SIZE,(void*)buffer,MIN(buf_len,GPRS_EVENT_DATA_SIZE));
    msg_ptr->event_data_len = MIN(buf_len,GPRS_EVENT_DATA_SIZE);

    /* Put the message on the message queue */
    q_put( &gprs_dd_ms_val->etsipkt_gprs_msg_q, &msg_ptr->link );
  }

  return;
} /* etsipkt_add_gprs_msg_to_queue */

/*===========================================================================
FUNCTION  DSATETSIPKT_REPORT_GPRS_EVENT_TO_TE

DESCRIPTION
  Reports the GPRS event report buffer in MT.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsipkt_report_gprs_event_to_te
(
  dsat_cgerep_e_type     event_type, 
  void                  *event_data,
  sys_modem_as_id_e_type subs_id
)
{
  char                           buffer[GPRS_EVENT_DATA_SIZE];
  char                           ip_address[MAX_IPADDR_STR_LEN+1];
  uint16                         buf_len = 0;
  dsat_num_item_type             cgerep_val ;
  dsat_cgerep_evt_info_s_type    *cgerep_evt_info = NULL;
  ds_umts_pdp_addr_type          pdp_addr;
  dsat_ip_addr_type                temp_pdp_addr;
  ds_profile_3gpp_pdp_context_type pdp_context;
  dsat_ip_addr_type                ip_addr;

  memset ( ip_address, 0, sizeof(ip_address));
  memset((void*)&pdp_context, 0x0, sizeof(pdp_context));
  memset(&temp_pdp_addr,0x00,sizeof(temp_pdp_addr));
  
  if (event_type >= ARR_SIZE(etsipkt_cgerep_event_buffer))
  {
    DS_AT_MSG1_ERROR("Invalid event type %d",(int)event_type);
    return;
  }

  buf_len = (uint16)strlen(etsipkt_cgerep_event_buffer[event_type]);
  buf_len = MIN(buf_len,sizeof(buffer) );
  
  DS_AT_MSG2_HIGH("In dsatetsipkt_report_gprs_event_to_te event = %d  subs = %d",event_type,subs_id);
  
  (void) dsatutil_memscpy((void*)buffer,
          GPRS_EVENT_DATA_SIZE,(void*)etsipkt_cgerep_event_buffer[event_type],buf_len);
  
  switch (event_type)
  {
    case DSAT_MS_INITIATED_PDP_DEACTIVATE:
    case DSAT_NW_INITIATED_PDP_DEACTIVATE:
    case DSAT_NW_INITIATED_PDP_REACTIVATE:
      {
        if( ( TRUE == dsatetsipkt_is_profile_context_valid((uint16)((dsat_cgerep_evt_info_s_type*)event_data)->cid,
                                                          DS_PROFILE_TECH_3GPP,
                                                          dsat_get_ds_profile_subs_id(subs_id))) ||
            ( TRUE == dsatetsipkt_is_profile_context_valid((uint16)((dsat_cgerep_evt_info_s_type*)event_data)->cid,
                                                          DS_PROFILE_TECH_EPC,
                                                          dsat_get_ds_profile_subs_id(subs_id))) )

        {
          if(dsatetsipkt_read_write_param_table(context_param_tbl,(uint16)(sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type)),
                       (uint16)((dsat_cgerep_evt_info_s_type*)event_data)->cid,READ_COMMAND,(byte*)&pdp_context,(uint16)sizeof(pdp_context),dsat_get_ds_profile_subs_id(subs_id)) != DSAT_ERROR)
          {

            if(DSAT_OK == ds_3gpp_rmsm_atif_get_pdp_addr((uint32)(int)((dsat_cgerep_evt_info_s_type*)event_data)->cid,
                                                          subs_id,&pdp_addr))
            {
              DS_AT_MSG2_MED("PDP context - %d has a dynamic address: version %d.",
                   (((dsat_cgerep_evt_info_s_type*)event_data)->cid),pdp_addr.ip_vsn);
              
              ip_addr.ip_vsn = (ds_profile_3gpp_ip_version_enum_type)pdp_addr.ip_vsn;
              ip_addr.addr.ds_profile_3gpp_pdp_addr_ipv4 = pdp_addr.pdp_addr_ipv4;

              dsatutil_memscpy((void*)&ip_addr.addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u,
                sizeof(pdp_addr.pdp_addr_ipv6.in6_u),
                 (void*)&pdp_addr.pdp_addr_ipv6.in6_u,
                sizeof(pdp_addr.pdp_addr_ipv6.in6_u));
              
              if( ip_addr.ip_vsn == DS_PROFILE_3GPP_IP_V4)
              {
                temp_pdp_addr.ip_vsn = DS_PROFILE_3GPP_IP_V4; 
                temp_pdp_addr.addr.ds_profile_3gpp_pdp_addr_ipv4 = ps_ntohl(pdp_addr.pdp_addr_ipv4);
                dsatutil_convert_ipaddr(INT_TO_STRING,&temp_pdp_addr,(byte *)ip_address);
              }
              else if( ip_addr.ip_vsn == DS_PROFILE_3GPP_IP_V6 || ip_addr.ip_vsn == DS_PROFILE_3GPP_IP_V4V6 )
              {
                temp_pdp_addr.ip_vsn = DS_PROFILE_3GPP_IP_V6;
                temp_pdp_addr.addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr64[0] = ps_ntohll(pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[1]);
                temp_pdp_addr.addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr64[1] = ps_ntohll(pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[0]);
                dsatutil_convert_ipaddr(INT_TO_STRING,&temp_pdp_addr,(byte *)ip_address);
              }
              else
              {
                return;
              }
              buf_len += (uint16)
              snprintf((char *)&buffer[buf_len],
              sizeof(buffer)-buf_len,
              "IP, %s, %d", 
              ip_address, 
              (int)((dsat_cgerep_evt_info_s_type*)event_data)->cid);
            }
            else
            {
              if( pdp_context.pdp_type == DS_PROFILE_3GPP_PDP_IPV6 ||
                               pdp_context.pdp_type == DS_PROFILE_3GPP_PDP_IPV4V6 )
              {
                temp_pdp_addr.ip_vsn = DS_PROFILE_3GPP_IP_V6;
              }
              else
              {
                temp_pdp_addr.ip_vsn = DS_PROFILE_3GPP_IP_V4;
              }
              dsatutil_convert_ipaddr(INT_TO_STRING,&temp_pdp_addr,(byte *)ip_address);
              buf_len += (uint16)
              snprintf((char *)&buffer[buf_len],
              sizeof(buffer)-buf_len,
              "IP, %s, %d", 
              ip_address, 
              (int)((dsat_cgerep_evt_info_s_type*)event_data)->cid);
            }
          }
        }
      }
      break;
    case DSAT_PDN_ACTIVATE:
    case DSAT_PDN_DEACTIVATE:
      buf_len += (uint16)snprintf((char *)&buffer[buf_len],
                                          sizeof(buffer)-buf_len,
                                          "%d", 
                    (int)((dsat_cgerep_evt_info_s_type*)event_data)->cid);
      break;
    case DSAT_DED_NW_DED_ACTIVATE:
    case DSAT_DED_NW_DEACTIVATE:
    case DSAT_MS_ACTIVATE:
    case DSAT_MS_DEACTIVATE:
      cgerep_evt_info = (dsat_cgerep_evt_info_s_type*)event_data;

      buf_len += (uint16)snprintf((char *)&buffer[buf_len],
                                          sizeof(buffer)-buf_len,
                                          "%d %d %d", 
                                          (int)cgerep_evt_info->p_cid,
                                          (int)cgerep_evt_info->cid,
                                          cgerep_evt_info->evt_type);
      break;
    case DSAT_NW_MODIFY:
    case DSAT_MS_MODIFY:
      cgerep_evt_info = (dsat_cgerep_evt_info_s_type*)event_data;

      buf_len += (uint16)snprintf((char *)&buffer[buf_len],
                                          sizeof(buffer)-buf_len,
                                          "%d %d %d", 
                                          (int)cgerep_evt_info->cid,
                                          cgerep_evt_info->change_reason,
                                          cgerep_evt_info->evt_type);
      break;
    default:;
  }/* Switch */
  cgerep_val =(dsat_num_item_type)dsatutil_get_val(
                                          DSATETSI_EXT_CGEREP_IDX,subs_id,0,NUM_TYPE);
  if ( !dsatcmdp_block_indications_ex() )
  {
   /* TA-TE link is not reserved, forward the results directly to TE 
    except when mode is DSAT_CGEREP_BUFFER_AT_MT */
    if ( ( cgerep_val == DSAT_CGEREP_BUFFER_DISCARD_FWD_TE ) ||
         ( cgerep_val == DSAT_CGEREP_BUFFER_FWD_TE ))
    {
      /* Fwd to TE as the MT is not in online-data mode */
      dsm_item_type *res_buff_ptr;
  
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
      if (NULL != res_buff_ptr)
      {
        (void) dsatutil_memscpy((void*)res_buff_ptr->data_ptr,
                res_buff_ptr->size,(void*)buffer,MIN(buf_len,sizeof(buffer)));
        
        res_buff_ptr->used = MIN(buf_len,sizeof(buffer));
        (void)dsatcmdp_send_urc_per_subs(subs_id, res_buff_ptr, DSAT_COMPLETE_RSP);
      }
    }
    /* TA-TE link is not reserved but mode is  
       DSAT_CGEREP_BUFFER_AT_MT so buffer it*/
    else if (  cgerep_val == DSAT_CGEREP_BUFFER_AT_MT )
    {
      etsipkt_add_gprs_msg_to_queue(buffer, buf_len, subs_id);
    }
  }
  else
  {
    /* TA-TE link is reserved. Buffer the result only when 
      mode is DSAT_CGEREP_BUFFER_FWD_TE or DSAT_CGEREP_BUFFER_AT_MT */
    /* Buffer the unsolicited result codes in MT. No forwarding 
     to TE needed */
    if ( ( cgerep_val == DSAT_CGEREP_BUFFER_FWD_TE )||
         ( cgerep_val == DSAT_CGEREP_BUFFER_AT_MT ))
      etsipkt_add_gprs_msg_to_queue(buffer, buf_len, subs_id);
  }
} /* dsatetsipkt_report_gprs_event_to_te */

/*===========================================================================
FUNCTION  DSATETSIPKT_INIT_SERVICE_PREFERENCE_FROM_NV

DESCRIPTION
  Retrieves MS's service preference option from NV.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsipkt_init_service_preference_from_nv
(
  sys_modem_as_id_e_type subs_id
)
{
  nv_item_type      nv_item;   /* NV interface data */  
  dsatcmif_servs_state_ms_info  *ph_ss_ms_val = NULL;
  
   ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, subs_id, TRUE);
   if (NULL == ph_ss_ms_val)
   {
     return;
   }

  if(dsatutil_get_nv_item_per_subs( NV_SERVICE_DOMAIN_PREF_I,
                                    &nv_item,
                                    subs_id) == NV_DONE_S)
    {
      ph_ss_ms_val->dsat_ms_service_preference = (cm_srv_domain_pref_e_type)nv_item.service_domain_pref.srv_domain;
    }
} /* dsatetsipkt_init_service_preference_from_nv */

#endif /* FEATURE_DSAT_ETSI_DATA */
